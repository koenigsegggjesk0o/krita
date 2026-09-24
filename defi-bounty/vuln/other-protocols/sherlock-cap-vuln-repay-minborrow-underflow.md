# Cap — `BorrowLogic.repay` Can Underflow on `minBorrow` Recomputation

**Protocol:** Cap (cap-labs-dev)
**Bounty:** $1,000,000 USDC
**Sherlock Bounty URL:** https://audits.sherlock.xyz/bug-bounties/114
**Source:** https://github.com/cap-labs-dev/cap-contracts
**Severity:** LOW
**Area:** Integer precision / DoS
**Status:** NOT SUBMITTED — local audit finding only.

---

## 1. Description

`BorrowLogic.repay` enforces a "minimum borrow" invariant by refusing to
leave a borrower with a dust debt below `reserve.minBorrow`. The
re-computation of the capped `repaid` amount can underflow Solidity 0.8+'s
checked arithmetic and revert, permanently blocking the borrower from
making a partial repayment.

```solidity
// contracts/lendingPool/libraries/BorrowLogic.sol:108-116
uint256 agentDebt = IERC20(reserve.debtToken).balanceOf(params.agent);
repaid = Math.min(params.amount, agentDebt);

uint256 remainingDebt = agentDebt - repaid;
if (remainingDebt > 0 && remainingDebt < reserve.minBorrow) {
    // Limit repayment to maintain minimum debt if not full repayment
    repaid = agentDebt - reserve.minBorrow;     // <-- UNDERFLOW
}
```

When `agentDebt < reserve.minBorrow` (which can occur because interest
realization mints debt tokens in arbitrary-sized increments via
`realizeRestakerInterest`, and `minBorrow` only gates *new* borrows — it
does not prevent an existing debt from shrinking below the threshold via
partial repayments or interest accrual on a near-zero principal), the
expression `agentDebt - reserve.minBorrow` underflows.

Solidity 0.8.28 reverts on underflow, so the call reverts with the
standard panic code 0x11.

---

## 2. Contract, Function, and Lines

| Field | Value |
|---|---|
| Contract | `BorrowLogic` (library) |
| File | `contracts/lendingPool/libraries/BorrowLogic.sol` |
| Function | `repay` |
| Lines | 99–116 (the underflow is on line 115) |

---

## 3. Attack Scenario

1. `reserve.minBorrow` is set to 100 USDC (a typical value to discourage
   dust positions).
2. A borrower opens a 100 USDC position (valid, exactly at the minimum).
3. The borrower partially repays 1 USDC. Now `agentDebt = 99`, which is
   below `minBorrow = 100`.
4. Time passes. The borrower wants to repay another 1 USDC. They call
   `Lender.repay(USDC, 1, agent)`.
5. In `BorrowLogic.repay`:
   - `agentDebt = 99`
   - `repaid = min(1, 99) = 1`
   - `remainingDebt = 99 - 1 = 98`
   - `98 > 0 && 98 < 100` → enter the branch
   - `repaid = 99 - 100` → **underflow, revert**.
6. The borrower cannot make any partial repayment. Their only escape is to
   repay the full 99 USDC at once.

### Why this is reachable

- `minBorrow` is enforced only in `ValidationLogic.validateBorrow` (line 60),
  i.e. on new borrows. There is no check preventing the debt from drifting
  below `minBorrow` via:
  - Partial repayments (each one is checked against `minBorrow`, but the
    first partial repayment on a debt *above* `minBorrow` that leaves
    `remainingDebt < minBorrow` triggers the re-computation. If
    `agentDebt >= minBorrow` at that point, `repaid = agentDebt - minBorrow`
    is positive and no underflow occurs. **But** a subsequent repayment
    on the now-below-`minBorrow` debt will underflow.)
  - `realizeRestakerInterest` minting tiny amounts of debt tokens when the
    borrower's principal is already small (e.g. 50 USDC principal with a
    60 USDC debt token balance after interest realization; if `minBorrow`
    is then raised by governance to 100, the borrower is below the new
    threshold through no action of their own).
- `setMinBorrow` (ReserveLogic) can raise `minBorrow` at any time,
  retroactively pushing existing sub-threshold debts into the underflow
  trap.

---

## 4. Proof of Concept (Forge-style)

```solidity
// SPDX-License-Identifier: UNLICENSED
pragma solidity ^0.8.28;

import "forge-std/Test.sol";
import {Lender} from "contracts/lendingPool/Lender.sol";
import {IERC20} from "@openzeppelin/contracts/token/ERC20/IERC20.sol";

contract CapRepayMinBorrowUnderflowPoC is Test {
    Lender lender;
    IERC20 usdc;
    address agent = address(0xA11CE);

    function setUp() public {
        lender = Lender(payable(LENDER));
        usdc = IERC20(USDC);
        // assume minBorrow = 100e6 and agent has opened a 100 USDC borrow
    }

    function testRepayUnderflow() public {
        // 1. Partially repay 1 USDC -> agentDebt becomes 99 (< minBorrow 100).
        vm.startPrank(agent);
        usdc.approve(address(lender), 1e6);
        lender.repay(address(usdc), 1e6, agent);
        vm.stopPrank();

        // 2. Try to repay another 1 USDC -> underflow revert.
        vm.startPrank(agent);
        usdc.approve(address(lender), 1e6);
        vm.expectRevert(); // Panic(0x11) arithmetic underflow
        lender.repay(address(usdc), 1e6, agent);
        vm.stopPrank();

        // 3. Only full repayment works.
        vm.startPrank(agent);
        usdc.approve(address(lender), 99e6);
        lender.repay(address(usdc), 99e6, agent); // OK
        vm.stopPrank();
    }
}
```

---

## 5. Impact

- **DoS of partial repayments:** affected borrowers cannot make partial
  repayments; they must repay the full remaining debt in one transaction.
  This is a usability / liveness issue, not a fund-loss issue.
- **Liquidation path interference:** a liquidator calling `liquidate` with a
  partial `_amount` ends up in `BorrowLogic.repay` via
  `LiquidationLogic.liquidate`. If the agent's debt is below `minBorrow`,
  the liquidation reverts. The liquidator must take the entire debt —
  which may be more than `maxLiquidatable` allows, leading to a stuck
  position that cannot be liquidated at all until the debt grows back
  above `minBorrow` via interest accrual.
- **Governance griefing:** raising `minBorrow` retroactively bricks all
  sub-threshold debts.

---

## 6. Severity: **LOW**

- Not MEDIUM because: no direct fund loss; the borrower can always repay
  in full. The liquidation-locking edge case is real but requires the
  debt to be both below `minBorrow` and above `maxLiquidatable`, which is
  a narrow window.

---

## 7. Three-Perspective Audit

### 7.1 Protocol / Business-logic perspective
The intent of the `minBorrow` re-computation is sensible: "don't let a
partial repayment leave a dust debt." But the formula assumes
`agentDebt >= minBorrow`, which is not invariant. The fix is either to
skip the re-computation when `agentDebt < minBorrow` (in which case any
partial repayment is already going to leave dust, and the protocol should
accept that) or to force a full repayment when `agentDebt < minBorrow`.

### 7.2 Integer-precision perspective
Solidity 0.8+'s checked arithmetic is doing its job here — the underflow
reverts rather than producing a nonsensical `repaid` value. But the
correct behavior is to handle the small-debt case explicitly, not to rely
on the revert as an implicit guard.

### 7.3 Operational / Threat-model perspective
The `setMinBorrow` admin function is a known knob that governance uses to
tune dust prevention. Raising it should not brick existing positions.
The underflow makes `setMinBorrow` a potentially disruptive operation,
which limits governance flexibility.

---

## 8. Suggested Fix

```diff
 uint256 remainingDebt = agentDebt - repaid;
-if (remainingDebt > 0 && remainingDebt < reserve.minBorrow) {
-    // Limit repayment to maintain minimum debt if not full repayment
-    repaid = agentDebt - reserve.minBorrow;
+if (remainingDebt > 0 && remainingDebt < reserve.minBorrow && agentDebt > reserve.minBorrow) {
+    // Limit repayment to maintain minimum debt if not full repayment.
+    // If agentDebt is already below minBorrow (e.g. due to a governance
+    // minBorrow raise or sub-threshold interest accrual), allow the
+    // partial repayment rather than underflowing.
+    repaid = agentDebt - reserve.minBorrow;
 }
```

Or, equivalently and arguably clearer:

```diff
 uint256 remainingDebt = agentDebt - repaid;
 if (remainingDebt > 0 && remainingDebt < reserve.minBorrow) {
-    repaid = agentDebt - reserve.minBorrow;
+    if (agentDebt > reserve.minBorrow) {
+        repaid = agentDebt - reserve.minBorrow;
+    }
+    // else: agentDebt <= minBorrow, allow the partial repayment as-is.
 }
```
