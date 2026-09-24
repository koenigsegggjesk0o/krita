# USDtb.sol — Vulnerability Analysis (First Pass)

## Contract Info
- **File:** contracts/usdtb/USDtb.sol
- **Lines:** 204
- **Solidity:** 0.8.26
- **Pattern:** Upgradeable (ERC20BurnableUpgradeable + ERC20PermitUpgradeable + ReentrancyGuardUpgradeable + SingleAdminAccessControlUpgradeable)
- **Immunefi scope:** USDtb.sol (0xc139...ac1c)
- **TVL:** $266.2M

## Key Features
1. **Transfer States:** FULLY_DISABLED (0), WHITELIST_ENABLED (1), FULLY_ENABLED (2)
2. **Blacklist/Whitelist:** Role-based (BLACKLISTED_ROLE, WHITELISTED_ROLE)
3. **Redistribution:** Admin can burn from blacklisted → mint to non-blacklisted
4. **Token Rescue:** Admin can rescue any token sent to contract
5. **Minter Role:** Only MINTER_CONTRACT role can mint
6. **No Renounce:** renounceRole() always reverts

## _beforeTokenTransfer Analysis

The `_beforeTokenTransfer` hook checks transfer states based on `msg.sender`:

### FULLY_ENABLED (state 2):
```
MINTER_CONTRACT calling + from not blacklisted + to==0 → redeeming (burn)
MINTER_CONTRACT calling + from==0 + to not blacklisted → minting
DEFAULT_ADMIN calling + from blacklisted + to==0 → redistributing (burn)
DEFAULT_ADMIN calling + from==0 + to not blacklisted → redistributing (mint)
Normal: msg.sender not blacklisted + from not blacklisted + to not blacklisted → OK
Else → revert
```

### WHITELIST_ENABLED (state 1):
Same as above + whitelisted users can transfer between whitelisted addresses + whitelisted can burn.

### FULLY_DISABLED (state 0):
All transfers revert.

## Potential Vulnerability Areas

### Area 1: msg.sender in _beforeTokenTransfer

**Observation:** The function uses `msg.sender` to determine if the caller is MINTER_CONTRACT or DEFAULT_ADMIN. This is the external caller, not the contract itself.

**Check:** When USDtbMinting calls `usdtb.mint()`, msg.sender = USDtbMinting address. When USDtbMinting calls `usdtb.burnFrom()`, msg.sender = USDtbMinting address. Both are correct — USDtbMinting has MINTER_CONTRACT role.

**Status:** No issue found. Logic is correct.

### Area 2: redistributeLockedAmount

**Code:**
```solidity
function redistributeLockedAmount(address from, address to) external nonReentrant onlyRole(DEFAULT_ADMIN_ROLE) {
    if (hasRole(BLACKLISTED_ROLE, from) && !hasRole(BLACKLISTED_ROLE, to)) {
        uint256 amountToDistribute = balanceOf(from);
        _burn(from, amountToDistribute);
        _mint(to, amountToDistribute);
    } else {
        revert OperationNotAllowed();
    }
}
```

**Observation:** Burns entire balance from blacklisted address, mints to non-blacklisted. Uses `_burn` and `_mint` which trigger `_beforeTokenTransfer`.

- `_burn(from, amount)`: _beforeTokenTransfer(from, 0, amount) → admin + blacklisted from + to==0 → passes
- `_mint(to, amount)`: _beforeTokenTransfer(0, to, amount) → admin + from==0 + to not blacklisted → passes

**Status:** No issue found. Logic is correct.

### Area 3: rescueTokens

**Code:**
```solidity
function rescueTokens(address token, uint256 amount, address to) external nonReentrant onlyRole(DEFAULT_ADMIN_ROLE) {
    IERC20Upgradeable(token).safeTransfer(to, amount);
}
```

**Observation:** Admin can rescue ANY token, including USDtb itself. This means admin can drain all USDtb from the contract. But this is admin-only, so it's a centralization risk (out of scope per Immunefi rules).

**Status:** Out of scope (centralization risk).

### Area 4: burnFrom in redeem

**Observation:** `USDtbMinting.redeem()` calls `usdtb.burnFrom(order.benefactor, order.usdtb_amount)`. This requires USDtbMinting to have allowance from benefactor. The allowance must be set off-chain by the benefactor before redeem.

**Potential issue:** If the benefactor gives unlimited allowance (max uint256), `_spendAllowance` won't decrement it. This is standard OpenZeppelin behavior. No issue.

**Status:** No issue found.

## Combined Analysis: USDtbMinting + USDtb

### Most Promishing Attack Vector: Cross-Asset Mint+Redeem with Stables Limit Bypass

**Hypothesis:** An attacker could mint USDtb with collateral A and redeem for collateral B, profiting from price differences, if:
1. Both assets are STABLE type
2. The stablesDeltaLimit allows both directions
3. The off-chain RFQ system doesn't catch it

**Analysis:**
- For MINT: `verifyStablesLimit` only checks when `usdtbAmount > normalizedCollateralAmount` (attacker gets more USDtb than collateral given)
- For REDEEM: `verifyStablesLimit` only checks when `normalizedCollateralAmount > usdtbAmount` (attacker gets more collateral than USDtb burned)

If:
- MINT: give 100 USDC, get 100 USDtb (1:1, no check triggered)
- REDEEM: burn 100 USDtb, get 100 USDT (1:1, no check triggered)

If USDC = $0.999 and USDT = $1.001, attacker profits $0.20 per $100 cycle.

**BUT:** This is mitigated by:
1. Off-chain RFQ system sets the price — benefactor can't choose arbitrary amounts
2. Minter/Redeemer has "last look" rights — can reject bad orders
3. Block limits cap the volume per block
4. Only whitelisted benefactors can participate
5. Only MINTER_ROLE/REDEEMER_ROLE can call mint/redeem

**Conclusion:** Not directly exploitable due to off-chain controls. But if the off-chain system is compromised or misconfigured, the on-chain stablesDeltaLimit check is the last line of defense, and it has the asymmetric checking weakness.

**Severity:** Low-Medium. Requires off-chain system failure to exploit.

### Next Investigation Targets

1. **Read test files** — understand what edge cases were already tested:
   - USDtbMinting.StableRatios.t.sol
   - USDtbMinting.blockLimits.t.sol
   - USDtbMinting.ACL.t.sol

2. **Compare with USDe Minting** — the older contract. Check what changed, what bugs were fixed.

3. **Look for upgradeability bugs** — USDtb uses UUPS/proxy pattern. Check:
   - Is there an unauthorized upgrade path?
   - Can implementation be self-destructed?
   - Are there storage slot collisions?

4. **Check EIP-712 domain separator** — cross-chain replay protection:
   - Domain separator uses `block.chainid` 
   - Cached as immutable in constructor
   - `getDomainSeparator()` recomputes if chainid changes (fork protection)
   - Check if this is correct

5. **Deep dive into nonce system** — the bitmap approach:
   - `uint64(nonce) >> 8` = slot (2^56 slots)
   - `1 << uint8(nonce)` = bit (256 bits per slot)
   - Upper 64 bits of uint128 nonce are ignored
   - Check if this allows any bypass

## Files Read
- [x] USDtbMinting.sol (681 lines) — first pass complete
- [x] USDtb.sol (204 lines) — first pass complete

## Files to Read Next
- [ ] IUSDtbMinting.sol (interface)
- [ ] IUSDtbDefinitions.sol (definitions)
- [ ] SingleAdminAccessControl.sol
- [ ] SingleAdminAccessControlUpgradeable.sol
- [ ] Test files (5+ files)
