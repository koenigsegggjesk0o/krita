# SSV Network — High: Stale EB Snapshot Imported on SSV→ETH Cluster Migration After Validator Removal

**Severity: HIGH**
**Area: Validator cluster — SSV→ETH migration / EB deviation import**
**Bounty scope: SSV Network (0xDD9B…a4E1) — Immunefi, max $250,000**
**Auditor: Opus (deep audit, Sep 2026 baseline)**
**Status: NOT SUBMITTED — for internal review only**

---

## 1. Summary

`migrateClusterToETH` reads `ebSnapshot.vUnits` verbatim and re-derives the
**deviation** to inject into the ETH-side accounting (`daoTotalEthVUnits` and
each `operatorEthVUnits`). However, the SSV-branch `removeValidator` /
`bulkRemoveValidator` path **never updates `ebSnapshot.vUnits`** when validators
are removed from an SSV cluster that has previously called
`updateClusterBalance` to set an explicit EB.

Consequently, if an SSV cluster:

1. has ≥ 2 validators,
2. calls `updateClusterBalance` to set `ebSnapshot.vUnits = V` (V > baseline),
3. removes some validators (SSV path — `ebSnapshot.vUnits` is left stale at V),
4. migrates to ETH via `migrateClusterToETH`,

then the migration imports `deviation = V - newBaseline` — which is inflated
by the full per-validator `vUnits` of every removed validator (not just the
deviation share). The inflation is then live in the ETH accounting until the
next EB update corrects it, and the excess fees charged in the interim are
non-refundable.

This is the **same root cause** as the Critical finding in
`ssv-vuln-validator-removal-deviation.md` (the deviation-only model is not
scaled on validator removal) but manifests through a **different code path**
(SSV removal + migration vs. ETH partial removal) and produces a **larger**
inflation per removed validator (the entire `vUnitsPerValidator`, not just the
deviation share).

---

## 2. Affected Code

### The stale read

**File:** `contracts/modules/SSVClusters.sol`
**Function:** `migrateClusterToETH`
**Lines:** 304–327

```solidity
304    StorageEB storage seb = SSVStorageEB.load();
305    ClusterEBSnapshot storage ebSnapshot = seb.clusterEB[hashedCluster];
306
307    // Deviation-only model: baseline added via ethValidatorCount ...
308    // Only add deviation if cluster has explicit EB tracking
309    uint64 vUnitsCluster = ebSnapshot.vUnits;          // <-- STALE if SSV removals happened
310    if (vUnitsCluster > 0) {
311        uint64 baseline = uint64(cluster.validatorCount) * BPS_DENOMINATOR;
312        if (vUnitsCluster > baseline) {
313            uint64 deviation = vUnitsCluster - baseline; // <-- INFLATED
314            sp.daoTotalEthVUnits += deviation;            // <-- INFLATED
315            for (uint256 i; i < n; ++i) {
316                if (s.operators[operatorIds[i]].ethSnapshot.block == 0) continue;
317                seb.operatorEthVUnits[operatorIds[i]] += deviation; // <-- INFLATED
318            }
319        }
320    }
```

### The missing write

**File:** `contracts/modules/SSVValidators.sol`
**Function:** `_bulkRemoveValidator` — SSV branch
**Lines:** 231–250

```solidity
231    } else if (version == VERSION_SSV) {
232        if (cluster.active) {
233            ...
234            (uint64 clusterIndex, ) = OperatorLib.updateClusterOperatorsSSV(
235                operatorIds, false, validatorsRemoved, s, sp
236            );
237            uint64 currentNetworkFeeIndexSSV = sp.currentNetworkFeeIndexSSV();
238            cluster.updateBalanceSSV(clusterIndex, currentNetworkFeeIndexSSV);
239            cluster.index = clusterIndex;
240            cluster.networkFeeIndex = currentNetworkFeeIndexSSV;
241            sp.updateDAOSSV(false, validatorsRemoved);
242        }
243
244        cluster.validatorCount -= validatorsRemoved;
245        s.clusters[hashedCluster] = cluster.hashClusterData();
246        // *** NO ebSnapshot.vUnits adjustment — unlike the ETH branch ***
247    }
```

Contrast with the ETH branch (L197-228) which at least subtracts
`validatorsRemoved * BPS_DENOMINATOR` (baseline) from `ebSnapshot.vUnits`,
even though it also misses the deviation scaling (the Critical finding). The
SSV branch subtracts **nothing**.

---

## 3. Reproduction (arithmetic)

`BPS_DENOMINATOR = 10_000`, `ebToVUnits(EB) = ceil(EB * 10000 / 32)`.

| Step | `validatorCount` | `ebSnapshot.vUnits` | `daoTotalEthVUnits` (SSV) | Notes |
|---|---|---|---|---|
| Register 2 validators (SSV) | 2 | 0 | 0 (SSV DAO uses `daoValidatorCount`, not `daoTotalEthVUnits`) | |
| `updateClusterBalance(EB=128)` (SSV branch only sets snapshot) | 2 | 40_000 | 0 | SSV EB update does **not** touch ETH DAO/operator vUnits |
| `removeValidator(1)` (SSV path) | 1 | **40_000** (unchanged) | 0 | **BUG**: vUnits not scaled |
| `migrateClusterToETH` | 1 | 40_000 | 0 + (40_000 - 10_000) = **30_000** | `deviation = 30_000` imported |

Correct post-migration state for 1 validator @ 64 ETH:
`vUnits = 20_000`, `deviation = 10_000`, `daoTotalEthVUnits` contribution = `20_000`.

Actual (buggy): `vUnits = 40_000`, `deviation = 30_000`, `daoTotalEthVUnits` = `30_000`.

→ **3× fee multiplier** on the migrated cluster (vs. 1.5× for the ETH-path
variant), because the inflation includes the baseline share of the removed
validator, not just its deviation share.

---

## 4. Attack Scenario

1. Attacker registers as an SSV operator (or is chosen by a victim cluster
   that still runs the legacy SSV-fee path — these clusters must migrate to
   ETH eventually per the v2.0.0 roadmap).
2. Victim runs an SSV cluster with ≥ 2 validators.
3. Victim (or anyone permissionlessly) calls `updateClusterBalance` to set
   an EB snapshot above 32 ETH/validator. (SSV clusters support EB updates
   specifically to "prepare for future migration" per the code comment at
   `SSVClusters._updateClusterBalanceInternal` L412-414.)
4. Victim removes some validators from the SSV cluster (e.g. to exit one
   validator on the beacon chain). `ebSnapshot.vUnits` is left stale.
5. Victim migrates to ETH (`migrateClusterToETH`). The stale vUnits is
   imported as inflated deviation.
6. From this point on, the same economic impact as the Critical finding
   applies: cluster pays inflated fees, operators and cSSV stakers receive
   the excess.

The SSV→ETH migration is a **mandatory** step for all legacy clusters on
v2.0.0, so this path will be exercised by every remaining SSV cluster that
has validators with compounded EB.

---

## 5. Impact

Same class of impact as the Critical finding (direct theft of cluster-owner
funds via inflated fees), but:

* **Larger per-validator inflation** (full `vUnitsPerValidator` vs. just
  `perValidatorDeviation`).
* **Triggered by the mandatory migration flow**, so the attack surface is
  every SSV cluster that has not yet migrated.
* **Severity**: HIGH rather than Critical because the SSV-fee legacy path is
  being deprecated and the number of remaining SSV clusters is shrinking;
  however, every cluster that does migrate with a stale EB snapshot is
  affected.

---

## 6. Three-Perspective Audit

### 🟢 Prosecutor

1. The SSV `removeValidator` path is the **only** cluster-mutation function
   that does not adjust `ebSnapshot.vUnits`. Even the ETH branch (which has
   the Critical bug) at least subtracts the baseline. The SSV branch is a
   strict regression.
2. The migration code at L309 reads `ebSnapshot.vUnits` **without any
   validation** that it is consistent with `cluster.validatorCount`. The
   comment at L325 ("EB floor is 32 ETH, so vUnitsCluster >= baseline
   always") is **false** after a partial removal — `vUnitsCluster` can be
   far above the (reduced) baseline.
3. The migration is mandatory for SSV clusters. This is not an optional
   edge case; it is the documented upgrade path.
4. The inflation is larger than the ETH-path variant because the baseline
   share of removed validators is also leaked.

### 🔴 Defense

1. SSV clusters that never call `updateClusterBalance` have
   `ebSnapshot.vUnits = 0`, and the migration's `if (vUnitsCluster > 0)`
   guard skips the deviation import entirely. Only clusters that
   **opt-in** to pre-migration EB tracking are affected.
2. Pre-migration EB tracking is a new v2.0.0 feature; adoption may be low.
3. After migration, the next `updateClusterBalance` (ETH branch) corrects
   the vUnits, bounding the inflation window.
4. The SSV path is being deprecated, so the long-term exposure shrinks over
   time.

### ⚖️ Judge

The Defense correctly notes that the trigger requires the cluster to have
opted into pre-migration EB tracking, which narrows the population. However,
the opt-in is **encouraged by the protocol** (the SSV EB-update code path
exists specifically "preparing for future migration"), and the migration
itself is **mandatory**. A cluster owner following the recommended
migration playbook (set EB → remove exited validators → migrate) will hit
this bug.

The impact is the same class as the Critical finding (inflated fees, wealth
transfer to operators/stakers), with a larger per-validator magnitude.
Given the narrower (but non-trivial and protocol-encouraged) trigger, this
is **HIGH** — below Critical but well above Medium.

**Verdict: HIGH.** Recommended remediation: in the SSV branch of
`_bulkRemoveValidator`, mirror the ETH-branch EB cleanup (and apply the
same proportional deviation scaling recommended for the ETH branch).
Additionally, in `migrateClusterToETH`, defensively re-derive
`vUnitsCluster` from `cluster.validatorCount` and the last-known
per-validator EB rather than trusting the stale snapshot verbatim, or
require a fresh `updateClusterBalance` call immediately before migration.

---

## 7. Files Touched by This Report

* `contracts/modules/SSVValidators.sol` — `_bulkRemoveValidator` SSV branch
  (missing write)
* `contracts/modules/SSVClusters.sol` — `migrateClusterToETH` (stale read)
* `contracts/modules/SSVClusters.sol` — `_updateClusterBalanceInternal` SSV
  branch (sets the snapshot that later goes stale)

## 8. Do NOT submit

This report is for internal review only. Per the task instructions, no
submission to Immunefi or any external party should be made.
