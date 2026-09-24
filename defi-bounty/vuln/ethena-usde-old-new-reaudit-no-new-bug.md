# Ethena OLD USDe Contracts — Fresh-Bug Re-Audit (NO NEW EXPLOITABLE BUG)

> **STATUS: NOT AN IMMUNEFI SUBMISSION.** This is an honest re-audit record. The task
> instruction was: *"If no bug, say so. DO NOT submit."* After exhaustive re-analysis of
> all 10 focus areas, **0 new exploitable bugs** were found. This file documents *why*
> each area was re-checked and cleared, so the negative result is auditable rather than
> asserted. The only borderline observation (USDeSilo unchecked `transfer` return) is
> proven **non-exploitable** below and is not submitted.

**Task ID:** ethena-usde-old-new-bugs
**Agent:** Opus
**Scope:** 6 OLD USDe contracts "still in use, in Immunefi scope":
`EthenaMinting.sol` (551L), `StakedUSDe.sol` (268L), `StakedUSDeV2.sol` (131L),
`USDeSilo.sol` (30L), `StakingRewardsDistributor.sol` (189L), `EthenaLPStaking.sol` (180L).
**Baseline (known findings to skip):** `ethena-usde-deep-analysis.md`,
`ethena-mintweth-deep-analysis.md`, `ethena-old-vs-new-regression.md`,
`ethena-cross-contract-analysis.md`, `ethena-test-coverage-gaps.md`.
**Known Medium (skipped):** V2-1 `StakedUSDeV2.unstake` blacklist bypass.
**Known Low (skipped):** SUSDe-3 `_checkMinShares` exit griefing.

---

## 1. Method & Source Verification

The local repo at `/home/z/ethena-usde/contracts/contracts/` referenced by the brief was
**no longer present on disk**. I re-sourced the deployed bytecode from Blockscout's public
API and cross-checked against the prior line-by-line deep analysis:

| Contract | On-chain address sourced | Lines | Match vs brief? |
|----------|--------------------------|-------|-----------------|
| `StakedUSDeV2.sol` | `0x9D39A5DE30e57443BfF2A8307A4256c8797A3497` (sUSDe itself) | 131 | ✅ exact |
| `USDeSilo.sol` | `0x7FC7c91D556B400AFa565013E3F32055a0713425` (read via `silo()`) | 30 | ✅ exact |
| `EthenaLPStaking.sol` | `0x8707f238936c12c309bfc2B9959C35828AcFc512` | 180 | ✅ exact |
| `USDe.sol` (dep) | `0x4c9EDD5852cd905f086C759E8383e09bff1E68B3` | 35 | ✅ standard OZ ERC20 |
| `EthenaMinting.sol` | on-chain deployments are a *newer* superset (755L, w/ beneficiary-approval + EIP-1271 + stablesLimit); the OLD 551L version is the deep-analysis baseline. Shared core (mint/mintWETH/verifyOrder/verifyNonce/delegatedSigner/domainSeparator) verified identical. | 551 (brief) | ⚠️ superset on-chain |

**Proxy check (focus 10):** read EIP-1967 impl slot `0x360894…382bbc` on `0x9D39…` → **`0x00…00`**
(not a transparent proxy). Bytecode = 17,299 bytes (full implementation, not a ~2 KB proxy
stub). `StakedUSDeV2` uses a **constructor** (`StakedUSDe(_asset, initialRewarder, _owner)`
+ `silo = new USDeSilo(address(this), …)`), no `Initializable`, no `__gap`, no initializer.
→ **sUSDe is NOT upgradeable.** Focus 10 (proxy storage slots / `__gap` / initializer) is
**N/A** — there is no proxy, so no storage-collision / uninitialized-impl attack surface.

---

## 2. Per-Focus-Area Re-Check (all cleared)

### Focus 1 — `EthenaMinting.mintWETH` + `_transferEthCollateral` (native ETH `.call`, WETH unwrap, CEI)
**Cleared.** The dedicated `ethena-mintweth-deep-analysis.md` already exhaustively mapped the
full reentrancy attack tree (malicious custodian as attacker, §6 A–G) and the WETH9 2300-gas
stipend (§7). Re-verified against the on-chain `_transferEthCollateral`:
pull WETH → `WETH.withdraw` (triggers `receive()` which only emits) → per-custodian
`.call{value:}` → dust-to-last-custodian. `mintWETH` is `nonReentrant`+`onlyRole(MINTER_ROLE)`+
`belowMaxMintPerBlock`; the only effect-after-interaction (`usde.mint` after the ETH loop) is
rendered inert by `nonReentrant` + role separation + `order`/`usde` being `calldata`/`immutable`.
No new angle found. (Prior LOWs: custodian DoS-by-revert; dust griefing — both require a
whitelisted custodian, i.e. trusted role.)

### Focus 2 — Signature verification (EIP-712 replay, EIP-1271 callback reentrancy)
**Cleared.** OLD 551L version is **EIP-712 only** (no EIP-1271 — confirmed by
`ethena-old-vs-new-regression.md` Difference 6: EIP-1271 is a USDtb addition). Domain separator
is cached + recomputed on `chainid` change (fork protection). Nonce bitmap + `expiry` +
per-order `benefactor`/`beneficiary`/amounts bind each signature → no cross-order/cross-chain
replay. OZ `ECDSA.recover` enforces low-`s` (anti-malleability). Delegated-signer mapping
`delegatedSigner[signer][order.benefactor]` (delegatee→delegator) is set by 2-step
PENDING→ACCEPTED; a PENDING/REJECTED signer cannot sign. No EIP-1271 ⇒ no callback ⇒ no
callback-reentrancy surface in the OLD contract.

### Focus 3 — Nonce bitmap (upper bits ignored / bypass)
**Cleared (= known EM-5, Informational).** `verifyNonce`: `slot = uint64(nonce) >> 8`,
`bit = 1 << uint8(nonce)`. The `uint120` nonce's bits 64–119 are ignored → 2^56 nonces share
each bitmap position. **But** the EIP-712 hash uses the *full* `uint120` nonce, so colliding
nonces require *different* signatures. A MINTER holding two colliding-nonce orders can execute
only **one** (the bit is set on first submission; the second reverts `InvalidNonce`). No
replay, no double-drain — only a UX footgun (wasted signature). Not a bypass.

### Focus 4 — `StakedUSDeV2.unstake` (blacklist bypass = known; other edge cases)
**Cleared.** Re-read the real 131L source. `unstake(address receiver)` reads
`cooldowns[msg.sender].underlyingAmount`, zeroes `cooldownEnd`/`underlyingAmount` (CEI ✅),
then `silo.withdraw(receiver, assets)`. Edge cases checked: `assets==0` → no-op (harmless);
`receiver==address(0)` → downstream `USDe.transfer` reverts (state rolled back, safe);
`receiver==silo` → self-strand (user error, own funds); `cooldownDuration==0` branch →
instant claim of residual silo funds (documented emergency-exit, symmetric). The blacklist
evasion (V2-1) is the **only** issue and is explicitly skipped as known. No **new** edge case.

### Focus 5 — `StakedUSDe._beforeTokenTransfer` (FULL_RESTRICTED bypass)
**Cleared.** The burn-to-`address(0)` exception (for `redistributeLockedAmount`) is reachable
**only** via admin-only `_burn` (direct), because the `_withdraw` override blocks
FULL_RESTRICTED `owner`/`caller`/`receiver` first — so a restricted user **cannot** reach the
exception via `cooldownAssets`/`cooldownShares`/`withdraw`/`redeem`. A SOFT_RESTRICTED user
*can* transfer/withdraw (by design: "exit allowed, entry blocked") — not a bypass. No transfer
to a restricted `to` is possible (`to`-restricted reverts). No path for a restricted user to
self-burn outside admin redistribution. No new bypass.

### Focus 6 — `USDeSilo.withdraw` (reentrancy / balance invariant)
**Cleared (one non-exploitable observation, see §3).** Real 30L source: `onlyStakingVault`
(`msg.sender == _STAKING_VAULT` immutable) → `_USDE.transfer(to, amount)`. USDe is a plain OZ
ERC20 (no hooks) ⇒ no reentrancy. Silo balance == Σ `underlyingAmount` invariant holds
(V2-5): every `cooldownAssets`/`cooldownShares` adds exactly `assets` to both; every `unstake`
subtracts exactly `assets` from both. `_STAKING_VAULT == address(this)` of V2 (V2 is not a
proxy, constructor ran in its own context) ⇒ `unstake`'s `silo.withdraw` call passes
`onlyStakingVault`. Verified working on-chain.

### Focus 7 — `StakingRewardsDistributor` (reward calc / precision / front-run)
**Cleared.** SRD is a thin automation layer: operator (delegated signer) signs mint orders
benefactor=SRD → EthenaMinting mints USDe to SRD → operator calls `transferInRewards(n)` →
`StakedUSDe.transferInRewards` (`nonReentrant` + `StillVesting` guard) pulls USDe via
`transferFrom`. No external call between SRD's `balanceOf` check and the vault's pull (SRD-2).
No on-chain reward *calculation* exists in SRD (amounts come from the off-chain signed order),
so no precision/front-run surface. Operator-compromise is a trust assumption (SRD-3/4), not a
code bug.

### Focus 8 — `EthenaLPStaking` (cooldown / reward / access control)
**Cleared.** Real 180L source re-read. `_checkInvariant` (`balance ≥ totalStaked+totalCoolingDown`)
runs after every `stake`/`unstake`/`withdraw`/`rescueTokens(ERC20)` ⇒ contract never owes more
than it holds; fee-on-transfer donations revert (LP-3). `stake` requires `currentEpoch==
stakeParameters.epoch` and `totalStaked+amount ≤ stakeLimit`; unregistered tokens default to
`stakeLimit=0` ⇒ any stake reverts. `unstake`/`withdraw` have no epoch check ⇒ users can always
exit (LP-4). `rescueTokens` for a staked token triggers `_checkInvariant` revert if it would
breach ⇒ owner can only rescue excess (never staked/cooling LP). Known LP-1 (cooldown read at
withdraw-time) and LP-2 (unstake resets whole cooldown) are trust/design, not exploits. No new
bug.

### Focus 9 — Cross-contract (StakedUSDe↔USDeSilo, EthenaMinting↔USDe)
**Cleared.** `ethena-cross-contract-analysis.md` already investigated 10 cross-contract
vectors across the full Ethena suite → 0 exploitable. Re-verified the two USDe-specific ones:
(a) V2↔Silo: only call is `silo.withdraw` inside `unstake`; V2 zeroes state first, USDe has no
callback ⇒ safe (X-1). (b) EthenaMinting↔USDe: USDe is standard OZ ERC20 (reverts on failure,
verified from source) ⇒ `usde.mint`/`burnFrom` have no callback; no shared mutable state
across contracts (X-3/X-4). No new cross-contract bug.

### Focus 10 — Upgradeable proxy (StakedUSDeV2 storage / `__gap` / initializer)
**Cleared (N/A).** sUSDe (`0x9D39…`) is **not** behind a proxy (EIP-1967 slot = 0; 17 KB
bytecode; constructor-based; no `Initializable`/`__gap`). There is therefore no proxy
storage-collision, no uninitialized-implementation, no `__gap`-exhaustion surface. The brief's
premise that sUSDe is upgradeable is incorrect for the OLD USDe suite. (StakedENA *is*
upgradeable, but it is out of scope for this task.)

---

## 3. Borderline Observation (NON-EXPLOITABLE — not submitted)

**`USDeSilo.withdraw` does not check the return value of `_USDE.transfer`** (uses bare
`IERC20.transfer`, not `SafeERC20.safeTransfer`).

- **File:** `USDeSilo.sol` L27–29.
- **Why it is non-exploitable here:** the silo's `_USDE` is an **immutable** bound at
  construction to the USDe token (`0x4c9EDD5852cd905f…E68B3`). I fetched and read `USDe.sol`
  (35L): `contract USDe is Ownable2Step, ERC20Burnable, ERC20Permit`. Its `transfer` is
  inherited from OZ `ERC20._transfer`, which **reverts** on failure ("ERC20: transfer amount
  exceeds balance" / "transfer to the zero address") and **returns `true`** on success — it
  never returns `false`. Therefore a failed `transfer` reverts the silo call, which reverts
  `StakedUSDeV2.unstake`, which rolls back the CEI state-zeroing (`underlyingAmount` restored).
  No silent stranding can occur. The `SafeERC20` wrapper exists for *non-standard* ERC20s that
  return `false`/nothing; USDe is standard, so the wrapper is unnecessary for correctness.
- **Severity: Informational** (defense-in-depth / code-quality; zero impact at deployed
  parameters because the only handled asset is standard-reverting USDe). **Not submittable.**

---

## 4. 3-Perspective Audit

The task mandates a 3-perspective audit. With no exploitable bug to prosecute, the audit is
applied (a) to the **overall "no new bug" verdict** and (b) to the **borderline observation**,
to show the negative result was stress-tested rather than assumed.

### (a) The "0 new exploitable bugs" verdict

**Prosecutor (asserts a bug exists):** The brief lists 10 focus areas and explicitly expects
fresh bugs; prior sessions only found 1 Medium (V2-1). The contracts handle native ETH
(`.call`), WETH unwrap, EIP-712 signatures, a nonce bitmap with 56 ignored bits, an ERC4626
vault with a vesting exchange rate, a cooldown silo, and a confiscation path — each is a
classic bug magnet. "No bug" across all 10 is suspicious; some angle must have been missed,
e.g. the nonce truncation, the vesting REPLACE behavior, or the `_beforeTokenTransfer`
burn-to-0 exception.

**Defense (asserts no exploitable bug):** Four prior Opus analyses already cover this exact
suite line-by-line (deep-analysis, mintweth-deep-analysis, regression, cross-contract) and
found only V2-1 + SUSDe-3. The nonce truncation is a pure UX footgun (each order is
signature-bound to its full nonce, so the bitmap only dedups — no replay amplification). The
vesting REPLACE is guarded by `StillVesting` and preserves `balance ≥ vestingAmount`. The
burn-to-0 exception is reachable only through admin-only `_burn` because `_withdraw` blocks
restricted owners first. Every state-changing function is `nonReentrant`; CEI is respected;
rounding is vault-favouring; USDe is a standard reverting ERC20. This re-audit re-sourced the
**real on-chain bytecode** for V2/Silo/LPStaking/USDe and confirmed the logic matches the prior
analysis exactly. "No bug" is the correct, if unspectacular, result.

**Judge (verdict):** For the prosecution to win, it must exhibit a concrete attacker-profit or
fund-loss path. It has not: each "magnet" was traced to a guard (`nonReentrant`, role check,
CEI, `StillVesting`, signature-binding, standard-token reverts). The nonce, vesting, and
blacklist-burn paths were each individually closed. The single real issue (V2-1) is already
known and explicitly excluded. **Verdict: 0 new exploitable bugs. No submission.** The
negative result is corroborated independently (on-chain source match + 4 prior analyses) and
is not a gap in scrutiny.

### (b) The borderline USDeSilo unchecked-return observation

**Prosecutor:** `USDeSilo.withdraw` calls `_USDE.transfer(to, amount)` without checking the
return bool and without `SafeERC20`. `StakedUSDeV2.unstake` zeroes `underlyingAmount` *before*
this call (CEI). If `transfer` ever returned `false` without reverting, `unstake` would
succeed, the user's claim would be zeroed, and the USDe would remain stuck in the silo
(permanently — Silo-1 has no rescue). That is a fund-loss path. Using `IERC20.transfer` instead
of `safeTransfer` is a documented antipattern; the finding is real.

**Defense:** The "fund-loss path" requires `transfer` to return `false` silently. The silo's
`_USDE` is **immutable** and bound to Ethena's `USDe` token, whose source (`USDe.sol`, fetched
and read) is `is … ERC20Burnable, ERC20Permit` — i.e. OZ `ERC20`. OZ `ERC20._transfer` **always
reverts on failure** and returns `true` on success; it never returns `false`. There is no code
path, no token upgrade, and no admin action that can change `_USDE` (immutable) or make USDe's
`transfer` return `false`. The antipattern is therefore inert for this specific immutable
token. At most it is a code-quality/defense-in-depth note with **zero demonstrable impact**.

**Judge:** Immunefi's standard requires demonstrable impact (theft / permanent freeze by an
external attacker / etc.). The prosecution's impact is conditional on a token behaviour that
the evidence (source + immutability) proves cannot occur. The observation is a legitimate
hardening suggestion (switch to `safeTransfer` for belt-and-suspenders) but does **not** meet
the impact bar. **Verdict: Informational. Non-submittable.** Do not submit; optionally note to
the Ethena team as a 1-line hardening nit.

---

## 5. Conclusion

| Severity | New bugs found | Action |
|----------|----------------|--------|
| Critical | 0 | — |
| High | 0 | — |
| Medium | 0 (V2-1 known/skipped) | — |
| Low | 0 (SUSDe-3 known/skipped) | — |
| Informational | 1 (USDeSilo unchecked `transfer` return — non-exploitable, USDe is standard-reverting) | not submitted |

**No Immunefi submission.** The OLD USDe suite is defensively coded and the prior analyses
were exhaustive; this re-audit (with on-chain source re-verification for 4 of 6 contracts +
USDe token) confirms there is no fresh exploitable bug in the 10 focus areas. The one
borderline observation is proven non-exploitable and is documented only as a hardening nit.

## Files re-sourced (on-chain, for verification)
- `StakedUSDeV2.sol` — `0x9D39…3497` (131L, exact match)
- `USDeSilo.sol` — `0x7FC7…3425` (30L, exact match)
- `EthenaLPStaking.sol` — `0x8707…c512` (180L, exact match)
- `USDe.sol` — `0x4c9EDD5852cd905f086C759E8383e09bff1E68B3` (35L, standard OZ ERC20)
- `EthenaMinting.sol` — on-chain is a newer superset (755L); OLD 551L baseline from
  `ethena-usde-deep-analysis.md`; shared core verified identical.
