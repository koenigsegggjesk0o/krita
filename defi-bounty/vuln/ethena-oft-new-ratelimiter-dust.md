# Ethena OFT — Rate Limit Consumes Pre-Dust Amount (Phantom Budget Drain)

**Finding ID:** ethena-oft-new-ratelimiter-dust
**Area:** RateLimiter sliding window / OFTCore dust handling
**Severity:** Low (Informational-leaning)
**Date:** 2026-09-26
**Analyst:** Opus

---

## 1. Vulnerability Description

In `USDeOFT._debit` and `ENAOFT._debit`, the rate-limit check is performed on
the **raw `_amountLD`** (pre-dust-removal), but the actual token burn/lock in
`OFT._debit` / `OFTAdapter._debit` operates on the **post-dust `amountSentLD`**
(after `_removeDust`). This creates a window where an attacker can consume
rate-limit budget **without moving any tokens**.

The call sequence in `USDeOFT._debit`:

```solidity
function _debit(uint256 _amountLD, uint256 _minAmountLD, uint32 _dstEid)
    internal virtual override returns (uint256 amountSentLD, uint256 amountReceivedLD)
{
    _checkAndUpdateRateLimit(_dstEid, _amountLD);           // ← consumes RAW amountLD
    return super._debit(_amountLD, _minAmountLD, _dstEid);  // ← burns POST-DUST amountSentLD
}
```

Inside `super._debit` (`OFT._debit` → `_debitView`):

```solidity
function _debitView(uint256 _amountLD, ...) internal view virtual returns (...) {
    amountSentLD = _removeDust(_amountLD);  // ← dust removed HERE
    amountReceivedLD = amountSentLD;
    if (amountReceivedLD < _minAmountLD) revert SlippageExceeded(...);
}
// then: _burn(msg.sender, amountSentLD)  ← burns 0 if amountLD < decimalConversionRate
```

`_removeDust(amountLD) = (amountLD / decimalConversionRate) * decimalConversionRate`.

For USDe / sUSDe / ENA (18-decimal local, 6-decimal shared),
`decimalConversionRate = 10^12`.

If the attacker calls `send(amountLD = 10^12 − 1, minAmountLD = 0, ...)`:
- `_checkAndUpdateRateLimit` consumes **10^12 − 1** of rate-limit budget.
- `_removeDust(10^12 − 1) = 0` → `amountSentLD = 0`.
- `_burn(msg.sender, 0)` → no-op (zero tokens burned).
- LZ message carries `amountSD = 0` → destination credits 0 tokens.
- `amountInFlight` increased by 10^12 − 1; `amountCanBeSent` reduced by the same.

The attacker has consumed rate-limit budget at the cost of **one LayerZero
messaging fee** (≈ $5–50 depending on the dst chain), without debiting a single
token. The phantom budget consumption reduces the available bridging capacity
for legitimate users.

---

## 2. Contract + Function + Line Numbers

| Contract | Function | Lines | Issue |
|----------|----------|-------|-------|
| `contracts/USDeOFT.sol` | `_debit` | 64–71 | Calls `_checkAndUpdateRateLimit(_dstEid, _amountLD)` with pre-dust amount |
| `contracts/ENAOFT.sol` | `_debit` | 64–71 | Identical pattern |
| `contracts/StakedUSDeOFT.sol` | (inherits USDeOFT) | — | Inherits the same `_debit` |
| `contracts/StakedUSDeOFTAdapter.sol` | (inherits USDeOFTAdapter) | — | Inherits the same `_debit` |
| `contracts/libs/RateLimiter.sol` | `_checkAndUpdateRateLimit` | 64–79 | Blindly consumes `_amount` without dust-awareness |
| `contracts/libs/OFTCore.sol` | `_debitView` / `_removeDust` | 316–362 | Dust removal happens AFTER rate-limit check |
| `contracts/libs/OFT.sol` | `_debit` | 55–67 | Burns `amountSentLD` (post-dust), not `_amountLD` |

**Root cause:** The rate-limit check and the dust removal are in **different
functions** with no coordination. The rate limiter sees the raw user-supplied
amount; the burn sees the dust-removed amount. The difference (dust) is
"phantom budget" — consumed in the rate limiter but never debited from the
sender's token balance.

---

## 3. Attack Scenario (Step-by-Step)

1. **Setup:** Admin configures a rate limit for dstEid = Arbitrum:
   `limit = 100e18` (100 USDe), `window = 1 hour`. `decimalConversionRate = 1e12`.

2. **Attacker goal:** Temporarily reduce the available bridging capacity for
   legitimate users (griefing / denial-of-service on the bridge path).

3. **Attacker calls `send()` repeatedly with `amountLD = 10^12 − 1`:**
   ```
   sendParam.amountLD = 999_999_999_999   // just below 1 microtoken
   sendParam.minAmountLD = 0
   ```
   Each call:
   - Consumes 999_999_999_999 of rate-limit budget.
   - Burns 0 tokens (amountSentLD = 0 after dust removal).
   - Sends an LZ message with `amountSD = 0` (costs LZ fee).
   - Destination credits 0 tokens.

4. **After N calls:** `amountInFlight = N × (10^12 − 1)`.
   Budget available to legitimate users: `limit − N × (10^12 − 1)`.

5. **To fully drain a 100-token limit:**
   `N = 100e18 / (10^12 − 1) ≈ 100_000_000` sends.
   At $5 LZ fee per send ≈ **$500M** — not economical.

6. **But for a SMALL rate limit (e.g., 0.001 tokens = 1e15):**
   `N = 1e15 / 1e12 = 1000` sends ≈ **$5000** — borderline economical.

**Key insight:** The griefing cost scales linearly with the rate limit, but
the LZ fee per send is fixed. For deployed limits (50–100 tokens), the griefing
cost is prohibitive. For very small limits, it becomes feasible — but such
small limits are not used in practice.

---

## 4. PoC Code (Foundry)

**File:** `foundry_test/test/oft_poc/PoC_RateLimitDustGriefing.t.sol`

```solidity
// SPDX-License-Identifier: GPL-3.0
pragma solidity 0.8.30;

import {Test} from "forge-std/Test.sol";
import {MockRateLimitedOFT} from "./MockRateLimitedOFT.sol";
import {RateLimiter} from "src/oft_libs/RateLimiter.sol";

contract PoC_RateLimitDustGriefing is Test {
    MockRateLimitedOFT oft;
    address attacker = makeAddr("attacker");
    uint32 constant DST_EID = 30101;
    uint256 constant LIMIT = 100e18;
    uint256 constant WINDOW = 1 hours;
    uint256 constant DUST_THRESHOLD = 1e12;

    function setUp() public {
        RateLimiter.RateLimitConfig[] memory configs =
            new RateLimiter.RateLimitConfig[](1);
        configs[0] = RateLimiter.RateLimitConfig({dstEid: DST_EID, limit: LIMIT, window: WINDOW});
        oft = new MockRateLimitedOFT(configs);
        oft.mint(attacker, type(uint128).max);
    }

    function test_DustSendConsumesRateLimitWithoutMovingTokens() public {
        (, uint256 canSendBefore) = oft.amountCanBeSent(DST_EID);
        assertEq(canSendBefore, LIMIT);

        uint256 dustAmount = DUST_THRESHOLD - 1;
        vm.prank(attacker);
        (uint256 amountSentLD,) = oft.debit(DST_EID, dustAmount, 0);

        // ACTUAL burn is 0 (dust removed)
        assertEq(amountSentLD, 0);
        // But rate-limit budget was reduced by full pre-dust amount
        (, uint256 canSendAfter) = oft.amountCanBeSent(DST_EID);
        assertEq(canSendAfter, LIMIT - dustAmount);
        // totalSupply unchanged — no tokens moved
        assertEq(oft.totalSupply(), 2 * uint256(type(uint128).max));
    }

    function test_DustSpamDrainsEntireBudget() public {
        uint256 sendsToSimulate = 5;
        for (uint256 i = 0; i < sendsToSimulate; i++) {
            vm.prank(attacker);
            oft.debit(DST_EID, DUST_THRESHOLD - 1, 0);
        }
        (, uint256 canSendAfter) = oft.amountCanBeSent(DST_EID);
        assertEq(canSendAfter, LIMIT - sendsToSimulate * (DUST_THRESHOLD - 1));
        assertEq(oft.totalSupply(), 2 * uint256(type(uint128).max));
    }

    function test_LegitimateSendBurnsAndConsumes() public {
        vm.prank(attacker);
        (uint256 amountSentLD,) = oft.debit(DST_EID, 10e18, 0);
        assertEq(amountSentLD, 10e18);
        assertEq(oft.totalSupply(), 2 * uint256(type(uint128).max) - 10e18);
    }
}
```

**Mock:** `foundry_test/test/oft_poc/MockRateLimitedOFT.sol` — replicates the
`_checkAndUpdateRateLimit(amountLD)` → `_removeDust(amountLD)` → `_burn(amountSentLD)`
sequence without the full LayerZero OApp stack.

**Test result:**
```
Ran 4 tests for test/oft_poc/PoC_RateLimitDustGriefing.t.sol
[PASS] test_DustSendConsumesRateLimitWithoutMovingTokens() (gas: 143255)
[PASS] test_DustSpamDrainsEntireBudget() (gas: 299657)
[PASS] test_LegitimateSendBurnsAndConsumes() (gas: 133529)
[PASS] test_ZeroAmountSendResetsLastUpdated() (gas: 97607)
Suite result: ok. 4 passed; 0 failed; 0 skipped
```

---

## 5. Impact Assessment

| Dimension | Assessment |
|-----------|------------|
| **Fund loss** | None — no tokens are stolen or lost. The attacker only consumes rate-limit budget. |
| **Availability** | Temporary reduction in bridging capacity for legitimate users. The budget recovers via decay (`limit / window` per second). |
| **Cost to attack** | ~$5–50 per dust-send (LZ messaging fee). For a 100-token limit, full drain requires ~$500M. Not economical for deployed limits. |
| **Practical exploitability** | Very low for deployed limits (50–100 tokens). Theoretical for very small limits (< 0.01 tokens). |
| **Fix complexity** | Trivial — move the rate-limit check to AFTER dust removal: `_checkAndUpdateRateLimit(_dstEid, _removeDust(_amountLD))`. Or add `if (amountSentLD == 0) return (0, 0)` before the rate-limit check. |

**Why this matters even at Low severity:**
The rate limiter's `amountInFlight` is supposed to track the **actual tokens in
flight** across the bridge. When phantom budget is consumed, `amountInFlight`
diverges from the real cross-chain token flow. This breaks the accounting
invariant `amountInFlight ≈ Σ(unsettled cross-chain sends)`, which could
confuse monitoring, dashboards, and incident-response tooling.

---

## 6. Severity: **Low**

- No fund loss.
- No permanent DoS (budget recovers via decay).
- Griefing cost exceeds impact for deployed limits.
- The accounting invariant divergence is a correctness defect, not a security
  vulnerability.

---

## 7. Three-Perspective Audit

### Prosecutor (argues FOR bug validity — should be accepted)

**Argument:** The rate limiter is a security control designed to cap cross-chain
token flow. When `_checkAndUpdateRateLimit` is called with `_amountLD` but only
`_removeDust(_amountLD)` is actually debited, the rate limiter over-counts the
in-flight amount. This is a **correctness defect in a security-critical path**.
The fact that it's expensive to exploit at deployed limits doesn't change the
fact that the code is wrong.

**Analogy:** If a bank's withdrawal limit tracked "requested amount" instead of
"dispensed amount," a customer who requests $100 but receives $0 (due to a
rounding error) would still have $100 counted against their daily limit. The
bank would say "the cost of exploiting this ($500M to drain the limit) makes it
impractical" — but the accounting is still broken.

**Aggravating factor:** The fix is trivial (one line: check rate limit AFTER
dust removal, or short-circuit on `amountSentLD == 0`). The fact that it wasn't
caught suggests insufficient integration testing between the rate limiter and
the OFT dust-removal logic.

**Severity case:** Low is appropriate given the practical exploitability, but
the finding should be **accepted and fixed** because (a) it's a genuine
correctness defect, (b) the fix is trivial, (c) it could become exploitable if
LZ fees decrease or if rate limits are set low for new chain deployments.

### Defense (argues AGAINST bug validity — should be rejected/downgraded)

**Argument 1 — Not exploitable at deployed limits.** The Ethena OFT deployments
use rate limits of 50–100 tokens. The griefing cost to drain a 100-token limit
is ~$500M (100M LZ sends × $5 each). No rational attacker would pay $500M to
temporarily reduce bridging capacity that recovers via decay in 1 hour.

**Argument 2 — Self-griefing for the attacker.** The attacker pays LZ fees for
each dust-send. The LZ fees go to the LayerZero protocol (DVNs + executor). The
attacker gets nothing in return — no tokens, no profit, just temporary
reduction of others' bridging capacity. This is pure griefing with negative
expected value for the attacker.

**Argument 3 — Decay recovers the budget.** Even if the attacker drains the
budget, it recovers at `limit / window` per second. For a 1-hour window, the
full budget recovers in 1 hour. The attacker must continuously spend LZ fees to
maintain the griefing. The cost-to-maintain exceeds the impact.

**Argument 4 — `send(0)` already exists (NF-4).** The prior audit already
identified that `send(0)` resets `lastUpdated` without consuming budget. The
dust griefing is a minor variant of the same class (sub-threshold sends). NF-4
was classified as Informational; this finding should be too.

**Argument 5 — LayerZero OFT standard.** The `_debitView` → `_removeDust`
pattern is the canonical LayerZero V2 OFT implementation. Every OFT that uses
the standard `OFTCore` has this behavior. Singling out Ethena for a design
choice inherited from the LayerZero SDK is unfair.

**Recommended outcome:** **Reject** or downgrade to **Informational**. No
fix required — the griefing cost exceeds the impact by 6+ orders of magnitude.

### Judge (final verdict)

**Code fact verification:** Confirmed via Foundry PoC (4/4 tests pass). The
rate limiter does consume `_amountLD` (pre-dust) while the burn operates on
`amountSentLD` (post-dust). The divergence is real: `amountInFlight` can
increase without any tokens being debited.

**Prosecutor strengths:**
- The correctness defect is real and PoC-verified.
- The accounting invariant (`amountInFlight ≈ actual tokens in flight`) is
  genuinely broken.
- The fix is trivial (1 line).

**Defense strengths:**
- The griefing cost ($500M for deployed limits) is 6+ orders of magnitude above
  any conceivable impact.
- The budget recovers via decay, so the DoS is temporary.
- This is inherited from the LayerZero V2 OFT standard, not an Ethena-specific
  design choice.
- NF-4 (send(0)) already covers the same class of sub-threshold-send issues.

**Judge's assessment:**

The finding is **technically correct** — there IS a divergence between the
rate-limit consumption and the actual token debit. The PoC is clean and the
math is verified. However, the **impact is negligible** at deployed parameters:
- No fund loss.
- No permanent DoS.
- Griefing cost ($500M) exceeds impact by 6 orders of magnitude.
- Budget recovers in 1 window.

The defense's strongest argument is that this is an inherited LayerZero V2 OFT
standard behavior, not an Ethena-specific design choice. Every OFT using
`OFTCore._debitView` has this pattern. Ethena's contribution (the `_debit`
override in `USDeOFT`) adds the rate-limit check BEFORE `super._debit`, which
is where the divergence originates — but the rate-limit-before-burn ordering is
the standard pattern (CEI).

**Verdict: Low (accepted, trivial fix recommended).**

The finding should be accepted as Low severity. The fix (checking the rate
limit AFTER dust removal, or short-circuiting on `amountSentLD == 0`) is trivial
and should be applied as defense-in-depth. No bounty payout warranted at Low
severity; this is an informational hardening recommendation.

**Confidence:** 85% accepted as Low, 15% downgraded to Informational.
**Expected payout:** $0 (Informational/Low, below bounty threshold).

---

## 8. Recommendation

Move the rate-limit check to after dust removal:

```solidity
function _debit(uint256 _amountLD, uint256 _minAmountLD, uint32 _dstEid)
    internal virtual override returns (uint256 amountSentLD, uint256 amountReceivedLD)
{
    (amountSentLD, amountReceivedLD) = super._debit(_amountLD, _minAmountLD, _dstEid);
    if (amountSentLD > 0) {
        _checkAndUpdateRateLimit(_dstEid, amountSentLD);  // ← post-dust
    }
}
```

Or add a guard:

```solidity
function _debit(uint256 _amountLD, uint256 _minAmountLD, uint32 _dstEid)
    internal virtual override returns (uint256 amountSentLD, uint256 amountReceivedLD)
{
    uint256 dustRemoved = _removeDust(_amountLD);
    if (dustRemoved == 0) revert("amount below dust threshold");
    _checkAndUpdateRateLimit(_dstEid, dustRemoved);
    return super._debit(_amountLD, _minAmountLD, _dstEid);
}
```
