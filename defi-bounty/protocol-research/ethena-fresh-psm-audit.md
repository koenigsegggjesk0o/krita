# PSM.sol — Fresh Audit (Opus)

**Task ID:** eth-fresh-psm-audit
**Agent:** Opus
**Scope:** Fresh audit of PSM.sol (2082 lines) — find NEW bugs NOT already documented
**Known bug excluded:** `removeBenefactor` mapping persistence (delegatedSigners/approvedBeneficiaries persist after delete) — already found + verified by prior agent. NOT re-reported here.

---

## Executive Summary

A thorough fresh audit was performed across all 10 focus areas. The contract is **well-engineered**: CEI is honored in `swap()`, `nonReentrant` is applied universally, the dual-path pricing `min(oneToOne, oracle)` robustly protects the protocol against oracle manipulation in all four depeg scenarios, and the 6-layer rate limit logic is internally consistent with no integer-overflow bypass (the validate functions use checked arithmetic that reverts on overflow BEFORE the unchecked update block is reached).

**No CRITICAL or HIGH new bugs were found.** The findings below are LOW / INFORMATIONAL severity — defense-in-depth gaps, missing validations, and admin-trust assumptions. Several overlap with the prior PSM deep analysis (`ethena-psm-deep-analysis.md`) and the untested-code findings (`ethena-untested-getquote-overflow.md`); those overlaps are explicitly noted.

### New bugs found (not in prior documentation)

| # | Severity | Area | Finding |
|---|----------|------|---------|
| F-1 | LOW | Epoch/Period (1) | `setEpochDuration`/`setPeriodDuration` round-trip (D1→D2→D1) within the same epoch can double a benefactor's effective rate limit because the D1 rate-limit counters are NOT reset when duration changes — only the NEW duration's state is fresh. Documented as "current usage effectively resets" but the round-trip case is not reset. Requires admin action (EPOCH_PERIOD_MANAGER). |
| F-2 | LOW | Admin/Rescue (6) | `rescueFunds` has NO check that `token != address(asset)` and NO check that `token` is not a registered collateral. A compromised DEFAULT_ADMIN could rescue asset/collateral tokens that the PSM might hold due to a custodian misconfiguration (e.g., tokens sent directly to PSM, or a custodian that erroneously transferred to PSM). The PSM holds no funds in normal operation, but if it ever does (bug elsewhere), admin can drain. (Note: similar to R-1 in prior analysis but explicitly extends to `address(asset)` which was only noted for `address(this)`.) |
| F-3 | LOW | Benefactor Delegation (10) | `setDelegatedSigner` and `setApprovedBeneficiary` are callable by ANY address, even non-benefactors. This creates orphaned state entries in `benefactorState[msg.sender].config` for addresses that are not (and may never become) active benefactors. An attacker can grief storage by calling these functions with many addresses, though each call costs the attacker gas. Not exploitable for permission gain (confirmDelegatedSigner requires active benefactor). |
| F-4 | LOW | View/Quote (7) | `getQuote` does NOT check `benefactorState[benefactor].config.isActive`. A caller can get a quote for a non-existent or inactive benefactor — the quote silently uses default fees and returns a valid `amountOut`. The returned quote is misleading because the swap would revert with `BenefactorNotActive`. This is a UX/integration hazard for off-chain quote aggregators. |
| F-5 | INFO | Constructor (8) | Constructor does NOT validate that `maxSwapForAssetPerEpoch`, `maxSwapForCollateralPerEpoch`, `maxSwapForAssetPerPeriod`, `maxSwapForCollateralPerPeriod` (in `GlobalConfig`) are non-zero. Same for collateral-level maxes in `_validateCollateralConfig`. Deploying with any of these = 0 silently bricks all swaps in that direction (effective permanent pause). Already noted as C-3 INFO in prior analysis — confirmed here. |
| F-6 | INFO | Validate/Order (2) | `_validateOrder` uses `order.expiry < block.timestamp` (strict less-than), so an order with `expiry == block.timestamp` is valid in the current block. This is benign (good-till semantics) but worth noting: a miner manipulating `block.timestamp` forward by 1 second can expire an otherwise-valid order, or backward by 1 second can un-expire an order. Standard timestamp limitation. |
| F-7 | INFO | Pricing (3) | In `_getQuote`, the multiplication `amountIn * feeRate` is performed in `uint128` (both operands are `uint128`). With `amountIn` near `type(uint128).max` (~3.4e38) and `feeRate = MAX_FEE = 100`, the product (~3.4e40) overflows `uint128` and reverts in Solidity 0.8+. This is a theoretical DoS for pathologically large swaps — unreachable in practice because no real token has a supply near 3.4e38, but it means the `feeAmount = (amountIn * feeRate) / BASIS_POINTS` line is not overflow-safe for adversarial inputs. |
| F-8 | INFO | Rate Limit (9) | No lifetime/global cap exists across epochs. A benefactor can swap up to `maxSwapForAssetPerEpoch` every epoch indefinitely, accumulating unbounded total outflow. By design — admins are expected to monitor and disable bad actors. Confirmed. |

### Already-documented findings confirmed (NOT new)

These were found in prior analysis and are confirmed correct — listed here only to show they were re-verified:
- `removeCollateral` rate-limit state persistence (RC-2 in prior analysis) — same Solidity `delete`-doesn't-clear-mappings limitation, causes recoverable DoS on re-add within same epoch.
- `getQuote` is non-view / emits `OraclePriceValidated` event (Issue 3 in getquote-overflow.md).
- `_getQuote` overflow with high `pegPrice` + high `amountIn` (Issue 1 in getquote-overflow.md).
- Misleading `feeAmount` reporting when oracle path wins (Issue 2 in getquote-overflow.md).
- `disableCollateral` / `disableBenefactor` missing `onlyValidAddress` (DC-1, DB-1).

---

## Area 1 — `_handleEpochPeriodOperations` (L1662-1836)

### Functions analyzed
- `_handleEpochPeriodOperations` (L1662)
- `_maybeRollEpoch` (L1844)
- `_maybeRollPeriod` (L1858)
- `_getCurrentEpoch` (L1638)
- `_getCurrentPeriod` (L1647)
- `_validateGlobalEpochLimits` / `_validateCollateralEpochLimits` / `_validateBenefactorEpochLimits` (L1876-1959)
- `_validateGlobalPeriodLimits` / `_validateCollateralPeriodLimits` / `_validateBenefactorPeriodLimits` (L1971-2054)
- `setEpochDuration` (L997), `setPeriodDuration` (L1082)

### Edge cases tested (mental simulation)

**1a. Can attacker bypass rate limits by manipulating epoch/period boundaries?**
No. Epoch/period numbers are derived from `block.timestamp / duration`. An attacker cannot manipulate `block.timestamp` (miners can, by ~15s). A ~15s shift can trigger an early rollover of at most 1-2 epochs (with `epochDuration = 10s`), which resets that epoch's counters. This is a known limitation of timestamp-based rate limits, not PSM-specific.

**1b. What happens if `epochDuration` is changed mid-epoch?**
The rate-limit state is keyed by duration: `epochStateByDuration[epochDuration]`. When duration changes D1→D2, the state at D2 is used. If D2 was never used in the current epoch, `_maybeRollEpoch` sets `epoch = currentEpoch` and `swapped = 0` — **fresh limit**. The NatSpec documents this: "current usage effectively resets". This is by design but means admin duration changes implicitly reset rate limits.

**1c. Round-trip duration change D1→D2→D1 within the same epoch (FINDING F-1).**
This is the NEW finding:
1. Duration = D1. Attacker swaps X. `epochStateByDuration[D1] = {epoch: N, swapped: X}`.
2. Admin changes to D2. `epochStateByDuration[D2]` is fresh → `_maybeRollEpoch` sets `{epoch: N, swapped: 0}`.
3. Attacker swaps Y (up to `max`). `epochStateByDuration[D2] = {epoch: N, swapped: Y}`.
4. Admin changes back to D1. `epochStateByDuration[D1]` still has `{epoch: N, swapped: X}` — `_maybeRollEpoch` does NOT roll (epoch matches).
5. Attacker swaps Z (up to `max - X`). 

**Total in epoch N: X + Y + Z = X + Y + (max - X) = Y + max.** If Y = max, attacker got **2× the epoch limit**.

This requires two admin duration changes within the same epoch window. The admin is trusted, but the code does not prevent this. The NatSpec's "current usage effectively resets" is accurate for the NEW duration but misleading for the round-trip case — the OLD duration's counters are NOT reset.

**1d. What if `periodDuration = 0`? = `epochDuration`?**
`periodDuration = 0` is prevented: `MIN_PERIOD_DURATION = 10` enforced in constructor + `setPeriodDuration`. `periodDuration == epochDuration` is allowed (not a bug, just redundant limiting on the same timescale).

**1e. Can attacker front-run admin duration change?**
Yes — an attacker who sees `setEpochDuration` in mempool can front-run with a swap at the OLD duration, then the duration changes, then swap again at the NEW (fresh) duration. This is the same root cause as F-1 but with a single duration change. Requires observing admin's mempool tx. Impact: 2× epoch limit. Mitigation: admin should not change duration within an epoch where swaps just occurred, or should manually wait for epoch rollover.

**1f. Integer overflow in epoch/period calculations?**
`block.timestamp / epochDuration`: `block.timestamp` ~1e10, `epochDuration` ≥ 10, so `currentEpoch` ~1e9. No overflow. `(currentEpoch + 1) * epochDuration` in `getEpochEndTimestamp`: max ~1e9 × 86400 = ~1e14. No overflow in uint256.

**1g. Unchecked arithmetic in update block (L1754-1761, L1827-1834)?**
The update uses `unchecked { _globalEpochState.swappedForAssetInEpoch += amountOut; }`. This is **safe** because the preceding `_validateGlobalEpochLimits` uses **checked** arithmetic: `if (currentTotal + amount > max)` — if `currentTotal + amount` overflows uint128, the expression reverts (Solidity 0.8+ checked arithmetic) BEFORE reaching the unchecked update. Since `max` is uint128, `currentTotal + amount <= max <= type(uint128).max`, so the unchecked addition cannot overflow. **No bypass via overflow.**

### Issues found
- **F-1 (LOW)**: Duration round-trip doubles epoch limit. See above.
- All other edge cases are safe or by-design.

---

## Area 2 — `_validateOrder` (L1472-1480)

### Functions analyzed
- `_validateOrder` (L1472)

### Edge cases tested

**2a. `amountIn < BASIS_POINTS` (10000) check — bypassable?**
No. The check is `order.amountIn < BASIS_POINTS` with strict less-than. `amountIn == BASIS_POINTS` (10000) passes. For a 6-decimal collateral, 10000 = 0.01 USDC — minimum dust. For 18-decimal asset, 10000 = 0.0001 asset. No bypass.

**2b. `amountIn = BASIS_POINTS` exactly?**
Passes. `feeAmount = 10000 * feeRate / 10000 = feeRate`. With `feeRate = 100` (1%), `feeAmount = 100`, `netAmountIn = 9900`. `amountOut` is computed from 9900. No edge-case issue.

**2c. `expiry` check — can attacker manipulate `block.timestamp`?**
`order.expiry < block.timestamp` reverts. A miner can shift `block.timestamp` by ~15s, potentially expiring or un-expiring an order near the boundary. Standard timestamp limitation. **F-6 (INFO)**: `expiry == block.timestamp` is valid (strict `<`), which is benign good-till semantics.

**2d. `chainId` check — what if chain forks?**
`order.chainId != block.chainid` reverts. On a chain fork, `block.chainid` changes on one side, invalidating orders. This is replay protection. There is NO signature on the order (see Area 5/10), so `chainId` is just a sanity check — the submitter provides the correct `chainId`. No exploit.

### Issues found
- **F-6 (INFO)**: `expiry == block.timestamp` valid; miner timestamp manipulation is a known limitation.

---

## Area 3 — `_getQuote` (L1571-1631)

### Functions analyzed
- `_getQuote` (L1571)

### Edge cases tested

**3a. `oraclePrice = 0`?**
`_validateOraclePrice` reverts at L1529 (`if (price == 0) revert`). `_getQuote` also has a defensive check at L1613 (`if (oraclePrice == 0) revert`). Double-protected. No issue.

**3b. `pegPrice = MAX_PEG_PRICE` (1000e18)?**
With `pegPrice = 1000e18`, `assetDecimals = 18`, `collateralDecimals = 6`:
- `oneToOneAmountOut` (swapForAsset) = `netAmountIn * 1e18 * 1e18 / (1000e18 * 1e6)` = `netAmountIn * 1e-9` — very small, no overflow.
- `oracleAmountOut` (swapForCollateral) = `amountIn * 1000e18 * 1e6 / (oraclePrice * 1e18)`. With `amountIn = type(uint128).max ≈ 3.4e38`, numerator = `3.4e38 * 1e21 * 1e6 = 3.4e65`. Below uint256 max (1.15e77). No overflow.
- However, with `collateralDecimals = 18` (not 6): numerator = `3.4e38 * 1e21 * 1e18 = 3.4e77 > 1.15e77` → **OVERFLOW → revert**. This is the already-documented Issue 1 in `ethena-untested-getquote-overflow.md`. Confirmed, not new.

**3c. Collateral with 0 decimals? 36 decimals?**
`_validateCollateralConfig` requires `config.decimals != 0` (L2080). So 0 decimals is blocked at config time. 36 decimals: `10^36` in the multiplication. With `amountIn = 3.4e38`, `3.4e38 * 1e18 * 1e36 = 3.4e92 > 1.15e77` → overflow revert. DoS only, no fund loss. This requires the admin to add a collateral with 36 decimals, which is implausible for stablecoins.

**3d. Division by zero?**
- `pegPrice * (10 ** _collateralConfig.decimals)`: `pegPrice > 0` (validated), `decimals > 0` (validated). No div-by-zero.
- `ONE_ETHER * (10 ** assetDecimals)`: `ONE_ETHER = 1e18 > 0`. `assetDecimals` could be 0 (no validation in constructor). `10^0 = 1`. `1e18 * 1 = 1e18 > 0`. No div-by-zero.
- `oraclePrice * (10 ** assetDecimals)`: `oraclePrice > 0` (validated). No div-by-zero.

**3e. `feeRate > BASIS_POINTS` cause negative `netAmountIn`?**
`feeRate` is capped: `customSwapForAssetFee` ≤ `MAX_FEE = 100` (validated in `setBenefactorSwapForAssetFee`). `defaultSwapForAssetFee` ≤ `MAX_FEE = 100` (validated in `_validateCollateralConfig`). So `feeRate ≤ 100 < 10000 = BASIS_POINTS`. `feeAmount = amountIn * feeRate / 10000 ≤ amountIn * 100 / 10000 = amountIn / 100 < amountIn`. No underflow in `netAmountIn = amountIn - feeAmount`.

**3f. `amountIn < feeAmount` (underflow)?**
As shown above, `feeAmount ≤ amountIn / 100 < amountIn` for `amountIn > 0` (and `amountIn ≥ BASIS_POINTS = 10000` is enforced). No underflow.

**3g. `amountIn * feeRate` overflow (FINDING F-7)?**
Both `amountIn` and `feeRate` are `uint128`. The multiplication `amountIn * feeRate` is performed in `uint128`. With `amountIn = type(uint128).max ≈ 3.4e38` and `feeRate = 100`, the product is `3.4e40`, which overflows `uint128` (max 3.4e38). Solidity 0.8+ reverts on overflow → DoS. This is theoretical (no real token has supply near 3.4e38) but the line is not overflow-safe for adversarial inputs. **F-7 (INFO)**.

**3h. Dual-path `min()` — does it protect the protocol in all 4 depeg scenarios?**

| Scenario | swapForAsset (coll→asset) | swapForCollateral (asset→coll) |
|----------|---------------------------|-------------------------------|
| Oracle = peg | oneToOne wins (after fee) — fee collected | oneToOne wins (after fee) — fee collected |
| Oracle > peg (coll expensive) | oracle path = MORE asset; oneToOne wins (less) → user gets 1:1-minus-fee. User loses vs market. | oracle path = LESS collateral; oracle wins → user gets less. User loses. |
| Oracle < peg (coll cheap) | oracle path = LESS asset; oracle wins → user gets less. User loses. | oracle path = MORE collateral; oneToOne wins (less) → user gets 1:1-minus-fee. User loses vs market. |

In ALL cases, `min()` picks the path that gives the user LESS, protecting the protocol. **The pricing logic is sound.** Oracle manipulation (within min/max bounds) cannot extract value from the protocol. The only "leak" is the fee bypass when oracle path wins (already documented as Issue 2).

### Issues found
- **F-7 (INFO)**: `amountIn * feeRate` can overflow uint128 for pathologically large `amountIn`. Theoretical DoS.
- Confirmed already-documented overflow with high `pegPrice` + 18-decimal collateral.

---

## Area 4 — `_validateOraclePrice` (L1522-1549)

### Functions analyzed
- `_validateOraclePrice` (L1522)

### Edge cases tested

**4a. `MAX_FUTURE_TIMESTAMP_TOLERANCE` value?**
`15` seconds (L114). `updatedAt > block.timestamp + 15` reverts. Allows minor clock drift (Pyth feeds). Reasonable.

**4b. `maxOracleAge` — what if oracle never updates?**
`timeDiff = block.timestamp - updatedAt` (if `updatedAt <= block.timestamp`). If `timeDiff > maxOracleAge`, reverts with `OraclePriceTooOld`. `maxOracleAge` is bounded: `MIN_ORACLE_AGE = 10` to `MAX_ORACLE_AGE = 1441 minutes` (~24h1m). If oracle never updates, after `maxOracleAge` seconds, all swaps revert. DoS, no fund loss. Correct behavior.

**4c. `minOraclePrice` / `maxOraclePrice` — can admin set to 0? To max uint?**
- `maxOraclePrice == 0`: blocked by L2076 (`revert InvalidOraclePriceThreshold`).
- `minOraclePrice == 0`: blocked by L2077.
- `minOraclePrice > maxOraclePrice`: blocked by L2077.
- No UPPER bound on `maxOraclePrice` (can be `type(uint128).max`) and no LOWER bound on `minOraclePrice` beyond non-zero (can be 1). Admin can effectively disable depeg protection by setting wide bounds. Admin trust.

**4d. Asymmetric bounds — `swapForAsset` checks min, `swapForCollateral` checks max. Exploit?**
- `swapForAsset` only checks `price < minOraclePrice` (collateral depeg DOWN protection).
- `swapForCollateral` only checks `price > maxOraclePrice` (collateral depeg UP protection).

Tested: if oracle is manipulated VERY HIGH (above `maxOraclePrice` but only checked for `swapForCollateral`):
- `swapForAsset` proceeds. `oracleAmountOut = amountIn * hugePrice / pegPrice` → huge. But `min(oneToOne, oracle)` picks `oneToOne` (smaller). User gets 1:1-minus-fee. **No exploit** — `min()` protects.
- If oracle is manipulated VERY LOW (below `minOraclePrice` but only checked for `swapForCollateral`):
- `swapForCollateral` proceeds. `oracleAmountOut = amountIn * pegPrice / lowPrice` → huge. But `min(oneToOne, oracle)` picks `oneToOne` (smaller). User gets 1:1-minus-fee. **No exploit**.

The asymmetric check + `min()` is **robust**. No exploit found.

**4e. Oracle reentrancy?**
`IOracleFeed.getPrice()` is called in `swap()` (nonReentrant) and `getQuote()` (nonReentrant). A malicious oracle could attempt reentry but is blocked by the reentrancy guard. The oracle could call view functions (no state change) or external contracts (no PSM state change). **No reentrancy exploit.**

### Issues found
- None new. The asymmetric bounds are safe due to `min()` in `_getQuote`.

---

## Area 5 — `swap()` (L268-336)

### Functions analyzed
- `swap` (L268)

### Edge cases tested

**5a. Order of operations: validate → quote → slippage → epochs → nonce → transfer.**
CEI pattern is followed:
- **Checks**: `_validateOrder`, collateral `isActive`, oracle + `_validateOraclePrice`, `_validateBenefactor`, `_getQuote`, slippage check.
- **Effects**: `_handleEpochPeriodOperations` (rate limit updates), `_benefactorState.orderNonceInvalidator[order.nonce] = true`.
- **Interactions**: `safeTransferFrom` calls (input pull, output push).

No state-modifying external call happens before effects. `nonReentrant` is applied. **CEI is honored.**

**5b. Can attacker front-run own order to bypass slippage?**
The slippage check is `amountOut < order.minAmountOut`. If attacker sets `minAmountOut = 1` (minimum allowed, since `_validateOrder` blocks 0), they accept any non-zero `amountOut`. This is the attacker's choice — they hurt themselves. No exploit.

**5c. `minAmountOut = 0` allowed?**
No. `_validateOrder` reverts: `if (order.minAmountOut == 0) revert InvalidAssetAmount(0)`. Minimum is 1.

**5d. Two orders same block to bypass rate limit?**
Each `swap()` call updates the rate-limit state in storage atomically. Two swaps in the same block (different transactions) are serialized by the EVM. The second swap sees the updated state from the first. If `currentTotal + amount2 > max`, the second reverts. **No bypass.**

**5e. Reentrancy via custodian callback?**
Custodians are wallets (EOAs or contracts) that are NOT called during swap — they are the `from` / `to` of `safeTransferFrom`. For standard ERC-20, no callback. For ERC-777/ERC-223, the token may trigger a callback on the recipient. The recipient (beneficiary or custodian) could be a contract that re-enters. But `swap()` is `nonReentrant`, and ALL state-modifying PSM functions are `nonReentrant`. **No reentrancy exploit.**

**5f. Fee-on-transfer tokens?**
If the input token charges a fee on transfer, the custodian receives less than `amountIn`, but the protocol sends full `amountOut` (based on `amountIn`). The protocol loses the fee portion. However, the protocol assumes standard ERC-20 tokens (admin must ensure collaterals are standard). Not a code bug — admin trust.

**5g. No signature on order — security implication?**
The order is a parameter struct, not a signed message. Authorization is via `msg.sender` (must be benefactor or ACCEPTED delegated signer). A delegated signer can submit ANY order with ANY nonce/beneficiary/amount (within rate limit and allowance). This is by design — delegated signers are trusted. The benefactor's recourse is `removeDelegatedSigner`. **Not a bug.**

### Issues found
- None new. `swap()` is robust.

---

## Area 6 — Admin functions (L340-1091)

### Functions analyzed
- `setAssetSendCustodian` (L345), `setAssetReceiveCustodian` (L369)
- `setPegPrice` (L394)
- `rescueFunds` (L414)
- `enableSwap` (L433), `disableSwap` (L445)
- `enableCollateral` (L458), `disableCollateral` (L481)
- `addCollateral` (L497), `removeCollateral` (L528), `updateCollateralConfig` (L552)
- `enableBenefactor` (L589), `disableBenefactor` (L611), `addBenefactor` (L624), `removeBenefactor` (L645) [removeBenefactor bug EXCLUDED]
- `setBenefactorMaxSwap*` (L658-835)
- `setDelegatedSigner` (L849), `confirmDelegatedSigner` (L868), `removeDelegatedSigner` (L887)
- `setApprovedBeneficiary` (L906)
- `setGlobalEpochLimits` (L930), `setGlobalPeriodLimits` (L1015)
- `setDefaultBenefactorMaxSwap*` (L955-1071)
- `setEpochDuration` (L997), `setPeriodDuration` (L1082)

### Edge cases tested

**6a. `setAssetSendCustodian` — race condition with swap?**
Both are `nonReentrant` → cannot run in same tx. Different-tx front-running is normal mempool behavior. If admin changes to an unprepared custodian (no approval/balance), swaps revert (fail-safe DoS, not theft). No exploit.

**6b. `setPegPrice` — can be called mid-swap?**
No. `swap()` and `setPegPrice()` are both `nonReentrant`. Shared `_status` flag prevents mid-swap call. `pegPrice` is read once at `_getQuote` L1596 and used for the remainder of the swap. **No mid-swap manipulation.**

**6c. `rescueFunds` — can drain user funds? (FINDING F-2)**
```solidity
function rescueFunds(address recipient, address token, uint128 amount) ... {
    if (amount == 0) revert InvalidAmount(amount);
    IERC20(token).safeTransfer(recipient, amount);
}
```
The PSM holds no funds in normal operation (transfers are direct benefactor↔custodian). BUT: there is NO check that `token != address(asset)` and NO check that `token` is not a registered collateral. If the PSM ever holds asset/collateral tokens (e.g., due to a custodian bug, accidental transfer, or fee-on-transfer dust), a compromised DEFAULT_ADMIN can drain them. The prior analysis (R-1) noted this for "any token" but did not specifically flag `address(asset)` and registered collaterals as the highest-value drain targets. **F-2 (LOW)** — extends R-1 with the specific asset/collateral drain vector.

**6d. `enableCollateral` / `disableCollateral` — race with swap?**
Both `nonReentrant`. `disableCollateral` sets `isActive = false`; `swap()` checks `isActive` at L282. If `disableCollateral` is in a tx before `swap()`, the swap reverts. Fail-safe. No exploit.

**6e. `addCollateral` — what if collateral already exists?**
L504: `if (collateralState.contains(collateral)) revert CollateralAlreadyExists(collateral)`. Protected.

**6f. `updateCollateralConfig` — what fields can be changed mid-swap?**
Cannot change mid-swap (nonReentrant). Between txs, admin can change: custodians, oracle feed, fees, limits, oracle bounds, `maxOracleAge`. A compromised `COLLATERAL_MANAGER` can swap to a malicious oracle or redirect custodians to attacker wallets. Admin trust — the role is trusted by design.

**6g. `removeCollateral` — rate limit state persistence (already documented as RC-2).**
`CollateralStateMap.remove` does `delete map._values[key].config` — zeroes config but CANNOT clear `epochStateByDuration` / `periodStateByDuration` mappings. Re-adding within same epoch → old counters persist → reduced limit or DoS. Conservative (protects protocol, never helps attacker). Recoverable on epoch rollover. **Confirmed, not new.**

**6h. `setGlobalEpochLimits` / `setGlobalPeriodLimits` — no zero validation (FINDING F-5).**
No `!= 0` check. Admin can set to 0 → all swaps in that direction revert (effective pause). Same for collateral-level maxes in `_validateCollateralConfig`. **F-5 (INFO)** — admin misconfiguration DoS, no attacker exploit.

**6i. `_isCustodian` O(n) gas (already implied by design).**
`_isCustodian` (L1436) iterates all collaterals. With many collaterals, `enableBenefactor`/`addBenefactor` become expensive. A compromised `COLLATERAL_MANAGER` could add ~500+ collaterals to exceed block gas limit for benefactor management. Confirmed — not new (implied by prior analysis), but worth flagging as a gas-DoS vector.

### Issues found
- **F-2 (LOW)**: `rescueFunds` can drain `address(asset)` and registered collaterals if PSM ever holds them.
- **F-5 (INFO)**: Missing zero-validation on global/collateral max swap limits.
- Confirmed RC-2 (removeCollateral persistence) and _isCustodian gas.

---

## Area 7 — View functions

### Functions analyzed
- `getQuote` (L1108) — **NOT view**
- `getBenefactorConfig` (L1153) — view
- `getBenefactorFeesForCollateral` (L1182) — view
- `getDelegatedSignerStatus` (L1213) — view
- `isApprovedBeneficiary` (L1230) — view
- `getGlobalEpochTotals` (L1241) — view
- `getCollateralEpochTotals` (L1262) — view
- `getBenefactorEpochTotal` (L1283) — view
- `getGlobalPeriodTotals` (L1303) — view
- `getCollateralPeriodTotals` (L1324) — view
- `getBenefactorPeriodTotal` (L1345) — view
- `globalConfig` (L1363) — view
- `collateralConfig` (L1372) — view
- `defaultBenefactorMaxSwap*` (L1380-1406) — view
- `getEpochEndTimestamp` (L1413) — view
- `getPeriodEndTimestamp` (L1423) — view

### Edge cases tested

**7a. `getQuote` is NOT view — side effects?**
`getQuote` is `external nonReentrant` (NOT `view`). It:
1. Calls `IOracleFeed(_collateralConfig.oracleFeed).getPrice()` — a state-modifying call (Pyth feeds require pushing price updates).
2. Calls `_validateOraclePrice` which EMITS `OraclePriceValidated(collateral, price)`.

This is already documented as Issue 3 in `ethena-untested-getquote-overflow.md`. Confirmed.

**7b. `getQuote` doesn't check benefactor active (FINDING F-4).**
`getQuote` reads `benefactorState[benefactor].config` but does NOT check `isActive`. For a non-existent or inactive benefactor, it returns a valid quote using default fees (since `customSwapForAssetFee` defaults to 0 → falls back to collateral default). The returned quote is **misleading** because the actual `swap()` would revert with `BenefactorNotActive`. Off-chain aggregators that use `getQuote` for estimation would get quotes for benefactors that can't actually swap. **F-4 (LOW)** — UX/integration hazard, no fund loss.

**7c. Other view functions — any side effects?**
All other view functions are pure reads (no external calls, no events, no state writes). Verified each. No side effects.

### Issues found
- **F-4 (LOW)**: `getQuote` doesn't validate benefactor is active — returns misleading quotes for inactive/non-existent benefactors.
- Confirmed `getQuote` non-view (Issue 3, already documented).

---

## Area 8 — Constructor + initialization (L185-247)

### Functions analyzed
- `constructor` (L185)
- `_grantRoleToAddresses` (L1456)

### Edge cases tested

**8a. Uninitialized state?**
All state is initialized in the constructor:
- `asset`, `assetDecimals` — set from params.
- `assetSendCustodianAddress`, `assetReceiveCustodianAddress` — set.
- `isSwapEnabled = true` — set.
- `globalState.config = _globalConfig` — set.
- Roles granted to admin + 7 role arrays.
No uninitialized state. ✓

**8b. Constructor front-run?**
Constructor runs in the deployment tx. The contract doesn't exist before. No front-run possible (unless CREATE2, but constructor params are fixed by deployer). ✓

**8c. `admin = address(0)`?**
Blocked by `onlyValidAddress(_admin)` modifier on constructor. ✓

**8d. No collaterals provided?**
Constructor doesn't take collaterals. Collaterals are added later via `addCollateral`. Deploying with zero collaterals is fine — no swaps can happen until collaterals are added. ✓

**8e. Missing validation on global max swap limits (FINDING F-5).**
Constructor validates:
- `epochDuration`, `periodDuration` in range ✓
- `defaultBenefactorMaxSwap*` non-zero ✓
- `pegPrice` non-zero, ≤ MAX_PEG_PRICE ✓

But does NOT validate:
- `maxSwapForAssetPerEpoch`, `maxSwapForCollateralPerEpoch` (global) — can be 0.
- `maxSwapForAssetPerPeriod`, `maxSwapForCollateralPerPeriod` (global) — can be 0.

Deploying with any = 0 → all swaps in that direction revert permanently (until admin sets non-zero via `setGlobalEpochLimits`/`setGlobalPeriodLimits`, which also lack zero-validation). **F-5 (INFO)** — confirmed.

**8f. `assetDecimals` validation?**
`assetDecimals = IERC20Metadata(_asset).decimals()` — no upper bound check. If asset reports >18 decimals, `_getQuote` can overflow (documented in constructor comment). DoS only, no fund loss. Already documented as C-2 INFO.

### Issues found
- **F-5 (INFO)**: Constructor missing zero-validation on global max swap limits.

---

## Area 9 — Rate limit accumulation across epochs

### Edge cases tested

**9a. Attacker does small swaps each epoch — cumulative effect?**
Each epoch, the rate-limit counters reset (`_maybeRollEpoch`). A benefactor can swap up to `maxSwapForAssetPerEpoch` every epoch, indefinitely. There is NO lifetime cap. Over N epochs, total outflow = N × max. **By design** — admins monitor and disable bad actors.

**9b. Global cap that prevents this?**
The only caps are per-epoch and per-period. No lifetime cap. The protocol relies on:
- Admin monitoring (off-chain).
- `BENEFACTOR_DISABLER_ROLE` to disable bad actors.
- `COLLATERAL_DISABLER_ROLE` to disable compromised collaterals.
- `GLOBAL_DISABLER_ROLE` to pause all swaps.

This is a trust assumption, not a code bug. **F-8 (INFO)** — confirmed by design.

**9c. Can attacker accumulate across multiple benefactors?**
Each benefactor has独立的 rate-limit counters. An attacker who controls multiple benefactor addresses can swap up to `maxPerBenefactor` per benefactor per epoch. Total = numBenefactors × maxPerBenefactor. But the GLOBAL limit (`maxSwapForAssetPerEpoch`) caps the total across all benefactors. So the global limit is the binding constraint. **No bypass via multiple benefactors.**

### Issues found
- **F-8 (INFO)**: No lifetime cap — by design, relies on admin monitoring.

---

## Area 10 — Benefactor delegation edge cases (NOT the persistence bug)

### Functions analyzed
- `setDelegatedSigner` (L849)
- `confirmDelegatedSigner` (L868)
- `removeDelegatedSigner` (L887)
- `setApprovedBeneficiary` (L906)
- `_validateBenefactor` (L1493)

### Edge cases tested

**10a. Can delegated signer delegate to another (transitive)?**
`setDelegatedSigner` uses `msg.sender` as the benefactor. A delegated signer (address D) acting on behalf of benefactor B can call `setDelegatedSigner(D2)` — but this sets D as the "benefactor" in `benefactorState[D].config`, NOT B. For D2 to confirm, D must be an active benefactor (`confirmDelegatedSigner` checks `isActive`). If D is not an active benefactor, confirmation reverts. **No transitive delegation.**

**10b. Can benefactor delegate to themselves?**
`setDelegatedSigner(benefactor)` → sets `delegatedSigners[benefactor] = PENDING`. `confirmDelegatedSigner(benefactor)` by benefactor → sets `ACCEPTED`. In `_validateBenefactor`, `msg.sender == order.benefactor` short-circuits the delegation check. Self-delegation is harmless (unnecessary but not exploitable). ✓

**10c. Delegated signer removed then re-added?**
1. `setDelegatedSigner(X)` → PENDING
2. `confirmDelegatedSigner` by X → ACCEPTED
3. `removeDelegatedSigner(X)` → REJECTED
4. `setDelegatedSigner(X)` → PENDING (no check on current status)
5. `confirmDelegatedSigner` by X → ACCEPTED

Re-delegation works. Requires benefactor to call `setDelegatedSigner` again. **By design.**

**10d. Attacker become delegated signer via race condition?**
`setDelegatedSigner` is called by `msg.sender` (the benefactor). An attacker cannot front-run this — the attacker would need to be `msg.sender`, making THEM the benefactor in that call. `confirmDelegatedSigner(benefactor)` checks `delegatedSigners[msg.sender] == PENDING` — only the address the benefactor specified can confirm. **No race condition.**

**10e. `setDelegatedSigner` / `setApprovedBeneficiary` callable by non-benefactors (FINDING F-3).**
Both functions use `msg.sender` as the benefactor and have NO `isActive` check. A non-benefactor can call `setDelegatedSigner(signer)` — this creates a `PENDING` entry in `benefactorState[msg.sender].config.delegatedSigners[signer]`. But:
- `confirmDelegatedSigner(benefactor)` requires `benefactorState[benefactor].config.isActive` → reverts for non-benefactor.
- The `PENDING` entry is orphaned (never confirmable).

Similarly, `setApprovedBeneficiary(beneficiary, true)` by a non-benefactor creates an orphaned `approvedBeneficiaries[beneficiary] = true` entry. Since the "benefactor" is not active, `_validateBenefactor` reverts before reaching the beneficiary check.

**Impact:** Minor storage bloat. An attacker can call these functions with many addresses, consuming gas (paid by attacker) and storage. No permission gain. **F-3 (LOW)**.

**10f. `removeDelegatedSigner` by non-benefactor?**
`removeDelegatedSigner(signer)` checks `if (config.delegatedSigners[signer] == REJECTED) revert`. For a non-benefactor, `delegatedSigners[signer]` defaults to `REJECTED` (enum index 0). So the function reverts. ✓ Non-benefactors can't call it.

**10g. No signature on order — delegated signer power?**
A delegated signer (ACCEPTED) can submit ANY order on behalf of the benefactor: any nonce, any approved beneficiary, any amount (up to rate limit + allowance). This is the documented trust model. The benefactor's only recourse is `removeDelegatedSigner`. **By design.**

### Issues found
- **F-3 (LOW)**: `setDelegatedSigner` / `setApprovedBeneficiary` callable by non-benefactors — orphaned state entries, minor storage bloat.

---

## Summary of New Findings

| ID | Severity | Area | Description | Exploitable? |
|----|----------|------|-------------|--------------|
| F-1 | LOW | Epoch/Period | Duration round-trip (D1→D2→D1) doubles epoch limit within same epoch | Requires 2 admin duration changes in same epoch |
| F-2 | LOW | Admin/Rescue | `rescueFunds` can drain `address(asset)` / registered collaterals if PSM holds them | Requires DEFAULT_ADMIN compromise + PSM holding funds |
| F-3 | LOW | Delegation | `setDelegatedSigner` / `setApprovedBeneficiary` callable by non-benefactors — orphaned state | No permission gain; storage bloat only |
| F-4 | LOW | View/Quote | `getQuote` doesn't validate benefactor active — misleading quotes | UX/integration hazard, no fund loss |
| F-5 | INFO | Constructor | No zero-validation on global/collateral max swap limits | Admin misconfiguration DoS |
| F-6 | INFO | Validate/Order | `expiry == block.timestamp` valid; miner timestamp manipulation | Standard limitation |
| F-7 | INFO | Pricing | `amountIn * feeRate` can overflow uint128 for pathological amounts | Theoretical DoS, unreachable in practice |
| F-8 | INFO | Rate Limit | No lifetime cap across epochs | By design |

**Critical: 0 | High: 0 | Medium: 0 | Low: 4 | Info: 4**

---

## Conclusion

The PSM contract is **well-engineered** with no Critical or High new bugs. The most interesting new finding is **F-1** (duration round-trip doubling the epoch rate limit), which is a real behavioral quirk but requires admin action to trigger and is partially documented in the NatSpec. The `min(oneToOne, oracle)` pricing logic is robust against oracle manipulation in all four depeg scenarios — this was the most important property to verify and it holds. The 6-layer rate limit with `unchecked` update blocks is safe because the validate functions use checked arithmetic that reverts on overflow before the update is reached.

The contract's main risk surface remains **admin trust** (compromised role holders can redirect custodians, change oracles, set limits to 0, rescue funds) and the **already-documented** `removeBenefactor`/`removeCollateral` mapping persistence issue (excluded from this report per instructions).

No PoC files were written because no Critical/High new bugs were found. The LOW/INFO findings do not warrant standalone PoC files per the task instructions.
