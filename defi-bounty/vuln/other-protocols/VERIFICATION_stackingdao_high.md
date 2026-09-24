# VERIFICATION — StackingDAO HIGH: Permanent freeze of accrued sBTC rewards for holders of a deactivated position

**Status:** CONFIRMED
**Original severity claim:** HIGH (arguably CRITICAL)
**Final severity (per Immunefi VSS 2.3):** **HIGH** — "Permanent freezing of unclaimed yield"
**Language:** Clarity (Stacks L2) — *not* Solidity/EVM; verified with `clarinet` + `vitest` simnet (the project's own harness).
**Verifier:** Opus (sub-agent)
**Date:** 2026-09-24

---

## 0. TL;DR

The bug is **real, deterministic, and reproducible end-to-end** against the
project's own simnet harness (the same one their CI uses). Three vitest cases
were added; all pass against the deployed `mainnet/contracts/rewards/ststxbtc-tracking-v2.clar`
source. The underflow is a concrete Clarity `Runtime(ArithmeticUnderflow, ...)`
abort, not speculation. The unguarded `save-pending-rewards` is confirmed
public and callable by anyone.

The "arguably CRITICAL" framing in the original write-up is **overstated**:
the frozen asset is sBTC yield, not principal, and a DAO-gated recovery path
exists. Per Immunefi VSS 2.3 this falls cleanly under
"Permanent freezing of unclaimed yield" → **HIGH** ($1k–$20k reward range).

---

## 1. Bug claim restated

In `ststxbtc-tracking-v2.clar`, the sBTC reward distributor uses a standard
cumulative-reward-per-token (`cumm-reward`) pattern. Supported positions
(external protocols holding stSTXbtc on behalf of users) can be deactivated by
the DAO via `set-supported-positions(active=false)`, which freezes the
position's `deactivated-cumm-reward` at the global `cumm-reward` at
deactivation time.

The defect is a **read/write `cumm-reward` mismatch**:

* **Read path** (`get-pending-rewards`, lines 120–145): for a deactivated
  position, the `cumm-reward` used to compute the holder's owed rewards is the
  **frozen** `deactivated-cumm-reward` (lines 125–132).
* **Write path** (`update-holder-position` / `update-holder-position-amount`
  in `ststxbtc-tracking-data-v2.clar`, lines 183–210): the holder's
  `cumm-reward` checkpoint is always set to the **live global**
  `(get-cumm-reward)`, regardless of deactivation.

After a holder of a deactivated position has been checkpointed once via
`save-pending-rewards`, `claim-pending-rewards`, or `refresh-position`
(all three call `update-holder-position`), their `holders-info.cumm-reward`
is bumped to `C_global`. Once the global grows past the deactivation snapshot
(`C_global > C_deact`, which happens as soon as any further `add-rewards`
streams in for active wallets), every subsequent
`get-pending-rewards(holder, position)` evaluates
`amount-owed-per-token = (- C_deact C_global)` → **uint underflow → Clarity
runtime abort**. `claim-pending-rewards` `unwrap-panic`s on `get-pending-rewards`
(line 157), so it also aborts; the banked `saved-rewards` entry is unreachable
without a DAO contract upgrade.

**Amplifier:** `save-pending-rewards` is `define-public` with **no
`check-is-protocol` guard** (its sibling `refresh-wallet` does have one). Any
external account can therefore apply the bricking to **any** victim holder for
a deactivated position — *after* banking the victim's full pending into
`saved-rewards` (which the function does unconditionally when
`pending > existing`), thus **maximizing the locked amount** before locking it.

---

## 2. Code verification (function by function)

Verified against the deployed source
`/home/z/stackingdao-audit/stackingdao-smart-contracts/mainnet/contracts/rewards/ststxbtc-tracking-v2.clar`
and `…/ststxbtc-tracking-data-v2.clar`. Line numbers below are from those files.

### 2.1 `get-pending-rewards` — lines 120–145 of `ststxbtc-tracking-v2.clar`

```clar
(define-read-only (get-pending-rewards (holder principal) (position principal))
  (let (
    (holders-info (contract-call? .ststxbtc-tracking-data-v2 get-holder-position holder position))

    (supported-position (contract-call? .ststxbtc-tracking-data-v2 get-supported-positions position))
    (cumm-reward (if
      (and
        (not (is-eq holder position))                        ; ← wallet positions are exempt
        (not (is-eq (get deactivated-cumm-reward supported-position) u0))
      )
        (get deactivated-cumm-reward supported-position)     ; ← FROZEN cumm-reward
        (contract-call? .ststxbtc-tracking-data-v2 get-cumm-reward)
    ))

    (amount-owed-per-token (- cumm-reward (get cumm-reward holders-info)))   ; ← line 134, underflow site
    ...
```

**Confirmed:** for a deactivated `(holder, position)` pair with
`holder != position`, `cumm-reward` is the **frozen** `deactivated-cumm-reward`
from `supported-positions`, while `holders-info.cumm-reward` is whatever the
write path last checkpointed. If the write path put `C_global` there and
`C_global > C_deact`, the `(- cumm-reward …)` on line 134 underflows the uint
and panics. ✅

### 2.2 `save-pending-rewards` — lines 82–97 of `ststxbtc-tracking-v2.clar`

```clar
(define-public (save-pending-rewards (holder principal) (position principal))
  (let (
    (pending-rewards (unwrap-panic (get-pending-rewards holder position)))
    (existing-rewards (get-saved-rewards holder position))
  )
    (if (> (- pending-rewards existing-rewards) u0)
      (begin
        (map-set saved-rewards { holder: holder, position: position } pending-rewards)
        ;; To set current cumm-reward
        (try! (contract-call? .ststxbtc-tracking-data-v2 update-holder-position holder position))
        (ok pending-rewards)
      )
      (ok u0)
    )
  )
)
```

**Confirmed:**
* `define-public` — externally callable. ✅
* **No `check-is-protocol` guard** (compare `refresh-wallet` at line 46:
  `(try! (contract-call? .dao check-is-protocol contract-caller))`). ✅
* Calls `update-holder-position` for any `(holder, position)`, which writes
  the live global `cumm-reward` to the holder's checkpoint — arming the
  underflow. ✅
* Banks `pending-rewards` into `saved-rewards` BEFORE checkpointing, so the
  banked amount is maximized. ✅

### 2.3 `claim-pending-rewards` — lines 155–172 of `ststxbtc-tracking-v2.clar`

```clar
(define-public (claim-pending-rewards (holder principal) (position principal))
  (let (
    (pending-rewards (unwrap-panic (get-pending-rewards holder position)))   ; ← line 157
  )
    (asserts! (var-get claims-enabled) (err ERR_CLAIMS_DISABLED))

    (if (>= pending-rewards u1)
      (begin
        (try! (as-contract (contract-call? '…sbtc-token transfer pending-rewards tx-sender holder none)))
        (map-delete saved-rewards { holder: holder, position: position })
        (try! (contract-call? .ststxbtc-tracking-data-v2 update-holder-position holder position))   ; ← line 165
        (ok pending-rewards)
      )
      (ok u0)
    )
  )
)
```

**Confirmed:**
* `unwrap-panic (get-pending-rewards …)` at line 157 — propagates the
  underflow panic, aborting the claim. ✅
* `update-holder-position` at line 165 — even a successful first claim
  checkpoints the holder to `C_global`, arming the underflow for the *next*
  claim. ✅ (Self-brick path.)

### 2.4 `update-holder-position` / `update-holder-position-amount` — lines 183–210 of `ststxbtc-tracking-data-v2.clar`

```clar
(define-public (update-holder-position (holder principal) (position principal))
  (begin
    (try! (contract-call? .dao check-is-protocol contract-caller))   ; ← only checks contract-caller == tracking contract
    (try! (add-holder holder))
    (map-set holder-position { holder: holder, position: position } (merge
      (get-holder-position holder position)
      { cumm-reward: (get-cumm-reward) }                            ; ← LIVE global, not deactivated
    ))
    (ok true)
  )
)
```

**Confirmed:** unconditionally writes `(get-cumm-reward)` (the live global)
to the holder's `cumm-reward` checkpoint, regardless of whether the position
is deactivated. This is the root cause of the read/write mismatch. ✅

### 2.5 `set-supported-positions` (deactivation branch) — lines 215–218 of `ststxbtc-tracking-v2.clar`

```clar
(begin
  (try! (contract-call? .ststxbtc-tracking-data-v2 update-holder-position position-address position-address))
  (contract-call? .ststxbtc-tracking-data-v2 set-supported-positions
    position-address active (get reserve supported-position)
    (get total supported-position)
    (contract-call? .ststxbtc-tracking-data-v2 get-cumm-reward)))   ; ← freezes deactivated-cumm-reward := live global
```

**Confirmed:** at deactivation, `deactivated-cumm-reward` is frozen at the
**current** global `cumm-reward`. The write path then continues to advance the
holder's checkpoint past this frozen value via subsequent `add-rewards`. ✅

### 2.6 Recovery path — `withdraw-tokens`, lines 178–186 of `ststxbtc-tracking-v2.clar`

```clar
(define-public (withdraw-tokens (recipient principal) (amount uint))
  (begin
    (try! (contract-call? .dao check-is-protocol contract-caller))
    (try! (as-contract (contract-call? '…sbtc-token transfer amount tx-sender recipient none)))
    (ok true)
  )
)
```

**Confirmed:** the only on-chain escape is DAO-gated (`check-is-protocol`).
It can sweep sBTC out of the contract but does **not** update any holder's
`saved-rewards` or `holders-info` state — so redistributing the frozen funds
to victims requires a separate DAO decision and likely a contract upgrade via
the 7-day-timelocked `dao-executor`. This is the basis for "High" rather than
"Critical" severity (a governance-mediated recovery path exists). ✅

---

## 3. Proof of Concept — Clarinet/vitest, simnet (RUNS, ALL 3 CASES PASS)

**File:** `/home/z/stackingdao-audit/stackingdao-contracts/tests/core/rewards_freeze_poc_test.ts`
**Harness:** `clarinet` + `vitest` (the project's own test harness, identical
to their CI; the local `ststxbtc-tracking-v2.clar` differs from mainnet only
by a `.sbtc-token` mock address, not by logic — confirmed via `diff`).
**Run command:** `npx vitest run tests/core/rewards_freeze_poc_test.ts`

```
 ✓ tests/core/rewards_freeze_poc_test.ts  (3 tests) 324ms
   ✓ PoC: rewards-freeze on deactivated position > reproduces the freeze end-to-end (attacker-driven)
   ✓ PoC: rewards-freeze on deactivated position > self-brick: even without an attacker, the victim's own claim bricks the entry
   ✓ PoC: rewards-freeze on deactivated position > control: a never-deactivated position does NOT brick
 Test Files  1 passed (1)
      Tests  3 passed (3)
```

Existing project tests (`tests/core/ststxbtc-tracking_test.ts`, 20 tests) still
pass — the PoC is purely additive and does not modify any contract code.

### 3.1 Test 1 — Attacker-driven freeze (the headline scenario)

Setup:
* Deployer mints 1 000 stSTXbtc to `position-mock` (reserve) and 1 000 to
  `wallet_3` (active wallet). Total supply = 2 000 stSTXbtc.
* Deployer activates `position-mock` as a supported position.
* Deployer refreshes `wallet_1`'s position via `position-mock`
  → `holders_info[wallet_1, position-mock] = { amount: 100, cumm-reward: 0 }`.
* Deployer streams 300 sats via `add-rewards` → global `cumm-reward = 150e9`
  (a.k.a. `C_deact`).
* Pre-deactivation snapshot: `get-pending-rewards(wallet_1, position-mock) = 15 sats` ✓

Attack sequence (after the DAO has deactivated the position and after the
keeper streamed a *second* 300 sats of rewards for active wallets, pushing
the global to `300e9 > C_deact`):

1. **`wallet_2` (the attacker) calls `save-pending-rewards(wallet_1, position-mock)`.**
   * `get-pending-rewards(wallet_1, position-mock)` returns 15 sats — no panic
     yet, since `wallet_1`'s checkpoint is still 0 ≤ `C_deact`.
   * `saved-rewards[wallet_1, position-mock] := 15 sats` (banked in full).
   * `update-holder-position(wallet_1, position-mock)` sets
     `holders_info[wallet_1, position-mock].cumm-reward := 300e9` (live global).
   * Returns `(ok 15)`.
2. **`wallet_1` (the victim) attempts `get-pending-rewards(wallet_1, position-mock)`.**
   * `amount-owed-per-token = (- C_deact=150e9 wallet_1.cumm-reward=300e9)` →
     **`Runtime(ArithmeticUnderflow)`** → call aborts. The clarinet SDK throws,
     the PoC's `tryGetPendingRewards` returns `null`.
3. **`wallet_1` attempts `claim-pending-rewards(wallet_1, position-mock)`.**
   * `unwrap-panic (get-pending-rewards …)` propagates the abort → claim reverts.
4. **Contract balance check:** `sbtc-token` balance of `ststxbtc-tracking-v2`
   is still **600 sats** (300 + 300, never left), and
   `saved-rewards[wallet_1, position-mock]` is still **15 sats** — owed but
   unreachable.

### 3.2 Test 2 — Self-brick (no attacker)

Same setup, but no `save-pending-rewards` call by `wallet_2`. Instead, the
victim's own first `claim-pending-rewards` (called after the second
`add-rewards`) succeeds in withdrawing 15 sats, then internally calls
`update-holder-position` which bumps the checkpoint to live global
(`300e9 > C_deact`). The very next `get-pending-rewards` and the very next
`claim-pending-rewards` both abort. **The bug is independent of the
unguarded-save amplifier** — the read/write mismatch alone is sufficient.

### 3.3 Test 3 — Control (no deactivation)

Negative control: identical setup and identical `save-pending-rewards` +
second `add-rewards` sequence, but **without** the deactivation step. The
read path uses the live global (no frozen `deactivated-cumm-reward`), so the
read and write `cumm-reward` values never diverge. The victim's
`get-pending-rewards` and `claim-pending-rewards` continue to succeed
(claim returns 30 sats — 15 banked + 15 newly accrued). This proves the bug
is gated by deactivation, not by the save/claim cycle in general.

---

## 4. Three-perspective re-verification

### 4.1 Prosecutor (the bug is real and severe)

* The underflow is observable at runtime: the simnet call produces
  `Runtime(ArithmeticUnderflow, Some([…, "ststxbtc-tracking-v2:get-pending-rewards", "_native_:special_let", "_native_:native_sub"]))`.
  This is the exact code path the original write-up identified. Not
  speculative.
* The unguarded `save-pending-rewards` is verified at the source level: no
  `check-is-protocol` (compare line 46 of `refresh-wallet`). Anyone can call
  it. The PoC does exactly this with `wallet_2` calling for the victim
  `wallet_1` — and it succeeds.
* Position deactivation is a first-class, documented operational feature
  (`set-supported-positions`, `deactivated-cumm-reward`, the over-reserve
  guard in `refresh-position`). It is realistically invoked whenever
  StackingDAO delists an integrated protocol. The bug is reachable in normal
  operation.
* The frozen value is real sBTC yield that has been **banked into the
  contract's own `saved-rewards` map** — it is allocated and owed to the
  user. The code defect makes it unclaimable. That is a fund freeze.

### 4.2 Defense (mitigating factors)

* The bug only manifests **after** a DAO-gated deactivation. An external
  attacker cannot create the precondition; they can only exploit it once
  governance acts. If no position is ever deactivated, the bug is
  unreachable. (Verified: Test 3, the no-deactivation control, does not
  brick.)
* For holders who claim *before* any post-deactivation `add-rewards` (i.e.
  before `C_global > C_deact`), the underflow does not yet occur and they
  can withdraw normally — so prompt claimers are unaffected. The attacker
  must win a race with the holder; the holder's own first claim also
  self-bricks them, but they got paid first.
* The frozen asset is **yield**, not principal. User stSTXbtc tokens and
  the underlying STX/sBTC principal are not affected (wallet transfers use
  `refresh-wallet(holder, holder)`, a `position == holder` key that is
  exempt from the deactivated-cumm-reward path by the `(not (is-eq holder
  position))` guard on line 127). No path to theft of principal exists here.
* Recovery is possible via the existing upgrade pathway (`dao-multisig` →
  `dao-executor` 7-day timelock → patched `ststxbtc-tracking-v2`), so the
  freeze is, in governance time, reversible — arguing for "temporary"
  (High) rather than "permanent" (Critical). Additionally, `withdraw-tokens`
  (DAO-only) can sweep the banked sBTC out for manual redistribution, though
  it does not fix the per-holder state.

### 4.3 Judge (verdict)

* The defect is genuine and the exploit path is valid and deterministic
  given the precondition. The Prosecutor's mechanism is correct: read/write
  `cumm-reward` mismatch + unguarded `save-pending-rewards` ⇒
  attacker-lockable accrued rewards.
* The Defense's points lower severity but do not refute it: the
  precondition (deactivation) is a realistic, documented governance action
  rather than an attacker-controlled trigger; the race favors the attacker
  (holders are often passive); and recovery requires a full DAO upgrade
  with 7-day timelock, during which funds are frozen.
* **Immunefi VSS 2.3 classification:** The frozen asset is sBTC *yield*
  (rewards that have accrued but not yet been claimed). Per StackingDAO's
  Immunefi page, "Permanent freezing of unclaimed yield" is explicitly
  categorized as **High** ($1k–$20k). The "Permanent freezing of funds"
  (Critical) category explicitly excludes unclaimed yield, and the
  governance-mediated recovery path further precludes Critical.
* **Verdict: HIGH.** Recommended fixes:
  (a) In `update-holder-position` / `update-holder-position-amount`,
      checkpoint to the **same** `cumm-reward` that `get-pending-rewards`
      will use (i.e. `deactivated-cumm-reward` for deactivated positions)
      instead of unconditionally `(get-cumm-reward)`.
  (b) Make `save-pending-rewards` either `define-private` or
      `check-is-protocol`-gated — it is currently public and unguarded,
      unlike its sibling `refresh-wallet`.
  (c) Defensively, change `amount-owed-per-token` to saturate at zero
      (`(if (> checkpoint cumm-reward) u0 (- cumm-reward checkpoint))`) so
      a checkpoint drift can never abort reads/claims. (Defense-in-depth;
      should not replace (a).)

---

## 5. Final verdict

**CONFIRMED — HIGH severity.**

* ✅ Read/write `cumm-reward` mismatch verified at the source level
  (`ststxbtc-tracking-v2.clar` lines 120–145 vs `ststxbtc-tracking-data-v2.clar`
  lines 183–210).
* ✅ `save-pending-rewards` unguarded (`define-public`, no
  `check-is-protocol`) verified at the source level (lines 82–97).
* ✅ End-to-end PoC reproduces the underflow runtime abort and the
  permanent lock of banked sBTC. All three vitest cases pass.
* ✅ Both `ststxbtc-tracking-v2` and `ststxbtc-tracking-data-v2` confirmed
  in Immunefi scope.
* ✅ Per Immunefi VSS 2.3 + StackingDAO's bug-bounty page, the correct
  severity is **HIGH** (Permanent freezing of unclaimed yield), reward
  range **$1k–$20k** depending on funds at risk.
* ⚠️ The "arguably CRITICAL" framing in the original write-up is
  **overstated** — should be **HIGH**. Critical explicitly excludes
  unclaimed yield.

**Submission recommendation:** Submit as **High** severity with the
attached vitest PoC.

---

## 6. Submission draft (Immunefi)

> **Title:** Permanent freeze of accrued sBTC rewards for holders of a deactivated supported position (read/write `cumm-reward` mismatch + unguarded `save-pending-rewards`)
>
> **Severity:** High
>
> **Impacts:**
> * Permanent freezing of unclaimed yield
>
> **In-scope assets:**
> * `SP4SZE494VC2YC5JYG7AYFQ44F5Q4PYV7DVMDPBG.ststxbtc-tracking-v2`
> * `SP4SZE494VC2YC5JYG7AYFQ44F5Q4PYV7DVMDPBG.ststxbtc-tracking-data-v2`
>
> **Summary:**
> `ststxbtc-tracking-v2` distributes sBTC rewards using a cumulative-reward-per-token
> (`cumm-reward`) pattern. Supported positions can be deactivated by the DAO via
> `set-supported-positions(active=false)`, which freezes the position's
> `deactivated-cumm-reward` at the global `cumm-reward` at deactivation time.
>
> The read path `get-pending-rewards` (lines 120–145 of `ststxbtc-tracking-v2.clar`)
> uses the **frozen** `deactivated-cumm-reward` for deactivated positions, while the
> write path `update-holder-position` / `update-holder-position-amount` (lines 183–210
> of `ststxbtc-tracking-data-v2.clar`) — invoked by `save-pending-rewards`,
> `claim-pending-rewards`, and `refresh-position` — always checkpoints the holder's
> `cumm-reward` to the **live global** `(get-cumm-reward)`.
>
> Once the global `cumm-reward` grows past the frozen `deactivated-cumm-reward`
> (which happens as soon as any further `add-rewards` streams in for active
> wallets), any post-deactivation save/claim/refresh bumps the holder's
> checkpoint above the frozen value. Every subsequent `get-pending-rewards(holder,
> position)` then evaluates `amount-owed-per-token = (- deactivated-cumm-reward
> holders-info.cumm-reward)`, which **underflows the unsigned integer** and
> panics (Clarity `Runtime(ArithmeticUnderflow)`). Because `claim-pending-rewards`
> `unwrap-panic`s on `get-pending-rewards` (line 157), the banked `saved-rewards`
> entry is **permanently unclaimable** without a DAO contract upgrade.
>
> **Amplifier:** `save-pending-rewards` is `define-public` with **no
> `check-is-protocol` guard** (unlike its sibling `refresh-wallet`). Any external
> account can therefore brick **any** victim holder for a deactivated position —
> *after* banking the victim's full pending into `saved-rewards` (which the
> function does unconditionally when `pending > existing`), maximizing the locked
> amount before locking it.
>
> **Vulnerability details:**
>
> * `ststxbtc-tracking-v2.clar` lines 82–97 — `save-pending-rewards` (no access control; calls `update-holder-position`).
> * `ststxbtc-tracking-v2.clar` lines 120–145 — `get-pending-rewards` (read path uses frozen `deactivated-cumm-reward`; line 134 `(- cumm-reward (get cumm-reward holders-info))` is the underflow site).
> * `ststxbtc-tracking-v2.clar` lines 155–172 — `claim-pending-rewards` (line 157 `unwrap-panic` propagates the abort; line 165 `update-holder-position` self-bricks).
> * `ststxbtc-tracking-v2.clar` lines 198–221 — `set-supported-positions` deactivation branch (line 217 freezes `deactivated-cumm-reward := (get-cumm-reward)`).
> * `ststxbtc-tracking-data-v2.clar` lines 183–210 — `update-holder-position` / `update-holder-position-amount` (write path always uses `(get-cumm-reward)` — the live global — regardless of deactivation).
>
> **Preconditions:**
>
> 1. DAO deactivates a supported position via `set-supported-positions(<position>, false, <reserve>)`. This is the documented mechanism for delisting an integrated protocol.
> 2. At least one `add-rewards` call streams sBTC rewards for active wallets after the deactivation, pushing the global `cumm-reward` past `deactivated-cumm-reward`.
>
> Both are realistic, in-normal-operation preconditions; #1 is DAO-gated, #2 happens automatically via the keeper.
>
> **Impact:**
>
> * **Permanent freezing of accrued sBTC rewards** for every holder of a deactivated supported position. The amount frozen per holder equals their full reward accrual up to deactivation.
> * Frozen funds are unrecoverable by the user; only a DAO contract upgrade (7-day timelock via `dao-executor`/`dao-multisig`) plus explicit per-holder recovery logic could remediate. The on-chain `withdraw-tokens` sweep does not reset per-holder state.
> * Secondary DoS: a bricked holder cannot `refresh-position` either, so the position's `total` can never be driven back to 0, permanently blocking reactivation (`set-supported-positions(active=true)` requires `total == 0`).
> * The bricking also poisons `claim-pending-rewards-many` / `get-pending-rewards-many` batches: a single bricked entry aborts the whole batch (`unwrap-panic` / read abort).
>
> The frozen asset is sBTC yield (not principal) and a governance-mediated recovery path exists, so per Immunefi VSS 2.3 this is **High** ("Permanent freezing of unclaimed yield"), not Critical.
>
> **Proof of Concept:**
>
> See attached file `rewards_freeze_poc_test.ts` (vitest + clarinet simnet, runs against the project's own `tests/` harness). Three cases:
>
> 1. **Attacker-driven freeze** — `wallet_2` calls `save-pending-rewards(wallet_1, position-mock)` after DAO-deactivation and after a second `add-rewards`. The victim's full pending (15 sats of sBTC) is banked into `saved-rewards`, then their checkpoint is bumped to the live global. Every subsequent `get-pending-rewards(wallet_1, position-mock)` aborts with `Runtime(ArithmeticUnderflow)`, and `claim-pending-rewards(wallet_1, position-mock)` also aborts. The 15 sats remain locked in `ststxbtc-tracking-v2`.
> 2. **Self-brick** — the victim's own first claim (after the second `add-rewards`) succeeds in withdrawing, but internally calls `update-holder-position` which bumps their checkpoint to the live global. Every subsequent read/claim aborts.
> 3. **Control** — identical sequence without deactivation does not brick; reads and claims continue to succeed.
>
> To run: `npx vitest run tests/core/rewards_freeze_poc_test.ts` from the `stackingdao-contracts/` directory.
>
> **Suggested remediation:**
>
> 1. In `update-holder-position` / `update-holder-position-amount`, write the *same* `cumm-reward` that `get-pending-rewards` will read (i.e. `deactivated-cumm-reward` for deactivated positions) instead of unconditionally `(get-cumm-reward)`.
> 2. Gate `save-pending-rewards` with `check-is-protocol` (or make it `define-private`) so external accounts cannot drive the bricking.
> 3. Defense-in-depth: change `amount-owed-per-token` to saturate at zero (`(if (> checkpoint cumm-reward) u0 (- cumm-reward checkpoint))`) so a checkpoint drift can never abort reads/claims.
