# Olympus YieldDirector — `_redeemAll` corrupts `recipientLookup` via swap-then-delete, breaking single-note redemption

## Summary

`YieldDirector._redeemAll` removes closed deposits from the recipient's
`recipientIds` array using a "swap-with-last then pop" pattern. When the closed
deposit is **not** the last element, the code swaps the last deposit's id into
the current slot and then deletes `recipientLookup` for the *swapped-in* id
instead of the *closed* id. This corrupts the recipient-lookup mapping: the
closed deposit's lookup entry is leaked, and an unrelated still-active deposit's
lookup entry is wiped to `address(0)`. Afterwards the recipient can no longer
redeem that active deposit via the single-deposit `redeemYield(id)` path (it
reverts `YieldDirector_NotYourYield`), and attempting to redeem the closed id
reverts `YieldSplitter_NotYourDeposit`. Only `redeemAllYield` (which bypasses
the `recipientLookup` check) still works.

## Contract / Function / Line

- Contract: `contracts/peripheral/YieldDirector.sol` — `YieldDirector`
- Function: `_redeemAll(address recipient_)`
- Lines: **480–510**, specifically the corruption at **500–504**:
```solidity
if (depositInfo[receiptIds[currIndex]].principalAmount == 0) {
    _closeDeposit(receiptIds[currIndex], currDepositor);

    if (currIndex != receiptIds.length - 1) {
        receiptIds[currIndex] = receiptIds[receiptIds.length - 1]; // swap
    }

    delete recipientLookup[receiptIds[currIndex]]; // BUG: deletes swapped id, not closed id
    receiptIds.pop();
}
```
- Compare with the (correct) single `_redeem` path at lines 457–469 which deletes
  `recipientLookup[depositId_]` *before* the swap.

## Root cause

The delete is performed **after** the in-place swap, so `receiptIds[currIndex]`
no longer references the closed deposit — it references the deposit that was
just moved from the tail. The closed id's lookup is never cleared; the tail id's
lookup is wrongly cleared.

Trace with `receiptIds = [5, 3, 7]`, all redeems processed back-to-front, and
deposit `5` (index 0) has `principalAmount == 0`:
1. Process `7` (idx 2), `3` (idx 1): not closed, no removal.
2. Process `5` (idx 0): closed. `currIndex(0) != length-1(2)` →
   `receiptIds[0] = receiptIds[2]` → array becomes `[7, 3, 7]`.
3. `delete recipientLookup[receiptIds[0]]` → `delete recipientLookup[7]` (WRONG;
   should be `recipientLookup[5]`).
4. `pop()` → `[7, 3]`.

Final state: `recipientLookup[5]` still points at the recipient (leak),
`recipientLookup[7] = address(0)` (active deposit orphaned).

## Attack scenario

1. Donor A deposits gOHM directing yield to recipient R (deposit `5`), then
   withdraws all principal (`withdrawAll`/`withdrawPrincipal`) so
   `principalAmount == 0` but unredeemed yield remains.
2. Donor B deposits gOHM directing yield to R (deposit `7`), keeps principal.
3. R calls `redeemAllYield()`. Because `5` precedes `7` and is closed, the swap
   corrupts `recipientLookup[7] = address(0)`.
4. R subsequently calls `redeemYield(7)` to redeem a single matured yield batch
   → reverts `YieldDirector_NotYourYield` (lookup is zero). R must instead call
   `redeemAllYield` every time, redeeming everything at once with no per-deposit
   control. Front-ends / integrations relying on `redeemYield(id)` break.
5. If R also (mistakenly) calls `redeemYield(5)` it reverts
   `YieldSplitter_NotYourDeposit` (`_closeDeposit` sees `depositor == address(0)`).

A griefing attacker (any donor) can deliberately create the principal-emptied
deposit and rely on array ordering to disable selective redemption for a target
recipient.

## PoC (Foundry)

```solidity
// SPDX-License-Identifier: AGPL-3.0-or-later
pragma solidity ^0.8.10;

import "forge-std/Test.sol";
import "../contracts/peripheral/YieldDirector.sol";
import "../contracts/governance/gOHM.sol";
import "../contracts/sOlympusERC20.sol";
// + mocks for sOHM index, staking.wrap/unwrap

contract RedeemAllPoC is Test {
    YieldDirector tyche;
    sOlympus sohm;       // indexWrapper
    // ... setup wires YieldDirector(sOHM, gOHM, staking, authority)

    function testRedeemAllCorruptsLookup() public {
        address recipient = address(0xR);
        // donor A: deposit 5 to recipient, then withdraw all principal (principal=0, yield>0)
        uint256 idA = tyche.deposit(10e18, recipient);
        tyche.withdrawAll();           // donor A, principalAmount -> 0
        // donor B: deposit 7 to recipient, keep principal
        // (ensure recipientIds ordering = [idA, idB] with idA not last)
        uint256 idB = tyche.deposit(20e18, recipient);
        // advance time / index so yield accrues
        vm.warp(block.timestamp + 7 days);

        // recipient redeems everything
        vm.prank(recipient);
        tyche.redeemAllYield();

        // idB's lookup is now wiped -> single redeem reverts
        vm.prank(recipient);
        vm.expectRevert(YieldDirector.YieldDirector_NotYourYield.selector);
        tyche.redeemYield(idB);

        // idA's lookup leaked, deposit deleted -> redeem reverts
        vm.prank(recipient);
        vm.expectRevert(YieldSplitter_NotYourDeposit.selector);
        tyche.redeemYield(idA);
    }
}
```

## Impact

- **Functional DoS of `redeemYield` / `redeemYieldAsSohm`** for any deposit that
  lands in a swapped slot after a `redeemAllYield` that closes a non-tail
  deposit. Recipients are forced onto the all-or-nothing `redeemAllYield` path,
  losing granularity and breaking front-end/integrator flows.
- Leaked `recipientLookup` entries for closed deposits cause hard reverts on
  single-redemption attempts, which can surface as unhandled reverts in
  aggregators/keepers.
- No direct principal theft (donors can still withdraw principal via
  `withdrawPrincipal`; recipients can still use `redeemAllYield`), hence not
  Critical, but a durable, self-propagating state corruption.

## Severity

**Medium** — permanent corruption of accounting mapping and loss of a documented
user-facing function path, triggerable by any donor through ordering; no fund
theft but real DoS / broken invariant.

## Three-perspective audit

- **Differential:** The single `_redeem` (lines 454–469) deletes
  `recipientLookup[depositId_]` *before* swapping/popping and uses the captured
  `depositId_`, so it is correct. `_redeemAll` reorders the operations
  (swap-then-delete-via-array-slot), introducing the bug — a classic
  copy-paste/drift defect.
- **Adversarial:** A donor can choose recipients and deposit timing to control
  array ordering, so the trigger is not accidental-only; it can be aimed.
- **Defensive / fix:** Capture the id being closed in a local before mutating
  the array, then delete that id's lookup:
```solidity
uint256 closedId = receiptIds[currIndex];
_closeDeposit(closedId, currDepositor);
if (currIndex != receiptIds.length - 1)
    receiptIds[currIndex] = receiptIds[receiptIds.length - 1];
delete recipientLookup[closedId];
receiptIds.pop();
```
  Also add a test asserting `recipientLookup` invariants hold after
  `redeemAllYield` with mixed principal-emptied deposits.
