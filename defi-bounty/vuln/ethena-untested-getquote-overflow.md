# PSM `_getQuote` — Overflow DoS and Pricing Math Issues in Untested Code

**Status:** Potential bug in untested code (NOT submitted to Immunefi)
**Severity:** Medium
**Source:** Test coverage gap analysis (PSM has ZERO test files, `_getQuote` never fuzzed)
**Contract:** `/home/z/fkr-step1/defi-bounty/contracts/PSM.sol`
**Function:** `_getQuote` (line 1571) / `getQuote` (line 1108) / `swap` (line 268)

---

## Summary

The PSM `_getQuote` internal function computes swap output amounts using multiplications that can overflow `uint256` when the peg price is near its maximum allowed value (`MAX_PEG_PRICE = 1000e18`) and the input amount is near `uint128` max. In Solidity 0.8+, arithmetic overflow causes an automatic revert, resulting in a denial-of-service where large swaps fail. The code comment acknowledges this risk for "abnormally high decimals" but the overflow is also reachable with **standard 18-decimal tokens** and high peg prices.

Additionally, the dual-path pricing logic (`min(oneToOneAmountOut, oracleAmountOut)`) has a fee-bypass characteristic that is completely untested and could have economic implications.

---

## Issue 1: Overflow in `swapForCollateral` Pricing

### Vulnerable code (PSM.sol, lines 1605–1610)

```solidity
} else {
    // netAmountIn assets → collateral at pegPrice
    oneToOneAmountOut = (uint256(netAmountIn) * pegPrice * (10 ** _collateralConfig.decimals)
            / (ONE_ETHER * (10 ** assetDecimals)))
    .toUint128();
}
```

### Overflow analysis

For `swapForCollateral` (asset → collateral):
- `netAmountIn`: `uint128`, max = `type(uint128).max` ≈ `3.4e38`
- `pegPrice`: `uint128`, max = `MAX_PEG_PRICE` = `1000e18` = `1e21`
- `10 ** _collateralConfig.decimals`: for 18-decimal collateral = `1e18`

**Multiplication:** `uint256(netAmountIn) * pegPrice * (10 ** decimals)`
- Worst case: `3.4e38 * 1e21 * 1e18 = 3.4e77`
- `uint256` max = `2^256 - 1` ≈ `1.15e77`
- **`3.4e77 > 1.15e77` → OVERFLOW → revert**

### Practical threshold

With 18-decimal collateral and `pegPrice = 1000e18`:
- Overflow occurs when `netAmountIn > 1.15e77 / (1e21 * 1e18) = 1.15e38`
- `uint128` max ≈ `3.4e38`
- So any `amountIn` between `~1.15e38` and `3.4e38` causes a revert

With `pegPrice = 1e18` (1 USD, the expected value for a stablecoin PSM):
- Overflow threshold: `netAmountIn > 1.15e77 / (1e18 * 1e18) = 1.15e41`
- But `uint128` max is `3.4e38`, so **no overflow** at pegPrice = 1e18 with 18 decimals

The overflow is only reachable when `pegPrice` is significantly above 1 USD (e.g., if the "asset" is a token worth $100+ and the peg is set accordingly). The `MAX_PEG_PRICE` of $1000 enables this.

### Same issue in oracle path (lines 1620–1625)

```solidity
} else {
    // gross amountIn assets * pegPrice / oraclePrice = collateral
    oracleAmountOut = (uint256(amountIn) * pegPrice * (10 ** _collateralConfig.decimals)
            / (oraclePrice * (10 ** assetDecimals)))
    .toUint128();
}
```

Same multiplication, same overflow risk.

### Impact

- **DoS:** Large swaps revert, preventing users from executing valid swaps within their rate limits.
- **No fund loss:** The overflow causes a revert, not a wrong result.
- **Rate limit interference:** If a benefactor's max swap is near the overflow threshold, they effectively cannot use their full allocation.

### Why no test caught this

PSM has **zero test files**. No fuzz test exists for `_getQuote` or `swap`. The overflow would be caught by a simple fuzz test:

```solidity
function testFuzz_getQuote_overflow(uint128 amountIn, uint128 pegPrice) public {
    pegPrice = uint128(bound(pegPrice, 1e18, 1000e18));
    // ... setup ...
    // this reverts for large amountIn + high pegPrice
    (, uint128 amountOut) = psm.getQuote(benefactor, collateral, amountIn, false);
    assertGt(amountOut, 0);
}
```

---

## Issue 2: Fee Bypass When Oracle Diverges from Peg

### The pricing logic (PSM.sol, lines 1564–1631)

```solidity
// 1. oneToOneAmountOut — based on pegPrice, using NET amountIn (after fee)
oneToOneAmountOut = netAmountIn * pegPrice / ...  // (simplified)

// 2. oracleAmountOut — based on oraclePrice, using GROSS amountIn (NO fee)
oracleAmountOut = amountIn * oraclePrice / ...  // (simplified)

// 3. Take the minimum
amountOut = oneToOneAmountOut < oracleAmountOut ? oneToOneAmountOut : oracleAmountOut;
```

### The fee bypass

When the oracle path wins (i.e., `oracleAmountOut < oneToOneAmountOut`), the output is computed from **gross `amountIn`** with **no fee deducted**. The fee is only reflected in `oneToOneAmountOut` (which uses `netAmountIn = amountIn - feeAmount`).

**For `swapForAsset` (collateral → asset):**
- Oracle path wins when `oraclePrice < pegPrice` (collateral depegged below peg).
- In this case, `amountOut = oracleAmountOut` (gross, no fee).
- The user pays the "depeg tax" (gets fewer assets) but **no explicit fee** is collected.
- The protocol receives `amountIn` collateral but sends `oracleAmountOut` assets — the difference between peg-based and oracle-based output is the implicit "fee" absorbed by the depeg.

**For `swapForCollateral` (asset → collateral):**
- Oracle path wins when `oraclePrice > pegPrice` (collateral above peg).
- Same: `amountOut = oracleAmountOut` (gross, no fee).
- User gets fewer collateral, no explicit fee.

### Is this a bug?

This is a **design choice** documented in the code comments (lines 1564–1570). However:

1. **No test verifies** that the fee is correctly skipped/collected in each scenario.
2. **No fuzz test** checks that the `min()` selection is correct across the full oracle/peg price space.
3. **The `feeAmount` return value of `_getQuote`** is set to the computed fee regardless of which path wins. This means `getQuote` and `swap` report a `feeAmount` that **may not actually be collected** if the oracle path wins. The `SwapExecuted` event emits `feeAmount` which could be misleading (reported but not actually deducted from the output).

### Impact

- **Misleading fee reporting:** `SwapExecuted.feeAmount` may report a non-zero fee even when the oracle path wins and no fee is actually collected.
- **Economic:** If the oracle can be manipulated within the `minOraclePrice`/`maxOraclePrice` bounds (even by a tiny amount), a user could push the oracle path to win and avoid paying the fee. With a 1% fee and an oracle that wobbles 0.1% around peg, the peg path wins (fee collected). But if oracle drops 0.5% below peg (within typical depeg thresholds), the oracle path wins and **no fee is collected** — the user gets the depeg-adjusted rate instead.

---

## Issue 3: `getQuote` Is Non-View and Emits Events

### The problem (PSM.sol, lines 1108–1142)

```solidity
function getQuote(address benefactor, address collateral, uint128 amountIn, bool isSwapForAsset)
    external
    nonReentrant          // ← not view
    returns (uint128 feeAmount, uint128 amountOut)
{
    // ...
    (uint256 oraclePrice, uint256 updatedAt) = IOracleFeed(_collateralConfig.oracleFeed).getPrice();
    _validateOraclePrice(_collateralConfig, collateral, isSwapForAsset, oraclePrice, updatedAt);
    // ↑ _validateOraclePrice emits OraclePriceValidated event
    return _getQuote(...);
}
```

`getQuote` is **not a `view` function** because `_validateOraclePrice` emits `OraclePriceValidated`. This means:

1. **Off-chain integrators cannot use `getQuote` via `eth_call`** when the oracle is depegged or stale — it will revert.
2. **`getQuote` costs gas** (emits event) even when called on-chain.
3. **Event spam:** Any caller can repeatedly invoke `getQuote` to emit `OraclePriceValidated` events, cluttering the event log.
4. **Cannot be called from `staticcall` context** (e.g., from a view function in another contract).

### What a quote function should do

A `getQuote` function is typically expected to be:
- `view` (no state changes, no events)
- Non-reverting (return the quote even if the swap would revert, or return a sentinel value)
- Callable via `eth_call` for off-chain estimation

The PSM `getQuote` violates all three. The intent (quote matches execution behavior) is reasonable, but the implementation (emitting events, reverting on depeg) makes it unusable as a standard quote function.

### Impact

- **Integration friction:** DApps and aggregators cannot reliably quote PSM swaps.
- **No test verifies** this behavior — no test checks that `getQuote` reverts on depeg, emits events, or cannot be called statically.

---

## Test Coverage

**Zero.** No test file exists for PSM. Specifically:

| What's Not Tested | Consequence |
|-------------------|-------------|
| `getQuote` happy path | Quote correctness unverified |
| `getQuote` with depegged oracle | Revert behavior unverified |
| `getQuote` as non-view | Event emission unverified |
| `_getQuote` with extreme amounts | Overflow unverified |
| `_getQuote` fee path selection | Fee bypass unverified |
| `_getQuote` with different decimals | Decimal scaling unverified |
| `swap` output matching `getQuote` | Quote/execution divergence unverified |
| Fuzz: amountIn × pegPrice × oraclePrice | Full pricing space unverified |

---

## Suggested Fix

### For overflow (Issue 1)

Use `SafeMath`-style checks or rearrange the computation to avoid overflow:

```solidity
// Instead of: netAmountIn * pegPrice * 10^decimals (can overflow)
// Use: (netAmountIn * pegPrice / ONE_ETHER) * 10^decimals (intermediate result is smaller)
// Or: use FullMath-style full-precision division
```

Or add an explicit bounds check:
```solidity
if (uint256(netAmountIn) > type(uint256).max / (pegPrice * (10 ** _collateralConfig.decimals))) {
    revert AmountTooLarge(netAmountIn);
}
```

### For fee reporting (Issue 2)

If the oracle path wins, set `feeAmount = 0` to accurately reflect that no fee was collected:

```solidity
amountOut = oneToOneAmountOut < oracleAmountOut ? oneToOneAmountOut : oracleAmountOut;
if (amountOut == oracleAmountOut) {
    feeAmount = 0;  // oracle path won, no fee collected
}
```

### For getQuote (Issue 3)

Split into two functions:
- `previewQuote(...)` — `view`, returns estimated output without oracle validation or events
- Keep `getQuote` for exact-quote-with-validation if needed, or remove it and let users call `swap` with `minAmountOut = 0` (but `_validateOrder` reverts on `minAmountOut == 0`)

Or make `getQuote` a `view` function that does NOT call `_validateOraclePrice` (just reads the oracle price without validation):

```solidity
function getQuote(...) external view returns (uint128 feeAmount, uint128 amountOut) {
    // ... read oracle price but don't validate/emit ...
    (uint256 oraclePrice,) = IOracleFeed(_collateralConfig.oracleFeed).getPrice();
    return _getQuote(..., oraclePrice);  // no _validateOraclePrice call
}
```

---

## Assessment

| Criterion | Assessment |
|-----------|------------|
| Overflow reachable? | YES — with pegPrice near MAX_PEG_PRICE and large amountIn |
| Overflow causes fund loss? | NO — reverts (DoS only) |
| Fee bypass is by design? | PARTIALLY — documented but untested, feeAmount reporting is misleading |
| getQuote non-view is by design? | UNCLEAR — no documentation explains why it emits events |
| Any test covers this? | NO — zero PSM tests, zero fuzz tests for pricing |
| Fix difficulty | Low (overflow) to Medium (fee reporting) |

**Conclusion:** The overflow is a real DoS vector that limits maximum swap sizes. The fee-bypass and non-view `getQuote` are design issues that need documentation and testing. All three are hidden by zero test coverage.
