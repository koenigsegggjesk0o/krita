# Cap — `Delegation.distributeRewards` Is Permissionless and Sweeps Entire Balance

**Protocol:** Cap (cap-labs-dev)
**Bounty:** $1,000,000 USDC
**Sherlock Bounty URL:** https://audits.sherlock.xyz/bug-bounties/114
**Source:** https://github.com/cap-labs-dev/cap-contracts
**Severity:** MEDIUM
**Area:** Access control / Cross-contract interaction
**Status:** NOT SUBMITTED — local audit finding only.

---

## 1. Description

`Delegation.distributeRewards(address _agent, address _asset)` is an
`external` function with **no access-control modifier**, yet it sweeps the
contract's *entire* balance of `_asset` and forwards it to the agent's
network for distribution to that agent's restakers:

```solidity
// contracts/delegation/Delegation.sol:55-70
function distributeRewards(address _agent, address _asset) external {       // <-- no checkAccess
    DelegationStorage storage $ = getDelegationStorage();
    uint256 _amount = IERC20(_asset).balanceOf(address(this));              // <-- entire balance

    uint256 totalCoverage = coverage(_agent);
    if (totalCoverage == 0) {
        IERC20(_asset).safeTransfer($.feeRecipient, _amount);
        return;
    }

    address network = $.agentData[_agent].network;
    IERC20(_asset).safeTransfer(network, _amount);                          // <-- full amount out
    ISymbioticNetworkMiddleware(network).distributeRewards(_agent, _asset);

    emit DistributeReward(_agent, _asset, _amount);
}
```

Every other state-mutating function on `Delegation` is gated by `checkAccess`
(`slash`, `setLastBorrow`, `addAgent`, `modifyAgent`, `registerNetwork`,
`setLtvBuffer`, `setFeeRecipient`, `setCoverageCap`). `distributeRewards` is
the sole exception, and it is the sole function that pushes tokens *out* of
the contract.

### Intended caller

`BorrowLogic.realizeRestakerInterest` (lines 217–219) calls it right after
borrowing the realized interest from the vault to the Delegation address:

```solidity
if (realizedInterest > 0) {
    IVault(reserve.vault).borrow(_asset, realizedInterest, $.delegation);
    IDelegation($.delegation).distributeRewards(_agent, _asset);
}
```

In the normal flow the borrow-then-distribute happens atomically in the same
function call, so the contract never holds a non-zero `_asset` balance
between calls — *as long as no one else donates or routes tokens to
`Delegation`*.

### The hole

Because `distributeRewards` is permissionless and reads the entire balance,
**anyone** can call it at any time to direct the full balance of any asset
held by `Delegation` to **any registered agent's network**. The choice of
`_agent` determines which network's restakers receive the funds.

This matters whenever `Delegation` holds a non-zero balance that was *not*
intended for the chosen `_agent`:

1. **Cross-agent stealing:** If `realizeRestakerInterest(agentA, USDC)` is
   somehow interleaved with `distributeRewards(agentB, USDC)` — for example
   via a re-entry from a malicious `IVault.borrow` callback, or via a
   flash-loaned donation — the funds meant for agentA's restakers get sent
   to agentB's network.
2. **Donation griefing:** An attacker donates USDC to `Delegation`, then
   calls `distributeRewards(attackerAgent, USDC)`. The attacker's own
   restaker position on `attackerAgent` receives the donated funds (minus
   the network's fee split). Net effect: the attacker recovers their
   donation *through the network's distribution logic*, which may pay it
   out to other restakers on the same network depending on
   `SymbioticNetworkMiddleware.distributeRewards` internals — but the
   attacker can structure their stake to capture most of it.
3. **Front-running legitimate distribution:** When a liquidator triggers
   `realizeRestakerInterest` via `Lender.repay`, the `borrow` to
   `$.delegation` and the `distributeRewards` call happen in the same tx,
   so there is no mempool window. But the permissionless `distributeRewards`
   is also reachable from `EigenAgentManager.setRestakerRate`, which loops
   over assets and calls `realizeRestakerInterest` per asset. Between two
   iterations of that loop, an attacker re-entering via a malicious vault
   could call `distributeRewards` with a different `_agent`.

---

## 2. Contract, Function, and Lines

| Field | Value |
|---|---|
| Contract | `Delegation` |
| File | `contracts/delegation/Delegation.sol` |
| Function | `distributeRewards` |
| Lines | 55–70 (the `external` signature on line 55 lacks `checkAccess`) |

---

## 3. Attack Scenario — Cross-Agent Reward Misdirection

1. agentA is a large borrowing agent with significant restaker coverage and
   accrued restaker interest.
2. agentB is an attacker-controlled agent (attacker is also a restaker on
   agentB's network, holding 100% of its stake).
3. The protocol (or any third party) calls `Lender.realizeRestakerInterest`
   for agentA. This calls `IVault.borrow(_asset, X, $.delegation)` which
   transfers `X` of `_asset` to the `Delegation` contract.
4. **Before** `IDelegation.distributeRewards(agentA, _asset)` is called in
   the same tx, a re-entry vector fires. Concretely: the `IVault.borrow`
   path goes through `VaultLogic.borrow` → `IERC20(asset).safeTransfer` to
   `$.delegation`. If the asset is a malicious ERC20 (or a legitimate but
   callback-capable token like ERC-777), the transfer triggers a callback
   to the attacker.
5. In the callback, the attacker calls
   `Delegation.distributeRewards(agentB, _asset)`.
6. `distributeRewards` reads the entire `_asset` balance of `Delegation`
   (currently `X`, intended for agentA), sees that `coverage(agentB) > 0`
   (the attacker made sure of that), and transfers all `X` to agentB's
   network.
7. Control returns to `BorrowLogic.realizeRestakerInterest`, which then
   calls `distributeRewards(agentA, _asset)` — but the balance is now 0,
   so agentA's restakers receive nothing.
8. The attacker, as the sole restaker on agentB, captures the stolen `X`
   via the network's `distributeRewards`.

### Simplified non-reentrancy variant

Even without a callback-capable token, if any Cap admin flow or third-party
integration ever transfers `_asset` directly to the `Delegation` address
(e.g. a malformed `IVault.borrow` receiver, a misconfigured `FeeReceiver`,
or a direct `safeTransfer` during migration), the first observer to call
`distributeRewards(theirAgent, _asset)` captures the full amount.

---

## 4. Proof of Concept (Forge-style)

```solidity
// SPDX-License-Identifier: UNLICENSED
pragma solidity ^0.8.28;

import "forge-std/Test.sol";
import {Delegation} from "contracts/delegation/Delegation.sol";
import {IERC20} from "@openzeppelin/contracts/token/ERC20/IERC20.sol";

contract MaliciousToken is IERC20 {
    // ... standard ERC-777-ish with _callBackTo on transfer
    function transfer(address to, uint256 amount) public returns (bool) {
        // do the transfer
        // then callback into the caller's hook:
        ITransferHook(msg.sender).onTokenReceived(to, amount);
        return true;
    }
}

contract CapDistributeRewardsPoC is Test {
    Delegation delegation;
    IERC20 maliciousToken;
    address agentA = address(0xA11CE);
    address agentB = address(0xB0B0); // attacker-controlled

    function testStealRewardViaReentry() public {
        // 1. agentA has accrued restaker interest in maliciousToken.
        // 2. Someone calls Lender.realizeRestakerInterest(agentA, maliciousToken).
        //    Internally: Vault.borrow -> maliciousToken.transfer(delegation, X)
        //                -> attacker's onTokenReceived fires
        //                -> attacker calls delegation.distributeRewards(agentB, maliciousToken)
        // 3. delegation's full balance of maliciousToken (X) is sent to agentB's network.
        // 4. attacker is the sole restaker on agentB -> captures X.

        // (Assert agentA's restakers got 0, agentB's restakers got X.)
        assertEq(delegation.distributedRewards(agentA, maliciousToken), 0);
        assertEq(delegation.distributedRewards(agentB, maliciousToken), X);
    }
}
```

A non-reentrancy variant is simpler — just call `distributeRewards` after a
direct donation:

```solidity
function testSweepDonation() public {
    deal(address(usdc), address(delegation), 1000e6); // simulate misrouted funds
    vm.prank(attacker);
    delegation.distributeRewards(agentB, address(usdc)); // sweeps 1000 USDC to agentB's network
}
```

---

## 5. Impact

- **Restaker reward theft:** funds intended for one agent's restakers can be
  redirected to another agent's restakers. The magnitude is bounded by the
  `realizedInterest` of any in-flight `realizeRestakerInterest` call (or by
  any direct deposit to `Delegation`).
- **Trust assumption violation:** every other `Delegation` function assumes
  only the `Lender` (via `checkAccess`) can move funds. `distributeRewards`
  silently breaks that assumption.
- **Compounds with the Swapper sweep issue:** both findings stem from the
  same pattern (reading `balanceOf(address(this))` instead of measuring a
  delta) and the same access-control gap (permissionless sweep).

---

## 6. Severity: **MEDIUM**

- Not HIGH because: exploitation requires either (a) a callback-capable
  collateral token (Cap's supported assets are USDC, wstETH, LBTC — none
  ERC-777) or (b) a separate operational mistake that sends tokens to
  `Delegation` outside the normal flow.
- Not LOW because: the access-control gap is explicit and the sweep is
  total. Once any balance lands on `Delegation` for any reason, it is
  immediately capturable by any observer.

---

## 7. Three-Perspective Audit

### 7.1 Protocol / Business-logic perspective
`distributeRewards` is conceptually an internal bookkeeping step: "the
Lender just sent us `X` for `_agent`, forward it to the network." There is
no legitimate reason for a third party to call it. Adding `checkAccess`
matching `slash` and `setLastBorrow` (both of which are also called by
`BorrowLogic`) closes the gap without changing any caller.

### 7.2 Mechanism / Reentrancy perspective
The function reads state, then transfers out, then calls an external
contract (`network.distributeRewards`). It is not protected by a
reentrancy guard. If `network.distributeRewards` re-enters
`distributeRewards` for the same `_asset`, the second call sees a zero
balance and either no-ops (if `coverage > 0`) or sends 0 to `feeRecipient`.
So reentrancy on the *same* asset is self-limiting — but reentrancy on a
*different* asset, or a callback during the `safeTransfer` step that calls
`distributeRewards` for the same asset before the transfer completes, could
double-spend. The `checkAccess` fix eliminates this entire class because
the Lender is a trusted contract that does not re-enter.

### 7.3 Operational / Threat-model perspective
Cap's `Delegation` is the central collateral-management contract — every
borrow, every slash, every reward flows through it. Treating it as a
"anyone can push funds out" contract is operationally dangerous. Even if
no exploit exists today, the surface area invites future bugs: any new
code path that sends tokens to `Delegation` becomes a de facto bounty.

---

## 8. Suggested Fix

```diff
-    function distributeRewards(address _agent, address _asset) external {
+    function distributeRewards(address _agent, address _asset)
+        external
+        checkAccess(this.distributeRewards.selector)
+    {
         DelegationStorage storage $ = getDelegationStorage();
-        uint256 _amount = IERC20(_asset).balanceOf(address(this));
+        uint256 _amount = IERC20(_asset).balanceOf(address(this));
+        // Optionally: pass expected amount in via the Lender call and assert
+        // equality, rather than inferring from balance.
```

The `Lender` (specifically `BorrowLogic.realizeRestakerInterest`) must then
be granted the `distributeRewards` role via `AccessControl.grantAccess`.

For defense in depth, also change the Lender-side caller to pass the
expected amount and have `distributeRewards` accept it as a parameter:

```diff
 // BorrowLogic.realizeRestakerInterest
 if (realizedInterest > 0) {
     IVault(reserve.vault).borrow(_asset, realizedInterest, $.delegation);
-    IDelegation($.delegation).distributeRewards(_agent, _asset);
+    IDelegation($.delegation).distributeRewards(_agent, _asset, realizedInterest);
 }
```

```diff
-    function distributeRewards(address _agent, address _asset)
-        external checkAccess(this.distributeRewards.selector)
-    {
-        uint256 _amount = IERC20(_asset).balanceOf(address(this));
+    function distributeRewards(address _agent, address _asset, uint256 _amount)
+        external checkAccess(this.distributeRewards.selector)
+    {
+        require(_amount <= IERC20(_asset).balanceOf(address(this)), "Delegation: insufficient");
```
