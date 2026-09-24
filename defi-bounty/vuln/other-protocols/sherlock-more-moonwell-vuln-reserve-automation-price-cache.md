# Moonwell — `ReserveAutomation` per-period Chainlink price cache enables stale-price arbitrage

**Protocol**: Moonwell (Sherlock bounty #350, up to $250,000 USD)
**Repo**: `github.com/moonwell-fi/moonwell-contracts-v2` (cloned to `/home/z/usual-moonwell`)
**Severity**: Low–Medium (note: Chainlink feed correctness itself is out-of-scope per bounty; the cache design is the in-scope concern)
**Area**: Oracle manipulation / integer precision (price caching)

---

## 1. Description

`ReserveAutomation` sells protocol reserves (e.g. USDC) for WELL tokens via a
decaying-price mini-auction. Each mini-auction period (`miniAuctionPeriod` seconds)
starts at a premium (`startingPremium > 1e18`) and decays linearly to a discount
(`maxDiscount < 1e18`).

To prevent intra-period price manipulation, the contract **caches** the Chainlink
prices for WELL and the reserve asset at the start of each period — but only lazily,
on the **first purchase** of the period:

```solidity
// ReserveAutomation.sol:530-540
if (startPeriodTimestampCachedChainlinkPrice[startTime].wellPrice == 0) {
    (int256 wellPrice, ) = getPriceAndDecimals(wellChainlinkFeed);
    startPeriodTimestampCachedChainlinkPrice[startTime].wellPrice = wellPrice;
    (int256 reservePrice, ) = getPriceAndDecimals(reserveChainlinkFeed);
    startPeriodTimestampCachedChainlinkPrice[startTime].reservePrice = reservePrice;
}
```

All subsequent purchases in the same period (including by *different* buyers) then use
the cached prices via `getNormalizedPrice` (line 284-295, `price = cachedPrice != 0 ?
cachedPrice : price`). Combined with two further properties, this creates a stale-price
arbitrage window:

1. **No `updatedAt` staleness check.** `getPriceAndDecimals` (line 402-417) validates
   only `price > 0 && answeredInRound >= roundId && updatedAt != 0`. There is no
   `block.timestamp - updatedAt < threshold` check, so a frozen/stale Chainlink feed
   is accepted and then cached for an entire `miniAuctionPeriod`.
2. **First-buyer anchoring.** The buyer who happens to be first in a period fixes the
   price for every later buyer in that period. If the live Chainlink price moves
   materially during the period, later buyers transact at a stale rate.
3. **Asymmetric payoff.** `getAmountReservesOut` (line 347-384) computes
   `amountOut = (amountWellIn * wellPrice) / (reservePrice * discount)`. If WELL's real
   price *falls* during the period but the cached `wellPrice` is the (higher) opening
   price, every later buyer receives **more** reserves per WELL than the live market
   implies — value transferred from the protocol (reserves) to the buyer.

---

## 2. Contract / function / line

| File | Function | Line(s) |
|------|----------|---------|
| `src/market/ReserveAutomation.sol` | `getReserves` (cache write) | 516-578, esp. 530-540 |
| `src/market/ReserveAutomation.sol` | `getNormalizedPrice` (cache read) | 284-295 |
| `src/market/ReserveAutomation.sol` | `getAmountReservesOut` | 347-384 |
| `src/market/ReserveAutomation.sol` | `getPriceAndDecimals` (no staleness) | 402-417 |
| `src/market/ReserveAutomation.sol` | `currentDiscount` | 262-278 |

---

## 3. Attack scenario

1. Owner starts a sale with `miniAuctionPeriod = 1 hour` and a wide
   `startingPremium → maxDiscount` sweep (e.g. 1.1e18 → 0.9e18).
2. WELL is trading at $0.40 at the start of a period. Attacker (or any user) is the
   first buyer and caches `wellPrice = $0.40`.
3. Over the next ~55 minutes WELL drops to $0.34 on secondary markets (and Chainlink
   updates), but the cached price remains $0.40.
4. Late in the period the discount has decayed to near `maxDiscount` (0.9e18), so
   reserves are ~10% off.
5. Attacker buys reserves using WELL bought on the open market at $0.34, but the
   contract credits them at the cached $0.40 WELL price. Effective rate received:
   `(0.40 / 0.34) * (1 / 0.9) ≈ 1.31×` the fair reserve amount.
6. Attacker sells the reserves back to the market, capturing the ~24–31% spread at
   the protocol's expense.

The same cache also bites in the opposite direction (WELL rallies during the period →
buyers underpay and the protocol over-receives WELL), but an attacker self-selects the
profitable direction by only buying when the cache is stale-favourable.

---

## 4. Proof of Concept (Foundry, sketch)

```solidity
// SPDX-License-Identifier: MIT
pragma solidity 0.8.19;

import "forge-std/Test.sol";

contract ReserveAutomationStaleCacheTest is Test {
    ReserveAutomation ra;
    MockChainlink wellFeed;     // 8 decimals
    MockChainlink reserveFeed;  // 8 decimals
    MockERC20 well;
    MockERC20 reserve;          // 6 decimals, e.g. USDC

    function setUp() public {
        // deploy ra with miniAuctionPeriod = 1 hour, startingPremium = 1.1e18, maxDiscount = 0.9e18
        // fund ra with 1_000_000 USDC, initiateSale
    }

    function test_stale_cache_arbitrage() public {
        // t0: WELL @ $0.40
        wellFeed.setPrice(0.40e8);
        // first buyer caches $0.40
        vm.prank(alice);
        ra.getReserves(1_000e18, 0);
        uint256 aliceOut = reserve.balanceOf(alice);

        // advance 50 minutes, WELL now $0.34 on market + chainlink
        vm.warp(block.timestamp + 50 minutes);
        wellFeed.setPrice(0.34e8);

        // attacker buys same 1_000 WELL at cached $0.40, discount ~0.92
        deal(address(well), attacker, 1_000e18);
        vm.startPrank(attacker);
        well.approve(address(ra), 1_000e18);
        ra.getReserves(1_000e18, 0);
        uint256 attackerOut = reserve.balanceOf(attacker);
        vm.stopPrank();

        // attacker gets strictly MORE reserves than alice for the same WELL,
        // despite the *real* WELL price being lower (which should mean fewer reserves).
        assertGt(attackerOut, aliceOut);

        // the contract used wellPrice = $0.40 (cached) not $0.34 (live):
        uint256 fairOutAtLive = (1_000e18 * 0.34e18) / (1e18 /*reserve=1*/ * 0.92e18 / 1e18);
        assertGt(attackerOut, fairOutAtLive, "attacker extracted above-fair reserves via stale cache");
    }
}
```

Run: `forge test --match-test test_stale_cache_arbitrage -vv`.

---

## 5. Impact

* **Fund loss**: reserves (USDC/asset held by `ReserveAutomation` for the WELL buyback)
  are sold at a stale price, leaking value to buyers proportional to
  `(cached − live)/live × reserve_notional` over each stale period. With volatile WELL
  and long `miniAuctionPeriod`, the leak compounds across periods.
* **Scope caveat**: the Moonwell bounty states *"Chainlink feeds are assumed to
  operate correctly … Incorrect data or pricing information supplied by third-party
  oracles is out of scope."* The pure Chainlink-staleness angle is therefore OoS. The
  **in-scope** element is the contract's *own* caching policy: even with a correctly
  updating feed, the first-buyer-anchors-the-period design manufactures synthetic
  staleness of up to `miniAuctionPeriod` seconds regardless of how fast Chainlink
  updates.
* **Severity**: Low–Medium. The owner controls `miniAuctionPeriod`, so a responsible
  config (short periods) bounds exposure; but nothing on-chain prevents a long period
  being set, and the cache mechanism is the contract's own design rather than a
  Chainlink failure.

---

## 6. Three-perspective audit

**Exploitability**
No special role needed — any buyer can be the first buyer of a period (anchoring the
cache) and any later buyer benefits from a stale-favourable cache. The attacker needs
WELL price to move during the period, which is market-dependent but routine for a
volatile governance token.

**Economic impact**
Proportional to reserve notional sold per period × intra-period WELL volatility ×
`miniAuctionPeriod`. For a protocol doing recurring large reserve sales, the
compounded leakage can be material. There is no theft of funds at rest; it is a
price-execution leak.

**Fix recommendation**
- **Re-read live Chainlink each call** (remove the per-period cache) and rely on the
  decay auction + `minAmountOut` for user protection, **or**
- **Bound the cache validity**: in `getNormalizedPrice`, fall back to the live feed if
  `block.timestamp - lastRoundUpdatedAt > miniAuctionPeriod / N`, **or**
- **Add a staleness check** to `getPriceAndDecimals`:
  ```solidity
  require(block.timestamp - updatedAt < STALENESS_THRESHOLD, "stale oracle");
  ```
- **Shorten `miniAuctionPeriod`** and enforce an on-chain upper bound in `initiateSale`
  (currently `_miniAuctionPeriod` is only constrained by `_auctionPeriod %
  _miniAuctionPeriod == 0` and `> 1`, not by an absolute maximum).

---

## 7. References

* Cache write: `src/market/ReserveAutomation.sol:530-540`
* Cache read: `src/market/ReserveAutomation.sol:284-295`
* No-staleness oracle read: `src/market/ReserveAutomation.sol:402-417`
* Discount decay: `src/market/ReserveAutomation.sol:262-278`
* `initiateSale` parameter validation: `src/market/ReserveAutomation.sol:587-649`
