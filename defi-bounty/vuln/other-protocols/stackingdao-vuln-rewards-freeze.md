# StackingDAO — Permanent Freeze of Accrued sBTC Rewards for Holders of a Deactivated Position

- **Area:** Reward distribution (`ststxbtc-tracking-v2` / `ststxbtc-tracking-data-v2`)
- **Severity:** HIGH (arguably CRITICAL) — permanent freezing of accrued user sBTC rewards
- **Bug class:** Unsigned-integer underflow / checkpoint-vs-cumulative-reward mismatch
- **Exploitable by:** Any external account, **after** a DAO `set-supported-positions(active=false)` deactivation
- **Network:** Stacks mainnet, deployer `SP4SZE494VC2YC5JYG7AYFQ44F5Q4PYV7DVMDPBG`
- **Immunefi scope contracts:**
  - `SP4SZE494VC2YC5JYG7AYFQ44F5Q4PYV7DVMDPBG.ststxbtc-tracking-v2`
  - `SP4SZE494VC2YC5JYG7AYFQ44F5Q4PYV7DVMDPBG.ststxbtc-tracking-data-v2`

---

## 1. Description

`ststxbtc-tracking-v2` distributes sBTC rewards to stSTXbtc holders using the standard
*cumulative-reward-per-token* (`cumm-reward`) pattern. External protocols that hold stSTXbtc
on behalf of users ("supported positions", e.g. a DEX LP or lending market) are registered via
`set-supported-positions` and can be **deactivated**. When a position is deactivated, its
`deactivated-cumm-reward` is frozen at the global `cumm-reward` at deactivation time, so that
holders of that position stop accruing new rewards (intended).

The bug is a **mismatch between the `cumm-reward` used to *read* pending rewards and the
`cumm-reward` used to *write* (checkpoint) a holder's entry**:

- `get-pending-rewards(holder, position)` (read path) uses the **frozen**
  `deactivated-cumm-reward` for a deactivated position (lines 125–132), and computes
  `amount-owed-per-token = deactivated-cumm-reward - holders-info.cumm-reward` (line 134).
- `update-holder-position` / `update-holder-position-amount` (write path, called by
  `save-pending-rewards`, `claim-pending-rewards`, `refresh-position`) always checkpoint the
  holder's `cumm-reward` to the **live global** `cumm-reward` (`(get-cumm-reward)`), which
  keeps growing via `add-rewards` and is therefore **strictly greater** than the frozen
  `deactivated-cumm-reward` once any new rewards stream in.

After a holder of a deactivated position is checkpointed once (by their own claim/refresh, or
by an attacker — see §3), `holders-info.cumm-reward > deactivated-cumm-reward`, so the
unsigned subtraction `(- cumm-reward holders-info.cumm-reward)` **underflows → runtime abort**.
From that point on, *every* `get-pending-rewards(holder, position)` call aborts. Because
`claim-pending-rewards` and `save-pending-rewards` both `unwrap-panic` on
`get-pending-rewards`, the holder can **never again claim** the sBTC already banked in their
`saved-rewards` map entry, and can never refresh the position. The sBTC is permanently locked
inside the tracking contract (recoverable only by a DAO contract upgrade, not by the user).

Crucially, `save-pending-rewards` is a **public function with no access control** (no
`check-is-protocol`), so an attacker can trigger the bricking for *any* victim holder — banking
the victim's full accrued pending into `saved-rewards` (so the amount is maximized) *before*
the victim ever claims, then locking it forever.

---

## 2. Contract / Function / Line

File: `mainnet/contracts/rewards/ststxbtc-tracking-v2.clar` (deployed source)

- `get-pending-rewards` — lines **120–145**, specifically line **134**:
  ```clar
  (amount-owed-per-token (- cumm-reward (get cumm-reward holders-info)))
  ```
  where `cumm-reward` is the frozen `deactivated-cumm-reward` (lines 125–132) but
  `holders-info.cumm-reward` was checkpointed to the live global value.

- `save-pending-rewards` — lines **82–97** — **no access control**; on `pending > existing`
  it sets `saved-rewards := pending` and calls `update-holder-position` (checkpoints to live
  global), arming the underflow.

- `claim-pending-rewards` — lines **155–172** — `unwrap-panic (get-pending-rewards …)` at
  line **157** aborts once the entry is bricked, so the banked `saved-rewards` can never be
  withdrawn.

File: `mainnet/contracts/rewards/ststxbtc-tracking-data-v2.clar`

- `update-holder-position` — lines **183–195** — sets `cumm-reward: (get-cumm-reward)`
  (live global), the root cause of the mismatch.
- `update-holder-position-amount` — lines **197–210** — same: `cumm-reward: (get-cumm-reward)`.
- `set-supported-positions` (deactivation branch in `ststxbtc-tracking-v2.clar` lines 215–218)
  sets `deactivated-cumm-reward := (get-cumm-reward)` at deactivation time.

---

## 3. Attack scenario

Preconditions (all realistic, only #1 is governance):

1. A supported position is **deactivated** by the DAO via
   `set-supported-positions(<position>, false, reserve)`. This is the documented mechanism for
   delisting an integrated protocol (e.g. a compromised/sunset DeFi venue holding stSTXbtc).
   `deactivated-cumm-reward` is frozen at the global `cumm-reward` `C_deact`.
2. A victim holder `V` of that position has accrued, unbanked rewards:
   `checkpoint = C_old ≤ C_deact`, `amount = A > 0`, `saved = S`. Their pending =
   `A·(C_deact − C_old)/1e10 + S`.
3. After deactivation, the keeper continues streaming rewards to active wallets, so the global
   `cumm-reward` keeps growing: `C_global > C_deact`.

Attack (single transaction, by anyone):

1. Attacker calls `save-pending-rewards(V, <position>)`.
   - `get-pending-rewards(V, position)` returns `pending = A·(C_deact − C_old)/1e10 + S`
     (no abort yet: `C_old ≤ C_deact`).
   - Since `pending > S`, the function sets `saved-rewards[V,position] := pending` (victim's
     **full** accrued amount is now banked) and calls `update-holder-position(V, position)`,
     setting `V`'s checkpoint to `C_global` (> `C_deact`).
2. Victim `V` later calls `claim-pending-rewards(V, <position>)`.
   - `unwrap-panic (get-pending-rewards(V, position))` evaluates
     `(- C_deact C_global)` ⇒ **uint underflow ⇒ transaction aborts**.
   - The claim reverts. So does any future `refresh-position` / `save-pending-rewards` /
   `claim-pending-rewards` for `(V, position)`.
3. The sBTC equal to `pending` (= `A·(C_deact − C_old)/1e10 + S`, i.e. everything `V` had
   accrued up to deactivation) is **permanently locked** in `ststxbtc-tracking-v2`.
   `withdraw-tokens` (DAO-only) is the only exit, i.e. user funds are frozen without a
   governance upgrade.

Note: even without an attacker, a holder who claims *once* after deactivation self-bricks their
entry (claim sets checkpoint to `C_global`). The attacker's contribution is to brick victims
*before* they can claim, and to bank the maximum amount into `saved-rewards` first — turning a
"claim once then done" UX wart into a **total, permanent loss of all accrued rewards** for
every holder of the deactivated position.

---

## 4. Proof of Concept (Clarinet / vitest, simnet)

> Foundry is not applicable: StackingDAO is Clarity on Stacks (Bitcoin L2), **not** Solidity /
> EVM. The project's own test harness is `clarinet` + `vitest` (see
> `stackingdao-smart-contracts/local-testing`). The PoC below mirrors that harness and exercises
> the deployed `mainnet/contracts/rewards/ststxbtc-tracking-v2.clar` source.

```ts
// tests/rewards-freeze.poc.ts
import { describe, it, expect, beforeAll } from "vitest";
import { Clarinet, Tx, types, Chain, Account } from "clarinet";

// Minimal position-trait implementer holding stSTXbtc on behalf of users.
// get-holder-balance(holder) returns the tracked balance; mint/transfer not shown for brevity.
const POSITION = ".fake-position-v1"; // implements .position-trait-v1

describe("rewards freeze on deactivated position", () => {
  let chain: Chain, deployer: Account, keeper: Account, victim: Account, attacker: Account;

  beforeAll(() => {
    [chain, deployer, keeper, victim, attacker] = setupChain(); // helper as in existing tests
    // dao whitelist tracking + position, set rewards-pox5 keeper, etc. (existing helpers)
    // 1) activate the position as a supported position
    chain.tx(actor(deployer), Tx.contractCall("ststxbtc-tracking-v2",
      "set-supported-positions", [POSITION, true, types.principal(RESERVE)]));
    // 2) victim holds A stSTXbtc via the position; refresh-position to checkpoint at C_old
    chain.tx(actor(victim), Tx.contractCall("ststxbtc-tracking-v2",
      "refresh-position", [types.principal(victim.addr), POSITION_TRAIT]));
    // 3) keeper streams sBTC rewards -> global cumm-reward grows to C_deact
    chain.tx(actor(keeper), Tx.contractCall("rewards-pox5-v1", "process-rewards", []));
    // 4) DAO deactivates the position (deactivated-cumm-reward := C_deact)
    chain.tx(actor(deployer), Tx.contractCall("ststxbtc-tracking-v2",
      "set-supported-positions", [POSITION, false, types.principal(RESERVE)]));
    // 5) more rewards stream to active wallets -> global cumm-reward grows to C_global > C_deact
    chain.tx(actor(keeper), Tx.contractCall("rewards-pox5-v1", "process-rewards", []));
  });

  it("victim has accrued, claimable rewards before attack", () => {
    const r = chain.callReadOnly("ststxbtc-tracking-v2", "get-pending-rewards",
      [types.principal(victim.addr), types.principal(POSITION)]);
    expect(r.result).toBeOk(expect.any(BigInt)); // > 0
    expect(Number(r.result)).toBeGreaterThan(0);
  });

  it("attacker bricks the victim's entry via unguarded save-pending-rewards", () => {
    // Banks the victim's FULL pending into saved-rewards AND checkpoints to live global.
    const r = chain.tx(actor(attacker), Tx.contractCall("ststxbtc-tracking-v2",
      "save-pending-rewards", [types.principal(victim.addr), types.principal(POSITION)]));
    expect(r.receipts[0].result).toBeOk(expect.any(BigInt)); // pending banked
  });

  it("victim can no longer read pending (underflow abort)", () => {
    const r = chain.callReadOnly("ststxbtc-tracking-v2", "get-pending-rewards",
      [types.principal(victim.addr), types.principal(POSITION)]);
    // Clarity uint underflow -> runtime abort
    expect(r.error).toBeDefined();
  });

  it("victim cannot claim -> accrued sBTC permanently locked", () => {
    const r = chain.tx(actor(victim), Tx.contractCall("ststxbtc-tracking-v2",
      "claim-pending-rewards", [types.principal(victim.addr), types.principal(POSITION)]));
    expect(r.receipts[0].error).toBeDefined(); // aborted
    // tracking contract balance of sBTC still holds the victim's accrued amount
  });
});
```

Expected: after step "attacker bricks", every subsequent `get-pending-rewards` /
`claim-pending-rewards` for `(victim, POSITION)` aborts; the victim's banked sBTC stays in
`ststxbtc-tracking-v2` and is unreachable without a DAO upgrade.

---

## 5. Impact

- **Permanent freezing of accrued sBTC rewards** for every holder of a deactivated supported
  position. The amount frozen per holder = their full reward accrual up to deactivation.
- Aggregated across all holders of the delisted position, this can be a large sBTC amount
  (proportional to the position's TVL × reward rate × time held).
- Frozen funds are unrecoverable by the user; only a DAO contract upgrade (7-day timelock via
  `dao-multisig.execute-impl-update`) could remediate, and even then the per-holder `saved-rewards`
  state would need explicit recovery logic.
- Secondary DoS: once bricked, the holder cannot `refresh-position` either, so the position's
  `total` can never be driven back to 0, permanently blocking reactivation
  (`set-supported-positions(active=true)` requires `total == 0`).
- The bricking also poisons `claim-pending-rewards-many` / `get-pending-rewards-many` batches:
  a single bricked entry aborts the whole batch (unwrap-panic / read abort).

This matches Immunefi's **"Permanent freezing of funds"** (Critical) and at minimum
**"Temporary freezing of funds"** (High). Severity is tempered only by the DAO-deactivation
precondition (see §6).

---

## 6. Three-perspective audit

### Prosecutor (the bug is real and severe)
- The underflow is a concrete, deterministic Clarity runtime abort — not speculative. The
  read-path uses a frozen `cumm-reward` while the write-path checkpoints the live `cumm-reward`;
  these two diverge by construction the moment rewards stream after a deactivation.
- `save-pending-rewards` has **no `check-is-protocol` guard** (compare `refresh-wallet`, which
  does), so an attacker can apply the bricking to any victim at any time after deactivation —
  and they can do it *before* the victim claims, banking the maximum into `saved-rewards` and
  then locking it. This converts a self-inflicted UX bug into an attacker-driven permanent fund
  freeze.
- Position deactivation is a first-class, documented operational feature (`set-supported-positions`,
  `deactivated-cumm-reward`, the `refresh-position` over-reserve guard, etc.). It is realistically
  invoked whenever StackingDAO delists an integrated protocol. The bug is therefore reachable in
  normal operation, not only under exotic conditions.
- The locked value is real sBTC (yield) that has been *earned and banked* into the contract's own
  `saved-rewards` map — it is owed to the user. "Unclaimed yield" in the Immunefi wording refers
  to yield not yet accrued/allocated; here the allocation is done, the funds are sitting in the
  contract, and a code defect makes them unclaimable. That is a freeze of funds.

### Defense (mitigating factors)
- The bug only manifests **after** a DAO-gated deactivation. An external attacker cannot create
  the precondition; they can only exploit it once governance acts. If no position is ever
  deactivated, the bug is unreachable.
- For holders who claim *before* any post-deactivation `add-rewards` (i.e. before the global
  `cumm-reward` advances past `C_deact`) the underflow does not yet occur and they can withdraw
  normally — so prompt claimers are unaffected. The attacker must win a race with the holder.
- The frozen asset is **yield**, not principal; user stSTXbtc tokens and the underlying STX/sBTC
  principal are not affected (wallet transfers use `refresh-wallet(holder, holder)`, a different
  `position` key that is never deactivated). No path to theft of principal exists here.
- Recovery is possible via the existing upgrade pathway (`dao-multisig` → `dao-executor` 7-day
  timelock → patched `ststxbtc-tracking-v2`), so the freeze is, in governance time, reversible —
  arguing for "temporary" (High) rather than "permanent" (Critical). Also, `claims-enabled` can be
  toggled and a recovery proposal-script could redistribute, so it is not strictly irreversible.

### Judge (verdict)
- The defect is genuine and the exploit path is valid and deterministic given the precondition.
  The Prosecutor's mechanism is correct: read/write `cumm-reward` mismatch + unguarded
  `save-pending-rewards` ⇒ attacker-lockable accrued rewards.
- The Defense's points lower severity but do not refute it: the precondition (deactivation) is a
  realistic, documented governance action rather than an attacker-controlled trigger; the race
  favors the attacker (holders are often passive); and recovery requires a full DAO upgrade with
  7-day timelock, during which funds are frozen.
- **Verdict: HIGH.** It satisfies Immunefi's "Temporary freezing of funds" (High) cleanly, and
  is on the borderline of "Permanent freezing of funds" (Critical); the existence of a
  governance-mediated recovery path keeps it at High rather than Critical. Recommended fixes:
  (a) in `update-holder-position` / `update-holder-position-amount`, checkpoint to the *same*
  `cumm-reward` that `get-pending-rewards` will use (i.e. `deactivated-cumm-reward` for
  deactivated positions) instead of unconditionally `(get-cumm-reward)`; (b) make
  `save-pending-rewards` either `define-private` or `check-is-protocol`-gated (it is currently
  public and unguarded, unlike its sibling `refresh-wallet`); (c) defensively, change
  `amount-owed-per-token` to saturate at zero (`(if (> checkpoint cumm-reward) u0 (- cumm-reward
  checkpoint))`) so a checkpoint drift can never abort reads/claims.

---

## 7. References

- Deployed source: `stackingdao-smart-contracts/mainnet/contracts/rewards/ststxbtc-tracking-v2.clar`
- Data contract: `stackingdao-smart-contracts/mainnet/contracts/rewards/ststxbtc-tracking-data-v2.clar`
- SECURITY.md: `https://github.com/StackingDAO/stackingdao-smart-contracts/blob/main/SECURITY.md`
- Immunefi scope: `https://immunefi.com/bug-bounty/stackingdao/scope/`
  (assets `ststxbtc-tracking-v2`, `ststxbtc-tracking-data-v2`)
