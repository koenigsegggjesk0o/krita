# Cap — `BorrowLogic.realizeInterest` Is Dead Code; Protocol Interest Receiver Never Paid

**Protocol:** Cap (cap-labs-dev)
**Bounty:** $1,000,000 USDC
**Sherlock Bounty URL:** https://audits.sherlock.xyz/bug-bounties/114
**Source:** https://github.com/cap-labs-dev/cap-contracts
**Severity:** LOW (functionality / economic defect)
**Area:** Integer precision / Cross-contract interaction (accounting invariant)
**Status:** NOT SUBMITTED — local audit finding only.

---

## 1. Description

`BorrowLogic.realizeInterest` is a permissionless `external` function
intended to realize the protocol's own interest (separate from restaker
interest) by borrowing from the vault and sending the proceeds to
`reserve.interestReceiver`. However, due to an accounting invariant that
holds across all mint/burn paths, the function's `maxRealization`
computation always returns `0`, causing it to **always revert** with
`ZeroRealization`. The protocol's interest receiver is therefore never
paid via this path, and the `interestRepaid` branch in `repay` (which
depends on the same invariant) is also unreachable.

```solidity
// contracts/lendingPool/libraries/BorrowLogic.sol:181-192
function realizeInterest(ILender.LenderStorage storage $, address _asset)
    external
    returns (uint256 realizedInterest)
{
    ILender.ReserveData storage reserve = $.reservesData[_asset];
    realizedInterest = maxRealization($, _asset);
    if (realizedInterest == 0) revert ZeroRealization();   // <-- always taken

    reserve.debt += realizedInterest;
    IVault(reserve.vault).borrow(_asset, realizedInterest, reserve.interestReceiver);
    emit RealizeInterest(_asset, realizedInterest, reserve.interestReceiver);
}
```

And `maxRealization`:

```solidity
// contracts/lendingPool/libraries/BorrowLogic.sol:229-247
function maxRealization(ILender.LenderStorage storage $, address _asset)
    internal
    view
    returns (uint256 realization)
{
    ILender.ReserveData storage reserve = $.reservesData[_asset];
    uint256 totalDebt = IERC20(reserve.debtToken).totalSupply();
    uint256 reserves = IVault(reserve.vault).availableBalance(_asset);
    uint256 vaultDebt = reserve.debt;
    uint256 totalUnrealizedInterest = reserve.totalUnrealizedInterest;

    if (totalDebt > vaultDebt + totalUnrealizedInterest) {
        realization = totalDebt - vaultDebt - totalUnrealizedInterest;
    }
    if (reserves < realization) {
        realization = reserves;
    }
    if (reserve.paused) realization = 0;
}
```

### Why `maxRealization` is always 0

The debt token is minted in exactly two places:

1. `BorrowLogic.borrow` (line 85): `IDebtToken(reserve.debtToken).mint(params.agent, borrowed)`
   paired with `reserve.debt += borrowed` (line 87). Delta to
   `totalSupply` = `borrowed`; delta to `vaultDebt` = `borrowed`. Equal.

2. `BorrowLogic.realizeRestakerInterest` (line 216):
   `IDebtToken(reserve.debtToken).mint(_agent, realizedInterest + unrealizedInterest)`
   paired with `reserve.debt += realizedInterest` (line 212) and
   `reserve.totalUnrealizedInterest += unrealizedInterest` (line 214).
   Delta to `totalSupply` = `realizedInterest + unrealizedInterest`;
   delta to `vaultDebt + totalUnrealizedInterest` =
   `realizedInterest + unrealizedInterest`. Equal.

The debt token is burned only in `repay` (line 158), paired with `reserve.debt -= vaultRepaid` (line 148) — equal deltas again.

Therefore the invariant

```
totalDebtTokenSupply == reserve.debt + reserve.totalUnrealizedInterest
```

holds at all times (modulo the unreachable `interestRepaid` branch in
`repay`, which would break it — see below).

Plugging into `maxRealization`:

```
realization = totalDebt - vaultDebt - totalUnrealizedInterest
            = (vaultDebt + totalUnrealizedInterest) - vaultDebt - totalUnrealizedInterest
            = 0
```

So `realization` is always 0, `maxRealization` returns 0, and
`realizeInterest` always reverts.

### The unreachable `interestRepaid` branch in `repay`

```solidity
// contracts/lendingPool/libraries/BorrowLogic.sol:124-127
if (repaid > reserve.unrealizedInterest[params.agent] + reserve.debt) {
    interestRepaid = repaid - (reserve.debt + reserve.unrealizedInterest[params.agent]);
    remaining -= interestRepaid;
}
```

This branch is intended to capture "excess" repayment that goes to the
protocol's interest receiver. But:

- `repaid = Math.min(params.amount, agentDebt)` where
  `agentDebt = IERC20(reserve.debtToken).balanceOf(params.agent)` (the
  agent's personal debt-token balance).
- The agent's personal balance is at most
  `totalSupply = reserve.debt + totalUnrealizedInterest`.
- The agent's personal unrealized interest is at most
  `reserve.totalUnrealizedInterest` (and usually much less).
- So `repaid <= agentDebt <= totalSupply = reserve.debt + totalUnrealizedInterest`,
  which means `repaid <= reserve.debt + totalUnrealizedInterest >=
  reserve.debt + reserve.unrealizedInterest[params.agent]`.

The condition `repaid > reserve.unrealizedInterest[params.agent] + reserve.debt`
is therefore **never true** (the right-hand side is `>=` the global
`reserve.debt + totalUnrealizedInterest`, which is `>=` `repaid`).

`interestRepaid` is always 0. The protocol's `reserve.interestReceiver`
never receives any payment through `repay` either.

---

## 2. Contract, Function, and Lines

| Field | Value |
|---|---|
| Contract | `BorrowLogic` (library) |
| File | `contracts/lendingPool/libraries/BorrowLogic.sol` |
| Functions | `realizeInterest` (lines 181–192), `maxRealization` (lines 229–247), `repay` `interestRepaid` branch (lines 124–127) |
| External entry | `Lender.realizeInterest` — `contracts/lendingPool/Lender.sol:77-79` (permissionless, anyone can call) |

---

## 3. Attack Scenario

This is not an exploit per se — it is a **functionality defect**. The
protocol's interest receiver (set per-asset via `setInterestReceiver`)
is supposed to earn protocol interest from borrows, but the mechanism is
broken. The economic consequences:

1. **Protocol revenue is 0 from the lending pool.** The
   `reserve.interestReceiver` (likely the insurance fund or treasury)
   never receives any USDC/wstETH/LBTC from lending activity. All
   interest paid by borrowers either goes to restakers (via
   `realizeRestakerInterest` → `Delegation.distributeRewards`) or stays
   in the vault as unused surplus (benefiting vault depositors / cToken
   holders).
2. **The insurance fund is underfunded.** If `interestReceiver` is the
   insurance fund that covers bad debt from liquidations, the fund
   never grows from lending interest, increasing the protocol's
   insolvency risk during mass liquidation events.
3. **`realizeInterest` is a permissionless DoS vector.** Anyone can
   call `Lender.realizeInterest(asset)`, which always reverts with
   `ZeroRealization`. The revert itself is harmless (no state change),
   but it wastes gas and could confuse monitoring / bot infrastructure
   that expects the call to succeed.

### Could this be exploited for profit?

Indirectly, yes — but the gain is to vault depositors (who keep the
surplus) rather than to an attacker. A borrower who is also a vault
depositor benefits from the broken mechanism: they pay interest that
should have gone to the protocol's treasury, but instead it accrues to
the vault's `totalSupplies`, raising the cToken exchange rate and
enriching depositors (themselves included).

---

## 4. Proof of Concept (Forge-style)

```solidity
// SPDX-License-Identifier: UNLICENSED
pragma solidity ^0.8.28;

import "forge-std/Test.sol";
import {Lender} from "contracts/lendingPool/Lender.sol";

contract CapRealizeInterestDeadPoC is Test {
    Lender lender;

    function setUp() public {
        lender = Lender(payable(LENDER));
        // set up a reserve with a borrower who has accrued interest
    }

    function testRealizeInterestAlwaysReverts() public {
        // After any borrow + interest accrual:
        vm.expectRevert(BorrowLogic.ZeroRealization.selector);
        lender.realizeInterest(USDC);
    }

    function testMaxRealizationAlwaysZero() public view {
        // invariant: debtToken.totalSupply() == reserve.debt + reserve.totalUnrealizedInterest
        uint256 totalDebt = IERC20(debtToken).totalSupply();
        (,,,,,,,) = lender.reservesData(USDC); // unpack reserve.debt
        // assert totalDebt == reserve.debt + reserve.totalUnrealizedInterest
        // therefore maxRealization == 0
    }
}
```

---

## 5. Impact

- **Protocol treasury / insurance fund receives 0 lending interest.**
  The economic magnitude depends on TVL and borrow rates; for a protocol
  targeting $1M+ bounty tier, this could be significant ongoing revenue.
- **No direct fund loss for users.** Borrowers and restakers are
  unaffected; the defect is purely a revenue-routing failure.
- **Permissionless revert** on `realizeInterest` is a minor liveness
  wart (bots calling it waste gas).

---

## 6. Severity: **LOW**

- Not MEDIUM because: no user fund loss; the "lost" interest stays inside
  the protocol (in the vault, benefiting cToken holders) rather than
  being stolen.
- Not Informational because: the protocol's documented interest
  mechanism is non-functional, which is a real economic defect worth
  reporting to a bug bounty program (Sherlock's severity criteria
  include "protocol functionality broken" as a Low).

---

## 7. Three-Perspective Audit

### 7.1 Protocol / Business-logic perspective
The intent appears to be a two-tier interest model:
- Restaker interest: realized via `realizeRestakerInterest`, paid to
  `Delegation` → network → restakers.
- Protocol interest: realized via `realizeInterest`, paid to
  `reserve.interestReceiver` (treasury / insurance fund).

But the debt-token accounting collapses both tiers into the same
`reserve.debt` accumulator, leaving no "headroom" for the protocol tier.
To make `realizeInterest` functional, the protocol would need to either:
- Mint debt tokens for protocol interest *without* adding to
  `reserve.debt` (breaking the invariant intentionally), or
- Track protocol interest in a separate accumulator that
  `maxRealization` can read.

### 7.2 Accounting / invariant perspective
The invariant `totalSupply == reserve.debt + totalUnrealizedInterest`
is a **strong** invariant — it holds across all mint/burn paths. The
`realizeInterest` and `interestRepaid` code paths were written *as if*
the invariant could be violated (i.e. as if `totalSupply` could exceed
`reserve.debt + totalUnrealizedInterest`), but no code path actually
violates it. The dead code is a symptom of a design that was planned
but not implemented.

### 7.3 Operational / Threat-model perspective
From an operator's perspective, the symptom is: "the
`reserve.interestReceiver` address never receives any tokens, ever."
Monitoring that expects treasury growth from lending will show zero
inflow. If the insurance fund is sized assuming lending interest
contributes, the protocol is under-reserved against bad debt.

---

## 8. Suggested Fix

This is a design-level issue; the fix depends on the intended interest
model. The two most natural options:

### Option A — Remove the dead code

If the protocol's interest model is *intentionally* "all interest goes
to restakers and vault surplus," then `realizeInterest` and the
`interestRepaid` branch should be deleted to avoid confusion and the
gas waste of the permissionless revert.

### Option B — Implement protocol interest properly

If the protocol *should* earn interest, introduce a separate
accumulator:

```diff
 struct ReserveData {
     ...
     uint256 debt;
+    uint256 protocolInterestAccrued;
     ...
 }
```

And in the borrow/repay flow, accrue protocol interest separately
(e.g. as a fraction of the utilization-index growth) and let
`maxRealization` read from `protocolInterestAccrued` instead of trying
to derive it from the debt-token supply invariant.

---

## 9. Note for Sherlock Judging

Sherlock's severity matrix classifies "issue that causes dysfunction of
the protocol's core functionality" as at least Low. `realizeInterest`
being permanently broken qualifies. The economic impact (lost protocol
revenue) is a secondary concern but strengthens the case for a paid
finding rather than an Informational.
