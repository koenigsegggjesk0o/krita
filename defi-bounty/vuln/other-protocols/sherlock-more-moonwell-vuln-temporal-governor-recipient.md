# Moonwell — `TemporalGovernor._executeProposal` fast-track path omits `intendedRecipient` verification

**Protocol**: Moonwell (Sherlock bounty #350, up to $250,000 USD)
**Repo**: `github.com/moonwell-fi/moonwell-contracts-v2` (cloned to `/home/z/usual-moonwell`)
**Severity**: Low (defense-in-depth; guardian-trusted path)
**Area**: Signature verification / cross-contract interaction (Wormhole VAA governance)

---

## 1. Description

`TemporalGovernor` settles Wormhole-bridged governance proposals in two phases:

1. **`_queueProposal`** (line 304) — parses the VAA, verifies the Wormhole signature,
   checks the emitter is a trusted sender, checks `intendedRecipient == address(this)`,
   and records `queueTime`.
2. **`_executeProposal`** (line 356) — re-parses the VAA, re-verifies the Wormhole
   signature, checks the timelock has elapsed (or is being bypassed via
   `fastTrackProposalExecution`), checks the emitter is trusted, checks `!executed`,
   then executes the `targets[].call(values, calldatas)`.

The `intendedRecipient == address(this)` check exists **only** in `_queueProposal`
(line 331-334). It is **absent** from `_executeProposal`:

```vyper
# _executeProposal, line 397
(, targets, values, calldatas) = abi.decode(
    vm.payload,
    (address, address[], uint256[], bytes[])
)
# intendedRecipient is discarded; no recipient check performed
```

For the **normal** execute path this is acceptable, because the VAA hash is the same
as the queued one (Wormhole `vm.hash` covers the entire payload), so a VAA can only be
executed if it was first queued — and queueing already enforced the recipient.

The gap is the **fast-track** path. `fastTrackProposalExecution` (line 276) calls
`_executeProposal(VAA, overrideDelay=true)`. When `overrideDelay` is true and the VAA
was *not* previously queued (`queueTime == 0`), `_executeProposal` simply sets
`queueTime = block.timestamp` and proceeds (line 376-379) — **without ever checking
`intendedRecipient`**. The recipient check that normally protects execution was only
performed at queue time, which the fast-track path skips.

Consequently, a Wormhole VAA that was emitted by a trusted sender but whose
`intendedRecipient` is a *different* contract (i.e. a VAA never meant for this
`TemporalGovernor`) can be fast-track-executed against this contract if the guardian
signs off, because the only fields that matter for execution (`targets`, `values`,
`calldatas`) are decoded straight from the payload with no scoping check.

---

## 2. Contract / function / line

| File | Function | Line(s) |
|------|----------|---------|
| `src/governance/TemporalGovernor.sol` | `_queueProposal` (recipient check present) | 331-334 |
| `src/governance/TemporalGovernor.sol` | `_executeProposal` (no recipient check) | 356-421 |
| `src/governance/TemporalGovernor.sol` | `fastTrackProposalExecution` | 276-280 |
| `src/governance/TemporalGovernor.sol` | fast-track branch inside `_executeProposal` | 376-379 |

---

## 3. Attack scenario

This requires the guardian (owner) to initiate fast-track, so it is not exploitable by
an arbitrary external attacker. The realistic threat model is **defense-in-depth**
against a guardian mistake or a partially-compromised guardian flow:

1. A trusted Wormhole emitter on chain X emits a VAA intended for a *different*
   `TemporalGovernor` deployment on chain Y (e.g. the Base governor instead of the
   Optimism governor). The payload's `targets` happen to be valid contracts on this
   chain.
2. The guardian — reacting to an emergency and fast-tracking several VAAs — includes
   this mis-addressed VAA in a batch.
3. `_executeProposal(VAA, overrideDelay=true)` runs: `queueTime == 0`, so it sets
   `queueTime = now` (line 378), skips the recipient check (it was never queued, so
   the queue-time check never ran), verifies only the emitter and `!executed`, and
   then calls `targets[i].call{value: values[i]}(calldatas[i])`.
4. The mis-addressed proposal executes arbitrary calls on this chain.

The same shape of bug also means that if the *only* check ever gating a fast-tracked
VAA is the emitter allow-list, a future change that relaxes the emitter set (e.g.
adding a new chain's emitter) would implicitly widen the set of payloads that can be
fast-tracked without any recipient scoping.

---

## 4. Proof of Concept (Foundry, sketch)

A full PoC requires a Wormhole devnet and a signed VAA; the structural check is:

```solidity
// Pseudocode / Foundry sketch
function test_fastTrack_skips_recipient_check() public {
    // 1. craft a Wormhole VAA whose payload decodes to
    //    (intendedRecipient = address(0xDEAD), targets=[comptroller], values=[0],
    //     calldatas=[abi.encodeWithSelector(_setCloseFactor.selector, 0.9e18)])
    // 2. have the guardian's emitter sign it (wormhole devnet).
    bytes memory vaa = _buildAndSignVAA(address(0xDEAD), ...);

    // 3. the VAA was never queued on this TemporalGovernor.
    assertEq(uint256(temporalGovernor.queuedTransactions(keccak256(vaa)).queueTime), 0);

    // 4. guardian fast-tracks it while paused.
    vm.prank(guardian);
    vm.expectRevert(); // <-- SHOULD revert with "Incorrect destination", but does NOT
    temporalGovernor.fastTrackProposalExecution{value: 0}(vaa);

    // In the current code the call above SUCCEEDS and comptroller._setCloseFactor
    // is executed, even though intendedRecipient != address(temporalGovernor).
}
```

The fix (below) makes step 4 revert as expected.

---

## 5. Impact

* **Fund loss**: only reachable via the guardian fast-track path, which is
  `onlyOwner` + `whenPaused`. Direct theft by an external attacker is therefore not
  possible.
* **Defense-in-depth**: the recipient scoping invariant that protects the normal path
  is silently dropped on the fast-track path. A guardian operating under emergency
  pressure (the exact scenario fast-track is designed for) has no on-chain guard
  preventing a mis-addressed or wrong-chain VAA from executing.
* **Bounty alignment**: matches Moonwell's *"permanent modification of protocol
  parameters leading to a significant loss"* and *"centralization risks"* criteria,
  but is gated behind the trusted guardian, hence Low rather than High/Critical.

---

## 6. Three-perspective audit

**Exploitability**
Requires the guardian account (owner of `TemporalGovernor`). Not exploitable by an
arbitrary user. The value of the finding is as a missing invariant that would catch
guardian/operator error during an emergency.

**Economic impact**
If a mis-addressed VAA with malicious `targets` is fast-tracked, arbitrary governance
calls execute (parameter changes, ownership transfers, fund sweeps). The blast radius
is the full set of contracts the `TemporalGovernor` owns (Comptroller admin, mToken
admins, etc.). Bounded in practice by the guardian being a multisig / DAO-controlled.

**Fix recommendation**
Add the `intendedRecipient == address(this)` check to `_executeProposal` as well, so
that both code paths enforce the same invariant regardless of whether the VAA was
previously queued:

```diff
 function _executeProposal(bytes memory VAA, bool overrideDelay) private {
     (IWormhole.VM memory vm, bool valid, string memory reason) =
         wormholeBridge.parseAndVerifyVM(VAA);
     require(valid, reason);

+    address intendedRecipient;
     address[] memory targets;
     uint256[] memory values;
     bytes[] memory calldatas;
-    (, targets, values, calldatas) = abi.decode(
+    (intendedRecipient, targets, values, calldatas) = abi.decode(
         vm.payload, (address, address[], uint256[], bytes[])
     );
+    require(
+        intendedRecipient == address(this),
+        "TemporalGovernor: Incorrect destination"
+    );
     _sanityCheckPayload(targets, values, calldatas);
     ...
```

This is cheap (one `abi.decode` field + one `require`) and makes the recipient
invariant uniform across queue and execute, removing the fast-track gap entirely.

---

## 7. References

* `_queueProposal` recipient check: `src/governance/TemporalGovernor.sol:317-334`
* `_executeProposal` missing check: `src/governance/TemporalGovernor.sol:356-421`
* `fastTrackProposalExecution`: `src/governance/TemporalGovernor.sol:276-280`
* fast-track branch: `src/governance/TemporalGovernor.sol:376-379`
