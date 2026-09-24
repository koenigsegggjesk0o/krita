# RewardVault — `stakeOnBehalf` Is an Irreversible Gift, Not Delegation

## Metadata

| Field | Value |
|---|---|
| **Severity** | Low / Informational |
| **Area** | Proof-of-Liquidity / Reward Vault Staking |
| **Contract** | `RewardVault.sol` |
| **Function** | `stakeOnBehalf` |
| **Line** | 425–428 |
| **File** | `src/pol/rewards/RewardVault.sol` |
| **Audited commit** | `70e392fc` (2026-08-11) |

## Description

`RewardVault` provides three staking entry points:

| Function | Caller pays tokens to | Balance credited to | Caller can withdraw? |
|---|---|---|---|
| `stake(amount)` | `msg.sender` | `msg.sender` (self) | Yes (self-stake) |
| `delegateStake(account, amount)` | `msg.sender` | `account` (delegate) | Yes (via `delegateWithdraw`) |
| `stakeOnBehalf(account, amount)` | `msg.sender` | `account` (self-stake!) | **No** |

```solidity
function stakeOnBehalf(address account, uint256 amount)
    external nonReentrant whenNotPaused
{
    if (account == address(0)) ZeroAddress.selector.revertWith();
    _stake(account, amount);   // credits account.balance, pulls from msg.sender
}
```

`_stake(account, amount)` credits `account`'s staking balance but **does not**
update `_delegateStake[account]`.  This means the deposited tokens become part
of `account`'s **self-staked** balance:

```
selfStaked(account) = _accountInfo[account].balance - _delegateStake[account].delegateTotalStaked
```

Since `_delegateStake[account]` is not modified, `selfStaked` increases by the
full `amount`.  Only `account` can withdraw these tokens (via `withdraw` which
checks `checkSelfStakedBalance(msg.sender, amount)`).  The caller who called
`stakeOnBehalf` **cannot** recover their tokens — there is no
`delegateWithdraw` path because the delegation was never recorded.

## Attack Scenario

### Unintended Gift (User Error)

1. A user (or a frontend) intends to "stake on behalf of" a friend or a
   contract — meaning to retain ownership while earning rewards for the friend.
2. The user calls `stakeOnBehalf(friend, 1000e18)`.
3. 1000 tokens are transferred from the user to the vault and credited as the
   friend's **self-staked** balance.
4. The user cannot withdraw. The friend can withdraw at any time.
5. The user has irreversibly gifted 1000 tokens to the friend.

### Operator Confusion

An operator who stakes on behalf of a user they manage (e.g., a vault service)
loses custody of the staked tokens. If the user is malicious, they can
immediately `withdraw` and steal the operator's tokens.

## Comparison with `delegateStake`

```solidity
function delegateStake(address account, uint256 amount) external nonReentrant whenNotPaused {
    if (account == address(0)) ZeroAddress.selector.revertWith();
    if (msg.sender == account) NotDelegate.selector.revertWith();

    _stake(account, amount);
    unchecked {
        DelegateStake storage info = _delegateStake[account];   // ← tracked!
        info.delegateTotalStaked += amount;
        info.stakedByDelegate[msg.sender] += amount;             // ← caller can withdraw
    }
    emit DelegateStaked(account, msg.sender, amount);
}
```

`delegateStake` properly tracks the delegation, allowing the caller to later
call `delegateWithdraw(account, amount)` to recover their tokens.
`stakeOnBehalf` skips this tracking entirely.

## Proof of Concept

```solidity
function test_stakeOnBehalfIsGift() public {
    deal(stakeToken, alice, 1000e18);

    // Alice stakes on behalf of Bob
    vm.startPrank(alice);
    IERC20(stakeToken).approve(address(vault), 1000e18);
    vault.stakeOnBehalf(bob, 1000e18);
    vm.stopPrank();

    // Bob's balance = 1000, self-staked = 1000
    assertEq(vault.balanceOf(bob), 1000e18);
    assertEq(vault.getTotalDelegateStaked(bob), 0);  // no delegation tracked

    // Bob withdraws everything (self-stake allows it)
    vm.prank(bob);
    vault.withdraw(1000e18);
    assertEq(IERC20(stakeToken).balanceOf(bob), 1000e18);

    // Alice has 0 tokens and 0 stake — irreversible loss
    assertEq(IERC20(stakeToken).balanceOf(alice), 0);
    assertEq(vault.balanceOf(alice), 0);
}
```

## Impact

- **Irreversible fund loss for callers** who misunderstand `stakeOnBehalf` as a
  non-custodial delegation mechanism.
- **Theft vector** if a service provider uses `stakeOnBehalf` to stake on behalf
  of a user — the user can immediately withdraw and steal the tokens.
- **No on-chain indication** of the gift semantics. The function name implies
  "staking on behalf of" which could be interpreted as retaining ownership.

## Three-Perspective Audit

### 1. Attacker perspective
A malicious user can ask a service (or friend) to `stakeOnBehalf` on their
address, then immediately `withdraw` to steal the tokens. No exploit needed —
just social engineering.

### 2. Protocol perspective
The function may be intentionally designed as a "gift" mechanism (e.g., for
airdrops or subsidies). If so, it should be clearly documented and named
accordingly (e.g., `giftStake`). If it's meant to be a non-custodial
"stake-on-behalf", it should track delegation like `delegateStake`.

### 3. Auditor perspective
The asymmetry between `stakeOnBehalf` (no delegation tracking) and
`delegateStake` (full tracking) is a footgun. Either:
- Rename and document `stakeOnBehalf` as an irreversible gift, or
- Add delegation tracking so the caller retains withdrawal rights.

## Recommendation

**Option A — Document as gift (minimal change):**
```solidity
/// @notice Permanently stakes tokens on behalf of `account`. The caller's
/// tokens become `account`'s self-staked balance and CANNOT be recovered
/// by the caller. Use `delegateStake` if you need to retain withdrawal rights.
function stakeOnBehalf(address account, uint256 amount)
    external nonReentrant whenNotPaused
{
    ...
}
```

**Option B — Track delegation (behavioural change):**
```solidity
function stakeOnBehalf(address account, uint256 amount)
    external nonReentrant whenNotPaused
{
    if (account == address(0)) ZeroAddress.selector.revertWith();
    _stake(account, amount);
+   unchecked {
+       DelegateStake storage info = _delegateStake[account];
+       info.delegateTotalStaked += amount;
+       info.stakedByDelegate[msg.sender] += amount;
+   }
+   emit DelegateStaked(account, msg.sender, amount);
}
```

This makes `stakeOnBehalf` behave like `delegateStake` but without the
`msg.sender != account` restriction, allowing self-delegation.
