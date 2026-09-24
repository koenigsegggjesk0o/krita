# Babylon Chain — Tally Quorum Arithmetic Uses Unchecked u64 (Theoretical Overflow)

- **Area:** Finality / Integer Precision
- **Severity:** LOW
- **Contract:** `btc-finality`
- **File:** `contracts/btc-finality/src/tallying.rs`
- **Function:** `tally` (lines 142–153)
- **Codebase:** `cosmos-bsn-contracts` (CosmWasm / Rust)

---

## Description

The 2/3-quorum check uses plain `u64` addition and multiplication with no
overflow protection:

```rust
// tallying.rs:142-153
fn tally(fp_power_table: &HashMap<String, u64>, voters: &[String]) -> bool {
    let voters: HashSet<_> = voters.iter().collect();
    let mut total_power = 0u64;
    let mut voted_power = 0u64;
    for (_pk, power) in fp_power_table {
        total_power += power;            // wrapping in release
        if voters.contains(_pk) { voted_power += power; }
    }
    voted_power * 3 > total_power * 2    // wrapping in release
}
```

CosmWasm contracts compile in release mode, so `u64` arithmetic wraps silently
on overflow. If `total_power` (sum of all active FPs' `total_active_sats`)
exceeds `2^63`, then `total_power * 2` wraps and the comparison
`voted_power * 3 > total_power * 2` can return `true` with far less than 2/3
of the vote, finalising a block that does not have a real quorum.

## Reachability

- `total_active_sats` per FP is set in `handle_active_delegation` from
  `active_delegation.total_sat` (a `u64`) with **no upper-bound check**
  (`ActiveBtcDelegation::validate` only checks emptiness; the BSN-side
  `verify_active_delegation` is a no-op). The value is trusted from Babylon
  Genesis via IBC.
- Total BTC supply is 2.1 × 10¹⁵ sats ≈ 2⁵⁰·⁹, so under honest Babylon the
  sum across all FPs is well under 2⁶³ and the multiplication is safe.
- To overflow, `total_power` would need to exceed 2⁶³ ≈ 9.2 × 10¹⁸ sats ≈
  92 billion BTC — only achievable if a malicious/compromised Babylon injects
  `total_sat = u64::MAX` delegations.

## Contract + Function + Line

```
contracts/btc-finality/src/tallying.rs
  L144  let mut total_power = 0;
  L147      total_power += power;
  L152  voted_power * 3 > total_power * 2
```

## Impact

Under the documented trust model (Babylon Genesis is honest) this is not
triggerable. If the IBC trust assumption is violated (or a Babylon bug sends
absurd `total_sat` values), a block could be falsely finalised with < 2/3
votes, breaking finality safety.

## Severity Justification (3 perspectives)

**Loss vs. likelihood:** Likelihood is negligible under the trust model;
impact would be a safety break but only via a compromised trusted source.
→ **LOW**.

**Sherlock rubric:** Defense-in-depth weakness; not independently exploitable.
→ **LOW**.

**Code-level:** Trivial to harden — use `Uint128` or `checked_add`/`checked_mul`.

## Suggested Fix

```rust
let mut total_power = Uint128::zero();
let mut voted_power = Uint128::zero();
for (_pk, power) in fp_power_table {
    total_power = total_power.checked_add(Uint128::from(power))?;
    if voters.contains(_pk) {
        voted_power = voted_power.checked_add(Uint128::from(power))?;
    }
}
voted_power * Uint128::from(3u128) > total_power * Uint128::from(2u128)
```
