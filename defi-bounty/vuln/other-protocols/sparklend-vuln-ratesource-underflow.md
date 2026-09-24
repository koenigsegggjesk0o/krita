# SparkLend — `PotRateSource` / `SSRRateSource` underflow if DSR/SSR < 1e27 → `getAPR()` reverts → interest calc reverts → entire reserve (DAI / USDS) frozen

**Repo / area:** `sparkdotfi/sparklend-advanced` (`PotRateSource.sol`, `SSRRateSource.sol`)
**Severity (auditor's assessment):** Low–Medium
**Date:** 2025
**Auditor:** Opus (independent, not submitted)

---

## 1. Description

Both rate sources annualise a per-second ray rate with a **subtraction that underflows
if the base rate is below 1e27** (i.e. a "negative" rate):

```solidity
// sparklend-advanced/src/PotRateSource.sol
18  function getAPR() external override view returns (uint256) {
19      return (pot.dsr() - 1e27) * 365 days;     // underflow / Panic if pot.dsr() < 1e27
20  }

// sparklend-advanced/src/SSRRateSource.sol
18  function getAPR() external override view returns (uint256) {
19      return (susds.ssr() - 1e27) * 365 days;   // underflow / Panic if susds.ssr() < 1e27
20  }
```

`getAPR()` is consumed by `RateTargetBaseInterestRateStrategy._getBaseVariableBorrowRate()`
and `RateTargetKinkInterestRateStrategy._getVariableRateSlope1()`, which feed
`VariableBorrowInterestRateStrategy.calculateInterestRates()`. Every state-changing
Pool action on the affected reserve (supply / borrow / repay / withdraw / liquidate)
calls `updateInterestRates` → `calculateInterestRates`, so a reverting `getAPR()`
**freezes the entire reserve** (DAI when wired to `PotRateSource`, USDS when wired to
`SSRRateSource`).

The assumption "DSR/SSR ≥ 1e27 always" is an *implicit* invariant on an *external*
MakerDAO/Sky contract. There is no on-chain guarantee:
* Governance (`Pot.file("dsr", _)` / the sUSDS rate setter) can in principle set the
  rate to any value, including `< 1e27` (a negative nominal rate), e.g. as part of a
  GNO-style negative-rate policy or a migration/maintenance spell that briefly parks
  the rate at 0 (`0 < dsr < 1e27` is representable).
* Even a temporary misconfiguration or a future Sky upgrade that changes `dsr()`
  semantics would brick the DAI reserve on SparkLend with no on-chain guard.

`getAPR()` is a `view`, so the revert cannot be caught by the Pool; it propagates
straight into `calculateInterestRates`. There is no clamping to `>= 1e27` and no
`try/catch` (the strategy calls `RATE_SOURCE.getAPR()` directly).

## 2. Contract / function / line

| Item | Location |
|---|---|
| Underflow site (DAI/DSR) | `sparklend-advanced/src/PotRateSource.sol:19` |
| Underflow site (USDS/SSR) | `sparklend-advanced/src/SSRRateSource.sol:19` |
| Consumer (reverts propagate) | `sparklend-advanced/src/RateTargetBaseInterestRateStrategy.sol:52` and `RateTargetKinkInterestRateStrategy.sol:55` → `VariableBorrowInterestRateStrategy.calculateInterestRates` (`VariableBorrowInterestRateStrategy.sol:147-214`) |

## 3. Attack scenario

Not directly attacker-initiated. Trigger paths:

1. **Governance sets a sub-1e27 rate.** MakerDAO/Sky governance (or a future spell)
   sets `Pot.dsr` (or the sUSDS SSR) to a value `< 1e27` (e.g. a brief negative-rate
   experiment, or a maintenance spell that parks it low). The very next interest
   accrual on SparkLend's DAI/USDS reserve reverts. **Every** subsequent supply /
   borrow / repay / withdraw / liquidation on that reserve reverts. DAI and USDS are
   SparkLend's deepest markets, so this is a protocol-wide lending freeze.
2. **Rate-source migration.** A future spell repoints `PotRateSource.pot` or
   `SSRRateSource.susds` to a new contract whose rate function returns `< 1e27` by
   design (e.g. a real-yield / negative-rate product). Same freeze.

The same arithmetic also has a secondary, non-security issue: it uses **simple**
annualisation (`per_second * 365 days`) rather than compounding. At single-digit rates
the error is negligible, but the contract is labelled `APR`, so simple interest is
arguably correct by intent; this is noted only for completeness and is not a
vulnerability.

## 4. PoC (Foundry)

```solidity
// SPDX-License-Identifier: AGPL-3.0
pragma solidity ^0.8.0;

import "forge-std/Test.sol";
import { PotRateSource } from "src/PotRateSource.sol";
import { SSRRateSource } from "src/SSRRateSource.sol";

interface IPot    { function dsr() external view returns (uint256); }
interface ISUSDS  { function ssr() external view returns (uint256); }

contract MockPot is IPot    { uint256 public dsr; function set(uint256 d) external { dsr = d; } }
contract MockSusds is ISUSDS { uint256 public ssr; function set(uint256 s) external { ssr = s; } }

contract RateSourceUnderflowPoC is Test {
    function test_potUnderflow() public {
        MockPot pot = new MockPot();
        PotRateSource src = new PotRateSource(address(pot));
        pot.set(1e27);              // 0% APR -> returns 0, fine
        assertEq(src.getAPR(), 0);

        pot.set(0.999e27 + 1);      // dsr < 1e27 (negative nominal rate)
        vm.expectRevert();          // Panic(0x11) underflow
        src.getAPR();
    }

    function test_ssrUnderflow() public {
        MockSusds s = new MockSusds();
        SSRRateSource src = new SSRRateSource(address(s));
        s.set(1e27 - 1);            // ssr < 1e27
        vm.expectRevert();
        src.getAPR();
    }
}
```

Wiring `src` behind `RateTargetBaseInterestRateStrategy` and calling
`calculateInterestRates` reproduces the reserve-wide revert that freezes the Pool.

## 5. Impact

* Full freeze of the DAI reserve (via `PotRateSource`) and/or USDS reserve (via
  `SSRRateSource`) — no supply / borrow / repay / withdraw / liquidation possible for
  that reserve for as long as the external rate is `< 1e27`.
* Because DAI/USDS are SparkLend's core markets, a freeze cascades into every account
  that borrows or supplies them (which is essentially every active account).
* Indirect: liquidations of positions involving the frozen asset cannot run, so bad
  debt can accrue during the outage.

## 6. Severity

**Low–Medium.** Impact is high (reserve-wide freeze) but likelihood is low: it requires
MakerDAO/Sky governance to set a sub-1e27 nominal rate, which has never happened and is
a governance decision rather than an attacker action. This is a robustness/defence-in-
depth gap (trusting an external invariant without an on-chain clamp), not an exploitable
logic bug.

## 7. Three-perspective audit

**Prosecutor:** The contract hard-codes `1e27` as a floor it does not enforce. The
result of an underflow is a `Panic` that is un-catchable by the consuming strategy and
that freezes the deepest SparkLend market. Trusting an external contract's invariant
without a clamp on a path that can brick the whole reserve is a textbook
defence-in-depth failure. At the very least it should `max(dsr, 1e27)`.

**Defense:** (a) MakerDAO's DSR has never been below 1e27 in years of operation; it is
a deep governance invariant. (b) `0% DSR` (dsr == 1e27) does *not* trigger this — only
`dsr < 1e27` does, which is a deliberate, extraordinary governance choice that would
itself be the news, not a side-effect. (c) If governance ever did set negative rates,
freezing SparkLend's DAI market until a fix spell is a *reasonable* safety stall, and
Spark governance can immediately swap the rate source. (d) The same `(rate - 1e27)`
pattern is used by MakerDAO's own `pot`/`jug`/`dai` plumbing, so it is idiomatic.

**Judge:** Technically correct but low-likelihood and operationally contained. I assess
**Low–Medium**. Cheap to fix (clamp to `>= 1e27`, or `try/catch` like
`CappedFallbackRateSource`), and worth fixing as defence-in-depth, but not a
reportable Critical/High on its own. Pair it with the broader "no revert-resilience on
external rate/price calls" theme in the oracle-DoS finding.
