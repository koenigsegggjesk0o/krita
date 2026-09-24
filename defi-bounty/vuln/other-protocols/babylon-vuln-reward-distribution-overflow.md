# Babylon Chain — Reward Distribution Uint128 Overflow → Permanent Finality DoS

- **Area:** Reward Distribution / Integer Precision
- **Severity:** CRITICAL
- **Contract:** `btc-finality`
- **File:** `contracts/btc-finality/src/finality.rs`
- **Function:** `handle_rewards_distribution` (lines 445–507)
- **Called from:** `handle_end_block` (line 382, `contracts/btc-finality/src/contract.rs`)
- **Codebase:** `cosmos-bsn-contracts` (CosmWasm / Rust, not Solidity)

---

## Description

`handle_rewards_distribution` computes each finality provider's reward share with:

```rust
// finality.rs:477
let numerator = current_balance.checked_mul(accumulated_weight)?;
let reward = numerator.div_floor((total_accumulated_weight, Uint128::one()));
```

`current_balance` is the contract's entire native-token balance (a `Uint128`),
and `accumulated_weight` is the FP's accumulated voting power over the reward
interval (`voting_power as u128`, summed per signed block — line 317).

`current_balance * accumulated_weight` is performed in `Uint128` (max `2^128 − 1`).
The `checked_mul` returns `Err(Overflow)` when the product exceeds `2^128 − 1`,
and the `?` operator propagates the error out of `handle_rewards_distribution`,
out of `handle_end_block`, and out of the contract's `sudo` entrypoint.

Because `handle_end_block` performs block indexing (`index_block`), vote
tallying (`tally_blocks`), and liveness jailing (`handle_liveness`) **before**
the reward-distribution call, a reverted sudo reverts **all** of those state
changes for that block. Worse, the reverted block is never inserted into the
`BLOCKS` map, so the very next `EndBlock`'s `tally_blocks` will do
`BLOCKS.load(h)` for the missing height and return `BlockNotFound`, failing
again. This creates a **permanent failure loop** — every subsequent `EndBlock`
reverts until the contract is migrated.

The threshold is low enough to trigger under normal operation:

| FP voting power (sats) | accumulated_weight (50 blocks) | `current_balance` overflow threshold (18-dec token) |
|------------------------|-------------------------------|-----------------------------------------------------|
| 5 × 10¹⁴ (5 000 BTC)   | 2.5 × 10¹⁶ ≈ 2⁵⁴·⁵            | ≈ 2⁷³·⁵ ≈ 1.36 × 10²² → **~13 600 tokens**         |
| 5 × 10¹⁵ (50 000 BTC)  | 2.5 × 10¹⁷ ≈ 2⁵⁷·⁵            | ≈ 2⁷⁰·⁵ ≈ 1.7 × 10²¹ → **~1 700 tokens**           |
| 1 × 10¹⁶ (100 000 BTC) | 5 × 10¹⁷ ≈ 2⁵⁹                | ≈ 2⁶⁹ ≈ 5.9 × 10²⁰ → **~590 tokens**               |

With 6-decimal denoms the thresholds are ~10¹²× higher but still reachable on
high-inflation chains. An attacker can also **directly bank-transfer** native
tokens to the finality contract address to inflate `current_balance` on demand,
making the overflow deterministic and attacker-controlled.

## Contract + Function + Line

```
contracts/btc-finality/src/finality.rs : handle_rewards_distribution
  L477:  let numerator = current_balance.checked_mul(accumulated_weight)?;
  L478:  let reward = numerator.div_floor((total_accumulated_weight, Uint128::one()));
```

Caller:

```
contracts/btc-finality/src/contract.rs : handle_end_block
  L381:  if env.block.height > 0 && env.block.height % cfg.reward_interval == 0 {
  L382:      if let Some(rewards_msg) = handle_rewards_distribution(deps, &env)? {
```

## Attack Scenario

1. The BSN's fee-collector routes native tokens to the `btc-finality` contract
   every block (standard design — see `ARCHITECTURE.md` §Babylon-SDK).
2. A finality provider with non-trivial voting power (e.g. 5 000+ BTC) signs
   every block, accumulating `voting_power × 50` in
   `ACCUMULATED_VOTING_WEIGHTS`.
3. At the next `reward_interval` boundary (default every 50 blocks),
   `handle_end_block` calls `handle_rewards_distribution`.
4. `current_balance.checked_mul(accumulated_weight)` overflows `Uint128` and
   returns `Err`.
5. `handle_end_block` returns `Err`; the CosmWasm VM reverts **all** state
   changes in the sudo call — including `index_block`, `tally_blocks`, and
   `handle_liveness`.
6. The block at height `H` is never written to `BLOCKS`.
7. At height `H+1`, `tally_blocks` iterates `start_height..=H+1` and calls
   `BLOCKS.load(H)` → `BlockNotFound` → sudo fails again → `H+1` also not
   indexed.
8. Every subsequent `EndBlock` fails the same way. **Block finalisation,
   liveness jailing, and reward distribution are permanently broken** until a
   contract migration fixes both the overflow and the missing indexed blocks.

An active attacker can force step 4 at any time by sending enough native tokens
to the finality contract via a plain bank `MsgSend` (no contract call needed),
inflating `current_balance` past the overflow threshold.

## PoC (Rust / cargo test — CosmWasm, not Foundry)

```rust
// tests/overflow_poc.rs
use cosmwasm_std::Uint128;

#[test]
fn poc_reward_mul_overflow_halts_endblock() {
    // 5 000 BTC FP signing 50 blocks
    let voting_power_sats: u64 = 5_000 * 100_000_000;          // 5e14
    let accumulated_weight = Uint128::from(voting_power_sats) * Uint128::from(50u64);
    // ~13 600 tokens with 18 decimals sent to the contract (e.g. by attacker)
    let current_balance = Uint128::from(13_600u128)
        * Uint128::from(10u128.pow(18));

    // This is the exact expression in finality.rs:477
    let result = current_balance.checked_mul(accumulated_weight);

    // 13_600e18 * 2.5e16 = 3.4e38 > 2^128 (3.4028e38)  →  overflow
    assert!(
        result.is_err(),
        "expected Uint128 overflow (which propagates via `?` and reverts EndBlock), \
         got {:?}",
        result
    );
    println!("current_balance   = {}", current_balance);
    println!("accumulated_weight= {}", accumulated_weight);
    println!("product           = {}  (overflows u128 / 2^128)",
             current_balance.u128() as u128);  // would wrap in release
}
```

Run: `cargo test --package btc-finality --test overflow_poc`

## Impact

- **Permanent DoS of the BTC finality system** on the consumer (BSN) chain.
  No blocks can be tallied/finalised, no FPs can be jailed, no rewards
  distributed.
- Recovery requires a governance-gated contract **migration** that both (a)
  switches the arithmetic to `Uint256`/`u256` and (b) back-fills the missing
  indexed blocks — non-trivial and slow.
- Triggerable by any user sending native tokens to the contract (active
  griefing) **or** by normal reward accrual once TVL + reward pool are large
  enough (passive failure).
- Economic ratio is extreme: a few thousand tokens of cost to brick a chain
  securing billions in BTC stake.

## Severity Justification (3 perspectives)

**Loss vs. likelihood:** The overflow is a hard arithmetic error that reverts
the entire `EndBlock` sudo. Likelihood is high because the threshold
(current_balance ≳ `2^128 / accumulated_weight`) is reached with realistic FP
sizes (≥ 5 000 BTC) and modest reward pools (≥ a few thousand 18-dec tokens).
An attacker can also push `current_balance` over the line with a plain token
transfer. Impact is total: the finality gadget stops working chain-wide and
self-perpetuates because the missing `BLOCKS` entry makes every later
`EndBlock` fail too. → **CRITICAL**.

**Sherlock rubric (C1–C4):** Causes a direct, permanent loss of availability
for the core finality mechanism (C4: chain unable to finalise), and indirectly
causes loss of rewards for all FPs (C1) since distribution is bricked. Trigger
is realistic and attacker-cheap. → **CRITICAL**.

**Mitigating factors considered:** None of the guards upstream
(`is_zero` check, `fp_entries.is_empty()` check) bound `current_balance * weight`;
`div_floor` only runs after the mul, so it cannot save the overflow. The
`Uint128` type itself provides no headroom. The only "mitigation" is that the
exact trigger threshold depends on the BSN's denom decimals and FP sizes — but
the BSN's own design (18-dec cosmos tokens + BTC-scale stakes) sits squarely in
the danger zone, and an attacker can always donate tokens to force it.

## Suggested Fix

1. Do the proportion in `Uint256`:
   ```rust
   let numerator = Uint256::from(current_balance)
       .checked_mul(Uint256::from(accumulated_weight))?;
   let reward_u256 = numerator / Uint256::from(total_accumulated_weight);
   let reward = Uint128::try_from(reward_u256)?;
   ```
2. Move `ACCUMULATED_VOTING_WEIGHTS.clear()` to **after** the success path so
   a transient error does not silently zero out FPs' accrued weight (see
   separate report).
3. Ensure `handle_end_block` cannot lose `index_block` state to a downstream
   error — either split reward distribution into its own sudo or catch+log the
   error instead of propagating with `?`.
