# NEW BUG: Blacklisting the EnaSilo Address Bricks the Entire Cooldown Exit Flow

**Agent:** Opus
**Task ID:** ethena-stakedena-new-bugs
**Date:** 2026-09-24
**Area:** Access Control / ENASilo Interaction

---

## 1. Vulnerability Description

`StakedENA._withdraw` (the internal function called by `withdraw`, `redeem`, `cooldownAssets`, and `cooldownShares`) checks `hasRole(BLACKLISTED_ROLE, receiver)` and reverts if the `receiver` is blacklisted. When `cooldownAssets` or `cooldownShares` is called, `receiver` is `address(silo)` — the internal `EnaSilo` contract that holds ENA during the cooldown window.

A holder of `BLACKLIST_MANAGER_ROLE` can call `addToBlacklist(address(silo))` (the silo is NOT the owner, so the `notOwner` modifier does not protect it). Once the silo is blacklisted, **every** `cooldownAssets` and `cooldownShares` call reverts, because `_withdraw` sees the silo as a blacklisted `receiver`.

When `cooldownDuration > 0` (the normal operating mode — initialized to 7 days), the standard `withdraw` and `redeem` functions are also disabled (`ensureCooldownOff` modifier reverts). This means **all users are locked** — they can neither start a cooldown nor directly withdraw. Their sENA shares are temporarily frozen until the admin intervenes by either:

1. Calling `removeFromBlacklist(address(silo))`, or
2. Calling `setCooldownDuration(0)` to enable the standard ERC4626 `withdraw`/`redeem` path.

This is a **temporary fund-freeze griefing vector** available to any `BLACKLIST_MANAGER_ROLE` holder. The `BLACKLIST_MANAGER_ROLE` is a separate, lower-trust role than `DEFAULT_ADMIN_ROLE` (it is typically granted to a compliance operations team). A compromise or mistake by this role-holder locks all stakers' funds.

The root cause is a **design flaw**: `_withdraw`'s `receiver` blacklist check does not distinguish between user-facing receivers (where the check is a compliance feature) and the internal silo receiver (where the check is a self-inflicted brick). The contract protects the `owner` from being blacklisted (`notOwner` modifier) but does **not** protect the `silo`.

---

## 2. Contract + Function + Line Number

**Contract:** `StakedENA.sol`

**Primary location:** `_withdraw` — lines 358–371, specifically the blacklist check at **lines 365–367**:
```solidity
function _withdraw(address caller, address receiver, address _owner, uint256 assets, uint256 shares)
    internal override nonReentrant notZero(assets) notZero(shares)
{
    if (hasRole(BLACKLISTED_ROLE, _owner) || hasRole(BLACKLISTED_ROLE, receiver)) {  // ← LINE 365
      revert OperationNotAllowed();                                                   // ← LINE 366
    }                                                                                 // ← LINE 367
    super._withdraw(caller, receiver, _owner, assets, shares);
    _checkMinShares();
}
```

**Caller path (cooldown):** `cooldownAssets` — lines 163–173, specifically line 171:
```solidity
_withdraw(msg.sender, address(silo), msg.sender, assets, shares);  // ← receiver = silo
```
`cooldownShares` — line 185, same pattern.

**Blacklist entry point:** `addToBlacklist` — lines 227–229:
```solidity
function addToBlacklist(address target) external nonReentrant onlyRole(BLACKLIST_MANAGER_ROLE) notOwner(target) {
    _grantRole(BLACKLISTED_ROLE, target);  // ← no check that target != address(silo)
}
```

**Exit path blocked:** `withdraw` — lines 116–124 and `redeem` — lines 129–137, both gated by `ensureCooldownOff` (lines 58–61) which reverts when `cooldownDuration != 0`.

**Unaffected path (proving the lock):** `unstake` — lines 146–159 does NOT call `_withdraw`; it calls `silo.withdraw(receiver, assets)` directly, so users with **existing** completed cooldowns can still unstake. But users who have NOT yet started a cooldown are locked.

---

## 3. Attack Scenario (Step-by-Step)

1. **Setup:** `StakedENA` is deployed and initialized with `cooldownDuration = 7 days` (the default from `initialize`, line 108). The `DEFAULT_ADMIN_ROLE` holder grants `BLACKLIST_MANAGER_ROLE` to a compliance operations address (`manager`).

2. **Users stake:** Multiple users deposit ENA and receive sENA shares. They intend to exit via the cooldown flow (`cooldownAssets` → wait 7 days → `unstake`).

3. **Attacker acts:** The `BLACKLIST_MANAGER_ROLE` holder (compromised, rogue, or simply mistaken) calls `addToBlacklist(address(silo))`.
   - `notOwner(address(silo))` passes because `address(silo) != owner()`.
   - `_grantRole(BLACKLISTED_ROLE, address(silo))` executes. The silo now has `BLACKLISTED_ROLE`.

4. **User tries to exit via cooldown:** User calls `cooldownAssets(100e18)`.
   - `ensureCooldownOn` passes (`cooldownDuration = 7 days > 0`).
   - `assets > maxWithdraw(msg.sender)` passes (user has enough shares).
   - State (`cooldownEnd`, `underlyingAmount`) updated.
   - `_withdraw(msg.sender, address(silo), msg.sender, assets, shares)` is called.
   - **`hasRole(BLACKLISTED_ROLE, receiver)` where `receiver = address(silo)` → `true` → `revert OperationNotAllowed()`.**
   - The entire transaction reverts. The user cannot start a cooldown.

5. **User tries to exit via standard withdraw:** User calls `withdraw(100e18, user, user)`.
   - **`ensureCooldownOff` reverts** because `cooldownDuration = 7 days != 0`.
   - The user cannot withdraw directly.

6. **User tries to exit via standard redeem:** User calls `redeem(shares, user, user)`.
   - **`ensureCooldownOff` reverts** for the same reason.

7. **Result:** The user's sENA shares are **temporarily frozen**. They cannot exit through any available path. This affects **all** stakers simultaneously.

8. **Recovery (requires admin):** The `DEFAULT_ADMIN_ROLE` holder must either:
   - Call `removeFromBlacklist(address(silo))` to un-blacklist the silo, restoring the cooldown flow; or
   - Call `setCooldownDuration(0)` to enable `withdraw`/`redeem` (bypassing the cooldown flow entirely).

   Until the admin acts, all stakers are locked.

---

## 4. PoC Code (Foundry)

```solidity
// SPDX-License-Identifier: GPL-3.0
pragma solidity 0.8.26;

import {Test} from "forge-std/Test.sol";

// ============================================================================
// Self-contained PoC: Blacklisting the EnaSilo bricks the cooldown exit flow.
// ----------------------------------------------------------------------------
// This PoC uses minimal mock contracts that replicate the EXACT logic of
// StakedENA's _withdraw (receiver blacklist check), cooldownAssets (receiver =
// silo), addToBlacklist (no silo protection), and ensureCooldownOff.
//
// The real StakedENA.sol lines referenced:
//   _withdraw lines 358-371 (blacklist check on receiver: line 365)
//   cooldownAssets lines 163-173 (passes address(silo) as receiver: line 171)
//   addToBlacklist lines 227-229 (no protection for silo)
//   withdraw/redeem lines 116-137 (ensureCooldownOff: lines 58-61)
//
// Run: forge test --match-contract SiloBlacklistGriefingPoC -vvvv
// ============================================================================

/// @dev Minimal ERC20 mock
contract MockENA {
    string public name;
    string public symbol;
    uint8 public constant decimals = 18;
    uint256 public totalSupply;
    mapping(address => uint256) public balanceOf;
    mapping(address => mapping(address => uint256)) public allowance;

    constructor(string memory _name, string memory _sym) {
        name = _name;
        symbol = _sym;
    }

    function mint(address to, uint256 amt) external {
        balanceOf[to] += amt;
        totalSupply += amt;
    }

    function transfer(address to, uint256 amt) external returns (bool) {
        require(balanceOf[msg.sender] >= amt, "insufficient");
        balanceOf[msg.sender] -= amt;
        balanceOf[to] += amt;
        return true;
    }

    function transferFrom(address from, address to, uint256 amt) external returns (bool) {
        require(allowance[from][msg.sender] >= amt, "allowance");
        require(balanceOf[from] >= amt, "insufficient");
        allowance[from][msg.sender] -= amt;
        balanceOf[from] -= amt;
        balanceOf[to] += amt;
        return true;
    }

    function approve(address spender, uint256 amt) external returns (bool) {
        allowance[msg.sender][spender] = amt;
        return true;
    }
}

/// @dev Minimal EnaSilo replica (real ENASilo.sol lines 11-28)
contract MockEnaSilo {
    address public immutable STAKING_VAULT;
    MockENA public immutable ENA;

    constructor(address stakingVault, MockENA ena) {
        STAKING_VAULT = stakingVault;
        ENA = ena;
    }

    function withdraw(address to, uint256 amount) external {
        require(msg.sender == STAKING_VAULT, "OnlyStakingVault");
        ENA.transfer(to, amount);
    }
}

/// @dev Simplified StakedENA replica focusing on the vulnerability path.
///      Replicates: _withdraw (with receiver blacklist check), cooldownAssets,
///      withdraw (ensureCooldownOff), addToBlacklist, deposit.
contract MockStakedENA {
    MockENA public ena;
    MockEnaSilo public silo;
    uint24 public cooldownDuration;

    // Shares (sENA) - simplified
    uint256 public totalSupply;
    mapping(address => uint256) public balanceOf;

    // Blacklist (replicates AccessControl roles)
    mapping(address => bool) public isBlacklisted;
    address public owner;
    address public blacklistManager;

    // Cooldown state (replicates UserCooldown struct)
    struct UserCooldown {
        uint104 cooldownEnd;
        uint152 underlyingAmount;
    }
    mapping(address => UserCooldown) public cooldowns;

    // Events
    event CooldownStarted(address indexed user, uint256 assets, uint256 shares);
    event Unstake(address indexed user, address indexed receiver, uint256 assets);

    constructor(MockENA _ena, address _owner) {
        ena = _ena;
        owner = _owner;
        silo = new MockEnaSilo(address(this), _ena);
        cooldownDuration = 7 days; // matches initialize line 108
    }

    // --- Modifiers (replicating StakedENA lines 57-79) ---
    modifier ensureCooldownOff() {
        require(cooldownDuration == 0, "OperationNotAllowed"); // line 59
        _;
    }

    modifier ensureCooldownOn() {
        require(cooldownDuration != 0, "OperationNotAllowed"); // line 65
        _;
    }

    modifier onlyBlacklistManager() {
        require(msg.sender == blacklistManager, "BLACKLIST_MANAGER_ROLE");
        _;
    }

    modifier notOwner(address target) {
        require(target != owner, "CantBlacklistOwner"); // line 77
        _;
    }

    // --- Deposit (simplified 1:1 exchange for PoC clarity) ---
    function deposit(uint256 assets, address receiver) external {
        require(assets > 0, "InvalidAmount");
        ena.transferFrom(msg.sender, address(this), assets);
        uint256 shares = assets; // 1:1 for simplicity
        totalSupply += shares;
        balanceOf[receiver] += shares;
    }

    // --- _withdraw replica (lines 358-371) ---
    function _withdraw(address caller, address receiver, address _owner, uint256 assets, uint256 shares) internal {
        // LINE 365-367: blacklist check on _owner AND receiver
        if (isBlacklisted[_owner] || isBlacklisted[receiver]) {
            revert("OperationNotAllowed"); // ← THIS IS THE BUG when receiver = silo
        }
        // burn shares
        balanceOf[_owner] -= shares;
        totalSupply -= shares;
        // transfer assets to receiver
        ena.transfer(receiver, assets);
    }

    // --- withdraw (lines 116-124) ---
    function withdraw(uint256 assets, address receiver, address _owner) external ensureCooldownOff {
        uint256 shares = assets; // 1:1
        _withdraw(msg.sender, receiver, _owner, assets, shares);
    }

    // --- cooldownAssets (lines 163-173) ---
    function cooldownAssets(uint256 assets) external ensureCooldownOn {
        uint256 shares = assets; // 1:1 for simplicity
        cooldowns[msg.sender].cooldownEnd = uint104(block.timestamp) + cooldownDuration;
        cooldowns[msg.sender].underlyingAmount += uint152(assets);
        // LINE 171: receiver = address(silo) ← triggers blacklist check on silo
        _withdraw(msg.sender, address(silo), msg.sender, assets, shares);
        emit CooldownStarted(msg.sender, assets, shares);
    }

    // --- unstake (lines 146-159) — does NOT call _withdraw, no receiver blacklist check ---
    function unstake(address receiver) external {
        UserCooldown storage uc = cooldowns[msg.sender];
        uint256 assets = uc.underlyingAmount;
        require(block.timestamp >= uc.cooldownEnd || cooldownDuration == 0, "InvalidCooldown");
        uc.cooldownEnd = 0;
        uc.underlyingAmount = 0;
        silo.withdraw(receiver, assets); // direct silo call, no _withdraw
        emit Unstake(msg.sender, receiver, assets);
    }

    // --- addToBlacklist (lines 227-229) — NO protection for silo ---
    function addToBlacklist(address target) external onlyBlacklistManager notOwner(target) {
        isBlacklisted[target] = true; // ← can target address(silo)!
    }

    function removeFromBlacklist(address target) external onlyBlacklistManager {
        isBlacklisted[target] = false;
    }

    function setCooldownDuration(uint24 duration) external {
        require(msg.sender == owner, "DEFAULT_ADMIN_ROLE");
        cooldownDuration = duration;
    }

    function setBlacklistManager(address mgr) external {
        require(msg.sender == owner, "DEFAULT_ADMIN_ROLE");
        blacklistManager = mgr;
    }

    function maxWithdraw(address user) public view returns (uint256) {
        return balanceOf[user]; // 1:1 simplified
    }
}

/// @title SiloBlacklistGriefingPoC
contract SiloBlacklistGriefingPoC is Test {
    MockENA ena;
    MockStakedENA stakedENA;
    address admin = makeAddr("admin");
    address manager = makeAddr("manager"); // BLACKLIST_MANAGER_ROLE holder
    address alice = makeAddr("alice"); // innocent staker

    function setUp() public {
        ena = new MockENA("Ethena", "ENA");
        vm.prank(admin);
        stakedENA = new MockStakedENA(ena, admin);
        vm.prank(admin);
        stakedENA.setBlacklistManager(manager);

        // Alice deposits 1000 ENA
        ena.mint(alice, 1000e18);
        vm.startPrank(alice);
        ena.approve(address(stakedENA), 1000e18);
        stakedENA.deposit(1000e18, alice);
        vm.stopPrank();
    }

    function test_siloBlacklistBricksCooldownExit() public {
        address siloAddr = address(stakedENA.silo());
        assertEq(stakedENA.cooldownDuration(), 7 days, "cooldown is ON");

        // --- STEP 1: Manager blacklists the silo ---
        vm.prank(manager);
        stakedENA.addToBlacklist(siloAddr);
        assertTrue(stakedENA.isBlacklisted(siloAddr), "silo should be blacklisted");

        // --- STEP 2: Alice tries cooldownAssets → REVERTS ---
        vm.prank(alice);
        vm.expectRevert("OperationNotAllowed");
        stakedENA.cooldownAssets(100e18);

        // --- STEP 3: Alice tries standard withdraw → REVERTS (cooldown ON) ---
        vm.prank(alice);
        vm.expectRevert("OperationNotAllowed");
        stakedENA.withdraw(100e18, alice, alice);

        // --- STEP 4: Alice is LOCKED — cannot exit via any path ---
        // Her 1000 ENA worth of sENA is frozen.
        assertEq(stakedENA.balanceOf(alice), 1000e18, "alice still holds shares");
        // No exit path available while cooldownDuration > 0 and silo is blacklisted

        // --- STEP 5 (recovery): Admin sets cooldownDuration = 0 ---
        vm.prank(admin);
        stakedENA.setCooldownDuration(0);
        // Now withdraw works (ensureCooldownOff passes), silo blacklist irrelevant
        vm.prank(alice);
        stakedENA.withdraw(100e18, alice, alice);
        assertEq(stakedENA.balanceOf(alice), 900e18, "alice partially exited after admin fix");

        // --- STEP 6 (alternative recovery): Admin un-blacklists silo ---
        vm.prank(admin);
        stakedENA.setCooldownDuration(7 days); // restore cooldown
        vm.prank(manager);
        stakedENA.addToBlacklist(siloAddr); // re-blacklist silo
        vm.prank(alice);
        vm.expectRevert("OperationNotAllowed");
        stakedENA.cooldownAssets(100e18); // still bricked

        vm.prank(manager);
        stakedENA.removeFromBlacklist(siloAddr); // un-blacklist
        // Now cooldown works again
        vm.prank(alice);
        stakedENA.cooldownAssets(100e18); // succeeds
        assertEq(stakedENA.balanceOf(alice), 800e18, "alice cooled down after un-blacklist");
    }

    function test_nonBlacklistedSiloAllowsCooldown() public {
        // Control test: without blacklisting silo, cooldown works fine
        vm.prank(alice);
        stakedENA.cooldownAssets(100e18);
        assertEq(stakedENA.balanceOf(alice), 900e18, "cooldown succeeded");
    }
}
```

---

## 5. Impact Assessment

**What happens:** A `BLACKLIST_MANAGER_ROLE` holder blacklists `address(silo)`. All `cooldownAssets`/`cooldownShares` calls revert. When `cooldownDuration > 0` (the default), `withdraw`/`redeem` are also disabled. **All stakers are locked** — no exit path exists until the admin intervenes.

**Who is affected:** Every sENA holder who has not yet completed a cooldown. Users with already-completed cooldowns can still `unstake` (that path doesn't call `_withdraw`), but users who haven't started a cooldown, or who are mid-cooldown and want to top up, are blocked.

**Fund loss:** No **permanent** fund loss. The admin can always recover by un-blacklisting the silo or setting `cooldownDuration = 0`. However, during the lock period:
- Users cannot react to market events (e.g., ENA price crash, depeg event).
- Panic selling of sENA on secondary markets (if listed) could cause sENA to trade at a discount.
- Reputational damage to the protocol.
- If the admin key is also unavailable (e.g., multi-sig delay), the lock persists.

**Likelihood:** Medium. The `BLACKLIST_MANAGER_ROLE` is a separate role from `DEFAULT_ADMIN_ROLE`, typically held by a compliance operations team. It may have weaker key management than the admin. A compromised or rogue manager can execute this attack trivially (one transaction). It could also happen by accident (manager accidentally includes the silo address in a batch blacklist transaction).

**Severity assessment:** The attack locks **all** users' funds temporarily with a single transaction from a semi-trusted role. While recoverable, the temporary freeze of the entire staking vault is a significant disruption. This is above the "Informational" threshold of prior findings (I-1 through I-5) because it affects **all** users simultaneously and requires admin intervention to resolve, not just the blacklisted address.

**Severity: Medium** (temporary full-vault lock, single-transaction attack, semi-trusted role, admin-recoverable).

---

## 6. Severity

**Medium**

Rationale: Temporary fund freeze affecting ALL stakers, executable by a role-holder (BLACKLIST_MANAGER) that is lower-trust than the admin. No permanent loss, but significant disruption and requires admin intervention. The contract explicitly protects the `owner` from blacklisting (`notOwner`) but fails to protect the `silo` — an internal contract whose blacklisting has systemic consequences.

---

## 7. Three-Perspective Audit

### Prosecutor (argues this IS a vulnerability)

**The contract has a systemic design flaw.** `_withdraw` performs a blacklist check on `receiver` (line 365). When `cooldownAssets` passes `address(silo)` as `receiver` (line 171), this check becomes a self-inflicted brick if the silo is ever blacklisted. The contract recognizes that some addresses should be protected from blacklisting — it has the `notOwner` modifier on `addToBlacklist` (line 76-79) specifically to prevent blacklisting the admin. But it does **not** extend this protection to the `silo`, even though blacklisting the silo has far more severe consequences (locking ALL users) than blacklisting a single user.

The `BLACKLIST_MANAGER_ROLE` is a separate, lower-trust role. In the real Ethena deployment, blacklist management is typically delegated to a compliance operations team with separate key custody. A compromise of this key — which is more likely than compromising the admin multi-sig — allows a single-transaction attack that freezes the entire vault. The attacker doesn't even need to be malicious: a compliance team member who mistakenly includes the silo address in a batch-blacklist operation (e.g., blacklisting a list of sanctioned addresses where the silo address is accidentally included) would trigger this lock.

The prior audits found I-1 (blacklisted rewarder can still distribute) and I-5 (blacklisted manager can self-unblacklist), but **neither identified that the silo itself can be blacklisted**. This is a distinct, higher-impact finding because it affects ALL users, not just the role-holder.

The defense will say "the admin can fix it." But the admin can fix ANY vulnerability — that's not the point. The point is that a lower-trust role can cause a system-wide freeze, and the contract does nothing to prevent it. The fix is trivial: add `require(target != address(silo))` to `addToBlacklist`, or exempt the silo from the `receiver` blacklist check in `_withdraw` when called from `cooldownAssets`/`cooldownShares`.

### Defense (argues this is NOT a vulnerability or is low severity)

**This is expected behavior of a role-based system, not a bug.** The `BLACKLIST_MANAGER_ROLE` is a trusted role. By definition, a trusted role-holder can cause harm if they go rogue — the admin can `grantRole` to anyone, the rewarder can refuse to distribute, etc. The question is whether the contract has a **logic error** or **missing safeguard** that makes this worse than inherent trusted-role risk.

The `notOwner` modifier exists not because blacklisting the owner is a "systemic" risk, but because the owner is the **only** address that can un-blacklist — blacklisting the owner would create a permanent lock with no recovery. Blacklisting the silo, by contrast, is **fully recoverable** by the admin (via `removeFromBlacklist` or `setCooldownDuration(0)`). The severity is fundamentally different: one is permanent, the other is temporary.

Furthermore, the `BLACKLIST_MANAGER_ROLE` holder who blacklists the silo is either (a) malicious, in which case they could equally cause harm by blacklisting random large stakers (causing individual fund freezes), or (b) mistaken, in which case the error is quickly noticed and reversed. The "all users locked" framing is dramatic but the recovery is a single admin transaction.

The prior audits explicitly analyzed the blacklist system (Area 8 of the fresh audit) and concluded that the gaps were Informational (I-1, I-5). This finding is the same class of issue — a role-holder exercising their granted power in a suboptimal way. It should be Informational, not Medium.

### Judge (weighs both sides)

**Verdict: Medium, confirmed as a new finding.**

The Defense makes a fair point that trusted roles inherently carry risk, and that `notOwner` exists specifically to prevent **unrecoverable** locks (blacklisting the admin who is the only one who can un-blacklist). However, the Prosecutor's argument has more weight for three reasons:

1. **Blast radius:** Blacklisting a random large staker affects ONE user. Blacklisting the silo affects ALL users simultaneously. The systemic impact is qualitatively different and justifies a higher severity than individual-user blacklist issues (I-1, I-5).

2. **Likelihood of accidental trigger:** Unlike blacklisting a specific user (which requires targeting), the silo address is a known, constant address that could plausibly appear in a batch-blacklist list (e.g., a compliance script that lists all contract addresses interacting with a flagged entity). The silo receives ENA from every cooldown — its address appears in every cooldown transaction's logs. An automated compliance tool could mistakenly flag it.

3. **Trivial fix:** The contract already has the pattern (`notOwner`) for protecting critical addresses. Extending it to the silo (`require(target != address(silo))` or a more general `require(target != address(silo) && !hasRole(DEFAULT_ADMIN_ROLE, target))`) is a one-line fix. The absence of this safeguard is a design oversight, not an inherent limitation.

**However**, the severity is tempered by:
- Full recoverability (admin can always fix it).
- Requires a trusted role (not exploitable by an external attacker).
- No permanent fund loss.

**Final ruling: Medium.** This is a legitimate new finding not covered by prior audits. It represents a design flaw where an internal contract (silo) is not protected from blacklisting, enabling a system-wide temporary freeze by a lower-trust role-holder. The fix is trivial and the issue is distinct from the previously-identified Informational findings (which only affected the role-holder themselves, not all users).

**Recommended remediation:**
```solidity
// Option A: Protect the silo in addToBlacklist
function addToBlacklist(address target) external nonReentrant onlyRole(BLACKLIST_MANAGER_ROLE) notOwner(target) {
    if (target == address(silo)) revert OperationNotAllowed();  // ← ADD THIS
    _grantRole(BLACKLISTED_ROLE, target);
}

// Option B: Exempt silo from receiver check in cooldown paths (less clean)
// Refactor cooldownAssets/cooldownShares to not use _withdraw's receiver check for the silo.
```
Option A is preferred — it's a one-line fix that prevents the silo from ever being blacklisted, similar to how `notOwner` prevents the admin from being blacklisted.
