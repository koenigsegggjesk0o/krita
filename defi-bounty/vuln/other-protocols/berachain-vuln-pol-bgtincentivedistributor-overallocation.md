# BGTIncentiveDistributor — Over-Allocation in `allocateIncentiveToValidator`

## Metadata

| Field | Value |
|---|---|
| **Severity** | Low |
| **Area** | Proof-of-Liquidity / Incentive Distribution |
| **Contract** | `BGTIncentiveDistributor.sol` |
| **Function** | `allocateIncentiveToValidator` |
| **Line** | 147–156 |
| **File** | `src/pol/rewards/BGTIncentiveDistributor.sol` |
| **Audited commit** | `70e392fc` (2026-08-11) |

## Description

`allocateIncentiveToValidator` allows a `MANAGER_ROLE` holder to set the
incentive-token balance attributed to the **zero pubkey** (a catch-all pool).
The solvency check compares `amount` against the contract's **total token
balance**, which includes tokens already attributed to other validators via
`receiveIncentive`.

```solidity
function allocateIncentiveToValidator(address token, uint256 amount)
    external onlyRole(MANAGER_ROLE)
{
    bytes memory pubkey = bytes(hex"0000...0000");   // zero pubkey
    if (amount > IERC20(token).balanceOf(address(this))) {   // ← TOTAL balance, not available
        InsufficientIncentiveTokens.selector.revertWith();
    }
    incentiveTokensPerValidator[pubkey][token] = amount;     // ← SET (not +=)
    emit IncentiveAllocated(pubkey, token, amount);
}
```

Because the check uses `balanceOf(address(this))` — which includes tokens
attributed to real validators — the manager can allocate the **same tokens
twice**: once to a real validator (via `receiveIncentive`) and again to the zero
pubkey (via `allocateIncentiveToValidator`).

## Attack Scenario

| Step | Actor | Action | State |
|---|---|---|---|
| 1 | Vault | `receiveIncentive(valPubkey_A, USDC, 1000)` via `_processIncentives` | `incentiveTokensPerValidator[A][USDC] = 1000`, `balance = 1000` |
| 2 | Manager | `allocateIncentiveToValidator(USDC, 1000)` | `incentiveTokensPerValidator[zero][USDC] = 1000`, `balance` still 1000 |
| 3 | User X | claims 1000 USDC from `valPubkey_A` | `balance = 0`, `map[A] = 0` |
| 4 | User Y | claims 1000 USDC from zero pubkey | `balance = 0` but `map[zero] = 1000` → `safeTransfer` **reverts** (InsufficientIncentiveTokens already passed, but actual `IERC20.safeTransfer` fails) |

At step 4, the `_claim` function passes the `incentiveTokensPerValidator` check
(mapping says 1000 ≥ 1000 ✓) but `IERC20(token).safeTransfer(_account, _amount)`
reverts because the contract holds 0 USDC. User Y's claim is permanently blocked
until more USDC is deposited.

## Proof of Concept

```solidity
function test_overAllocationBlocksClaim() public {
    // Step 1: 1000 USDC attributed to validator A
    usdc.mint(address(distributor), 1000e6);
    vm.prank(address(vault));
    distributor.receiveIncentive(valPubkeyA, address(usdc), 1000e6);
    // balance = 1000, map[A] = 1000

    // Step 2: manager allocates same 1000 to zero pubkey
    vm.prank(manager);
    distributor.allocateIncentiveToValidator(address(usdc), 1000e6);
    // balance still 1000, map[zero] = 1000  ← DOUBLE COUNTED

    // Step 3: user claims from validator A — succeeds, drains balance
    claimFromValidator(valPubkeyA, 1000e6);
    assertEq(usdc.balanceOf(address(distributor)), 0);

    // Step 4: user claims from zero pubkey — REVERTS (no USDC left)
    vm.expectRevert();   // safeTransfer fails
    claimFromValidator(zeroPubkey, 1000e6);
}
```

## Impact

- **DoS on incentive claims.** When the manager over-allocates, claims from the
  zero-pubkey pool (or from real validators, depending on claim order) will
  revert because the contract lacks sufficient token balance.
- **Stuck incentives.** Once the balance is drained by the first claimer, all
  subsequent claimers for the over-allocated pool are permanently blocked until
  the manager deposits more tokens or reduces the allocation.
- **Trust assumption.** This is a manager-controlled function, so exploitation
  requires a compromised or negligent manager account. However, the contract
  should enforce the invariant `sum(incentiveTokensPerValidator[*][token]) ≤
  balance` regardless of trust.

## Three-Perspective Audit

### 1. Attacker perspective
Not directly exploitable by an external attacker. Requires a compromised
`MANAGER_ROLE` or an operational error. However, the resulting DoS is
permanent until governance intervenes.

### 2. Protocol perspective
The function uses `=` (set) rather than `+=` (add), which means it overwrites
the zero-pubkey allocation. This is by design (the manager sets the total
allocation for the catch-all pool). The bug is that the solvency check should
compare against **available** balance (`balance - sum(other allocations)`),
not total balance.

### 3. Auditor perspective
The cleanest fix is to track a running `totalAllocated[token]` and check
`amount ≤ balance - (totalAllocated - currentZeroPubkeyAllocation)`.
Alternatively, since `=` is used, check `amount ≤ balance -
sumOtherAllocations`.

## Recommendation

```solidity
mapping(address => uint256) public totalAllocatedPerToken;

function receiveIncentive(bytes calldata pubkey, address token, uint256 _amount) external {
    IERC20(token).safeTransferFrom(msg.sender, address(this), _amount);
    incentiveTokensPerValidator[pubkey][token] += _amount;
+   totalAllocatedPerToken[token] += _amount;
    ...
}

function allocateIncentiveToValidator(address token, uint256 amount)
    external onlyRole(MANAGER_ROLE)
{
    bytes memory pubkey = bytes(hex"0000...0000");
-   if (amount > IERC20(token).balanceOf(address(this))) {
+   uint256 available = IERC20(token).balanceOf(address(this))
+       - (totalAllocatedPerToken[token] - incentiveTokensPerValidator[pubkey][token]);
+   if (amount > available) {
        InsufficientIncentiveTokens.selector.revertWith();
    }
+   totalAllocatedPerToken[token] += amount - incentiveTokensPerValidator[pubkey][token];
    incentiveTokensPerValidator[pubkey][token] = amount;
    ...
}
```

And in `_claim`, update `totalAllocatedPerToken` when subtracting:
```solidity
incentiveTokensPerValidator[pubkey][token] -= _amount;
+ totalAllocatedPerToken[token] -= _amount;
```
