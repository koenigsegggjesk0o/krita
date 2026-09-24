# ENS Vulnerability Report: NameWrapper.extendExpiry Clears Stored Fuses on Expired Subnames

## Severity: LOW (Informational)

## Summary

The `NameWrapper.extendExpiry()` function uses `getData()` (which returns **effective** fuses — cleared to 0 when the name is expired) instead of `super.getData()` (which returns **raw stored** fuses). When `extendExpiry()` is called on an expired subname, it writes the effective fuses (0) back to storage via `_setData()`, permanently clearing all stored fuses including `PARENT_CANNOT_CONTROL`, `CANNOT_UNWRAP`, `CANNOT_TRANSFER`, etc.

## Contract & Function & Line

- **Contract**: `NameWrapper.sol`
- **Function**: `extendExpiry(bytes32 parentNode, bytes32 labelhash, uint64 expiry)`
- **Lines**: ~461-477 (NameWrapper.sol)

```solidity
function extendExpiry(
    bytes32 parentNode,
    bytes32 labelhash,
    uint64 expiry
) public returns (uint64) {
    bytes32 node = _makeNode(parentNode, labelhash);
    if (!_isWrapped(node)) { revert NameIsNotWrapped(); }

    bool canExtendSubname = canExtendSubnames(parentNode, msg.sender);
    if (!canExtendSubname && !canModifyName(node, msg.sender)) {
        revert Unauthorised(node, msg.sender);
    }

    (address owner, uint32 fuses, uint64 oldExpiry) = getData(uint256(node)); // <-- BUG: returns EFFECTIVE (cleared) fuses

    if (!canExtendSubname && fuses & CAN_EXTEND_EXPIRY == 0) {
        revert OperationProhibited(node);
    }

    (, , uint64 maxExpiry) = getData(uint256(parentNode));
    expiry = _normaliseExpiry(expiry, oldExpiry, maxExpiry);

    _setData(node, owner, fuses, expiry); // <-- Writes effective fuses (0) to storage
    emit ExpiryExtended(node, expiry);
    return expiry;
}
```

## Root Cause

`getData()` applies `_clearOwnerAndFuses()` which returns `fuses = 0` when `expiry < block.timestamp`. The `extendExpiry()` function then stores these zeroed fuses back via `_setData()`, destroying the original stored fuse values.

The fix should use `super.getData()` (raw stored values) to preserve fuses across expiry extensions:
```solidity
(address owner, uint32 fuses, uint64 oldExpiry) = super.getData(uint256(node));
```

## Attack Scenario

1. Parent `P` creates subname `foo.parent.eth` for user `U` with fuses = `CANNOT_UNWRAP | CANNOT_TRANSFER` and a 30-day expiry.
2. `U` relies on `CANNOT_TRANSFER` to ensure the subname cannot be moved.
3. The subname expires (30 days pass without renewal).
4. `P` calls `extendExpiry(parentNode, labelhash, newExpiry)`.
5. **All stored fuses are cleared to 0**, including `CANNOT_TRANSFER`.
6. The subname now has a valid expiry but no fuse protection.

## Impact Analysis

**Limited impact** — the fuse clearing is real but does not grant the parent any capability they didn't already have:

- When `PARENT_CANNOT_CONTROL` (PCC) **IS** burned and the subname is expired: `_isWrapped()` returns `false` (because `ownerOf()` returns 0 via `_clearOwnerAndFuses`), so `extendExpiry()` **reverts** with `NameIsNotWrapped()`. The parent cannot call it. **Not exploitable.**

- When PCC is **NOT** burned and the subname is expired: The parent can already call `setSubnodeOwner()` to take full ownership (the expired subname's effective PCC is 0, so `_checkCanCallSetSubnodeOwner` passes). The `extendExpiry` fuse-clearing doesn't give additional power.

The issue is **cosmetic** — stored fuses are lost, but the security model is not broken because:
1. PCC-protected expired subnames can't have their expiry extended (guarded by `_isWrapped`).
2. Non-PCC expired subnames are already controllable by the parent.

## PoC (Foundry)

See `/home/z/ens-poc/test/ExtendExpiryFuseClear.t.sol` — two tests confirming the behavior:

```solidity
// Test 1: PCC | CU | CANNOT_TRANSFER subname expires → extendExpiry clears all
// Test 2: CU | CANNOT_TRANSFER (no PCC) subname expires → extendExpiry clears all
```

Both tests pass, confirming the fuse-clearing behavior.

## 3-Perspective Audit

### Prosecutor (Arguments for vulnerability)
- The `extendExpiry()` function unconditionally destroys stored fuse data when operating on expired subnames. This is clearly not the intended behavior — fuses should persist across expiry extensions. The use of `getData()` instead of `super.getData()` is an oversight. If future changes to `_isWrapped()` or `canModifyName()` relax the guards, this could become exploitable.

### Defense (Arguments against vulnerability)
- The current guards make this non-exploitable:
  - PCC-burned expired subnames: `_isWrapped()` returns false → `extendExpiry()` reverts.
  - Non-PCC expired subnames: parent already has full control via `setSubnodeOwner()`.
- The stored fuses being 0 after expiry doesn't matter because `getData()` already returns 0 for expired names. The only change is that the fuses remain 0 after the expiry is extended, but the parent could set new fuses via `setSubnodeOwner` or `setChildFuses` anyway.
- No user funds or assets are at risk. The subname owner already lost control when the name expired.

### Judge (Verdict)
- **Low severity / Informational.** The behavior is technically incorrect (fuses should be preserved), but the current access control guards prevent exploitation. The issue should be fixed for correctness and defense-in-depth, but it does not meet the threshold for a Medium or higher severity finding. The fix is trivial: use `super.getData()` instead of `getData()` in `extendExpiry()`.

## Recommendation

Change line in `extendExpiry()`:
```solidity
// Before (buggy):
(address owner, uint32 fuses, uint64 oldExpiry) = getData(uint256(node));

// After (fixed):
(address owner, uint32 fuses, uint64 oldExpiry) = super.getData(uint256(node));
```

This preserves the raw stored fuses across expiry extensions.
