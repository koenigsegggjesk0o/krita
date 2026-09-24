# USDtbMinting.sol — Vulnerability Analysis (First Pass)

## Contract Info
- **File:** contracts/usdtb/USDtbMinting.sol
- **Lines:** 681
- **Solidity:** 0.8.26
- **Immunefi scope:** USDtbMinting.sol (0xa3DD...416a)
- **TVL:** Part of $266.2M (USDtb ecosystem)

## Architecture Overview

USDtbMinting handles:
- **mint()**: Minter role takes collateral from benefactor → sends to custodians → mints USDtb to beneficiary
- **redeem()**: Redeemer role burns USDtb from benefactor → sends collateral from contract to beneficiary
- **Order verification**: EIP-712 signed orders with off-chain RFQ pricing
- **Nonce deduplication**: Bitmap-based per-benefactor
- **Block limits**: Per-asset + global max mint/redeem per block
- **Delegate signer**: Smart contracts can delegate signing to EOA
- **Route verification**: Collateral split across multiple custodians by ratio

## Potential Vulnerability Areas (First Pass)

### Area 1: verifyStablesLimit — Asymmetric Checking

**Location:** Lines ~510-530

**Code:**
```solidity
function verifyStablesLimit(...) public view returns (bool) {
    // ... normalization logic ...
    
    uint128 difference = normalizedCollateralAmount > usdtbAmount
      ? normalizedCollateralAmount - usdtbAmount
      : usdtbAmount - normalizedCollateralAmount;
    uint128 differenceInBps = (difference * STABLES_RATIO_MULTIPLIER) / usdtbAmount;

    if (orderType == OrderType.MINT) {
      return usdtbAmount > normalizedCollateralAmount ? differenceInBps <= stablesDeltaLimit : true;
    } else {
      return normalizedCollateralAmount > usdtbAmount ? differenceInBps <= stablesDeltaLimit : true;
    }
}
```

**Observation:** The check is ASYMMETRIC:
- MINT: Only checks when `usdtbAmount > normalizedCollateralAmount` (minting more than collateral)
- REDEEM: Only checks when `normalizedCollateralAmount > usdtbAmount` (giving more collateral than USDtb burned)

In both cases, the direction that HURTS THE PROTOCOL is checked. The direction that hurts the user is NOT checked (returns true).

**Potential exploit vector:** Combined mint+redeem with DIFFERENT collateral assets in same block. If:
1. MINT with asset A (give 100 A, get 100 USDtb — within limit)
2. REDEEM for asset B (burn 100 USDtb, get 101 B — within limit if B < A)

If asset A and B have slightly different prices and stablesDeltaLimit allows both, attacker profits the spread.

**Status:** Needs deeper analysis. Need to check:
- Can mint and redeem use different collateral assets?
- What is the current stablesDeltaLimit value?
- Are there oracles or price feeds that could be manipulated?
- Is there a same-block mint+redeem protection?

### Area 2: Nonce Bitmap — Upper 64 Bits Ignored

**Location:** Lines ~490-500

**Code:**
```solidity
function verifyNonce(address sender, uint128 nonce) public view override returns (...) {
    if (nonce == 0) revert InvalidNonce();
    uint128 invalidatorSlot = uint64(nonce) >> 8;
    uint256 invalidatorBit = 1 << uint8(nonce);
    // ...
}
```

**Observation:** `nonce` is `uint128` but only lower 64 bits are used (`uint64(nonce)`). Upper 64 bits are completely ignored.

**Impact:** Two nonces that differ only in upper 64 bits map to same bitmap slot+bit. Not a replay vulnerability (both would be treated as same nonce), but a potential confusion vector.

**Status:** Low severity. Not directly exploitable for replay.

### Area 3: verifyStablesLimit — Division Truncation

**Location:** Lines ~515-520

**Code:**
```solidity
normalizedCollateralAmount =
    usdtbDecimals > collateralDecimals ? collateralAmount * scale : collateralAmount / scale;
```

**Observation:** When `usdtbDecimals < collateralDecimals`, division truncates. For example:
- collateralDecimals = 18, usdtbDecimals = 6
- scale = 10^12
- collateralAmount = 1.5 * 10^18 (1.5 tokens)
- normalizedCollateralAmount = 1.5 * 10^18 / 10^12 = 1.5 * 10^6 → truncated to 1500000

This loses 0.5 * 10^6 in the normalized amount, which could affect the stablesDeltaLimit check.

**Status:** Low severity. Precision loss is small relative to amounts. Not directly exploitable.

### Area 4: verifyStablesLimit — Multiplication Overflow

**Location:** Lines ~515-520

**Code:**
```solidity
normalizedCollateralAmount =
    usdtbDecimals > collateralDecimals ? collateralAmount * scale : collateralAmount / scale;
```

**Observation:** When `usdtbDecimals > collateralDecimals`, `collateralAmount * scale` could overflow uint128. Max uint128 ≈ 3.4 * 10^38. If scale = 10^12 (18-6 decimals) and collateralAmount > 10^26, overflow.

Solidity 0.8.x reverts on overflow, so this is a DoS vector, not a theft vector.

**Status:** Low severity. Realistic amounts won't trigger this.

### Area 5: No Same-Block Mint+Redeem Restriction

**Location:** mint() and redeem() functions

**Observation:** There's no check preventing a user from minting AND redeeming in the same block. Block limits are separate for mint and redeem:
- `totalPerBlockPerAsset[block.number][asset].mintedPerBlock`
- `totalPerBlockPerAsset[block.number][asset].redeemedPerBlock`

These are tracked independently. An attacker could mint and redeem in the same block, potentially exploiting price differences.

**Status:** Medium severity. Combined with Area 1 (asymmetric stables check), this could be exploitable. Need to verify if there are additional protections.

### Area 6: Order of Checks in verifyOrder

**Location:** Lines ~450-480

**Observation:** Expiry check is LAST:
```
1. Signature verification
2. Benefactor whitelist
3. Beneficiary approval
4. Stables limit
5. Zero address/amount
6. Expiry (last)
```

An expired order still goes through all checks before reverting. Gas inefficiency but not a vulnerability.

**Status:** Informational only.

## Next Steps for Deeper Analysis

1. **Check USDtb.sol token contract** — look for transfer restriction bypass, redistribution bugs
2. **Analyze test files** — foundry tests might reveal what was already tested + edge cases
3. **Compare with USDe Minting** — older contract, check what changed
4. **On-chain analysis** — check current stablesDeltaLimit value, supported assets, custodian addresses
5. **Flash loan attack simulation** — can attacker mint with asset A, redeem for asset B, profit?
6. **Check for upgradeable proxy pattern** — if contract is upgradeable, check for implementation bugs
7. **Check EIP-712 domain separator** — chain ID fork protection, cross-chain replay

## Files to Read Next
- contracts/usdtb/USDtb.sol (token contract)
- contracts/SingleAdminAccessControl.sol (access control)
- test/foundry/test/USDtbMinting.StableRatios.t.sol (stables limit tests)
- test/foundry/test/USDtbMinting.blockLimits.t.sol (block limit tests)
