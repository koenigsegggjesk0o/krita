# Aave V4 Audit — Hub / Spoke Core Accounting

## Summary

Deep audit of `Hub.sol` (`add`, `remove`, `draw`, `restore`, `reportDeficit`,
`eliminateDeficit`, `refreshPremium`, `payFeeShares`, `transferShares`,
`sweep`, `reclaim`, `mintFeeShares`), `AssetLogic.sol`, `SharesMath.sol`,
`Premium.sol`, `MathUtils.sol`, `WadRayMath.sol`, `PercentageMath.sol`, and
the `Spoke.sol` user-facing functions (`supply`, `withdraw`, `borrow`,
`repay`).

**No Critical or High severity bug was identified in the core accounting
path.**  Rounding directions are consistent (debt rounds against the user,
collateral rounds against the user, fees round in favour of the protocol),
share-price manipulation is mitigated by the 1e6 virtual-shares/assets
dead-zone, and the Hub↔Spoke trust boundary is enforced via balance checks
on every `add`/`restore`/`reclaim`.

Below are the lower-severity observations.

---

## Finding L-4: Borrow allowance consumed on debt delta, not on `amount` — premium drift

- **Severity**: Low (consistent with Aave V3 credit-delegation semantics)
- **Contract / Function / Line**:
  `src/position-manager/TakerPositionManager.sol`, `borrowOnBehalfOf`,
  lines 223–271.

### Description

```solidity
require(currentAllowance >= amount, InsufficientBorrowAllowance(currentAllowance, amount));
…
uint256 borrowedAssetsBefore = ISpoke(spoke).getUserTotalDebt(reserveId, onBehalfOf);
(uint256 borrowedShares, uint256 borrowedAmount) = ISpoke(spoke).borrow(...);
uint256 borrowedAssetsAfter  = ISpoke(spoke).getUserTotalDebt(reserveId, onBehalfOf);
_updateBorrowAllowance({
  …,
  newAllowance: currentAllowance.zeroFloorSub(borrowedAssetsAfter - borrowedAssetsBefore)
});
```

The `require` gates only on the **input principal** `amount`, but the
allowance is debited by the **actual total-debt delta** which, due to
`rayMulUp` on the drawn shares and the premium shares minted by
`_notifyRiskPremiumUpdate`, can exceed `amount` by 1–2 wei of underlying.

The same pattern exists in `withdrawOnBehalfOf` for the supply side; there
the inequality was *proved* safe (the debit is always `≥ withdrawnAmount`,
so the owner is never over-credited).  For `borrowOnBehalfOf` the debit can
be *strictly greater* than `amount`, so a spender with allowance exactly
`amount` can push the realised debt increase slightly above the approved
envelope; the residual is absorbed by `zeroFloorSub` clamping to 0.

### Attack scenario / Impact

* Owner approves spender for `X`.
* Spender calls `borrowOnBehalfOf(X)`; actual debt increase is
  `X + δ` (δ ≤ 2 wei of underlying plus the instantaneous premium accrual
  bucket, which is 0 for a fresh borrower and bounded by `drawnShares *
  riskPremium / 10000 * (index growth since last refresh)` for an existing
  borrower).
* Allowance is set to `max(0, X − (X + δ)) = 0` instead of going negative.
* Over many small borrows the spender can therefore borrow marginally more
  total debt than the owner authorised — but only by rounding dust, and
  only while each individual call still satisfies `amount ≤ allowance`.

The premium component is non-zero only when the user already has stale
premium shares whose offset has drifted; in the steady-state refresh model
the premium debt at borrow time is 0, so δ collapses to ≤ 2 wei.

### PoC (Foundry sketch)

```solidity
function testBorrowAllowanceDrift() public {
    // 1. Owner approves spender for exactly X underlying.
    // 2. Spender calls borrowOnBehalfOf(X).
    // 3. Assert: getUserTotalDebt(owner) == X + δ  with δ ∈ {0,1,2}.
    // 4. Assert: borrowAllowance(owner, spender) == 0  (not X - δ).
}
```

### 3-perspective audit

1. **Exploitability**: The overshoot is bounded by rounding dust plus any
   stale premium accrual — not a material fund-theft vector.
2. **Economic impact**: Negligible per call; only relevant if an owner
   relies on the allowance as a hard ceiling and the spender bundles many
   borrows in a single tx.
3. **Fix recommendation**: Either tighten the `require` to
   `currentAllowance >= borrowedAssetsAfter - borrowedAssetsBefore`
   (requires a preview), or document that the allowance is a principal
   ceiling consistent with Aave V3 and the premium is a separate, protocol-
   controlled cost.

---

## Finding L-5: Donations to the Hub are not auto-skimmed despite `IHubBase.add` docstring

- **Severity**: Low / Informational
- **Contract / Function / Line**:
  - `src/hub/interfaces/IHubBase.sol`, `add` natspec, lines 92–99
    ("Extra untracked underlying liquidity … can be skimmed … through this
    action").
  - `src/hub/Hub.sol`, `add`, lines 200–221.

### Description

The `add` docstring promises that extra untracked balance in the Hub can be
skimmed into `liquidity` via `add`.  The implementation does **not** do
this — `liquidity` is set to `oldLiquidity + amount` (the input), not to
`balance`.  Donations sent directly to the Hub therefore remain
unaccounted: they do not inflate `totalAddedAssets`, do not raise the share
price, and cannot be withdrawn by suppliers.  The only way to capture them
is `TreasurySpoke.supplySkimmed` (owner-only) or `Hub.reclaim` (capped at
`swept`).

### Attack scenario / Impact

* A well-intentioned donor (or a misplaced transfer) sends tokens to the
  Hub.  The tokens are stuck until governance skims them via
  `TreasurySpoke.supplySkimmed`.  Suppliers see no benefit.
* This is **not** an inflation-attack vector — the 1e6 virtual
  shares/assets plus the `balance ≥ liquidity + amount` check on every
  `add` mean a malicious first-depositor cannot extract value from a
  later depositor.

### 3-perspective audit

1. **Exploitability**: None — donations are captured by the treasury, not
   by an attacker.
2. **Economic impact**: Donations to the Hub are effectively donations to
   the DAO treasury, not to suppliers.  This may surprise users but is not
   a vulnerability.
3. **Fix recommendation**: Align the `add` natspec with the implementation
   (remove the "skim" claim) or implement an optional skim parameter.

---

## Finding L-6: `deficitRay` counted inside `totalAddedAssets` inflates perceived share price

- **Severity**: Low / Informational (by design)
- **Contract / Function / Line**: `src/hub/libraries/AssetLogic.sol`,
  `totalAddedAssets`, lines 79–96; `Hub.reportDeficit`, lines 304–330;
  `Hub.eliminateDeficit`, lines 333–359.

### Description

`totalAddedAssets = liquidity + swept + aggregatedOwedRay.fromRayUp()
                   − realizedFees − unrealizedFees`, and
`aggregatedOwedRay = drawnShares*drawnIndex + premiumRay + deficitRay`.
Reported bad debt therefore stays inside the share-price numerator until
`eliminateDeficit` burns matching shares.  Suppliers observe a share price
that includes the (uncollectable) deficit, so `previewRemoveByShares`
returns a value larger than what `Hub.remove` can actually deliver
(`remove` is capped by `asset.liquidity`).

This is a deliberate accounting choice — the deficit is *expected* to be
eliminated by governance burning treasury shares — but it means that, in
the window between `reportDeficit` and `eliminateDeficit`, the displayed
share price overstates withdrawable liquidity.

### Attack scenario / Impact

* No direct fund theft: withdrawals are still bounded by
  `require(amount <= liquidity)`.
* Front-end / integration code that trusts `previewRemoveByShares` as
  "withdrawable" may mislead users during the deficit window.
* `eliminateDeficit` provably preserves the share price (shown analytically
  in the audit), so the inflation is temporary and bounded by the size of
  the bad debt.

### 3-perspective audit

1. **Exploitability**: None — withdrawals are liquidity-capped.
2. **Economic impact**: Cosmetic / UX; protocol solvency is preserved
   because `liquidity` is the hard ceiling on outflows.
3. **Fix recommendation**: Optionally expose a `getWithdrawableAssets`
   view that returns `min(totalAddedAssets, liquidity)` for UI use.

---

## Areas verified clean (no bug found)

- **First-depositor / donation inflation attack**: mitigated by 1e6
  virtual shares/assets (`SharesMath.VIRTUAL_ASSETS`,
  `VIRTUAL_SHARES`) **and** by `liquidity` never auto-absorbing donations
  (see L-5).  Verified the round-trip
  `toAddedSharesDown(toAddedAssetsUp(shares)) == shares` for all share
  prices `≥ 1`, which holds post-fee-minting.
- **Fee minting share-price preservation**: `_mintFeeShares` provably
  leaves `(totalAddedAssets + VIRTUAL_ASSETS) / (addedShares + VIRTUAL_SHARES)`
  unchanged (the `floor` on `toAddedSharesDown(fees)` introduces a
  sub-wei donation to existing suppliers, not to the feeReceiver).
- **`reportDeficit` share reconciliation**: `drawnShares` returned by
  `toDrawnSharesDown(drawnAmount)` is provably equal to the user's
  `drawnShares` when `drawnAmount = drawnShares.rayMulUp(drawnIndex)` —
  confirmed by case analysis on the fractional residual.
- **`_validateDraw` draw-cap arithmetic**: `drawCap * 10^decimals` cannot
  overflow `uint256` for any `uint40` cap and any decimals ≤ 18.
- **`updateAssetConfig` fee-receiver swap**: old feeReceiver retains its
  shares but its `addCap`/`drawCap`/`riskPremiumThreshold` are zeroed,
  preventing further adds/draws while preserving withdrawability.
- **`reclaim` accounting**: `asset.swept -= amount` underflow-reverts if
  `amount > swept`, so the reinvestment controller cannot mint liquidity
  from nothing.
- **`transferShares` cap enforcement**: receiver's `addCap` is checked
  against `toAddedAssetsUp(receiverSpoke.addedShares + shares)` (rounds
  up), preventing cap evasion via share transfers.
