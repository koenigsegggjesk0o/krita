# Cosmos SDK — `SlashRedelegation` Over-Slash on Partial Undelegation From Destination

**Audit target:** Cosmos Hub / `cosmos/cosmos-sdk` (module `x/staking`)
**Area:** Staking — validator slashing, redelegations, undelegation
**Severity:** Medium–High (fund loss; bounded ~5% extra on Cosmos Hub params, up to 100% on high-slash-factor SDK chains)
**Status:** Not submitted. For internal review only.

---

## 1. Description

`Keeper.SlashRedelegation` (in `x/staking/keeper/slash.go`) is invoked when a
validator that was the *source* of a redelegation is slashed for an infraction
that happened while the redelegated stake was still bonded to it. The function
must slash `slashFactor × redelegation.InitialBalance` worth of stake, drawn
from wherever that stake currently lives:

* the portion still delegated to the **destination** validator, and
* the portion the delegator has since **undelegated** from the destination
  (now sitting in an unbonding delegation on the destination validator).

A block (lines 336–367) titled *"Handle undelegation after redelegation /
Prioritize slashing unbondingDelegation than delegation"* was added to fix a
prior slashing-bypass (see CHANGELOG: *"Fix a possible bypass of delegator
slashing: GHSA-86h5-xcpx-cfqc"*). It drains the destination-side unbonding
delegation **up to `slashAmount`** and decrements `slashAmount` accordingly.

The bug is in the follow-up step (line 372):

```go
// slash.go:327-330  — total slash owed for this redelegation entry
slashAmountDec := slashFactor.MulInt(entry.InitialBalance)
slashAmount     := slashAmountDec.TruncateInt()

// slash.go:340-367  — drain destination unbonding delegation, decrement slashAmount
for i, entry := range unbondingDelegation.Entries {
    unbondingSlashAmount := math.MinInt(slashAmount, entry.Balance)
    ...
    slashAmount = slashAmount.Sub(unbondingSlashAmount)   // line 357
    ...
}

// slash.go:372  — slash the moved delegation
sharesToUnbond := slashFactor.Mul(entry.SharesDst)        // <-- BUG
if sharesToUnbond.IsZero() || slashAmount.IsZero() {
    continue
}
...
tokensToBurn, err := k.Unbond(ctx, delegatorAddress, valDstAddr, sharesToUnbond)  // line 387
```

`sharesToUnbond` is computed as `slashFactor × entry.SharesDst` — i.e. the
**full** slash fraction applied to the **full original** redelegated shares —
and is **not** reduced by the tokens already burned from the unbonding
delegation in the loop above. The only guard is `slashAmount.IsZero()`, which
fires only when the unbonding delegation absorbed the *entire* slash. In the
partial case (unbonding balance `U` strictly less than `slashAmount`), the
delegation is unbonded by the full `slashFactor × SharesDst` regardless of how
much was already taken from the unbonding side, so the two burns overlap and
the delegator is over-slashed.

`entry.SharesDst` is the historical record of the shares created at the
destination at redelegation time (`SetRedelegationEntry`,
`delegation.go:684-700`); it is never decremented when the delegator later
undelegates from the destination, so it always reflects the *original*
redelegated amount — exactly the value that must be reduced here.

---

## 2. Location

* **Repository / module:** `github.com/cosmos/cosmos-sdk`, `x/staking`
* **File:** `x/staking/keeper/slash.go`
* **Function:** `Keeper.SlashRedelegation`
* **Lines:** `372` (root cause), `328-330`, `340-367`, `387-406` (burn path)
* **Caller chain:** `x/slashing/keeper.SlashWithInfractionReason`
  → `x/staking/keeper.SlashWithInfractionReason` → `Keeper.Slash`
  (slash.go:37) → `Keeper.SlashRedelegation` (slash.go:297), invoked for every
  redelegation whose source is the slashed validator (slash.go:127-143).
* **Reachable via:** equivocation evidence (`x/evidence/keeper/infraction.go:125`)
  and liveness slashing (`x/slashing/keeper/infractions.go:132`).

---

## 3. Attack Scenario / Trigger

No special privilege is required — the bug fires whenever the following
sequence occurs, which is a routine delegator pattern:

1. Delegator `D` delegates to validator `S`.
2. `D` redelegates the full amount from `S` to validator `V`
   (redelegation entry: `InitialBalance = R`, `SharesDst = R`).
3. `D` **partially** undelegates from `V`, creating an unbonding delegation on
   `V` with balance `U` where `0 < U < slashFactor × R`.
   (The remaining `R − U` worth of shares stays delegated to `V`.)
4. Validator `S` is slashed for an infraction committed at a height `<=` the
   redelegation's `CreationHeight`, while the redelegation entry is still
   within the unbonding window (not mature).

On Cosmos Hub, `slashFactor` is `0.05` (double-sign) / `0.01` (downtime), so
the trigger window is `U < 5%` (or `1%`) of the redelegated stake. A delegator
who redelegates and then takes a small partial withdrawal satisfies it.

A malicious validator can deliberately weaponize this: run `S` with a minimal
self-bond, attract delegations, wait for delegators to redelegate away and
partially withdraw, then double-sign. Every affected redelegator is
over-slashed; the attacker is tombstoned but only loses `slashFactor` of their
own (small) self-bond. This is a griefing / fund-destruction vector — the
over-slashed tokens are **burned**, not stolen — so it is most relevant as a
competitor-sabotage or pure-disruption attack, plus a latent correctness bug
that harms ordinary delegators on every slash.

---

## 4. Proof of Concept (numeric)

Assume 1:1 token:share at the destination for clarity (the bug is independent
of the exchange rate). Let `R = 100`, `slashFactor f = 0.5`, and let the
delegator undelegate `U = 20` from `V` (so `delegation.Shares = 80`,
`unbonding.Balance = 20`).

| Step | Code action | Value |
|------|-------------|-------|
| slash.go:328-329 | `slashAmount = f × R = 0.5 × 100` | `50` |
| slash.go:342 | `unbondingSlashAmount = min(slashAmount, U) = min(50, 20)` | `20` |
| slash.go:357 | `slashAmount = 50 − 20` | `30` |
| slash.go:359 | `notBondedBurnedAmount += 20` (burned from unbonding) | `20` |
| slash.go:372 | `sharesToUnbond = f × SharesDst = 0.5 × 100` | `50` (shares) |
| slash.go:373 | `slashAmount(30)` not zero → do **not** skip | — |
| slash.go:383-385 | cap to `delegation.Shares = 80` → no cap | `50` |
| slash.go:387 | `Unbond(50 shares)` → `tokensToBurn ≈ 50` | `50` |
| slash.go:401 | `bondedBurnedAmount += 50` (burned from delegation) | `50` |

**Total burned = 20 + 50 = 70.**
**Correct slash = f × R = 50.**
**Over-slash = 20** (the delegator keeps `30` instead of `50`).

General formula (1:1, destination exchange rate ≈ 1):

* Case A — `U >= f·R` (unbonding can absorb the whole slash): total is correct,
  only the *split* between unbonding and delegation is wrong (no fund loss).
* Case B — `U < f·R`:
  * if `f·R <= R − U` (always true when `f + U/R <= 1`): over-slash **= U**;
  * else (high `f`, mid-range `U`): the delegation is fully drained and the
    over-slash caps at **R·(1 − f)** — the delegator can lose **100%** of the
    redelegated stake (e.g. `f = 0.9`, `0.1·R < U < 0.9·R`).

Cosmos Hub (`f = 0.05`): always Case B-subcase-1, over-slash `<= 5%` of `R`
(delegator loses up to `10%` instead of `5%`).
SDK chains with `f >= 0.5` (some consumer chains / custom deployments): up to
`100%` loss of the redelegated stake.

A Go test reproducing this (drop into `x/slashing/keeper/slash_redelegation_test.go`
alongside the existing `TestSlashRedelegation`, which only covers the *full*
undelegation case and therefore never exercises the bug):

```go
// TestSlashRedelegation_PartialUndelegation_OverSlash reproduces the over-slash.
func TestSlashRedelegation_PartialUndelegation_OverSlash(t *testing.T) {
	// Setup identical to TestSlashRedelegation (two validators evilVal/goodVal,
	// testAcc1 funded with power-10 coins).
	//
	// 1. testAcc1 delegates 10 (consensus power) to evilVal at height 3.
	// 2. testAcc1 redelegates ALL 10 from evilVal -> goodVal at height 4.
	// 3. testAcc1 undelegates only 3 (i.e. 30%) from goodVal at height 4,
	//    leaving 70% delegated.  -> U = 3 < slashFactor * R for f = 0.9.
	// 4. Slash evilVal for infraction at height 3 with slashFactor = 0.9.
	//
	// Expected (correct): acc1 loses 9 tokens, keeps 1 (balance_after * 10 == balance_before).
	// Actual (buggy):    acc1 loses 3 (unbonding) + 9 (delegation, full f*SharesDst) = 12,
	//                    i.e. balance goes to 0 (over-slash by 3).
	//
	// assert: balance1AfterSlashing.Amount must equal balance1Before.Amount.Sub(math.NewInt(9))
	//         but actually equals 0  (or near 0 after exchange-rate rounding).
}
```

---

## 5. Impact

* **Direct fund loss to delegators.** Any delegator in the trigger state loses
  more stake than the slash factor prescribes. Tokens are burned (removed from
  supply), not transferred, so there is no direct thief-gain path — but
  delegators are irreversibly underpaid when their unbonding matures and their
  still-delegated stake is also over-reduced.
* **Aggregate / systemic.** Every slash event on a validator with redelegators
  in the partial-undelegation state hits *all* of them simultaneously; the
  aggregate over-burn can be material during a major equivocation event.
* **Griefing by a malicious validator.** A validator with a small self-bond
  can sacrifice itself (double-sign → tombstone) to over-slash competing
  validators' delegators who have redelegated away and partially withdrawn.
* **Parameter-dependent severity.** On Cosmos Hub the over-slash is bounded at
  ~5% of the redelegated stake; on SDK consumer/custom chains with higher
  `SlashFractionDoubleSign` the same code path can destroy 100% of the
  redelegated stake.
* **No state corruption / no chain halt.** Delegation shares are capped at
  `delegation.Shares` (slash.go:383-385) and the validator token burn is
  floored at zero, so the over-slash does not itself break invariants or halt
  the chain — it is a pure economic over-burn.

---

## 6. Severity

**Medium–High.**

Rationale: it is a real, reachable fund-loss bug in core slashing logic with a
realistic trigger and a credible griefing amplification path. It does not
enable direct theft (tokens are burned), and on Cosmos Hub's parameters
(`f = 0.05`) the per-delegator over-slash is bounded at ~5% extra, which keeps
it out of "Critical". The shared-SDK dimension (up to 100% loss on high-`f`
chains) and the systemic/aggregate nature push it above a plain Medium.
Recommended bounty band: Medium–High.

---

## 7. Three-Perspective Audit

### 7.1 Attacker perspective
The attacker cannot steal the over-slashed tokens (they are burned), so this is
not a profit-making exploit. The realistic attacker is a malicious or
sacrificial validator who:
1. registers a validator with the minimum self-bond,
2. attracts delegations (e.g. low commission),
3. waits for delegators to redelegate to a competitor and take a small partial
   withdrawal,
4. double-signs once.
The attacker is tombstoned and loses `slashFactor × selfBond`, while each
affected redelegator loses up to `2 × slashFactor × R` (Cosmos Hub) or up to
`R` (high-`f` chains). The attack is cheap for the attacker relative to the
aggregate harm, which makes it attractive as a sabotage vector against rival
validators' delegator bases.

### 7.2 Protocol / maintainer perspective
This is a correctness regression introduced by the
GHSA-86h5-xcpx-cfqc fix ("bypass of delegator slashing"). That fix correctly
closed the bypass (delegators can no longer escape slashing by undelegating
from the destination), but the accounting between the unbonding-delegation burn
and the delegation burn was left inconsistent: the unbonding side correctly
decrements `slashAmount`, but the delegation side recomputes the full
`slashFactor × SharesDst` instead of consuming the remainder. The existing test
`TestSlashRedelegation` only exercises the *full* undelegation case (`U >=
slashAmount`), which is precisely the one branch where the total is still
correct, so the regression escaped CI. Adding the partial-undelegation case to
the test matrix would have caught it.

### 7.3 Auditor / defender perspective
The fix is localized. Two viable approaches:

* **Preferred (token-accountant):** after the unbonding-delegation loop,
  convert the remaining `slashAmount` into destination shares and unbond
  *that*, e.g.
  ```go
  // remaining tokens to slash, expressed in destination shares
  if slashAmount.IsPositive() {
      valDst, _ := k.GetValidator(ctx, valDstAddr)
      // shares = tokens / (tokens-per-share); cap to delegation.Shares
      sharesToUnbond := math.LegacyNewDecFromInt(slashAmount).
          QuoRoundup(valDst.TokensFromSharesDec(math.LegacyOneDec())) // = tokens/1 share
      ...
  }
  ```
  i.e. unbond exactly the remaining slash in *tokens*, not `f × SharesDst`.
* **Minimal patch:** scale `sharesToUnbond` down by the fraction already
  absorbed by the unbonding side:
  ```go
  // after the unbonding loop, slashAmount holds the remaining tokens to burn
  remainingFraction := math.LegacyNewDecFromInt(slashAmount).
      QuoTruncate(slashAmountDec)            // slashAmountDec = f * InitialBalance
  sharesToUnbond := slashFactor.Mul(entry.SharesDst).Mul(remainingFraction)
  ```
  (with care for the `slashAmountDec.IsZero()` edge case).

Either fix must be paired with a regression test mirroring §4's numeric
scenario (partial undelegation from the destination, `U < slashFactor × R`),
asserting that the delegator's total remaining stake equals
`R × (1 − slashFactor)`.

---

## 8. References

* `x/staking/keeper/slash.go` — `Slash` (L37), `SlashRedelegation` (L297-418),
  `SlashUnbondingDelegation` (L240-289, correct reference implementation).
* `x/staking/keeper/delegation.go` — `SetRedelegationEntry` (L684),
  `BeginRedelegation` (L1225), `Unbond` (L979).
* `x/slashing/keeper/keeper.go` — `SlashWithInfractionReason` (L87).
* `x/slashing/keeper/infractions.go` — downtime slash path (L132).
* `x/evidence/keeper/infraction.go` — double-sign slash path (L125).
* `x/slashing/keeper/slash_redelegation_test.go` — existing (insufficient)
  coverage, only the `U >= slashAmount` branch.
* CHANGELOG.md L528: *"Fix a possible bypass of delegator slashing:
  GHSA-86h5-xcpx-cfqc"* — the fix that introduced this regression.
