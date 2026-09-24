# Beanstalk — BDV Preservation in `ConvertFacet.convert` Enables Seigniorage Inflation

**Program:** Beanstalk (https://immunefi.com/bug-bounty/beanstalk/information/)
**KYC Status:** Not Required
**Max Bounty:** $1,100,000
**Severity:** Medium
**Area:** Integer Precision / Cross-Contract
**Date:** 2026-09-24

---

## Description

`ConvertFacet.convert()` chooses the BDV of the *output* deposit as
`max(newBdv, fromBdv)` (unless the `decreaseBDV` flag is explicitly set
by an `ANTI_LAMBDA_LAMBDA` convert). This means that whenever a user
converts a deposit into a different token and the new BDV (computed from
the current Well reserves) is lower than the old BDV, the **old, higher
BDV is silently preserved** on the new deposit.

Because Stalk (and therefore seigniorage entitlement) is minted
proportional to BDV, a user can chain converts to permanently maintain
an inflated BDV on a deposit whose actual underlying token value is
lower. Each season the excess BDV captures seigniorage that should have
been distributed pro-rata to all other Stalk holders.

The issue compounds with the Well-LP BDV oracle finding (see
`immunefi-nokyc-beanstalk-vuln-oracle-manipulation.md`): an attacker
who inflates BDV via a flash-loan-manipulated Well reserve *and* chains
that into a lossy convert can lock in the inflated BDV permanently,
even after the flash loan is repaid and the Well reserves normalise.

## Contract + Function + Line

**Contract:** `protocol/contracts/beanstalk/silo/ConvertFacet.sol`
**Function:** `convert(bytes,int96[],uint256[])`
**Lines:** 112–116

```solidity
// Calculate the bdv of the new deposit.
uint256 newBdv = LibTokenSilo.beanDenominatedValue(cp.toToken, cp.toAmount);

// If `decreaseBDV` flag is not enabled, set toBDV to the max of the two bdvs.
toBdv = (newBdv > fromBdv || cp.decreaseBDV) ? newBdv : fromBdv;
```

`fromBdv` is the BDV that was recorded on the *input* deposit at the
time it was originally created. `newBdv` is recomputed live from the
current Well reserves. When `newBdv < fromBdv`, the deposit retains the
stale (higher) `fromBdv` value, even though the user now holds a
different token whose live BDV is lower.

## Attack Scenario

1. Attacker deposits `100 000` Bean into the Silo. BDV = `100 000`.
   Stalk = `100 000 * stalkPerBean + 0`.
2. Beanstalk is above peg. Attacker calls `convert(BEANS_TO_WELL_LP)`
   to convert `100 000` Bean into Well LP. Due to price impact in the
   Well, the live BDV of the resulting LP is only `92 000` Bean. The
   convert logic preserves the original `100 000` BDV. New deposit:
   `amount = Z LP tokens, bdv = 100 000`.
3. Several seasons pass. The attacker's `100 000` BDV worth of Stalk
   earns seigniorage, even though the LP tokens are only worth `92 000`
   Bean.
4. Beanstalk flips below peg. Attacker calls `convert(WELL_LP_TO_BEANS)`
   to convert the LP back to Bean. The live BDV of the resulting Bean
   is `95 000` (the LP is worth a bit more now due to peg flip). The
   convert logic again preserves the `100 000` BDV. New deposit:
   `amount = 95 000 Bean, bdv = 100 000`.
5. The attacker now holds `95 000` Bean with BDV `100 000`. The
   `5 000` "phantom BDV" earns seigniorage indefinitely until the
   attacker withdraws.

The attacker repeats this cycle across peg flips to compound the
discrepancy. Because `fundsSafu` only checks token *amounts* (not BDV),
the invariant never reverts.

## Proof of Concept (Foundry)

```solidity
// SPDX-License-Identifier: MIT
pragma solidity ^0.8.20;

import {Test} from "forge-std/Test.sol";

/// Reproduces the BDV-preservation math from ConvertFacet.convert.
contract BeanstalkBdvPreservationTest is Test {
    function _convertBdv(uint fromBdv, uint newBdv, bool decreaseBDV)
        internal pure returns (uint)
    {
        return (newBdv > fromBdv || decreaseBDV) ? newBdv : fromBdv;
    }

    function test_BdvPreservedAcrossLossyConverts() public {
        uint fromBdv = 100_000e6;

        // Step 2: BEANS_TO_WELL_LP, live BDV is 92_000 due to price impact
        uint afterStep2 = _convertBdv(fromBdv, 92_000e6, false);
        assertEq(afterStep2, 100_000e6, "BDV preserved after lossy Bean->LP");

        // Step 4: WELL_LP_TO_BEANS, live BDV is 95_000 due to peg flip
        uint afterStep4 = _convertBdv(afterStep2, 95_000e6, false);
        assertEq(afterStep4, 100_000e6, "BDV preserved after lossy LP->Bean");

        // The attacker now holds 95_000 Bean but is credited 100_000 BDV.
        // 5_000 phantom BDV earns seigniorage every season.
        assertGt(afterStep4, 95_000e6, "phantom BDV exists");
    }

    function test_ChainedConvertsInflateBdv() public {
        // Demonstrate that repeated cycles never decrease BDV.
        uint bdv = 100_000e6;
        for (uint i = 0; i < 10; i++) {
            // Each cycle: convert with a live BDV that is 5% lower
            bdv = _convertBdv(bdv, bdv * 95 / 100, false);
        }
        // After 10 cycles, BDV is still 100_000e6 even though the
        // underlying token value would be ~59_900e6.
        assertEq(bdv, 100_000e6);
    }
}
```

## Impact

Slow but cumulative drain of protocol seigniorage. Each cycle preserves
a BDV that is higher than the actual value of the deposit, and the
excess Stalk earns Beans during every `sunrise`. Over many seasons the
attacker extracts Beans that should have been distributed to honest
Stalk holders. The attack is low-risk and requires only:
- An initial Bean deposit (no flash loan needed in the simplest form).
- Waiting for natural peg flips between seasons (typically 1–2 per day
  on Beanstalk).

Combined with the Well-LP BDV oracle manipulation finding, the attacker
can amplify the initial BDV inflation by 10× or more before chaining
converts.

## Severity

**Medium** — direct but slow value extraction. Does not immediately
drain the protocol but compounds over time.

## Three-Perspective Audit

**1. Attacker perspective.** The convert rules (BEANS_TO_WELL_LP only
above peg, WELL_LP_TO_BEANS only below peg) mean each cycle requires a
peg flip, which occurs naturally. The attacker does not need to
manipulate the peg — they simply wait. Each cycle preserves BDV at the
max, so the attack is monotonic: BDV never decreases.

**2. Protocol team perspective.** The `max(newBdv, fromBdv)` rule was
introduced to encourage converts towards peg by not penalising users
who convert at a temporary BDV loss. The unintended consequence is
that BDV becomes a "high-water mark" that never decreases, decoupling
Stalk from actual deposited value. Possible fixes: (a) use `min(newBdv,
fromBdv)` (which would penalise legitimate converts); (b) re-evaluate
BDV at every `sunrise` against current reserves (gas-heavy); (c) apply
a "BDV decay" factor per season; or (d) only preserve BDV for converts
that are strictly towards peg (verified by deltaB direction) and use
`newBdv` otherwise.

**3. Auditor perspective.** This is a design-level economic bug rather
than a memory-safety issue. The pattern `max(new, old)` on a
value-that-earns-yield is a classic "high-water mark" trap. The
`fundsSafu` invariant is again insufficient because it checks amounts,
not BDV. A reviewer should flag any place where a yield-bearing
quantity is taken as `max(new, old)` without a corresponding mechanism
to decrease it when the underlying value decreases.
