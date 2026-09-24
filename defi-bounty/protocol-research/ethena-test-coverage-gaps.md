# Ethena Test Coverage Gap Analysis

**Task ID:** eth-test-gaps
**Date:** 2024-09-24
**Analyst:** Opus (test-gap analysis agent)
**Scope:** Identify bugs by analyzing what Ethena's tests DON'T cover

---

## Executive Summary

| Metric | Value |
|--------|-------|
| Test files analyzed | 22 (USDtb: 9, USDe: 13) |
| Total test functions | 483 (USDtb: 234, USDe: 249) |
| PSM test files | **0** |
| PSM test functions | **0** |
| Invariant test suites | **0** |
| Fuzz test functions | ~25 (across both repos) |
| Coverage gaps identified | 64 |
| Potential bugs in untested areas | 7 |
| Critical bugs (untested) | 2 |

**Headline finding:** `PSM.sol` (2,082 lines, the newest and most complex contract in scope) has **ZERO test files and ZERO test functions**. Every function, every edge case, and every code path is completely untested. This is the single largest test coverage gap in the entire scope and the primary source of potential bugs.

---

## 1. Test File Inventory

### 1.1 USDtb Test Files (`/home/z/ethena-usdtb/test/foundry/`)

| File | Lines | Test Fns | Fuzz | Focus |
|------|-------|----------|------|-------|
| `test/USDtbMinting.core.t.sol` | 460 | 19 | 3 | Basic mint/redeem, custody ratios, unsupported assets, expired orders |
| `test/USDtbMinting.ACL.t.sol` | 748 | 59 | 11 | Role auth, admin transfer, gatekeeper powers, init config |
| `test/USDtbMinting.Delegate.t.sol` | 248 | 5 | 0 | Delegated signer mint/redeem success + failure |
| `test/USDtbMinting.blockLimits.t.sol` | 245 | 10 | 7 | Per-block and global mint/redeem limits |
| `test/USDtbMinting.Whitelist.t.sol` | 317 | 5 | 0 | Whitelisted benefactor mint/redeem, beneficiary approval |
| `test/USDtbMinting.StableRatios.t.sol` | 218 | 6 | 0 | Stables delta limit for mint/redeem |
| `test/USDtbMinting.SmartContractSigning.t.sol` | 91 | 1 | 0 | EIP-1271 smart contract signature |
| `USDtb.admin.t.sol` | 180 | 7 | 0 | Blacklist/whitelist role management |
| `USDtb.transfers.t.sol` | 1517 | 122 | 0 | Exhaustive blacklist×whitelist×transfer-state matrix |

### 1.2 USDe Test Files (`/home/z/ethena-usde/contracts/test/foundry/`)

| File | Lines | Test Fns | Fuzz | Focus |
|------|-------|----------|------|-------|
| `minting/tests/EthenaMinting.core.t.sol` | 457 | 19 | 3 | Basic mint/redeem, custody ratios, unsupported assets |
| `minting/tests/EthenaMinting.ACL.t.sol` | 694 | 57 | 11 | Role auth, admin transfer, gatekeeper powers |
| `minting/tests/EthenaMinting.Delegate.t.sol` | 242 | 5 | 0 | Delegated signer |
| `minting/tests/EthenaMinting.blockLimits.t.sol` | 197 | 9 | 6 | Per-block limits |
| `minting/tests/EthenaMinting.WETH.t.sol` | 598 | 23 | 1 | WETH mint/redeem, native ETH handling |
| `minting/tests/USDe.t.sol` | 175 | 15 | 0 | USDe token ownership, minter management |
| `staking/StakedUSDe.t.sol` | 510 | 21 | 3 | Staking, unstaking, rewards, rescue |
| `staking/StakedUSDe.ACL.t.sol` | 181 | 11 | 0 | Staking role auth |
| `staking/StakedUSDe.blacklist.t.sol` | 594 | 29 | 0 | Blacklist redistribution |
| `staking/StakedUSDeV2.cooldownEnabled.t.sol` | 626 | 27 | 5 | Cooldown assets/shares, unstake, duration setter |
| `staking/StakedUSDeV2.cooldownDisabled.t.sol` | 59 | 2 | 0 | Cooldown disabled reverts |
| `staking/StakedUSDeV2.blacklist.t.sol` | 558 | 28 | 0 | V2 blacklist + soft/full variants |
| `staking/EthenaBalancerRateProvider.t.sol` | 200 | 3 | 2 | Rate provider |

### 1.3 PSM Test Files

```
SEARCH: find /home/z -type f -iname "*psm*test*" -o -iname "*test*psm*"
RESULT: (none)

SEARCH: find /home/z/fkr-step1/defi-bounty -type f -name "*.t.sol"
RESULT: (none — no Solidity test files exist in the bounty project)
```

**PSM.sol has ZERO tests.** The contract is 2,082 lines with 36 external functions, 18 internal functions, complex dual-layer rate limiting, oracle validation, delegated signer logic, and a dual-price quote engine. None of it is tested.

---

## 2. PSM Coverage Matrix (CRITICAL — Zero Tests)

### 2.1 External Functions

| Function | Line | Tested? | Fuzzed? | Notes |
|----------|------|---------|---------|-------|
| `swap(Order)` | 268 | **NO** | **NO** | Main entry point — completely untested |
| `setAssetSendCustodian` | 345 | **NO** | **NO** | Custodian/benefactor conflict check untested |
| `setAssetReceiveCustodian` | 369 | **NO** | **NO** | Same conflict check untested |
| `setPegPrice` | 394 | **NO** | **NO** | Peg price update — bounds untested |
| `rescueFunds` | 414 | **NO** | **NO** | Token rescue — zero-amount check untested |
| `enableSwap` | 433 | **NO** | **NO** | |
| `disableSwap` | 445 | **NO** | **NO** | |
| `enableCollateral` | 458 | **NO** | **NO** | |
| `disableCollateral` | 481 | **NO** | **NO** | |
| `addCollateral` | 497 | **NO** | **NO** | Decimals mismatch check untested |
| `removeCollateral` | 528 | **NO** | **NO** | |
| `updateCollateralConfig` | 552 | **NO** | **NO** | Custodian rotation, isActive preservation untested |
| `enableBenefactor` | 589 | **NO** | **NO** | `_isCustodian` conflict check untested |
| `disableBenefactor` | 611 | **NO** | **NO** | |
| `addBenefactor` | 624 | **NO** | **NO** | |
| `removeBenefactor` | 645 | **NO** | **NO** | **BUG: `delete` doesn't clear mappings — see vuln file** |
| `setBenefactorMaxSwapForAssetPerEpoch` | 658 | **NO** | **NO** | |
| `setBenefactorMaxSwapForCollateralPerEpoch` | 680 | **NO** | **NO** | |
| `setBenefactorSwapForAssetFee` | 704 | **NO** | **NO** | MAX_FEE check untested |
| `setBenefactorSwapForCollateralFee` | 730 | **NO** | **NO** | |
| `setBenefactorZeroSwapForAssetFeeExemption` | 755 | **NO** | **NO** | |
| `setBenefactorZeroSwapForCollateralFeeExemption` | 778 | **NO** | **NO** | |
| `setBenefactorMaxSwapForAssetPerPeriod` | 800 | **NO** | **NO** | |
| `setBenefactorMaxSwapForCollateralPerPeriod` | 822 | **NO** | **NO** | |
| `setDelegatedSigner` | 849 | **NO** | **NO** | No active-benefactor check — untested |
| `confirmDelegatedSigner` | 868 | **NO** | **NO** | PENDING→ACCEPTED transition untested |
| `removeDelegatedSigner` | 887 | **NO** | **NO** | |
| `setApprovedBeneficiary` | 906 | **NO** | **NO** | No active-benefactor check — untested |
| `setGlobalEpochLimits` | 930 | **NO** | **NO** | |
| `setDefaultBenefactorMaxSwapForAssetPerEpoch` | 955 | **NO** | **NO** | Zero-check untested |
| `setDefaultBenefactorMaxSwapForCollateralPerEpoch` | 975 | **NO** | **NO** | |
| `setEpochDuration` | 997 | **NO** | **NO** | Duration change mid-epoch untested |
| `setGlobalPeriodLimits` | 1015 | **NO** | **NO** | |
| `setDefaultBenefactorMaxSwapForAssetPerPeriod` | 1040 | **NO** | **NO** | |
| `setDefaultBenefactorMaxSwapForCollateralPerPeriod` | 1060 | **NO** | **NO** | |
| `setPeriodDuration` | 1082 | **NO** | **NO** | Duration change mid-period untested |
| `getQuote` | 1108 | **NO** | **NO** | Non-view, emits events — untested |

### 2.2 Internal Functions (all untested)

| Function | Line | Risk if buggy |
|----------|------|---------------|
| `_isCustodian` | 1436 | Gas DoS with many collaterals; missed conflict |
| `_grantRoleToAddresses` | 1456 | Zero-address in array |
| `_validateOrder` | 1472 | Order expiry, chain ID, min amount bypass |
| `_validateBenefactor` | 1493 | Delegated signer bypass, beneficiary bypass |
| `_validateOraclePrice` | 1522 | Stale oracle, future timestamp, depeg bypass |
| `_getQuote` | 1571 | **Pricing math, overflow, fee bypass** |
| `_getCurrentEpoch` | 1638 | Epoch calculation error |
| `_getCurrentPeriod` | 1647 | Period calculation error |
| `_handleEpochPeriodOperations` | 1662 | **Rate limit bypass, unchecked overflow** |
| `_maybeRollEpoch` | 1844 | Epoch rollover failure |
| `_maybeRollPeriod` | 1858 | Period rollover failure |
| `_validateGlobalEpochLimits` | 1876 | Limit bypass via uint128 overflow revert |
| `_validateCollateralEpochLimits` | 1906 | Same |
| `_validateBenefactorEpochLimits` | 1939 | Same |
| `_validateGlobalPeriodLimits` | 1971 | Same |
| `_validateCollateralPeriodLimits` | 2001 | Same |
| `_validateBenefactorPeriodLimits` | 2034 | Same |
| `_validateCollateralConfig` | 2065 | Invalid config acceptance |

### 2.3 Untested PSM Edge Cases

| Edge Case | Function | Risk |
|-----------|----------|------|
| Zero amount swap | `swap` / `_validateOrder` | Reverts (BASIS_POINTS check) — but not verified |
| Max uint128 swap | `swap` / `_getQuote` | **Overflow in pricing math** — see §6.2 |
| Swap with self as beneficiary | `swap` / `_validateBenefactor` | Bypasses approval check — not verified |
| Swap with zero minAmountOut | `swap` / `_validateOrder` | Reverts — but not verified |
| Oracle returning 0 | `_validateOraclePrice` | Reverts — but not verified |
| Oracle returning max uint256 | `_validateOraclePrice` | Depeg check bypass — not verified |
| Oracle stale (updatedAt old) | `_validateOraclePrice` | Reverts — but not verified |
| Oracle future timestamp | `_validateOraclePrice` | Reverts — but not verified |
| Epoch boundary swap | `_handleEpochPeriodOperations` | **Limit reset behavior untested** |
| Period boundary swap | `_handleEpochPeriodOperations` | **Limit reset behavior untested** |
| Rate limit boundary (exact max) | `_validate*Limits` | Off-by-one not verified |
| Rate limit uint128 overflow | `_validate*Limits` | `currentTotal + amount` reverts — not verified |
| Blacklist during cooldown | N/A (PSM has no blacklist) | N/A |
| Delegate signer edge cases | `setDelegatedSigner` / `confirmDelegatedSigner` | **Remove+re-add retains ACCEPTED status — see vuln file** |
| Custodian = benefactor conflict | `setAssetSendCustodian` / `addCollateral` | Conflict check untested |
| `setEpochDuration` mid-epoch | `setEpochDuration` | **Epoch isolation via duration-keyed mapping untested** |
| `setPeriodDuration` mid-period | `setPeriodDuration` | Same |
| `rescueFunds` with zero amount | `rescueFunds` | Reverts — but not verified |
| `rescueFunds` rescuing asset token | `rescueFunds` | Can rescue the asset token itself — untested |
| `getQuote` as non-view (emits event) | `getQuote` | Event spam, can't be called statically — untested |

---

## 3. USDtbMinting Coverage Matrix

### 3.1 Functions × Test Status

| Function | Tested? | Fuzzed? | Gap |
|----------|---------|---------|-----|
| `mint` | YES | partial | Fee-on-transfer collateral not tested |
| `redeem` | YES | partial | Fee-on-transfer asset not tested |
| `setGlobalMaxMintPerBlock` | YES | NO | Zero-value setter not tested |
| `setGlobalMaxRedeemPerBlock` | YES | NO | Zero-value setter not tested |
| `disableMintRedeem` | YES | NO | — |
| `setDelegatedSigner` | YES | NO | — |
| `confirmDelegatedSigner` | YES | NO | — |
| `removeDelegatedSigner` | YES | NO | — |
| `transferToCustody` | YES | NO | — |
| `removeSupportedAsset` | YES | NO | — |
| `removeCustodianAddress` | YES | NO | — |
| `removeMinterRole` | YES | NO | — |
| `removeRedeemerRole` | YES | NO | — |
| `removeCollateralManagerRole` | YES | NO | — |
| `removeWhitelistedBenefactor` | YES | NO | — |
| `addCustodianAddress` | YES | NO | — |
| `addWhitelistedBenefactor` | YES | NO | — |
| `setApprovedBeneficiary` | YES | NO | — |
| `addSupportedAsset` | YES | NO | — |
| `setMaxMintPerBlock` | YES | YES | — |
| `setMaxRedeemPerBlock` | YES | YES | — |
| `setTokenType` | **NO** | **NO** | **Untested admin function** |
| `setStablesDeltaLimit` | **NO** | **NO** | **Untested — no upper bound** |
| `setUSDtbToken` | **NO** | **NO** | **Untested — no zero-address check** |
| `verifyOrder` | YES | NO | — |
| `verifyRoute` | YES | NO | — |
| `verifyNonce` | YES | NO | Max nonce (uint128) edge case not fuzzed |
| `verifyStablesLimit` | YES | NO | Divide-by-zero if usdtbAmount=0 (guarded upstream) |
| `_transferCollateral` | YES | NO | **Fee-on-transfer tokens not tested** |
| `_transferToBeneficiary` | YES | NO | Native ETH reentrancy via `call{value}` not tested |

### 3.2 Untested USDtbMinting Edge Cases

| Edge Case | Risk |
|-----------|------|
| Fee-on-transfer collateral token | `_transferCollateral` assumes full amount received; protocol mints full USDtb for less collateral |
| Fee-on-transfer on redeem | `_transferToBeneficiary` uses `safeTransfer` (correct for fees) but doesn't verify received |
| Rebasing token as collateral | Balance changes mid-transfer could break accounting |
| `setTokenType` with invalid enum | No validation beyond `isActive` check |
| `setStablesDeltaLimit(type(uint128).max)` | Effectively disables stables limit — no upper bound |
| `setUSDtbToken(address(0))` | **Bricks mint/redeem — no zero-address check** |
| Nonce = max uint128 | `uint64(nonce) >> 8` truncation — bitmap slot collision |
| Block boundary (block.number overflow) | `uint128(block.number)` cast in `belowGlobalMaxMintPerBlock` |
| `disableMintRedeem` then re-enable | Sets globals to 0; re-enable requires explicit setGlobalMax* call |
| Custody ratio with 0 addresses | `verifyRoute` returns false for empty — verified? |
| Native ETH rescue | `rescueTokens` doesn't handle NATIVE_TOKEN |

---

## 4. USDtb Token Coverage

| Function | Tested? | Gap |
|----------|---------|-----|
| `initialize` | YES | — |
| `addMinter` | YES | — |
| `removeMinter` | YES | — |
| `addBlacklistAddress` | YES | — |
| `removeBlacklistAddress` | YES | — |
| `addWhitelistAddress` | YES | — |
| `removeWhitelistAddress` | YES | — |
| `redistributeLockedAmount` | YES | — |
| `rescueTokens` | YES | — |
| `mint` | YES | — |
| `renounceRole` | YES | — |
| `updateTransferState` | YES | — |
| `_beforeTokenTransfer` | YES (122 transfer tests) | Exhaustive matrix |

**USDtb token is well-tested.** The 122-function transfer matrix covers all blacklist×whitelist×transfer-state combinations. Main gap: no fuzz tests on transfer amounts.

---

## 5. Cross-Contract / Integration Test Gaps

| Scenario | Tested? | Risk |
|----------|---------|------|
| PSM ↔ asset token integration | **NO** (no PSM tests) | All PSM integration untested |
| PSM ↔ oracle feed integration | **NO** | Stale oracle, depeg, feed upgrade |
| PSM ↔ custodian wallet interaction | **NO** | Approval race, insufficient allowance |
| PSM swap with reentrancy via malicious collateral | **NO** | ERC-777/fee-on-transfer reentrancy |
| USDtbMinting ↔ USDtb token burn in redeem | partial | Burn-then-transfer ordering |
| StakedUSDeV2 cooldown → unstake with admin duration change mid-cooldown | **NO** | User locked funds if duration increased |
| StakedUSDeV2 `unstake` with `cooldownDuration == 0` and existing silo funds | **NO** | Escape hatch untested |
| EthenaMinting WETH → native ETH round trip | YES | — |
| Multi-collateral PSM concurrent swaps | **NO** | Cross-collateral rate limit interaction |
| PSM `getQuote` matching actual `swap` output | **NO** | Quote/execution divergence |

---

## 6. Fuzz Test Coverage Gaps

### 6.1 Existing Fuzz Tests

| Contract | Fuzz Functions | What's Fuzzed |
|----------|---------------|---------------|
| USDtbMinting.core | 3 | Mint slippage, custody ratios |
| USDtbMinting.ACL | 11 | Non-minter/non-admin addresses |
| USDtbMinting.blockLimits | 7 | Per-block mint/redeem amounts, setters |
| EthenaMinting.core | 3 | Same as USDtb |
| EthenaMinting.ACL | 11 | Same as USDtb |
| EthenaMinting.blockLimits | 6 | Same as USDtb |
| EthenaMinting.WETH | 1 | WETH mint amount |
| StakedUSDe | 3 | Vested balance, fair prices, mint slippage |
| StakedUSDeV2.cooldown | 5 | Cooldown assets/shares, duration setter |

### 6.2 Critical Missing Fuzz Tests

| What Should Be Fuzzed | Contract | Why Critical |
|----------------------|----------|--------------|
| `swap` with random amounts, prices, fees | PSM | **ZERO fuzz on main swap path** |
| `_getQuote` pricing math with extreme inputs | PSM | **Overflow in `netAmountIn * pegPrice * 10^decimals`** |
| Oracle price vs pegPrice interaction | PSM | Fee bypass when oracle near peg |
| Rate limit boundary with random amounts | PSM | Off-by-one in `currentTotal + amount > max` |
| Epoch/period rollover with random timestamps | PSM | Limit reset correctness |
| `verifyStablesLimit` with random amounts/decimals | USDtbMinting | Divide-by-zero, overflow in scaling |
| Nonce bitmap with random nonces | USDtbMinting | Slot collision, bit overflow |
| `cooldownAssets` called multiple times | StakedUSDeV2 | `underlyingAmount += uint152(assets)` truncation |
| `unstake` after duration change | StakedUSDeV2 | Cooldown end vs current duration |

### 6.3 Missing Invariant Tests

**Zero invariant test suites exist in either repo.** Critical missing invariants:

| Invariant | Contract | Description |
|-----------|----------|-------------|
| Conservation of funds | PSM | `assetOut + fee <= assetIn` for any swap |
| Rate limit monotonicity | PSM | `swappedInEpoch` never exceeds `max` |
| Nonce uniqueness | PSM / USDtbMinting | Each nonce used at most once per benefactor |
| Epoch total ≤ global max | PSM | After any swap, `globalSwapped <= maxSwapPerEpoch` |
| Benefactor can't exceed personal limit | PSM | `benefactorSwapped <= benefactorMax` |
| Quote ≥ actual swap output | PSM | `getQuote` never overestimates |
| USDtb supply = collateral held | USDtbMinting | Total USDtb ≤ total collateral received |
| StakedUSDe share price monotonic | StakedUSDeV2 | Rate never decreases (except donation) |

---

## 7. Potential Bugs in Untested Areas

### 7.1 PSM `removeBenefactor` — Mapping Persistence Bug (CRITICAL)

**File:** `/home/z/fkr-step1/defi-bounty/vuln/ethena-untested-removebenefactor-mapping-persistence.md`

`removeBenefactor` uses `delete benefactorState[benefactor].config`, but `BenefactorConfig` contains 6 mappings that Solidity `delete` cannot clear. After remove + re-add, old delegated signers (ACCEPTED status), approved beneficiaries, fee exemptions, and custom fees silently persist. A previously-accepted delegated signer can resume swapping immediately without re-confirmation.

**Severity:** High (privilege persistence after revocation)
**Test coverage:** Zero — no PSM tests exist

### 7.2 PSM `_getQuote` — Overflow DoS with High Peg Price (MEDIUM)

For `swapForCollateral`, the computation `netAmountIn * pegPrice * (10 ** collateralDecimals)` can overflow `uint256` when `pegPrice` is near `MAX_PEG_PRICE` (1000e18) and `amountIn` is near `uint128` max. With 18-decimal collateral and pegPrice = 1000e18:
- `3.4e38 (uint128 max) * 1e21 * 1e18 = 3.4e77 > 1.15e77 (uint256 max)` → **revert**

This limits the maximum swap size and causes large swaps to revert. The code comment acknowledges decimals > 18 causes overflow, but normal 18-decimal tokens with high pegPrice also overflow.

**Severity:** Medium (DoS, no fund loss)
**Test coverage:** Zero — `_getQuote` is never fuzzed

### 7.3 PSM `_validate*Limits` — uint128 Addition Overflow (LOW)

All six `_validate*Limits` functions compute `currentTotal + amount` where both are `uint128`. In Solidity 0.8+, this reverts on overflow (Panic 0x11) instead of returning the custom "limit exceeded" error. If `currentTotal` + `amount` overflows uint128, the swap reverts with an uninformative panic rather than the intended error.

**Severity:** Low (swap still rejected, just wrong error)
**Test coverage:** Zero

### 7.4 PSM `getQuote` — Non-View Function Emits Events (LOW/MEDIUM)

`getQuote` is declared `nonReentrant` (not `view`) and calls `_validateOraclePrice` which emits `OraclePriceValidated`. This means:
- Off-chain integrators cannot call it via `eth_call` (staticcall) when oracle is depegged
- It costs gas and emits events on every call
- Can be used for event-spam griefing

**Severity:** Low/Medium (design issue, integration breakage)
**Test coverage:** Zero

### 7.5 PSM `setDelegatedSigner` / `setApprovedBeneficiary` — No Active-Benefactor Check (MEDIUM)

Both functions allow ANY address to call them, even if the caller is not an active benefactor. A non-benefactor can pre-set delegated signers (PENDING) and approved beneficiaries in their (inactive) config. Combined with bug 7.1, if they are later added as a benefactor, these pre-set permissions activate without fresh approval.

**Severity:** Medium (combined with 7.1)
**Test coverage:** Zero

### 7.6 USDtbMinting `_transferCollateral` — Fee-on-Transfer Token Handling (MEDIUM)

`_transferCollateral` calls `token.safeTransferFrom(benefactor, addresses[i], amountToTransfer)` and accumulates `totalTransferred += amountToTransfer` (the REQUESTED amount, not the ACTUAL received). With a fee-on-transfer token, the custodian receives less than `amountToTransfer`, but the protocol mints the full `order.usdtb_amount`. This lets a malicious fee-on-transfer collateral token extract more USDtb than the collateral received.

**Severity:** Medium (depends on admin adding such a token — `TokenType` field exists but is only checked for STABLE ratio, not fee-on-transfer)
**Test coverage:** Zero — no fee-on-transfer mock token in tests

### 7.7 StakedUSDeV2 `cooldownAssets` — uint152 Truncation (LOW)

`cooldowns[msg.sender].underlyingAmount += uint152(assets)` casts `uint256 assets` to `uint152` without `SafeCast`. If `assets > type(uint152).max` (~5.7e45), the value silently truncates, causing the user to receive less than expected on `unstake`. Practically unreachable with current USDe supply (~1e27) but violates safe-coding standards.

**Severity:** Low (practically unreachable, but unsafe cast)
**Test coverage:** Fuzz test bounds `amount` to `1e40`, below the truncation threshold

---

## 8. Untested Admin Functions Summary

### PSM (all untested — 36 functions)

Every single admin function in PSM is untested. The most critical untested admin functions:

| Function | What Could Go Wrong |
|----------|---------------------|
| `removeBenefactor` | **Mapping persistence — delegated signers survive removal** |
| `updateCollateralConfig` | Custodian rotation race, isActive preservation |
| `setEpochDuration` / `setPeriodDuration` | Mid-epoch duration change isolates state (untested) |
| `setPegPrice` | Price bounds, economic impact |
| `rescueFunds` | Can rescue the asset token itself (untested) |
| `addCollateral` | Decimals mismatch, custodian-benefactor conflict |
| `setDelegatedSigner` | No active-benefactor requirement |
| `setApprovedBeneficiary` | No active-benefactor requirement |

### USDtbMinting (3 untested)

| Function | Risk |
|----------|------|
| `setTokenType` | No enum validation |
| `setStablesDeltaLimit` | No upper bound — can disable limit |
| `setUSDtbToken` | No zero-address check — can brick contract |

---

## 9. Recommendations

1. **Write PSM tests immediately.** This is the highest priority. Start with:
   - Constructor validation tests (all revert paths)
   - `swap` happy path for both directions
   - Oracle validation (stale, future, zero, depeg)
   - Rate limit boundary tests (epoch + period, global + collateral + benefactor)
   - `removeBenefactor` → `addBenefactor` mapping persistence test
   - Fuzz test for `_getQuote` with extreme inputs

2. **Add invariant tests** for fund conservation and rate limit monotonicity.

3. **Add fee-on-transfer token tests** for USDtbMinting `_transferCollateral`.

4. **Fix `removeBenefactor`** to explicitly clear mappings or document that removal does NOT clear permissions.

5. **Add zero-address check** to `setUSDtbToken`.

6. **Add upper bound** to `setStablesDeltaLimit`.

7. **Make `getQuote` a view function** (split event emission into `swap` only).

---

## 10. File Paths

- Analysis: `/home/z/fkr-step1/defi-bounty/protocol-research/ethena-test-coverage-gaps.md`
- Vuln (removeBenefactor): `/home/z/fkr-step1/defi-bounty/vuln/ethena-untested-removebenefactor-mapping-persistence.md`
- Vuln (getQuote overflow): `/home/z/fkr-step1/defi-bounty/vuln/ethena-untested-getquote-overflow.md`
- PSM contract: `/home/z/fkr-step1/defi-bounty/contracts/PSM.sol`
- USDtbMinting contract: `/home/z/ethena-usdtb/contracts/usdtb/USDtbMinting.sol`
- USDtb tests: `/home/z/ethena-usdtb/test/foundry/`
- USDe tests: `/home/z/ethena-usde/contracts/test/foundry/`
