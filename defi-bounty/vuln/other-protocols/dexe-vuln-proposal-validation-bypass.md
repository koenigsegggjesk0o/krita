# DeXe Protocol — Proposal Validation Bypass via Unregistered Main Executor

**Severity: HIGH**
**Area: Proposal execution / Governance voting manipulation**
**Contracts: GovPool, GovPoolCreate, DistributionProposal, GovPoolExecute**

---

## 1. Description

The `GovPool.createProposal` flow calls `_validateProposalCreation` (in
`GovPoolCreate.sol`) to verify that the last action's executor (the "main
executor") implements `IProposalValidator.validate` and returns `true`.

The validation logic is:

```solidity
// GovPoolCreate.sol, _validateProposalCreation, lines 206–215
function _validateProposalCreation(
    address executor,
    IGovPool.ProposalAction[] calldata actionsFor
) internal view {
    (bool ok, bytes memory data) = executor.staticcall{gas: 30000}(
        abi.encodeWithSelector(IProposalValidator.validate.selector, actionsFor)
    );

    require(!ok || data.length == 0 || abi.decode(data, (bool)), "Gov: validation failed");
}
```

The `require` condition is:

```
!ok || data.length == 0 || abi.decode(data, (bool))
```

When the `staticcall` **reverts or targets a non-contract address** (`ok == false`),
the condition `!ok` is `true` and the `require` **passes unconditionally**. This
means:

- If the main executor is an EOA, `address(0)`, or any contract that does **not**
  implement `validate`, the `staticcall` returns `ok = false` and **all proposal
  validation is silently skipped**.

- The proposal then falls back to `ExecutorType.DEFAULT` settings
  (`executorToSettings` returns `0` for unregistered executors), under which
  **arbitrary actions are allowed** with no selector whitelisting
  (`_handleDataForProposal` returns `false` for `DEFAULT`).

This completely bypasses the executor-specific validation that `DistributionProposal`
(and any other `IProposalValidator`) is supposed to enforce.

## 2. Contract + Function + Line

| Item | Location |
|------|----------|
| `GovPool.createProposal` | `contracts/gov/GovPool.sol:186–194` |
| `_validateProposal` | `contracts/libs/gov/gov-pool/GovPoolCreate.sol:134–161` |
| `_validateProposalCreation` (bug) | `contracts/libs/gov/gov-pool/GovPoolCreate.sol:206–215` |
| `DistributionProposal.validate` (bypassed) | `contracts/gov/proposals/DistributionProposal.sol:102–108` |
| `GovPoolExecute.execute` (arbitrary call) | `contracts/libs/gov/gov-pool/GovPoolExecute.sol:60–68` |

## 3. Attack Scenario

`DistributionProposal.validate` ensures that a proposal calling
`DP.execute(proposalId, ...)` uses the **current** `latestProposalId`. This
prevents creating a distribution proposal that rewards voters of an **old**
proposal. When validation is bypassed, an attacker can target any old proposal
that does not yet have a distribution set up.

### Step-by-step

1. The attacker identifies an **old, succeeded proposal** `oldId` where they
   (or an accomplice) voted "For" and which has **no distribution** yet
   (`proposals[oldId].rewardAddress == address(0)`).

2. The attacker calls `GovPool.createProposal` with `actionsOnFor` constructed
   as:

   | # | executor | value | data |
   |---|----------|-------|------|
   | 0 | `rewardToken` | 0 | `approve(DP, amount)` *(for ERC20 path)* |
   | 1 | `DistributionProposal` | `amount` *(ETH path)* or 0 | `execute(oldId, token, amount)` |
   | 2 | `address(0)` or any EOA | 0 | `""` **← main executor (last element)** |

3. Because the main executor (`address(0)`) has no code, `staticcall` returns
   `ok = false`. The `require(!ok || …)` passes. Validation is skipped.

4. `executorToSettings(address(0))` returns `0` (`DEFAULT`). `_handleDataForProposal`
   returns `false`. The proposal is created with `DEFAULT` settings.

5. The attacker votes "For" (and/or accumulates enough voting power to meet the
   `DEFAULT` quorum).

6. Once the proposal succeeds and the execution delay passes, anyone calls
   `GovPool.execute(newProposalId)`.

7. `GovPoolExecute.execute` iterates the actions:
   - `rewardToken.approve(DP, amount)` — GovPool approves DP.
   - `DP.execute(oldId, token, amount)` — DP calls `safeTransferFrom(GovPool, DP, …)`
     and sets `proposals[oldId].rewardAddress = token`,
     `proposals[oldId].rewardAmount = actualAmount`.
   - The no-op call to `address(0)` does nothing.

8. The attacker (who voted "For" on `oldId`) calls
   `DistributionProposal.claim(oldId)` and receives a pro-rata share of
   `actualAmount`, **draining GovPool treasury funds**.

### ETH variant (no approval needed)

For `token == ETHEREUM_ADDRESS`, `DP.execute` uses `msg.value` directly (line 62–66
of `DistributionProposal.sol`). The proposal action carries `value = amount`,
which is sent from the GovPool's ETH balance. **No `approve` step is required**,
making the attack even simpler.

## 4. Proof of Concept (pseudo-Solidity)

```solidity
// SPDX-License-Identifier: MIT
pragma solidity ^0.8.20;

import {IGovPool} from "../interfaces/gov/IGovPool.sol";
import {IDistributionProposal} from "../interfaces/gov/proposals/IDistributionProposal.sol";

contract ValidationBypassPoC {
    function attack(
        IGovPool govPool,
        IDistributionProposal dp,
        uint256 oldProposalId,      // a succeeded "For" proposal with no distribution
        address rewardToken,        // ERC20 held by GovPool, or ETHEREUM_ADDRESS
        uint256 amount
    ) external {
        IGovPool.ProposalAction[] memory actionsOnFor =
            new IGovPool.ProposalAction[](rewardToken == address(0) ? 2 : 3);

        uint256 idx;
        if (rewardToken != address(0xEeeeEeeeEeEeEeEeEeEeeEEEeeeeEeeeeeeeEEeE)) {
            actionsOnFor[idx++] = IGovPool.ProposalAction({
                executor: rewardToken,
                value: 0,
                data: abi.encodeWithSelector(0x095ea7b3, address(dp), amount) // approve
            });
        }
        actionsOnFor[idx++] = IGovPool.ProposalAction({
            executor: address(dp),
            value: rewardToken == address(0xEeeeEeeeEeEeEeEeEeEeeEEEeeeeEeeeeeeeEEeE) ? amount : 0,
            data: abi.encodeWithSelector(
                IDistributionProposal.execute.selector,
                oldProposalId,
                rewardToken,
                amount
            )
        });
        // Last action: unregistered executor → validation skipped, DEFAULT settings
        actionsOnFor[idx] = IGovPool.ProposalAction({
            executor: address(0x1234), // EOA / no code
            value: 0,
            data: ""
        });

        govPool.createProposal("ipfs://...", actionsOnFor, new IGovPool.ProposalAction[](0));

        // … vote "For" with enough power to meet DEFAULT quorum …
        // … wait for execution delay …
        // govPool.execute(latestProposalId);
        // dp.claim(attacker, [oldProposalId]);
    }
}
```

## 5. Impact

| Impact | Detail |
|--------|--------|
| **Direct theft of GovPool treasury funds** | The attacker can set up arbitrary distributions for old proposals and claim rewards. Any ERC20 or ETH held by the GovPool can be drained. |
| **Governance manipulation** | Old, already-decided proposals can be retroactively turned into distribution proposals, distorting the intended governance outcome. |
| **Bypass of all IProposalValidator checks** | Any future executor that implements `IProposalValidator.validate` can be bypassed by using an unregistered main executor. |

The only gate is the `DEFAULT`-settings quorum. If the DAO configures
`DEFAULT` with a low quorum (a common practice for "general" proposals), the
attack is trivially achievable. Even with a moderate quorum, a whale or
flash-loan-assisted voter (subject to the `_lockBlock`/`_checkBlock` same-block
guard, which does **not** prevent multi-block voting) can pass the proposal.

## 6. Severity: **HIGH**

- **Direct theft of user funds** (GovPool treasury) — matches Immunefi
  Critical impact "Direct theft of any user funds, whether at-rest or
  in-motion".
- **Manipulation of governance** — matches Immunefi Critical impact
  "Manipulation of governance voting result deviating from voted outcome".
- Rated HIGH (rather than Critical) because exploitation requires passing a
  governance vote under `DEFAULT` settings, which introduces a dependency on
  the DAO's quorum configuration. With a low-to-moderate quorum the attack is
  practical.

## 7. Three-Perspective Audit

### 7.1 Attacker perspective
The attacker only needs enough voting power to pass one `DEFAULT`-settings
proposal. They can reuse tokens across proposals because `_unlock` frees tokens
from proposals that have reached quorum (`Locked`/`Succeeded`) — see
`GovPoolUnlock._proposalIsActive` which returns `false` for `Locked`. After
voting in one proposal and waiting for `Locked` state, the same tokens can be
unlocked and reused. The attacker then waits for execution, claims the old
distribution, and repeats for every unprotected old proposal.

### 7.2 Defender (protocol) perspective
The `_validateProposalCreation` function was intended as a defence-in-depth
check: even if `DEFAULT` settings allow arbitrary calls, the `validate`
function on registered executors (DP, TSP, SP) provides executor-specific
invariants. The `!ok` short-circuit silently defeats this layer for any
unregistered executor. The protocol likely assumed that the `staticcall`
failing means "not a validator, treat as DEFAULT" — but it failed to enforce
that `DEFAULT`-settings proposals must NOT call `onlyGov` functions on
registered executors like DP.

### 7.3 Neutral (auditor) perspective
The root cause is a logical error in the `require` condition: `!ok` should
**fail** the validation (the executor could not be validated), not pass it.
The correct logic is:

```solidity
require(ok && data.length >= 32 && abi.decode(data, (bool)), "Gov: validation failed");
```

Additionally, `DEFAULT`-settings proposals should be restricted from calling
`onlyGov`/`onlyOwner` functions on registered executors, or the
`DistributionProposal.execute` function should independently verify that
`proposalId == latestProposalId` (defence-in-depth at the executor level, not
only at the proposal-creation level).

## 8. Recommended Fix

**Option A (minimal):** Correct the `require` in `_validateProposalCreation`:

```solidity
require(ok && data.length >= 32 && abi.decode(data, (bool)), "Gov: validation failed");
```

**Option B (defence-in-depth):** Add an independent check in
`DistributionProposal.execute`:

```solidity
require(
    proposalId == GovPool(payable(govAddress)).latestProposalId(),
    "DP: not the latest proposal"
);
```

**Option C (belt-and-suspenders):** In `GovPoolCreate._handleDataForProposal`,
when `settingsId == DEFAULT`, scan all actions and reject any whose `executor`
is a registered executor (`executorToSettings(executor) != 0`).
