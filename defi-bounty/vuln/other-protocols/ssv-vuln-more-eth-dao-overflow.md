# ETH DAO networkTotalEarnings uint64 Cast Overflow — Protocol-Wide DoS

**Area:** DAO fee distribution — precision, rounding, overflow
**Severity:** HIGH
**Status:** CONFIRMED (code-level analysis)

---

## Description

`ProtocolLib.networkTotalEarnings()` computes the ETH DAO's total accumulated
earnings.  The intermediate multiplication is done in `uint128`, but the
result is cast to `uint64` via `_safeUint64()`, which **reverts** if the value
exceeds `type(uint64).max`:

```solidity
// ProtocolLib.sol, lines 84-90
function networkTotalEarnings(StorageProtocol storage sp) internal view returns (PackedETH) {
    uint128 units = sp.daoTotalEthVUnits;
    uint128 idx = uint64(block.number) - sp.ethDaoIndexBlockNumber;

    uint128 earningsUnits = (idx * PackedETH.unwrap(sp.ethNetworkFee) * units) / BPS_DENOMINATOR;
    return sp.ethDaoBalance.add(PackedETH.wrap(_safeUint64(earningsUnits)));  // ← revert if > uint64.max
}
```

`_safeUint64`:

```solidity
// SSVCoreTypes.sol, lines 26-29
function _safeUint64(uint128 value) pure returns (uint64) {
    if (value > type(uint64).max) revert SafeCastOverflow();
    return uint64(value);
}
```

When `earningsUnits > type(uint64).max`, the function reverts with
`SafeCastOverflow`.  Additionally, the final
`sp.ethDaoBalance.add(PackedETH.wrap(...))` can itself overflow `uint64`
(the `PackedETH` type is `uint64`-backed and `.add()` uses checked arithmetic).

### Affected functions

`networkTotalEarnings()` is called by:

| Caller | Effect when `networkTotalEarnings` reverts |
|---|---|
| `ProtocolLib.updateDAOEarnings` | Cannot update ETH DAO balance |
| `ProtocolLib.updateDAO` | Cannot register / remove / liquidate / reactivate / migrate ETH clusters |
| `ProtocolLib.updateDAOEthVUnits` | Cannot update EB (updateClusterBalance reverts) |
| `SSVStaking._syncFees` | Cannot sync fees → `stake`, `requestUnstake`, `claimEthRewards`, `onCSSVTransfer`, `syncFees` all revert |
| `SSVViews.getNetworkEarnings` | View function returns wrong data or reverts |
| `SSVViews.previewClaimableEth` | View function reverts |
| `SSVViews.isLiquidatable` | View function reverts |

Once the overflow threshold is crossed, **the entire ETH side of the protocol
is bricked**: no cluster operations, no staking, no fee syncing, no
liquidations.

---

## Contract / Function / Line

| Item | Location |
|---|---|
| **Contract** | `ProtocolLib` (library) |
| **Function** | `networkTotalEarnings` |
| **File** | `contracts/libraries/ProtocolLib.sol` |
| **Lines** | 84-90 (computation + `_safeUint64` cast) |
| **Overflow guard** | `SSVCoreTypes.sol` L26-29 (`_safeUint64`) |

---

## Overflow Threshold

`earningsUnits = blockDiff × ethNetworkFee_raw × daoTotalEthVUnits / BPS_DENOMINATOR`

Overflow when `earningsUnits > type(uint64).max ≈ 1.8 × 10¹⁹`.

Mainnet parameters (from `deployments/mainnet/config.json`):
- `networkFeeEth = 3_366_600_000` wei → `ethNetworkFee_raw = 33_666`
- `daoTotalEthVUnits` scales with total validators × vUnits per validator

| Validators | Avg EB/val | `daoTotalEthVUnits` | Blocks to overflow | ≈ wall-clock |
|---|---|---|---|---|
| 100 000 | 32 ETH | 1 × 10⁹ | 5.4 × 10⁹ | ~2 000 yr |
| 100 000 | 2048 ETH | 6.4 × 10¹⁰ | 8.4 × 10⁷ | ~32 yr |
| 1 000 000 | 2048 ETH | 6.4 × 10¹¹ | 8.4 × 10⁶ | ~3.2 yr |
| 1 000 000 | 32 ETH | 1 × 10¹⁰ | 5.4 × 10⁸ | ~200 yr |

With current mainnet fee (3.37 Gwei/block), the overflow is not reachable in
practice.  However:

1. **The DAO owner can raise `ethNetworkFee`** via `updateNetworkFee`.  If the
   fee is raised to 1 ETH/block (`raw = 1e13`), the threshold drops by
   ~300 000×:
   - 1M validators @ 2048 ETH: **28 blocks** (~5.5 minutes).
   - 100k validators @ 32 ETH: **180 blocks** (~36 minutes).

2. **`ethDaoBalance` accumulation**: Even if `earningsUnits` per sync is
   small, `ethDaoBalance` grows monotonically (only decreased by
   `claimEthRewards`).  After enough blocks, `ethDaoBalance_raw` alone can
   exceed `type(uint64).max`, causing the `.add()` to overflow regardless of
   `earningsUnits`.

   With 1M validators @ 32 ETH and 3.37 Gwei/block:
   - `ethDaoBalance` grows by `33_666 × 1e10 / 1e4 = 3.37e8` raw per block.
   - Overflow after `1.8e19 / 3.37e8 = 5.3e10` blocks (~2 000 years).
   - Not realistic at current fee, but with a 100× fee increase: ~20 years.

---

## Attack Scenario

### Scenario A — High-fee DoS

1. The DAO owner (or a compromised owner key) sets `ethNetworkFee` to a high
   value (e.g., 1 ETH/block) via `updateNetworkFee`.
2. With 100k validators @ 32 ETH, after **180 blocks** (~36 minutes),
   `networkTotalEarnings()` reverts.
3. All ETH cluster operations, staking, and fee syncing revert.
4. The DAO owner cannot lower the fee: `updateNetworkFee` calls
   `updateDAOEarnings` → `networkTotalEarnings` → revert.
5. **Permanent DoS** (same bricking pattern as the SSV overflow bug).

### Scenario B — Gradual balance accumulation

1. The protocol operates normally with a moderate fee.
2. Over years of operation, `ethDaoBalance_raw` grows (if stakers don't claim
   all rewards).
3. Eventually, `ethDaoBalance_raw + earningsUnits > type(uint64).max`.
4. `networkTotalEarnings()` reverts. Same DoS as Scenario A.

---

## PoC (Foundry)

```solidity
// SPDX-License-Identifier: GPL-3.0-or-later
pragma solidity 0.8.24;

import {Test} from "forge-std/Test.sol";
import {SSVClustersHarness} from "../../contracts/test/harness/SSVClustersHarness.sol";
import {SSVStorageProtocol} from "../../contracts/libraries/storage/SSVStorageProtocol.sol";
import {PackedETH} from "../../contracts/libraries/SSVCoreTypes.sol";

contract POC_ETHDAOOverflow is Test {
    SSVClustersHarness internal clusters;

    function setUp() public {
        clusters = new SSVClustersHarness();
        for (uint64 i; i < 4; ++i) {
            bytes memory pk = new bytes(48);
            pk[0] = bytes1(uint8(0xFF));
            pk[1] = bytes1(uint8(i));
            clusters.mockOperator(pk, address(this), 0, false);
        }
        clusters.mockValidatorsPerOperatorLimit(type(uint32).max);
        clusters.mockMinimumBlocksBeforeLiquidation(type(uint64).max);
        clusters.mockMinimumLiquidationCollateral(0);

        // Simulate high-fee + high-validator-count state.
        SSVStorageProtocol storage sp = SSVStorageProtocol.load();
        sp.ethNetworkFee = PackedETH.wrap(10_000_000_000_000); // raw = 1e13 → 1 ETH/block
        sp.ethNetworkFeeIndex = 0;
        sp.ethNetworkFeeIndexBlockNumber = uint32(block.number);
        sp.ethDaoValidatorCount = 100_000;
        sp.daoTotalEthVUnits = 1_000_000_000; // 100k validators × 10_000 vUnits each
        sp.ethDaoBalance = PackedETH.wrap(0);
        sp.ethDaoIndexBlockNumber = uint32(block.number);

        vm.deal(address(clusters), 1000 ether);
    }

    function test_ethDAOOverflow() public {
        // Overflow threshold:
        //   earningsUnits = blockDiff * 1e13 * 1e9 / 1e4 = blockDiff * 1e18
        //   > 1.8e19 → blockDiff > 180
        vm.roll(block.number + 181);

        // networkTotalEarnings() now reverts with SafeCastOverflow.
        // Any ETH operation that calls updateDAO → updateDAOEarnings →
        // networkTotalEarnings will revert.
        vm.expectRevert(); // SafeCastOverflow or arithmetic overflow
        clusters.syncFees();
    }
}
```

---

## Impact

| Dimension | Assessment |
|---|---|
| **Availability** | Complete DoS of all ETH-side operations: cluster register/remove/liquidate/reactivate/migrate/withdraw/updateClusterBalance, staking stake/unstake/claim/sync. |
| **Recoverability** | **None without upgrade.** The DAO owner cannot lower the fee (updateNetworkFee overflows). Stakers cannot claim (claimEthRewards overflows). The only recovery is a contract upgrade to widen the arithmetic. |
| **Likelihood** | Medium with current parameters (not reachable). High if the DAO owner raises the fee. The gradual-accumulation path is multi-year but unstoppable. |
| **Financial** | All ETH cluster deposits become inaccessible (can't withdraw or liquidate). Staking rewards are locked. |

---

## Three-Perspective Audit

### Prosecutor

The `_safeUint64` cast is a bomb waiting to detonate.  The DAO owner can
raise the fee to a level that triggers the overflow in minutes.  Even without
malice, the `ethDaoBalance` grows monotonically and will eventually overflow
given enough time.  There is no graceful degradation, no cap on `ethDaoBalance`,
no automatic fee reduction, and no recovery path short of a contract upgrade.

### Defence

At current mainnet parameters (3.37 Gwei/block, ~100k validators @ 32 ETH),
the overflow threshold is ~2000 years.  The scenario requires either a
compromised owner key setting an absurdly high fee, or thousands of years of
continuous operation.  The SSV staking module (`_syncFees`) is callable by
anyone, keeping `blockDiff` at 0-1 in practice, which means `earningsUnits`
per call is tiny.

### Judge

The Defence's argument holds for *current* parameters but ignores the DAO
owner's ability to raise fees.  A fee increase of 100× (still plausible — from
3.37 Gwei to 337 Gwei) reduces the threshold from 2000 years to 20 years.  A
malicious or compromised owner can brick the protocol in minutes.  The
monotonic growth of `ethDaoBalance` is a structural issue — there is no
mechanism to prevent it from approaching `uint64.max` over the protocol's
lifetime.  **Verdict: Confirmed HIGH** — not immediately exploitable at current
parameters, but structurally fragile with no recovery path.

---

## Recommended Fix

1. **Widen `networkTotalEarnings` to `uint256`:**

```solidity
function networkTotalEarnings(StorageProtocol storage sp) internal view returns (PackedETH) {
    uint256 units = sp.daoTotalEthVUnits;
    uint256 idx = uint256(block.number) - sp.ethDaoIndexBlockNumber;
    uint256 earningsUnits = (idx * uint256(PackedETH.unwrap(sp.ethNetworkFee)) * units) / BPS_DENOMINATOR;
    uint256 total = uint256(PackedETH.unwrap(sp.ethDaoBalance)) + earningsUnits;
    require(total <= type(uint64).max, "ETH DAO earnings overflow");
    return PackedETH.wrap(uint64(total));
}
```

2. **Add a public `syncEthEarnings()` function** that anyone can call to
   reset `ethDaoIndexBlockNumber` without performing a cluster operation,
   keeping `blockDiff` small.

3. **Cap `ethNetworkFee`** at a value that prevents overflow given the
   current `daoTotalEthVUnits` and a maximum expected `blockDiff`.
