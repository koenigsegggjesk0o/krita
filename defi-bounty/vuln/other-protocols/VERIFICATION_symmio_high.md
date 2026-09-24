# VERIFICATION: SYMMIO — Deferred Liquidation Signature Staleness

**Task ID:** symmio-verify-high
**Agent:** Opus
**Date:** 2026-01-15
**Vuln file:** `sherlock-symmio-vuln-deferred-liquidation-staleness.md`
**Source repo:** `/home/z/sherlock-audit/symmio-protocol-core/` (mirror of `SYMM-IO/protocol-core`)
**Verdict:** **CONFIRMED** — with one factual correction to the vuln file's nonce analysis.

---

## 1. Claim Being Verified

> `DeferredLiquidationFacetImpl.deferredLiquidatePartyA` and
> `DeferredLiquidationFacetImpl.deferredSetSymbolsPrice` verify the supplied
> `DeferredLiquidationSig` via `LibMuonLiquidation.verifyDeferredLiquidationSig`,
> but — unlike every other liquidation / settlement / force-action entry point —
> **that verifier performs no freshness (`upnlValidTime`) check** on the
> muon-signed timestamp.

---

## 2. Code Verification (against actual SYMMIO source)

### 2.1 The deferred path — NO staleness check

**File:** `contracts/facets/liquidation/DeferredLiquidationFacetImpl.sol`

```solidity
// Line 22-33
function deferredLiquidatePartyA(address partyA, DeferredLiquidationSig memory liquidationSig) internal {
    MAStorage.Layout storage maLayout = MAStorage.layout();
    AccountStorage.Layout storage accountLayout = AccountStorage.layout();

    LibMuonLiquidation.verifyDeferredLiquidationSig(liquidationSig, partyA);
    // <-- NO require(block.timestamp <= liquidationSig.timestamp + upnlValidTime)

    int256 liquidationAvailableBalance = LibAccount.partyAAvailableBalanceForLiquidation(
            liquidationSig.upnl,
            liquidationSig.liquidationAllocatedBalance,   // stale T1 values
            partyA
    );
    require(liquidationAvailableBalance < 0, "LiquidationFacet: PartyA is solvent");
    ...
}
```

```solidity
// Line 62-67
function deferredSetSymbolsPrice(address partyA, DeferredLiquidationSig memory liquidationSig) internal {
    ...
    LibMuonLiquidation.verifyDeferredLiquidationSig(liquidationSig, partyA);
    // <-- NO staleness check
    require(maLayout.liquidationStatus[partyA], "LiquidationFacet: PartyA is solvent");
    ...
}
```

### 2.2 The verifier itself — NO staleness check inside

**File:** `contracts/libraries/muon/LibMuonLiquidation.sol:39-63`

```solidity
function verifyDeferredLiquidationSig(DeferredLiquidationSig memory liquidationSig, address partyA) internal view {
    MuonStorage.Layout storage muonLayout = MuonStorage.layout();
    require(liquidationSig.prices.length == liquidationSig.symbolIds.length, "LibMuon: Invalid length");
    bytes32 hash = keccak256(abi.encodePacked(
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
        liquidationSig.timestamp,               // bound in hash, never compared to block.timestamp
        liquidationSig.liquidationBlockNumber,
        liquidationSig.liquidationTimestamp,
        liquidationSig.liquidationAllocatedBalance,
        LibMuon.getChainId()
    ));
    LibMuon.verifyTSSAndGateway(hash, liquidationSig.sigs, liquidationSig.gatewaySignature);
}
```

### 2.3 The regular path — HAS staleness check (control)

**File:** `contracts/facets/liquidation/LiquidationFacetImpl.sol:26-27`

```solidity
LibMuonLiquidation.verifyLiquidationSig(liquidationSig, partyA);
require(block.timestamp <= liquidationSig.timestamp + MuonStorage.layout().upnlValidTime,
        "LiquidationFacet: Expired signature");
```

### 2.4 Sibling verifiers — ALL have staleness check inside

| Verifier | File:Line | Has `block.timestamp <= sig.timestamp + upnlValidTime`? |
|---|---|---|
| `LibMuon.verifyPartyBUpnl` | `LibMuon.sol:43` | ✅ YES |
| `LibMuonSettlement.verifySettlement` | `LibMuonSettlement.sol:15` | ✅ YES |
| `LibMuonForceActions.verifyHighLowPrice` | `LibMuonForceActions.sol:15` | ✅ YES |
| `LibMuonLiquidation.verifyLiquidationSig` | `LibMuonLiquidation.sol:16-37` | ❌ NO (but rescued by `require` in `liquidatePartyA` line 27) |
| `LibMuonLiquidation.verifyDeferredLiquidationSig` | `LibMuonLiquidation.sol:39-63` | ❌ NO — **and NOT rescued by any `require` in the caller** |

### 2.5 Nonce bump analysis — CORRECTION to vuln file

The vuln file states:
> The nonce (`partyANonces[partyA]`) is only incremented **after** the full
> settlement (`LiquidationFacetImpl.settlePartyALiquidation`, line 312).

**This is INACCURATE.** `partyANonces[partyA]` is incremented in **8** locations:

| Location | File:Line | Trigger |
|---|---|---|
| `openPosition` | `PartyBPositionActionsFacetImpl.sol:30` | PartyB opens a position |
| `fillCloseRequest` | `PartyBPositionActionsFacetImpl.sol:74` | PartyB fills a close |
| `emergencyClosePosition` | `PartyBPositionActionsFacetImpl.sol:111` | Emergency close |
| Force actions | `ForceActionsFacetImpl.sol:89` | Force cancel/close |
| Funding rate | `FundingRateFacetImpl.sol:76` | Funding rate settlement |
| PartyB liquidation | `LibLiquidation.sol:87` | Liquidating partyB |
| `settleUpnl` | `LibSettlement.sol:33` | General settlement |
| `settlePartyALiquidation` | `LiquidationFacetImpl.sol:312` | PartyA liquidation settlement |

**However, this correction does NOT invalidate the vulnerability.** The critical
observation is that `deposit()` and `allocate()` — the two functions partyA would
use to recover from insolvency — do **NOT** bump the nonce:

- `AccountFacetImpl.deposit` (line 19): no solvency check, no nonce bump.
- `AccountFacetImpl.allocate` (line 38): no solvency check (only
  `notLiquidatedPartyA` modifier), no nonce bump.

So the realistic attack path is:
1. **T1:** partyA is insolvent. Liquidator obtains a `DeferredLiquidationSig`
   (nonce = N, timestamp = T1, upnl = negative).
2. **T1→T2:** partyA deposits + allocates more collateral to recover. The nonce
   stays at N because `deposit`/`allocate` don't bump it. partyA cannot trade
   (openPosition/closePosition require solvency), so no trading nonce bumps
   either.
3. **T2:** Market moves in partyA's favor; partyA is now solvent at current
   prices. But the nonce is still N.
4. **T2:** Liquidator replays `deferredLiquidatePartyA(partyA, staleSig)`. The
   sig is 30 days old (well past `upnlValidTime`), but there is no staleness
   check. The hash matches (nonce = N), TSS+gateway verification passes. The
   stale `upnl(T1)` and `liquidationAllocatedBalance(T1)` make partyA appear
   insolvent. `liquidationStatus[partyA] = true`.
5. **T2+:** `deferredSetSymbolsPrice` writes stale T1 prices.
   `liquidatePositionsPartyA` closes positions at stale T1 marks.

**The window is bounded by partyA's first successful trade after recovering
solvency, not by `upnlValidTime`.** Since `deposit`/`allocate` are the primary
recovery mechanism and don't bump the nonce, the window can be arbitrarily long.

---

## 3. Proof of Concept (Foundry)

### 3.1 PoC location

```
/home/z/fkr-step1/defi-bounty/vuln/other-protocols/symmio-poc/
├── foundry.toml
├── src/
│   ├── SymmioHarness.sol          # Thin wrapper exposing internal library fns
│   └── symmio_contracts/          # EXACT unmodified copy of SYMMIO source
├── test/
│   └── SymmioDeferredStalenessPoC.t.sol
└── lib/
    ├── forge-std/
    ├── openzeppelin-contracts/          # v4.9.6
    └── openzeppelin-contracts-upgradeable/  # v4.9.6
```

### 3.2 Methodology

- The **actual, unmodified SYMMIO source** is compiled and linked.
- `SymmioHarness` exposes `LiquidationFacetImpl.liquidatePartyA` and
  `DeferredLiquidationFacetImpl.deferredLiquidatePartyA` as external functions,
  using the same diamond storage slots (`MuonStorage.layout()`, etc.).
- The muon TSS + gateway ECDSA verification is bypassed via
  `vm.mockCall(address(0x01), bytes(""), abi.encode(MOCK_GATEWAY))` on the
  ecrecover precompile. This is legitimate because the bug being tested — the
  missing `block.timestamp` check — is evaluated independently of signature
  validity. The SYMMIO source itself documents this testing pattern
  (`LibMuon.sol` lines 20-26).
- `upnlValidTime` is set to 1 hour. The stale sig has `timestamp = block.timestamp - 30 days`.

### 3.3 Test results

```
Ran 4 tests for test/SymmioDeferredStalenessPoC.t.sol:SymmioDeferredStalenessPoC
[PASS] test_DeferredPath_AcceptsFreshSig_SANITY()           (gas: 234397)
[PASS] test_DeferredPath_AcceptsStaleSig_BUG()              (gas: 247544)
[PASS] test_DeferredSetSymbolsPrice_WritesStalePrices_REPLAY() (gas: 413661)
[PASS] test_RegularPath_RejectsStaleSig_CONTROL()           (gas: 58984)
Suite result: ok. 4 passed; 0 failed; 0 skipped
```

### 3.4 Test descriptions

| Test | Purpose | Result |
|---|---|---|
| `test_DeferredPath_AcceptsStaleSig_BUG` | 30-day-stale `DeferredLiquidationSig` is accepted by `deferredLiquidatePartyA` — no staleness check | ✅ PASS (sig accepted, partyA liquidated, nonce NOT bumped) |
| `test_RegularPath_RejectsStaleSig_CONTROL` | Equivalent stale `LiquidationSig` is rejected by `liquidatePartyA` — `require` at line 27 fires | ✅ PASS (reverts with "LiquidationFacet: Expired signature") |
| `test_DeferredPath_AcceptsFreshSig_SANITY` | A fresh sig (within `upnlValidTime`) is accepted — confirms harness is correctly wired | ✅ PASS |
| `test_DeferredSetSymbolsPrice_WritesStalePrices_REPLAY` | After stale liquidation, `deferredSetSymbolsPrice` also accepts the same stale sig and writes stale prices | ✅ PASS |

### 3.5 Execution trace (BUG test)

```
SymmioHarness::deferredLiquidatePartyA(0xA11CE, {timestamp: 1997408000, upnl: -1000e18, ...})
  ├─ PRECOMPILES::ecrecover(...) → 0xBEEF     [TSS Schnorr verify — mocked]
  ├─ PRECOMPILES::ecrecover(...) → 0xBEEF     [Gateway ECDSA verify — mocked]
  └─ ← [Stop]                                 [NO staleness revert — sig is 30 days old, upnlValidTime = 1 hour]
SymmioHarness::getLiquidationStatus(0xA11CE) → true    [partyA IS in liquidation]
SymmioHarness::getPartyANonce(0xA11CE) → 0             [nonce NOT bumped — sig is replayable]
```

### 3.6 Execution trace (CONTROL test)

```
SymmioHarness::liquidatePartyA(0xA11CE, {timestamp: 1997408000, upnl: -1000e18, ...})
  ├─ PRECOMPILES::ecrecover(...) → 0xBEEF     [TSS verify — mocked, passes]
  ├─ PRECOMPILES::ecrecover(...) → 0xBEEF     [Gateway verify — mocked, passes]
  └─ ← [Revert] LiquidationFacet: Expired signature    [require at line 27 fires]
SymmioHarness::getLiquidationStatus(0xA11CE) → false   [partyA NOT liquidated]
```

---

## 4. Three-Perspective Re-Verification

### 4.1 Code / Static-analysis perspective

**CONFIRMED.** The divergence is a literal line-for-line omission:

- `LiquidationFacetImpl.liquidatePartyA` (line 27) has:
  `require(block.timestamp <= liquidationSig.timestamp + MuonStorage.layout().upnlValidTime, "LiquidationFacet: Expired signature");`
- `DeferredLiquidationFacetImpl.deferredLiquidatePartyA` (after line 26) has NO such line.
- `DeferredLiquidationFacetImpl.deferredSetSymbolsPrice` (after line 66) has NO such line.
- The verifier `verifyDeferredLiquidationSig` (unlike `verifyPartyBUpnl`, `verifySettlement`, `verifyHighLowPrice`) does not contain the check internally either.

The `DeferredLiquidationSig` struct does carry `timestamp`, `liquidationBlockNumber`, and `liquidationTimestamp` fields, and the muon hash binds all of them — so the signature is *internally consistent* and *correctly verified as authentic*. The defect is purely a missing consumption-policy check: the code verifies *what* was signed but not *when it may be consumed*.

### 4.2 Cryptographic / Signature perspective

**CONFIRMED.** The muon TSS signature scheme is sound:
- `LibMuonV04ClientBase.muonVerify` correctly implements Schnorr signature verification via `ecrecover` abuse.
- `LibMuon.verifyTSSAndGateway` correctly checks both the TSS signature and the gateway ECDSA signature.
- The hash in `verifyDeferredLiquidationSig` binds all semantically relevant fields: `upnl`, `prices`, `symbolIds`, `nonce`, `timestamp`, `liquidationBlockNumber`, `liquidationTimestamp`, `liquidationAllocatedBalance`.

The vulnerability is **not** a signature forgery or malleability issue. It is a **freshness policy gap**: the signature remains cryptographically valid forever (the TSS key doesn't rotate per-signature), and the contract never checks whether the signed timestamp is within an acceptable window of the current block. This is a textbook "signature-valid ≠ action-valid" gap.

### 4.3 Operational / Threat-model perspective

**CONFIRMED — with severity nuance.**

The attack requires:
1. `LIQUIDATOR_ROLE` (permissioned, but distributed to market-maker bots).
2. A previously-valid insolvency at T1 (partyA was genuinely liquidatable).
3. The deferred sig was issued but not consumed to completion.
4. partyA's nonce did not change between T1 and T2.

Condition 4 is the key constraint. The vuln file overstates this by claiming the nonce is "only incremented after full settlement." In reality, the nonce is also bumped on position opens/closes, force actions, funding rate, and general settlement. **However**, the two actions partyA would take to *recover* — `deposit` and `allocate` — do **NOT** bump the nonce. Furthermore, while partyA is insolvent, they cannot open/close positions (solvency checks in those flows block them). So the nonce is effectively frozen from T1 until partyA recovers and makes their first successful trade.

The most dangerous scenario: partyA is insolvent at T1, a deferred sig is issued, partyA deposits collateral and the market recovers (making them solvent), but partyA hasn't traded yet. The liquidator can replay the stale sig to liquidate at T1 prices, capturing the surplus collateral partyA just deposited.

**Severity assessment: HIGH (confirmed).** The impact is direct loss of user funds at the scale of an entire position plus surplus collateral. The role requirement (`LIQUIDATOR_ROLE`) is operationally distributed, not a trusted admin. The trigger conditions (insolvency event + sig issuance + non-consumption + recovery) are realistic during volatile markets. Not Critical because partyA must have been genuinely insolvent at T1 — the bug doesn't allow liquidating a party that was never insolvent.

---

## 5. Severity

**HIGH** — confirmed.

- Direct loss of partyA's collateral (including surplus posted after T1).
- No privileged role beyond `LIQUIDATOR_ROLE` (distributed).
- Realistic trigger conditions during volatile markets.
- The missing check is a straightforward omission vs. the sibling function and all other muon verifiers in the same codebase.

---

## 6. Suggested Fix

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
     ...

 function deferredSetSymbolsPrice(address partyA, DeferredLiquidationSig memory liquidationSig) internal {
     ...
     LibMuonLiquidation.verifyDeferredLiquidationSig(liquidationSig, partyA);
+    require(
+        block.timestamp <= liquidationSig.timestamp + MuonStorage.layout().upnlValidTime,
+        "LiquidationFacet: Expired deferred signature"
+    );
     require(maLayout.liquidationStatus[partyA], "LiquidationFacet: PartyA is solvent");
```

Alternatively (more DRY, matches sibling pattern): add the check inside
`verifyDeferredLiquidationSig` itself.

---

## 7. PoC Reproduction

```bash
cd /home/z/fkr-step1/defi-bounty/vuln/other-protocols/symmio-poc
export PATH="/home/z/.foundry/bin:$PATH"
forge test -vvvv
```

Expected output: 4 tests passed, 0 failed.

---

## 8. Submission Recommendation

**SUBMIT.** The vulnerability is confirmed by:
1. Direct code inspection — the missing `require` is a literal line-for-line omission vs. the sibling function.
2. A working Foundry PoC against the unmodified SYMMIO source — the deferred path accepts a 30-day-stale sig; the regular path rejects the equivalent sig.
3. Three-perspective re-verification (code, crypto, operational) — all confirm.

**Correction to include in submission:** The vuln file's claim that the nonce is "only incremented after full settlement" is inaccurate — it's also bumped on position opens/closes, force actions, funding rate, and general settlement. However, `deposit` and `allocate` (the recovery actions) do NOT bump the nonce, so the exploit window remains open from T1 until partyA's first successful trade after recovering solvency. This correction strengthens (or at minimum does not weaken) the exploit case.

**Severity:** HIGH (confirmed).

---

## 9. Submission Draft

> **Title:** Deferred liquidation signatures can be replayed indefinitely — `verifyDeferredLiquidationSig` omits the `upnlValidTime` staleness check
>
> **Severity:** High
>
> **Summary:**
> `DeferredLiquidationFacetImpl.deferredLiquidatePartyA` and
> `DeferredLiquidationFacetImpl.deferredSetSymbolsPrice` call
> `LibMuonLiquidation.verifyDeferredLiquidationSig` but, unlike every other
> muon-signed action in the protocol (settlement, force-close, partyB uPNL, and
> the non-deferred `liquidatePartyA`), neither the verifier nor the calling
> facet function checks `block.timestamp <= sig.timestamp + upnlValidTime`. A
> deferred-liquidation signature that was valid at block T1 can therefore be
> replayed at any later block T2 to liquidate partyA using stale prices and a
> stale uPNL, as long as `partyANonces[partyA]` has not changed.
>
> **Root cause:** `LiquidationFacetImpl.liquidatePartyA` (the non-deferred
> sibling) explicitly adds `require(block.timestamp <= liquidationSig.timestamp
> + MuonStorage.layout().upnlValidTime, "LiquidationFacet: Expired signature")`
> at line 27, immediately after `verifyLiquidationSig`. The deferred path
> omits this line entirely, and `verifyDeferredLiquidationSig` does not contain
> it internally (unlike `LibMuon.verifyPartyBUpnl:43`,
> `LibMuonSettlement.verifySettlement:15`, and
> `LibMuonForceActions.verifyHighLowPrice:15`, all of which enforce the check
> inside the verifier).
>
> **Impact:** A liquidator with `LIQUIDATOR_ROLE` can warehouse a valid
> deferred-liquidation signature and exercise it at the most opportune future
> moment — even after partyA has deposited more collateral and the market has
> moved in their favor. partyA's positions are closed at stale T1 prices, and
> the surplus collateral posted between T1 and T2 is captured by the liquidator
> and counterparties. `deposit` and `allocate` — partyA's primary recovery
> actions — do not bump `partyANonces[partyA]`, so the replay window extends
> from T1 until partyA's first successful trade after recovering solvency.
>
> **PoC:** Foundry test against unmodified SYMMIO source. A 30-day-stale
> `DeferredLiquidationSig` (with `upnlValidTime = 1 hour`) is accepted by
> `deferredLiquidatePartyA` while the equivalent stale `LiquidationSig` is
> correctly rejected by `liquidatePartyA` with "Expired signature".
>
> **Fix:** Add `require(block.timestamp <= liquidationSig.timestamp +
> MuonStorage.layout().upnlValidTime, "LiquidationFacet: Expired deferred
> signature")` after `verifyDeferredLiquidationSig` in both
> `deferredLiquidatePartyA` and `deferredSetSymbolsPrice`, or inside
> `verifyDeferredLiquidationSig` itself (matching the sibling verifier pattern).
