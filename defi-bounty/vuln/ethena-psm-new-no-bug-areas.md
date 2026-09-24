# PSM New-Bug Audit — Areas With No New Bugs Found

**Status:** Audit complete (NOT submitted to Immunefi)
**Severity:** N/A (no new bugs in these areas)
**Source:** Systematic audit of PSM.sol focus areas
**Contract:** `/home/z/fkr-step1/defi-bounty/contracts/PSM.sol`

---

## Summary

This report documents the focus areas that were audited but **no new bug was found**. Each area was analyzed thoroughly; the analysis and reasoning are documented below for transparency.

---

## Area 2: `_getQuote` (L1571) — No NEW bug (already covered)

### Analysis

| Edge case | Status | Reasoning |
|-----------|--------|-----------|
| `oraclePrice = 0` | **Handled** | Line 1613: `if (oraclePrice == 0) revert InvalidOraclePrice(...)`. Also double-checked in `_validateOraclePrice` (line 1529). |
| `pegPrice = MAX` | **Handled** | Constructor (line 217) and `setPegPrice` (line 396) validate `pegPrice <= MAX_PEG_PRICE = 1000e18`. |
| 0-decimal collateral | **Handled** | `_validateCollateralConfig` (line 2080) reverts if `config.decimals == 0`. |
| Division by zero | **Handled** | All divisors (`pegPrice`, `ONE_ETHER`, `oraclePrice`, `10 ** decimals`) are validated to be non-zero. |
| `feeRate > BASIS_POINTS` | **Handled** | `setBenefactorSwapForAssetFee` (line 712) validates `fee > MAX_FEE (100)`. `_validateCollateralConfig` (line 2069) validates `defaultSwapForAssetFee <= MAX_FEE (100)`. Since `MAX_FEE (100) < BASIS_POINTS (10000)`, `feeRate` is always in `[0, 100]`. |
| Overflow in pricing math | **Already covered** | The overflow when `pegPrice` is near `MAX_PEG_PRICE` and `amountIn` is near `uint128` max is documented in the existing report `ethena-untested-getquote-overflow.md`. Not a new bug. |
| Fee bypass when oracle diverges | **Already covered** | Documented in the existing report. Not a new bug. |

### Verdict: **No new bug.** The `_getQuote` function's edge cases are either handled or already documented in the existing overflow report.

---

## Area 3: `rescueFunds` — No bug

### Analysis

| Concern | Status | Reasoning |
|---------|--------|-----------|
| Admin drain user funds | **Safe** | The PSM contract never holds user funds in normal operation. `swap` uses `safeTransferFrom` to move tokens BETWEEN custodians and users — the PSM is the caller (msg.sender of transferFrom), not the FROM or TO. So the PSM doesn't receive tokens during swaps. |
| Rescue asset token | **Safe** | `rescueFunds` allows rescuing ANY token (including `asset`). But the PSM shouldn't hold asset tokens in normal operation. If someone accidentally sends tokens to the PSM, `rescueFunds` recovers them — this is the intended use. |
| `recipient = address(0)` | **Handled** | `onlyValidAddress(recipient)` (line 419) reverts if recipient is zero. |
| `token = address(0)` | **Handled** | `onlyValidAddress(token)` (line 420) reverts if token is zero. |
| `amount = 0` | **Handled** | Line 422: `if (amount == 0) revert InvalidAmount(amount)`. |
| Rescue during swap | **Safe** | Both `rescueFunds` and `swap` are `nonReentrant`. They can't interleave within a single tx. |

### Verdict: **No bug.** `rescueFunds` is correctly guarded and serves its intended purpose (recovering accidentally-sent tokens). The PSM doesn't hold user funds, so there's nothing to drain.

---

## Area 4: `swap()` Reentrancy via Custodian — No bug

### Analysis

| Concern | Status | Reasoning |
|---------|--------|-----------|
| `swap` is `nonReentrant` | **Handled** | Line 268: `function swap(Order calldata order) external override nonReentrant`. Reentrancy guard prevents any re-entry into `swap` or other `nonReentrant` functions. |
| All state-modifying functions are `nonReentrant` | **Handled** | Every external function that modifies state (`swap`, `setAssetSendCustodian`, `setAssetReceiveCustodian`, `setPegPrice`, `rescueFunds`, `enableSwap`, `disableSwap`, `enableCollateral`, `disableCollateral`, `addCollateral`, `removeCollateral`, `updateCollateralConfig`, `enableBenefactor`, `disableBenefactor`, `addBenefactor`, `removeBenefactor`, `setBenefactor*`, `setDelegatedSigner`, `confirmDelegatedSigner`, `removeDelegatedSigner`, `setApprovedBeneficiary`, `setGlobal*`, `set*Duration`) has `nonReentrant`. No cross-function reentrancy possible. |
| Checks-effects-interactions pattern | **Handled** | `swap` does: (1) validate, (2) compute quote, (3) update epoch/period state (EFFECTS), (4) mark nonce used, (5) transfer tokens (INTERACTIONS). Effects before interactions. ✓ |
| Malicious custodian contract | **Safe** | If a custodian is a malicious contract that reverts on `transferFrom`, the swap reverts (DoS only). If it tries to reenter, `nonReentrant` blocks it. |
| Malicious token (ERC-777 hooks) | **Safe** | `safeTransferFrom` may trigger token hooks. But `swap` is `nonReentrant`, and all state is updated before transfers. |
| `order.benefactor` is a contract | **Safe** | If benefactor is a contract with `onTransferReceived` hook, `safeTransferFrom` may trigger it. But `nonReentrant` blocks re-entry. |
| `order.beneficiary` is a contract | **Safe** | Same as above. |

### Verdict: **No bug.** The PSM uses `nonReentrant` on ALL state-modifying functions and follows checks-effects-interactions. No reentrancy vector found.

---

## Area 5: `_validateOraclePrice` — No bug

### Analysis

| Edge case | Status | Reasoning |
|-----------|--------|-----------|
| `MAX_FUTURE_TIMESTAMP_TOLERANCE` | **Handled** | Line 1531: `if (updatedAt > block.timestamp + MAX_FUTURE_TIMESTAMP_TOLERANCE)` reverts. Tolerance = 15 seconds. |
| `maxOracleAge = 0` | **Handled** | `_validateCollateralConfig` (line 2073) reverts if `maxOracleAge < MIN_ORACLE_AGE (10)`. So `maxOracleAge` is always >= 10. |
| `min > max` | **Handled** | `_validateCollateralConfig` (line 2077) reverts if `minOraclePrice > maxOraclePrice`. So `min <= max` always. |
| `price = 0` | **Handled** | Line 1529: `if (price == 0) revert InvalidOraclePrice(...)`. |
| Stale price | **Handled** | Lines 1535-1536: `timeDiff > maxOracleAge` reverts. |
| Future price (slight) | **Handled** | Line 1535: `updatedAt > block.timestamp ? 0 : block.timestamp - updatedAt`. A slightly-future price (within tolerance) gives `timeDiff = 0`, which passes the staleness check. |
| Depeg (swapForAsset) | **Handled** | Line 1539: `if (price < minOraclePrice) revert OracleSwapForAssetDepegDetected(...)`. |
| Depeg (swapForCollateral) | **Handled** | Line 1543: `if (price > maxOraclePrice) revert OracleSwapForCollateralDepegDetected(...)`. |
| `updatedAt = 0` | **Handled** | `timeDiff = block.timestamp - 0 = block.timestamp`, which is huge (> maxOracleAge). Reverts. |
| `block.timestamp + MAX_FUTURE_TIMESTAMP_TOLERANCE` overflow | **Not realistic** | Would require `block.timestamp` near `type(uint256).max`. Not a practical concern. |

### Verdict: **No bug.** All oracle price edge cases are handled. The validation is thorough and correct.

---

## Area 6: Admin Function Race Conditions — No bug

### Analysis

| Concern | Status | Reasoning |
|---------|--------|-----------|
| `setAssetSendCustodian` mid-swap | **Safe** | Both `swap` and `setAssetSendCustodian` are `nonReentrant`. They can't interleave within a single tx. Across txs, each reads the current state. |
| `setPegPrice` mid-swap | **Safe** | Same — `nonReentrant`. The peg price is read at the start of `_getQuote` (line 1596) and used consistently within the same tx. |
| `setEpochDuration` mid-swap | **Safe** | Same — `nonReentrant`. The duration is read at the start of `_handleEpochPeriodOperations` (line 1670). |
| `updateCollateralConfig` mid-swap | **Safe** | Same — `nonReentrant`. The collateral config is read at the start of `swap` (line 280). |
| `removeCollateral` mid-swap | **Safe** | Same — `nonReentrant`. |
| `disableBenefactor` mid-swap | **Safe** | Same — `nonReentrant`. |
| `disableSwap` mid-swap | **Safe** | Same — `nonReentrant`. `isSwapEnabled` is checked at line 269. |
| Cross-tx race (same block) | **Safe** | Each tx reads the state at its execution. The second tx sees the updated state. No inconsistency. |

### Verdict: **No bug.** All admin functions are `nonReentrant`, preventing mid-swap modification. Cross-tx races are handled by the EVM's serial tx execution within a block.

---

## Area 8: Constructor Initialization — No bug

### Analysis

| Concern | Status | Reasoning |
|---------|--------|-----------|
| Uninitialized state | **Handled** | The constructor sets all initial state: `asset`, `assetDecimals`, `assetSendCustodianAddress`, `assetReceiveCustodianAddress`, `isSwapEnabled = true`, `globalState.config`. No uninitialized variables. |
| Front-run | **N/A** | The constructor is called at deployment. Deployment is atomic (single tx). No front-running possible. |
| `admin = address(0)` | **Handled** | Line 202: `onlyValidAddress(_admin)` reverts if admin is zero. |
| `_asset = address(0)` | **Handled** | Line 199: `onlyValidAddress(_asset)` reverts if asset is zero. |
| `_assetSendCustodian = address(0)` | **Handled** | Line 200: `onlyValidAddress(_assetSendCustodian)`. |
| `_assetReceiveCustodian = address(0)` | **Handled** | Line 201: `onlyValidAddress(_assetReceiveCustodian)`. |
| Role arrays with zero addresses | **Handled** | `_grantRoleToAddresses` (line 1458) checks each address for zero. |
| `_globalConfig` validation | **Handled** | Lines 204-229: extensive validation of epoch/period durations, peg price, and default benefactor limits. |
| `assetDecimals` too high | **Known limitation** | Line 232 comment: "Assumes a well-behaved token (decimals <= 18); abnormally high decimals cause _getQuote overflow (DoS, no fund loss)". This is documented and covered in the existing overflow report. |
| `assetDecimals = 0` | **Safe** | If asset has 0 decimals, `10 ** assetDecimals = 1`. Pricing math still works (just different scaling). No bug. |

### Verdict: **No bug.** The constructor is thoroughly validated. All inputs are checked. The only known limitation (high asset decimals) is documented.

---

## Area 9: View Functions with Side Effects — No NEW bug (already covered)

### Analysis

| Function | View? | Side effects | Status |
|----------|-------|-------------|--------|
| `getQuote` (line 1108) | **NO** (`nonReentrant`, not `view`) | Emits `OraclePriceValidated` via `_validateOraclePrice` | **Already covered** in existing report `ethena-untested-getquote-overflow.md` (Issue 3) |
| `getBenefactorConfig` (line 1153) | `view` | None | **Safe** |
| `getBenefactorFeesForCollateral` (line 1182) | `view` | None | **Safe** |
| `getDelegatedSignerStatus` (line 1213) | `view` | None | **Safe** |
| `isApprovedBeneficiary` (line 1230) | `view` | None | **Safe** |
| `getGlobalEpochTotals` (line 1241) | `view` | None | **Safe** |
| `getCollateralEpochTotals` (line 1262) | `view` | None | **Safe** |
| `getBenefactorEpochTotal` (line 1283) | `view` | None | **Safe** |
| `getGlobalPeriodTotals` (line 1303) | `view` | None | **Safe** |
| `getCollateralPeriodTotals` (line 1324) | `view` | None | **Safe** |
| `getBenefactorPeriodTotal` (line 1345) | `view` | None | **Safe** |
| `globalConfig` (line 1363) | `view` | None | **Safe** |
| `collateralConfig` (line 1372) | `view` | None | **Safe** |
| `defaultBenefactor*` (lines 1380-1405) | `view` | None | **Safe** |
| `getEpochEndTimestamp` (line 1413) | `view` | None | **Safe** |
| `getPeriodEndTimestamp` (line 1423) | `view` | None | **Safe** |

### Verdict: **No new bug.** The only non-view function (`getQuote`) is already documented in the existing report. All other "view" functions are properly `view` and have no side effects.

---

## Overall Summary for No-Bug Areas

| Area | Verdict | Reason |
|------|---------|-------|
| 2. `_getQuote` | No new bug | All edge cases handled or already documented |
| 3. `rescueFunds` | No bug | PSM doesn't hold user funds; all inputs validated |
| 4. Reentrancy via custodian | No bug | All functions `nonReentrant`; CEI pattern followed |
| 5. `_validateOraclePrice` | No bug | All edge cases handled |
| 6. Admin race conditions | No bug | `nonReentrant` prevents mid-swap modification |
| 8. Constructor | No bug | All inputs validated |
| 9. View side effects | No new bug | Only `getQuote` is non-view, already documented |
