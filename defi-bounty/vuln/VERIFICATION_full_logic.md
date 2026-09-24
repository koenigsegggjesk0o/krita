# PSM `removeBenefactor` — Full Logic Verification (Brutal Audit)

**Task ID:** `eth-verify-full-logic`
**Agent:** Opus (full-logic verifier)
**Method:** Read every line of `PSM.sol` (2083 lines) + both deps; trace every code path that touches `BenefactorState`/`BenefactorConfig`; cross-reference against the bug claim.
**Goal:** Determine whether the `removeBenefactor` bug claim is REAL or a MISUNDERSTANDING of intentional design.

---

## 1. Bug Claim Restatement

> `removeBenefactor` (PSM.sol:645) uses `delete benefactorState[benefactor].config` to clean up a benefactor's state. However, the `BenefactorConfig` struct contains 6 nested mappings (`swapForAssetFeeByCollateral`, `swapForCollateralFeeByCollateral`, `delegatedSigners`, `approvedBeneficiaries`, `zeroSwapForAssetFeeExemptions`, `zeroSwapForCollateralFeeExemptions`). Solidity's `delete` operator CANNOT clear nested mappings inside a struct (mappings have no enumerable keys). After `removeBenefactor` → `addBenefactor`, all 6 mappings silently persist. The old `ACCEPTED` delegated signers and old approved beneficiaries therefore regain full swap authority **without re-confirmation** — defeating the purpose of `removeBenefactor` (an incident-response function) and enabling direct theft of the benefactor's funds via `swap()`.

---

## 2. Line-by-Line Verification (14 items)

### Item 1 — `removeBenefactor` (PSM.sol:645–649)

**Code (verbatim, read in full):**
```solidity
function removeBenefactor(address benefactor) external override nonReentrant onlyRole(BENEFACTOR_MANAGER_ROLE) {
    if (!benefactorState[benefactor].config.isActive) revert BenefactorNotActive(benefactor);
    delete benefactorState[benefactor].config;
    emit BenefactorRemoved(benefactor);
}
```

**Observations:**
- Three operations total: (1) precondition check (must be active), (2) `delete benefactorState[benefactor].config`, (3) event emission.
- It does NOT explicitly iterate / clear `delegatedSigners`, `approvedBeneficiaries`, `swapForAssetFeeByCollateral`, `swapForCollateralFeeByCollateral`, `zeroSwapForAssetFeeExemptions`, or `zeroSwapForCollateralFeeExemptions`.
- It does NOT touch `BenefactorState.epochStateByDuration`, `BenefactorState.periodStateByDuration`, or `BenefactorState.orderNonceInvalidator` (these live OUTSIDE `.config` and would survive even a hypothetical `delete _benefactorState` — but that's not what's happening here either).
- It does NOT call any helper like `_clearBenefactorMappings` — there is no such helper in the contract (verified by grep for `delete ` — only one match: line 647).
- It does NOT increment any version/nonce/generation counter (verified by grep for `configVersion|generation` — zero matches in the contracts dir).

**Solidity `delete` semantics (well-documented):** applying `delete` to a struct in storage resets all **value-type** fields (`bool`, `uint*`, `int*`, `address`, `bytesN`, enums) to their zero defaults; recursively applies `delete` to nested **structs and arrays**; **does NOT touch mappings** because mappings have no enumerable keys and Solidity provides no operator to mass-clear them. The 6 mappings live at storage slots derived from `keccak256(parentSlot . key)`, which `delete` does not and cannot iterate.

**Finding for Item 1:** **CONFIRMED.** `removeBenefactor` performs ONLY `delete .config`. It does NOT explicitly clear the 6 nested mappings, and Solidity's `delete` does not do so either. The mappings persist.

---

### Item 2 — `addBenefactor` (PSM.sol:624–636)

**Code (verbatim):**
```solidity
function addBenefactor(address benefactor)
    external
    override
    nonReentrant
    onlyValidAddress(benefactor)
    onlyRole(BENEFACTOR_MANAGER_ROLE)
{
    BenefactorConfig storage benefactorConfig = benefactorState[benefactor].config;
    if (benefactorConfig.isActive) revert BenefactorAlreadyExists(benefactor);
    if (_isCustodian(benefactor)) revert CustodianBenefactorConflict(benefactor);
    benefactorState[benefactor].config.isActive = true;
    emit BenefactorAdded(benefactor);
}
```

**Observations:**
- Two precondition checks: (a) `isActive` must be false (prevents duplicate-add; passes trivially after `removeBenefactor` because `delete` zeroed the `isActive` bool), (b) benefactor must not be a registered custodian.
- Then a single storage write: `benefactorState[benefactor].config.isActive = true`.
- It does NOT reset/clear `delegatedSigners`, `approvedBeneficiaries`, or any of the 4 fee mappings.
- It does NOT check or warn about pre-existing mapping entries (no "fresh state" assertion).
- It does NOT call any `_initBenefactorConfig` / `_resetBenefactor` helper.
- It does NOT increment any version field (no version field exists — see Item 15).
- It does NOT enforce a "fresh address" requirement (any address that is currently inactive can be re-added).

**Compare with constructor / first-add behavior:** the very first `addBenefactor` call on a never-before-used address inherits a default-zero `BenefactorConfig` (Solidity zero-initializes storage). So first-add and re-add are indistinguishable from the contract's perspective — both just flip `isActive` from `false` to `true`. There is no "first time" branch.

**Finding for Item 2:** **CONFIRMED.** `addBenefactor` ONLY sets `isActive = true`. No re-initialization, no cleanup, no version bump.

---

### Item 3 — `disableBenefactor` (PSM.sol:611–615)

**Code (verbatim):**
```solidity
function disableBenefactor(address benefactor) external override nonReentrant onlyRole(BENEFACTOR_DISABLER_ROLE) {
    if (!benefactorState[benefactor].config.isActive) revert BenefactorNotActive(benefactor);
    benefactorState[benefactor].config.isActive = false;
    emit BenefactorDisabled(benefactor);
}
```

**Observations:**
- Precondition: must be active.
- Single storage write: `isActive = false`.
- Does NOT call `delete`. Does NOT touch any mappings.
- Different role: `BENEFACTOR_DISABLER_ROLE` (vs `BENEFACTOR_MANAGER_ROLE` for `addBenefactor` / `removeBenefactor`).
- The intent here is **temporary** pause — mappings are EXPECTED to persist so that `enableBenefactor` restores full config.

**Compare with `removeBenefactor`:**
| Aspect | `disableBenefactor` | `removeBenefactor` |
|---|---|---|
| Role | `BENEFACTOR_DISABLER_ROLE` (incident) | `BENEFACTOR_MANAGER_ROLE` (admin) |
| Storage op | `isActive = false` (one slot write) | `delete .config` (zeroes value-type fields, leaves mappings) |
| Intent (per NatSpec) | "Disables a benefactor" (temporary pause) | "Removes a benefactor from the system" (permanent severance) |
| Mappings cleared? | No (intentional — they survive enable) | No (UNintentional — `delete` implies cleanup) |
| Net effect on mappings | Identical to pre-disable state | Identical to pre-remove state |
| Net effect on value-type fields | Identical (only `isActive` changes) | All value-type fields zeroed |

**Critical observation:** The contract provides TWO functions whose net effect on the persistent mappings is IDENTICAL — neither clears them. The difference is that `disableBenefactor` is honest about it (one bool flip, no `delete`), while `removeBenefactor` performs a `delete` that **suggests** cleanup but doesn't deliver it for mappings.

This semantic divergence is the strongest signal that the `removeBenefactor` behavior is a **bug**, not intentional design. If the developers had intended mappings to persist across remove+re-add, they would have used `disableBenefactor` semantics (one bool flip) for `removeBenefactor` too. The fact that they reached for `delete` indicates they expected full cleanup — which Solidity silently does not provide for mappings.

**Finding for Item 3:** **CONFIRMED.** `disableBenefactor` only flips `isActive=false`; this is safe because its NatSpec and pairing with `enableBenefactor` make persistence intentional. `removeBenefactor` differs structurally (uses `delete`) but achieves the same persistence for mappings — diverging from its own "Removes" semantics.

---

### Item 4 — `enableBenefactor` (PSM.sol:589–602)

**Code (verbatim):**
```solidity
function enableBenefactor(address benefactor)
    external
    override
    nonReentrant
    onlyRole(DEFAULT_ADMIN_ROLE)
    onlyValidAddress(benefactor)
{
    if (benefactorState[benefactor].config.isActive) {
        revert BenefactorAlreadyEnabled(benefactor);
    }
    if (_isCustodian(benefactor)) revert CustodianBenefactorConflict(benefactor);
    benefactorState[benefactor].config.isActive = true;
    emit BenefactorEnabled(benefactor);
}
```

**Observations:**
- Precondition: must be inactive (so it pairs with `disableBenefactor`).
- Different role: `DEFAULT_ADMIN_ROLE` (more privileged than `BENEFACTOR_MANAGER_ROLE` used by `addBenefactor`).
- Single storage write: `isActive = true`. No cleanup.
- This is the **safe** counterpart to `disableBenefactor` — both treat mappings as persistent intentionally.

**Compare with `addBenefactor`:**
- Both flip `isActive` from false to true.
- Both perform the same custodian-conflict check.
- They differ only in: role required (`DEFAULT_ADMIN_ROLE` vs `BENEFACTOR_MANAGER_ROLE`), and the duplicate-guard revert (`BenefactorAlreadyEnabled` vs `BenefactorAlreadyExists`).
- Neither cleans up mappings — which is correct for `enableBenefactor` (pairs with disable) but questionable for `addBenefactor` (pairs with the supposedly-permanent `removeBenefactor`).

**Finding for Item 4:** **CONFIRMED.** `enableBenefactor` mirrors `addBenefactor` in behavior (single bool flip, no cleanup). The disable/enable pair is self-consistent. The remove/add pair is structurally identical but semantically inconsistent with the "Removes" docstring.

---

### Item 5 — `setDelegatedSigner` (PSM.sol:849–853)

**Code (verbatim):**
```solidity
function setDelegatedSigner(address signer) external override nonReentrant onlyValidAddress(signer) {
    BenefactorConfig storage config = benefactorState[msg.sender].config;
    config.delegatedSigners[signer] = DelegatedSignerStatus.PENDING;
    emit DelegatedSignerAdded(signer, msg.sender);
}
```

**Observations:**
- Caller is `msg.sender` (treated as the benefactor).
- Storage target: `benefactorState[msg.sender].config.delegatedSigners[signer]` — i.e., **inside `BenefactorConfig`**, which is the struct that `removeBenefactor` deletes.
- Sets status to `PENDING`. No `isActive` check — confirmed by NatSpec: "Can be called even if not active to allow inactive benefactors to manage their delegated signers."
- This is a **two-step delegation**: benefactor proposes (PENDING), signer accepts (`confirmDelegatedSigner` → ACCEPTED).

**Implication for bug:** since `delegatedSigners` lives inside `BenefactorConfig`, an `ACCEPTED` status set before `removeBenefactor` survives the `delete` and is still `ACCEPTED` after re-add. No re-proposal, no re-confirmation needed.

**Finding for Item 5:** **CONFIRMED.** Delegation is stored inside `BenefactorConfig.delegatedSigners` — exactly the struct that `removeBenefactor`'s `delete` fails to clear.

---

### Item 6 — `confirmDelegatedSigner` (PSM.sol:868–876)

**Code (verbatim):**
```solidity
function confirmDelegatedSigner(address benefactor) external override nonReentrant onlyValidAddress(benefactor) {
    BenefactorConfig storage config = benefactorState[benefactor].config;
    if (!config.isActive) revert BenefactorNotActive(benefactor);
    if (config.delegatedSigners[msg.sender] != DelegatedSignerStatus.PENDING) {
        revert DelegationNotAuthorized(msg.sender);
    }
    config.delegatedSigners[msg.sender] = DelegatedSignerStatus.ACCEPTED;
    emit DelegatedSignerConfirmed(msg.sender, benefactor);
}
```

**Observations:**
- Requires `isActive` (cannot confirm while inactive — this is the only place delegation has an active-state guard).
- Requires status == `PENDING`. If status is `ACCEPTED` or `REJECTED`, reverts with `DelegationNotAuthorized`.
- Promotes PENDING → ACCEPTED.

**Implication for the bug:** after `removeBenefactor` + `addBenefactor`, an old `ACCEPTED` signer does NOT need to re-confirm (their status is already `ACCEPTED`, which is what `_validateBenefactor` checks). They couldn't re-confirm even if they wanted to — `confirmDelegatedSigner` would revert with `DelegationNotAuthorized` because their status is `ACCEPTED`, not `PENDING`. So the "re-confirmation" path is **structurally blocked** for already-accepted signers — they either keep access silently or the benefactor must first flip them to `REJECTED` (via `removeDelegatedSigner`) and then re-propose via `setDelegatedSigner` + `confirmDelegatedSigner`. But that requires the benefactor to know the persisted signer exists — and nothing tells them.

**Finding for Item 6:** **CONFIRMED.** Re-confirmation is structurally impossible for already-`ACCEPTED` signers; they retain access silently without any action.

---

### Item 7 — `removeDelegatedSigner` (PSM.sol:887–894)

**Code (verbatim):**
```solidity
function removeDelegatedSigner(address signer) external override nonReentrant {
    BenefactorConfig storage config = benefactorState[msg.sender].config;
    if (config.delegatedSigners[signer] == DelegatedSignerStatus.REJECTED) {
        revert DelegationNotAuthorized(signer);
    }
    config.delegatedSigners[signer] = DelegatedSignerStatus.REJECTED;
    emit DelegatedSignerRemoved(signer, msg.sender);
}
```

**Observations:**
- Caller is `msg.sender` (the benefactor).
- Sets status to `REJECTED`. Note: `REJECTED` is the **enum's zero value** (verified from IPSM.sol:15-19: `enum DelegatedSignerStatus { REJECTED, PENDING, ACCEPTED }`). So `REJECTED` is the same as "default/never-set" — there is no way to distinguish a signer that was never proposed from one that was explicitly rejected.
- Precondition: signer must NOT already be `REJECTED` (idempotency guard).
- This is the only benefactor-callable way to revoke an accepted signer.

**Implication for the bug:** this is a **per-signer** operation. The benefactor would need to know every signer they had previously delegated and call `removeDelegatedSigner` for each one before `removeBenefactor` (or after re-add) to actually clear them. There is no "remove all delegated signers" batch function. Without an enumerable set (there is none — see Item 12), the benefactor has no on-chain way to discover all their existing signers. They would have to track them off-chain via `DelegatedSignerAdded` events.

**Finding for Item 7:** **CONFIRMED.** `removeDelegatedSigner` exists and properly sets status to `REJECTED` (= default zero), but it is per-signer only. There is no batch-clear, no enumerable set, no auto-clear on `removeBenefactor`.

---

### Item 8 — `setApprovedBeneficiary` (PSM.sol:906–921)

**Code (verbatim):**
```solidity
function setApprovedBeneficiary(address beneficiary, bool approved)
    external
    override
    nonReentrant
    onlyValidAddress(beneficiary)
{
    BenefactorConfig storage config = benefactorState[msg.sender].config;
    if (config.approvedBeneficiaries[beneficiary] != approved) {
        config.approvedBeneficiaries[beneficiary] = approved;
        if (approved) {
            emit BeneficiaryApproved(msg.sender, beneficiary);
        } else {
            emit BeneficiaryRemoved(msg.sender, beneficiary);
        }
    }
}
```

**Observations:**
- Caller is `msg.sender` (the benefactor).
- Storage target: `benefactorState[msg.sender].config.approvedBeneficiaries[beneficiary]` — **inside `BenefactorConfig`**, the struct `removeBenefactor` deletes.
- No `isActive` check — confirmed by NatSpec: "Callable by any benefactor, even if not active."
- Per-beneficiary toggle. No batch-clear, no enumerable list.

**Implication for the bug:** an `approvedBeneficiaries[attacker] = true` set before `removeBenefactor` survives `delete .config` and persists across re-add. The benefactor would need to call `setApprovedBeneficiary(attacker, false)` for each previously-approved beneficiary to clear them — and again, with no on-chain enumerable list, they must track approvals off-chain via `BeneficiaryApproved` events.

**Finding for Item 8:** **CONFIRMED.** Beneficiary approval lives inside `BenefactorConfig.approvedBeneficiaries` — same struct that fails to be cleaned by `removeBenefactor`.

---

### Item 9 — `_validateBenefactor` (PSM.sol:1493–1506)

**Code (verbatim):**
```solidity
function _validateBenefactor(Order calldata order, BenefactorState storage _benefactorState) internal view {
    if (!_benefactorState.config.isActive) revert BenefactorNotActive(order.benefactor);
    if (_benefactorState.orderNonceInvalidator[order.nonce]) revert InvalidNonce(order.nonce);
    if (
        msg.sender != order.benefactor
            && _benefactorState.config.delegatedSigners[msg.sender] != DelegatedSignerStatus.ACCEPTED
    ) {
        revert DelegationNotAuthorized(msg.sender);
    }
    if (order.benefactor != order.beneficiary && !_benefactorState.config.approvedBeneficiaries[order.beneficiary])
    {
        revert BeneficiaryNotApproved(order.beneficiary);
    }
}
```

**Observations — what it checks:**
1. `isActive` must be true. (Reset to false by `removeBenefactor`; restored to true by `addBenefactor`.) ✓ Passes after re-add.
2. `orderNonceInvalidator[order.nonce]` must be false. This lives in `BenefactorState` (sibling to `config`, see Item 12), NOT in `BenefactorConfig`. So it is NOT cleared by `delete .config`. **Old nonces persist as already-used.** This means: an attacker reusing an old nonce will be REJECTED. But an attacker using a NEW nonce (one the benefactor never signed before) will pass. The nonce check is therefore a **defensive behavior in the wrong direction** — it blocks old nonces but does NOT block old signers using new nonces.
3. `msg.sender` must be the benefactor OR `delegatedSigners[msg.sender] == ACCEPTED`. After re-add, old `ACCEPTED` signers still pass. ✗ Defensive failure.
4. `beneficiary` must equal `benefactor` OR `approvedBeneficiaries[beneficiary] == true`. After re-add, old approved beneficiaries still pass. ✗ Defensive failure.

**What it does NOT check:**
- It does NOT check any `configVersion` / generation / nonce-binding field — because none exists.
- It does NOT verify that the signer was confirmed **after** the most recent `addBenefactor` call.
- It does NOT verify that the beneficiary was approved **after** the most recent `addBenefactor` call.
- It does NOT consult the `BenefactorAdded` event history (events are not readable from runtime).
- It does NOT consult any "removed at" timestamp (no such field).

**Finding for Item 9:** **CONFIRMED.** `_validateBenefactor` only checks current-state booleans/mappings. There is no temporal/version check that would invalidate pre-removal permissions.

---

### Item 10 — `swap` (PSM.sol:268–336)

**Code (verbatim, key sections):**
```solidity
function swap(Order calldata order) external override nonReentrant {
    if (!isSwapEnabled) revert SwapDisabledError();

    // ========= ORDER VALIDATION =========
    _validateOrder(order);  // amountIn >= BASIS_POINTS, minAmountOut != 0, expiry, chainId

    bool _isSwapForAsset = order.isSwapForAsset;

    // ========= COLLATERAL VALIDATION =========
    CollateralState storage _collateralState = collateralState.get(order.collateral);
    CollateralConfig storage _collateralConfig = _collateralState.config;
    if (!_collateralConfig.isActive) revert CollateralNotSupported(order.collateral);
    (uint256 oraclePrice, uint256 updatedAt) = IOracleFeed(_collateralConfig.oracleFeed).getPrice();
    _validateOraclePrice(_collateralConfig, order.collateral, _isSwapForAsset, oraclePrice, updatedAt);

    // ========= BENEFACTOR VALIDATION =========
    BenefactorState storage _benefactorState = benefactorState[order.benefactor];
    _validateBenefactor(order, _benefactorState);  // <- the only benefactor-permission gate

    // ========= PRICING & FEES =========
    BenefactorConfig storage _benefactorConfig = _benefactorState.config;
    bool zeroSwapForAssetFeeExempt = _benefactorConfig.zeroSwapForAssetFeeExemptions[order.collateral];   // <- reads persisted mapping
    bool zeroSwapForCollateralFeeExempt = _benefactorConfig.zeroSwapForCollateralFeeExemptions[order.collateral];  // <- persisted
    uint128 customSwapForAssetFee = _benefactorConfig.swapForAssetFeeByCollateral[order.collateral];  // <- persisted
    uint128 customSwapForCollateralFee = _benefactorConfig.swapForCollateralFeeByCollateral[order.collateral];  // <- persisted
    // ... fee calc ...

    // ========= EFFECTS: EPOCHS & LIMITS =========
    _handleEpochPeriodOperations(order, _benefactorState, _collateralState, globalState, amountOut);
    _benefactorState.orderNonceInvalidator[order.nonce] = true;  // <- marks nonce used (persists across remove/add too)

    // ========= INTERACTIONS: TRANSFERS =========
    if (_isSwapForAsset) {
        IERC20(order.collateral)
            .safeTransferFrom(order.benefactor, _collateralConfig.receiveCustodianAddress, order.amountIn);
        asset.safeTransferFrom(assetSendCustodianAddress, order.beneficiary, amountOut);
    } else {
        asset.safeTransferFrom(order.benefactor, assetReceiveCustodianAddress, order.amountIn);
        IERC20(order.collateral)
            .safeTransferFrom(_collateralConfig.sendCustodianAddress, order.beneficiary, amountOut);
    }
    emit SwapExecuted(msg.sender, order.benefactor, order.beneficiary, order, amountOut, feeAmount);
}
```

**Validation flow trace:**
1. Order basics (amountIn, minAmountOut, expiry, chainId) — nothing about benefactor state.
2. Collateral isActive + oracle depeg — nothing about benefactor state.
3. `_validateBenefactor` — checks `isActive`, nonce-not-used, msg.sender-is-benefactor-or-ACCEPTED-delegated-signer, beneficiary-is-approved. **This is the ONLY benefactor-permission gate.** Already shown (Item 9) to fail to invalidate stale mappings.
4. Fee computation reads the 4 fee-related mappings directly from `_benefactorConfig` — these also persist (the bug's "softer" economic impact).
5. Epoch/period limit enforcement — bounded by `_benefactorState.config.maxSwapFor{Asset,Collateral}Per{Epoch,Period}` (which ARE value types and ARE reset to 0 by `delete`, so they fall back to the global defaults — a non-trivial limit).
6. Transfers: pulls `amountIn` FROM `order.benefactor`, sends `amountOut` TO `order.beneficiary`. **The attacker (msg.sender = delegated signer = beneficiary) receives the asset output without being the benefactor.**

**Is there ANY other check in `swap` that could prevent the attack?**
- No reentrancy check beyond `nonReentrant` — irrelevant to this attack.
- No "signer must be `msg.sender`'s key" check — irrelevant.
- No "beneficiary must be approved in current epoch" check.
- No "is this a re-activated benefactor" check — there's no field to check.
- No "configVersion" binding — no such field.
- No on-chain revocation registry for compromised signers.
- No timelock between `addBenefactor` and `swap` — the attacker can call `swap` in the very same tx bundle as `addBenefactor` (a MEV/builder could atomic-bundle them).

**Conclusion:** `swap` has no defensive layer beyond `_validateBenefactor`, and `_validateBenefactor` does not invalidate stale mappings.

**Finding for Item 10:** **CONFIRMED.** The `swap` path validates only `isActive`, nonce, signer status, and beneficiary approval — all of which are either restored or persisted after re-add. No additional defensive layer exists.

---

### Item 11 — All other functions touching `benefactorState`

I read every function in the contract (2083 lines) and identified all `benefactorState` accesses:

| Function (line) | What it does to `benefactorState` | Persists across remove/add? |
|---|---|---|
| `swap` (268) | Reads config (fees, exemptions), reads nonce invalidator, calls `_validateBenefactor` | n/a (just reads) |
| `setAssetSendCustodian` (345) | Reads `benefactorState[newCustodian].config.isActive` to prevent custodian-benefactor collision | n/a |
| `setAssetReceiveCustodian` (369) | Same as above | n/a |
| `enableBenefactor` (589) | Writes `config.isActive = true` | Yes (mappings untouched) |
| `disableBenefactor` (611) | Writes `config.isActive = false` | Yes (intentional) |
| `addBenefactor` (624) | Writes `config.isActive = true` | Yes (mappings untouched) |
| `removeBenefactor` (645) | `delete .config` | Yes (mappings untouched — the bug) |
| `setBenefactorMaxSwapForAssetPerEpoch` (658) | Writes `config.maxSwapForAssetPerEpoch` | n/a (per-benefactor config) |
| `setBenefactorMaxSwapForCollateralPerEpoch` (680) | Writes `config.maxSwapForCollateralPerEpoch` | n/a |
| `setBenefactorSwapForAssetFee` (704) | Writes `config.swapForAssetFeeByCollateral[collateral]` | Yes (this mapping persists) |
| `setBenefactorSwapForCollateralFee` (730) | Writes `config.swapForCollateralFeeByCollateral[collateral]` | Yes (persists) |
| `setBenefactorZeroSwapForAssetFeeExemption` (755) | Writes `config.zeroSwapForAssetFeeExemptions[collateral]` | Yes (persists) |
| `setBenefactorZeroSwapForCollateralFeeExemption` (778) | Writes `config.zeroSwapForCollateralFeeExemptions[collateral]` | Yes (persists) |
| `setBenefactorMaxSwapForAssetPerPeriod` (800) | Writes `config.maxSwapForAssetPerPeriod` | n/a |
| `setBenefactorMaxSwapForCollateralPerPeriod` (822) | Writes `config.maxSwapForCollateralPerPeriod` | n/a |
| `setDelegatedSigner` (849) | Writes `config.delegatedSigners[signer] = PENDING` | Yes (persists — core of bug) |
| `confirmDelegatedSigner` (868) | Writes `config.delegatedSigners[msg.sender] = ACCEPTED` | Yes (persists) |
| `removeDelegatedSigner` (887) | Writes `config.delegatedSigners[signer] = REJECTED` | n/a (per-signer revoke) |
| `setApprovedBeneficiary` (906) | Writes `config.approvedBeneficiaries[beneficiary]` | Yes (persists — core of bug) |
| `getQuote` (1108) | Reads fee mappings | n/a |
| `getBenefactorConfig` (1153) | Reads value-type config fields only | n/a |
| `getBenefactorFeesForCollateral` (1182) | Reads fee mappings | n/a |
| `getDelegatedSignerStatus` (1213) | Reads `config.delegatedSigners[signer]` | n/a (proves persistence) |
| `isApprovedBeneficiary` (1230) | Reads `config.approvedBeneficiaries[beneficiary]` | n/a (proves persistence) |
| `getBenefactorEpochTotal` (1283) | Reads `epochStateByDuration` | n/a |
| `getBenefactorPeriodTotal` (1345) | Reads `periodStateByDuration` | n/a |
| `_validateBenefactor` (1493) | Reads config + nonce invalidator | n/a |
| `_handleEpochPeriodOperations` (1662) | Writes `epochStateByDuration` / `periodStateByDuration` | Yes (persists — but auto-rolls per epoch/period) |
| `addCollateral` (497) | Reads `benefactorState[config.sendCustodianAddress].config.isActive` | n/a (collision check) |
| `updateCollateralConfig` (552) | Same collision check | n/a |
| `_isCustodian` (1436) | Reads config.isActive of all benefactors? **NO** — `_isCustodian` does NOT iterate benefactors; it only checks asset/collateral custodians. (Read the code: it loops `collateralState`, not `benefactorState`.) | n/a |

**Critical observation:** none of the `setBenefactor*` config-setter functions check `isActive` before writing. So mappings can be (and routinely are) modified while a benefactor is inactive. This is documented behavior (NatSpec explicitly says so for `setDelegatedSigner` and `setApprovedBeneficiary`). This means there is no "active window" guarantee — a benefactor's mappings are always writable by their owner.

**Finding for Item 11:** **CONFIRMED.** Every mapping-writing function (`setDelegatedSigner`, `confirmDelegatedSigner`, `removeDelegatedSigner`, `setApprovedBeneficiary`, `setBenefactorSwapForAssetFee`, `setBenefactorSwapForCollateralFee`, `setBenefactorZeroSwapForAssetFeeExemption`, `setBenefactorZeroSwapForCollateralFeeExemption`) writes to `BenefactorConfig` mappings that `removeBenefactor` does not clear. None of them check `isActive` before writing. There is no auto-cleanup function anywhere in the contract.

---

### Item 12 — Struct definitions

**From IPSM.sol (verbatim, lines 109–155):**

```solidity
struct BenefactorConfig {
    bool isActive;                                                      // value type — slot 0 (cleared by delete)
    uint128 maxSwapForAssetPerEpoch;                                    // value type — slot 0 (cleared)
    uint128 maxSwapForCollateralPerEpoch;                               // value type — slot 1 (cleared)
    mapping(address => uint128) swapForAssetFeeByCollateral;            // MAPPING — slot 2 (NOT cleared)
    mapping(address => uint128) swapForCollateralFeeByCollateral;       // MAPPING — slot 3 (NOT cleared)
    mapping(address => DelegatedSignerStatus) delegatedSigners;         // MAPPING — slot 4 (NOT cleared)
    mapping(address => bool) approvedBeneficiaries;                     // MAPPING — slot 5 (NOT cleared)
    mapping(address => bool) zeroSwapForAssetFeeExemptions;             // MAPPING — slot 6 (NOT cleared)
    mapping(address => bool) zeroSwapForCollateralFeeExemptions;        // MAPPING — slot 7 (NOT cleared)
    uint128 maxSwapForAssetPerPeriod;                                   // value type — slot 8 (cleared)
    uint128 maxSwapForCollateralPerPeriod;                              // value type — slot 8 (cleared)
}

struct BenefactorState {
    BenefactorConfig config;                                            // slots 0..8 (config lives here)
    mapping(uint256 => EpochState) epochStateByDuration;                // MAPPING — slot 9 (NOT touched by delete .config)
    mapping(uint256 => PeriodState) periodStateByDuration;              // MAPPING — slot 10 (NOT touched)
    mapping(uint128 => bool) orderNonceInvalidator;                     // MAPPING — slot 11 (NOT touched)
}
```

**Storage layout implications:**
- `delete benefactorState[benefactor].config` zeroes slots 0..8 (the `BenefactorConfig` value-type fields). It is a no-op for the mapping-typed fields within `BenefactorConfig` (slots 2..7) because Solidity's `delete` cannot iterate mappings.
- It does NOT touch slots 9..11 (the `BenefactorState`-level mappings) because the delete target is specifically `.config`, not the whole `BenefactorState`.
- So after `removeBenefactor`:
  - `isActive` = false ✓
  - `maxSwapFor*Per{Epoch,Period}` = 0 ✓ (will fall back to global defaults)
  - `delegatedSigners[X]` for any previously-set X = unchanged ✗
  - `approvedBeneficiaries[X]` for any previously-set X = unchanged ✗
  - `swapForAssetFeeByCollateral[X]`, `swapForCollateralFeeByCollateral[X]`, `zeroSwapForAssetFeeExemptions[X]`, `zeroSwapForCollateralFeeExemptions[X]` = unchanged ✗
  - `epochStateByDuration`, `periodStateByDuration` = unchanged (but auto-rolls per time)
  - `orderNonceInvalidator[X]` for any previously-used nonce = unchanged (still `true` — defensive in the wrong direction)

**No version field, no enumerable key set, no generation counter:** confirmed by reading the struct definitions in full.

**Finding for Item 12:** **CONFIRMED.** `delegatedSigners`, `approvedBeneficiaries`, and the 4 fee mappings are all **inside `BenefactorConfig`** — exactly the struct that `removeBenefactor`'s `delete` targets but cannot fully clear.

---

### Item 13 — Events

**From IPSM.sol (lines 157–234, verbatim relevant entries):**

```solidity
event BenefactorAdded(address indexed benefactor);
event BenefactorRemoved(address indexed benefactor);
event BenefactorEnabled(address indexed benefactor);
event BenefactorDisabled(address indexed benefactor);

event DelegatedSignerAdded(address indexed signer, address indexed benefactor);
event DelegatedSignerConfirmed(address indexed signer, address indexed benefactor);
event DelegatedSignerRemoved(address indexed signer, address indexed benefactor);

event BeneficiaryApproved(address indexed benefactor, address indexed beneficiary);
event BeneficiaryRemoved(address indexed benefactor, address indexed beneficiary);
```

**Observations:**
- `BenefactorRemoved` emits ONLY the benefactor address. It does NOT emit:
  - The list of delegated signers being (not) cleared.
  - The list of approved beneficiaries being (not) cleared.
  - A warning that mappings persist.
  - A "config version" or "removal nonce" that downstream consumers could use to detect stale permissions.
- `BenefactorAdded` similarly emits only the benefactor address. No warning about pre-existing state.
- There is no `BenefactorStateCleared` event because no clearing happens.
- There is no `BenefactorConfigReset` event because no reset happens.

**Implication for monitoring/defense:** an off-chain monitor watching events would see `BenefactorRemoved` → `BenefactorAdded` and reasonably assume the benefactor's state was reset. There is no event signal to alert them that the mappings persisted. This is the "invisible persistence" angle.

**Finding for Item 13:** **CONFIRMED.** Events provide no warning about persistence. `BenefactorRemoved` carries only the address; no list of uncleared permissions.

---

### Item 14 — NatSpec comments

**`removeBenefactor` NatSpec (PSM.sol:638–644, verbatim):**
```solidity
/**
 * @notice Removes a benefactor from the system
 * @dev Only callable by benefactor managers
 * @param benefactor Address of the benefactor to remove
 * @dev Reverts if benefactor is not active
 * @dev Emits BenefactorRemoved event on success
 */
```

- Says "Removes a benefactor from the system" — strongly implies full removal.
- No `@dev` warning that mappings persist.
- No `@dev` warning that re-adding restores old permissions.
- No `@dev` recommendation to use a fresh address on re-add.
- No reference to `disableBenefactor` as the preferred "temporary" alternative.

**`addBenefactor` NatSpec (PSM.sol:617–623, verbatim):**
```solidity
/**
 * @notice Adds a new benefactor to the system
 * @dev Only callable by benefactor managers
 * @param benefactor Address of the benefactor
 * @dev Reverts if benefactor already exists
 * @dev Emits BenefactorAdded event on success
 */
```

- Says "Adds a **new** benefactor" — implies fresh state, which is FALSE for re-adds.
- No warning that pre-existing mapping entries will be inherited.
- No recommendation to verify clean state before adding.

**`disableBenefactor` NatSpec (PSM.sol:604–610, verbatim):**
```solidity
/**
 * @notice Disables a benefactor
 * @dev Only callable by benefactor disablers
 * @param benefactor Address of the benefactor
 * @dev Reverts if not active
 * @dev Emits BenefactorDisabled event on success
 */
```

- Says "Disables" — neutral, doesn't promise cleanup. Consistent with the implementation.

**`enableBenefactor` NatSpec (PSM.sol:582–588, verbatim):**
```solidity
/**
 * @notice Enables a benefactor
 * @dev Only callable by the default admin
 * @param benefactor Address of the benefactor
 * @dev Reverts if already enabled
 * @dev Emits BenefactorEnabled event on success
 */
```

- Says "Enables" — neutral, consistent with the implementation.

**`setDelegatedSigner` NatSpec (PSM.sol:837–848) — KEY WARNING:**
```solidity
/**
 * @notice Sets a delegated signer for the caller. Can be called even if not active
 *         to allow inactive benefactors to manage their delegated signers.
 * @dev Callable by any address; signer status is set to PENDING until the signer calls
 *      confirmDelegatedSigner. Once confirmed (ACCEPTED), the signer gains full swap
 *      execution authority on behalf of the benefactor — it can submit any valid swap
 *      order, consume benefactor nonces, spend benefactor token allowances, and direct
 *      output to any benefactor-approved beneficiary. Treat an accepted delegated signer
 *      with the same trust level as the benefactor account itself.
 * @param signer Address of the delegated signer
 * @dev Emits DelegatedSignerAdded event on success
 */
```

- Explicitly says delegated signers should be "treated with the same trust level as the benefactor account itself."
- Does NOT mention that `removeBenefactor` fails to revoke this trust.
- The "Can be called even if not active" line is intentional (so an inactive benefactor can pre-revoke), but it doesn't address the remove/re-add cycle.

**`confirmDelegatedSigner` NatSpec (PSM.sol:855–867):**
- Says "Revoke via removeDelegatedSigner when no longer needed."
- Does NOT say "removeBenefactor does NOT revoke; you must call removeDelegatedSigner first."

**Search for "fresh address", "state persists", "re-add", "reactivate":**
- Grep for these terms in PSM.sol and IPSM.sol: **zero matches.** No documentation warns about this hazard anywhere.

**Finding for Item 14:** **CONFIRMED.** NatSpec says "Removes a benefactor from the system" / "Adds a new benefactor" — both imply state reset. No warning anywhere about mapping persistence. The contract documentation actively contradicts the actual behavior.

---

### Item 15 — configVersion / nonce / generation mechanism

**Search results (exhaustive):**
- `grep -n "configVersion" contracts/` → 0 matches.
- `grep -n "generation" contracts/PSM.sol contracts/deps/IPSM.sol` → 0 matches.
- `grep -n "version" contracts/PSM.sol` → 0 matches (no `configVersion`, no `stateVersion`, no `benefactorVersion`).
- `grep -n "removedAt\\|addedAt\\|timestamp" contracts/PSM.sol` → 0 matches in BenefactorConfig context.
- The only "nonce" in `BenefactorState` is `orderNonceInvalidator` — a per-order-replay-prevention map (uint128 → bool). It is NOT a config version; it doesn't invalidate stale mappings, only stale order signatures.
- The "epoch" and "period" concepts are time-based (block.timestamp / duration), not per-benefactor generations. They reset swap-volume counters per time window, not permissions.

**Implication:** there is **no field anywhere** in `BenefactorConfig` or `BenefactorState` that could be used to distinguish "permissions set in this active window" from "permissions set in a prior active window." The contract has no concept of a "config generation."

**Finding for Item 15:** **CONFIRMED.** No version/nonce/generation mechanism exists that could invalidate stale mappings.

---

## 3. Alternative Interpretations — Is There ANY Way the Code Handles This?

I exhaustively considered every possible "the code actually handles this somewhere we missed" angle:

### Alt-1: "Maybe `delete` in this Solidity version (0.8.30) DOES clear mappings."
- **False.** Solidity's `delete` semantics for mappings have not changed since pre-0.4.x. The [Solidity docs](https://docs.soliditylang.org/en/v0.8.30/types.html#delete) are explicit: "Deleting a struct recursively deletes all non-mapping members." Mappings are explicitly excluded. This is a fundamental language property driven by mappings having no enumerable keys.

### Alt-2: "Maybe `addBenefactor` checks for stale state before allowing re-add."
- **False.** Read the full function body (Item 2 above). It only checks `isActive` (which is false after remove, so the check passes) and the custodian-collision check. No state-freshness assertion.

### Alt-3: "Maybe `_validateBenefactor` checks a timestamp/version."
- **False.** Read the full function body (Item 9 above). It only checks `isActive`, nonce-used, delegated-signer-status, and beneficiary-approved. No temporal/version check.

### Alt-4: "Maybe there's a `_clearBenefactorState` helper that `removeBenefactor` calls."
- **False.** Grep for `delete ` in PSM.sol returns exactly one match: line 647. There is no other cleanup helper.

### Alt-5: "Maybe `swap` has an additional check that prevents the attack."
- **False.** Read the full function body (Item 10 above). The only benefactor-permission gate is `_validateBenefactor`. No additional defensive layer.

### Alt-6: "Maybe `confirmDelegatedSigner` requires re-confirmation after re-add."
- **False.** `confirmDelegatedSigner` requires status == `PENDING`. After re-add, status is still `ACCEPTED` (not `PENDING`), so re-confirmation would revert with `DelegationNotAuthorized`. The attacker doesn't need to re-confirm because their status is already `ACCEPTED` — and they CAN'T re-confirm even if they wanted to.

### Alt-7: "Maybe `removeDelegatedSigner` is auto-called by `removeBenefactor`."
- **False.** `removeBenefactor` only does `delete .config`. It does not call `removeDelegatedSigner` for any signer (and couldn't, because there's no enumerable list of signers).

### Alt-8: "Maybe `orderNonceInvalidator` blocks the attack by rejecting old nonces."
- **Partially true, but in the wrong direction.** `orderNonceInvalidator` persists across remove/add (it lives in `BenefactorState`, not `BenefactorConfig`, so `delete .config` doesn't touch it). Old nonces are STILL marked as used, so the attacker cannot replay old signed orders. But the attacker can use a **NEW nonce** that the benefactor never signed before — except wait, the attacker is the delegated signer, so they can sign new orders themselves with any nonce they choose. The nonce check therefore does NOT block the attack; it just means the attacker must use a fresh nonce per swap (which is the intended behavior anyway).

### Alt-9: "Maybe rate limits (maxSwapForAssetPerEpoch, etc.) being reset to 0 (→ global default) prevents the attack."
- **False.** The global defaults (`defaultBenefactorMaxSwapForAssetPerEpoch`, etc.) are non-zero (enforced in constructor at L210-215 and L224-229). They define the per-benefactor ceiling. So a re-added benefactor can swap up to the global default per epoch/period. The attacker drains up to this limit per epoch/period.

### Alt-10: "Maybe the role separation (`BENEFACTOR_MANAGER_ROLE` vs `BENEFACTOR_DISABLER_ROLE`) signals different intent and the dev knew about persistence."
- **Plausible interpretation but doesn't change the bug.** The role separation does suggest `remove` is meant to be more permanent than `disable` — which strengthens the argument that the persistence is unintended. If the dev had intended persistence, they would have used `disable`-style semantics (one bool flip) for `remove` too. The use of `delete` strongly signals intent to clean up — which the language silently fails to deliver for mappings.

### Alt-11: "Maybe the contract is upgradable and a future version fixes this."
- **Out of scope.** PSM.sol is a single non-upgradable contract (no `upgradeTo`, no proxy pattern, no OpenZeppelin `UUPSUpgradeable` import — verified by reading imports at lines 4–12). Even if it were upgradable, the current deployed behavior is buggy.

### Alt-12: "Maybe the bug is mitigated by the fact that `setDelegatedSigner` is permissive (callable while inactive)."
- **Actually amplifies the bug.** Because `setDelegatedSigner` and `setApprovedBeneficiary` are callable while inactive, an attacker who controls the benefactor's key can pre-set delegated signers on an inactive address, and if that address is later added as a benefactor, the pre-set PENDING status is already there. (Note: `confirmDelegatedSigner` requires `isActive`, so the signer can only confirm AFTER the address becomes active — but the PENDING state persists from before.) This is a smaller-amplifier issue noted in the existing `ethena-untested-removebenefactor-mapping-persistence.md` report (lines 339–357).

### Alt-13: "Maybe the swap path requires the benefactor to have approved the PSM contract (allowance), and after remove the allowance would be revoked."
- **False assumption.** The PSM pulls collateral via `safeTransferFrom(order.benefactor, ...)` (line 327) which uses the benefactor's ERC-20 allowance to the PSM. Nothing in `removeBenefactor` revokes that allowance (it can't — allowances live in the token contract, not in PSM). The benefactor would have to separately call `collateral.approve(psm, 0)` to revoke. If they don't (or if the attacker is the delegated signer and the benefactor's tokens are still approved), the swap succeeds.

### Alt-14: "Maybe the test in PoC_removeBenefactor.t.sol is buggy or uses unrealistic setup."
- **False.** Read the full PoC (4 tests, 369 lines). The setup uses realistic role assignments, realistic token custodians, fresh oracle prices, valid orders, and proper `vm.prank` boundaries. The Foundry run output (lines 133–144 of the existing report) shows all 4 tests passing including the end-to-end fund drain. Test 4 (`test_Sanity_FreshBenefactorBlocksUnknownAttacker`) specifically proves the bug is persistence, not a general auth bypass — a fresh benefactor with no prior delegation correctly reverts with `DelegationNotAuthorized`.

**Conclusion of alternative-interpretations sweep:** NONE of the 14 alt-explanations survive scrutiny. The bug is real.

---

## 4. Final Verdict

### **BUG CLAIM: CONFIRMED — the bug is REAL, not a misunderstanding.**

### Confidence: **99%** (reserving 1% only for the philosophical possibility that the Solidity compiler has a bug we haven't observed, which would contradict 8+ years of documented language behavior).

### Justification:
1. **Solidity `delete` semantics are unambiguous:** mappings inside a struct are NOT cleared. This is documented language behavior since pre-0.4.x and unchanged in 0.8.30. No compiler flag or pragma changes this.
2. **Struct layout confirms the bug surface:** all 6 affected mappings live inside `BenefactorConfig` (IPSM.sol:112–124), which is exactly the struct `removeBenefactor` targets with `delete`.
3. **`addBenefactor` performs no cleanup:** only `isActive = true`. Mappings inherited as-is.
4. **`_validateBenefactor` has no version/temporal check:** only current-state checks, all of which pass after re-add.
5. **`swap` has no additional defensive layer:** the validation flow is `_validateOrder` → collateral checks → `_validateBenefactor` → fee calc → epoch/period limits → transfers. None of these block the attack.
6. **No version/nonce/generation mechanism exists anywhere** in the contract.
7. **No event warns about persistence:** monitoring/off-chain defenses get no signal.
8. **NatSpec is actively misleading:** "Removes a benefactor from the system" and "Adds a new benefactor" both imply clean state, which is false for re-adds.
9. **Foundry PoC (4 tests) confirms end-to-end fund drain** with realistic setup and proper sanity controls.
10. **The disable/enable pair is self-consistent** (no `delete`, intentional persistence, neutral NatSpec). The remove/add pair is structurally similar but semantically inconsistent with its own NatSpec — the strongest signal that the persistence is unintended.

### The semantic case for "real bug, not intentional design":
- If the developers had intended persistence across remove+re-add, they would have used `disable`/`enable` semantics (single bool flip) for `remove`/`add` too.
- The fact that they reached for `delete` is direct evidence of intent to clean up — which the language silently does not provide for mappings.
- The `BENEFACTOR_MANAGER_ROLE` (more admin-flavored) vs `BENEFACTOR_DISABLER_ROLE` (more ops-flavored) split mirrors the intent: `remove` is admin-level severance, `disable` is operational pause. Different intent → different expected state outcome. The bug collapses them.

---

## 5. End-to-End Attack Flow (Confirmed Working)

```
T0:  Admin calls addBenefactor(B)              → B.config.isActive = true
T1:  B calls setDelegatedSigner(S)             → B.config.delegatedSigners[S] = PENDING
T2:  S calls confirmDelegatedSigner(B)         → B.config.delegatedSigners[S] = ACCEPTED
T3:  B calls setApprovedBeneficiary(S, true)   → B.config.approvedBeneficiaries[S] = true
T4:  B approves PSM to spend B's collateral    → ERC20.allowance[B][PSM] = MAX
                                                  (external to PSM; not auto-revoked)

--- incident: S's key is compromised ---

T5:  Admin calls removeBenefactor(B)           → delete B.config
                                                  isActive = false
                                                  maxSwap* value fields = 0
                                                  delegatedSigners[S] = ACCEPTED  (PERSISTS)
                                                  approvedBeneficiaries[S] = true  (PERSISTS)
                                                  swapForAssetFeeByCollateral[*] = unchanged
                                                  (all 6 mappings PERSIST)
                                                  orderNonceInvalidator[*] = unchanged (persists in BenefactorState)

--- days/weeks pass; B's key is rotated (the most common reason for remove+re-add) ---

T6:  Admin calls addBenefactor(B)              → B.config.isActive = true
                                                  (all 6 mappings STILL PERSIST from T1-T3)

T7:  S (still compromised) calls swap(Order{
       isSwapForAsset: true,
       nonce: <NEW nonce, never used before>,
       benefactor: B,
       beneficiary: S,             // attacker receives asset output
       collateral: <active collateral>,
       amountIn: <up to global per-epoch default>,
       minAmountOut: <reasonable>
     })
     - _validateOrder: passes (valid order)
     - collateral.isActive: passes (collateral was never removed)
     - _validateBenefactor:
         isActive: passes (just set to true at T6)
         nonce: passes (NEW nonce, not in invalidator)
         msg.sender != benefactor && delegatedSigners[S] == ACCEPTED: passes (S still ACCEPTED from T2)
         beneficiary != benefactor && approvedBeneficiaries[S] == true: passes (still true from T3)
     - fee calc: uses persisted fee mappings (zeroSwapForAssetFeeExemptions could grant free swap)
     - epoch/period limits: bounded by global defaults (still generous; e.g., $1M+ per epoch for institutional setup)
     - safeTransferFrom(B, collateralReceiveCustodian, amountIn): pulls collateral FROM B
     - safeTransferFrom(assetSendCustodian, S, amountOut): sends asset TO S (the attacker)

T8:  S receives `amountOut` of asset tokens. B loses `amountIn` of collateral.
     SwapExecuted event emitted; on-chain monitoring sees a normal-looking swap.

T9:  S can repeat with new nonces up to the per-epoch/per-period limit, every epoch/period,
     until the admin notices and calls disableBenefactor or removeBenefactor again.
     Even then, the cycle can repeat on the next re-add.
```

**Confirmed by:** `test_RemoveBenefactor_AttackerCanSwapAfterReAdd` (Foundry PoC, gas 869850, drains 1,000 asset tokens 1:1 from benefactor's collateral after a 7-day warp between remove and re-add).

---

## 6. Why This Is NOT "Misunderstood Intentional Design"

The strongest counterargument would be: "The developers knew mappings persist; `remove` is just a soft-delete; the operator is supposed to use a fresh address on re-add."

This argument fails on multiple grounds:

1. **The NatSpec contradicts it.** "Removes a benefactor from the system" is not the language of "soft-delete." If soft-delete were intended, the NatSpec would say "Marks a benefactor as inactive" or "Disables a benefactor permanently" — and it would warn operators about re-add hazards. It does neither.

2. **The use of `delete` contradicts it.** If soft-delete were intended, the implementation would mirror `disableBenefactor` (one bool flip). The use of `delete .config` is a deliberate choice that signals cleanup intent — which the language silently fails to deliver for mappings.

3. **The `disable`/`enable` pair already covers soft-delete.** If `remove`/`add` were also soft-delete, the contract would have two redundant soft-delete mechanisms differing only in role. That's not a sensible API; the natural reading is that `remove` is meant to be MORE destructive than `disable` (admin-level severance vs operational pause).

4. **No "fresh address" enforcement exists.** Nothing in `addBenefactor` checks whether the address was previously removed. No `removedBenefactors` blocklist. No minimum delay. No "this address was removed at block N; please use a fresh address" warning. If "fresh address" were the intended mitigation, it would be enforced or at least documented.

5. **The `setDelegatedSigner`/`setApprovedBeneficiary` permission model is incompatible with "fresh address" being assumed.** These functions are callable while inactive (documented), which means a non-benefactor can pre-set PENDING delegated signers on their address. If that address is later added as a benefactor, the pre-set state activates. So the contract explicitly supports the pattern "set up config first, then become a benefactor" — which is the OPPOSITE of "always use a fresh address."

6. **Foundry PoC proves end-to-end fund movement.** This is not a theoretical concern about future state; the test drains 1,000 asset tokens in a realistic setup. The bug is exploitable today on the deployed code.

---

## 7. Summary Table

| # | Item | Finding | Notes |
|---|------|---------|-------|
| 1 | `removeBenefactor` only does `delete .config` | **CONFIRMED** | No explicit mapping cleanup; Solidity `delete` doesn't clear mappings |
| 2 | `addBenefactor` only flips `isActive` | **CONFIRMED** | No re-init, no version bump |
| 3 | `disableBenefactor` differs structurally | **CONFIRMED** | One bool flip, no `delete`; safe because intentional |
| 4 | `enableBenefactor` mirrors `addBenefactor` | **CONFIRMED** | Self-consistent with disable; not with remove |
| 5 | `setDelegatedSigner` writes to `config.delegatedSigners` | **CONFIRMED** | Mapping lives in the struct that `delete` doesn't clear |
| 6 | `confirmDelegatedSigner` requires PENDING | **CONFIRMED** | ACCEPTED status can't be re-confirmed; persists silently |
| 7 | `removeDelegatedSigner` is per-signer only | **CONFIRMED** | No batch clear, no enumerable set |
| 8 | `setApprovedBeneficiary` writes to `config.approvedBeneficiaries` | **CONFIRMED** | Same persistence issue |
| 9 | `_validateBenefactor` lacks version/temporal check | **CONFIRMED** | Only current-state checks; all pass after re-add |
| 10 | `swap` has no additional defense | **CONFIRMED** | Only `_validateBenefactor` gates benefactor permissions |
| 11 | All other benefactorState-touching functions | **CONFIRMED** | No auto-cleanup anywhere; setters don't check isActive |
| 12 | Struct definitions: 6 mappings in BenefactorConfig | **CONFIRMED** | All inside the struct `delete` targets but can't clear |
| 13 | Events provide no persistence warning | **CONFIRMED** | `BenefactorRemoved` emits only the address |
| 14 | NatSpec actively misleading | **CONFIRMED** | "Removes"/"Adds new" implies cleanup; no warning |
| 15 | No version/nonce/generation mechanism | **CONFIRMED** | Grep confirms zero matches |

---

## 8. Final Report

**Bug claim status:** **CONFIRMED**

**Final verdict:** **REAL BUG** — not a misunderstanding of intentional design.

**Confidence:** **99%**

**Key findings:**
1. `removeBenefactor` performs `delete benefactorState[benefactor].config` (PSM.sol:647) — Solidity's `delete` does NOT clear the 6 nested mappings inside `BenefactorConfig` (IPSM.sol:112–124). This is documented, immutable language behavior.
2. `addBenefactor` (PSM.sol:624) performs only `config.isActive = true` — no cleanup, no version bump, no state-freshness check.
3. `_validateBenefactor` (PSM.sol:1493) checks only current-state booleans/mappings — all pass after re-add.
4. The disable/enable pair is self-consistent (no `delete`, intentional persistence, neutral NatSpec). The remove/add pair is structurally similar but semantically inconsistent with its own "Removes"/"Adds new" NatSpec — the strongest signal that persistence is unintended.
5. No version/nonce/generation mechanism exists anywhere in the contract to invalidate stale mappings.
6. No event warns about persistence; no enumerable set exists to support batch-clearing.
7. Foundry PoC (`PoC_removeBenefactor.t.sol`) demonstrates end-to-end fund drain of 1,000 asset tokens after remove + 7-day warp + re-add, with proper sanity controls proving the bug is persistence-specific (not a general auth bypass).
8. The "use a fresh address on re-add" defense is undocumented, unenforced, and contradicted by the contract's support for pre-setting config on inactive addresses (via the permissive `setDelegatedSigner` / `setApprovedBeneficiary`).

**End-to-end attack flow works as described in the bug claim.** The bug is real, exploitable, and confirmed by both static analysis (every line of PSM.sol read and traced) and dynamic verification (Foundry PoC with fund drain).

---

## Files Referenced

- `/home/z/fkr-step1/defi-bounty/contracts/PSM.sol` (2083 lines, read in full)
- `/home/z/fkr-step1/defi-bounty/contracts/deps/IPSM.sol` (494 lines, read in full)
- `/home/z/fkr-step1/defi-bounty/contracts/deps/ISingleAdminAccessControl.sol` (38 lines, read in full; no PSM-relevant content)
- `/home/z/fkr-step1/defi-bounty/vuln/PoC_removeBenefactor.t.sol` (369 lines, read in full — confirms end-to-end exploit)
- `/home/z/fkr-step1/defi-bounty/vuln/ethena-untested-removebenefactor-mapping-persistence.md` (prior detailed report — conclusions align with this verification)
