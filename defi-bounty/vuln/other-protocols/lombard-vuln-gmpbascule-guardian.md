# Lombard Finance — GMPBasculeV2 Threshold Guardian Role Not Renounced After Raise

**Severity:** LOW
**Area:** Bascule (Drawbridge) — Mint Validation Threshold
**Contracts:** `GMPBasculeV2.sol`
**Functions:** `GMPBasculeV2.updateValidateThreshold()`
**Lines:** `GMPBasculeV2.sol:126-136`

---

## Description

The `GMPBasculeV2` contract is an on-chain component of Lombard's "drawbridge" system that validates GMP mints against off-chain deposit reports. It has a `validateThreshold` — mints with amounts below this threshold bypass validation, while mints at or above the threshold require a matching deposit report.

The `updateValidateThreshold` function in `GMPBasculeV2` requires only `VALIDATION_GUARDIAN_ROLE` for ANY threshold change (both raising and lowering):

```solidity
// GMPBasculeV2.sol:126-136
function updateValidateThreshold(
    uint256 newThreshold
) public onlyRole(VALIDATION_GUARDIAN_ROLE) whenNotPaused {
    if (newThreshold == validateThreshold) {
        revert SameValidationThreshold();
    }
    // Actually update the threshold
    _updateValidateThreshold(newThreshold);
}
```

**Contrast with `BasculeV3` (the non-GMP version), which has a more secure design:**

```solidity
// BasculeV3.sol:212-240
function updateValidateThreshold(uint256 newThreshold) public whenNotPaused {
    if (newThreshold == validateThreshold()) revert SameValidationThreshold();
    if (newThreshold < validateThreshold()) {
        // Lowering: requires DEFAULT_ADMIN_ROLE
        if (!hasRole(DEFAULT_ADMIN_ROLE, _msgSender())) revert ...;
    } else {
        // Raising: requires VALIDATION_GUARDIAN_ROLE
        if (!hasRole(VALIDATION_GUARDIAN_ROLE, _msgSender())) revert ...;
        // Renounce the role immediately — prevents persistent privilege
        renounceRole(VALIDATION_GUARDIAN_ROLE, _msgSender());
    }
    _updateValidateThreshold(newThreshold);
}
```

Key differences:
1. **BasculeV3** renounces `VALIDATION_GUARDIAN_ROLE` after raising the threshold, ensuring the guardian cannot repeatedly raise it. Each raise requires a fresh role grant by the admin.
2. **GMPBasculeV2** does NOT renounce the role. A single `VALIDATION_GUARDIAN_ROLE` holder can raise the threshold repeatedly without any additional admin action.
3. **BasculeV3** separates lowering (admin) from raising (guardian). **GMPBasculeV2** allows the guardian to both raise AND lower.

---

## Attack Scenario

1. The Lombard admin grants `VALIDATION_GUARDIAN_ROLE` to an operator for a one-time threshold raise.
2. The operator raises the threshold to a large value (e.g., `type(uint256).max`), effectively disabling mint validation for all mints below that amount.
3. In `BasculeV3`, the role would be renounced after step 2, preventing further changes. The admin would need to re-grant the role for any future threshold change.
4. In `GMPBasculeV2`, the operator **retains** the role. They can:
   - Keep the threshold high indefinitely (disabling validation).
   - Lower and re-raise the threshold at will without admin involvement.
   - Toggle validation on/off for any time window.

5. With validation disabled, mints that were NOT reported by the off-chain drawbridge can pass through `validateMint` without reverting (the `mintMsg.amount >= validateThreshold` check at line 242 is skipped when the threshold is set above the mint amount).

---

## Impact

- **Reduced defense-in-depth:** The Bascule is a secondary validation layer. Disabling it removes one layer of protection against unauthorized mints. The primary layer (consortium signature verification) remains.
- **Persistent privilege:** The `VALIDATION_GUARDIAN_ROLE` holder retains the ability to toggle validation indefinitely. A compromised guardian account can disable validation at any time.
- **No direct fund theft:** Disabling the Bascule does NOT bypass consortium verification. Mints still require valid consortium proofs. The Bascule is a cross-check, not the primary gate.
- **Stealth:** Threshold changes emit `UpdateValidateThreshold` events but don't revert. The change could go unnoticed if monitoring is insufficient.

---

## Three-Perspective Audit

### Prosecutor (Bug Confirmed)

The `GMPBasculeV2.updateValidateThreshold` function deviates from the more secure design in `BasculeV3`. The `VALIDATION_GUARDIAN_ROLE` is a powerful privilege — it can disable the Bascule's mint validation entirely. By not renouncing the role after a threshold raise, the contract allows persistent privilege that the `BasculeV3` design explicitly prevents. This is a defense-in-depth regression: the same project has a more secure version (`BasculeV3`) but the GMP variant (`GMPBasculeV2`) omits the safety mechanism. The inconsistency suggests the GMP version was written without carrying forward the hardening applied to V3.

### Defense (Mitigating Factors)

1. **Trusted role:** `VALIDATION_GUARDIAN_ROLE` is granted by `DEFAULT_ADMIN_ROLE`. Only trusted Lombard operators hold this role.
2. **Not the primary gate:** The Bascule is a secondary validation layer. The consortium proof is the primary gate for mints. Disabling the Bascule does not allow unauthorized mints — it only removes the cross-check.
3. **Event emission:** `UpdateValidateThreshold` events are emitted, allowing off-chain monitoring to detect threshold changes.
4. **Pausable:** The admin can pause the Bascule (`pause()`) to freeze all threshold changes.
5. **Admin can revoke:** `DEFAULT_ADMIN_ROLE` can revoke `VALIDATION_GUARDIAN_ROLE` at any time, limiting the window of abuse.

### Judge (Verdict: LOW)

The inconsistency with `BasculeV3` is real and the missing role-renunciation is a defense-in-depth gap. However, the practical impact is minimal: (1) the role is trusted and admin-controlled, (2) the Bascule is a secondary validation layer, (3) disabling it does not bypass the consortium, and (4) recovery is straightforward (admin revokes the role). The issue is a design inconsistency worth fixing but not a fund-theft vulnerability. **LOW** severity.

---

## Recommended Fix

Align `GMPBasculeV2.updateValidateThreshold` with the `BasculeV3` design:

```solidity
function updateValidateThreshold(
    uint256 newThreshold
) public whenNotPaused {
    if (newThreshold == validateThreshold) {
        revert SameValidationThreshold();
    }
    if (newThreshold < validateThreshold) {
        // Lowering: requires DEFAULT_ADMIN_ROLE
        if (!hasRole(DEFAULT_ADMIN_ROLE, _msgSender())) {
            revert AccessControlUnauthorizedAccount(_msgSender(), DEFAULT_ADMIN_ROLE);
        }
    } else {
        // Raising: requires VALIDATION_GUARDIAN_ROLE, then renounce
        if (!hasRole(VALIDATION_GUARDIAN_ROLE, _msgSender())) {
            revert AccessControlUnauthorizedAccount(_msgSender(), VALIDATION_GUARDIAN_ROLE);
        }
        renounceRole(VALIDATION_GUARDIAN_ROLE, _msgSender());
    }
    _updateValidateThreshold(newThreshold);
}
```
