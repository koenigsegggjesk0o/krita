# Balancer V3 — `LPOracleBase` Accepts Negative and Stale Chainlink Answers Without Validation

**Program:** Balancer (https://immunefi.com/bug-bounty/balancer/information/)
**KYC Status:** Not Required
**Max Bounty:** $1,000,000
**Severity:** Medium
**Area:** Oracle Manipulation
**Date:** 2026-09-24

---

## Description

`LPOracleBase.getFeedData()` reads the latest round from each
constituent token's Chainlink feed and uses the raw `answer` as the
token's USD price. There is **no validation** that:

1. `answer > 0` — Chainlink feeds can return 0 or negative values
   during feed malfunctions, depeg events, or sequencer outages on L2.
2. `feedUpdatedAt` is recent — stale feeds (where `updatedAt` is far in
   the past) are passed through unchecked. The `updatedAt` timestamp is
   returned to the caller but no minimum-freshness check is enforced
   inside `getFeedData` itself.
3. `answer` is within a sane range — no circuit-breaker against
   flash-crash answers.

The only pre-check is `_ensureSequencerUptime`, which guards against
L2 sequencer downtime but does **not** guard against the more common
failure modes (stale feeds, negative answers, broken feeds returning
`type(int256).min`).

Because `LPOracleBase` is the canonical BPT-price oracle for lending
protocols and perp DEXes that accept Balancer LP tokens as collateral,
a malfunctioning constituent feed can cause the BPT price to be
misreported by orders of magnitude. A negative `answer` will either
revert downstream (if the derived-contract's TVL computation uses
`SafeCast.toUint256`) or silently produce a wrong TVL (if it uses
signed arithmetic throughout).

## Contract + Function + Line

**Contract:** `pkg/oracles/contracts/LPOracleBase.sol`
**Function:** `getFeedData`
**Lines:** 237–252

```solidity
function getFeedData() public view returns (int256[] memory prices, uint256[] memory updatedAt) {
    _ensureSequencerUptime();

    uint256 totalTokens = _totalTokens;
    AggregatorV3Interface[] memory feeds = _getFeeds(totalTokens);
    uint256[] memory feedDecimalScalingFactors = _getFeedTokenDecimalScalingFactors(totalTokens);

    prices = new int256[](totalTokens);
    updatedAt = new uint256[](totalTokens);

    for (uint256 i = 0; i < totalTokens; i++) {
        (, int256 answer, , uint256 feedUpdatedAt, ) = feeds[i].latestRoundData();
        prices[i] = answer * feedDecimalScalingFactors[i].toInt256();
                                                     // ^^ no check: answer > 0
                                                     // ^^ no check: feedUpdatedAt recent
        updatedAt[i] = feedUpdatedAt;
    }
}
```

The same issue propagates to `latestRoundData` (lines 196–219), which
calls `getFeedData` and then computes `lpPrice = tvl.divUp(totalSupply)`
without re-validating the prices. A single negative `answer` in a
2-token weighted pool will cause `_computeTVL` to either revert
(safe) or produce a wild TVL (unsafe), depending on the derived
contract's arithmetic.

## Attack Scenario

1. A lending protocol (e.g., a fork of Aave) uses a Balancer
   WeightedLPOracle as the price source for a BPT collateral type.
2. One of the constituent tokens (say, a low-liquidity LST) has a
   Chainlink feed that malfunctions — either due to a feed migration,
   an aggregator compromise, or a sequencer outage on an L2 deployment
   that wasn't covered by `_ensureSequencerUptime` (e.g., a custom L2
   without a published sequencer uptime feed).
3. The feed returns `answer = -1` (a known malfunction signature) or
   `answer = 0` (stale/dead feed), with `updatedAt` hours or days in
   the past.
4. `getFeedData` accepts the bad answer, scales it, and passes it to
   `_computeTVL`.
5. The TVL computation either:
   - Reverts, causing a temporary DoS of all borrowing/liquidation
     flows that depend on the BPT price (griefing).
   - Or, if the derived oracle uses signed arithmetic, produces a
     wildly wrong TVL. For a 50/50 weighted pool with one feed
     returning `-1` and the other returning `$1_000`, the TVL could
     be negative, which when cast to `uint256` becomes
     `type(uint256).max`, making the BPT appear infinitely valuable.
6. An attacker borrows against the "infinitely valuable" BPT and drains
   the lending protocol.

A more realistic variant: the stale feed returns an outdated (high)
price for a token that has since depegged. The BPT is overvalued, the
attacker deposits BPT, borrows against the inflated valuation, and
exits before the feed updates.

## Proof of Concept (Foundry)

```solidity
// SPDX-License-Identifier: MIT
pragma solidity ^0.8.24;

import {Test} from "forge-std/Test.sol";

/// Minimal reproduction of LPOracleBase.getFeedData's missing validation.
contract BalancerLPOracleValidationTest is Test {
    struct FeedRound {
        int256  answer;
        uint256 updatedAt;
    }

    FeedRound[] public feeds;

    function _getFeedData(uint256[] memory scalingFactors)
        internal view returns (int256[] memory prices, uint256[] memory updatedAt)
    {
        prices = new int256[](feeds.length);
        updatedAt = new uint256[](feeds.length);
        for (uint256 i = 0; i < feeds.length; i++) {
            (, int256 answer, , uint256 feedUpdatedAt, ) =
                (uint80(0), feeds[i].answer, uint256(0), feeds[i].updatedAt, uint80(0));
            prices[i] = answer * int256(scalingFactors[i]);
            updatedAt[i] = feedUpdatedAt;
        }
        // No checks on answer > 0 or feedUpdatedAt freshness.
    }

    function test_NegativeAnswerPropagates() public {
        feeds.push(FeedRound({answer:  2_000e18, updatedAt: block.timestamp}));
        feeds.push(FeedRound({answer: -1,         updatedAt: block.timestamp}));

        uint256[] memory factors = new uint256[](2);
        factors[0] = 1; factors[1] = 1;

        (int256[] memory prices, ) = _getFeedData(factors);

        // prices[1] is -1, which will propagate to _computeTVL unchecked.
        assertLt(prices[1], 0, "negative answer should propagate");
    }

    function test_StaleFeedPropagates() public {
        feeds.push(FeedRound({answer: 2_000e18, updatedAt: block.timestamp - 7 days}));
        feeds.push(FeedRound({answer: 1_000e18, updatedAt: block.timestamp}));

        uint256[] memory factors = new uint256[](2);
        factors[0] = 1; factors[1] = 1;

        (, uint256[] memory updatedAt) = _getFeedData(factors);

        // updatedAt[0] is 7 days old, but getFeedData does not revert.
        assertLt(updatedAt[0], block.timestamp - 6 days, "stale feed accepted");
    }
}
```

## Impact

- **Negative/zero answers**: DoS (revert) in best case, infinite-BPT-price
  exploit in worst case (if downstream casts negative TVL to uint256).
- **Stale answers**: overvaluation of BPT collateral, enabling
  undercollateralised borrowing and lending-protocol drains.
- **Combined with sequencer-outage edge cases**: the
  `_ensureSequencerUptime` check only covers L2s that publish a
  sequencer uptime feed; L1 and L2s without such a feed have no
  malfunction guard at all.

The issue affects every protocol that consumes a Balancer V3 LP oracle
(`WeightedLPOracle`, `StableLPOracle`, `EclpLPOracle`, and
`DynamicWeightedLPOracle`) since they all inherit `getFeedData` from
`LPOracleBase`.

## Severity

**Medium** — requires a malfunctioning or stale Chainlink feed, which
is an external dependency. The impact ranges from DoS to
collateral-drain depending on the downstream consumer's arithmetic.
The bug is a missing-validation issue rather than a logic error in
Balancer's own code.

## Three-Perspective Audit

**1. Attacker perspective.** The attacker does not need to compromise
the Chainlink feed — they only need to wait for one to malfunction
(historically, Chainlink feeds have returned stale or zero values
during migrations and during L2 sequencer outages). When a feed
malfunctions, the attacker can exploit the mispriced BPT in any
downstream protocol that uses the Balancer LP oracle. The attacker's
profit comes from the downstream protocol, not from Balancer directly.

**2. Protocol team perspective.** The fix is to add standard Chainlink
validation in `getFeedData`:

```solidity
for (uint256 i = 0; i < totalTokens; i++) {
    (, int256 answer, , uint256 feedUpdatedAt, ) = feeds[i].latestRoundData();
    require(answer > 0, "LPOracle: non-positive answer");
    require(feedUpdatedAt >= block.timestamp - STALENESS_THRESHOLD, "LPOracle: stale feed");
    prices[i] = answer * feedDecimalScalingFactors[i].toInt256();
    updatedAt[i] = feedUpdatedAt;
}
```

The `STALENESS_THRESHOLD` should be configurable per-oracle (different
feeds have different heartbeat expectations). A circuit-breaker on the
answer magnitude (e.g., `answer < 1e6 || answer > 1e30`) would further
harden against wild values.

**3. Auditor perspective.** This is the canonical "missing Chainlink
validation" pattern. Every Chainlink consumer must check (a) `answer >
0`, (b) `updatedAt` is recent, and (c) `answeredInRound >= roundId`.
Balancer's `LPOracleBase` skips all three. The `_ensureSequencerUptime`
check is a partial mitigation for L2 sequencer outages but does not
cover the broader malfunction space. A reviewer should flag any
`latestRoundData` consumer that does not perform these standard checks.
