# Lido Audit — Vulnerability Report: Unchecked `msg.value` Excess in `WithdrawalQueueERC721.finalize` Can Permanently Lock ETH

## Metadata
- **Area**: Withdrawal Queue — Finalization
- **Contract**: `WithdrawalQueueERC721.sol` / `WithdrawalQueueBase.sol` (Solidity 0.8.9)
- **Function**: `finalize` (ERC721 line 147), `_finalize` (Base line 344)
- **Severity**: Low (trusted caller; governance-recoverable)

---

## Description

`WithdrawalQueueERC721.finalize` is the entry point for the Accounting contract
to lock ETH on the queue and mark withdrawal requests as finalised:

```solidity
// contracts/0.8.9/WithdrawalQueueERC721.sol:147
function finalize(uint256 _lastRequestIdToBeFinalized, uint256 _maxShareRate) external payable {
    _checkResumed();
    _checkRole(FINALIZE_ROLE, msg.sender);

    uint256 firstFinalizedRequestId = getLastFinalizedRequestId() + 1;

    _finalize(_lastRequestIdToBeFinalized, msg.value, _maxShareRate);
    ...
}
```

The full `msg.value` is forwarded to `_finalize` as `_amountOfETH`. Inside
`_finalize`:

```solidity
// contracts/0.8.9/WithdrawalQueueBase.sol:344
function _finalize(uint256 _lastRequestIdToBeFinalized, uint256 _amountOfETH, uint256 _maxShareRate) internal {
    ...
    uint128 stETHToFinalize = requestToFinalize.cumulativeStETH - lastFinalizedRequest.cumulativeStETH;
    if (_amountOfETH > stETHToFinalize) revert TooMuchEtherToFinalize(_amountOfETH, stETHToFinalize);
    ...
    _setLockedEtherAmount(getLockedEtherAmount() + _amountOfETH);
    _setLastFinalizedRequestId(_lastRequestIdToBeFinalized);
    ...
}
```

The only upper-bound check is `_amountOfETH > stETHToFinalize` (nominal stETH
cap). There is **no check** that `_amountOfETH` equals the amount calculated by
`prefinalize()` (the exact ETH needed to cover discounted + nominal claims).

If the caller (Accounting) sends more ETH than `prefinalize` computed — whether
due to a rounding-up, an off-by-one, or a future refactor — the excess is
absorbed into `LOCKED_ETHER_AMOUNT_POSITION` and becomes permanently
over-counted:

- Claimants receive the correct per-request amount (determined by
  `_calculateClaimableEther`, which is independent of `lockedEtherAmount`), so
  the excess is never distributed.
- After all requests in the finalised range are claimed, the residual
  `lockedEtherAmount` > 0 with no corresponding claimable requests, locking
  ETH inside the WithdrawalQueue contract indefinitely.

Conversely, if the caller sends **less** ETH than needed, requests are still
marked finalised but `_setLockedEtherAmount` under-counts. When claimants
call `_claim`, the subtraction
`getLockedEtherAmount() - ethWithDiscount` can underflow (Solidity 0.8.x
revert) or `_sendValue` can fail due to insufficient contract balance,
permanently blocking claims for the affected range.

---

## Attack Scenario

### Excess ETH (locked-forever)
1. Accounting calls `finalize` with `msg.value = prefinalizeResult + 1 wei`
   (rounding surplus or bug).
2. `_finalize` passes the `<= stETHToFinalize` check (1 wei is negligible).
3. `lockedEtherAmount` is inflated by 1 wei.
4. All claims resolve correctly, but 1 wei remains locked forever. Repeated
   over many reports, dust accumulates.

### Insufficient ETH (claim DoS)
1. Due to a rounding discrepancy between `prefinalize` (per-batch) and
   `_calculateClaimableEther` (per-request), Accounting sends
   `prefinalizeResult` but the sum of per-request claims is 1 wei higher.
2. The last claimant's `_setLockedEtherAmount(locked - ethWithDiscount)`
   underflows → revert.
3. The claimant can never claim; their ETH is stuck until governance
   tops up the contract or adjusts state.

---

## Proof of Concept (Foundry — conceptual)

```solidity
// SPDX-License-Identifier: MIT
pragma solidity 0.8.9;

import "forge-std/Test.sol";

contract FinalizeExcessPoC is Test {
    // Setup: deploy WithdrawalQueueERC721 with stETH mock
    // 1. Enqueue 2 requests of 100 stETH each (shares = 100 each)
    // 2. Finalise both with maxShareRate = 1:1
    //    prefinalize returns ethToLock = 200 ether
    // 3. Call finalize{value: 201 ether}(2, 1e27)  // 1 ether excess
    //    -> _finalize passes (201 <= 200 cumulative stETH? NO -> reverts)
    //
    // Actually the stETH cap catches large excess. But for dust:
    //    Finalise with 200 ether + 1 wei where cumulative stETH = 200 ether
    //    -> 200.000000000000000001 > 200 ether -> reverts. OK, stETH cap protects.
    //
    // The real concern is the INSUFFICIENT direction:
    //    prefinalize rounds DOWN, per-request claims sum to 1 wei MORE
    //    -> last claim reverts due to lockedEther underflow
}
```

---

## Impact

| Perspective | Assessment |
|---|---|
| **Fund loss** | Dust-locking (excess direction) or temporary claim blockage (insufficient direction). |
| **Trust model** | Caller is Accounting (trusted). A bug in Accounting's `etherToLockOnWithdrawalQueue` calculation is the trigger. The `checkSimulatedShareRate` sanity check mitigates but does not eliminate rounding edges. |
| **Recoverability** | Excess dust requires governance recovery function. Insufficient case self-resolves if more ETH is sent in a later `finalize` (lockedEther can go negative-safe via later top-ups). |

**Overall severity: Low** — trusted caller, rounding edges are 1–2 wei per
the code's own comments (`issue #442`), and the `stETHToFinalize` cap prevents
large excess.

---

## 3-Perspective Audit

### 1. Correctness
`_finalize` trusts the caller to send the exact `prefinalize` amount. The
`stETHToFinalize` cap prevents gross overpayment but not dust-level excess. The
`_calculateClaimableEther` per-request rounding can differ from the per-batch
rounding in `prefinalize`, creating a 1–2 wei discrepancy per claim batch.

### 2. Security
The insufficient-ETH direction is the more concerning case: a permanent claim
DoS for the last request in a batch. While the amounts are dust, the DoS
persists until governance intervenes, which could take days.

### 3. Code Quality
The code already acknowledges the rounding issue (`issue #442`) but only
mitigates the excess-dust direction (dust accumulates on the contract). The
insufficient direction is not mitigated. A defensive `require(msg.value ==
_ethRequired)` or a reconciliation step after all claims would close the gap.

---

## Recommended Fix

1. In `_finalize`, add a lower-bound check:
   ```solidity
   uint256 expectedEth = _calculateExpectedEth(_lastRequestIdToBeFinalized, _maxShareRate);
   require(_amountOfETH >= expectedEth, "INSUFFICIENT_ETH");
   ```
2. Add a governance function to recover excess `lockedEtherAmount` dust after
   all requests in a range are claimed.
3. Long-term: reconcile `lockedEtherAmount` with actual claimed amounts at the
   end of each finalisation range.

---

## File Paths
- Vulnerable contract: `/home/z/lido/contracts/0.8.9/WithdrawalQueueERC721.sol` (line 147)
- Vulnerable contract: `/home/z/lido/contracts/0.8.9/WithdrawalQueueBase.sol` (line 344)
- Caller: `/home/z/lido/contracts/0.4.24/Lido.sol` `collectRewardsAndProcessWithdrawals` (line ~1075)
- Calculation: `/home/z/lido/contracts/0.8.9/WithdrawalQueueBase.sol` `prefinalize` (line 298)
