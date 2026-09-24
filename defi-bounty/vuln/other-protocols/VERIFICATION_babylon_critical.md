# VERIFICATION — Babylon Chain: Reward Distribution Uint128 Overflow → Finality DoS

- **Vuln file:** `vuln/other-protocols/babylon-vuln-reward-distribution-overflow.md`
- **Codebase:** `/home/z/babylon/` (`cosmos-bsn-contracts`, CosmWasm / Rust)
- **Target:** `contracts/btc-finality` — `handle_rewards_distribution` (`finality.rs:445–507`)
- **Verifier:** Opus (sub-agent)
- **Date:** 2025-09-24

---

## 1. Claim (as stated by the report)

`handle_rewards_distribution` computes a finality provider's reward share as
`current_balance.checked_mul(accumulated_weight)?` (finality.rs:477) in `Uint128`
arithmetic. `current_balance` is the contract's entire native-token balance and
`accumulated_weight` is the FP's accumulated voting power (sats) over the
50-block reward interval. When the product exceeds `2^128 − 1`, `checked_mul`
returns `Err(OverflowError)`, which propagates via `?` (→ `ContractError::Overflow`,
error.rs:116) out of `handle_rewards_distribution`, out of `handle_end_block`
(contract.rs:382), and out of the `sudo` entrypoint (contract.rs:264). The
CosmWasm VM then reverts **all** state changes in the EndBlock — including
`index_block`'s save of `BLOCKS[H]`. The next `EndBlock`'s `tally_blocks` then
fails on the missing `BLOCKS[H]`, creating a **permanent failure loop** until
contract migration.

Reported severity: **CRITICAL**.

---

## 2. Code Verification (line-by-line)

All file/line references below were checked against the source at
`/home/z/babylon/`.

| Report claim | Source evidence | Status |
|---|---|---|
| `handle_rewards_distribution` at `finality.rs:445–507` | `pub fn handle_rewards_distribution(deps, env) -> Result<Option<WasmMsg>, ContractError>` at L445; body L446–507 | ✅ |
| L477: `let numerator = current_balance.checked_mul(accumulated_weight)?;` | Exact match, L477 | ✅ |
| L478: `let reward = numerator.div_floor((total_accumulated_weight, Uint128::one()));` | Exact match, L478 | ✅ |
| `current_balance` is `Uint128` from bank query | `deps.querier.query_balance(...)?.amount` at L452–455; `coin.amount` is `Uint128` | ✅ |
| `accumulated_weight` is `Uint128` (sum of `voting_power as u128` per signed block, L317) | `ACCUMULATED_VOTING_WEIGHTS.update(..., existing.unwrap_or(0) + (voting_power as u128))` at L317–319; storage type `Map<&str, u128>` (state/finality.rs:44); `collect_accumulated_voting_weights` converts to `Uint128` (state/finality.rs:131–147) | ✅ |
| `voting_power` is FP's BTC stake in sats (u64) | `get_fp_power` returns `u64` (state/finality.rs:84–91); `FP_POWER_TABLE: Map<(u64,&str), u64>` "total active sats" (state/finality.rs:17–18) | ✅ |
| `ContractError` converts `OverflowError` via `?` | `#[error(transparent)] Overflow(#[from] OverflowError)` at error.rs:115–116 | ✅ |
| Called from `handle_end_block` (contract.rs:382) with `?` | `if let Some(rewards_msg) = handle_rewards_distribution(deps, &env)? {` at L382 | ✅ |
| `handle_end_block` order: `index_block` → `tally_blocks` → `handle_liveness` → `handle_rewards_distribution` | L364 (`index_block`), L368 (`tally_blocks`), L377 (`handle_liveness`), L382 (`handle_rewards_distribution`) — confirmed sequential with `?` | ✅ |
| `handle_end_block` is the `EndBlock` sudo entrypoint | `sudo` (contract.rs:258) → `SudoMsg::EndBlock {..} => handle_end_block(...)` at L261–264 | ✅ |
| `tally_blocks` loads `BLOCKS[h]` and fails if missing | `let mut indexed_block = BLOCKS.load(deps.storage, h)?;` at tallying.rs:70; loop at L69 | ✅ |
| `DEFAULT_REWARD_INTERVAL = 50` | `pub const DEFAULT_REWARD_INTERVAL: u64 = 50;` (state/config.rs:11) | ✅ |

**Code-level conclusion:** the overflow path, the `?` propagation chain, the
ordering of operations in `handle_end_block`, and the cascade mechanism are all
exactly as described in the report.

### Discrepancy 1 — cascade error variant (minor)

The report says the cascade produces `ContractError::BlockNotFound`. In fact,
`BlockNotFound` (error.rs:95–96) is only raised by `handle_finality_signature`
(finality.rs:239–241). `tally_blocks` calls `BLOCKS.load(...)?` directly, which
on a missing key yields `StdError::NotFound` → `ContractError::Std(...)`. The
substance of the claim (next EndBlock fails due to the missing indexed block) is
correct; only the error variant name is imprecise.

---

## 3. PoC

Two PoC artefacts were added to the Babylon tree:

### 3.1 In-crate test (`contracts/btc-finality/src/contract.rs`, tests module)

`poc_handle_rewards_distribution_overflow` — calls the **actual**
`handle_rewards_distribution` with mock state matching the bug scenario.
`poc_cascade_missing_block_breaks_next_tally` — calls the actual
`tally_blocks` with a missing `BLOCKS[H]` to demonstrate the cascade.

### 3.2 Standalone test (`contracts/btc-finality/tests/overflow_poc.rs`)

Pure-`Uint128` math PoC that reproduces the exact `checked_mul` expression at
finality.rs:477, plus a test documenting that the report's original numbers do
**not** overflow (see §4), plus a corrected threshold table.

### 3.3 How to run

```bash
cd /home/z/babylon
cargo test --package btc-finality --lib --features library poc_ -- --nocapture
cargo test --package btc-finality --test overflow_poc --features library -- --nocapture
```

### 3.4 Results (captured, all passing)

```
running 2 tests   [in-crate, --lib]
TALLYING: Start - NEXT_HEIGHT: 50, start_height: 50, max_blocks: 200
poc: current_balance=1000000000000000000000000, accumulated_weight=500000000000000,
     product=3.402823669209385e38 > 2^128=3.402823669209385e38
poc: tally_blocks returned Err = Std(NotFound { kind: "type: babylon_apis::finality_api::IndexedBlock; key: [..32]" })
test contract::tests::poc_cascade_missing_block_breaks_next_tally ... ok
poc: handle_rewards_distribution returned Err = Overflow(OverflowError { operation: Mul })
test contract::tests::poc_handle_rewards_distribution_overflow ... ok
test result: ok. 2 passed; 0 failed

running 3 tests   [standalone, tests/overflow_poc.rs]
=== Report's original numbers (corrected sats) ===
current_balance    = 13600000000000000000000
accumulated_weight = 25000000000000
product            = 340000000000000000000000000000000000
u128::MAX          = 340282366920938463463374607431768211455
overflows?         = false  (report claimed true)
test poc_report_original_numbers_do_not_overflow ... ok
=== CORRECTED PoC (actually overflows) ===
current_balance     = 1000000000000000000000000  (~1e24)
accumulated_weight  = 500000000000000  (~5e14)
product (saturated) = 340282366920938463463374607431768211455  (= u128::MAX)
test poc_reward_mul_overflow_corrected ... ok
FP BTC stake | min tokens (18-dec) to overflow
      5000  |  13611295
     50000  |  1361130
    100000  |  680565
    500000  |  136113
   1000000  |  68057
test poc_corrected_overflow_threshold_table ... ok
test result: ok. 3 passed; 0 failed
```

**Key facts proven by the passing PoCs:**

1. `handle_rewards_distribution` returns
   `Err(ContractError::Overflow(OverflowError { operation: Mul }))` for realistic
   inputs (100 000 BTC FP × 50-block interval × 1 000 000 18-dec tokens). This
   is the exact error variant that propagates via `?` and reverts the EndBlock.
2. `tally_blocks` returns `Err(ContractError::Std(NotFound))` when `BLOCKS[H]`
   is missing — the cascade that makes the DoS permanent.

---

## 4. Material Discrepancy — the report's numbers are ~1000× too low

The report's threshold table claims:

| FP voting power (sats) | accumulated_weight (50 blocks) | overflow threshold (18-dec tokens) |
|---|---|---|
| 5 × 10¹⁴ (5 000 BTC) | 2.5 × 10¹⁶ | ~13 600 tokens |

**This is wrong.** 1 BTC = 10⁸ sats, so:

- 5 000 BTC = 5 × 10¹¹ sats  (not 5 × 10¹⁴)
- accumulated_weight = 5 × 10¹¹ × 50 = 2.5 × 10¹³  (not 2.5 × 10¹⁶)
- threshold current_balance = 2¹²⁸ / 2.5×10¹³ ≈ 1.36 × 10²⁵
- in 18-dec tokens: 1.36 × 10²⁵ / 10¹⁸ = **13 611 295 tokens** (≈ 13.6 million),
  not 13 600.

The report confused BTC and sats by a factor of 10³ (it wrote 5 × 10¹⁴ for
5 000 BTC, but the correct figure is 5 × 10¹¹). Every row of the report's table
inherits this 1000× error.

**Consequence:** the report's exact PoC numbers
(`13_600 × 10¹⁸ × 2.5×10¹³ = 3.4 × 10³⁸`) fall **just under** 2¹²⁸ ≈
3.4028 × 10³⁸ and **do not overflow** — proven by the passing
`poc_report_original_numbers_do_not_overflow` test. Anyone copy-pasting the
report's PoC will see the test fail (the assert `result.is_err()` does not hold).

**Corrected thresholds** (1 BTC = 10⁸ sats, reward_interval = 50, 18-dec token):

| FP BTC stake | min tokens (18-dec) to overflow |
|---|---|
| 5 000 BTC | 13 611 295 (~13.6 M) |
| 50 000 BTC | 1 361 130 (~1.36 M) |
| 100 000 BTC | 680 565 (~681 k) |
| 500 000 BTC | 136 113 |
| 1 000 000 BTC | 68 057 |

The vulnerability is still real and reachable, but the trigger threshold is
~1000× higher than the report claims.

---

## 5. 3-Perspective Re-verification

### Perspective A — Adversary / triggerability

- **Passive trigger (normal accrual):** requires a single FP with ≥ 100 000 BTC
  stake *and* a reward pool of ≥ ~680 000 18-dec tokens, or proportionally more
  tokens for smaller FPs. At Babylon's target TVL (multi-hundred-thousand BTC)
  on a high-inflation BSN, this is plausible but not "normal operation" as the
  report implies — the report's "~13 600 tokens" figure is 1000× too low.
- **Active trigger (donation griefing):** an attacker can `MsgSend` native
  tokens to the finality contract address (no contract call needed), inflating
  `current_balance`. The cost is ~68 k–13.6 M 18-dec tokens depending on FP TVL.
  On a low-value or testnet BSN token this is trivial; on a valuable token it is
  expensive but still far cheaper than the economic damage (bricking the chain's
  finality).
- **Verdict:** triggerable, but materially harder than the report states. The
  donation vector remains the most practical attack.

### Perspective B — Impact / blast radius

- The overflow reverts the **entire** `EndBlock` sudo, including `index_block`,
  `tally_blocks`, and `handle_liveness` — so block finalisation, liveness
  jailing, and reward distribution all stop.
- The cascade is real and verified: the missing `BLOCKS[H]` makes every
  subsequent `EndBlock` fail at `tally_blocks` (`StdError::NotFound`), so the
  system is **permanently** bricked until a governance-gated contract migration
  that both (a) switches the arithmetic to `Uint256` and (b) back-fills the
  missing indexed blocks.
- No direct fund loss, but complete loss of availability for the core finality
  gadget — and indirect loss of all FP rewards while distribution is down.
- **Verdict:** impact is total and permanent, matching the report.

### Perspective C — Code correctness / fix

- The fix is straightforward and the report's suggested `Uint256` upgrade is
  correct.
- An additional hardening (also suggested by the report): move
  `ACCUMULATED_VOTING_WEIGHTS.clear()` (finality.rs:490) to **after** the
  success path, and/or split reward distribution into its own sudo / catch+log
  so a transient error cannot zero out FPs' accrued weight or revert
  `index_block` state.
- The existing upstream guards (`current_balance.is_zero()`,
  `fp_entries.is_empty()`, `total_accumulated_weight.is_zero()`) do not bound
  the product; `div_floor` runs after the mul and cannot save the overflow.
- **Verdict:** the bug is a genuine arithmetic-precision defect with a clean
  fix.

---

## 6. Verdict

**CONFIRMED** — with one material numerical discrepancy documented in §4.

The overflow path in `handle_rewards_distribution` (finality.rs:477), its
propagation via `?` through `handle_end_block` and `sudo`, the rollback of
`index_block`'s state, and the permanent cascade via `tally_blocks`'
`BLOCKS.load` failure are all real and verified by passing in-crate and
standalone PoC tests.

The report's specific threshold numbers are **~1000× too low** (it mis-states
5 000 BTC as 5 × 10¹⁴ sats; the correct figure is 5 × 10¹¹ sats), and the
report's exact PoC does not actually overflow. The corrected thresholds are
~13.6 M tokens (5 000 BTC) down to ~68 k tokens (1 000 000 BTC) at 18 decimals.

### Severity

**HIGH (borderline CRITICAL).** Rationale:

- The DoS is permanent and requires migration to recover — a CRITICAL-class
  impact.
- The trigger threshold is ~1000× higher than the report claims, making passive
  triggering harder, but the active donation vector still works and the
  corrected thresholds are reachable on high-TVL / high-inflation BSNs.
- Net: I would score this **HIGH** rather than CRITICAL because the report
  materially overstated triggerability; but a programme that weights "permanent
  chain-finality DoS" heavily could reasonably retain CRITICAL.

---

## 7. Submission Recommendation

**Submit, with the corrected numbers.** The vulnerability is real and the fix
is non-trivial (governance migration required to recover). The submission
**must**:

1. Correct the sat-to-BTC conversion (1 BTC = 10⁸ sats) and present the
   corrected threshold table from §4.
2. Replace the report's PoC with the corrected PoC (100 000 BTC FP × 50 blocks
   × 1 000 000 18-dec tokens, or similar) — the report's original PoC fails.
3. Note the cascade error is `ContractError::Std(NotFound)`, not
   `BlockNotFound`.
4. Recommend the `Uint256` fix plus splitting reward distribution out of
   `handle_end_block` (or catching/logging the error) so an arithmetic fault
   cannot brick block indexing.

See §8 for the submission draft.

---

## 8. Submission Draft

> **Title:** `btc-finality`: Uint128 overflow in `handle_rewards_distribution`
> permanently freezes block finality
>
> **Severity:** High (borderline Critical — permanent chain-wide finality DoS;
> trigger threshold higher than initially estimated)
>
> **Summary**
>
> `handle_rewards_distribution` (`contracts/btc-finality/src/finality.rs:477`)
> computes each finality provider's reward share as
> `current_balance.checked_mul(accumulated_weight)?` in `Uint128` arithmetic.
> `current_balance` is the contract's full native-token balance and
> `accumulated_weight` is the FP's accumulated voting power (sats × signed
> blocks) over the 50-block reward interval. When the product exceeds 2¹²⁸ − 1,
> `checked_mul` returns `Err(OverflowError)`, which propagates via `?`
> (`ContractError::Overflow`, `error.rs:116`) out of `handle_rewards_distribution`,
> out of `handle_end_block` (`contract.rs:382`), and out of the `sudo`
> `EndBlock` entrypoint (`contract.rs:264`). The CosmWasm VM reverts all state
> in the EndBlock — including `index_block`'s save of `BLOCKS[H]`. The next
> EndBlock's `tally_blocks` (`tallying.rs:70`) then fails on
> `BLOCKS.load(H)` (`StdError::NotFound`), and every subsequent EndBlock fails
> the same way. Block finalisation, liveness jailing, and reward distribution
> are permanently broken until a governance-gated contract migration.
>
> **Trigger threshold (corrected, 1 BTC = 10⁸ sats, reward_interval = 50,
> 18-dec token):**
>
> | FP BTC stake | min contract balance to overflow |
> |---|---|
> | 5 000 BTC | ~13.6 M tokens |
> | 50 000 BTC | ~1.36 M tokens |
> | 100 000 BTC | ~681 k tokens |
> | 500 000 BTC | ~136 k tokens |
> | 1 000 000 BTC | ~68 k tokens |
>
> Reachable passively on high-TVL / high-inflation BSNs, or actively by sending
> native tokens to the finality contract via a plain `MsgSend`.
>
> **PoC:** `cargo test --package btc-finality --lib --features library poc_`
> — `poc_handle_rewards_distribution_overflow` returns
> `Err(ContractError::Overflow(OverflowError { operation: Mul }))` and
> `poc_cascade_missing_block_breaks_next_tally` returns
> `Err(ContractError::Std(NotFound))`. (See
> `contracts/btc-finality/tests/overflow_poc.rs` for the standalone math PoC.)
>
> **Fix:**
> 1. Do the proportion in `Uint256`:
>    ```rust
>    let numerator = Uint256::from(current_balance)
>        .checked_mul(Uint256::from(accumulated_weight))?;
>    let reward = Uint128::try_from(numerator / Uint256::from(total_accumulated_weight))?;
>    ```
> 2. Move `ACCUMULATED_VOTING_WEIGHTS.clear()` (finality.rs:490) to after the
>    success path so a transient error does not zero out accrued weight.
> 3. Split reward distribution into its own sudo entrypoint, or catch+log the
>    error in `handle_end_block` instead of propagating with `?`, so an
>    arithmetic fault cannot revert `index_block` / `tally_blocks` /
>    `handle_liveness` state.

---

## 9. Files changed in the Babylon tree (deliverable artefacts)

- `contracts/btc-finality/src/contract.rs` — added
  `poc_handle_rewards_distribution_overflow` and
  `poc_cascade_missing_block_breaks_next_tally` to the `tests` module.
- `contracts/btc-finality/tests/overflow_poc.rs` — new standalone test file
  with three tests (corrected-overflow PoC, report's-numbers-do-not-overflow
  documentation, and corrected threshold table).

No production code was modified.
