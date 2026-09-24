# Sherlock "More Bounties" Audit — Summary Report

**Task ID**: sherlock-more-audit
**Agent**: Opus (general-purpose sub-agent)
**Date**: 2026-09-24
**Scope**: Audit additional Sherlock bug-bounty protocols not yet covered.

---

## 1. Protocols selected

Fetched `https://audits.sherlock.xyz/bug-bounties` (38 active bounties). Excluded
already-audited protocols (Cap, SYMMIO, Tenor, Aave V4, Babylon, Lombard, ENS,
Hedera, Dexe, Berachain, Cosmos-SDK, SSV, StackingDAO, TheGraph). Filtered for
**bounty > $100k** and **public EVM source code**.

| Protocol | Bounty | Source | Repo | Cloned to |
|----------|--------|--------|------|-----------|
| **Midas** | $500,000 USDC | `github.com/midas-apps/contracts` | Solidity 0.8.9 | `/home/z/usual-midas-contracts` |
| **Moonwell** | $250,000 USD | `github.com/moonwell-fi/moonwell-contracts-v2` | Solidity 0.8.19 | `/home/z/usual-moonwell` |
| **Yearn V3** | $200,000 USDC | `github.com/yearn/yearn-vaults-v3` | Vyper 0.3.10 | `/home/z/usual-yearn-v3` |

**Deprioritised** (no public source or non-EVM):
- Usual Labs ($16M) — no public GitHub repo found; contracts are Etherscan-verified
  only (closed-source audit not feasible in-scope).
- Usual - Fira UZR ($7.5M) — same.
- Flying Tulip ($1M) — only `flyingtulipdotcom/security` (known-issues repo) is
  public; main contracts repo is private.
- Paradex ($500k) — Starknet / Cairo (ZK-rollup L2), non-EVM.
- Fira Protocol ($500k) — no public repo located.

---

## 2. Audit checklist coverage (per protocol)

For each protocol the following eight areas were reviewed end-to-end against the
in-scope contracts:

| Area | Midas | Moonwell | Yearn V3 |
|------|-------|----------|----------|
| Access control | `MidasAccessControl`, `onlyVaultAdmin`, role admin model — see finding #2 | `TemporalGovernor` role/owner model, `Comptroller.admin`, `ReserveAutomation.onlyOwner` — see finding #3 | `Roles.*` enforcement in every external entry; clean |
| Reentrancy | `DepositVault`/`RedemptionVault` transfers-before-mint/burn pattern reviewed; no guard but allowance/limit + access-control on approve path contain reentry. Clean. | `TemporalGovernor._executeProposal` sets `executed=true` before external calls (line 392); `ReserveAutomation.getReserves` CEI; clean. | `@nonreentrant("lock")` on all stateful external fns; clean. |
| Integer precision | `_truncate` + lossless round-trip check in `_tokenTransferFromUser`; fee cap at 100%; see finding #1 (allowance gross/net) | Compound V2 `Exponential` math; `ReserveAutomation` 18-dec normalisation; clean | `_convert_to_shares/assets` rounding directions correct; `unsafe_sub` guarded; see finding #5 (buy_debt proportional) |
| Oracle manipulation | `CompositeDataFeed` ratio + min/max bounds; `stable` flag forces 1e18 (admin config); `CustomAggregatorV3CompatibleFeedGrowth` growth-APR feed — `applyGrowth` overflow bounded by min/maxAnswer | Chainlink `latestRoundData` in `ReserveAutomation` — **see finding #4**; Comptroller `PriceOracle` is admin-set | Strategies' `convertToAssets` is trusted per NatSpec; vault does not source external oracles. Clean. |
| Signature verification | N/A (no EIP-712 in scope) | `TemporalGovernor` Wormhole VAA verification via `parseAndVerifyVM`; **see finding #3** (recipient check gap) | EIP-2612 `_permit` with nonce + deadline + dynamic `domain_separator()` (chain-id aware). Clean. |
| Upgradeable proxy | OZ `Initializable` + `reinitializer(2)` on `DepositVault`; storage gaps sized; `__gap` arrays present. Clean. | `MErc20Delegator` delegate pattern (Compound V2 standard); `MWethOwnerWrapper` `Initializable` + `_disableInitializers` in constructor. Clean. | Vyper non-upgradeable (factory deploys fresh `VaultV3`); `initialize` guarded by `assert self.asset == empty(address)`. Clean. |
| Flash loan attack | mToken rate from `mTokenDataFeed` (admin-submitted growth feed, not spot AMM) — not flash-loan manipulable; `variationTolerance` caps rate drift on approve. Clean. | Comptroller uses Chainlink/PriceOracle (not spot); `ReserveAutomation` caches per-period (see #4). Clean re flash loans. | PPS derived from `total_idle + total_debt` (accounting, not spot price); profit unlock smooths PPS. Clean. |
| Cross-contract interaction | `RedemptionVaultWithSwapper` → `mTbillRedemptionVault.redeemInstant` cross-vault swap — **see finding #1** (double allowance consumption) | `TemporalGovernor` → arbitrary `target.call` (governance); `FeeSplitter` → MetaMorpho redeem; `OEVProtocolFeeRedeemer` → mToken redeem/addReserves. Reviewed, clean except #3. | `VaultV3` → `IStrategy.deposit/redeem/convertToAssets`; `buy_debt` share transfer — **see finding #5**; `_update_debt` balance-diff accounting robust. |

---

## 3. Findings

| # | Protocol | File | Severity | Area |
|---|----------|------|----------|------|
| 1 | Midas | `/home/z/sherlock-more-midas-vuln-allowance-overdecrement.md` | Low–Medium | Integer precision / cross-contract |
| 2 | Midas | `/home/z/sherlock-more-midas-vuln-keeper-access.md` | Low | Access control |
| 3 | Moonwell | `/home/z/sherlock-more-moonwell-vuln-temporal-governor-recipient.md` | Low | Signature verification / cross-contract |
| 4 | Moonwell | `/home/z/sherlock-more-moonwell-vuln-reserve-automation-price-cache.md` | Low–Medium | Oracle manipulation |
| 5 | Yearn V3 | `/home/z/sherlock-more-yearn-vuln-buy-debt-profit-extraction.md` | Low | Access control / integer precision |

**Count**: 5 findings. **Severity breakdown**: 0 Critical, 0 High, 2 Low–Medium, 3 Low.

No Critical/High found. All three codebases exhibit strong engineering hygiene
(formal verification / multi-audit history for Yearn; role separation + rounding
discipline for Midas; Compound V2 lineage + Wormhole best-practices for Moonwell).
The findings are real but bounded — either by trusted roles, admin-configurable
parameters, or conservative (fail-closed) accounting.

---

## 4. Notable clean areas (explicitly verified, no bug)

- **Midas `_tokenTransferFromUser` lossless round-trip check** (`ManageableVault.sol:423-426`):
  reverts if a base-18 amount cannot round-trip through the token's decimals, preventing
  silent dust loss on transfers. Good.
- **Midas `_requireVariationTolerance`** (`ManageableVault.sol:569-583`): bounds mToken-rate
  drift between request and approve, preventing rate-timing abuse on the safe-approve path.
- **Moonwell `TemporalGovernor._executeProposal` reentrancy**: `executed` flag set before
  the external-call loop (line 392), so re-entry with the same VAA reverts.
- **Moonwell `FeeSplitter`**: per-call balance-snapshot accounting means concurrent/MEV
  `split()` calls process disjoint portions; no double-spend.
- **Yearn V3 `_update_debt`**: uses pre/post balance-diff for both deposit and withdraw
  paths, correctly handling fee-on-transfer and partial-loss strategies; approval reset
  to 0 after deposit (line 1130).
- **Yearn V3 `_permit`**: nonce-incremented, deadline-checked, dynamic `domain_separator()`
  (chain-id fork-safe).

---

## 5. Methodology

1. `agent-browser` to fetch `audits.sherlock.xyz/bug-bounties` and each protocol's
   detail page (`/bug-bounties/{56,122,248,272,298,300,30,233,344,350,…}`) to extract
   scope, severity rules, in-scope contracts, and source links.
2. `git clone --depth 1` of each public repo.
3. Read every in-scope contract end-to-end; traced call graphs from external entry
   points down to token transfers and storage mutations.
4. For each of the 8 audit areas, applied the corresponding checks (CEI for
   reentrancy, rounding-direction for precision, role/owner for access control,
   VAA/signature for governance, decimals round-trip for ERC20 interactions, etc.).
5. Cross-referenced each repo's `audits/` folder (Yearn ships 4 audit reports + Certora
  FV; Moonwell ships multiple; Midas references prior Sherlock/Spearbit reports) to
  avoid re-reporting known issues.

## 6. Status

**NOT submitted.** All findings written to `/home/z/sherlock-more-<protocol>-vuln-<area>.md`
for internal review. Worklog appended to `/home/z/my-project/worklog.md`.
