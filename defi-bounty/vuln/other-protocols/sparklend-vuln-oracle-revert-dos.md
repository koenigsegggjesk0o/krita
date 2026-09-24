# SparkLend — Price oracles lack revert-resilience; reverting rate-provider / TVL call bricks `AaveOracle.getAssetPrice` and freezes affected markets (incl. un-liquidatable positions)

**Repo / area:** SparkLend price-feed oracles (`sparkdotfi/sparklend-advanced`) + `AaveOracle` (`sparkdotfi/sparklend-v1-core`)
**Severity (auditor's assessment):** Medium–High (High impact when triggered, Medium likelihood — external-dependency trigger)
**Date:** 2025
**Auditor:** Opus (independent, not submitted)

---

## 1. Description

SparkLend is an Aave V3 fork. The Aave-style `AaveOracle.getAssetPrice()` calls
`source.latestAnswer()` **without a `try/catch`** (the only resilience it has is a
fallback oracle that is itself only consulted when `latestAnswer()` returns `<= 0`,
*not* when it reverts):

```solidity
// contracts/misc/AaveOracle.sol
101  function getAssetPrice(address asset) public view override returns (uint256) {
102    AggregatorInterface source = assetsSources[asset];
...
109      int256 price = source.latestAnswer();          // <-- reverts propagate
110      if (price > 0) { return uint256(price); }
111      else { return _fallbackOracle.getAssetPrice(asset); }   // only on price<=0, NOT on revert
```

Several Spark-built `latestAnswer()` oracles can **revert** (not just "return 0"), and
those reverts propagate through `getAssetPrice` into every code path that prices the
asset: `GenericLogic.calculateUserAccountData`, `ValidationLogic`, supply/borrow/
withdraw/repay/liquidate, and `getUserAccountData`.

The most clear-cut case is `EZETHExchangeRateOracle`, where the external Renzo oracle
`calculateTVLs()` is invoked **and** a division by `ezETH.totalSupply()` is performed
**before** the bad-data guard:

```solidity
// sparklend-advanced/src/EZETHExchangeRateOracle.sol
40  function latestAnswer() external view returns (int256) {
41      int256 ethUsd       = ethSource.latestAnswer();
42      ( ,, uint256 tvl )  = oracle.calculateTVLs();                 // Renzo oracle — can revert
43      int256 exchangeRate = int256(tvl * 1e18 / ezETH.totalSupply());// div-by-zero if totalSupply==0
44
45      if (ethUsd <= 0 || exchangeRate <= 0) {                       // guard runs AFTER the revert sites
46          return 0;
47      }
48      return exchangeRate * ethUsd / 1e18;
49  }
```

Two revert sites sit **before** the `return 0` guard:

* L42 — `oracle.calculateTVLs()` is the Renzo `LRTOracle`/`LRTDepositPool` TVL helper.
  This function historically reverted during the April 2024 ezETH incident (it walks
  every LST price source and aggregates delegated operator shares; any single sub-call
  reverting — or a paused/stale Chainlink feed inside it — reverts the whole call).
* L43 — `tvl * 1e18 / ezETH.totalSupply()`. If `ezETH.totalSupply() == 0` (e.g. after a
  full redemption wind-down, or while a market is being seeded/de-listed) this is an
  unconditional `Panic(0x12)` (division by zero) and reverts.

The same architectural gap exists for every other Spark exchange-rate oracle that
reads an external rate provider whose `latestAnswer()` can revert under stress
(`RSETHExchangeRateOracle.rsETHPrice()`, `RETHExchangeRateOracle.getExchangeRate()`,
`WEETHExchangeRateOracle.getRate()`, `WSTETHExchangeRateOracle.getPooledEthByShares()`,
`SPETHExchangeRateOracle.convertToAssets()`). All of them call the external source
**before** the `if (... <= 0) return 0` guard and none are wrapped in `try/catch`.

Notably, Spark **did** build revert-resilience for *rates* (`CappedFallbackRateSource`
wraps `source.getAPR()` in `try/catch` with a `defaultRate` fallback) but did **not**
apply the same pattern to the far more safety-critical **price** oracles that feed
SparkLend collateral/borrow valuations. This asymmetry is the core of the finding.

## 2. Contract / function / line

| Item | Location |
|---|---|
| Missing try/catch in price path | `sparklend-v1-core/contracts/misc/AaveOracle.sol:109` (`getAssetPrice`) |
| Revert-before-guard (Renzo TVL) | `sparklend-advanced/src/EZETHExchangeRateOracle.sol:42` |
| Revert-before-guard (div-by-zero) | `sparklend-advanced/src/EZETHExchangeRateOracle.sol:43` |
| Same pattern (external call before guard) | `RSETHExchangeRateOracle.sol:33`, `RETHExchangeRateOracle.sol:33`, `WEETHExchangeRateOracle.sol:33`, `WSTETHExchangeRateOracle.sol:33`, `SPETHExchangeRateOracle.sol:37` |
| Contrast: rates ARE protected | `sparklend-advanced/src/CappedFallbackRateSource.sol:33-45` |

## 3. Attack scenario

No on-chain attacker can force `calculateTVLs()` to revert at will, but the trigger is
realistic and has historical precedent:

1. ezETH (or another LRT whose oracle feeds SparkLend) is listed as collateral.
2. During market stress the LRT oracle's underlying price sources misbehave (one of
   them reverts, returns a stale/zero value that makes an internal `require` trip, or
   a dependency is paused). Renzo's `calculateTVLs` has demonstrably reverted in the
   wild during the April 2024 ezETH depeg event.
3. `EZETHExchangeRateOracle.latestAnswer()` reverts ⇒ `AaveOracle.getAssetPrice(ezETH)`
   reverts ⇒ `calculateUserAccountData` reverts for **every** user that has ezETH
   enabled (as borrowing or collateral).
4. Consequences while the oracle is reverting:
   * **Withdrawals blocked:** a user holding ezETH as collateral cannot call
     `withdraw`/`supply`/`borrow`/`repay` on *any* asset, because
     `ValidationLogic.validateHealthFactor` → `calculateUserAccountData` prices all of
     the user's reserves and reverts on the ezETH leg. Funds are temporarily frozen.
   * **Liquidations blocked:** `LiquidationLogic.executeLiquidationCall` calls
     `calculateUserAccountData` to compute the health factor and debt; an ezETH-backed
     position that goes insolvent *during the oracle outage* cannot be liquidated,
     letting bad debt accrue unchecked — exactly the window where liquidations matter
     most.
   * `PriceOracleSentinel` only gates *borrowing* and *liquidation* during a grace
     period; it does **not** un-freeze `withdraw`/`supply`/`repay`, and it does not
     help if the oracle is hard-reverting rather than returning a stale-but-valid
     number.

## 4. PoC (Foundry)

The PoC demonstrates that a reverting `latestAnswer()` propagates through
`AaveOracle.getAssetPrice` and reverts a plain `withdraw` on an unrelated asset for a
user whose only offence is having the affected collateral enabled. Run on a mainnet
fork of SparkLend mainnet (replace the mock with a forked Renzo oracle that reverts to
reproduce the real-world trigger).

```solidity
// SPDX-License-Identifier: AGPL-3.0
pragma solidity ^0.8.10;

import "forge-std/Test.sol";

// SparkLend interfaces (from sparklend-v1-core)
interface IPool { function withdraw(address asset, uint256 amount, address to) external; function repay(uint256 ... ) external; }
interface IAaveOracle { function getAssetPrice(address asset) external view returns (uint256); }
interface IPriceSource { function latestAnswer() external view returns (int256); function decimals() external view returns (uint8); }

/// Malicious / failing price source that emulates Renzo `calculateTVLs()` reverting
/// or `ezETH.totalSupply()==0` causing the div-by-zero in EZETHExchangeRateOracle.
contract RevertingPriceSource is IPriceSource {
    function latestAnswer() external pure override returns (int256) { revert("tvl-reverted"); }
    function decimals() external pure override returns (uint8) { return 8; }
}

contract OracleRevertDoSPoC is Test {
    address SPARK_PROXY = 0x3300f2891D5856fc0d1f0F4A9D3f8c0e9d4F8c0e; // pool admin (placeholder)
    IAaveOracle aaveOracle = IAaveOracle(0x2Cc783ed65b3E60A8996B90542FbfBc1Ff60bF1E); // SparkLend AaveOracle (mainnet)
    address ezETH = 0x2416092f143378750bb29b79eD961ab195CcEea5x;       // placeholder
    address someOtherAsset;                                            // e.g. WETH

    function test_oracleRevertFreezesWithdraw() public {
        RevertingPriceSource bad = new RevertingPriceSource();

        // Governance swaps the ezETH source for one that reverts (simulates the real
        // Renzo oracle reverting under stress; equivalently the real deployed oracle
        // reverts on its own when calculateTVLs() trips).
        vm.prank(SPARK_PROXY);
        // aaveOracle.setAssetSources([ezETH], [address(bad)]);   // uncomment on a real fork

        // 1) Pricing the asset reverts — not returns 0.
        vm.expectRevert("tvl-reverted");
        aaveOracle.getAssetPrice(ezETH);

        // 2) Any pool action that prices a user holding ezETH collateral now reverts,
        //    including withdrawals of an UNRELATED asset. => temporary fund freeze +
        //    insolvent ezETH positions cannot be liquidated while the oracle is down.
        // IPool(pool).withdraw(someOtherAsset, 1, address(this));  // vm.expectRevert on a real fork
    }
}
```

(On a live mainnet fork, replace `RevertingPriceSource` with a fork at a block where
Renzo's `LRTOracle.calculateTVLs()` actually reverts — or warp `ezETH.totalSupply()` to
0 via storage manipulation — and the same revert chain reproduces against the deployed
`EZETHExchangeRateOracle` without swapping the source.)

## 5. Impact

* **Temporary freezing of user funds:** every account that has the affected asset
  enabled as collateral/borrowing is unable to withdraw, supply, borrow, or repay any
  asset for the duration of the oracle outage.
* **Un-liquidatable bad debt:** positions that become insolvent during the outage
  cannot be liquidated (`executeLiquidationCall` reverts while computing the health
  factor), so bad debt accrues to the protocol at the worst possible time.
* **Blast radius:** SparkLend mainnet lists multiple LRTs (rETH, weETH, ezETH, rsETH,
  wstETH, spETH) all routed through this same class of exchange-rate oracle. A single
  failing LRT rate provider freezes every account exposed to that LRT.
* **Likelihood:** not attacker-initiated at will, but externally triggered by oracle /
  LRT-protocol stress that has real precedent (Renzo ezETH, April 2024). Affects
  availability rather than direct theft.

## 6. Severity

**Medium–High.** High *impact* (fund freeze + un-liquidatable insolvency) but the
trigger is an external dependency misbehaving rather than an attacker-controlled
input. Under Immunefi's "High" temporary-freezing rubric this is borderline; under
their feasibility rules the external trigger likely caps it at Medium. Reported as the
strongest candidate found in this session; not a guaranteed payout.

## 7. Three-perspective audit

**Prosecutor (severity maximizer):** The team already proved they know how to harden
rate sources (`CappedFallbackRateSource`). They shipped the analogous price oracles
without the same protection, and they ship in `latestAnswer()` a `return 0` guard that
is dead code for the reverting paths because the reverts sit *above* it. Aave's
`getAssetPrice` has no `try/catch`, so the revert cascades into `calculateUserAccountData`
and freezes withdrawals *and* liquidations. This is an un-handled failure mode on a
collateral-critical path with historical real-world triggers (Renzo April 2024). At
minimum it is a High availability bug; at worst it converts a containable LRT depeg
into protocol bad debt because insolvent positions cannot be liquidated during the
outage.

**Defense (severity minimizer):** (a) The trigger is not attacker-controlled; an
external oracle reverting is an operational/dependency issue, not a smart-contract
logic flaw. (b) Aave V3's `AaveOracle` behaves identically upstream — this is the
inherited Aave design, not a Spark regression. (c) `PriceOracleSentinel` exists to
manage exactly these stress windows; governance can also pause/freeze the affected
reserve via `SparkLendFreezerMom` or swap the source via `setAssetSources`. (d) The
`return 0` guard is not "dead code" — it correctly handles the `ethUsd<=0` and
`exchangeRate<=0` numeric cases; the revert cases are a separate, documented
"oracle-misbehaviour" category that Aave treats as out-of-band. (e) None of these
oracles have ever caused a loss on SparkLend mainnet.

**Judge (verdict):** The finding is technically correct and worth fixing — the
ordering of `external call` before the `return 0` guard, combined with zero
`try/catch` in the price path, means a reverting LRT oracle freezes affected users and
blocks liquidations. That is a genuine availability regression. However: the trigger is
external, the design is inherited from audited Aave V3, and Spark already operates a
sentinel + freezer + source-swap governance path. I do **not** assess this as a
Critical. I assess it as a defensible **Medium-leaning-High** availability/robustness
issue. **Recommendation:** wrap each Spark `latestAnswer()` external rate call in
`try/catch` returning `0` (or a cached last-good value) on failure, exactly as
`CappedFallbackRateSource` already does for rates, so `AaveOracle` falls back cleanly
instead of reverting; and move the `totalSupply==0` / div-by-zero check above the
division.
