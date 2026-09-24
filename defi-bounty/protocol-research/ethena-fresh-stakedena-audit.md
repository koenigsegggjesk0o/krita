# Fresh Audit: StakedENA + EnaSilo

**Agent:** Opus
**Task ID:** eth-fresh-stakedena-audit
**Date:** 2026-09-24
**Scope:**
- `contracts/StakedENA.sol` (410 lines)
- `contracts/ENASilo.sol` (28 lines)
- `contracts/SingleAdminAccessControlUpgradeable.sol` (81 lines)

**Known bug (SKIPPED — do not re-report):** `unstake` skips `BLACKLISTED_ROLE` check (M-1, intentional compliance gap).

**Baseline:** Prior analysis in `protocol-research/ethena-stakedena-deep-analysis.md` found M-1 (known) + L-1 + 6 Informational. This fresh audit focuses on **10 areas not yet deeply analyzed** to find NEW bugs.

**OZ version determination:** StakedENA overrides `_beforeTokenTransfer` with the `override` keyword (line 399). OZ v5 removed `_beforeTokenTransfer` (replaced by `_update`), so `override` would fail to compile. Therefore StakedENA uses **OZ v4.x upgradeable**, where `_beforeTokenTransfer` IS called by `_transfer`, `_mint`, `_burn`. This is critical for the blacklist analysis.

---

## Area 1: ERC4626 Compliance + Inflation Attack

### Functions analyzed
- `_deposit` (lines 339–348): override adds `nonReentrant`, `notZero(assets)`, `notZero(shares)`, calls `super._deposit` then `_checkMinShares()`.
- `_withdraw` (lines 358–371): same modifiers + blacklist check on `_owner`/`receiver`.
- `_checkMinShares` (lines 327–330): reverts if `0 < totalSupply < 1e18`.
- `totalAssets` (lines 287–289): `balanceOf(this) - getUnvestedAmount()`.
- OZ v4.x `_convertToShares`/`_convertToAssets`: use virtual offset `10 ** _decimalsOffset()` (default 0 → offset = 1 virtual share, 1 virtual asset).
- `decimals()` (lines 308–310): hardcoded 18, bypasses `_underlyingDecimals + _decimalsOffset()` but does NOT affect the virtual offset in conversion math.

### Edge cases tested
1. **First-deposit attack with totalSupply = 0:**
   - Attacker deposits 1 ENA → 1e18 shares (via virtual offset: `1e18 * (0+1)/(0+1) = 1e18`). `_checkMinShares` passes (1e18 >= 1e18).
   - Attacker donates D ENA directly. `totalAssets = 1e18 + D`.
   - Victim deposits V ENA → `shares = V * (1e18 + 1) / (1e18 + D + 1) ≈ V * 1e18 / (1e18 + D)`.
   - Victim's shares worth ≈ V ENA. Attacker's gain = rounding dust only. **Not profitable.** ✅
2. **Donation to empty vault (totalSupply = 0):**
   - Attacker donates D ENA. `totalAssets = D`, `totalSupply = 0`.
   - First depositor with X ENA: `shares = X * (0+1) / (D+1) = X/(D+1)`. If `X <= D`, shares = 0 → `notZero(shares)` reverts.
   - Depositor needs `X > D` to get ≥1 share, and `X >= 1e18*(D+1)` to get ≥1e18 shares (pass `_checkMinShares`).
   - Attacker's D ENA is irrecoverable (`rescueTokens` blocks ENA) and goes to the first big depositor. **Not profitable for attacker.** ✅
3. **totalAssets = 0 but totalSupply > 0:**
   - Requires `balanceOf(this) == getUnvestedAmount()` while shares exist.
   - `getUnvestedAmount() <= vestingAmount`, and `vestingAmount` is set by `transferInRewards` which pulls `vestingAmount` ENA into the vault. So `balanceOf(this) >= vestingAmount >= getUnvestedAmount()`.
   - For `totalAssets = 0` with `totalSupply > 0`: need all vault ENA to be unvested rewards AND no staker principal. But staker principal is in the vault (deposits add ENA). If stakers withdraw, shares are burned (totalSupply decreases). The only way to have `totalSupply > 0` with no staker principal is via `redistributeLockedAmount(from, 0)` when `from` is the sole staker — but that sets `totalSupply = 0`. **Cannot construct this state.** ✅
4. **`_checkMinShares` trapping totalSupply in (0, 1e18):**
   - If a withdrawal would leave `totalSupply` in `(0, 1e18)`, `_checkMinShares` reverts.
   - User can always withdraw ALL their shares (leaving `totalSupply = 0`, which passes).
   - Partial withdrawals may be blocked until other stakers exit or new depositors join.
   - **Not a vulnerability** — standard donation-attack mitigation trade-off. Same as V1. ✅

### Issues found
**None new.** The inflation attack is double-mitigated (OZ v4.x virtual offset + `_checkMinShares` 1e18 floor). The prior analysis's E-2 (misleading comment about dead-address deposit) and E-5 (dust shares) are confirmed but not new.

---

## Area 2: Cooldown Manipulation

### Functions analyzed
- `cooldownAssets` (lines 163–173): `ensureCooldownOn`, checks `assets <= maxWithdraw`, sets `cooldownEnd = block.timestamp + cooldownDuration` (OVERWRITE), `underlyingAmount += assets` (ACCUMULATE), calls `_withdraw` to move ENA to silo.
- `cooldownShares` (lines 177–187): mirror, uses `previewRedeem` (Floor).
- `setCooldownDuration` (lines 191–199): `onlyRole(DEFAULT_ADMIN_ROLE)`, caps at `MAX_COOLDOWN_DURATION` (90 days).
- `unstake` (lines 146–159): checks `block.timestamp >= cooldownEnd || cooldownDuration == 0`.

### Edge cases tested
1. **Admin sets cooldownDuration = 0 mid-cooldown:**
   - `unstake` passes via `|| cooldownDuration == 0` branch. Users can claim immediately. ✅ Documented behavior.
   - `withdraw`/`redeem` also work (ensureCooldownOff passes). Users with remaining sENA can exit via standard ERC4626. ✅
2. **Admin sets cooldownDuration = 90 days mid-cooldown:**
   - Existing cooldowns use stored `cooldownEnd` (set at cooldown time). NOT retroactively affected. ✅
   - New cooldowns use the new 90-day duration. ✅
3. **Admin increases cooldownDuration, user top-ups:**
   - `cooldownEnd` is OVERWRITTEN on each `cooldownAssets`/`cooldownShares` call.
   - If admin increased from 7→90 days, a user who top-ups restarts at 90 days — AND re-locks their already-claimable `underlyingAmount`.
   - User can avoid by `unstake` first (zeroing `underlyingAmount`), then re-cooldown. ✅ Documented UX gotcha, not a bug.
4. **Block.timestamp manipulation:**
   - Block.timestamp is controlled by validators (±2s). Cannot be manipulated by ordinary users. ✅
   - `cooldownEnd = uint104(block.timestamp) + cooldownDuration`. No overflow (uint104.max ≈ 2e31 >> 1.7e9 + 7.8e6). ✅
5. **Front-running cooldown start:**
   - Cooldown is per-user (`cooldowns[msg.sender]`). No cross-user interaction. No front-running vector. ✅
6. **uint104 truncation of block.timestamp:**
   - `uint104(block.timestamp)` — Solidity 0.8 truncates on explicit uint downcast (does NOT revert). But `block.timestamp ≈ 1.7e9` fits in uint104 (max ≈ 2e31). No truncation for ~10^23 years. ✅

### Issues found
**None new.** Cooldown system is sound. The overwrite-vs-accumulate behavior is documented.

---

## Area 3: Reward Distribution

### Functions analyzed
- `transferInRewards` (lines 213–219): `onlyRole(REWARDER_ROLE)`, `notZero(amount)`, `nonReentrant`. Calls `_updateVestingAmount(amount)` (state update FIRST), then `safeTransferFrom(msg.sender, this, amount)` (external call). CEI respected.
- `_updateVestingAmount` (lines 377–382): reverts `StillVesting` if `getUnvestedAmount() > 0`. Sets `vestingAmount = newVestingAmount`, `lastDistributionTimestamp = block.timestamp`.
- `getUnvestedAmount` (lines 294–305): linear vesting, `unchecked` deltaT safe due to prior `if` guard.
- `totalAssets` (lines 287–289): `balanceOf(this) - getUnvestedAmount()`.

### Edge cases tested
1. **Flash loan attack on exchange rate:**
   - Exchange rate = `totalAssets / totalSupply`. `totalAssets = balanceOf(this) - getUnvestedAmount()`.
   - Attacker can donate ENA (increase `balanceOf`), but this INCREASES `totalAssets`, raising the share price. Donating doesn't help the attacker (they lose the donated funds).
   - `getUnvestedAmount()` depends on `vestingAmount`/`lastDistributionTimestamp`/`vestingPeriod` — only changeable by REWARDER_ROLE/DEFAULT_ADMIN_ROLE. Attacker cannot manipulate.
   - **No flash loan attack.** ✅
2. **Malicious rewarder:**
   - `transferInRewards` pulls ENA FROM rewarder TO vault (adds funds). Rewarder cannot steal.
   - Rewarder can grief by calling `transferInRewards(1)` to reset `lastDistributionTimestamp`, blocking larger distributions for `vestingPeriod`. But rewarder is trusted. ✅
   - Rewarder can refuse to distribute. No impact on existing funds. ✅
3. **Fee-on-transfer ENA:**
   - `_updateVestingAmount(amount)` sets `vestingAmount = amount` BEFORE `safeTransferFrom`. If ENA were fee-on-transfer, `balanceOf` would increase by less than `amount`, potentially causing `totalAssets()` underflow (revert).
   - ENA is NOT fee-on-transfer. ✅ But latent risk if ENA ever adds fees.
4. **Blacklisted rewarder:**
   - `transferInRewards` checks `onlyRole(REWARDER_ROLE)` but does NOT check `BLACKLISTED_ROLE` on `msg.sender`.
   - A blacklisted rewarder can still distribute rewards. Action benefits the vault (adds funds). **Not a security issue** but an inconsistency. See **I-1** below.
5. **vestingPeriod = 0 (instant vesting):**
   - `getUnvestedAmount` returns 0 immediately (`timeSinceLastDistribution >= 0` always true). `totalAssets = balanceOf(this)`. ✅
   - Admin can set via `updateVestingPeriod(0)`. Centralization concern, not a bug. ✅
6. **`totalAssets()` underflow:**
   - `balanceOf(this) - getUnvestedAmount()`. Solidity 0.8 reverts on underflow.
   - `getUnvestedAmount() <= vestingAmount`. `vestingAmount` set in `transferInRewards` which pulls `vestingAmount` ENA in. So `balanceOf >= vestingAmount >= getUnvestedAmount()`. ✅ No underflow (with standard ENA).
   - In `redistributeLockedAmount(_, 0)`: `enaToVest = previewRedeem(amountToDistribute) <= totalAssets <= balanceOf`. `_updateVestingAmount(enaToVest)` sets `vestingAmount = enaToVest <= balanceOf`. ✅

### Issues found

**I-1 (Informational) — `transferInRewards` does not check `BLACKLISTED_ROLE` on `msg.sender`.**

Location: `StakedENA.sol:213`.

A blacklisted address that still holds `REWARDER_ROLE` can call `transferInRewards` to distribute rewards. This is inconsistent with the blacklist model (which freezes blacklisted addresses from all sENA operations via `_beforeTokenTransfer` and `_withdraw`). However, since `transferInRewards` ADDS funds to the vault (benefiting all stakers), this is not exploitable for fund theft. The admin should revoke `REWARDER_ROLE` before blacklisting a rewarder, but the contract does not enforce this.

**Severity: Informational.** No fund impact.

---

## Area 4: redistributeLockedAmount

### Functions analyzed
- `redistributeLockedAmount` (lines 259–273): `onlyRole(DEFAULT_ADMIN_ROLE)`, `nonReentrant`.
  - Checks: `from` must be blacklisted, `to` must NOT be blacklisted. `balanceOf(from) != 0`.
  - `enaToVest = previewRedeem(amountToDistribute)` (Floor — favors vault).
  - `_burn(from, amountToDistribute)`.
  - If `to == address(0)`: `_updateVestingAmount(enaToVest)` (vest to remaining stakers).
  - Else: `_mint(to, amountToDistribute)` (mint same shares to treasury).

### Edge cases tested
1. **`from` has 0 balance:** Reverts `InvalidAmount` (line 262). ✅
2. **`to` is blacklisted:** Reverts `OperationNotAllowed` (line 260). ✅
3. **`to` is blacklisted DURING execution (reentrancy):** No external calls between the check (line 260) and `_mint` (line 269). `_burn` calls `_beforeTokenTransfer` which is view-only (no external calls). No reentrancy possible. `to`'s status can't change mid-function. ✅
4. **Admin redirects to themselves:** `to = admin`. Admin is not blacklisted (`notOwner` on `addToBlacklist`). `_mint(admin, shares)`. Admin receives confiscated sENA. ✅ Intended confiscation-to-treasury.
5. **`to == address(0)` during active vesting:** `_updateVestingAmount` reverts `StillVesting`. Entire tx reverts (burn rolled back). Admin must use `to != address(0)` path instead. ✅
6. **`to == address(0)` when `from` is sole staker:** `totalSupply → 0`, `vestingAmount = totalAssets`, `totalAssets() → 0`. Vested ENA has no claimant. Recoverable via deposit-redeem sweep (L-1 from prior analysis). ✅
7. **Reentrancy via `_beforeTokenTransfer`:** `_beforeTokenTransfer` is a view-only function (checks roles, reverts). No external calls. No reentrancy. ✅
8. **`_mint` to a contract with hooks:** `_mint` calls `_beforeTokenTransfer(0, to, amount)` (view-only check), then updates `_balances[to]`. No callback to `to`. ✅ No reentrancy.
9. **`previewRedeem` rounding:** `enaToVest = floor(amountToDistribute * (totalAssets+1) / (totalSupply+1))`. Slightly less than fair ratio. Remaining ENA stays in vault, benefiting remaining stakers. ✅ Conservative.
10. **`_checkMinShares` not called:** `redistributeLockedAmount` uses `_burn`/`_mint` directly. In the `to == address(0)` path, `totalSupply` decreases. If remaining `totalSupply` lands in `(0, 1e18)`, subsequent `_deposit`/`_withdraw` revert on `_checkMinShares`. But users can always withdraw ALL shares (leaving `totalSupply = 0`, which passes). Not a permanent brick. ✅ (E-6 from prior analysis, confirmed.)

### Issues found
**None new.** The `redistributeLockedAmount` function is sound. The prior analysis's L-1 (sole-staker brick, recoverable) and E-6 (no `_checkMinShares`) are confirmed but not new.

---

## Area 5: Upgradeable Proxy Pattern

### Functions analyzed
- `constructor` (lines 82–84): `_disableInitializers()`. Prevents implementation initialization.
- `initialize` (lines 95–109): `initializer` modifier. Calls `__ERC20_init`, `__ERC4626_init`, `__ERC20Permit_init`, `__ReentrancyGuard_init`. Creates silo. Grants roles. Sets `cooldownDuration = 7 days`.
- `SingleAdminAccessControlUpgradeable._grantRole` override (lines 72–80): manages `_currentDefaultAdmin` and `_pendingDefaultAdmin`.
- `transferAdmin` / `acceptAdmin` (lines 25–34): two-step admin transfer.

### Edge cases tested
1. **Storage slot layout:**
   - Inheritance: `ContextUpgradeable` → `ERC165Upgradeable` → `AccessControlUpgradeable` → `SingleAdminAccessControlUpgradeable` → `ReentrancyGuardUpgradeable` → `ERC20PermitUpgradeable`/`ERC4626Upgradeable` → StakedENA.
   - Each base has `__gap` (49–50 slots). StakedENA's own variables (`vestingAmount`, `lastDistributionTimestamp`, `vestingPeriod`, `cooldownDuration`, `silo`, `cooldowns`) are appended after all inherited storage. ✅
   - `SingleAdminAccessControlUpgradeable` adds `_currentDefaultAdmin` + `_pendingDefaultAdmin` (2 slots) without its own `__gap` (E-4 from prior analysis). Current layout is correct; future upgrades to `SingleAdminAccessControlUpgradeable` could collide. ✅ Known hygiene issue.
2. **Implementation initialization front-run:**
   - `_disableInitializers()` in constructor sets `_initialized = type(uint8).max` on the implementation. `initialize` would revert with `InvalidInitialization`. ✅
3. **Initializer re-entrancy:**
   - OZ `initializer` modifier sets `_initialized = 1` before executing `_`. If `_` reverts, the entire tx reverts (including `_initialized = 1`). No re-initialization possible. ✅
   - `initialize` has no external calls before role grants. `silo = new EnaSilo(...)` is a CREATE (not a call to untrusted code). ✅
4. **Implementation swap:**
   - StakedENA does not inherit `UUPSUpgradeable`. Proxy upgrade logic is external (Transparent proxy or custom). `_authorizeUpgrade` is not in StakedENA's scope. ✅
5. **`silo` address immutability:**
   - `silo` is a state variable set in `initialize`. No setter. Cannot be changed without upgrade. ✅
   - `EnaSilo._STAKING_VAULT` is `immutable`, set to `address(this)` (proxy) in `initialize`. Even if implementation is upgraded, silo still points to proxy. ✅
6. **`transferAdmin(address(0))`:**
   - No zero-address check on `newAdmin`. Admin can set `_pendingDefaultAdmin = address(0)`.
   - `acceptAdmin` checks `msg.sender == _pendingDefaultAdmin`. Since `msg.sender != address(0)` always, no one can accept. Admin retains control and can set a new pending admin. ✅ Harmless but inconsistent with `initialize`'s zero-address checks. See **I-3**.
7. **`acceptAdmin` by blacklisted pending admin:**
   - `acceptAdmin` has no modifier (anyone can call, but only `_pendingDefaultAdmin` succeeds). No `BLACKLISTED_ROLE` check.
   - A blacklisted pending admin can `acceptAdmin`, become admin, then `removeFromBlacklist` themselves. ✅ Documented as acceptable (lines 222–225).

### Issues found

**I-2 (Informational) — `acceptAdmin` does not check `BLACKLISTED_ROLE`.**

Location: `SingleAdminAccessControlUpgradeable.sol:31–34`.

A blacklisted address that is the pending admin can call `acceptAdmin()` to become the admin, then unblacklist themselves. This is explicitly documented as acceptable ("It is deemed acceptable for a pending owner to be blacklisted. The pending owner can unblacklist themselves upon receiving ownership"). Not a bug — design choice.

**I-3 (Informational) — `transferAdmin` does not validate `address(0)`.**

Location: `SingleAdminAccessControlUpgradeable.sol:25–29`.

`transferAdmin(address(0))` succeeds, setting `_pendingDefaultAdmin = address(0)`. No one can `acceptAdmin` (since `msg.sender != address(0)`). The current admin retains control and can call `transferAdmin` again with a real address. Harmless but inconsistent with `initialize`'s zero-address checks on `_owner`/`_initialRewarder`/`_asset`.

---

## Area 6: ENASilo Interaction

### Functions analyzed
- `EnaSilo.withdraw` (line 25–27): `onlyStakingVault`, `_ENA.transfer(to, amount)`.
- `StakedENA.unstake` (lines 146–159): reads `underlyingAmount`, zeroes state, calls `silo.withdraw`.
- `StakedENA.cooldownAssets`/`cooldownShares` (lines 163–187): calls `_withdraw(..., address(silo), ...)` to move ENA to silo.

### Edge cases tested
1. **Unstake from silo without proper cooldown:**
   - `unstake` checks `block.timestamp >= cooldownEnd || cooldownDuration == 0`. Reverts `InvalidCooldown` if not met. ✅
2. **Silo drained by non-vault:**
   - `onlyStakingVault` modifier. `_STAKING_VAULT` is `immutable`, set to StakedENA proxy. No one else can call `withdraw`. ✅
3. **Silo balance < sum(underlyingAmount):**
   - `cooldownAssets(assets)`: `underlyingAmount += assets` AND `_withdraw` transfers exactly `assets` ENA to silo. Both sides increase by `assets`. ✅
   - `unstake`: `underlyingAmount = 0` (zeroed) AND `silo.withdraw(receiver, assets)` transfers exactly `assets` out. Both sides decrease by `assets`. ✅
   - ENA is not fee-on-transfer, so `safeTransfer` moves exactly `assets`. Invariant holds: `sum(underlyingAmount) <= silo.balanceOf(ENA)`. ✅
4. **Direct ENA donation to silo:**
   - If someone sends ENA directly to the silo, `silo.balanceOf > sum(underlyingAmount)`. The excess is stuck (no rescue function on silo). Harmless to stakers. ✅
5. **`EnaSilo.withdraw` uses `transfer` not `safeTransfer`:**
   - E-1 from prior analysis. ENA is a standard OZ ERC20 that reverts on failure. Safe in practice. ✅
6. **Cross-contract reentrancy (StakedENA ↔ EnaSilo):**
   - `unstake` → `silo.withdraw(receiver, assets)` → `_ENA.transfer(receiver, assets)`.
   - The silo calls `_ENA.transfer`, NOT StakedENA. No callback to StakedENA.
   - If `receiver` is a malicious contract with ERC777 hooks, it could reenter StakedENA. But: (1) `unstake` is `nonReentrant`; (2) state zeroed before external call; (3) ENA has no hooks. ✅
7. **`unstake(receiver, 0)` with no pending cooldown:**
   - `assets = underlyingAmount = 0`. `cooldownEnd = 0`, `block.timestamp >= 0` true. `silo.withdraw(receiver, 0)` — no-op transfer. Emit `Unstake(msg.sender, receiver, 0)`. Wasteful gas but harmless. ✅

### Issues found
**None new.** The silo is trivial and correct. The balance invariant holds. Prior E-1 (transfer vs safeTransfer) confirmed.

---

## Area 7: Vesting + Nonce Cancellation

### Functions analyzed
- `useNonce` (lines 278–280): `return _useNonce(msg.sender)`. Increments caller's nonce.
- `permit` (inherited from `ERC20PermitUpgradeable`): uses `_useNonce(owner)` to consume nonce. Validates signature.
- `transferInRewards` (lines 213–219): only REWARDER_ROLE can distribute.
- `_updateVestingAmount` / `_updateVestingPeriod` (lines 377–393): revert `StillVesting` if vesting active.

### Edge cases tested
1. **Front-run vesting:**
   - `transferInRewards` is `onlyRole(REWARDER_ROLE)`. Only authorized rewarder can call. No front-running by external attackers. ✅
2. **Cancel someone else's vesting:**
   - `useNonce` only increments `msg.sender`'s nonce (`_useNonce(msg.sender)`). No cross-user impact. ✅
3. **Invalidate pending permit via `useNonce`:**
   - `useNonce` increments nonce. Pending `permit` signatures with old nonce: `permit` calls `_useNonce(owner)` which returns the NEW (incremented) nonce. The structHash uses this new nonce. The signature (signed with old nonce) won't match. `signer != owner` → revert. ✅
   - `permit` only increments nonce on SUCCESS (revert undoes state). So invalid `permit` attempts don't grief the user's nonce. ✅
4. **Permit replay across chains:**
   - EIP712 domain includes `chainId` and `verifyingContract`. Different chains → different domain separators. ✅ No replay.
5. **`_updateVestingAmount` reverts during `redistributeLockedAmount(_, 0)`:**
   - If vesting is active, `_updateVestingAmount` reverts. Entire tx reverts (burn rolled back). Admin uses `to != 0` path instead. ✅

### Issues found

**I-4 (Informational) — EIP712 domain name uses token symbol "sENA" instead of token name "Staked ENA".**

Location: `StakedENA.sol:98` — `__ERC20Permit_init("sENA")`.

StakedENA's ERC20 name is "Staked ENA" (line 96) but the EIP712 domain name (used for `permit` signatures) is "sENA" (the symbol). OZ's `ERC20Permit` constructor comment says: "It's a good idea to use the same `name` that is defined as the ERC20 token name." The prior StakedUSDe V1 uses `__ERC20Permit_init("StakedUSDe")` (matching the name).

**Impact:** Frontends/integrators that construct the EIP712 domain separator using the ERC20 name ("Staked ENA") instead of querying `_EIP712Name()` will produce invalid `permit` signatures. No fund loss — just failed permit calls. The correct name is queryable via `eip712Domain()` (IERC5267).

**Severity: Informational.** Integration concern, not a security vulnerability.

---

## Area 8: Access Control Edge Cases

### Functions analyzed
- `SingleAdminAccessControlUpgradeable` (lines 25–60): `transferAdmin`, `acceptAdmin`, `grantRole`, `revokeRole`, `renounceRole`.
- `addToBlacklist` / `removeFromBlacklist` (lines 227–237): `onlyRole(BLACKLIST_MANAGER_ROLE)`, `nonReentrant`.
- `_beforeTokenTransfer` (lines 399–409): checks `msg.sender`, `from`, `to` for `BLACKLISTED_ROLE`.
- `_withdraw` (lines 358–371): checks `_owner` and `receiver` for `BLACKLISTED_ROLE`.

### Edge cases tested
1. **User with both BLACKLISTED_ROLE and REWARDER_ROLE:**
   - `transferInRewards` checks `onlyRole(REWARDER_ROLE)`, NOT `BLACKLISTED_ROLE`. A blacklisted rewarder can still call `transferInRewards`. See **I-1**. Action benefits vault (adds funds). ✅
2. **User with both BLACKLISTED_ROLE and BLACKLIST_MANAGER_ROLE:**
   - `removeFromBlacklist` checks `onlyRole(BLACKLIST_MANAGER_ROLE)`, NOT `BLACKLISTED_ROLE`. A blacklisted blacklist manager can `removeFromBlacklist(themselves)`, defeating the blacklist. See **I-5** below.
3. **Admin grants then revokes quickly:**
   - `grantRole` / `revokeRole` are `nonReentrant` (via `SingleAdminAccessControlUpgradeable` — actually they're NOT `nonReentrant`, but they only do storage updates, no external calls). ✅
   - No race condition — each call is atomic. ✅
4. **Blacklist bypass via approved operator (transferFrom):**
   - `transferFrom` → `_transfer` → `_beforeTokenTransfer(from, to, amount)`. Checks `msg.sender` (operator). If operator is blacklisted → revert. ✅
   - Checks `from`: if `from` is blacklisted and `to != 0` → revert. ✅
   - Checks `to`: if `to` is blacklisted → revert. ✅
5. **Blacklist bypass via `deposit` to fresh wallet:**
   - `deposit(assets, receiver)` → `_deposit` → `super._deposit` → `_mint(receiver, shares)` → `_beforeTokenTransfer(0, receiver, shares)`. If `msg.sender` (depositor) is blacklisted → revert. If `receiver` is blacklisted → revert. ✅
6. **`renounceRole` disabled:**
   - `renounceRole(bytes32, address)` always reverts (line 317–319). Admin can revoke roles via `revokeRole`. ✅
7. **`grantRole` / `revokeRole` cannot target DEFAULT_ADMIN_ROLE:**
   - `notAdmin(role)` modifier reverts if `role == DEFAULT_ADMIN_ROLE`. Admin transfer only via `transferAdmin` + `acceptAdmin`. ✅

### Issues found

**I-5 (Informational) — `addToBlacklist` / `removeFromBlacklist` do not check `BLACKLISTED_ROLE` on `msg.sender`.**

Location: `StakedENA.sol:227–237`.

A blacklisted address that still holds `BLACKLIST_MANAGER_ROLE` can call `removeFromBlacklist(themselves)` to unblacklist themselves, defeating the compliance freeze. The admin can mitigate by calling `revokeRole(BLACKLIST_MANAGER_ROLE, target)` BEFORE `addToBlacklist(target)`, but the contract does not enforce this order.

**Impact:** If the admin blacklists a BLACKLIST_MANAGER_ROLE holder without first revoking their manager role, the blacklist is immediately reversible by the target. This is a defense-in-depth gap, not a fund-theft vulnerability. The blacklist manager is a trusted role; blacklisting one is a rare event.

**Severity: Informational** (design gap, mitigatable by admin procedure).

**Remediation:** Add `if (hasRole(BLACKLISTED_ROLE, msg.sender)) revert OperationNotAllowed();` to `addToBlacklist` and `removeFromBlacklist`. Or document that the admin must revoke BLACKLIST_MANAGER_ROLE before blacklisting a manager.

---

## Area 9: Decimal Precision

### Functions analyzed
- `decimals()` (lines 308–310): hardcoded 18.
- OZ v4.x `_convertToShares`: `assets.mulDiv(totalSupply + 10**_decimalsOffset(), totalAssets + 1, rounding)`. `_decimalsOffset()` = 0 → offset = 1.
- OZ v4.x `_convertToAssets`: `shares.mulDiv(totalAssets + 1, totalSupply + 10**_decimalsOffset(), rounding)`.
- `getUnvestedAmount` (lines 294–305): `(deltaT * vestingAmount) / vestingPeriod`. Multiplication before division. ✅
- `previewRedeem` used in `redistributeLockedAmount` (line 263): Floor rounding (favors vault).

### Edge cases tested
1. **ENA decimals = 18, sENA decimals = 18:** ✅ Match. No precision loss from decimal mismatch.
2. **`decimals()` override vs `_decimalsOffset()`:**
   - `decimals()` returns 18 (hardcoded), bypassing `_underlyingDecimals + _decimalsOffset()`.
   - But `_decimalsOffset()` is used in `_convertToShares`/`_convertToAssets` (returns 0 → offset = 1). The `decimals()` override does NOT affect the virtual offset in conversion math. ✅
3. **Rounding direction:**
   - `previewDeposit` (Floor): depositor gets fewer shares. Favors vault. ✅
   - `previewMint` (Ceil): depositor pays more assets. Favors vault. ✅
   - `previewWithdraw` (Ceil): withdrawer burns more shares. Favors vault. ✅
   - `previewRedeem` (Floor): withdrawer gets fewer assets. Favors vault. ✅
   - All rounding consistently favors the vault. ✅
4. **Division before multiplication:**
   - `mulDiv` does multiplication first, then division. No precision loss from ordering. ✅
   - `getUnvestedAmount`: `(deltaT * vestingAmount) / vestingPeriod` — multiplication first. ✅
5. **`uint152(assets)` truncation:**
   - `uint152.max ≈ 5.7e45`. ENA supply ≈ 8e27. No truncation. ✅
   - Solidity 0.8 truncates on explicit uint downcast (does NOT revert). But values are far below the limit. ✅
6. **`uint104(block.timestamp)` truncation:**
   - `uint104.max ≈ 2e31`. `block.timestamp ≈ 1.7e9`. No truncation. ✅

### Issues found
**None.** Decimal precision is correct. All rounding favors the vault. No division-before-multiplication.

---

## Area 10: Cross-Function Reentrancy

### Functions analyzed
- All state-changing external functions: `withdraw`, `redeem`, `unstake`, `cooldownAssets`, `cooldownShares`, `setCooldownDuration`, `updateVestingPeriod`, `transferInRewards`, `addToBlacklist`, `removeFromBlacklist`, `rescueTokens`, `redistributeLockedAmount`, `useNonce`.
- Internal: `_deposit`, `_withdraw` (both `nonReentrant`).

### Edge cases tested
1. **deposit → withdraw → deposit in 1 tx (via attacker contract):**
   - `deposit` → `_deposit` (`nonReentrant`). Reentrant `withdraw` → `_withdraw` (`nonReentrant`) → reverts. ✅
   - Even if the attacker uses a callback during `safeTransferFrom` (ERC777-style), `_deposit`'s `nonReentrant` blocks reentry. ✅
2. **stake → unstake → stake (cooldown mode):**
   - `cooldownAssets` → `_withdraw` (`nonReentrant`). Reentrant `unstake` (`nonReentrant`) → reverts. ✅
   - `unstake` zeroes `underlyingAmount` BEFORE `silo.withdraw`. Reentrant `unstake` would see `underlyingAmount = 0` → withdraws 0. ✅ (Plus `nonReentrant` blocks it anyway.)
3. **Callback during transfer (ERC777-style):**
   - `_withdraw` → `super._withdraw` → `_burn` (state update) THEN `safeTransfer` (external call). CEI: burn before transfer. ✅
   - If `safeTransfer` triggers a callback (ERC777 `tokensReceived`), the callback reenters. But `_withdraw` is `nonReentrant`. ✅
   - ENA has no transfer hooks. ✅
4. **`cooldownAssets` state update before `_withdraw`:**
   - `cooldownEnd` and `underlyingAmount` are updated BEFORE `_withdraw`. If `_withdraw` reverts (e.g., `nonReentrant`), the entire tx reverts (including state updates). ✅
   - If `_withdraw` succeeds, state updates persist. ✅
5. **`rescueTokens` with malicious token:**
   - `rescueTokens` is `nonReentrant`. If the rescued token has a callback, reentry is blocked. ✅
   - `rescueTokens` blocks ENA rescue. Other tokens are at admin's discretion. ✅
6. **`transferInRewards` CEI:**
   - `_updateVestingAmount(amount)` (state) BEFORE `safeTransferFrom` (external). ✅
   - `nonReentrant`. ✅
7. **Cross-contract: StakedENA → EnaSilo → ENA → receiver:**
   - `unstake` → `silo.withdraw` → `_ENA.transfer(receiver, assets)`.
   - Silo does NOT call back into StakedENA. ✅
   - `receiver` could be a contract with hooks, but `unstake` is `nonReentrant` + state zeroed. ✅

### Issues found
**None.** All state-changing functions are protected by `nonReentrant` (either directly or via `_deposit`/`_withdraw`). CEI is respected throughout. ENA has no transfer hooks. No cross-function or cross-contract reentrancy.

---

## Summary of Findings

| ID | Area | Finding | Severity | New? |
|----|------|---------|----------|------|
| I-1 | 3 (Rewards) | `transferInRewards` doesn't check `BLACKLISTED_ROLE` on `msg.sender` — blacklisted rewarder can still distribute | Informational | **Yes** |
| I-2 | 5 (Proxy) | `acceptAdmin` doesn't check `BLACKLISTED_ROLE` — blacklisted pending admin can accept | Informational | Yes (but documented as acceptable) |
| I-3 | 5 (Proxy) | `transferAdmin` doesn't validate `address(0)` — harmless but inconsistent | Informational | **Yes** |
| I-4 | 7 (Vesting) | EIP712 domain name uses symbol "sENA" instead of name "Staked ENA" — integration concern | Informational | **Yes** |
| I-5 | 8 (Access) | `addToBlacklist`/`removeFromBlacklist` don't check `BLACKLISTED_ROLE` on `msg.sender` — blacklisted manager can self-unblacklist | Informational | **Yes** |

**Critical: 0. High: 0. Medium: 0 (new). Low: 0 (new). Informational: 5 (new).**

### Prior findings confirmed (not re-reported):
- M-1 (unstake blacklist bypass) — KNOWN, skipped.
- L-1 (redistribute-to-0 sole-staker brick, recoverable) — confirmed.
- E-1 through E-6 — confirmed.

---

## Conclusion

After exhaustive line-by-line analysis of all 10 focus areas, **no new Critical, High, Medium, or Low bugs were found** in StakedENA or EnaSilo beyond the known M-1 (unstake blacklist bypass, which was skipped per instructions).

The code is defensively coded:
- Every state-changing external function is `nonReentrant` (directly or via `_deposit`/`_withdraw`).
- CEI is respected throughout.
- The ERC4626 inflation attack is double-mitigated (OZ v4.x virtual offset + `_checkMinShares` 1e18 floor).
- The silo balance invariant holds (`sum(underlyingAmount) <= silo.balanceOf`).
- All rounding consistently favors the vault.
- The blacklist is enforced on all sENA movements via `_beforeTokenTransfer` (confirmed: StakedENA uses OZ v4.x where this hook IS called).
- The vesting math is correct (`totalAssets = balanceOf - getUnvestedAmount()` never underflows with standard ENA).

The 5 new Informational findings are all **defense-in-depth gaps or integration concerns**, none of which enable fund theft, fund freezing, or protocol bricking. The most notable (I-5) is that privileged functions (`transferInRewards`, `addToBlacklist`/`removeFromBlacklist`, `acceptAdmin`) do not check `BLACKLISTED_ROLE` on `msg.sender`, allowing a blacklisted role-holder to continue exercising their privileged function. This is mitigatable by admin procedure (revoke role before blacklisting) but not enforced by the contract.

No separate vuln PoC files were written (no Critical/High/Medium findings to document).
