# Olympus NoteKeeper — `pullNote` does not clear `noteTransfers`, allowing repeat pulls of ghost notes; plus permissionless `redeem`

## Summary

Two related defects in `NoteKeeper` (the base of `OlympusBondDepositoryV2`):

1. `pullNote` copies a note from `_from` to `msg.sender` and `delete`s the
   original, but it **never clears `noteTransfers[_from][_index]`**. Because a
   deleted note has `redeemed == 0` (the default), the guard
   `require(notes[_from][_index].redeemed == 0)` keeps passing, so the same
   approval can be pulled again — pushing ghost (all-zero) notes into the
   puller's array. This is unbounded storage pollution on the victim's array and
   breaks `indexesFor` / redemption accounting assumptions.

2. `redeem(address _user, uint256[] _indexes, bool _sendgOHM)` is `public` with
   **no access control**: anyone can force-redemption of another user's matured
   notes (payout sent to `_user`). While payout is not stolen, the owner loses
   control over *when* redemption happens (tax/strategy timing), and front-ends
   that assume only the owner redeems can be griefed.

## Contract / Function / Lines

- Contract: `contracts/types/NoteKeeper.sol`
- `pullNote` lines **143–151** — missing `delete noteTransfers[_from][_index];`
- `redeem` lines **92–113** — `public`, no `msg.sender`/authority check
- Guard that enables repeat pulls: line **145**
  `require(notes[_from][_index].redeemed == 0, ...)` (a deleted note satisfies
  this) and line **144** `require(noteTransfers[_from][_index] == msg.sender)`
  (never cleared).
- Array-enumeration consumer: `indexesFor` lines **162–181** (filters on
  `redeemed == 0 && payout != 0`, so ghost notes are skipped — but they still
  inflate gas and any consumer not filtering on `payout != 0` is exposed).

## Root cause (pullNote)

```solidity
function pullNote(address _from, uint256 _index) external override returns (uint256 newIndex_) {
    require(noteTransfers[_from][_index] == msg.sender, "Depository: transfer not found");
    require(notes[_from][_index].redeemed == 0, "Depository: note redeemed");

    newIndex_ = notes[msg.sender].length;
    notes[msg.sender].push(notes[_from][_index]);  // ghost on repeat: pushes zeroed Note
    delete notes[_from][_index];                    // original zeroed, but approval REMAINS
    // NOTE: no `delete noteTransfers[_from][_index];`
}
```

After the first `pullNote`, `notes[_from][_index]` is a zeroed `Note`
(`payout=0, created=0, matured=0, redeemed=0, marketID=0`). The repeat-pull guard
`redeemed == 0` passes; `noteTransfers` still equals `msg.sender`; so the puller
can `push` unbounded zeroed `Note` structs into **their own** array.

## Root cause (redeem)

```solidity
function redeem(address _user, uint256[] memory _indexes, bool _sendgOHM)
    public override returns (uint256 payout_)   // no auth
{
    ...
    gOHM.transfer(_user, payout_); // or staking.unwrap(_user, payout_)
}
```

No check that `msg.sender == _user` or that `_user` authorized `msg.sender`. The
payout goes to `_user`, so not theft — but redemption timing is forced.

## Attack scenario (pullNote)

1. Alice `pushNote(attacker, idx)` to approve transfer of note `idx`.
2. Attacker `pullNote(Alice, idx)` — receives the note, Alice's is deleted.
3. Attacker calls `pullNote(Alice, idx)` again in a loop — each call pushes a
   ghost `Note` into `notes[attacker]` (and re-deletes an already-zero slot).
   Loop `N` times → attacker's `notes` array grows by `N` ghost entries.
4. Any caller iterating `notes[attacker]` (e.g. `indexesFor`, or off-chain
   indexing) now pays `O(N)` extra gas; `redeemAll(attacker, ...)` iterates all
   `N` ghosts (skipped via `payout != 0` filter, but still gas-burned). An
   attacker can bloat a *target's* array by approving them and having the target
   pull, or bloat their own to grief keepers/indexers.

## Attack scenario (redeem)

1. Victim holds a matured bond note.
2. Attacker front-runs/observes and calls `redeem(victim, [idx], true)` —
   victim's note is marked redeemed and gOHM is sent to the victim. Victim
   wanted to hold for a later tax event or to unwrap at a specific index; choice
   removed. Repeatable for all of the victim's notes.

## PoC (Foundry)

```solidity
// SPDX-License-Identifier: AGPL-3.0-only
pragma solidity ^0.8.10;

import "forge-std/Test.sol";
import "../contracts/BondDepository.sol"; // is NoteKeeper

contract PullNotePoC is Test {
    OlympusBondDepositoryV2 bond;
    address alice = address(0xA11CE);
    address attacker = address(0xBAD);

    function testRepeatPullBloatsArray() public {
        // assume alice has a note at index 0 (created via deposit)
        uint256 idx = 0;
        vm.prank(alice);
        bond.pushNote(attacker, idx);

        uint256 beforeLen = /* bond.getNotesLength(attacker) */ 0;
        vm.startPrank(attacker);
        bond.pullNote(alice, idx);          // legitimate first pull
        bond.pullNote(alice, idx);          // repeat: pushes ghost
        bond.pullNote(alice, idx);          // repeat: pushes ghost
        vm.stopPrank();
        // attacker now has 3 entries (1 real + 2 ghost); can be looped unbounded
        // assertEq(notesLength(attacker), beforeLen + 3);
    }

    function testPermissionlessRedeem() public {
        uint256 idx = 0; // victim's matured note
        uint256[] memory idxs = new uint256[](1);
        idxs[0] = idx;
        // anyone can redeem for the victim -> payout goes to victim, timing forced
        bond.redeem(/*_user=*/address(0xV1C), idxs, true);
    }
}
```

## Impact

- **pullNote:** unbounded array bloat (gas griefing of the puller, and of any
  party iterating that array). Storage pollution. Breaks the implicit
  invariant that a `noteTransfers` approval is single-use. Low-to-Medium.
- **redeem:** loss of redemption-timing control for note owners; front-end /
  keeper griefing. Informational-to-Low (no fund theft).

## Severity

- `pullNote` repeat-pull: **Low** (gas/storage griefing; no value theft, ghosts
  are non-redeemable because `payout == 0`).
- `redeem` permissionless: **Low / Informational** (payout always to owner).

## Three-perspective audit

- **Differential:** `pushNote` (133–136) sets `noteTransfers`; the symmetric
  consumer `pullNote` should clear it. The single-use intent is implied by the
  `redeemed == 0` guard (which only makes sense if a note can be pulled once).
  The missing `delete` is an oversight versus that intent.
- **Adversarial:** `pullNote` is permissionless beyond the approval, so the
  repeat-pull is cheap and repeatable; an attacker can grief any keeper that
  iterates `notes[]`. `redeem`'s lack of auth is exploitable for timing grief.
- **Defensive / fix:**
```solidity
function pullNote(address _from, uint256 _index) external override returns (uint256 newIndex_) {
    require(noteTransfers[_from][_index] == msg.sender, "Depository: transfer not found");
    require(notes[_from][_index].created != 0, "Depository: note not found"); // block ghost pulls
    require(notes[_from][_index].redeemed == 0, "Depository: note redeemed");
    newIndex_ = notes[msg.sender].length;
    notes[msg.sender].push(notes[_from][_index]);
    delete notes[_from][_index];
    delete noteTransfers[_from][_index]; // <-- fix
}
```
  For `redeem`, gate with `require(msg.sender == _user || approvedRedeemer[msg.sender])`
  or document the permissionless-redemption behavior explicitly and ensure all
  consumers account for forced redemption.
