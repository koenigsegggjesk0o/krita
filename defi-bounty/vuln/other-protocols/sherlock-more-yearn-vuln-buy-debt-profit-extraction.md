# Yearn V3 — `buy_debt` proportional-share payout lets a DEBT_PURCHASER extract unrealized strategy profit

**Protocol**: Yearn V3 (Sherlock bounty #30, up to $200,000 USDC)
**Repo**: `github.com/yearn/yearn-vaults-v3` (cloned to `/home/z/usual-yearn-v3`)
**Severity**: Low (role-gated; informational on value extraction)
**Area**: Access control / integer precision / cross-contract interaction

---

## 1. Description

`VaultV3.buy_debt` (line 1708) lets a `DEBT_PURCHASER` role buy a strategy's bad debt
from the vault: the purchaser sends `amount` of `asset` to the vault and receives the
proportional slice of the vault's strategy shares in return:

```vyper
# VaultV3.vy:1735
shares: uint256 = IStrategy(strategy).balanceOf(self) * _amount / current_debt
```

The comment explicitly states the design intent: *"We assume this is being used due to
strategy issues so won't rely on its conversion rates."* The proportional formula is
used **instead of** the value-based formula
(`shares = _convert_to_shares_for_strategy(_amount)`), precisely so that the vault does
not depend on the strategy's `convertToAssets` during an emergency.

The side-effect of this choice is that the payout is **not value-neutral**: the
purchaser's received shares are worth
`shares * strategy_pps` where `strategy_pps = convertToAssets(balanceOf(self)) /
balanceOf(self)`. The vault paid `_amount` of `asset` (line 1739) but the shares it
hands over (line 1753) are worth `_amount * (strategy_total_worth / current_debt)`:

- **If the strategy is in profit** (`strategy_total_worth > current_debt`, i.e.
  unrealised profit not yet reported via `process_report`), the purchaser receives
  shares worth **more** than `_amount` — extracting the strategy's unrealised profit
  that would otherwise have accrued to all vault share-holders via PPS unlock.
- **If the strategy is in loss**, the purchaser receives shares worth **less** than
  `_amount` — they eat the loss, which is the intended use-case.

Because `process_report` is the only mechanism that crystallises strategy profit into
the vault's `total_debt` (and hence into PPS), a `DEBT_PURCHASER` calling `buy_debt`
*before* a pending profit is reported can capture that profit privately, bypassing the
`profit_max_unlock_time` distribution curve that ordinary depositors are subject to.

This is not a theft by an arbitrary attacker (the role is governance-assigned), but it
is a value-leak vector that a privileged role can trigger, and it is not documented as
an accepted risk in the function's NatSpec.

---

## 2. Contract / function / line

| File | Function | Line(s) |
|------|----------|---------|
| `contracts/VaultV3.vy` | `buy_debt` | 1708-1755 |
| `contracts/VaultV3.vy` | proportional share formula | 1735 |
| `contracts/VaultV3.vy` | asset pull-in | 1739 |
| `contracts/VaultV3.vy` | strategy share transfer-out | 1753 |
| `contracts/VaultV3.vy` | role gate | 1719 (`Roles.DEBT_PURCHASER`) |

---

## 3. Attack scenario

1. Strategy S holds the vault's deposit plus accrued yield. The vault's recorded
   `current_debt` for S is 1,000,000 USDC, but S's shares are actually worth
   1,050,000 USDC (5% unrealised profit, not yet reported).
2. A `DEBT_PURCHASER` (or a governance proposal that the purchaser influences) calls
   `buy_debt(S, 1_000_000)` *before* anyone calls `process_report(S)`.
3. The purchaser sends 1,000,000 USDC to the vault and receives
   `balanceOf(self) * 1_000_000 / 1_000_000 = 100%` of the vault's S shares — which
   are redeemable for 1,050,000 USDC.
4. The 50,000 USDC of unrealised profit that should have been distributed to all vault
   depositors over `profit_max_unlock_time` is instead captured in full, atomically,
   by the purchaser.
5. Net: the purchaser converts 1,000,000 USDC into 1,050,000 USDC of strategy shares,
   a 5% instant gain, while ordinary vault depositors see no PPS increase from S's
   profit (the debt was "bought" at book value before the profit was crystallised).

The same mechanic in reverse (buying debt of a lossy strategy before a loss report)
lets the purchaser *avoid* a loss that would otherwise have hit PPS — i.e. socialise
the loss onto remaining depositors.

---

## 4. Proof of Concept (Foundry, sketch)

```solidity
// SPDX-License-Identifier: MIT
pragma solidity 0.8.19;
import "forge-std/Test.sol";
// imports: VaultV3 (Vyper via compiled artefact), MockStrategy (ERC4626), MockAsset

contract YearnBuyDebtProfitExtractionTest is Test {
    VaultV3 vault;
    MockStrategy strategy;     // ERC4626, asset = USDC
    MockERC20 usdc;
    address debtPurchaser;

    function setUp() public {
        // deploy vault, add strategy, deposit 1_000_000 USDC via update_debt
        // strategy accrues 5% yield (manipulate its convertToAssets to return 1.05e6)
        // DO NOT call process_report yet
    }

    function test_buy_debt_captures_unrealised_profit() public {
        uint256 stratWorthBefore = strategy.convertToAssets(strategy.balanceOf(address(vault)));
        assertGt(stratWorthBefore, vault.strategies(address(strategy)).current_debt()); // profit unreported

        uint256 debtToBuy = vault.strategies(address(strategy)).current_debt();
        deal(address(usdc), debtPurchaser, debtToBuy);

        vm.startPrank(debtPurchaser);
        usdc.approve(address(vault), debtToBuy);
        vault.buy_debt(address(strategy), debtToBuy);
        uint256 sharesReceived = strategy.balanceOf(debtPurchaser);
        uint256 redeemable = strategy.convertToAssets(sharesReceived);
        vm.stopPrank();

        // Purchaser paid `debtToBuy` USDC but received shares worth > debtToBuy
        assertGt(redeemable, debtToBuy, "DEBT_PURCHASER extracted unrealised profit");
        assertEq(strategy.balanceOf(address(vault)), 0, "vault holds no more strategy shares");

        // After a subsequent process_report, vault PPS does NOT rise — profit was stolen
        // by the purchaser rather than crystallised into the vault.
    }
}
```

Run: `forge test --match-test test_buy_debt_captures_unrealised_profit -vv`.

---

## 5. Impact

* **Fund loss**: the vault's depositors lose the unrealised profit of the bought-out
  strategy (it accrues to the `DEBT_PURCHASER` instead of to PPS). Magnitude =
  `(strategy_worth − current_debt) × (amount_bought / current_debt)`.
* **Scope**: requires the `DEBT_PURCHASER` role, which is governance-assigned, so this
  is a privileged value-extraction rather than an external attack. Severity is
  therefore Low (governance trust assumption), but it is a real economic leak that the
  NatSpec does not disclose and that is avoidable with a one-line fix.
* **Bounty alignment**: Yearn's bounty targets direct theft/freezing of user funds;
  this is a role-gated profit-redirection, so it sits at the Low boundary. It is
  reported because the function's "emergency only" framing understates the
  profit-extraction side-effect.

---

## 6. Three-perspective audit

**Exploitability**
Requires `DEBT_PURCHASER`. Not reachable by a random caller. The role is meant for
emergencies, so the realistic concern is a purchaser who is also a large depositor
using `buy_debt` to privatise yield ahead of `process_report`, or a governance
proposal front-running a profit report.

**Economic impact**
Equal to the unreported strategy profit at the time of the buy. For a vault with
multiple strategies and frequent reporting, the window is small; for a vault with
infrequent reports and a profitable strategy, the leak can be significant. No
insolvency is caused — the vault's books stay consistent (`current_debt` and
`total_debt` are reduced in lockstep with the asset received) — but depositor PPS is
lower than it would have been.

**Fix recommendation**
Force a `process_report(strategy)` immediately before computing the share payout in
`buy_debt`, so that any unrealised profit/loss is crystallised into `current_debt`
first; the proportional formula then operates on a debt figure that already reflects
the strategy's true worth:

```diff
 function buy_debt(strategy: address, amount: uint256):
     self._enforce_role(msg.sender, Roles.DEBT_PURCHASER)
     assert self.strategies[strategy].activation != 0, "not active"
+    # Crystallise any pending P&L so the proportional payout is value-neutral.
+    self._process_report(strategy)
     current_debt: uint256 = self.strategies[strategy].current_debt
     ...
```

Alternatively, document explicitly in the NatSpec that `buy_debt` may transfer
unrealised strategy P&L to the `DEBT_PURCHASER` and that `process_report` should be
called first if value-neutrality is desired.

---

## 7. References

* `buy_debt`: `contracts/VaultV3.vy:1708-1755`
* `_process_report` (the profit-crystallising function `buy_debt` skips):
  `contracts/VaultV3.vy:1149-1360`
* `Roles.DEBT_PURCHASER` enforcement: `contracts/VaultV3.vy:1719`
* Non-reentrancy guard on `buy_debt`: `contracts/VaultV3.vy:1707`
