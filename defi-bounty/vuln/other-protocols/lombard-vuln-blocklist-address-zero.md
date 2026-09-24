# Lombard Finance — BlocklistOracle address(0) Check on Mint/Burn Can Freeze Strategy

**Severity:** LOW-MEDIUM
**Area:** Strategy Vault — Blocklist Compliance Check
**Contracts:** `StrategyBaseUpgradeable.sol`, `BlocklistOracle.sol`
**Functions:** `StrategyBaseUpgradeable._update()`, `BlocklistOracle.check()`
**Lines:** `StrategyBaseUpgradeable.sol:551-559`, `BlocklistOracle.sol:160-179,246-254`

---

## Description

The `StrategyBaseUpgradeable` contract overrides ERC20 `_update` to enforce a blocklist check on BOTH `from` and `to` addresses for every transfer, mint, and burn:

```solidity
// StrategyBaseUpgradeable.sol:551-559
function _update(
    address from,
    address to,
    uint256 value
) internal virtual override(ERC20PausableUpgradeable) {
    _requireBlocklistAllows(from);   // ← checks `from`
    _requireBlocklistAllows(to);     // ← checks `to`
    super._update(from, to, value);
}
```

The `_requireBlocklistAllows` function calls `IBlocklistOracle.check(account)`:

```solidity
// BlocklistOracle.sol:246-254
function check(address account) external view override {
    BlocklistOracleStorage storage $ = _getBlocklistOracleStorage();
    if ($.blocklistedAccounts.contains(account)) {
        revert BlocklistOracle_NotAllowed(DenialReason.Blocklist);
    }
    if (_isSanctioned($, account)) {
        revert BlocklistOracle_NotAllowed(DenialReason.Sanction);
    }
}
```

The `_isSanctioned` function iterates over all registered sanction lists and calls `ISanctionList(sanctionList).isSanctioned(account)` for each:

```solidity
// BlocklistOracle.sol:165-179
function _isSanctioned(
    BlocklistOracleStorage storage $,
    address account
) internal view returns (bool) {
    if ($.allowlistedAccounts.contains(account)) {
        return false;
    }
    uint256 len = $.sanctions.length();
    for (uint256 i; i < len; ++i) {
        if (ISanctionList($.sanctions.at(i)).isSanctioned(account)) {
            return true;
        }
    }
    return false;
}
```

**The issue:** During mint operations, `from = address(0)`. During burn operations, `to = address(0)`. The `_update` function calls `_requireBlocklistAllows(address(0))` in both cases. The `BlocklistOracle` cannot blocklist or allowlist `address(0)`:

```solidity
// BlocklistOracle.sol:84 — blockAccount reverts on address(0)
if (account == address(0)) revert BlocklistOracle_ZeroAddress();

// BlocklistOracle.sol:115 — allowAccount also reverts on address(0)
if (account == address(0)) revert BlocklistOracle_ZeroAddress();
```

So `address(0)` can never be allowlisted (to bypass sanction checks). If ANY registered sanction list's `isSanctioned(address(0))` returns `true`, then **every mint and every burn on the strategy will revert**, permanently freezing all deposits and redemptions.

---

## Attack Scenario

### Scenario A: Misconfigured Sanction List

1. The Lombard admin registers a third-party sanction list (e.g., Chainalysis, TRM Labs) via `BlocklistOracle.addSanctionList(sanctionList)`.
2. The sanction list contract has a bug or edge-case where `isSanctioned(address(0))` returns `true` (e.g., the list checks `account.code.length > 0` and `address(0)` has special behavior, or the list has a default-deny policy for unknown addresses).
3. Now, every call to `StrategyBaseUpgradeable._update` with `from = address(0)` (mint) or `to = address(0)` (burn) calls `BlocklistOracle.check(address(0))`, which calls `_isSanctioned(address(0))`, which returns `true`.
4. The strategy reverts on every deposit (`_mint` → `_update(address(0), receiver, ...)`) and every redemption (`_burn` → `_update(owner, address(0), ...)`).
5. **The strategy is completely frozen.** Users cannot deposit or redeem.

### Scenario B: Malicious Sanction List

If a registered sanction list is later upgraded (via proxy) to return `true` for `address(0)`, the same freeze occurs. The admin would need to call `removeSanctionList` to unfreeze, but redemptions are blocked in the meantime.

### Scenario C: Sanction List Reverts

If a sanction list's `isSanctioned(address(0))` call reverts (instead of returning `true`), the `check` function also reverts (because it's a `view` function with no try/catch). This similarly freezes all mints and burns.

---

## Impact

- **Complete strategy freeze:** All deposits and redemptions on affected strategies revert. The strategy becomes a frozen vault.
- **Share transfers unaffected:** ERC20 transfers between non-zero addresses are not affected (neither `from` nor `to` is `address(0)`). Users can still transfer shares, but cannot deposit or redeem.
- **Recovery:** The admin can remove the offending sanction list via `removeSanctionList`, which restores functionality. But during the freeze, users are locked.
- **Cascading effect:** If multiple strategies share the same `BlocklistOracle`, all of them freeze simultaneously.
- **Likelihood:** Low — standard sanction lists (Chainalysis, TRM) typically return `false` for `address(0)`. But the contract doesn't enforce this, and a buggy or adversarial list could trigger the freeze.

---

## Three-Perspective Audit

### Prosecutor (Bug Confirmed)

The `_update` override unconditionally checks both `from` and `to` against the blocklist, including `address(0)` for mints and burns. The `BlocklistOracle` cannot allowlist `address(0)` to exempt it from sanction checks. This creates a hard dependency on every registered sanction list returning `false` for `isSanctioned(address(0))` — a property the contract cannot enforce. A single misconfigured, upgraded, or adversarial sanction list freezes the entire strategy. The `_update` function should skip the blocklist check for `address(0)` (the zero address is not a real user and cannot be sanctioned).

### Defense (Mitigating Factors)

1. **Admin control:** The admin controls which sanction lists are registered. If a list misbehaves, the admin can remove it.
2. **Sanction list vetting:** Lombard vets sanction lists before registration. Standard lists (Chainalysis, TRM) handle `address(0)` correctly.
3. **Not exploitable by external attackers:** The freeze requires a misconfigured sanction list, which the admin must register. An external attacker cannot trigger this.
4. **Recovery is straightforward:** `removeSanctionList` unfreezes the strategy immediately.
5. **Only affects strategy contracts:** The LBTC token contracts (`NativeLBTC`, `StakedLBTC`) do NOT have this override — their `_update` only checks pause status. So the core LBTC token is not affected.

### Judge (Verdict: LOW-MEDIUM)

The vulnerability is real — the `_update` override does not skip `address(0)`, creating an unguarded dependency on external sanction list behavior for `isSanctioned(address(0))`. However, the exploitability is low: it requires a misconfigured or adversarial sanction list that the admin explicitly registered. The impact (strategy freeze) is significant but recoverable. The fix is trivial (skip the check for `address(0)`). **LOW-MEDIUM** reflects the real but low-probability DoS vector with straightforward recovery.

---

## Recommended Fix

Skip the blocklist check for `address(0)` in the `_update` override:

```solidity
function _update(
    address from,
    address to,
    uint256 value
) internal virtual override(ERC20PausableUpgradeable) {
    if (from != address(0)) _requireBlocklistAllows(from);  // skip for mint
    if (to != address(0)) _requireBlocklistAllows(to);      // skip for burn
    super._update(from, to, value);
}
```

Alternatively, add `address(0)` to the allowlist automatically in `BlocklistOracle._isSanctioned`:

```solidity
function _isSanctioned(
    BlocklistOracleStorage storage $,
    address account
) internal view returns (bool) {
    if (account == address(0)) return false;  // zero address is never sanctioned
    if ($.allowlistedAccounts.contains(account)) return false;
    // ... rest of checks ...
}
```

The first fix (in `_update`) is preferred because it avoids the gas cost of an external call for every mint/burn.
