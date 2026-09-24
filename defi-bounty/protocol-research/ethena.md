# Ethena Protocol — Bug Bounty Research

## Bounty Info
- **Platform:** Immunefi
- **URL:** https://immunefi.com/bug-bounty/ethena/
- **Max Bounty:** $3,000,000 (Critical Smart Contract)
- **Min Reward:** $100,000 (Critical)
- **Live Since:** 04 April 2024
- **Last Updated:** 11 August 2026
- **KYC:** Required (user has Indonesian KTP — OK)
- **PoC:** Required
- **Triaged by Immunefi:** Yes
- **Vault:** $12,497 available (protocol pays rest directly)

## Protocol Overview
Ethena is a synthetic dollar protocol on Ethereum. Issues:
- **USDe** — synthetic dollar stablecoin
- **sUSDe** — staked USDe (yield-bearing)
- **USDtb** — newer stablecoin backed by T-Bills (added 2025)
- **sENA** — staked ENA token

Multi-chain: Ethereum mainnet + L2s (Arbitrum, Optimism, Mantle, BNB, etc) + TON.

## Assets in Scope (28 total, 27 impacts)

### Priority Targets (sorted by recency × TVL)

| # | Contract | Address | Added | TVL | Priority |
|---|----------|---------|-------|-----|----------|
| 1 | **USDtb PSM Contract** | 0x73E3...3728 | 10 Aug 2026 | ? | **HIGHEST** (newest) |
| 2 | StakedUSDeOFTAdapter.sol | 0x211c...e5d2 | 15 Apr 2024 | $416.2M | HIGH (big TVL) |
| 3 | USDtb.sol | 0xc139...ac1c | 4 Apr 2025 | $266.2M | HIGH (big TVL) |
| 4 | StakedENA.sol | 0x8bE3...B3b9 | 4 Apr 2025 | ? | MEDIUM |
| 5 | USDtbMinting.sol | 0xa3DD...416a | 4 Apr 2025 | ? | MEDIUM |
| 6 | USDe OFT on TON | tonview... | 20 May 2025 | ? | MEDIUM (TON chain) |
| 7 | tsUSDe vault on TON | tonview... | 20 May 2025 | ? | MEDIUM |
| 8 | tsUSDe minter on TON | tonview... | 20 May 2025 | ? | MEDIUM |
| 9 | USDe minter on TON | tonview... | 20 May 2025 | ? | MEDIUM |
| 10 | ENAOFT.sol (multi-chain) | various | 15 Apr 2024 | ? | LOWER |
| 11 | StakedUSDeOFT.sol (multi-chain) | various | 15 Apr 2024 | ? | LOWER |
| 12 | USDeOFT.sol (multi-chain) | various | 15 Apr 2024 | ? | LOWER |

## Impacts in Scope (Critical = highest payout)

Critical ($100k-$3M):
- Direct theft of user funds (at-rest or in-motion, excluding unclaimed yield)
- Permanent freezing of funds
- Protocol insolvency
- Governance manipulation resulting in direct change from intended effect

High ($10k-$75k):
- Theft of unclaimed yield
- Theft of unclaimed royalties
- Permanent freezing of unclaimed yield
- Permanent freezing of unclaimed royalties
- Temporary freezing of funds (reward doubles per additional time block)

Medium:
- Smart contract unable to operate due to lack of token funds
- Block stuffing
- Griefing (no profit motive but damage to users/protocol)

## Out of Scope (DO NOT waste time on these)

- Oracle manipulation (UNLESS flash loan attack — flash loans ARE in scope)
- 51% attacks / basic economic attacks
- Lack of liquidity impacts
- Sybil attacks
- Centralization risks
- Attacks requiring leaked keys/credentials
- Attacks requiring privileged addresses (governance/strategist) without modifications
- Depegging of external stablecoin (unless directly caused by code bug)
- Best practice recommendations / feature requests
- Phishing/social engineering

## Vulnerability Classes to Focus On

Based on scope + protocol type (stablecoin + staking + cross-chain):

1. **Reentrancy** — especially in mint/redeem flows of USDtb and USDe
2. **Access control** — who can call mint/burn/redeem? Any bypass?
3. **Flash loan attacks** — manipulate price/collateral ratio in 1 tx
4. **Cross-chain bridge bugs** — OFT contracts (LayerZero) are complex
5. **Integer overflow/underflow** — especially in yield calculation (sUSDe)
6. **Precision loss** — division before multiplication, rounding errors
7. **Front-running** — mint/redeem ordering, MEV exposure
8. **Upgradeable contract bugs** — proxy pattern implementation flaws
9. **TON chain contracts** — newer, less audited, different VM (TVM vs EVM)
10. **PSM (Peg Stability Module)** — newest contract, prime target

## Research Strategy

### Phase 1 (Hours 1-8): USDtb PSM Contract
- Newest contract (added 10 Aug 2026, only 6 weeks old)
- PSM = Peg Stability Module, typically swaps USDe<->USDtb 1:1
- Common PSM bugs: fee bypass, rate manipulation, infinite mint
- Get source from Etherscan (0x73E3...3728)

### Phase 2 (Hours 8-16): StakedUSDeOFTAdapter.sol
- $416M TVL — highest value target
- OFT = LayerZero Omnichain Fungible Token
- Adapter pattern = complex, common bug source
- Cross-chain message passing vulnerabilities

### Phase 3 (Hours 16-32): USDtb.sol + USDtbMinting.sol
- $266M TVL
- Newer than USDe (April 2025)
- Minting logic = collateral ratio checks

### Phase 4 (Hours 32-48): TON chain contracts
- Different VM (TVM vs EVM)
- Less audited (TON DeFi is newer)
- Need to learn TVM specifics

## Next Steps (immediate)

1. Fetch USDtb PSM Contract source from Etherscan
2. Read contract line-by-line
3. Document any suspicious patterns
4. Write PoC if vulnerability found
5. If found, write vuln/ethena-psm-<type>.md

## Sources
- https://immunefi.com/bug-bounty/ethena/scope/
- https://docs.ethena.fi
- https://etherscan.io (contract source)
- https://github.com/ethena-labs (if public)
