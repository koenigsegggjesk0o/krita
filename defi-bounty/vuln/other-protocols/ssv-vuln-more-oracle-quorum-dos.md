# Oracle Quorum DoS — Fixed oracleCount Prevents Recovery from Dead Slots

**Area:** Governance — vote manipulation, oracle committee liveness
**Severity:** MEDIUM
**Status:** CONFIRMED (code-level analysis)

---

## Description

`SSVDAO.commitRoot` computes each oracle's voting weight using
`oracleCount = s.defaultOracleIds.length`, which is the **fixed array length**
(4 = `MAX_DELEGATION_SLOTS`), not the count of active oracles:

```solidity
// SSVDAO.sol, lines 191-207
uint256 oracleCount = s.defaultOracleIds.length;   // always 4
uint256 totalStaked = seb.roundFrozenSupply[commitmentKey];
if (totalStaked == 0) {
    uint256 rawSupply = ICSSVToken(CSSV_ADDRESS).totalSupply();
    if (rawSupply == 0) revert ZeroCSSVSupply();
    totalStaked = rawSupply - (rawSupply % oracleCount);
    if (totalStaked == 0) revert InsufficientCSSVSupply();
    seb.roundFrozenSupply[commitmentKey] = totalStaked;
}

uint256 weight = totalStaked / oracleCount;        // = totalStaked / 4
seb.rootCommitments[commitmentKey] += weight;

uint256 threshold = (totalStaked * s.quorumBps) / BPS_DENOMINATOR;
```

`defaultOracleIds` is a `uint32[4]` fixed-size array set at initialization.
It **cannot be modified after deployment** (no setter function exists).  If
one or more oracle slots are "dead" (key lost, or never assigned — ID = 0),
the array length remains 4, but only 3 (or fewer) oracles can actually vote.

With `quorumBps = 7500` (mainnet, 75%):

| Active oracles | Weight each | Total weight (all vote) | Threshold (75%) | Quorum reached? |
|---|---|---|---|---|
| 4 | `T/4` | `T` | `0.75T` | ✅ |
| 3 | `T/4` | `0.75T` | `0.75T` | ✅ (barely) |
| 2 | `T/4` | `0.5T` | `0.75T` | ❌ |
| 1 | `T/4` | `0.25T` | `0.75T` | ❌ |

If **2 or more** oracle slots are dead, the EB root commit process is
permanently DoS'd.  No new EB roots can be committed, so
`updateClusterBalance` (which requires `ctx.blockNum == seb.latestCommittedBlock`)
ceases to function.  Clusters cannot update their effective balance, which
means:

- Clusters that should be auto-liquidated (EB dropped → liquidatable) cannot
  be liquidated via `updateClusterBalance`.
- Clusters that should have their fees reduced (EB dropped → lower vUnits →
  lower fees) continue paying inflated fees.

---

## Contract / Function / Line

| Item | Location |
|---|---|
| **Contract** | `SSVDAO` |
| **Function** | `commitRoot` |
| **File** | `contracts/modules/SSVDAO.sol` |
| **Lines** | 191 (`oracleCount = s.defaultOracleIds.length`) |
| **Related** | `replaceOracle` (L226-249) — can replace oracle addresses but not the array length or IDs |
| **Config** | `defaultOracleIds` is `uint32[MAX_DELEGATION_SLOTS]` (`uint32[4]`) — `SSVStorageStaking.sol` L33 |

---

## Attack Scenario

### Scenario A — Key loss (operational)

1. The protocol starts with 4 oracles: IDs [1, 2, 3, 4].
2. Oracle 3's private key is lost (or the oracle operator goes offline
   permanently).
3. `replaceOracle(3, newOracle)` is called — but the new oracle is also
   later compromised or lost.
4. Now only 3 oracles (IDs 1, 2, 4) can vote.  With `quorumBps = 7500`,
   3 oracles barely reach quorum (`0.75T ≥ 0.75T`).
5. If one more oracle is lost, only 2 remain.  Quorum is unreachable
   (`0.5T < 0.75T`).  **Permanent EB commit DoS.**

### Scenario B — Uninitialized slot (deployment defect)

1. During initialization, `defaultOracleIds` is set to `[1, 2, 3, 0]` (the
   4th slot was never assigned — a deployment configuration error).
2. `oracleCount = 4` (array length), but oracle ID 0 doesn't exist
   (`s.oracles[0] = address(0)`, `s.oracleIdOf[address(0)] = 0` →
   `revert NotOracle()`).
3. Only 3 oracles can vote.  Same as Scenario A.

### Scenario C — Quorum rounding edge case

Even with `quorumBps = 7500` and 3 active oracles, the integer arithmetic
`threshold = (totalStaked * 7500) / 10000` can cause `3 * weight` to exactly
equal `threshold` (quorum reached) or fall 1 short (quorum not reached),
depending on `totalStaked`.  For example:

- `totalStaked = 40`, `weight = 10`, `3 * 10 = 30`,
  `threshold = 40 * 7500 / 10000 = 30` → `30 ≥ 30` ✅ (barely).
- `totalStaked = 41`, but `totalStaked` must be divisible by 4, so
  `totalStaked = 40` (rawSupply = 41 → `41 - 41%4 = 40`).  Always 30 ≥ 30.

With `quorumBps = 7501`:
- `threshold = 40 * 7501 / 10000 = 30` (integer division).
- `3 * 10 = 30 ≥ 30` ✅.
- But with `totalStaked = 400`: `threshold = 400 * 7501 / 10000 = 300`.
  `3 * 100 = 300 ≥ 300` ✅.
- With `totalStaked = 4000`: `threshold = 4000 * 7501 / 10000 = 3000`.
  `3 * 1000 = 3000 ≥ 3000` ✅.

Hmm, 7501 still passes with 3/4. Let me check 7600:
- `threshold = 40 * 7600 / 10000 = 30`. `3 * 10 = 30 ≥ 30` ✅.
- `threshold = 400 * 7600 / 10000 = 304`. `3 * 100 = 300 < 304` ❌!

So with `quorumBps = 7600` and `totalStaked = 400`, 3 oracles CANNOT reach
quorum.  The DAO owner can set `quorumBps` up to `BPS_DENOMINATOR = 10000`.

---

## PoC (Foundry)

```solidity
// SPDX-License-Identifier: GPL-3.0-or-later
pragma solidity 0.8.24;

import {Test} from "forge-std/Test.sol";
import {SSVDAOHarness} from "../../contracts/test/harness/SSVDAOHarness.sol";
import {SSVStorageStaking} from "../../contracts/libraries/storage/SSVStorageStaking.sol";

contract POC_OracleQuorumDoS is Test {
    SSVDAOHarness internal dao;
    address internal oracle1 = address(0x01);
    address internal oracle2 = address(0x02);
    address internal oracle3 = address(0x03);
    address internal oracle4 = address(0x04);

    function setUp() public {
        dao = new SSVDAOHarness(address(0)); // CSSV address placeholder
        SSVStorageStaking.load().quorumBps = 7500;
        // Initialize oracles
        dao.mockSetOracle(1, oracle1);
        dao.mockSetOracle(2, oracle2);
        dao.mockSetOracle(3, oracle3);
        dao.mockSetOracle(4, oracle4);
        dao.mockSetOracleIdOf(oracle1, 1);
        dao.mockSetOracleIdOf(oracle2, 2);
        dao.mockSetOracleIdOf(oracle3, 3);
        dao.mockSetOracleIdOf(oracle4, 4);
        // defaultOracleIds is [1,2,3,4] — set via initializer in production.
    }

    function test_quorumDoSWithTwoDeadOracles() public {
        // Simulate: oracles 3 and 4 are dead (keys lost).
        // Only oracles 1 and 2 can vote.
        // weight = totalStaked / 4 each.
        // With 2 votes: 2 * T/4 = 0.5T < 0.75T → quorum NOT reached.

        // Oracle 1 votes.
        vm.prank(oracle1);
        dao.commitRoot(bytes32(uint256(1)), 100);

        // Oracle 2 votes.
        vm.prank(oracle2);
        dao.commitRoot(bytes32(uint256(1)), 100);

        // Quorum not reached — ebRoots[100] is still bytes32(0).
        assertEq(dao.getCommittedRoot(100), bytes32(0), "BUG: 2 oracles can't reach quorum with 2 dead slots");
        // Even if oracles 3 and 4 were alive, they can't vote (keys lost).
        // The EB commit process is permanently DoS'd.
    }
}
```

---

## Impact

| Dimension | Assessment |
|---|---|
| **EB commit liveness** | If 2+ oracle slots are dead, no new EB roots can be committed. `updateClusterBalance` ceases to function (requires `ctx.blockNum == latestCommittedBlock`). |
| **Fee accuracy** | Clusters cannot update their EB, so they continue paying fees based on stale EB values. Over-charged clusters cannot reduce fees; under-charged clusters cannot be corrected. |
| **Auto-liquidation** | `updateClusterBalance`'s auto-liquidation path (`_liquidateAfterEBUpdateIfNeeded`) is disabled, so clusters that should be liquidated (EB dropped) remain active. |
| **Recoverability** | `replaceOracle` can replace dead oracle addresses, but only if the owner knows the new oracle's address. If 2+ slots are dead and replacements aren't available, the DoS is permanent. |
| **Likelihood** | Medium — key loss is a common operational risk, especially for multi-sig oracle setups. |

---

## Three-Perspective Audit

### Prosecutor

Using a fixed array length (`defaultOracleIds.length = 4`) instead of the
actual active oracle count is a design defect.  There is no mechanism to
reduce `oracleCount` when oracles become permanently unavailable.  Combined
with a 75% quorum threshold, losing 2 oracles bricks the EB commit process
irreversibly.  The `replaceOracle` function is insufficient — it requires a
known replacement address, which may not be available in a key-loss scenario.

### Defence

The 4-oracle, 75% quorum design is intentionally conservative to prevent
oracle collusion.  Key loss is an operational risk that the DAO owner is
expected to manage (via `replaceOracle` with backup keys).  The scenario
requires 2 simultaneous key losses, which is an extreme operational failure.
In practice, oracles use hardware security modules (HSMs) and multi-sig
wallets, making key loss rare.

### Judge

The Defence's argument about operational diligence is valid, but the code
should be resilient to operational failures.  A single `replaceOracle` call
can fix one dead oracle, but if the replacement also fails (or if 2 oracles
are lost simultaneously before replacement), the system is bricked with no
code-level recovery.  The fix is straightforward: count active oracles
dynamically, or allow the DAO owner to reduce `oracleCount` via governance.
**Verdict: Confirmed MEDIUM** — liveness risk under operational failure, no
code-level recovery for multi-oracle loss.

---

## Recommended Fix

**Option A — Count active oracles dynamically:**

```solidity
uint256 oracleCount;
for (uint256 i = 0; i < s.defaultOracleIds.length; ++i) {
    if (s.oracles[s.defaultOracleIds[i]] != address(0)) {
        oracleCount++;
    }
}
if (oracleCount == 0) revert NoActiveOracles();
```

**Option B — Allow governance to update `defaultOracleIds`:**

Add a `updateDefaultOracleIds(uint32[MAX_DELEGATION_SLOTS] memory ids)` function
callable by the DAO owner, so dead slots can be removed or replaced.

**Option C — Lower quorum threshold when oracles are lost:**

Automatically reduce `quorumBps` proportionally to the number of active
oracles (e.g., if 2 of 4 are dead, quorum = 75% × (2/4) = 37.5%).
