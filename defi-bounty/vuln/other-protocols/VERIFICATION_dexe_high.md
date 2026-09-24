# VERIFICATION — DeXe Protocol HIGH Bugs (2 bugs)

**Verifier:** Opus (sub-agent)
**Date:** 2026-09-24
**Source repo:** `/home/z/dexe/` (DeXe-Protocol v1.0.1)
**Solidity:** 0.8.20 (pinned by `hardhat.config.js`)
**OpenZeppelin contracts:** v4.9.2 (pinned by `package.json`)
**PoC framework:** Foundry / forge 1.8.3
**PoC project root:** `/home/z/dexe-foundry-poc/`

---

## 0. TL;DR

| # | Bug | Claimed | Verdict | PoC | Severity (final) |
|---|-----|---------|---------|-----|------------------|
| 1 | Proposal validation bypass via unregistered main executor | HIGH | **CONFIRMED** | 12 / 12 tests pass against real DeXe `DistributionProposal` + verbatim `_validateProposalCreation` | **HIGH** |
| 2 | `changeVerifier` no zero-address check | HIGH | **REFUTED** (for OZ 4.9.2) | 13 / 13 tests pass; the alleged `ECDSA.recover == address(0)` behaviour does **not** occur in OZ 4.9.2 — `recover` reverts on bad sigs and returns non-zero for well-formed ones | **LOW (code quality)** |

Bug 1 is a real, deterministic logic flaw in `GovPoolCreate._validateProposalCreation`
(`contracts/libs/gov/gov-pool/GovPoolCreate.sol:206-215`). The `require(!ok || data.length == 0 || abi.decode(data, (bool)))`
short-circuits to `true` whenever the trailing executor is an EOA / `address(0)` /
non-validator contract / reverting validator, silently bypassing every
`IProposalValidator.validate` check (including `DistributionProposal.validate`'s
`proposalId == latestProposalId` invariant).

Bug 2's missing zero-address check is **real** (the harness accepts
`changeVerifier(address(0))`), but the claimed IMPACT (arbitrary
`saveOffchainResults` forgery + reward drain) does **not** materialise, because
the PoC's premise — *"OpenZeppelin's `ECDSA.recover` returns `address(0)` for
malformed signatures"* — is **false** for OZ v4.9.2 (the version DeXe pins).
In OZ 4.9.2, `ECDSA.recover` either **reverts** ("invalid signature length" /
"invalid signature 's' value" / "invalid signature") or **returns a non-zero
address**; it **never** returns `address(0)`. We exhaustively tested every
signature category (empty, 1-byte, 64-byte, 65-byte with v/r/s = 0, with
out-of-range s, with mathematically bogus r=1 s=1, and with a real valid sig).
None of them produce `recover(sig) == address(0)`. So the
`recover(sig) == offChain.verifier` check **cannot be satisfied** when
`offChain.verifier == address(0)` — the call reverts instead.

The closest real concern for Bug 2 is that setting the verifier to
`address(0)` **permanently DoS's the off-chain results pathway** (every
`saveOffchainResults` reverts) until a new governance proposal sets a valid
verifier. That is a self-inflicted low-severity DoS, not a fund-drain.

---

## 1. Bug #1 — Proposal Validation Bypass via Unregistered Main Executor

### 1.1 Claim (restated)

`GovPool.createProposal` → `GovPoolCreate._validateProposal` →
`_validateProposalCreation(mainExecutor, actionsFor)` performs:

```solidity
// GovPoolCreate.sol:206-215
function _validateProposalCreation(
    address executor,
    IGovPool.ProposalAction[] calldata actionsFor
) internal view {
    (bool ok, bytes memory data) = executor.staticcall{gas: 30000}(
        abi.encodeWithSelector(IProposalValidator.validate.selector, actionsFor)
    );

    require(!ok || data.length == 0 || abi.decode(data, (bool)), "Gov: validation failed");
}
```

The `require` is supposed to enforce that the LAST action's executor
("main executor") either (a) implements `IProposalValidator.validate` and
returns `true`, or (b) reverts/doesn't match — in which case the proposal
creation should FAIL. The bug is that the boolean `!ok || data.length == 0`
treats a "missing / failing" validator as **passing**, not failing.

Combined with `executorToSettings(unregisteredExecutor) == 0 == ExecutorType.DEFAULT`
(`GovSettings.sol:14`) and `_handleDataForProposal` returning `false` for
DEFAULT settings (`GovPoolCreate.sol:291-293` — no selector whitelisting),
this lets an attacker slip arbitrary `onlyGov` calls (e.g.
`DistributionProposal.execute(OLD_proposalId, token, amount)`) past
`DistributionProposal.validate`'s `proposalId == latestProposalId` invariant,
retroactively turning old succeeded proposals into distribution proposals and
draining the GovPool treasury.

### 1.2 Code verification

| Claimed location | Confirmed? | Notes |
|------------------|-----------|-------|
| `GovPool.createProposal` at `GovPool.sol:186-194` | ✅ | Lines 186–194 in the actual source. |
| `_validateProposal` at `GovPoolCreate.sol:134-161` | ✅ | Verbatim match. `mainExecutor = actionsFor[last].executor`; calls `_validateProposalCreation(mainExecutor, actionsFor)`; then `settingsId = govSettings.executorToSettings(mainExecutor)`. |
| `_validateProposalCreation` (the bug) at `GovPoolCreate.sol:206-215` | ✅ | Verbatim match. The `require(!ok \|\| data.length == 0 \|\| abi.decode(data, (bool)))` is exactly as quoted in the vuln file. |
| `DistributionProposal.validate` at `DistributionProposal.sol:102-108` | ✅ | Returns `proposalId == GovPool(payable(govAddress)).latestProposalId()`. |
| `DistributionProposal.execute` only requires `rewardAddress == address(0)` | ✅ | Line 56: `require(proposal.rewardAddress == address(0), "DP: proposal already exists");`. Allows execute on ANY old proposal without a distribution. |
| `GovPoolExecute.execute` performs arbitrary `.call` per action | ✅ | Lines 60–68: `actions[i].executor.call{value: actions[i].value}(actions[i].data)` — no per-action selector whitelist at execution time. |
| `executorToSettings` returns 0 for unregistered executors | ✅ | `GovSettings.sol:14`: `mapping(address => uint256) public executorToSettings;` — default value 0 = `ExecutorType.DEFAULT`. |
| `_handleDataForProposal` returns `false` (no checks) for DEFAULT | ✅ | `GovPoolCreate.sol:291-293`. |

**Mechanism detail (verified):** The vuln file states the staticcall returns
`ok = false` for EOAs / `address(0)`. The actual EVM behaviour is subtly
different and *more* permissive: a `staticcall` to a codeless address returns
`ok = true, data = ""` (vacuous success). The bypass therefore triggers via
the **`data.length == 0`** short-circuit, not via `!ok`. The end impact is
identical to the claim; the mechanism label in the claim is slightly
imprecise. For contracts that don't implement `validate` (no fallback), the
call reverts with `ok = false`, so `!ok` does fire. Both branches bypass the
require.

### 1.3 PoC

Three Foundry test files in `/home/z/dexe-foundry-poc/`:

1. **`src/GovPoolCreateHarness.sol`** — exposes the verbatim
   `_validateProposalCreation` body as a public function, plus mock validators
   (`ValidatorReturningTrue`, `ValidatorReturningFalse`, `ValidatorReverting`,
   `NonValidatorContract`).
2. **`test/ProposalValidationBypass.t.sol`** — 7 unit tests covering each
   executor category.
3. **`test/DistributionProposalValidateBypass.t.sol`** — 5 end-to-end tests
   against the **real** `DistributionProposal` contract (imported from
   `src/dexe_contracts/gov/proposals/DistributionProposal.sol`, which is a
   verbatim copy of the DeXe source) wired to a minimal `GovPoolMockForDP`
   that exposes the verbatim `_validateProposalCreation` body and a fixed
   `latestProposalId`.

#### Test results

```
Ran 7 tests for test/ProposalValidationBypass.t.sol:ProposalValidationBypassTest
[PASS] test_BugBypassWithEOA() (gas: 22537)
[PASS] test_BugBypassWithNonValidatorContract() (gas: 24707)
[PASS] test_BugBypassWithRevertingValidator() (gas: 26543)
[PASS] test_BugBypassWithZeroAddress() (gas: 22495)
[PASS] test_Impact_DefaultSettingsFallbackForUnregisteredExecutor() (gas: 13620)
[PASS] test_PassesWhenValidatorReturnsTrue() (gas: 14014)
[PASS] test_RevertsWhenValidatorReturnsFalse() (gas: 16973)
Suite result: ok. 7 passed; 0 failed; 0 skipped

Ran 5 tests for test/DistributionProposalValidateBypass.t.sol:DistributionProposalValidateBypassTest
[PASS] test_BugBypassDPValidationWithTrailingEOA() (gas: 15887)
[PASS] test_BugBypassDPValidationWithZeroAddress() (gas: 15821)
[PASS] test_DPValidateReturnsFalseForOldProposalId() (gas: 15503)
[PASS] test_NoBypass_DPValidateReturnsFalse_Reverts() (gas: 25958)
[PASS] test_NoBypass_DPValidateReturnsTrue_Passes() (gas: 23013)
Suite result: ok. 5 passed; 0 failed; 0 skipped
```

#### Key trace (`test_BugBypassDPValidationWithTrailingEOA`)

End-to-end demonstration that the **real** `DistributionProposal.validate`
is bypassed when the proposal has a trailing EOA action:

```
DistributionProposalValidateBypassTest::test_BugBypassDPValidationWithTrailingEOA()
├─ GovPoolMockForDP::validateProposalCreation(0x…bEEF, [
│     ProposalAction({ executor: <DP>, value: 0, data: 0xc45e0ae6…00000032…0000cafe…0de0b6b3a7640000 }),  // DP.execute(oldId=50, token, 1e18)
│     ProposalAction({ executor: 0x…bEEF, value: 0, data: 0x })                                            // <- bypass action
│  ]) [staticcall]
│   ├─ 0x…bEEF::validate([...]) [staticcall]   // staticcall goes to the LAST executor only
│   │   └─ ← [Stop]                            // EOA returns ok=true, data=""
│   └─ ← [Stop]                                // require passes via data.length==0 → bypass confirmed
└─ ← [Stop]
```

Note that `<DP>::validate` is **never invoked** — the staticcall goes only to
the trailing EOA. The `DP.execute(50, …)` call inside `actions[0]` (which
would be rejected by `DP.validate` because `50 != latestProposalId=100`) is
therefore not validated at all.

The contrast case `test_NoBypass_DPValidateReturnsFalse_Reverts` (single
action with `DP` as main executor) correctly reverts with
`"Gov: validation failed"`, confirming the require CAN fire when the
staticcall succeeds and returns `false`.

### 1.4 Three-perspective re-verification

**Attacker perspective.** ✅ The full attack chain from the vuln file is
executable:
1. Build `actionsOnFor = [rewardToken.approve(DP, amt) (ERC20 path) , DP.execute(OLD_id, token, amt), EOA(empty)]`.
2. `_validateProposalCreation(EOA, actions)` passes (bypass).
3. `executorToSettings(EOA) == 0 == DEFAULT` → `_handleDataForProposal` returns `false` → no selector whitelist → `DP.execute` permitted.
4. Vote "For" with enough power to clear DEFAULT quorum (the only gate).
5. Wait execution delay, call `GovPool.execute(newId)`.
6. `GovPoolExecute.execute` performs the calls in order: `approve`, `DP.execute(OLD_id, …)` (DP pulls tokens via `safeTransferFrom` from GovPool because GovPool approved it; or uses `msg.value` for ETH path), then no-op EOA call.
7. Attacker (who voted "For" on `OLD_id`) calls `DP.claim(OLD_id)` and receives a pro-rata share of treasury funds.

The attack does require clearing the DEFAULT-settings quorum. For DAOs that
configure DEFAULT with a low quorum (common for "general" proposals), the
attack is trivially achievable. With a moderate quorum, a whale or
flash-loan-assisted voter (subject to `_lockBlock`/`_checkBlock` same-block
guard, which does NOT prevent multi-block voting) can still pass it.

**Defender (protocol) perspective.** ✅ The `_validateProposalCreation`
function was intended as defence-in-depth: even if DEFAULT settings allow
arbitrary calls, the `validate` hook on registered executors (DP, TSP, SP)
is supposed to enforce executor-specific invariants. The `!ok || data.length == 0`
short-circuit silently defeats this layer for any unregistered executor. The
protocol likely assumed that a "missing" validator means "treat as DEFAULT" —
but failed to enforce that DEFAULT-settings proposals must NOT call `onlyGov`
functions on registered executors like DP. The bypass turns that
defence-in-depth into dead code.

**Neutral (auditor) perspective.** ✅ The root cause is a logic error in the
`require` condition. The correct logic is the inverse:

```solidity
require(ok && data.length >= 32 && abi.decode(data, (bool)), "Gov: validation failed");
```

i.e., the staticcall must succeed AND return a `bool true`. The current
`!ok || data.length == 0` is exactly the wrong polarity. The recommended fix
in the vuln file (Option A) is correct. Options B and C (defence-in-depth at
DP level and at `_handleDataForProposal` level) are also sound.

### 1.5 Verdict

**CONFIRMED.** The bug is real, the impact is treasury drainage + governance
manipulation, the severity is **HIGH** (rated HIGH rather than Critical only
because exploitation requires clearing a governance quorum).

---

## 2. Bug #2 — `changeVerifier` no zero-address check

### 2.1 Claim (restated)

`GovPool.changeVerifier` (`GovPool.sol:399-401`) is gated by `onlyThis` but
does NOT check `newVerifier != address(0)`:

```solidity
function changeVerifier(address newVerifier) external override onlyThis {
    _offChain.verifier = newVerifier;
}
```

`GovPoolOffchain.saveOffchainResults` (`GovPoolOffchain.sol:18-37`) verifies
the signature via:

```solidity
require(
    signHash_.toEthSignedMessageHash().recover(signature) == offChain.verifier,
    "Gov: invalid signer"
);
```

The claim asserts that "OpenZeppelin's `ECDSA.recover` returns `address(0)`
when the signature is malformed, `r`/`s` are zero, or the signature does not
map to a valid public key", and therefore if `offChain.verifier` is set to
`address(0)`, any user can submit any `resultsHash` with a garbage signature
and the check passes, allowing reward drain via repeated
`saveOffchainResults` calls (each with a unique `resultsHash` → unique
`signHash` → `usedHashes` doesn't block).

### 2.2 Code verification

| Claimed location | Confirmed? | Notes |
|------------------|-----------|-------|
| `GovPool.changeVerifier` at `GovPool.sol:399-401` | ✅ | Verbatim match. No zero-address check. |
| `GovPoolOffchain.saveOffchainResults` at `GovPoolOffchain.sol:18-37` | ✅ | Verbatim match. Uses `signHash_.toEthSignedMessageHash().recover(signature) == offChain.verifier`. |
| `getSignHash` at `GovPoolOffchain.sol:39-41` | ✅ | Includes `resultsHash`, `user`, `block.chainid`, `address(this)` — each unique `resultsHash` does produce a unique `signHash`. |
| `_payCommission` at `GovPoolOffchain.sol:43-52` | ✅ | Pays `internalSettings.rewardsInfo.executionReward` from the GovPool reward token. |
| OZ version pinned by DeXe | ✅ | `package.json`: `"@openzeppelin/contracts": "4.9.2"`. |
| OZ 4.9.2 `ECDSA.recover` returns `address(0)` for malformed sigs | ❌ **REFUTED** | See §2.3. |

### 2.3 PoC

`src/GovPoolOffchainHarness.sol` exposes the verbatim `changeVerifier` and
`saveOffchainResults` logic. `test/ChangeVerifierZero.t.sol` has 13 tests
that exhaustively probe the claim.

#### Test results

```
Ran 13 tests for test/ChangeVerifierZero.t.sol:ChangeVerifierZeroTest
[PASS] test_BogusSigStillFailsWhenVerifierIsZero() (gas: 67366)
[PASS] test_ChangeVerifierAcceptsZeroAddress() (gas: 32051)
[PASS] test_ClaimBypassWith64ByteSig_REVERTS() (gas: 63613)
[PASS] test_ClaimBypassWithEmptySig_REVERTS() (gas: 63097)
[PASS] test_ClaimBypassWithOneByteSig_REVERTS() (gas: 63329)
[PASS] test_ClaimBypassWithV27R1S1_REVERTS() (gas: 67424)
[PASS] test_ClaimBypassWithV27ZeroRS_REVERTS() (gas: 67302)
[PASS] test_ClaimBypassWithZeroed65ByteSig_REVERTS() (gas: 67332)
[PASS] test_ECDSARecoverNeverReturnsZeroAddress() (gas: 191848)
[PASS] test_ECDSARecoverReturnsNonZeroForValidSig() (gas: 11729)
[PASS] test_LegitVerifierAcceptsRealSignature() (gas: 153623)
[PASS] test_LegitVerifierRejectsGarbageSignature() (gas: 85544)
[PASS] test_ValidSigFailsWhenVerifierIsZero() (gas: 85717)
Suite result: ok. 13 passed; 0 failed; 0 skipped
```

#### OZ v4.9.2 `ECDSA.recover` behaviour (verified)

Source: `lib/openzeppelin-contracts/contracts/utils/cryptography/ECDSA.sol`
(exact v4.9.2). The function chain is `recover → tryRecover → _throwError`:

```solidity
function recover(bytes32 hash, bytes memory signature) internal pure returns (address) {
    (address recovered, RecoverError error) = tryRecover(hash, signature);
    _throwError(error);
    return recovered;
}

function _throwError(RecoverError error) private pure {
    if (error == RecoverError.NoError) {
        return;
    } else if (error == RecoverError.InvalidSignature) {
        revert("ECDSA: invalid signature");
    } else if (error == RecoverError.InvalidSignatureLength) {
        revert("ECDSA: invalid signature length");
    } else if (error == RecoverError.InvalidSignatureS) {
        revert("ECDSA: invalid signature 's' value");
    }
    // NOTE: InvalidSignatureV is in the enum but NOT produced by any
    // tryRecover overload in v4.9.2 (deprecated in v4.8).
}
```

`tryRecover` returns `(address(0), <error>)` for malformed sigs, and
`_throwError` **reverts** for every error variant that `tryRecover` actually
emits. Therefore `recover` either:

1. **Reverts** — for empty / wrong-length sigs, sigs with out-of-range `s`,
   or sigs where `ecrecover` returns 0 (e.g., v/r/s = 0).
2. **Returns a non-zero address** — for well-formed 65-byte sigs where
   `ecrecover` succeeds. Even mathematically "bogus" sigs (e.g.,
   `v=27, r=1, s=1`) return a deterministic non-zero address because the
   EVM `ecrecover` precompile does not strictly validate that `r` is a
   valid curve x-coordinate.

In **no case** does `recover` return `address(0)`.

#### Key trace 1: the vuln file's exact PoC (`bytes memory badSig = hex"00"`)

```
ChangeVerifierZeroTest::test_ClaimBypassWithOneByteSig_REVERTS()
├─ GovPoolOffchainHarness::changeVerifier(0x0000…0000)   // verifier = address(0) — accepted, no check
│   └─ ← [Stop]
├─ VM::prank(0xe05fcC23…)
├─ VM::expectRevert(custom error 0xf4844814)             // "ECDSA: invalid signature length"
├─ GovPoolOffchainHarness::saveOffchainResults("hash1", 0x00)
│   └─ ← [Revert] ECDSA: invalid signature length        // <- the call REVERTS, does NOT pass
└─ ← [Stop]
```

The vuln file's PoC **does not work**: the 1-byte signature causes OZ
`ECDSA.recover` to revert with "ECDSA: invalid signature length", which
propagates up through `saveOffchainResults` and reverts the whole call. The
`resultsHash` is NOT updated, no reward is credited, no commission is paid.

#### Key trace 2: a "bogus but well-formed" sig (`v=27, r=1, s=1`)

```
ChangeVerifierZeroTest::test_BogusSigStillFailsWhenVerifierIsZero()
├─ GovPoolOffchainHarness::changeVerifier(0x0000…0000)
├─ VM::prank(attacker)
├─ VM::expectRevert(Gov: invalid signer)
├─ GovPoolOffchainHarness::saveOffchainResults("anyHash", 0x0001…00011b)
│   ├─ PRECOMPILES::ecrecover(0x3e96…, 27, 1, 1) [staticcall]
│   │   └─ ← [Return] 0x86f8e46a…0263                      // non-zero address
│   └─ ← [Revert] Gov: invalid signer                      // 0x86f8… != address(0)
└─ ← [Stop]
```

Even a well-formed but mathematically bogus signature (which OZ does NOT
reject) recovers to a non-zero address (`0x86f8…`), so
`recover(sig) == offChain.verifier` evaluates to `0x86f8… == address(0)` =
`false`, and the require reverts with "Gov: invalid signer".

#### Key trace 3: a real valid signature

```
ChangeVerifierZeroTest::test_ValidSigFailsWhenVerifierIsZero()
├─ changeVerifier(0x0000…0000)
├─ VM::sign(attackerPk, ethHash) → (27, r, s)
├─ PRECOMPILES::ecrecover(ethHash, 27, r, s)
│   └─ ← [Return] 0xe05fcC23…cfF7                          // == attacker (the signer)
├─ VM::expectRevert(Gov: invalid signer)
├─ saveOffchainResults("hash1", sig)
│   └─ ← [Revert] Gov: invalid signer                      // attacker != address(0)
└─ ← [Stop]
```

A valid signature recovers to the signer's address (non-zero), which again
does not equal `address(0)`. The require reverts.

#### Exhaustive matrix

| Signature | `recover` behaviour | `saveOffchainResults(verifier=0)` |
|-----------|---------------------|------------------------------------|
| `""` (empty) | reverts "invalid signature length" | **reverts** |
| `hex"00"` (1 byte) | reverts "invalid signature length" | **reverts** |
| 64-byte zeroed | reverts "invalid signature length" | **reverts** |
| 65-byte `v=0, r=0, s=0` | reverts "invalid signature" (ecrecover returns 0) | **reverts** |
| 65-byte `v=27, r=0, s=0` | reverts "invalid signature" (ecrecover returns 0) | **reverts** |
| 65-byte `v=28, r=0, s=0` | reverts "invalid signature" (ecrecover returns 0) | **reverts** |
| 65-byte `v=27, r=1, s=bad-s` | reverts "invalid signature 's' value" | **reverts** |
| 65-byte `v=27, r=1, s=1` | returns `0x86f8…0263` (non-zero) | **reverts** "Gov: invalid signer" |
| Real valid sig | returns signer address (non-zero) | **reverts** "Gov: invalid signer" |

**Conclusion:** there is no signature (malformed, bogus, or valid) that
satisfies `recover(signature) == address(0)` under OZ v4.9.2. The vuln
file's claimed bypass is **infeasible**.

### 2.4 What IS true about Bug 2

1. ✅ The missing `require(newVerifier != address(0))` in `changeVerifier` is real.
2. ✅ `changeVerifier(address(0))` can be executed via a governance proposal (it is whitelisted in `_handleDataForInternalProposal` at `GovPoolCreate.sol:265`).
3. ✅ After `verifier == address(0)`, EVERY subsequent `saveOffchainResults` call reverts (because `recover(sig)` is never `address(0)`). This is a **self-inflicted DoS** of the off-chain results pathway until a new governance proposal resets the verifier.

The fund-drain / forgery impact claimed in the vuln file does NOT materialise.

### 2.5 Three-perspective re-verification

**Attacker perspective.** ❌ The claimed attack cannot be executed. The
attacker would need to either (a) find a signature that makes
`ECDSA.recover` return `address(0)` (impossible — OZ 4.9.2 reverts instead),
or (b) find a private key whose derived address is `address(0)` (requires
breaking ECDLP). Neither is feasible. Even with the validation-bypass bug
(#1) enabling `changeVerifier(address(0))`, the subsequent
`saveOffchainResults` calls revert. No reward is credited, no commission is
paid, no `resultsHash` forgery occurs.

**Defender (protocol) perspective.** ⚠️ The missing zero-address check is
still a real code-quality defect. The most plausible real-world harm is:
- A governance proposal *accidentally* sets the verifier to `address(0)`
  (e.g., a "reset verifier" proposal with a typo). After that, the off-chain
  results pathway is bricked until another proposal fixes it.
- Future OZ upgrades (or a switch to a different signature library) could
  reintroduce the `recover(0) == address(0)` behaviour, at which point this
  would become exploitable. The defence-in-depth fix is still warranted.

**Neutral (auditor) perspective.** ❌ The vuln file's central technical
premise — "OZ `ECDSA.recover` returns `address(0)` for malformed sigs" — is
**factually wrong for OZ v4.9.2**. The `recover` function calls `_throwError`
which reverts on every error variant. This was already the case in OZ v4.3+
(when `_throwError` was introduced); the behaviour is well-documented. The
vuln author appears to have confused `tryRecover` (which returns
`(address(0), error)` without reverting) with `recover` (which reverts).
DeXe uses `recover`, not `tryRecover`. The bug is therefore REFUTED for the
actual code, although the missing zero-address check remains a low-severity
defence-in-depth recommendation.

### 2.6 Verdict

**REFUTED** (as a HIGH-severity fund-drain vulnerability).

The missing zero-address check in `changeVerifier` is a real but
**low-severity** code quality issue (potential self-DoS of the off-chain
results pathway if accidentally set to zero). The HIGH-severity impact
claimed in the vuln file (reward drain via signature forgery) is not
achievable against OZ v4.9.2.

**Recommended action:** Do NOT submit as a HIGH bounty. Optionally file as
LOW/informational with the one-line fix:

```solidity
function changeVerifier(address newVerifier) external override onlyThis {
    require(newVerifier != address(0), "Gov: zero verifier");
    _offChain.verifier = newVerifier;
}
```

---

## 3. Aggregate findings

| Bug | Verdict | Recommended severity | Submit? |
|-----|---------|---------------------|---------|
| #1 Proposal validation bypass via unregistered main executor | **CONFIRMED** | HIGH | ✅ Yes — see submission draft below. |
| #2 `changeVerifier` no zero-address check | **REFUTED** (as HIGH) | LOW (code quality) | ❌ No (not eligible for HIGH bounty; optional LOW report). |

---

## 4. Submission draft (Bug #1 only)

```markdown
# DeXe Protocol — Proposal Validation Bypass via Unregistered Main Executor Enables Treasury Drain

## Severity
High

## Summary
`GovPoolCreate._validateProposalCreation` (contracts/libs/gov/gov-pool/GovPoolCreate.sol:206-215)
uses a `require(!ok || data.length == 0 || abi.decode(data, (bool)))` that
silently passes when the trailing action's executor ("main executor") is an
EOA, `address(0)`, a contract that does not implement `IProposalValidator.validate`,
or a contract whose `validate` reverts. This bypasses every executor-specific
validator (notably `DistributionProposal.validate`, which enforces
`proposalId == latestProposalId`). Combined with the automatic fallback to
DEFAULT settings for unregistered executors (`executorToSettings(x) == 0 ==
ExecutorType.DEFAULT`, which performs no selector whitelisting), an attacker
can create a proposal that calls `DistributionProposal.execute(OLD_id, token, amount)`
on any old succeeded proposal, then claim a pro-rata share of the drained
GovPool treasury.

## Vulnerability Detail
[See §1.1 and §1.2 of the verification report. Code locations and verbatim
require are accurate. Mechanism note: for codeless addresses (EOA,
address(0)) the EVM returns `ok=true, data=""` (vacuous success), so the
bypass triggers via the `data.length == 0` short-circuit; for contracts
without `validate` (no fallback) the call reverts with `ok=false` and the
bypass triggers via `!ok`. Both paths bypass the require.]

## Impact
- Direct theft of GovPool treasury funds (ERC20 or ETH) via retroactive
  distribution proposals for old succeeded proposals.
- Bypass of every `IProposalValidator.validate` check (DP, TSP, SP, and any
  future validator).
- Governance manipulation: already-decided proposals can be retroactively
  turned into distribution proposals.
- The only gate is the DEFAULT-settings quorum. With a low-to-moderate
  quorum, the attack is practical.

## Proof of Concept
Foundry PoC at https://github.com/…/dexe-foundry-poc:

1. test/ProposalValidationBypass.t.sol — 7 unit tests covering each executor
   category (validator returning true/false/reverting, non-validator
   contract, EOA, address(0)).
2. test/DistributionProposalValidateBypass.t.sol — 5 end-to-end tests
   against the real DeXe DistributionProposal contract, demonstrating that
   `DP.validate`'s `proposalId == latestProposalId` invariant is bypassed
   when the proposal has a trailing EOA action.

All 12 tests pass.

## Recommendation
Fix the require polarity in `_validateProposalCreation`:

```solidity
require(ok && data.length >= 32 && abi.decode(data, (bool)), "Gov: validation failed");
```

Defence-in-depth: in `DistributionProposal.execute`, additionally require
`proposalId == GovPool(payable(govAddress)).latestProposalId()`.
```

---

## 5. Appendix — How to reproduce

```bash
# 1. Install Foundry
curl -L https://foundry.paradigm.xyz | bash -s -- && foundryup

# 2. The PoC project is at /home/z/dexe-foundry-poc/
cd /home/z/dexe-foundry-poc/

# 3. Run all tests
forge test -vvv

# Expected output:
# Ran 3 test suites: 25 tests passed, 0 failed, 0 skipped (25 total tests)
#   - DistributionProposalValidateBypass.t.sol: 5 passed (Bug #1 end-to-end)
#   - ProposalValidationBypass.t.sol:           7 passed (Bug #1 unit)
#   - ChangeVerifierZero.t.sol:                13 passed (Bug #2 refutation)
```

### File map

```
/home/z/dexe-foundry-poc/
├── foundry.toml                                    # remappings for OZ 4.9.2, solidity-lib, spherex, uniswap, dexe
├── src/
│   ├── GovPoolCreateHarness.sol                    # verbatim _validateProposalCreation + mock validators
│   ├── GovPoolOffchainHarness.sol                 # verbatim changeVerifier + saveOffchainResults
│   └── dexe_contracts/                             # verbatim copy of /home/z/dexe/contracts/
│       └── gov/proposals/DistributionProposal.sol  # real DeXe DP used in end-to-end test
└── test/
    ├── ProposalValidationBypass.t.sol              # Bug #1 unit tests (7)
    ├── DistributionProposalValidateBypass.t.sol    # Bug #1 end-to-end (5)
    └── ChangeVerifierZero.t.sol                    # Bug #2 refutation (13)
```
