# DeXe Protocol — Zero-Address Verifier Enables Forgery of Off-chain Results

**Severity: HIGH**
**Area: Signature verification / Off-chain governance**
**Contracts: GovPool, GovPoolOffchain**

---

## 1. Description

`GovPool.changeVerifier` is gated by `onlyThis` (proposal action only) but does
**not** check that `newVerifier != address(0)`:

```solidity
// GovPool.sol, lines 399–401
function changeVerifier(address newVerifier) external override onlyThis {
    _offChain.verifier = newVerifier;
}
```

The off-chain results verification in `GovPoolOffchain.saveOffchainResults`
relies on `ECDSA.recover`:

```solidity
// GovPoolOffchain.sol, lines 26–29
require(
    signHash_.toEthSignedMessageHash().recover(signature) == offChain.verifier,
    "Gov: invalid signer"
);
```

OpenZeppelin's `ECDSA.recover` returns `address(0)` when the signature is
malformed, `r`/`s` are zero, or the signature does not map to a valid public
key. If `offChain.verifier` is ever set to `address(0)`, **any** user can
submit **any** `resultsHash` with a garbage signature and the check passes,
because `recover(garbage) == address(0) == offChain.verifier`.

Each successful `saveOffchainResults` call:

1. Overwrites `offChain.resultsHash` (the canonical off-chain vote result).
2. Marks `usedHashes[signHash_] = true` (preventing replay of the same hash).
3. Calls `_payCommission()` — sends a commission to the DeXe treasury.
4. Calls `_updateRewards(0, msg.sender, SaveOffchainResults)` — credits the
   caller `executionReward` tokens as off-chain reward, claimable via
   `claimRewards([0], …)`.

Because the `signHash` includes `msg.sender` and `resultsHash`, every call
with a new `resultsHash` produces a new `signHash`, so the `usedHashes` guard
offers no protection. The attacker can call `saveOffchainResults` in a loop,
each time with a new `resultsHash` string, crediting themselves
`executionReward` per iteration and draining the GovPool reward token balance.

## 2. Contract + Function + Line

| Item | Location |
|------|----------|
| `GovPool.changeVerifier` (missing zero check) | `contracts/gov/GovPool.sol:399–401` |
| `GovPoolOffchain.saveOffchainResults` | `contracts/libs/gov/gov-pool/GovPoolOffchain.sol:18–37` |
| `GovPoolOffchain.getSignHash` | `contracts/libs/gov/gov-pool/GovPoolOffchain.sol:39–41` |
| `_payCommission` | `contracts/libs/gov/gov-pool/GovPoolOffchain.sol:43–52` |
| `GovPool.saveOffchainResults` (caller) | `contracts/gov/GovPool.sol:426–433` |

## 3. Attack Scenario

### Path A — Governance proposal sets verifier to `address(0)`

1. A proposal (possibly the validation-bypass proposal described in
   `dexe-vuln-proposal-validation-bypass.md`, or a legitimate-looking
   "reset verifier" proposal) sets `offChain.verifier = address(0)` via
   `GovPool.changeVerifier(address(0))`.
2. Any BABT-holder calls `saveOffchainResults("hash1", "0x00")`. The
   `recover` returns `address(0)`, matching `offChain.verifier`.
3. The caller is credited `executionReward` (from `INTERNAL` settings).
4. The caller repeats with `"hash2"`, `"hash3"`, … — each is a new `signHash`
   so `usedHashes` does not block.
5. The caller calls `claimRewards([0], caller)` to withdraw all accumulated
   off-chain rewards, draining the GovPool's reward token.

### Path B — Accidental / misconfigured deployment

If the DAO ever legitimately intends to "disable" off-chain verification by
setting the verifier to `address(0)` (a plausible but dangerous
misconfiguration), the same exploit applies without any malicious proposal.

## 4. Proof of Concept

```solidity
contract VerifierZeroPoC {
    function attack(IGovPool govPool, uint256 iterations) external {
        bytes memory badSig = hex"00";
        for (uint256 i = 0; i < iterations; i++) {
            // each unique resultsHash → unique signHash → bypasses usedHashes
            string memory h = string(abi.encodePacked("h", i));
            govPool.saveOffchainResults(h, badSig);
        }
        uint256[] memory ids = new uint256[](1);
        ids[0] = 0;
        govPool.claimRewards(ids, address(this));
    }
}
```

## 5. Impact

| Impact | Detail |
|--------|--------|
| **Theft of unclaimed yield / reward tokens** | The attacker drains the GovPool reward token balance by repeatedly crediting themselves `executionReward`. Matches Immunefi High impact "Theft of unclaimed yield". |
| **Off-chain result forgery** | `offChain.resultsHash` can be set to arbitrary values, corrupting any off-chain governance integration that reads it. |
| **Commission drain** | Each call also pays a commission from the GovPool reward token to the DeXe treasury, compounding the drain. |

## 6. Severity: **HIGH**

If combined with the validation-bypass bug (Path A), the attack can be
self-executing: a single malicious proposal sets the verifier to `address(0)`
and the attacker then drains rewards permissionlessly. Even without the
validation bypass, any governance decision (or deployer mistake) that sets
the verifier to zero permanently exposes the reward pool.

## 7. Three-Perspective Audit

### 7.1 Attacker perspective
The attacker needs only one successful governance proposal (or one
misconfiguration) to set the verifier to `address(0)`. After that, the drain
is permissionless and bounded only by the reward token balance and gas. The
loop is cheap: each iteration is a single `saveOffchainResults` call (a
string + a 1-byte signature).

### 7.2 Defender perspective
The protocol likely assumed `changeVerifier` would always be called with a
legitimate address. The `ECDSA.recover == address(0)` edge case is a
well-known footgun. The fix is a one-line `require(newVerifier != address(0))`
in `changeVerifier`, and ideally also reverting in `saveOffchainResults` when
`offChain.verifier == address(0)`.

### 7.3 Neutral perspective
This is a classic "zero-address validation missing" finding. It is
low-complexity but high-impact because the off-chain reward path is
permissionless once triggered. The `usedHashes` guard is correctly implemented
but does not help because the attacker controls the `resultsHash` input,
making each `signHash` unique.

## 8. Recommended Fix

```solidity
// GovPool.sol
function changeVerifier(address newVerifier) external override onlyThis {
    require(newVerifier != address(0), "Gov: zero verifier");
    _offChain.verifier = newVerifier;
}

// GovPoolOffchain.sol — defence-in-depth
function saveOffchainResults(…) external {
    require(offChain.verifier != address(0), "Gov: verifier not set");
    …
}
```
