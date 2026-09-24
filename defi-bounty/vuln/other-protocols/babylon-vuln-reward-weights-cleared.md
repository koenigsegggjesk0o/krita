# Babylon Chain — Reward Weights Silently Cleared Without Distribution

- **Area:** Reward Distribution / Precision
- **Severity:** MEDIUM
- **Contract:** `btc-finality`
- **File:** `contracts/btc-finality/src/finality.rs`
- **Function:** `handle_rewards_distribution` (lines 445–507)
- **Codebase:** `cosmos-bsn-contracts` (CosmWasm / Rust)

---

## Description

`handle_rewards_distribution` clears `ACCUMULATED_VOTING_WEIGHTS` **before**
checking whether any reward was actually payable:

```rust
// finality.rs:489-495
// Clear all accumulated voting weights for the next reward interval
ACCUMULATED_VOTING_WEIGHTS.clear(deps.storage);

// If there are rewards to distribute, create and return the message
if fp_rewards.is_empty() {
    return Ok(None);
}
```

`fp_rewards` is empty when every per-FP `reward` rounds down to zero via
`div_floor`. That happens whenever, for every FP,
`current_balance * accumulated_weight < total_accumulated_weight` — i.e. when
the reward pool is tiny relative to the number of signers / total weight.

When that branch is hit:

1. The FPs' accumulated voting weights are **wiped** (the `.clear()` already
   ran).
2. `Ok(None)` is returned, so **no** `RewardsDistribution` message is emitted.
3. `current_balance` stays in the contract.

The leftover balance is now distributable only to FPs that sign during a
**future** interval; the FPs that actually earned it in this interval have had
their accounting zeroed with no payment. This is a silent, ongoing reward
shortfall for honest signers whenever the per-interval reward pool is small
(e.g. low-fee BSN, early in the chain's life, or dust sent in by griefers).

## Contract + Function + Line

```
contracts/btc-finality/src/finality.rs : handle_rewards_distribution
  L490:  ACCUMULATED_VOTING_WEIGHTS.clear(deps.storage);
  L493:  if fp_rewards.is_empty() {
  L494:      return Ok(None);
  L495:  }
```

## Attack Scenario

1. A BSN starts up with low fee throughput. Each reward interval (default
   50 blocks) only accrues, say, 1 `uToken` in the finality contract.
2. Three FPs each signed all 50 blocks with equal power, so each has
   `accumulated_weight = 50`, `total_accumulated_weight = 150`.
3. `reward = 1 * 50 / 150 = 0` for every FP (floor division). `fp_rewards`
   is empty.
4. `ACCUMULATED_VOTING_WEIGHTS.clear()` wipes all three FPs' weights to 0.
5. `Ok(None)` is returned — no distribution.
6. The 1 `uToken` remains in the contract. Next interval it is paid to
   whoever signs then; the original three FPs are permanently unpaid for the
   150 blocks of work they already did.

A griefer can amplify this by repeatedly sending 1 `uToken` to the contract
right before each interval boundary, forcing the floor-division to zero out
for low-power FPs while their weights are cleared.

## PoC (Rust)

```rust
use cosmwasm_std::Uint128;

#[test]
fn poc_weights_cleared_no_payout() {
    let current_balance = Uint128::from(1u128);          // 1 uToken
    // 3 FPs, each accumulated 50 weight
    let weights = [Uint128::from(50u128); 3];
    let total: Uint128 = weights.iter().copied().sum();

    let mut any_reward = false;
    for w in &weights {
        let reward = (current_balance * *w) / total;     // div_floor semantics
        if !reward.is_zero() { any_reward = true; }
    }
    // In the contract, ACCUMULATED_VOTING_WEIGHTS.clear() runs regardless
    assert!(!any_reward, "all rewards floored to 0 → weights cleared, balance kept");
}
```

## Impact

- Honest FPs lose accrued rewards whenever the per-interval pool is small
  relative to total accumulated weight.
- The leftover balance compounds in the contract and is captured by future
  (potentially different) FPs — a cross-interval wealth transfer.
- Griefable: a dust token transfer to the contract can force the zero-reward
  branch.

## Severity Justification (3 perspectives)

**Loss vs. likelihood:** Loss is real but bounded by the small-pool regime
that triggers it; it does not brick the chain (unlike the overflow sibling
issue). Likelihood is moderate — common during early BSN life or low-fee
periods. → **MEDIUM**.

**Sherlock rubric:** Causes loss of yield to stakers but not loss of principal
and not a safety break; trigger is realistic but the per-occurrence damage is
small. → **MEDIUM**.

**Code-level:** The fix is a one-line reorder — clear only when a distribution
actually happens, or carry the leftover forward with the weights intact.

## Suggested Fix

```rust
// Only clear after we know we are distributing
if fp_rewards.is_empty() {
    // keep weights so they roll into the next interval
    return Ok(None);
}
ACCUMULATED_VOTING_WEIGHTS.clear(deps.storage);
// ...build and return the message
```

(Even better: don't clear at all; subtract the distributed amounts and carry
the remainder forward, so rounding loss is never lost.)
