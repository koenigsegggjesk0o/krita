# DeXe Protocol — Division-by-Zero in Quorum Check When `totalPower == 0`

**Severity: LOW**
**Area: Integer precision / Governance voting / Edge case**
**Contracts: GovPoolVote, GovUserKeeper**

---

## 1. Description

`GovPoolVote._quorumReached` computes:

```solidity
// GovPoolVote.sol, lines 367–375
uint256 totalPower = IGovUserKeeper(userKeeperAddress).getTotalPower();
return
    PERCENTAGE_100.ratio(core.votesFor, totalPower) >= core.settings.quorum ||
    PERCENTAGE_100.ratio(core.votesAgainst, totalPower) >= core.settings.quorum;
```

`MathHelper.ratio(base, num, denom)` is `(base * num) / denom` with no
zero-denominator guard:

```solidity
// MathHelper.sol, lines 12–14
function ratio(uint256 base, uint256 num, uint256 denom) internal pure returns (uint256) {
    return (base * num) / denom;
}
```

`GovUserKeeper.getTotalPower()` returns `0` when:
- `tokenAddress == address(0)` (no ERC20 set), AND
- `_nftInfo.nftAddress == address(0)` (no NFT set), OR
- the NFT contract reports `totalPower() == 0` (e.g. all NFTs burned or
  `totalRawPower` driven to zero via the recalculation path described in
  `dexe-vuln-quorum-manipulation.md`), AND no ERC20 is configured.

When `totalPower == 0`, any call to `_quorumReached` reverts with a
division-by-zero panic (`Panic(0x12)`). This reverting path is reachable
from several `external`/`public` functions:

| Caller | Effect |
|--------|--------|
| `GovPool.vote` → `_vote` → `_updateGlobalState` → `_quorumReached` | Voting reverts — no user can vote. |
| `GovPool.cancelVote` → `_updateGlobalState` → `_quorumReached` | Cancelling a vote reverts. |
| `GovPool.delegate` / `undelegate` → `_revoteDelegated` → `_updateGlobalState` → `_quorumReached` | Delegation reverts. |
| `GovPool.getProposalState` (view) → `core._quorumReachedThroughVoting()` *(note: this checks `executeAfter != 0`, not `_quorumReached`)* | Not directly affected, but any view that calls `_quorumReached` reverts. |
| `GovPool.getProposalRequiredQuorum` (view) | Uses `_govUserKeeper.getTotalPower().ratio(quorum, PERCENTAGE_100)` — also divides by `totalPower` when total is zero? No — here `totalPower` is the *base* and `quorum`/`PERCENTAGE_100` are num/denom, so `totalPower * quorum / PERCENTAGE_100` is fine even when `totalPower == 0`. |

The most impactful path is `vote`: when `totalPower == 0`, **no proposal can
ever be voted on**, permanently freezing governance until an admin sets a
token or NFT address (which itself requires a proposal — a catch-22 unless
`GovSettings`/`GovUserKeeper` ownership allows direct action).

## 2. Contract + Function + Line

| Item | Location |
|------|----------|
| `_quorumReached` (div-by-zero) | `contracts/libs/gov/gov-pool/GovPoolVote.sol:367–375` |
| `MathHelper.ratio` (no zero guard) | `contracts/libs/math/MathHelper.sol:12–14` |
| `GovUserKeeper.getTotalPower` (can return 0) | `contracts/gov/user-keeper/GovUserKeeper.sol:651–677` |

## 3. Attack Scenario

### Accidental — fresh deployment

A newly deployed GovPool where `GovUserKeeper` has not yet been given a token
or NFT address (`__GovUserKeeper_init` allows both to be zero, requiring only
that at least one is non-zero, but the factory sets them from
`parameters.userKeeperParams`). If a misconfiguration passes both as zero,
every `vote`/`cancelVote`/`delegate` call reverts, bricking the pool.

### Induced — driving NFT total power to zero

If the pool uses `ERC721RawPower` and all NFTs are burned (or their
`totalRawPower` is driven to zero via repeated collateral removal +
recalculation), and no ERC20 is configured, `getTotalPower()` returns `0`.
From that point, governance is frozen.

## 4. Proof of Concept

```solidity
// Precondition: GovUserKeeper has tokenAddress == 0 and nftAddress == 0
// (or NFT totalPower() == 0 and tokenAddress == 0).

function brickGovernance(IGovPool govPool, uint256 proposalId) external {
    uint256[] memory empty = new uint256[](0);
    // Reverts with Panic(0x12) (division by zero)
    govPool.vote(proposalId, true, 1, empty);
}
```

## 5. Impact

| Impact | Detail |
|--------|--------|
| **Permanent freezing of governance** | No proposals can be voted on, cancelled, or delegated. |
| **No fund theft** | The issue is a denial-of-service, not a theft. |

## 6. Severity: **LOW**

- Requires misconfiguration or extreme edge cases to trigger.
- Fund loss is not possible; only governance DoS.
- Immunefi scope lists "Permanent freezing of funds" as Critical, but this
  bug freezes **governance**, not funds. Users can still withdraw via
  `GovPool.withdraw` (which does not call `_quorumReached`). So this is a
  governance-only DoS.

## 7. Three-Perspective Audit

### 7.1 Attacker perspective
Limited value: the attacker can only cause a DoS, not steal funds. The
attack requires either a misconfigured deployment or the ability to drive
NFT total power to zero (which requires burning all NFTs — typically an
`onlyOwner` action).

### 7.2 Defender perspective
A trivial `if (totalPower == 0) return false;` guard in `_quorumReached`
eliminates the revert. Proposals would simply stay in `Voting` until
`voteEnd`, then move to `Defeated`, which is the correct behaviour when
there is no voting power in the system.

### 7.3 Neutral perspective
This is a classic "missing zero-denominator check" in a ratio helper. It is
low-severity on its own but worth fixing because it can compound with other
issues (e.g. if an attacker finds a way to drive `totalPower` to zero, they
can permanently brick governance).

## 8. Recommended Fix

```solidity
// GovPoolVote.sol
function _quorumReached(IGovPool.ProposalCore storage core) internal view returns (bool) {
    (, address userKeeperAddress, , , ) = IGovPool(address(this)).getHelperContracts();
    uint256 totalPower = IGovUserKeeper(userKeeperAddress).getTotalPower();
    if (totalPower == 0) return false; // ← guard
    return
        PERCENTAGE_100.ratio(core.votesFor, totalPower) >= core.settings.quorum ||
        PERCENTAGE_100.ratio(core.votesAgainst, totalPower) >= core.settings.quorum;
}
```

Also consider adding `require(denom != 0)` (or returning 0) inside
`MathHelper.ratio` for defence-in-depth.
