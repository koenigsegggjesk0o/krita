# Usual Labs — No removeUsd0Rwa() in TokenMapping Prolongs Oracle-DoS Outage

**Severity:** MEDIUM
**Area:** TokenMapping / Oracle / Operational resilience
**Bounty:** Sherlock Usual Labs Bug Bounty (#56)

---

## Description

`TokenMapping.addUsd0Rwa(address rwa)` lets the admin register a new RWA as USD0 collateral.
Once registered, the RWA is permanently included in `getAllUsd0Rwa()`, which is the list
iterated by `Usd0.mint()` to compute the total backing.

**There is no `removeUsd0Rwa(address rwa)` function.** The contract has no way to delist a
broken RWA — not even for the `DEFAULT_ADMIN_ROLE` holder. Combined with the cross-RWA revert
cascade documented in `usual-vuln-oracle-dos.md`, this means:

- If an RWA's Chainlink feed goes stale or its stablecoin depeggs beyond `maxDepegThreshold`,
  the admin **cannot** delist that RWA from the backing loop without a full contract upgrade.
- The contract is behind a `TransparentUpgradeableProxy`, so upgrades require the
  `USUAL_PROXY_ADMIN_MAINNET` multisig + timelock — typically hours to days.
- During the outage, **all** `swap()` and `redeem()` are frozen (per the cross-RWA DoS).

Even outside the DoS scenario, the inability to remove an RWA is an operational hazard: an
RWA that is delisted by its issuer, redeemed-away, or otherwise defunct still pollutes the
backing loop and the oracle calls forever.

---

## Contract / Function / Lines

**Contract:** `TokenMapping` (impl `0x334b18E5e81657efA2057F80e19b8E81F0e5783C`)

**File:** `src/TokenMapping.sol`, lines 85–110 (`addUsd0Rwa`), 127–139 (`getAllUsd0Rwa`).

```solidity
function addUsd0Rwa(address rwa) external returns (bool) {
    if (rwa == address(0)) revert NullAddress();
    if (IERC20Metadata(rwa).decimals() == 0) revert Invalid();
    TokenMappingStorageV0 storage $ = _tokenMappingStorageV0();
    $._registryAccess.onlyMatchingRole(DEFAULT_ADMIN_ROLE);
    if ($.isUsd0Collateral[rwa]) revert SameValue();
    $.isUsd0Collateral[rwa] = true;
    ++$._usd0ToRwaLastId;
    if ($._usd0ToRwaLastId > MAX_RWA_COUNT) revert TooManyRWA();
    $.USD0Rwas[$._usd0ToRwaLastId] = rwa;
    emit AddUsd0Rwa(rwa, $._usd0ToRwaLastId);
    return true;
}
// ↑ No removeUsd0Rwa counterpart anywhere in the contract.
```

`getAllUsd0Rwa()` (lines 127–139) returns every RWA from index 1 to `_usd0ToRwaLastId`
inclusive — there is no "active" flag and no compaction; every slot 1..N is always returned.

`Usd0.mint()` (lines 120–133) iterates this list verbatim:
```solidity
address[] memory rwas = $.tokenMapping.getAllUsd0Rwa();
for (uint256 i = 0; i < rwas.length;) {
    address rwa = rwas[i];
    uint256 rwaPriceInUSD = uint256(oracle.getPrice(rwa));  // ← no skip, no try/catch
    ...
}
```

---

## Attack Scenario

1. RWA #4 (a stablecoin) depeggs to $0.97 — `ClassicalOracle.getPrice(rwa4)` reverts with
   `StablecoinDepeg()`.
2. `Usd0.mint()` reverts because the loop hits rwa4.
3. Admin wants to delist rwa4 to unblock minting — but `TokenMapping` has **no removal function**.
4. Admin's only options:
   a. `setMaxDepegThreshold(10000)` — accepts any stablecoin price $0–$2; the backing calc then
      uses the depegged $0.97 price (≈3% backing loss for that RWA), which may still cause
      `AmountExceedBacking` reverts if supply is close to backing.
   b. Contract upgrade via `ProxyAdmin` multisig — multi-hour delay during which the protocol
      remains frozen.
5. Result: prolonged outage, market panic, USD0 depeg on secondary markets.

---

## Impact

- **Prolongs the cross-RWA DoS** from "minutes" (if delisting were possible) to "hours/days"
  (upgrade required). For a stablecoin with $200M+ market cap, every hour of frozen
  redeemability translates to real market losses and reputational damage.
- **No graceful degradation:** even if the admin is willing to "sacrifice" one RWA's backing
  contribution, the contract provides no mechanism to do so — it's all-or-nothing.
- **Operational debt:** every RWA ever added remains in the loop forever, even after the issuer
  delists the token or the Chainlink feed is decommissioned.

---

## Three-Perspective Audit

### 1. Attacker Perspective
An attacker who can cause one RWA's oracle to fail (e.g. by exploiting a third-party Chainlink
feed issue, or by timing an attack around a known stablecoin vulnerability) gains a **prolonged
freeze window** because the protocol cannot quickly delist the broken RWA. During this window,
the attacker can profit from USD0's secondary-market dislocation.

### 2. Protocol / Defender Perspective
This is a straightforward missing-feature: `addUsd0Rwa` without `removeUsd0Rwa` is an obvious
asymmetry. Adding a removal function (with admin gating and event emission) is low-risk and
would dramatically improve operational resilience. The fix is a few lines of code.

### 3. Auditor / Sherlock-Judging Perspective
- **In scope:** `TokenMapping` is a Critical-tier contract.
- **Severity calibration:** the bug itself is a missing function (no direct fund loss), but it
  materially extends the duration of the cross-RWA DoS (the HIGH finding). MEDIUM is appropriate
  as a standalone finding; together with the oracle DoS, the combined impact is HIGH.
- **Not a duplicate:** the cross-RWA DoS is about the mint loop lacking `try/catch`; this
  finding is about the inability to delist — they have different root causes and different fixes.

---

## Recommended Fix

Add a `removeUsd0Rwa(address rwa)` function to `TokenMapping`:

```solidity
function removeUsd0Rwa(address rwa) external returns (bool) {
    TokenMappingStorageV0 storage $ = _tokenMappingStorageV0();
    $._registryAccess.onlyMatchingRole(DEFAULT_ADMIN_ROLE);
    if (!$.isUsd0Collateral[rwa]) revert InvalidToken();
    $.isUsd0Collateral[rwa] = false;
    // Compact the array: find and clear the slot, swap with last, decrement counter.
    uint256 lastId = $._usd0ToRwaLastId;
    for (uint256 i = 1; i <= lastId; ) {
        if ($.USD0Rwas[i] == rwa) {
            $.USD0Rwas[i] = $.USD0Rwas[lastId];
            delete $.USD0Rwas[lastId];
            $._usd0ToRwaLastId = lastId - 1;
            emit RemoveUsd0Rwa(rwa, i);
            return true;
        }
        unchecked { ++i; }
    }
    return false;
}
```

Additionally, even with `removeUsd0Rwa`, `Usd0.mint()` should still use `try/catch` per RWA
(per the oracle-DoS fix) so that a depeg event does not freeze the protocol before the admin
can react.

---

## References

- `TokenMapping.sol` lines 85–110 (`addUsd0Rwa`), 127–139 (`getAllUsd0Rwa`)
- `Usd0.sol` lines 120–133 (mint loop consuming `getAllUsd0Rwa`)
- `usual-vuln-oracle-dos.md` (companion finding — the cross-RWA revert cascade)
- Mainnet state: 7 RWAs currently registered (`getLastUsd0RwaId() = 7`)
