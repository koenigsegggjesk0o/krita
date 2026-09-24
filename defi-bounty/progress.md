# Progress Log

## Session 1 — 2026-09-24 22:00-23:30 WIB

### Status: SCANNING Ethena USDtb contracts

### Done this session:
- [x] Created folder structure: defi-bounty/{protocol-research,vuln}
- [x] Wrote README.md (system explanation)
- [x] Wrote conversation-full.txt (full chat history session 1)
- [x] Wrote progress.md (this file)
- [x] Researched Immunefi active bounties (172 programs live)
- [x] Selected Ethena as primary target ($3M max bounty)
- [x] Wrote protocol-research/ethena.md (bounty scope + strategy)
- [x] Cloned ethena-labs/ethena-usdtb-contest repo from GitHub
- [x] Read USDtbMinting.sol (681 lines) — first pass
- [x] Read USDtb.sol (204 lines) — first pass
- [x] Wrote protocol-research/ethena-usdtb-minting-analysis.md
- [x] Wrote protocol-research/ethena-usdtb-token-analysis.md
- [x] Committed + pushed to GitHub (commits 185f1fb, f500b2c)

### Vulnerabilities found: 0 (first pass, no critical found yet)
### Income earned: $0

### Next actions (queue):
- [ ] Read IUSDtbMinting.sol + IUSDtbDefinitions.sol (interfaces)
- [ ] Read SingleAdminAccessControl.sol + Upgradeable variant
- [ ] Read test files (StableRatios, blockLimits, ACL, core, whitelist)
- [ ] Compare with older USDe contracts for regression bugs
- [ ] Deep dive verifyStablesLimit edge cases
- [ ] Check EIP-712 domain separator fork protection
- [ ] Check nonce bitmap for bypass
- [ ] If find vulnerability: write vuln/<protocol>-<type>.md with PoC

### Key findings so far:
1. verifyStablesLimit has asymmetric checking (only checks one direction per order type)
2. No same-block mint+redeem restriction (but mitigated by off-chain RFQ)
3. Nonce bitmap ignores upper 64 bits of uint128 nonce (not exploitable)
4. Contract depends heavily on off-chain RFQ system for price protection
5. On-chain stablesDeltaLimit is last line of defense, has potential weakness

### Commit history:
- 185f1fb: defi-bounty: persistent memory system setup
- f500b2c: defi-bounty: Ethena research + USDtbMinting first-pass analysis
- (pending): USDtb token analysis + progress update
