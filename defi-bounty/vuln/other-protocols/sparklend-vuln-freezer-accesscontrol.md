# SparkLend — `SparkLendFreezerMom` asymmetric `rely`/`deny`: any ward can deny other wards incl. the automated freeze/pause keeper → DoS of the safety mechanism

**Repo / area:** `sparkdotfi/sparklend-freezer` (`SparkLendFreezerMom.sol`)
**Severity (auditor's assessment):** Low
**Date:** 2025
**Auditor:** Opus (independent, not submitted)

---

## 1. Description

`SparkLendFreezerMom` is the emergency pause/freeze switch for SparkLend markets. It
uses a MakerDAO-style `wards` pattern, but with an **asymmetric** permission model:

```solidity
// sparklend-freezer/src/SparkLendFreezerMom.sol
39  modifier onlyOwner { require(msg.sender == owner, "..."); _; }
44  modifier auth      { require(isAuthorized(msg.sender, msg.sig), "..."); _; }
...
64  function rely(address usr) external override onlyOwner { wards[usr] = 1; ... }   // owner-only
69  function deny(address usr) external override auth     { wards[usr] = 0; ... }   // any auth
...
112 function isAuthorized(address src, bytes4 sig) internal view returns (bool) {
113     if (src == address(this))              return true;
115     else if (src == owner || wards[src] == 1) return true;
117     else if (authority == address(0))      return false;
120     else return AuthorityLike(authority).canCall(src, address(this), sig);
121 }
```

`deny` is `auth` while `rely` is `onlyOwner`. `auth` is satisfied by `owner`, by **any
ward**, or by an `Authority`-approved caller. Consequently:

* Any single ward (typically the automated keeper / monitor bot) can `deny` **every
  other ward**, including the separate emergency-response keeper that is expected to
  call `freezeMarket`/`freezeAllMarkets`/`pauseAllMarkets` during an incident.
* A caller approved via the `Authority` for the `deny` signature can likewise remove
  wards.
* `owner` cannot be removed this way (`owner` is a separate storage slot, not a ward),
  so the system is not fully bricked — but if `owner` is a slow governance timelock, the
  freeze capability is effectively DoS'd until an `executive spell` re-`rely`s the
  keeper.

In the standard MakerDAO `wards` pattern both `rely` and `deny` are `auth` and wards
are mutually trusted peers; the ability for any ward to remove others is accepted
because wards are co-equal. Here `rely` was tightened to `onlyOwner` (so a ward cannot
*escalate* privileges — good), but `deny` was left `auth`, which creates an asymmetric
"can tear down but cannot rebuild" capability: a compromised ward can silently strip
the redundant keepers, leaving itself as the sole freeze-capable entity and then
refusing to act during the exact incident the freezer exists for.

## 2. Contract / function / line

| Item | Location |
|---|---|
| `rely` owner-only | `sparklend-freezer/src/SparkLendFreezerMom.sol:64` |
| `deny` auth (asymmetric) | `sparklend-freezer/src/SparkLendFreezerMom.sol:69` |
| `auth` resolves any ward / authority-approved caller | `SparkLendFreezerMom.sol:112-122` |
| Affected safety functions | `freezeAllMarkets` L78, `freezeMarket` L88, `pauseAllMarkets` L93, `pauseMarket` L103 |

## 3. Attack scenario

1. SparkLend operates ≥2 freezer keepers (redundancy for the safety switch).
2. Attacker compromises ONE keeper key (a ward).
3. Compromised keeper calls `deny` on every *other* ward. Because `deny` is `auth` and
   the compromised keeper is itself a ward, all calls succeed.
4. Attacker now controls the only live freeze/pause caller and simply does nothing. The
   moment a real incident occurs (oracle manipulation, bad debt, exploit in progress),
   the remaining (denied) keepers cannot fire `freezeMarket`/`pauseAllMarkets`. The
   safety switch is dark until an `owner` governance spell re-`rely`s a keeper.
5. Net effect: the incident response is delayed by the governance cycle (hours to
   days), magnifying any underlying exploit.

This is a DoS of a *safety mechanism*, not a direct fund loss. It is most damaging when
combined with another finding (e.g. the oracle-revert DoS or a hypothetical price
manipulation), where every minute the freezer is offline translates to bad debt.

## 4. PoC (Foundry)

```solidity
// SPDX-License-Identifier: AGPL-3.0-or-later
pragma solidity ^0.8.13;

import "forge-std/Test.sol";
import { SparkLendFreezerMom } from "src/SparkLendFreezerMom.sol";

contract FreezerDoSPoC is Test {
    SparkLendFreezerMom mom;
    address owner    = address(0xa11ce);
    address keeper1  = address(0xkee1);   // compromised
    address keeper2  = address(0xkee2);   // legitimate backup

    function setUp() public {
        vm.prank(owner);
        mom = new SparkLendFreezerMom(address(0xc0nf), address(0xc0f1));
        vm.startPrank(owner);
        mom.rely(keeper1);
        mom.rely(keeper2);
        vm.stopPrank();
    }

    function test_wardCanDenyAllOthers() public {
        // keeper1 is a ward and can deny keeper2 (and every other ward)
        vm.prank(keeper1);
        mom.deny(keeper2);
        assertEq(mom.wards(keeper2), 0);

        // keeper2 can no longer fire the safety switch
        vm.prank(keeper2);
        vm.expectRevert("SparkLendFreezerMom/not-authorized");
        mom.freezeAllMarkets(true);
    }
}
```

## 5. Impact

* DoS of SparkLend's emergency pause/freeze capability by a single compromised ward.
* No direct fund theft; impact is realised only in conjunction with another incident
  that the freezer would have contained. Delays incident response by the governance
  spell cycle.

## 6. Severity

**Low.** Requires a compromised privileged key (ward), the impact is indirect
(disabling a safety switch rather than stealing funds), and `owner` can restore
capability via a governance spell. Worth tightening as defence-in-depth but not a
reportable Critical/High.

## 7. Three-perspective audit

**Prosecutor:** The freezer is the *last line of defence* for SparkLend. Letting any
single ward unilaterally disarm every other freeze-capable keeper — while
asymmetrically forbidding them from re-arming — is a needless weakening. The
"redundant keepers" design is defeated the moment one key is taken. During an active
exploit, every block the freezer is dark is protocol bad debt.

**Defense:** (a) `deny` being `auth` is the *standard* MakerDAO ward pattern; the
asymmetry with `rely` is actually a *security improvement* (wards cannot escalate new
wards). (b) `owner` cannot be denied, so the freezer is never fully bricked — only the
automated path is, and `owner` is a governance timelock that can re-`rely` keepers in a
spell. (c) Compromising a ward already implies significant operational breach; the
freezer DoS is a second-order concern relative to whatever the compromised ward could
already do elsewhere. (d) `freezeAllMarkets`/`pauseAllMarkets` can also be invoked via
the `Authority` path or by `owner`, so there are multiple remaining callers.

**Judge:** Correct as a defence-in-depth observation, but the practical impact is low
(compromised-ward precondition, owner remains a backstop, multiple freeze paths).
**Low severity.** Suggested hardening: make `deny` `onlyOwner` as well (symmetric with
`rely`), or require `deny` to leave at least one ward, or route both through the
`Authority`. Not a payout-grade finding on its own.
