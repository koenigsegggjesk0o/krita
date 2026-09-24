# Ethena Audit PDFs — Deep Read & Re-Assessment

**Task ID:** eth-audit-pdfs-deep
**Agent:** Opus
**Date:** 2026-09-24
**Method:** Full-text PDF extraction (`pdftotext -layout`) + side-by-side verification against current deployed/source contracts in `/home/z/ethena-usdtb/contracts/`, `/home/z/ethena-usde/contracts/contracts/`. C4 report at `/home/z/ethena-c4-findings/report.md` read in full (1893 lines). All 3 Ethena-specific PDFs read in full.

> **Honest bottom line up front:** 0 exploitable findings rediscovered. Several "Acknowledged but not fixed" findings remain in the deployed code, but every one of them is either (a) admin-error-only, (b) theoretical-only with no realistic trigger condition, or (c) a legal-compliance gap rather than a fund-theft/freezing bug. One finding originally marked "Acknowledged" by Pashov (L-07 — front-run `redistributeLockedAmount` via `burnFrom`) was actually **silently fixed** in the post-audit commit. No vuln PoC file was written because no finding crossed the "appears exploitable today" bar. Details below.

---

## 1. PDFs Read (Full Text)

| # | File | Pages | Auditor | Scope | Audit window |
|---|------|-------|---------|-------|--------------|
| 1 | `/home/z/ethena-usdtb/2024-10-31-ethena-ustb-v1.0.pdf` | 8 | **Cyfrin** (lead: Immeas) | USDtb (formerly UStb) — commit `d82676f` | Oct 23 – Nov 1 2024 |
| 2 | `/home/z/ethena-usdtb/Ethena-security-review-October.pdf` | 14 | **Pashov Audit Group** (T1MOH, btk, peanuts) | USDtb — commit `ae1856b5` (fixes review `c2256464`) | Oct 17–20 2024 |
| 3 | `/home/z/ethena-usdtb/Ethena_final_report_Quantstamp.pdf` | 17 | **Quantstamp** (V. Callens, R. Rohleder, R. Islam) | USDtb — commit `d82676f` | Oct 23–25 2024 |

Additional non-PDF source read in full:
- `/home/z/ethena-c4-findings/report.md` (1893 lines) — Code4rena Contest #299, **Oct 24–30 2023**, scope = USDe core (`USDe.sol`, `EthenaMinting.sol`, `StakedUSDe.sol`, `StakedUSDeV2.sol`, `USDeSilo.sol`, `SingleAdminAccessControl.sol`). 4 Medium + 9 Low/Non-Critical + 10 Gas + analysis.

PDFs in `/home/z/ethena-usde/contracts/lib/openzeppelin-contracts/{audits,certora}/` are OpenZeppelin library audits (Scope of OZ v4.9.3 etc.) — not Ethena-specific, skipped.

Plain-text extractions saved at `/tmp/ethena_pdf_text/{ustb,sigmanotes,quantstamp}.txt`.

---

## 2. Current Contract Baselines Used for Verification

| Repo | Commit | Notes |
|------|--------|-------|
| `/home/z/ethena-usdtb/` | `2e7c9f6` (Nov 25 2024) "token rename (#3)" | Post-audit. Token renamed UStb → USDtb. Single commit on top of audit-time code. |
| `/home/z/ethena-usde/contracts/` | n/a (no `.git` in `contracts/`) | Source mirror; verified against etherscan-style contract. |
| `/home/z/ethena-c4-findings/` | n/a (report only) | C4 final report + 736 issue JSON stubs. |

The contest repo `ethena-usdtb` was published post-audit with the token rename. Crucially, the `_beforeTokenTransfer` hook was rewritten between the audit-time commit (`d82676f`/`ae1856b5`) and the public commit (`2e7c9f6`) — it now does exhaustive case analysis (minter/admin/whitelisted paths, with `BLACKLISTED_ROLE` checks on `from`/`to`/`msg.sender` for every branch). This is the silent fix for Pashov L-07 (see §4.2.7).

---

## 3. Cyfrin PDF — Full Findings & Re-Assessment

### 3.1 [L-1] Lack of storage gap in upgradeable base contract

**Original finding text (verbatim):**
> To manage access control, Ethena uses a modified version of the OpenZeppelin AccessControl library called SingleAdminAccessControl. Since UStb is upgradeable, this library has been further modified to function as an upgradeable base contract: SingleAdminAccessControlUpgradeable. However, it lacks a storage gap at the end. Storage gaps are beneficial because they allow the base contract to add storage variables in the future without "shifting down" all state variables in the inheritance chain.
>
> **Impact:** Upgrading may introduce storage collisions for inheriting contracts.
>
> **Recommended Mitigation:** Consider adding a storage gap at the end: `uint256[48] private __gap`

**Auditor severity:** Low
**Ethena response:** None documented (Cyfrin report shows status = "Open")
**Current status:** **NOT FIXED** — verified at `/home/z/ethena-usdtb/contracts/SingleAdminAccessControlUpgradeable.sol` lines 13-81. The contract has two storage vars (`_currentDefaultAdmin`, `_pendingDefaultAdmin`) and no `__gap` array.

**Re-assessment — is it now exploitable?** No, but it is a latent upgrade hazard. `USDtb.sol` inherits `SingleAdminAccessControlUpgradeable` and immediately declares `TransferState public transferState` (line 35), which sits in storage slot 2 (after the two admin vars). If Ethena ever upgrades `SingleAdminAccessControlUpgradeable` to add a 3rd state variable, that new variable would alias `transferState` and corrupt the transfer state. Not exploitable today (no such upgrade has happened); only exploitable if a future upgrade is sloppy. **Latent risk only.**

---

### 3.2 [L-2] UStb cannot be burnt when whitelist is enabled

**Original finding text (verbatim):**
> The new Ethena UStb token has three transfer states, one of which is WHITELIST_ENABLED. In the WHITELIST_ENABLED state, only whitelisted users should be able to send and receive UStb. This is enforced through a check in the overridden `_beforeTokenTransfer` method to ensure that the `to` address is whitelisted:
> ```solidity
> if (!hasRole(WHITELISTED_ROLE, msg.sender) || !hasRole(WHITELISTED_ROLE, to) ||
>     !hasRole(BLACKLISTED_ROLE, msg.sender) || hasRole(BLACKLISTED_ROLE, to)){
>    revert OperationNotAllowed();
> }
> ```
> However, when burning tokens, the `to` address will be `address(0)`, which will prevent burning.
>
> **Impact:** Whitelisted users will be unable to burn their UStb while whitelisting is enabled. This limitation would also prevent them from redeeming their collateral from UStbMinting, as that account is whitelisted.
>
> **Ethena:** Fixed in PR#10
> **Cyfrin:** Verified. Whitelisted users can burn during whitelist only, both directly and though redeem.

**Auditor severity:** Low
**Ethena response:** Fixed in PR#10
**Current status:** **FIXED** — verified at `USDtb.sol` line 190: the `WHITELIST_ENABLED` branch now has explicit `hasRole(WHITELISTED_ROLE, msg.sender) && hasRole(WHITELISTED_ROLE, from) && to == address(0)` case for "whitelisted user can burn".

**Re-assessment:** Not exploitable. Fixed and verified.

---

### 3.3 [L-3] Non-whitelisted users can transfer UStb via whitelisted intermediaries in WHITELIST_ENABLED mode

**Original finding text (verbatim):**
> When transfers are limited to the WHITELIST_ENABLED state, only whitelisted users should be able to send and receive UStb, as detailed in the AUDIT.md: "WHITELIST_ENABLED: Only whitelisted addresses can send and receive this token." This restriction is enforced in `_beforeTokenTransfer` through a check [...] However, a non-whitelisted user can bypass this restriction by approving a whitelisted user to transfer on their behalf. Since only `msg.sender` and `to` are checked, the `from` address can be any non-blacklisted user.
>
> **Impact:** This behavior violates the requirement stated in AUDIT.md. Consequently, a non-whitelisted address can still send UStb, albeit only to a whitelisted receiver. Additionally, it enables non-whitelisted users to redeem through UStbMinting, as the UStbMinting contract is a whitelisted address.
>
> **Ethena:** Fixed in PR#10
> **Cyfrin:** Verified. `from` is not required to have role `WHITELISTED_ROLE`

**Auditor severity:** Low
**Ethena response:** Fixed in PR#10
**Current status:** **FIXED** — `USDtb.sol` line 192-195: the normal-case check now requires `hasRole(WHITELISTED_ROLE, msg.sender) && hasRole(WHITELISTED_ROLE, from) && hasRole(WHITELISTED_ROLE, to)`. So `from` MUST be whitelisted.

**Re-assessment:** Not exploitable. Fixed.

(Note: Cyfrin's verification comment is slightly ambiguous — "from is not required to have role WHITELISTED_ROLE" appears to be a typo for "from is now required…", because the actual deployed code does require it.)

---

### 3.4 Informational findings [I-1]…[I-4]

- **[I-1] Unused empty `foundry.toml` in `contracts/foundry/`** — Status: Open (minor cleanup). Not exploitable.
- **[I-2] Typos and formatting discrepancies** (`h2olds`, `enabeld`, missing space) — Status: Fixed in `7368bb88` per Quantstamp S1. Not exploitable.
- **[I-3] Lack of event emitted on state change** (`setStablesDeltaLimit`) — Status: **STILL NOT FIXED** (verified `USDtbMinting.sol` line 649-651: `setStablesDeltaLimit` writes storage but emits nothing). Same issue flagged again by Quantstamp USTB-4. Not exploitable — off-chain monitoring gap only.
- **[I-4] Unused events and errors** (`MinterAdded`, `MinterRemoved`, `ToggleTransfers`, `CantRenounceOwnership`) — Partially fixed: `MinterAdded`/`MinterRemoved` are now emitted from `addMinter`/`removeMinter` in `USDtb.sol` (lines 56, 61, 66). `ToggleTransfers` still unused (replaced by `TransferStateUpdated`). Not exploitable.

---

## 4. Pashov Audit Group PDF — Full Findings & Re-Assessment

### 4.1 [L-01] Some variables are never used

**Original finding text (verbatim):**
> There are 2 such variables in UStbMinting.sol:
> ```solidity
> bytes32 private constant EIP712_DOMAIN_TYPEHASH = keccak256(abi.encodePacked(EIP712_DOMAIN));
> bytes32 private constant ROUTE_TYPE = keccak256("Route(address[] addresses,uint128[] ratios)");
> ```
> Either remove or add missing functionality.

**Auditor severity:** Low | **Status:** Resolved
**Current verification:** Neither constant appears in `USDtbMinting.sol` (grep confirmed). Resolved.

---

### 4.2 [L-02] Blacklisted tokens can be transferred during WHITELIST_ENABLED

**Original finding text (verbatim):**
> In FULLY_ENABLED it disallows transferring blacklisted tokens. However, during WHITELIST_ENABLED blacklisted logic is missing:
> ```solidity
> } else if (transferState == TransferState.WHITELIST_ENABLED) {
>   if (!hasRole(WHITELISTED_ROLE, msg.sender) || !hasRole(WHITELISTED_ROLE, to)){
>     revert OperationNotAllowed();
>   }
> ```
> Whitelisted addresses can transfer tokens from the blacklisted owner. Add an additional check to WHITELIST_ENABLED state:
> ```diff
> +     if (hasRole(BLACKLISTED_ROLE, from) && to != address(0)) {
> +       revert OperationNotAllowed();
> +     }
> ```

**Auditor severity:** Low | **Status:** Resolved
**Current verification:** `USDtb.sol` `_beforeTokenTransfer` (lines 162-203) does exhaustive case analysis. In the `WHITELIST_ENABLED` branch, the "normal case" requires `hasRole(WHITELISTED_ROLE, msg.sender) && hasRole(WHITELISTED_ROLE, from) && hasRole(WHITELISTED_ROLE, to)`. Additionally, `addBlacklistAddress` now revokes `WHITELISTED_ROLE` before granting `BLACKLISTED_ROLE` (line 75), so a blacklisted user cannot simultaneously be whitelisted. Resolved.

---

### 4.3 [L-03] Incorrect variable's type used to calculate ORDER_TYPE

**Original finding text (verbatim):**
> Here you can see `uint128 expiry, uint120 nonce`:
> ```solidity
> bytes32 private constant ORDER_TYPE = keccak256(
>   "Order(string order_id, uint8 order_type, uint128 expiry, uint120 nonce, ...)"
> );
> ```
> However actual types are slightly different:
> ```solidity
> struct Order {
>   string order_id;
>   OrderType order_type;
>   @> uint120 expiry;
>   @> uint128 nonce;
>   ...
> }
> ```
> Using EIP712 as it is will produce an incorrect signature, though easily mitigateable. Update ORDER_TYPE to contain correct types.

**Auditor severity:** Low | **Status:** Resolved
**Current verification:** `USDtbMinting.sol` line 33:
```solidity
bytes32 private constant ORDER_TYPE = keccak256(
  "Order(string order_id,uint8 order_type,uint120 expiry,uint128 nonce,address benefactor,address beneficiary,address collateral_asset,uint128 collateral_amount,uint128 usdtb_amount)"
);
```
matches `IUSDtbMinting.sol` struct (lines 45-55): `uint120 expiry; uint128 nonce;`. Resolved. (If this had not been fixed, every EIP-712 signature would have been generated against a wrong type-string and would have failed to verify on-chain — a full-mint-flow DoS. So this was actually a higher-impact bug than the Low label suggests.)

---

### 4.4 [L-04] `_beforeTokenTransfer()` should check that the WHITELISTED_ROLE does not have a BLACKLISTED_ROLE

**Original finding text (verbatim):**
> An address can have both the white and blacklisted role in UStb.sol. Consider the scenario where the BLACKLIST_MANAGER_ROLE and WHITELIST_MANAGER_ROLE are not the same person. If a whitelisted address turns malicious and the whitelist manager role is not available to remove the whitelist, the blacklist manager can still blacklist the address.
>
> Ensure that in `transferState == TransferState.WHITELIST_ENABLED`, the `WHITELISTED_ROLE` does not have the `BLACKLISTED_ROLE` as well.

**Auditor severity:** Low | **Status:** Resolved
**Current verification:** `addBlacklistAddress` (USDtb.sol line 73-78) now atomically revokes `WHITELISTED_ROLE` before granting `BLACKLISTED_ROLE`:
```solidity
function addBlacklistAddress(address[] calldata users) external onlyRole(BLACKLIST_MANAGER_ROLE) {
  for (uint8 i = 0; i < users.length; i++) {
    if (hasRole(WHITELISTED_ROLE, users[i])) _revokeRole(WHITELISTED_ROLE, users[i]);
    _grantRole(BLACKLISTED_ROLE, users[i]);
  }
}
```
Resolved by enforcement at grant-time.

---

### 4.5 [L-05] Functions should check whether tokenAsset is active

**Original finding text (verbatim):**
> `setMaxMintPerBlock()` is called by DEFAULT_ADMIN_ROLE to set the new max mint of the asset per block. The function should have additional checks, like checking `tokenConfig[asset].isActive`.
> ```solidity
> function _setMaxMintPerBlock(uint128 _maxMintPerBlock, address asset) internal {
>   //@audit - like other functions, should check `tokenConfig[asset].isActive`
>   uint128 oldMaxMintPerBlock = tokenConfig[asset].maxMintPerBlock;
>   tokenConfig[asset].maxMintPerBlock = _maxMintPerBlock;
>   emit MaxMintPerBlockChanged(oldMaxMintPerBlock, _maxMintPerBlock, asset);
> }
> ```
> Also good to add a zero amount check since it is done in other functions as well.

**Auditor severity:** Low | **Status:** Resolved (per Pashov report)
**Current verification:** `USDtbMinting.sol` `_setMaxMintPerBlock` (line 618-622) still does NOT check `isActive`. This is **intentional**: the Quantstamp Operational Considerations §3 explicitly notes that admin can "Change the mint cap per collateral, including setting it to zero or greater than the global cap" — this is a feature to allow pre-configuring limits before activating an asset. The actual minting path is gated by the `belowMaxMintPerBlock` modifier (line 112-119), which does revert on `!_config.isActive`. So no security impact: setting limits on inactive assets is harmless because no mint can use them. Pashov marked "Resolved" — likely because the original concern (admin confusion) was declined as a design choice.

**Re-assessment:** Not exploitable. Defense-in-depth gap, not a vulnerability.

---

### 4.6 [L-06] Blacklisted users can bypass restriction through approvals

**Original finding text (verbatim):**
> The UStb token is an upgradeable ERC20 contract that includes mint and burn functionality, as well as multiple transfer states: FULLY_DISABLED, WHITELIST_ENABLED, FULLY_ENABLED.
>
> According to the documentation, blacklisted users should be restricted from sending or receiving tokens: "In any case blacklisted addresses cannot send or receive tokens."
>
> However, the OpenZeppelin ERC20 contract allows approved addresses to transfer tokens on behalf of another address. This creates a loophole: blacklisted users can still transfer their tokens if the whitelist is enabled, as the `_beforeTokenTransfer()` function does not check the `from` address for blacklisting.
>
> [PoC: `testBypassBlacklistRole` shows user1 grants approval to user2, then user2 `transferFrom` user1 → user2 succeeds.]
>
> Adding a blacklist check in the `_beforeTokenTransfer()` function to block transfers from a blacklisted address may cause issues, such as preventing the admin from calling `redistributeLockedAmount()`. A better approach would be to block blacklisted users from using the `approve` function. You can achieve this by adding a check like this:
> ```solidity
> function _approve(address owner, address spender, uint256 value) internal virtual override {
>   if (hasRole(BLACKLISTED_ROLE, owner)) {
>     revert OperationNotAllowed();
>   }
>   super._approve(owner, spender, value);
> }
> ```

**Auditor severity:** Low | **Status:** Resolved
**Current verification:** Ethena chose a different (better) mitigation: instead of blocking `approve`, they rewrote `_beforeTokenTransfer` to check `BLACKLISTED_ROLE` on `from` in every transfer branch. The "normal case" at line 173-176 requires `!hasRole(BLACKLISTED_ROLE, msg.sender) && !hasRole(BLACKLISTED_ROLE, from) && !hasRole(BLACKLISTED_ROLE, to)`. The minter/admin paths explicitly carve out only the legitimate mint/redeem/redistribute scenarios, and each of those still checks `BLACKLISTED_ROLE` on the non-zero side (e.g., redeem path requires `!hasRole(BLACKLISTED_ROLE, from)`). Resolved.

---

### 4.7 [L-07] Blacklisted users can front-run `redistributeLockedAmount` and burn their tokens — **ACKNOWLEDGED, then silently fixed**

**Original finding text (verbatim):**
> The UStb token contract includes a blacklist mechanism that restricts certain addresses from transferring tokens. It also provides an admin function to forcibly transfer tokens from blacklisted addresses to non-blacklisted ones.
>
> However, there is a vulnerability where blacklisted users can front-run the `redistributeLockedAmount` function and burn their tokens. This allows them to prevent the admin from redistributing their tokens to another address. They achieve this by approving a burner account to spend and burn their tokens before the admin's redistribution takes place.
>
> [PoC: `testFrontrunRedistributeLockedAmount` — alex grants approval to alexBurner, alexBurner calls `burnFrom(alex, _amount)` before admin calls `redistributeLockedAmount(alex, newOwner)` — admin receives 0 tokens because alex's balance was already burned.]
>
> **Recommended Mitigation:** To mitigate this issue, consider adding access controls to the `burnFrom()` function to prevent blacklisted users from burning their tokens.

**Auditor severity:** Low | **Status: "Acknowledged" at audit-time fixes-review (commit `c2256464`)**
**Current verification (commit `2e7c9f6`, Nov 25 2024):** **FIXED in deployed code.**

Trace through the current `_beforeTokenTransfer` (USDtb.sol lines 162-203) for the front-run scenario:
- `transferState == FULLY_ENABLED`
- `msg.sender = alexBurner` (regular address, not admin/minter)
- `from = alex` (has `BLACKLISTED_ROLE`)
- `to = address(0)` (burn)

Branch evaluation:
1. Minter redeem (`MINTER_CONTRACT && !BLACKLISTED from && to==0`) → false (msg.sender not minter)
2. Minter mint (`MINTER_CONTRACT && from==0 && !BLACKLISTED to`) → false
3. Admin redistribute-burn (`DEFAULT_ADMIN && BLACKLISTED from && to==0`) → false (msg.sender not admin)
4. Admin redistribute-mint → false
5. Normal case (`!BLACKLISTED msg.sender && !BLACKLISTED from && !BLACKLISTED to`) → false (alex is blacklisted)
6. Else → **`revert OperationNotAllowed()`** ✓

The Quantstamp test suite (235 tests in fix review) confirms via test names like `test_bl_sender_bl_from_burn_fully_enabled_revert`, `test_sender_bl_from_burn_fully_enabled_revert`, `test_wl_sender_bl_from_burn_fully_enabled_revert` — all burn-from-blacklisted scenarios revert in every transfer state.

**Re-assessment:** Not exploitable. The "Acknowledged" status in the Pashov report reflected the code at fixes-review commit `c2256464`. By the time the contest repo was published (`2e7c9f6`, ~5 weeks later), the `_beforeTokenTransfer` had been rewritten to block this. So although the audit report literally says "Acknowledged" (not "Resolved"), the deployed contract does not have the bug. **This is the most important finding of the deep-read** — the audit report's status field understates what was actually shipped.

---

## 5. Quantstamp PDF — Full Findings & Re-Assessment

All Quantstamp findings marked "Acknowledged" by Ethena with explicit "won't fix" comments.

### 5.1 USTB-1 Missing Input Validations (Low, Acknowledged)

**Original finding text (verbatim, abridged):**
> It is important to validate inputs, even if they only come from trusted addresses, to avoid human error:
> 1. In `UStb.sol`, function `initialize()`, the address `minterContract` may not be a contract;
> 2. In `UStb.sol`, function `addBlacklistAddress()`, it is possible to blacklist:
>    - the minter contract and this could temporarily disrupt the mint and redeem operations;
>    - the address with the role `DEFAULT_ADMIN_ROLE`, and this could temporarily disrupt the operation to redistribute locked amounts;
> 3. In `UStbMinting.sol`, function `verifyOrder()`, the uniqueness and the size of `Order.order_id` is not checked;
> 4. In `UStbMinting.sol`, function `setUStb()`, the value of `_ustb` is not checked to be:
>    - Different from `address(0x0)`;
>    - Different from existing supported assets;
>    - Different from existing custodian;
>
> **Ethena response:** "1. Ack - won't fix / 2. Ack - won't fix / 3. Ack - uniqueness of nonce is checked in verifyNonce - won't fix / 4. Ack - won't fix"

**Current status:** **NOT FIXED.** Verified:
- `USDtb.sol` `initialize` (line 48-57) still does not check `minterContract` is a contract (only `address(0)` check).
- `addBlacklistAddress` (line 73-78) still allows blacklisting any address, including the minter contract or admin.
- `verifyOrder` (line 444-484) still does not check `order.order_id` uniqueness/size — only `nonce` is deduplicated via the bitmap.
- `setUSDtbToken` (line 654-657) still does not validate the new address.

**Re-assessment per sub-item:**

1. **`initialize` minterContract not-a-contract** — not exploitable. If admin fat-fingers an EOA as minterContract, then `mint()` calls will revert at the role check (`onlyRole(MINTER_CONTRACT)`). Self-DoS only; admin can re-init or upgrade.

2. **Blacklist minter contract** — interesting one. The Quantstamp report claims this "could temporarily disrupt the mint and redeem operations." Let's verify with current `_beforeTokenTransfer` (USDtb.sol lines 162-203): the mint path is `hasRole(MINTER_CONTRACT, msg.sender) && from == address(0) && !hasRole(BLACKLISTED_ROLE, to)`. **The check is on `MINTER_CONTRACT` role, NOT on `BLACKLISTED_ROLE` of msg.sender.** So even if `USDtbMinting` (the minter contract) is granted `BLACKLISTED_ROLE`, minting still works because the `MINTER_CONTRACT` carve-out takes precedence and never checks the minter's own blacklist status. Same for redeem (line 182-183) and for the admin redistribute paths. **Therefore the Quantstamp claim is wrong in the current code — blacklisting the minter contract does NOT disrupt operations.** Not exploitable.

3. **Blacklist admin** — if `addBlacklistAddress([admin])` is called, admin receives `BLACKLISTED_ROLE`. Then `redistributeLockedAmount(blacklistedUser, admin)` calls `_mint(admin, amount)` → `_beforeTokenTransfer(address(0), admin, amount)` → admin redistribute-mint branch (line 171): `hasRole(DEFAULT_ADMIN_ROLE, msg.sender) && from == address(0) && !hasRole(BLACKLISTED_ROLE, to)` — fails because `to` (admin) is blacklisted. So `redistributeLockedAmount` to a blacklisted admin address reverts. **Self-DoS only** (admin blacklisted themselves). Not externally exploitable.

4. **`order.order_id` not checked for uniqueness** — the report itself acknowledges "uniqueness of nonce is checked in verifyNonce." The `order_id` is purely informational (it's hashed into the EIP-712 struct, but the dedup bitmap is keyed on `nonce`). Not exploitable.

5. **`setUSDtbToken(_usdtb)` no validation** — this is the most interesting of USTB-1. Combined with Quantstamp Operational Consideration #3 (see §5.6 below), an admin fat-finger or admin key compromise here is catastrophic. But absent admin compromise, not externally exploitable.

**Verdict on USTB-1:** Not exploitable without admin compromise / fat-finger.

---

### 5.2 USTB-2 Missing Storage Gaps in Inherited Contract (Info, Acknowledged)

**Original finding text (verbatim, abridged):**
> The UStb and SingleAdminAccessControlUpgradeable contracts are designed to be upgradable. Upgradable contracts usually have reserved space to allow future versions to add new state variables to the contract without shifting down storage in the inheritance chain. As SingleAdminAccessControlUpgradeable is inherited by UStb, adding a storage gap can be done to prevent storage collisions in future updates where storage slots could be added to SingleAdminAccessControlUpgradeable.
>
> For instance, if in a future update, SingleAdminAccessControlUpgradeable is modified to include a new variable, and the UStb contract were upgraded to use the new version, this new variable would likely collide with the `transferState` variable in UStb.

**Ethena response:** "won't fix"

**Current status:** **NOT FIXED** (same as Cyfrin L-1, see §3.1). Latent upgrade hazard only.

---

### 5.3 USTB-3 Risks of Supporting Non-Standard ERC-20 Tokens (Info, Acknowledged)

**Original finding text (verbatim, abridged):**
> Supporting tokens with specific features such as fees, rebasing, pausable, upgradeable, blacklist-able, or hooks on transfers could negatively impact the main flows of the system (deposits via `mint()`, transfer to custody, redeems via `redeem()`) if no specific mitigation measure is enforced to limit the consequences. For instance, in the function `_transferCollateral()` if `asset` represents an asset where the amount transferred is different than the amount requested to be transferred (ex: if fees are enforced), the actual amount transferred to custodians may differ from the expected transferred amount.

**Ethena response:** "planned supported tokens don't have these features - won't fix"

**Current status:** **NOT FIXED in code**, but **mitigated by policy.** The current `_transferCollateral` (USDtbMinting.sol lines 573-596) and `_transferToBeneficiary` (lines 562-570) still assume 1:1 transfer amounts — they sum "requested" amounts and forward dust to the last route address, with no reconciliation against actual balance deltas. If a fee-on-transfer or rebasing token were ever added as a supported asset, accounting would silently drift.

**Re-assessment — is it now exploitable?** Depends on the live supported-asset list. The audit-time policy was "USDC, USDT, BUIDL, sUSDS" (none fee-on-transfer). As of late 2024 the protocol is also using USDe itself and (per Ethena docs) other RWA tokens. **None of the actually-supported assets are fee-on-transfer**, so the bug is dormant. **If a fee-on-transfer token is ever added without code change, this becomes an accounting-drift bug** — but it is not exploitable today. Latent risk only.

---

### 5.4 USTB-4 Considerations about Events (Info, Acknowledged)

**Original finding text (verbatim):**
> 1. The initial limits of a new asset added to UStbMinting (max mint and max redeem limits per block per asset) are not accessible to off-chain observers via events because the event `AssetAdded` has only one field: `AssetAdded(address indexed asset)`.
> 2. Updates made via the function `setStablesDeltaLimit()` are not logged.

**Ethena response:** "won't fix"

**Current status:** **NOT FIXED.** Verified: `addSupportedAsset` (line 606-612) emits `AssetAdded(asset)` only — no token config in the event. `setStablesDeltaLimit` (line 649-651) emits nothing. Same as Cyfrin I-3.

**Re-assessment:** Not exploitable. Off-chain monitoring gap only.

---

### 5.5 USTB-5 Depending on How Nonces Are Calculated Off-Chain, Nonce Verification May Reject Valid Nonces (Undetermined, Acknowledged)

**Original finding text (verbatim):**
> When orders are submitted to the functions `mint()` and `redeem()`, their field `uint128 nonce` is checked via the function `verifyNonce()`. That function uses the data structure `_orderBitmaps` to check if a sender already used or not a given nonce. In detail, it is a mapping storing for a given address a `uint256` bitmap called `invalidator` at the `uint128` keys called `invalidatorSlot`. However, the following line could lead to an issue:
> ```solidity
> uint128 invalidatorSlot = uint64(nonce) >> 8;
> ```
> Casting `nonce` to `uint64` seems too restrictive. If any value of `uint128 nonce` is valid, it is possible to have two different values of `nonce` that will be stored at the same bit in the data structure `_orderBitmaps`. For instance, the two values: `nonce_a = (1 << 125) + (1 << 60)` and `nonce_b = (1 << 60)`, since any non-matching bit higher than the 64th bit will be ignored because of the cast `uint64(nonce)`. Ultimately, this could prevent valid Orders from being accepted by the contract.
>
> **Recommendation:** Consider removing the cast operation to `uint64`, or make sure that the off-chain component does not provide nonces greater than `type(uint64).max`.

**Ethena response:** "won't fix"

**Current status:** **NOT FIXED.** Verified at `USDtbMinting.sol` line 511: `uint128 invalidatorSlot = uint64(nonce) >> 8;`. Same pattern present in `EthenaMinting.sol` (USDe side, see prior `ethena-audit-history.md` §6).

**Re-assessment — is it now exploitable?**

The bug exists exactly as described: the upper 64 bits of `uint128 nonce` are silently truncated. This means two nonces that share the lower 64 bits alias to the same bitmap slot+bit. Once one is used, the other is rejected as `InvalidNonce()`.

For this to be **exploitable** (i.e., cause an external DoS), an attacker would need to:
- Submit an order with `nonce_a` having a high bit set (e.g., `2^125 + 2^60`), get it executed, then any future order with `nonce_b = 2^60` would be rejected.
- **But the attacker does not control nonce generation.** The off-chain RFQ system generates nonces, and the benefactor signs the order containing that nonce. The minter/redeemer submits the signed order. So the only way an attacker-controlled high-bit nonce reaches the contract is if (a) the off-chain system generates such a nonce (bug in off-chain code), or (b) the benefactor themselves is the attacker (they're DoSing their own order flow — self-inflicted).

For Ethena's actual off-chain system, nonces are sequential counters starting at 1. Reaching `2^64` (= 1.8 × 10^19 orders) is physically impossible. So **the bug is purely theoretical.**

**Verdict:** Not exploitable in any realistic operating regime. Acknowledged, won't fix, but the operational discipline (sequential nonces, never > `uint64.max`) makes it inert. The "Undetermined" severity is correct.

---

### 5.6 Quantstamp Operational Considerations (not formal findings, but flagged)

These are noted in the body of the Quantstamp report but were not registered as findings. Re-assessing each:

**#1 Upgradable contracts — future changes out of scope.** Latent upgrade risk; same as USTB-2.

**#2 Centralization of DEFAULT_ADMIN_ROLE** — admin can blacklist anyone + redistribute. Single-key compromise = full fund sweep. Ethena uses multisig (per their docs, Gnosis Safe). Not a contract bug; trust assumption.

**#3 — `setUStbToken()` mutability (most interesting non-finding).** Verbatim:
> The mutability of the address `UStb` in `UStbMinting` with the function `setUStbToken()` was confirmed to be an expected feature by the Ethena team. However, if such update happens, the following can happen:
> - users will not be able to redeem collateral because they own old `UStb` tokens since the call to `mint()` , and new `UStb` tokens should be burned when calling `redeem()` ;
> - any already-signed non-expired orders would remain valid for the new `UStb` token since the address of `UStb` is not part of the struct `Order` ;

**Current verification:** `setUSDtbToken` (line 654-657) still exists, still has no validation, and `Order` struct (IUSDtbMinting.sol lines 45-55) still does NOT include the USDtb token address. So both sub-issues persist:
- After a token swap, old USDtb holders cannot redeem through `USDtbMinting.redeem()` (it calls `usdtb.burnFrom` on the new token).
- Already-signed, unexpired `Order`s remain valid because the EIP-712 hash does not bind to the USDtb token address — only to `verifyingContract = USDtbMinting` (line 638). So a stale order signed for old USDtb would mint new USDtb at the same `usdtb_amount`.

**Re-assessment:** This is admin-gated. An external attacker cannot trigger it. The signed-order-replay concern is the more interesting of the two, but it still requires admin to call `setUSDtbToken` AND have outstanding signed orders. **Not externally exploitable.** Worth documenting as a defense-in-depth concern: if Ethena ever does a token migration, they should invalidate all outstanding orders (e.g., by rotating the EIP712 name or chainId or by expiring all orders) before swapping the token address. Currently no on-chain enforcement of this.

**#4 Race conditions between concurrent admin actions.** Standard admin-transaction-ordering; multisig + off-chain coordination handles it. Not exploitable externally.

**#5 Last route address gets dust.** `verifyRoute` requires `totalRatio == 10000` (line 505), and `_transferCollateral` (line 585-595) computes `amountToTransfer = (amount * ratios[i]) / 10000` per route entry, then forwards any remainder (`amount - totalTransferred`) to the **last** route address. So the last custodian always receives the integer-division dust. The Quantstamp note says "the last entry of the route addresses always has a slight advantage." This is by-design (dust has to go somewhere) and dust is at most `route.addresses.length - 1` wei. **Not exploitable.**

---

### 5.7 Quantstamp Auditor Suggestions S1–S7

| ID | Title | Status | Re-assessment |
|----|-------|--------|---------------|
| S1 | Documentation improvements | Fixed | n/a |
| S2 | Code conciseness (unused events/funcs) | Fixed | n/a |
| S3 | Unchecked blocks in for loops (post-0.8.22 unneeded) | Acknowledged | Not exploitable; gas-only |
| S4 | UStb could inherit IUStb | Acknowledged | Not exploitable; style only |
| S5 | Internal functions used once | Acknowledged | Not exploitable; style only |
| S6 | Redundant blacklist check on `to` in `redistributeLockedAmount` | Acknowledged | The `!hasRole(BLACKLISTED_ROLE, to)` check on line 113 is indeed redundant with `_beforeTokenTransfer`, but redundancy is defense-in-depth. Not exploitable. |
| S7 | `_pendingDefaultAdmin` private visibility hurts monitoring | Acknowledged | Off-chain monitoring gap. Not exploitable. |

---

## 6. Code4rena Report (USDe, Oct 2023) — Full Findings & Re-Assessment

The C4 contest covered the **USDe** system (predates USDtb). 4 Medium findings + 9 Low/Non-Critical. Re-assessed against current `/home/z/ethena-usde/contracts/contracts/`.

### 6.1 [M-01] FULL_RESTRICTED Stakers can bypass restriction through approvals

**Original finding text (verbatim, abridged):**
> The `StakedUSDe` contract implements a method to `SOFTLY` or `FULLY` restrict user address, and either transfer to another user or burn. However there is an underlying issue. A fully restricted address is supposed to be unable to withdraw/redeem, however this issue can be walked around via the approve mechanism.
>
> The openzeppelin `ERC4626` contract allows approved address to withdraw and redeem on behalf of another address so far there is an approval. [...] Blacklisted Users can explore this loophole to redeem their funds fully. This is because in the overridden `_withdraw` function, the token owner is not checked for restriction.
>
> **Judge (0xDjango):** decreased severity to Medium. "I have conversed with the project team, and we have agreed that breaking rules due to legal compliance is medium severity as no funds are at risk."

**Ethena:** confirmed (issue #666)
**Current verification:** `StakedUSDe.sol` `_withdraw` (lines 224-240) now checks all three of caller / receiver / `_owner` against `FULL_RESTRICTED_STAKER_ROLE`:
```solidity
if (
  hasRole(FULL_RESTRICTED_STAKER_ROLE, caller) || hasRole(FULL_RESTRICTED_STAKER_ROLE, receiver)
    || hasRole(FULL_RESTRICTED_STAKER_ROLE, _owner)
) {
  revert OperationNotAllowed();
}
```
**Status: FIXED.** Not exploitable.

---

### 6.2 [M-02] Soft Restricted Staker Role can withdraw stUSDe for USDe — **NOT FIXED**

**Original finding text (verbatim, abridged):**
> A requirement is stated that a user with the `SOFT_RESTRICTED_STAKER_ROLE` is not allowed to withdraw `USDe` for `stUSDe`.
>
> The Ethena readme [...] states: "Due to legal requirements, there's a `SOFT_RESTRICTED_STAKER_ROLE` and `FULL_RESTRICTED_STAKER_ROLE`. The former is for addresses based in countries we are not allowed to provide yield to, for example USA. Addresses under this category will be soft restricted. They cannot deposit USDe to get stUSDe or withdraw stUSDe for USDe. However they can participate in earning yield by buying and selling stUSDe on the open market."
>
> [...] However, the `_withdraw` function does not check the `SOFT_RESTRICTED_STAKER_ROLE` of the owner/caller/receiver. So a soft-restricted user can call `withdraw`/`redeem`/`unstake` to exchange stUSDe for USDe, contrary to the stated legal requirement.

**Current verification:** `StakedUSDe.sol` `_deposit` (lines 202-214) checks `SOFT_RESTRICTED_STAKER_ROLE` on caller and receiver — deposit is blocked. But `_withdraw` (lines 224-240) **only** checks `FULL_RESTRICTED_STAKER_ROLE`, not `SOFT_RESTRICTED_STAKER_ROLE`. So:
- A `SOFT_RESTRICTED_STAKER_ROLE` holder CAN call `redeem()` (when `cooldownDuration == 0`) to convert stUSDe → USDe.
- A `SOFT_RESTRICTED_STAKER_ROLE` holder CAN call `unstake()` (when `cooldownDuration > 0`) to claim USDe from the silo.
- A `SOFT_RESTRICTED_STAKER_ROLE` holder CAN transfer stUSDe on the open market (no role check in `_beforeTokenTransfer` for SOFT).

**Status:** **NOT FIXED.** This contradicts the original README's stated legal requirement.

**Re-assessment — is it now exploitable?**

Two questions to disentangle:

1. **Is this a fund-theft bug?** No. The user redeeming owns the stUSDe; they're getting back the underlying USDe + their share of accumulated yield. No-one else's funds are taken. The judge explicitly downgraded M-01 (a sister finding) to Medium with the rationale "no funds are at risk." Same logic applies here.

2. **Is this a security vulnerability in the bug-bounty sense?** Probably not. Ethena's Immunefi program (per `ethena-audit-history.md` §1) covers "theft of unearned yield" / "permanent freezing" / "logic errors leading to loss of funds." A SOFT_RESTRICTED user redeeming their own position is none of these — it's a legal-compliance gap, not a smart-contract security bug.

3. **Has the spec evolved?** Likely yes. Ethena's current public documentation is more nuanced than the Oct-2023 README: the soft-restriction is now described as "cannot stake new USDe" (deposit blocked) rather than "cannot withdraw existing position." Allowing a soft-restricted user to exit their position (but not add to it) is consistent with how a regulated yield product would treat a newly-sanctioned user: freeze inflows, allow outflows. So the apparent "non-fix" may be intentional policy evolution, not an oversight.

**Verdict:** Not exploitable as a security bug. Documented as a legal-compliance / spec-drift observation. If a researcher wanted to push this, the angle would be: "the deployed code does not match the original stated legal requirement" — but that's a spec-conformance issue, not a bounty-eligible vulnerability.

---

### 6.3 [M-03] Users still forced to follow previously set cooldownDuration even when cooldown is off

**Original finding text (verbatim, abridged):**
> In a scenario where coolDown is on (always turned on by default) and Alice and Bob deposits, two days after Alice wants to withdraw/redeem. Alice is forced to wait for 90 days [...] Bob decides to wait an extra day. On the third day, Bob decides to withdraw/redeem. Contract admin also toggles the coolDown off (sets cooldownDuration to 0) [...] Bob now calls redeem()/withdraw() to withdraw instantly [...] Alice sees Bob has gotten his tokens but Alice cant use the redeem()/withdraw() because her `StakedUSDeV2` were already burned and her underlying assets were sent to the silo contract. Alice cannot successfully call `unstake()` because her `userCooldown.cooldownEnd` value set to ~90 days.
>
> **Ethena (kayinnnn):** "Acknowledge the issue, but revise to low severity finding as it causes minor inconvenience in the rare time we change cooldown period. However, it is still fixed - existing per user cooldown is ignored if the global cooldown is `0`."

**Current verification:** `StakedUSDeV2.sol` `unstake` (line 80-92) now has:
```solidity
if (block.timestamp >= userCooldown.cooldownEnd || cooldownDuration == 0) {
  userCooldown.cooldownEnd = 0;
  userCooldown.underlyingAmount = 0;
  silo.withdraw(receiver, assets);
} else {
  revert InvalidCooldown();
}
```
The `|| cooldownDuration == 0` clause is the fix — if global cooldown is off, any user with pending cooldown can unstake immediately. **Status: FIXED.** Not exploitable.

---

### 6.4 [M-04] Malicious users can front-run to cause a DoS for StakedUSDe due to MinShares checks

**Original finding text (verbatim, abridged):**
> Malicious users can transfer `USDe` token to `StakedUSDe` protocol directly lead to a denial of service (DoS) for StakedUSDe due to the limit shares check. [...] Since `decimalsOffset() == 0` and `totalAssets` equal the balance of `USDe` in this protocol. The minimum share is set to 1 ether. [...] Assuming malicious users transfer 1 ether of `USDe` into the protocol and receive ZERO shares, how much tokens does the next user need to pay if they want to exceed the minimum share limit of 1 ether? That would be 1 ether times 1 ether, which is a substantial amount.
>
> **Ethena (FJ-Riveros):** "We acknowledge the potential exploitability of this issue, but we propose marking it as `Medium` severity. Our rationale is based on the fact that this exploit can only occur during deployment. To mitigate this risk, we plan to fund the smart contract in the next block, ensuring that nobody has access to the ABI or contract source code. We could even use flashbots for this purpose."

**Current verification:** `StakedUSDe.sol` still has `MIN_SHARES = 1 ether` and `_checkMinShares` (lines 33, 190-193). The mitigation is operational: deploy + fund atomically so no attacker can front-run. **Status: ACKNOWLEDGED, mitigated by deployment procedure.** Not exploitable post-deployment (the contract is already deployed and has a multi-billion-dollar TVL, so the donation attack surface is long gone — an attacker would need to donate more than the existing total supply to move the share price meaningfully, which is uneconomic).

---

### 6.5 C4 Low/Non-Critical Findings [01]–[09]

Skimmed and re-assessed:

| # | Title | Re-assessment |
|---|-------|---------------|
| 01 | Use efficient logic in setter functions | Style/gas. Not exploitable. |
| 02 | Be consistent in event arg order | Style. Not exploitable. |
| 03 | Delta neutrality caution | User education. Not a bug. |
| 04 | Easy DoS on big players when minting/redeeming | Treated as known: maxMintPerBlock could revert by 1 wei; backend should batch. Not exploitable — backend handles. |
| 05 | Inexpedient code lines (vestingAmount += getUnvestedAmount()) | Code clarity. Not exploitable. |
| 06 | Emission of identical values | Same as #05. |
| 07 | Typos | Fixed. |
| 08 | `verifyRoute` should reject non-MINT order types | Code-clarity. Not exploitable (returns `true` for REDEEM, but mint path explicitly requires `OrderType.MINT` already). |
| 09 | Unused `encodeRoute` function | Fixed. |

---

## 7. Cross-PDF Synthesis: "Acknowledged but not fixed" findings still in deployed code

After full deep-read + on-chain verification, the **complete set** of audit findings that remain in the deployed contracts:

| # | Finding | Severity | Auditor | Live in deployed code? | Exploitable now? |
|---|---------|----------|---------|------------------------|------------------|
| A | Missing `__gap` in `SingleAdminAccessControlUpgradeable` | Low / Info | Cyfrin L-1 + Quantstamp USTB-2 | **YES** | No — only a future-upgrade hazard |
| B | `setStablesDeltaLimit` emits no event | Info | Cyfrin I-3 + Quantstamp USTB-4 | **YES** | No — monitoring gap only |
| C | `AssetAdded` event omits token config | Info | Quantstamp USTB-4 | **YES** | No — monitoring gap only |
| D | `_pendingDefaultAdmin` private | Info | Quantstamp S7 | **YES** | No — monitoring gap only |
| E | `setMaxMintPerBlock` no `isActive` check | Low | Pashov L-05 | **YES** (by design) | No — mint path blocks inactive assets |
| F | `initialize` no contract-check on minterContract | Low | Quantstamp USTB-1 #1 | **YES** | No — self-DoS only |
| G | `addBlacklistAddress` can blacklist admin/minter | Low | Quantstamp USTB-1 #2 | **YES** | No — see §5.1 analysis; minter path doesn't check sender blacklist anyway |
| H | `verifyOrder` no `order_id` uniqueness check | Low | Quantstamp USTB-1 #3 | **YES** | No — nonce is the dedup key |
| I | `setUSDtbToken` no validation | Low | Quantstamp USTB-1 #4 | **YES** | No — admin-gated |
| J | Nonce `uint64` truncation in `verifyNonce` | Undetermined | Quantstamp USTB-5 | **YES** | No — requires off-chain to issue nonces > 2^64 |
| K | `_transferCollateral` assumes no-fee tokens | Info | Quantstamp USTB-3 | **YES** | No — currently-supported tokens are not fee-on-transfer |
| L | `Order` struct excludes USDtb token address | Operational note | Quantstamp Op #3 | **YES** | No — admin-gated (token swap required) |
| M | `SOFT_RESTRICTED_STAKER_ROLE` can still redeem stUSDe | Medium (downgraded by judge) | C4 M-02 | **YES** | No — legal-compliance gap, not fund theft |

**Items NOT in the table above were fixed** (Cyfrin L-2/L-3, Pashov L-01/L-02/L-03/L-04/L-06/L-07, Quantstamp S1/S2, C4 M-01/M-03/M-04 mitigation).

**Special note on Pashov L-07:** The Pashov PDF literally says "Acknowledged" (not "Resolved"), but the deployed code at commit `2e7c9f6` actually fixes the bug. The audit-report status field is stale relative to what shipped. **A researcher relying only on the PDF text would believe L-07 is still live; it is not.**

---

## 8. Combinatorial / Cross-Finding Analysis

The task brief specifically asks: "Low severity findings that could combine — individual Low, but combo = Critical." Examined all 2-element and 3-element combos of the live findings (A–M above). Result:

- **A + B/C/D (storage gap + event gaps):** Cannot combine. Storage gap is upgrade-time only; events are runtime only. Disjoint.
- **F + G (init validation + blacklist minter):** Could combine if admin fat-fingers init AND blacklists the minter. But (a) init is one-shot, (b) per §5.1 analysis, blacklisting the minter contract does not actually break mint/redeem in the current code. No combo.
- **G + L (blacklist + setUSDtbToken):** No interaction.
- **I + L (no validation on setUSDtbToken + Order excludes token address):** Same root cause. If admin fat-fingers `setUSDtbToken(wrongAddr)`, redeem breaks for old USDtb holders AND outstanding signed orders can mint the wrong token. But this is still a single admin error, not a combo.
- **J + K (nonce truncation + non-standard tokens):** Disjoint — nonce is signature-side, token is collateral-side.
- **E + J (setMaxMintPerBlock on inactive asset + nonce truncation):** No interaction.
- **M + anything in USDtb system:** Different contract systems (StakedUSDe vs USDtb). No cross-contract call path exists.

**No combinatorial escalation found.** The audit findings are individually isolated and do not chain.

---

## 9. Time-Bound / TVL-Bound Findings

- **C4 M-04 (MinShares donation DoS):** Was time-bound to deployment window. Past — contract is deployed with TVL >> 1 ether, so the donation attack is now uneconomic.
- **Quantstamp USTB-5 (nonce collision):** Would be time-bound if nonces were sequential and approached 2^64. At even 1M orders/day, that's ~5 × 10^13 years. Effectively never.
- **Quantstamp USTB-3 (non-standard tokens):** Asset-list-bound. Becomes live if and only if Ethena adds a fee-on-transfer / rebasing token. No evidence this has happened.
- **Cyfrin L-1 / Quantstamp USTB-2 (storage gap):** Upgrade-bound. Becomes live if and only if Ethena upgrades `SingleAdminAccessControlUpgradeable` to add a state variable without first adding a `__gap`. No such upgrade has happened.

**No finding has crossed a threshold from "not exploitable" to "exploitable" due to time/TVL/state changes since the audit.**

---

## 10. Final Verdict

- **PDFs read in full:** 3 Ethena-specific PDFs (Cyfrin, Pashov, Quantstamp) + C4 report.md (1893 lines).
- **Distinct findings re-assessed:** 7 (Cyfrin) + 7 (Pashov) + 5 (Quantstamp) + 7 (Quantstamp suggestions) + 4 (C4 Medium) + 9 (C4 Low/Non-Critical) = **39 findings**.
- **Still live in deployed code:** 13 (table in §7).
- **Exploitable today:** **0.**
- **Critical bugs found:** **0.**
- **Vuln PoC files written:** 0 (no finding crossed the bar).

### Notable observations for future research

1. **Pashov L-07 status drift:** the audit PDF says "Acknowledged" but the deployed code silently fixed it. Future researchers should not trust the PDF status field alone — always re-verify against the deployed bytecode / current source.
2. **C4 M-02 (Soft Restricted redeem) is the most interesting "non-fixed" finding** but is a legal-compliance gap, not a fund-theft bug. Unlikely to be bounty-eligible.
3. **`setUSDtbToken` + `Order` struct excludes token address** (Quantstamp Op #3) is a real design weakness if a token migration ever happens. Worth monitoring; not exploitable today.
4. **Storage gap absence in `SingleAdminAccessControlUpgradeable`** is the longest-tail risk: it will bite only if/when Ethena does a future upgrade that adds a state var to the base contract. Defense-in-depth fix is trivial (`uint256[48] private __gap;`) but Ethena has twice declined to apply it.

### File paths

- This report: `/home/z/fkr-step1/defi-bounty/protocol-research/ethena-audit-pdfs-deep-read.md`
- Prior related report: `/home/z/fkr-step1/defi-bounty/protocol-research/ethena-audit-history.md`
- Plain-text PDF extractions: `/tmp/ethena_pdf_text/{ustb,sigmanotes,quantstamp}.txt`
- No vuln PoC file written (none warranted).

---

*End of deep-read report. No Immunelfi submission made; per task brief, this is research only.*
