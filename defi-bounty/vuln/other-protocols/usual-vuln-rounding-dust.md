# Usual Labs — Rounding in _swapRWAtoStbc Return Path Can Confiscate RWA Dust

**Severity:** LOW
**Area:** DaoCollateral / Integer precision
**Bounty:** Sherlock Usual Labs Bug Bounty (#56)

---

## Description

In `DaoCollateral._swapRWAtoStbc()`, when a swap via `swapperEngine.swapUsd0()` leaves an
unmatched USD0 amount (`wadRwaNotTakenInUSD > 0`), the function converts that unmatched USD0
back to the RWA token and returns it to the caller:

```solidity
uint256 rwaTokensToReturn = _getQuoteInToken(wadRwaNotTakenInUSD, rwaToken);
IERC20Metadata(rwaToken).safeTransferFrom($.treasury, caller, rwaTokensToReturn);
matchedAmountInTokenDecimals = amountInTokenDecimals - rwaTokensToReturn;
```

`_getQuoteInToken` calls `Normalize.wadTokenAmountForPrice(wadStableAmount, wadPrice, decimals)`,
which uses `Math.Rounding.Floor`:

```solidity
function wadTokenAmountForPrice(uint256 wadStableAmount, uint256 wadPrice, uint8 tokenDecimals)
    internal pure returns (uint256)
{
    return Math.mulDiv(wadStableAmount, 10 ** tokenDecimals, wadPrice, Math.Rounding.Floor);
}
```

When `wadRwaNotTakenInUSD` is small relative to the RWA's price and decimal scale,
`rwaTokensToReturn` rounds down to **zero**. The function then sets:

```solidity
matchedAmountInTokenDecimals = amountInTokenDecimals - rwaTokensToReturn;
//                       = amountInTokenDecimals - 0 = amountInTokenDecimals
```

This means **the entire pulled RWA amount is treated as "matched"**, even though the swapper
only matched `wadRwaQuoteInUSD - wadRwaNotTakenInUSD` worth of USD0. The small unmatched RWA
slice is **confiscated** — it stays in the treasury (because `safeTransferFrom` of 0 is a no-op)
but the accounting claims it was matched. The user loses that slice and the intent's
`_orderAmountTaken` is over-incremented.

There is **no guard** checking `rwaTokensToReturn > 0` before computing
`matchedAmountInTokenDecimals` (contrast with `_burnStableTokenAndTransferCollateral` at line
543, which *does* guard `returnedCollateral == 0`).

---

## Contract / Function / Lines

**Contract:** `DaoCollateral` (impl `0x0eEc861D49f15F585D6Bb4301FC4f89BCe22AF4e`)

**File:** `src/daoCollateral/DaoCollateral.sol`, lines 614–631

```solidity
if (wadRwaNotTakenInUSD > 0) {
    if (!IERC20($.usd0).approve(address($.swapperEngine), 0)) {
        revert ApprovalFailed();
    }
    $.usd0.burnFrom(address(this), wadRwaNotTakenInUSD);

    uint256 rwaTokensToReturn = _getQuoteInToken(wadRwaNotTakenInUSD, rwaToken);
    // ← no check that rwaTokensToReturn > 0

    IERC20Metadata(rwaToken).safeTransferFrom($.treasury, caller, rwaTokensToReturn);

    matchedAmountInTokenDecimals = amountInTokenDecimals - rwaTokensToReturn;
    //                       = amountInTokenDecimals - 0   (if rounded to zero)
}
```

**Normalise helper:** `src/utils/normalize.sol` lines 74–80 (`wadTokenAmountForPrice`,
`Math.Rounding.Floor`).

---

## Attack Scenario

1. An RWA token with 6 decimals (e.g. USYC) priced at ~$1.05 (`wadPriceInUSD = 1.05e18`).
2. User swaps 1,000,000 USYC (1e6 units = 1M tokens) via `swapRWAtoStbc` with partial matching.
3. The orders almost fully match, leaving only 1 wei of USD0 unmatched
   (`wadRwaNotTakenInUSD = 1`).
4. `rwaTokensToReturn = Math.mulDiv(1, 1e6, 1.05e18, Floor) = 0` (1 * 1e6 / 1.05e18 < 1, floors to 0).
5. `safeTransferFrom(treasury, caller, 0)` — no-op, no RWA returned.
6. `matchedAmountInTokenDecimals = 1,000,000 - 0 = 1,000,000` — the entire amount is "matched".
7. The user loses the tiny unmatched RWA slice (dust) and the intent accounting over-counts the
   matched amount by the dust.

Per-instance impact is dust, but the issue is systematic: every partial match with a small
unmatched tail can confiscate a rounding slice, and repeated calls compound the loss.

---

## Impact

- **User-facing dust loss** on every partial swap where the unmatched USD0 is small relative to
  the RWA's price × decimal scale.
- **Intent accounting drift:** `_orderAmountTaken` is over-incremented, causing the intent to be
  consumed faster than the actual matched amount.
- Severity is LOW because the per-transaction loss is sub-cent; the issue is a precision hygiene
  concern rather than a fund-theft vector.

---

## Three-Perspective Audit

### 1. Attacker Perspective
Not directly exploitable for meaningful profit — the loss per call is dust. However, a
`INTENT_MATCHING_ROLE` holder could craft many small partial matches specifically to trigger the
round-to-zero path, slowly extracting dust from victims' intents over time.

### 2. Protocol / Defender Perspective
The fix is trivial: add `if (rwaTokensToReturn == 0) revert AmountTooLow();` (matching the
existing guard in `_burnStableTokenAndTransferCollateral`), or use `Math.Rounding.Ceil` for the
return conversion so the protocol absorbs the dust instead of the user.

### 3. Auditor / Sherlock-Judging Perspective
- **In scope:** `DaoCollateral` is a Critical-tier contract.
- **Severity:** LOW — rounding dust, no fund-theft path, no DoS. Sherlock's `QA`/`Low` bucket is
  the most likely home, though the intent-accounting drift edge could push it to `Low`/`Refinement`.
- The bounty scope explicitly excludes "only resulting in minor rounding/precision errors", so
  this may be judged out-of-scope. Documented for completeness.

---

## Recommended Fix

```solidity
uint256 rwaTokensToReturn = _getQuoteInToken(wadRwaNotTakenInUSD, rwaToken);
if (rwaTokensToReturn == 0) {
    revert AmountTooLow();   // mirror the guard in _burnStableTokenAndTransferCollateral
}
IERC20Metadata(rwaToken).safeTransferFrom($.treasury, caller, rwaTokensToReturn);
matchedAmountInTokenDecimals = amountInTokenDecimals - rwaTokensToReturn;
```

Alternatively, use `Math.Rounding.Ceil` in `wadTokenAmountForPrice` for this specific return
path so the user is never short-changed.

---

## References

- `DaoCollateral.sol` lines 614–631 (unmatched-return path)
- `normalize.sol` lines 74–80 (`wadTokenAmountForPrice`, `Floor`)
- `DaoCollateral.sol` line 543 (the *existing* `returnedCollateral == 0` guard in
  `_burnStableTokenAndTransferCollateral` — the same guard is missing here)
