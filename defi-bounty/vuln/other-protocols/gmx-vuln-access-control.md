# GMX V2 — `SubaccountRouter._autoTopUpSubaccount` Overpayment via Pre-Cap `executionFee`

**Program:** GMX (https://immunefi.com/bug-bounty/gmx/information/)
**KYC Status:** Not Required
**Max Bounty:** $5,000,000
**Severity:** Low
**Area:** Access Control / Order Execution
**Date:** 2026-09-24

---

## Description

`SubaccountRouter._autoTopUpSubaccount` tops up the subaccount's native
token balance based on `params.numbers.executionFee` — the **pre-cap**
execution fee. However, `OrderUtils.createOrder` caps the execution fee
via `GasUtils.validateAndCapExecutionFee` and refunds the excess to the
**master account** (not the subaccount). This creates a discrepancy:
the subaccount is topped up for the pre-cap amount, but the actual
execution fee (and thus the refund) is based on the post-cap amount.

If the subaccount sets a very high `executionFee`, the master account's
WNT is used to top up the subaccount by an amount proportional to the
pre-cap `executionFee`, even though the actual execution fee is capped.
The subaccount effectively receives a larger top-up than the actual gas
cost + execution fee would justify.

While `validateAndCapExecutionFee` caps the execution fee when a
callback contract is present (mitigating the most severe abuse), the
cap is based on `gasLimit * basefee * multiplier`, which can still be
significantly higher than the actual gas cost. The subaccount receives
a top-up proportional to this capped-but-still-high amount.

## Contract + Function + Line

**Contract:** `contracts/router/SubaccountRouter.sol`
**Function:** `_autoTopUpSubaccount`
**Lines:** 230–274

```solidity
function _autoTopUpSubaccount(address account, address subaccount, uint256 startingGas, uint256 executionFee) internal {
    uint256 amount = SubaccountUtils.getSubaccountAutoTopUpAmount(dataStore, account, subaccount);
    if (amount == 0) {
        return;
    }

    IERC20 wnt = IERC20(dataStore.getAddress(Keys.WNT));

    if (wnt.allowance(account, address(router)) < amount) { return; }
    if (wnt.balanceOf(account) < amount) { return; }

    // cap the top up amount to the amount of native tokens used
    uint256 nativeTokensUsed = (startingGas - gasleft()) * tx.gasprice + executionFee;  // <-- pre-cap executionFee
    if (nativeTokensUsed < amount) { amount = nativeTokensUsed; }

    router.pluginTransfer(
        address(wnt), // token
        account, // account
        address(this), // receiver
        amount // amount
    );

    TokenUtils.withdrawAndSendNativeToken(
        dataStore,
        address(wnt),
        subaccount,
        amount
    );
    ...
}
```

The `executionFee` parameter is `params.numbers.executionFee` from the
`createOrder` call (line 147):
```solidity
_autoTopUpSubaccount(
    account, // account
    msg.sender, // subaccount
    startingGas, // startingGas
    params.numbers.executionFee // executionFee — PRE-CAP value
);
```

But inside `OrderUtils.createOrder`, the execution fee is capped:
```solidity
(executionFee, cache.executionFeeDiff) = GasUtils.validateAndCapExecutionFee(
    dataStore,
    cache.estimatedGasLimit,
    params.numbers.executionFee,  // pre-cap
    cache.oraclePriceCount,
    shouldCapMaxExecutionFee
);
order.setExecutionFee(executionFee);  // post-cap

if (cache.executionFeeDiff != 0) {
    GasUtils.transferExcessiveExecutionFee(dataStore, eventEmitter, orderVault, order.account(), cache.executionFeeDiff);
    // refund goes to order.account() (master), NOT subaccount
}
```

## Attack Scenario

1. Master account sets `autoTopUpAmount` to a large value and approves
   the router to spend WNT.
2. Subaccount creates an order with `executionFee = MAX_UINT` (or very
   large) and a `callbackContract`.
3. `SubaccountRouter.createOrder` calls `orderHandler.createOrder`
   which:
   a. Records the WNT transfer (the subaccount sent `executionFee`
      WNT to the orderVault).
   b. Caps the execution fee to `gasLimit * basefee * multiplier`.
   c. Refunds the excess `(pre-cap - post-cap)` to the **master
      account** via `transferExcessiveExecutionFee`.
4. `_autoTopUpSubaccount` is called with the **pre-cap** `executionFee`:
   ```
   nativeTokensUsed = gasUsed * gasprice + pre-cap executionFee
   ```
   Since `pre-cap executionFee` is very large, `nativeTokensUsed` is
   very large, so `amount = autoTopUpAmount` (the full configured
   amount).
5. The master account's WNT is transferred to the subaccount as a
   top-up.
6. The subaccount receives the excess execution fee refund (via
   `order.receiver()` which is set to `account` = master for
   subaccounts) — but wait, the refund goes to `order.receiver()`,
   which is the master account, not the subaccount.

So the subaccount gets:
- The auto-top-up amount (in native tokens, from master's WNT)
- But the execution fee refund goes to the master account

The net effect is: the master account pays `executionFee` WNT to the
orderVault, gets `executionFeeDiff` refunded, and the subaccount gets
`autoTopUpAmount` in native tokens from the master's WNT. The
subaccount's top-up is proportional to the pre-cap `executionFee`,
which can be much larger than the actual gas cost.

However, the cap (`validateAndCapExecutionFee`) limits the execution
fee to `gasLimit * basefee * multiplier`, which bounds the
`nativeTokensUsed` calculation. So the top-up is bounded by
`autoTopUpAmount` (which the master configured) and
`gasLimit * basefee * multiplier`.

The real issue is that the master account may not realize that the
`autoTopUpAmount` can be consumed in full on every subaccount order,
even if the actual gas cost is much lower.

## Impact

The subaccount can drain the master account's WNT balance faster than
the actual gas costs would warrant, because the top-up is proportional
to the pre-cap `executionFee` rather than the actual gas usage. The
master account's `autoTopUpAmount` configuration acts as a per-order
subsidy to the subaccount, which the subaccount can claim on every
order.

This is a design issue rather than a security vulnerability — the
master account configures the `autoTopUpAmount` and can set it to 0
to disable the feature. But the use of the pre-cap `executionFee`
amplifies the top-up beyond what the master might expect.

## Severity

**Low** — The master account configures the `autoTopUpAmount` and
approves the WNT spending. The subaccount cannot extract more than the
configured amount per order. The issue is that the top-up is larger
than the actual gas cost due to the pre-cap `executionFee` being used,
which could deplete the master's WNT faster than expected.

## Three-Perspective Audit

**1. Attacker perspective.** A malicious subaccount can create orders
with high `executionFee` values to maximize the auto-top-up. Each order
drains `autoTopUpAmount` from the master's WNT. The subaccount receives
native tokens that can be used for gas or withdrawn. The profit per
order is `autoTopUpAmount - actual_gas_cost`, which is positive when
the pre-cap `executionFee` is high.

**2. Protocol team perspective.** The fix is to pass the **post-cap**
`executionFee` to `_autoTopUpSubaccount` instead of the pre-cap value.
This would require restructuring the code so that `_autoTopUpSubaccount`
is called after `orderHandler.createOrder` returns (which it already is
— the issue is that the post-cap value isn't returned to the
SubaccountRouter). Alternatively, cap `nativeTokensUsed` to the actual
gas cost only (without the `executionFee` component), since the
execution fee is already handled separately.

**3. Auditor perspective.** Any time a pre-validation value is used for
accounting that should be based on a post-validation value, there's a
potential for discrepancy. The auditor should trace all values that
are used in financial calculations and verify they reflect the actual
(post-validation) state. In this case, the `executionFee` used for
top-up is the user-supplied value, not the system-validated value.
