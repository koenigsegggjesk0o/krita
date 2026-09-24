# Finding #1 — `isAuthorized(address,bytes,bytes)` gas does not account for caller-supplied signature-map size

**Severity:** Low–Medium (gas underpricing / bounded DoS amplification)
**Component:** Hedera Account Service (HAS) system contract, `0x169`
**Immunefi scope:** `hiero-consensus-node` / `hedera-smart-contract-service-impl`
**Repo / commit:** `hashgraph/hedera-services` @ `51f889b` (v0.79.0-SNAPSHOT)

## Contract + function + line

File:
`hedera-node/hedera-smart-contract-service-impl/src/main/java/com/hedera/node/app/service/contract/impl/exec/systemcontracts/has/isauthorized/IsAuthorizedCall.java`

```
L78   final var keyCounts = signatureVerifier.countSimpleKeys(key);
L79   final long gasRequirement = keyCounts.numEcdsaKeys()
L80                   * hederaGasCalculatorImpl.getEcrecPrecompiledContractGasCost()
L81           + keyCounts.numEddsaKeys() * hederaGasCalculatorImpl.getEdSignatureVerificationSystemContractGasCost();
L83   final var authorized = verifyMessage(
L84           key, wrap(message), MessageType.RAW, sigMap, ky -> SimpleKeyStatus.ONLY_IF_CRYPTO_SIG_VALID);
```

Selector declared in `IsAuthorizedTranslator.IS_AUTHORIZED = "isAuthorized(address,bytes,bytes)"`.
The caller-controlled `bytes signatureBlob` (ABI arg index 2) is parsed at L72 into a `SignatureMap`:

```
L70   SignatureMap sigMap;
L71   try {
L72       sigMap = requireNonNull(SignatureMap.PROTOBUF.parseStrict(wrap(signatureBlob)));
```

## Description

`isAuthorized` lets a smart contract verify that a Hedera account signed an arbitrary message. Its gas is priced
from the **account's** key structure only: `countSimpleKeys(account.key())` (`AppSignatureVerifier.countSimpleKeys`
walks `KeyList`/`ThresholdKey` and counts ED25519 + ECDSA leaves).

The actual work performed by `AppSignatureVerifier.verifySignature` is, however, a function of **both** the
account key and the caller-supplied `SignatureMap`:

1. `SignatureMap.PROTOBUF.parseStrict(signatureBlob)` — O(blob size) protobuf parse, **not** priced.
2. `signatureExpander.expand(key, signatureMap.sigPair(), sigPairs)` — for each simple key in the account key, it
   scans every `SignaturePair` in the caller's map for a matching `pubKeyPrefix`. Complexity
   `O(numSimpleKeys × |sigPairs|)`, where `|sigPairs|` is fully caller-controlled.

A caller can therefore supply an account whose key is a single ECDSA leaf (gas = 1 × `ECREC` cost) together with a
`SignatureMap` carrying the maximum number of (junk) `SignaturePair`s that fit in `transactionMaxBytes` (6144 B).
The node performs up to `1 × N` prefix comparisons and the protobuf parse for the price of one ECDSA verification.

This is the *non-raw* path specifically. The sibling `isAuthorizedRaw(address,bytes,bytes)` (HIP-632) prices gas
from the **signature length** (`EC_SIGNATURE_MIN_LENGTH..MAX_LENGTH` vs `ED_SIGNATURE_LENGTH`) and is **not**
affected.

## Attack scenario

1. Deploy a victim/caller contract `C`.
2. Pick any existing Hedera account `A` whose key is a single ECDSA(secp256k1) key (the cheapest case for the
   attacker; gas charged ≈ one ECREC precompile = 3000 gas).
3. From `C`, call `HAS.isAuthorized(A, message, signatureBlob)` where `signatureBlob` is a serialized
   `SignatureMap` packed with the maximum number of `SignaturePair` entries (each a minimal `pubKeyPrefix` + a
   64-byte `ecdsaSecp256k1` or 64-byte `ed25519` value) that fits in `transactionMaxBytes` (≈ 6 KB ⇒ on the order
   of 50–60 pairs).
4. The call returns `authorized=false` (the junk pairs don't match `A`'s key) but the node has done ≈ 60 prefix
   comparisons + a 6 KB protobuf parse for the price of one ECDSA verify.

### PoC sketch (Hedera JSON-RPC / Solidity)

```solidity
// Caller contract
interface HAS { function isAuthorized(address a, bytes memory msg_, bytes memory sig) external returns (int64, bool); }

contract GasGrief {
    address constant HAS = address(0x169);
    function grief(address victim, bytes memory message, bytes memory fatSigMap) external {
        // fatSigMap is built off-chain as a protobuf SignatureMap with N junk SignaturePairs,
        // sized to <= transactionMaxBytes (6144).
        (int64 rc, bool ok) = HAS(HAS).isAuthorized(victim, message, fatSigMap);
        // rc == 22 (SUCCESS), ok == false; node did O(N) prefix work for ~1 ECREC gas.
    }
}
```

Off-chain, build `fatSigMap` with e.g. 50 `SignaturePair { pubKeyPrefix: 0x00, ecdsaSecp256k1: <64 random bytes> }`.

## Impact

- **Theft / unauthorized transfer:** None. The function is a read-only view call (`isViewCall = true`); it cannot
  move assets.
- **DoS amplification:** Limited. The `signatureBlob` is hard-capped by `transactionMaxBytes = 6144` bytes (see
  `umbrella.properties: transactionMaxBytes=6144`; enforced in
  `AbstractNativeSystemContract.computeFully` L123: `input.size() <= maxInputBytes`). The amplification is one
  cheap byte-prefix comparison per extra `SignaturePair`, **not** an extra cryptographic verification — the
  cryptographic work is still bounded by `countSimpleKeys(account.key())`, because the expander only emits
  `ExpandedSignaturePair`s whose prefix matches a key in the account key. Net amplification ≈ `|sigPairs|`
  × (cheap compare) for the price of one ECDSA verify.
- **Economic:** The caller pays gas for the call and receives no benefit, so this is at most a
  self-inflicted/indirect network-load amplifier, not a profitable attack.

## Severity rationale

Low–Medium. The issue is real (gas does not reflect the caller-controlled `|sigPairs|` factor or the protobuf parse
cost), but the blast radius is bounded by `transactionMaxBytes` and the extra work is non-cryptographic. No path to
asset theft, fund loss, or consensus breakdown was identified. It does not meet the CRITICAL/HIGH threshold of the
Hedera Immunefi program.

## Suggested fix

Price the gas as a function of the parsed `SignatureMap` size, e.g.:

```java
final int numSigPairs = sigMap.sigPair().size();
final long gasRequirement =
    keyCounts.numEcdsaKeys() * ecrecCost
  + keyCounts.numEddsaKeys() * edCost
  + numSigPairs * SIG_PAIR_SCAN_GAS            // charge for prefix-scan work
  + signatureBlob.length * PARSE_GAS_PER_BYTE; // charge for protobuf parse
```

(Compute `numSigPairs` *after* `parseStrict` and *before* `verifyMessage`; reject `numSigPairs` above a small cap.)

## Three-perspective audit

**Defender (Hedera team) perspective.**
The gas formula mirrors the *cryptographic* cost (number of leaf keys × per-verify cost), which is the dominant
cost in the common case. The caller-controlled `|sigPairs|` factor only adds cheap prefix comparisons, and the
input is hard-bounded by `transactionMaxBytes`. The realistic amplification is therefore small and not a
consensus-level concern. The fix is cheap and worth doing for gas-accuracy hygiene and to remove a (minor)
network-load amplifier, but it is not urgent.

**Attacker perspective.**
No profit vector. The call is `static`/view and returns `false` for junk pairs, so it cannot be turned into a
theft primitive. The only "benefit" is forcing the network to do slightly more work than the gas reimburses — but
the attacker pays the gas anyway, and the work is bounded to a 6 KB parse + tens of prefix comparisons per call.
At Hedera's gas/tinybar pricing this is not an economically attractive DoS vector versus simply sending many
cheap transactions.

**Protocol-economics perspective.**
Gas should track worst-case compute to keep fee markets honest and to prevent a future relaxation of
`transactionMaxBytes` (e.g. jumbo transactions, already partially present via `JumboTransactionsConfig`) from
silently magnifying this into a real amplification. Adding a `|sigPairs|` and `signatureBlob.length` term future-
proofs the pricing and closes the gap with `isAuthorizedRaw`, which already prices from the (bounded) signature
length. Recommend fixing before any further max-tx-size increase.

## References

- HIP-632 (Account Service `isAuthorized` / `isAuthorizedRaw`).
- `SignatureMapUtils.stripRecoveryIdFromEcdsaSignatures` / `preprocessEcdsaSignatures` (sibling utility).
- `AppSignatureVerifier.verifySignature` → `SignatureExpander.expand(key, sigPairs, expanded)`.
