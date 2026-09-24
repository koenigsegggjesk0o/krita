# PSM `setEpochDuration` / `setPeriodDuration` — Stale Rate-Limit State Restoration on Duration Revert

**Status:** Verified by code analysis (NOT submitted to Immunefi)
**Severity:** Low
**Source:** Systematic audit of PSM.sol focus areas (Area 1: _handleEpochPeriodOperations / duration change)
**Contract:** `/home/z/fkr-step1/defi-bounty/contracts/PSM.sol`
**Functions:** `setEpochDuration` (line 997) + `setPeriodDuration` (line 1082) + `_handleEpochPeriodOperations` (line 1662) + `_maybeRollEpoch` (line 1844)

---

## 1. Vulnerability Description

The PSM uses a "state-per-duration" pattern: epoch and period rate-limit counters are stored in mappings keyed by duration (`epochStateByDuration[duration]`, `periodStateByDuration[duration]`). When the admin changes the duration via `setEpochDuration` or `setPeriodDuration`, the contract simply starts reading from the new duration's slot. The old duration's slot is **never cleared**.

The code comment at line 994 states:
> "Automatically isolates epochs when duration changes; current usage effectively resets"

This is **partially true**: when switching to a *new* duration that was never used before, the default state (`epoch=0`) triggers `_maybeRollEpoch` to reset (since `0 != currentEpoch`). So the counters do "effectively reset."

However, the comment is **misleading** in the reverse direction: if the admin changes the duration **back to a previously used value**, and the current block.timestamp happens to fall in the **same epoch/period number** as the last time that duration was used, `_maybeRollEpoch` detects **no mismatch** (`epochState.epoch == currentEpoch`) and does **NOT** reset the counters. The **stale counters from the previous usage** silently apply.

This creates two issues:
1. **Stale counter restoration**: The admin might expect a fresh start when changing the duration back, but old counters apply.
2. **Silent rate-limit confusion**: The admin has no way to know (without inspecting storage) whether the counters are fresh or stale.

### Why the same-epoch collision is reachable

The epoch number is `block.timestamp / epochDuration`. Two different timestamps can map to the same epoch number if the duration is the same. For example:
- Duration = 10s. `block.timestamp = 100`. Epoch = 10. State at duration=10: `{epoch: 10, swapped: 500}`.
- Admin changes duration to 20s. State at duration=20 is fresh. Counters reset.
- Admin changes back to 10s at `block.timestamp = 109`. Epoch = 10 (still). State at duration=10: `{epoch: 10, swapped: 500}` — **no mismatch, no reset, stale counter applies**.

The window is `epochDuration` seconds (10s to 24 hours). Within this window, any revert to a previously used duration restores stale counters.

---

## 2. Contract + Function + Line Number

| Component | File | Line(s) |
|-----------|------|---------|
| `setEpochDuration` | `PSM.sol` | 997-1006 |
| `setPeriodDuration` | `PSM.sol` | 1082-1091 |
| `_handleEpochPeriodOperations` | `PSM.sol` | 1662-1836 |
| `_maybeRollEpoch` | `PSM.sol` | 1844-1850 |
| `_maybeRollPeriod` | `PSM.sol` | 1858-1864 |
| `_getCurrentEpoch` | `PSM.sol` | 1638-1640 |
| `_getCurrentPeriod` | `PSM.sol` | 1647-1649 |
| Duration-keyed state mappings | `deps/IPSM.sol` | 140-141 (GlobalState), 146-147 (CollateralState), 152-153 (BenefactorState) |

---

## 3. Attack Scenario (Step-by-Step)

### Setup
1. Admin deploys PSM with `epochDuration = 10 seconds` (the minimum).
2. `block.timestamp = 1000`. Current epoch = `1000 / 10 = 100`.
3. Benefactor A swaps 800 asset worth of collateral. Global epoch state at `duration=10`: `{epoch: 100, swappedForAssetInEpoch: 800}`.

### Duration change (legitimate adjustment)
4. Admin calls `setEpochDuration(20)` to increase the epoch length.
   - `globalState.config.epochDuration = 20`.
   - State at `duration=10` is **NOT cleared** — `{epoch: 100, swappedForAssetInEpoch: 800}` persists.
   - State at `duration=20` is fresh (default `{epoch: 0, ...}`).
5. `block.timestamp = 1000`. Current epoch = `1000 / 20 = 50`.
6. `_maybeRollEpoch` on `duration=20` slot: `0 != 50` → resets. Counters = 0. Fresh start. ✓ (This is the documented "effectively resets" behavior.)
7. Benefactor A swaps 500 asset. State at `duration=20`: `{epoch: 50, swappedForAssetInEpoch: 500}`.

### Duration revert (triggers the bug)
8. Admin calls `setEpochDuration(10)` to revert to the original duration.
   - `globalState.config.epochDuration = 10`.
   - State at `duration=10` is still `{epoch: 100, swappedForAssetInEpoch: 800}` — **NOT cleared**.
9. `block.timestamp = 1009` (9 seconds later, still in epoch 100 at duration=10).
   - Current epoch = `1009 / 10 = 100`.
10. Next swap: `_handleEpochPeriodOperations` loads `globalState.epochStateByDuration[10]` → `{epoch: 100, swappedForAssetInEpoch: 800}`.
11. `_maybeRollEpoch` checks `100 != 100` → **no mismatch** → **NO reset**.
12. The stale counter (800) applies. If the global max is 1000, only 200 more capacity remains — even though the admin "reset" by changing the duration and changing it back.

### Impact
- The admin expected a fresh start (or at least the current usage at `duration=20`). Instead, the **stale counter from the original `duration=10` usage** applies.
- If the admin had set a new, lower `maxSwapForAssetPerEpoch` (e.g., 500) expecting fresh counters, the stale counter (800) would **block all swaps** until the epoch rolls over.
- The bug is **silent** — no event, no warning, no way to detect without storage inspection.

### Variant: stale counter LOWER than expected
If the stale counter is **lower** than the actual usage (because swaps happened at a different duration), the protocol allows **more swaps than intended** within the current epoch. This is a rate-limit weakening, but only for the remainder of the epoch.

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

/// @title PoC_setEpochDuration_StaleState
/// @notice Verifies that changing epochDuration back to a previously used value
///         restores stale rate-limit counters if the current epoch number matches.
contract PoC_setEpochDuration_StaleState is Test {
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

    function setUp() public {
        asset = new MockERC20("Asset", "AST", 18);
        collateral = new MockERC20("Collateral", "COL", 18);
        oracle = new MockOracleFeed(PEG, block.timestamp);

        asset.mint(assetSendCustodian, 1_000_000e18);
        collateral.mint(collateralSendCustodian, 1_000_000e18);
        collateral.mint(benefactorA, 1_000_000e18);

        // Start with epochDuration = 10 seconds (the minimum)
        IPSM.GlobalConfig memory gc = IPSM.GlobalConfig({
            maxSwapForAssetPerEpoch: 1000e18,    // limit = 1000
            maxSwapForCollateralPerEpoch: LARGE,
            defaultBenefactorMaxSwapForAssetPerEpoch: LARGE,
            defaultBenefactorMaxSwapForCollateralPerEpoch: LARGE,
            epochDuration: 10,
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

        vm.startPrank(collateralManager);
        IPSM.CollateralConfig memory cc = IPSM.CollateralConfig({
            sendCustodianAddress: collateralSendCustodian,
            receiveCustodianAddress: collateralReceiveCustodian,
            oracleFeed: address(oracle),
            maxSwapForAssetPerEpoch: LARGE,
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
        psm.addCollateral(address(collateral), cc);
        vm.stopPrank();

        vm.prank(assetSendCustodian);
        asset.approve(address(psm), type(uint256).max);
        vm.prank(benefactorA);
        collateral.approve(address(psm), type(uint256).max);

        vm.prank(benefactorManager);
        psm.addBenefactor(benefactorA);
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

    /// @dev Test: stale epoch counters restore when duration is changed back
    ///      within the same epoch number.
    function test_SetEpochDuration_StaleCountersRestore() public {
        oracle.setPrice(PEG, block.timestamp);

        // ---- Step 1: Swap 800 at duration=10, epoch=block.timestamp/10 ----
        vm.prank(benefactorA);
        psm.swap(_buildOrder(1, 800e18));

        (uint128 swapped1, ) = psm.getGlobalEpochTotals();
        assertEq(swapped1, 800e18, "global epoch counter = 800 at duration=10");

        // ---- Step 2: Admin changes duration to 20 (fresh slot, resets) ----
        vm.prank(epochManager);
        psm.setEpochDuration(20);

        // Swap 500 at duration=20 (fresh counters)
        oracle.setPrice(PEG, block.timestamp);
        vm.prank(benefactorA);
        psm.swap(_buildOrder(2, 500e18));

        (uint128 swapped2, ) = psm.getGlobalEpochTotals();
        assertEq(swapped2, 500e18, "global epoch counter = 500 at duration=20 (fresh)");

        // ---- Step 3: Admin reverts to duration=10 (NO warp — same epoch number) ----
        vm.prank(epochManager);
        psm.setEpochDuration(10);

        // ---- Step 4: BUG — stale counter (800) is restored ----
        (uint128 swapped3, ) = psm.getGlobalEpochTotals();
        assertEq(
            swapped3,
            800e18,
            "BUG: stale counter (800) from duration=10 restored after revert — admin expected 0 or current usage"
        );

        // ---- Step 5: Only 200 capacity remains (1000 - 800), not 1000 ----
        // If the admin tries to swap 300, it reverts (800 + 300 > 1000)
        oracle.setPrice(PEG, block.timestamp);
        vm.prank(benefactorA);
        vm.expectRevert(
            abi.encodeWithSelector(
                IPSM.GlobalMaxSwapForAssetPerEpochExceeded.selector,
                300e18,                       // requested
                800e18,                       // currentUsage (STALE!)
                1000e18,                      // maxAllowed
                block.timestamp / 10,         // currentEpoch
                10                            // epochDuration
            )
        );
        psm.swap(_buildOrder(3, 300e18));

        // But 200 succeeds (800 + 200 <= 1000)
        vm.prank(benefactorA);
        psm.swap(_buildOrder(4, 200e18));
    }
}
```

### Expected test results

```
[PASS] test_SetEpochDuration_StaleCountersRestore()
  → assertEq(swapped1, 800e18)   PASSES
  → assertEq(swapped2, 500e18)   PASSES (fresh at duration=20)
  → assertEq(swapped3, 800e18)   PASSES (BUG: stale 800 restored at duration=10)
  → vm.expectRevert(GlobalMaxSwapForAssetPerEpochExceeded) PASSES (300 reverts: 800+300>1000)
  → swap(200) succeeds (800+200<=1000)
```

---

## 5. Impact Assessment

| Criterion | Assessment |
|-----------|------------|
| Fund loss? | **NO** — stale counters can only reduce capacity (block swaps) or reflect accurate usage |
| DoS? | **YES (temporary)** — if stale counter is high and admin expects fresh start, swaps blocked until epoch rollover |
| Rate-limit weakening? | **POSSIBLE (temporary)** — if stale counter is lower than actual usage at the current duration, protocol allows more swaps than intended for the rest of the epoch |
| Preconditions | Admin must change duration away and back to a previously used value, within the same epoch number |
| Detection difficulty | High — no event indicates stale state; admin has no visibility into which duration slots have stale data |
| Scope of impact | Global (all swaps affected), temporary (until epoch rollover) |
| Fix complexity | Low — clear old duration slot on change, or document the behavior |

### Rate-limit weakening scenario (more concerning)

If the stale counter at the reverted duration is **lower** than the actual recent usage, the protocol allows more swaps than intended:

1. Duration=10. Swap 800. Counter at (10, epoch=100) = 800.
2. Change to duration=20. Counter at (20, epoch=50) = 0 (fresh).
3. Swap 900 at duration=20. Counter at (20, epoch=50) = 900.
4. Revert to duration=10, same epoch=100. Counter at (10, epoch=100) = 800 (stale).
5. The admin sees 800 usage, but the actual recent usage was 900 at duration=20.
6. The protocol allows 200 more (1000 - 800), but the admin intended only 100 more (1000 - 900).

This is a **rate-limit weakening** — the protocol allows more swaps than the admin intended. However:
- The weakening is temporary (until epoch rollover).
- The "extra" capacity is only 100 (the difference between stale 800 and actual 900).
- No direct fund loss — the swaps are still within the global max (1000).

---

## 6. Severity: Low

**Rationale:**
- No fund loss. The stale counters only affect rate-limit enforcement, not authorization.
- The DoS/weakening is temporary (until epoch rollover, max 24 hours).
- The bug requires the admin to revert to a previously used duration within the same epoch number — a narrow window.
- A simple operational mitigation exists: never revert to a previously used duration within the same epoch, or explicitly warp past the epoch boundary before reverting.

**Why not Medium:**
- The rate-limit weakening variant is concerning, but the "extra" capacity is bounded by the difference between stale and actual usage, and it's temporary.
- The admin is the only one who can trigger the bug (setEpochDuration requires EPOCH_PERIOD_MANAGER_ROLE). No external attacker can trigger it.
- The bug is documented (partially) in the code comment: "current usage effectively resets" — the revert behavior is just an undocumented edge case.

---

## 7. Three-Perspective Audit

### Prosecutor (why the bug is real and exploitable)

1. **The code path is reachable.** `setEpochDuration` is a standard admin function. The admin can change the duration at any time.

2. **The stale state is real.** `setEpochDuration` only updates `config.epochDuration`. It does NOT clear `epochStateByDuration[oldDuration]`. The old state persists indefinitely.

3. **The same-epoch collision is reachable.** With `MIN_EPOCH_DURATION = 10 seconds`, the epoch window can be as short as 10 seconds. The admin can easily change the duration and revert within this window.

4. **The rate-limit weakening variant is exploitable.** A malicious or careless epoch manager could:
   - Swap a large amount at duration=10.
   - Change to duration=20 (fresh counters).
   - Swap more at duration=20.
   - Revert to duration=10 within the same epoch.
   - The stale counter at duration=10 is from before the duration=20 swaps, so it's lower than actual usage.
   - The protocol allows more swaps than intended.

5. **The bug is undocumented.** The code comment says "current usage effectively resets" but doesn't mention the revert behavior. The admin has no way to know about this edge case.

### Defense (why the bug is invalid or out of scope)

1. **The admin is trusted.** `setEpochDuration` requires `EPOCH_PERIOD_MANAGER_ROLE`. A trusted role holder is not an attacker. If they misuse the function, that's a trusted-key compromise, not a contract bug.

2. **The "effectively resets" comment is about the forward direction.** The comment says "current usage effectively resets" when the duration changes. This is true for the forward direction (new duration = fresh counters). The revert direction is an edge case, not the primary use case.

3. **The rate-limit weakening is bounded.** The "extra" capacity is bounded by the difference between stale and actual usage, and by the global max. No swap exceeds the global max. So there's no fund loss — just a temporary miscalculation of remaining capacity.

4. **The bug is self-inflicted.** The admin chose to revert the duration. If they don't revert, there's no bug. The admin can also warp past the epoch boundary before reverting to avoid the collision.

5. **The duration-keyed mapping is an intentional design.** The purpose is to "prevent epoch collisions" (per the comment). Using a new mapping slot per duration ensures that changing the duration doesn't mix old and new epoch numbers. The stale state at the old duration is an intentional trade-off for this isolation.

6. **Immunefi scope.** A temporary rate-limit miscalculation caused by a trusted role's action, with no fund loss, is below the threshold for a bounty.

### Judge (final verdict)

**Verdict: REAL BUG (design edge case), but LOW severity. DO NOT SUBMIT to Immunefi.**

**Reasoning:**

The bug is **technically real**. The code confirms:
- `setEpochDuration` only updates `config.epochDuration` (line 1003).
- It does NOT clear `epochStateByDuration[oldDuration]`.
- `_maybeRollEpoch` only resets when `epochState.epoch != currentEpoch`.
- If the duration is reverted within the same epoch number, `epochState.epoch == currentEpoch` and no reset occurs.
- The stale counter from the previous usage applies.

The Foundry PoC would confirm this behavior.

However, the **impact is LOW**:
- No fund loss. The stale counters only affect rate-limit enforcement.
- The DoS/weakening is temporary (until epoch rollover, max 24 hours).
- The bug requires a trusted role (EPOCH_PERIOD_MANAGER_ROLE) to trigger it.
- A simple operational mitigation exists (don't revert within the same epoch, or warp first).
- The rate-limit weakening variant is bounded by the global max — no swap exceeds it.

The bug is a **design edge case** rather than a clear vulnerability. The duration-keyed mapping pattern is intentional (to prevent epoch collisions), and the stale state at old durations is a trade-off. The code comment partially documents the behavior ("current usage effectively resets") but doesn't mention the revert edge case.

**Recommendation:** Do not submit to Immunefi. Document the revert behavior in the `setEpochDuration` and `setPeriodDuration` docstrings. Optionally, clear the old duration slot on change (gas cost: one SSTORE per change) to eliminate the edge case entirely.

**If submitted anyway:** Expect "Informational" or "Low" from triage. The lack of fund loss, trusted-role requirement, and temporary nature would likely be cited as reasons for low severity.

---

## Suggested Fix

**Option A: Clear old slot on change (simple, gas cost: 2 SSTOREs per change)**

```solidity
function setEpochDuration(uint256 newDuration) external override nonReentrant onlyRole(EPOCH_PERIOD_MANAGER_ROLE) {
    if (newDuration < MIN_EPOCH_DURATION) revert EpochDurationTooShort(newDuration, MIN_EPOCH_DURATION);
    if (newDuration > MAX_EPOCH_DURATION) revert EpochDurationTooLong(newDuration, MAX_EPOCH_DURATION);
    GlobalConfig storage config = globalState.config;
    if (config.epochDuration != newDuration) {
        uint256 oldDuration = config.epochDuration;
        // Clear old duration state to prevent stale restoration on revert
        delete globalState.epochStateByDuration[oldDuration];
        config.epochDuration = newDuration;
        emit EpochDurationUpdated(oldDuration, newDuration);
    }
}
```

Note: This only clears the GLOBAL state. Collateral and benefactor states would also need clearing (which requires enumeration — gas-expensive). A version/nonce scheme is more practical.

**Option B: Document the behavior**

Update the docstring:
> "Uses separate epoch state mappings per duration to prevent epoch collisions.
> When the duration changes, the new duration's state starts fresh (counters reset on first use).
> NOTE: If the duration is reverted to a previously used value within the same epoch number,
> the stale counters from the previous usage will apply. Admins should warp past the epoch
> boundary before reverting to avoid this."

**Option C: Version/nonce scheme (most robust)**

Use `epochStateByVersion[version][duration]` instead of `epochStateByDuration[duration]`. Increment `version` on every `setEpochDuration` call. Old state becomes unreachable.
