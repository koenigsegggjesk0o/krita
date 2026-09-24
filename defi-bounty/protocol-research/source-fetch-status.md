# Ethena Source Fetch Status

**Task:** eth-source-fetch
**Date:** 2026-09-25
**Agent:** Opus

## Summary

All 6 requested contracts were sourced successfully. The key insight was that
**Blockscout's public API** (`eth.blockscout.com/api/v2/smart-contracts/<addr>`)
returns verified Solidity source without Cloudflare blocking, unlike Etherscan.
The Ethena docs "Key Addresses" page (fetched via agent-browser) provided the
authoritative address list, which corrected a typo in the task brief.

## Contracts Sourced (6/6 deliverables + extras)

| # | Contract | Address | Source | Bytes | File |
|---|----------|---------|--------|-------|------|
| 1 | **PSM.sol** (USDtb Peg Stability Module) | 0x73E35C5c35A274E34AdE6EB13cC7f62aEE323728 | Blockscout mainnet | 96,578 | `contracts/PSM.sol` |
| 2 | **StakedENA.sol** (sENA implementation) | impl 0x7fD57b46aE1a7b14f6940508381877Ee03e1018B behind proxy 0x8bE3460A480c80728a8C4D7a5D5303c85ba7B3b9 | Blockscout mainnet | 15,600 | `contracts/StakedENA.sol` |
| 3 | **StakedUSDeOFTAdapter.sol** | 0x211Cc4DD073734dA055fbF44a2b4667d5E5fE5d2 | Blockscout mainnet | 2,394 | `contracts/StakedUSDeOFTAdapter.sol` |
| 4 | **ENAOFT.sol** (non-adapter, from Arbitrum — does not exist on mainnet) | 0x58538e6A46E07434d7E7375Bc268D3cb839C0133 | Blockscout Arbitrum | 2,735 | `contracts/ENAOFT.sol` |
| 5 | **StakedUSDeOFT.sol** (non-adapter, from Arbitrum — does not exist on mainnet) | 0x211Cc4DD073734dA055fbF44a2b4667d5E5fE5d2 | Blockscout Arbitrum | 4,468 | `contracts/StakedUSDeOFT.sol` |
| 6 | **USDeOFT.sol** (non-adapter, from Arbitrum — does not exist on mainnet) | 0x5d3a1Ff2b6BAb83b63cd9AD0787074081a52ef34 | Blockscout Arbitrum | 2,738 | `contracts/USDeOFT.sol` |

### Extras (mainnet Adapter versions of the multi-chain OFT contracts)

These are the **mainnet** representatives of contracts #4, #5, #6. On mainnet,
Ethena deploys `OFTAdapter` (which wraps the canonical mainnet ERC-20) rather
than the bare `OFT` (which IS the token, used on destination L2s). The Ethena
docs confirm the same address is deployed across "Most L2s" via CREATE2.

| Contract | Address | File |
|----------|---------|------|
| ENAOFTAdapter.sol (mainnet) | 0x58538e6A46E07434d7E7375Bc268D3cb839C0133 | `contracts/ENAOFTAdapter.sol` |
| USDeOFTAdapter.sol (mainnet) | 0x5d3a1Ff2b6BAb83b63cd9AD0787074081a52ef34 | `contracts/USDeOFTAdapter.sol` |
| (StakedUSDeOFTAdapter already listed as #3) | 0x211Cc4DD073734dA055fbF44a2b4667d5E5fE5d2 | `contracts/StakedUSDeOFTAdapter.sol` |

### Bonus dependency files sourced

| File | Source | Bytes |
|------|--------|-------|
| `ENASilo.sol` (StakedENA dep, deployed standalone) | Blockscout mainnet (0x85fEB4edC3198fEfC5aD36b80CfF182eF2bF2F79) | 719 |
| `SingleAdminAccessControl.sol` (PSM dep) | GitHub ethena-labs/ethena-usdtb-contest | 2,855 |
| `SingleAdminAccessControlUpgradeable.sol` (StakedENA dep) | GitHub ethena-labs/ethena-usdtb-contest | 2,922 |
| `ISingleAdminAccessControl.sol` (interface) | GitHub ethena-labs/ethena-usdtb-contest | 317 |

## Methods Tried

### Methods that WORKED

1. **Blockscout API v2** (primary method) — `https://eth.blockscout.com/api/v2/smart-contracts/<addr>`
   - Returns verified `source_code` field as a single .sol file
   - Works for ALL Ethena mainnet contracts, no Cloudflare blocking
   - Limitation: returns only the **main contract file**, not imported dependencies
   - Used for: PSM, StakedENA (impl), StakedUSDeOFTAdapter, USDeOFTAdapter (mainnet), ENAOFTAdapter (mainnet), ENASilo

2. **Blockscout Arbitrum** — `https://arbitrum.blockscout.com/api/v2/smart-contracts/<addr>`
   - Used to fetch the **non-adapter** OFT contracts (ENAOFT, USDeOFT, StakedUSDeOFT)
   - These don't exist on mainnet (mainnet only has Adapters); Arbitrum has the OFT versions at the same addresses (CREATE2 deploy)

3. **agent-browser on Ethena docs** — `https://docs.ethena.fi/solution-design/key-addresses`
   - Confirmed all 6 addresses + revealed the StakedENA address correction
   - Also confirmed mainnet OFT addresses match across "Most L2s"

4. **GitHub API (ethena-labs org)** — `https://api.github.com/repos/ethena-labs/ethena-usdtb-contest`
   - Provided SingleAdminAccessControl.sol + Upgradeable variant + interface
   - Confirmed no OFT contracts in public ethena-labs repos (they are in a private `onchain-minting-internal` package)

5. **Blockscout search** — `https://eth.blockscout.com/api/v2/search?q=<name>`
   - Used to discover multiple candidate addresses for each contract name
   - Found 2 StakedENA implementations (current + previous) and confirmed the current one via EIP-1967 slot read

### Methods that DID NOT work (or were not needed)

1. **Etherscan V2 API** — `https://api.etherscan.io/v2/api?...&apikey=<KEY>`
   - Failed without API key: `"Missing/Invalid API Key"`
   - Did not register a key (Blockscout worked first, no need)
   - Etherscan web still blocked by Cloudflare in sandbox

2. **Sourcify v2 API** — `https://sourcify.dev/server/v2/contract/1/<addr>`
   - Returns `match: exact_match` for PSM and StakedENA, but **`files: []`** in the response
   - The Sourcify repo (`repo.sourcify.dev`) now serves a Next.js web app, not raw S3 files
   - v1 `/server/files/<chain>/<addr>` endpoint returns 404 ("Cannot GET")
   - Could NOT retrieve multi-file source trees from Sourcify

3. **Tenderly** — not attempted (Blockscout already succeeded)

4. **DeFiLlama** — fetched `/protocol/ethena` (returned TVL history) and `/contracts/ethena` (returned nothing useful); no contract source links

5. **GitHub code search for lib files** — searched for `filename:OFTOwnable2StepAdapter.sol` and `RateLimiter.sol` in ethena-labs org; 0 results in ethena-labs. Broader search found `tangtj/eth-contract-database` which stores per-address single-file sources (same as Blockscout, no multi-file).

6. **LayerZero OFT standard repo** — Ethena's OFT contracts import from local `../libs/` paths, NOT from `@layerzerolabs/`. Ethena has their own custom `OFTOwnable2Step`, `OFTOwnable2StepAdapter`, and `RateLimiter` libs (not the LayerZero standard).

## Address Corrections

The task brief had two address issues:

1. **StakedENA** — brief gave `0x8bE3Ed22903325125dD19a34811c113F4B3b9` (38 hex chars, truncated/malformed). The Ethena docs "Key Addresses" page shows the sENA Token Contract (proxy) is at `0x8bE3460A480c80728a8C4D7a5D5303c85ba7B3b9` (40 hex chars). The brief's prefix `0x8bE3` and suffix `B3b9` match — the middle was corrupted. Confirmed via EIP-1967 implementation slot read: implementation = `0x7fD57b46aE1a7b14f6940508381877Ee03e1018B` (one of two StakedENA-named contracts found by Blockscout search).

2. **StakedUSDeOFTAdapter** — brief gave `0x211c...e5d2` (partial). Confirmed full address `0x211Cc4DD073734dA055fbF44a2b4667d5E5fE5d2` via both Ethena docs and Blockscout search (exact match on prefix/suffix).

## Outstanding (NOT fetched — interfaces/abstract bases, not deployed standalone)

These imported dependencies are NOT available as standalone verified contracts
on any explorer. They live in Ethena's private `onchain-minting-internal-1.0.0`
package (per the PSM `file_path` metadata) and would only be retrievable via
multi-file verification or the private repo.

### PSM dependencies (in `onchain-minting-internal/src/swap/` and `src/oracle/`)
- `IPSM.sol` (interface)
- `CollateralStateMap.sol` (library/contract)
- `IOracleFeed.sol` (interface — PSM calls `IOracleFeed(feed).getPrice() returns (uint256,uint256)`)
- OracleFeed implementation (the Chainlink-style OracleFeed at 0xbF59e... does NOT implement IOracleFeed; the real one is stored per-collateral in PSM config — would need to read PSM storage to identify)

### StakedENA dependencies (in `contracts/` and `contracts/interfaces/`)
- `IStakedENA.sol` (interface)
- `IEnaSiloDefinitions.sol` (interface — imported by ENASilo.sol)
- Note: `SingleAdminAccessControlUpgradeable.sol` WAS fetched (from ethena-usdtb-contest repo)

### OFT dependencies (in `contracts/libs/`)
- `OFTOwnable2Step.sol` (base for ENAOFT, USDeOFT, StakedUSDeOFT — the non-Adapter versions)
- `OFTOwnable2StepAdapter.sol` (base for ENAOFTAdapter, USDeOFTAdapter, StakedUSDeOFTAdapter)
- `RateLimiter.sol` (custom Ethena rate limiter, NOT the LayerZero standard)

### How to get the outstanding deps (for a future task)
- Register a free Etherscan API key and call the V2 `getsourcecode` endpoint — it returns ALL source files (flattened or multi-file JSON) in one response, including imports.
- Or ask Ethena for access to the `onchain-minting-internal` package.
- The libs can be partially reconstructed from LayerZero-v2's OFTAdapter/OFT base + the RateLimiter pattern, but Ethena's versions have custom modifications (e.g., the RateLimiter in OFT contracts is Ethena-specific, used for `_debit` rate-limiting).

## Notes for Analysis Task

1. **PSM.sol is large (2082 lines, 96KB)** — derived from OnChainMinting.sol. Key surfaces: dual rate-limiting (epoch + period), oracle depeg protection, delegated signers, fee system, multi-collateral. The inventory model uses external custodian wallets (assetSendCustodianAddress, assetReceiveCustodianAddress) — PSM itself never holds funds in normal operation.

2. **StakedENA (410 lines)** — ERC4626Upgradeable staking vault for ENA with cooldown, vesting, blacklist roles (REWARDER_ROLE, BLACKLIST_MANAGER_ROLE, BLACKLISTED_ROLE). Has EnaSilo for cooldown storage. Max vesting 90 days, max cooldown 90 days.

3. **OFT contracts are thin wrappers** (~70-112 lines each) over Ethena's custom `OFTOwnable2Step`/`OFTOwnable2StepAdapter` + `RateLimiter` libs. The interesting security surface is in the **libs** (not fetched) — especially `RateLimiter._checkAndUpdateRateLimit` which the OFT contracts call in `_debit`. Without the lib source, the rate-limit bypass surface cannot be fully analyzed. **Recommend fetching libs via Etherscan API key as next step.**

4. **StakedUSDeOFT inherits from USDeOFT** (Arbitrum version) and adds a blacklist mechanism (`blackLister` + `blackList` mapping). The mainnet StakedUSDeOFTAdapter inherits from USDeOFTAdapter instead. The blacklist logic in StakedUSDeOFT._debit/_credit is the novel attack surface.

5. **All addresses confirmed via Ethena docs** — https://docs.ethena.fi/technical-design/key-addresses (note: the docs URL redirected from `/solution-design/key-addresses` to `/technical-design/key-addresses`).
