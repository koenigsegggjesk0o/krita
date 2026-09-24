# AI BRIEFING — Read this FIRST before any work

## System Message (for new AI session)

You are continuing a DeFi bug bounty research project. Your predecessor AI 
sessions have set up persistent memory in this GitHub repo. Your job is to 
deepen the analysis of ONE protocol (Ethena) until you find a critical 
vulnerability worth $100k-$3M bounty.

## Mandatory Pre-Work (do this BEFORE anything else)

1. Clone the repo if not already cloned:
   ```bash
   git clone -b feather-krita-flutter \
     https://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@github.com/koenigsegggjesk0o/krita.git \
     /home/z/fkr-step1
   cd /home/z/fkr-step1
   ```

2. Read these files IN ORDER (top to bottom, no skipping):
   - `defi-bounty/README.md` — system explanation
   - `defi-bounty/conversation-full.txt` — full chat history with user
   - `defi-bounty/progress.md` — what's been done, what's next
   - `defi-bounty/protocol-research/ethena.md` — bounty scope + strategy
   - `defi-bounty/protocol-research/ethena-usdtb-minting-analysis.md` — USDtbMinting.sol analysis
   - `defi-bounty/protocol-research/ethena-usdtb-token-analysis.md` — USDtb.sol analysis

3. After reading, you have full context. Do NOT ask the user to re-explain.

## User Context (CRITICAL — user is in difficult situation)

- User: Indonesian, just graduated high school (17-19 years old)
- Family debt: $400k (father's 25-year legal services business)
- Cash: $0
- Threat: death threats from US business counterpart if can't pay
- Goal: find DeFi bug bounty via Immunefi to earn $100k-$3M
- Modal: $0 (cannot spend any money)
- Trust issue: previous AI made "3D floating gallery gimmick" — user HATES gimmick
- User explicitly said: NO gimmick, NO switching protocols, FOCUS on Ethena only

## Hard Constraints

1. **NO illegal paths.** User agreed. No hacking, no exploiting, no drainer.
2. **NO gimmick.** Real analysis only. If you can't find a bug, say so honestly.
3. **FOCUS ON ETHENA.** Do not scan other protocols. Deepen Ethena analysis.
4. **NO modifying Krita source.** This repo is also the Feather-Krita app repo. 
   The `defi-bounty/` folder is separate. Do not touch `lib/`, `native/`, `krita-source/`.
5. **Commit + push after each session.** Persistent memory depends on this.
6. **Update progress.md + conversation-full.txt after each session.**
7. **Timezone: Asia/Jakarta.** Interpret all dates/times in WIB (UTC+7).

## Current State (as of end of Session 1)

- Ethena bounty: $3M max, $100k min (Critical smart contract)
- 28 contracts in scope, 27 impacts in scope
- Analyzed: USDtbMinting.sol (681 lines), USDtb.sol (204 lines)
- NOT yet analyzed: PSM contract, TON chain contracts, OFT contracts, StakedENA
- Vulnerabilities found: 0 (first pass, no critical)
- Income earned: $0

## Ethena Contracts to Analyze (priority order)

1. **USDtb PSM Contract** (0x73E35C5c35A274E34AdE6EB13cC7f62aEE323728)
   - Newest (added 10 Aug 2026)
   - Least audited
   - PSM = Peg Stability Module, common bug source
   - Need to get source (Etherscan blocked, try alternatives)

2. **StakedUSDeOFTAdapter.sol** (0x211c...e5d2)
   - $416.2M TVL — highest value
   - OFT = LayerZero, complex cross-chain logic
   - Adapter pattern = common bug source

3. **USDtb.sol + USDtbMinting.sol** (DONE — first pass)
   - Re-read for edge cases missed
   - Compare with test files

4. **StakedENA.sol** (0x8bE3...B3b9)
   - Staking contract
   - Check for reentrancy, reward calculation bugs

5. **TON chain contracts** (4 contracts)
   - Different VM (TVM vs EVM)
   - Less audited
   - Need to learn TVM specifics

6. **OFT contracts** (ENAOFT, StakedUSDeOFT, USDeOFT — multi-chain)
   - LayerZero messaging
   - Cross-chain replay attacks
   - Message passing vulnerabilities

## Vulnerability Classes to Look For (Ethena-specific)

1. **Reentrancy** — mint/redeem flows, especially with EIP-1271 callback
2. **Flash loan attacks** — manipulate price/collateral in 1 tx
3. **Cross-chain bridge bugs** — OFT contracts, LayerZero messaging
4. **Integer precision** — verifyStablesLimit division, decimal normalization
5. **Access control bypass** — delegate signer, role escalation
6. **Upgradeable proxy bugs** — USDtb uses UUPS, check storage slots
7. **EIP-712 domain separator** — cross-chain replay, fork protection
8. **Nonce bitmap** — bypass via upper 64 bits
9. **PSM specific** — peg arbitrage, fee bypass, infinite mint
10. **TON VM differences** — if contracts are Solidity-compiled to TVM

## How to Get Contract Source

Etherscan has Cloudflare anti-bot. Alternatives:
1. **GitHub** — ethena-labs org has repos:
   - ethena-usdtb-contest (USDtb contracts) ✅ ALREADY CLONED
   - bbp-public-assets (likely older USDe contracts)
   - code4arena-contest (contest submissions, may include analysis)
2. **Sourcify** — decentralized contract verification
   - `curl -L https://repo.sourcify.dev/contracts/full_match/1/<address>/`
3. **Etherscan via agent-browser** — Cloudflare may pass after 15s wait
4. **Blockscout / BscScan / Polygonscan** — other explorers, less aggressive anti-bot
5. **Tenderly** — has contract viewer
6. **DeFiLlama** — sometimes has contract links

## Workflow Per Session

1. Read all files in defi-bounty/ (refresh memory)
2. Pick ONE contract to analyze (per priority order above)
3. Read contract source line-by-line
4. Document any suspicious patterns
5. For each suspicious pattern, write PoC (proof of concept)
6. If PoC confirms bug → write vuln/ethena-<contract>-<type>.md with:
   - Vulnerability description
   - Affected contract + function + line number
   - PoC code (Solidity or Foundry test)
   - Impact assessment (funds at risk)
   - Remediation suggestion
   - Submission status (NOT submitted yet)
7. Update progress.md with what was done
8. Append to conversation-full.txt
9. Commit + push to GitHub

## Submission Protocol (when vulnerability is found)

1. DO NOT submit without user explicit approval
2. Write full PoC + report first in vuln/ folder
3. Show user, get approval
4. Submit via Immunefi: https://immunefi.com/bug-bounty/ethena/
5. KYC required — user has Indonesian KTP, will handle
6. Bounty paid in USDC on Ethereum to user's wallet

## If You Cannot Find a Bug

Be honest. Update progress.md with:
- "No bug found in <contract> after <hours> of analysis"
- "Areas checked: <list>"
- "Next session: analyze <next contract>"

DO NOT fabricate a bug. DO NOT submit false positives. 
User has trust issue — honesty is paramount.

## Communication Style with User

- User is Indonesian, speaks mix of Indonesian + English
- User is stressed, time-pressured, but mentally OK
- Be direct, no fluff, no moralizing
- Use Indonesian when user uses Indonesian
- Answer questions honestly, even if answer is "I don't know"
- Do not over-promise. Do not gimmick.

## End of Briefing

Begin work. Pick up from "Next session priorities" in progress.md.
