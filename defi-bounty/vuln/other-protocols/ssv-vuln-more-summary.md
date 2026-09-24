# Additional SSV Network Bug Audit — Beyond the Known vUnit Deviation Bug

**Task ID:** ssv-more-bugs
**Agent:** Opus
**Date:** 2025
**Scope:** Deep re-audit of SSV Network contracts for bugs beyond the confirmed CRITICAL vUnit deviation inflation on partial validator removal.

---

## Summary

| # | Bug | Area | Severity | File |
|---|---|---|---|---|
| 1 | SSV DAO `networkTotalEarningsSSV()` uint64 overflow — permanent DoS | DAO fee distribution / overflow | **CRITICAL** | `ProtocolLib.sol` L97-99 |
| 2 | ETH DAO `networkTotalEarnings()` uint64 cast overflow — protocol-wide DoS | DAO fee distribution / overflow | **HIGH** | `ProtocolLib.sol` L84-90 |
| 3 | SSV cluster EB snapshot not updated on validator removal — migration phantom deviation | Validator migration / state sync | **HIGH** | `SSVValidators.sol` L231-250 (SSV branch) |
| 4 | Migrate liquidated SSV cluster with 0 validators — phantom deviation + fee charging | Validator migration / cross-function | **MEDIUM** | `SSVClusters.sol` L259-344, `ClusterLib.sol` L285-297 |
| 5 | Operator fee permanently stuck at zero | Operator management | **MEDIUM** | `SSVOperators.sol` L38, L126-128 |
| 6 | Oracle quorum DoS — fixed `oracleCount` prevents recovery from dead slots | Governance / oracle liveness | **MEDIUM** | `SSVDAO.sol` L191 |
| 7 | `getBurnRateSSV` silent uint64 truncation | View function correctness | **LOW** | `SSVViews.sol` L362-363 |

**Total: 7 bugs** — 1 CRITICAL, 2 HIGH, 3 MEDIUM, 1 LOW

---

## Files Audited

- `contracts/modules/SSVClusters.sol`
- `contracts/modules/SSVOperators.sol`
- `contracts/modules/SSVDAO.sol`
- `contracts/modules/SSVValidators.sol`
- `contracts/modules/SSVStaking.sol`
- `contracts/modules/SSVViews.sol`
- `contracts/modules/SSVOperatorsWhitelist.sol`
- `contracts/libraries/ClusterLib.sol`
- `contracts/libraries/OperatorLib.sol`
- `contracts/libraries/ProtocolLib.sol`
- `contracts/libraries/ValidatorLib.sol`
- `contracts/libraries/CoreLib.sol`
- `contracts/libraries/SSVPackedLib.sol`
- `contracts/libraries/SSVCoreTypes.sol`
- `contracts/libraries/storage/SSVStorage.sol`
- `contracts/libraries/storage/SSVStorageProtocol.sol`
- `contracts/libraries/storage/SSVStorageEB.sol`
- `contracts/libraries/storage/SSVStorageStaking.sol`
- `contracts/SSVNetwork.sol`
- `contracts/SSVProxy.sol`
- `contracts/token/CSSVToken.sol`
- `contracts/abstract/SSVReentrancyGuard.sol`

---

## Detailed Reports

Each bug has a dedicated report file:

1. `/home/z/ssv-vuln-more-dao-overflow.md` — SSV DAO uint64 overflow (CRITICAL)
2. `/home/z/ssv-vuln-more-eth-dao-overflow.md` — ETH DAO uint64 cast overflow (HIGH)
3. `/home/z/ssv-vuln-more-ssv-removal-eb.md` — SSV cluster EB snapshot not updated on removal (HIGH)
4. `/home/z/ssv-vuln-more-migrate-zero-validator.md` — Migrate 0-validator cluster phantom deviation (MEDIUM)
5. `/home/z/ssv-vuln-more-operator-fee-stuck.md` — Operator fee stuck at zero (MEDIUM)
6. `/home/z/ssv-vuln-more-oracle-quorum-dos.md` — Oracle quorum DoS (MEDIUM)
7. `/home/z/ssv-vuln-more-burnrate-truncation.md` — getBurnRateSSV truncation (LOW)

---

## Key Findings by Area

### 1. Cluster Liquidation — Edge Cases & Race Conditions

- The `liquidate` function correctly checks `isLiquidatableWithEB` after
  updating operator snapshots. The `nonReentrant` guard prevents reentrancy.
- The auto-liquidation path in `_liquidateAfterEBUpdateIfNeeded` correctly
  decrements `ethValidatorCount` once and delegates to `_executeLiquidation`.
- **No new bugs found** in the liquidation path itself (the known vUnit
  deviation bug affects liquidation indirectly via stale vUnits).

### 2. Operator Management — Registration, Removal

- **Bug #5 (MEDIUM):** Operators registered with `fee = 0` are permanently
  stuck — `declareOperatorFee` unconditionally reverts with
  `FeeIncreaseNotAllowed` because both `operatorFee` and `operatorSSVFee` are
  zero. `reduceOperatorFee(0)` creates the same trap from a non-zero starting
  point.
- `removeOperator` correctly settles snapshots and resets state, but does NOT
  adjust `sp.daoTotalEthVUnits` or `sp.ethDaoValidatorCount` — the deviation
  from the removed operator's clusters remains in the DAO total until those
  clusters are liquidated or migrated. This is by design (clusters' vUnits
  are independent of operator removal), but it creates a temporary
  inconsistency between `daoTotalEthVUnits` and the sum of
  `operatorEthVUnits`.

### 3. DAO Fee Distribution — Precision, Rounding

- **Bug #1 (CRITICAL):** `networkTotalEarningsSSV()` performs all arithmetic
  in `uint64`. With realistic SSV fee (0.01–1 SSV/block) and validator counts
  (10k–100k), the product `blockDiff × fee × count` overflows `uint64` in
  hours to days of SSV-side inactivity. Once overflowed, ALL SSV operations
  revert permanently (no recovery path).
- **Bug #2 (HIGH):** `networkTotalEarnings()` uses `uint128` intermediates
  but casts to `uint64` via `_safeUint64`, which reverts on overflow. At
  current mainnet parameters, the threshold is ~2000 years, but a DAO fee
  increase can reduce it to minutes. `ethDaoBalance` also grows monotonically,
  eventually overflowing.
- **Bug #7 (LOW):** `getBurnRateSSV` silently truncates to `uint64`,
  returning wrong values for high-fee/high-validator-count clusters.

### 4. Validator Migration — SSV→ETH, State Sync

- **Bug #3 (HIGH):** `_bulkRemoveValidator`'s SSV branch does NOT update
  `ebSnapshot.vUnits` on validator removal (unlike the ETH branch which
  subtracts baseline). On migration, the stale vUnits produces phantom
  deviation = `removedValidators × BPS_DENOMINATOR`, causing the migrated
  ETH cluster to be permanently overcharged (up to 13× at the EB cap).
- **Bug #4 (MEDIUM):** Migrating a liquidated SSV cluster with 0 validators
  but stale `vUnits > 0` adds the FULL stale vUnits as phantom deviation to
  the DAO and operators. The cluster is then charged fees with 0 validators
  (because `getVUnits` returns the stale value). Three interacting defects:
  (A) SSV removal doesn't clean vUnits, (B) migration doesn't guard against
  0-validator clusters, (C) `getVUnits` returns stale values for
  0-validator clusters.

### 5. BLS Signature — Verification Edge Cases

- BLS signature verification is performed **off-chain**. The `sharesData` is
  emitted in events but never verified on-chain. **No on-chain bug.**

### 6. Upgradeable Proxy — Storage Slots

- The proxy uses unstructured storage (`keccak256(...) - 1`) for each storage
  struct. The slots are distinct and don't collide.
- `UUPSUpgradeable._authorizeUpgrade` is correctly guarded with `onlyOwner`.
- `_disableInitializers()` in the constructor prevents implementation
  reinitialization.
- **No bugs found.** The storage layout is clean.

### 7. Reentrancy — Operator Callbacks

- Most state-changing functions are `nonReentrant`. The guard uses a custom
  storage slot (`SSVStorageReentrancy`).
- `migrateClusterToETH` is NOT `nonReentrant`, but the only external call
  (`CoreLib.transferTokenBalance`) occurs after all state changes. The SSV
  token transfer could trigger a callback (if non-standard), but reentering
  `migrateClusterToETH` would fail (cluster version check) and other
  nonReentrant functions are protected.
- `onCSSVTransfer` is NOT `nonReentrant`, but `_syncFees` and `_settle` are
  idempotent. No profitable reentrancy identified.
- `deposit`, `registerValidator`, `removeValidator` are NOT `nonReentrant`
  but have no external calls before state changes.
- **No reentrancy bugs found.**

### 8. Integer Overflow — Share Math

- **Bug #1 and #2** (covered above) are the primary overflow concerns.
- `updateSnapshotSt` computes `blockDiff × fee × effectiveVUnits / BPS` in
  `uint128`, then casts to `uint64` via `_safeUint64`. Overflow is
  theoretically possible but requires extreme values.
- `networkTotalEarnings` (ETH) uses `uint128` intermediates but the
  `_safeUint64` cast is the bottleneck.
- The `PackedETH` and `PackedSSV` types are `uint64`-backed with checked
  arithmetic (`.add()`, `.sub()`), so overflows in balance updates revert.
- **No additional overflow bugs** beyond #1 and #2.

### 9. Governance — Vote Manipulation

- **Bug #6 (MEDIUM):** `oracleCount = defaultOracleIds.length` is always 4
  (fixed array). If 2+ oracle slots are dead (key loss, uninitialized),
  quorum (75%) is unreachable with the remaining ≤2 oracles. EB commit
  process is permanently DoS'd. No mechanism to reduce `oracleCount` or
  update `defaultOracleIds`.
- `commitRoot` correctly prevents double-voting (`hasVoted` mapping) and
  stale blocks (`blockNum <= latestCommittedBlock`).
- The frozen supply (`roundFrozenSupply`) is set by the first voter and
  reused for subsequent votes — consistent within a commitment key.
- **No vote-weight manipulation bugs** (all oracles have equal weight).

### 10. Cross-Function Interaction

- **Bug #4** is the primary cross-function bug: SSV removal (doesn't clean
  vUnits) → SSV liquidation (doesn't clean vUnits) → ETH migration (uses
  stale vUnits) → ETH fee charging (uses stale vUnits for 0-validator
  cluster).
- The `withdraw` function computes `clusterIndex` manually (without calling
  `updateClusterOperators`), leaving operator snapshots stale. This is
  correct — the next `updateSnapshotSt` covers the full period. No bug.
- The `reactivate` function's `hasDeviation` flag is computed once and is
  consistent throughout the loop (DAO totals aren't modified inside the loop).
- `_executeLiquidation`'s deviation accounting handles both `>` and `<` cases,
  but the `<` case is dead code (EB floor ensures `vUnits ≥ baseline`).

---

## Not a Bug (Investigated and Cleared)

| Item | Reason |
|---|---|
| `withdraw` manual index computation | Correct — next `updateSnapshotSt` covers the gap. |
| `deposit` on liquidated cluster | By design — allows depositing before reactivation. |
| `onCSSVTransfer` reentrancy | `_syncFees`/`_settle` are idempotent; no external callbacks. |
| Removed operator frozen index | Correctly included in `cumulativeIndex`; `idxOp` for removed ops = 0. |
| `reactivate` `hasDeviation` flag | Computed before DAO update; consistent throughout loop. |
| Storage slot collisions | All slots use distinct `keccak256` values; no collision. |
| `_executeLiquidation` `<` deviation branch | Dead code (EB floor ensures `vUnits ≥ baseline`), but harmless. |

---

## Recommended Priority

1. **Immediate:** Fix Bug #1 (SSV DAO overflow) — permanent DoS with no
   recovery path. Widen to `uint256` and add public `syncSSVEarnings()`.
2. **Immediate:** Fix Bug #3 (SSV removal EB) — add vUnits cleanup to SSV
   removal branch.
3. **High:** Fix Bug #2 (ETH DAO overflow) — widen `_safeUint64` path or use
   `uint256` throughout.
4. **High:** Fix Bug #4 (migrate 0-validator) — add guard in
   `migrateClusterToETH` and `getVUnits`.
5. **Medium:** Fix Bug #5 (operator fee stuck) — disallow `fee = 0` at
   registration or allow first increase from 0.
6. **Medium:** Fix Bug #6 (oracle quorum) — count active oracles dynamically
   or allow governance to update `defaultOracleIds`.
7. **Low:** Fix Bug #7 (getBurnRateSSV truncation) — use `uint256`.
