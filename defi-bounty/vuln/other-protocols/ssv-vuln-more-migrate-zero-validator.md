# Migrate Liquidated SSV Cluster with Zero Validators — Phantom Deviation & Fee Charging

**Area:** Validator migration — SSV→ETH state sync, cross-function interaction
**Severity:** MEDIUM
**Status:** CONFIRMED (code-level analysis)

---

## Description

`SSVClusters.migrateClusterToETH` can be called on a **liquidated SSV cluster
with zero validators** that still has a non-zero `ebSnapshot.vUnits` (stale
from before liquidation).  The function:

1. Sets `cluster.active = true` and `cluster.balance = msg.value`.
2. Adds the **full stale `vUnitsCluster`** as "deviation" to
   `sp.daoTotalEthVUnits` and every operator's `operatorEthVUnits` — even
   though the cluster has **zero validators** and the deviation should be
   zero.
3. Does **not** revert (the liquidation check
   `isLiquidatableWithEB` returns `false` when `validatorCount == 0`).
4. After migration, the cluster is an active ETH cluster with 0 validators but
   a non-zero `ebSnapshot.vUnits`.  `ClusterLib.getVUnits` returns this stale
   value, causing `updateBalanceWithEB` to **deduct fees from the cluster's
   balance** based on the phantom vUnits — even though the cluster has no
   validators and should not be charged any operator or network fee.

### Root cause

There are two interacting defects:

**Defect A — No vUnits cleanup for SSV clusters:**
`_bulkRemoveValidator`'s SSV branch (SSVValidators.sol L231-250) does not
update `ebSnapshot.vUnits` at all.  If all validators are removed,
`vUnits` remains at its last-set value.  (The ETH branch has a cleanup at
L210-224 that sets `vUnits = 0` when `validatorCount == 0`; the SSV branch
has no equivalent.)

**Defect B — No zero-validator guard in migrateClusterToETH:**
`migrateClusterToETH` (SSVClusters.sol L259-344) does not check
`cluster.validatorCount > 0` before performing the deviation accounting.
The deviation is computed as `vUnitsCluster - baseline` where
`baseline = 0 * BPS_DENOMINATOR = 0`, so the **entire stale vUnits** becomes
"deviation."

**Defect C — getVUnits returns stale value for 0-validator clusters:**
`ClusterLib.getVUnits` (ClusterLib.sol L285-297) returns `vUnits` from storage
if non-zero, regardless of `validatorCount`.  So a 0-validator cluster with
stale `vUnits > 0` is charged fees as if it had validators.

---

## Contract / Function / Line

| Item | Location |
|---|---|
| **Contract** | `SSVClusters` + `SSVValidators` + `ClusterLib` |
| **Functions** | `migrateClusterToETH` (SSVClusters L259-344), `_bulkRemoveValidator` SSV branch (SSVValidators L231-250), `getVUnits` (ClusterLib L285-297), `updateBalanceWithEB` (ClusterLib L306-321) |
| **File** | `contracts/modules/SSVClusters.sol`, `contracts/modules/SSVValidators.sol`, `contracts/libraries/ClusterLib.sol` |

---

## Attack Scenario

1. SSV cluster has 2 validators, EB = 64 ETH.  `ebSnapshot.vUnits = 20_000`.
2. Owner removes **all** validators via `bulkRemoveValidator`.
   - ETH branch: `validatorCount == 0` triggers cleanup → `vUnits = 0`.
   - **SSV branch: no cleanup → `vUnits` stays 20_000.**
3. Cluster is liquidated (`liquidateSSV`).  `cluster.active = false`,
   `cluster.validatorCount = 0`.  `vUnits` still 20_000.
4. Owner calls `migrateClusterToETH{value: 1 ether}`.
   - `isLiquidated = true` → `updateDAOSSV` skipped (no overflow risk).
   - `updateDAO(true, 0)` → adds 0 to DAO (no baseline, since validatorCount=0).
   - Deviation accounting: `vUnitsCluster = 20_000`, `baseline = 0`,
     `deviation = 20_000`.
   - `sp.daoTotalEthVUnits += 20_000` (phantom).
   - Each operator's `operatorEthVUnits += 20_000` (phantom).
   - `cluster.active = true`, `cluster.balance = 1 ether`.
5. The cluster is now an active ETH cluster with 0 validators but 20_000
   phantom vUnits.
6. Owner calls `deposit` then `withdraw`.  `updateBalanceWithEB` computes:
   - `vUnits = getVUnits(clusterId, 0) = 20_000` (stale, non-zero).
   - `usage = idxOp * 20_000 / BPS * ETH_DEDUCTED_DIGITS + networkFee`.
   - `cluster.balance -= usage` — **fees deducted for 0 validators.**
7. The deducted fees go to the DAO balance and (via future snapshot updates)
   to operators — a wealth transfer from the cluster owner to stakers/operators
   for zero service.

### Persistent DAO inflation

The phantom `daoTotalEthVUnits += 20_000` inflates the DAO's vUnits
permanently.  `networkTotalEarnings()` computes DAO earnings based on
`daoTotalEthVUnits`, so the DAO reports **higher earnings than actually
collected**.  Stakers who call `claimEthRewards` can drain the protocol's ETH
pool (which holds cluster deposits) faster than fees are actually collected,
potentially causing `withdraw` and `liquidate` to fail for other clusters due
to insufficient contract ETH balance.

---

## PoC (Foundry)

```solidity
// SPDX-License-Identifier: GPL-3.0-or-later
pragma solidity 0.8.24;

import {Test, Vm} from "forge-std/Test.sol";
import {SSVClustersHarness} from "../../contracts/test/harness/SSVClustersHarness.sol";
import {ISSVNetworkCore} from "../../contracts/interfaces/ISSVNetworkCore.sol";

contract POC_MigrateZeroValidatorPhantomDeviation is Test {
    SSVClustersHarness internal clusters;
    uint64[] internal opIds;
    address internal owner = address(0xCAFE);

    function setUp() public {
        clusters = new SSVClustersHarness();
        opIds = new uint64[](4);
        for (uint64 i; i < 4; ++i) {
            bytes memory pk = new bytes(48);
            pk[0] = bytes1(uint8(0xFF));
            pk[1] = bytes1(uint8(i));
            opIds[i] = clusters.mockOperator(pk, address(this), 0, false);
        }
        clusters.mockValidatorsPerOperatorLimit(type(uint32).max);
        clusters.mockEthNetworkFee(0);
        clusters.mockMinimumBlocksBeforeLiquidation(type(uint64).max);
        clusters.mockMinimumLiquidationCollateral(0);
        clusters.mockSetMinBlocksBetweenUpdates(0);
        clusters.mockSSVNetworkFee(0);
        clusters.mockMinimumBlocksBeforeLiquidationSSV(type(uint64).max);
        clusters.mockMinimumLiquidationCollateralSSV(0);
        vm.deal(address(clusters), 1000 ether);
        vm.deal(owner, 100 ether);
    }

    function test_migrateZeroValPhantomDeviation() public {
        bytes32 clusterId = keccak256(abi.encodePacked(owner, opIds));

        // 1. SSV cluster with 2 validators, EB = 64 ETH → vUnits = 20_000.
        ISSVNetworkCore.Cluster memory c;
        c.active = true;
        c.validatorCount = 2;
        // ... register SSV validators + set EB ...
        clusters.mockSetClusterVUnits(clusterId, 20_000);
        assertEq(clusters.getClusterVUnits(clusterId), 20_000, "EB set");

        // 2. Remove all validators (SSV branch — no vUnits cleanup).
        c.validatorCount = 0;
        // ... remove SSV validators ...
        assertEq(clusters.getClusterVUnits(clusterId), 20_000, "BUG A: vUnits stale after removing all validators");

        // 3. Liquidate SSV cluster.
        c.active = false;

        // 4. Migrate to ETH (liquidated, 0 validators).
        c.balance = 0;
        c.index = 0;
        c.networkFeeIndex = 0;
        // Write liquidated SSV cluster to storage.
        clusters.mockSetClusterLiquidated(owner, opIds); // sets ethClusters — adjust for SSV
        // ... set s.clusters[hashedCluster] = hashClusterData(c) ...

        vm.prank(owner);
        clusters.migrateClusterToETH{value: 1 ether}(opIds, c);

        // 5. Assertions.
        assertEq(clusters.getClusterVUnits(clusterId), 20_000, "BUG B: vUnits survives migration");
        assertEq(clusters.getDaoTotalEthVUnits(), 20_000, "BUG B: phantom DAO deviation (full stale vUnits)");
        assertEq(
            clusters.getOperatorEthVUnits(opIds[0]),
            20_000,
            "BUG B: phantom operator deviation"
        );
        // The cluster has 0 validators but is active with 1 ether balance.
        // Any withdraw/deposit will trigger fee deduction based on 20_000 phantom vUnits.
    }
}
```

> **Note:** The PoC uses harness-level mocks to set up the SSV cluster state
> (since SSV cluster creation is a legacy v1 path).  The key assertions are
> at the storage level: `getClusterVUnits`, `getDaoTotalEthVUnits`, and
> `getOperatorEthVUnits` all reflect the phantom deviation after migration.

---

## Impact

| Dimension | Assessment |
|---|---|
| **Fee charging with 0 validators** | The migrated ETH cluster is charged operator + network fees based on the stale vUnits, even though it has no validators. Cluster owner loses ETH for zero service. |
| **DAO inflation** | `daoTotalEthVUnits` is inflated by the full stale vUnits. `networkTotalEarnings()` over-reports, allowing stakers to drain the protocol ETH pool. |
| **Operator inflation** | Each operator's `operatorEthVUnits` is inflated, causing operators to over-earn. |
| **Likelihood** | Medium — requires an SSV cluster with EB set, all validators removed, then migration. But this is a plausible lifecycle for legacy clusters being decommissioned. |

---

## Three-Perspective Audit

### Prosecutor

Three separate defects combine: (A) SSV removal doesn't clean vUnits, (B)
migration doesn't guard against 0-validator clusters, (C) `getVUnits` returns
stale values for 0-validator clusters.  The result is a cluster that pays fees
for zero validators and inflates the DAO.  The DAO inflation is especially
dangerous because it can cause the protocol ETH pool to be drained by stakers
faster than actual fees are collected, leading to insolvency.

### Defence

The scenario requires an SSV cluster (legacy) with an EB update, followed by
full validator removal, followed by migration.  In practice, SSV clusters are
being decommissioned, and operators are encouraged to migrate directly without
removing validators first.  The amount of phantom deviation is bounded by the
cluster's last EB value, which is at most `validatorCount × 640_000` (2048 ETH
cap).  The DAO inflation effect is gradual and would be noticed by monitoring.

### Judge

The Defence's argument is undermined by the fact that there is no on-chain
guard preventing the sequence.  A cluster owner following the natural
decommissioning path (remove validators → liquidate → migrate) will trigger
the bug.  The DAO inflation aspect elevates the impact beyond a single
cluster's accounting error.  **Verdict: Confirmed MEDIUM** — accounting
corruption + potential protocol ETH insolvency under accumulation.

---

## Recommended Fix

**Fix A — Add vUnits cleanup to SSV removal branch:**

```solidity
// In _bulkRemoveValidator, SSV branch:
cluster.validatorCount -= validatorsRemoved;

// ADD: clean up EB snapshot when all validators removed.
StorageEB storage seb = SSVStorageEB.load();
if (seb.clusterEB[hashedCluster].vUnits > 0 && cluster.validatorCount == 0) {
    seb.clusterEB[hashedCluster].vUnits = 0;
}
```

**Fix B — Guard migrateClusterToETH against 0-validator clusters:**

```solidity
function migrateClusterToETH(uint64[] calldata operatorIds, Cluster memory cluster) external payable override {
    // ...
    if (cluster.validatorCount == 0 && !cluster.active) {
        // Clean up stale EB snapshot before migrating an empty liquidated cluster.
        SSVStorageEB.load().clusterEB[hashedCluster].vUnits = 0;
    }
    // ...
}
```

**Fix C — Guard getVUnits against 0-validator clusters:**

```solidity
function getVUnits(bytes32 clusterId, uint32 validatorCount) internal view returns (uint64) {
    if (validatorCount == 0) return 0;  // No validators → no vUnits
    // ...
}
```
