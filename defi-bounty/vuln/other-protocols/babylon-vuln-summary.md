# Babylon Chain Audit — Summary

**Target:** `cosmos-bsn-contracts` (babylonlabs-io/cosmos-bsn-contracts, `main`)
**Bounty:** Sherlock Babylon Chain Bug Bounty ($500 000)
**Date:** 2025-09-24
**Auditor:** Opus sub-agent
**Codebase:** CosmWasm (Rust) — NOT Solidity; PoCs are `cargo test`, not Foundry.

## Scope reviewed

| Contract / package | Key files |
|---|---|
| `babylon` (IBC orchestrator) | `contract.rs`, `ibc.rs`, `state/mod.rs`, `state/babylon_epoch_chain.rs`, `state/consumer_header_chain.rs` |
| `btc-staking` | `staking.rs`, `validation.rs`, `queries.rs`, `state/staking.rs`, `state/delegations.rs` |
| `btc-finality` | `finality.rs`, `tallying.rs`, `power_dist_change.rs`, `liveness.rs`, `msg.rs`, `state/finality.rs`, `state/public_randomness.rs` |
| `btc-light-client` | `contract.rs`, `bitcoin.rs`, `state.rs` |
| `eots` | `eots.rs` |
| `merkle` | `proof.rs` |
| `apis` | `validate.rs`, `finality_api.rs`, `signing_context.rs` |

## Findings

| # | Severity | Area | File | Issue |
|---|----------|------|------|-------|
| 1 | **CRITICAL** | Reward distribution / integer precision | `btc-finality/src/finality.rs:477` | `current_balance.checked_mul(accumulated_weight)` overflows `Uint128` at realistic FP sizes + reward pools → reverts `EndBlock` sudo → block never indexed → **permanent finality DoS** (every later `tally_blocks` fails on the missing `BLOCKS` entry). Attacker can force it by donating tokens to the contract. |
| 2 | **MEDIUM** | Reward distribution / precision | `btc-finality/src/finality.rs:490` | `ACCUMULATED_VOTING_WEIGHTS.clear()` runs before the `fp_rewards.is_empty()` early-return; when floor-division zeroes every reward, FPs lose accrued weight with no payout and the dust balance is captured by future intervals. |
| 3 | **MEDIUM** | BTC staking / voting power | `btc-staking/src/staking.rs:319,235` | `process_expired_btc_delegations` discounts power but doesn't mark the delegation, so a later `handle_undelegation` for the same delegation discounts again (double-discount), corrupting FP `total_active_sats`. |
| 4 | **LOW** | Finality tally / integer precision | `btc-finality/src/tallying.rs:144-152` | `tally` uses wrapping `u64` add/mul for the 2/3 quorum check; only exploitable if trusted Babylon injects absurd `total_sat` (unreachable under honest trust model). |

## Not exploitable / by-design (noted, not reported as bugs)

- **BTC light client does not verify PoW** (`bitcoin.rs::verify_headers` only
  checks prev-hash chaining + height). This is by design — the BSN trusts
  Babylon Genesis to validate BTC headers over IBC. Same trust model applies
  to `verify_epoch_and_checkpoint` and `verify_consumer_header` (all no-ops).
- **`BtcStaking` / `Slash` execute handlers** are gated to
  `config.babylon || admin`; undelegation has no sig check because Babylon is
  trusted to relay only valid packets.
- **EOTS verify / extract** (`eots.rs`) matches BIP-340 Schnorr semantics;
  challenge binds `(R, P, H(m))`; malleability guarded by `R.y even` +
  `R != identity` + `R.x == r`. Correct.
- **Merkle proof** (`merkle/proof.rs`) follows CometBFT IAVL proof logic with
  `index < total` guard; `MAX_AUNTS = 100` caps DoS. Correct.
- **Signing context** binds `chain_id` + `contract_address`; no cross-chain
  replay.
- **Slashing path** in `handle_finality_signature` correctly defers the slash
  until both canonical + fork sigs exist (EOTS extractability requirement).
- **Access control** on `Slashing` (babylon contract → sender == finality),
  `RewardsDistribution` (sender == finality), `UpdateStaking` /
  `UpdateContractAddresses` (babylon || admin), `UpdateConfig` (admin) — all
  checked.
- **Duplicate-FP-in-delegation** is caught by `check_duplicated_fps` +
  `create_distribution`'s "already exists" error.
- **`PubRandCommit::end_height()` underflow** when `num_pub_rand == 0` is
  unreachable because `min_pub_rand` defaults to 1 and only admin can lower
  it; even at 0 the commit is simply unusable (`in_range` always false), not
  a security issue.

## Output files

- `/home/z/babylon-vuln-reward-distribution-overflow.md` (CRITICAL)
- `/home/z/babylon-vuln-reward-weights-cleared.md` (MEDIUM)
- `/home/z/babylon-vuln-double-discount.md` (MEDIUM)
- `/home/z/babylon-vuln-tally-overflow.md` (LOW)
- `/home/z/babylon-vuln-summary.md` (this file)

## Top recommendation

Fix #1 first: switch `handle_rewards_distribution` to `Uint256` for the
`balance × weight` product, move the `ACCUMULATED_VOTING_WEIGHTS.clear()` to
after a successful distribution, and make `handle_end_block` tolerant of
reward-distribution errors so a downstream arithmetic failure cannot revert
`index_block` / `tally_blocks` / `handle_liveness` and brick the chain.
