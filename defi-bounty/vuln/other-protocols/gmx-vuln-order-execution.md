# GMX V2 — `ShiftUtils.executeShift` Double `recordTransferIn` DoS via StrictBank Underflow

**Program:** GMX (https://immunefi.com/bug-bounty/gmx/information/)
**KYC Status:** Not Required
**Max Bounty:** $5,000,000
**Severity:** Medium
**Area:** Order Execution / Liquidity / Cross-Contract
**Date:** 2026-09-24

---

## Description

`ShiftUtils.executeShift` calls `shiftVault.recordTransferIn` for the
deposit market's long and short tokens **before** the withdrawal and
again **after** the withdrawal. The purpose of the pre-withdrawal call
is to "absorb" any tokens donated directly to the ShiftVault so they
are not counted as withdrawal output (avoiding deposit-fee avoidance).

However, both calls go through `StrictBank._recordTransferIn`, which
reverts on underflow (`nextBalance - prevBalance`). If the ShiftVault's
balance of either token decreases between the pre-withdrawal and
post-withdrawal `recordTransferIn` calls — for example because the
withdrawal sends tokens to the ShiftVault but a rebasing token
simultaneously negative-rebases, or because the token charges a
fee-on-transfer that reduces the ShiftVault's balance — the
post-withdrawal `recordTransferIn` reverts, permanently bricking the
shift.

This is a **concrete manifestation** of the StrictBank underflow bug
(already reported separately) in the shift execution path. The shift
path is particularly vulnerable because it involves TWO
`recordTransferIn` calls per token (one before and one after the
withdrawal), doubling the window in which a balance decrease can
trigger the underflow.

## Contract + Function + Line

**Contract:** `contracts/shift/ShiftUtils.sol`
**Function:** `executeShift`
**Lines:** 199–200 (pre-withdrawal), 254–255 (post-withdrawal)

```solidity
// pre-withdrawal: absorb donated tokens
params.shiftVault.recordTransferIn(cache.depositMarket.longToken);   // line 199
params.shiftVault.recordTransferIn(cache.depositMarket.shortToken);  // line 200

// ... withdrawal executes, sends tokens to shiftVault ...

// post-withdrawal: record the withdrawal output
cache.initialLongTokenAmount = params.shiftVault.recordTransferIn(cache.depositMarket.longToken);   // line 254
cache.initialShortTokenAmount = params.shiftVault.recordTransferIn(cache.depositMarket.shortToken);  // line 255
```

Each `recordTransferIn` call invokes `StrictBank._recordTransferIn`:

```solidity
function _recordTransferIn(address token) internal returns (uint256) {
    uint256 prevBalance = tokenBalances[token];
    uint256 nextBalance = IERC20(token).balanceOf(address(this));
    tokenBalances[token] = nextBalance;
    return nextBalance - prevBalance;   // reverts if nextBalance < prevBalance
}
```

## Attack Scenario

1. A user creates a shift from Market A to Market B, where both markets
   share the same long token (e.g. WETH) and short token (e.g. USDC).
2. The shift is executed:
   a. Pre-withdrawal `recordTransferIn(WETH)` sets `tokenBalances[WETH]`
      to the current ShiftVault WETH balance (e.g. 100 WETH).
   b. The withdrawal from Market A sends 50 WETH to the ShiftVault.
   c. Post-withdrawal `recordTransferIn(WETH)` should return 50 (the
      withdrawal output).
3. If WETH is a rebasing token (or has a fee mechanism that reduces
   the ShiftVault's balance) and the balance drops from 150 to 140
   between steps (b) and (c), the post-withdrawal call sees
   `nextBalance (140) < prevBalance (150)` and reverts.
4. The shift execution fails. Since shifts are executed by keepers, the
   shift remains pending and the user's market tokens are stuck in the
   ShiftVault until the shift is cancelled or an admin intervenes.

This is particularly impactful for GLV shifts (`GlvShiftUtils.executeGlvShift`),
which call `ShiftUtils.executeShift` internally. A single rebasing token
in any GLV-supported market could brick all GLV shifts for that token.

## Proof of Concept (Foundry)

The PoC for the underlying StrictBank underflow is in:
`/home/z/gmx-poc/test/StrictBankUnderflowPoC.t.sol`

The `test_TransferOutDriftThenUnderflow` test demonstrates the exact
scenario: a balance decrease between two `recordTransferIn` calls
causes an underflow revert.

## Impact

- All shifts involving a token whose balance can decrease between the
  pre-withdrawal and post-withdrawal `recordTransferIn` calls are DoSed.
- GLV shifts (`GlvShiftUtils`) are also affected since they delegate to
  `ShiftUtils.executeShift`.
- JIT orders (`JitOrderHandler`) that use GLV shifts are indirectly
  affected.
- The shift remains pending until cancelled; the user's market tokens
  are stuck in the ShiftVault.

## Severity

**Medium** — DoS on shifts and GLV shifts for affected tokens. The
shift can be cancelled, but the user's funds are temporarily stuck.
The attack requires a rebasing or fee-on-transfer token, which limits
the attack surface.

## Three-Perspective Audit

**1. Attacker perspective.** The attacker does not need to be a
depositor — anyone can trigger a negative rebase on a rebasing token
(if the token's rebase mechanism is permissionless or triggered by
market conditions). The cost is the gas of the rebase trigger. The
impact is a DoS on all shifts for that token pair.

**2. Protocol team perspective.** The fix is the same as for the
StrictBank underflow: make `_recordTransferIn` defensive by returning 0
when `nextBalance < prevBalance` instead of reverting. This would make
the pre-withdrawal `recordTransferIn` calls (which don't use the return
value) safe, and the post-withdrawal calls would return 0 (correctly
indicating no net transfer) if the balance decreased.

**3. Auditor perspective.** The double `recordTransferIn` pattern in
shifts is an amplifier for the StrictBank underflow bug. Even if the
underlying StrictBank bug is fixed, the auditor should verify that the
shift's pre-withdrawal `recordTransferIn` calls (which intentionally
discard the return value) do not cause accounting issues. The
pre-withdrawal calls set `tokenBalances[token]` to the current balance,
which means any donated tokens are absorbed. If the balance subsequently
decreases, the post-withdrawal call would return 0 (with the fix),
correctly indicating no withdrawal output — but the donated tokens would
be lost. This is acceptable since donated tokens are meant to be
absorbed anyway.
