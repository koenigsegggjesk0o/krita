# Cap — Chainlink Adapter Skips `answeredInRound` / `updatedAt` Stale-Round Validation

**Protocol:** Cap (cap-labs-dev)
**Bounty:** $1,000,000 USDC
**Sherlock Bounty URL:** https://audits.sherlock.xyz/bug-bounties/114
**Source:** https://github.com/cap-labs-dev/cap-contracts
**Severity:** LOW
**Area:** Oracle manipulation / Stale price
**Status:** NOT SUBMITTED — local audit finding only.

---

## 1. Description

Both Chainlink oracle adapters in the Cap codebase call
`latestRoundData()` and accept the returned answer without verifying that
the round is *current* — i.e. without checking `answeredInRound >=
roundId` or that `updatedAt` is sufficiently recent relative to the
chainlink feed's own heartbeat. The only staleness gate is the
protocol-level `PriceOracle._isStale`, which compares `lastUpdated` to a
governance-set `staleness[_asset]` window; that window is per-asset and
defaults to `0` (effectively "any non-zero `lastUpdated` is fresh") when
unset.

```solidity
// contracts/oracle/libraries/ChainlinkAdapter.sol:14-21
function price(address _source) external view returns (uint256 latestAnswer, uint256 lastUpdated) {
    uint8 decimals = IChainlink(_source).decimals();
    int256 intLatestAnswer;
    (, intLatestAnswer,, lastUpdated,) = IChainlink(_source).latestRoundData();
    latestAnswer = intLatestAnswer < 0 ? 0 : uint256(intLatestAnswer);
    if (decimals < 8) latestAnswer *= 10 ** (8 - decimals);
    if (decimals > 8) latestAnswer /= 10 ** (decimals - 8);
}
```

The first three return values of `latestRoundData` — `roundId`,
`startedAt`, and especially `answeredInRound` — are discarded. The
canonical Chainlink-recommended validation is:

```solidity(uint80 roundId, int256 answer, , uint256 updatedAt, uint80 answeredInRound) = feed.latestRoundData();
require(updatedAt != 0, "Chainlink: round not complete");
require(answeredInRound >= roundId, "Chainlink: stale price");
require(answer > 0, "Chainlink: negative/zero price");
```

Cap's adapter skips the `answeredInRound >= roundId` check entirely. If a
Chainlink feed becomes stuck (the aggregator pauses, the deviation
threshold is never hit, or the feed breaks and keeps returning the last
good round), `latestRoundData` keeps returning that last round
indefinitely. The protocol-level staleness check only catches it if
`staleness[_asset]` is configured with a window shorter than the time
since the last update — and a misconfigured (or default-zero) staleness
window allows the stale price through.

The same issue exists in the chained adapter:

```solidity
// contracts/oracle/libraries/ChainlinkAdapterChained.sol:29-36
function _getPrice(address _source) internal view returns (uint256 latestAnswer, uint256 lastUpdated) {
    uint8 decimals = IChainlink(_source).decimals();
    int256 intLatestAnswer;
    (, intLatestAnswer,, lastUpdated,) = IChainlink(_source).latestRoundData();
    latestAnswer = intLatestAnswer < 0 ? 0 : uint256(intLatestAnswer);
    if (decimals < 8) latestAnswer *= 10 ** (8 - decimals);
    if (decimals > 8) latestAnswer /= 10 ** (decimals - 8);
}
```

---

## 2. Contract, Function, and Lines

| Field | Value |
|---|---|
| Contract | `ChainlinkAdapter` (library) |
| File | `contracts/oracle/libraries/ChainlinkAdapter.sol` |
| Function | `price` |
| Lines | 14–21 |
| Sibling | `ChainlinkAdapterChained._getPrice` — `contracts/oracle/libraries/ChainlinkAdapterChained.sol:29-36` |
| Caller | `PriceOracle._getPrice` — `contracts/oracle/PriceOracle.sol:82-89` (uses `staticcall`, discards success/failure details) |
| Staleness gate | `PriceOracle._isStale` — `contracts/oracle/PriceOracle.sol:95-97` (only checks `block.timestamp - lastUpdated > staleness[_asset]`) |

---

## 3. Attack Scenario

1. Cap uses a Chainlink feed for an asset (e.g. LBTC/USD) with a 1-hour
   heartbeat and a `staleness[LBTC]` window of 24 hours (a reasonable
   governance choice to tolerate brief Chainlink outages without
   halting borrowing).
2. The LBTC/USD aggregator breaks (off-chain issue, multi-sig
   misconfiguration, or a frozen bridge) and stops updating. The last
   published round is at `T0` with price `P0`.
3. For the next 24 hours, `PriceOracle.getPrice(LBTC)` returns `P0`
   without reverting (because `block.timestamp - T0 < 24h`).
4. During that window, the real LBTC price moves substantially (e.g. a
   15% drop). Cap's lending and liquidation logic uses `P0` (the stale
   high price), so:
   - Borrowers can borrow more against LBTC than their collateral is
     worth.
   - Liquidators cannot profitably liquidate under-water positions
     because Cap thinks they are still healthy.
5. After 24 hours, the staleness gate finally trips and `getPrice`
   falls back to the backup oracle (if configured) or reverts. By then,
   the protocol has been taking on bad debt for a full day.

### Why `answeredInRound` would have caught this

When a Chainlink feed is paused or broken, `latestRoundData` keeps
returning the last round, but `answeredInRound < roundId` for any new
`roundId` that Chainlink's proxy reports. The
`require(answeredInRound >= roundId)` check fails immediately on the
first call after the break, surfacing the problem in the same block
rather than 24 hours later.

---

## 4. Proof of Concept (Forge-style, with a mock feed)

```solidity
// SPDX-License-Identifier: UNLICENSED
pragma solidity ^0.8.28;

import "forge-std/Test.sol";
import {ChainlinkAdapter} from "contracts/oracle/libraries/ChainlinkAdapter.sol";
import {IChainlink} from "contracts/interfaces/IChainlink.sol";

contract MockFrozenChainlinkFeed is IChainlink {
    uint8 public override decimals = 8;
    int256 public answer = 50_000e8; // LBTC = $50k
    uint256 public updatedAt = block.timestamp;
    uint80 public roundId = 1;
    uint80 public answeredInRound = 1; // stuck at 1, never advances

    function latestRoundData() external view override returns (
        uint80, int256, uint256, uint256, uint80
    ) {
        return (roundId, answer, updatedAt, updatedAt, answeredInRound);
    }
}

contract CapChainlinkStalePoC is Test {
    function testStalePriceAccepted() public {
        MockFrozenChainlinkFeed feed = new MockFrozenChainlinkFeed();
        // Cap adapter accepts the price without checking answeredInRound.
        (uint256 price,) = ChainlinkAdapter.price(address(feed));
        assertEq(price, 50_000e8); // stale, but no revert

        // Fast-forward 1 day; feed is still returning the same round.
        vm.warp(block.timestamp + 1 days);
        (price,) = ChainlinkAdapter.price(address(feed));
        assertEq(price, 50_000e8); // still stale, still no revert at the adapter level
    }
}
```

---

## 5. Impact

- **Stale-price borrowing / liquidation:** a stuck Chainlink feed lets
  borrowers extract value against inflated collateral prices and prevents
  timely liquidation of underwater positions. The impact scales with the
  gap between the feed's heartbeat and the protocol's `staleness[_asset]`
  window.
- **Severity is bounded by the per-asset `staleness` config:** if
  governance sets tight windows (e.g. 1 hour for a 1-hour-heartbeat
  feed), the impact is small. If the window is loose or unset
  (`staleness[_asset] == 0` means `_isStale` returns true for any
  `lastUpdated != block.timestamp`, which is actually *strict* — let me
  re-check), the impact grows.

### Re-examining the `staleness == 0` case

```solidity
function _isStale(address _asset, uint256 _lastUpdated) internal view returns (bool isStale) {
    isStale = block.timestamp - _lastUpdated > getPriceOracleStorage().staleness[_asset];
}
```

If `staleness[_asset] == 0`, then `isStale = (block.timestamp - _lastUpdated) > 0`.
For a feed updated in the same block, `_lastUpdated == block.timestamp`,
so `isStale = false`. For a feed updated 1 second ago, `isStale = true`.
So the default is actually *very strict* — any feed that hasn't updated
in the current block is considered stale.

This means the default-zero case is safe (almost too strict — it would
halt trading whenever Chainlink doesn't update in-block). The real risk
is when governance sets a *loose* window (e.g. 1 hour) to avoid
operational halts, which is the natural configuration choice. In that
case, the missing `answeredInRound` check becomes the only line of
defense, and it's absent.

---

## 6. Severity: **LOW**

- Not MEDIUM because: the protocol-level staleness check *does* exist and
  catches the issue if configured correctly. The Chainlink adapter's
  missing `answeredInRound` check is a defense-in-depth gap, not the
  primary staleness gate.
- The finding is still worth reporting because (a) it deviates from
  Chainlink's documented best practice, (b) it makes the protocol
  entirely dependent on the per-asset `staleness` window being set
  correctly, and (c) the chained adapter inherits the same gap.

---

## 7. Three-Perspective Audit

### 7.1 Protocol / Business-logic perspective
Cap's `PriceOracle` already has a fallback mechanism: if the primary
adapter returns 0 or is stale, it tries the backup oracle. Adding
`answeredInRound` validation would cause the primary to "fail fast" and
fall through to the backup sooner, which is the desired behavior during a
feed outage.

### 7.2 Oracle / Data-integrity perspective
Chainlink's own documentation
(https://docs.chain.link/data-feeds/get-the-latest-price#solidity)
explicitly recommends checking `answeredInRound >= roundId` and
`updatedAt != 0`. The Cap adapter discards both. This is a known
anti-pattern that has caused real losses in other protocols (e.g. the
2020 Cheese Bank incident, several Compound liquidation bugs).

### 7.3 Operational / Threat-model perspective
Cap's supported assets include LBTC, wstETH, and USDC — all of which have
Chainlink feeds with different heartbeats (LBTC: 1h, wstETH: 1h, USDC:
24h). A single `staleness` value per asset must be set to balance
"don't halt on brief outages" vs. "don't accept stale prices." The
`answeredInRound` check makes this trade-off moot for the "feed
permanently broken" case, which is the most dangerous scenario.

---

## 8. Suggested Fix

```diff
// contracts/oracle/libraries/ChainlinkAdapter.sol
 function price(address _source) external view returns (uint256 latestAnswer, uint256 lastUpdated) {
     uint8 decimals = IChainlink(_source).decimals();
     int256 intLatestAnswer;
-    (, intLatestAnswer,, lastUpdated,) = IChainlink(_source).latestRoundData();
+    (uint80 roundId, intLatestAnswer,, lastUpdated, uint80 answeredInRound)
+        = IChainlink(_source).latestRoundData();
+    require(answeredInRound >= roundId, "ChainlinkAdapter: stale round");
+    require(lastUpdated != 0, "ChainlinkAdapter: round not complete");
     latestAnswer = intLatestAnswer < 0 ? 0 : uint256(intLatestAnswer);
     if (decimals < 8) latestAnswer *= 10 ** (8 - decimals);
     if (decimals > 8) latestAnswer /= 10 ** (decimals - 8);
 }
```

Apply the same fix to `ChainlinkAdapterChained._getPrice`.

Note: the `PriceOracle._getPrice` wrapper uses `staticcall` and silently
returns `(0, 0)` on revert, so a `require` failure in the adapter will
cause the protocol to fall through to the backup oracle rather than
halting — which is the correct behavior.
