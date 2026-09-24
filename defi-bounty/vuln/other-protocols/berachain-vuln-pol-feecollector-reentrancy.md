# FeeCollector — Missing ReentrancyGuard on `claimFees`

## Metadata

| Field | Value |
|---|---|
| **Severity** | Low |
| **Area** | Proof-of-Liquidity / Fee Auction |
| **Contract** | `FeeCollector.sol` |
| **Function** | `claimFees` |
| **Line** | 95–113 |
| **File** | `src/pol/FeeCollector.sol` |
| **Audited commit** | `70e392fc` (2026-08-11) |

## Description

`FeeCollector.claimFees` is an open, permissionless auction function: anyone who
transfers `payoutAmount` of `payoutToken` to `rewardReceiver` (BGTStaker) may
sweep all accumulated fee tokens.  The function is **not** `nonReentrant` and
makes several external calls before state finalisation.

```solidity
function claimFees(address _recipient, address[] calldata _feeTokens)
    external whenNotPaused
{
    IERC20(payoutToken).safeTransferFrom(msg.sender, rewardReceiver, payoutAmount);  // ① external pull
    BGTStaker(rewardReceiver).notifyRewardAmount(payoutAmount);                     // ② external call

    for (uint256 i; i < _feeTokens.length;) {
        address feeToken = _feeTokens[i];
        uint256 amt = IERC20(feeToken).balanceOf(address(this));
        IERC20(feeToken).safeTransfer(_recipient, amt);                              // ③ external, callback
        ...
    }

    if (queuedPayoutAmount != 0) _setPayoutAmount();   // ④ state finalisation LAST
}
```

Three external-call sites (①②③) execute **before** `_setPayoutAmount()` at ④.
A callback from any fee-token transfer (③) or from `BGTStaker.notifyRewardAmount`
(②, which internally calls `_notifyRewardAmount` → `_setRewardRate` → external
calls) re-enters `claimFees` while `payoutAmount` is still the old value.

## Attack Scenario

1. Governance queues a `payoutAmount` increase via
   `queuePayoutAmountChange(newAmount)`.
2. `queuedPayoutAmount = newAmount` but `payoutAmount` is still the old, lower
   value.
3. An attacker front-runs the next `claimFees` that would apply the update, and
   calls `claimFees` with a callback-capable fee token.
4. Inside the callback the attacker re-enters `claimFees`, paying the **old**
   `payoutAmount` again and sweeping a second batch of fees.
5. The re-entrant call returns; the outer call's ④ `_setPayoutAmount()` finally
   applies the new rate.

Result: the attacker claims fee batches at the stale, lower payout rate before
the increase takes effect. While each call still pays `payoutAmount`, the
attacker acquires more fee batches than would be possible after the rate
increase.

## Proof of Concept (pseudo-Solidity)

```solidity
contract CallbackFeeToken is ERC777 {
    function transfer(address to, uint256 amt) public returns (bool) {
        // trigger re-entry into claimFees before _setPayoutAmount runs
        IFeeCollector(msg.sender).claimFees(address(this), remainingFeeTokens);
        // ...
    }
}
```

## Impact

- **Stale-rate exploitation:** multiple fee batches claimed at the old
  `payoutAmount` before a queued increase is applied.
- **Reward-notification amplification:** `notifyRewardAmount` is called
  multiple times (once per re-entrant call), inflating the reward rate in
  BGTStaker. Each call is backed by a real `payoutAmount` transfer, so the
  BGT stakers genuinely receive more rewards — but at the attacker's expense.
  The attacker's loss is the extra `payoutAmount` payments, offset by the extra
  fee batches acquired at the old rate.
- **CEI violation:** `_setPayoutAmount()` should execute before any external
  calls, or the function should be `nonReentrant`.

## Three-Perspective Audit

### 1. Attacker perspective
Profitable only when `queuedPayoutAmount > payoutAmount` (a rate increase is
queued). The attacker pays the old rate multiple times to sweep multiple fee
batches that would otherwise cost the new, higher rate. Net profit =
`(newRate - oldRate) × (batchesAcquired - 1)`.

### 2. Protocol perspective
The auction mechanism is sound in principle but the missing guard creates a
front-running incentive around governance-configured rate changes. Adding
`nonReentrant` or moving `_setPayoutAmount()` to the top of the function
eliminates the window.

### 3. Auditor perspective
`FeeCollector` does not inherit `ReentrancyGuardUpgradeable` at all, unlike its
sibling `IncentivesCollector` (which inherits it but forgets to apply the
modifier — see separate report). Both contracts should be hardened.

## Recommendation

```solidity
import { ReentrancyGuardUpgradeable } from "...";

contract FeeCollector is ..., ReentrancyGuardUpgradeable {
    function initialize(...) external initializer {
        ...
        __ReentrancyGuard_init();
    }

    function claimFees(address _recipient, address[] calldata _feeTokens)
-       external whenNotPaused
+       external nonReentrant whenNotPaused
    {
        ...
    }
}
```

Alternatively, move `_setPayoutAmount()` to the **beginning** of `claimFees` so
the queued rate is applied before any external call:

```solidity
function claimFees(...) external whenNotPaused {
+   if (queuedPayoutAmount != 0) _setPayoutAmount();   // apply FIRST
    IERC20(payoutToken).safeTransferFrom(msg.sender, rewardReceiver, payoutAmount);
    ...
-   if (queuedPayoutAmount != 0) _setPayoutAmount();   // remove from end
}
```
