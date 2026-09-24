# Ethena StakedENA + EnaSilo — Deep Security Analysis

**Analyst:** Opus (DeFi security researcher)
**Task ID:** eth-stakedena-deep
**Date:** 2026-09-24
**Scope:**
- `/home/z/fkr-step1/defi-bounty/contracts/StakedENA.sol` (410 lines, 15,600 bytes)
- `/home/z/fkr-step1/defi-bounty/contracts/ENASilo.sol` (28 lines, 719 bytes)
- `/home/z/fkr-step1/defi-bounty/contracts/SingleAdminAccessControlUpgradeable.sol` (81 lines, dependency)

**Comparison baseline:**
- `/home/z/ethena-usde/contracts/contracts/StakedUSDe.sol` (268 lines) — older V1 staking
- `/home/z/ethena-usde/contracts/contracts/StakedUSDeV2.sol` (131 lines) — newer V2 staking with cooldown
- `/home/z/ethena-usde/contracts/contracts/USDeSilo.sol` (30 lines) — older silo

**On-chain addresses (mainnet):**
- StakedENA proxy: `0x8bE3460A480c80728a8C4D7a5D5303c85ba7B3b9`
- StakedENA implementation: `0x7fD57b46aE1a7b14f6940508381877Ee03e1018B`
- EnaSilo: `0x85fEB4edC3198fEfC5aD36b80CfF182eF2bF2F79`

**Solidity:** 0.8.26
**OpenZeppelin:** Upgradeable v4.x (contracts-upgradeable import paths)

---

## 0. Executive Summary / Honest Conclusion

Both contracts were read line-by-line. **No CRITICAL vulnerability (theft of unrelated user funds, permanent freezing of protocol funds by an external attacker) was found.** StakedENA is a faithful, upgradeable port of `StakedUSDeV2` merged with `StakedUSDe`'s vesting/reward logic. It is defensively coded: every state-changing external function is `nonReentrant`, the Checks-Effects-Interactions pattern is respected, rounding consistently favors the vault, role-based access control is granular, and the ERC4626 inflation attack is double-mitigated (OZ v4 virtual shares/assets offset + Ethena's own `_checkMinShares` requiring ≥1e18 shares).

The most notable finding is the **same Medium design gap** already documented for `StakedUSDeV2.unstake` (finding V2-1 in `ethena-usde-deep-analysis.md`): `unstake` does not consult `BLACKLISTED_ROLE` on `msg.sender` or `receiver`. A user who enters the cooldown flow *before* being blacklisted can still claim their silo'd ENA after the cooldown expires — even while fully blacklisted — because `unstake` skips the blacklist check and the admin's `redistributeLockedAmount` confiscation tool only operates on live sENA balances (which are already burned once a cooldown begins). This is a compliance/blacklist-evasion gap, **not** an arbitrary-fund-theft bug. It is **not** a new discovery — it is the StakedENA analogue of a known issue in the V2 lineage and is unlikely to be a fresh Immunefi target (the V2 version has been live and audited since Oct 2023; re-reporting would likely duplicate).

Lower-severity observations (Low / Informational) are documented per-contract below, with explicit comparison to `StakedUSDe`/`StakedUSDeV2`.

**Critical bugs: 0. Medium bugs: 1 (known/duplicate of V2-1). Low: 1. Informational: 6.**

---

## 1. Contract: `StakedENA.sol` — Architecture Overview

### Purpose
ERC4626Upgradeable staking vault for the ENA governance token. Users deposit ENA, receive sENA shares, and earn rewards streamed in by a `REWARDER_ROLE` and linearly vested over a configurable period (max 90 days). Includes a cooldown-based unstaking flow (max 90 days) that routes ENA through a separate `EnaSilo` contract, plus a single-tier blacklist (`BLACKLISTED_ROLE`) with an admin confiscation tool (`redistributeLockedAmount`).

### Inheritance chain
```
SingleAdminAccessControlUpgradeable  (IERC5313, ISingleAdminAccessControl, AccessControlUpgradeable)
├─ ReentrancyGuardUpgradeable
├─ ERC20PermitUpgradeable            (ERC20Upgradeable, EIP712Upgradeable, NoncesUpgradeable)
├─ ERC4626Upgradeable                (ERC20Upgradeable — diamond-inherited with ERC20PermitUpgradeable)
└─ IStakedENA
```

### Key state variables (appended after inherited storage)
| Slot | Variable | Type | Notes |
|------|----------|------|-------|
| +0 | `vestingAmount` | uint256 | Total ENA being vested from last distribution |
| +1 | `lastDistributionTimestamp` | uint256 | When the current vesting batch started |
| +2 | `vestingPeriod` | uint24 | Configurable, max 90 days, set to 7 days in `initialize` |
| +3 (packed) | `cooldownDuration` | uint24 | 0 = standard ERC4626; >0 = cooldown mode. Set to 7 days in `initialize` |
| +4 | `silo` | EnaSilo | Set once in `initialize` via `new EnaSilo(...)` |
| +5 | `cooldowns` | mapping(address → UserCooldown) | Per-user cooldown state |

`UserCooldown = { uint104 cooldownEnd; uint152 underlyingAmount }` — packed into one 256-bit slot.

### Constants
| Name | Value | Purpose |
|------|-------|---------|
| `REWARDER_ROLE` | keccak256("REWARDER_ROLE") | Can call `transferInRewards` |
| `BLACKLIST_MANAGER_ROLE` | keccak256("BLACKLIST_MANAGER_ROLE") | Can add/remove blacklist |
| `BLACKLISTED_ROLE` | keccak256("BLACKLISTED_ROLE") | Frozen role (single tier — no soft/full split like V1) |
| `_MIN_SHARES` | 1 ether (1e18) | Donation-attack floor on `totalSupply` |
| `MAX_VESTING_PERIOD` | 90 days | Cap on `vestingPeriod` |
| `MAX_COOLDOWN_DURATION` | 90 days | Cap on `cooldownDuration` |

---

## 2. StakedENA — Line-by-Line Analysis

### Lines 1–10: Pragmas & imports
- `pragma solidity 0.8.26` — newer than V1/V2's `0.8.20`. Includes bug fixes; no security regression.
- Imports OZ upgradeable variants (ERC4626Upgradeable, SafeERC20Upgradeable, ReentrancyGuardUpgradeable, ERC20PermitUpgradeable) + local `SingleAdminAccessControlUpgradeable`, `IStakedENA`, `ENASilo`. ✅

### Lines 16–22: Contract declaration
Multiple inheritance from upgradeable bases + `IStakedENA`. Diamond inheritance on `ERC20Upgradeable` (via both `ERC20PermitUpgradeable` and `ERC4626Upgradeable`) — Solidity resolves this correctly. ✅

### Lines 25–39: Constants
- `BLACKLISTED_ROLE` is a **single** blacklist tier. Compare V1 which has `SOFT_RESTRICTED_STAKER_ROLE` + `FULL_RESTRICTED_STAKER_ROLE` (two tiers). StakedENA collapses to one. This simplifies the model but loses the "soft" (deposit-blocked-only) tier.
- `_MIN_SHARES = 1 ether` — same as V1's `MIN_SHARES`.
- `MAX_VESTING_PERIOD = 90 days` — **new** (V1 had fixed `VESTING_PERIOD = 8 hours`). Configurable vesting is a new feature.
- `MAX_COOLDOWN_DURATION = 90 days` — same as V2.

### Lines 41–55: State variables
See table above. `silo` is a regular state variable (not `immutable` as in V2), set in `initialize`. This is correct for upgradeables (immutable doesn't survive proxy delegatecall). ✅

### Lines 57–79: Modifiers
- `ensureCooldownOff` / `ensureCooldownOn` — gate `withdraw`/`redeem` vs `cooldownAssets`/`cooldownShares`. Same as V2.
- `notZero(amount)` — reverts on 0. Same as V1.
- `notOwner(target)` — prevents blacklisting the current admin. Same as V1. Note: does **not** protect the *pending* admin (documented as acceptable — see `addToBlacklist` comment lines 222–225).

### Lines 81–84: Constructor
```solidity
constructor() { _disableInitializers(); }
```
Prevents initialization of the implementation contract directly. ✅ Standard upgradeable pattern. The `@custom:oz-upgrades-unsafe-allow constructor` annotation allows the constructor (which would otherwise be flagged by the upgrades plugin).

### Lines 86–109: `initialize`
```solidity
function initialize(IERC20Upgradeable _asset, address _initialRewarder, address _owner) public initializer {
  __ERC20_init("Staked ENA", "sENA");
  __ERC4626_init(_asset);
  __ERC20Permit_init("sENA");
  __ReentrancyGuard_init();
  if (_owner == address(0) || _initialRewarder == address(0) || address(_asset) == address(0)) {
    revert InvalidZeroAddress();
  }
  silo = new EnaSilo(address(this), address(_asset));
  _updateVestingPeriod(7 days);
  _grantRole(REWARDER_ROLE, _initialRewarder);
  _grantRole(DEFAULT_ADMIN_ROLE, _owner);
  cooldownDuration = 7 days;
}
```

**Observations:**
1. `initializer` modifier ensures single-call. ✅
2. Zero-address checks on all three params. ✅
3. `silo = new EnaSilo(address(this), address(_asset))` — `address(this)` in the proxy context is the **proxy address**, so the silo is bound to the proxy, not the implementation. ✅ Correct for upgradeables. If the implementation is upgraded later, the silo still points to the proxy. ✅
4. `_updateVestingPeriod(7 days)` — sets initial vesting to 7 days (V1 was hardcoded 8 hours).
5. `cooldownDuration = 7 days` — **different from V2** which sets `MAX_COOLDOWN_DURATION = 90 days` in its constructor. StakedENA starts with a shorter cooldown. Either choice is valid.
6. `_grantRole(DEFAULT_ADMIN_ROLE, _owner)` triggers the override in `SingleAdminAccessControlUpgradeable` which sets `_currentDefaultAdmin = _owner` and clears `_pendingDefaultAdmin`. ✅
7. **No initial deposit to a dead address.** The comment in `_checkMinShares` (line 325) says *"This should never happen due to the initial deposit to the dead address"* — but **no such deposit exists in `initialize`**. The comment is misleading/inherited from a different design. The actual mitigation is `_checkMinShares` itself (forces first depositor to mint ≥1e18 shares). See **E-2** below.

### Lines 111–137: `withdraw` / `redeem` overrides
Both add `ensureCooldownOff` — standard ERC4626 withdraw/redeem only works when `cooldownDuration == 0`. When cooldown is on, users must use `cooldownAssets`/`cooldownShares` + `unstake`. Same as V2. ✅

### Lines 139–159: `unstake`
```solidity
function unstake(address receiver) external nonReentrant {
  UserCooldown storage userCooldown = cooldowns[msg.sender];
  uint256 assets = userCooldown.underlyingAmount;
  if (block.timestamp >= userCooldown.cooldownEnd || cooldownDuration == 0) {
    userCooldown.cooldownEnd = 0;
    userCooldown.underlyingAmount = 0;
    silo.withdraw(receiver, assets);
    emit Unstake(msg.sender, receiver, assets);
  } else {
    revert InvalidCooldown();
  }
}
```

**Differences from V2:**
- **`nonReentrant` added** (V2's `unstake` lacked it). ✅ Strengthening — closes a defense-in-depth gap (V2 was safe in practice because ENA/USDe has no transfer hooks and CEI was respected, but the modifier is good hygiene).
- **Blacklist check still missing.** ❌ Same as V2-1. See **M-1**.

**CEI:** State (`cooldownEnd`, `underlyingAmount`) zeroed BEFORE `silo.withdraw`. ✅
**Reentrancy:** `nonReentrant` + CEI + ENA has no transfer hooks → no reentrancy vector. ✅
**Receiver validation:** None — `receiver` can be any address (including contracts, fresh EOAs). Combined with the missing blacklist check (M-1), a blacklisted user can route their claimed ENA to any address.

### Lines 161–187: `cooldownAssets` / `cooldownShares`
```solidity
function cooldownAssets(uint256 assets) external ensureCooldownOn returns (uint256 shares) {
  if (assets > maxWithdraw(msg.sender)) revert ExcessiveWithdrawAmount();
  shares = previewWithdraw(assets);  // rounds UP (conservative)
  cooldowns[msg.sender].cooldownEnd = uint104(block.timestamp) + cooldownDuration;
  cooldowns[msg.sender].underlyingAmount += uint152(assets);
  _withdraw(msg.sender, address(silo), msg.sender, assets, shares);
  emit CooldownStarted(msg.sender, assets, shares);
}
```

**Observations:**
1. `ensureCooldownOn` — only callable when cooldown is active. ✅
2. `assets > maxWithdraw(msg.sender)` — caps at the user's withdrawable assets. `maxWithdraw = previewRedeem(balanceOf)` (rounded down). ✅
3. `shares = previewWithdraw(assets)` — rounds UP, so the user burns slightly more shares than the strict ratio. Conservative (favors vault). ✅
4. `cooldownEnd` is **overwritten** (not added) on each call — topping up a cooldown restarts the clock. `underlyingAmount` is **accumulated** (`+=`). Same as V2. Documented behavior. Informational.
5. `_withdraw(msg.sender, address(silo), msg.sender, assets, shares)` — burns shares from msg.sender, transfers `assets` ENA to the silo. `_withdraw` checks `BLACKLISTED_ROLE` on `_owner` (msg.sender) and `receiver` (silo). So a currently-blacklisted user CANNOT start a cooldown. ✅
6. `cooldownShares` is the mirror — uses `previewRedeem` (rounds DOWN, conservative) and `maxRedeem = balanceOf`. ✅

**Type-cast safety:**
- `uint104(block.timestamp) + cooldownDuration` — `block.timestamp ≈ 1.7e9`, `cooldownDuration ≤ 90 days ≈ 7.8e6`. Sum ≈ 1.7e9. `uint104.max ≈ 2e31`. ✅ No overflow.
- `uint152(assets)` — `assets ≤ maxWithdraw ≤ totalAssets ≈ ENA supply ≈ 8e27`. `uint152.max ≈ 5.7e45`. ✅ No truncation. (Solidity 0.8 would revert on the `+=` if it overflowed `uint152`, but the values are far from the limit.)

**Not `nonReentrant` directly, but calls `_withdraw` which is `nonReentrant`.** A reentrant call to `cooldownAssets` (via an ENA transfer hook, if one existed) would update `cooldownEnd`/`underlyingAmount` in storage, then hit `_withdraw` which reverts on the reentrancy guard. The entire tx reverts, so no state persists. ✅ Safe.

### Lines 189–199: `setCooldownDuration`
```solidity
function setCooldownDuration(uint24 duration) external nonReentrant onlyRole(DEFAULT_ADMIN_ROLE) {
  if (duration > MAX_COOLDOWN_DURATION) revert InvalidCooldown();
  uint24 previousDuration = cooldownDuration;
  cooldownDuration = duration;
  emit CooldownDurationUpdated(previousDuration, cooldownDuration);
}
```
- `nonReentrant` added (V2's version lacked it). ✅ Strengthening.
- Caps at 90 days. ✅
- Existing cooldowns use their stored `cooldownEnd` — not retroactively affected. ✅
- Setting to 0 enables standard ERC4626 withdraw/redeem and lets pending cooldowns claim immediately (`|| cooldownDuration == 0` in `unstake`). ✅

### Lines 201–207: `updateVestingPeriod`
```solidity
function updateVestingPeriod(uint24 newVestingPeriod) external nonReentrant onlyRole(DEFAULT_ADMIN_ROLE) {
  _updateVestingPeriod(newVestingPeriod);
}
```
- **New** (V1 had fixed `VESTING_PERIOD = 8 hours`). Admin can configure.
- `_updateVestingPeriod` reverts if `getUnvestedAmount() > 0` (can't change mid-vesting) and if `> MAX_VESTING_PERIOD`. ✅
- Can set to 0 — meaning rewards are instantly vested (`getUnvestedAmount` returns 0 when `vestingPeriod == 0` because `timeSinceLastDistribution >= 0` is always true). Centralization concern (admin can skip vesting), not a code bug.

### Lines 209–219: `transferInRewards`
```solidity
function transferInRewards(uint256 amount) external nonReentrant onlyRole(REWARDER_ROLE) notZero(amount) {
  _updateVestingAmount(amount);  // state update FIRST
  IERC20Upgradeable(asset()).safeTransferFrom(msg.sender, address(this), amount);  // external call
  emit RewardsReceived(amount);
}
```
- CEI: state (`vestingAmount`, `lastDistributionTimestamp`) updated before external `safeTransferFrom`. ✅
- `nonReentrant`. ✅
- `_updateVestingAmount` reverts `StillVesting` if previous batch not fully vested. So the rewarder must wait `vestingPeriod` between distributions. ✅
- `safeTransferFrom` handles non-standard ERC20 return values. ✅ But **does not account for fee-on-transfer** — `vestingAmount = amount` (requested), not the actually-received amount. ENA is not fee-on-transfer, so no issue. Same as V1. ✅
- If `safeTransferFrom` fails (insufficient allowance/balance), the whole tx reverts, rolling back state. ✅

### Lines 221–237: `addToBlacklist` / `removeFromBlacklist`
```solidity
function addToBlacklist(address target) external nonReentrant onlyRole(BLACKLIST_MANAGER_ROLE) notOwner(target) {
  _grantRole(BLACKLISTED_ROLE, target);
}
function removeFromBlacklist(address target) external nonReentrant onlyRole(BLACKLIST_MANAGER_ROLE) {
  _revokeRole(BLACKLISTED_ROLE, target);
}
```
- **Simplified from V1** — no `isFullBlacklisting` parameter (single tier). ✅
- `nonReentrant` added (V1 lacked it). ✅ Strengthening.
- `notOwner` protects the current admin. Does NOT protect the *pending* admin — documented as acceptable (pending admin can `acceptAdmin` then unblacklist themselves). ✅
- `removeFromBlacklist` has no `notOwner` check (not needed — removing blacklist is always safe). ✅

### Lines 239–252: `rescueTokens`
```solidity
function rescueTokens(address token, uint256 amount, address to) external nonReentrant onlyRole(DEFAULT_ADMIN_ROLE) {
  if (address(token) == asset()) revert InvalidToken();
  IERC20Upgradeable(token).safeTransfer(to, amount);
  emit TokensRescued(token, to, amount);
}
```
- Blocks rescuing ENA (the asset) — protects staker funds. ✅
- Allows rescuing sENA (the shares token) and any other token — per the comment, sENA should never sit in this contract, so rescuing accidental sends is intended. ✅
- `nonReentrant`. ✅ Same as V1.

### Lines 254–273: `redistributeLockedAmount`
```solidity
function redistributeLockedAmount(address from, address to) external nonReentrant onlyRole(DEFAULT_ADMIN_ROLE) {
  if (!hasRole(BLACKLISTED_ROLE, from) || hasRole(BLACKLISTED_ROLE, to)) revert OperationNotAllowed();
  uint256 amountToDistribute = balanceOf(from);
  if(amountToDistribute == 0) revert InvalidAmount();
  uint256 enaToVest = previewRedeem(amountToDistribute);
  _burn(from, amountToDistribute);
  if (to == address(0)) {
    _updateVestingAmount(enaToVest);
  } else {
    _mint(to, amountToDistribute);
  }
  emit StakedENARedistributed(from, to, amountToDistribute);
}
```

**Differences from V1:**
- Adds `amountToDistribute == 0` revert. ✅ Cleaner (V1 would no-op + emit).
- Uses `BLACKLISTED_ROLE` (single tier) instead of `FULL_RESTRICTED_STAKER_ROLE`. ✅
- Logic equivalent: V1 checks `hasRole(from) && !hasRole(to)`; StakedENA checks `!hasRole(from) || hasRole(to)` (De Morgan's equivalent). ✅

**Behavior:**
- `to != address(0)`: burns `from`'s sENA, mints the same amount to `to`. `totalSupply` unchanged. The `to` now holds the confiscated sENA and can redeem/withdraw it. ✅ Confiscation-to-treasury path.
- `to == address(0)`: burns `from`'s sENA, vests the equivalent ENA to remaining stakers via `_updateVestingAmount`. `totalSupply` decreases. ✅ Confiscation-and-redistribute path.
- `_updateVestingAmount` reverts `StillVesting` if a prior reward batch is mid-vesting. So the burn-to-0 path is **blocked during vesting**. The admin can still use the mint-to-treasury path during vesting. ✅ Not a vulnerability (design choice).

**Cannot reach silo'd funds:** If `from` already cooldowned, `balanceOf(from) == 0` → reverts `InvalidAmount`. So the admin cannot confiscate ENA that has already moved to the silo. This is the root of M-1. ❌

**Accounting check (mental simulation):**
- Before: `totalSupply = S`, `totalAssets = A`, `from` has `f` shares. `enaToVest = f * A / S` (rounded down).
- After burn: `totalSupply = S - f`, `totalAssets = A` (burn doesn't move ENA).
- After `_updateVestingAmount(enaToVest)`: `vestingAmount = enaToVest`, `getUnvestedAmount() = enaToVest` (immediately), so `totalAssets() = A - enaToVest = A - f*A/S = A*(S-f)/S`.
- Per-share value after: `(A*(S-f)/S) / (S-f) = A/S`. ✅ Unchanged. As vesting progresses, per-share value increases to `A/(S-f)`. ✅ Correct — remaining stakers gradually receive the confiscated ENA.

### Lines 275–280: `useNonce`
```solidity
function useNonce() external returns (uint256) {
  return _useNonce(msg.sender);
}
```
- **New** (not in V1/V2). Wraps OZ `NoncesUpgradeable._useNonce` so a user can increment their permit nonce, invalidating any unexecuted `permit` signatures before their deadline.
- No `nonReentrant`, but `_useNonce` is a pure storage update (no external calls). ✅ Safe.
- Security feature (permit cancellation). ✅

### Lines 282–305: `totalAssets` / `getUnvestedAmount`
```solidity
function totalAssets() public view override returns (uint256) {
  return IERC20Upgradeable(asset()).balanceOf(address(this)) - getUnvestedAmount();
}
function getUnvestedAmount() public view returns (uint256) {
  uint256 timeSinceLastDistribution = block.timestamp - lastDistributionTimestamp;
  if (timeSinceLastDistribution >= vestingPeriod) return 0;
  uint256 deltaT;
  unchecked { deltaT = (vestingPeriod - timeSinceLastDistribution); }
  return (deltaT * vestingAmount) / vestingPeriod;
}
```
- Same logic as V1, but with configurable `vestingPeriod` instead of constant `VESTING_PERIOD`. ✅
- `unchecked` block for `deltaT = vestingPeriod - timeSinceLastDistribution` is safe because the `if` above guarantees `timeSinceLastDistribution < vestingPeriod`. ✅
- `block.timestamp - lastDistributionTimestamp` cannot underflow because `lastDistributionTimestamp` is always set to `block.timestamp` in `_updateVestingAmount` and never set to a future value. ✅
- **Underflow check on `totalAssets`:** `balanceOf(this) - getUnvestedAmount()`. Could this underflow? `getUnvestedAmount() ≤ vestingAmount`. And `vestingAmount` was set to `amount` in `transferInRewards`, which then did `safeTransferFrom(msg.sender, this, amount)` — so `balanceOf` increased by exactly `amount` (assuming no fee-on-transfer). Therefore `balanceOf ≥ vestingAmount ≥ getUnvestedAmount` after each `transferInRewards`. Withdrawals reduce `balanceOf` but are capped by `totalAssets()` (which already subtracts unvested), so users can only withdraw the vested portion. ✅ No underflow.
- Edge case: `vestingPeriod == 0` → `timeSinceLastDistribution >= 0` always true → returns 0. ✅ No division by zero (the division is unreachable).

### Lines 307–310: `decimals`
```solidity
function decimals() public pure override(ERC4626Upgradeable, ERC20Upgradeable) returns (uint8) {
  return 18;
}
```
- Hardcoded 18. ENA has 18 decimals. ✅ No mismatch.
- `pure` (no storage read). ✅

### Lines 312–319: `renounceRole`
```solidity
function renounceRole(bytes32, address) public virtual override {
  revert OperationNotAllowed();
}
```
- Disables role renunciation entirely. Same as V1. ✅ The admin can revoke roles via `revokeRole` if needed.

### Lines 322–330: `_checkMinShares`
```solidity
function _checkMinShares() internal view {
  uint256 _totalSupply = totalSupply();
  if (_totalSupply > 0 && _totalSupply < _MIN_SHARES) revert MinSharesViolation();
}
```
- Reverts if `totalSupply` is in `(0, 1e18)`. Allows `0` and `≥1e18`. ✅
- Called in `_deposit` and `_withdraw` (after the super call). ✅
- **Misleading comment** (line 325): *"This should never happen due to the initial deposit to the dead address"* — but `initialize` makes no such deposit. The actual mitigation is the floor itself. See **E-2**.

### Lines 332–371: `_deposit` / `_withdraw` overrides
```solidity
function _deposit(address caller, address receiver, uint256 assets, uint256 shares)
  internal override nonReentrant notZero(assets) notZero(shares)
{
  super._deposit(caller, receiver, assets, shares);
  _checkMinShares();
}

function _withdraw(address caller, address receiver, address _owner, uint256 assets, uint256 shares)
  internal override nonReentrant notZero(assets) notZero(shares)
{
  if (hasRole(BLACKLISTED_ROLE, _owner) || hasRole(BLACKLISTED_ROLE, receiver)) {
    revert OperationNotAllowed();
  }
  super._withdraw(caller, receiver, _owner, assets, shares);
  _checkMinShares();
}
```

**Key difference from V1's `_withdraw`:** StakedENA checks only `_owner` and `receiver` for `BLACKLISTED_ROLE` — **NOT `caller`**. V1 checks all three (`caller || receiver || _owner`).

**Is the missing `caller` check a vulnerability?** No — it is covered by `_beforeTokenTransfer`'s `msg.sender` check (lines 399–409), which fires during the `_burn(owner, shares)` inside `super._withdraw`. Since `caller` is always `msg.sender` in this contract's flows (`deposit`/`mint`/`withdraw`/`redeem`/`cooldownAssets`/`cooldownShares` all pass `msg.sender` as `caller`), the `_beforeTokenTransfer` check catches a blacklisted caller. ✅ Defense-in-depth is slightly weaker than V1 (two checks vs three), but functionally equivalent. Informational — see **E-3**.

**`_deposit` doesn't check blacklist at all** — but `_beforeTokenTransfer(address(0), receiver, amount)` fires during `_mint` and blocks if `msg.sender` or `receiver` is blacklisted. ✅ Covered.

### Lines 373–393: `_updateVestingAmount` / `_updateVestingPeriod`
```solidity
function _updateVestingAmount(uint256 newVestingAmount) internal {
  if (getUnvestedAmount() > 0) revert StillVesting();
  vestingAmount = newVestingAmount;
  lastDistributionTimestamp = block.timestamp;
}
function _updateVestingPeriod(uint24 newVestingPeriod) internal {
  if (getUnvestedAmount() > 0) revert StillVesting();
  if (newVestingPeriod > MAX_VESTING_PERIOD) revert InvalidVestingPeriod();
  emit VestingDurationUpdated(vestingPeriod, newVestingPeriod);
  vestingPeriod = newVestingPeriod;
}
```
- Both revert if vesting is in progress. ✅
- `_updateVestingAmount` does NOT carry over unvested remainder (despite the comment on `vestingAmount` line 44 saying "+ any unvested remainder at that time"). The code reverts instead. Same as V1. The comment is inherited/misleading. Informational.
- `_updateVestingPeriod` allows setting to 0 (instant vesting). Admin centralization, not a bug.

### Lines 395–409: `_beforeTokenTransfer`
```solidity
function _beforeTokenTransfer(address from, address to, uint256) internal virtual override {
  if (hasRole(BLACKLISTED_ROLE, msg.sender)) {
    revert OperationNotAllowed();
  }
  if (hasRole(BLACKLISTED_ROLE, from) && to != address(0)) {
    revert OperationNotAllowed();
  }
  if (hasRole(BLACKLISTED_ROLE, to)) {
    revert OperationNotAllowed();
  }
}
```

**Differences from V1:**
- V1 checks only `from` (with `to != address(0)` carve-out for burns) and `to`. StakedENA **adds a `msg.sender` check**. ✅ Strengthening — closes the "blacklisted approved-operator moves a non-blacklisted owner's tokens" gap (V1's M-01 was a related but different fix for `_withdraw`; this `_beforeTokenTransfer` check is broader and covers `_transfer` too).

**Behavior:**
- `msg.sender` blacklisted → revert (blocks blacklisted users from initiating ANY sENA movement, even on behalf of others). ✅
- `from` blacklisted AND `to != address(0)` → revert (blocks transfers FROM blacklisted, but allows burns to `address(0)` — needed for `redistributeLockedAmount`). ✅
- `to` blacklisted → revert (blocks transfers TO blacklisted, including mints). ✅

**Interaction with `redistributeLockedAmount`:** When admin calls it, `msg.sender = admin` (never blacklisted due to `notOwner`). `_burn(from, amount)` triggers `_beforeTokenTransfer(from, 0, amount)` — `from` is blacklisted but `to == 0`, so the `from` check passes. ✅ `_mint(to, amount)` triggers `_beforeTokenTransfer(0, to, amount)` — `to` not blacklisted (checked at line 260). ✅

---

## 3. StakedENA vs StakedUSDeV2 — Difference Matrix

| Aspect | StakedUSDe (V1) | StakedUSDeV2 | StakedENA | Notes |
|--------|-----------------|--------------|-----------|-------|
| Upgradeable | ❌ | ❌ | ✅ (UUPS-style, `_disableInitializers`) | New |
| Vesting period | Fixed 8h | (inherited 8h) | Configurable, max 90d, default 7d | New feature |
| Cooldown | ❌ | ✅ 90d default | ✅ 7d default | Different default |
| Blacklist tiers | Soft + Full | (inherited) | Single (`BLACKLISTED_ROLE`) | Simplified |
| `unstake` `nonReentrant` | n/a | ❌ | ✅ | Strengthening |
| `setCooldownDuration` `nonReentrant` | n/a | ❌ | ✅ | Strengthening |
| `addToBlacklist`/`removeFromBlacklist` `nonReentrant` | ❌ | (inherited ❌) | ✅ | Strengthening |
| `_beforeTokenTransfer` checks `msg.sender` | ❌ | (inherited ❌) | ✅ | Strengthening (covers M-01-style operator bypass for `_transfer`) |
| `_withdraw` checks `caller` | ✅ | (inherited ✅) | ❌ (covered by `_beforeTokenTransfer`) | Minor inconsistency |
| `useNonce()` (permit cancellation) | ❌ | ❌ | ✅ | New feature |
| `redistributeLockedAmount` zero-balance guard | ❌ | (inherited ❌) | ✅ | Cleaner |
| Initial deposit to dead address | ❌ | ❌ | ❌ (comment lies — see E-2) | Same |
| `unstake` checks blacklist | ❌ | ❌ | ❌ | **Same Medium gap (M-1)** |
| `silo` storage | n/a | `immutable` (constructor) | state var (initializer) | Required for upgradeable |
| `SOFT_RESTRICTED_STAKER_ROLE` deposit block | ✅ | (inherited ✅) | ❌ (removed) | StakedENA has only one tier; `_deposit` relies on `_beforeTokenTransfer` to block blacklisted `msg.sender`/`receiver` |

**Net security posture:** StakedENA is **strictly more defensive** than V2 on reentrancy and the `_beforeTokenTransfer` operator-bypass gap, and adds configurable vesting + permit-nonce cancellation. The one regression is the loss of the soft/full blacklist split (but the single-tier model is simpler and the deposit path is still protected via `_beforeTokenTransfer`). The persistent Medium (`unstake` blacklist bypass) is unchanged from V2.

---

## 4. Contract: `EnaSilo.sol` — Line-by-Line

```solidity
contract EnaSilo is IEnaSiloDefinitions {
  address immutable _STAKING_VAULT;
  IERC20 immutable _ENA;

  constructor(address stakingVault, address ena) {
    _STAKING_VAULT = stakingVault;
    _ENA = IERC20(ena);
  }

  modifier onlyStakingVault() {
    if (msg.sender != _STAKING_VAULT) revert OnlyStakingVault();
    _;
  }

  function withdraw(address to, uint256 amount) external onlyStakingVault {
    _ENA.transfer(to, amount);
  }
}
```

### Analysis
- **Trivial holding contract.** Stores ENA during the cooldown window. Only the staking vault (StakedENA proxy) can withdraw. ✅
- `_STAKING_VAULT` and `_ENA` are `immutable` — set in constructor, bound to the StakedENA proxy address (passed as `address(this)` from `initialize`). ✅ Cannot be changed. If the proxy is upgraded, the silo still points to the proxy. ✅
- `onlyStakingVault` modifier — no one else can call `withdraw`. ✅
- `withdraw` uses `_ENA.transfer(to, amount)` — **NOT `safeTransfer`**. If ENA were a non-standard ERC20 that returns `false` on failure (instead of reverting), a failed transfer would be silently ignored. **However, ENA is a standard OZ ERC20 that reverts on failure**, so this is safe in practice. Same pattern as V1's `USDeSilo`. Informational — see **E-1**.
- No `amount > 0` check — but `amount == 0` is a no-op transfer. StakedENA's `unstake` can call `silo.withdraw(receiver, 0)` if a user with no cooldown calls `unstake` (wasteful but harmless). ✅
- No `to != address(0)` check — but OZ ERC20's `transfer` reverts on `address(0)` recipient. ✅
- **No rescue function** — if ENA is accidentally sent directly to the silo (not via `cooldownAssets`), it's permanently stuck. Same as V1's `USDeSilo`. This is a feature (prevents admin theft of cooldown funds) but means dust donations are irrecoverable. Informational.
- **No reentrancy guard** — but the silo only does a plain `transfer` (no hooks on ENA). Even if ENA had a callback, the silo has no state to corrupt and no other functions to reenter. ✅
- **Cross-contract reentrancy with StakedENA:** `unstake` calls `silo.withdraw(receiver, assets)`. The silo calls `_ENA.transfer(receiver, assets)`. The receiver is an arbitrary address. If the receiver is a malicious contract with an `onTokenTransfer` hook (ERC777-style), it could reenter StakedENA. But: (1) ENA is a plain ERC20 with no hooks; (2) `unstake` is `nonReentrant`; (3) `unstake` zeroes state before the external call. So even with a hypothetical hook, a reentrant `unstake` would see `underlyingAmount == 0` and `cooldownEnd == 0`, leading to `silo.withdraw(receiver, 0)` (no-op). ✅ No cross-contract reentrancy.

### Silo balance invariant
Every `cooldownAssets(assets)` / `cooldownShares(shares)`:
- Transfers exactly `assets` ENA from StakedENA to the silo (via OZ `_withdraw`'s `safeTransfer` of the exact `assets` value; ENA is not fee-on-transfer).
- Credits exactly `assets` to `cooldowns[user].underlyingAmount`.

Every `unstake`:
- Debits exactly `assets = underlyingAmount` from `cooldowns[user].underlyingAmount`.
- Pulls exactly `assets` ENA from the silo.

Therefore `sum(cooldowns[u].underlyingAmount for all u) ≤ silo.balanceOf(ENA)` always, with equality unless someone donates ENA directly to the silo (in which case the silo balance is higher and the donation is stuck). **No cross-user theft is possible via the silo.** ✅

---

## 5. Findings

### M-1 — `unstake` skips `BLACKLISTED_ROLE` check (Medium — blacklist-evasion, NOT fund theft)

**Severity:** Medium (compliance/blacklist-evasion gap; does NOT enable theft of other users' funds).

**Location:** `StakedENA.sol:146–159` (`unstake`).

**Description:**
`unstake` does not consult `BLACKLISTED_ROLE` on `msg.sender` or `receiver`. The blacklist is enforced in:
- `_withdraw` (lines 365–367): blocks `BLACKLISTED_ROLE` on `_owner` and `receiver`. This gates `withdraw`, `redeem`, `cooldownAssets`, `cooldownShares`.
- `_beforeTokenTransfer` (lines 399–409): blocks blacklisted `msg.sender`, `from` (non-burn), and `to`.

But once a user has called `cooldownAssets`/`cooldownShares`, their sENA is already burned (via `_withdraw`) and their ENA sits in `EnaSilo`. At that point:
- `balanceOf(user) == 0` → `redistributeLockedAmount` reverts with `InvalidAmount` (line 262). The admin's confiscation tool **cannot reach silo'd funds**.
- `unstake` is the only way to claim the silo'd ENA, and it skips the blacklist check.

**Attack scenario:**
1. User U is not blacklisted. U stakes 1000 ENA, receiving sENA.
2. U calls `cooldownAssets(1000 ENA worth)`. sENA burned, 1000 ENA moved to silo. `cooldowns[U].cooldownEnd = now + 7 days`, `underlyingAmount = 1000 ENA`.
3. U is blacklisted (added to `BLACKLISTED_ROLE`).
4. After 7 days, U calls `unstake(freshWallet)`. `block.timestamp >= cooldownEnd` → passes. `silo.withdraw(freshWallet, 1000 ENA)`. U's fresh wallet receives 1000 ENA.
5. The admin's `redistributeLockedAmount(U, ...)` would revert because `balanceOf(U) == 0`.

**Impact:**
- A user flagged for sanctions can "pre-flight" a cooldown and escape with their **own legitimately-staked** ENA 7–90 days later, defeating the compliance freeze.
- **Does NOT enable theft of other users' funds.** The silo balance invariant (Section 4) guarantees each user can only claim their own `underlyingAmount`.
- Setting `cooldownDuration = 0` does NOT close the gap — `unstake` remains callable (the `|| cooldownDuration == 0` branch at line 150) and still skips the blacklist check.

**Remediation:**
- Add `if (hasRole(BLACKLISTED_ROLE, msg.sender) || hasRole(BLACKLISTED_ROLE, receiver)) revert OperationNotAllowed();` to `unstake`.
- AND/OR add an admin function to sweep a blacklisted user's silo cooldown (e.g., `redistributeSiloCooldown(from, to)` that zeroes `cooldowns[from].underlyingAmount` and vests/transfers the silo'd ENA).

**Status:** This is the StakedENA analogue of finding V2-1 in `ethena-usde-deep-analysis.md` (StakedUSDeV2.unstake). The V2 version has been live and Code4rena-audited since Oct 2023 (the Code4rena M-02 finding was about a *different* issue — soft-restricted withdraw via `_withdraw`, not `unstake`). The `unstake` blacklist gap does not appear in the public audit findings, suggesting it was either (a) considered acceptable by design (the comment at lines 141–143 says *"No attempt is made to restrict blacklisted addresses from claiming their assets at this point as the assets have already been converted to ENA and ENA is a permissionless token"*) or (b) a known, unfixed gap. **Likely a duplicate / known issue — re-reporting to Immunefi would risk being closed as "acknowledged" or "out of scope (compliance, not fund theft)."**

---

### L-1 — `redistributeLockedAmount(from, address(0))` can brick ENA if `from` is the only staker (Low — admin error, recoverable)

**Severity:** Low (requires admin to choose `address(0)` while the blacklisted user is the sole staker; recoverable via a deposit-redeem sweep).

**Location:** `StakedENA.sol:259–273` (`redistributeLockedAmount`).

**Description:**
If the blacklisted user `from` is the **only** staker:
- `balanceOf(from) = totalSupply`.
- `enaToVest = previewRedeem(totalSupply) = totalAssets` (since `from` is the only staker).
- `_burn(from, totalSupply)` → `totalSupply = 0`.
- `_updateVestingAmount(enaToVest = totalAssets)` → `vestingAmount = totalAssets`, `lastDistributionTimestamp = now`.
- Immediately after: `getUnvestedAmount() = totalAssets`, so `totalAssets() = balance - totalAssets = 0` (since `balance == totalAssets` before the burn).
- As vesting progresses, `totalAssets()` rises from 0 back to `totalAssets`, but `totalSupply == 0`, so **no one has shares to redeem**. The vested ENA sits in the contract with no claimant.

**Recovery path:**
A new depositor (e.g., the admin) can deposit `X` ENA and receive shares proportional to the virtual offset:
- `shares = X * (0 + 1) / (totalAssets + 1) ≈ X / totalAssets` (tiny if `totalAssets` is large).
- To get ≥1 share, the depositor needs `X > totalAssets`. They deposit `totalAssets + 1` wei, get 1 share, then redeem it for `~(totalAssets + balance)` ENA — sweeping the stuck funds.

This requires the admin to come up with `~totalAssets` ENA temporarily (they get it back plus the stuck amount). Not a permanent lockup, but an admin-error path that bricked ENA can be recovered from.

**Impact:** Low. Requires admin misjudgment (choosing `address(0)` instead of a real treasury address when `from` is the sole staker). The admin should use `redistributeLockedAmount(from, treasury)` (mint path) when `from` is the only staker, or wait for other depositors. Same behavior as V1's `redistributeLockedAmount`. Not exploitable by an external attacker.

**Remediation:** Add a guard: if `to == address(0) && totalSupply - amountToDistribute == 0`, revert or require a non-zero `to`. Or document the recovery path.

---

### E-1 — `EnaSilo.withdraw` uses `transfer` not `safeTransfer` (Informational)

**Location:** `EnASilo.sol:26`.

`_ENA.transfer(to, amount)` does not check the return value. If ENA were a non-standard ERC20 (returning `false` on failure instead of reverting), a failed transfer would be silently ignored. **ENA is a standard OZ ERC20** that reverts on failure, so this is safe in practice. Same pattern as V1's `USDeSilo`. Defensive coding would use `SafeERC20.safeTransfer`. No fund impact.

---

### E-2 — `_checkMinShares` comment references a non-existent "initial deposit to the dead address" (Informational)

**Location:** `StakedENA.sol:323–326`.

```solidity
/// @notice ensures a small non-zero amount of shares does not remain, exposing to donation attack
/// This should never happen due to the initial deposit to the dead address
function _checkMinShares() internal view {
  uint256 _totalSupply = totalSupply();
  if (_totalSupply > 0 && _totalSupply < _MIN_SHARES) revert MinSharesViolation();
}
```

The comment claims an initial deposit to a dead address prevents the donation attack. **No such deposit exists in `initialize`.** The actual mitigation is the `_checkMinShares` floor itself (first depositor must mint ≥1e18 shares, i.e., deposit ≥1 ENA at 1:1) plus OZ v4's virtual shares/assets offset (`+1` in numerator and denominator of `_convertToShares`/`_convertToAssets`).

**Donation attack analysis (mental simulation):**
- Attacker deposits 1 ENA → 1e18 shares. `totalSupply = 1e18`, `totalAssets = 1e18`.
- Attacker donates `D` ENA directly. `totalAssets = 1e18 + D`, `totalSupply = 1e18`.
- Victim deposits `V` ENA → `shares = V * (1e18 + 1) / (1e18 + D + 1) ≈ V * 1e18 / (1e18 + D)`.
- After: `totalSupply = 1e18 + victim_shares`, `totalAssets = 1e18 + D + V`.
- Attacker's 1e18 shares worth: `1e18 * (1e18 + D + V) / (1e18 + victim_shares)`.
- Victim's shares worth: `victim_shares * (1e18 + D + V) / (1e18 + victim_shares)`.

With the virtual offset, the victim gets `≈ V * 1e18 / (1e18 + D)` shares, each worth `≈ (1e18 + D + V) / (1e18 + victim_shares)` ENA. The product is `≈ V` ENA — the victim gets approximately what they deposited. The attacker's gain is only rounding dust (a few wei), and the attacker had to lock 1 ENA + donate `D` ENA to get it. Not profitable. ✅

The Code4rena M-04 finding (acknowledged by Ethena, deployment-time-only) is the same issue. On the live StakedENA contract with non-trivial TVL, this is not exploitable. **Not a bounty target on the live contract.**

---

### E-3 — `_withdraw` omits `caller` blacklist check (Informational — covered by `_beforeTokenTransfer`)

**Location:** `StakedENA.sol:365–367`.

StakedENA's `_withdraw` checks `BLACKLISTED_ROLE` on `_owner` and `receiver` only. V1's `_withdraw` also checks `caller`. The `caller` check is **covered** by `_beforeTokenTransfer`'s `msg.sender` check (line 400), which fires during the `_burn(owner, shares)` inside `super._withdraw`. Since `caller` is always `msg.sender` in this contract's flows, a blacklisted caller is caught. ✅ Functionally equivalent to V1, just structured differently. Minor inconsistency — defense-in-depth would add the `caller` check to `_withdraw` too.

---

### E-4 — `SingleAdminAccessControlUpgradeable` lacks its own `__gap` (Informational — acknowledged, cosmetic)

**Location:** `SingleAdminAccessControlUpgradeable.sol:13–81`.

The contract adds `_currentDefaultAdmin` and `_pendingDefaultAdmin` (two storage slots) after `AccessControlUpgradeable`'s storage (which ends with a `__gap`). It does **not** declare its own `__gap` buffer. This is the same issue flagged by Cyfrin (L-1) and Quantstamp (USTB-2) for the USDtb variant. It means a future version of `SingleAdminAccessControlUpgradeable` cannot easily add new state variables without risking collision with descendants. For the **current** deployment, the storage layout is fixed and correct — this is a hygiene issue, not an exploitable bug.

---

### E-5 — Dust shares can be stuck for partial exits (Informational)

**Location:** `StakedENA.sol:163–187` (`cooldownAssets`/`cooldownShares`) + `_checkMinShares`.

Due to rounding (`previewWithdraw` rounds UP, `previewRedeem` rounds DOWN), a user who calls `cooldownAssets(maxWithdraw(...))` to exit their full position may be left with a few wei of dust shares (the OZ invariant `previewWithdraw(previewRedeem(balanceOf)) ≤ balanceOf` is satisfied, but not always with equality). If `previewRedeem(dust) == 0` (rounds down), the user cannot `cooldownShares(dust)` (reverts on `notZero(assets)`), and cannot `cooldownAssets(1)` if `maxWithdraw == 0` (reverts on `ExcessiveWithdrawAmount`). The dust is stuck.

**Impact:** A few wei of shares per user — negligible value. Does not brick the vault (other users can still operate; `totalSupply` remains ≥1e18 as long as any staker has ≥1e18 shares). Same as V2. Not a vulnerability.

---

### E-6 — `redistributeLockedAmount` does not call `_checkMinShares` (Informational)

**Location:** `StakedENA.sol:259–273`.

`_deposit`/`_withdraw` call `_checkMinShares` after the super call, but `redistributeLockedAmount` uses `_burn`/`_mint` directly and does not call `_checkMinShares`. In theory, if the admin burns a blacklisted user's shares via the `to == address(0)` path and the remaining `totalSupply` lands in `(0, 1e18)`, subsequent deposits/withdrawals would revert on `_checkMinShares` until `totalSupply` reaches 0 or ≥1e18.

**Practical impact:** Minimal. For `totalSupply` to land in `(0, 1e18)` after a redistribute-to-burn, there must be other stakers with combined shares in that range. But `_checkMinShares` on prior deposits/withdrawals enforces that every staker has either 0 or enough shares such that `totalSupply` is 0 or ≥1e18 — EXCEPT that individual stakers can have <1e18 shares as long as `totalSupply ≥1e18`. So a scenario like: Staker A has 1.5e18, Staker B has 0.5e18, `totalSupply = 2e18`. Admin blacklists A and redistributes-to-burn: `totalSupply = 0.5e18` (in the danger zone). Now B cannot deposit (would push `totalSupply` to 1e18 only if depositing ≥0.5e18 worth; smaller deposits revert) and cannot withdraw partially (would leave dust). B CAN withdraw all 0.5e18 (leaving `totalSupply = 0`, which passes). So the vault is not bricked, just temporarily restricted on small deposits. Same as V1. Informational.

---

## 6. Attack Vectors Tested (Mental Simulation Results)

| # | Vector | Result | Notes |
|---|--------|--------|-------|
| 1 | ERC4626 first-deposit inflation attack | ❌ Not exploitable | `_checkMinShares` forces ≥1e18 shares; OZ v4 virtual offset caps attacker gain to rounding dust. Attacker must lock ≥1 ENA. |
| 2 | Donation attack on live vault | ❌ Not exploitable | Live TVL >> 1 ENA; donation is a rounding-dust rounding event. |
| 3 | Reentrancy via `unstake` → `silo.withdraw` → ENA transfer hook | ❌ Not exploitable | `nonReentrant` on `unstake`; CEI (state zeroed before external call); ENA has no hooks. Reentrant `unstake` would withdraw 0. |
| 4 | Reentrancy via `cooldownAssets` → `_withdraw` → `safeTransfer` hook | ❌ Not exploitable | `_withdraw` is `nonReentrant`; reentrant `cooldownAssets` would update `cooldownEnd`/`underlyingAmount` then revert at `_withdraw`. Entire tx reverts. |
| 5 | Cross-contract reentrancy StakedENA ↔ EnaSilo | ❌ Not exploitable | Silo has only `withdraw` (plain transfer, no callback to StakedENA). |
| 6 | Blacklist bypass via `unstake` after pre-cooldown | ⚠️ Medium (M-1) | Confirmed — user who cooldowned before blacklisting can claim silo'd ENA. Compliance gap only. |
| 7 | Blacklist bypass via approved operator | ❌ Not exploitable | `_beforeTokenTransfer` checks `msg.sender`; `_withdraw` checks `_owner`/`receiver`. |
| 8 | Blacklist bypass via `transfer` to fresh wallet then `unstake` | ❌ Not exploitable | `unstake` operates on `cooldowns[msg.sender]`; transferring sENA doesn't move the cooldown. And sENA is already burned before cooldown. |
| 9 | Cooldown bypass (unstake before `cooldownEnd`) | ❌ Not exploitable | `unstake` checks `block.timestamp >= cooldownEnd \|\| cooldownDuration == 0`. |
| 10 | Cooldown bypass via `withdraw`/`redeem` when cooldown is on | ❌ Not exploitable | `ensureCooldownOff` reverts. |
| 11 | Admin steals ENA from vault | ❌ Not exploitable | `rescueTokens` blocks ENA; `redistributeLockedAmount` only operates on blacklisted users' sENA. |
| 12 | Admin steals ENA from silo | ❌ Not exploitable | Silo only callable by StakedENA proxy; `unstake` is per-user; no admin sweep of silo. |
| 13 | Rewarder drains vault via `transferInRewards` | ❌ Not exploitable | `safeTransferFrom` pulls FROM rewarder TO vault (adds funds, doesn't remove). |
| 14 | `totalAssets()` underflow | ❌ Not exploitable | `vestingAmount` always ≤ `balanceOf(this)` after `transferInRewards`; withdrawals capped by `totalAssets()`. |
| 15 | `vestingPeriod = 0` division-by-zero | ❌ Not exploitable | `getUnvestedAmount` returns early when `timeSinceLastDistribution >= vestingPeriod` (0 >= 0 true). |
| 16 | Type-cast overflow in `cooldownEnd`/`underlyingAmount` | ❌ Not exploitable | `uint104`/`uint152` max >> realistic values; Solidity 0.8 reverts on arithmetic overflow. |
| 17 | `redistributeLockedAmount` to `address(0)` bricks sole-staker ENA | ⚠️ Low (L-1) | Confirmed — recoverable via deposit-redeem sweep. Admin error. |
| 18 | Upgradeable storage collision | ❌ Not exploitable (current) | OZ `__gap` pattern used; `SingleAdminAccessControlUpgradeable` lacks its own gap (E-4) but layout is fixed post-deploy. |
| 19 | Initialize front-run on proxy deployment | ❌ Deployment concern | Standard upgradeable risk; mitigated by deploy+initialize in one tx. |
| 20 | `useNonce` griefing | ❌ Not exploitable | Each user only increments their own nonce; no cross-user impact. |

---

## 7. Silo Balance Invariant (Verified)

**Claim:** `sum(cooldowns[u].underlyingAmount for all u) ≤ EnaSilo.balanceOf(ENA)`, with equality unless someone donates ENA directly to the silo.

**Proof:**
- `cooldownAssets(assets)`: `underlyingAmount += assets` AND `_withdraw(... assets ...)` transfers exactly `assets` ENA to the silo. Both sides increase by `assets`. ✅
- `cooldownShares(shares)`: `underlyingAmount += assets` (where `assets = previewRedeem(shares)`, rounded down) AND `_withdraw(... assets ...)` transfers exactly `assets` ENA to the silo. Both sides increase by `assets`. ✅
- `unstake`: `underlyingAmount = 0` (zeroed, not decremented by `assets` — but `assets` was read from `underlyingAmount` first) AND `silo.withdraw(receiver, assets)` transfers exactly `assets` ENA out. Both sides decrease by `assets`. ✅

No drift. No cross-user theft via the silo. The only way the silo balance exceeds the sum is a direct ENA transfer to the silo (donation), which is irrecoverable but harmless to stakers.

---

## 8. Summary Table

| ID | Contract | Finding | Severity | New? |
|----|----------|---------|----------|------|
| M-1 | StakedENA | `unstake` skips `BLACKLISTED_ROLE` — blacklisted user can claim pre-cooldowned silo ENA | **Medium** | No — analogue of V2-1 (StakedUSDeV2). Compliance gap, not fund theft. |
| L-1 | StakedENA | `redistributeLockedAmount(from, 0)` can brick ENA if `from` is sole staker (recoverable) | **Low** | Same as V1 behavior |
| E-1 | EnaSilo | `transfer` not `safeTransfer` (safe for standard ENA) | Informational | Same as V1 USDeSilo |
| E-2 | StakedENA | `_checkMinShares` comment references non-existent dead-address deposit | Informational | Same as V1 (comment inherited) |
| E-3 | StakedENA | `_withdraw` omits `caller` blacklist check (covered by `_beforeTokenTransfer`) | Informational | Minor regression from V1, functionally equivalent |
| E-4 | SingleAdminAccessControlUpgradeable | No `__gap` (Cyfrin L-1 / Quantstamp USTB-2) | Informational | Acknowledged in USDtb audits |
| E-5 | StakedENA | Dust shares can be stuck on partial exit | Informational | Same as V2 |
| E-6 | StakedENA | `redistributeLockedAmount` doesn't call `_checkMinShares` | Informational | Same as V1 |

---

## 9. Conclusion

**Critical bugs: 0.**
**Medium bugs: 1 (M-1 — known/duplicate of V2-1, compliance gap only, not fund theft).**
**Low bugs: 1 (L-1 — admin error, recoverable).**
**Informational: 6.**

StakedENA is a well-executed upgradeable port of the StakedUSDeV2 + StakedUSDe lineage. It is **strictly more defensive** than V2 on reentrancy hygiene (`nonReentrant` added to `unstake`, `setCooldownDuration`, `addToBlacklist`/`removeFromBlacklist`) and on the operator-bypass gap (`_beforeTokenTransfer` now checks `msg.sender`). The configurable vesting period and `useNonce` permit-cancellation are clean additions.

The single Medium finding (M-1) is the same compliance gap already present in `StakedUSDeV2.unstake` since Oct 2023 — a blacklisted user who pre-flights a cooldown can claim their own silo'd ENA after the cooldown, bypassing the freeze. It does **not** enable theft of other users' funds (the silo balance invariant holds). The comment at lines 141–143 explicitly states this is intentional ("No attempt is made to restrict blacklisted addresses from claiming their assets at this point as the assets have already been converted to ENA and ENA is a permissionless token"). Re-reporting this to Immunefi would likely be closed as acknowledged or out-of-scope (compliance, not fund theft).

No vulnerability in this set meets the "theft of funds / permanent freezing of protocol funds by an external attacker" bar for a Critical/High Immunefi payout. No separate vuln PoC file was written.

---

## 10. Files Read

- `/home/z/fkr-step1/defi-bounty/contracts/StakedENA.sol` (410 lines) — target
- `/home/z/fkr-step1/defi-bounty/contracts/ENASilo.sol` (28 lines) — target
- `/home/z/fkr-step1/defi-bounty/contracts/SingleAdminAccessControlUpgradeable.sol` (81 lines) — dependency
- `/home/z/ethena-usde/contracts/contracts/StakedUSDe.sol` (268 lines) — comparison baseline V1
- `/home/z/ethena-usde/contracts/contracts/StakedUSDeV2.sol` (131 lines) — comparison baseline V2
- `/home/z/ethena-usde/contracts/contracts/USDeSilo.sol` (30 lines) — comparison baseline silo
- `/home/z/ethena-usde/contracts/contracts/interfaces/IStakedUSDe.sol` — interface reference
- `/home/z/ethena-usde/contracts/contracts/interfaces/IStakedUSDeCooldown.sol` — interface reference (UserCooldown struct)
- `/home/z/ethena-usde/contracts/contracts/interfaces/IUSDeSiloDefinitions.sol` — interface reference
- `/home/z/fkr-step1/defi-bounty/protocol-research/ethena-usde-deep-analysis.md` — prior V2-1 finding
- `/home/z/fkr-step1/defi-bounty/protocol-research/ethena-audit-history.md` — audit context
- `/home/z/fkr-step1/defi-bounty/protocol-research/source-fetch-status.md` — deployment context

## 11. Next Actions

1. **Do NOT submit M-1 to Immunefi** — it is a known/intentional compliance gap (documented in the contract comments) and the V2 analogue has been live since Oct 2023. Risk of duplicate rejection.
2. If pursuing further: examine the **PSM contract** (`/home/z/fkr-step1/defi-bounty/contracts/PSM.sol`, 2082 lines) — the largest unaudited surface in the current scope.
3. If the `RateLimiter` / `OFTOwnable2Step` libs become available (via Etherscan API), analyze the OFT contracts' `_debit` rate-limiting path.
4. Verify the deployed StakedENA proxy's `initialize` was called atomically with deployment (Etherscan tx history) to rule out initialize front-run.
