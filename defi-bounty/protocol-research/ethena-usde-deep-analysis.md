# Ethena USDe Contracts — Deep Security Analysis

**Analyst:** Opus (DeFi security researcher)
**Date:** 2024-09-24
**Scope:** 6 contracts in `/home/z/ethena-usde/contracts/contracts/`
**Comparison baseline:** `/home/z/ethena-usdtb/contracts/usdtb/USDtbMinting.sol` (already analyzed, no critical found)
**OpenZeppelin version:** 4.9.5 (includes virtual shares/assets inflation-attack mitigation)
**Solidity:** 0.8.20

---

## Executive Summary

All 6 contracts were read line-by-line. **No CRITICAL vulnerability (theft of unrelated user funds, permanent freezing of protocol funds by an external attacker) was found.** The contracts are well-architected: every state-changing function uses `nonReentrant`, the Checks-Effects-Interactions pattern is respected, rounding consistently favors the vault, role-based access control is granular, and the ERC4626 inflation attack is mitigated by both OZ v4.9 virtual shares AND Ethena's own `_checkMinShares`.

The most notable finding is a **Medium** design gap in `StakedUSDeV2.unstake`: a user who enters the cooldown flow *before* being blacklisted can still claim their silo'd USDe after the cooldown expires, because `unstake` does not consult the `FULL_RESTRICTED_STAKER_ROLE`. The admin's `redistributeLockedAmount` confiscation tool only operates on live sUSDe balances (which are already burned once a cooldown begins), so it cannot reach funds that have already moved to the silo. This is a compliance/blacklist-evasion gap, not an arbitrary-fund-theft bug, and it requires the user to act before being flagged.

Lower-severity observations (Low / Informational) are documented per-contract below. Where a USDtb equivalent exists, the differences are called out.

---

## Contract 1: `EthenaMinting.sol` (551 lines)

### Purpose
Off-chain-RFQ mint/redeem router for USDe. A benefactor signs an EIP-712 `Order`; a `MINTER_ROLE`/`REDEEMER_ROLE` holder submits it. Mint pulls collateral from the benefactor to custodian addresses per a `Route`; redeem burns USDe from the benefactor and sends collateral to the beneficiary. Also supports `mintWETH` (unwrap WETH → ETH to custodians) and `transferToCustody` (sweep).

### Line-by-line analysis

| Lines | Area | Notes |
|------|------|-------|
| 21-66 | Constants | EIP-712 domain, order/route typehashes, role constants, `NATIVE_TOKEN` sentinel, `ROUTE_REQUIRED_RATIO = 10_000`. ✅ |
| 66-99 | State | `usde` immutable, `_supportedAssets` / `_custodianAddresses` EnumerableSets, `_orderBitmaps` nonce dedup, per-block mint/redeem caps, `delegatedSigner` mapping. ✅ |
| 105-115 | Modifiers | `belowMaxMintPerBlock` / `belowMaxRedeemPerBlock` check before state mutation. ✅ |
| 119-163 | Constructor | Validates non-zero addresses; grants admin to `msg.sender` then to `_admin` (SingleAdminAccessControl revokes the previous admin atomically, so no dual-admin window). Calls `addSupportedAsset`/`addCustodianAddress` while `msg.sender` is briefly admin — fine, constructor is atomic. ✅ |
| 170-172 | `receive()` | Accepts arbitrary ETH, only emits. See **Finding EM-3** (stuck-ETH). |
| 179-204 | `mint` | `nonReentrant` + `onlyRole(MINTER_ROLE)` + `belowMaxMintPerBlock`. Order: verifyOrder → verifyRoute → deduplicate → bump `mintedPerBlock` → `_transferCollateral` → `usde.mint`. CEI ok (state bumps before external transfer, but `nonReentrant` + trusted USDe make this safe). ✅ |
| 211-236 | `mintWETH` | Same shape as `mint` but calls `_transferEthCollateral`. ✅ |
| 243-265 | `redeem` | `nonReentrant` + `onlyRole(REDEEMER_ROLE)` + `belowMaxRedeemPerBlock`. Order: verifyOrder → deduplicate → bump `redeemedPerBlock` → `usde.burnFrom` → `_transferToBeneficiary`. Burn-before-transfer is safe because the whole tx reverts if `_transferToBeneficiary` fails (nonce dedup also reverts). ✅ |
| 268-281 | `setMaxMintPerBlock` / `setMaxRedeemPerBlock` / `disableMintRedeem` | Admin / Gatekeeper gated. `disableMintRedeem` sets both to 0. ✅ |
| 284-302 | Delegated signer | 2-step (PENDING → ACCEPTED). `removeDelegatedSigner` sets REJECTED unconditionally (harmless). ✅ |
| 305-318 | `transferToCustody` | `nonReentrant` + `COLLATERAL_MANAGER_ROLE`. Custodian must be in set. Native ETH via `.call`. ✅ |
| 321-353 | Asset/custodian/role removals | Admin / Gatekeeper gated. ✅ |
| 358-371 | `addSupportedAsset` / `addCustodianAddress` | Admin gated; rejects 0, `usde`, duplicates. Note: does NOT reject `NATIVE_TOKEN` — see EM-4. ✅ (intentional) |
| 376-400 | EIP-712 helpers | Caches domain separator; recomputes if `chainId` changed (fork protection). ✅ |
| 403-418 | `verifyOrder` | ECDSA recover; signer must be `benefactor` OR an ACCEPTED delegated signer. Rejects 0 beneficiary, 0 amounts, expired. **No whitelist of benefactors** — see EM-1. |
| 421-440 | `verifyRoute` | Addresses/ratios equal length, non-empty, each address is a custodian & non-zero & ratio>0, sum == 10_000. ✅ |
| 443-451 | `verifyNonce` | Bitmap: `slot = uint64(nonce) >> 8`, `bit = 1 << uint8(nonce)`. See EM-5 (truncation). |
| 456-459 | `_deduplicateOrder` | Check-then-set atomic (no external calls between). ✅ |
| 464-473 | `_transferToBeneficiary` | Native: balance check then `.call`. Non-native: `supportedAssets` check then `safeTransfer`. ✅ |
| 476-499 | `_transferCollateral` | Rejects NATIVE_TOKEN (can't mint with raw ETH). Per-custodian `(amount * ratios[i]) / 10_000` (rounds down); leftover dust goes to last custodian. Total pulled == `amount`. ✅ |
| 502-530 | `_transferEthCollateral` | Requires `asset == WETH`. Pulls WETH, `WETH.withdraw`, sends ETH to custodians. Total sent == `amount`; no ETH left behind. If any `.call` fails, reverts (atomic). ✅ |
| 533-550 | Internal setters / domain | ✅ |

### Suspicious patterns / Edge cases tested

**EM-1 — No benefactor whitelist (Informational, defense-in-depth gap vs USDtb)**
`verifyOrder` accepts any address as `order.benefactor` provided the signature is valid. `USDtbMinting` added `_whitelistedBenefactors.contains(order.benefactor)` and a per-benefactor approved-beneficiary set. In EthenaMinting a MINTER_ROLE holder could submit a phished/leaked signature from any address that approved the contract. Not a code bug — the signature authorizes the action — but the newer contract is strictly safer.

**EM-2 — No on-chain price check (Informational)**
Neither `collateral_amount` vs `usde_amount` consistency nor a stable-price delta is enforced (USDtb added `verifyStablesLimit`). Minting is fully trusted to the off-chain RFQ + MINTER_ROLE. This is the documented design; not a bug.

**EM-3 — Donated ETH is reclaimable only via redeem/transferToCustody (Informational)**
`receive()` accepts arbitrary ETH. The balance can leave only through `_transferToBeneficiary` (NATIVE_TOKEN redeem, signed by a benefactor burning USDe) or `transferToCustody` (COLLATERAL_MANAGER_ROLE). An attacker donating ETH cannot profit: any redeem consumes signed USDe burn. No theft vector; only a stuck-funds accounting note.

**EM-4 — NATIVE_TOKEN can be added as a "supported asset" (Informational)**
`addSupportedAsset` does not reject `NATIVE_TOKEN`. If added, NATIVE_TOKEN is valid for `redeem` (sends ETH to beneficiary) but still rejected for `mint` (`_transferCollateral` reverts on NATIVE_TOKEN). This asymmetry is intentional: it lets the protocol redeem in ETH. No exploit — the admin controls `addSupportedAsset`.

**EM-5 — Nonce truncation to uint64 (Informational)**
`verifyNonce` casts `uint256 nonce` → `uint64(nonce)` before computing slot/bit. Nonces `n` and `n + 2^64` collide to the same bitmap bit. This shrinks the effective nonce space and could cause a benefactor's later order to be rejected as "already used" if they unknowingly reuse a truncated value. Not a replay vector (signatures are bound to the exact nonce), purely a UX/footgun. USDtbMinting has the identical `uint64(nonce)` cast (not a regression fix).

**EM-6 — Reentrancy surface during ETH callbacks (tested, safe)**
`_transferToBeneficiary` (NATIVE) and `_transferEthCollateral` perform `.call{value:...}("")` to beneficiary/custodian addresses. `redeem`/`mint`/`mintWETH`/`transferToCustody` are all `nonReentrant`. The only non-`nonReentrant` external-ish entrypoints reachable during such a callback are `setDelegatedSigner`/`confirmDelegatedSigner`/`removeDelegatedSigner` (no token movement) and view functions. No state an attacker can corrupt to profit. ✅

**EM-7 — `_transferCollateral` rounding (safe)**
`amountToTransfer = (amount * ratios[i]) / 10_000` rounds down per custodian; the cumulative `totalTransferred` may be < `amount` by up to `(addresses.length - 1)` wei. The `remainingBalance` is sent to the last custodian, so the protocol always pulls exactly `amount` from the benefactor. No dust is lost to the contract. USDtb has the identical pattern. ✅

### Comparison with USDtbMinting
USDtb added: whitelisted benefactors + approved beneficiaries (EM-1), EIP-1271 smart-contract signatures, per-asset + global per-block caps, `verifyStablesLimit` stable-price delta (EM-2), and downgraded order/amount types to `uint128`. USDtb **removed** `mintWETH`. None of the missing features in EthenaMinting constitute an exploitable critical bug; they are defense-in-depth layers.

### Severity
- EM-1 … EM-7: **Informational**. No critical/high/medium in EthenaMinting.

---

## Contract 2: `StakedUSDe.sol` (268 lines) — parent of V2

### Purpose
ERC4626 vault for USDe → sUSDe. Rewards are streamed in by a `REWARDER_ROLE` and vested linearly over 8 hours (`VESTING_PERIOD`). Provides soft/full blacklist roles and an admin `redistributeLockedAmount` confiscation tool.

### Line-by-line analysis

| Lines | Area | Notes |
|------|------|-------|
| 19-34 | Inheritance & constants | `SingleAdminAccessControl, ReentrancyGuard, ERC20Permit, ERC4626`. `MIN_SHARES = 1 ether` (1e18) anti-donation floor. `VESTING_PERIOD = 8 hours`. ✅ |
| 40-43 | `vestingAmount` / `lastDistributionTimestamp` | Drives `getUnvestedAmount()`. ✅ |
| 48-57 | Modifiers | `notZero`, `notOwner` (can't blacklist the admin). ✅ |
| 68-79 | Constructor | Validates non-zero; grants REWARDER_ROLE + admin. No `msg.sender` admin grant (unlike EthenaMinting) — fine, constructor makes no role-gated calls. ✅ |
| 87-93 | `transferInRewards` | `nonReentrant` + REWARDER_ROLE + `notZero`. Order: `_updateVestingAmount(amount)` (state) → `safeTransferFrom` (pull). Reverts if StillVesting. USDe is a plain ERC20 (no callback) so the external pull cannot reenter. ✅ |
| 100-117 | Blacklist add/remove | BLACKLIST_MANAGER_ROLE; `addToBlacklist` blocked on owner. ✅ |
| 128-131 | `rescueTokens` | `nonReentrant` + admin; rejects `asset()` (USDe) so stakers' USDe cannot be drained. sUSDe (the share token) CAN be rescued (intentional — accidental transfers). ✅ |
| 138-154 | `redistributeLockedAmount` | Admin-only. Burns `from`'s full sUSDe balance; if `to == address(0)` calls `_updateVestingAmount(usdeToVest)` to re-vest the confiscated USDe to remaining stakers over 8h; else mints the same share amount to `to`. Requires `from` FULL_RESTRICTED and `to` NOT FULL_RESTRICTED. See **SUSDe-2**. |
| 161-163 | `totalAssets` | `balanceOf(this) - getUnvestedAmount()`. Could revert (underflow) only if `getUnvestedAmount() > balance` — see **SUSDe-1**. |
| 168-180 | `getUnvestedAmount` | Linear decay over 8h: `deltaT * vestingAmount / VESTING_PERIOD`. `unchecked` on `deltaT` is safe (guarded by the `>= VESTING_PERIOD` early return). ✅ |
| 183-185 | `decimals` | Resolves ERC20/ERC4626 clash → 18. ✅ |
| 190-193 | `_checkMinShares` | Reverts if `0 < totalSupply < 1e18`. Called after every deposit/withdraw. Anti-donation floor. See **SUSDe-3**. |
| 202-214 | `_deposit` override | `nonReentrant`, `notZero`, blocks SOFT_RESTRICTED on caller/receiver, calls super, then `_checkMinShares`. ✅ |
| 224-240 | `_withdraw` override | `nonReentrant`, `notZero`, blocks FULL_RESTRICTED on caller/receiver/owner, calls super, then `_checkMinShares`. ✅ |
| 242-247 | `_updateVestingAmount` | Reverts if `getUnvestedAmount() > 0` (StillVesting); then REPLACES (not adds) `vestingAmount`. See **SUSDe-2** detail. |
| 253-260 | `_beforeTokenTransfer` | Blocks any transfer/mint where `from` or `to` is FULL_RESTRICTED (except burn-to-0 from a restricted address is allowed for the redistribution path). ✅ |
| 265-267 | `renounceRole` | Disabled. ✅ |

### Suspicious patterns / Edge cases tested

**SUSDe-1 — `totalAssets()` underflow if `getUnvestedAmount()` exceeds balance (Informational, trusted-only)**
`totalAssets = balance - getUnvestedAmount()` reverts on underflow in 0.8.x. This could brick ERC4626 view functions IF `vestingAmount` were ever set higher than the actual USDe balance. The only setters are `transferInRewards` (REWARDER_ROLE, pulls `amount` USDe in the same call, so balance ≥ vestingAmount) and `redistributeLockedAmount` with `to == address(0)` (admin-only; sets `vestingAmount = previewRedeem(burnedShares)` which is bounded by the share-backed USDe already in the contract). Both paths keep `balance ≥ vestingAmount`. No untrusted path can violate this. ✅

**SUSDe-2 — `redistributeLockedAmount` replaces (not adds) `vestingAmount` (Informational)**
When `to == address(0)`, `_updateVestingAmount(usdeToVest)` overwrites the prior `vestingAmount`. The `StillVesting` guard ensures we only overwrite when the prior amount has fully vested (so nothing is lost). Math verified: post-redistribution exchange rate `(A - usdeToVest) / (S - shares)` equals the pre-redistribution rate `A / S`, then drifts up as `usdeToVest` vests — exactly the intended "confiscate gradually to remaining stakers" behavior. ✅

**SUSDe-3 — `_checkMinShares` griefing (Low)**
If `totalSupply` is in `(0, 1e18)` after a withdrawal, the withdrawal reverts. An attacker holding most of the supply can withdraw down to just above `1e18`, leaving a small depositor unable to fully exit (their final partial withdrawal would drop `totalSupply` below `1e18`). Funds are not stolen — only temporarily locked until new deposits push `totalSupply` back above the floor. This is a known limitation of the min-shares pattern and is acceptable for a vault with continuous reward inflows. Severity **Low**.

**SUSDe-4 — Donation / inflation attack (safe)**
Verified non-profitable: with OZ v4.9.5 virtual shares (`totalSupply + 1`, `totalAssets + 1`) plus `_checkMinShares` plus `notZero(shares)`, an attacker depositing then donating breaks even on round-trip (the donation is shared pro-rata with all stakers, including virtual shares). The `notZero(shares)` guard prevents the zero-share theft variant. No profitable attack path found. ✅

**SUSDe-5 — Flash-loan exchange-rate manipulation (safe)**
`getUnvestedAmount()` depends only on `vestingAmount` + `lastDistributionTimestamp`, both writable solely by REWARDER_ROLE/admin. A flash-loan depositor cannot move the vesting state, and deposit/withdraw round-trips lose the rounding dust to the vault. No profitable flash-loan path. ✅

### Comparison with USDtb equivalent
There is no USDtb staking vault in the analyzed set; USDtb is a flat RWA stablecoin. No regression comparison possible.

### Severity
- SUSDe-3: **Low** (min-shares griefing, funds not stolen).
- SUSDe-1, SUSDe-2, SUSDe-4, SUSDe-5: **Informational**.

---

## Contract 3: `StakedUSDeV2.sol` (131 lines)

### Purpose
Adds a **cooldown** unstaking flow on top of `StakedUSDe`. When `cooldownDuration > 0`, the ERC4626 `withdraw`/`redeem` are disabled; users instead call `cooldownAssets`/`cooldownShares` to move USDe into a `USDeSilo` and start a 90-day (max) cooldown, then `unstake` to claim. When `cooldownDuration == 0`, standard ERC4626 withdraw/redeem is re-enabled and `unstake` remains callable to drain residual silo balances.

### Line-by-line analysis

| Lines | Area | Notes |
|------|------|-------|
| 17-26 | State | `cooldowns` mapping, immutable `silo`, `MAX_COOLDOWN_DURATION = 90 days`, `cooldownDuration`. ✅ |
| 29-38 | Modifiers | `ensureCooldownOff` / `ensureCooldownOn`. ✅ |
| 44-47 | Constructor | Deploys `USDeSilo(address(this), asset)`; sets `cooldownDuration = MAX_COOLDOWN_DURATION`. Silo's `_STAKING_VAULT` is immutable → bound to this contract forever. ✅ |
| 54-62 | `withdraw` override | `ensureCooldownOff` then `super.withdraw`. ✅ |
| 67-75 | `redeem` override | `ensureCooldownOff` then `super.redeem`. ✅ |
| 80-92 | `unstake` | Reads `underlyingAmount`; if `block.timestamp >= cooldownEnd` OR `cooldownDuration == 0`, zeros state then `silo.withdraw(receiver, assets)`. **No `nonReentrant`. No blacklist check.** See **V2-1**, **V2-2**. |
| 96-105 | `cooldownAssets` | `ensureCooldownOn`; `assets <= maxWithdraw(msg.sender)`; `shares = previewWithdraw(assets)` (rounds UP, conservative); sets `cooldownEnd` + `underlyingAmount`; calls `_withdraw(msg.sender, silo, msg.sender, assets, shares)`. CEI ok (state before external). `_withdraw` is `nonReentrant` + blacklist-checked. ✅ |
| 109-118 | `cooldownShares` | `ensureCooldownOn`; `shares <= maxRedeem(msg.sender)`; `assets = previewRedeem(shares)` (rounds DOWN, conservative); sets `cooldownEnd` + `underlyingAmount`; calls `_withdraw`. ✅ |
| 122-130 | `setCooldownDuration` | Admin-only; capped at `MAX_COOLDOWN_DURATION`. ✅ |

### Suspicious patterns / Edge cases tested

**V2-1 — `unstake` has no `nonReentrant` and no blacklist check (Medium — compliance gap)**
This is the most material finding in the set. Two sub-issues:

1. **Missing `nonReentrant` (safe in practice).** `unstake` calls `silo.withdraw` which calls `_USDE.transfer(receiver, assets)`. USDe is a plain OZ ERC20 with no hooks (no ERC777 `tokensReceived`, no callback), so the transfer cannot reenter. State (`underlyingAmount`, `cooldownEnd`) is zeroed BEFORE the external call (CEI), and a reentrant `unstake` would see `underlyingAmount == 0` and `cooldownEnd == 0` → condition `block.timestamp >= 0` is true → `silo.withdraw(receiver, 0)` is a no-op. So even a hypothetical callback is harmless. The missing modifier is a style/defense-in-depth gap, NOT an exploitable reentrancy. **No fund-theft vector.**

2. **Missing `FULL_RESTRICTED_STAKER_ROLE` check (Medium — blacklist evasion).** The parent's `_withdraw` (used by `cooldownAssets`/`cooldownShares`) blocks restricted addresses, and `_beforeTokenTransfer` blocks restricted sUSDe transfers. BUT once a user has entered cooldown, their sUSDe is already burned and their USDe sits in the silo. `unstake` does NOT consult the blacklist, so a user who started a cooldown BEFORE being blacklisted can still claim their silo'd USDe after the 90-day cooldown — even while fully blacklisted. The admin's `redistributeLockedAmount` confiscation tool only burns *live* sUSDe; after a cooldown there is nothing left to burn (`balanceOf(user) == 0`), so it cannot reach silo funds. Setting `cooldownDuration = 0` does not help either: `unstake` remains callable (the `|| cooldownDuration == 0` branch) and still skips the blacklist check.

   **Impact:** A user flagged for sanctions can "pre-flight" a cooldown and escape with their own (legitimately staked) USDe 90 days later, defeating the compliance freeze. This does **not** let an attacker steal *other users'* funds — the silo balance always equals the sum of `underlyingAmount` claims (verified: every `cooldownAssets`/`cooldownShares` adds exactly `assets` to both the silo and the claim; every `unstake` subtracts exactly `assets` from both). It is a blacklist-evasion / compliance gap, severity **Medium**. Whether it qualifies for an Immunefi payout depends on the program's scope (compliance vs. fund-theft).

   **Remediation:** Add `if (hasRole(FULL_RESTRICTED_STAKER_ROLE, msg.sender)) revert OperationNotAllowed();` to `unstake`, OR give the admin a path to sweep restricted users' silo balances (e.g. a `redistributeSiloCooldown` admin function that burns the `underlyingAmount` and vests the USDe to remaining stakers via `_updateVestingAmount`).

**V2-2 — `cooldownAssets` / `cooldownShares` reset `cooldownEnd` on each call (Informational, by design)**
Every call overwrites `cooldownEnd = block.timestamp + cooldownDuration`. A user who tops up their cooldown restarts the 90-day clock. This only affects the caller's own cooldown (no cross-user impact) and is documented behavior. Not a vulnerability.

**V2-3 — Rounding direction (safe)**
`cooldownAssets` uses `previewWithdraw` (shares rounded UP → user burns slightly more shares) and `cooldownShares` uses `previewRedeem` (assets rounded DOWN → user gets slightly fewer assets). Both favor the vault. Combined with OZ's `previewWithdraw(maxWithdraw(owner)) <= balanceOf(owner)` invariant, a user can always fully exit through `cooldownAssets(maxWithdraw(...))` without hitting an "insufficient shares" revert. ✅

**V2-4 — Type casts in cooldown state (safe)**
`uint104(block.timestamp) + cooldownDuration` and `uint152(assets)`. `type(uint104).max ≈ 2e31` vs `block.timestamp ≈ 1.7e9` + 90 days — no overflow. `type(uint152).max ≈ 5.7e45` vs USDe 18-decimals total supply — no practical overflow. Solidity 0.8 would revert on overflow anyway. ✅

**V2-5 — Silo balance always matches sum of `underlyingAmount` (safe)**
Verified the accounting invariant: `cooldownAssets`/`cooldownShares` transfer exactly `assets` USDe to the silo (OZ `_withdraw` uses `SafeERC20.safeTransfer` of the exact `assets` value, and USDe is not fee-on-transfer) and credit exactly `assets` to `underlyingAmount`. `unstake` debits exactly `assets` from `underlyingAmount` and pulls exactly `assets` from the silo. No drift, no cross-user theft. ✅

### Comparison with USDtb equivalent
N/A (no USDtb staking vault).

### Severity
- V2-1 (blacklist bypass on `unstake`): **Medium**.
- V2-2 … V2-5: **Informational**.

---

## Contract 4: `USDeSilo.sol` (30 lines)

### Purpose
Trivial holding contract for USDe during the cooldown window. Only the staking vault can withdraw.

### Line-by-line analysis

| Lines | Area | Notes |
|------|------|-------|
| 13-20 | State + constructor | `_STAKING_VAULT` and `_USDE` immutable. Set once at construction by `StakedUSDeV2`. ✅ |
| 22-25 | `onlyStakingVault` | `msg.sender == _STAKING_VAULT`. Immutable → no takeover possible. ✅ |
| 27-29 | `withdraw` | `_USDE.transfer(to, amount)`. No reentrancy surface (plain ERC20 transfer). ✅ |

### Suspicious patterns / Edge cases tested

**Silo-1 — No rescue function (Informational)**
USDe accidentally sent directly to the silo (not via `cooldownAssets`) is permanently stuck — the silo only exposes `withdraw(to, amount)` for the staking vault, and the staking vault only calls it inside `unstake` with a user's `underlyingAmount`. There is no admin sweep. This is a feature (prevents admin theft of cooldown funds) but means dust donations to the silo are irrecoverable. No exploit.

**Silo-2 — Anyone can deposit USDe to the silo (safe)**
The silo has no `deposit` function and no `receive()` (USDe is ERC20, not ETH). Anyone can `USDe.transfer(silo, x)` directly, but that just inflates the silo balance above the sum of `underlyingAmount` — it does not create a claim. The excess is unreachable by anyone (Silo-1). No theft.

### Comparison with USDtb equivalent
N/A.

### Severity
- Silo-1, Silo-2: **Informational**.

---

## Contract 5: `StakingRewardsDistributor.sol` (189 lines)

### Purpose
Automation helper. Holds collateral, is an EthenaMinting delegated signer (operator), mints USDe via EthenaMinting, then pushes the minted USDe into `StakedUSDe` as rewards via `transferInRewards`. Owner = multisig (config), operator = delegated signer (mint + reward push).

### Line-by-line analysis

| Lines | Area | Notes |
|------|------|-------|
| 24-43 | State | `STAKING_VAULT`, `USDE_TOKEN` immutable; `mintContract` + `operator` storage. ✅ |
| 43-80 | Constructor | Validates non-zero; sets immutables; approves staking vault for max USDe; `setOperator(_operator)` (which delegates signer on EthenaMinting); `approveToMintContract(_assets)` (max approval to mint contract). Ownership transferred to `_admin` at end. ✅ |
| 88-95 | `transferInRewards` | Operator-only; balance check; calls `STAKING_VAULT.transferInRewards`. **No `nonReentrant`** — see **SRD-1**. |
| 104-117 | `rescueTokens` | Owner + `nonReentrant`; ETH via `.call`, else `safeTransfer`. ✅ |
| 124-128 | `setMintingContract` | Owner-only. ✅ |
| 135-143 | `approveToMintContract` | Owner-only; max-approves assets to `mintContract`. ✅ |
| 151-163 | `revokeApprovals` | Owner-only; can't revoke from current `mintContract`. ✅ |
| 172-181 | `setOperator` | Owner-only; removes old operator's delegation, sets new operator to PENDING (new operator must `confirmDelegatedSigner`). Allows `address(0)` as emergency kill. ✅ |
| 186-188 | `renounceOwnership` | Disabled. ✅ |

### Suspicious patterns / Edge cases tested

**SRD-1 — `transferInRewards` lacks `nonReentrant` (safe)**
The function calls `STAKING_VAULT.transferInRewards`, which is itself `nonReentrant` on the staking vault and does `_updateVestingAmount` (reverts if StillVesting) then `safeTransferFrom` of USDe (plain ERC20, no callback). No reentrancy path. The downstream `StillVesting` guard also prevents a second `transferInRewards` within 8 hours even if it could be called. ✅

**SRD-2 — Balance check vs actual transfer (safe)**
`USDE_TOKEN.balanceOf(address(this)) < _rewardsAmount` is checked before the staking vault pulls via `transferFrom`. The pull is from `msg.sender = distributor`, and USDe is non-fee. No reordering possible between check and pull (no external call in between). ✅

**SRD-3 — `setOperator` mid-window operational pause (Informational)**
When `setOperator(new)` runs, the old operator is immediately REJECTED and the new one is PENDING (not yet ACCEPTED). Until the new operator calls `confirmDelegatedSigner`, NO operator can sign mints. This is intentional 2-step security, not a bug — but it does mean a botched operator rotation can pause reward distribution. Operator rotation also cannot be used to steal funds: the distributor only mints USDe from its own pre-approved collateral and pushes it to the staking vault.

**SRD-4 — `approveToMintContract` gives max allowance (Informational, by design)**
The distributor max-approves the mint contract for each asset. If the mint contract were ever compromised, it could drain the distributor's collateral. This is inherent to the delegated-mint architecture; the mint contract is itself role-gated and audited.

### Comparison with USDtb equivalent
N/A (USDtb has no equivalent distributor in the analyzed set).

### Severity
- SRD-1 … SRD-4: **Informational**.

---

## Contract 6: `EthenaLPStaking.sol` (180 lines)

### Purpose
Stake LP tokens (Curve/Convex/Balancer USDe-pool LPs) to earn off-chain airdrop shards. Pure custody contract with a cooldown on withdrawal. Rewards are computed off-chain; this contract only holds LP tokens.

### Line-by-line analysis

| Lines | Area | Notes |
|------|------|-------|
| 18-38 | State | `currentEpoch`, per-user-per-token `stakes`, per-token `stakeParametersByToken`. `StakeParameters` packs epoch/stakeLimit/totalStaked/totalCoolingDown/cooldown. ✅ |
| 42-45 | Constructor | Validates non-zero owner. ✅ |
| 53-56 | `checkAmount` | Rejects 0. ✅ |
| 64-68 | `setEpoch` | Owner-only; must differ from current. ✅ |
| 77-85 | `updateStakeParameters` | Owner-only; cooldown ≤ 90 days. Owner can change epoch/stakeLimit/cooldown but NOT totalStaked/totalCoolingDown (storage assignment only touches the named fields). ✅ |
| 93-104 | `rescueTokens` | Owner + `nonReentrant` + `checkAmount`; ETH via `.call` (no invariant check — ETH isn't staked); ERC20 `safeTransfer` then `_checkInvariant`. ✅ |
| 107-109 | `renounceOwnership` | Disabled. ✅ |
| 118-128 | `stake` | `nonReentrant` + `checkAmount`; epoch match; stakeLimit check; bumps `totalStaked` + user `stakedAmount`; `safeTransferFrom`; `_checkInvariant`. CEI ok (state before transfer, but `nonReentrant` + invariant-after protect). ✅ |
| 136-147 | `unstake` | `nonReentrant` + `checkAmount`; balance check; moves `stakedAmount → coolingDownAmount`; resets `cooldownStartTimestamp`; bumps totals; `_checkInvariant`. ✅ (cooldown reset is by-design — see **LP-2**) |
| 154-164 | `withdraw` | `nonReentrant` + `checkAmount`; coolingDown check; `block.timestamp >= cooldownStartTimestamp + stakeParameters.cooldown` (reads CURRENT cooldown, not unstake-time — see **LP-1**); `safeTransfer`; `_checkInvariant`. ✅ |
| 175-179 | `_checkInvariant` | `balance >= totalStaked + totalCoolingDown`. Re-sloads stakeParameters intentionally (defensive). Catches any state where the contract would owe more than it holds. ✅ |

### Suspicious patterns / Edge cases tested

**LP-1 — `withdraw` reads cooldown at withdraw-time, not unstake-time (Informational)**
`stakeParameters.cooldown` is read fresh in `withdraw`. If the owner raises the cooldown after a user unstaked, that user must wait longer; if lowered, shorter. This is a trust assumption on the owner (multisig), not a code bug. The owner cannot reduce the invariant (totalStaked/totalCoolingDown are untouched by `updateStakeParameters`), so fund safety is preserved.

**LP-2 — `unstake` resets `cooldownStartTimestamp` (Informational, by design)**
Each `unstake` overwrites `cooldownStartTimestamp = block.timestamp`, restarting the clock for the ENTIRE `coolingDownAmount`. A user who unstakes in batches can inadvertently push out their withdrawability. Self-only impact, documented behavior. Not a vulnerability.

**LP-3 — `_checkInvariant` defends against fee-on-transfer / donation edge cases (safe)**
If an LP token were fee-on-transfer, `safeTransfer` in `withdraw`/`rescueTokens` could drop the contract balance below `totalStaked + totalCoolingDown`; `_checkInvariant` would catch it and revert. Owner `rescueTokens` is also bounded by the invariant — the owner can only rescue *excess* tokens, never staked/cooling amounts. ✅

**LP-4 — `stake` epoch gating (safe)**
Staking requires `currentEpoch == stakeParameters.epoch`. `unstake`/`withdraw` have no epoch check, so users can always exit even after the epoch rolls. ✅

**LP-5 — `stakeLimit` is uint248, `amount` is uint104 (safe)**
`totalStaked + amount > stakeLimit` uses uint256 arithmetic (Solidity promotes). No overflow at realistic LP-token supplies. ✅

### Comparison with USDtb equivalent
N/A.

### Severity
- LP-1, LP-2: **Informational**.
- LP-3, LP-4, LP-5: **Informational** (defenses verified working).

---

## Cross-Contract Findings

### X-1 — StakedUSDeV2 ↔ USDeSilo reentrancy (safe)
The only call from V2 into the silo is `silo.withdraw` inside `unstake`. The silo does a plain `USDe.transfer` (no hooks). Even if USDe had a callback, V2 zeroes `underlyingAmount`/`cooldownEnd` before the call, so a reentrant `unstake` would withdraw 0. No cross-contract reentrancy.

### X-2 — StakedUSDeV2 ↔ StakedUSDe parent (safe)
`cooldownAssets`/`cooldownShares` call `_withdraw` (parent), which is `nonReentrant`. The parent's `_beforeTokenTransfer` is invoked on the sUSDe burn and does not block burns to `address(0)`/the silo (the silo is not FULL_RESTRICTED). No bypass.

### X-3 — EthenaMinting ↔ StakingRewardsDistributor (safe)
The distributor is a delegated signer + MINTER on EthenaMinting and a REWARDER on StakedUSDe. The only fund flows are: distributor's collateral → EthenaMinting custodians (mint), minted USDe → distributor, distributor USDe → StakedUSDe (`transferInRewards`). All gated by roles; no path for an external attacker to trigger any of these without the operator key.

### X-4 — Decimal handling (safe)
USDe = 18 decimals (verified in `StakedUSDe.decimals()` and OZ ERC20 default). All collateral math in EthenaMinting is in the collateral token's native decimals with the off-chain RFQ providing the conversion; there is no on-chain decimal normalization (unlike USDtbMinting's `verifyStablesLimit`, which does explicit decimal scaling). No precision-loss bug because no conversion is done on-chain.

---

## Findings Summary Table

| ID | Contract | Finding | Severity |
|----|----------|---------|----------|
| V2-1 | StakedUSDeV2 | `unstake` skips `FULL_RESTRICTED_STAKER_ROLE` — blacklisted user can claim pre-cooldowned silo USDe after 90d | **Medium** |
| SUSDe-3 | StakedUSDe | `_checkMinShares` can temporarily lock a small depositor's exit when totalSupply is near 1e18 | Low |
| EM-1 | EthenaMinting | No benefactor/beneficiary whitelist (vs USDtb) | Informational |
| EM-2 | EthenaMinting | No on-chain price/slippage check on mint | Informational |
| EM-3 | EthenaMinting | Donated ETH only reclaimable via redeem/transferToCustody | Informational |
| EM-4 | EthenaMinting | NATIVE_TOKEN addable as supported asset (intentional asymmetry) | Informational |
| EM-5 | EthenaMinting | Nonce truncated to uint64 (collision, no replay) | Informational |
| EM-6 | EthenaMinting | ETH-callback reentrancy surface (mitigated by nonReentrant) | Informational |
| EM-7 | EthenaMinting | Collateral routing rounding (dust to last custodian, no loss) | Informational |
| SUSDe-1 | StakedUSDe | `totalAssets()` underflow theoretically possible (trusted-only) | Informational |
| SUSDe-2 | StakedUSDe | `redistributeLockedAmount` replaces vestingAmount (StillVesting guards) | Informational |
| SUSDe-4 | StakedUSDe | Donation/inflation attack (non-profitable, OZ v4.9 + min-shares) | Informational |
| SUSDe-5 | StakedUSDe | Flash-loan rate manipulation (non-profitable) | Informational |
| V2-2 | StakedUSDeV2 | Cooldown reset on each `cooldownAssets` (by design) | Informational |
| V2-3 | StakedUSDeV2 | Rounding favors vault | Informational |
| V2-4 | StakedUSDeV2 | Type-cast overflow analysis (safe) | Informational |
| V2-5 | StakedUSDeV2 | Silo balance == sum(underlyingAmount) invariant holds | Informational |
| Silo-1 | USDeSilo | No rescue function (dust irrecoverable) | Informational |
| Silo-2 | USDeSilo | Direct USDe donation to silo is unreachable (safe) | Informational |
| SRD-1 | StakingRewardsDistributor | `transferInRewards` no nonReentrant (safe, plain ERC20) | Informational |
| SRD-2 | StakingRewardsDistributor | Balance-check-then-pull (safe) | Informational |
| SRD-3 | StakingRewardsDistributor | Operator rotation pause window (by design) | Informational |
| SRD-4 | StakingRewardsDistributor | Max approval to mint contract (by design) | Informational |
| LP-1 | EthenaLPStaking | Withdraw reads cooldown at withdraw-time (trust assumption) | Informational |
| LP-2 | EthenaLPStaking | Unstake resets cooldown clock (by design) | Informational |
| LP-3/4/5 | EthenaLPStaking | Invariant/epoch/limit defenses verified | Informational |

---

## Conclusion

**Critical bugs found: 0.**
**High bugs found: 0.**
**Medium bugs found: 1** (V2-1: blacklist bypass on `unstake` — compliance gap, not fund theft).
**Low bugs found: 1** (SUSDe-3: min-shares exit griefing, funds not stolen).

The Ethena USDe suite is defensively coded: `nonReentrant` everywhere it matters, CEI respected, rounding vault-favoring, role separation clean, and the ERC4626 inflation attack is double-mitigated (OZ v4.9.5 virtual shares + `_checkMinShares`). The single Medium finding is a design gap in the cooldown-claim path that lets a *pre-flagged* user eventually reclaim their own silo'd USDe; it does not enable theft of other users' funds. No vulnerability in this set meets the "theft of funds / permanent freezing of protocol funds by an external attacker" bar for a Critical/High Immunefi payout.

**No submission to Immunefi is recommended from this analysis.** The Medium finding may be worth an internal note to the Ethena team depending on their compliance scope, but per task instructions nothing is submitted.

---

## Files read
- `/home/z/ethena-usde/contracts/contracts/EthenaMinting.sol` (551 lines) — full
- `/home/z/ethena-usde/contracts/contracts/StakedUSDeV2.sol` (131 lines) — full
- `/home/z/ethena-usde/contracts/contracts/USDeSilo.sol` (30 lines) — full
- `/home/z/ethena-usde/contracts/contracts/StakedUSDe.sol` (268 lines) — full
- `/home/z/ethena-usde/contracts/contracts/StakingRewardsDistributor.sol` (189 lines) — full
- `/home/z/ethena-usde/contracts/contracts/EthenaLPStaking.sol` (180 lines) — full
- `/home/z/ethena-usdtb/contracts/usdtb/USDtbMinting.sol` (681 lines) — comparison baseline
- Supporting: `SingleAdminAccessControl.sol`, `USDe.sol`, `interfaces/{IStakedUSDe,IStakedUSDeCooldown,IUSDeSiloDefinitions,IEthenaMinting,IEthenaLPStakingDefinitions,IStakingRewardsDistributor}.sol`, OZ `ERC4626.sol` (v4.9.5)
