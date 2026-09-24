# DeXe Protocol — Permissionless `recalculateNftPowers` Enables Quorum Manipulation on Stale `totalRawPower`

**Severity: MEDIUM**
**Area: Governance voting / Quorum calculation / Integer precision**
**Contracts: ERC721RawPower, GovUserKeeper, GovPoolVote**

---

## 1. Description

`ERC721RawPower.totalPower()` returns the stored `totalRawPower`:

```solidity
// ERC721RawPower.sol, lines 45–47
function totalPower() external view override returns (uint256) {
    return totalRawPower;
}
```

`totalRawPower` is **only** updated when `_recalculateRawNftPower(tokenId)` is
called — on transfer, collateral change, or an explicit
`recalculateNftPowers` call. Between recalculations, NFT powers decay (per
`reductionPercent * elapsed_time`) but `totalRawPower` remains stale at the
**old, higher** value.

The quorum check in `GovPoolVote._quorumReached` reads `getTotalPower()`
live:

```solidity
// GovPoolVote.sol, lines 367–375
function _quorumReached(IGovPool.ProposalCore storage core) internal view returns (bool) {
    (, address userKeeperAddress, , , ) = IGovPool(address(this)).getHelperContracts();
    uint256 totalPower = IGovUserKeeper(userKeeperAddress).getTotalPower();
    return
        PERCENTAGE_100.ratio(core.votesFor, totalPower) >= core.settings.quorum ||
        PERCENTAGE_100.ratio(core.votesAgainst, totalPower) >= core.settings.quorum;
}
```

`GovUserKeeper.getTotalPower()` → `IERC721Power(token).totalPower()` →
`totalRawPower` (stale).

`recalculateNftPowers` is **permissionless** (`external`, no access control):

```solidity
// ERC721RawPower.sol, lines 39–43
function recalculateNftPowers(uint256[] calldata tokenIds) external override {
    for (uint256 i = 0; i < tokenIds.length; i++) {
        _recalculateRawNftPower(tokenIds[i]);
    }
}
```

Anyone can call it on **any** set of NFTs to force `totalRawPower` down to
the current decayed total. This lets an attacker selectively lower the
quorum denominator right before (or in the same transaction as) a vote,
causing a proposal that would **not** reach quorum under the stale total to
suddenly reach it.

## 2. Contract + Function + Line

| Item | Location |
|------|----------|
| `ERC721RawPower.totalPower` (stale source) | `contracts/gov/ERC721/powers/ERC721RawPower.sol:45–47` |
| `ERC721RawPower.recalculateNftPowers` (permissionless) | `contracts/gov/ERC721/powers/ERC721RawPower.sol:39–43` |
| `AbstractERC721Power._recalculateRawNftPower` | `contracts/gov/ERC721/powers/AbstractERC721Power.sol:202–218` |
| `GovUserKeeper.getTotalPower` | `contracts/gov/user-keeper/GovUserKeeper.sol:651–677` |
| `GovPoolVote._quorumReached` | `contracts/libs/gov/gov-pool/GovPoolVote.sol:367–375` |
| `GovPoolCreate._calculateNewQuorum` (also reads live total) | `contracts/libs/gov/gov-pool/GovPoolCreate.sol:319–329` |

## 3. Attack Scenario

1. Several NFTs exist with decaying power. No one has called
   `recalculateNftPowers` recently, so `totalRawPower` is inflated relative
   to the true current sum of NFT powers.
2. The attacker holds NFTs (or ERC20 tokens) and wants proposal `P` to pass.
   Under the stale `totalRawPower`, the attacker's vote is short of quorum.
3. In the **same transaction** (or the block immediately before voting), the
   attacker calls `ERC721RawPower.recalculateNftPowers([id1, id2, …])` on
   **other** users' NFTs. `totalRawPower` drops to the true (lower) value.
4. The attacker calls `GovPool.vote(P, true, amount, nftIds)`.
   `_quorumReached` now divides by the reduced `totalRawPower` and quorum is
   reached.
5. The proposal moves to `Locked` → `SucceededFor` and can be executed.

The attacker's own vote power is `getNftPower(tokenId)` which already
reflects the decayed value regardless of recalculation, so the attacker's
numerator is unchanged while the denominator shrinks — a pure win.

### Secondary effect — `_calculateNewQuorum` manipulation

`GovPoolCreate._calculateNewQuorum` (used when treasury is exempted from a
proposal) also reads `getTotalPower()` live. A stale/refreshed total
directly scales the adjusted quorum, so the same technique can depress the
required quorum for treasury-exempt proposals.

## 4. Proof of Concept

```solidity
contract QuorumManipPoC {
    function attack(
        IGovPool govPool,
        ERC721RawPower nft,
        uint256[] calldata otherNftIds, // NFTs owned by other users
        uint256 proposalId,
        uint256 voteAmount
    ) external {
        // 1. Force totalRawPower down to current decayed values
        nft.recalculateNftPowers(otherNftIds);

        // 2. Vote — quorum check now uses the reduced denominator
        uint256[] memory empty = new uint256[](0);
        govPool.vote(proposalId, true, voteAmount, empty);

        // Proposal may now be in Locked state whereas it would have stayed
        // in Voting under the stale totalRawPower.
    }
}
```

## 5. Impact

| Impact | Detail |
|--------|--------|
| **Governance manipulation** | Proposals that should not reach quorum can be forced through. Matches Immunefi Critical impact "Manipulation of governance voting result deviating from voted outcome". |
| **Asymmetric** | The manipulation only works **downward** (lowering `totalRawPower`), so it favours passing proposals, not blocking them. An attacker can pass beneficial proposals (treasury spends, settings changes) with less than the intended quorum. |
| **Composability with validation bypass** | Combined with the `_validateProposalCreation` bypass, an attacker can both lower the quorum and inject malicious actions. |

## 6. Severity: **MEDIUM**

The attack is real but bounded:
- It requires NFT powers to have decayed (time-dependent).
- It requires the attacker to hold enough voting power to be close to quorum
  even under the corrected total.
- `ERC721EquivalentPower` (the other power contract) returns a fixed
  `_powerEquivalent` from `totalPower()` and is **not** affected. Only pools
  using `ERC721RawPower` are vulnerable.

## 7. Three-Perspective Audit

### 7.1 Attacker perspective
The attacker monitors proposals that are just below quorum. Right before
voting, they call `recalculateNftPowers` on the highest-power NFTs they don't
own, maximally depressing `totalRawPower`. This is cheap (a single external
call) and can be bundled with the vote in one transaction.

### 7.2 Defender perspective
The protocol relies on keepers to keep `totalRawPower` fresh
(`GovUserKeeper.updateNftPowers` is called inside `_vote`/`delegate`, but
only for the voter's own NFTs). Other NFTs remain stale. The quorum
denominator should be either a snapshot taken at proposal creation or a
value that is refreshed atomically for **all** NFTs before each quorum check.

### 7.3 Neutral perspective
This is a "stale oracle" pattern applied to governance. The root cause is
that `totalPower()` returns a mutable, lazily-updated aggregate instead of a
snapshot. The permissionless `recalculateNftPowers` compounds the issue by
letting any caller pick which NFTs to refresh, creating an asymmetric
denominator.

## 8. Recommended Fix

1. **Snapshot the total power at proposal creation** and use that snapshot
   for all quorum checks on that proposal (similar to how `GovValidatorsToken`
   uses `totalSupplyAt(snapshotId)`).

2. **If a live total is required**, recalculate **all** NFTs atomically
   inside `getTotalPower` (gas-heavy but correct), or restrict
   `recalculateNftPowers` to `onlyOwner` and run it via a keeper.

3. At minimum, make `recalculateNftPowers` `onlyOwner` (the GovPool /
   user-keeper) so external attackers cannot selectively depress the
   denominator.
