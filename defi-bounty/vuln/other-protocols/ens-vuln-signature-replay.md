# ENS Vulnerability Report: Signature Replay Within Expiry Window in L2ReverseRegistrar / DefaultReverseRegistrar

## Severity: LOW

## Summary

The `setNameForAddrWithSignature()` and `setNameForOwnableWithSignature()` functions in `L2ReverseRegistrar` and `DefaultReverseRegistrar` allow signature replay within the 1-hour expiry window. There is no nonce or nullifier mechanism to prevent the same valid signature from being submitted multiple times. An attacker who observes a valid signature (from the mempool or a prior transaction) can replay it to overwrite a user's subsequently-changed reverse name, reverting it to the signed value.

## Contracts & Functions & Lines

1. **`L2ReverseRegistrar.sol`** — `setNameForAddrWithSignature()` (lines ~74-96)
2. **`L2ReverseRegistrar.sol`** — `setNameForOwnableWithSignature()` (lines ~99-125)
3. **`DefaultReverseRegistrar.sol`** — `setNameForAddrWithSignature()` (lines ~31-51)
4. **`SignatureUtils.sol`** — `validateSignatureWithExpiry()` (lines ~41-62)

## Root Cause

The signed message includes:
- `address(this)` (contract address — chain-specific binding)
- Function selector
- `addr` / `contractAddr` / `owner`
- `signatureExpiry` (constrains validity to a 1-hour window)
- `name`
- `coinTypes` (L2 only — cross-chain replay is intended)

**Missing**: There is no nonce, sequence number, or nullifier. The same `(addr, name, signatureExpiry, coinTypes, signature)` tuple can be submitted multiple times until `signatureExpiry` passes.

## Attack Scenario

1. User `U` signs a message to set their L2 reverse name to `"old.eth"` with `signatureExpiry = T + 1 hour`.
2. The transaction is submitted and mined. The name is set to `"old.eth"`. The signature is now public (in the transaction calldata).
3. `U` later calls `setName("new.eth")` directly (no signature) to update their name.
4. Within the 1-hour window (before `T + 1 hour`), attacker `A` replays the original signature by calling `setNameForAddrWithSignature(U, T+1h, "old.eth", coinTypes, signature)`.
5. The signature is still valid (not expired), so the name is reverted to `"old.eth"`.

## Impact

- **Griefing**: An attacker can repeatedly revert a user's name change within the 1-hour window. The user must wait until the old signature expires before their new name change sticks.
- **No fund loss**: No assets are at risk. The attacker pays gas for the replay.
- **Window**: Maximum 1 hour (limited by `SignatureExpiryTooHigh` check: `signatureExpiry > block.timestamp + 1 hours`).

## PoC (Foundry — conceptual)

```solidity
// 1. User signs setNameForAddrWithSignature(addr, expiry, "old.eth", coinTypes, ...)
// 2. Transaction mined → name = "old.eth"
// 3. User calls setName("new.eth") → name = "new.eth"
// 4. Attacker replays the original signature (still within 1-hour window)
// 5. name = "old.eth" again (reverted)

// The contract has no nonce check, so step 4 succeeds.
```

## 3-Perspective Audit

### Prosecutor (Arguments for vulnerability)
- The absence of a nonce/nullifier is a well-known anti-pattern in signature-based operations. The EIP-712 typed data structure includes `signatureExpiry` but no `nonce`. While the 1-hour window limits the impact, an attacker can still grief users by reverting name changes. In scenarios where the reverse name is used for identity verification (e.g., displaying "old.eth" instead of "new.eth" on dApps), this could cause confusion or reputational damage.

### Defense (Arguments against vulnerability)
- The 1-hour expiry window is intentionally tight, limiting the attack window.
- The `address(this)` in the signed message binds the signature to a specific contract instance, preventing cross-contract replay (except for the intended `coinTypes` cross-chain replay).
- The impact is purely griefing — no funds or assets are at risk.
- The user can simply re-submit their new name change after the old signature expires.
- This is a known design tradeoff: using a nonce would require an on-chain storage write per signature, increasing gas costs for the common case.

### Judge (Verdict)
- **Low severity.** The signature replay is real but the impact is limited to short-term griefing within a 1-hour window. No funds are at risk. The design tradeoff (no nonce for gas efficiency) is reasonable for the use case (reverse name registration, not asset transfer). A nonce could be added as an optional parameter for users who want replay protection, but the current design is not a security vulnerability per se.

## Recommendation

Optional improvement: Add an optional `nonce` parameter to the signed message and track used nonces:
```solidity
mapping(address => uint256) public nonces;
// Include nonces[addr]++ in the signed message
```

Or use a bitmap nullifier for signatures that have been consumed, allowing the same signature to only be used once.
