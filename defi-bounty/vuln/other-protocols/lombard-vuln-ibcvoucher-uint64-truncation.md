# Lombard Finance — IBCVoucher Rate Limit uint64 Truncation on Large Wraps/Spend

**Severity:** LOW
**Area:** IBCVoucher — IBC Rate Limiting
**Contracts:** `IBCVoucher.sol`
**Functions:** `IBCVoucher._wrap()`, `IBCVoucher._spend()`, `IBCVoucher._setRateLimit()`
**Lines:** `IBCVoucher.sol:185-228,248-274,120-155`

---

## Description

The `IBCVoucher` contract implements IBC-style rate limiting using `uint64` fields for `supplyAtUpdate`, `limit`, `credit`, and amount casts:

```solidity
// IBCVoucher.sol:25-33
struct RateLimit {
    uint64 supplyAtUpdate;
    uint64 limit;
    uint64 credit;
    uint64 startTime;
    uint64 window;
    uint64 epoch;
    uint16 threshold;
}
```

In `_wrap`, the `amountAfterFee` (a `uint256`) is cast to `uint64` before being added to `credit`:

```solidity
// IBCVoucher.sol:215
$.rateLimit.credit += uint64(amountAfterFee);   // ← truncation if amountAfterFee > type(uint64).max
```

In `_spend`, the `amount` (a `uint256`) is cast to `uint64` for both the comparison and the subtraction:

```solidity
// IBCVoucher.sol:259-268
if (uint64(amount) > $.rateLimit.credit) {       // ← truncated comparison
    revert RateLimitExceeded(...);
}
$.rateLimit.credit -= uint64(amount);            // ← truncated subtraction
```

In `_setRateLimit`, the `totalSupply()` (a `uint256`) is cast to `uint64` for both `supplyAtUpdate` and the `limit` calculation:

```solidity
// IBCVoucher.sol:135-139
$.rateLimit.supplyAtUpdate = uint64(totalSupply);                          // ← truncation
uint64 limit = uint64((threshold * totalSupply) / RATIO_MULTIPLIER);       // ← truncation
$.rateLimit.limit = limit;
$.rateLimit.credit = limit;
```

**In Solidity 0.8+, explicit downcasting `uint64(uint256)` does NOT revert on overflow — it silently truncates.** Only arithmetic operations (`+`, `-`, `*`) revert on overflow.

If `amountAfterFee` exceeds `type(uint64).max` (≈ 1.8 × 10¹⁹), the cast truncates to the lower 64 bits. For `_spend`, if `amount` exceeds `type(uint64).max`, `uint64(amount)` truncates to a small value, potentially passing the rate-limit check without consuming proportional credit.

---

## Attack Scenario

### Scenario A: Rate Limit Bypass on Spend (Theoretical)

1. The IBCVoucher has a rate limit with `credit = 0` (fully consumed).
2. An attacker (with `RELAYER_ROLE` or `OPERATOR_ROLE` for `spendFrom`) attempts to `spend` an amount > `type(uint64).max` (≈ 184 billion BTC in 8-decimal units).
3. `uint64(amount)` truncates to a small value (e.g., if `amount = type(uint64).max + 1`, then `uint64(amount) = 0`).
4. The check `uint64(amount) > credit` becomes `0 > 0`, which is false — the check passes.
5. `$.rateLimit.credit -= uint64(amount)` becomes `credit -= 0 = credit` — no change.
6. The attacker spends `amount` (huge) vouchers and receives `amount` LBTC, but the rate limit credit was not consumed.

### Scenario B: Credit Inflation on Wrap (Theoretical)

1. An attacker wraps an amount > `type(uint64).max`.
2. `uint64(amountAfterFee)` truncates to a small value.
3. `$.rateLimit.credit += uint64(amountAfterFee)` adds only the truncated value.
4. The attacker receives `amountAfterFee` vouchers (full amount via `_mint`), but the rate limit credit only increases by the truncated value.
5. This effectively allows wrapping more than the rate limit intends, because the credit doesn't reflect the true wrapped amount.

### Practicality

`type(uint64).max = 18,446,744,073,709,551,615`. With 8-decimal LBTC (satoshi units), this represents **184,467,440,737 BTC** — approximately 8.8 million times the total Bitcoin supply (21 million BTC). The attack requires wrapping or spending an amount that exceeds the entire Bitcoin supply by 8 orders of magnitude. **This is unreachable in practice.**

---

## Impact

- **Theoretical rate limit bypass:** The rate limit could be bypassed for amounts exceeding ~184 billion BTC.
- **Unreachable in practice:** The total Bitcoin supply is 21 million BTC. The required amount is ~8,800× the total Bitcoin supply.
- **No direct fund theft:** Even if exploited, the attacker would need to control > 184 billion LBTC, which cannot exist (the supply is backed by real BTC).
- **Latent risk:** If the IBCVoucher were ever used with a token having more than 8 decimals (e.g., 18-decimal ERC20), the threshold would be ~18.4 tokens, making the attack practical. However, the IBCVoucher is hardcoded for LBTC (8 decimals).

---

## Three-Perspective Audit

### Prosecutor (Bug Confirmed)

The use of `uint64` for rate limit accounting with silent truncation casts is a latent integer overflow vulnerability. While the LBTC supply (8 decimals) makes this unreachable today, the contract uses `uint64` for amounts that should be `uint256`. The `_spend` function's `uint64(amount) > credit` check can be trivially bypassed if `amount` exceeds `type(uint64).max`, because the truncation produces a small value that passes the check. The `_setRateLimit` function similarly truncates `totalSupply()` to `uint64`, which could produce an incorrect `limit` if supply exceeds the uint64 range. The LBTC `decimals()` is hardcoded to 8, but the contract should still use safe types for robustness.

### Defense (Mitigating Factors)

1. **Unreachable amounts:** `type(uint64).max` in 8-decimal satoshi units = 184 billion BTC. The total Bitcoin supply is 21 million BTC. The attack requires an amount 8,800× the total Bitcoin supply.
2. **Backed supply:** LBTC is backed 1:1 by BTC. The supply can never exceed ~21 million BTC (2.1 × 10¹⁵ satoshi), which is well within `uint64` range.
3. **Hardcoded decimals:** The IBCVoucher's `decimals()` returns 8 (line 334). If this were ever changed to 18, the risk would become real (~18.4 tokens). But the contract is LBTC-specific.
4. **Role-gated:** `_spend` requires either the caller to burn their own vouchers (in which case they already have the balance) or `OPERATOR_ROLE` for `spendFrom`. An external attacker cannot trigger the truncation without first obtaining a huge voucher balance.
5. **`_mint` uses `uint256`:** The actual voucher mint/burn uses `uint256`, so the ERC20 supply is not affected by the truncation — only the rate limit accounting is.

### Judge (Verdict: LOW)

The truncation is real and the cast pattern is unsafe, but the practical exploitability is zero given the 8-decimal LBTC supply and the total Bitcoin supply constraint. The attack requires an amount that is physically impossible to achieve. The issue is a code-quality / latent-risk concern rather than an exploitable vulnerability. **LOW** severity is appropriate. If the contract were ever adapted for higher-decimal tokens, this would become HIGH.

---

## Recommended Fix

Use `uint256` for rate limit amount fields, or add overflow checks on the cast:

```solidity
// Option A: Use uint256 for amounts
struct RateLimit {
    uint256 supplyAtUpdate;
    uint256 limit;
    uint256 credit;
    uint64 startTime;
    uint64 window;
    uint64 epoch;
    uint16 threshold;
}

// Option B: Add bounds check before cast
if (amountAfterFee > type(uint64).max) revert AmountTooLarge();
$.rateLimit.credit += uint64(amountAfterFee);
```

Option A is preferred for future-proofing. Option B is a minimal fix that preserves the current storage layout.
