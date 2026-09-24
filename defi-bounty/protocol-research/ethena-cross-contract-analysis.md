# Ethena Cross-Contract Interaction Analysis

**Analyst:** Opus (DeFi security researcher)
**Task ID:** eth-cross-contract
**Date:** 2026-09-26
**Scope:** Cross-contract interactions across all Ethena contracts (PSM, USDtbMinting, USDtb, EthenaMinting, StakedUSDe/V2, StakedENA, OFT contracts, ENA token)
**Prior Work:** 7 isolated-contract analyses by Opus agents found 0 critical bugs. This analysis focuses exclusively on bugs that only appear when multiple contracts interact.

---

## Executive Summary

**10 attack vectors investigated. 0 exploitable. 0 critical bugs found.**

The Ethena contract suite is well-architected against cross-contract attacks. The key design properties that prevent cross-contract exploitation are:

1. **PSM's `min(pegPrice, oraclePrice)` pricing** — the PSM never overpays in either swap direction. This single design decision neutralizes 4 of the 10 attack vectors (Vectors 1, 2, 9, and the USDtbMinting cross-arbitrage).
2. **Universal `nonReentrant`** — every state-changing function across all contracts uses `ReentrancyGuard`. Cross-contract reentrancy via token callbacks (ERC777/ERC1363) is universally blocked.
3. **Dual-side blacklist enforcement on OFT bridges** — both the send side (`_update`/`_debit`) and the receive side (`_credit` redirect to owner) check blacklist status, preventing cross-chain blacklist escape.
4. **Separation of concerns** — PSM (USDtb swaps), USDtbMinting (USDtb mint/redeem), and EthenaMinting (USDe mint/redeem) operate on different tokens with different custodian sets and different access control. No shared mutable state across contracts.
5. **`rescueFunds`/`rescueTokens` scope** — PSM holds no funds in normal operation; StakedUSDe/StakedENA explicitly block rescuing the staked asset; USDtb contract doesn't hold user balances in its token balance. Admin cannot steal user funds via rescue functions.

The only findings are LOW/informational design notes and a residual risk on the oracle implementation (which is in a private package not available for analysis).

---

## Contracts Analyzed

| Contract | Address | Lines | Role |
|----------|---------|-------|------|
| PSM.sol | 0x73E3...3728 | 2,082 | USDtb peg stability module (collateral ↔ USDtb swaps) |
| USDtbMinting.sol | 0xa3DD...416a | 681 | USDtb mint/redeem (off-chain RFQ) |
| USDtb.sol | 0xc139...ac1c | 204 | USDtb ERC20 token (with blacklist/whitelist) |
| EthenaMinting.sol | (USDe minting) | 551 | USDe mint/redeem (off-chain RFQ, legacy) |
| StakedUSDeV2.sol | 0x9D39...3497 | 131 | sUSDe ERC4626 vault with cooldown |
| StakedUSDe.sol | (V1 base) | 268 | sUSDe base (vesting, blacklist, redistribute) |
| USDeSilo.sol | (cooldown storage) | 30 | USDe storage during cooldown |
| StakedENA.sol | 0x8bE3...B3b9 | 410 | sENA ERC4626 vault with cooldown (upgradeable) |
| ENASilo.sol | 0x85fE...2F79 | 28 | ENA storage during cooldown |
| ENA.sol | 0x57e1...6061 | 55 | ENA governance token (plain ERC20) |
| USDeOFT/Adapter | (L2/mainnet) | 72/70 | USDe cross-chain bridge |
| StakedUSDeOFT/Adapter | (L2/mainnet) | 112/57 | sUSDe cross-chain bridge with blacklist |
| ENAOFT/Adapter | (L2/mainnet) | 72/70 | ENA cross-chain bridge |

---

## Vector 1: PSM ↔ USDtbMinting Interaction

### Question
Can an attacker mint USDtb via USDtbMinting, swap via PSM, then redeem, profiting from price discrepancies? Can the same-block flow (mint with collateral A → PSM swap → redeem for collateral B) produce arbitrage?

### Feasibility Analysis

**NOT EXPLOITABLE.** The PSM's `min(pegPrice, oraclePrice)` pricing model makes cross-contract arbitrage with USDtbMinting impossible.

#### PSM Pricing Model (the key defense)

The PSM `_getQuote` function calculates two output amounts and takes the minimum:

```
For swapForAsset (collateral → USDtb):
  oneToOneAmountOut = netAmountIn(after fee) × 1e18 × 10^assetDec / (pegPrice × 10^colDec)
  oracleAmountOut   = grossAmountIn × oraclePrice × 10^assetDec / (pegPrice × 10^colDec)
  amountOut = min(oneToOneAmountOut, oracleAmountOut)

For swapForCollateral (USDtb → collateral):
  oneToOneAmountOut = netAmountIn(after fee) × pegPrice × 10^colDec / (1e18 × 10^assetDec)
  oracleAmountOut   = grossAmountIn × pegPrice × 10^colDec / (oraclePrice × 10^assetDec)
  amountOut = min(oneToOneAmountOut, oracleAmountOut)
```

**In both directions, `min()` picks the path that gives the user LESS.** The protocol never overpays:

| Condition | swapForAsset | swapForCollateral |
|-----------|-------------|-------------------|
| oracle > peg (collateral expensive) | min picks oneToOne (fee collected, oracle value clipped) | min picks oracle (user gets less collateral) |
| oracle < peg (collateral cheap) | min picks oracle (user gets less USDtb) | min picks oneToOne (fee collected, oracle value clipped) |
| oracle ≈ peg | min picks oneToOne (fee collected) | min picks oneToOne (fee collected) |

#### USDtbMinting Pricing (the off-chain RFQ)

USDtbMinting uses off-chain RFQ pricing with `verifyStablesLimit` as a safety net:
- **MINT**: Only checks when `usdtbAmount > normalizedCollateralAmount` (minting more USDtb than collateral). Protects protocol from over-minting.
- **REDEEM**: Only checks when `normalizedCollateralAmount > usdtbAmount` (giving more collateral than USDtb burned). Protects protocol from over-paying.
- In the direction that hurts the user (user overpays), no check is enforced — returns `true`.

This asymmetric check is intentional: it protects the protocol in both directions while allowing the off-chain RFQ operator to set exact prices.

#### Cross-Contract Arbitrage Proof

For any cross-contract round-trip (mint → PSM swap → redeem), the attacker's value is bounded:

1. **Mint via USDtbMinting**: Attacker pays collateral A, receives USDtb at off-chain RFQ price. Value out ≤ value in (RFQ is fair, stablesDeltaLimit blocks over-minting).

2. **PSM swap**: Attacker pays USDtb, receives collateral B at `min(peg, oracle)`. Value out ≤ peg value of USDtb ≤ value in (min clips any oracle advantage, fee reduces further).

3. **Redeem via USDtbMinting**: Attacker burns USDtb, receives collateral at off-chain RFQ price. Value out ≤ value in (RFQ is fair, stablesDeltaLimit blocks over-paying).

**Net: value_out ≤ value_in - fees** in every step. No arbitrage possible.

#### The Stale-Price Scenario

The only scenario where profit is theoretically possible is if USDtbMinting's off-chain RFQ price diverges from PSM's fresh oracle price:

- If collateral A depegged and the RFQ operator hasn't updated the mint/redeem price, an attacker could redeem USDtb for overpriced collateral A (at the stale $1.00 price when collateral A is now $1.02).
- **But this profit comes from USDtbMinting's stale pricing, not from the PSM interaction.** The PSM is not involved in the profit.
- Can the PSM amplify this? **No.** If the attacker swaps the overpriced collateral A → USDtb via PSM, the `min()` clips to the peg-based amount (not the inflated oracle amount). The attacker gets `oneToOneAmountOut` (peg-based, minus fee), not `oracleAmountOut` (inflated). The PSM actually PREVENTS amplification.

### Required Conditions for Exploitation
- USDtbMinting MINTER_ROLE or REDEEMER_ROLE must submit an order with a stale/misaligned price.
- The off-chain RFQ operator must fail to update prices during a depeg event.
- **This is a trusted-operator/process risk, not a smart contract bug.**

### Blockers
- PSM `min(peg, oracle)` prevents any oracle-based amplification
- USDtbMinting `verifyStablesLimit` blocks over-minting/over-redeeming (in the protocol-harming direction)
- Both contracts use `nonReentrant` — no cross-contract reentrancy
- PSM uses `msg.sender` auth (not signatures) — no EIP-712/EIP-1271 attack surface
- USDtbMinting requires `_whitelistedBenefactors.contains(order.benefactor)` — attacker can't be a benefactor without admin approval

### Severity: **NOT EXPLOITABLE** (trusted-operator risk only)

---

## Vector 2: PSM ↔ StakedUSDe Interaction

### Question
Can an attacker use sUSDe as collateral in PSM? Can flash-loaning sUSDe and swapping in PSM manipulate the exchange rate for profit?

### Feasibility Analysis

**NOT EXPLOITABLE.** The PSM's `min(peg, oracle)` pricing prevents exchange rate manipulation from being profitable.

#### Exchange Rate Manipulation Analysis

sUSDe is an ERC4626 vault. Its exchange rate = `totalAssets() / totalSupply()`, where `totalAssets() = USDe.balanceOf(StakedUSDe) - getUnvestedAmount()`.

An attacker could attempt to inflate the exchange rate by donating USDe to the StakedUSDe contract (increasing `totalAssets()`). If the PSM's oracle reads this manipulated exchange rate:

**For swapForAsset (sUSDe → USDtb) with inflated oracle:**
- `oracleAmountOut` increases (more USDtb per sUSDe)
- `oneToOneAmountOut` stays fixed (peg-based, after fee)
- `min()` picks `oneToOneAmountOut` (the lower one)
- **Attacker does NOT benefit from the inflated oracle.** They get the peg-based amount minus fee.

**For swapForCollateral (USDtb → sUSDe) with inflated oracle:**
- `oracleAmountOut` decreases (less sUSDe per USDtb, since sUSDe is "worth more")
- `oneToOneAmountOut` stays fixed (peg-based, after fee)
- `min()` picks `oracleAmountOut` (the lower one)
- **Attacker gets LESS sUSDe.** No benefit from inflation.

**Deflating the oracle** (if possible) also doesn't help:
- swapForAsset: `oracleAmountOut` decreases → `min` picks oracle → user gets less. Hurts user.
- swapForCollateral: `oracleAmountOut` increases → `min` picks oneToOne → user gets peg-based. No extra benefit.

#### Flash Loan Round-Trip

An attacker flash-loans sUSDe and attempts a round-trip:
1. Flash loan sUSDe
2. PSM swapForAsset: sUSDe → USDtb. Attacker receives `min(peg, oracle)` USDtb.
3. To repay the flash loan, attacker needs sUSDe back. Must PSM swapForCollateral: USDtb → sUSDe.
4. Second swap gives `min(peg, oracle)` sUSDe, minus another fee.
5. **Net: attacker has less sUSDe than they started with (two fees deducted).** Can't repay flash loan.

#### Cooldown Interaction

The sUSDe cooldown (StakedUSDeV2) only affects staking/unstaking (deposit/withdraw/cooldown/unstake). The PSM treats sUSDe as a plain ERC20 — it just does `safeTransferFrom`. No staking/unstaking is involved in PSM swaps. The cooldown is irrelevant to PSM interactions.

### Required Conditions for Exploitation
- sUSDe must be configured as a PSM collateral (admin-gated)
- The sUSDe oracle must be manipulable (if it's Pyth/Chainlink, it's not)
- The `min()` must be bypassed (impossible — it's in `_getQuote` which is called in every swap)

### Blockers
- `min(peg, oracle)` clips any oracle manipulation benefit
- Two-fee round-trip (swapForAsset + swapForCollateral) always loses value
- PSM `nonReentrant` blocks reentrancy during swaps
- Oracle likely Pyth/Chainlink (off-chain, not flash-loan manipulable)

### Severity: **NOT EXPLOITABLE**

---

## Vector 3: USDtb ↔ OFT Cross-Chain

### Question
Can minting USDtb on mainnet, bridging via OFT, and redeeming on another chain produce profit from price discrepancies? Can bridge timing be exploited?

### Feasibility Analysis

**NOT APPLICABLE / NOT EXPLOITABLE.** USDtb does not have an OFT contract. The OFT contracts are for USDe, sUSDe, and ENA only.

#### USDtb Cross-Chain Status

The provided OFT contracts are:
- USDeOFT / USDeOFTAdapter (for USDe)
- StakedUSDeOFT / StakedUSDeOFTAdapter (for sUSDe)
- ENAOFT / ENAOFTAdapter (for ENA)

**No USDtbOFT or USDtbOFTAdapter exists in the provided contracts.** USDtb is mainnet-only as of the analyzed codebase.

#### USDe Cross-Chain Analysis (substitute)

For USDe (which does have OFT):
1. Mint USDe on mainnet via EthenaMinting (collateral → USDe at off-chain RFQ price)
2. Bridge USDe to L2 via USDeOFTAdapter (rate-limited, fees)
3. On L2, USDe is the OFT token — no EthenaMinting on L2, no redeem mechanism
4. To redeem, bridge back to mainnet (rate-limited, fees), then redeem via EthenaMinting

**Round-trip: mint → bridge → bridge back → redeem.** Bridge fees + rate limits make this unprofitable. The off-chain RFQ pricing is the same on mint and redeem (same operator). No price discrepancy to exploit.

#### sUSDe Cross-Chain Analysis

For sUSDe:
- Mainnet: StakedUSDeV2 (ERC4626 vault with rewards, cooldown, blacklist)
- L2: StakedUSDeOFT (plain ERC20, no staking functionality)
- Bridging sUSDe to L2 locks mainnet sUSDe in the adapter. Rewards continue accruing on mainnet.
- Bridging back returns the same number of sUSDe tokens (now worth more USDe due to rewards).
- **No timing profit** — rewards accrue whether bridged or not.

#### Bridge Timing Attack

"Bridging sUSDe to L2, waiting for exchange rate to increase, bridging back" — this doesn't produce extra profit because:
- The sUSDe tokens are the same count on return
- The exchange rate increase benefits ALL sUSDe holders equally
- The attacker would have gotten the same rewards by just holding on mainnet
- Bridge fees make it strictly worse

### Required Conditions for Exploitation
- A USDtb OFT contract would need to exist (it doesn't)
- Cross-chain price discrepancies would need to persist (rate limits + arbitrageurs close gaps)
- Off-chain RFQ prices would need to differ across chains (same operator = same prices)

### Blockers
- No USDtb OFT exists
- USDe/sUSDe OFT bridges have rate limits (RateLimiter per dstEid)
- EthenaMinting is mainnet-only (no cross-chain mint/redeem arbitrage)
- sUSDe exchange rate accrues identically whether bridged or not
- Bridge fees make round-trips unprofitable

### Severity: **NOT APPLICABLE** (no USDtb OFT) / **NOT EXPLOITABLE** (for USDe/sUSDe/ENA)

---

## Vector 4: EthenaMinting ↔ PSM (Both Active)

### Question
Are both EthenaMinting and PSM active simultaneously? Can using both in one transaction create a reentrancy attack? Can `mintWETH` in EthenaMinting + swap in PSM create a cross-contract reentrancy?

### Feasibility Analysis

**NOT EXPLOITABLE.** EthenaMinting and PSM handle different assets (USDe vs USDtb) with no shared state. Cross-contract reentrancy is blocked by universal `nonReentrant`.

#### Both Active?

Both contracts can be active simultaneously — they serve different purposes:
- EthenaMinting: mints/redeems USDe (delta-neutral synthetic dollar)
- PSM: swaps collateral ↔ USDtb (RWA stablecoin)

They don't share tokens, custodians, or access control. A single transaction can call both (via a smart contract wrapper), but there's no interaction between them.

#### Cross-Contract Reentrancy via mintWETH

EthenaMinting.mintWETH flow:
1. `WETH.safeTransferFrom(benefactor, address(this), amount)` — pull WETH
2. `WETH.withdraw(amount)` — unwrap to ETH
3. `addresses[i].call{value: amountToTransfer}("")` — send ETH to custodian (external call)
4. `usde.mint(beneficiary, usde_amount)` — mint USDe

Step 3 is an external call to a custodian address. If the custodian is a malicious contract, it receives a callback during step 3. At this point:
- EthenaMinting is in `nonReentrant` state (can't reenter EthenaMinting)
- The attacker could call PSM.swap during this callback

**But PSM.swap requires its own valid order** (signed by a PSM benefactor, with valid nonce, chain ID, etc.). The EthenaMinting custodian is not necessarily a PSM benefactor. And even if it were:
- PSM.swap is `nonReentrant` (different guard instance, so it CAN be entered)
- PSM.swap operates on USDtb, not USDe — no shared state with EthenaMinting
- PSM.swap's effects (rate limits, nonce) are independent of EthenaMinting's state
- EthenaMinting's effects (mintedPerBlock, nonce bitmap) are already updated before the external call (CEI honored)

**No cross-contract state corruption is possible.** The two contracts operate on completely independent state. A reentrant call to PSM during EthenaMinting's execution doesn't affect EthenaMinting's invariants.

#### Same-Transaction mintWETH + PSM.swap

An attacker could wrap both calls in a single transaction:
1. Call EthenaMinting.mintWETH (mint USDe from WETH)
2. Call PSM.swap (swap collateral for USDtb)

But these are sequential calls to independent contracts. No interaction, no shared state, no reentrancy. The attacker just mints USDe and swaps for USDtb — two unrelated operations.

### Required Conditions for Exploitation
- A custodian address in EthenaMinting must be a malicious contract (admin-gated)
- The malicious custodian must also be a PSM benefactor with a valid swap order (admin-gated)
- The PSM swap must somehow corrupt EthenaMinting's state (impossible — independent contracts)

### Blockers
- EthenaMinting and PSM operate on different tokens (USDe vs USDtb)
- No shared state between contracts
- Both contracts use `nonReentrant`
- EthenaMinting honors CEI (effects before interactions)
- PSM uses `msg.sender` auth (not signatures) — custodian can't forge a swap order
- Custodian addresses are admin-set (trusted)

### Severity: **NOT EXPLOITABLE**

---

## Vector 5: StakedENA ↔ ENASilo ↔ ENA Token

### Question
Beyond the known Medium (stake → cooldown → blacklist → unstake from silo), can stake → delegate vote → unstake → still vote create a governance attack?

### Feasibility Analysis

**KNOWN MEDIUM (M-1) CONFIRMED** for the blacklist bypass. **CANNOT ANALYZE** the governance voting vector because ENA.sol has no voting/delegation functions.

#### Known Medium: Blacklist Bypass via Cooldown

StakedENA.unstake does NOT check BLACKLISTED_ROLE on `msg.sender` or `receiver`:

```solidity
function unstake(address receiver) external nonReentrant {
    UserCooldown storage userCooldown = cooldowns[msg.sender];
    uint256 assets = userCooldown.underlyingAmount;
    if (block.timestamp >= userCooldown.cooldownEnd || cooldownDuration == 0) {
        userCooldown.cooldownEnd = 0;
        userCooldown.underlyingAmount = 0;
        silo.withdraw(receiver, assets);  // No blacklist check!
        emit Unstake(msg.sender, receiver, assets);
    } else {
        revert InvalidCooldown();
    }
}
```

A user who enters cooldown BEFORE being blacklisted can claim their ENA from the silo AFTER cooldown expires, even while blacklisted. The admin's `redistributeLockedAmount` only operates on live sENA balances (which are already burned once cooldown begins) — it cannot reach funds in the silo.

This is a compliance/blacklist-evasion gap, NOT fund theft. It is documented in the contract comments (lines 141-143): "No attempt is made to restrict blacklisted addresses from claiming their assets at this point as the assets have already been converted to ENA and ENA is a permissionless token."

This is the same finding as V2-1 for StakedUSDeV2 (known since Oct 2023, not a fresh discovery).

#### Governance Voting Vector

ENA.sol is a **plain ERC20** (ERC20Burnable + ERC20Permit + Ownable2Step):
- No `delegate()` / `delegateBySig()` functions
- No `getCurrentVotes()` / `getPriorVotes()` functions
- No ERC20Votes extension
- No checkpoint mechanism
- Just `transfer()`, `approve()`, `mint()` (owner-only), `burn()`

StakedENA (sENA) is an ERC4626 vault — also no voting/delegation functions.

**Without a governance contract in scope, the flash-loan voting attack cannot be analyzed.** However:
- If Ethena's governance uses a separate Governor contract that reads `ENA.balanceOf()` (live balance, not snapshots), then flash-loaning ENA → voting → repaying WOULD work. But this is a governance contract bug, not an ENA/StakedENA bug.
- If governance uses ERC20Votes (which ENA does NOT extend), flash loans wouldn't work (checkpoints prevent balance manipulation).
- StakedENA does NOT compound voting power. If governance counts both ENA and sENA balance, unstaking sENA → ENA shifts balance from one token to another. This could enable double-voting IF the governance contract counts both — but this is a governance contract design issue.

#### The ENASilo Factor

ENASilo holds ENA during the cooldown period. The silo's `withdraw` function is `onlyStakingVault` — only StakedENA can call it. An attacker cannot directly withdraw from the silo. The only way to get ENA out of the silo is via `StakedENA.unstake`, which requires the cooldown to have expired.

The silo does NOT have a blacklist check. Once the cooldown expires, `unstake` pulls ENA from the silo to any receiver address. This is the root cause of the M-1 finding.

### Required Conditions
- **Blacklist bypass (M-1)**: User must enter cooldown BEFORE being blacklisted. Admin's `redistributeLockedAmount` cannot reach silo'd funds.
- **Flash-loan voting**: Requires a governance contract that reads live `balanceOf` (not in scope).

### Blockers
- ENA has no built-in voting mechanism (plain ERC20)
- StakedENA has no voting mechanism (ERC4626 vault)
- ENASilo withdraw is `onlyStakingVault` — no direct access
- The M-1 finding is intentional/documented (ENA is permissionless once unstaked)

### Severity: **MEDIUM (M-1, known/documented)** for blacklist bypass. **CANNOT ANALYZE** for governance voting (no governance contract in scope).

---

## Vector 6: OFT Adapter ↔ Underlying Token Blacklist

### Question
Can an attacker bridge tokens cross-chain to escape a blacklist? Specifically:
- Bridge sUSDe FROM mainnet TO Arbitrum to escape mainnet blacklist?
- Bridge sUSDe FROM Arbitrum TO mainnet to escape L2 blacklist?

### Feasibility Analysis

**NOT EXPLOITABLE.** Dual-side blacklist enforcement prevents escape in both directions.

#### Architecture

| Chain | Contract | Blacklist Mechanism |
|-------|----------|-------------------|
| Mainnet | StakedUSDeV2 | `FULL_RESTRICTED_STAKER_ROLE` (AccessControl role) |
| Mainnet | StakedUSDeOFTAdapter | Checks innerToken (StakedUSDeV2) `hasRole(FULL_RESTRICTED_STAKER_ROLE, recipient)` in `_credit` |
| L2 (Arbitrum) | StakedUSDeOFT | `blackList[address]` mapping, checked in `_update` and `_credit` |

#### Direction 1: Mainnet → L2 (escape mainnet blacklist)

An attacker blacklisted on mainnet (FULL_RESTRICTED_STAKER_ROLE on StakedUSDeV2) wants to bridge sUSDe to L2.

**Send side (mainnet):** `StakedUSDeOFTAdapter._debit` → `OFTAdapter._debit` → `innerToken.safeTransferFrom(msg.sender, address(this), amountSentLD)`

This calls `StakedUSDeV2._beforeTokenTransfer(from, to, amount)`:
```solidity
function _beforeTokenTransfer(address from, address to, uint256) internal virtual override {
    if (hasRole(FULL_RESTRICTED_STAKER_ROLE, from) && to != address(0)) {
        revert OperationNotAllowed();  // BLOCKED
    }
    ...
}
```

**The transfer FROM the blacklisted user to the adapter reverts.** The attacker cannot bridge sUSDe out of mainnet. **Escape blocked on the send side.**

#### Direction 2: L2 → Mainnet (escape L2 blacklist)

An attacker blacklisted on L2 (StakedUSDeOFT.blackList[user] = true) wants to bridge sUSDe to mainnet.

**Send side (L2):** `StakedUSDeOFT._debit` → `OFT._debit` → `_burn(msg.sender, amountSentLD)` → `_update(msg.sender, address(0), amount)`:
```solidity
function _update(address _from, address _to, uint256 _amount) internal override {
    if (blackList[_from]) revert BlackListed(_from);  // BLOCKED
    if (blackList[_to]) revert BlackListed(_to);
    super._update(_from, _to, _amount);
}
```

**The burn FROM the blacklisted user reverts.** The attacker cannot bridge sUSDe out of L2. **Escape blocked on the send side.**

#### Direction 3: Receive on mainnet while blacklisted on L2 only

An attacker is blacklisted on L2 but NOT on mainnet. They have sUSDe on L2 (acquired before being blacklisted). Can they bridge to mainnet?

**Send side (L2):** `_update` checks `blackList[_from]` → reverts. **Blocked.**

Even if they somehow sent before being blacklisted on L2:

**Receive side (mainnet):** `StakedUSDeOFTAdapter._credit` checks `innerToken.hasRole(FULL_RESTRICTED_STAKER_ROLE, _to)`. If the user is NOT blacklisted on mainnet, the check passes and they receive sUSDe on mainnet. This is correct — they're not blacklisted on mainnet.

#### Direction 4: Receive on L2 while blacklisted on mainnet only

An attacker is blacklisted on mainnet but NOT on L2. They bridge sUSDe from mainnet to L2.

**Send side (mainnet):** `_beforeTokenTransfer` checks `FULL_RESTRICTED_STAKER_ROLE` on `from` → reverts. **Blocked.**

Even if they sent before being blacklisted on mainnet:

**Receive side (L2):** `StakedUSDeOFT._credit` checks `blackList[_to]`. If NOT blacklisted on L2, they receive. This is correct — they're not blacklisted on L2.

#### Temporal Window (blacklist lag)

If blacklists are not synced across chains simultaneously, there's a temporal window where a user is blacklisted on one chain but not the other. During this window:

| Scenario | Send side | Receive side | Result |
|----------|-----------|-------------|--------|
| Blacklisted on mainnet first, not yet on L2 | Mainnet `_beforeTokenTransfer` blocks | N/A | **Blocked** |
| Blacklisted on L2 first, not yet on mainnet | L2 `_update` blocks | N/A | **Blocked** |
| Not blacklisted on mainnet, bridging TO mainnet while blacklisted on L2 | L2 `_update` blocks | N/A | **Blocked** |
| Not blacklisted on L2, bridging TO L2 while blacklisted on mainnet | Mainnet `_beforeTokenTransfer` blocks | N/A | **Blocked** |

**In all temporal-window scenarios, the send-side check blocks the escape.** The receive-side redirect-to-owner is a secondary defense.

#### USDe and ENA OFT (no blacklist check)

USDeOFTAdapter and ENAOFTAdapter do NOT check blacklist in `_credit`. However:
- ENA.sol has NO blacklist mechanism (plain ERC20) — no blacklist to escape.
- USDe's blacklist mechanism (if any) is separate from the OFT — but USDe bridged via USDeOFTAdapter is just locking/unlocking USDe. If USDe has a blacklist, the `_beforeTokenTransfer` on USDe would block transfers to/from blacklisted addresses, including transfers to the adapter. (This needs the USDe contract to confirm, which is not in the provided files.)

### Required Conditions for Exploitation
- Bypass `_beforeTokenTransfer` on StakedUSDeV2 (impossible — it's a hard revert)
- Bypass `_update` on StakedUSDeOFT (impossible — it's a hard revert)
- Have sUSDe on the non-blacklisted chain without having bridged it (impossible — all sUSDe originates from mainnet staking or L2 OFT minting)

### Blockers
- StakedUSDeV2 `_beforeTokenTransfer` blocks transfers from FULL_RESTRICTED users (send-side)
- StakedUSDeOFT `_update` blocks transfers from blacklisted users (send-side)
- StakedUSDeOFTAdapter `_credit` redirects to owner if recipient is FULL_RESTRICTED (receive-side)
- StakedUSDeOFT `_credit` redirects to owner if recipient is blacklisted (receive-side)
- Four-layer defense: send-side on source chain + receive-side on destination chain

### Severity: **NOT EXPLOITABLE**

---

## Vector 7: Governance Attack (ENA Token)

### Question
Can an attacker flash-loan ENA, vote, and repay in one transaction? Can stENA be used as voting power? Does unstaking remove voting power immediately?

### Feasibility Analysis

**CANNOT FULLY ANALYZE** — no governance contract in scope. ENA.sol has no voting functions. The risk is in a separate (unseen) governance contract.

#### ENA Token Analysis

ENA.sol extends:
- `Ownable2Step` — ownership transfer (not governance)
- `ERC20Burnable` — burn from own balance or approved
- `ERC20Permit` — gasless approvals (not governance)

**Notable absences:**
- No `ERC20Votes` — no checkpoint-based voting power tracking
- No `delegate()` / `delegateBySig()` — no delegation mechanism
- No `getVotes()` / `getPastVotes()` — no voting power queries
- No `Comp`-like voting system

This means ENA is a **plain ERC20** from a governance perspective. Any governance system built on top would need to either:
1. Use a separate token (e.g., a governance token wrapper)
2. Read `ENA.balanceOf()` directly (vulnerable to flash loans)
3. Use an external snapshot mechanism (off-chain)

#### Flash-Loan Vulnerability Assessment

If Ethena's governance contract reads `ENA.balanceOf(voter)` at vote time (live balance, no snapshots):
- Attacker flash-loans ENA from a lending pool (Aave/Compound)
- Attacker votes with inflated balance
- Attacker repays flash loan in the same transaction
- **This would be a governance contract bug, not an ENA token bug.**

The ENA token itself cannot prevent this because it has no voting mechanism. The fix would be in the governance contract (using ERC20Votes/snapshots or a commit-reveal scheme).

#### StakedENA as Voting Power

StakedENA (sENA) is an ERC4626 vault. It doesn't have voting functions either. If a governance contract counts sENA balance:
- Staking ENA → sENA increases (sENA voting power increases)
- Cooldown → sENA balance unchanged (sENA voting power maintained)
- Unstake → sENA burned, ENA received (sENA voting power drops to 0, ENA voting power increases)

If governance counts BOTH ENA and sENA:
- Before unstake: voter has sENA voting power
- After unstake: voter has ENA voting power (equal value, different token)
- **No double-counting** — the voting power shifts from sENA to ENA, it doesn't double

If governance counts ONLY sENA (not ENA):
- Unstaking removes sENA voting power
- Voter loses voting power after unstake
- But they could re-stake to regain it

If governance counts ONLY ENA (not sENA):
- Staking removes ENA voting power (transferred to vault)
- sENA holders have no direct ENA voting power
- This is the standard "staked tokens don't vote" model

**Without the governance contract, we cannot determine which model Ethena uses.** The ENA and StakedENA contracts themselves are neutral — they don't implement voting.

#### Immunefi Scope Check

The Immunefi scope mentions "Manipulation of governance voting result deviating from voted outcome." This suggests a governance contract exists. But it's not in the provided files. The ENA token contract alone cannot cause governance manipulation — it requires a governance contract with a balance-reading vulnerability.

### Required Conditions
- A governance contract that reads live `ENA.balanceOf()` (not in scope)
- A flash loan provider for ENA (Aave/Compound — likely available)
- The governance contract must not use snapshots/checkpoints

### Blockers
- ENA.sol has no voting functions — the bug (if any) is in the governance contract
- StakedENA has no voting functions — same
- Without the governance contract, no exploit can be demonstrated
- If governance uses ERC20Votes/checkpoints, flash loans don't work

### Severity: **CANNOT ANALYZE** (governance contract not in scope). If governance reads live `balanceOf`, risk is HIGH — but this would be a governance contract bug, not an ENA/StakedENA bug.

---

## Vector 8: rescueFunds Abuse Across Contracts

### Question
Can PSM.rescueFunds drain USDtb accidentally sent to PSM? Can admin rescue funds that belong to users (not just accidental sends)?

### Feasibility Analysis

**NOT EXPLOITABLE.** rescueFunds/rescueTokens across all contracts can only drain accidentally sent tokens, never user funds.

#### PSM.rescueFunds

```solidity
function rescueFunds(address recipient, address token, uint128 amount)
    external override nonReentrant onlyRole(DEFAULT_ADMIN_ROLE) onlyValidAddress(recipient) onlyValidAddress(token)
{
    if (amount == 0) revert InvalidAmount(amount);
    IERC20(token).safeTransfer(recipient, amount);
    emit RescueFunds(recipient, token, amount);
}
```

**Can drain ANY token** from the PSM contract. But PSM **holds no funds in normal operation** — all transfers are direct `safeTransferFrom` between benefactor and custodian:

```solidity
// swapForAsset:
collateral.safeTransferFrom(benefactor, receiveCustodian, amountIn)  // benefactor → custodian directly
asset.safeTransferFrom(assetSendCustodian, beneficiary, amountOut)    // custodian → beneficiary directly

// swapForCollateral:
asset.safeTransferFrom(benefactor, assetReceiveCustodian, amountIn)   // benefactor → custodian directly
collateral.safeTransferFrom(collateralSendCustodian, beneficiary, amountOut) // custodian → beneficiary directly
```

PSM's own token balance is always 0 (for all tokens) in normal operation. `rescueFunds` can only recover:
- Tokens accidentally sent to the PSM address (user error)
- Dust from rounding in edge cases (none found — all transfers are exact)

**Admin cannot steal user funds** because user funds never pass through PSM's balance.

**Can admin drain USDtb from PSM?** Only if USDtb was accidentally sent to the PSM address. In normal operation, USDtb flows directly from `assetSendCustodian` to `beneficiary` — it never touches PSM's balance. The `safeTransferFrom(assetSendCustodian, ...)` pulls from the custodian's allowance, not from PSM's balance.

**Cross-contract concern:** If someone accidentally sends USDtb to the PSM address, the admin CAN drain it via `rescueFunds`. This is the intended use (rescue accidental sends). It's not a theft vector because:
1. The sender made a mistake (not a protocol operation)
2. The admin (DEFAULT_ADMIN_ROLE) is a trusted role
3. The funds would otherwise be stuck forever

#### USDtb.rescueTokens

```solidity
function rescueTokens(address token, uint256 amount, address to) external nonReentrant onlyRole(DEFAULT_ADMIN_ROLE) {
    IERC20Upgradeable(token).safeTransfer(to, amount);
    emit TokensRescued(token, to, amount);
}
```

**Can drain ANY token** from the USDtb contract. But the USDtb contract's token balance only contains:
- Tokens accidentally sent to the USDtb contract address
- NOT user USDtb balances (those are in the `_balances` mapping, not the contract's own token balance)

**Can admin drain user USDtb?** No. `IERC20Upgradeable(token).safeTransfer(to, amount)` transfers from the USDtb contract's own `_balances[address(this)]` entry. User balances are in `_balances[user]` — a different mapping entry. The admin cannot touch user balances.

**Can admin drain USDtb itself?** If someone sent USDtb to the USDtb contract address (accidentally), the admin can rescue it. This is fine — it's rescuing accidentally sent tokens, not user balances.

#### StakedUSDe.rescueTokens

```solidity
function rescueTokens(address token, uint256 amount, address to) external nonReentrant onlyRole(DEFAULT_ADMIN_ROLE) {
    if (address(token) == asset()) revert InvalidToken();  // CANNOT rescue USDe!
    IERC20(token).safeTransfer(to, amount);
}
```

**Explicitly blocks rescuing USDe** (the staked asset). This prevents the admin from draining staker funds. Other tokens (sUSDe, USDC, etc.) can be rescued if accidentally sent.

#### StakedENA.rescueTokens

```solidity
function rescueTokens(address token, uint256 amount, address to) external nonReentrant onlyRole(DEFAULT_ADMIN_ROLE) {
    if (address(token) == asset()) revert InvalidToken();  // CANNOT rescue ENA!
    IERC20Upgradeable(token).safeTransfer(to, amount);
}
```

**Explicitly blocks rescuing ENA** (the staked asset). Same protection as StakedUSDe.

#### USDeSilo / EnaSilo

Neither silo has a `rescueTokens` function. The only way to get tokens out of a silo is via `withdraw(to, amount)`, which is `onlyStakingVault`. **Admin cannot directly drain silo funds.**

#### Cross-Contract Rescue Attack Flow

Could an admin coordinate rescueFunds across multiple contracts to steal user funds?

1. **PSM → USDtb:** Admin can't drain USDtb from PSM (PSM holds 0 USDtb in normal operation).
2. **USDtb → PSM:** Admin can't drain PSM-held tokens via USDtb.rescueTokens (different contract).
3. **StakedUSDe → USDeSilo:** Admin can't drain silo (no rescue function). Can't drain StakedUSDe's USDe (blocked by `asset()` check).
4. **StakedENA → ENASilo:** Same as above.

**No cross-contract rescue attack is possible.** Each contract's rescue function only affects its own balance, and the staking contracts explicitly block rescuing the staked asset.

### Required Conditions for Exploitation
- Admin (DEFAULT_ADMIN_ROLE) must be compromised
- Even with compromised admin: PSM has 0 balance, USDtb user balances are in mapping (not token balance), StakedUSDe/StakedENA block the staked asset, silos have no rescue function
- **Compromised admin cannot steal user funds via rescue functions.** They could steal accidentally-sent tokens, but not user balances.

### Blockers
- PSM holds 0 funds in normal operation (all transfers are direct benefactor↔custodian)
- USDtb contract doesn't hold user balances in its token balance (they're in `_balances` mapping)
- StakedUSDe/StakedENA explicitly block rescuing the staked asset (`if (address(token) == asset()) revert`)
- USDeSilo/EnaSilo have no rescue function (only `withdraw` via staking vault)
- All rescue functions require DEFAULT_ADMIN_ROLE (trusted role)

### Severity: **NOT EXPLOITABLE**

---

## Vector 9: Oracle Manipulation Across PSM Contracts

### Question
What oracle does PSM use? Can flash loans manipulate it? Can manipulating one collateral's oracle affect another?

### Feasibility Analysis

**NOT EXPLOITABLE** (assuming standard oracle). The oracle implementation is in a private package (`onchain-minting-internal`), but the interface and usage pattern strongly suggest Pyth Network.

#### Oracle Interface Analysis

```solidity
interface IOracleFeed {
    function getPrice() external returns (uint256 price, uint256 updatedAt);
}
```

Key observations:
1. **`getPrice()` is NOT `view`** — it's a state-modifying call. This is consistent with Pyth Network feeds, where the caller must first push a price update (which modifies the Pyth contract's state) before reading the price.
2. **`MAX_FUTURE_TIMESTAMP_TOLERANCE = 15 seconds`** — allows minor clock drift, consistent with Pyth's off-chain price aggregation with on-chain delivery.
3. **Returns `(price, updatedAt)`** — price in 18-decimal USD, timestamp for staleness check.
4. **PSM validates staleness** via `maxOracleAge` (configurable per collateral, 10s to ~24h).
5. **PSM validates depeg bounds** via `minOraclePrice` (for swapForAsset) and `maxOraclePrice` (for swapForCollateral).

#### Flash Loan Manipulation Analysis

**If oracle is Pyth (likely):**
- Pyth prices come from off-chain price feeds (PythNet), aggregated from multiple publishers
- Flash loans cannot manipulate Pyth prices — they're off-chain
- Pyth prices are pushed on-chain by relayers, not read from on-chain DEX pools
- **NOT manipulable by flash loans**

**If oracle is Chainlink:**
- Chainlink aggregators pull from off-chain data sources
- Flash loans cannot manipulate Chainlink prices
- **NOT manipulable by flash loans**

**If oracle is a spot DEX oracle (unlikely but possible):**
- If the oracle reads a Uniswap V2/V3 spot price, flash loans COULD manipulate it
- But the PSM's `minOraclePrice`/`maxOraclePrice` depeg bounds would likely revert the swap (extreme price movement triggers depeg protection)
- Even if the depeg bounds are wide, `min(peg, oracle)` clips the manipulated price — the attacker gets the peg-based amount (minus fee), not the manipulated amount
- **The `min()` defense makes even a manipulable oracle unprofitable** (as analyzed in Vector 2)

#### Cross-Oracle Interaction

Each collateral has its own `oracleFeed` (per `CollateralConfig`). The PSM reads only the oracle for the specific collateral in the swap order:

```solidity
CollateralState storage _collateralState = collateralState.get(order.collateral);
(uint256 oraclePrice, uint256 updatedAt) = IOracleFeed(_collateralConfig.oracleFeed).getPrice();
```

**Manipulating collateral A's oracle does NOT affect collateral B's swap.** Each swap reads only its own collateral's oracle. There's no cross-oracle interaction.

#### Multiple PSMs?

There's ONE PSM contract with multiple collaterals (via `collateralState` mapping). Each collateral has its own config (oracle, custodians, limits, fees). Swaps for different collaterals use different oracles. No shared oracle state across collaterals.

#### Pyth Update Frontrunning

With Pyth, the caller pushes a price update before reading it. In PSM.swap, the price is read at the beginning:
```solidity
(uint256 oraclePrice, uint256 updatedAt) = IOracleFeed(_collateralConfig.oracleFeed).getPrice();
```

This `getPrice()` call likely includes the Pyth price update (since it's non-view). The update is atomic within the swap transaction. An attacker cannot front-run the price update because:
1. The update and the swap are in the same transaction
2. The Pyth price is valid for a short window (controlled by `maxOracleAge`)
3. MEV searchers can't insert a different price between the update and the swap (same transaction)

### Required Conditions for Exploitation
- The oracle must be a manipulable spot DEX oracle (unlikely — interface suggests Pyth)
- The depeg bounds must be wide enough to allow the manipulated price (admin-configured)
- The `min(peg, oracle)` must be bypassed (impossible — hardcoded in `_getQuote`)
- Flash loan must affect the oracle price within a single transaction (impossible for Pyth/Chainlink)

### Blockers
- Oracle is likely Pyth (non-view getPrice, 15s clock drift tolerance) — off-chain, not flash-loan manipulable
- Each collateral has its own oracle — no cross-oracle interaction
- `min(peg, oracle)` clips any oracle manipulation benefit
- `minOraclePrice`/`maxOraclePrice` depeg bounds revert extreme price movements
- `maxOracleAge` staleness check prevents stale price exploitation

### Severity: **NOT EXPLOITABLE** (assuming Pyth/Chainlink oracle). Residual risk: if the oracle is a custom spot-DEX oracle, the `min()` still prevents profit, but a stale/frozen oracle could allow swaps at wrong prices (DoS or unfair pricing, not fund theft).

---

## Vector 10: Rate Limit Bypass Across Contracts

### Question
Can an attacker split swaps across multiple benefactors? Multiple collaterals? Can OFT bridges bypass PSM rate limits?

### Feasibility Analysis

**NOT EXPLOITABLE by external attackers.** The global rate limit is the ultimate cap. Rate limit bypass requires compromised admin/role holders.

#### PSM Rate Limit Architecture

PSM has **6 layers** of rate limits:

| Layer | Scope | Direction | Epoch (short) | Period (long) |
|-------|-------|-----------|---------------|---------------|
| Global | All swaps | swapForAsset | `maxSwapForAssetPerEpoch` | `maxSwapForAssetPerPeriod` |
| Global | All swaps | swapForCollateral | `maxSwapForCollateralPerEpoch` | `maxSwapForCollateralPerPeriod` |
| Per-collateral | One collateral | swapForAsset | `maxSwapForAssetPerEpoch` | `maxSwapForAssetPerPeriod` |
| Per-collateral | One collateral | swapForCollateral | `maxSwapForCollateralPerEpoch` | `maxSwapForCollateralPerPeriod` |
| Per-benefactor | One benefactor | swapForAsset | `maxSwapForAssetPerEpoch` (or default) | `maxSwapForAssetPerPeriod` (or default) |
| Per-benefactor | One benefactor | swapForCollateral | `maxSwapForCollateralPerEpoch` (or default) | `maxSwapForCollateralPerPeriod` (or default) |

Each swap is checked against ALL 6 applicable limits. If any limit is exceeded, the swap reverts.

#### Split Across Multiple Benefactors

An attacker could use multiple benefactor addresses to bypass the **per-benefactor** limit. But:
1. **Each benefactor must be added by BENEFACTOR_MANAGER_ROLE** (admin-gated). An external attacker cannot create new benefactors.
2. The **global limit** still applies across ALL benefactors. `globalSwappedForAssetInEpoch += amountOut` accumulates across all benefactors.
3. The **per-collateral limit** still applies across all benefactors using the same collateral.

**Result:** Splitting across benefactors bypasses per-benefactor limits but NOT global or per-collateral limits. The global limit is the ultimate cap.

#### Split Across Multiple Collaterals

An attacker could use multiple collateral types to bypass the **per-collateral** limit. But:
1. Each collateral must be added by COLLATERAL_MANAGER_ROLE (admin-gated).
2. The **global limit** still applies across ALL collaterals.

**Result:** Splitting across collaterals bypasses per-collateral limits but NOT the global limit.

#### Combined: Multiple Benefactors × Multiple Collaterals

Even with N benefactors × M collaterals, the **global limit** caps the total swap volume per epoch/period. No combination of benefactors and collaterals can exceed the global limit.

#### OFT Bridge to Bypass PSM Rate Limits

PSM is mainnet-only (swaps between mainnet collateral and USDtb). OFT bridges operate on USDe/sUSDe/ENA — NOT USDtb. There's no USDtb OFT.

Could an attacker:
1. Swap collateral → USDtb via PSM (rate-limited)
2. Bridge USDtb to L2 (no USDtb OFT — impossible)
3. ... no further interaction

**OFT bridges cannot bypass PSM rate limits** because:
- There's no USDtb OFT (USDtb is mainnet-only)
- USDe/sUSDe/ENA OFTs don't interact with PSM (different tokens)
- PSM rate limits are per-epoch/per-period on mainnet — bridging doesn't reset them

#### setEpochDuration / setPeriodDuration Reset

`setEpochDuration` and `setPeriodDuration` use separate state mappings per duration:
```solidity
EpochState storage _globalEpochState = _globalState.epochStateByDuration[_epochDuration];
```

When the duration changes, the new duration maps to a new epoch state that starts at 0 (effectively resetting the rate limit usage). This is documented as SD-3 (LOW) in the PSM deep analysis.

**This requires EPOCH_PERIOD_MANAGER_ROLE** (trusted role). A compromised role holder could:
1. Set epochDuration from 10s to 20s → new epoch state starts at 0
2. Swap up to the full limit again
3. Set epochDuration back to 10s → another new epoch state starts at 0
4. Repeat

**This is a trusted-role risk, not an external attacker exploit.** The EPOCH_PERIOD_MANAGER_ROLE is admin-granted and expected to be trusted.

#### Delegated Signer Bypass

A delegated signer can submit swaps on behalf of a benefactor. The rate limit is tracked per-benefactor (not per-signer):
```solidity
_benefactorEpochState.swappedForAssetInEpoch += amountOut;
```

Multiple delegated signers for the same benefactor share the benefactor's rate limit. **No bypass.**

But if the attacker has multiple benefactors (each with delegated signers), they could split across benefactors. As analyzed above, this bypasses per-benefactor limits but not the global limit.

#### Cross-Contract Rate Limit Interaction

USDtbMinting has its own rate limits (per-block mint/redeem caps). These are independent of PSM rate limits. An attacker can't use USDtbMinting to bypass PSM limits because:
- USDtbMinting limits are on mint/redeem volume (USDtb token operations)
- PSM limits are on swap volume (collateral ↔ USDtb swaps)
- They're different operations on different contracts
- An attacker could mint USDtb via USDtbMinting (up to USDtbMinting limits) and then swap via PSM (up to PSM limits) — but both limits apply independently

### Required Conditions for Exploitation
- **External attacker:** Must have multiple benefactors (requires BENEFACTOR_MANAGER_ROLE) and multiple collaterals (requires COLLATERAL_MANAGER_ROLE). Still capped by global limit.
- **Compromised EPOCH_PERIOD_MANAGER:** Can reset rate limits by changing duration. Trusted-role risk.
- **Compromised admin:** Can set global limits to max (effectively unlimited). Trusted-role risk.

### Blockers
- Global rate limit applies across ALL benefactors and ALL collaterals — cannot be bypassed by splitting
- Benefactor addition requires BENEFACTOR_MANAGER_ROLE (admin-gated)
- Collateral addition requires COLLATERAL_MANAGER_ROLE (admin-gated)
- No USDtb OFT exists — OFT bridges can't bypass PSM rate limits
- Delegated signers share the benefactor's rate limit (not separate)
- setEpochDuration/setPeriodDuration reset requires EPOCH_PERIOD_MANAGER_ROLE (trusted)
- All rate limit checks happen BEFORE transfers (CEI honored, nonReentrant)

### Severity: **NOT EXPLOITABLE** by external attackers. LOW (trusted-role risk) for EPOCH_PERIOD_MANAGER_DURATION reset.

---

## Additional Cross-Contract Vectors Analyzed

### Bonus A: StakedUSDeOFTAdapter._credit — abi.decode Before success Check

From the OFT deep analysis (F-1), `StakedUSDeOFTAdapter._credit` does:
```solidity
(bool success, bytes memory data) = address(innerToken).call(
    abi.encodeWithSignature("hasRole(bytes32,address)", FULL_RESTRICTED_STAKER_ROLE, _to)
);
bool isBlackListed = abi.decode(data, (bool));  // decoded BEFORE success check
if (!success || isBlackListed) { ... }
```

If the `hasRole` call fails and returns empty data, `abi.decode(data, (bool))` reverts (can't decode 0 bytes as bool). This would block all incoming sUSDe bridges until the innerToken recovers.

**Cross-contract impact:** If StakedUSDeV2 is ever paused/broken, all L2→mainnet sUSDe bridges are blocked. But StakedUSDeV2 is not upgradeable (constructor-based) and `hasRole` is a simple mapping lookup that can't fail unless the contract is self-destructed (impossible in Solidity 0.8.x).

**Severity: LOW (robustness gap, not exploitable in practice)**

### Bonus B: USDtb transferState + PSM Interaction

USDtb has a `transferState` (FULLY_ENABLED, WHITELIST_ENABLED, FULLY_DISABLED). In FULLY_DISABLED, all USDtb transfers revert — including PSM swaps (the `_beforeTokenTransfer` hook reverts).

If admin switches to WHITELIST_ENABLED and forgets to whitelist PSM custodians, PSM swaps would fail (DoS). This is an admin configuration error, not an attacker exploit.

**Severity: INFORMATIONAL (admin configuration risk)**

### Bonus C: Cross-Contract Reentrancy via ERC777/ERC1363 Tokens

If a collateral token in PSM is an ERC777/ERC1363 (with transfer callbacks), could an attacker reenter during a swap?

PSM.swap is `nonReentrant`. The transfer callbacks happen inside `safeTransferFrom`, which is inside `swap`. The reentrancy guard blocks reentry to PSM.swap and ALL other PSM functions (all are `nonReentrant`).

Could the attacker call a DIFFERENT contract (e.g., USDtbMinting, StakedUSDe) during the callback? Yes, but:
- USDtbMinting.mint/redeem are `nonReentrant` (their own guard)
- StakedUSDe._deposit/_withdraw are `nonReentrant` (their own guard)
- These are different contracts with independent state — no cross-contract state corruption
- PSM's effects (rate limits, nonce) are already applied before the transfer (CEI honored)

**Severity: NOT EXPLOITABLE** (universal nonReentrant + CEI)

### Bonus D: USDtb Blacklist + PSM Interaction

If a PSM benefactor is blacklisted on USDtb (BLACKLISTED_ROLE):
- swapForAsset (collateral → USDtb): The USDtb transfer TO the beneficiary would be blocked if the beneficiary is blacklisted. The collateral transfer FROM the benefactor is not affected (it's a different token).
- swapForCollateral (USDtb → collateral): The USDtb transfer FROM the benefactor would be blocked (blacklisted users can't transfer USDtb out).

This is expected behavior — blacklisted users can't use USDtb. No exploit.

The admin can `redistributeLockedAmount` to confiscate blacklisted users' USDtb. This requires DEFAULT_ADMIN_ROLE on USDtb. Not an external attacker exploit.

**Severity: NOT EXPLOITABLE**

---

## Findings Summary

| Vector | Description | Exploitable? | Severity |
|--------|-------------|-------------|----------|
| 1 | PSM ↔ USDtbMinting arbitrage | No | NOT EXPLOITABLE (trusted-operator risk) |
| 2 | PSM ↔ StakedUSDe exchange rate manipulation | No | NOT EXPLOITABLE (min(peg,oracle) defense) |
| 3 | USDtb ↔ OFT cross-chain bridge | N/A | NOT APPLICABLE (no USDtb OFT) |
| 4 | EthenaMinting ↔ PSM cross-contract reentrancy | No | NOT EXPLOITABLE (different assets, nonReentrant) |
| 5 | StakedENA ↔ ENASilo ↔ ENA (blacklist + governance) | Partially | MEDIUM (M-1, known/documented blacklist bypass) |
| 6 | OFT Adapter ↔ blacklist escape | No | NOT EXPLOITABLE (dual-side enforcement) |
| 7 | ENA governance flash-loan voting | Unknown | CANNOT ANALYZE (no governance contract in scope) |
| 8 | rescueFunds abuse across contracts | No | NOT EXPLOITABLE (no user funds in contract balances) |
| 9 | Oracle manipulation across PSM | No | NOT EXPLOITABLE (likely Pyth, min() defense) |
| 10 | Rate limit bypass across contracts | No | NOT EXPLOITABLE (global limit is ultimate cap) |
| Bonus A | StakedUSDeOFTAdapter abi.decode before success check | No | LOW (robustness gap) |
| Bonus B | USDtb transferState + PSM interaction | No | INFORMATIONAL (admin config risk) |
| Bonus C | Cross-contract reentrancy via ERC777/ERC1363 | No | NOT EXPLOITABLE (universal nonReentrant) |
| Bonus D | USDtb blacklist + PSM interaction | No | NOT EXPLOITABLE (expected behavior) |

---

## Key Design Properties That Prevent Cross-Contract Exploitation

### 1. PSM's `min(pegPrice, oraclePrice)` Pricing

The single most important cross-contract defense. In both swap directions, the PSM takes the minimum of the peg-based amount and the oracle-based amount. This means:
- The protocol NEVER overpays, regardless of oracle price
- Oracle manipulation cannot produce profit (the min clips the manipulated amount)
- Cross-contract arbitrage with USDtbMinting is impossible (PSM never gives more value than the peg)

This one design decision neutralizes Vectors 1, 2, 9, and the cross-arbitrage bonus.

### 2. Universal `nonReentrant`

Every state-changing function across ALL contracts uses `ReentrancyGuard`:
- PSM: swap, all admin functions, getQuote
- USDtbMinting: mint, redeem, transferToCustody
- EthenaMinting: mint, mintWETH, redeem, transferToCustody
- StakedUSDe/V2: _deposit, _withdraw, transferInRewards, redistributeLockedAmount, unstake, rescueTokens
- StakedENA: _deposit, _withdraw, transferInRewards, redistributeLockedAmount, unstake, rescueTokens, setCooldownDuration, addToBlacklist, removeFromBlacklist

Cross-contract reentrancy via token callbacks is universally blocked. Even if an attacker reenters a DIFFERENT contract during a callback, the first contract's state is already updated (CEI honored) and the second contract has its own guard.

### 3. Dual-Side Blacklist Enforcement on OFT

For sUSDe (the only token with both mainnet and L2 blacklist mechanisms):
- **Send side:** StakedUSDeV2 `_beforeTokenTransfer` blocks transfers from FULL_RESTRICTED users (mainnet); StakedUSDeOFT `_update` blocks transfers from blacklisted users (L2)
- **Receive side:** StakedUSDeOFTAdapter `_credit` redirects to owner if recipient is FULL_RESTRICTED (mainnet); StakedUSDeOFT `_credit` redirects to owner if recipient is blacklisted (L2)

Four layers of defense prevent cross-chain blacklist escape.

### 4. Separation of Concerns

PSM (USDtb), USDtbMinting (USDtb), EthenaMinting (USDe), StakedUSDe (sUSDe), StakedENA (sENA) operate on:
- Different tokens
- Different custodian sets
- Different access control systems
- Different pricing mechanisms

No shared mutable state across contracts. A bug in one contract cannot corrupt another's state.

### 5. `rescueFunds` Scope Limitations

- PSM holds 0 funds in normal operation (all transfers are direct benefactor↔custodian)
- StakedUSDe/StakedENA explicitly block rescuing the staked asset
- USDtb contract doesn't hold user balances in its token balance (mapping entries)
- Silos have no rescue function (only staking vault can withdraw)

Admin cannot steal user funds via rescue functions.

---

## Residual Risks (Not Exploitable as Smart Contract Bugs)

### 1. Trusted-Operator Risk (USDtbMinting + EthenaMinting)

Both minting contracts use off-chain RFQ pricing. If the MINTER/REDEEMER operator submits orders with stale or misaligned prices, users could profit from the price discrepancy. This is a process risk, not a code bug. The on-chain safety nets (`verifyStablesLimit` for USDtb, no price check for USDe) are designed as backstops, not primary pricing mechanisms.

### 2. Oracle Implementation Unknown

The actual oracle implementation (`IOracleFeed`) is in Ethena's private `onchain-minting-internal` package. The interface suggests Pyth (non-view `getPrice`, 15s clock drift tolerance), but if it's a custom spot-DEX oracle, flash loans could theoretically manipulate it. Even then, `min(peg, oracle)` prevents profit from manipulation. The residual risk is unfair pricing (not fund theft) if the oracle is stale or frozen.

### 3. Governance Contract Not in Scope

The Immunefi scope mentions "Manipulation of governance voting result." ENA.sol has no voting functions. If a separate governance contract reads live `ENA.balanceOf()` for voting power, flash-loan voting would be possible. This is a governance contract bug, not an ENA/StakedENA bug. Without the governance contract, this cannot be confirmed or denied.

### 4. EPOCH_PERIOD_MANAGER Duration Reset

`setEpochDuration`/`setPeriodDuration` reset rate limit usage when the duration changes. A compromised EPOCH_PERIOD_MANAGER_ROLE holder could repeatedly change durations to bypass rate limits. This is a trusted-role risk (LOW, documented as SD-3 in the PSM deep analysis).

### 5. Known Medium (M-1): StakedENA/StakedUSDeV2 Blacklist Bypass

`unstake` doesn't check blacklist status. A user who enters cooldown before being blacklisted can claim funds after cooldown expires, even while blacklisted. This is documented in both StakedUSDeV2 and StakedENA contract comments as intentional ("ENA/USDe is a permissionless token once unstaked"). Known since Oct 2023 (StakedUSDeV2), likely not a fresh Immunefi target.

---

## Conclusion

**10 cross-contract attack vectors were investigated in depth. 0 are exploitable as smart contract bugs.**

The Ethena contract suite is well-architected against cross-contract attacks. The key defenses are:
1. PSM's `min(peg, oracle)` pricing (neutralizes oracle manipulation and cross-contract arbitrage)
2. Universal `nonReentrant` (blocks cross-contract reentrancy)
3. Dual-side OFT blacklist enforcement (blocks cross-chain blacklist escape)
4. Separation of concerns (no shared mutable state)
5. `rescueFunds` scope limitations (admin can't steal user funds)

The only confirmed finding is the known M-1 (blacklist bypass via cooldown) which is documented as intentional and has been live since Oct 2023. All other vectors are either not exploitable, not applicable, or require trusted-operator/admin compromise.

**No Immunefi submission is warranted based on this cross-contract analysis.**

---

## Files Referenced

- `/home/z/fkr-step1/defi-bounty/contracts/PSM.sol` (2,082 lines)
- `/home/z/fkr-step1/defi-bounty/contracts/deps/IPSM.sol` (495 lines)
- `/home/z/fkr-step1/defi-bounty/contracts/deps/IOracleFeed.sol` (51 lines)
- `/home/z/fkr-step1/defi-bounty/contracts/deps/CollateralStateMap.sol` (48 lines)
- `/home/z/ethena-usdtb/contracts/usdtb/USDtbMinting.sol` (681 lines)
- `/home/z/ethena-usdtb/contracts/usdtb/USDtb.sol` (204 lines)
- `/home/z/ethena-usde/contracts/contracts/EthenaMinting.sol` (551 lines)
- `/home/z/ethena-usde/contracts/contracts/StakedUSDeV2.sol` (131 lines)
- `/home/z/ethena-usde/contracts/contracts/StakedUSDe.sol` (268 lines)
- `/home/z/ethena-usde/contracts/contracts/USDeSilo.sol` (30 lines)
- `/home/z/fkr-step1/defi-bounty/contracts/StakedENA.sol` (410 lines)
- `/home/z/fkr-step1/defi-bounty/contracts/ENASilo.sol` (28 lines)
- `/home/z/ethena-usde/contracts/contracts/ENA.sol` (55 lines)
- `/home/z/fkr-step1/defi-bounty/contracts/USDeOFT.sol` (72 lines)
- `/home/z/fkr-step1/defi-bounty/contracts/USDeOFTAdapter.sol` (70 lines)
- `/home/z/fkr-step1/defi-bounty/contracts/StakedUSDeOFT.sol` (112 lines)
- `/home/z/fkr-step1/defi-bounty/contracts/StakedUSDeOFTAdapter.sol` (57 lines)
- `/home/z/fkr-step1/defi-bounty/contracts/ENAOFT.sol` (72 lines)
- `/home/z/fkr-step1/defi-bounty/contracts/ENAOFTAdapter.sol` (70 lines)
- `/home/z/fkr-step1/defi-bounty/contracts/libs/OFTCore.sol` (397 lines)
- `/home/z/fkr-step1/defi-bounty/contracts/libs/OFT.sol` (87 lines)
- `/home/z/fkr-step1/defi-bounty/contracts/libs/OFTAdapter.sol` (105 lines)
- `/home/z/fkr-step1/defi-bounty/contracts/libs/RateLimiter.sol` (81 lines)
- `/home/z/fkr-step1/defi-bounty/contracts/libs/OFTOwnable2Step.sol` (77 lines)

Prior analysis files referenced:
- `ethena-psm-deep-analysis.md` (RB-1, RC-2, SD-3, GQ-1, SG-1 findings)
- `ethena-oft-deep-analysis.md` (F-1 through F-10 findings)
- `ethena-stakedena-deep-analysis.md` (M-1 blacklist bypass)
- `ethena-usde-deep-analysis.md` (V2-1 blacklist bypass)
- `ethena-usdtb-minting-analysis.md` (verifyStablesLimit asymmetric check)
- `ethena-old-vs-new-regression.md` (uint128 downgrade, block limits)
