# getBurnRateSSV Silent uint64 Truncation

**Area:** DAO fee distribution — precision, rounding, view-function correctness
**Severity:** LOW
**Status:** CONFIRMED (code-level analysis)

---

## Description

`SSVViews.getBurnRateSSV` computes the SSV burn rate and **silently truncates**
the result to `uint64` without an overflow check:

```solidity
// SSVViews.sol, lines 344-364
function getBurnRateSSV(
    address clusterOwner,
    uint64[] calldata operatorIds,
    Cluster memory cluster
) external view override returns (uint256) {
    // ...
    PackedSSV aggregateFee;
    for (uint256 i; i < operatorsLength; ++i) {
        Operator memory operator = s.operators[operatorIds[i]];
        if (operator.owner != address(0)) {
            aggregateFee = aggregateFee.add(operator.fee);
        }
    }

    uint128 burnRate = PackedSSV.unwrap(aggregateFee.add(SSVStorageProtocol.load().networkFee))
                       * cluster.validatorCount;
    return PackedSSVLib.unpack(PackedSSV.wrap(uint64(burnRate)));  // ← TRUNCATION
}
```

`uint128 burnRate` is the product of a `uint64` (aggregate fee + network fee)
and a `uint32` (`validatorCount`).  This product can be up to
`~7.7 × 10²⁸`, which exceeds `type(uint64).max ≈ 1.8 × 10¹⁹`.  The explicit
cast `uint64(burnRate)` in Solidity 0.8 **does not revert** — it silently
truncates the upper bits.

### Contrast with getBurnRate (ETH)

The ETH equivalent `getBurnRate` (lines 309-339) computes the result entirely
in `uint256` and returns a `uint256` — no truncation.  The SSV version is
inconsistent.

---

## Contract / Function / Line

| Item | Location |
|---|---|
| **Contract** | `SSVViews` |
| **Function** | `getBurnRateSSV` |
| **File** | `contracts/modules/SSVViews.sol` |
| **Lines** | 362-363 (`uint64(burnRate)` truncation) |

---

## Attack Scenario

1. An SSV cluster has `validatorCount = 500 000` (a large staking pool).
2. The aggregate operator fee + network fee = `1e14` raw (1e21 wei = 1000
   SSV/block — high but DAO-configurable).
3. `burnRate = 1e14 * 500000 = 5e19`.  This exceeds `type(uint64).max ≈ 1.8e19`.
4. `uint64(5e19)` = `5e19 mod 2^64` = `5e19 - 2*2^64 ≈ 1.4e19` (truncated).
5. `PackedSSVLib.unpack(PackedSSV.wrap(1.4e19))` = `1.4e19 * 1e7 = 1.4e26`.
6. The function returns `1.4e26` instead of the correct `5e26`.  **72%
   under-reporting.**

### Downstream effects

- `isLiquidatableSSV` (view) uses a *separate* computation that does not
  truncate, so on-chain liquidation logic is unaffected.
- However, off-chain integrations (dashboards, liquidation bots, explorer UIs)
  that call `getBurnRateSSV` to estimate burn rates will receive **wrong
  values**, potentially causing liquidation bots to miss liquidatable clusters
  or to attempt liquidation of healthy clusters.

---

## PoC (Foundry)

```solidity
// SPDX-License-Identifier: GPL-3.0-or-later
pragma solidity 0.8.24;

import {Test} from "forge-std/Test.sol";
import {SSVViewsHarness} from "../../contracts/test/harness/SSVViewsHarness.sol";

contract POC_GetBurnRateSSVTruncation is Test {
    SSVViewsHarness internal views;

    function setUp() public {
        views = new SSVViewsHarness(address(0));
        // Set up operators and cluster with high fee × high validator count.
        // ...
    }

    function test_truncation() public view {
        // aggregateFee + networkFee = 1e14 raw
        // validatorCount = 500_000
        // burnRate (uint128) = 5e19  (> type(uint64).max)
        // uint64(burnRate)   = 5e19 % 2^64 ≈ 1.4e19  (truncated!)
        //
        // Correct return: 5e19 * 1e7 = 5e26
        // Actual return:   1.4e19 * 1e7 = 1.4e26  (72% under-reporting)
        uint256 result = views.getBurnRateSSV(/* ... */);
        // assert result < correct value
    }
}
```

---

## Impact

| Dimension | Assessment |
|---|---|
| **On-chain state** | None (view function only). |
| **Off-chain integrations** | Dashboards, bots, and explorers receive wrong burn-rate values, potentially causing missed or erroneous liquidation attempts. |
| **Likelihood** | Low — requires high fees and high validator counts. But DAO fee increases can make it reachable. |

---

## Three-Perspective Audit

### Prosecutor

The explicit `uint64()` cast is a known Solidity footgun — it truncates
silently.  The ETH counterpart `getBurnRate` uses `uint256` throughout.  The
inconsistency is a clear oversight.  View functions are part of the contract's
API; returning wrong values is a correctness defect.

### Defence

The values required to trigger truncation (1e14 raw fee × 500k validators)
are unrealistic for current mainnet parameters.  The function is a view
convenience; on-chain liquidation logic uses a different code path.

### Judge

View functions are part of the public API and are relied upon by off-chain
infrastructure.  Silent truncation is never acceptable — it should either
revert or use `uint256`.  The fix is a one-line change.  **Verdict: Confirmed
LOW** — no direct fund loss, but a correctness defect in a public view
function.

---

## Recommended Fix

```solidity
function getBurnRateSSV(
    address clusterOwner,
    uint64[] calldata operatorIds,
    Cluster memory cluster
) external view override returns (uint256) {
    // ...
    uint256 burnRate = uint256(PackedSSV.unwrap(aggregateFee.add(networkFee)))
                       * uint256(cluster.validatorCount);
    return PackedSSVLib.unpack(PackedSSV.wrap(burnRate));  // revert on overflow instead of truncating
    // Or simply: return burnRate * DEDUCTED_DIGITS;  // fully uint256
}
```
