# USDtbMinting — Untested Admin Functions and Fee-on-Transfer Gap

**Status:** Potential bugs in untested code (NOT submitted to Immunefi)
**Severity:** Medium
**Source:** Test coverage gap analysis
**Contract:** `/home/z/ethena-usdtb/contracts/usdtb/USDtbMinting.sol`

---

## Summary

Three USDtbMinting admin functions (`setTokenType`, `setStablesDeltaLimit`, `setUSDtbToken`) have **zero test coverage**. Each has a missing validation that could lead to bricked state or disabled safety checks. Additionally, `_transferCollateral` (used in every mint) is never tested with fee-on-transfer tokens, creating a hidden accounting discrepancy.

---

## Issue 1: `setUSDtbToken` Has No Zero-Address Check

### Code (USDtbMinting.sol, lines 653–657)

```solidity
/// @notice set the USDtb token address
function setUSDtbToken(IUSDtb _usdtb) external onlyRole(DEFAULT_ADMIN_ROLE) {
    usdtb = _usdtb;
    emit USDtbSet(address(_usdtb));
}
```

### Problem

There is no check that `_usdtb != address(0)` or that `_usdtb` is a valid ERC-20. An admin (or a compromised admin key) could call `setUSDtbToken(IUSDtb(address(0)))`, setting `usdtb` to the zero address. After this:

- `mint()` calls `usdtb.mint(order.beneficiary, order.usdtb_amount)` → call to address(0) → reverts
- `redeem()` calls `usdtb.burnFrom(order.benefactor, order.usdtb_amount)` → reverts
- `verifyStablesLimit()` calls `_getDecimals(address(usdtb))` → `IERC20Metadata(address(0)).decimals()` → reverts

**All minting and redemption is permanently bricked** until `setUSDtbToken` is called again with a valid address.

### Test coverage

**Zero.** No test calls `setUSDtbToken`. The function exists in the contract but is never exercised. A test like this would catch the issue:

```solidity
function test_setUSDtbToken_zeroAddress_revert() public {
    vm.expectRevert();
    usdtbMinting.setUSDtbToken(IUSDtb(address(0)));
}
```

But no such test exists, and the function has no `onlyValidAddress` modifier.

### Contrast with other setter functions

The constructor checks `if (_admin == address(0)) revert InvalidZeroAddress()`. The `addSupportedAsset` function checks `asset == address(0)`. But `setUSDtbToken` has no such check — it's an inconsistency.

---

## Issue 2: `setStablesDeltaLimit` Has No Upper Bound

### Code (USDtbMinting.sol, lines 648–651)

```solidity
/// @notice set the allowed price delta in bps for stablecoin minting
function setStablesDeltaLimit(uint128 _stablesDeltaLimit) external onlyRole(DEFAULT_ADMIN_ROLE) {
    stablesDeltaLimit = _stablesDeltaLimit;
}
```

### Problem

There is no upper-bound check on `_stablesDeltaLimit`. The limit is used in `verifyStablesLimit` (line 542):

```solidity
uint128 differenceInBps = (difference * STABLES_RATIO_MULTIPLIER) / usdtbAmount;
// ...
return usdtbAmount > normalizedCollateralAmount ? differenceInBps <= stablesDeltaLimit : true;
```

If `stablesDeltaLimit` is set to `type(uint128).max` (or any very large value), the check `differenceInBps <= stablesDeltaLimit` is **always true**, effectively disabling the stable-price safety check. This allows minting USDtb with arbitrarily mismatched collateral amounts (e.g., mint 1,000,000 USDtb for 1 USDC of collateral, as long as the order is signed by an authorized minter).

### Test coverage

**Zero.** The `StableRatios.t.sol` test file tests `verifyStablesLimit` with valid and invalid ratios, but **never tests `setStablesDeltaLimit` itself**. No test verifies that an unreasonably high limit is rejected.

### Impact

If an admin accidentally sets `stablesDeltaLimit` to a very high value (e.g., by passing wei instead of bps), the stable-price protection is silently disabled. The `StableRatios` test only tests with reasonable values (the default limit), not with extreme values.

---

## Issue 3: `setTokenType` Has No Enum Validation

### Code (USDtbMinting.sol, lines 641–646)

```solidity
function setTokenType(address asset, TokenType tokenType) external onlyRole(DEFAULT_ADMIN_ROLE) {
    if (!tokenConfig[asset].isActive) revert UnsupportedAsset();
    tokenConfig[asset].tokenType = tokenType;
    emit TokenTypeSet(asset, uint256(tokenType));
}
```

### Problem

The `TokenType` enum has only two values: `STABLE` (0) and `ASSET` (1). In Solidity, an enum can be cast from any `uint8` value, including values outside the defined range. However, Solidity 0.8+ reverts on out-of-bounds enum casts at runtime, so passing an invalid value would revert. The issue is more subtle:

- No test verifies that setting `tokenType = ASSET` correctly skips the `verifyStablesLimit` check in `verifyOrder`.
- No test verifies that setting `tokenType = STABLE` correctly enables the check.
- The `StableRatios.t.sol` tests only test with tokens that are already `STABLE` type (set during construction). The `setTokenType` function is never called in any test.

### Impact

If a token that should be `STABLE` is accidentally set to `ASSET`, the stable-price protection is bypassed for that token. Conversely, if an `ASSET` token is set to `STABLE`, the stable-price check would incorrectly apply and may revert valid mints.

---

## Issue 4: `_transferCollateral` Doesn't Handle Fee-on-Transfer Tokens

### Code (USDtbMinting.sol, lines 573–596)

```solidity
function _transferCollateral(
    uint128 amount,
    address asset,
    address benefactor,
    address[] calldata addresses,
    uint128[] calldata ratios
) internal {
    if (!tokenConfig[asset].isActive || asset == NATIVE_TOKEN) revert UnsupportedAsset();
    IERC20 token = IERC20(asset);
    uint128 totalTransferred = 0;
    for (uint128 i = 0; i < addresses.length;) {
        uint128 amountToTransfer = (amount * ratios[i]) / ROUTE_REQUIRED_RATIO;
        token.safeTransferFrom(benefactor, addresses[i], amountToTransfer);
        totalTransferred += amountToTransfer;  // ← uses REQUESTED amount, not actual received
        unchecked { ++i; }
    }
    uint128 remainingBalance = amount - totalTransferred;
    if (remainingBalance > 0) {
        token.safeTransferFrom(benefactor, addresses[addresses.length - 1], remainingBalance);
    }
}
```

### Problem

The function accumulates `totalTransferred += amountToTransfer` using the **requested** transfer amount, not the **actual** amount received by the custodian. With a fee-on-transfer token:

1. `safeTransferFrom(benefactor, custodian, 100)` transfers 100 from benefactor.
2. Custodian receives only 95 (5 fee deducted by the token).
3. `totalTransferred += 100` (the requested amount).
4. The mint function then mints `order.usdtb_amount` (the full amount based on the order).

**The protocol mints USDtb backed by less collateral than expected.** If a fee-on-transfer stablecoin is added as a supported asset (which is possible — the admin just calls `addSupportedAsset` and `setTokenType(STABLE)`), each mint extracts a fee from the collateral transfer but still mints the full USDtb amount. Over many mints, the protocol becomes under-collateralized.

### Is this exploitable?

The `TokenType` field exists but is only used for the stable-price ratio check, not for fee-on-transfer detection. There is no `isFeeOnTransfer` flag in `TokenConfig`. The protocol relies on all supported assets being "well-behaved" standard ERC-20 tokens. If an admin adds a fee-on-transfer token (by mistake or malice), the accounting discrepancy is silent.

### Test coverage

**Zero.** No test uses a fee-on-transfer mock token. All tests use standard ERC-20 tokens (or MockToken implementations that transfer 1:1). The `_transferCollateral` function is tested in `USDtbMinting.core.t.sol` but only with standard tokens.

### Suggested fix

For fee-on-transfer tokens, check balances before and after transfer:

```solidity
uint256 balanceBefore = token.balanceOf(addresses[i]);
token.safeTransferFrom(benefactor, addresses[i], amountToTransfer);
uint256 actualReceived = token.balanceOf(addresses[i]) - balanceBefore;
totalTransferred += uint128(actualReceived);
```

Or explicitly disallow fee-on-transfer tokens by documenting that `_transferCollateral` assumes 1:1 transfers.

---

## Issue 5: `_setMaxMintPerBlock` / `_setMaxRedeemPerBlock` Don't Check Asset Is Active

### Code (USDtbMinting.sol, lines 618–633)

```solidity
function _setMaxMintPerBlock(uint128 _maxMintPerBlock, address asset) internal {
    uint128 oldMaxMintPerBlock = tokenConfig[asset].maxMintPerBlock;
    tokenConfig[asset].maxMintPerBlock = _maxMintPerBlock;  // ← no isActive check
    emit MaxMintPerBlockChanged(oldMaxMintPerBlock, _maxMintPerBlock, asset);
}

function _setMaxRedeemPerBlock(uint128 _maxRedeemPerBlock, address asset) internal {
    uint128 oldMaxRedeemPerBlock = tokenConfig[asset].maxRedeemPerBlock;
    tokenConfig[asset].maxRedeemPerBlock = _maxRedeemPerBlock;  // ← no isActive check
    emit MaxRedeemPerBlockChanged(oldMaxRedeemPerBlock, _maxRedeemPerBlock, asset);
}
```

### Problem

These functions (called by the public `setMaxMintPerBlock` / `setMaxRedeemPerBlock`) don't verify that the asset is active. An admin can set max-per-block values for an unsupported asset. While this doesn't directly cause harm (the `belowMaxMintPerBlock` modifier checks `isActive` and reverts for unsupported assets), it's a gap:

- If an asset is removed via `removeSupportedAsset` (which sets `isActive = false` but doesn't zero out the config), and later re-added via `addSupportedAsset` (which calls `_setTokenConfig` and resets the config), the old max values are overwritten. But between removal and re-add, the config retains stale values.
- No test verifies behavior when setting max values for an inactive asset.

### Test coverage

**Zero.** The `blockLimits.t.sol` tests only call `setMaxMintPerBlock` and `setMaxRedeemPerBlock` on active assets. No test calls them on an inactive or never-added asset.

---

## Summary of Untested USDtbMinting Functions

| Function | Tested? | Gap |
|----------|---------|-----|
| `setTokenType` | NO | No enum validation test |
| `setStablesDeltaLimit` | NO | No upper bound — can disable safety check |
| `setUSDtbToken` | NO | No zero-address check — can brick contract |
| `_transferCollateral` with FoT token | NO | Accounting discrepancy |
| `_setMaxMintPerBlock` on inactive asset | NO | No isActive check |
| `_setMaxRedeemPerBlock` on inactive asset | NO | No isActive check |

---

## Assessment

| Issue | Severity | Test Coverage | Exploitable? |
|-------|----------|---------------|--------------|
| `setUSDtbToken(address(0))` | High (bricks contract) | Zero | Yes — single admin call |
| `setStablesDeltaLimit(max)` | Medium (disables safety) | Zero | Yes — single admin call |
| `setTokenType` wrong type | Medium (bypasses/enables wrong check) | Zero | Yes — single admin call |
| `_transferCollateral` FoT | Medium (under-collateralization) | Zero | Requires adding FoT token |
| `_setMax*PerBlock` on inactive | Low (stale state) | Zero | No direct exploit |

**Note:** Issues 1–3 require admin access, so they are primarily "admin mistake" or "compromised admin" scenarios. However, defense-in-depth principles require these checks even for admin-only functions — admins make mistakes, and the tests should verify the guardrails work.

The fee-on-transfer issue (Issue 4) is the most economically dangerous because it silently causes under-collateralization over time and doesn't require any admin mistake beyond adding the token (which is a legitimate operation if a fee-on-transfer stablecoin is intentionally added).
