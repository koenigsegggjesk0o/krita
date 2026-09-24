# Lombard Finance Deep Audit — Summary & No-Finding Areas

**Audit Date:** September 2026
**Scope:** Lombard Finance EVM smart contracts (Bitcoin LST, Ethereum)
**Repo:** `https://github.com/lombard-finance/evm-smart-contracts` @ commit `7fe83e5`
**Auditor:** Opus (automated deep audit)

---

## Summary

| # | Finding | Severity | File |
|---|---------|----------|------|
| 1 | Fee Approval EIP-712 Signature Replay | MEDIUM | `lombard-vuln-fee-approval-replay.md` |
| 2 | Consortium Validator Set Duplicate Check Missing | MEDIUM | `lombard-vuln-consortium-duplicate-validators.md` |
| 3 | BlocklistOracle address(0) Check Freezes Strategy | LOW-MEDIUM | `lombard-vuln-blocklist-address-zero.md` |
| 4 | GMPBasculeV2 Guardian Role Not Renounced | LOW | `lombard-vuln-gmpbascule-guardian.md` |
| 5 | IBCVoucher uint64 Truncation on Rate Limit | LOW | `lombard-vuln-ibcvoucher-uint64-truncation.md` |

**No CRITICAL or HIGH vulnerabilities found.** All findings are MEDIUM or below. The codebase shows evidence of multiple prior audits (Halborn, OpenZeppelin, Veridise, Sherlock, ABDK) and the contracts are well-hardened.

---

## Areas Audited With NO Critical/High Bugs Found

### 1. LBTC Token — Mint/Burn Access Control, ERC2612 Permit

**Contracts reviewed:** `BaseLBTC.sol`, `NativeLBTC.sol`, `StakedLBTC.sol`, `ShareToken.sol`, `BridgeTokenAdapter.sol`

**Findings:**
- Mint/burn access control is properly gated by `MINTER_ROLE` with rate limiting (`RateLimitsV2`).
- Proof-based mints (`mintV1`) correctly use `usedPayloads[sha256(payload)]` for replay protection.
- `batchMintV1` skips already-used payloads (no revert, emits `BatchMintSkipped`).
- ERC2612 permit is inherited from OZ `ERC20PermitUpgradeable` — standard, no issues.
- `pauseTransfers` / `pauseMintBurn` are properly separated with distinct roles.
- `burn(address, amount)` requires `MINTER_ROLE` — no allowance check, but this is by design (the `AssetRouter` uses it for fee collection and the user authorizes via EIP-712 signature).
- **No inflation attack** on LBTC itself (not an ERC4626 vault).

### 2. Vault — ERC4626 Inflation Attack, Withdrawal Queue

**Contracts reviewed:** `MultiAssetDepositUpgradeable.sol`, `StrategyBaseUpgradeable.sol`, `AsyncRedemptionUpgradeable.sol`, `FeeManagerUpgradeable.sol`, `MultiAssetWithAsyncRedemptionStrategy.sol`

**Findings:**
- **No classic ERC4626 inflation attack:** The strategy uses an operator-posted `pricePerShare` (PPS) model, NOT a balance-based NAV. Direct token donations to the strategy do NOT affect PPS, so donation attacks are ineffective.
- PPS changes are rate-limited (`PriceRateLimiter`) and excess moves trigger deposit+redeem pause.
- `AsyncRedemptionUpgradeable.requestRedeem` burns shares and records `pendingAssets` at the current PPS. `fulfillRedeemRequests` pays `min(pendingAssets, currentPPS × shares)`, protecting the vault from overpaying on PPS drops.
- Same-block flash-loan prevention: `fulfillRedeemRequests` enforces `block.number > requestBlock`.
- Management fee accrual uses the correct gross-up formula (`totalSupply * f / (1 - f)`) to account for self-dilution.
- Performance fee harvest only triggers when PPS exceeds `highWaterMark`.
- **No withdrawal queue front-running:** Redemption requests are sequential (requestBlock-based) and fulfilled by `PAY_REDEMPTIONS_ROLE`.

### 3. Bridge — Bitcoin↔Ethereum Peg, Custodian

**Contracts reviewed:** `BridgeV2.sol`, `Bridge.sol` (deprecated), `LombardTokenPoolV2.sol`, `LBTCOFTAdapter.sol`, `LBTCBurnMintOFTAdapter.sol`

**Findings:**
- `BridgeV2._deposit` properly burns tokens before sending the GMP message.
- `BridgeV2.handlePayload` checks `msg.sender == mailbox`, validates `sourceBridge`, and uses `payloadSpent` for replay protection.
- `BridgeV2._withdraw` checks rate limits BEFORE minting.
- `LombardTokenPoolV2.releaseOrMint` verifies `payloadHash == sourcePoolData` to ensure the delivered payload matches the CCIP message.
- OFT adapters use `EfficientRateLimiter` for net-flow rate limiting (outbound and inbound offset each other).
- The deprecated `Bridge.sol` is marked `@deprecated` and not in active scope.
- **No custodian bypass:** BTC custody is managed off-chain by the consortium; on-chain contracts only mint/burn LBTC representations based on consortium proofs.

### 4. Signature Verification — BLS, EIP-712

**Contracts reviewed:** `Consortium.sol`, `EIP1271SignatureUtils.sol`, `Assert.sol`

**Findings:**
- Consortium uses ECDSA (not BLS) for validator signatures. Each validator signs `sha256(payload)` with raw ECDSA (no EIP-191 prefix) — this is intentional for Cosmos SDK keyring compatibility.
- Signature malleability (no low-S enforcement) is not exploitable because the attacker cannot produce signatures for different messages.
- EIP-1271 signature verification correctly handles both EOA (ECDSA) and contract (EIP-1271) signers, including EIP-7702 delegation.
- **The fee approval EIP-712 signature has a missing-nonce issue** (see Finding #1).

### 5. Reentrancy via Callbacks

**Contracts reviewed:** All contracts with external calls.

**Findings:**
- All state-changing entrypoints use `ReentrancyGuardUpgradeable` (`nonReentrant`).
- `NativeLBTC.mintV1` / `batchMintV1` — `mintV1` is `nonReentrant`; `batchMintV1` calls `mintV1` internally (guard applies per-iteration).
- `AssetRouter.mint` / `mintWithFee` / `deposit` / `redeem` — all `nonReentrant`.
- `BridgeV2._deposit` / `handlePayload` — both `nonReentrant`.
- `Mailbox.send` / `deliverAndHandle` — both `nonReentrant`.
- `StrategyBaseUpgradeable.requestFunding` — `nonReentrant`.
- `MultiAssetDepositUpgradeable._deposit` — `nonReentrant`, pulls tokens before minting shares (CEI pattern).
- `AsyncRedemptionUpgradeable.requestRedeem` / `fulfillRedeemRequests` — both `nonReentrant`.
- **No reentrancy vectors found.** The codebase consistently uses `nonReentrant` and follows checks-effects-interactions.

### 6. Upgradeable Proxy — Storage Slots

**Contracts reviewed:** All upgradeable contracts.

**Findings:**
- All contracts use ERC-7201 namespaced storage (not `__gap` arrays). This is the modern OZ-recommended pattern and prevents storage collisions between inherited contracts.
- Storage slot constants are correctly computed: `keccak256(abi.encode(uint256(keccak256("...")) - 1)) & ~bytes32(uint256(0xff))`.
- `StakedLBTC` correctly preserves legacy storage fields via `@custom:oz-renamed-from` annotations for the migration from the old `LBTC` contract.
- `migrateToAccessControl` is a `reinitializer(3)` — properly versioned.
- `ProxyFactory` uses `CREATE3` for deterministic deployment with `TransparentUpgradeableProxy`.
- **No storage collision vulnerabilities found.**

### 7. Cross-Chain Message Verification

**Contracts reviewed:** `Mailbox.sol`, `GMPUtils.sol`, `MessagePath.sol`, `Assets.sol`

**Findings:**
- `Mailbox.deliverAndHandle` verifies consortium proof on first delivery, marks `deliveredPayload[hash] = true`.
- Handler errors are caught (try/catch) — payload can be retried without a new proof (intended design).
- Each handler (`AssetRouter`, `BridgeV2`) has its own replay protection (`usedPayloads`, `payloadSpent`).
- Message paths are explicitly enabled/disabled by admin (`enableMessagePath` / `disableMessagePath`).
- `msgDestinationCaller` check ensures only the designated caller can execute the message (or anyone if set to zero).
- Inbound message path is verified: `payload.msgSender == sourceBridge` for BridgeV2.
- **No cross-chain message forgery vectors found.** All messages require consortium signatures.

### 8. Fee Distribution — Precision, Rounding

**Contracts reviewed:** `Validation.sol`, `FeeUtils.sol`, `FeeManagerUpgradeable.sol`, `MultiAssetDepositUpgradeable.sol`, `AssetRouter.sol`

**Findings:**
- Deposit fee (`_splitDepositFeeView`) rounds UP (Ceil) — favorable to protocol.
- Redeem fee is a flat `redeemFee` + relative `toNativeCommission` — correctly computed as `amount - redeemFee - toNativeCommission`.
- Management fee uses the correct gross-up formula to account for self-dilution.
- Performance fee uses two-step `mulDiv` to avoid overflow.
- `Math.min(maximumMintCommission, feeAction.fee)` correctly caps the fee.
- BTCB/CBBTC PMM swap rounds in favor of the user (pays slightly less BTCB for the same LBTC) — this is by design (round-trip computation).
- **No precision loss or rounding exploits found.**

### 9. Blacklist/Whitelist Bypass

**Contracts reviewed:** `BlocklistOracle.sol`, `DepositNotarizationBlacklist.sol`, `StrategyBaseUpgradeable.sol`

**Findings:**
- `BlocklistOracle.check` verifies both manual blocklist and external sanction lists.
- Allowlisted accounts bypass sanction checks (but NOT manual blocklist).
- `StrategyBaseUpgradeable._update` checks blocklist on every transfer, mint, and burn — no bypass for share movements.
- `DepositNotarizationBlacklist` allows blocking specific BTC deposit outputs (txId + vout) from being notarized.
- **The `address(0)` check on mint/burn is a latent DoS** (see Finding #3), but not a bypass.
- **No blacklist bypass found** for real addresses.

### 10. Flash Loan Attack on Exchange Rate

**Contracts reviewed:** `StakedLBTCOracle.sol`, `StrategyBaseUpgradeable.sol`, `AssetRouter.sol`

**Findings:**
- `StakedLBTCOracle` ratio updates require consortium proof and are rate-limited (`ratioThreshold` bounds the per-day change). A flash loan cannot manipulate the ratio because it's set by the consortium, not by market activity.
- Strategy PPS is operator-posted and rate-limited — flash loans cannot manipulate PPS.
- `AsyncRedemptionUpgradeable.fulfillRedeemRequests` enforces a 1-block delay (`block.number > requestBlock`), preventing same-block flash-loan cycles of deposit → requestRedeem → fulfill.
- LBTC mint rate limits (`RateLimitsV2`) are leaky-bucket with configurable capacity — flash-loan volume is bounded by the bucket capacity.
- **No flash loan attack vectors found** on exchange rates or PPS.

---

## Conclusion

The Lombard Finance codebase is well-architected and has been hardened by multiple prior audits. The contracts consistently use:
- `nonReentrant` guards on all state-changing functions
- ERC-7201 namespaced storage for upgrade safety
- Rate limiting on mint/burn/bridge operations
- Consortium signature verification for cross-chain messages
- Blocklist/sanction compliance checks
- PPS-based (not balance-based) vault accounting to prevent inflation attacks

The 5 findings identified are all MEDIUM or LOW severity. The most significant (fee approval signature replay, MEDIUM) requires a compromised `CLAIMER_ROLE` to exploit and does not result in direct fund theft — only fee over-charging. No CRITICAL or HIGH vulnerabilities were found that would allow direct theft of user funds, unauthorized minting, or consensus takeover without a trusted-role compromise.
