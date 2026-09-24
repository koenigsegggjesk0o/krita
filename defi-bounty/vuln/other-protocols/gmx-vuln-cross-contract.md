# GMX V2 — `FeeHandler.claimFees` V1 Underflow on `balanceAfter - balanceBefore`

**Program:** GMX (https://immunefi.com/bug-bounty/gmx/information/)
**KYC Status:** Not Required
**Max Bounty:** $5,000,000
**Severity:** Low
**Area:** Cross-Contract / Integer Precision
**Date:** 2026-09-24

---

## Description

`FeeHandler.claimFees` computes the fee amount received from the V1 vault
by diffing the pre- and post-call `balanceOf`. The diff is computed as
`balanceAfter - balanceBefore` using raw unsigned subtraction in Solidity
0.8+, which reverts on underflow. This is the same class of bug as the
`StrictBank._recordTransferIn` underflow (already reported separately),
but in a different contract and code path.

If the FeeHandler's ERC20 balance of `feeToken` decreases between the
`balanceBefore` and `balanceAfter` snapshots (e.g. because `feeToken`
is a rebasing token that negative-rebases during the `withdrawFees`
call, or because the V1 vault's `withdrawFees` invokes a callback that
transfers tokens out of the FeeHandler), the subtraction reverts and the
V1 fee claim is permanently DoSed.

## Contract + Function + Line

**Contract:** `contracts/fee/FeeHandler.sol`
**Function:** `claimFees`
**Lines:** 67–73

```solidity
function claimFees(address market, address feeToken, uint256 version) external nonReentrant {
    uint256 feeAmount;
    if (version == v1) {
        uint256 balanceBefore = IERC20(feeToken).balanceOf(address(this));
        IVaultGovV1(vaultV1.gov()).withdrawFees(address(vaultV1), feeToken, address(this));
        uint256 balanceAfter = IERC20(feeToken).balanceOf(address(this));
        feeAmount = balanceAfter - balanceBefore;   // <-- reverts if balanceAfter < balanceBefore
    } else if (version == v2) {
        ...
    }
}
```

## Attack Scenario

1. The FeeHandler accumulates V1 fees in `feeToken` (e.g. WETH, USDC).
2. `feeToken` is a rebasing token (or has an admin-controlled `sweep`
   function) whose balance can decrease without a corresponding
   `transfer` call from the FeeHandler.
3. A `feeKeeper` calls `claimFees(market, feeToken, v1)`.
4. Between the `balanceBefore` and `balanceAfter` snapshots, the
   `feeToken` balance of FeeHandler decreases (e.g. negative rebase
   or a callback from the V1 vault that transfers tokens out).
5. `balanceAfter - balanceBefore` reverts with arithmetic underflow.
6. V1 fee claiming is DoSed until an admin manually intervenes.

This is lower severity than the StrictBank variant because:
- It only affects V1 fee claiming (legacy path).
- V1 is being phased out.
- The `feeToken` must be a rebasing/sweepable token.

## Impact

Denial-of-service on V1 fee claiming for any token whose balance in
`FeeHandler` can decrease between the two `balanceOf` snapshots.

## Severity

**Low** — V1-only legacy path, requires special token behaviour.

## Three-Perspective Audit

**1. Attacker perspective.** The attacker would need to be the `feeKeeper`
(or cause a rebasing token to negative-rebase between the two
`balanceOf` calls). The attack is a griefing vector against the protocol's
fee claiming, not a direct theft.

**2. Protocol team perspective.** The fix is to use a defensive diff:
```solidity
if (balanceAfter > balanceBefore) {
    feeAmount = balanceAfter - balanceBefore;
} else {
    feeAmount = 0;
}
```
Or use `SafeERC20.tryOpIncrease` / `Calc.diff` pattern.

**3. Auditor perspective.** Any `balanceAfter - balanceBefore` pattern
on an ERC20 `balanceOf` is a candidate for this class of bug. The
auditor should flag all such instances, especially when the intermediate
call (`withdrawFees`) is an external call that could trigger callbacks.
