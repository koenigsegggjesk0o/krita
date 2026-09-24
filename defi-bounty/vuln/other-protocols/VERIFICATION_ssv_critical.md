# SSV Network Critical Bug — Independent Verification Report

**Task ID:** ssv-verify-critical
**Agent:** Opus (verifier)
**Date:** 2026-09-24
**Source vuln file:** `/home/z/fkr-step1/defi-bounty/vuln/other-protocols/ssv-vuln-validator-removal-deviation.md`
**Source contract:** `/home/z/ssv/contracts/modules/SSVValidators.sol` (function `_bulkRemoveValidator`, L197–228)
**PoC test:** `/home/z/ssv/test/foundry/POC_PartialRemovalDeviationInflation.t.sol`

---

## 0. Verification Result (TL;DR)

| Field | Value |
|---|---|
| **Verdict** | ✅ **CONFIRMED** |
| **PoC status** | ✅ **PASSES** (2/2 tests, 100%) |
| **Severity** | **CRITICAL** (per Immunefi SSV scope: "Direct theft of any user funds") |
| **Submission recommendation** | **Submit** to SSV Immunefi program (after internal legal review — see §6) |
| **Honest caveat** | The economic harm is real and deterministic but **bounded** by the cluster's remaining deposit; it is not unbounded minting. The original report slightly overstates the "permanence" — the accounting self-heals on the next oracle EB update, but the **already-paid excess is never refunded**, which is the actual harm. |

---

## 1. Bug Claim Restated

**Claim.** When an active ETH cluster with an explicit Effective-Balance (EB) snapshot above the 32 ETH floor removes **some but not all** of its validators via `removeValidator` / `bulkRemoveValidator`, the contract's `_bulkRemoveValidator` (ETH branch) only subtracts the **baseline** portion (`validatorsRemoved * BPS_DENOMINATOR = validatorsRemoved * 10_000`) from:

- `ebSnapshot.vUnits` (cluster-level), and
- `daoTotalEthVUnits` (DAO-level, via `updateDAO(false, validatorsRemoved)`)

…and **does not** scale down the **deviation** portion that is stored per-operator in `operatorEthVUnits[op]`. The deviation-cleanup branch is only entered when `cluster.validatorCount == 0` (full removal).

**Consequence.** After a partial removal, three bookkeeping values are inflated by `validatorsRemoved * perValidatorDeviation` where `perValidatorDeviation = (ebToVUnits(totalEB) / validatorCount) - BPS_DENOMINATOR > 0`:

| Storage slot | Correct post-removal | Actual (buggy) | Delta |
|---|---|---|---|
| `ebSnapshot.vUnits` | `newTotalEB * BPS / 32` | `oldVUnits - validatorsRemoved * BPS` | `+validatorsRemoved * perValidatorDeviation` |
| `operatorEthVUnits[op]` (each) | `newDeviation` | `oldDeviation` (unchanged) | `+validatorsRemoved * perValidatorDeviation` |
| `daoTotalEthVUnits` | `newClusterVUnits` | `oldDaoVUnits - validatorsRemoved * BPS` | `+validatorsRemoved * perValidatorDeviation` |

Because every fee calculation (operator fee accrual, network-fee accrual, liquidation threshold) is **linear in `vUnits`**, the cluster owner is debited at `k×` the correct rate, where `k = inflatedVUnits / correctVUnits`. The excess is credited to operators (via `operatorEthSnapshot.balance`) and to cSSV holders (via `ethDaoBalance`). The inflation persists across liquidation/reactivation cycles (the `ebSnapshot.vUnits` is never reset outside of `updateClusterBalance` / full-removal-cleanup), and the **already-paid excess is never refunded** even after the next oracle EB update snaps the bookkeeping back to truth.

---

## 2. Code Verification (Line-by-Line)

**File:** `/home/z/ssv/contracts/modules/SSVValidators.sol`
**Function:** `_bulkRemoveValidator` (L153–258)
**ETH branch:** L178–230

### 2.1 The buggy block (L197–228)

```solidity
197    {
198        // Deviation-only model: baseline removed via ethValidatorCount (already updated above)
199        // Do NOT subtract baseline from operatorEthVUnits
200        // Only handle deviation cleanup for explicit EB clusters
201        StorageEB storage seb = SSVStorageEB.load();
202        ClusterEBSnapshot storage ebSnapshot = seb.clusterEB[hashedCluster];
203        
204        if (ebSnapshot.vUnits > 0) {
205            // Cluster has explicit EB tracking - subtract baseline from snapshot
206            uint64 deltaClusterVUnits = uint64(validatorsRemoved) * BPS_DENOMINATOR;
207            ebSnapshot.vUnits -= deltaClusterVUnits;            // ← only baseline subtracted
208            
209            // When cluster becomes empty, clean up any remaining deviation
210            if (cluster.validatorCount == 0) {                  // ← GATE: full removal only
211                uint64 remainingVUnits = ebSnapshot.vUnits;
212                if (remainingVUnits > 0 && cluster.active) {
213                    // remainingVUnits is pure deviation (no baseline left since validatorCount=0)
214                    uint256 operatorsLength = operatorIds.length;
215                    for (uint256 i; i < operatorsLength; ++i) {
216                        if (s.operators[operatorIds[i]].ethSnapshot.block == 0) continue;
217                        seb.operatorEthVUnits[operatorIds[i]] -= remainingVUnits;   // ← deviation cleanup
218                    }
219                    StorageProtocol storage sp = SSVStorageProtocol.load();
220                    sp.updateDAOEthVUnits(remainingVUnits, 0);   // ← dao cleanup
221                }
222                ebSnapshot.vUnits = 0;
223            }
224        }
225        // For implicit clusters (ebSnapshot.vUnits == 0): nothing to do
226        // Baseline removal handled via ethValidatorCount decrement
227    }
```

### 2.2 Line-by-line analysis

| Line | What it does | Correct? |
|---|---|---|
| **L198–199 (comment)** | States the "deviation-only model" intent: baseline removed via `ethValidatorCount`, deviation untouched. | ⚠️ Misleading — the comment is correct **only when EB = 32 ETH** (deviation = 0). For EB > 32, deviation must also be scaled on removal. |
| **L201–202** | Loads EB storage and the cluster's `ClusterEBSnapshot`. | ✅ |
| **L204** | Gate: only runs for clusters with explicit EB tracking (`ebSnapshot.vUnits > 0`). | ✅ |
| **L206** | Computes `deltaClusterVUnits = validatorsRemoved * BPS_DENOMINATOR` — the **baseline** delta only. | ✅ for baseline; ❌ missing the deviation delta. |
| **L207** | `ebSnapshot.vUnits -= deltaClusterVUnits` — subtracts **only baseline** from the cluster snapshot. | ❌ **BUG** — should also subtract `validatorsRemoved * perValidatorDeviation`. |
| **L210** | `if (cluster.validatorCount == 0)` — gate for the deviation cleanup branch. | ❌ **BUG** — this gate is `true` only for **full** removal. For **partial** removal (`validatorCount > 0` after), the entire cleanup block is skipped. |
| **L211–223** | Full-removal deviation cleanup: subtracts `remainingVUnits` (= full remaining deviation) from each operator and the DAO, then zeros `ebSnapshot.vUnits`. | ✅ for full removal (the `BUG-4` test confirms this path works). |
| **L225–227** | For implicit clusters (`ebSnapshot.vUnits == 0`), does nothing — baseline is removed via `ethValidatorCount` decrement in `updateClusterOperators` (L182–188). | ✅ for implicit clusters (deviation is always 0 there). |

### 2.3 The missing code (what should be there)

For partial removal (`cluster.validatorCount > 0` after, `ebSnapshot.vUnits > 0` before), the function should compute and apply the **deviation delta**:

```solidity
// MISSING — should be inside the L204 `if` block, before/after L207:
if (cluster.validatorCount > 0) {  // partial removal
    uint64 baselineBefore = (uint64(cluster.validatorCount) + uint64(validatorsRemoved)) * BPS_DENOMINATOR;
    uint64 deviationBefore = ebSnapshot.vUnits - baselineBefore;  // vUnits before this call
    // ... or capture ebSnapshot.vUnits BEFORE the L207 subtraction
    uint64 newDeviation = deviationBefore * uint64(cluster.validatorCount)
                        / (uint64(cluster.validatorCount) + uint64(validatorsRemoved));
    uint64 deviationDelta = deviationBefore - newDeviation;
    // apply deviationDelta to each operator and DAO
}
```

### 2.4 Consumers of the inflated values (verified)

| Consumer | File:Function | How inflation flows | Verified? |
|---|---|---|---|
| Cluster fee charge | `ClusterLib.sol:updateBalanceWithEB` (L306–321) via `getVUnits` (L285–297) | `cluster.balance -= idxOp * inflatedVUnits / BPS` → debits too fast | ✅ `getVUnits` returns `ebSnapshot.vUnits` directly (L287, L296) |
| Operator earnings | `OperatorLib.sol:updateSnapshotSt` (L52–72) | `operator.ethSnapshot.balance += blockDiff * fee * inflatedEffectiveVUnits / BPS` | ✅ `effectiveVUnits = operatorEthVUnits[op] + ethValidatorCount * BPS` (L63–64) |
| DAO / staking pool | `ProtocolLib.sol:networkTotalEarnings` (L84–90) | `ethDaoBalance += blockDiff * networkFee * inflatedDaoVUnits / BPS` | ✅ uses `sp.daoTotalEthVUnits` directly (L85) |
| Liquidation threshold | `ClusterLib.sol:isLiquidatableWithEB` (L67–84) | `threshold = minBlocks * rate * inflatedVUnits / BPS` → higher threshold → easier to liquidate | ✅ uses `getVUnits` (L78) |
| Reactivation | `SSVClusters.sol:reactivate` (L129–181) | `clusterDeviation = ebSnapshot.vUnits - baseline` is inflated → re-injects phantom deviation | ✅ PoC step 6 confirms |

### 2.5 Secondary vector (SSV→ETH migration, NOT separately PoC'd but verified by reading)

The report's §2 also claims a secondary vector via `migrateClusterToETH` (`SSVClusters.sol` L304–327): for an SSV cluster that previously called `updateClusterBalance` (which sets `ebSnapshot.vUnits` for SSV clusters too, per `_updateClusterBalanceInternal` L412–414) and then removes validators, the SSV branch of `_bulkRemoveValidator` (L231–250) **does not touch `ebSnapshot.vUnits` at all**. The migration then imports the full stale `vUnitsCluster` as deviation.

**Verified by reading:** The SSV branch (L231–250) indeed only calls `updateClusterOperatorsSSV`, `updateBalanceSSV`, `updateDAOSSV`, and decrements `cluster.validatorCount` — it never touches `ebSnapshot.vUnits`. And `migrateClusterToETH` (L309–327) reads `ebSnapshot.vUnits` and computes `deviation = vUnitsCluster - baseline` without checking whether `vUnitsCluster` is stale relative to the current `validatorCount`. **This secondary vector is real but not separately PoC'd** — the primary ETH-branch PoC is sufficient to confirm the critical bug.

---

## 3. Math Verification

### 3.1 `ebToVUnits` (ClusterLib.sol L366–371)

```solidity
function ebToVUnits(uint32 effectiveBalance) internal pure returns (uint64) {
    uint256 vUnits = uint256(effectiveBalance) * BPS_DENOMINATOR;
    uint256 vUnitsPerValidator = DEFAULT_EB_PER_VALIDATOR / 1 ether;  // = 32
    return uint64(vUnits == 0 ? 0 : (vUnits - 1) / vUnitsPerValidator + 1);  // ceil division
}
```

With `BPS_DENOMINATOR = 10_000` and `DEFAULT_EB_PER_VALIDATOR = 32 ether`:

| `effectiveBalance` (ETH) | `ebToVUnits` | per-validator vUnits | per-validator deviation |
|---|---|---|---|
| 32 (1 val) | 10_000 | 10_000 | 0 |
| 64 (1 val) | 20_000 | 20_000 | 10_000 |
| 128 (2 vals) | 40_000 | 20_000 | 10_000 |
| 2048 (1 val) | 640_000 | 640_000 | 630_000 |
| 26_624 (13 vals × 2048) | 8_320_000 | 640_000 | 630_000 |

### 3.2 Core PoC scenario (2 vals @ 64 ETH, remove 1)

```
State before removal (2 vals @ 64 ETH, total EB = 128):
  ebSnapshot.vUnits         = ebToVUnits(128) = 40_000
  baseline (2 vals)         = 2 * 10_000      = 20_000
  deviation                 = 40_000 - 20_000 = 20_000
  operatorEthVUnits[op]     = 20_000  (each of 4 operators)
  daoTotalEthVUnits         = 40_000
  ethValidatorCount[op]     = 2       (each operator serves 2 validators)

removeValidator(1):
  ethValidatorCount[op]    -= 1   => 1          ✅ correct (baseline removal via operator count)
  daoTotalEthVUnits        -= 1*10_000 => 30_000 ❌ should be 20_000 (inflated by 10_000)
  ebSnapshot.vUnits        -= 1*10_000 => 30_000 ❌ should be 20_000 (inflated by 10_000)
  operatorEthVUnits[op]    unchanged => 20_000  ❌ should be 10_000 (inflated by 10_000)

Resulting effectiveVUnits[op] = 20_000 + 1*10_000 = 30_000  (should be 20_000)  → 1.5× fees
```

### 3.3 Amplification scenario (13 vals @ 2048 ETH cap, remove 12)

```
State before removal (13 vals @ 2048 ETH, total EB = 26_624):
  ebSnapshot.vUnits         = ebToVUnits(26_624) = 8_320_000
  baseline (13 vals)        = 130_000
  deviation                 = 8_190_000
  perValidatorDeviation     = 8_190_000 / 13 = 630_000

After removing 12 validators one-by-one (each is a partial removal):
  ebSnapshot.vUnits         = 8_320_000 - 12*10_000 = 8_200_000  ❌ should be 640_000
  operatorEthVUnits[op]     = 8_190_000              ❌ should be 630_000
  effectiveVUnits[op]       = 8_190_000 + 1*10_000 = 8_200_000  ❌ should be 640_000

fee multiplier = 8_200_000 / 640_000 = 12.8125×  → 1281%
```

---

## 4. PoC Code + Test Result

### 4.1 Test file

**Path:** `/home/z/ssv/test/foundry/POC_PartialRemovalDeviationInflation.t.sol`

The test uses the existing `SSVClustersHarness` (which exposes `mockOperator`, `mockSetEBRoot`, `getClusterVUnits`, `getOperatorEthVUnits`, `getDaoTotalEthVUnits`, `getEffectiveOperatorVUnits`). Cluster state is captured from emitted events via `vm.recordLogs()` + `abi.decode` of the trailing `Cluster` static tuple.

Two test functions:

1. **`test_partialRemovalInflatesVUnitsAndDeviation`** — the core PoC:
   - Registers 2 validators with 4 operators.
   - Oracle EB update to 128 ETH (64/validator) → establishes baseline + deviation.
   - Asserts the pre-removal state is correct (vUnits=40_000, deviation=20_000, dao=40_000, effective=40_000).
   - Removes 1 validator (partial removal).
   - Asserts all 6 bug claims:
     - BUG 1: `ebSnapshot.vUnits` = 30_000 (inflated by 10_000; should be 20_000)
     - BUG 2: `operatorEthVUnits[op]` = 20_000 for all 4 ops (NOT scaled; should be 10_000)
     - BUG 3: `daoTotalEthVUnits` = 30_000 (inflated by 10_000; should be 20_000)
     - BUG 4: `effectiveVUnits[op]` = 30_000 (1.5× correct 20_000)
     - fee multiplier = 150%
     - BUG 5: after self-liquidation, `ebSnapshot.vUnits` survives at 30_000 (DAO and op deviation correctly cleared to 0 by `_executeLiquidation`)
     - BUG 6: after reactivation, the phantom deviation is re-injected (dao=30_000, op deviation=20_000, effective=30_000)

2. **`test_amplification_13vals_2048ETHcap`** — the upper-bound amplification:
   - Registers 13 validators with 13 operators.
   - Oracle EB update to 26_624 ETH (2048/validator, the EB cap).
   - Removes 12 of 13 validators one-by-one (each is a partial removal).
   - Asserts: `ebSnapshot.vUnits` = 8_200_000 (should be 640_000), `operatorEthVUnits` = 8_190_000 (should be 630_000), `effectiveVUnits` = 8_200_000 (should be 640_000).
   - fee multiplier = **1281% (12.81×)**.

### 4.2 Test result (forge output)

```
$ forge test -vv
Compiling 1 files with Solc 0.8.24
Solc 0.8.24 finished in 24.18s
Compiler run successful with warnings.

Ran 2 tests for test/foundry/POC_PartialRemovalDeviationInflation.t.sol:POC_PartialRemovalDeviationInflation
[PASS] test_amplification_13vals_2048ETHcap() (gas: 10495161)
Logs:
  13-val @2048 fee multiplier (percent of correct): 1281

[PASS] test_partialRemovalInflatesVUnitsAndDeviation() (gas: 1567386)
Logs:
  fee multiplier (percent of correct): 150

Suite result: ok. 2 passed; 0 failed; 0 skipped; finished in 7.82ms
```

### 4.3 Key storage-slot return values (from `-vvvv` trace)

**Core PoC — after partial removal of 1 of 2 validators (EB=128):**

| Storage slot | Return value | Correct value | Bug? |
|---|---|---|---|
| `getClusterVUnits(clusterId)` | `30_000` | `20_000` | ❌ inflated +10_000 |
| `getOperatorEthVUnits(op[0..3])` | `20_000` (each) | `10_000` | ❌ NOT scaled |
| `getDaoTotalEthVUnits()` | `30_000` | `20_000` | ❌ inflated +10_000 |
| `getEffectiveOperatorVUnits(op[0])` | `30_000` | `20_000` | ❌ 1.5× |
| fee multiplier | `150%` | `100%` | ❌ +50% |

**Core PoC — after liquidation:**

| Storage slot | Return value | Notes |
|---|---|---|
| `getClusterVUnits(clusterId)` | `30_000` | ❌ BUG 5: survives liquidation |
| `getDaoTotalEthVUnits()` | `0` | ✅ cleared by `_executeLiquidation` |
| `getOperatorEthVUnits(op[0])` | `0` | ✅ cleared by `_executeLiquidation` |

**Core PoC — after reactivation:**

| Storage slot | Return value | Notes |
|---|---|---|
| `getClusterVUnits(clusterId)` | `30_000` | ❌ BUG 6: still inflated |
| `getDaoTotalEthVUnits()` | `30_000` | ❌ BUG 6: phantom deviation re-injected (10k baseline + 20k phantom dev) |
| `getOperatorEthVUnits(op[0])` | `20_000` | ❌ BUG 6: phantom deviation re-injected |
| `getEffectiveOperatorVUnits(op[0])` | `30_000` | ❌ BUG 6: still 1.5× |

**Amplification PoC — after removing 12 of 13 validators (EB=26_624, 2048/val):**

| Storage slot | Return value | Correct value | Multiplier |
|---|---|---|---|
| `getClusterVUnits(clusterId)` | `8_200_000` | `640_000` | 12.81× |
| `getOperatorEthVUnits(op[0])` | `8_190_000` | `630_000` | 13.0× |
| `getEffectiveOperatorVUnits(op[0])` | `8_200_000` | `640_000` | 12.81× |
| fee multiplier | `1281%` | `100%` | 12.81× |

---

## 5. Three-Perspective Re-Verification

### 🟢 Prosecutor (bug is real, exploitable, and meets the Critical bar)

1. **The PoC empirically confirms every claim.** All 6 bug assertions pass against the current `main` branch of the SSV repo. The 13-validator amplification confirms the 12.81× fee multiplier claimed in the report.
2. **The invariant is violated in substance, not just form.** The deviation-only model requires `effectiveVUnits(op serving cluster C) == clusterVUnits(C)` for the fee math to balance. The PoC shows this invariant is **preserved formally** (both sides are inflated to 30_000) but **wrong in substance** — the true vUnits for 1 validator @ 64 ETH is 20_000, not 30_000. The bookkeeping is internally consistent and externally wrong — the worst kind of accounting bug, because it does not revert.
3. **The trigger is mundane.** "Validator reaches EB > 32 ETH" is the expected state of every healthy mainnet validator within weeks of activation. "Cluster owner removes one of several validators" is a documented lifecycle operation (key rotation, partial exit, operator rebalancing).
4. **The excess is real ETH.** The fee calculation is linear in `vUnits` (confirmed by reading `ClusterLib.updateBalanceWithEB` L317–319 and `OperatorLib.updateSnapshotSt` L67–69). Inflated `vUnits` → inflated `cluster.balance` debit → inflated `operator.ethSnapshot.balance` credit + inflated `ethDaoBalance`. Operators can `withdrawOperatorEarnings` for the excess; cSSV holders can claim via the staking pool. The wealth transfer from cluster owner to operators + cSSV holders is real and deterministic.
5. **The excess is not refundable.** The next `updateClusterBalance` snaps `operatorEthVUnits` and `daoTotalEthVUnits` back to oracle truth via `_updateOperatorVUnits` and `updateDAOEthVUnits`, but it does **not** claw back the excess already credited to `operator.ethSnapshot.balance` or `ethDaoBalance`. The cluster owner's loss is permanent.
6. **The inflation is sticky.** It survives liquidation (BUG 5) and is re-injected on reactivation (BUG 6). The only correction is a fresh oracle EB update — and even then the already-paid excess is gone.
7. **No existing test covers this.** The repo's `removeValidator.test.ts` and `bulkRemoveValidator.test.ts` use single-validator clusters or full removals. The `bug4-double-deviation-liquidated.test.ts` covers the full-removal-from-liquidated-cluster case (the L210 gate). The partial-removal-from-active-EB>32 case is absent.

### 🔴 Defense (mitigating factors that lower duration/magnitude)

1. **The bug self-heals on the next oracle EB update.** `updateClusterBalance` → `_updateOperatorVUnits(storedVUnits, newVUnits)` + `updateDAOEthVUnits(storedVUnits, newVUnits)` snaps both `operatorEthVUnits` and `daoTotalEthVUnits` to oracle truth. The window of inflation is bounded by `minBlocksBetweenUpdates` + oracle cadence (typically hours on mainnet). **However, the already-paid excess is NOT refunded** — this is the actual harm, and it is permanent.
2. **The absolute theft is bounded by the cluster's own balance.** The cluster cannot be charged more than its remaining deposit; once it hits the liquidation threshold (which is also inflated, making liquidation *easier* — see `isLiquidatableWithEB`), the cluster is closed. So the worst case is "cluster owner loses their entire remaining deposit to fees", not "unbounded minting of ETH". This caps the per-cluster loss.
3. **The trigger requires oracle cooperation.** The oracle must actually report an EB above 32 for the cluster. If the oracle never does (e.g. EB is sticky at 32 in the deployed oracle config), the bug is unreachable. In practice, the oracle reports real beacon-chain EB, which exceeds 32 ETH for any healthy validator within weeks.
4. **Partial removals are less common than full exits in practice.** Most cluster owners either remove all validators (full exit) or none. The partial-removal flow is less exercised in production — but it IS a documented and supported operation, and the bug triggers deterministically whenever it occurs.
5. **The 12.81× amplification requires an extreme configuration** (13 validators, all at the 2048 ETH EB cap, 12 removed). Realistic amplifications for typical mainnet clusters (e.g. 4 validators @ 33–40 ETH, 1 removed) are closer to 1.03–1.25×. The harm is still real but smaller in magnitude.
6. **The secondary SSV→ETH migration vector** (report §2) is real by code reading but not separately PoC'd. It requires an SSV cluster to have called `updateClusterBalance` before migrating, which is a less common flow.

### ⚖️ Judge (verdict)

**The Defense's points 1, 2, 3, and 5 mitigate the duration and magnitude of the excess, but do not eliminate it.** The key facts the Prosecutor establishes — all empirically confirmed by the PoC:

- ✅ The excess is **real ETH** taken from the cluster owner and credited to operators + cSSV holders. (Defense concedes this.)
- ✅ The trigger is **normal protocol operation** (EB > 32 is the steady state of healthy validators; partial removal is documented). (Defense's point 5 is a frequency argument, not a correctness argument.)
- ✅ The self-healing on the next EB update does **not refund** the already-paid excess. (Defense concedes this in point 1.)
- ✅ The bug is **deterministic and silent** (no revert), so it will not be detected by monitoring until someone reconciles the books.
- ✅ The inflation **survives liquidation and is re-injected on reactivation** (PoC BUG 5 & 6), so the phantom vUnits persist across cluster lifecycle events.
- ✅ The 12.81× upper-bound amplification is **confirmed** by the PoC for the extreme configuration; realistic configurations see smaller (but still non-zero) multipliers.

**Honest caveats (not in the original report):**

- The original report's claim that the inflation is "permanent" is **slightly overstated** — the *bookkeeping* self-heals on the next oracle EB update. What is permanent is the **already-paid excess**, which is the actual harm. The report should be precise about this distinction.
- The original report's 12.8× amplification is **accurate** for the extreme configuration (PoC confirms 1281% = 12.81×), but this requires 13 validators all at the 2048 ETH EB cap with 12 removed — an unusual configuration. Typical mainnet harm is smaller (1.0×–1.3× for modest EB deviations).
- The original report does not mention that the cluster owner can **self-liquidate** to cap their loss (the `liquidate` function bypasses the `isLiquidatableWithEB` check when `msg.sender == clusterOwner`). This is a partial mitigation available to an informed victim, though it still results in the loss of their remaining deposit.

**Per the SSV Immunefi scope, "Direct theft of any user funds, whether at-rest or in-motion" is a Critical impact.** The cluster owner's deposited ETH is at-rest user funds; the contract debits it at an inflated rate and credits the excess to third parties (operators + cSSV holders). This meets the Critical bar.

**Verdict: CRITICAL — CONFIRMED.**

---

## 6. Submission Recommendation

### 6.1 Recommended remediation (for the submission's "Fix" section)

In `_bulkRemoveValidator` (ETH branch, `SSVValidators.sol` L197–228), when `ebSnapshot.vUnits > 0` and `cluster.validatorCount > 0` after the removal (partial removal), scale the deviation down proportionally:

```solidity
if (ebSnapshot.vUnits > 0) {
    uint64 validatorCountBefore = uint64(cluster.validatorCount) + uint64(validatorsRemoved);
    uint64 baselineBefore = validatorCountBefore * BPS_DENOMINATOR;
    uint64 deviationBefore = ebSnapshot.vUnits > baselineBefore
        ? ebSnapshot.vUnits - baselineBefore : 0;

    // Subtract baseline from snapshot (existing L206–207)
    uint64 deltaClusterVUnits = uint64(validatorsRemoved) * BPS_DENOMINATOR;
    ebSnapshot.vUnits -= deltaClusterVUnits;

    if (cluster.validatorCount == 0) {
        // ... existing full-removal cleanup (L210–224) ...
    } else if (deviationBefore > 0) {
        // NEW: partial-removal deviation scaling
        uint64 deviationAfter = deviationBefore
            * uint64(cluster.validatorCount) / validatorCountBefore;
        uint64 deviationDelta = deviationBefore - deviationAfter;
        if (deviationDelta > 0) {
            uint256 operatorsLength = operatorIds.length;
            for (uint256 i; i < operatorsLength; ++i) {
                if (s.operators[operatorIds[i]].ethSnapshot.block == 0) continue;
                seb.operatorEthVUnits[operatorIds[i]] -= deviationDelta;
            }
            sp.daoTotalEthVUnits -= deviationDelta;  // or via updateDAOEthVUnits
        }
    }
}
```

Apply the same proportional scaling in:
- The SSV branch of `_bulkRemoveValidator` (L231–250), which currently does not touch `ebSnapshot.vUnits` at all.
- `migrateClusterToETH` (`SSVClusters.sol` L304–327), which currently trusts the stale `ebSnapshot.vUnits` verbatim.

### 6.2 Submission readiness checklist

- [x] Bug claim verified by independent PoC (this report).
- [x] PoC test passes 2/2 against current `main` branch.
- [x] Line-by-line code verification with exact line numbers.
- [x] Math verification of all claimed values (40_000, 20_000, 30_000, 8_200_000, 12.81×).
- [x] 3-perspective audit (Prosecutor / Defense / Judge).
- [x] Remediation sketch provided.
- [ ] **Internal legal review** — confirm the SSV Immunefi program is active and the bug is in-scope (the vuln file header says "NOT SUBMITTED — for internal review only"; the verifier recommends submission *if* the program accepts this class of accounting-drift bugs as Critical).
- [ ] **Responsible disclosure** — coordinate with SSV team before any public disclosure; the PoC test file should NOT be pushed to a public repo until the fix is deployed.

### 6.3 Honesty caveat (for the submitter)

The original report's framing is **slightly more dramatic than the verified reality**:
- "Permanent inflation" → the *bookkeeping* self-heals on the next EB update; the *already-paid excess* is permanent. Be precise.
- "12.8× fee multiplier" → confirmed only for the extreme 13-validator-@-2048-ETH-cap configuration. Typical mainnet harm is 1.0×–1.3×. Lead with the realistic case; mention the extreme as the upper bound.
- "Direct theft" → the excess is real ETH, but it is bounded by the cluster's remaining deposit and is captured by *passive* beneficiaries (operators, cSSV holders) rather than an active attacker. The "theft" framing is defensible but the submitter should be prepared for the SSV team to argue it is "accounting drift that self-heals" — the rebuttal is that the already-paid excess does NOT self-heal.

---

## 7. Files Touched by This Verification

| File | Action |
|---|---|
| `/home/z/ssv/test/foundry/POC_PartialRemovalDeviationInflation.t.sol` | **Created** — Foundry PoC test (2 test functions, 6+8 bug assertions) |
| `/home/z/ssv/foundry.toml` | **Edited** — added `lib` to `libs`, added `forge-std` remapping, set `test = "test/foundry"` |
| `/home/z/ssv/lib/forge-std` | **Symlinked** → `/home/z/ens-poc/lib/forge-std` (forge-std was not previously installed in the SSV repo) |
| `/home/z/ssv/out/` | **Generated** — forge build artifacts (no source changes) |
| `/home/z/ssv/node_modules/` | **Installed** — `npm install` (OpenZeppelin contracts, etc.) |

**No production contract source files were modified.** The PoC test is isolated under `test/foundry/` and does not affect the existing Hardhat/Mocha test suite.

---

## 8. Final Verdict

**CONFIRMED — CRITICAL.**

The PoC empirically demonstrates that `_bulkRemoveValidator` (ETH branch, `SSVValidators.sol` L197–228) fails to scale down the per-operator deviation (`operatorEthVUnits`) and the DAO deviation (`daoTotalEthVUnits`) on partial validator removal from a cluster with `EB > 32 ETH`. The inflation is `validatorsRemoved * perValidatorDeviation`, persists across liquidation/reactivation cycles, and results in a fee multiplier of 1.5× (realistic 2-validator case) up to 12.81× (extreme 13-validator @ 2048 ETH cap case). The excess ETH is debited from the cluster owner's deposit and credited to operators + cSSV holders, and is not refundable even after the next oracle EB update snaps the bookkeeping back to truth.

**The original report's core claim is accurate.** The secondary SSV→ETH migration vector (report §2) is real by code reading but not separately PoC'd. The original report's "permanent inflation" framing is slightly overstated — the bookkeeping self-heals, but the already-paid excess is permanent, which is the actual harm.

**Submission recommendation: Submit** (after internal legal review confirms the SSV Immunefi program accepts this class of accounting-drift bugs as Critical, and after responsible-disclosure coordination with the SSV team).
