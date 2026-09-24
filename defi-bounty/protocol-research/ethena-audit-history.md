# Ethena Protocol — Audit History & Unfixed-Finding Analysis

**Task ID:** eth-audit-reports
**Agent:** Opus
**Date:** 2026-09-24
**Purpose:** Catalogue public Ethena audit reports, verify each HIGH/MEDIUM/LOW finding against the *current* in-scope contracts, and flag any finding that appears unfixed and still exploitable (bug-bounty research only — nothing submitted).

---

## 0. TL;DR / Honest Conclusion

- **6 public audit artifacts located** for Ethena (1 Code4rena contest for USDe core + 3 PDF reports for USDtb + Spearbit/Cantina summary on docs.ethena.fi + pre-audit questionnaires in-repo).
- **Findings analyzed:** 4 Medium (Code4rena USDe) + 7 Low (Pashov USDtb) + 3 Low / 4 Info (Cyfrin USDtb) + 1 Low / 1 Undetermined / 3 Info + 7 suggestions (Quantstamp USDtb) = **~26 distinct findings**.
- **Outcome:** All of the genuinely impactful findings are **FIXED** in the current contracts. The remaining "unfixed" items are explicitly **Acknowledged / Disputed** design decisions or deployment-time mitigations, and **none are exploitable for a fund-theft/freezing bounty impact today.**
- **No separate vuln PoC file was written**, because no unfixed finding met the bar of "appears exploitable / bounty-eligible." The closest candidate (EIP-712 nonce `uint64` truncation, present in *both* `EthenaMinting` and `USDtbMinting`) is documented below in §6 and judged **not** bounty-eligible (false-rejection only, already Acknowledged).
- **Highest-value research direction going forward** is the *unaudited post-audit attack surface* (§7): `EthenaMinting.mintWETH` / `_transferEthCollateral`, native-ETH `transferToCustody`, and the delegated-signer flow were all added after the Oct-2023 Code4rena scope and have **no public audit coverage**. No concrete exploit was found in the time available, but this is where further effort is best spent.

---

## 1. Audit Reports Found

| # | Auditor | Scope | Date | Source / URL | Local copy |
|---|---------|-------|------|--------------|------------|
| 1 | **Code4rena** (contest #299, judge 0xDjango) | USDe core: `USDe.sol`, `EthenaMinting.sol`, `SingleAdminAccessControl.sol`, `StakedUSDeV2.sol` (+ `StakedUSDe.sol`, `USDeSilo.sol`) | Oct 24–30 2023 (report Dec 21 2023) | https://github.com/code-423n4/2023-10-ethena-findings | `/home/z/ethena-c4-findings/` |
| 2 | **Spearbit** (x Cantina) | USDe core | completed 18 Oct 2023 | https://docs.ethena.fi/ethena/security/audits — "Spearbit x Ethena - Audit" | not downloadable (JS-rendered page); summary says "no critical or high issues" |
| 3 | **Code4rena** (Ethena Labs Invitational) | USDtb | report dated 2024-12-02 | https://code4rena.com/contests (Ethena Labs Invitational Findings & Analysis Report) | overlaps with PDFs below |
| 4 | **Pashov Audit Group** (T1MOH, btk, peanuts) | USDtb (`UStb`, `UStbMinting`, `SingleAdminAccessControl*`) — commit `ae1856b5` | Oct 17–20 2024 | `Ethena-security-review-October.pdf` | `/home/z/ethena-usdtb/Ethena-security-review-October.pdf` → `/tmp/october.txt` |
| 5 | **Cyfrin** (lead: Immeas) | USDtb — commit `d82676f` | Oct 23 – Nov 1 2024 | `2024-10-31-ethena-ustb-v1.0.pdf` | `/home/z/ethena-usdtb/2024-10-31-ethena-ustb-v1.0.pdf` → `/tmp/ustb.txt` |
| 6 | **Quantstamp** (V. Callens, R. Rohleder, R. Islam) | USDtb (`UStb`, `UStbMinting`) — commit `d82676f` | Oct 23–25 2024 | `Ethena_final_report_Quantstamp.pdf` | `/home/z/ethena-usdtb/Ethena_final_report_Quantstamp.pdf` → `/tmp/quantstamp.txt` |

Additional references: pre-audit questionnaires in `/home/z/ethena-usde/contracts/audit/AUDIT_{STAKING,LENDING,MINT}.md` (these are intake forms, not findings). Ethena's own summary (Blockworks/Messari/Balancer BIP-583) confirms a multi-phase program: **Zellic → Spearbit & Cantina → Quantstamp → Pashov → Code4rena → Chaos Labs (economic)**.

**Immunefi program:** https://immunefi.com/bounty/ethena/ — Live since 04 Apr 2024, "Triaged by Immunefi", KYC + PoC required, vault `0xCd3a85aB5aF518370bc5e679C043BBE0AED1F6E5` (~12.5k USDT). Smart-contract Critical max **$3,000,000** (10% of funds at risk), Primacy-of-Impact High $10k–$75k, Medium flat $10k, Low flat $2.5k.

**Bug-bounty payout history:** No public Immunefi disclosure of a paid Ethena smart-contract critical/high was found via web search. The program is "Triaged by Immunefi" with KYC, so any payouts would be under NDA / not publicly disclosed. Treat the absence of public disclosures as *unknown*, not *zero*.

---

## 2. Current Contract Baselines Used for Verification

| Repo | Origin | Role |
|------|--------|------|
| `/home/z/ethena-usde` | `github.com/ethena-labs/bbp-public-assets` | Ethena's bug-bounty public-asset snapshot — `USDe.sol`, `EthenaMinting.sol`, `StakedUSDe.sol`, `StakedUSDeV2.sol`, `USDeSilo.sol`, `EthenaLPStaking.sol`, `StakingRewardsDistributor.sol`, `EthenaBalancerRateProvider.sol` |
| `/home/z/ethena-usdtb` | `github.com/ethena-labs/ethena-usdtb-contest` (commit `2e7c9f6`) | USDtb contest snapshot — `USDtb.sol`, `USDtbMinting.sol`, `SingleAdminAccessControl{,Upgradeable}.sol` |
| `/home/z/ethena-c4-findings` | `github.com/code-423n4/2023-10-ethena-findings` | Full warden submission set + judge's `report.md` |

Code4rena contest scope commit: `ee67d9b542642c9757a6b826c82d0cae60256509` (588 LoC, 6 contracts).

---

## 3. Code4rena (USDe core, Oct 2023) — Findings & Fix Status

Aggregated result: **0 Critical, 4 Medium, 98 Low/non-critical, 41 gas**. The 4 Mediums are the only materially impactful findings; each was checked against the current `StakedUSDe.sol` / `StakedUSDeV2.sol`.

### M-01 — `FULL_RESTRICTED` stakers can bypass restriction through approvals
- **Vuln:** `_withdraw(caller, receiver, _owner, …)` only reverted if `caller` or `receiver` had `FULL_RESTRICTED_STAKER_ROLE`. A fully-restricted `owner` could `approve` a fresh EOA, which then called `redeem(shares, receiver, owner)` to pull USDe out — bypassing the freeze.
- **Current code** (`StakedUSDe.sol:224-240`):
  ```solidity
  if (
    hasRole(FULL_RESTRICTED_STAKER_ROLE, caller) || hasRole(FULL_RESTRICTED_STAKER_ROLE, receiver)
      || hasRole(FULL_RESTRICTED_STAKER_ROLE, _owner)   // <-- added
  ) { revert OperationNotAllowed(); }
  ```
- **Status: FIXED.** The `_owner` term was added. `_beforeTokenTransfer` (lines 253-260) also still blocks transfers *from* restricted addresses (except burn-to-0), so the restricted owner cannot relocate shares to a fresh account first. Bypass is closed.

### M-02 — `SOFT_RESTRICTED_STAKER_ROLE` can withdraw stUSDe for USDe
- **Vuln:** `_withdraw` never checked `SOFT_RESTRICTED_STAKER_ROLE`, so a soft-restricted (e.g. US-sanctioned) holder could redeem stUSDe→USDe, contrary to the README's stated legal requirement.
- **Ethena's response:** **Disputed.** "The docs were incorrect to say withdrawal by soft restricted role is not allowed. Only depositing is not allowed."
- **Current code:** `_deposit` (lines 202-214) blocks `SOFT_RESTRICTED_STAKER_ROLE` on `caller`/`receiver`; `_withdraw` (224-240) does **not** check soft-restricted. Matches the disputed/intended behavior.
- **Status: DISPUTED / WON'T-FIX (by design).** The in-repo README still contains the old (now-contradicted) wording ("They cannot deposit USDe to get stUSDe or withdraw stUSDe for USDe"), so the doc/code mismatch persists, but this is a known, public, Ethena-rejected issue. **Not a fresh bounty target** — re-submitting would be a duplicate of a disputed public finding, and the impact (compliance) is not an Immunefi fund-theft/freezing category.

### M-03 — Users forced to honour previously-set cooldown even after cooldown is turned off
- **Vuln:** Once `cooldownDuration` was set to 0, users who had already called `cooldownAssets/Shares` (and whose USDe was sitting in the `USDeSilo` with `cooldownEnd` ~90 days out) could not `unstake` until their personal cooldown expired — even though new users could withdraw instantly.
- **Current code** (`StakedUSDeV2.sol:80-92`):
  ```solidity
  if (block.timestamp >= userCooldown.cooldownEnd || cooldownDuration == 0) {   // <-- "|| cooldownDuration == 0" added
      ... silo.withdraw(receiver, assets);
  } else { revert InvalidCooldown(); }
  ```
- **Status: FIXED.** The `|| cooldownDuration == 0` clause lets already-cooling-down users claim immediately once the global cooldown is disabled.

### M-04 — DoS of StakedUSDe via MinShares check (donation attack)
- **Vuln:** A direct transfer of ~1 USDe to an empty `StakedUSDe` vault inflates `totalAssets` while `totalSupply=0`; the first depositor then receives < `MIN_SHARES` (1e18) shares and `_checkMinShares()` reverts. A 1-ether donation bricks deposits until an astronomically large deposit is made.
- **Current code:** `MIN_SHARES = 1 ether` and `_checkMinShares()` are **still present** (`StakedUSDe.sol:33-34, 190-193, 213, 239`).
- **Ethena's response:** **Acknowledged.** "This exploit can only occur during deployment. To mitigate this risk, we plan to fund the smart contract in the next block, ensuring that nobody has access to the ABI or contract source code."
- **Status: ACKNOWLEDGED / OPERATIONALLY MITIGATED — NOT LIVE.** The deployed sUSDe vault already has a multi-billion-USD `totalSupply`, so a donation now cannot drive `totalSupply < MIN_SHARES`. The residual risk is redeployment/clone deployments only. **Not a bounty target on the live contract.**

### Low/non-critical (98) & gas (41)
Spot-checked the top low report (`0xmystery`). All are code-quality / input-validation / event-emission nits; none represent a fund-loss vector. Not individually enumerated here.

---

## 4. Pashov Audit Group (USDtb, Oct 17–20 2024) — Findings & Fix Status

Scope commit `ae1856b5`; fixes reviewed at `c2256464`. **7 Low, 0 Med/High.** All checked against current `USDtb.sol` (`_beforeTokenTransfer` was substantially rewritten).

| ID | Title | Status (Pashov) | Re-verified in current `USDtb.sol` |
|----|-------|-----------------|-------------------------------------|
| L-01 | Unused variables (`EIP712_DOMAIN_TYPEHASH`, `ROUTE_TYPE`) in `UStbMinting` | Resolved | n/a (cosmetic) |
| L-02 | Blacklisted tokens transferable during `WHITELIST_ENABLED` | Resolved | **FIXED** — `WHITELIST_ENABLED` branch now checks `BLACKLISTED_ROLE` on `from`/`to`/`msg.sender` across all sub-cases (lines 181-202) |
| L-03 | `ORDER_TYPE` EIP-712 typestring had `uint128 expiry`/`uint120 nonce` swapped vs struct | Resolved | **FIXED** — `USDtbMinting.sol:32-33` typestring now `uint120 expiry,uint128 nonce` matching the struct |
| L-04 | `_beforeTokenTransfer` should reject an address holding both WHITELISTED + BLACKLISTED | Resolved | **FIXED** — `addBlacklistAddress` revokes `WHITELISTED_ROLE` (line 75); `addWhitelistAddress` skips blacklisted (line 94) |
| L-05 | `setMaxMintPerBlock`/`setMaxRedeemPerBlock` should check `tokenConfig[asset].isActive` | Resolved | (admin-only config hardening) |
| L-06 | Blacklisted users bypass restriction via approvals | Resolved | **FIXED** — current normal-case check requires `!BLACKLISTED(from)` so an approved operator cannot move a blacklisted owner's tokens |
| L-07 | Blacklisted users can front-run `redistributeLockedAmount` and burn via approved burner | Acknowledged | **Effectively closed** by the rewrite: a blacklisted `from` cannot burn (no `_beforeTokenTransfer` branch matches), so front-running the redistribution with a `burnFrom` now reverts |

---

## 5. Cyfrin (USDtb, Oct 23 – Nov 1 2024) — Findings & Fix Status

Scope commit `d82676f`. **3 Low, 4 Informational.**

| ID | Title | Status | Re-verified |
|----|-------|--------|-------------|
| L-1 | Missing storage gap in `SingleAdminAccessControlUpgradeable` | Open | Still open (cosmetic, acknowledged) |
| L-2 | USDtb cannot be burnt when `WHITELIST_ENABLED` | Resolved (PR#10) | **FIXED** — `WHITELIST_ENABLED` branch now allows whitelisted `from`→`address(0)` burn (lines 190-191) |
| L-3 | Non-whitelisted users can transfer via whitelisted intermediary | Resolved (PR#10) | **FIXED** — normal-case now requires `WHITELISTED_ROLE` on `msg.sender` **and** `from` **and** `to` (lines 192-195) |
| I-1..I-4 | empty foundry.toml, typos, missing event on `setStablesDeltaLimit`, unused events/errors | Open | cosmetic |

---

## 6. Quantstamp (USDtb, Oct 23–25 2024) — Findings & Fix Status

Scope commit `d82676f`. **0 High, 0 Medium, 1 Low, 1 Undetermined, 3 Informational** (+ 7 auditor suggestions). All marked Acknowledged by Ethena.

| ID | Title | Severity | Status | Notes |
|----|-------|----------|--------|-------|
| USTB-1 | Missing input validations (`initialize` minter-is-contract; blacklisting minter/admin; `order_id` uniqueness/size; `setUStb` zero/duplicate checks) | Low | Acknowledged | admin/off-chain trust assumptions |
| USTB-2 | Missing storage gaps in `SingleAdminAccessControlUpgradeable` | Info | Acknowledged | cosmetic |
| USTB-3 | Risks of supporting non-standard ERC-20 (fee/rebasing tokens) in `UStbMinting` | Info | Acknowledged | "planned supported tokens don't have these features" |
| USTB-4 | Missing events (`AssetAdded` lacks limit fields; `setStablesDeltaLimit` not logged) | Info | Acknowledged | observability |
| **USTB-5** | **EIP-712 nonce `uint64` truncation** — `uint128 invalidatorSlot = uint64(nonce) >> 8;` discards bits ≥64, so two distinct `uint128` nonces sharing low-64 bits collide on the same bitmap bit | Undetermined | Acknowledged ("won't fix") | see analysis below |
| S1-S7 | Doc fixes, code conciseness, unchecked blocks, interface inheritance, redundant checks, private `_pendingDefaultAdmin` | — | Fixed / Acknowledged | cosmetic |

### USTB-5 deep-dive (the one truly "unfixed-in-code" item — and why it is NOT a bounty target)

The identical pattern exists in **both** minting contracts:

- `EthenaMinting.sol:443-451` (USDe, `nonce` is `uint256`):
  ```solidity
  function verifyNonce(address sender, uint256 nonce) public view override returns (uint256, uint256, uint256) {
      if (nonce == 0) revert InvalidNonce();
      uint256 invalidatorSlot = uint64(nonce) >> 8;   // <-- truncates to low 64 bits
      uint256 invalidatorBit = 1 << uint8(nonce);
      uint256 invalidator = _orderBitmaps[sender][invalidatorSlot];
      if (invalidator & invalidatorBit != 0) revert InvalidNonce();
      return (invalidatorSlot, invalidator, invalidatorBit);
  }
  ```
- `USDtbMinting.sol:509-516` (USDtb, `nonce` is `uint128`): same `uint64(nonce) >> 8` / `1 << uint8(nonce)`.

**Exploitability analysis:**
- The bitmap stores *used* nonces. After nonce A (low-64 = X) is consumed, its bit is SET. Any later nonce B with the same low-64 bits X sees `invalidator & bit != 0` and **reverts** (`InvalidNonce`).
- Therefore two colliding nonces cause a **false rejection** of the second order — *not* a replay/acceptance of a duplicate. There is **no path** by which an attacker gets a nonce accepted twice (double-spend) or gets someone else's order replayed.
- An attacker can only grief **their own** orders (the benefactor signs their own orders; the nonce belongs to the benefactor). They cannot affect other users' orders.
- The off-chain component simply needs to keep nonces below `2^64` to avoid collisions entirely — Ethena's acknowledged stance.

**Verdict:** Low-severity availability/self-griefing only, explicitly Acknowledged, present in both contracts but **not exploitable for fund theft, freezing, or unauthorized minting**. **Not Immunefi-bounty-eligible** (no qualifying impact under Primacy of Impact). Documented here for completeness; **no separate vuln PoC file written** because it does not meet the "appears exploitable" bar.

---

## 7. Unaudited Post-Audit Attack Surface (highest research value)

The Oct-2023 Code4rena scope (`ee67d9b`) covered only `mint`, `redeem`, `_transferCollateral`, `_transferToBeneficiary`. The following were added afterwards and have **no public audit coverage** (the Spearbit/Cantina reports are not publicly downloadable, and the USDtb audits only cover USDtb, not these USDe additions):

1. **`EthenaMinting.mintWETH`** (`EthenaMinting.sol:211-236`) + **`_transferEthCollateral`** (lines 502-530).
   - Pulls WETH from benefactor, `WETH.withdraw(amount)` → sends native ETH to custodian addresses via low-level `.call{value:}`.
   - **Reentrancy:** `mintWETH` is `nonReentrant`; state changes (`mintedPerBlock`, nonce dedup) occur before external calls; `usde.mint` happens after. Reentry into `mint`/`redeem`/`transferToCustody` is blocked by the shared `ReentrancyGuard`. The only reentrancy-reachable non-guarded functions are `setDelegatedSigner`/`confirmDelegatedSigner`/`removeDelegatedSigner` (delegation state only, no funds). No exploit found.
   - **Custodian trust:** `.call{value:}` to a custodian that reverts/consumes-gas bricks the mint, but custodians are admin-whitelisted. Dust from integer division goes to the last custodian (consistent with `_transferCollateral`). No issue found.
   - **Open question worth probing:** `mintWETH` does **not** verify `order.collateral_asset == address(WETH)` in `verifyOrder`; the check is deferred to `_transferEthCollateral` (line 509). A mismatched asset reverts, so no fund loss — but worth confirming there is no signature-reuse path where a WETH-collateralized order signature could be replayed on `mint` (it cannot: `mint`→`_transferCollateral` rejects `NATIVE_TOKEN`/non-supported assets, and the order_type is `MINT` in both). No exploit found.
2. **`EthenaMinting.transferToCustody` native-ETH path** (lines 305-318): `wallet.call{value: amount}` for `asset == NATIVE_TOKEN`. Gated by `COLLATERAL_MANAGER_ROLE` and `_custodianAddresses` membership. Trusted-role only; no third-party exploit.
3. **Delegated-signer flow** (`setDelegatedSigner`/`confirmDelegatedSigner`/`removeDelegatedSigner`, lines 284-302) + `verifyOrder` acceptance (line 411). Two-step accept (`PENDING`→`ACCEPTED`) prevents unilateral delegation spoofing. Status `REJECTED` is terminal. No issue found, but the delegation map is keyed `[delegatee][delegator]` — verify there is no cross-chain-replay or stale-delegation edge case (domain separator caches chainId, so cross-chain replay is handled).
4. **`EthenaLPStaking.sol`, `StakingRewardsDistributor.sol`, `EthenaBalancerRateProvider.sol`** — these were **outside** the Code4rena scope entirely. `EthenaLPStaking` is an `Ownable2Step`+`ReentrancyGuard` LP-token staking contract with a configurable cooldown; rewards are computed off-chain (the contract only holds staked LP tokens). Worth a dedicated pass (especially `stake`/`unstake` accounting and the cooldown-reset logic), but it holds LP tokens, not USDe principal, so blast radius is smaller than the minting/staking core.

---

## 8. Immunefi Bug-Bounty Program Snapshot

- **URL:** https://immunefi.com/bounty/ethena/
- **Live since:** 04 April 2024. **Max bounty:** $3,000,000 (Smart Contract Critical, 10% of funds at risk). Primacy of Impact applies.
- **Tiered rewards:** Critical $100k–$3M; High (PoI) $10k–$75k; Medium (PoI) flat $10k; Low (PoI) flat $2.5k. Web/App Critical $20k–$50k.
- **Operational flags:** Triaged by Immunefi, PoC required, KYC required, Arbitration enabled, Vault program (≈12.5k USDT).
- **Repeatable-attack clause:** since Ethena's contracts are upgradeable/pausable, only the *initial* attack counts for repeatable critical bugs (relevant when sizing any future finding).
- **Publicly disclosed/paid reports:** **None found** via web search for Ethena smart contracts. KYC + NDA terms mean payouts are not publicly itemised; absence of disclosure ≠ absence of payouts. Bug classes the program is most likely to have already received (based on audit coverage): EIP-712 signature/order handling, blacklist/whitelist bypass, ERC4626 donation/first-depositor, cooldown accounting — all of which are now closed (see §3–§6).

---

## 9. Findings Status Summary Table

| Finding | Auditor | Severity | Status in current contracts | Bounty-eligible now? |
|---------|---------|----------|------------------------------|----------------------|
| M-01 restricted-staker approval bypass | Code4rena | Medium | **Fixed** (`_owner` check added) | No — fixed |
| M-02 soft-restricted can withdraw | Code4rena | Medium | Disputed/by-design | No — disputed, compliance impact only |
| M-03 cooldown not bypassed when disabled | Code4rena | Medium | **Fixed** (`\|\| cooldownDuration==0`) | No — fixed |
| M-04 MinShares donation DoS | Code4rena | Medium | Acknowledged; deployment-time only | No — not live (huge TVL) |
| Pashov L-02..L-07 (USDtb blacklist/whitelist) | Pashov | Low | **Fixed** (rewrite of `_beforeTokenTransfer`) | No — fixed |
| Cyfrin L-2, L-3 (USDtb burn/whitelist intermediary) | Cyfrin | Low | **Fixed** (PR#10) | No — fixed |
| Pashov L-03 (ORDER_TYPE typestring) | Pashov | Low | **Fixed** | No — fixed |
| USTB-5 / `verifyNonce` `uint64` truncation (USDe **and** USDtb) | Quantstamp | Undetermined/Low | **Unfixed** (Acknowledged, won't-fix) | **No** — false-rejection only, self-griefing, no fund impact |
| USTB-1 input validations | Quantstamp | Low | Acknowledged | No — admin/off-chain trust |
| USTB-2 storage gap | Quantstash/Cyfrin | Info | Acknowledged | No — cosmetic |
| USTB-3 non-standard ERC20 | Quantstamp | Info | Acknowledged | No — token-list trust |
| USTB-4 missing events | Quantstamp | Info | Acknowledged | No — observability |

---

## 10. Next Actions / Recommendations

1. **Primary focus — unaudited surface (§7).** Spend research hours on `EthenaMinting.mintWETH`/`_transferEthCollateral` (native-ETH handling, custodian `.call` semantics, WETH unwrap ordering) and the delegated-signer lifecycle. These are the only meaningful gaps between audit coverage and live code. No exploit was found in this pass, but the surface is real.
2. **Secondary — `EthenaLPStaking` / `StakingRewardsDistributor`.** Never audited publicly. Lower blast radius (LP tokens / rewards) but worth a dedicated review of stake accounting and cooldown math.
3. **Do NOT pursue:** M-01/M-03 (fixed), M-02 (disputed + compliance-only), M-04 (deployment-only), USTB-5 (acknowledged false-rejection). Re-reporting these would be duplicates and would be rejected.
4. **Obtain the Spearbit/Cantina PDFs** if access becomes available (docs.ethena.fi links are JS-rendered; try Ethena team / Cantina business page) — they may cover the mintWETH-era code and would close the §7 coverage gap.
5. **Scope check before any submission:** confirm the exact asset list on the Immunefi page (it renders client-side; the `EthenaLPStaking`/`StakingRewardsDistributor`/`EthenaBalancerRateProvider` contracts' inclusion in scope should be verified against the live Immunefi scope table, not assumed).

---

## 11. Files / Artifacts

- This report: `/home/z/fkr-step1/defi-bounty/protocol-research/ethena-audit-history.md`
- No vuln PoC file written (no exploitable unfixed finding identified).
- Source artifacts retained: `/home/z/ethena-c4-findings/` (Code4rena), `/home/z/ethena-usde/` (USDe contracts), `/home/z/ethena-usdtb/` (USDtb contracts + 3 audit PDFs), `/tmp/{quantstamp,october,ustb}.txt` (extracted PDF text).
