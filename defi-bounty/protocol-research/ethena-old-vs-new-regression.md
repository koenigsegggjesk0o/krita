# Ethena: Old (USDe) vs New (USDtb) — Regression Analysis

## Methodology
Compare `EthenaMinting.sol` (old, USDe) with `USDtbMinting.sol` (new, USDtb) 
to find regression bugs introduced during the migration.

## Old contract: EthenaMinting.sol
- File: /home/z/ethena-usde/contracts/contracts/EthenaMinting.sol
- Lines: 551
- Solidity: 0.8.20
- Token: USDe (delta-neutral synthetic dollar)

## New contract: USDtbMinting.sol
- File: /home/z/ethena-usdtb/contracts/usdtb/USDtbMinting.sol
- Lines: 681
- Solidity: 0.8.26
- Token: USDtb (RWA stablecoin backed by treasuries)

## Key Differences Found

### Difference 1: uint128 downgrade in _transferCollateral

**OLD (EthenaMinting):**
```solidity
function _transferCollateral(
    uint256 amount,      // uint256
    address asset,
    address benefactor,
    address[] calldata addresses,
    uint256[] calldata ratios  // uint256
) internal {
    // ...
    uint256 amountToTransfer = (amount * ratios[i]) / ROUTE_REQUIRED_RATIO;
    // ...
}
```

**NEW (USDtbMinting):**
```solidity
function _transferCollateral(
    uint128 amount,      // uint128 (DOWNGRADED)
    address asset,
    address benefactor,
    address[] calldata addresses,
    uint128[] calldata ratios  // uint128 (DOWNGRADED)
) internal {
    // ...
    uint128 amountToTransfer = (amount * ratios[i]) / ROUTE_REQUIRED_RATIO;
    // ...
}
```

**Impact:**
- `amount * ratios[i]` can overflow uint128 if amount is near max uint128
- Max uint128 ≈ 3.4 * 10^38
- ROUTE_REQUIRED_RATIO = 10_000
- If amount > 3.4 * 10^34, then `amount * 10000` overflows
- USDtb has 18 decimals, so 3.4 * 10^34 = 3.4 * 10^16 USDtb = 34 quadrillion USDtb
- **Realistic?** No — total USDtb supply is ~$266M = 2.66 * 10^8 USDtb
- **Exploitable?** DoS only (Solidity 0.8.x reverts on overflow). Not theft.

**Severity:** Informational. Not exploitable with realistic amounts.

### Difference 2: Order struct field types

**OLD:** `uint256 collateral_amount, uint256 usde_amount`
**NEW:** `uint128 collateral_amount, uint128 usdtb_amount`

Same downgrade. Same overflow concern. Same conclusion: not realistic.

### Difference 3: Block limits (NEW feature)

**OLD (EthenaMinting):** No block limits
**NEW (USDtbMinting):** Per-asset + global max mint/redeem per block

This is a NEW feature in USDtbMinting. New features = new attack surface.

**Potential issues:**
1. Block limits tracked in mappings: `totalPerBlockPerAsset[block.number][asset]`
2. These never get cleared (gas cost to clear would be high)
3. Storage grows unbounded — but only `block.number` keys, so ~7,200 blocks/day = 2.6M keys/year
4. Not a vulnerability, just storage bloat

**Check:** Can block limits be bypassed?
- `belowMaxMintPerBlock` modifier checks BEFORE minting
- `totalPerBlockPerAsset[block.number][asset].mintedPerBlock += order.usdtb_amount` happens AFTER modifier check
- No reentrancy possible (nonReentrant modifier on mint/redeem)
- **Conclusion:** Block limits are correctly implemented.

### Difference 4: Stables limit (NEW feature)

**OLD (EthenaMinting):** No stables limit
**NEW (USDtbMinting):** `verifyStablesLimit` for STABLE token type

New feature. Already analyzed in ethena-usdtb-minting-analysis.md.

**Key concern:** Asymmetric checking — only checks direction that hurts protocol, not direction that hurts user.

**Conclusion:** Intentional design, mitigated by off-chain RFQ. Not exploitable on-chain alone.

### Difference 5: Delegate signer (NEW feature)

**OLD (EthenaMinting):** No delegate signer
**NEW (USDtbMinting):** Two-step delegate signer system

New feature. Let me analyze:

```solidity
function setDelegatedSigner(address _delegateTo) external {
    delegatedSigner[_delegateTo][msg.sender] = DelegatedSignerStatus.PENDING;
}

function confirmDelegatedSigner(address _delegatedBy) external {
    if (delegatedSigner[msg.sender][_delegatedBy] != DelegatedSignerStatus.PENDING) {
        revert DelegationNotInitiated();
    }
    delegatedSigner[msg.sender][_delegatedBy] = DelegatedSignerStatus.ACCEPTED;
}
```

**Check:** In `verifyOrder`:
```solidity
if (!(signer == order.benefactor || delegatedSigner[signer][order.benefactor] == DelegatedSignerStatus.ACCEPTED))
{
    revert InvalidEIP712Signature();
}
```

**Potential issue:** The mapping is `delegatedSigner[signer][order.benefactor]`. 
- `signer` = address that signed the order
- `order.benefactor` = address whose order it is

So this checks: "is `signer` a delegated signer for `order.benefactor`?"

The delegation flow:
1. Benefactor calls `setDelegatedSigner(delegateTo)` → `delegatedSigner[delegateTo][benefactor] = PENDING`
2. Delegatee calls `confirmDelegatedSigner(benefactor)` → `delegatedSigner[delegatee][benefactor] = ACCEPTED`
3. Now delegatee can sign orders on behalf of benefactor

**Check for bypass:**
- Can someone set themselves as delegated signer without benefactor's consent? No — step 1 requires msg.sender = benefactor.
- Can a PENDING (not yet ACCEPTED) signer sign? No — check requires ACCEPTED status.
- Can a REJECTED signer sign? No — check requires ACCEPTED status.

**Conclusion:** Delegate signer system is correctly implemented. No bypass found.

### Difference 6: EIP-1271 support (NEW)

**OLD:** Only EIP-712 signatures
**NEW:** EIP-712 + EIP-1271 (smart contract signatures)

EIP-1271 allows smart contracts to "sign" orders via `isValidSignature(bytes32,bytes)`.

**Potential issue:** EIP-1271 callbacks can execute arbitrary code. If the benefactor is a malicious smart contract, its `isValidSignature` could:
1. Reenter the minting contract
2. Manipulate state during verification

**Check:**
- `verifyOrder` is called in `mint()` which has `nonReentrant` modifier
- EIP-1271 call happens inside `verifyOrder` which is inside `mint`
- `nonReentrant` prevents reentering `mint` or `redeem`
- BUT: the callback could call OTHER functions (not mint/redeem)

**What other functions could be called?**
- `addCustodianAddress` — only DEFAULT_ADMIN_ROLE
- `addWhitelistedBenefactor` — only DEFAULT_ADMIN_ROLE
- `setDelegatedSigner` — anyone can call (sets PENDING status)
- `confirmDelegatedSigner` — anyone can call (confirms PENDING)

**Attack vector:** During EIP-1271 callback, malicious benefactor contract could:
1. Call `setDelegatedSigner(attackerAddress)` — sets PENDING
2. Attacker calls `confirmDelegatedSigner(maliciousBenefactor)` — sets ACCEPTED
3. Now attacker is delegated signer for malicious benefactor
4. Attacker can sign orders on behalf of malicious benefactor

**But:** This doesn't steal funds because:
- The benefactor still needs to be whitelisted (`_whitelistedBenefactors.contains(order.benefactor)`)
- The benefactor still needs to have collateral to transfer
- The minter/redeemer role still controls whether mint/redeem is called

**Conclusion:** EIP-1271 callback can set up delegate signer, but this doesn't bypass any fund protection. Not exploitable for theft.

### Difference 7: removeSupportedAsset (NEW)

**OLD:** No remove asset function
**NEW:** `removeSupportedAsset(address asset)` — admin can remove asset

**Check:**
```solidity
function removeSupportedAsset(address asset) external onlyRole(DEFAULT_ADMIN_ROLE) {
    if (!tokenConfig[asset].isActive) revert InvalidAssetAddress();
    delete tokenConfig[asset];
    emit AssetRemoved(asset);
}
```

**Potential issue:** If asset is removed while there are pending orders, mint/redeem would fail with `UnsupportedAsset`. But this is admin-only and doesn't steal funds.

**Conclusion:** Not exploitable. Admin centralization risk (out of scope).

## Summary of Regression Analysis

**No critical regression bug found.**

The migration from EthenaMinting (USDe) to USDtbMinting (USDtb) introduced:
1. uint128 downgrade — DoS only, not realistic
2. Block limits — correctly implemented
3. Stables limit — asymmetric but intentional
4. Delegate signer — correctly implemented
5. EIP-1271 — no reentrancy bypass (nonReentrant protects)
6. removeSupportedAsset — admin only, out of scope

## Next Steps

1. **Get StakedENA.sol source** — try Etherscan via different method, or check if it's in another Ethena repo
2. **Analyze OFT contracts** — LayerZero cross-chain, complex
3. **Read remaining test files** — ACL, Delegate, SmartContractSigning, Whitelist
4. **Check audit reports** — Code4rena/Cantina reports for Ethena, see what was flagged
5. **Deep dive verifyStablesLimit** — write Foundry PoC to test edge cases
6. **Check EIP-712 domain separator** — cross-chain replay protection

## Files Compared
- [x] EthenaMinting.sol (551 lines) — old USDe minting
- [x] USDtbMinting.sol (681 lines) — new USDtb minting
- [x] _transferCollateral function — compared
- [x] verifyOrder function — compared
- [x] Delegate signer system — analyzed (new feature)
- [x] EIP-1271 support — analyzed (new feature)
