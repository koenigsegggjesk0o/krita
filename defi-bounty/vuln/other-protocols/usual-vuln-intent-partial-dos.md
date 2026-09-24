# Usual Labs — Partial Intent Matching Requires Full Remaining Intent Amount Upfront (DoS / Griefing)

**Severity:** MEDIUM
**Area:** DaoCollateral — Intent matching
**Bounty:** Sherlock Usual Labs Bug Bounty (#56)

---

## Description

In `DaoCollateral.swapRWAtoStbcIntent()` with `partialMatching = true`, the function computes
`remainingAmountUnmatched` (the entire remaining intent amount) and passes it to
`_swapRWAtoStbc()` as `amountInTokenDecimals`. `_swapRWAtoStbc()` then does:

1. `safeTransferFrom(intent.recipient, treasury, amountInTokenDecimals)` — pulls the **full**
   remaining intent amount of RWA from the recipient.
2. Mints USD0 for the full amount.
3. Calls `swapperEngine.swapUsd0(..., partialMatching=true)` — matches as much as it can.
4. Burns the unmatched USD0 and **returns** the corresponding RWA to the recipient.

This "take-everything-then-return-the-rest" pattern means that **a partial match requires the
intent recipient to hold (and have approved) the full remaining intent amount**, even though only
a small fraction will actually be consumed. If the recipient's RWA balance or allowance is below
`remainingAmountUnmatched`, the `safeTransferFrom` at step 1 reverts and the entire transaction
fails — the partial match cannot proceed at all.

This is exploitable as a **griefing / DoS vector** by any holder of the `INTENT_MATCHING_ROLE`
and as a **UX failure** for legitimate partial-fill flows.

Additionally, because the unmatched portion is round-tripped RWA → USD0 → RWA through floor
rounding in both directions (`_getQuoteInUsd` floors, `_getQuoteInToken` floors), each partial
match can permanently trap a small amount of RWA dust in the treasury — `rwaTokensToReturn`
can round to **zero** when `wadRwaNotTakenInUSD` is small relative to the token's price/decimals,
causing the user to lose that slice of RWA entirely.

---

## Contract / Function / Lines

**Contract:** `DaoCollateral` (impl `0x0eEc861D49f15F585D6Bb4301FC4f89BCe22AF4e`)

**Functions:**
- `swapRWAtoStbcIntent` — `src/daoCollateral/DaoCollateral.sol` lines 843–889
- `_swapRWAtoStbc` — lines 565–637 (specifically line 602: the `safeTransferFrom` of the full
  `amountInTokenDecimals`)
- `_isValidIntent` — lines 764–798 (computes `remainingAmountUnmatched`)
- `_useIntentAmount` — lines 805–835 (deducts only the matched portion)

```solidity
// swapRWAtoStbcIntent — line 859
(uint256 remainingAmountUnmatched, uint256 nonce) = _isValidIntent(intent);
// ...
(uint256 matchedAmountInTokenDecimals, uint256 matchedAmountInUSD) = _swapRWAtoStbc(
    intent.recipient,
    intent.rwaToken,
    remainingAmountUnmatched,   // ← FULL remaining amount, not a partial slice
    partialMatching,
    orderIdsToTake,
    approval
);
```

```solidity
// _swapRWAtoStbc — line 602
IERC20Metadata(rwaToken).safeTransferFrom(caller, $.treasury, amountInTokenDecimals);
```

---

## Attack Scenario

1. Alice signs an EIP-712 intent to swap up to 1,000,000 USYC for USDC over a long deadline.
2. Alice only keeps 50,000 USYC in her wallet at any time (selling USDC as it arrives).
3. A matcher with `INTENT_MATCHING_ROLE` tries to partially fill Alice's intent for 10,000 USYC
   worth of orders.
4. `swapRWAtoStbcIntent` is called with `partialMatching = true`.
5. `_isValidIntent` returns `remainingAmountUnmatched = 1,000,000`.
6. `_swapRWAtoStbc` tries `safeTransferFrom(Alice, treasury, 1,000,000 USYC)`.
7. Alice only has 50,000 USYC → `safeTransferFrom` reverts.
8. The **partial match for a mere 10,000 USYC fails** even though Alice has more than enough
   to cover the matched amount.

**Griefing variant:** A matcher holding `INTENT_MATCHING_ROLE` (or an attacker who compromised
that role) can deliberately submit tiny partial matches against a victim's intent with
`orderIdsToTake` that match almost nothing. The victim's full remaining RWA balance is pulled,
round-tripped, and returned — minus rounding dust — wasting gas and slowly draining dust. Each
call also triggers the full mint/burn/backing-check gas cost.

**Dust-loss variant:** When `wadRwaNotTakenInUSD` is small (e.g. only a few wei of USD0 were
unmatched), `_getQuoteInToken(wadRwaNotTakenInUSD, rwaToken)` floors to `0` for high-priced
tokens or tokens with fewer decimals. The user gets `0` RWA back and `matchedAmountInTokenDecimals
= amountInTokenDecimals - 0 = amountInTokenDecimals` — the entire pulled amount is treated as
"matched," even though the swapper only matched a few wei of USD0. The user loses that RWA.

---

## Impact

- **Partial matching is effectively broken** for any intent whose recipient does not maintain a
  balance ≥ the full remaining intent amount. Users must over-provision collateral or the
  matching service silently fails.
- **Griefing vector** via the `INTENT_MATCHING_ROLE`: repeated no-op partial matches cause
  unnecessary transfers and gas waste for the recipient.
- **Dust RWA loss** from the rounding asymmetry in the take-all / return-remainder pattern.
- Usual's intent-matching system is the protocol's primary off-chain-coordinated RWA→USDC
  routing mechanism; breaking partial fills degrades the user experience and orderbook depth.

---

## Proof of Concept (Foundry, sketch)

```solidity
// Alice signs intent for 1_000_000e6 USYC but only holds 50_000e6.
// A matcher calls swapRWAtoStbcIntent with partialMatching=true and orders totalling 10_000e6.
// Expected: 10_000e6 matched, 990_000e6 returned, _orderAmountTaken += 10_000e6.
// Actual: reverts at safeTransferFrom(Alice, treasury, 1_000_000e6) — Alice doesn't have it.

function testPartialMatchFailsWhenRecipientLacksFullIntentAmount() public {
    uint256 intentAmount = 1_000_000e6;
    uint256 aliceBalance = 50_000e6;

    // Alice signs intent (omitted: EIP-712 signature)
    Intent memory intent = Intent({
        recipient: alice,
        rwaToken:   USYC,
        amountInTokenDecimals: intentAmount,
        deadline: block.timestamp + 1 days,
        signature: sig
    });

    // Alice only has 50k USYC
    deal(USYC, alice, aliceBalance);

    // Matcher tries to fill 10k worth of orders with partialMatching = true
    uint256[] memory orderIds = new uint256[](1);
    orderIds[0] = smallOrderId; // 10_000e6 USDC order

    vm.prank(matcher); // INTENT_MATCHING_ROLE
    vm.expectRevert(); // ← safeTransferFrom tries to pull 1_000_000e6 from Alice
    daoCollateral.swapRWAtoStbcIntent(orderIds, approval, intent, true);
}
```

---

## Three-Perspective Audit

### 1. Attacker Perspective
A `INTENT_MATCHING_ROLE` holder can grief users by submitting repeated partial matches with
near-empty `orderIdsToTake`, forcing each victim's entire remaining intent balance to be pulled,
round-tripped, and returned (minus dust). With many users and intents, this is a sustained gas
drain. More critically, the attacker can keep the intent "alive" (nonce never consumed if the
remaining amount stays above `nonceThreshold`) and repeat indefinitely.

### 2. Protocol / Defender Perspective
The take-all-then-return pattern was likely chosen for simplicity (one `safeTransferFrom` instead
of a matched-amount pull), but it conflates "partial orderbook fill" with "partial intent fill."
The two should be independent: the matcher should specify how much of the intent to consume, and
only that amount should be pulled. The fix is to add a `matchAmountInTokenDecimals` parameter
to `swapRWAtoStbcIntent` and cap the pull at `min(matchAmount, remainingAmountUnmatched)`.

### 3. Auditor / Sherlock-Judging Perspective
- **In scope:** `DaoCollateral` is a Critical-tier contract.
- **Not privileged-access:** the trigger is a legitimate user flow (partial intent match); the
  role gating is on the matcher, not the victim.
- **Impact:** functional DoS of partial matching + dust fund loss. Falls below "fund theft" but
  above "gas optimisation"; MEDIUM is appropriate.
- **Caveat:** whether this is judged MEDIUM or LOW depends on Sherlock's view of the
  `INTENT_MATCHING_ROLE` trust assumption and whether the dust-loss path is reachable in
  practice with the current RWA decimals.

---

## Recommended Fix

1. Add an explicit `matchAmountInTokenDecimals` parameter to `swapRWAtoStbcIntent` and only
   pull `min(matchAmountInTokenDecimals, remainingAmountUnmatched)` from the recipient:
   ```solidity
   uint256 amountToSwap = matchAmountInTokenDecimals > remainingAmountUnmatched
       ? remainingAmountUnmatched
       : matchAmountInTokenDecimals;
   _swapRWAtoStbc(intent.recipient, intent.rwaToken, amountToSwap, partialMatching, ...);
   ```
2. Guard against `rwaTokensToReturn == 0` in `_swapRWAtoStbc` when `wadRwaNotTakenInUSD > 0`
   (revert with `AmountTooLow` rather than silently confiscating the dust).
3. Consider using `Math.Rounding.Ceil` for the return conversion (`_getQuoteInToken`) so the
   user is never short-changed by flooring, with the protocol absorbing the dust.

---

## References

- `DaoCollateral.sol` lines 565–637 (`_swapRWAtoStbc`), 805–835 (`_useIntentAmount`),
  843–889 (`swapRWAtoStbcIntent`)
- `src/utils/normalize.sol` lines 74–80 (`wadTokenAmountForPrice` — `Math.Rounding.Floor`)
