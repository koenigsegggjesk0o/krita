# Hedera Deep Audit — Summary

**Target:** Hedera (HTS, HCS, smart contracts / system-contract precompiles)
**Scope source:** Immunefi bug-bounty `https://immunefi.com/bug-bounty/hedera/scope/` (max bounty $30,000)
**In-scope repos (per Immunefi page):**
- `hiero-ledger/hiero-consensus-node` (mirrored as `hashgraph/hedera-services`) — consensus node; contains HTS/HCS/HAS/HSS native system-contract precompiles
- `hiero-ledger/hiero-cryptography`
- `hiero-ledger/hiero-json-rpc-relay`
- `hiero-ledger/hiero-mirror-node`
- `hiero-ledger/hiero-sdk-{go,java,js}`
- `hashgraph/hedera-transaction-tool`

**Repos cloned & reviewed:**
- `/home/z/hedera/services` → `hashgraph/hedera-services` @ `51f889b` (v0.79.0-SNAPSHOT)
- `/home/z/hedera/smart-contracts` → `hashgraph/hedera-smart-contracts` @ `43854b5`
- `/home/z/hedera/protobufs`, `/home/z/hedera/sdk-java`

**Immunefi scope text of note:**
> "Hedera Token Service functions exposed via System contracts (e.g. transferring assets out of an account without authorization)"
This was the primary north-star for the audit (the exact bug class Hedera explicitly asks reviewers to hunt).

---

## Result

**No CRITICAL or HIGH-severity bug was confirmed.**

After a deep, line-level review of the highest-value attack surfaces (HTS mint/burn/transfer/KYC/freeze/approve/airdrops, HCS submit-message, HAS `isAuthorized`/`isAuthorizedRaw`/`hbarApprove`, HSS `scheduleNative`/`signSchedule`, and EIP-7702 code-delegation processing), the access-control model proved sound:

- Every privileged HTS/HAS synthetic dispatch is gated by the dispatched handler's `preHandle`, which calls
  `PreHandleContext.requireKeyOrThrow(owner/admin/receiver/sender, …)`.
- Contract-call dispatches use `ActiveContractVerificationStrategy`, which auto-satisfies **only** the calling
  contract's own `CONTRACT_ID`/`DELEGATABLE_CONTRACT_ID` key. Every other required key still needs a real
  cryptographic signature present in the transaction signature map.
- Keyless / hollow accounts are *not* a bypass: `requireKeyOrThrow` throws on a null/empty key
  (`INVALID_ALLOWANCE_OWNER_ID` / `INVALID_ACCOUNT_ID`), and `requireAliasedKeyOrThrow` /
  `requireSignatureForHollowAccount` require the ECDSA alias signature for hollow accounts.
- `isApproval` flags on synthetic `CryptoTransfer` debits are derived correctly (true iff the debit is not the
  sender's own account), so a contract cannot drain a non-approved account.
- The EIP-7702 → `hbarApprove` redirect chain (the most promising new-surface candidate) is blocked because the
  delegated hollow account keeps a Hedera key equal to the authorizer's ECDSA key, and an existing hollow account
  cannot receive a delegation without `isRegularAccount()` and nonce checks — and even if delegated, the
  `CryptoApproveAllowance` preHandle still requires the owner key.

A small number of **Low / Informational** issues were identified and documented individually:

| # | Area | File(s) | Severity | File |
|---|------|---------|----------|------|
| 1 | HAS / signature-verification gas | `IsAuthorizedCall.java` | Low–Medium (gas underpricing, bounded) | `hedera-vuln-has-isauthorized-gas.md` |
| 2 | HTS/HAS / int256→long truncation | `Erc20TransfersTranslator.java`, `HbarApproveTranslator.java` | Low (self-inflicted gas griefing) | `hedera-vuln-hts-int256-truncation.md` |

Both are documented with the contract+function+line, attack scenario, PoC sketch, impact, severity, and a
three-perspective audit (defender / attacker / protocol-economics).

---

## Areas reviewed (negative results documented for reproducibility)

1. **HTS mint/burn/KYC/freeze** — translators build synthetic `TokenMint/Burn/Freeze/GrantKyc` bodies and dispatch;
   authorization is delegated to handler preHandle (admin/supply/kyc/freeze keys). No bypass found.
2. **HTS transfer (classic + ERC-20 + ERC-721)** — `isApproval` derived correctly; `transferFrom` forces
   `requiresApproval=true`; ERC-721 `isApproval` derived from *actual* NFT owner (`getOwner()`), not the
   caller-supplied `from`. Ownership mismatch caught by handler. No bypass found.
3. **HTS grantApproval / setApprovalForAll** — NFT delegate-approve path requires `approveForAll`; revocation path
   keyed on spender `0.0.0`. No bypass found.
4. **HTS airdrops (airdrop / claim / cancel)** — `TokenClaimAirdropHandler.preHandle` requires receiver key;
   `TokenCancelAirdropHandler.preHandle` requires sender key. Classic decoder takes receiver/sender from user
   tuples but the key requirement still binds. No bypass found.
5. **HCS submit-message** — `ConsensusSubmitMessageHandler`: submit-key enforced in preHandle; chunk-info payer
   consistency validated; running-hash computed deterministically (SHA-384 over Java-serialized fields — opaque but
   stable across homogeneous JVMs; `RUNNING_HASH_VERSION=3`). No ordering/replay bypass found.
6. **HAS `isAuthorized` / `isAuthorizedRaw`** — signature verification keyed on the looked-up account's own key;
   EC-vs-ED disambiguated by signature length; `isLongZero` guard for EC per HIP-632; `v`/chainId not re-validated
   in raw path (acceptable: raw caller supplies the hash directly, public key already known). See Issue #1 for gas.
7. **HAS `hbarApprove`** — owner signature required via `CryptoApproveAllowanceHandler.preHandle`; proxy variant
   uses `redirectAccount` as owner but key requirement still binds. See Issue #2 for truncation.
8. **HSS `scheduleNative` / `signSchedule` / `authorizeSchedule`** — `scheduleNative` validates inner call is a
   create/update selector and inner contract == `0x167`; `signSchedule` rebuilds keys from verified sig-pairs over
   `messageFromScheduleId(scheduleId)` and only accepts keys with a valid cryptographic signature. No forgery path.
9. **EIP-7702 `CodeDelegationProcessor`** — chain-id, `s` ≤ half-order (EIP-2), `yParity < 256`, nonce match,
   `canSetCodeDelegation` (empty or already-delegated code), `isRegularAccount` checks all present and ordered per
   EIP-7702. Lazy-created authority accounts are created **with** the authorizer's public key. No keyless-delegation
   bypass found.
10. **Solidity reference contracts** (`hedera-smart-contracts`) — `WHBAR` (WETH9 clone, CEI-correct),
    `HRC719TokenProxy` (pure redirect to `0x167`), `Multicall3` (canonical), diamond `LibDiamond` (canonical
    EIP-2535), `BLS`/`Pairing` (reference). These are example/reference contracts, not in-scope production value
    handlers, and contain no novel defect.

## Conclusion

The Hedera consensus-node system-contract layer is mature and defended in depth: the preHandle key-requirement
plus the `ActiveContractVerificationStrategy` together form a consistent authorization boundary that we could not
bypass for any asset-moving operation. The two documented findings are low-severity gas/robustness issues that do
not permit theft, freeze, or unauthorized transfer of assets and therefore do not meet the CRITICAL/HIGH bar of
the Immunefi program. They are recorded for completeness only.

**No submission was made.**
