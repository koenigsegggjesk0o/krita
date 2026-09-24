# PSM `removeCollateral` — Epoch/Period State Mapping Persistence After Removal

**Status:** Verified by code analysis (NOT submitted to Immunefi)
**Severity:** Low
**Source:** Systematic audit of PSM.sol focus areas (Area 7: removeCollateral persistence)
**Contract:** `/home/z/fkr-step1/defi-bounty/contracts/PSM.sol`
**Functions:** `removeCollateral` (line 528) + `addCollateral` (line 497) + `CollateralStateMap.remove` (deps, line 27)
**Related:** Same root-cause class as the `removeBenefactor` mapping-persistence bug (HIGH severity), but with lower impact because the persisted data is rate-limit counters, not authorization mappings.

---

## 1. Vulnerability Description

`removeCollateral` calls `collateralState.remove(collateral)`, which internally does `delete map._values[key].config`. The `CollateralConfig` struct contains only value types (addresses, uints, bools, uint8), so `delete` properly clears it. However, the parent `CollateralState` struct also contains two nested mappings:

```solidity
// IPSM.sol lines 144-148
struct CollateralState {
    CollateralConfig config;                                   // <- deleted by removeCollateral
    mapping(uint256 => EpochState) epochStateByDuration;       // <- NOT deleted (mapping)
    mapping(uint256 => PeriodState) periodStateByDuration;     // <- NOT deleted (mapping)
}
```

Solidity's `delete` operator on a struct resets value-type fields but **cannot clear mappings** (mappings are not iterable). `CollateralStateMap.remove` only deletes `_values[key].config` — it does NOT touch the `epochStateByDuration` or `periodStateByDuration` mappings. These mappings silently persist.

When the same collateral address is re-added via `addCollateral`, the `CollateralStateMap.add` function only sets `_values[key].config = config`. It does NOT clear the old epoch/period state mappings. As a result, the stale rate-limit counters from before the removal are still present and will be used for limit enforcement if the re-add occurs within the same epoch/period number.

This is the **exact same root cause** as the `removeBenefactor` bug (HIGH severity): `delete` on a struct with nested mappings does not clear the mappings. The difference is that the persisted data here is rate-limit counters (not authorization mappings), so the impact is limited to a temporary DoS rather than direct fund loss.

---

## 2. Contract + Function + Line Number

| Component | File | Line(s) |
|-----------|------|---------|
| `removeCollateral` | `PSM.sol` | 528-539 |
| `addCollateral` | `PSM.sol` | 497-519 |
| `CollateralStateMap.remove` | `deps/CollateralStateMap.sol` | 27-30 |
| `CollateralStateMap.add` | `deps/CollateralStateMap.sol` | 22-25 |
| `CollateralState` struct | `deps/IPSM.sol` | 144-148 |
| `_handleEpochPeriodOperations` | `PSM.sol` | 1662-1836 |
| `_maybeRollEpoch` | `PSM.sol` | 1844-1850 |

---

## 3. Attack Scenario (Step-by-Step)

### Setup
1. Admin adds collateral `C` via `addCollateral(C, config)` where `config.maxSwapForAssetPerEpoch = 1000`.
2. Epoch duration = 1 hour. `block.timestamp = 3600`. Current epoch = 1.
3. Benefactors swap through collateral `C`. After several swaps, `epochStateByDuration[3600].swappedForAssetInEpoch = 800` (at epoch 1).
4. The collateral's rate-limit capacity for the rest of epoch 1 is 200 (1000 - 800).

### Incident (e.g., oracle depeg triggers temporary removal)
5. Admin calls `removeCollateral(C)` to temporarily disable the collateral (e.g., during an oracle incident).
   - `collateralState.remove(C)` executes:
     - `map._keys.remove(C)` — C is removed from the enumerable set.
     - `delete map._values[C].config` — `CollateralConfig` is cleared (all value types reset).
   - **`epochStateByDuration[3600]` is NOT cleared** — it still has `{epoch: 1, swappedForAssetInEpoch: 800, swappedForCollateralInEpoch: ...}`.
   - **`periodStateByDuration[86400]` is NOT cleared** — same issue for period state.
6. `emit CollateralRemoved(C)`. The event gives no indication that rate-limit state persists.

### Re-add (same epoch — within 1 hour of step 4)
7. Admin calls `addCollateral(C, newConfig)` where `newConfig.maxSwapForAssetPerEpoch = 500` (reduced limit, perhaps to be cautious post-incident).
   - `collateralState.add(C, newConfig)` executes:
     - `map._keys.add(C)` — C is added back to the enumerable set.
     - `map._values[C].config = newConfig` — config is set.
   - **The old `epochStateByDuration[3600]` is still `{epoch: 1, swappedForAssetInEpoch: 800}`.**

### DoS manifests
8. A benefactor attempts a swap through collateral `C` with `amountOut = 100`.
9. `_handleEpochPeriodOperations` loads `_collateralState.epochStateByDuration[3600]` → `{epoch: 1, swappedForAssetInEpoch: 800}`.
10. `_maybeRollEpoch` checks `epochState.epoch (1) != currentEpoch (1)` → **no mismatch** → counters NOT reset.
11. `_validateCollateralEpochLimits` checks `currentTotal (800) + amount (100) > max (500)` → `900 > 500` → **reverts** with `CollateralMaxSwapForAssetPerEpochExceeded`.
12. **ALL swaps through collateral C are blocked for the remainder of epoch 1**, even though the admin intended to re-enable the collateral with a fresh limit of 500.

### Why this is a bug
- The admin's intent in removing and re-adding the collateral was to start fresh (or at least to re-enable it with a new config). The persisted rate-limit counters silently carry over, defeating the admin's intent.
- If the admin reduced the limit below the old counter, the collateral is **permanently blocked** until the next epoch rollover (when `_maybeRollEpoch` finally detects a mismatch and resets).
- There is **no event, no warning, no documentation** indicating that rate-limit state persists across remove + re-add.
- The `CollateralRemoved` event implies a clean removal; the `CollateralAdded` event implies a fresh addition. Neither reflects the actual state.

### Narrower variant (cross-epoch)
If the re-add happens in a **different epoch** (e.g., epoch 2), `_maybeRollEpoch` detects `1 != 2` and resets the counters. So the bug only manifests when the re-add occurs **within the same epoch** as the removal. With `MIN_EPOCH_DURATION = 10 seconds`, this window can be as short as 10 seconds or as long as 24 hours (`MAX_EPOCH_DURATION`).

---

## 4. PoC Code (Foundry)

```solidity
// SPDX-License-Identifier: GPL-3.0
pragma solidity 0.8.30;

import {Test} from "forge-std/Test.sol";
import {PSM} from "../src/PSM.sol";
import {IPSM} from "../src/IPSM.sol";
import {MockERC20} from "./mocks/MockERC20.sol";
import {MockOracleFeed} from "./mocks/MockOracleFeed.sol";

/// @title PoC_removeCollateral
/// @notice Verifies that removeCollateral does NOT clear epochStateByDuration / periodStateByDuration
///         mappings. After remove + re-add within the same epoch, stale rate-limit counters persist
///         and can block all swaps (DoS) if the admin reduced the limit below the old counter.
contract PoC_removeCollateral is Test {
    address admin = makeAddr("admin");
    address collateralManager = makeAddr("collateralManager");
    address benefactorManager = makeAddr("benefactorManager");
    address epochManager = makeAddr("epochManager");

    address benefactorA = makeAddr("benefactorA");
    address assetSendCustodian = makeAddr("assetSendCustodian");
    address assetReceiveCustodian = makeAddr("assetReceiveCustodian");
    address collateralSendCustodian = makeAddr("collateralSendCustodian");
    address collateralReceiveCustodian = makeAddr("collateralReceiveCustodian");

    PSM internal psm;
    MockERC20 internal asset;
    MockERC20 internal collateral;
    MockOracleFeed internal oracle;

    uint128 constant PEG = 1e18;
    uint128 constant LARGE = type(uint128).max;
    uint128 constant SWAP_AMOUNT = 100e18;

    function setUp() public {
        asset = new MockERC20("Asset", "AST", 18);
        collateral = new MockERC20("Collateral", "COL", 18);
        oracle = new MockOracleFeed(PEG, block.timestamp);

        asset.mint(assetSendCustodian, 1_000_000e18);
        collateral.mint(collateralSendCustodian, 1_000_000e18);
        collateral.mint(benefactorA, 1_000_000e18);

        IPSM.GlobalConfig memory gc = IPSM.GlobalConfig({
            maxSwapForAssetPerEpoch: LARGE,
            maxSwapForCollateralPerEpoch: LARGE,
            defaultBenefactorMaxSwapForAssetPerEpoch: LARGE,
            defaultBenefactorMaxSwapForCollateralPerEpoch: LARGE,
            epochDuration: 1 hours,
            pegPrice: PEG,
            maxSwapForAssetPerPeriod: LARGE,
            maxSwapForCollateralPerPeriod: LARGE,
            defaultBenefactorMaxSwapForAssetPerPeriod: LARGE,
            defaultBenefactorMaxSwapForCollateralPerPeriod: LARGE,
            periodDuration: 1 days
        });

        address[] memory epochManagers = _arr(epochManager);
        address[] memory collateralManagers = _arr(collateralManager);
        address[] memory benefactorManagers = _arr(benefactorManager);
        address[] memory empty = new address[](0);

        psm = new PSM(
            address(asset), assetSendCustodian, assetReceiveCustodian, gc,
            admin, epochManagers, empty, collateralManagers, empty,
            benefactorManagers, empty, empty
        );

        // Add collateral with per-epoch limit of 1000 asset
        vm.startPrank(collateralManager);
        IPSM.CollateralConfig memory cc = _defaultCollateralConfig(1000e18);
        psm.addCollateral(address(collateral), cc);
        vm.stopPrank();

        // Approvals
        vm.prank(assetSendCustodian);
        asset.approve(address(psm), type(uint256).max);
        vm.prank(collateralSendCustodian);
        collateral.approve(address(psm), type(uint256).max);
        vm.prank(benefactorA);
        collateral.approve(address(psm), type(uint256).max);

        // Add benefactor
        vm.prank(benefactorManager);
        psm.addBenefactor(benefactorA);
    }

    function _defaultCollateralConfig(uint128 maxPerEpoch) internal view returns (IPSM.CollateralConfig memory) {
        return IPSM.CollateralConfig({
            sendCustodianAddress: collateralSendCustodian,
            receiveCustodianAddress: collateralReceiveCustodian,
            oracleFeed: address(oracle),
            maxSwapForAssetPerEpoch: maxPerEpoch,
            maxSwapForCollateralPerEpoch: LARGE,
            minOraclePrice: 0.9e18,
            maxOraclePrice: 1.1e18,
            maxOracleAge: 1 hours,
            isActive: true,
            decimals: 18,
            defaultSwapForAssetFee: 0,
            defaultSwapForCollateralFee: 0,
            maxSwapForAssetPerPeriod: LARGE,
            maxSwapForCollateralPerPeriod: LARGE
        });
    }

    function _arr(address a) internal pure returns (address[] memory r) {
        r = new address[](1);
        r[0] = a;
    }

    function _buildOrder(uint128 nonce, uint128 amountIn) internal view returns (IPSM.Order memory) {
        return IPSM.Order({
            isSwapForAsset: true,
            expiry: uint120(block.timestamp + 1 hours),
            nonce: nonce,
            chainId: block.chainid,
            benefactor: benefactorA,
            beneficiary: benefactorA,
            collateral: address(collateral),
            amountIn: amountIn,
            minAmountOut: amountIn
        });
    }

    /// @dev Test: stale epoch counters persist across removeCollateral + addCollateral
    ///      within the same epoch, causing DoS when the admin reduces the limit.
    function test_RemoveCollateral_StaleEpochCountersPersist() public {
        // ---- Step 1: Swap 800 asset worth of collateral (limit is 1000) ----
        oracle.setPrice(PEG, block.timestamp);
        vm.prank(benefactorA);
        psm.swap(_buildOrder(1, 800e18));

        (uint128 swapped, ) = psm.getCollateralEpochTotals(address(collateral));
        assertEq(swapped, 800e18, "epoch counter should be 800 after first swap");

        // ---- Step 2: Admin removes collateral ----
        vm.prank(collateralManager);
        psm.removeCollateral(address(collateral));

        // After removal, getCollateralEpochTotals returns 0 (collateral not in set,
        // but the underlying storage mapping persists)
        (swapped, ) = psm.getCollateralEpochTotals(address(collateral));
        assertEq(swapped, 0, "view returns 0 because collateral is removed from enumerable set");

        // ---- Step 3: Admin re-adds collateral with REDUCED limit (500) ----
        // NOTE: we're still in the same epoch (no warp)
        vm.startPrank(collateralManager);
        IPSM.CollateralConfig memory cc2 = _defaultCollateralConfig(500e18);
        psm.addCollateral(address(collateral), cc2);
        vm.stopPrank();

        // ---- Step 4: BUG — the old counter (800) is still present ----
        // The view function now reads the persisted (stale) counter because the
        // collateral is back in the enumerable set.
        (swapped, ) = psm.getCollateralEpochTotals(address(collateral));
        assertEq(swapped, 800e18, "BUG: stale epoch counter (800) persists after remove + re-add");

        // ---- Step 5: DoS — any swap now reverts because 800 + amount > 500 ----
        vm.prank(benefactorA);
        vm.expectRevert(
            abi.encodeWithSelector(
                IPSM.CollateralMaxSwapForAssetPerEpochExceeded.selector,
                address(collateral),
                100e18,       // requested
                800e18,       // currentUsage (STALE!)
                500e18,       // maxAllowed (new reduced limit)
                block.timestamp / 1 hours, // currentEpoch
                1 hours       // epochDuration
            )
        );
        psm.swap(_buildOrder(2, 100e18));

        // ---- Step 6: After epoch rollover, _maybeRollEpoch resets — swap succeeds ----
        vm.warp(block.timestamp + 1 hours + 1);
        oracle.setPrice(PEG, block.timestamp);

        vm.prank(benefactorA);
        psm.swap(_buildOrder(3, 100e18)); // succeeds — counters were reset by _maybeRollEpoch
    }

    /// @dev Test: if re-add happens in a different epoch, _maybeRollEpoch resets. No DoS.
    function test_RemoveCollateral_DifferentEpochResetsCleanly() public {
        oracle.setPrice(PEG, block.timestamp);
        vm.prank(benefactorA);
        psm.swap(_buildOrder(1, 800e18));

        vm.prank(collateralManager);
        psm.removeCollateral(address(collateral));

        // Warp to next epoch
        vm.warp(block.timestamp + 1 hours + 1);

        vm.startPrank(collateralManager);
        IPSM.CollateralConfig memory cc2 = _defaultCollateralConfig(500e18);
        psm.addCollateral(address(collateral), cc2);
        vm.stopPrank();

        (uint128 swapped, ) = psm.getCollateralEpochTotals(address(collateral));
        assertEq(swapped, 0, "different epoch → _maybeRollEpoch resets counters");

        // Swap succeeds — fresh counters
        oracle.setPrice(PEG, block.timestamp);
        vm.prank(benefactorA);
        psm.swap(_buildOrder(2, 100e18));
    }
}
```

### Expected test results

```
[PASS] test_RemoveCollateral_StaleEpochCountersPersist()
  → assertEq(swapped, 800e18) PASSES (bug confirmed: stale counter persists)
  → vm.expectRevert(CollateralMaxSwapForAssetPerEpochExceeded) PASSES (DoS confirmed)
  → post-warp swap succeeds (epoch rollover resets)

[PASS] test_RemoveCollateral_DifferentEpochResetsCleanly()
  → assertEq(swapped, 0) PASSES (no bug in different-epoch case)
  → swap succeeds
```

---

## 5. Impact Assessment

| Criterion | Assessment |
|-----------|------------|
| Fund loss? | **NO** — persisted counters can only REDUCE capacity (enforce limits more strictly), never increase it |
| DoS? | **YES** — if admin re-adds with a reduced limit below the old counter, all swaps blocked until next epoch rollover |
| Privilege escalation? | **NO** |
| Preconditions | Admin must call `removeCollateral` then `addCollateral` within the same epoch, AND the new limit must be below the old counter |
| Detection difficulty | Medium — no event indicates stale state; admin sees `CollateralRemoved` + `CollateralAdded` and assumes clean state |
| Scope of impact | Per-collateral, temporary (until epoch rollover) |
| Fix complexity | Low — clear the mappings explicitly (requires enumerable duration keys) or use a version/nonce scheme |

### Why this is LOWER severity than `removeBenefactor`

| Aspect | `removeBenefactor` (HIGH) | `removeCollateral` (LOW) |
|--------|--------------------------|--------------------------|
| Persisted data | Authorization mappings (delegatedSigners, approvedBeneficiaries) | Rate-limit counters (swappedForAssetInEpoch, etc.) |
| Impact of persistence | Attacker regains swap authority → **direct fund drain** | Stale counters → **temporary DoS only** |
| Can attacker benefit? | **YES** — regained authority allows swapping | **NO** — counters can only reduce capacity, not increase |
| Exploitation window | Days/weeks (until re-add) | Single epoch (until `_maybeRollEpoch` resets) |
| Requires admin misconfiguration? | No (any remove + re-add triggers it) | Yes (admin must reduce limit below old counter) |

---

## 6. Severity: Low

**Rationale:**
- No fund loss — the persisted data is rate-limit counters, which can only block swaps (DoS), not enable unauthorized ones.
- The DoS is temporary — it lasts only until the next epoch/period rollover, at which point `_maybeRollEpoch`/`_maybeRollPeriod` resets the counters.
- The DoS requires admin misconfiguration (reducing the limit below the old counter during re-add within the same epoch).
- A simple operational mitigation exists: wait for epoch rollover before re-adding, or re-add with a limit >= the old counter.

**Why not Medium:**
- The bug is the same root cause as `removeBenefactor` (delete on struct with nested mappings), which might suggest a higher severity by pattern. However, the IMPACT is fundamentally different: `removeBenefactor` enables fund theft, while `removeCollateral` only causes a temporary DoS. Severity is based on impact, not pattern.

---

## 7. Three-Perspective Audit

### Prosecutor (why the bug is real and exploitable)

The bug is real and confirmed by code analysis:

1. **Solidity semantics guarantee it.** `delete` on a struct with nested mappings does NOT clear the mappings. This is documented Solidity behavior. The `CollateralState` struct has two nested mappings (`epochStateByDuration`, `periodStateByDuration`), and `CollateralStateMap.remove` only deletes `config`, not these mappings.

2. **The code path is reachable.** `removeCollateral` (line 528) and `addCollateral` (line 497) are standard admin functions. There is no barrier to calling them in sequence within the same epoch.

3. **The DoS is real.** If the admin removes a collateral (e.g., during an oracle incident) and re-adds it with a reduced limit within the same epoch, the stale counter blocks all swaps. The PoC demonstrates this with `vm.expectRevert(CollateralMaxSwapForAssetPerEpochExceeded)`.

4. **The bug is silent.** No event, no documentation, no warning indicates that rate-limit state persists. The admin has no way to detect the stale state without manually inspecting storage.

5. **It's the same root cause as a known HIGH bug.** The `removeBenefactor` bug was confirmed HIGH severity with the same `delete`-doesn't-clear-mappings root cause. The pattern repeating in `removeCollateral` indicates a systemic issue.

### Defense (why the bug is invalid or out of scope)

1. **No fund loss.** The persisted data is rate-limit counters, not authorization mappings. The counters can only REDUCE capacity (block swaps), never INCREASE it. An attacker cannot use this to swap more than the limit allows. The worst case is a temporary DoS.

2. **The DoS is self-inflicted.** It requires the admin to reduce the limit below the old counter during re-add. If the admin keeps the same or higher limit, there's no DoS. The admin controls both the removal and the re-add, so they can avoid the issue by not reducing the limit.

3. **The DoS is temporary.** It lasts only until the next epoch rollover (at most `MAX_EPOCH_DURATION = 24 hours`). After that, `_maybeRollEpoch` resets the counters. A 24-hour DoS on a single collateral is low impact.

4. **Operational mitigation exists.** The admin can simply wait for epoch rollover before re-adding, or re-add with a limit >= the old counter. This is a standard operational practice.

5. **The bug requires same-epoch re-add.** If the re-add happens in a different epoch (which is the common case for incident recovery — you wait for the situation to stabilize), `_maybeRollEpoch` resets the counters and there's no bug.

6. **Immunefi scope.** Immunefi bug bounties typically require fund loss or severe DoS. A temporary, self-inflicted, per-collateral DoS with a simple mitigation is below the threshold for a bounty.

### Judge (final verdict)

**Verdict: REAL BUG, but LOW severity. DO NOT SUBMIT to Immunefi.**

**Reasoning:**

The bug is **technically real**. The Solidity semantics are clear: `delete` on a struct with nested mappings does not clear the mappings. `CollateralStateMap.remove` only deletes `config`, not the `epochStateByDuration` and `periodStateByDuration` mappings. After remove + re-add within the same epoch, stale counters persist. This is confirmed by code analysis and would be confirmed by a Foundry PoC.

However, the **impact is LOW**:
- No fund loss. The persisted data is rate-limit counters, which can only block swaps (DoS), not enable unauthorized ones.
- The DoS is temporary (until epoch rollover, max 24 hours).
- The DoS requires admin misconfiguration (reducing the limit below the old counter).
- A simple operational mitigation exists (wait for epoch rollover or keep the limit >= old counter).

Compared to the `removeBenefactor` bug (HIGH severity, direct fund drain), this bug has fundamentally lower impact. The root cause is the same (`delete` doesn't clear mappings), but the IMPACT is what determines severity, not the root cause.

**Recommendation:** Do not submit to Immunefi. The bug is real but below the severity threshold for a bounty. Document it as a known design issue and recommend a fix (clear mappings explicitly using enumerable duration keys, or use a version/nonce scheme to invalidate old state).

**If submitted anyway:** Expect a "Low" or "Informational" rating from triage. The triage team would likely note the lack of fund loss and the temporary, self-inflicted nature of the DoS.

---

## Suggested Fix

**Option A: Version/nonce scheme (recommended)**

Add a `uint256 configVersion` to `CollateralState`. Increment it on `removeCollateral`. Use `epochStateByDuration[duration][configVersion]` instead of `epochStateByDuration[duration]`. Old state becomes unreachable.

**Option B: Explicit clearing (requires enumerable keys)**

Track all used duration keys in an `EnumerableSet`. On `removeCollateral`, iterate the set and delete each `epochStateByDuration[duration]` and `periodStateByDuration[duration]`. Gas-expensive but thorough.

**Option C: Document the behavior**

If the current behavior is intended (rate-limit state persists across remove + re-add within the same epoch), document it clearly in the `removeCollateral` and `addCollateral` docstrings. Add an event that indicates stale state is present.
