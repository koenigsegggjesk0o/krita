# BeraChef — Unbounded `rewardAllocationInactivityBlockSpan` Causes Overflow DoS

## Metadata

| Field | Value |
|---|---|
| **Severity** | Low |
| **Area** | Proof-of-Liquidity / Reward Allocation |
| **Contract** | `BeraChef.sol` |
| **Function** | `setRewardAllocationInactivityBlockSpan` + `_checkInactivity` + `getActiveRewardAllocation` |
| **Line** | 257–263 (setter), 572–575 (check), 384–402 (getter) |
| **File** | `src/pol/rewards/BeraChef.sol` |
| **Audited commit** | `70e392fc` (2026-08-11) |

## Description

`setRewardAllocationInactivityBlockSpan` enforces only a **minimum** (`43_200`
blocks ≈ 1 day) but no maximum:

```solidity
function setRewardAllocationInactivityBlockSpan(uint64 _rewardAllocationInactivityBlockSpan)
    external onlyOwner
{
    if (_rewardAllocationInactivityBlockSpan < MIN_REWARD_ALLOCATION_INACTIVITY_BLOCK_SPAN) {
        InvalidRewardAllocationInactivityBlockSpan.selector.revertWith();
    }
    // ← no upper-bound check
    rewardAllocationInactivityBlockSpan = _rewardAllocationInactivityBlockSpan;
    ...
}
```

The value is later used inside `_checkInactivity`, which is called from the
view function `getActiveRewardAllocation` — itself called by the `Distributor`
on every block via `beraChef.getActiveRewardAllocation(pubkey)`:

```solidity
function _checkInactivity(bytes calldata valPubkey, uint64 startBlock)
    internal view returns (bool)
{
    if (isValExemptedFromInactivity[valPubkey]) return false;
    return block.number > startBlock + rewardAllocationInactivityBlockSpan;  // ← uint64 addition
}
```

If `rewardAllocationInactivityBlockSpan` is set to a large value (e.g.,
`type(uint64).max`), the addition `startBlock + rewardAllocationInactivityBlockSpan`
**overflows** in Solidity 0.8.x and reverts. Since `getActiveRewardAllocation`
is called in the hot path of `_distributeFor`, every call to
`distributeFor` will revert, **halting all block-reward distribution** for any
validator whose `startBlock != 0` and who is not exempted from inactivity.

## Attack Scenario

1. A governance proposal (or a compromised owner) sets
   `rewardAllocationInactivityBlockSpan = type(uint64).max` (or any value
   large enough that `startBlock + value` overflows for current block numbers).
2. The `Distributor._distributeFor` calls
   `beraChef.getActiveRewardAllocation(pubkey)`.
3. Inside, `_checkInactivity(valPubkey, ara.startBlock)` computes
   `startBlock + rewardAllocationInactivityBlockSpan` → **overflow → revert**.
4. `distributeFor` reverts. No block rewards are processed for that validator.
5. If **all** validators have non-zero `startBlock`, **all** reward distribution
   halts.
6. Since `distributeFor` is called by the system address (post-Pectra11), a
   persistent revert prevents the chain from processing PoL rewards until
   governance reduces the span or exempts all validators.

## Proof of Concept

```solidity
function test_inactivityOverflowHaltsDistribution() public {
    // Validator has an active reward allocation
    vm.prank(distributor);
    beraChef.activateReadyQueuedRewardAllocation(valPubkey);
    // activeRewardAllocations[valPubkey].startBlock = block.number (> 0)

    // Owner sets inactivity span to max
    vm.prank(owner);
    beraChef.setRewardAllocationInactivityBlockSpan(type(uint64).max);
    // No revert — only minimum is checked

    // Distributor tries to get active allocation
    vm.expectRevert(); // arithmetic overflow in _checkInactivity
    beraChef.getActiveRewardAllocation(valPubkey);

    // → distributeFor reverts → all PoL rewards halted
}
```

## Impact

- **Chain-wide PoL reward halt.** If governance (or a compromised owner) sets a
  large value, all non-exempt validators' reward distribution reverts.
- **No automatic recovery.** The only remediation is another governance call to
  reduce the span or exempt validators — but governance itself may be slowed by
  the timelock.
- **Governance-gated.** Requires `onlyOwner` (governance timelock), so an
  external attacker cannot trigger this directly. However, a governance
  proposal with an erroneous value, or a compromised owner key, can cause it.

## Three-Perspective Audit

### 1. Attacker perspective
Not directly exploitable without governance access. But if the attacker controls
governance (e.g., via a flash-loan voting attack or a malicious proposal), this
is a one-call chain halt.

### 2. Protocol perspective
The setter mirrors `setRewardAllocationBlockDelay` which **does** have an upper
bound (`MAX_REWARD_ALLOCATION_BLOCK_DELAY`). The inactivity span setter should
follow the same pattern for consistency and safety.

### 3. Auditor perspective
Even without a malicious intent, a fat-fingered governance transaction setting
the span to a very large (but not max) value could cause overflow once block
numbers grow. The fix is a simple upper-bound constant, consistent with the
existing `MAX_REWARD_ALLOCATION_BLOCK_DELAY` pattern.

## Recommendation

```solidity
+ /// @dev Maximum inactivity block span (~30 days at 2s blocks).
+ uint64 public constant MAX_REWARD_ALLOCATION_INACTIVITY_BLOCK_SPAN = 1_315_000;

  function setRewardAllocationInactivityBlockSpan(uint64 _rewardAllocationInactivityBlockSpan)
      external onlyOwner
  {
-     if (_rewardAllocationInactivityBlockSpan < MIN_REWARD_ALLOCATION_INACTIVITY_BLOCK_SPAN) {
+     if (
+         _rewardAllocationInactivityBlockSpan < MIN_REWARD_ALLOCATION_INACTIVITY_BLOCK_SPAN
+             || _rewardAllocationInactivityBlockSpan > MAX_REWARD_ALLOCATION_INACTIVITY_BLOCK_SPAN
+     ) {
          InvalidRewardAllocationInactivityBlockSpan.selector.revertWith();
      }
      rewardAllocationInactivityBlockSpan = _rewardAllocationInactivityBlockSpan;
      ...
  }
```

Additionally, use `unchecked` with a cap in `_checkInactivity` to avoid any
overflow even if the span is large:

```solidity
function _checkInactivity(bytes calldata valPubkey, uint64 startBlock)
    internal view returns (bool)
{
    if (isValExemptedFromInactivity[valPubkey]) return false;
+   unchecked {
+     // startBlock is always <= block.number; span is capped.
+     return block.number - startBlock > rewardAllocationInactivityBlockSpan;
+   }
-   return block.number > startBlock + rewardAllocationInactivityBlockSpan;
}
```

Using `block.number - startBlock` (subtraction, always safe since
`startBlock <= block.number`) avoids the overflow entirely.
