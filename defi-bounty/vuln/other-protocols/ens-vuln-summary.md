# ENS Deep Audit Summary Report

## Audit Metadata
- **Target**: ENS (Ethereum Name Service) contracts
- **Repository**: https://github.com/ensdomains/ens-contracts
- **Commit**: 121dc23 (latest, ~Sept 2026)
- **Platform**: Immunefi (KYC Not Required, max bounty $250k)
- **Auditor**: Opus (automated deep audit)

## Scope

### New Contracts Audited (Added Recently, Less Audited)
1. **RegistrarSecurityController** — Break-glass controller for base registrar
2. **RootSecurityController** — Break-glass controller for ENS root
3. **CCIPBatcher** — CCIP-Read batch gateway client (had 3 recent security fixes)
4. **L2ReverseRegistrar** — L2 reverse name registrar with EIP-191 signatures
5. **L2ReverseRegistrarWithMigration** — Migration helper for L2 reverse registrar
6. **DefaultReverseRegistrar** — Default reverse registrar for all EVM chains
7. **StandaloneReverseRegistrar** — Base standalone reverse registrar
8. **SignatureUtils** — Signature validation with expiry (ERC6492 support)
9. **P256SHA256Algorithm** — DNSSEC Algorithm 13 using EIP-7951 precompile
10. **NameCoder** — DNS/ENS name encoding/decoding library
11. **ENSIP19** — Reverse name parsing per ENSIP-19
12. **ChainReverseResolver** — L2 reverse resolver via Unruggable Gateway
13. **ETHReverseResolver** — Ethereum reverse resolver
14. **DefaultReverseResolver** — Default reverse resolver
15. **AbstractReverseResolver** — Base reverse resolver
16. **ShuffledGatewayProvider** — Gateway provider with deterministic shuffling
17. **GatewayProvider** — Simple gateway URL storage
18. **MigrationHelper** — Name migration tool
19. **LibMem** — Memory manipulation library
20. **DataResolver** — ENSIP-24 arbitrary data resolution

### Existing Contracts Re-Audited
1. **NameWrapper** — ERC1155 wrapping, fuses, subname management
2. **ETHRegistrarController** — .eth name registration and renewal
3. **BaseRegistrarImplementation** — ERC721 base registrar
4. **Root** — ENS root TLD management
5. **ENSRegistry** — ENS core registry
6. **PublicResolver** — Public resolver with all profiles
7. **ReverseRegistrar** — Ethereum reverse registrar (addr.reverse)
8. **UniversalResolver** — Universal resolver with CCIP-Read
9. **OffchainDNSResolver** — DNS-based resolution with CCIP-Read
10. **DNSSECImpl** — DNSSEC oracle
11. **ERC1155Fuse** — ERC1155 with fuse storage

## Deep Audit Focus Areas Covered

| # | Focus Area | Status | Findings |
|---|-----------|--------|----------|
| 1 | Name Wrapper (unwrap, fuses) | ✅ Audited | Low: extendExpiry clears stored fuses |
| 2 | Resolver (cross-chain, L2) | ✅ Audited | No critical issues found |
| 3 | Registrar (registration, renewal, expiry) | ✅ Audited | No critical issues found |
| 4 | Registry access controls | ✅ Audited | No critical issues found |
| 5 | EIP-712 signature (replay, malleability) | ✅ Audited | Low: signature replay within 1hr window |
| 6 | Subname takeover (fuses, permissions) | ✅ Audited | No exploitable subname takeover found |
| 7 | Reentrancy via resolver callbacks | ✅ Audited | No exploitable reentrancy found |
| 8 | Upgradeable proxy storage slots | ✅ Audited | No proxy pattern issues found |
| 9 | Cross-chain ENS / L2 bridge | ✅ Audited | No critical cross-chain issues found |
| 10 | Gas/DoS vectors | ✅ Audited | Low: assert() in readTXT causes gas griefing |

## Bugs Found

### Summary
| Severity | Count |
|----------|-------|
| CRITICAL | 0 |
| HIGH | 0 |
| MEDIUM | 0 |
| LOW | 3 |
| **Total** | **3** |

### Bug #1: NameWrapper.extendExpiry Clears Stored Fuses on Expired Subnames
- **Severity**: LOW (Informational)
- **File**: `/home/z/ens-vuln-namewrapper-extendexpiry.md`
- **Root Cause**: `extendExpiry()` uses `getData()` (effective fuses, cleared on expiry) instead of `super.getData()` (raw stored fuses), writing 0 to storage.
- **Exploitability**: Not currently exploitable due to `_isWrapped()` guard preventing `extendExpiry` on PCC-burned expired subnames.
- **PoC**: `/home/z/ens-poc/test/ExtendExpiryFuseClear.t.sol` (2 passing tests)

### Bug #2: Signature Replay Within Expiry Window
- **Severity**: LOW
- **File**: `/home/z/ens-vuln-signature-replay.md`
- **Root Cause**: No nonce/nullifier in `setNameForAddrWithSignature()` and `setNameForOwnableWithSignature()` in L2ReverseRegistrar and DefaultReverseRegistrar.
- **Impact**: Attacker can revert a user's name change within the 1-hour expiry window.
- **Exploitability**: Griefing only — no fund loss.

### Bug #3: assert() in OffchainDNSResolver.readTXT() Causes Unbounded Gas Consumption
- **Severity**: LOW (DoS)
- **File**: `/home/z/ens-vuln-offchaindnsresolver-assert.md`
- **Root Cause**: `assert()` (INVALID opcode, consumes all gas) used for external data validation instead of `require()`.
- **Impact**: DNS zone owner can craft malformed TXT records to cause DoS/gas griefing.
- **Exploitability**: Limited — attacker is the DNS zone owner (can only DoS their own zone).

## Key Positive Findings

1. **CCIPBatcher security fixes were correctly applied** — The recent `UnsafeBatchGatewayResponse` wrapping (commit fe71b73) and `_toResponseArray` padding fix (commit eaedc77) properly prevent malicious batch gateway error data from being propagated as valid responses.

2. **NameWrapper fuse invariant is correctly enforced** — The `_canFusesBeBurned()` check ensures `PARENT_CANNOT_CONTROL | CANNOT_UNWRAP` must be burned before any non-parent-controlled fuse. The `_checkParentFuses()` check ensures parent-controlled fuses can only be burned if the parent has `CANNOT_UNWRAP`.

3. **Signature expiry is tightly bounded** — The `SignatureExpiryTooHigh` check limits signatures to 1-hour validity, reducing replay window.

4. **`_isWrapped()` guard correctly prevents `extendExpiry` on PCC-burned expired subnames** — This prevents the most dangerous exploitation of the fuse-clearing issue.

5. **Reverse resolution forward verification** — `AbstractReverseResolver.reverseAddressCallback()` correctly verifies that the forward address matches the lookup address, preventing reverse record spoofing.

6. **P256 precompile integration is safe** — `P256Precompile.verify()` correctly checks the staticcall return value and validates the output byte. If the precompile is not available, it returns false (safe default).

## Contracts With No Issues Found

- **RegistrarSecurityController** — Simple pass-through with proper `onlyOwner`/`onlyController` modifiers.
- **RootSecurityController** — Simple break-glass with `onlyOwner`.
- **BaseRegistrarImplementation** — Well-tested, no new issues.
- **ENSRegistry** — Standard registry, no issues.
- **ETHRegistrarController** — Commitment/reveal registration is sound.
- **GatewayProvider / ShuffledGatewayProvider** — Simple storage, no issues.
- **MigrationHelper** — Owner-only migration, no issues.
- **NameCoder** — Correct encoding/decoding with proper validation.
- **ENSIP19** — Correct reverse name parsing.
- **HexUtils** — Correct hex parsing with right-alignment.
- **LibMem** — Correct memory copy implementation.

## Recommendation

**DO NOT SUBMIT** — No CRITICAL or HIGH severity bugs were found. The 3 LOW severity issues are documented for the team's awareness but do not meet the threshold for Immunefi submission. The ENS contracts demonstrate strong security practices, particularly in the recently-added CCIPBatcher (which had 3 security fixes applied correctly) and the NameWrapper fuse system.
