# PSM `removeBenefactor` — Delegated Signer & Beneficiary Persistence After Removal

**Status:** VERIFIED & CONFIRMED via Foundry PoC (4/4 tests pass — including end-to-end fund drain)
**Severity:** High (borderline Critical — see severity assessment below)
**Source:** Test coverage gap analysis (PSM has ZERO test files) → confirmed by Foundry PoC
**Contract:** `/home/z/fkr-step1/defi-bounty/contracts/PSM.sol`
**Functions:** `removeBenefactor` (line 645) + `addBenefactor` (line 624) + `_validateBenefactor` (line 1493)
**Foundry PoC:** `/home/z/fkr-step1/defi-bounty/vuln/PoC_removeBenefactor.t.sol`
**Foundry project (runnable):** `/home/z/fkr-step1/defi-bounty/foundry_test/`

---

## TL;DR — VERIFICATION RESULT

| Question | Answer |
|----------|--------|
| Does `delete benefactorState[benefactor].config` clear nested mappings? | **NO** — confirmed by Solidity semantics + runtime assertions |
| Does `addBenefactor` re-initialize the config or just flip `isActive`? | **Just flips `isActive = true`** (PSM.sol:634) — no cleanup |
| After remove + re-add, is `delegatedSigners[attacker]` still `ACCEPTED`? | **YES** — runtime-asserted in `test_RemoveBenefactor_DelegatedSignerAndBeneficiaryPersist` |
| After remove + re-add, is `approvedBeneficiaries[attacker]` still `true`? | **YES** — runtime-asserted in the same test |
| Can the attacker actually call `swap()` and drain funds after re-add? | **YES** — `test_RemoveBenefactor_AttackerCanSwapAfterReAdd` drains 1,000 asset tokens (1:1 with benefactor's collateral) |
| Is the swap path otherwise sound (i.e., is this a real bug, not test artifact)? | **YES** — `test_Sanity_FreshBenefactorBlocksUnknownAttacker` shows a fresh benefactor correctly blocks an unknown caller |
| **Exploitable?** | **YES — CONFIRMED EXPLOITABLE** |

**Verification result:** CONFIRMED EXPLOITABLE (High severity, bordering on Critical depending on threat model — see below).

---

## Summary

`removeBenefactor` uses `delete benefactorState[benefactor].config` to "clean up" a benefactor's configuration. However, the `BenefactorConfig` struct contains **6 mappings** that Solidity's `delete` operator **cannot clear** (mappings are not iterable and cannot be reset en masse in Solidity). After `removeBenefactor` sets `isActive = false`, all 6 mappings — including `delegatedSigners` (with ACCEPTED status) and `approvedBeneficiaries` — silently persist.

When the same address is re-added via `addBenefactor`, the old delegated signers immediately regain full swap authority without re-confirmation, and old beneficiaries remain approved. An admin who removes a compromised benefactor and later re-adds them (or a different entity at the same address) unknowingly restores all previous permissions.

---

## Root Cause

### The struct (IPSM.sol, lines 112–124)

```solidity
struct BenefactorConfig {
    bool isActive;                                          // value type — deleted OK
    uint128 maxSwapForAssetPerEpoch;                        // value type — deleted OK
    uint128 maxSwapForCollateralPerEpoch;                   // value type — deleted OK
    mapping(address => uint128) swapForAssetFeeByCollateral;           // mapping — NOT deleted
    mapping(address => uint128) swapForCollateralFeeByCollateral;      // mapping — NOT deleted
    mapping(address => DelegatedSignerStatus) delegatedSigners;        // mapping — NOT deleted
    mapping(address => bool) approvedBeneficiaries;                    // mapping — NOT deleted
    mapping(address => bool) zeroSwapForAssetFeeExemptions;           // mapping — NOT deleted
    mapping(address => bool) zeroSwapForCollateralFeeExemptions;      // mapping — NOT deleted
    uint128 maxSwapForAssetPerPeriod;                       // value type — deleted OK
    uint128 maxSwapForCollateralPerPeriod;                  // value type — deleted OK
}
```

### The removal (PSM.sol, lines 645–649)

```solidity
function removeBenefactor(address benefactor) external override nonReentrant onlyRole(BENEFACTOR_MANAGER_ROLE) {
    if (!benefactorState[benefactor].config.isActive) revert BenefactorNotActive(benefactor);
    delete benefactorState[benefactor].config;  // <- only clears value types, NOT mappings
    emit BenefactorRemoved(benefactor);
}
```

### The re-add (PSM.sol, lines 624–636)

```solidity
function addBenefactor(address benefactor) external override nonReentrant onlyValidAddress(benefactor) onlyRole(BENEFACTOR_MANAGER_ROLE) {
    BenefactorConfig storage benefactorConfig = benefactorState[benefactor].config;
    if (benefactorConfig.isActive) revert BenefactorAlreadyExists(benefactor);  // passes (isActive was reset to false)
    if (_isCustodian(benefactor)) revert CustodianBenefactorConflict(benefactor);
    benefactorState[benefactor].config.isActive = true;  // <- re-activates with all old mappings intact
    emit BenefactorAdded(benefactor);
}
```

### `_validateBenefactor` (PSM.sol, lines 1493–1506) — the gate that should have stopped the attacker

```solidity
function _validateBenefactor(Order calldata order, BenefactorState storage _benefactorState) internal view {
    if (!_benefactorState.config.isActive) revert BenefactorNotActive(order.benefactor);
    if (_benefactorState.orderNonceInvalidator[order.nonce]) revert InvalidNonce(order.nonce);
    if (
        msg.sender != order.benefactor
            && _benefactorState.config.delegatedSigners[msg.sender] != DelegatedSignerStatus.ACCEPTED  // <- reads persisted mapping!
    ) {
        revert DelegationNotAuthorized(msg.sender);
    }
    if (order.benefactor != order.beneficiary && !_benefactorState.config.approvedBeneficiaries[order.beneficiary])
    {
        revert BeneficiaryNotApproved(order.beneficiary);  // <- reads persisted mapping!
    }
}
```

### Why `delete` doesn't clear mappings

In Solidity, the `delete` operator on a struct in storage:
- Resets all **value types** (bool, uint, int, address, enum, bytesN) to their default values
- Recursively deletes nested **structs and arrays**
- Does **NOT** affect **mappings** — mappings have no enumerable keys and cannot be mass-cleared

This is documented Solidity behavior (see [Solidity docs: delete](https://docs.soliditylang.org/en/latest/types.html#delete)). It is a common source of bugs when developers assume `delete struct` fully cleans state.

---

## Foundry PoC — VERIFIED EXPLOITABLE

**Environment used for verification:**
- Foundry `forge 1.8.3` (installed via `foundryup`)
- Solidity `0.8.30` (matches contract pragma)
- OpenZeppelin Contracts `v5.0.2`
- `via_ir = true` (required — PSM otherwise hits "stack too deep")
- EVM version: `cancun`

**Test files:**
- `/home/z/fkr-step1/defi-bounty/vuln/PoC_removeBenefactor.t.sol` (canonical copy — also in `foundry_test/test/`)
- `/home/z/fkr-step1/defi-bounty/vuln/MockERC20.sol` (mock ERC-20)
- `/home/z/fkr-step1/defi-bounty/vuln/MockOracleFeed.sol` (mock oracle)
- Runnable project: `/home/z/fkr-step1/defi-bounty/foundry_test/`

### How to run

```bash
cd /home/z/fkr-step1/defi-bounty/foundry_test
forge test -vvvv --match-contract PoC_removeBenefactor
```

### Test results (actual output)

```
Ran 4 tests for test/PoC_removeBenefactor.t.sol:PoC_removeBenefactor
[PASS] test_RemoveBenefactor_AttackerCanSwapAfterReAdd() (gas: 869850)
Logs:
  attacker asset gain: 1000.000000000000000000
  benefactorA collateral loss: 1000.000000000000000000

[PASS] test_RemoveBenefactor_DelegatedSignerAndBeneficiaryPersist() (gas: 444454)
[PASS] test_Sanity_AttackerCanSwapBeforeRemove() (gas: 566255)
[PASS] test_Sanity_FreshBenefactorBlocksUnknownAttacker() (gas: 150485)
Suite result: ok. 4 passed; 0 failed; 0 skipped; finished in 2.27ms
```

### What the four tests prove

| Test | Purpose | Result |
|------|---------|--------|
| `test_RemoveBenefactor_DelegatedSignerAndBeneficiaryPersist` | Storage-level proof: `delegatedSigners[attacker]` stays `ACCEPTED` and `approvedBeneficiaries[attacker]` stays `true` across remove + re-add | **PASS** (bug confirmed at state level) |
| `test_RemoveBenefactor_AttackerCanSwapAfterReAdd` | End-to-end exploit: after remove + 7-day warp + re-add, attacker calls `swap()` with `beneficiary=attacker` and drains 1,000 asset tokens | **PASS** (fund drain confirmed) |
| `test_Sanity_AttackerCanSwapBeforeRemove` | Sanity: the swap path is correctly set up (delegated signer works as intended pre-incident) | **PASS** |
| `test_Sanity_FreshBenefactorBlocksUnknownAttacker` | Sanity: a fresh benefactor with no prior delegation correctly reverts with `DelegationNotAuthorized`. Proves the bug is specifically persistence, not a general auth bypass | **PASS** |

### Key trace from `test_RemoveBenefactor_AttackerCanSwapAfterReAdd`

After `removeBenefactor(benefactorA)` → `vm.warp(+7 days)` → `addBenefactor(benefactorA)`, the attacker (NOT benefactorA) calls:

```
PSM::swap(Order({ isSwapForAsset: true, expiry: 608401, nonce: 1, chainId: 31337,
    benefactor: 0x12942E57B5C0758bc0F9E196f7c44A50B7C46b56,           // <- benefactorA
    beneficiary: 0x9dF0C6b0066D5317aA5b38B36850548DaCCa6B4e,         // <- attacker
    collateral: 0x2e234DAe75C793f67A35089C9d99245E1C58470b,
    amountIn: 1000000000000000000000, minAmountOut: 1000000000000000000000 }))
  -> MockERC20::transferFrom(benefactorA, collateralReceiveCustodian, 1000e18)  // collateral pulled FROM benefactorA
  -> MockERC20::transferFrom(assetSendCustodian, attacker, 1000e18)             // asset sent TO attacker
  -> emit SwapExecuted(orderExecutor: attacker, benefactor: benefactorA, beneficiary: attacker, amountOut: 1000e18, feeAmount: 0)

attacker asset gain:        1000.000000000000000000
benefactorA collateral loss: 1000.000000000000000000
```

The swap succeeds **without** any re-confirmation by benefactorA and **without** any re-approval of the attacker as beneficiary.

---

## Step-by-step Attack Scenario

### Setup (pre-incident — all intended behavior)
1. Admin adds `benefactorA` via `addBenefactor(benefactorA)`.
2. `benefactorA` calls `setDelegatedSigner(delegatedSignerX)` → status = `PENDING`.
3. `delegatedSignerX` calls `confirmDelegatedSigner(benefactorA)` → status = `ACCEPTED`.
4. `benefactorA` calls `setApprovedBeneficiary(beneficiaryY, true)`.
5. `benefactorA` approves the PSM contract to spend its collateral (normal operational setup).
6. `benefactorA`'s collateral custodian is funded (normal operational setup).

### Incident
7. `delegatedSignerX` (= `beneficiaryY`, the attacker) is compromised (key leak, rogue insider, supply-chain attack on the signer service, etc.).
8. Admin removes `benefactorA` via `removeBenefactor(benefactorA)` in an attempt to cut ties.
   - `isActive` = false (value type reset)
   - `delegatedSigners[delegatedSignerX]` = **ACCEPTED (persists!)**
   - `approvedBeneficiaries[beneficiaryY]` = **true (persists!)**
9. Admin believes `delegatedSignerX` can no longer swap on behalf of `benefactorA`. The `BenefactorRemoved` event gives no indication that mappings survived.

### Re-activation
10. Days/weeks later, admin re-adds `benefactorA` via `addBenefactor(benefactorA)`.
    - `isActive` = true
    - All mappings still intact from steps 2–4
11. The compromised `delegatedSignerX` calls `swap(...)` with `order.benefactor = benefactorA` and `order.beneficiary = beneficiaryY` (= attacker).
12. `_validateBenefactor` (line 1493–1506) checks:
    - `isActive` → passes (just re-activated)
    - `msg.sender != order.benefactor && delegatedSigners[msg.sender] != ACCEPTED` → does NOT revert (status is still ACCEPTED from step 3)
    - `order.benefactor != order.beneficiary && !approvedBeneficiaries[order.beneficiary]` → does NOT revert (still true from step 4)
13. **The swap succeeds.** Collateral is pulled FROM `benefactorA` and asset is sent TO `beneficiaryY` (the attacker). Funds flow up to the benefactor's configured rate limits.

### Why this is dangerous
- The admin's intent (remove benefactor → cut off all associated signers) is **not achieved**.
- There is **no event, no warning, no re-confirmation** when re-adding restores old permissions.
- The persistence is **invisible** — nothing in the `BenefactorAdded` event indicates stale mappings.
- The attack requires only a single compromised delegated-signer key, which is a realistic threat model for a system that explicitly supports delegation.
- Critically: the attacker doesn't need to phish or compromise `benefactorA` again. The persistence is **at the contract level**, in `benefactorA`'s own storage slot. Even if `benefactorA`'s key was rotated (the most common reason for remove + re-add), the attacker retains authority.

---

## Affected Mappings (all persist after removeBenefactor)

| Mapping | Impact if persisted |
|---------|---------------------|
| `delegatedSigners` | **CRITICAL: ACCEPTED signers can swap immediately after re-add** |
| `approvedBeneficiaries` | Old beneficiaries can receive swap output without re-approval |
| `swapForAssetFeeByCollateral` | Old custom fees apply (economic: could be 0 fee) |
| `swapForCollateralFeeByCollateral` | Same for opposite direction |
| `zeroSwapForAssetFeeExemptions` | Old fee exemptions persist (free swaps) |
| `zeroSwapForCollateralFeeExemptions` | Same for opposite direction |

The PoC explicitly exercises `delegatedSigners` and `approvedBeneficiaries` (the two with direct fund-loss impact). The four fee-related mappings have a softer economic impact (stale fee configurations) but follow the same root cause.

---

## Contrast with `disableBenefactor` (safe, intended semantics)

`disableBenefactor` (line 611) only sets `isActive = false` without `delete`:

```solidity
function disableBenefactor(address benefactor) external override nonReentrant onlyRole(BENEFACTOR_DISABLER_ROLE) {
    if (!benefactorState[benefactor].config.isActive) revert BenefactorNotActive(benefactor);
    benefactorState[benefactor].config.isActive = false;  // <- no delete, mappings intentionally persist
    emit BenefactorDisabled(benefactor);
}
```

`disableBenefactor` is safe because it's a **temporary** measure — re-enabling via `enableBenefactor` is expected to restore the benefactor's full config. The mappings persisting is **intended** for disable/enable cycles.

The bug is specifically in `removeBenefactor` using `delete`, which implies a **permanent** cleanup that doesn't actually happen for mappings. The semantic distinction between "disable" (temporary) and "remove" (permanent) is broken.

---

## Realistic Conditions for Exploitation

For the bug to manifest in production, **all** of the following must be true:

1. **A benefactor is removed via `removeBenefactor`** (not `disableBenefactor`). This is the action an admin would take when they want to permanently sever ties — e.g., offboarding a market-maker, removing a compromised benefactor, or sunsetting a partner relationship. It is the **natural** choice when the intent is permanent.

2. **The same address is later re-added via `addBenefactor`**. This is realistic in several scenarios:
   - Admin removes a compromised benefactor, then re-adds the same address after rotating keys.
   - A market-maker is offboarded and later re-onboarded at the same address.
   - An admin removes a benefactor "to be safe" during an incident and re-adds them post-incident.
   - An address collision: a new partner is onboarded at an address that was previously a benefactor (vanishingly rare but possible).

3. **A delegated signer and/or beneficiary was set BEFORE removal**. This is the **default** operating state for any benefactor that actually uses the delegation feature.

4. **The pre-removal delegated signer key OR beneficiary address is controlled by the attacker at the time of re-add**. Realistic threat vectors:
   - Compromised signer service / HSM breach.
   - Rogue insider who controlled the signer key.
   - Beneficiary address was a hot wallet that got drained; attacker retains the ability to receive funds.
   - Signer key leak via off-chain infrastructure compromise (CI/CD, secrets manager, etc.).
   - Note: the attacker does NOT need ongoing control of `benefactorA` itself — only of the delegated-signer key and/or beneficiary address.

**Likelihood assessment:** Conditions 1+3 are common operational states. Condition 2 is a periodic operational event (e.g., incident response, partner churn). Condition 4 is the bar — but the entire reason `removeBenefactor` exists is precisely to recover from condition 4. So the bug defeats the primary use case of the function.

---

## Severity Assessment

| Factor | Rating | Notes |
|--------|--------|-------|
| Fund loss? | **Yes** | Direct drain of asset tokens up to the benefactor's rate limits per epoch/period |
| Privilege escalation? | **Yes** | Attacker regains delegated-signer authority without re-confirmation |
| Preconditions | Moderate | Requires remove + re-add cycle (operational, not adversarial) AND a compromised signer/beneficiary (adversarial but realistic) |
| Bypass of intent | **Severe** | `removeBenefactor`'s sole purpose is severed-ties; the bug defeats that purpose silently |
| Detection difficulty | High | No event indicates stale mappings; admin sees `BenefactorRemoved` + `BenefactorAdded` and assumes clean state |
| Scope of impact | Per-benefactor | Not a protocol-wide drain; bounded by benefactor's rate limits and the custodian's asset inventory |
| Fix complexity | Moderate | Requires either versioning (`configVersion` field) or enumerable key sets |

**Severity: HIGH (borderline Critical).**

Rationale for not labeling Critical outright:
- The attack is bounded by the benefactor's per-epoch/per-period rate limits, not the entire protocol TVL.
- It requires the admin to perform a remove + re-add cycle (not a single malicious tx).
- The attacker must already have a foothold (compromised signer or beneficiary) at the time of re-add.

Rationale for High (not Medium):
- The bug **silently** defeats the primary purpose of `removeBenefactor` — an incident-response function.
- Direct fund loss is achievable end-to-end (proven by PoC).
- The attack window is potentially **days/weeks** between remove and re-add, and the attacker needs no further action during that window — the persistence is at the contract level.
- A 1:1 swap of collateral for asset means the attacker walks away with asset tokens (essentially cash) at the benefactor's expense.

This is the kind of bug Immunefi typically adjudicates as **High** ($10k–$100k range for Ethena-scale programs) with an outside shot at Critical if the reviewer weights the incident-response-defeat angle heavily.

---

## Suggested Fix

**Option A: Explicitly clear critical mappings before delete** (requires tracking keys)

This is difficult because mappings don't store their keys. Would require an enumerable set of delegated signers and beneficiaries (e.g., OpenZeppelin's `EnumerableSet`).

**Option B: Add a `configVersion` to `BenefactorConfig`** that invalidates old mappings

```solidity
struct BenefactorConfig {
    bool isActive;
    uint256 configVersion;  // <- new: incremented on removeBenefactor
    ...
}
```

Then `_validateBenefactor` checks `delegatedSigners[signer]` against the current `configVersion`:

```solidity
mapping(address => mapping(uint256 => DelegatedSignerStatus)) delegatedSignersByVersion;
// check: delegatedSignersByVersion[benefactor][config.configVersion][msg.sender] == ACCEPTED
```

`removeBenefactor` would increment `configVersion` instead of (or in addition to) `delete`. Old mappings remain in storage but are unreachable via the active version. This is gas-efficient and doesn't require enumerable sets.

**Option C: Don't use `delete` — use `disableBenefactor` semantics for removal too**

If removal is meant to be permanent, the benefactor address should be blocklisted from re-adding (e.g., via a `removedBenefactors` enumerable set or mapping). OR explicitly clear mappings by iterating over known keys (requires storing key lists).

**Option D (simplest): Document that `removeBenefactor` does NOT clear permissions and require explicit cleanup**

Add a check that all delegated signers are REJECTED and all beneficiaries are removed before allowing `removeBenefactor`. This requires enumerable sets to verify.

**Recommendation:** Option B (`configVersion`) is the cleanest. It also future-proofs against any new mapping fields being added to `BenefactorConfig` without the dev remembering to clear them.

---

## Related: `setDelegatedSigner` and `setApprovedBeneficiary` have no active-benefactor check

Both `setDelegatedSigner` (line 849) and `setApprovedBeneficiary` (line 906) can be called by **any address**, regardless of whether the caller is an active benefactor:

```solidity
function setDelegatedSigner(address signer) external override nonReentrant onlyValidAddress(signer) {
    BenefactorConfig storage config = benefactorState[msg.sender].config;
    config.delegatedSigners[signer] = DelegatedSignerStatus.PENDING;  // <- no isActive check
    emit DelegatedSignerAdded(signer, msg.sender);
}
```

This means a non-benefactor can **pre-set** PENDING delegated signers and approved beneficiaries in their (inactive) config. Combined with the `removeBenefactor` bug:

1. Attacker calls `setDelegatedSigner(attackerSigner)` on their own (non-benefactor) address.
2. `attackerSigner` calls `confirmDelegatedSigner(attacker)` — but this reverts because `confirmDelegatedSigner` checks `if (!config.isActive) revert BenefactorNotActive`. So this pre-setting alone is not exploitable.
3. However, if the attacker's address is later added as a benefactor (e.g., a new employee address that happens to collide, or the attacker social-engineers their addition), the PENDING status is already there, and `attackerSigner` can immediately confirm.

This is a lower-severity issue on its own, but it **amplifies** the `removeBenefactor` bug — the "phantom permission" surface area is larger than just "permissions set during an active window".

---

## Test Coverage

**Zero.** PSM.sol has no test files. This bug would have been caught by a simple test:

```solidity
function test_removeBenefactor_clearsDelegatedSigners() public {
    psm.addBenefactor(benefactor);
    vm.prank(benefactor);
    psm.setDelegatedSigner(signer);
    vm.prank(signer);
    psm.confirmDelegatedSigner(benefactor);
    assert(psm.getDelegatedSignerStatus(benefactor, signer) == DelegatedSignerStatus.ACCEPTED);

    psm.removeBenefactor(benefactor);
    psm.addBenefactor(benefactor);

    // BUG: signer is still ACCEPTED
    assert(psm.getDelegatedSignerStatus(benefactor, signer) == DelegatedSignerStatus.ACCEPTED); // <- this would FAIL the test
}
```

The PoC in `PoC_removeBenefactor.t.sol` is essentially this test (plus the end-to-end swap drain).

---

## Immunefi Submission Recommendation

**SUBMIT: YES.**

Reasons:
1. **Verified, not theoretical.** Foundry PoC passes end-to-end with actual fund movement (1,000 asset tokens drained from benefactorA's collateral). Not a "could-be" bug.
2. **Violates documented intent.** The docstring on `removeBenefactor` says "Removes a benefactor from the system" — the implementation does not actually remove the benefactor's permissions. This is a clear semantic bug, not a design dispute.
3. **Direct fund loss.** The PoC shows asset tokens being transferred to an attacker-controlled beneficiary. This is not a DoS or economic edge case.
4. **Realistic threat model.** Compromised delegated-signer key is a standard threat for any delegation-based system. The function exists precisely to recover from such compromises; the bug defeats that recovery.
5. **No prerequisites the attacker controls.** The attacker needs no special role, no governance vote, no flash loan — only the pre-existing (compromised) delegated-signer key and the admin's remove + re-add cycle.
6. **Bounded but meaningful impact.** Bounded by per-benefactor rate limits and custodian inventory — not protocol-wide TVL, but enough to be material (each benefactor can swap up to its configured per-epoch/per-period maximum).

**Suggested severity for submission:** High (with a request for the triage team to consider Critical given the incident-response-defeat angle).

**Suggested bounty range (Ethena's program):** $25k–$100k. If the triage team agrees the bug defeats an incident-response function and the persistence is invisible (no event), the upper end is justifiable.

**Caveats to disclose honestly in the submission:**
- The bug requires a remove + re-add cycle (operational, not always triggered).
- The attacker must retain control of the compromised signer/beneficiary at re-add time.
- Impact is per-benefactor, not protocol-wide.
- A simple operational mitigation exists: never re-add a removed benefactor at the same address (use a fresh address). This is a workaround, not a fix, but it bounds real-world impact.

---

## Assessment Summary

| Criterion | Assessment |
|-----------|------------|
| Is the code path reachable? | YES — `removeBenefactor` and `addBenefactor` are standard admin functions |
| Is the behavior unintended? | YES — `delete` implies cleanup; docstring says "Removes a benefactor from the system" |
| Is there a test? | NO — zero PSM tests in the original repo (PoC added in this verification) |
| Is the fix straightforward? | Moderate — requires either versioning or enumerable key tracking |
| Is this a known pattern? | YES — "delete doesn't clear mappings" is a well-known Solidity gotcha |
| Realistic threat model? | YES — compromised delegated signer key is a standard threat |
| **Foundry PoC passing?** | **YES — 4/4 tests pass, including end-to-end fund drain** |
| **Exploitable?** | **YES — CONFIRMED** |
| **Severity** | **High (borderline Critical)** |
| **Immunefi submission?** | **YES** |
