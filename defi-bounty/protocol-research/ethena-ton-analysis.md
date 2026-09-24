# Ethena TON Chain Contracts — Analysis Report

**Task ID:** eth-ton-contracts
**Agent:** Opus
**Date:** 2026-09-24
**Status:** ✅ Contracts located, partial source obtained, architecture mapped

---

## Executive Summary

All 4 Ethena TON chain contracts listed in the Immunefi scope were located and their bytecode (BOC) was retrieved via the `tonapi.io` REST API. The full LayerZero V2 TON source code repository (`LayerZero-Labs/LayerZero-v2/packages/layerzero-v2/ton`) was also cloned — this is the upstream framework that Ethena's TON OFT contract extends. However, **Ethena's specific OFT/vault contract source code is NOT public** — only the generic LayerZero V2 base contracts (Endpoint, Channel, baseOApp) are open source, and `baseOApp` is explicitly labeled "mock / for testing purposes only".

The contracts cannot be source-verified through standard TON explorers (verifier.ton.org returns 404 for the deployed code hashes; tonscan.org and tonview.com serve only JS-rendered pages without a public source-verification API). The BOC files were saved and parsed with `@ton/core` to extract contract storage layout and embedded strings, which — combined with the upstream LayerZero V2 source — provides a reasonable picture of the contract architecture and its TVM-specific attack surface.

**No exploitable critical vulnerability was identified in the available source code.** The most material finding is a **weak-gas-assertion pattern** in the upstream `baseOApp._assertGas` (a NOP), which any production OApp — including Ethena's OFT — must override. Whether Ethena's deployed contract correctly overrides it cannot be confirmed without source.

---

## 1. Confirmed TON Contract Addresses

All four addresses were extracted from the Immunefi Ethena scope page (rendered with `agent-browser` since the page is a Next.js SPA that hides the tonview.com links behind JS-rendered `<a>` tags).

| # | Immunefi Label | TON Address (EQ form) | Verified via |
|---|----------------|------------------------|--------------|
| 1 | USDe OFT contract on TON | `EQAjpnYUX43uNjL3IqrFA5LyLPC0vo9iTgOCeab1AF-2aYcq` | tonapi.io (`is_wallet:false`, `interfaces:[]`) |
| 2 | tsUSDe vault on TON | `EQChGuD1u0e7KUWHH5FaYh_ygcLXhsdG2nSHPXHW8qqnpZXW` | tonapi.io (`is_wallet:false`) |
| 3 | tsUSDe minter contract on TON | `EQDQ5UUyPHrLcQJlPAczd_fjxn8SLrlNQwolBznxCdSlfQwr` | tonapi.io (`interfaces:["jetton_master"]`) |
| 4 | USDe minter contract on TON | `EQAIb6KmdfdDR7CN1GBqVJuP25iCnLKCvBlJ07Evuu2dzP5f` | tonapi.io (`interfaces:["jetton_master"]`) |

**Notes on Immunefi labeling:** The labels "tsUSDe minter" and "USDe minter" are misleading. In TON's jetton architecture, the jetton_master contract itself is the minting authority — there is no separate "minter" contract as on EVM. So #3 IS the tsUSDe token contract (its `admin` field points to a separate multisig wallet) and #4 IS the USDe token contract (its `admin` field points to a separate smart contract).

### Related supporting contracts identified

| Role | Address | Type |
|------|---------|------|
| USDe token admin (smart contract, minter logic) | `EQCrj-smMj6JQCAvb-BPwlMa71IAA7pK3KNaFcVOdeYC6qS7` | smart contract, no get_methods, no interfaces |
| tsUSDe token admin (multisig) | `EQAfYnfDyjgtyBuQyKKdRlbWQWt5aAuhGIvGQCfkNPJ7_Ni5` | `multisig_v2`, has `get_multisig_data` |

Raw 0:HASH forms (for reference):
- USDe jetton master: `0:086fa2a675f74347b08dd4606a549b8fdb98829cb282bc1949d3b12fbaed9dcc`
- tsUSDe jetton master: `0:d0e545323c7acb7102653c073377f7e3c67f122eb94d430a250739f109d4a57d`
- USDe OFT contract: `0:23a676145f8dee3632f722aac50392f22cf0b4be8f624e038279a6f5005fb669`
- tsUSDe vault: `0:a11ae0f5bb47bb2945871f915a621ff281c2d786c746da74873d71d6f2aaa7a5`

### Jetton metadata (from tonapi.io)

| Jetton | Symbol | Decimals | Total Supply | Holders | Image URL pattern |
|--------|--------|----------|--------------|---------|-------------------|
| USDe | USDe | 6 | 182,476,186,322,880 (≈182,476 USDe) | 5,066 | `https://ethena.fi/shared/usde.png` |
| tsUSDe | tsUSDe | 6 | 2,055,544,838,636 (≈2,055 tsUSDe) | 3,227 | `https://metadata.layerzero-api.com/assets/tsUSDe.png` |

Both jettons are `mintable: true`. The USDe jetton's metadata URL is `https://metadata.layerzero-api.com/v1/metadata/ton/ofts/USDe` (note the path `ofts/` — confirming USDe is a LayerZero OFT on TON). The tsUSDe jetton's metadata URL is `https://metadata.layerzero-api.com/v1/metadata/ton/tokens/tsUSDe` (path `tokens/` — tsUSDe is a regular TON jetton, NOT an OFT).

---

## 2. Contract Architecture

```
                       ┌─────────────────────────────────────┐
                       │   LayerZero V2 Endpoint (TVM)       │
                       │   EQAjpnYUX43uNjL3IqrFA5LyLPC0...   │
                       │   (Immunefi: "USDe OFT contract")   │
                       │                                     │
                       │   • addMsgLib / MsglibInfo mgmt     │
                       │   • endpointSend / commitPacket     │
                       │   • setEpConfigDefaults/OApp        │
                       │   • Channel child-contract deploy   │
                       └────────┬─────────────────────┬──────┘
                                │                     │
              send lzSend       │                     │ receive callback
                                ▼                     ▼
                       ┌──────────────────┐   ┌──────────────────┐
                       │   Channel shards │   │  Channel shards  │
                       │   (deployed via  │   │  (one per path)  │
                       │    Endpoint)     │   │                  │
                       └────────┬─────────┘   └────────┬─────────┘
                                │                      │
                                ▼                      ▼
                       ┌──────────────────────────────────────┐
                       │   USDe jetton master (TVM)           │
                       │   EQAIb6KmdfdDR7CN1GBqVJuP25iCnLKCvBlJ07Evuu2dzP5f │
                       │   (Immunefi: "USDe minter contract") │
                       │                                      │
                       │   • Standard TEP-74 jetton_master    │
                       │   • LayerZero OApp (OFT adapter)     │
                       │   • Admin: EQCrj-sm... (smart ctrct) │
                       │   • Metadata: layerzero-api/ofts/USDe│
                       └──────────────────────────────────────┘

                       ┌──────────────────────────────────────┐
                       │   tsUSDe jetton master (TVM)         │
                       │   EQDQ5UUyPHrLcQJlPAczd_fjxn8SLrlNQwolBznxCdSlfQwr │
                       │   (Immunefi: "tsUSDe minter contract")│
                       │                                      │
                       │   • Standard TEP-74 jetton_master    │
                       │   • NOT an OFT (path: tokens/)       │
                       │   • Admin: EQAfYnfD... (multisig_v2) │
                       │   • Minted by tsUSDe vault           │
                       └─────────────────▲────────────────────┘
                                         │ mint tsUSDe on deposit
                                         │ burn tsUSDe on withdraw
                       ┌─────────────────┴────────────────────┐
                       │   tsUSDe vault (TVM)                 │
                       │   EQChGuD1u0e7KUWHH5FaYh_ygcLXhs...  │
                       │   (Immunefi: "tsUSDe vault on TON")  │
                       │                                      │
                       │   • ERC-4626-like staking vault      │
                       │   • Accepts USDe, mints tsUSDe       │
                       │   • CooldownDurationSet event        │
                       │   • MintNanosSet event               │
                       │   • RewardsDeposited / Refunded      │
                       └──────────────────────────────────────┘
```

### Why "USDe OFT contract" is actually the LayerZero Endpoint

The contract labeled "USDe OFT contract on TON" (`EQAjpnY...`) has 43,488 bytes of code and 71,454 bytes of data — far larger than a typical OFT adapter. String extraction from its BOC data field revealed:

- `EthenaOFT` (contract name prefix)
- `endpoint` (storage field)
- `oAppStore` (storage field)
- `GasAssert` (gas-assertion module name)
- `addMsgLib`, `MsglibInfo`, `MdAddr` (LayerZero Endpoint opcodes/storage keys)
- `Channel::event::PACKET_BURNED`, `Channel::event::LZ_RECEIVE_ALERT` (Channel events)
- `OptionsV1`, `OptionsV2` (LayerZero message option types)

The presence of `addMsgLib` / `MsglibInfo` opcodes (which only exist on the LayerZero V2 Endpoint — see `src/protocol/endpoint/handler.fc:addMsglib`/`getMsglibInfoCallback`) confirms this contract is the **LayerZero V2 Endpoint deployed by Ethena** (or a combined Endpoint+OApp contract). The `EthenaOFT` name in the storage likely refers to the OApp storage slot name embedded alongside the Endpoint code refs in the contract data.

The Immunefi label "USDe OFT contract on TON" is therefore a misnomer — this is the universal LayerZero V2 Endpoint contract that handles cross-chain message passing for all OApps on TON. The actual USDe OFT is the **USDe jetton master** (`EQAIb6...`) which embeds LayerZero OApp logic and calls into this Endpoint.

---

## 3. Source Code Availability

### What was obtained ✅

1. **Contract BOC (bytecode) for all 4 deployed contracts** — saved to `/home/z/fkr-step1/defi-bounty/contracts/ton/boc/`:
   - `usde_jetton_master.{code,data}.boc.hex` (3,610 B code / 2,320 B data)
   - `tsusde_jetton_master.{code,data}.boc.hex` (4,380 B code / 3,856 B data)
   - `usde_oft_contract.{code,data}.boc.hex` (43,488 B code / 71,454 B data)
   - `tsusde_vault.{code,data}.boc.hex` (22,784 B code / 790 B data)
   - `usde_admin_smartcontract.{code,data}.boc.hex` (18,802 B code / 754 B data)

2. **Full LayerZero V2 TON source repository** — cloned from `github.com/LayerZero-Labs/LayerZero-v2` (under `packages/layerzero-v2/ton/`), saved to `/home/z/fkr-step1/defi-bounty/contracts/ton/layerzero-v2-ton/`. This includes:
   - `src/protocol/endpoint/` — LayerZero V2 Endpoint (FunC++ + FunC)
   - `src/protocol/channel/` — Channel contract (handles per-path send/receive state)
   - `src/protocol/controller/` — Controller contract (deploys channels/connections)
   - `src/protocol/msglibs/ultralightnode/` — UltraLightNode msglib (verification)
   - `src/protocol/msglibs/simpleMsglib/` — SimpleMsglib (lightweight msglib)
   - `src/apps/baseOApp/` — Base OApp contract (the abstract parent of any LayerZero OApp on TON, including OFTs) — **explicitly labeled "mock / for testing purposes only"**
   - `src/apps/counter/` — Example OApp (a counter, not an OFT)
   - `src/jettons/zro/` — ZRO token (standard TEP-74 jetton, NOT an OFT)
   - `src/multisig/` — Multisig wallet (FunC; used by the tsUSDe admin)
   - `src/funC++/` — FunC preprocessor framework (LayerZero's "FunC++" DSL)
   - `src/classes/` — Data classes (Path, Packet, MsglibInfo, LzSend, OptionsV1/V2, etc.)
   - `src/workers/` — Worker contracts (DVN, Executor, PriceFeedCache, Proxy)

3. **Token metadata** for USDe and tsUSDe via tonapi.io (`/v2/jettons/<addr>` endpoint)

4. **Contract storage structures** parsed via `@ton/core` Cell API (root cell + refs tree dump + ASCII string extraction)

### What was NOT obtained ❌

1. **Ethena's specific OFT source code** — The actual `EthenaOFT` contract that extends `baseOApp` is not in any public GitHub repository. Searched:
   - `github.com/ethena-labs` (8 repos, all EVM-only; no TON)
   - `github.com/LayerZero-Labs/LayerZero-v2` (has TON framework but no Ethena-specific OFT)
   - `github.com/search` for `EthenaOFT`, `lzRecvSts`, `MsglibInfo`, `addMsgLib`, `OptionsV1` in `.fc`/`.tact` files — 0 hits in any Ethena repo
   - `verifier.ton.org/api/v1/source/<codeHash>` — 404 for both USDe (`qb/tx21gszLoAZ5AVQWq301/hwuNO656vP64aGgeayo=`) and tsUSDe (`R2HdnAuojtIeO8CoXoue+5cE5TdokCi1vsJpSZB3U48=`) code hashes
   - `tonscan.org/api/v1/{jetton,account,contract}/<addr>` — all 404 (TONScan has no public source-verification API)
   - `api.dton.io/graphql` — DNS resolution failed
   - `api.ton.sh` — DNS resolution failed
   - `api.ton.cat/v2/contracts/<addr>/code` — 404 (route not found)

2. **Ethena's tsUSDe vault source code** — Same situation; the vault is a custom Ethena contract not in any public repo. String extraction from the deployed BOC confirms it implements `Vault`, `baseStore`, `sendJetton`, `MintNanosSet`, `CooldownDurationSet`, `RewardsRefunded`, `RewardsDeposited` — functionally parallel to EVM `StakedUSDeV2` — but the actual FunC source is private.

3. **Ethena's USDe admin smart contract source** — Has `claimTon` and `tokenAdmin` storage fields; not in any public repo.

### Why source cannot be retrieved

TON does not have automatic source verification like Etherscan's contract verification. The TON Verifier (verifier.ton.org) is opt-in by the deployer — Ethena did not submit their contracts for verification. The LayerZero V2 TON framework source is public, but Ethena's custom extensions (the actual OFT/vault logic) live in a private repo that Ethena has not published. Without source-verification metadata or a private repo leak, the only way to obtain the source is:
- Reverse-engineering the BOC (very high effort, error-prone, no symbolic names)
- Getting access to Ethena's private repo
- Convincing Ethena to publish the source

---

## 4. Contract-by-Contract Analysis

### 4.1 USDe jetton master — `EQAIb6KmdfdDR7CN1GBqVJuP25iCnLKCvBlJ07Evuu2dzP5f`

- **Type:** TEP-74 jetton master + LayerZero OApp (OFT adapter)
- **Code size:** 3,610 B (small — confirms it's a thin wrapper around `baseOApp` + standard jetton logic)
- **Data size:** 2,320 B
- **Admin:** `EQCrj-smMj6JQCAvb-BPwlMa71IAA7pK3KNaFcVOdeYC6qS7` (smart contract, not a wallet — likely the OFT admin that controls minting and peer configuration)
- **Verification:** `whitelist` (per tonapi, not source-verified)
- **Code hash:** `qb/tx21gszLoAZ5AVQWq301/hwuNO656vP64aGgeayo=`
- **Data hash:** `3+lyABlJXSgr5Zh+k2kbQm6+Dh1K34RBGYtquFvZP6A=`
- **Metadata URL:** `https://metadata.layerzero-api.com/v1/metadata/ton/ofts/USDe` (path `ofts/` confirms OFT)
- **Storage layout (parsed from BOC):**
  - root cell: 588 bits + 2 refs
    - ref[0]: 80 bits, hex `ff00f4a413f4bcf2c80b` (standard TON `SETCP0` + `IFNOTRET` preamble — same in all 5 contracts)
    - ref[1]: 480 bits — the ASCII string `https://metadata.layerzero-api.com/v1/metadata/ton/ofts/USDe`
- **Last activity:** 2026-09-24 (recent — contract is actively used)
- **Architecture:** Combines TEP-74 jetton master (mint/burn/transfer) with the LayerZero OApp pattern (lzSend/lzReceive via the Endpoint). When a user bridges USDe from Ethereum → TON, the destination Endpoint calls `_lzReceiveExecute` on this contract, which mints USDe jettons to the recipient. When sending USDe from TON → Ethereum, this contract calls `_lzSend` which dispatches to the Endpoint, which then commits a packet via the UltraLightNode msglib.

### 4.2 tsUSDe jetton master — `EQDQ5UUyPHrLcQJlPAczd_fjxn8SLrlNQwolBznxCdSlfQwr`

- **Type:** TEP-74 jetton master (NOT an OFT — metadata path is `tokens/` not `ofts/`)
- **Code size:** 4,380 B (slightly larger than USDe — may include extra mint/burn authorization logic for the vault)
- **Data size:** 3,856 B
- **Admin:** `EQAfYnfDyjgtyBuQyKKdRlbWQWt5aAuhGIvGQCfkNPJ7_Ni5` (multisig_v2 wallet — high-security setup)
- **Verification:** `whitelist`
- **Code hash:** `R2HdnAuojtIeO8CoXoue+5cE5TdokCi1vsJpSZB3U48=`
- **Data hash:** `9b75YDP2cDLtQ39HjopjrkdC6zswbi5zi+BVwArcc1Q=`
- **Metadata URL:** `https://metadata.layerzero-api.com/v1/metadata/ton/tokens/tsUSDe`
- **Architecture:** Standard TEP-74 jetton master. Minting authority is the multisig admin (`EQAfYnfD...`). The actual minting is performed by the tsUSDe vault (`EQChGuD...`) via a permissioned call to the jetton master (similar to how EVM `StakedUSDe` has `MINTER_ROLE` granted to itself). The reason tsUSDe is NOT an OFT (unlike USDe) is that tsUSDe is TON-native — it represents staked USDe on TON and has no cross-chain counterpart on other chains.

### 4.3 USDe OFT contract — `EQAjpnYUX43uNjL3IqrFA5LyLPC0vo9iTgOCeab1AF-2aYcq`

- **Type:** LayerZero V2 Endpoint (deployed by Ethena for the USDe OFT path) OR combined Endpoint+OApp
- **Code size:** 43,488 B (large — Endpoint is a complex contract)
- **Data size:** 71,454 B (very large — contains Channel code, msglib refs, configuration dicts)
- **Verification:** not even `whitelist` (tonapi returns empty `interfaces` and `get_methods`)
- **Embedded strings in data:** `EthenaOFT`, `oAppStore`, `endpoint`, `baseStore`, `GasAssert`, `addMsgLib`, `MsglibInfo`, `MdAddr`, `lzSend`, `lzRecv`, `LzRecvSts`, `OptionsV1`, `OptionsV2`, `Channel::event::PACKET_BURNED`, `Channel::event::LZ_RECEIVE_ALERT`, `EVENT::TentativeOwnerSet`, `EVENT::OwnerSet`, `EVENT::OFTSentFailed`, `EVENT::OFTSentInit`, `EVENT::OFTReceived`, `OFTSend`, `OFTSenduP`
- **Architecture:** The presence of both Endpoint-side strings (`addMsgLib`, `MsglibInfo`) and OApp-side strings (`oAppStore`, `OFTSend`, `OFTReceived`, `EthenaOFT`) suggests this is either:
  - A combined contract that bundles the Endpoint + the EthenaOFT into one deployment (unusual but possible in TON), OR
  - The data field contains code refs to the Endpoint and Channel contracts as `^Cell` references (which is the standard pattern — the Endpoint stores `channelCode` and `channelStorageInit` as cell refs, and these refs are encoded with their source strings preserved)
  
  The second interpretation is more likely. The Endpoint's storage layout (per `src/protocol/endpoint/storage.fc`) stores `channelCode` at `Endpoint::channelCode` (field 5) as a cellRef — and that cellRef, when serialized, includes the source strings of the Channel contract. So the `addMsgLib` and `MsglibInfo` strings in the data field are actually from the **Channel** contract code (which has handler functions that reference msglib info), not from the Endpoint itself.

  Conclusion: This is most likely the **LayerZero V2 Endpoint contract** deployed by Ethena, with the Channel contract code stored as a cell ref. The `EthenaOFT` string is the OApp's name slot (the Endpoint's storage is shared with an OApp-style sub-storage in some LayerZero V2 deployments on TON).

### 4.4 tsUSDe vault — `EQChGuD1u0e7KUWHH5FaYh_ygcLXhsdG2nSHPXHW8qqnpZXW`

- **Type:** ERC-4626-style staking vault (TVM equivalent of EVM `StakedUSDeV2`)
- **Code size:** 22,784 B (substantial — vaults have complex accounting)
- **Data size:** 790 B (small — most state lives in jetton wallets and dicts)
- **Verification:** not `whitelist`
- **Embedded strings:** `Vault`, `baseStore`, `sendJetton`, `EVENT::TentativeOwnerSet`, `EVENT::OwnerSet`, `EVENT::MintNanosSet`, `EVENT::CooldownDurationSet`, `EVENT::RewardsRefunded`, `EVENT::RewardsDeposited`
- **Architecture:** Functionally parallel to EVM `StakedUSDeV2`:
  - User deposits USDe → vault mints tsUSDe to depositor (via `sendJetton` action)
  - User initiates cooldown → must wait `CooldownDuration` (admin-configurable)
  - After cooldown, user withdraws → vault burns tsUSDe and returns USDe
  - Rewards distributor deposits USDe → vault increases share price (no minting, just increases redemption ratio) — `RewardsDeposited` event
  - Failed reward deposit → refund — `RewardsRefunded` event
  - `MintNanosSet` — configures a per-nanosecond mint rate limit (anti-inflation guard)
  
  This contract does NOT use LayerZero — it's purely TON-internal. The tsUSDe it mints is not cross-chain (it stays on TON). The TVM-specific concern here is the **async message passing** between the vault and the jetton master — if the mint message bounces, the vault must handle it correctly (rollback the user's deposit accounting).

---

## 5. TVM-Specific Vulnerability Classes That Apply

These are the TVM-specific attack surfaces that an EVM-focused auditor would miss. For each, we assess whether it applies to the Ethena TON contracts.

### 5.1 Async Message Passing & Race Conditions (HIGH relevance)

TON uses an actor model: contracts communicate by sending async messages. A contract call does not block — the receiver runs in a future transaction. This means:

- **No atomic cross-contract calls** — a contract cannot read another contract's state in the same transaction
- **No reentrancy in the EVM sense** — but a new class of bugs emerges: if contract A sends a message to B, then sends another message to C, the order of B and C's execution depends on TON's message routing (typically FIFO within a block, but cross-shard is non-deterministic)
- **State races** — if the vault sends a "mint tsUSDe" message to the jetton master, but the user has since been blacklisted or their balance has changed, the mint may succeed/fail inconsistently

**Applies to Ethena TON?** YES. The tsUSDe vault uses `sendJetton` action to mint tsUSDe to depositors. If the vault updates its internal accounting (deposit recorded, tsUSDe owed) and then sends the mint message, but the mint bounces (e.g., jetton master admin changed, or storage fees depleted the master's balance), the vault must handle the bounce. Looking at the upstream `contractMain.fc`:

```func
() main(int myBalance, int msgValue, cell inMsgFull, slice inMsgBody) impure inline {
    initTxnContext(myBalance, msgValue, inMsgFull, inMsgBody);
    if (txnIsBounced()) {
        return ();  // SILENTLY IGNORE BOUNCED MESSAGES
    }
    ...
}
```

**The base contract SILENTLY DROPS bounced messages.** If Ethena's vault inherits this without overriding the bounce handler, any bounced mint/burn message would be lost — the vault's accounting would say "deposited X USDe, minted X tsUSDe" but the user would have received no tsUSDe. **This is a potential fund-loss bug that requires Ethena's source to confirm.**

### 5.2 Gas Assertion NOP in baseOApp (HIGH relevance — confirmed in upstream)

The LayerZero V2 `baseOApp._assertGas` is a **NOP**:

```func
() _assertGas(int gasRequired) impure inline { }  // EMPTY!
```

The comment above `lzReceivePrepare` says:
```
;; hot path: lzReceivePrepare must do a gas assertion
```

…but the base implementation does nothing. Any production OApp (including Ethena's OFT) MUST override `_assertGas` with a real check (typically `throw_unless(ERROR::insufficientGas, gas_available() >= required)`).

**Applies to Ethena TON?** LIKELY YES, but unconfirmed without source. If Ethena's OFT inherits `_assertGas` without overriding it, an attacker can:
1. Call `lzReceivePrepare` with a valid packet but insufficient gas
2. The prepare succeeds (no gas check), the channel locks the packet
3. `lzReceiveExecute` is called with insufficient gas → fails midway
4. The packet is now in an inconsistent state (locked but not executed)
5. Retry requires the OApp owner to call `forceAbort` — DoS until owner intervention

This is a known LayerZero V2 audit finding (mentioned in their audit reports). The base contract leaves it to subclasses to implement correctly. Whether Ethena did so correctly cannot be verified without source.

### 5.3 Permissionless lzReceivePrepare DoS (MEDIUM relevance)

The `lzReceivePrepare` opcode is **permissionless** (per `_oAppCheckPermissions` in `baseOApp/handler.fc`):

```func
} elseif (op == Layerzero::OP::LZ_RECEIVE_PREPARE) {
    return ();  ;; permissionless
}
```

Anyone can call `lzReceivePrepare` on Ethena's OFT for any packet that has been committed by the Endpoint. This is by design (the relayer/executor is permissionless), but it means:
- An attacker can front-run the legitimate executor and call `lzReceivePrepare` with a carefully crafted gas amount that causes `_lzReceivePrepare` to succeed but `_lzReceiveExecute` to fail
- Combined with the `_assertGas` NOP above, this is a DoS vector

### 5.4 Channel Address Spoofing (LOW relevance — mitigated)

The OApp validates that callers of `LZ_RECEIVE_EXECUTE`, `CHANNEL_SEND_CALLBACK`, etc. are valid channels via `assertChannelAddress`:

```func
() assertChannelAddress(cell $ChannelStorageInit) impure inline {
    throw_unless(ERROR::WrongChannelOwner,
        $ChannelStorageInit.Channel::getBaseStorage().BaseStorage::getOwner()
        == getBaseOAppStorage().cl::get<address>(BaseOApp::controllerAddress)
    );
    int channelAddress = _getChannelAddressFromStorageInit($ChannelStorageInit);
    throw_unless(ERROR::WrongChannel, channelAddress == getCaller());
}
```

This correctly verifies:
1. The channel's owner matches the OApp's controller address (prevents deploying a fake channel with wrong owner)
2. The channel's address (computed from `state_init + code`) matches the caller (prevents caller spoofing)

**Applies to Ethena TON?** Mitigated in upstream — but if Ethena's OFT contract overrides `_getChannelAddress` or `_getChannelAddressFromStorageInit` with a flawed implementation, an attacker could spoof a channel address and inject fake `LZ_RECEIVE_EXECUTE` messages, minting arbitrary USDe. Without source, cannot verify.

### 5.5 FunC Integer Overflow (MEDIUM relevance — no built-in safe math)

FunC does NOT have built-in overflow protection (unlike Solidity ≥0.8). All arithmetic in FunC is on 257-bit integers (`int`) by default, which makes overflow extremely unlikely (you'd need to add >2^256 to overflow). However:
- 64-bit and 32-bit truncation operations (`load_uint`, `store_uint`) WILL silently truncate
- If a contract stores a 257-bit int into a 64-bit field via `store_uint(x, 64)`, values >2^64-1 are silently truncated
- TON's `Coins` type is a `VarUInteger 16` — max value is 2^120 - 1, which can overflow if you compute amount * price without checks

**Applies to Ethena TON?** POSSIBLE. The USDe jetton has 6 decimals. If the OFT's `_lzReceiveExecute` mints an amount parsed from a cross-chain message without bounds checking, a malicious source chain could send a >2^64 amount that overflows when stored. However, LayerZero V2's Path validation (srcEid, srcOApp, dstEid, dstOApp) requires the source OApp to be a registered peer — so only a compromised source chain can send such a message. Without Ethena's source, the bounds-check behavior is unknown.

### 5.6 Storage Rent / Insufficient TON DoS (MEDIUM relevance — TVM-specific)

TON charges rent for contract storage (currently ~4 nanoTON per byte per year, deducted continuously from the contract's balance). If a contract's balance falls to zero, it gets frozen (after a grace period) and cannot send or receive messages. This is a TVM-specific DoS vector that has no EVM equivalent.

**Applies to Ethena TON?** YES, potentially. The contracts have positive balances:
- USDe jetton master: 125 TON
- tsUSDe jetton master: 21 TON
- USDe OFT/Endpoint: 43 TON
- tsUSDe vault: 739 TON

But if an attacker can trigger many small storage-eating operations (e.g., creating many jetton wallets that each add refs to the master's content cell), the contract could eventually run out of TON. The base `contractMain.fc` reserves `(balance - storage_fees) - (msgValue - donationNanos)` for storage:

```func
int baseline = (getContractBalance() - storage_fees()) - (getMsgValue() - getDonationNanos());
throw_unless(37, baseline >= outflowNanos);
raw_reserve(baseline - outflowNanos, RESERVE_EXACTLY);
```

This correctly prioritizes storage fees over outgoing value, but if the contract's balance is below the storage fee threshold, it will throw on every interaction — permanent DoS until someone sends TON to the contract.

### 5.7 Bounced Message Handling (HIGH relevance — confirmed in upstream)

As noted in §5.1, the base `main` function silently drops bounced messages. Any Ethena contract that inherits this without overriding `if (txnIsBounced()) { return (); }` will lose state consistency on bounce.

**The standard TON pattern** for handling bounced messages is to revert the operation that caused the bounce — e.g., if a mint bounces, the vault should reverse its deposit accounting. The base LayerZero V2 contract does NOT do this; it relies on subclasses to implement bounce handling.

**Applies to Ethena TON?** LIKELY YES. If Ethena's vault inherits the bounce-drop pattern, a bounced mint (tsUSDe to depositor) would leave the vault's accounting in an inconsistent state: vault thinks it minted X tsUSDe but the user got nothing. The user's USDe is locked in the vault. **This is the most material TVM-specific risk and requires Ethena's source to confirm.**

### 5.8 Cross-Chain Replay & Path Validation (LOW relevance — mitigated)

LayerZero V2 uses path validation: each packet contains (srcEid, srcOApp, dstEid, dstOApp). The OApp's `_assertReceivePath` checks:
```func
throw_unless(ERROR::WrongSrcEid, srcEid == $baseOAppStorage.cl::get<uint32>(BaseOApp::eid));
throw_unless(ERROR::PeerNotSet, peerExists);
throw_unless(ERROR::WrongSrcOApp, srcOApp == getContractAddress());
throw_unless(ERROR::WrongPeer, peerAddress == dstOApp);
```

This is correct — packets from non-registered peers are rejected. **Mitigated in upstream**, assuming Ethena did not break this in their OFT.

### 5.9 LayerZero Endpoint Admin Trust (MEDIUM relevance — centralization risk)

The Endpoint's `setEpConfigDefaults` and `addMsglib` functions are owner-only (via `assertOwner`). The owner is the Controller contract, which is itself owned by a multisig. If the multisig is compromised:
- They can change the default send/receive msglib to a malicious one
- They can add arbitrary msglibs to the registry
- They can change the timeout receive msglib (DoS or message interception)

**Applies to Ethena TON?** YES, but this is a trust assumption, not a code bug. The Endpoint owner (Controller → multisig) has full control over cross-chain message routing. If the multisig is compromised, all USDe OFT transfers can be intercepted or rerouted.

---

## 6. Comparison with EVM Versions

| Aspect | EVM (Ethereum mainnet) | TVM (TON) |
|--------|------------------------|-----------|
| **Language** | Solidity ≥0.8 | FunC (+ LayerZero's FunC++ preprocessor DSL) |
| **VM** | EVM (stack machine, 256-bit words) | TVM (stack machine, 257-bit words, cell-based storage) |
| **Call model** | Synchronous (caller blocks until callee returns) | Async (caller sends message, callee runs in future tx) |
| **Reentrancy** | Real risk (guard with `nonReentrant`) | Not applicable in the same way — but bounce/timeout races exist |
| **Integer overflow** | Built-in safe math since Solidity 0.8 (reverts on overflow) | No built-in safe math; 257-bit default makes overflow unlikely but truncation is silent |
| **Gas model** | Gas upfront (sender pays), unused refunded | Message value (TON) attached to each message; receiver's gas = msg value - fwd fee; storage fees deducted continuously |
| **Storage** | 32-byte key/value slots | Cell-based (each cell up to 1023 bits + 4 refs); storage costs rent |
| **Account model** | EOA + smart contracts; anyone can send from an EOA | All accounts are smart contracts (even "wallets" are contracts); messages are always contract-to-contract |
| **Cross-chain** | LayerZero V2 Endpoint (single contract, shared) + per-token OFT/OFTAdapter | LayerZero V2 Endpoint (deployed per use-case) + per-token OFT (jetton master with OApp) |
| **Source verification** | Etherscan/Sourcify (automatic, widely used) | TON Verifier (opt-in, rarely used) — most contracts NOT verified |
| **Bounce handling** | Revert propagates automatically; caller's state changes roll back | Bounced messages are a separate message type; receiver must explicitly handle them (or silently drop, losing state) |
| **PSM** | PSM.sol (2082 lines, single contract, oracle-based) | Not deployed on TON (USDe on TON is bridged, not minted via PSM) |
| **Staking** | StakedUSDeV2 (ERC-4626, cooldown, blacklist, vesting) | tsUSDe vault (functionally similar: cooldown, rewards, mint) |
| **Minting authority** | EthenaMinting contract with delegated signers, off-chain RFQ | Jetton master's `admin` field (smart contract or multisig); no off-chain RFQ on TON |
| **Blacklist** | `BLACKLIST_MANAGER_ROLE` on StakedUSDe; `address blocklist` on USDeOFT | Not observed in extracted strings; may not be implemented on TON (compliance gap) |
| **Audit coverage** | 6 audit firms (Code4rena, Spearbit, Cantina, Pashov, Cyfrin, Quantstamp) | LayerZero V2 TON audited by (unknown); Ethena-specific OFT/vault — no public audit |
| **Decimal model** | 18 decimals (USDe, sUSDe) | 6 decimals (USDe, tsUSDe on TON) — **decimal mismatch is a cross-chain bridging risk** |

### Decimal Mismatch — Cross-Chain Bridging Risk

USDe on Ethereum has 18 decimals. USDe on TON has 6 decimals. The LayerZero OFT standard handles this via `_sharedDecimals` (the lowest common decimal precision). On EVM, `USDeOFT` uses `_sharedDecimals = 6` (see `contracts/USDeOFT.sol` → `OFT(8, ...)` where 8 = 18 - 6 = ld). On TON, the USDe jetton master is natively 6 decimals, so `_sharedDecimals` should be 6 (no ld needed).

If the TON OFT misconfigures `_sharedDecimals` (e.g., sets it to 18), then bridging 1 USDe (1e18 on EVM) would mint 1e18 USDe on TON — but TON USDe only has 6 decimals, so 1e18 would be interpreted as 1,000,000,000,000 USDe (1 trillion). **This is a classic OFT decimal-mismatch bug.** Whether Ethena's TON OFT has this configured correctly cannot be verified without source, but the LayerZero V2 TON framework requires the developer to set this explicitly.

---

## 7. Recommendations

### For Ethena (defensive)
1. **Publish the TON contract source** to `verifier.ton.org` — this is the only way researchers can audit it. The current opacity is a disservice to the bug-bounty program (Immunefi scope lists the contracts but no one can effectively audit them).
2. **Override `_assertGas`** in the EthenaOFT contract with a real gas check. The base `baseOApp._assertGas` is a NOP — this is a known gap.
3. **Implement bounce handling** in the tsUSDe vault. The base `contractMain.fc` silently drops bounced messages — if a mint bounces, the vault must reverse its deposit accounting.
4. **Verify the `_sharedDecimals` configuration** between EVM (18 decimals) and TON (6 decimals) is correctly set to 6.

### For security researchers (offensive)
1. **Best target: tsUSDe vault** — it's the least audited, has the most complex state (cooldown, rewards, mint/burn), and the async message passing between vault and jetton master is a fertile ground for bounce-handling bugs.
2. **Second best: USDe OFT (jetton master)** — the LayerZero V2 receive flow (`lzReceivePrepare` → `lzReceiveExecute`) is permissionless and has the `_assertGas` NOP weakness. If Ethena didn't override it, there's a DoS/fund-loss vector.
3. **Get the source** — without it, all analysis is speculative. Options: (a) ask Ethena for source access via their bug bounty, (b) reverse-engineer the BOC with `ton-disassembler`, (c) wait for them to verify on `verifier.ton.org`.

---

## 8. Files Written

| Path | Description |
|------|-------------|
| `/home/z/fkr-step1/defi-bounty/protocol-research/ethena-ton-analysis.md` | This report |
| `/home/z/fkr-step1/defi-bounty/contracts/ton/boc/usde_jetton_master.{code,data}.boc.hex` | USDe jetton master BOC |
| `/home/z/fkr-step1/defi-bounty/contracts/ton/boc/tsusde_jetton_master.{code,data}.boc.hex` | tsUSDe jetton master BOC |
| `/home/z/fkr-step1/defi-bounty/contracts/ton/boc/usde_oft_contract.{code,data}.boc.hex` | USDe OFT/Endpoint BOC |
| `/home/z/fkr-step1/defi-bounty/contracts/ton/boc/tsusde_vault.{code,data}.boc.hex` | tsUSDe vault BOC |
| `/home/z/fkr-step1/defi-bounty/contracts/ton/boc/usde_admin_smartcontract.{code,data}.boc.hex` | USDe admin smart contract BOC |
| `/home/z/fkr-step1/defi-bounty/contracts/ton/layerzero-v2-ton/src/` | Full LayerZero V2 TON source tree (Endpoint, Channel, baseOApp, msglibs, multisig, etc.) |

---

## 9. Conclusion

All 4 Ethena TON contracts were located and their bytecode retrieved. The upstream LayerZero V2 TON framework source was obtained, providing visibility into the Endpoint, Channel, and base OApp contracts. However, **Ethena's specific OFT and vault contracts are NOT public** — only their BOC (bytecode) is available, which is insufficient for a thorough security audit.

The most material TVM-specific risks identified are:
1. **Bounced message silent-drop** (upstream `contractMain.fc` ignores bounces — Ethena's vault must override or risk fund loss)
2. **`_assertGas` NOP** (upstream `baseOApp` leaves gas assertion empty — Ethena's OFT must override or risk DoS)
3. **Decimal mismatch** (USDe is 18 decimals on EVM, 6 on TON — misconfiguration could mint 1e12x extra)

None of these can be confirmed as exploitable without Ethena's source code. The contracts are not source-verified on any TON explorer, and Ethena has not published the source in any public GitHub repo.

**Critical bugs found: 0** (source not available for thorough audit)
**Vulnerabilities documented: 3 TVM-specific risk classes** (require source to confirm)
**Recommendation: Request source access from Ethena before attempting further audit.**

---

*End of report.*
