# Babylon Chain — Double Discount of Voting Power on Expired-Then-Undelegated BTC Delegation

- **Area:** BTC Staking / Slashing / Voting Power
- **Severity:** MEDIUM
- **Contract:** `btc-staking`
- **File:** `contracts/btc-staking/src/staking.rs`
- **Functions:** `process_expired_btc_delegations` (L271) and
  `handle_undelegation` (L220) / `discount_delegation_power` (L340)
- **Codebase:** `cosmos-bsn-contracts` (CosmWasm / Rust)

---

## Description

`process_expired_btc_delegations` (the staking contract's `BeginBlock` sudo)
discounts a delegation's voting power when its BTC end-height passes, but it
does **not** mark the delegation as "processed" — the only guard against
re-discounting is `BtcDelegation::is_unbonded_early()`, which checks
`undelegation_info.delegator_unbonding_info.is_some()`. Expiry processing
never sets that field:

```rust
// staking.rs:317-327 (process_expired_btc_delegations)
if !btc_del.is_unbonded_early() {
    discount_delegation_power(
        deps.storage,
        env.block.height,
        &staking_tx_hash,
        &btc_del,
    )?;
}
// ↑ no flag set on btc_del; the index entry is removed but the delegation
//   record still looks "active" to handle_undelegation
```

`handle_undelegation` (invoked later via an IBC `BtcStaking{unbonded_del}`
packet, or by the admin) re-checks the same flag and, finding it `None`,
proceeds to call `discount_delegation_power` **again** and then
`btc_undelegate` (which finally sets `delegator_unbonding_info`):

```rust
// staking.rs:233-248 (handle_undelegation)
if btc_del.is_unbonded_early() {
    return Err(ContractError::DelegationIsNotActive(...));
}
verify_undelegation(&cfg, &btc_del)?;            // no-op
btc_undelegate(storage, &staking_tx_hash, &mut btc_del)?;
discount_delegation_power(storage, height, staking_tx_hash.as_ref(), &btc_del)?;
```

`discount_delegation_power` uses `saturating_sub`, so the second call does not
panic, but it subtracts `btc_del.total_sat` from every affected FP's
`total_active_sats` and from the per-(tx,FP) `DelegationDistribution.stake`
**a second time**. The FP's voting power is therefore understated by
`total_sat`, and the delegation's recorded stake goes to 0 (or would go
negative without saturation).

## Contract + Function + Line

```
contracts/btc-staking/src/staking.rs
  L271  pub fn process_expired_btc_delegations(...)        // BeginBlock sudo
  L319      if !btc_del.is_unbonded_early() {
  L321          discount_delegation_power(...)?;            // 1st discount
  L327      }
  //  — no mutation of btc_del to prevent a later 2nd discount —

  L220  fn handle_undelegation(...)                         // BtcStaking exec
  L235      if btc_del.is_unbonded_early() { return Err(...); }   // still false
  L245      btc_undelegate(storage, ...)?;                  // sets the flag AFTER
  L248      discount_delegation_power(...)?;                // 2nd discount

  L340  fn discount_delegation_power(...)
  L356      fp_state.total_active_sats = fp_state.total_active_sats
  L357          .saturating_sub(btc_del.total_sat);
  L364      delegation.stake = delegation.stake
                 .saturating_sub(btc_del.total_sat);
```

## Attack Scenario

This requires the trusted Babylon Genesis to relay (or the admin to send) an
`unbonded_del` packet for a delegation whose BTC expiry height has already
been processed by `process_expired_btc_delegations`. Two realistic ways this
happens:

1. **Race / relayer latency:** A delegator broadcasts an early-unbonding BTC
   tx; the BSN's BTC light client tip advances past the delegation's
   `end_height − unbonding_time` (triggering expiry processing in
   `BeginBlock`) before Babylon's `unbonded_del` IBC packet lands. The packet
   then arrives and `handle_undelegation` double-discounts.
2. **Babylon re-send / replay:** If Babylon (or a relayer) re-sends an
   `unbonded_del` for a delegation that the BSN already expired (e.g. after an
   IBC timeout+retry, or a Babylon-side bug), the BSN accepts it because
   `is_unbonded_early()` is still `false` (expiry never set it).

Effect: the affected FP(s) lose `total_sat` of voting power they should still
have (from other, still-active delegations). If the FP's remaining
`total_active_sats` is small, this can drop them out of the active set or to
zero power, silently censoring their finality votes and skewing the 2/3 quorum
computation. In aggregate across many delegations this can stall block
finalisation.

## PoC (Rust / multi-test sketch)

```rust
// Pseudocode — adapt to the existing `active_delegation_happy_path` harness
// in contracts/btc-staking/src/staking.rs::tests

// 1. Register FP, add active delegation with total_sat = T.
//    fp_state.total_active_sats == T after this.
//
// 2. Simulate BTC tip advancing past actual_expiry_height and run
//    process_expired_btc_delegations:
//      - discount_delegation_power subtracts T
//      - fp_state.total_active_sats == 0  (correct so far)
//      - delegation.stake == 0
//      - BTC_DELEGATION_EXPIRY_INDEX entry removed
//      - BUT btc_del.undelegation_info.delegator_unbonding_info is still None
//
// 3. Now call execute(BtcStaking{ unbonded_del: [UnbondedBtcDelegation{
//        staking_tx_hash: <same hash> }] }).
//      - handle_undelegation:
//          is_unbonded_early() == false  → passes guard
//          btc_undelegate sets delegator_unbonding_info = Some(...)
//          discount_delegation_power subtracts T AGAIN
//      - fp_state.total_active_sats = 0.saturating_sub(T) == 0  (hidden by saturate)
//      - delegation.stake = 0.saturating_sub(T) == 0            (hidden by saturate)
//
// 4. Now add a SECOND delegation (total_sat = T2) to the same FP.
//    Expected: fp_state.total_active_sats == T2.
//    Actual:   fp_state.total_active_sats == T2  (because the double-discount
//              was saturating-subtracted from 0, not from T2).
//    BUT the per-delegation DelegationDistribution for the expired tx now
//    records stake = 0 while the FP's total_active_sats was reduced by an
//    extra T — the books are internally inconsistent, and any later
//    `discount_delegation_power` on the SECOND delegation will subtract T2
//    from an fp_state that is already short by T.

// Direct assertion of the inconsistency:
// After step 3, sum of all DelegationDistribution.stake for the FP  == 0
// (only the second delegation, with stake T2, should remain).
// fp_state.total_active_sats == T2 (from step 4).
// These agree ONLY because saturating_sub hid the underflow.
// The real damage shows when you query `finality_provider_info` between
// step 3 and step 4: total_active_sats == 0 even though other delegations
// to this FP exist and should still be counted.
```

The cleanest reproduction is in the existing `multitest` suite: advance the
BTC light-client mock past `end_height - unbonding_time`, call
`process_expired_btc_delegations`, then send the `unbonded_del` message and
observe that `discount_delegation_power` runs twice (add a debug log or
assert on `fp_state.total_active_sats` after each step).

## Impact

- **Voting-power accounting corruption:** affected FPs' `total_active_sats`
  is understated, which flows into `compute_active_finality_providers` and
  the 2/3 quorum check in `tally`. Depending on delegation distribution this
  can:
  - drop a legitimate FP out of the active set (censorship / liveness),
  - or, when many delegations are hit, make the chain unable to reach 2/3
    finality (DoS).
- The `saturating_sub` masks the bug from diagnostics — no panic, just
  silently wrong power.
- Requires the trusted Babylon to deliver an `unbonded_del` for an already-
  expired delegation, which the IBC timeout/retry path and Babylon-side
  edge cases make plausible (and the admin path makes trivial).

## Severity Justification (3 perspectives)

**Loss vs. likelihood:** Impact is a correctness/availability break in voting
power, but the trigger requires the trusted source to deliver a slightly-
out-of-order undelegation. Not directly attacker-controlled by an arbitrary
user, but realistic under relayer latency or IBC retry. → **MEDIUM**.

**Sherlock rubric:** Corrupts core accounting used for safety-critical quorum
decisions; impact ranges from single-FP griefing to chain-wide finality
stall depending on blast radius. Not a direct fund theft. → **MEDIUM**.

**Code-level:** The root cause is that `process_expired_btc_delegations`
shares the "discounted" invariant with `handle_undelegation` via a single
flag (`is_unbonded_early`) that only the undelegation path sets. A second,
explicit "expiry-processed" flag (or unifying on one "power already
discounted" bit) closes the hole.

## Suggested Fix

Mark the delegation as power-discounted when expiry processing runs, and have
`handle_undelegation` (and `discount_delegation_power`) short-circuit when
that bit is already set:

```rust
// In BtcDelegation, add:  pub power_discounted: bool   (or reuse a dedicated
// `DelegationStatus::Expired` variant).

// process_expired_btc_delegations:
if !btc_del.is_unbonded_early() && !btc_del.power_discounted {
    discount_delegation_power(...)?;
    btc_del.power_discounted = true;
    BTC_DELEGATIONS.save(deps.storage, &staking_tx_hash, &btc_del)?;
}

// handle_undelegation / discount_delegation_power:
if btc_del.power_discounted {
    // power already removed; only set the unbonding tx record
} else {
    discount_delegation_power(...)?;
}
```

Alternatively, have `process_expired_btc_delegations` call `btc_undelegate`
(so `is_unbonded_early()` becomes true and the existing guard in
`handle_undelegation` works), matching the Babylon Genesis invariant that an
expired delegation is treated as unbonded.
