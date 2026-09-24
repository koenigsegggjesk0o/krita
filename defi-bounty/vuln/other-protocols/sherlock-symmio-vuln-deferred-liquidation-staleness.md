# SYMMIO — Deferred Liquidation Signature Missing Staleness Check

**Protocol:** SYMMIO (Symmetry)
**Bounty:** $808,808 USDC
**Sherlock Bounty URL:** https://audits.sherlock.xyz/bug-bounties/5
**Source:** https://github.com/SYMM-IO/protocol-core
**Severity:** HIGH
**Area:** Signature verification / Oracle manipulation (stale price reuse)
**Status:** NOT SUBMITTED — local audit finding only.

---

## 1. Description

`DeferredLiquidationFacetImpl.deferredLiquidatePartyA` and
`DeferredLiquidationFacetImpl.deferredSetSymbolsPrice` verify the supplied
`DeferredLiquidationSig` via `LibMuonLiquidation.verifyDeferredLiquidationSig`,
but — unlike every other liquidation / settlement / force-action entry point —
**that verifier performs no freshness (`upnlValidTime`) check** on the
muon-signed timestamp. As a result, a deferred-liquidation signature that was
valid at block `T1` can be replayed at any later block `T2` (days, weeks, or
months later) to liquidate a party using *stale* prices and a *stale* uPNL, as
long as the per-partyA nonce has not been bumped in the meantime.

The nonce (`partyANonces[partyA]`) is only incremented **after** the full
settlement (`LiquidationFacetImpl.settlePartyALiquidation`, line 312). So if a
deferred-liquidation signature is obtained but never followed through to
settlement, it remains replayable forever against the same partyA.

### Contrast with the non-deferred path

`LiquidationFacetImpl.liquidatePartyA` (the regular, non-deferred path) does
check staleness explicitly:

```solidity
// contracts/facets/liquidation/LiquidationFacetImpl.sol:26-27
LibMuonLiquidation.verifyLiquidationSig(liquidationSig, partyA);
require(block.timestamp <= liquidationSig.timestamp + MuonStorage.layout().upnlValidTime,
        "LiquidationFacet: Expired signature");
```

The deferred path omits the second line entirely:

```solidity
// contracts/facets/liquidation/DeferredLiquidationFacetImpl.sol:22-33
function deferredLiquidatePartyA(address partyA, DeferredLiquidationSig memory liquidationSig) internal {
    MAStorage.Layout storage maLayout = MAStorage.layout();
    AccountStorage.Layout storage accountLayout = AccountStorage.layout();

    LibMuonLiquidation.verifyDeferredLiquidationSig(liquidationSig, partyA);
    // <-- NO timestamp / upnlValidTime check here

    int256 liquidationAvailableBalance = LibAccount.partyAAvailableBalanceForLiquidation(
        liquidationSig.upnl,
        liquidationSig.liquidationAllocatedBalance,
        partyA
    );
    require(liquidationAvailableBalance < 0, "LiquidationFacet: PartyA is solvent");
    ...
}
```

And the verifier itself only checks array length and the muon TSS+gateway
signature, never the timestamp:

```solidity
// contracts/libraries/muon/LibMuonLiquidation.sol:39-63
function verifyDeferredLiquidationSig(DeferredLiquidationSig memory liquidationSig, address partyA) internal view {
    MuonStorage.Layout storage muonLayout = MuonStorage.layout();
    require(liquidationSig.prices.length == liquidationSig.symbolIds.length, "LibMuon: Invalid length");
    bytes32 hash = keccak256(
        abi.encodePacked(
            muonLayout.muonAppId,
            liquidationSig.reqId,
            liquidationSig.liquidationId,
            address(this),
            "verifyDeferredLiquidationSig",
            partyA,
            AccountStorage.layout().partyANonces[partyA],
            liquidationSig.upnl,
            liquidationSig.totalUnrealizedLoss,
            liquidationSig.symbolIds,
            liquidationSig.prices,
            liquidationSig.timestamp,                // <-- included in hash for binding, but NOT compared to block.timestamp
            liquidationSig.liquidationBlockNumber,
            liquidationSig.liquidationTimestamp,
            liquidationSig.liquidationAllocatedBalance,
            LibMuon.getChainId()
        )
    );
    LibMuon.verifyTSSAndGateway(hash, liquidationSig.sigs, liquidationSig.gatewaySignature);
}
```

Compare with `LibMuon.verifyPartyBUpnl` (lines 43), `LibMuonSettlement.verifySettlement` (line 15),
`LibMuonForceActions.verifyHighLowPrice` (line 15) — all of them gate on
`block.timestamp <= sig.timestamp + muonLayout.upnlValidTime`.

---

## 2. Contract, Function, and Lines

| Field | Value |
|---|---|
| Contract | `DeferredLiquidationFacetImpl` (library) |
| File | `contracts/facets/liquidation/DeferredLiquidationFacetImpl.sol` |
| Functions | `deferredLiquidatePartyA` (lines 22–60), `deferredSetSymbolsPrice` (lines 62–98) |
| Verifier | `LibMuonLiquidation.verifyDeferredLiquidationSig` — `contracts/libraries/muon/LibMuonLiquidation.sol:39-63` |
| External entry | `LiquidationFacet.deferredLiquidatePartyA` / `LiquidationFacet.deferredSetSymbolsPrice` — `contracts/facets/liquidation/LiquidationFacet.sol:62-93` |
| Nonce bump site | `LiquidationFacetImpl.settlePartyALiquidation` line 312 — only AFTER full settlement |

---

## 3. Attack Scenario

1. At time `T1`, partyA becomes insolvent (e.g. after a sharp market move). A
   liquidator with `LIQUIDATOR_ROLE` requests a deferred liquidation signature
   from the muon network. The signature binds:
   - `upnl(T1)`, `totalUnrealizedLoss(T1)`, `liquidationAllocatedBalance(T1)`
   - `symbolIds[]`, `prices[](T1)`
   - `liquidationBlockNumber(T1)`, `liquidationTimestamp(T1)`, `timestamp(T1)`
   - `partyANonces[partyA]` at `T1` (call it `N`)
2. The liquidator does **not** call `deferredLiquidatePartyA` at `T1`
   (changed their mind, gas too high, etc.).
3. Between `T1` and `T2`, partyA trades out of trouble, deposits more
   collateral, or the market moves in their favor. They are solvent at `T2`.
   Crucially, partyA was never liquidated, so `partyANonces[partyA]` is still
   `N`.
4. At `T2`, the same liquidator (or anyone they share the signature with —
   signatures are off-chain data) calls
   `LiquidationFacet.deferredLiquidatePartyA(partyA, staleSig)`.
5. `verifyDeferredLiquidationSig` succeeds — the hash still matches because the
   nonce is unchanged and the timestamp is only bound *inside* the hash, never
   compared to `block.timestamp`.
6. `deferredLiquidatePartyA` then uses `staleSig.upnl` and
   `staleSig.liquidationAllocatedBalance` to compute
   `liquidationAvailableBalance`. Since those values reflected insolvency at
   `T1`, the `require(liquidationAvailableBalance < 0)` check passes even
   though partyA is solvent at `T2`.
7. The liquidator follows up with `deferredSetSymbolsPrice(partyA, staleSig)`
   which writes `staleSig.prices[]` into `accountLayout.symbolsPrices[partyA]`
   — so the subsequent `liquidatePositionsPartyA` closes partyA's open
   positions at the **stale T1 prices**, not the current T2 prices.

### Economic impact

The attacker can choose the historical moment `T1` that is most favorable to
them (i.e. the prices that maximize the PnL extracted from partyA's positions
when realized). In effect, the liquidator gets a free American-style option
over the liquidation time, exercisable at any future point — even after partyA
has recovered.

If prices moved *against* partyA between `T1` and `T2`, the liquidator can
still realize the smaller `T1` loss and pocket the difference vs. the larger
`T2` insolvency. If prices moved *in favor* of partyA, the liquidator can
liquidate using the `T1` prices anyway, forcing partyA's positions to be
settled at the less-favorable `T1` mark.

---

## 4. Proof of Concept (Forge-style pseudocode)

```solidity
// SPDX-License-Identifier: UNLICENSED
pragma solidity ^0.8.18;

import "forge-std/Test.sol";
import {LiquidationFacet} from "contracts/facets/liquidation/LiquidationFacet.sol";
import {DeferredLiquidationSig} from "contracts/storages/MuonStorage.sol";

contract SymmioDeferredStalenessPoC is Test {
    LiquidationFacet facet;
    address partyA = address(0xA11CE);
    address liquidator = address(0xB0B);

    // captured once at T1, then replayed at T2 >> T1 + upnlValidTime
    DeferredLiquidationSig staleSig;

    function setUp() public {
        facet = LiquidationFacet(address(SYMMIO_DIAMOND));
        // grant liquidator role, set up partyA with an open position, fast-forward
        // to a point where partyA is insolvent, obtain staleSig from muon.
        vm.warp(T1);
        staleSig = _obtainDeferredLiquidationSig(partyA); // valid at T1
    }

    function testReplayStaleDeferredSig() public {
        // Fast-forward far beyond upnlValidTime.
        vm.warp(T1 + 30 days);

        // partyA has since recovered and is solvent at T2.
        assertGt(_currentAvailableBalance(partyA), 0);

        // The regular path would revert:
        // vm.expectRevert("LiquidationFacet: Expired signature");
        // facet.liquidatePartyA(partyA, _asRegularSig(staleSig));

        // The deferred path does NOT revert — signature is accepted.
        vm.prank(liquidator);
        facet.deferredLiquidatePartyA(partyA, staleSig); // <-- passes

        // Prices are frozen at T1 values, positions closed at T1 marks.
        vm.prank(liquidator);
        facet.deferredSetSymbolsPrice(partyA, staleSig);

        // partyA's open positions are liquidated using stale prices.
        uint256[] memory quoteIds = _openQuotesOf(partyA);
        vm.prank(liquidator);
        facet.liquidatePositionsPartyA(partyA, quoteIds);
    }
}
```

A complete runnable PoC would require standing up the full Symmio diamond, a
mock muon gateway, and a chainlink-style price feed; the snippet above shows
the temporal-replay invariant and the divergent behavior between the two
liquidation entry points.

---

## 5. Impact

- **Fund loss for partyA:** partyA can be liquidated using stale prices long
  after they have recovered, realizing PnL at outdated marks. Depending on
  market movement between `T1` and `T2`, partyA's realized loss can be
  substantially larger than it would be at current prices — the surplus
  collateral that partyA posted after `T1` is captured by the liquidator and
  the counterparty PartyBs.
- **Counterparty risk for PartyBs:** PartyBs on the other side of partyA's
  positions may receive settlement at stale prices that do not reflect their
  actual credit exposure, creating accounting discrepancies in
  `settlementStates`.
- **Bypass of the regular-path staleness gate:** The non-deferred path is
  explicitly protected by `upnlValidTime`; the deferred path silently bypasses
  that protection, undermining the protocol-wide invariant that all
  muon-signed prices used for liquidation are fresh.
- **Permissioned but not privileged:** `LIQUIDATOR_ROLE` is broadly granted to
  market makers and external liquidator bots — any of them can warehouse a
  valid deferred signature and exercise it opportunistically.

---

## 6. Severity: **HIGH**

Rationale:
- Direct loss of user funds (partyA's collateral) at the scale of an entire
  position, not just dust.
- No privileged role required beyond `LIQUIDATOR_ROLE`, which is operationally
  distributed.
- Triggers only when (a) partyA was briefly insolvent, (b) a deferred sig was
  issued but not consumed, (c) partyA recovered — a realistic operational
  pattern during volatile markets.
- Not a "theoretical" finding: the missing line is a straightforward omission
  vs. the sibling function in the same file, and the bypass is exact.

Not Critical because: the attack requires a previously-valid insolvency
condition (partyA must have been genuinely liquidatable at `T1`), so it does
not allow liquidating a party that was *never* insolvent.

---

## 7. Three-Perspective Audit

### 7.1 Protocol / Business-logic perspective
The deferred liquidation flow was clearly added later than the regular flow
(the struct carries extra fields `liquidationBlockNumber`,
`liquidationTimestamp`, `liquidationAllocatedBalance` that the regular
`LiquidationSig` lacks). The deferred path exists to handle the case where a
liquidator could not act in real-time (chain congestion, missing muon
signature) and must "prove" that partyA was insolvent at a past block.

That use case is legitimate, but it does **not** require unbounded replay.
The deferred signature already encodes the historical `liquidationTimestamp`
and `liquidationBlockNumber`; freshness should be enforced relative to the
*current* block (i.e. "you may only defer by up to `X` hours"), not dropped
entirely. A reasonable policy: `require(block.timestamp <=
liquidationSig.liquidationTimestamp + deferredLiquidationWindow)` for some
governance-set `deferredLiquidationWindow` (e.g. 1–6 hours).

### 7.2 Cryptographic / Signature perspective
The muon TSS signature is **correct** — it binds all relevant fields including
`timestamp`, `liquidationBlockNumber`, `liquidationTimestamp`, nonce, prices,
and uPNL. Nothing about the signature itself is malleable or forgeable. The
defect is purely in the **consumption policy**: the verifier checks *what* was
signed but not *when it may be consumed*. This is a textbook
"signature-valid ≠ action-valid" gap.

Notably, `verifyPartyBUpnl` (LibMuon.sol:43) and every other verifier in the
codebase enforce `block.timestamp <= sig.timestamp + upnlValidTime`. Only
`verifyDeferredLiquidationSig` and its sibling `verifyLiquidationSig` omit
it — and the latter is rescued by a separate `require` in
`LiquidationFacetImpl.liquidatePartyA`. The deferred sibling has no such
rescue line.

### 7.3 Operational / Threat-model perspective
In production, muon signatures are requested by liquidator bots via an
off-chain muon gateway. Failed liquidation attempts are routine (gas
optimization, mempool competition, re-orgs). Each such attempt leaves a
valid, unexpired-by-nonce signature sitting in the bot's logs.

The threat model should assume that any signature ever issued will eventually
be public (bots get compromised, logs leak, sigs are shared with
collaborators). Without a freshness gate, **every past insolvency event
becomes a permanent option against the user**. For a derivatives protocol
with $808K on the table, this is unacceptable.

Mitigation should be layered:
1. Add `require(block.timestamp <= liquidationSig.timestamp +
   muonLayout.upnlValidTime)` (or a dedicated `deferredLiquidationValidTime`)
   to `verifyDeferredLiquidationSig` or to
   `DeferredLiquidationFacetImpl.deferredLiquidatePartyA`.
2. Bump `partyANonces[partyA]` *immediately* upon a successful
   `deferredLiquidatePartyA` (not only after full settlement), so that the
   same signature cannot be re-used even within the validity window.
3. Optionally: require `liquidationSig.liquidationTimestamp >=
   block.timestamp - maxDeferral` to bound how far back the deferred
   insolvency can be.

---

## 8. Suggested Fix

```diff
// contracts/facets/liquidation/DeferredLiquidationFacetImpl.sol
 function deferredLiquidatePartyA(address partyA, DeferredLiquidationSig memory liquidationSig) internal {
     MAStorage.Layout storage maLayout = MAStorage.layout();
     AccountStorage.Layout storage accountLayout = AccountStorage.layout();

     LibMuonLiquidation.verifyDeferredLiquidationSig(liquidationSig, partyA);
+    require(
+        block.timestamp <= liquidationSig.timestamp + MuonStorage.layout().upnlValidTime,
+        "LiquidationFacet: Expired deferred signature"
+    );
     int256 liquidationAvailableBalance = LibAccount.partyAAvailableBalanceForLiquidation(
         liquidationSig.upnl,
         liquidationSig.liquidationAllocatedBalance,
         partyA
     );
     require(liquidationAvailableBalance < 0, "LiquidationFacet: PartyA is solvent");
     ...
 }

 function deferredSetSymbolsPrice(address partyA, DeferredLiquidationSig memory liquidationSig) internal {
     MAStorage.Layout storage maLayout = MAStorage.layout();
     AccountStorage.Layout storage accountLayout = AccountStorage.layout();

     LibMuonLiquidation.verifyDeferredLiquidationSig(liquidationSig, partyA);
+    require(
+        block.timestamp <= liquidationSig.timestamp + MuonStorage.layout().upnlValidTime,
+        "LiquidationFacet: Expired deferred signature"
+    );
     require(maLayout.liquidationStatus[partyA], "LiquidationFacet: PartyA is solvent");
     ...
 }
```

(Or, more DRY: add the check inside `verifyDeferredLiquidationSig` itself,
matching the pattern of `LibMuon.verifyPartyBUpnl`.)
