# StackingDAO — Secondary / Informational Findings

These are lower-severity issues identified during the deep audit. They are documented for
completeness; none individually rises to CRITICAL/HIGH with an attacker-controlled trigger on
the current deployment, but several warrant hardening. The primary HIGH finding is in
`/home/z/stackingdao-vuln-rewards-freeze.md`.

Scope/network: Stacks mainnet, deployer `SP4SZE494VC2YC5JYG7AYFQ44F5Q4PYV7DVMDPBG`.
Language: **Clarity** (Stacks Bitcoin L2). Note: the original task brief assumed Solidity/EVM
("ERC4626", "upgradeable proxy", "Foundry PoC", "Ethereum"); the actual StackingDAO contracts
are Clarity on Stacks and there is **no Ethereum/Solidity component** in the Immunefi scope.
Audit was adapted accordingly.

---

## S1. `save-pending-rewards` is public with no access control (enabler of the primary bug)

- **Contract / line:** `ststxbtc-tracking-v2.clar` lines 82–97.
- **Issue:** Unlike its sibling `refresh-wallet` (line 44, which calls
  `(contract-call? .dao check-is-protocol contract-caller)`), `save-pending-rewards` has **no
  guard**. Any principal can call `save-pending-rewards(holder, position)` for arbitrary
  `holder`/`position`. The function snapshots the holder's pending rewards into `saved-rewards`
  and advances their `cumm-reward` checkpoint to the live global value.
- **Standalone impact:** Calling it on someone else is *non-destructive* in isolation — it only
  banks pending rewards and advances the checkpoint, so the holder loses nothing (they can still
  claim). **However**, combined with the deactivated-position `cumm-reward` mismatch (primary
  bug), it lets an attacker brick a victim's entry *before* the victim claims, locking the
  banked `saved-rewards` permanently. It is the amplifier that turns a self-inflicted UX bug
  into an attacker-driven fund freeze.
- **Severity:** LOW on its own; HIGH as the enabler of the primary finding.
- **Fix:** Make `save-pending-rewards` `define-private`, or gate it with
  `check-is-protocol contract-caller` (consistent with `refresh-wallet`). It is only meant to
  be invoked internally by `refresh-wallet` / `refresh-position`.

---

## S2. `claim-pending-rewards` has no caller==holder check (anyone-can-claim-on-behalf)

- **Contract / line:** `ststxbtc-tracking-v2.clar` lines 155–172; also `claim-pending-rewards-many` (147) / `-iter` (151).
- **Issue:** `claim-pending-rewards(holder, position)` pays the sBTC to `holder` regardless of
  `tx-sender`. Anyone can trigger a claim on behalf of any holder.
- **Standalone impact:** The funds go to the `holder`, not the caller, so there is **no theft**.
  It is a front-run/gas-grief and removes the holder's ability to *defer* claiming. This pattern
  is common and usually considered acceptable. It does, however, interact with the primary bug:
  it guarantees a holder can always be made to "claim" (and thus self-brick) even if they never
  intended to.
- **Severity:** INFORMATIONAL (no fund loss by itself).
- **Note:** This is likely intentional (permissionless claim keeps rewards from being stuck if a
  holder is inactive). No change required beyond awareness.

---

## S3. `commission-sbtc-v1.add-commission` ignores its `staking-end-block` parameter

- **Contract / line:** `commission-sbtc-v1.clar` lines 16–41.
- **Issue:** `add-commission (sbtc-amount uint) (staking-end-block uint)` accepts
  `staking-end-block` but never reads, stores, or uses it. The caller `rewards-pox5-v1` (line
  148) computes `staking-end = (+ burn-block-height RELEASE_WINDOW_BLOCKS)` and passes it, but
  the value is silently discarded. The contract simply splits `sbtc-amount` into a signer share
  (forwarded to `signer-payout-v1`) and a treasury share (retained), with no time lock.
- **Impact:** No direct fund impact. It indicates an intended-but-unimplemented behavior (e.g.
  commission was meant to be locked/unvested until `staking-end`). If a downstream consumer or
  future governance assumes time-locking exists, that assumption is wrong.
- **Severity:** INFORMATIONAL / LOW.
- **Fix:** Either remove the parameter or implement the intended time-bound behavior; update the
  caller accordingly.

---

## S4. Cross-contract reward-distribution DoS dependency on out-of-scope `ststxbtc-tracking` (v1)

- **Contract / line:** `rewards-pox5-v1.clar` lines 93–115 (release phase), specifically the
  v1 branch at lines 100–109: `(try! … (contract-call? .ststxbtc-tracking add-rewards rewards-v1))`.
- **Issue:** `rewards-pox5-v1.process-rewards` splits the ststxbtc reward share between v1 and
  v2 tracking pro-rata by token supply (`ststxbtc-supply-v1` / `total-supply`). If
  `ststxbtc-supply-v1 > 0` and the **out-of-scope** `ststxbtc-tracking` (v1) contract's
  `add-rewards` reverts (e.g. its internal `total-supply` data var is 0 causing a divide-by-zero,
  or it is paused/broken), the `try!` propagates and `process-rewards` aborts.
- **Blast radius:** `rewards-pox5-v1.process-rewards` is invoked by `stacking-dao-core-stbtc-v1`
  (deposit/init-withdraw/withdraw-idle), `stacking-dao-core-ststxbtc-v2`
  (deposit/init-withdraw/withdraw-idle), and `swap-ststx-ststxbtc-v4`. A revert would therefore
  **block all stBTC and stSTXbtc deposits, withdrawal initiations, idle withdrawals, and swaps**
  until v1 tracking is fixed — i.e. a protocol-wide temporary freeze of the BTC-side products
  (final NFT withdrawals are unaffected, as `withdraw` does not call `process-rewards`).
- **Attacker control:** Not directly attacker-triggered (depends on v1 contract state, which is
  out of scope and DAO-managed). It is a robustness/fragility issue arising from a hard
  dependency on an out-of-scope legacy contract.
- **Severity:** LOW (state-dependent, not attacker-triggered); would be HIGH if v1 supply is
  non-zero and v1 tracking becomes dysfunctional.
- **Fix:** Wrap the v1 `add-rewards` call in a `match`/best-effort pattern (skip on error) or
  gate the v1 branch on `ststxbtc-supply-v1 == 0` being false AND a health check; or migrate
  remaining v1 supply to v2 and remove the v1 branch entirely.

---

## S5. Theoretical ERC4626-style donation/inflation on first deposit (not exploitable live)

- **Contracts / lines:** `stacking-dao-core-stx-v2.clar` deposit (39–67),
  `stacking-dao-core-stbtc-v1.clar` deposit (35–63); exchange rates in `data-stx-v2.clar`
  `compute-ratio` (31–50) and `data-stbtc-v1.clar` `compute-ratio` (26–46).
- **Issue:** The exchange rate is `active-backing / active-supply`, where `active-backing`
  includes the reserve's raw token balance. A direct token donation to the reserve (plain
  `stx-transfer?` to `.stx-reserve-v2`, or sBTC transfer to `.stbtc-reserve`) increases
  `active-backing` without increasing `active-supply`, inflating the ratio. With a tiny
  `active-supply` (first-deposit regime), a donor could inflate the ratio so the next depositor
  receives 0 shares for a large deposit.
  - stSTX has a `DEAD_SHARES` (1000) first-deposit guard and the receiving core is in
    `data-stx-v2`'s `escrow-cores`, so the dead shares are subtracted from `active-supply` — a
    reasonable mitigation, though a large direct donation can still dwarf it.
  - stBTC has `DEAD_SHARES` but its `data-stbtc-v1` uses `pending-shares` (not an escrow list),
    so the dead shares remain in `active-supply` and provide no donation protection.
- **Live exploitability:** **None.** Both vaults already have substantial mainnet TVL, so
  `active-supply` is large; an attacker would need to donate more than total TVL to move the
  ratio meaningfully. Additionally, stSTX and stSTXbtc deposits are currently shut down
  (`shutdown-deposits = true` in both cores); only stBTC deposits are open, and stBTC is already
  initialized with TVL.
- **Severity:** INFORMATIONAL (theoretical, not exploitable on the current deployment).
- **Fix (defense-in-depth):** add a virtual-price / dead-shares bootstrap that is subtracted
  from `active-supply` for stBTC (mirror the stSTX escrow approach), and/or enforce a non-zero
  `min-shares-out` on first deposit.

---

## S6. Inconsistent withdraw-fee routing across products

- **Contracts / lines:**
  - `stacking-dao-core-stx-v2.clar` `withdraw` (96–121): the `stx-fee` is **not** sent anywhere;
    only `stx-user` is paid out, so the fee stays as idle in the reserve (accruing to stSTX
    holders via the exchange rate).
  - `stacking-dao-core-stbtc-v1.clar` `withdraw` (93–119): same — fee stays in reserve.
  - `stacking-dao-core-ststxbtc-v2.clar` `withdraw` (76–105): the `stx-fee` **is** sent to
    `(var-get treasury)` via `request-stx-for-withdrawal-ststxbtc`.
- **Issue:** The three products handle the same `withdraw-fee` concept differently. For stSTX
  and stBTC the fee is effectively never collected (it just remains idle and benefits holders);
  for stSTXbtc it is routed to a treasury. All `withdraw-fee` values are currently `u0`, so there
  is no live fund impact, but enabling fees on stSTX/stBTC would not behave as a "treasury fee".
- **Severity:** INFORMATIONAL (design inconsistency; no fund loss).
- **Fix:** Unify the fee-routing semantics or document the per-product intent.

---

## S7. `reward-split-calculator-v1.compute-and-apply` trusts caller-supplied inputs

- **Contract / line:** `reward-split-calculator-v1.clar` `compute-and-apply` (106–131); it takes
  `r-current`, `t-stbtc`, `t-ststxbtc`, `t-ststx` as parameters and applies the resulting split
  to `rewards-pox5-v1.set-split-bps`.
- **Issue:** The TVL/reward inputs are not validated on-chain against actual token supplies; the
  caller (a DAO-gated protocol contract / keeper) supplies them. A misbehaving or compromised
  keeper could feed skewed inputs to bias the reward split between stSTXbtc / stSTX / stBTC.
- **Impact:** Trust assumption on the keeper/DAO role — explicitly **out of scope** per Immunefi
  ("centralization risks", "access to privileged addresses"). Noted only for completeness.
- **Severity:** INFORMATIONAL (centralization/trust).
- **Fix:** Derive `t-stbtc`/`t-ststxbtc`/`t-ststx` on-chain from the token supplies inside
  `compute-and-apply` instead of accepting them as parameters, leaving only `r-current` (an
  off-chain reward-rate observation) as a trusted input.

---

## What was checked and found sound

For transparency, the following focus areas from the brief were examined and **no exploitable
critical/high issue was found**:

- **Vault (deposit/withdraw queue):** `stacking-dao-core-{stx-v2,stbtc-v1,ststxbtc-v2}` — access
  control (`dao.check-is-enabled` + per-function shutdowns), reserve earmark math, withdrawal
  NFT ownership and unlock-height checks, and atomic revert-on-insufficient-balance are
  consistent. Withdrawal-idle and NFT-withdraw paths are sound.
- **Token mint/burn:** `ststx-token`, `ststxbtc-token-v2`, `stbtc-token` — all mint/burn are
  `check-is-protocol contract-caller` gated; SIP-010 `transfer` correctly checks
  `tx-sender == sender`. No unauthorized-mint path.
- **Bridge / Bitcoin L2 ↔ L1 custody:** sBTC bridge is the external `SM3VDXK3…sbtc-token` (out
  of scope, per SECURITY.md). The in-scope custody path (`stbtc-reserve`, `stx-reserve-v2`,
  stakers, `strategy-v6`, signer-managers) is consistently `check-is-protocol`-gated and uses
  `with-ft`/`with-stx`/`with-staking`/`with-pox` asset scopes correctly.
- **Signature verification:** PoX-5 `grant-signer-key`/`register-signer` are invoked only via
  `signer-manager-*.register-self`, which is admin-gated (`authorize-admin`: `contract-caller ==
  tx-sender` and `is-admin`). No unauthenticated signer-key grant path.
- **Reentrancy:** Clarity is non-reentrant through unknown code (per SECURITY.md); the only
  trait callbacks (`pox-5` `validate-stake!`, position `get-holder-balance`) are bracketed by
  the `is-delegating` flag / over-reserve guard. No reentrancy found.
- **Upgradeable proxy:** `dao-executor` (impl pointer) + `dao-multisig` (7-day
  `IMPL-UPDATE-TIMELOCK`, two-proposal schedule→execute). `execute-proposal` runs as-contract
  with `with-all-assets-unsafe` but is gated by `check-impl-auth` (caller == impl == multisig)
  and proposal matching. Sound. (Note: `urgent` proposals bypass the 1-day `TIMELOCK` — a
  governance/trust choice, out of scope as centralization.)
- **Reward distribution precision:** `rewards-pox5-v1` split (stbtc absorbs rounding dust),
  `rewards-stx-v2` / `rewards-pox5-v1` streaming fold math, and the `ststxbtc-tracking-v2`
  cumulative-per-token math are all rounded in the protocol's favor; no rounding arbitrage in
  the fee-free `swap-ststx-ststxbtc-v4` (round-trip is provably ≤ input because
  `get-stx-per-ststx-up` ≥ `get-stx-per-ststx`).
- **Blacklist/kill-switch:** `dao.set-contracts-enabled(false)` (global) and
  `dao.set-contract-active(addr, false)` (per-contract) plus per-function `shutdown-*` flags
  provide layered, admin-gated disable. No bypass found.
- **Flash-loan attack:** stSTXbtc reward tracking checkpoints holders on every
  mint/burn/transfer (`refresh-wallet`), so acquiring tokens just before `add-rewards` yields no
  retroactive reward; the swap is value-preserving (1:1 with exchange-rate rounding favoring the
  protocol). No flash-loan profit path found.
- **Cross-contract interaction:** reserve earmark invariants (`stx-for-withdrawals-ststxbtc ≤
  stx-for-ststxbtc-idle`, `stx-for-withdrawals-ststx` backed by balance) hold across all
  deposit/withdraw/swap/staking paths examined.

The single HIGH finding (`rewards-freeze`) and the informational items above constitute the
complete result of this deep audit.
