# Operator Fee Permanently Stuck at Zero

**Area:** Operator management — registration, fee declaration
**Severity:** MEDIUM
**Status:** CONFIRMED (code-level analysis)

---

## Description

`SSVOperators.registerOperator` explicitly allows registering with `fee = 0`:

```solidity
// SSVOperators.sol, lines 38-43
if (fee != 0 && fee < PackedETHLib.unpack(sp.minimumOperatorEthFee)) {
    revert FeeTooLow();
}
if (fee > PackedETHLib.unpack(sp.operatorMaxFee)) {
    revert FeeTooHigh();
}
```

When `fee = 0`, the first check is skipped (the `fee != 0 &&` guard).  The
operator is stored with `op.ethFee = PACKED_ETH_ZERO` and, critically,
`op.ethSnapshot.block = uint32(block.number)` (non-zero).  This means
`ensureETHDefaults()` — which would set a default fee — is never called for
this operator, because `ensureETHDefaults` only acts when
`ethSnapshot.block == 0`.

Later, if the operator attempts to raise the fee via `declareOperatorFee`:

```solidity
// SSVOperators.sol, lines 124-128
PackedSSV operatorSSVFee = s.operators[operatorId].fee;        // 0
PackedETH operatorFee   = s.operators[operatorId].ethFee;      // 0
PackedETH shrunkFee     = PackedETHLib.pack(fee);              // > 0

if (operatorFee.eq(shrunkFee)) {
    revert SameFeeChangeNotAllowed();
} else if (shrunkFee.raw() != 0 && operatorFee.raw() == 0 && operatorSSVFee.raw() == 0) {
    revert FeeIncreaseNotAllowed();   // ← ALWAYS HIT for zero-fee operators
}
```

Both `operatorFee` and `operatorSSVFee` are zero (the operator was registered
post-v2 with no SSV fee).  The condition
`shrunkFee.raw() != 0 && operatorFee.raw() == 0 && operatorSSVFee.raw() == 0`
is **always true**, so `declareOperatorFee` unconditionally reverts with
`FeeIncreaseNotAllowed`.

The operator is **permanently locked at zero fee**.  `reduceOperatorFee`
cannot help (it only allows lowering).  The only escape is `removeOperator`
+ `registerOperator` with a new ID, which forfeits the operator's on-chain
identity, validator count, whitelist, and accumulated snapshot index.

---

## Contract / Function / Line

| Item | Location |
|---|---|
| **Contract** | `SSVOperators` |
| **Functions** | `registerOperator` (L31-66), `declareOperatorFee` (L109-141) |
| **File** | `contracts/modules/SSVOperators.sol` |
| **Key lines** | L38 (fee=0 allowed), L58 (ethSnapshot.block set non-zero), L126-128 (FeeIncreaseNotAllowed) |
| **Related** | `OperatorLib.ensureETHDefaults` (L122-133) — skipped because block != 0 |

---

## Attack Scenario

1. A new operator calls `registerOperator(publicKey, 0, false)` — perhaps
   intending to start free and raise the fee later, or simply by mistake
   (the function does not revert).
2. The operator runs validators at zero cost for a period.
3. The operator decides to charge a fee and calls
   `declareOperatorFee(operatorId, 10000000)` (0.01 ETH/block).
4. The transaction reverts with `FeeIncreaseNotAllowed`.
5. The operator is stuck.  No combination of `declareOperatorFee`,
   `reduceOperatorFee`, `executeOperatorFee`, or `cancelDeclaredOperatorFee`
   can raise the fee above zero.
6. The only recovery: `removeOperator` (which pays out any earnings and
   resets state) → `registerOperator` with a new ID (losing all accumulated
   snapshot index, validator count, and on-chain reputation).

### Who is affected

- **Honest operators** who register with `fee = 0` (either intentionally or
  by mistake) and later wish to charge.
- **Operators testing on testnet** who register at 0 and want to raise.
- **Operators who `reduceOperatorFee` to 0** (this is allowed) — they are
  similarly stuck, because after the reduction `operatorFee = 0` and
  `operatorSSVFee = 0` (for post-v2 operators).

---

## PoC (Foundry)

```solidity
// SPDX-License-Identifier: GPL-3.0-or-later
pragma solidity 0.8.24;

import {Test} from "forge-std/Test.sol";
import {SSVOperatorsHarness} from "../../contracts/test/harness/SSVOperatorsHarness.sol";

contract POC_OperatorFeeStuckAtZero is Test {
    SSVOperatorsHarness internal ops;
    address internal opOwner = address(0x0FE);

    function setUp() public {
        ops = new SSVOperatorsHarness(block.timestamp);
        // Set DAO params
        ops.mockMinimumOperatorEthFee(1_000_000);     // 0.001 ETH min
        ops.mockOperatorMaxFee(1_000_000_000);        // 1 ETH max
        ops.mockOperatorFeeIncreaseLimit(1000);        // 10%
        ops.mockDeclareOperatorFeePeriod(100);
        ops.mockExecuteOperatorFeePeriod(100);
    }

    function test_feeStuckAtZero() public {
        bytes memory pk = new bytes(48);
        pk[0] = 0x42;

        // 1. Register with fee = 0 (allowed).
        vm.prank(opOwner);
        uint64 opId = ops.registerOperator(pk, 0, false);
        assertEq(ops.getOperatorEthFee(opId), 0, "fee is 0");

        // 2. Try to declare a non-zero fee — reverts.
        vm.prank(opOwner);
        vm.expectRevert(ISSVOperators.FeeIncreaseNotAllowed.selector);
        ops.declareOperatorFee(opId, 1_000_000); // 0.001 ETH

        // 3. Try reduceOperatorFee — can't reduce below 0.
        vm.prank(opOwner);
        vm.expectRevert(ISSVOperators.FeeIncreaseNotAllowed.selector);
        ops.reduceOperatorFee(opId, 0);

        // 4. Confirm: the operator is permanently stuck at 0.
        assertEq(ops.getOperatorEthFee(opId), 0, "still 0");
    }
}
```

> **Note:** The harness `SSVOperatorsHarness` would need to expose the mock
> setters and `getOperatorEthFee`.  The existing `SSVClustersHarness` already
> provides `mockSetOperatorFee` and `getOperatorEthFee` — the same pattern
> applies.

---

## Impact

| Dimension | Assessment |
|---|---|
| **Operator lock-in** | Operator cannot raise fee from 0. Must remove + re-register, losing snapshot index and ID. |
| **Economic** | Operator provides services for free indefinitely. If the operator has many validators, the lost revenue can be significant. |
| **Likelihood** | Medium. Registering at 0 is an easy mistake, and `reduceOperatorFee(0)` is explicitly allowed. |
| **Protocol integrity** | Low direct protocol impact, but operators stuck at 0 may exit (remove), reducing network capacity. |

---

## Three-Perspective Audit

### Prosecutor

`registerOperator` has an explicit `fee != 0 &&` guard that permits `fee = 0`.
This is an invitation to register at zero.  But `declareOperatorFee` then
blocks any increase from zero with an irreversible revert.  The two functions
are contradictory: one allows the state, the other makes it inescapable.
`reduceOperatorFee` allows reducing *to* zero, compounding the trap.  There is
no documented warning, no event, no fail-safe.

### Defence

Registering at `fee = 0` is unusual and not the intended usage.  Operators
should register with a non-zero fee from the start.  The `FeeIncreaseNotAllowed`
guard exists to prevent legacy operators (with no ETH fee) from arbitrarily
setting a high ETH fee without going through the increase-limit path.  The
edge case of a genuinely new operator at 0 is an acceptable trade-off.

### Judge

The Defence's argument would hold if `registerOperator` reverted on `fee = 0`.
It does not — it explicitly allows it.  Allowing a state that is then
impossible to escape is a design defect.  Furthermore, `reduceOperatorFee`
allows reaching `fee = 0` from a non-zero starting point, creating a second
path into the trap.  The fix is trivial (either disallow `fee = 0` at
registration, or allow the first `declareOperatorFee` from 0 to bypass the
`FeeIncreaseNotAllowed` check).  **Verdict: Confirmed MEDIUM** — operator
lock-in with no recovery path short of operator removal.

---

## Recommended Fix

**Option A — Disallow zero fee at registration:**

```solidity
function registerOperator(bytes calldata publicKey, uint256 fee, bool setPrivate)
    external override returns (uint64 id)
{
    if (fee < PackedETHLib.unpack(sp.minimumOperatorEthFee)) revert FeeTooLow();
    // ...
}
```

**Option B — Allow first increase from zero (remove the `operatorSSVFee.raw() == 0` guard or add an exception):**

```solidity
// In declareOperatorFee, replace the FeeIncreaseNotAllowed check:
if (shrunkFee.raw() != 0 && operatorFee.raw() == 0) {
    // Allow increase from 0 only if the operator has never had an SSV fee
    // and has an active ETH snapshot (i.e. was registered post-v2).
    // Apply the standard increase-limit check instead.
}
```

Option A is simpler and safer.
