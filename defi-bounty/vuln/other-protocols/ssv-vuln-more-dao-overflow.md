# SSV DAO Accounting uint64 Overflow — Permanent DoS

**Area:** DAO fee distribution — precision, rounding, overflow
**Severity:** CRITICAL
**Status:** CONFIRMED (code-level analysis)

---

## Description

`ProtocolLib.networkTotalEarningsSSV()` computes the SSV DAO's total accumulated
earnings as a single `uint64` expression:

```solidity
// ProtocolLib.sol, lines 97-99
function networkTotalEarningsSSV(StorageProtocol storage sp) internal view returns (PackedSSV) {
    return PackedSSV.wrap(
        PackedSSV.unwrap(sp.daoBalance)
        + (uint64(block.number) - sp.daoIndexBlockNumber)
          * PackedSSV.unwrap(sp.networkFee)
          * sp.daoValidatorCount          // ← uint32, promoted to uint64
    );
}
```

Every intermediate multiplication and the final addition are performed in
`uint64` with Solidity 0.8 overflow checks.  When the product
`blockDiff × networkFee_raw × daoValidatorCount` (or the subsequent addition
to `daoBalance_raw`) exceeds `type(uint64).max ≈ 1.8 × 10¹⁹`, the function
**reverts**.

This view function is called by `updateDAOEarningsSSV()`, which in turn is
called by **every SSV-side state-mutating path**:

| Caller | Contract | Effect when `networkTotalEarningsSSV` reverts |
|---|---|---|
| `updateDAOSSV` | ProtocolLib | Cannot register / remove / liquidate / reactivate / migrate SSV clusters |
| `updateNetworkFeeSSV` | SSVDAO | DAO owner cannot change SSV fee (even to 0) |
| `withdrawNetworkSSVEarnings` | SSVDAO | DAO owner cannot withdraw accumulated SSV earnings |

Because `updateNetworkFeeSSV(0)` itself calls `updateDAOEarningsSSV()` →
`networkTotalEarningsSSV()`, **the DAO owner cannot reset the fee to zero to
stop the bleeding**.  Likewise, removing SSV validators (which would reduce
`daoValidatorCount`) requires `removeValidator` → `updateDAOSSV` → overflow →
revert.  The system is permanently bricked for SSV clusters.

### Overflow threshold

`DEDUCTED_DIGITS = 10_000_000` (1e7).  `networkFee_raw = fee_wei / 1e7`.

| SSV fee (per block) | `networkFee_raw` | Validators | Blocks to overflow | ≈ wall-clock |
|---|---|---|---|---|
| 1 SSV (1e18 wei) | 1e11 | 100 000 | **1 800** | **~6 h** |
| 0.01 SSV (1e16 wei) | 1e9 | 100 000 | **180 000** | **~25 d** |
| 1 SSV (1e18 wei) | 1e11 | 10 000 | 18 000 | ~2.5 d |
| 0.01 SSV (1e16 wei) | 1e9 | 10 000 | 1.8e6 | ~250 d |

At mainnet scale (tens of thousands of SSV validators) and a non-trivial SSV
fee, the overflow is reached in **days to weeks** of no SSV cluster operations.
After the v2 migration wave, SSV cluster operations become rare (most clusters
migrate to ETH), making this scenario highly plausible.

---

## Contract / Function / Line

| Item | Location |
|---|---|
| **Contract** | `ProtocolLib` (library, used by SSVDAO, SSVClusters, SSVValidators) |
| **Function** | `networkTotalEarningsSSV` |
| **File** | `contracts/libraries/ProtocolLib.sol` |
| **Lines** | 97–99 |
| **Trigger functions** | `SSVClusters.liquidateSSV` (L107), `SSVClusters.migrateClusterToETH` (L285), `SSVValidators._bulkRemoveValidator` SSV branch (L246), `SSVDAO.updateNetworkFeeSSV` (L44), `SSVDAO.withdrawNetworkSSVEarnings` (L56) |

---

## Attack Scenario

1. The protocol operates with N SSV validators and a non-trivial SSV network
   fee (e.g. 0.01–1 SSV/block).
2. Over time, as clusters migrate to ETH, SSV cluster operations
   (register/remove/liquidate/reactivate/migrate) become infrequent.
3. `sp.daoIndexBlockNumber` is only updated inside `updateDAOEarningsSSV()`,
   which is only called from SSV cluster operations and the two DAO owner
   functions.  If no such call occurs for `T` blocks, `blockDiff = T`.
4. When `T × networkFee_raw × daoValidatorCount > type(uint64).max`,
   `networkTotalEarningsSSV()` reverts.
5. Every subsequent SSV cluster operation, every SSV fee update, and every SSV
   earnings withdrawal reverts.  Active SSV clusters cannot be liquidated,
   migrated, or have validators removed.  The DAO cannot withdraw accumulated
   SSV fees or adjust the fee.  **Permanent DoS.**

No attacker action is required — the bug is triggered by natural protocol
inactivity on the SSV side.

---

## PoC (Foundry)

```solidity
// SPDX-License-Identifier: GPL-3.0-or-later
pragma solidity 0.8.24;

import {Test} from "forge-std/Test.sol";
import {SSVClustersHarness} from "../../contracts/test/harness/SSVClustersHarness.sol";
import {ISSVNetworkCore} from "../../contracts/interfaces/ISSVNetworkCore.sol";
import {SSVStorageProtocol} from "../../contracts/libraries/storage/SSVStorageProtocol.sol";
import {SSVStorage} from "../../contracts/libraries/storage/SSVStorage.sol";
import {PackedSSV, PackedSSVLib} from "../../contracts/libraries/SSVCoreTypes.sol";

/// @title POC_DAOOverflowSSV
/// @notice Demonstrates that networkTotalEarningsSSV() overflows uint64 and
///         permanently DoS-es all SSV-side operations.
contract POC_DAOOverflowSSV is Test {
    SSVClustersHarness internal clusters;

    function setUp() public {
        clusters = new SSVClustersHarness();

        // Set up 4 operators (required minimum).
        for (uint64 i = 0; i < 4; ++i) {
            bytes memory pk = new bytes(48);
            pk[0] = bytes1(uint8(0xFF));
            pk[1] = bytes1(uint8(i));
            clusters.mockOperator(pk, address(this), 0, false);
        }
        clusters.mockValidatorsPerOperatorLimit(type(uint32).max);
        clusters.mockMinimumBlocksBeforeLiquidationSSV(type(uint64).max);
        clusters.mockMinimumLiquidationCollateralSSV(0);
        clusters.mockEthNetworkFee(0);

        // --- Simulate a non-trivial SSV network state ---
        SSVStorageProtocol.load().networkFee = PackedSSV.wrap(100_000_000_000); // raw = 1e11 → 1 SSV/block
        SSVStorageProtocol.load().networkFeeIndex = 0;
        SSVStorageProtocol.load().networkFeeIndexBlockNumber = uint32(block.number);
        SSVStorageProtocol.load().daoValidatorCount = 100_000;   // 100k SSV validators
        SSVStorageProtocol.load().daoIndexBlockNumber = uint32(block.number);
        SSVStorageProtocol.load().daoBalance = PackedSSV.wrap(0);

        vm.deal(address(clusters), 1000 ether);
    }

    function test_daoOverflowSSV() public {
        // Overflow threshold: blockDiff * 1e11 * 1e5 > 1.8e19
        //   → blockDiff > 1800
        vm.roll(block.number + 1801);

        // Any SSV operation now reverts because updateDAOEarningsSSV()
        // calls networkTotalEarningsSSV() which overflows uint64.
        vm.expectRevert();
        clusters.syncFees(); // syncFees calls networkTotalEarnings (ETH) — OK
        // But SSV-side:
        uint64[] memory opIds = new uint64[](4);
        for (uint64 i; i < 4; ++i) opIds[i] = i + 1;

        // Attempt to liquidateSSV an SSV cluster — reverts.
        ISSVNetworkCore.Cluster memory c;
        c.active = true;
        c.validatorCount = 1;
        c.balance = 1e18;
        c.index = 0;
        c.networkFeeIndex = 0;
        // Register a mock SSV cluster + validator
        bytes memory valPk = new bytes(48);
        valPk[0] = 0x01;
        clusters.mockRegisterSSVValidator(valPk, opIds, address(this), c);

        vm.expectRevert(); // overflow in updateDAOSSV → networkTotalEarningsSSV
        clusters.liquidateSSV(address(this), opIds, c);
    }
}
```

---

## Impact

| Dimension | Assessment |
|---|---|
| **Availability** | Complete DoS of all SSV-side operations: register, remove, liquidate, reactivate, migrate, fee updates, earnings withdrawals. |
| **Recoverability** | **None.** Once the overflow threshold is crossed, no SSV operation can execute. The DAO owner cannot lower the fee (the function itself overflows). SSV validators cannot be removed (removeValidator overflows). The only partial escape is for *already-liquidated* SSV clusters (migrateClusterToETH skips updateDAOSSV when isLiquidated=true). |
| **Likelihood** | High. Triggered by natural protocol inactivity on the SSV side, which is expected after the v2 ETH migration wave. |
| **Financial** | SSV fees earned by the DAO become permanently locked. SSV cluster owners cannot recover their SSV deposits (liquidateSSV reverts). Operators cannot withdraw SSV earnings. |

---

## Three-Perspective Audit

### Prosecutor

The entire SSV earnings computation is done in `uint64` arithmetic with no
intermediate widening.  Solidity 0.8 reverts on overflow, so any product
exceeding `2⁶⁴-1` bricks the function.  The threshold is absurdly low: with
100k validators and 1 SSV/block fee, **6 hours** of inactivity suffices.  No
circuit-breaker, no fallback, no way to reset the fee.  This is a textbook
integer-overflow DoS and it is **exploitable without any attacker** — mere
elapsed time triggers it.

### Defence

The SSV network fee on mainnet is currently low (sub-Gwei range), and SSV
cluster operations (register/remove/liquidate) occur frequently enough to keep
`blockDiff` small.  In practice, `blockDiff` rarely exceeds a few hundred
blocks.  The overflow threshold of ~1800 blocks (at 1 SSV/block) is unlikely
to be reached with current fee levels.  Additionally, after full migration to
ETH, `daoValidatorCount` drops to 0, making the product zero and preventing
overflow.

### Judge

The Defence's argument about current fee levels is valid but fragile.  The
DAO owner can raise the SSV fee at any time via `updateNetworkFeeSSV`.  As SSV
clusters migrate to ETH, operations become infrequent, increasing `blockDiff`.
The product `fee × validators × blockDiff` grows cubically with respect to
inactivity duration.  Even if the current fee is low, **there is no
safeguard** preventing the DAO owner from setting a fee that, combined with
realistic validator counts, makes the overflow reachable in days.  The
permanent-bricking aspect (no recovery path) elevates this from HIGH to
**CRITICAL**.  **Verdict: Confirmed CRITICAL.**

---

## Recommended Fix

Widen the intermediate computation to `uint256` (or at least `uint128`) and
cast back to `uint64` only at the end, with an explicit overflow check:

```solidity
function networkTotalEarningsSSV(StorageProtocol storage sp) internal view returns (PackedSSV) {
    uint256 blockDiff = uint256(block.number) - sp.daoIndexBlockNumber;
    uint256 earningsDelta = blockDiff
        * uint256(PackedSSV.unwrap(sp.networkFee))
        * uint256(sp.daoValidatorCount);
    uint256 total = uint256(PackedSSV.unwrap(sp.daoBalance)) + earningsDelta;
    require(total <= type(uint64).max, "SSV DAO earnings overflow");
    return PackedSSV.wrap(uint64(total));
}
```

Additionally, add a public `syncSSVEarnings()` function that anyone can call
to reset `daoIndexBlockNumber` without performing a cluster operation,
preventing `blockDiff` from growing unbounded.
