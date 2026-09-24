# Lido Audit — Vulnerability Report: Integer Underflow in `NodeOperatorsRegistry.reportValidatorExitDelay`

## Metadata
- **Area**: Node Operator Registry — Validator Exit Delay Reporting
- **Contract**: `NodeOperatorsRegistry.sol` (Solidity **0.4.24** — no built-in overflow/underflow checks)
- **Functions**: `reportValidatorExitDelay` (line 1136), `isValidatorExitDelayPenaltyApplicable` (line 1115)
- **Severity**: Low
- **Impact**: Bypass of `exitPenaltyCutoffTimestamp` guard; false late-exit penalty reports

---

## Description

`NodeOperatorsRegistry` is compiled with Solidity 0.4.24, which does **not** revert on
integer underflow/overflow. The exit-delay penalty logic performs unchecked subtraction
of `_eligibleToExitInSec` from `_proofSlotTimestamp`:

```solidity
// contracts/0.4.24/nos/NodeOperatorsRegistry.sol:1136
function reportValidatorExitDelay(
    uint256 _nodeOperatorId,
    uint256 _proofSlotTimestamp,
    bytes _publicKey,
    uint256 _eligibleToExitInSec
) external {
    _auth(STAKING_ROUTER_ROLE);
    require(_publicKey.length == 48, "INVALID_PUBLIC_KEY");

    require(_eligibleToExitInSec >= _exitDeadlineThreshold(), "EXIT_DELAY_BELOW_THRESHOLD");
    require(_proofSlotTimestamp - _eligibleToExitInSec >= exitPenaltyCutoffTimestamp(),
            "TOO_LATE_FOR_EXIT_DELAY_REPORT");   // <--- UNDERFLOW HERE
    ...
}
```

The identical pattern appears in the view function:

```solidity
// line 1115
function isValidatorExitDelayPenaltyApplicable(...) external view returns (bool) {
    ...
    return _eligibleToExitInSec >= _exitDeadlineThreshold()
        && _proofSlotTimestamp - _eligibleToExitInSec >= exitPenaltyCutoffTimestamp();
}
```

If the oracle (via StakingRouter → SRLib._reportValidatorExitDelay) supplies a payload
where `_proofSlotTimestamp < _eligibleToExitInSec`, the subtraction wraps around to
`2^256 - (_eligibleToExitInSec - _proofSlotTimestamp)`, a value effectively guaranteed
to exceed `exitPenaltyCutoffTimestamp()`. The guard is therefore **silently bypassed**.

By contrast, the companion function `_setExitDeadlineThreshold` explicitly protects
against underflow (`require(block.timestamp >= _threshold + _lateReportingWindow)`),
which demonstrates the developers were aware of 0.4.24 arithmetic pitfalls — the
`reportValidatorExitDelay` path was missed.

---

## Attack Scenario

1. A compromised or buggy oracle/StakingRouter produces a payload with
   `_eligibleToExitInSec` set larger than `_proofSlotTimestamp`.
2. The underflowing subtraction yields `type(uint256).max - delta`, which is always
   `>= exitPenaltyCutoffTimestamp()`.
3. `reportValidatorExitDelay` marks the validator pubkey in
   `_validatorProcessedLateKeys` and emits `ValidatorExitStatusUpdated`.
4. Off-chain slashing/penalty infrastructure that consumes the event applies
   penalties to a node operator whose validator was not actually late (or was late
   only before the cutoff).

Although the on-chain effect is limited to an event + mapping write (no share
burning in the current implementation), the penalty signalling can trigger
off-chain reputation/fee adjustments and pollutes the
`_validatorProcessedLateKeys` mapping, permanently blocking legitimate future
reports for the same pubkey.

---

## Proof of Concept (Foundry)

```solidity
// SPDX-License-Identifier: UNLICENSED
pragma solidity 0.4.24;

import "forge-std/Test.sol";
import "../../contracts/0.4.24/nos/NodeOperatorsRegistry.sol";

contract ExitDelayUnderflowTest is Test {
    NodeOperatorsRegistry nor;
    address stakingRouter = address(0xBEEF);

    function setUp() public {
        // Deploy a minimal NOR harness (mock AragonApp + locator)
        // ... grant STAKING_ROUTER_ROLE to stakingRouter ...
        // ... initialize with threshold = 3600, cutoff = now-7200 ...
    }

    function testUnderflowBypassesCutoff() public {
        // Eligible time exceeds proof slot timestamp -> underflow
        uint256 proofSlotTs   = 1000;
        uint256 eligibleInSec = 5000;            // > threshold (3600)
        // proofSlotTs - eligibleInSec underflows to ~2^256-4000

        vm.prank(stakingRouter);
        // Should revert with TOO_LATE_FOR_EXIT_DELAY_REPORT, but succeeds
        nor.reportValidatorExitDelay(
            0, proofSlotTs, bytes48("pubkey"), eligibleInSec
        );
        // Assertion: validator is now marked as late-processed
        assertTrue(nor.isValidatorExitingKeyReported(bytes("pubkey")));
    }
}
```

---

## Impact

| Perspective | Assessment |
|---|---|
| **Direct fund loss** | None — no shares burned/slashed on-chain. |
| **Indirect operational** | False penalty events can trigger off-chain slashing/penalty logic; `_validatorProcessedLateKeys` is permanently set, blocking legitimate future reports. |
| **Trust model** | Requires compromised oracle/StakingRouter OR a bug in oracle data construction; these are trusted roles, limiting exploitability. |

**Overall severity: Low** — the issue is real but bounded by the trusted oracle/StakingRouter trust assumption and the absence of on-chain fund movement.

---

## 3-Perspective Audit

### 1. Correctness
The subtraction `_proofSlotTimestamp - _eligibleToExitInSec` is mathematically valid
only when `_proofSlotTimestamp >= _eligibleToExitInSec`. In Solidity 0.4.24 there is no
revert on underflow, so the guard is non-functional when inputs are inverted. A
`require(_proofSlotTimestamp >= _eligibleToExitInSec)` precondition is missing.

### 2. Security
The `exitPenaltyCutoffTimestamp` is a safety mechanism to ensure validators that
were requested to exit *before* the cutoff are not retroactively penalised. Bypassing
it via underflow defeats this safety mechanism. While on-chain impact is limited to
event/mapping writes, the integrity of the penalty reporting channel is compromised.

### 3. Code Quality / Maintainability
The codebase inconsistently handles 0.4.24 arithmetic: `_setExitDeadlineThreshold`
has explicit underflow protection, while `reportValidatorExitDelay` does not. This
inconsistency suggests the guard was added during a later review of the setter but
not propagated to the consumer. Adding a simple `require(_proofSlotTimestamp >= _eligibleToExitInSec)`
or migrating the contract to 0.8.x would eliminate the class of bugs.

---

## Recommended Fix

```solidity
require(_proofSlotTimestamp >= _eligibleToExitInSec, "PROOF_SLOT_BEFORE_ELIGIBILITY");
require(_proofSlotTimestamp - _eligibleToExitInSec >= exitPenaltyCutoffTimestamp(),
        "TOO_LATE_FOR_EXIT_DELAY_REPORT");
```

Apply the same precondition to `isValidatorExitDelayPenaltyApplicable`.

---

## File Paths
- Vulnerable contract: `/home/z/lido/contracts/0.4.24/nos/NodeOperatorsRegistry.sol` (lines 1115–1170)
- Related: `/home/z/lido/contracts/0.8.25/sr/SRLib.sol` `_reportValidatorExitDelay` (line 558)
