# DeFi Bug Bounty — Persistent Memory System

This folder is my (AI) persistent memory across chat sessions. Since I reset every new chat, this folder is how I "remember" the user and our work.

## How it works

1. **Before starting any new work session:**
   - Read `conversation-full.txt` — full history of chat with user
   - Read `progress.md` — what I've done so far
   - Read all files in `protocol-research/` — protocols I've analyzed
   - Read all files in `vuln/` — vulnerabilities I've found

2. **During work session:**
   - Scan new protocols from Immunefi
   - Write research findings to `protocol-research/<protocol>.md`
   - If find vulnerability, write to `vuln/<protocol>-<vuln-type>.md` with full code + explanation
   - Update `progress.md` with what was done this session
   - Append key new info to `conversation-full.txt`

3. **After work session:**
   - Commit + push to GitHub
   - Next session, start from step 1

## User context (CRITICAL — read first)

- Name: (user)
- Age: just graduated high school (17-19)
- Location: Indonesia, Asia/Jakarta timezone
- Situation: $0 cash, family in danger (international business dispute, US counterpart, death threats if can't pay $400k debt)
- Goal: Find DeFi bug bounty via Immunefi to earn $50k-500k in 1-3 months
- Modal: $0. Cannot spend any money.
- Constraints:
  - NO illegal paths (user agreed, we discussed extensively)
  - NO gimmick (user explicitly rejected stub/fake work)
  - Real working code only
  - User has trust issue after previous AI agent made "3D floating gallery gimmick"
- Resources user has:
  - GitHub account: koenigsegggjesk0o
  - Google Drive (for file uploads)
  - HP Android + internet
- Resources user does NOT have:
  - Money for Apple/Google dev account ($25-99)
  - Apple ID with paid apps
  - iOS device
  - Computer with GPU
  - Credit card for cloud services

## Workflow protocol

Every session:
1. `cat conversation-full.txt` — refresh memory
2. `cat progress.md` — see where I left off
3. `ls protocol-research/ vuln/` — see what I've done
4. Read all files in both folders
5. Pick up where I left off
6. When done with session, update progress.md + commit/push

## What counts as "finding a vulnerability"

- Must be a real bug, not theoretical
- Must have PoC (proof of concept) code
- Must be reproducible
- Must be in scope of Immunefi bounty program
- Must be reported via official Immunefi channel (NOT exploited)

## Files

- `conversation-full.txt` — full chat history (append-only)
- `progress.md` — session log (update each session)
- `protocol-research/<name>.md` — one file per protocol scanned
- `vuln/<protocol>-<type>.md` — one file per vulnerability found
