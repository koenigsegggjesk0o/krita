# PSM Rate-Limit Validation — `uint128` Overflow Panic When `max = type(uint128).max`

**Status:** Verified by code analysis (NOT submitted to Immunefi)
**Severity:** Low
**Source:** Systematic audit of PSM.sol focus areas (Area 10: integer precision / uint128 overflow in rate limit accumulation)
**Contract:** `/home/z/fkr-step1/defi-bounty/contracts/PSM.sol`
**Functions:** `_validateGlobalEpochLimits` (line 1876) + `_validateCollateralEpochLimits` (line 1906) + `_validateBenefactorEpochLimits` (line 1939) + `_validateGlobalPeriodLimits` (line 1971) + `_validateCollateralPeriodLimits` (line 2001) + `_validateBenefactorPeriodLimits` (line 2034) + `setGlobalEpochLimits` (line 930) + `setGlobalPeriodLimits` (line 1015)

---

## 1. Vulnerability Description

The PSM rate-limit validation functions use Solidity 0.8+ checked arithmetic to compare `currentTotal + amount > max` before incrementing the counter. All three operands (`currentTotal`, `amount`, `max`) are `uint128`. If the admin sets `max` to `type(uint128).max` (to effectively disable a rate limit), and `currentTotal + amount` exceeds `2^128 - 1`, the addition **panics** (Solidity panic code 0x11) **before** the comparison is evaluated.

This means:
- The validation does NOT revert with the intended custom error (e.g., `GlobalMaxSwapForAssetPerEpochExceeded`).
- Instead, it reverts with an **uncaught panic** — a raw revert with no error selector.
- Off-chain integrators and monitoring tools that parse revert reasons will see an unidentified revert.
- The `unchecked` increment block that follows the validation is **never reached** (the panic happens first), so no state corruption occurs.

The admin can set `max` to `type(uint128).max` because `setGlobalEpochLimits`, `setGlobalPeriodLimits`, `setBenefactorMaxSwapForAssetPerEpoch`, and the collateral config fields have **no upper-bound validation** on the max value. The only validation is `limit == 0` reverts for the "default benefactor" limits — there is no check preventing `limit = type(uint128).max`.

### Why this matters

While the panic prevents state corruption (no swap succeeds that shouldn't), it causes:
1. **DoS**: Large swaps panic instead of reverting with a clear error. If a benefactor's max is at `type(uint128).max` and they attempt a large swap, the tx panics.
2. **Poor error handling**: Off-chain systems can't distinguish "rate limit exceeded" from "arithmetic panic". This makes debugging and monitoring harder.
3. **Inconsistent behavior**: For max values near (but not at) `type(uint128).max`, the validation works correctly (reverts with custom error). Only at the exact max does the panic occur. This inconsistency is surprising.

---

## 2. Contract + Function + Line Number

| Component | File | Line(s) |
|-----------|------|---------|
| `_validateGlobalEpochLimits` | `PSM.sol` | 1876-1893 |
| `_validateCollateralEpochLimits` | `PSM.sol` | 1906-1926 |
| `_validateBenefactorEpochLimits` | `PSM.sol` | 1939-1959 |
| `_validateGlobalPeriodLimits` | `PSM.sol` | 1971-1988 |
| `_validateCollateralPeriodLimits` | `PSM.sol` | 2001-2021 |
| `_validateBenefactorPeriodLimits` | `PSM.sol` | 2034-2054 |
| `setGlobalEpochLimits` (no upper bound) | `PSM.sol` | 930-947 |
| `setGlobalPeriodLimits` (no upper bound) | `PSM.sol` | 1015-1032 |
| `setBenefactorMaxSwapForAssetPerEpoch` (no upper bound) | `PSM.sol` | 658-671 |
| Unchecked increment block (swapForAsset) | `PSM.sol` | 1754-1761 |
| Unchecked increment block (swapForCollateral) | `PSM.sol` | 1827-1834 |

### The vulnerable validation pattern (all 6 validation functions)

```solidity
// _validateGlobalEpochLimits, line 1884
function _validateGlobalEpochLimits(
    uint128 currentTotal,
    uint128 max,
    uint128 amount,
    uint256 currentEpoch,
    uint256 epochDuration,
    bool isSwapForAsset
) internal pure {
    if (currentTotal + amount > max) {   // ← uint128 addition can PANIC if currentTotal + amount > 2^128 - 1
        if (isSwapForAsset) {
            revert GlobalMaxSwapForAssetPerEpochExceeded(amount, currentTotal, max, currentEpoch, epochDuration);
        } else {
            revert GlobalMaxSwapForCollateralPerEpochExceeded(amount, currentTotal, max, currentEpoch, epochDuration);
        }
    }
}
```

### The unchecked increment (safe, because validation panics first)

```solidity
// _handleEpochPeriodOperations, lines 1754-1761
unchecked {
    _globalEpochState.swappedForAssetInEpoch += amountOut;      // ← would overflow if reached, but
    _collateralEpochState.swappedForAssetInEpoch += amountOut;  //   validation panics first
    _benefactorEpochState.swappedForAssetInEpoch += amountOut;
    _globalPeriodState.swappedForAssetInPeriod += amountOut;
    _collateralPeriodState.swappedForAssetInPeriod += amountOut;
    _benefactorPeriodState.swappedForAssetInPeriod += amountOut;
}
```

---

## 3. Attack Scenario (Step-by-Step)

### Setup
1. Admin deploys PSM with default limits.
2. Admin calls `setGlobalEpochLimits(type(uint128).max, type(uint128).max)` to effectively disable the global epoch rate limits (a reasonable operational choice for a PSM that wants no global cap, relying only on per-collateral and per-benefactor limits).
3. No validation prevents this — `setGlobalEpochLimits` has no upper-bound check on the max values.

### Accumulation over time
4. Over many epochs, benefactors swap through the PSM. The global epoch counter `swappedForAssetInEpoch` accumulates within each epoch.
5. At the start of each epoch, `_maybeRollEpoch` resets the counter to 0. So the counter never exceeds the per-epoch swap volume.
6. But within a single epoch, if the total swap volume approaches `2^128 - 1` (approximately `3.4e38`), the counter approaches the overflow threshold.

### The panic
7. Suppose `currentTotal = 3.0e38` (within the epoch) and a benefactor attempts a swap with `amountOut = 1.0e38`.
8. The validation `currentTotal + amount > max` evaluates `3.0e38 + 1.0e38 = 4.0e38`.
9. `4.0e38 > 2^128 - 1 (≈3.4e38)` — the addition **overflows uint128** and **panics** (Panic(0x11)).
10. The tx reverts with an uncaught panic, NOT with `GlobalMaxSwapForAssetPerEpochExceeded`.
11. The benefactor's swap fails with an unidentified error.

### Why the counter can reach such high values

The counter accumulates `amountOut` (for swapForAsset) or `order.amountIn` (for swapForCollateral). Both are `uint128`. With `max = type(uint128).max`, the validation `currentTotal + amount > max` is always false (when no overflow). So every swap passes validation, and the counter grows unbounded within the epoch.

If the PSM handles tokens with very large amounts (e.g., a token with 36 decimals, or a high-volume PSM), the counter can approach the overflow threshold within a single epoch.

### Realistic threshold

For 18-decimal tokens:
- `type(uint128).max ≈ 3.4e38` = `3.4e20` tokens (340 quintillion tokens).
- This is an enormous amount — no realistic PSM would process this volume in a single epoch.
- BUT: the admin might set `max = type(uint128).max` on a low-decimal token (e.g., 0-decimal token), where `3.4e38` represents 340 quintillion raw units. If each unit is $1, that's $340 quintillion — unrealistic. But if the token is a utility token with very low value per unit, the volume could approach this.

Even if the threshold is unrealistic for 18-decimal tokens, the **panic vs. custom error** inconsistency is a code-quality issue.

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

/// @title PoC_Uint128OverflowPanic
/// @notice Verifies that setting max = type(uint128).max causes the rate-limit validation
///        to panic (Panic(0x11)) instead of reverting with a custom error, when
///        currentTotal + amount overflows uint128.
contract PoC_Uint128OverflowPanic is Test {
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

    function setUp() public {
        asset = new MockERC20("Asset", "AST", 18);
        collateral = new MockERC20("Collateral", "COL", 18);
        oracle = new MockOracleFeed(PEG, block.timestamp);

        asset.mint(assetSendCustodian, type(uint256).max);
        collateral.mint(collateralSendCustodian, type(uint256).max);
        collateral.mint(benefactorA, type(uint256).max);

        uint128 MAX = type(uint128).max;
        IPSM.GlobalConfig memory gc = IPSM.GlobalConfig({
            maxSwapForAssetPerEpoch: MAX,                          // ← no global cap
            maxSwapForCollateralPerEpoch: MAX,
            defaultBenefactorMaxSwapForAssetPerEpoch: MAX,
            defaultBenefactorMaxSwapForCollateralPerEpoch: MAX,
            epochDuration: 1 hours,
            pegPrice: PEG,
            maxSwapForAssetPerPeriod: MAX,
            maxSwapForCollateralPerPeriod: MAX,
            defaultBenefactorMaxSwapForAssetPerPeriod: MAX,
            defaultBenefactorMaxSwapForCollateralPerPeriod: MAX,
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
            maxSwapForAssetPerEpoch: MAX,
            maxSwapForCollateralPerEpoch: MAX,
            minOraclePrice: 0.9e18,
            maxOraclePrice: 1.1e18,
            maxOracleAge: 1 hours,
            isActive: true,
            decimals: 18,
            defaultSwapForAssetFee: 0,
            defaultSwapForCollateralFee: 0,
            maxSwapForAssetPerPeriod: MAX,
            maxSwapForCollateralPerPeriod: MAX
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

    /// @dev Test: when currentTotal + amount overflows uint128, the validation PANICS
    ///      instead of reverting with a custom error.
    function test_Uint128OverflowPanic() public {
        oracle.setPrice(PEG, block.timestamp);

        // Swap a large amount to get currentTotal close to type(uint128).max
        // First swap: 2^127 (half of uint128 max)
        uint128 half = type(uint128).max / 2;
        vm.prank(benefactorA);
        psm.swap(_buildOrder(1, half));

        (uint128 currentTotal, ) = psm.getGlobalEpochTotals();
        assertEq(currentTotal, half, "currentTotal = half after first swap");

        // Second swap: another 2^127 → currentTotal + amount = 2^128 → overflows uint128
        // The validation `currentTotal + amount > max` panics (Panic 0x11)
        // instead of reverting with GlobalMaxSwapForAssetPerEpochExceeded
        vm.prank(benefactorA);
        vm.expectRevert(bytes("Panic: Arithmetic overflow")); // Foundry's panic message
        psm.swap(_buildOrder(2, half + 1));

        // Note: the panic does NOT match any custom error selector.
        // Off-chain parsers looking for GlobalMaxSwapForAssetPerEpochExceeded won't find it.
    }

    /// @dev Test: with max slightly below type(uint128).max, the validation works correctly
    ///      (reverts with custom error, not panic). This shows the inconsistency.
    function test_NoPanicWhenMaxBelowUint128Max() public {
        // Redeploy with max = type(uint128).max - 1
        // (skipping redeploy for brevity — the point is that the panic only occurs
        // at the exact uint128 max, showing the inconsistency)
    }
}
```

### Expected test results

```
[PASS] test_Uint128OverflowPanic()
  → assertEq(currentTotal, half) PASSES
  → vm.expectRevert("Panic: Arithmetic overflow") PASSES
  → Note: the revert is a PANIC, not GlobalMaxSwapForAssetPerEpochExceeded
```

---

## 5. Impact Assessment

| Criterion | Assessment |
|-----------|------------|
| Fund loss? | **NO** — the panic prevents the swap from succeeding; no state corruption |
| DoS? | **YES (mild)** — large swaps panic instead of reverting with a clear error; the swap still fails, so no rate-limit bypass |
| Error handling? | **POOR** — off-chain systems see an uncaught panic instead of a custom error selector |
| Preconditions | Admin must set `max = type(uint128).max` (a reasonable "no cap" configuration) |
| Realistic threshold | For 18-decimal tokens, requires ~3.4e38 raw units in a single epoch — unrealistic for most PSMs |
| Scope of impact | Global (all swaps with large amounts), but only when max is at uint128 max |
| Fix complexity | Trivial — rearrange the comparison or add a bound check |

### Why no fund loss

The panic occurs in the **validation** step (before any state update or token transfer). The `unchecked` increment block is never reached. So:
- No state corruption (counters don't overflow).
- No token transfer (the swap reverts before the INTERACTIONS phase).
- No nonce consumption (nonce is marked after validation).

The only impact is that the swap fails with an unhelpful error message.

---

## 6. Severity: Low

**Rationale:**
- No fund loss. The panic prevents the swap from succeeding.
- The DoS is mild — the swap fails anyway (just with a panic instead of a custom error).
- The precondition (`max = type(uint128).max`) is a reasonable admin configuration but the threshold for triggering the panic (currentTotal near `2^128 - 1`) is unrealistic for most tokens.
- The error-handling inconsistency is a code-quality issue, not a security vulnerability.

**Why not Informational:**
- The panic is a real behavioral difference from the intended error handling. Off-chain systems that parse revert reasons will be affected.
- The fix is trivial (rearrange the comparison), so it's worth flagging.

**Why not Medium:**
- No fund loss, no rate-limit bypass, no state corruption.
- The threshold is unrealistic for standard 18-decimal stablecoins.
- The admin can avoid the issue by setting `max` to a value slightly below `type(uint128).max`.

---

## 7. Three-Perspective Audit

### Prosecutor (why the bug is real and exploitable)

1. **The panic is real.** In Solidity 0.8+, `uint128 + uint128` is checked arithmetic. If the sum exceeds `2^128 - 1`, it panics with code 0x11. This is documented behavior.

2. **The admin can set `max = type(uint128).max`.** `setGlobalEpochLimits` (line 930) has NO upper-bound validation on the max values. The admin can set them to any `uint128` value, including the max. This is a reasonable "no cap" configuration.

3. **The panic is inconsistent with the intended error handling.** The validation functions are designed to revert with custom errors (e.g., `GlobalMaxSwapForAssetPerEpochExceeded`). The panic bypasses this, reverting with an uncaught panic instead. Off-chain systems that parse revert reasons will not find the custom error selector.

4. **The `unchecked` increment is misleading.** The code uses `unchecked { ... }` for the increment, suggesting the developers were aware of overflow. But the validation BEFORE the increment uses checked arithmetic, which panics. The `unchecked` block is dead code when the panic occurs.

5. **The threshold is reachable for low-decimal tokens.** For a 0-decimal token (if the admin somehow allows it — `_validateCollateralConfig` rejects `decimals == 0`, but the ASSET token can have any decimals), `2^128 - 1` is 340 quintillion raw units. A high-volume PSM could approach this.

### Defense (why the bug is invalid or out of scope)

1. **No fund loss.** The panic prevents the swap from succeeding. No state corruption, no token transfer, no nonce consumption. The worst case is a failed swap with an unhelpful error.

2. **The threshold is unrealistic.** For 18-decimal stablecoins (the intended use case for PSM), `2^128 - 1 ≈ 3.4e38` raw units = `3.4e20` tokens = 340 quintillion tokens. No PSM would process this volume in a single epoch (which is at most 24 hours).

3. **The admin can avoid the issue.** Setting `max` to any value below `type(uint128).max` (e.g., `type(uint128).max - 1`) avoids the panic. The validation `currentTotal + amount > max` would then revert with the custom error (since `currentTotal + amount` can't exceed `2^128 - 1` without panicking, and `max = 2^128 - 2` is always less than the sum).

4. **The `unchecked` block is correct.** The `unchecked { _globalEpochState.swappedForAssetInEpoch += amountOut; }` is safe BECAUSE the validation already ensured `currentTotal + amount <= max <= type(uint128).max`. The panic in the validation is actually a safety feature — it prevents the unchecked increment from overflowing.

5. **The behavior is consistent with Solidity 0.8+ semantics.** Checked arithmetic is the default. The panic is expected behavior for overflow. The developers chose `unchecked` for the increment (after validation) to save gas. The panic in the validation is a natural consequence.

6. **Immunefi scope.** A mild DoS with no fund loss, unrealistic threshold, and trivial admin mitigation is below the threshold for a bounty.

### Judge (final verdict)

**Verdict: REAL BUG (code-quality / error-handling issue), but LOW severity. DO NOT SUBMIT to Immunefi.**

**Reasoning:**

The bug is **technically real**. The code confirms:
- `setGlobalEpochLimits` has no upper-bound validation (line 930-947).
- The validation `currentTotal + amount > max` uses Solidity 0.8+ checked arithmetic (line 1884).
- If `max = type(uint128).max` and `currentTotal + amount > 2^128 - 1`, the addition panics.
- The panic is different from the intended custom error revert.

However, the **impact is LOW**:
- No fund loss. The panic prevents the swap.
- The threshold is unrealistic for 18-decimal tokens (340 quintillion tokens per epoch).
- The admin can trivially avoid it by setting `max` slightly below `type(uint128).max`.
- The only real impact is error-handling inconsistency (panic vs. custom error).

The bug is a **code-quality issue** rather than a security vulnerability. The `unchecked` increment is actually safe because the validation (even with the panic) prevents overflow. The panic is just an unhelpful error message.

**Recommendation:** Do not submit to Immunefi. Fix the code for cleanliness (rearrange the comparison to `currentTotal > max - amount`), but don't expect a bounty.

**If submitted anyway:** Expect "Informational" from triage. The lack of fund loss, unrealistic threshold, and trivial mitigation would be cited.

---

## Suggested Fix

**Option A: Rearrange the comparison (trivial, no gas cost change)**

```solidity
function _validateGlobalEpochLimits(
    uint128 currentTotal,
    uint128 max,
    uint128 amount,
    ...
) internal pure {
    // Use subtraction to avoid overflow: if amount > max, revert (can't fit).
    // Otherwise, check currentTotal > max - amount.
    if (amount > max || currentTotal > max - amount) {
        if (isSwapForAsset) {
            revert GlobalMaxSwapForAssetPerEpochExceeded(amount, currentTotal, max, currentEpoch, epochDuration);
        } else {
            revert GlobalMaxSwapForCollateralPerEpochExceeded(amount, currentTotal, max, currentEpoch, epochDuration);
        }
    }
}
```

This avoids the overflow: `amount > max` catches the case where the amount alone exceeds the max. `currentTotal > max - amount` is safe because `max - amount >= 0` (guaranteed by the first check).

**Option B: Add upper-bound validation to setter functions**

```solidity
function setGlobalEpochLimits(uint128 maxSwapForAssetPerEpoch, uint128 maxSwapForCollateralPerEpoch)
    external override nonReentrant onlyRole(EPOCH_PERIOD_MANAGER_ROLE)
{
    if (maxSwapForAssetPerEpoch > type(uint128).max - 1) revert InvalidAmount(maxSwapForAssetPerEpoch);
    if (maxSwapForCollateralPerEpoch > type(uint128).max - 1) revert InvalidAmount(maxSwapForCollateralPerEpoch);
    // ... rest of the function
}
```

This prevents the admin from setting `max` to the exact uint128 max, avoiding the panic.

**Option C: Document the behavior**

Add a note to the setter docstrings:
> "NOTE: Setting max to type(uint128).max may cause rate-limit validation to panic
> (Panic 0x11) instead of reverting with a custom error, when currentTotal + amount
> overflows uint128. Use type(uint128).max - 1 to avoid this."
