# Lido Audit — Vulnerability Report: Deposit Accounting Edge-Case in `Lido.processClStateUpdate` with Multi-Frame Report Delay

## Metadata
- **Area**: Lido core — CL state update / deposit tracking
- **Contract**: `Lido.sol` (Solidity 0.4.24)
- **Function**: `processClStateUpdate` (line ~982), `_getDepositedNextReportAdjusted` (line ~791)
- **Severity**: Low (temporary share-rate distortion; self-correcting)

---

## Description

Lido tracks ETH deposited to the DepositContract but not yet reflected in the
oracle's CL balance report via two packed counters:

- `depositedPostReport` — cumulative deposits since the last report.
- `depositedNextReport` — deposits in the *current* reporting frame, paired
  with a `lastDepositNonce` (the refSlot at which the counter was last reset).

Internal ether is:
```
internalEther = bufferedEther + clValidatorsBalance + clPendingBalance + depositedPostReport
```

During `processClStateUpdate`, the protocol resets `depositedPostReport`:

```solidity
// contracts/0.4.24/Lido.sol ~line 982
function processClStateUpdate(uint256 _reportTimestamp, uint256 _clValidatorsBalance, uint256 _clPendingBalance) external {
    _whenNotStopped();
    _auth(_accounting());

    (uint256 depositedNextReport, uint256 curNonce) = _getDepositedNextReportAdjusted();
    _setDepositedNextReportAndLastDepositNonce(depositedNextReport, curNonce);
    _setDepositedPostReport(depositedNextReport);  // <--- potentially lossy
    ...
}
```

`_getDepositedNextReportAdjusted` returns `0` when the current refSlot
(`curNonce`) differs from `lastDepositNonce`:

```solidity
function _getDepositedNextReportAdjusted() internal view returns (uint256 depositedNextReport, uint256 curNonce) {
    uint256 lastNonce;
    (depositedNextReport, lastNonce) = _getDepositedNextReportAndLastDepositNonce();
    (curNonce,) = _getCurrentFrame();
    if (curNonce != lastNonce) {
        depositedNextReport = 0;   // <--- resets to zero on frame change
    }
}
```

The design assumes the oracle report is always for the **current** frame's
refSlot, so `depositedNextReport` correctly captures only post-refSlot deposits.
However, if the oracle **skips one or more frames** (missed report deadline,
consensus failure, or governance-triggered pause), the report is for an
**earlier** refSlot. In that case:

1. Deposits made in intermediate frames (between the report's refSlot and the
   current frame) were sent to the DepositContract — they are **not** in
   `bufferedEther` (already subtracted via `_spendDepositableEther`) and **not**
   in the CL balances reported for the earlier refSlot.
2. `depositedNextReport` was reset to `0` when the frame changed, so
   `_setDepositedPostReport(0)` discards those intermediate-frame deposits.
3. `internalEther` undercounts by the intermediate deposit amount, causing a
   **temporary negative rebase** (share-rate dip) until the next oracle report
   includes the missing CL balance.

---

## Attack Scenario

1. Frame N begins at refSlot N. Lido deposits 1000 ETH to the DepositContract.
   `depositedPostReport = 1000`, `depositedNextReport = 1000`,
   `lastNonce = N`.
2. Frame N+1 begins. No oracle report for frame N is submitted (missed deadline
   or consensus failure). Another 500 ETH is deposited.
   `_getDepositedNextReportAdjusted` detects `curNonce (N+1) != lastNonce (N)`,
   resets `depositedNextReport = 0 + 500 = 500`. `depositedPostReport = 1500`.
3. Frame N+2 begins. Oracle finally submits a report for **frame N**'s refSlot
   (retrospective catch-up).
4. `processClStateUpdate` runs:
   - `_getDepositedNextReportAdjusted`: `curNonce = N+2`,
     `lastNonce = N+1` → reset → `depositedNextReport = 0`.
   - `_setDepositedPostReport(0)` → **1500 ETH of un-CL-reflected deposits
     vanish from `internalEther`**.
5. Share rate drops by `1500 ETH / totalPooledEther`. stETH holders see a
   temporary balance decrease. The 1500 ETH is not lost — it is on the beacon
   chain as pending deposits — but the on-chain accounting temporarily
   undercounts it.
6. The next report (for frame N+1 or N+2) will include the pending deposits in
   `clPendingBalance`, restoring the share rate.

---

## Proof of Concept (Foundry — conceptual)

```solidity
// SPDX-License-Identifier: MIT
pragma solidity 0.8.25;

import "forge-std/Test.sol";

contract DepositAccountingPoC is Test {
    // 1. Deploy Lido, AccountingOracle, HashConsensus with 1-epoch frames
    // 2. Submit 32 ETH to Lido -> buffer increases
    // 3. StakingRouter.deposit -> _spendDepositableEther(32 ether)
    //    depositedPostReport = 32, depositedNextReport = 32, lastNonce = N
    // 4. Advance to frame N+1 (no report submitted)
    // 5. StakingRouter.deposit -> _spendDepositableEther(32 ether)
    //    depositedPostReport = 64, depositedNextReport = 32, lastNonce = N+1
    // 6. Advance to frame N+2
    // 7. Oracle submits report for frame N (catch-up)
    // 8. processClStateUpdate sets depositedPostReport = 0
    // 9. internalEther undercounts by 64 ETH until next report
    //
    // Assert: share rate drops immediately after step 8, recovers after step 9's
    // follow-up report.
}
```

---

## Impact

| Perspective | Assessment |
|---|---|
| **Fund loss** | None — ETH is on the beacon chain; accounting self-corrects on the next report. |
| **Market impact** | Temporary share-rate dip can be arbitraged (buy stETH at discount, wait for correction). If the dip is large (e.g., many deposits in the missed window), this could affect DeFi integrations that use stETH as collateral. |
| **Likelihood** | Low — requires oracle to skip a frame AND significant deposits in the interim. The Lido oracle has historically been reliable; single-frame skips are rare. |

**Overall severity: Low** — temporary, self-correcting, and requires an oracle
miss which is itself an exceptional event.

---

## 3-Perspective Audit

### 1. Correctness
The design invariant is `depositedPostReport = depositedNextReport` after
`processClStateUpdate`, which holds when the report is for the current frame.
When the report is for an earlier frame (multi-frame skip), the invariant
discards legitimate un-CL-reflected deposits. The correct reset should be:
```
depositedPostReport = (total deposits since report's refSlot)
                    = depositedPostReport_old - depositsReflectedInReport
```
rather than `depositedPostReport = depositedNextReport_adjusted`.

### 2. Security
A temporary share-rate dip creates a short arbitrage window. While the dip is
bounded by the deposit amount (typically small relative to TVL), a coordinated
attack that triggers an oracle miss (e.g., via network-level DoS of oracle
members) while front-running large deposits could amplify the distortion.

### 3. Code Quality
The `_getDepositedNextReportAdjusted` logic is subtle and tightly coupled to the
assumption that reports are always for the current frame. A comment in the code
documents the design ("whenever the nonce changes, we reset depositedNextReport
to zero") but does not address the multi-frame-skip edge case. Adding an
explicit assertion or a more robust accounting method would improve clarity.

---

## Recommended Fix

The most robust fix is to track the **report's refSlot** (not just the current
frame's refSlot) and compute the post-report `depositedPostReport` as:

```solidity
uint256 depositsSinceReportRefSlot = _getDepositedPostReport() - _getDepositedNextReportForRefSlot(_reportRefSlot);
_setDepositedPostReport(depositsSinceReportRefSlot);
```

Alternatively, enforce that `processClStateUpdate` can only be called for the
current frame's refSlot (revert if the report is stale), which would force the
oracle to always report for the latest frame.

---

## File Paths
- Vulnerable contract: `/home/z/lido/contracts/0.4.24/Lido.sol` (lines 791–810, 982–1006)
- Related: `/home/z/lido/contracts/0.8.9/Accounting.sol` `_snapshotPreReportState` (line ~105)
- Related: `/home/z/lido/contracts/0.8.9/oracle/AccountingOracle.sol` `_handleConsensusReportData`
