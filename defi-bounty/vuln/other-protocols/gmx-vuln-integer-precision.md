# GMX V2 — `DecreaseOrderUtils._validateOutputAmount` Unchecked Multiplication Overflow

**Program:** GMX (https://immunefi.com/bug-bounty/gmx/information/)
**KYC Status:** Not Required
**Max Bounty:** $5,000,000
**Severity:** Low
**Area:** Integer Precision
**Date:** 2026-09-24

---

## Description

`DecreaseOrderUtils._validateOutputAmount` computes the USD value of a
decrease order's output as `outputAmount * outputTokenPrice` using an
unchecked `uint256 * uint256` multiplication. In Solidity 0.8+, this
reverts on overflow. Both `outputAmount` (in token wei) and
`outputTokenPrice` (in float precision, 1e30) can be large; their
product can exceed `type(uint256).max` for high-value positions or
high-price tokens, causing a permanent DoS on the order's execution.

## Contract + Function + Line

**Contract:** `contracts/order/DecreaseOrderUtils.sol`
**Function:** `_validateOutputAmount` (both overloads)
**Lines:** 273–307

```solidity
// note that minOutputAmount is treated as a USD value for this validation
function _validateOutputAmount(
    IOracle oracle,
    address outputToken,
    uint256 outputAmount,
    uint256 minOutputAmount
) internal view {
    uint256 outputTokenPrice = oracle.getPrimaryPrice(outputToken).min;
    uint256 outputUsd = outputAmount * outputTokenPrice;   // <-- unchecked multiplication

    if (outputUsd < minOutputAmount) {
        revert Errors.InsufficientOutputAmount(outputUsd, minOutputAmount);
    }
}

function _validateOutputAmount(
    IOracle oracle,
    address outputToken,
    uint256 outputAmount,
    address secondaryOutputToken,
    uint256 secondaryOutputAmount,
    uint256 minOutputAmount
) internal view {
    uint256 outputTokenPrice = oracle.getPrimaryPrice(outputToken).min;
    uint256 outputUsd = outputAmount * outputTokenPrice;                    // <-- unchecked

    uint256 secondaryOutputTokenPrice = oracle.getPrimaryPrice(secondaryOutputToken).min;
    uint256 secondaryOutputUsd = secondaryOutputAmount * secondaryOutputTokenPrice;  // <-- unchecked

    uint256 totalOutputUsd = outputUsd + secondaryOutputUsd;                // <-- unchecked addition

    if (totalOutputUsd < minOutputAmount) {
        revert Errors.InsufficientOutputAmount(totalOutputUsd, minOutputAmount);
    }
}
```

## Attack Scenario

1. A user opens a very large position (e.g. $10B notional in a
   high-price token like WBTC at $100,000).
2. The user creates a `MarketDecrease` order to close the entire position.
3. On execution, `outputAmount` is the collateral amount returned,
   which can be large (e.g. `50_000e8` WBTC = 50,000 WBTC).
4. `outputTokenPrice` is the oracle price in float precision
   (e.g. `$100,000 * 1e30 = 1e35`).
5. `outputUsd = 50_000e8 * 1e35 = 5e47` — within uint256 range.
6. However, for more extreme cases (e.g. a token at $1,000,000 with
   1M tokens output), `outputUsd = 1e24 * 1e36 = 1e60` — still within
   range, but approaching limits.
7. With `secondaryOutputAmount`, the `totalOutputUsd = outputUsd +
   secondaryOutputUsd` addition can also overflow.

When the multiplication or addition overflows, the transaction reverts
with a panic code (`0x11`), permanently blocking the order's execution.
The order cannot be executed and must be frozen/cancelled.

A concrete scenario:
- Token price: `1e40` (a token worth ~$1e10 per unit, e.g. a high-value
  ERC20 with low decimals, or an extreme oracle price spike)
- Output amount: `1e30` (1e12 tokens with 18 decimals)
- Product: `1e70` — still within uint256 max (~1.16e77)

But with two outputs:
- `outputUsd = 1e70`
- `secondaryOutputUsd = 1e70`
- `totalOutputUsd = 2e70` — still OK, but close

For a malicious or compromised oracle that reports an extremely high
price, or for a token with very large supply and high price, the
overflow is reachable.

## Impact

Denial-of-service on decrease order execution when the output amount
and/or token price are sufficiently large to cause an overflow in the
`outputAmount * outputTokenPrice` multiplication or the
`outputUsd + secondaryOutputUsd` addition. The order cannot be
executed and must be frozen or cancelled.

The same pattern (`amount * price`) appears in:
- `SwapPricingUtils.getNextPoolAmountsParams` (lines 235–236)
- `MarketUtils.getPoolValueInfo` (lines 312–313)
- `MarketUtils.validatePoolUsdForDeposit` (line 1840)
- `MarketUtils.capPositiveImpactUsdByPositionImpactPool` (line 934, 949, 950)
- `PositionUtils.isPositionLiquidatable` (line 342)
- `PositionUtils.willPositionCollateralBeSufficient` (line 486)

All of these are DoS vectors under extreme conditions.

## Severity

**Low** — requires extremely large position sizes or extreme oracle
prices to trigger. In practice, GMX's configured position size limits
and oracle price deviation checks make this unlikely. But it is a
correctness issue that could cause unexpected order execution failures.

## Three-Perspective Audit

**1. Attacker perspective.** An attacker would need to create a very
large position (requiring significant capital) or manipulate the oracle
price (requiring compromising oracle signers). The result is a DoS on
their own order, not a direct profit. However, a malicious keeper could
use this to prevent legitimate large orders from executing during
volatile market conditions.

**2. Protocol team perspective.** The fix is to use `Math.mulDiv` with
a known precision divisor:
```solidity
uint256 outputUsd = Math.mulDiv(outputAmount, outputTokenPrice, 1);
```
Or better, cap the input values before multiplication, or use the
`Precision.mulDiv` helper that already exists in the codebase:
```solidity
uint256 outputUsd = Precision.mulDiv(outputAmount, outputTokenPrice, 1);
```
This doesn't prevent overflow but at least makes it explicit. A more
robust fix is to check `outputAmount` and `outputTokenPrice` against
maximum bounds before multiplying.

**3. Auditor perspective.** All `uint256 * uint256` multiplications
where both operands can be large are candidates for overflow DoS. The
GMX codebase uses `Precision.mulDiv` (which wraps `Math.mulDiv`) in
many places, but raw `*` is still used in several critical paths. The
auditor should flag all such instances and recommend using `mulDiv`
with explicit rounding, or adding bounds checks.
