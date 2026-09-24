# Staker Vaults — Permissionless `receiveRewards` Enables Share-Price Manipulation

## Metadata

| Field | Value |
|---|---|
| **Severity** | Low |
| **Area** | Proof-of-Liquidity / Staker Vaults |
| **Contracts** | `LSTStakerVault.sol`, `WBERAStakerVault.sol` |
| **Function** | `receiveRewards` |
| **Lines** | LSTStakerVault:194–197, WBERAStakerVault:269–272 |
| **File** | `src/pol/lst/LSTStakerVault.sol`, `src/pol/WBERAStakerVault.sol` |
| **Audited commit** | `70e392fc` (2026-08-11) |

## Description

Both staker vaults expose a `receiveRewards` function with **no access control**
— anyone can call it to deposit the underlying asset (WBERA or LST) into the
vault, which increases `totalAssets()` and therefore the ERC-4626 share price.

```solidity
// LSTStakerVault.sol:194
function receiveRewards(uint256 amount) external {
    asset().safeTransferFrom(msg.sender, address(this), amount);
    emit RewardsReceived(msg.sender, amount, totalAssets());
}

// WBERAStakerVault.sol:269
function receiveRewards(uint256 amount) external {
    WBERA.safeTransferFrom(msg.sender, address(this), amount);
    emit RewardsReceived(msg.sender, amount, totalAssets());
}
```

Because `totalAssets()` is computed as `balanceOf(vault) - reservedAssets`, any
direct deposit inflates the per-share value. The intended caller is
`IncentivesCollector._claim`, but the function is open to all.

## Attack Scenario

### Scenario A — Inflation Attack on Empty Vault (LSTStakerVault)

1. The LST staker vault is freshly deployed with 0 total supply (or 1 share
   of dust).
2. Attacker calls `receiveRewards(largeAmount)`, donating a large amount of the
   LST asset.
3. `totalAssets()` jumps from ~0 to `largeAmount`. The share price is now
   extremely high.
4. Subsequent legitimate depositors receive ~0 shares for their deposit
   (rounded down), effectively losing their funds to the attacker (if the
   attacker is the only other shareholder) or to all existing shareholders.

This is the classic ERC-4626 inflation attack, enabled here because
`receiveRewards` is unguarded.

### Scenario B — Oracle / Accounting Manipulation

`IncentivesCollector._splitAmount` uses `IERC4626(vault).totalAssets()` to
determine the proportional split of WBERA payouts:

```solidity
stakes[0] = IERC4626(wberaStakerVault).totalAssets();
...
uint256 stake = vault.totalAssets();
```

An attacker who donates WBERA to `wberaStakerVault` via `receiveRewards`
inflates `stakes[0]`, skewing the split so the WBERA vault receives a larger
share of future payouts. While the attacker doesn't directly profit (the donated
WBERA benefits all shareholders), they can manipulate the payout distribution
ratio between vaults.

### Scenario C — Sandwich on Withdrawal

1. Victim queues a withdrawal (burns shares, reserves `assets`).
2. Attacker front-runs with `receiveRewards(hugeAmount)`, inflating the share
   price.
3. Victim cancels the withdrawal:
   ```solidity
   uint256 newSharesToMint = previewDeposit(assets);  // fewer shares due to higher price
   _mint(msg.sender, newSharesToMint);
   ```
   The victim gets back **fewer shares** than they burned — their proportional
   stake in the vault is permanently reduced.

## Proof of Concept (Scenario C)

```solidity
function test_sandwichCancelReducesShares() public {
    // Vault: 100 WBERA, 100 shares. Price = 1:1
    deal(WBERA, address(vault), 100e18);
    vm.prank(alice);
    vault.deposit(100e18, alice);   // alice has 100 shares

    // Alice queues withdrawal of 50 shares → reserves 50 WBERA
    vm.prank(alice);
    vault.queueRedeem(50e18, alice, alice);
    // totalAssets = 100 - 50 = 50, totalShares = 50, price still 1:1

    // Attacker donates 950 WBERA
    deal(WBERA, attacker, 950e18);
    vm.startPrank(attacker);
    IERC20(WBERA).approve(address(vault), 950e18);
    vault.receiveRewards(950e18);
    vm.stopPrank();
    // totalAssets = 50 + 950 = 1000, totalShares = 50, price = 20:1

    // Alice cancels: assets = 50, previewDeposit(50) = 50 * 50 / 1000 = 2 shares
    vm.prank(alice);
    vault.cancelQueuedWithdrawal(requestId);
    // Alice now has 50 + 2 = 52 shares (was 100). Lost 48 shares!
    assertLt(vault.balanceOf(alice), 100e18);
}
```

## Impact

- **Inflation attack on new vaults:** depositors can lose 100% of their deposit
  if an attacker front-runs with a large `receiveRewards` donation.
- **Withdrawal-cancel sandwich:** users who cancel queued withdrawals receive
  fewer shares than they originally burned, permanently losing proportional
  stake.
- **Payout-split manipulation:** `IncentivesCollector._splitAmount` can be
  skewed by donating to a specific vault.
- **Mitigating factor:** the attacker must donate real tokens (which they lose
  to existing shareholders), so the inflation attack is only profitable when
  the vault has very few or zero other shareholders.

## Three-Perspective Audit

### 1. Attacker perspective
Profitable only against near-empty vaults (classic 4626 inflation attack vector)
or as a griefing vector against withdrawal cancellers. The cost is the donated
tokens, which benefit existing shareholders.

### 2. Protocol perspective
The intended caller (`IncentivesCollector`) is a trusted contract. Restricting
`receiveRewards` to `onlyRole(MANAGER_ROLE)` or to the `IncentivesCollector`
address would prevent external abuse without breaking the reward-distribution
flow.

### 3. Auditor perspective
Both vaults inherit `AccessControlUpgradeable` / `FactoryOwnable` but do not
apply any modifier to `receiveRewards`. Adding a whitelist check (e.g.,
`onlyIncentivesCollector` or `onlyFactoryOwner`) is a minimal, safe change.

## Recommendation

```solidity
// LSTStakerVault.sol
+ address public incentivesCollector;  // set by factory owner
+
+ modifier onlyIncentivesCollector() {
+     if (msg.sender != incentivesCollector) Unauthorized.selector.revertWith();
+     _;
+ }

- function receiveRewards(uint256 amount) external {
+ function receiveRewards(uint256 amount) external onlyIncentivesCollector {
      asset().safeTransferFrom(msg.sender, address(this), amount);
      emit RewardsReceived(msg.sender, amount, totalAssets());
  }
```

Apply the same change to `WBERAStakerVault.sol`.

For defence-in-depth, also add a **dead-share** mechanism (mint 1_000 dead
shares on first deposit) or use OpenZeppelin's `ERC4626` virtual-share offset
(`_useVirtualShares() returns (true)`) to prevent the inflation attack on empty
vaults.
