# ENS Vulnerability Report: assert() in OffchainDNSResolver.readTXT() Causes Unbounded Gas Consumption

## Severity: LOW (DoS)

## Summary

The `OffchainDNSResolver.readTXT()` function uses `assert()` to validate TXT record field lengths. In Solidity 0.8+, `assert()` compiles to the `INVALID` opcode (0xFE), which consumes **all remaining gas**. If a DNSSEC-verified TXT record has a malformed field length that triggers the assertion, the entire `resolveCallback()` transaction reverts with out-of-gas, causing a denial of service for DNS-based ENS resolution.

## Contract & Function & Line

- **Contract**: `OffchainDNSResolver.sol`
- **Function**: `readTXT(bytes memory data, uint256 startIdx, uint256 lastIdx)`
- **Line**: 191

```solidity
function readTXT(
    bytes memory data,
    uint256 startIdx,
    uint256 lastIdx
) internal pure returns (bytes memory) {
    // TODO: Concatenate multiple text fields
    uint256 fieldLength = data.readUint8(startIdx);
    assert(startIdx + fieldLength < lastIdx);  // <-- LINE 191: uses assert(), not require()
    return data.substring(startIdx + 1, fieldLength);
}
```

## Root Cause

`assert()` in Solidity 0.8+ uses the `INVALID` (0xFE) opcode, which:
1. Consumes ALL remaining gas (no refund).
2. Should only be used for invariant checks that should never fail under any circumstances.

The condition `startIdx + fieldLength < lastIdx` is **not** an invariant — it depends on the DNS TXT record data, which is provided by the DNS zone owner. A DNS zone owner (who controls their DNS records) could craft a TXT record with a `fieldLength` that violates this condition.

While the DNS record is verified by the DNSSEC oracle (signature is valid), the **format** of the TXT record's rdata is not validated by the oracle — only the signature is checked. A DNS zone owner could publish a validly-signed TXT record with a malformed field length.

## Attack Scenario

1. DNS zone owner for `example.com` publishes a TXT record at `_ens.example.com` with a malformed rdata field length.
2. The TXT record is signed by the DNS zone's DNSSEC key (valid signature).
3. A user tries to resolve `example.com` using the `OffchainDNSResolver`.
4. The gateway fetches the DNSSEC proof and submits it to `resolveCallback()`.
5. `readTXT()` is called with the malformed data.
6. `assert(startIdx + fieldLength < lastIdx)` fails.
7. The transaction reverts with **out-of-gas**, consuming all gas sent by the caller.
8. The resolution permanently fails for `example.com` as long as the malformed TXT record exists.

## Impact

- **DoS on DNS-based resolution**: The resolution of the affected DNS name permanently fails with out-of-gas.
- **Gas griefing**: The caller loses all gas sent for the transaction.
- **Scope**: Only affects DNS names using `OffchainDNSResolver`. `.eth` names are not affected.
- **Trust model**: The DNS zone owner is the attacker. They must control the DNS zone and have valid DNSSEC signatures. This is within the DNS zone owner's capabilities.

## PoC (Conceptual)

```
DNS TXT record for _ens.example.com:
  rdata = 0xFF (fieldLength = 255, but actual data is only 10 bytes)
  signed by DNSSEC key (valid signature)

When resolveCallback() processes this:
  fieldLength = 255
  startIdx + fieldLength = rdataOffset + 255
  lastIdx = nextOffset (e.g., rdataOffset + 11)
  assert(rdataOffset + 255 < rdataOffset + 11) → FAILS → out-of-gas
```

## 3-Perspective Audit

### Prosecutor (Arguments for vulnerability)
- Using `assert()` for external data validation is a well-known anti-pattern. The Solidity documentation explicitly states: "The assert function creates an error of type Panic(uint256) ... Assert should only be used to test for internal errors, and to check invariants." The TXT record data is external (from DNS), not an internal invariant. A DNS zone owner can craft a malformed record to trigger the assertion, causing DoS and gas griefing for any user trying to resolve the name.

### Defense (Arguments against vulnerability)
- The DNS zone owner is the "attacker" — they can only DoS their own zone's resolution. If they want their DNS name to resolve via ENS, they wouldn't publish malformed TXT records.
- The `OffchainDNSResolver` is only used for DNS names, not `.eth` names. The impact is limited to DNS-based ENS resolution.
- The `assert` condition `startIdx + fieldLength < lastIdx` should always hold for well-formed DNS TXT records (the field length is bounded by the RR's rdata length, which is bounded by `lastIdx - startIdx`). The assertion is checking a format that should be correct for valid DNS records.
- The gas griefing is bounded by the block gas limit.

### Judge (Verdict)
- **Low severity.** The `assert()` should be changed to `require()` for proper error handling and gas refund, but the practical impact is limited. The "attacker" is the DNS zone owner who can only DoS their own zone. The condition is a format check that should hold for valid DNS records. The fix is trivial: replace `assert` with `require` and return empty bytes on failure (or revert with a descriptive error).

## Recommendation

Replace `assert()` with `require()` and handle the error gracefully:

```solidity
function readTXT(
    bytes memory data,
    uint256 startIdx,
    uint256 lastIdx
) internal pure returns (bytes memory) {
    uint256 fieldLength = data.readUint8(startIdx);
    if (startIdx + fieldLength >= lastIdx) {
        return ""; // or revert with a descriptive error
    }
    return data.substring(startIdx + 1, fieldLength);
}
```

Additionally, consider using `<` instead of `<=` (the current check is `<`, which means a TXT field that extends exactly to `lastIdx` would fail — this might be too strict depending on the DNS RR format).
