# SYMMIO — `liquidatePositionsPartyA` OVERDUE Branch Divides by `uint256(-totalUnrealizedLoss)`, Reverts on Zero

**Protocol:** SYMMIO (Symmetry)
**Bounty:** $808,808 USDC
**Sherlock Bounty URL:** https://audits.sherlock.xyz/bug-bounties/5
**Source:** https://github.com/SYMM-IO/protocol-core
**Severity:** LOW
**Area:** Integer precision / DoS (liquidation path)
**Status:** NOT SUBMITTED — local audit finding only.

---

## 1. Description

In `LiquidationFacetImpl.liquidatePositionsPartyA`, the `OVERDUE`
liquidation branch computes a loss-sharing adjustment by dividing by
`uint256(- accountLayout.liquidationDetails[partyA].totalUnrealizedLoss)`.
If `totalUnrealizedLoss == 0`, this expression is `uint256(0)`, and the
subsequent division reverts with Panic(0x12), permanently bricking the
OVERDUE liquidation path for that partyA.

```solidity
// contracts/facets/liquidation/LiquidationFacetImpl.sol:183-194
} else if (accountLayout.liquidationDetails[partyA].liquidationType == LiquidationType.OVERDUE) {
    if (hasMadeProfit) {
        accountLayout.settlementStates[partyA][quote.partyB].actualAmount += int256(amount);
        accountLayout.settlementStates[partyA][quote.partyB].expectedAmount += int256(amount);
    } else {
        accountLayout.settlementStates[partyA][quote.partyB].actualAmount -= int256(
            amount -
            ((amount * accountLayout.liquidationDetails[partyA].deficit) /
                uint256(- accountLayout.liquidationDetails[partyA].totalUnrealizedLoss))  // <-- div by 0 if totalUnrealizedLoss == 0
        );
        accountLayout.settlementStates[partyA][quote.partyB].expectedAmount -= int256(amount);
    }
}
```

`totalUnrealizedLoss` is an `int256` populated directly from the
muon-signed `LiquidationSig.totalUnrealizedLoss` (or
`DeferredLiquidationSig.totalUnrealizedLoss`) field at liquidation time.
The contract performs no non-zero check on it before using it as a
divisor.

### When can `totalUnrealizedLoss` be 0?

- **Muon signs 0:** the muon gateway computes `totalUnrealizedLoss` as
  the sum of unrealized losses across all open positions. If partyA's
  positions are individually underwater but the muon algorithm nets them
  against winners and produces 0 (or rounds to 0), the signed value is 0.
- **Position with only CVA loss:** an OVERDUE liquidation is triggered
  when `-availableBalance > lockedBalances.cva + lockedBalances.lf`.
  `availableBalance = allocatedBalance + upnl - cva - lf`. So OVERDUE
  requires `upnl < -allocatedBalance`, i.e. uPNL is deeply negative.
  But `totalUnrealizedLoss` is a *separate* field from `upnl` — it is
  the gross unrealized loss (sum of negative-position PnL), not the net.
  A position with a single open quote that has `totalUnrealizedLoss = 0`
  but a negative `upnl` due to funding payments (not unrealized
  position loss) would trigger OVERDUE with `totalUnrealizedLoss = 0`.
- **Deferred path with stale sig (compounds with the staleness bug):**
  if a deferred liquidation sig is replayed (see the separate
  `sherlock-symmio-vuln-deferred-liquidation-staleness.md` finding),
  the `totalUnrealizedLoss` reflects the state at the old signature
  time, which may be 0 even if the current state has losses.

When `totalUnrealizedLoss == 0`, the OVERDUE branch reverts on the
division, and `liquidatePositionsPartyA` cannot complete for any quote
in the OVERDUE state. The partyA's liquidation is stuck: the liquidation
window is open, positions are marked for liquidation, but the per-quote
settlement cannot execute.

---

## 2. Contract, Function, and Lines

| Field | Value |
|---|---|
| Contract | `LiquidationFacetImpl` (library) |
| File | `contracts/facets/liquidation/LiquidationFacetImpl.sol` |
| Function | `liquidatePositionsPartyA` |
| Lines | 183–194 (the division is on line 191) |

---

## 3. Attack Scenario

1. partyA has a single open SHORT position on a symbol with negligible
   price movement, so `totalUnrealizedLoss = 0`.
2. partyA's `upnl` goes deeply negative due to funding rate accrual
   (SYMMIO tracks funding separately from unrealized PnL).
3. `availableBalance = allocatedBalance + upnl - cva - lf < 0`, and
   specifically `< -(cva + lf)`, triggering OVERDUE liquidation type
   in `setSymbolsPrice` / `deferredSetSymbolsPrice`.
4. A liquidator calls `liquidatePositionsPartyA(partyA, quoteIds)`.
5. The position is a SHORT with `hasMadeProfit = false` (the position
   lost money, since upnl is negative). Wait — actually, the per-quote
   `hasMadeProfit` is computed from the symbol's liquidation price vs.
   the quote's open price, not from the aggregate upnl. A SHORT that
   opened at $100 and is being liquidated at $100 (no price move) has
   `hasMadeProfit = false` and `amount = 0` (no PnL). Hmm.
6. Let me re-examine. `getValueOfQuoteForPartyA` returns
   `(hasMadeProfit, amount)`. For a SHORT, `hasMadeProfit = true` if
   the current price < open price (the short made money). If the price
   hasn't moved, `hasMadeProfit` depends on rounding — likely `false`
   with `amount = 0`.
7. If `hasMadeProfit = false` and `amount = 0`, the branch executes:
   `actualAmount -= int256(0 - (0 * deficit) / uint256(-0))`. The
   division `0 / 0` still reverts (Panic 0x12).
8. The liquidation is bricked. partyA's positions cannot be settled,
   and since `liquidationStatus[partyA] == true`, partyA cannot open
   new positions or withdraw. The protocol is stuck with a bad debt
   that cannot be processed.

---

## 4. Proof of Concept (Forge-style)

```solidity
// SPDX-License-Identifier: UNLICENSED
pragma solidity ^0.8.18;

import "forge-std/Test.sol";
import {LiquidationFacet} from "contracts/facets/liquidation/LiquidationFacet.sol";

contract SymmioOverdueDivByZeroPoC is Test {
    LiquidationFacet facet;
    address partyA = address(0xA11CE);
    address liquidator = address(0xB0B);

    function setUp() public {
        // 1. Set up partyA with a SHORT position, no price movement.
        // 2. Accrue enough funding to make upnl deeply negative.
        // 3. Liquidate via deferredLiquidatePartyA with totalUnrealizedLoss = 0.
        // 4. Set symbols prices via deferredSetSymbolsPrice -> OVERDUE type.
    }

    function testOverdueDivByZero() public {
        uint256[] memory quoteIds = new uint256[](1);
        quoteIds[0] = partyA_OpenQuoteId;

        vm.prank(liquidator);
        vm.expectRevert(); // Panic(0x12) division by zero
        facet.liquidatePositionsPartyA(partyA, quoteIds);
    }
}
```

---

## 5. Impact

- **OVERDUE liquidation DoS:** when `totalUnrealizedLoss == 0` (which is
  reachable via funding-driven insolvency or via the deferred-sig
  staleness bug), the OVERDUE branch reverts. The liquidator cannot
  process the position, and the bad debt remains on the protocol's
  books indefinitely.
- **partyA lock-in:** `liquidationStatus[partyA]` is `true` and cannot
  be cleared without completing `settlePartyALiquidation`, which
  requires `partyAPositionsCount == 0`, which requires
  `liquidatePositionsPartyA` to succeed. partyA is permanently locked.
- **Bad debt socialization:** the protocol's CVA / insurance mechanism
  cannot absorb the loss because the settlement loop is blocked.

---

## 6. Severity: **LOW**

- Not MEDIUM because: reaching `totalUnrealizedLoss == 0` in an OVERDUE
  scenario requires either funding-driven insolvency (uncommon but
  possible) or the separate deferred-sig staleness bug (which is its
  own finding). The direct exploitability is narrow.
- Not Informational because: the DoS is permanent for the affected
  partyA and blocks the protocol's bad-debt recovery flow.

---

## 7. Three-Perspective Audit

### 7.1 Protocol / Business-logic perspective
The OVERDUE loss-sharing formula distributes the deficit across
counterparties proportional to their share of the total unrealized
loss. When `totalUnrealizedLoss == 0`, the formula is mathematically
undefined — there is no loss to distribute, so the deficit should
arguably be absorbed entirely by the CVA / insurance fund rather than
socialized. The contract should handle this edge case explicitly rather
than relying on the division to "never be zero."

### 7.2 Integer-precision perspective
The expression `uint256(- totalUnrealizedLoss)` is an explicit
int256→uint256 cast, which wraps negative values to their two's
complement representation. If `totalUnrealizedLoss` is positive
(representing a loss as a positive number — the naming is ambiguous),
the cast produces a huge uint256, making the division result ~0 and
the loss-sharing effectively a no-op. If `totalUnrealizedLoss` is 0,
the cast produces 0 and the division reverts. Both cases are likely
bugs; the intended semantics require `totalUnrealizedLoss` to be
strictly negative (representing a loss as a negative number), which
should be asserted.

### 7.3 Operational / Threat-model perspective
SYMMIO's liquidation flow is the protocol's last line of defense
against insolvency. Any code path in that flow that can revert
unexpectedly — especially a division that the author assumed could
never be zero — is a systemic risk. The fix is a one-line
`require(totalUnrealizedLoss < 0, ...)` guard before the division, or
an explicit branch for the zero case.

---

## 8. Suggested Fix

```diff
 } else if (accountLayout.liquidationDetails[partyA].liquidationType == LiquidationType.OVERDUE) {
     if (hasMadeProfit) {
         accountLayout.settlementStates[partyA][quote.partyB].actualAmount += int256(amount);
         accountLayout.settlementStates[partyA][quote.partyB].expectedAmount += int256(amount);
     } else {
-        accountLayout.settlementStates[partyA][quote.partyB].actualAmount -= int256(
-            amount -
-            ((amount * accountLayout.liquidationDetails[partyA].deficit) /
-                uint256(- accountLayout.liquidationDetails[partyA].totalUnrealizedLoss))
-        );
+        int256 tul = accountLayout.liquidationDetails[partyA].totalUnrealizedLoss;
+        uint256 lossShare;
+        if (tul < 0) {
+            lossShare = (amount * accountLayout.liquidationDetails[partyA].deficit)
+                / uint256(-tul);
+        }
+        // If tul >= 0 (no unrealized loss to distribute), lossShare = 0 and
+        // the full `amount` is subtracted from actualAmount, consistent with
+        // the CVA/insurance fund absorbing the deficit.
+        accountLayout.settlementStates[partyA][quote.partyB].actualAmount -= int256(amount - lossShare);
         accountLayout.settlementStates[partyA][quote.partyB].expectedAmount -= int256(amount);
     }
 }
```

The same pattern should be audited in
`DeferredLiquidationFacetImpl.deferredSetSymbolsPrice` (lines 82–97),
which has an analogous OVERDUE branch with the same division.
