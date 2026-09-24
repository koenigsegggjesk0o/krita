# Aave V4 Audit — Liquidation Area

## Summary

Deep audit of `LiquidationLogic.sol`, the `liquidationCall` entry-point in
`Spoke.sol`, and the Hub interactions (`remove`, `restore`, `payFeeShares`,
`reportDeficit`) during liquidation.

After exhaustive review (including cross-checking against the Z3 formal
verification specs in `tests/misc/z3/liquidation_logic.py` and
`tests/misc/z3/debt_to_liquidate.py`), **no Critical or High severity bug was
identified in the liquidation path**. The rounding directions, dust-prevention
logic, deficit reporting, and collateral/debt coupling are sound.

Below are the lower-severity observations that survived scrutiny.

---

## Finding L-1: Orphan collateral shares when `receiveShares == false`

- **Severity**: Low
- **Contract / Function / Line**: `src/spoke/libraries/LiquidationLogic.sol`,
  `_liquidateCollateral`, lines 450–489.

### Description

When a liquidator elects to receive underlying (`receiveShares == false`) and
`sharesToLiquidator < sharesToLiquidate`, the function computes:

```solidity
amountToLiquidator = params.hub.previewRemoveByShares(
    params.assetId,
    params.sharesToLiquidator
);                                   // floor -> assets
params.hub.remove(params.assetId, amountToLiquidator, params.liquidator);
```

Inside `Hub.remove`, the shares debited from the Spoke are computed with the
opposite rounding:

```solidity
uint120 shares = asset.toAddedSharesUp(amount).toUint120();   // ceil
asset.addedShares -= shares;
spoke.addedShares  -= shares;
```

Because of `floor` then `ceil`, the shares actually removed from the Hub-side
Spoke accounting, `shares`, can be **strictly less than** `sharesToLiquidator`
(verified by case analysis: with share price `R ≥ 1`, which always holds
post-fee-minting due to virtual shares/assets, the round trip is identity in
the integer case and `floor-1` can occur only when the fractional residual
pushes the ceil below the original).  The user's `userPosition.suppliedShares`
was already decremented by the full `sharesToLiquidate`.

Net effect: `sharesToLiquidator − shares` shares become "orphaned" — they
remain credited to the Spoke in the Hub but are claimable by no user, slightly
inflating the share price for the remaining suppliers.

### Attack scenario / Impact

* Per liquidation the orphanage is at most **1 share**.
* The orphan shares silently accrue to all remaining suppliers of that reserve
  (a sub-wei-level donation).
* Not exploitable for material gain; cannot be amplified because each
  liquidation is independent and the rounding differential is bounded by 1.

### PoC (Foundry sketch)

```solidity
// SPDX-License-Identifier: UNLICENSED
pragma solidity 0.8.28;

import {Test} from "forge-std/Test.sol";

// Illustrative: requires the standard Aave V4 test setup
// (tests/setup/Base.t.sol). Run with: forge test --match-test testOrphanShares -vvv
contract OrphanSharesTest is Test {
    // … standard setup wiring Hub/Spoke/Oracle …

    function testOrphanShares() public {
        // 1. Supplier deposits collateral with an index that produces a
        //    non-integer share price (e.g. borrow + accrue to make R = 1.7e27).
        // 2. User borrows against it, HF drops below 1.
        // 3. Liquidator calls liquidationCall(receiveShares=false) with
        //    debtToCover chosen so sharesToLiquidator < sharesToLiquidate.
        // 4. Assert: hub.getSpokeAddedShares(assetId, spoke)
        //            > sum over users of getUserSuppliedShares(reserveId, user).
        //    The delta is ≤ 1 share.
    }
}
```

### 3-perspective audit

1. **Exploitability**: Cannot be weaponised — the orphanage is bounded by 1
   share per liquidation and benefits the supplier pool, not an attacker.
2. **Economic impact**: Negligible (sub-wei per event). Over the lifetime of
   the protocol it remains immaterial.
3. **Fix recommendation (optional)**: When `receiveShares == false` and
   `sharesToLiquidator < sharesToLiquidate`, after `hub.remove` burn the
   residual `(sharesToLiquidator − shares)` from the Spoke via `payFeeShares`
   so the Hub-side and per-user accounting stay byte-identical. Alternatively,
   compute `amountToLiquidator` from `sharesToLiquidator` with `toAddedAssetsUp`
   and let `Hub.remove` round the shares down — but this changes the liquidator
   payout direction and must be evaluated for bonus correctness.

---

## Finding L-2: Stale user `dynamicConfigKey` can delay liquidation

- **Severity**: Low (governance-mitigated by design)
- **Contract / Function / Line**: `src/spoke/Spoke.sol`, `liquidationCall` /
  `_calculateUserAccountData`, lines 354, 699–701; `LiquidationLogic._executeLiquidation`,
  lines 207–209, 364.

### Description

`liquidationCall` computes the user's health factor via
`_calculateUserAccountData(user)` which calls
`_processUserAccountData(user, /*refreshConfig=*/false)`. The collateral
factor and max-liquidation-bonus are read from
`_dynamicConfig[reserveId][userPosition.dynamicConfigKey]` — i.e. the
**user's own (possibly stale) key**, not `reserve.dynamicConfigKey`.

When governance lowers risk parameters by **adding a new** dynamic config
(`addDynamicReserveConfig`) rather than updating the existing one
(`updateDynamicReserveConfig`), users whose `dynamicConfigKey` still points
to the older, more permissive config will be evaluated with the old
collateral factor. A user that is insolvent under the new config but
healthy under the old config cannot be liquidated, and `updateUserDynamicConfig`
will revert (it calls `_refreshAndValidateUserAccountData` which itself
enforces `healthFactor ≥ HEALTH_FACTOR_LIQUIDATION_THRESHOLD`).

### Attack scenario / Impact

* Governance reduces `collateralFactor` 80% → 50% by adding a new key.
* A user with `HF_old = 1.05` / `HF_new = 0.66` sits in limbo: cannot be
  liquidated (HF check uses old factor), cannot refresh (`updateUserDynamicConfig`
  reverts on the now-insolvent position).
* The user's debt keeps accruing interest; eventually `HF_old` also drops below
  1 and the position becomes liquidatable. During the interim the protocol
  carries extra unbacked risk.
* **Mitigation exists in code**: governance can call
  `updateDynamicReserveConfig(reserveId, oldKey, newConfig)` to force the
  change on the existing key, immediately making the user liquidatable.
  `_validateUpdateDynamicReserveConfig` only forbids setting
  `collateralFactor == 0`.

### PoC (Foundry sketch)

```solidity
function testStaleConfigBlocksLiquidation() public {
    // 1. List reserve with collateralFactor = 80_00 (key 0).
    // 2. User supplies collateral, enables as collateral, borrows to HF ≈ 1.05.
    // 3. Governance calls addDynamicReserveConfig with collateralFactor = 50_00 (key 1).
    // 4. vm.expectRevert: liquidationCall reverts (HealthFactorNotBelowThreshold).
    // 5. vm.expectRevert: spoke.updateUserDynamicConfig(user) reverts (HealthFactorBelowThreshold).
    // 6. Governance calls updateDynamicReserveConfig(reserveId, 0, newConfig with CF=50_00).
    // 7. liquidationCall now succeeds.
}
```

### 3-perspective audit

1. **Exploitability**: Requires governance to (a) lower a collateral factor
   and (b) choose `addDynamicReserveConfig` over `updateDynamicReserveConfig`.
   No unprivileged attacker can trigger this alone.
2. **Economic impact**: Delayed liquidation of an insolvent position; protocol
   insolvency exposure grows with the interest accrual during the delay.
   Fundamentally the protocol remains solvent (collateral is still held) —
   only risk realisation is postponed.
3. **Fix recommendation**: Either (a) document the operational requirement
   that risk reductions **must** use `updateDynamicReserveConfig` on existing
   keys when immediate effect is desired, or (b) have `liquidationCall` use
   `min(userKey, reserveKey)` config so the more conservative factor always
   applies — note this changes the "grandfathering" semantics that the
   current design intentionally provides.

---

## Finding L-3: Theoretical `int200` / `uint200` cast overflow on `premiumOffsetRay` and `deficitRay`

- **Severity**: Low (theoretical; requires debt sizes far beyond any realistic TVL)
- **Contract / Function / Line**:
  - `src/hub/libraries/Premium.sol` `calculatePremiumRay`, line 22.
  - `src/hub/Hub.sol` `reportDeficit`, line 322 (`deficitAmountRay.toUint200()`).
  - `src/hub/Hub.sol` `_validateApplyPremiumDelta`, line 958
    (`newPremiumOffsetRay.toInt200()`).
  - `src/spoke/libraries/UserPositionUtils.sol` `calculatePremiumDelta`,
    lines 68–73.

### Description

Both the Asset and SpokeData structs pack `premiumOffsetRay` into `int200`
and `deficitRay` into `uint200`. The intermediate computations produce
`uint256` / `int256` values that are downcast at the storage boundary:

```
newPremiumOffsetRay = (newPremiumShares * drawnIndex).signedSub(
    premiumDebtRay - restoredPremiumRay
);
… .toInt200();
```

```
deficitAmountRay = uint256(drawnShares) * asset.drawnIndex
                 + premiumDelta.restoredPremiumRay;
asset.deficitRay += deficitAmountRay.toUint200();
```

`drawnShares` is `uint120` (max ≈ 1.33e36) and `drawnIndex` is `uint120`
(typically O(1e27), can grow under high rates).  The product
`drawnShares * drawnIndex` reaches `int200` / `uint200` bounds
(≈ 1.6e60 / 1.6e60) only when `drawnShares ≳ 1e33` *and* `riskPremium`
is at its `MAX_ALLOWED_COLLATERAL_RISK = 1000_00` cap simultaneously — i.e.
the position represents ≈ 1e33 underlying units (≈ 1e15 tokens for an
18-decimal asset, ≈ 1e27 tokens for a 6-decimal asset).

### Attack scenario / Impact

* Not reachable with any plausible mainnet TVL.
* If ever reached, the cast reverts (`SafeCast.toUint200` /
  `toInt200` revert on overflow), permanently freezing the affected
  account's `restore` / `repay` / `reportDeficit` paths until governance
  intervenes (e.g. via `eliminateDeficit`).

### PoC

Purely symbolic; no realistic on-chain state reaches the bound.

### 3-perspective audit

1. **Exploitability**: None in practice — requires debt sizes 6+ orders of
   magnitude above the entire stablecoin supply.
2. **Economic impact**: Theoretical only.
3. **Fix recommendation**: Optionally widen `premiumOffsetRay` to `int208`
   and `deficitRay` to `uint208` if storage layout permits; or document the
   hard cap as an explicit protocol invariant.

---

## Areas verified clean (no bug found)

For completeness, the following high-risk patterns were examined and
**cleared**:

- **Reentrancy**: every state-mutating Spoke entry-point (`supply`,
  `withdraw`, `borrow`, `repay`, `liquidationCall`, `setUsingAsCollateral`,
  `updateUserRiskPremium`, `updateUserDynamicConfig`) carries
  `nonReentrant` (transient storage guard). The Hub has no reentrancy
  surface to standard ERC20s (scope excludes non-standard hooks).
- **`MustNotLeaveDust` enforcement**: cross-checked against
  `tests/misc/z3/debt_to_liquidate.py` — the
  `debtToCover ≥ drawnSharesToLiquidate.rayMulUp(drawnIndex)
  + premiumDebtRayToLiquidate.fromRayUp()` invariant holds for every
  rounding combination.
- **Self-liquidation**: `_validateLiquidationCall` rejects
  `user == liquidator`; net-worth analysis confirms a proxy-mediated
  self-liquidation yields zero profit (the bonus is paid out of the user's
  own collateral).
- **`_evaluateDeficit`**: `activeCollateralCount > 1` short-circuit correctly
  defers deficit reporting until the *last* collateral reserve is emptied;
  `borrowCount > 1` correctly forces `notifyReportDeficit` to write off
  every remaining debt reserve.
- **Cross-Hub liquidation**: collateral and debt Hubs are decoupled;
  `hub.remove` / `hub.restore` are independent calls and cannot interfere.
- **Frozen / paused flags**: paused reserves cannot be liquidated; frozen
  reserves can be liquidated only with `receiveShares == false`
  (intentional — frozen blocks new activity, not liquidations).
- **Premium threshold DoS during `notifyReportDeficit`**: if writing off a
  low-risk-premium user would push the Spoke above
  `riskPremiumThreshold`, `_applyPremiumDelta` reverts and rolls back the
  liquidation. This is the threshold working as designed (governance-set).
