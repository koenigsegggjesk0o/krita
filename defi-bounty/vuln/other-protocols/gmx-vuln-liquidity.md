# GMX V2 — `GlvWithdrawal._getMarketTokenAmount` Market Selection Exploitable for Favorable Conversion

**Program:** GMX (https://immunefi.com/bug-bounty/gmx/information/)
**KYC Status:** Not Required
**Max Bounty:** $5,000,000
**Severity:** Low
**Area:** Liquidity / Price Impact
**Date:** 2026-09-24

---

## Description

`GlvWithdrawalUtils._getMarketTokenAmount` converts the user's GLV
token amount into a market token amount for withdrawal. The conversion
uses the **minimized** GLV value (`maximize = false`) and the
**maximized** target market pool value (`maximize = true`). While each
individual min/max choice is conservative for the protocol, the
combination creates a systematic bias that a user can exploit by
selecting which market to withdraw from.

The GLV value is based on **all** markets in the GLV, but the market
token amount is calculated using only the **target** market's pool
value. If the target market's pool value (maximized) is low relative to
the GLV's overall value, the user receives **more** market tokens for
their GLV tokens. They can then immediately withdraw those market
tokens from the target market, potentially extracting more value than
their GLV share represents.

## Contract + Function + Line

**Contract:** `contracts/glv/glvWithdrawal/GlvWithdrawalUtils.sol`
**Function:** `_getMarketTokenAmount`
**Lines:** 291–322

```solidity
function _getMarketTokenAmount(
    DataStore dataStore,
    IOracle oracle,
    GlvWithdrawal.Props memory glvWithdrawal
) internal view returns (uint256) {
    (uint256 glvValue, ) = GlvUtils.getGlvValue(
        dataStore,
        oracle,
        glvWithdrawal.glv(),
        false // maximize — MINIMIZED glv value
    );
    uint256 glvSupply = GlvToken(payable(glvWithdrawal.glv())).totalSupply();
    uint256 glvTokenUsd = GlvUtils.glvTokenAmountToUsd(glvWithdrawal.glvTokenAmount(), glvValue, glvSupply);

    Market.Props memory market = MarketUtils.getEnabledMarket(dataStore, glvWithdrawal.market());
    MarketPoolValueInfo.Props memory poolValueInfo = MarketUtils.getPoolValueInfo(
        dataStore,
        market,
        oracle.getPrimaryPrice(market.indexToken),
        oracle.getPrimaryPrice(market.longToken),
        oracle.getPrimaryPrice(market.shortToken),
        Keys.MAX_PNL_FACTOR_FOR_WITHDRAWALS,
        true // maximize — MAXIMIZED target market pool value
    );
    uint256 marketTokenAmount = MarketUtils.usdToMarketTokenAmount(
        glvTokenUsd,
        poolValueInfo.poolValue.toUint256(),
        ERC20(market.marketToken).totalSupply()
    );

    return marketTokenAmount;
}
```

The formula is:

```
marketTokenAmount = marketTokenSupply * glvTokenUsd / poolValue_max

where:
  glvTokenUsd = glvValue_min * glvTokenAmount / glvSupply
  glvValue_min = sum of (marketTokenBalance * marketPoolValue_min / marketTokenSupply) for all GLV markets
  poolValue_max = target market's maximized pool value
```

## Attack Scenario

Consider a GLV with two markets:
- Market A: poolValue = $1,000,000 (small market)
- Market B: poolValue = $9,000,000 (large market)
- Total GLV value = $10,000,000
- GLV supply = 10,000,000 (1 GLV = $1)

A user holds 1,000 GLV tokens (worth $1,000).

**Withdraw from Market A (small pool):**
- `glvTokenUsd = $1,000` (minimized, but roughly $1,000)
- `poolValue_max` for Market A ≈ $1,000,000 (maximized)
- `marketTokenAmount = supply_A * $1,000 / $1,000,000`
- If Market A has 1,000,000 market tokens, user gets 1,000 market tokens
- Withdrawing 1,000 market tokens from Market A gives ~$1,000

**Withdraw from Market B (large pool):**
- `glvTokenUsd = $1,000`
- `poolValue_max` for Market B ≈ $9,000,000
- `marketTokenAmount = supply_B * $1,000 / $9,000,000`
- If Market B has 9,000,000 market tokens, user gets 1,000 market tokens
- Withdrawing 1,000 market tokens from Market B gives ~$1,000

In the balanced case, the result is the same regardless of market
selection. **However**, when there is a price spread between markets
(different index token, different long/short token imbalance), the
`maximize` flag on the target market's pool value creates an asymmetry:

- Withdrawing from a market whose **maximized** pool value is high
  relative to its **minimized** pool value gives fewer market tokens
  (conservative for protocol).
- Withdrawing from a market whose **maximized** pool value is low
  relative to the GLV's overall value gives more market tokens.

A user can cherry-pick the market with the most favorable conversion,
withdraw market tokens, then immediately withdraw those market tokens
from the target market at the actual (non-maximized) pool value —
potentially extracting a small profit.

The profit per withdrawal is bounded by the price spread (min vs max
oracle prices) and the withdrawal fee, making this a low-severity issue.
But repeated withdrawals could accumulate value over time.

## Impact

Low-value value extraction through market selection in GLV withdrawals.
The extractable value is bounded by the oracle price spread and
withdrawal fees, making this a minor economic issue rather than a
security vulnerability.

## Severity

**Low** — The extraction is bounded by oracle spreads and fees. The
min/max choices are individually conservative; only the combination
creates a small exploitable asymmetry. No funds are at risk of total
loss; the issue is an economic efficiency concern.

## Three-Perspective Audit

**1. Attacker perspective.** The attacker monitors GLV markets for
price spread asymmetries, then creates GLV withdrawals from the most
favorable market. The profit per withdrawal is small (bounded by the
spread between min and max oracle prices). The attack requires
monitoring and quick execution, and the profit is likely less than the
gas cost for small withdrawals.

**2. Protocol team perspective.** The fix is to use the same `maximize`
flag for both the GLV value and the target market pool value, or to
use `maximize = true` for both (which would minimize the market token
amount given to the user). Alternatively, the GLV withdrawal could
proportionally withdraw from all markets rather than allowing the user
to select a single market.

**3. Auditor perspective.** Any cross-market conversion that uses
different `maximize` flags for the numerator (GLV value) and denominator
(target market pool value) creates a potential asymmetry. The auditor
should verify that the combination of flags doesn't create exploitable
arbitrage opportunities. In this case, the asymmetry is small and
bounded by oracle spreads, but it's still a design issue worth noting.
