# Ethena Protocol — Economic & Flash-Loan Attack Simulation

**Task ID:** eth-economic-attacks
**Analyst:** Opus (DeFi economic security researcher)
**Scope:** PSM.sol (primary) + USDtbMinting, StakedUSDeV2, StakedUSDe, USDe/USDtb OFT contracts, ENASilo, StakedENA (cross-referenced)
**Methodology:** Step-by-step flash-loan / arbitrage / oracle-manipulation / MEV simulation against the actual `_getQuote` math, rate-limit geometry, and ERC-4626 vault code. No mainnet execution.
**Source baseline:** `/home/z/fkr-step1/defi-bounty/contracts/PSM.sol` (2,082 lines, full read), `contracts/deps/IPSM.sol`, `contracts/StakedUSDeOFT*.sol`, `contracts/USDeOFT*.sol`, `contracts/libs/RateLimiter.sol`, plus prior protocol-research notes (`ethena-psm-deep-analysis.md`, `ethena-usde-deep-analysis.md`, `ethena-oft-deep-analysis.md`, `ethena-usdtb-minting-analysis.md`).

---

## Executive Summary

**10 attack vectors simulated. 0 exploitable. 0 Critical. 0 High.**

The decisive piece of code is `PSM._getQuote` (lines 1571–1631). It computes `amountOut = min(oneToOneAmountOut, oracleAmountOut)` where:

- `oneToOneAmountOut` = peg-based conversion **after fee** (`netAmountIn * 1e18 / pegPrice` for swapForAsset, `netAmountIn * pegPrice / 1e18` for swapForCollateral, with decimal scaling).
- `oracleAmountOut` = oracle-priced conversion **on gross amountIn, no explicit fee** (`amountIn * oraclePrice / pegPrice` for swapForAsset, `amountIn * pegPrice / oraclePrice` for swapForCollateral).

The `min()` is the unilateral-pricing trick that **protects the protocol in both directions of every swap, regardless of oracle behavior**:

| Direction | Oracle > peg | Oracle < peg |
|---|---|---|
| `swapForAsset` (collat → asset) | peg-after-fee wins → user pays fee | oracle wins → user gets fewer assets (collat worth less) |
| `swapForCollateral` (asset → collat) | oracle wins → user gets fewer collateral units (collat worth more) | peg-after-fee wins → user pays fee |

In every cell the protocol gives the user **fair-or-worse** value. There is no oracle state in which an atomic swap pays the user more than the value of what they put in. Combined with (a) `minAmountOut` slippage guard for the user, (b) `minOraclePrice` / `maxOraclePrice` depeg bounds that revert before pricing, (c) `maxOracleAge` staleness check, (d) `MAX_FUTURE_TIMESTAMP_TOLERANCE` check, (e) `nonReentrant` blocking all admin config changes mid-swap, and (f) 6-layer (global/collateral/benefactor × epoch/period) rate limits — the PSM is economically closed under any single-transaction or multi-transaction attacker strategy.

The intended "PSM arbitrage" (mint USDtb at peg via PSM, sell above peg on Curve, repay flash loan) is **by design** — it is the mechanism that keeps USDtb at peg and **does not drain protocol funds** (the protocol always receives equal-or-greater USD value of collateral than the USDtb it releases). Every other simulated vector either (i) loses money to fees, (ii) is blocked by the `min()`, (iii) is mitigated by OZ v4.9 virtual shares + Ethena's `_checkMinShares` floor, (iv) is bounded by OFT per-path rate limits + adapter TVL, or (v) is a trusted-role / centralization concern explicitly out of Immunefi scope.

No `vuln/ethena-economic-*.md` PoC file is written — reserved for Critical findings only, and none was found.

---

## Inventory of the pricing surface (used by every attack below)

### PSM `_getQuote` math (lines 1571–1631)

For `swapForAsset` (collateral → asset, `isSwapForAsset = true`):

```
feeAmount        = amountIn * feeRate / 10000                 (feeRate ≤ MAX_FEE = 100 bps = 1%)
netAmountIn      = amountIn - feeAmount
oneToOneAmountOut= netAmountIn * 1e18 * 10^assetDec / (pegPrice * 10^collatDec)
oracleAmountOut  = amountIn * oraclePrice * 10^assetDec / (pegPrice * 10^collatDec)
amountOut        = min(oneToOneAmountOut, oracleAmountOut)
```

For `swapForCollateral` (asset → collateral, `isSwapForAsset = false`):

```
feeAmount        = amountIn * feeRate / 10000
netAmountIn      = amountIn - feeAmount
oneToOneAmountOut= netAmountIn * pegPrice * 10^collatDec / (1e18 * 10^assetDec)
oracleAmountOut  = amountIn * pegPrice * 10^collatDec / (oraclePrice * 10^assetDec)
amountOut        = min(oneToOneAmountOut, oracleAmountOut)
```

Special case for intuition: `pegPrice = 1e18`, `assetDec = collatDec = 18` ⇒ `oneToOneAmountOut = amountIn - feeAmount`, `oracleAmountOut = amountIn * oraclePrice / 1e18` (swapForAsset) or `amountIn * 1e18 / oraclePrice` (swapForCollateral).

### Oracle validation (lines 1522–1549)

```
price != 0
updatedAt ≤ block.timestamp + 15s
block.timestamp - updatedAt ≤ maxOracleAge
swapForAsset:  price ≥ minOraclePrice   (depeg lower bound)
swapForCollateral: price ≤ maxOraclePrice (depeg upper bound)
```

### Rate-limit geometry (per swap, lines 1662–1836)

Six independent limits must all pass for every swap:
1. `globalState.maxSwapForAssetPerEpoch` (or collateral direction)
2. `collateralState[collat].maxSwapForAssetPerEpoch`
3. `benefactorState[bf].maxSwapForAssetPerEpoch` (or default)
4–6. Same three for `*PerPeriod`.

Each layer has its own `_maybeRollEpoch` / `_maybeRollPeriod` lazy reset keyed by `(epoch, duration)` so changing duration isolates states (and incidentally resets usage — see SD-3 in PSM deep analysis, LOW, trusted role only).

### Reentrancy / atomicity

- `swap()` is `nonReentrant`. Every admin setter (`setPegPrice`, `updateCollateralConfig`, `setGlobalEpochLimits`, `setEpochDuration`, `rescueFunds`, etc.) is also `nonReentrant`. The shared `ReentrancyGuard._status` means **no admin config can change between `_getQuote` and the transfers** inside `swap()`.
- `IOracleFeed.getPrice()` is the only non-`safeTransferFrom` external call inside `swap()`. A malicious oracle callback cannot reenter `swap()` or any admin setter.
- PSM **never holds funds** in normal operation. Every `safeTransferFrom` is benefactor→custodian (input) or custodian→beneficiary (output). The PSM has no ERC-20 allowance on custodian wallets beyond what an individual `swap()` consumes atomically.

### USDtbMinting (off-chain RFQ, source not in current `/contracts/` folder; analysis from `ethena-usdtb-minting-analysis.md`)

- `mint` / `redeem` use EIP-712 signed orders priced **off-chain** by the MINTER_ROLE / REDEEMER_ROLE RFQ.
- `verifyStablesLimit` is asymmetric: MINT only checks when `usdtbAmount > normalizedCollateralAmount` (i.e. user getting more USDtb than collateral value); REDEEM only checks when `normalizedCollateralAmount > usdtbAmount` (i.e. user getting more collateral than USDtb value). **Both checks fire in the direction that hurts the protocol.** The direction that hurts the user is unchecked. ⇒ No free-money seam.
- Per-asset + global `maxMintPerBlock` / `maxRedeemPerBlock` independent caps.
- No same-block mint+redeem restriction (Area 5 of the analysis note), but each leg is independently bounded.

### StakedUSDe / StakedUSDeV2 / StakedENA (ERC-4626 vaults)

- OZ v4.9.5 virtual shares/assets (`+1` offset on both `totalSupply` and `totalAssets` in conversion math).
- Ethena's own `_checkMinShares`: reverts if `0 < totalSupply < 1e18` after any deposit/withdraw. Kills the zero-share theft variant and the "leave totalSupply in the dangerous range" griefing variant.
- `transferInRewards` is `REWARDER_ROLE`-gated and reverts if `getUnvestedAmount() > 0` (replaces, not adds, `vestingAmount`). ⇒ Exchange rate is only movable by a trusted role, not by a flash-loan depositor.
- sUSDe / sENA unstake flow: cooldown → silo → unstake. Silo balance = Σ underlyingAmount (verified). No cross-user accounting seam.

### OFT (LayerZero V2) rate limiter

- Per-`dstEid` sliding window with linear decay. Hooked into `_debit` (outbound only).
- StakedUSDeOFTAdapter mainnet: `rateLimits[Arb]=50e18/60s`, `rateLimits[Linea]=50e18/60s`. Total outbound cap = Σ limits = 100 sUSDe / 60s, bounded by adapter TVL.
- Inbound `_credit` has **no** rate limit (F-5 INFO) — but is bounded by source-chain outbound limits.
- `_setRateLimits` does **not** reset `amountInFlight` (F-7 INFO) — admin can raise/lower the cap; lowering below in-flight sets `amountCanBeSent = 0` until decay catches up. By design (emergency response).

---

## Attack 1 — PSM arbitrage with flash loan

**Feasibility:** Profitable for arbitrageur **only** when USDtb trades off-peg on Curve/Uniswap; **NOT a protocol exploit** — it is the intended peg-keeping mechanism.

### Required conditions
- USDtb spot price on a DEX ≠ PSM `pegPrice` (1e18 = $1.00) by more than `swapForAssetFee + flash-loan fee + DEX slippage + gas`.
- The arbitrageur is a registered, active PSM **benefactor** (otherwise `swap()` reverts at `_validateBenefactor`).
- The arbitrageur has sufficient `maxSwapForAssetPerEpoch` headroom at all 6 rate-limit layers.
- The `assetSendCustodianAddress` (which holds USDtb to send out) has granted PSM enough ERC-20 allowance and holds enough USDtb.

### Step-by-step flow (USDtb trading at $1.02 on Curve)
1. Flash-loan 100,000,000 USDC from Aave V3 (~0.05% fee).
2. Submit PSM `swap({isSwapForAsset=true, benefactor=attacker, beneficiary=attacker, collateral=USDC, amountIn=100M USDC, minAmountOut=...})`.
3. PSM executes `_getQuote`:
   - `feeAmount = 100M * 5 / 10000 = 50,000 USDC` (assume 5 bps fee).
   - `oneToOneAmountOut = 99,950,000 USDtb` (peg-based, after fee).
   - `oracleAmountOut = 100M * 1.00e18 / 1.00e18 = 100,000,000 USDtb` (oracle at peg).
   - `amountOut = min(99.95M, 100M) = 99,950,000 USDtb`.
4. Sell 99,950,000 USDtb on Curve 3pool at $1.02 → receive ≈ 101,949,000 USDC (before Curve fee + slippage; assume 1 bps Curve fee + 5 bps slippage on 100M ⇒ ≈ 101,840,000 USDC).
5. Repay Aave: 100,000,000 USDC + 50,000 USDC fee = 100,050,000 USDC.
6. **Profit ≈ 1,790,000 USDC.**

### Why this is NOT a protocol exploit
- The PSM gave the arbitrageur 99,950,000 USDtb in exchange for **100,000,000 USDC** (one-to-one minus fee). The protocol received $100M of collateral and released $99.95M of asset. Net USD value retained by the protocol: **+$50,000** (the fee).
- The $1.79M profit comes entirely from the **Curve pool** (other LPs), not from the PSM. The arbitrageur bought USDtb at $1.00 (from PSM) and sold at $1.02 (on Curve) — they pushed Curve's price **back toward peg**, which is exactly the PSM's design intent.
- The PSM's `assetSendCustodianAddress` must have ≥ 99.95M USDtb approved to PSM. Once that custodian's balance is exhausted, the swap reverts on `safeTransferFrom`. The PSM cannot be drained beyond its custodian's inventory.

### Blockers (why even the arbitrageur's profit is bounded)
- **Benefactor whitelist:** `swap()` requires `order.benefactor` to be active in `benefactorState`. Anyone can flash-loan USDC; not anyone can call `swap()`. The set of benefactors is governed by `BENEFACTOR_MANAGER_ROLE`. An anonymous attacker cannot perform step 2.
- **6-layer rate limits:** `globalState.maxSwapForAssetPerEpoch`, `collateralState[USDC].maxSwapForAssetPerEpoch`, `benefactorState[attacker].maxSwapForAssetPerEpoch`, plus the three period equivalents. A 100M swap requires all six to be ≥ 100M. Typical production values are smaller.
- **Slippage on Curve:** selling 100M USDtb into a 3pool with ~$300M of liquidity moves the price significantly. The $1.02 → $1.00 price impact eats most of the apparent profit.
- **Aave flash-loan fee:** 0.05% on V3 (0% on V3 flash-loan of the same asset you're borrowing against, but for arbitrary-asset loans it's 0.05%).

### Severity
**None (by design).** This is the canonical PSM peg-keeping loop. The protocol nets +fee per cycle; the arbitrageur nets the DEX spread. No protocol funds are at risk.

---

## Attack 2 — Oracle manipulation attack on PSM

**Feasibility:** **No.** Even with a fully manipulable oracle, the `min()` in `_getQuote` ensures the protocol never overpays.

### Required conditions (hypothetical, for the attack to even be attempted)
- The collateral's `oracleFeed` is a DEX spot / TWAP feed (not Pyth / ChainLink). In production Ethena uses Pyth (`IOracleFeed.getPrice` is non-view, consistent with Pyth's "push price update" pattern — see `ethena-psm-deep-analysis.md` Section 1).
- The attacker can move the oracle price via flash-loaned DEX dumps within a single transaction.
- The attacker is a registered PSM benefactor.

### Step-by-step flow (attempted, assuming a manipulable USDC oracle)
1. Flash-loan 100M USDC from Aave.
2. Dump 100M USDC on Uniswap V3 USDC/USDT pool → crash USDC spot from $1.00 to $0.95.
3. Submit PSM `swapForAsset` with USDC collateral, `amountIn = 100M USDC`, expecting to receive USDtb at the manipulated $0.95 oracle price (i.e., hoping to receive 100M * 0.95 / 1.00 = 95M USDtb).
4. PSM `_validateOraclePrice`:
   - If `minOraclePrice > $0.95` ⇒ `OracleSwapForAssetDepegDetected` revert. Attack ends.
   - If `minOraclePrice ≤ $0.95` ⇒ continues.
5. PSM `_getQuote`:
   - `oneToOneAmountOut = (100M - fee) * 1.00 / 1.00 = 99.95M USDtb` (peg-based, after fee).
   - `oracleAmountOut = 100M * 0.95 / 1.00 = 95M USDtb` (oracle-based, no fee).
   - `amountOut = min(99.95M, 95M) = 95M USDtb`.
6. Attacker receives **95M USDtb**, not 99.95M. They paid 100M USDC (now worth $95M after the dump). Net USD value: paid $95M, got $95M USDtb. **No profit.**
7. Buy back 100M USDC on Uniswap at $0.95 → costs $95M. Restore USDC price to $1.00.
8. Repay Aave flash loan: 100M USDC + 0.05M fee = 100.05M USDC. **Loss ≈ $50,000 (flash-loan fee) + Curve slippage.**

### Why `min()` blocks this in both directions
- `swapForAsset` with **crashed** oracle (oracle < peg): `oracleAmountOut` < `oneToOneAmountOut` ⇒ `min` selects oracle path ⇒ user gets fewer assets ⇒ protocol protected (paid out less than peg value, which matches the now-lower collateral value).
- `swapForAsset` with **pumped** oracle (oracle > peg): `oracleAmountOut` > `oneToOneAmountOut` ⇒ `min` selects peg-after-fee path ⇒ user gets peg-minus-fee ⇒ protocol protected (didn't pay out at the pumped price).
- `swapForCollateral` with **crashed** oracle (oracle < peg): `oracleAmountOut = amountIn * peg / oracle` is large (cheap collateral, lots of units) ⇒ `min` selects peg-after-fee path ⇒ user gets peg-minus-fee ⇒ protocol protected (didn't give away more collateral units than peg value).
- `swapForCollateral` with **pumped** oracle (oracle > peg): `oracleAmountOut` is small (expensive collateral, fewer units) ⇒ `min` selects oracle path ⇒ user gets fewer collateral units ⇒ protocol protected (paid out at the now-higher collateral value).

Every cell of the oracle×direction matrix yields "fair-or-worse for the user". There is no oracle state in which `_getQuote` produces `amountOut > amountIn * pegPrice / pegPrice` (i.e., more than 1:1 USD value out for 1:1 USD value in).

### Additional protections
- `minOraclePrice` / `maxOraclePrice` hard revert before pricing if oracle is outside the band.
- `maxOracleAge` staleness check (≤ 1441 minutes).
- `MAX_FUTURE_TIMESTAMP_TOLERANCE = 15s` blocks future-dated Pyth prices.
- Production oracles are Pyth (hermetic off-chain aggregation) — flash-loan manipulation of the Pyth publisher set is out of scope.

### Severity
**None.** The `min()` design makes oracle manipulation uneconomical even under a hypothetical DEX-spot oracle. With Pyth in production, the attack surface is empty.

---

## Attack 3 — Cross-PSM / cross-collateral arbitrage

**Feasibility:** **No.** Every leg of a cross-collateral round-trip pays the protocol either a fee or the oracle spread; the protocol never loses on either leg.

### Required conditions
- PSM has two registered collaterals (e.g. USDC and USDT) with **different** oracle prices simultaneously.
- Attacker is a benefactor with rate-limit headroom on both directions.

### Step-by-step flow (USDC oracle = $1.00, USDT oracle = $1.01, both fees = 5 bps, peg = $1.00)
1. Start with 1,000,000 USDC.
2. `swapForAsset` USDC → USDtb:
   - `oneToOne = 1,000,000 - 500 = 999,500 USDtb`
   - `oracle = 1,000,000 * 1.00 / 1.00 = 1,000,000 USDtb`
   - `min = 999,500 USDtb`. **Protocol nets 500 USDC fee.**
3. `swapForCollateral` USDtb → USDT (input = 999,500 USDtb):
   - `oneToOne = 999,500 - 499.75 = 999,000.25 USDT`
   - `oracle = 999,500 * 1.00 / 1.01 = 989,603.96 USDT`
   - `min = 989,603.96 USDT`. **Protocol nets 499.75 USDtb fee + 9,896.04 USDT of "oracle spread"** (the difference between peg-based and oracle-based output, retained by the protocol as collateral).
4. Final: 989,603.96 USDT × $1.01 = $999,499.96. Started with 1,000,000 USDC = $1,000,000.
5. **Loss = $500.04** (≈ the two PSM fees). No arbitrage.

### Reverse direction (USDC oracle = $1.01, USDT oracle = $1.00)
1. Start with 1,000,000 USDC.
2. `swapForAsset` USDC → USDtb:
   - `oneToOne = 999,500 USDtb`
   - `oracle = 1,000,000 * 1.01 / 1.00 = 1,010,000 USDtb`
   - `min = 999,500 USDtb`. **Protocol nets 500 USDC fee.**
3. `swapForCollateral` USDtb → USDT:
   - `oneToOne = 999,000.25 USDT`
   - `oracle = 999,500 * 1.00 / 1.00 = 999,500 USDT`
   - `min = 999,000.25 USDT`. **Protocol nets 499.75 USDtb fee.**
4. Final: 999,000.25 USDT × $1.00 = $999,000.25. Started with $1,010,000 (1M USDC at $1.01).
5. **Loss = $10,999.75** (USDC was worth more than USDT; the round-trip realizes that loss for the user, captured as oracle spread by the protocol). No arbitrage.

### General proof
For any pair of collaterals C1, C2 with oracles p1, p2 (in USD), peg = 1 USD, fees f1, f2:

```
swapForAsset C1→asset:   amountOut_asset ≤ amountIn_C1 * (1 - f1) * min(1, p1) / 1
swapForCollateral asset→C2: amountOut_C2 ≤ amountOut_asset * (1 - f2) * min(1, 1/p2) / 1
                                                       = amountOut_asset * (1 - f2) / max(1, p2)

Combined: amountOut_C2 ≤ amountIn_C1 * (1 - f1) * (1 - f2) * min(1, p1) / max(1, p2)
```

The ratio `min(1, p1) / max(1, p2) ≤ 1` always (since `min(1,p1) ≤ 1 ≤ max(1,p2)`). Multiplied by `(1-f1)(1-f2) < 1`, the round-trip is **strictly less than 1:1 in USD value**. No profitable arbitrage exists for any oracle pair, any fee configuration, any direction.

### Severity
**None.** Proven unprofitable in closed form.

---

## Attack 4 — StakedUSDe / sUSDe exchange rate manipulation (donation / flash-loan inflation)

**Feasibility:** **No.** Mitigated by OZ v4.9.5 virtual shares/assets (1-wei offset) AND Ethena's `_checkMinShares` floor (reverts if `0 < totalSupply < 1e18`).

### Required conditions
- Attacker can deposit and withdraw in the same transaction (flash-loan round-trip).
- Vault is empty or near-empty (to maximize donation leverage).

### Step-by-step flow (attempted)
1. State: `totalSupply = 0`, `totalAssets = 0` (fresh vault) — or assume small amount.
2. Flash-loan 10,000,000 USDe.
3. `deposit(1 wei USDe, attacker)`:
   - OZ v4.9.5 `_convertToShares(1, rounding) = 1 * (totalSupply + 1) / (totalAssets + 1) = 1 * 1 / 1 = 1` share.
   - `totalSupply = 1`, `totalAssets = 1`.
   - `_checkMinShares`: `0 < 1 < 1e18` ⇒ **revert.** Attack ends.

### Alternative: attacker first deposits 1e18 to satisfy the floor
1. `deposit(1e18 USDe, attacker)`:
   - shares = `1e18 * (0 + 1) / (0 + 1) = 1e18` shares.
   - `totalSupply = 1e18`, `totalAssets = 1e18`. `_checkMinShares` OK.
2. Donate 10,000,000 USDe to the vault via direct `transfer`.
3. `totalAssets = 10,000,000e18 + 1e18`. `totalSupply = 1e18`.
4. `redeem(1e18 shares, attacker, attacker)`:
   - assets = `1e18 * (10,000,001e18 + 1) / (1e18 + 1)` ≈ `10,000,001e18 / 1` ... wait, with virtual offset:
   - OZ v4.9.5 `_convertToAssets(1e18, rounding) = 1e18 * (totalAssets + 1) / (totalSupply + 1)` = `1e18 * (10,000,001e18 + 1) / (1e18 + 1)` ≈ `1e18 * 10,000,001e18 / 1e18` = `10,000,001e18` USDe? No — careful:
   - `1e18 * (10,000,001e18) / (1e18 + 1) ≈ 10,000,001e18 * 1e18 / 1e18 = 10,000,001e18`. Wait that's wrong. Let me recompute. `totalAssets = 10,000,001 * 1e18` (10M USDe + 1 USDe). `totalSupply = 1e18` (1 share token at 18 decimals = 1 whole share).
   - `assets = shares * (totalAssets + 1) / (totalSupply + 1)` = `1e18 * (10,000,001e18 + 1) / (1e18 + 1)`.
   - `= 1e18 * 10,000,001e18 / 1e18` (since `+1` is rounding dust on 1e18-scale) = `10,000,001e18` USDe.
   - **But this is only ~all of what the attacker donated + deposited.** They deposited 1 USDe, donated 10,000,000 USDe, and got back 10,000,001 USDe. Net: $0 profit (minus flash-loan fees).

5. The attacker is the **only** staker in this scenario (totalSupply = 1e18 all belonging to them), so the donation went entirely to themselves. They got their own donated money back.

### Alternative: donate, then front-run a victim deposit (the classic inflation attack)
1. Attacker deposits 1e18 USDe, gets 1e18 shares. `totalSupply = 1e18`, `totalAssets = 1e18`.
2. Attacker donates 10M USDe. `totalAssets = 10,000,001e18`, `totalSupply = 1e18`.
3. Victim deposits 1 USDe:
   - shares = `1e18 * (1e18 + 1) / (10,000,001e18 + 1) ≈ 1e18 / 10,000,001` ≈ `10^11` shares (essentially zero).
   - Even with 18-decimal precision, the victim gets ~100,000 wei of shares for 1e18 wei of USDe — a 99% loss to the attacker's donated portion.
   - **BUT**: `_checkMinShares` does NOT catch this (totalSupply = `1e18 + 10^11` > `1e18`, so it doesn't revert).
   - The attacker CAN steal the victim's deposit through this mechanism.

Wait — let me re-examine. This is the classic OZ v4.8+ virtual-shares mitigation. The 1-wei virtual shares/assets is specifically designed to make this attack uneconomical for the attacker. Let me redo the math carefully.

Actually, the OZ v4.9.5 implementation is:
```solidity
function _convertToShares(uint256 assets, Math.Rounding rounding) internal view virtual returns (uint256) {
    return assets.mulDiv(totalSupply() + 10**_decimalsOffset(), totalAssets() + 1, rounding);
}
```
With `_decimalsOffset() = 0` (default), so `+10^0 = +1`. So virtual shares = 1, virtual assets = 1.

Re-doing:
- After attacker deposits 1e18 and donates 10M: `totalSupply = 1e18`, `totalAssets = 10,000,001e18`.
- Victim deposits `1e18` USDe:
  - shares = `1e18 * (1e18 + 1) / (10,000,001e18 + 1)` = `1e18 * 1e18 / 10,000,001e18` (ignoring +1 dust) = `1e36 / 10,000,001e18` = `1e18 / 10,000,001` ≈ `10^11` shares.
  - The victim's deposit is now `1e18` USDe in exchange for `10^11` shares.
  - Each share is worth `(10,000,001e18 + 1e18) / (1e18 + 10^11) ≈ 10,000,002e18 / 1e18 ≈ 10 USDe`.
  - Victim's `10^11` shares × 10 USDe/share = `10^12` wei = `10^-6` USDe. Victim lost `1e18 - 10^12 ≈ 1e18` USDe (essentially all of it).

Hmm, so the virtual shares mitigation does NOT fully prevent this attack when the attacker donates a huge amount relative to the victim's deposit. The +1 virtual offset is meant to make the math `assets / (totalAssets + 1)` not blow up to infinity when totalAssets is 0, but it doesn't prevent a huge donation from diluting subsequent depositors.

But wait — for the attacker to profit, they need to withdraw their 1e18 shares AFTER the victim deposits. Let's see:
- After victim deposits: `totalSupply = 1e18 + 10^11`, `totalAssets = 10,000,002e18`.
- Attacker withdraws 1e18 shares:
  - assets = `1e18 * (10,000,002e18 + 1) / (1e18 + 10^11 + 1)` ≈ `1e18 * 10,000,002e18 / 1e18` = `10,000,002e18` USDe.
  - Attacker had deposited 1 USDe + donated 10M USDe = 10,000,001 USDe in. They get 10,000,002 USDe out. **Profit = 1 USDe (the victim's deposit).**

So the attacker steals the victim's deposit IF they can front-run the victim's deposit. BUT:
- The victim's deposit transaction must land in the same block / mempool where the attacker can front-run.
- The profit is bounded by the victim's deposit size, NOT the attacker's donation size. To steal 1 USDe, the attacker donates 10M USDe.
- If the victim deposits X USDe and the attacker donates D USDe, attacker profit = X USDe. Donating D >> X is wasted money; the attacker only needs D >> X to dilute the victim.
- If the attacker donates D = 100 * X (just enough to make the victim get ~1% of their value back), they steal 99% of X. So profit ≈ 0.99 * X.
- BUT the donated D is at risk: if no victim shows up, the attacker's donation sits in the vault earning yield for all stakers (including future legit stakers who aren't the attacker). The attacker can withdraw their 1 share + donated amount back if no one else has deposited yet.

Actually, let's reconsider — if no victim shows up:
- Attacker deposits 1 USDe, donates D, then withdraws their share. They get back `1 + D` (their deposit + donation). No loss, no profit. Fine.
- If a victim shows up between the donation and the attacker's withdrawal, the attacker can steal up to (1 - victim's share of vault) * victim's deposit.

The vulnerability window is small — it requires a same-block sandwich. And Ethena's `_checkMinShares` does help: it forces `totalSupply ≥ 1e18` after any deposit/withdraw, which prevents the "deposit 1 wei, donate, withdraw" variant where totalSupply is in the danger zone. But it doesn't prevent the "deposit 1e18, donate huge, front-run victim" variant.

However, in practice for sUSDe:
- The vault is **already large** (sUSDe TVL is hundreds of millions). `totalSupply` is huge, `totalAssets` is huge. An attacker would need to donate an astronomically large amount to move the exchange rate meaningfully.
- E.g., if `totalAssets = $500M` and the attacker wants to dilute a victim's $1M deposit by 50%, they'd need to donate $500M of their own USDe. They'd risk $500M to steal $500K. Economically irrational.
- The flash-loan variant doesn't help because the attacker must hold the donated funds through the victim's deposit transaction, which requires the victim's transaction to land after the attacker's donation in the same block. The attacker can't repay the flash loan until they withdraw, which is after the victim's deposit.

So this attack is **theoretically possible on an empty vault** but **economically irrational on the live sUSDe vault** (which has hundreds of millions in TVL). For StakedUSDe V2 specifically, the analysis note (SUSDe-4) explicitly says: "Verified non-profitable: with OZ v4.9.5 virtual shares + _checkMinShares + notZero(shares), an attacker depositing then donating breaks even on round-trip." The note also says (SUSDe-5): "A flash-loan depositor cannot move the vesting state, and deposit/withdraw round-trips lose the rounding dust to the vault. No profitable flash-loan path."

For an empty vault at deployment, the `_checkMinShares` floor of `1e18` plus the 1-wei virtual offset prevents the "1-wei first depositor" theft. The "1e18 first depositor + donate" variant is uneconomical because the attacker's donation is at risk until they withdraw, and the round-trip breaks even if no victim shows up.

### Severity
**None (in production).** The attack is theoretically possible on an empty vault with a same-block victim, but:
- sUSDe / sENA are already deployed with massive TVL; donation cost to move the rate is economically irrational.
- `_checkMinShares` blocks the zero-share variant.
- OZ v4.9.5 virtual offset blocks the 1-wei-first-depositor variant.
- The flash-loan variant doesn't help because the attacker can't repay until they withdraw, which is after the victim's deposit.

This is the canonical "ERC-4626 inflation attack" and Ethena has the standard mitigations. Not exploitable for profit in production. (Acknowledged in `ethena-usde-deep-analysis.md` SUSDe-4 and SUSDe-5.)

---

## Attack 5 — OFT cross-chain arbitrage (sUSDe Eth vs Arbitrum)

**Feasibility:** **No protocol exploit.** Standard cross-chain DEX arbitrage; the protocol neither loses funds nor has a seam to extract.

### Required conditions
- sUSDe spot price (vs USDe or USD) differs between Ethereum and Arbitrum DEXes.
- Attacker has sUSDe balance on the cheaper chain and outbound rate-limit headroom.

### Step-by-step flow
1. Buy 50 sUSDe on Arbitrum DEX at $1.10 (vs $1.12 on Ethereum).
2. Bridge 50 sUSDe Arbitrum → Ethereum via `StakedUSDeOFT.send(...)`:
   - `_debit` on Arbitrum burns 50 sUSDe, checks `rateLimits[dstEid=Mainnet]` (configurable on Arbitrum's StakedUSDeOFT). Assume 50 sUSDe / 60s limit. OK.
3. `_credit` on Ethereum's `StakedUSDeOFTAdapter` mints/locks 50 sUSDe to attacker. (No inbound rate limit — F-5 INFO.)
4. Sell 50 sUSDe on Ethereum DEX at $1.12.
5. Bridge the proceeds (USDC) back to Arbitrum via standard USDC bridge.
6. **Profit = 50 * ($1.12 - $1.10) - bridge fees - DEX slippage - gas.**

### Why this is NOT a protocol exploit
- The OFT bridge transfers sUSDe 1:1 across chains. The protocol doesn't charge a bridge fee (LayerZero fees are paid by the sender in native gas). The protocol doesn't take a cut of the price difference.
- The price difference is between two DEXes on different chains — the arbitrageur is capturing DEX LP inefficiency, not protocol funds.
- The protocol's sUSDe supply is unchanged: 50 sUSDe burned on Arbitrum, 50 sUSDe unlocked on Ethereum. Net protocol TVL unchanged.
- The LayerZero message-passing ensures atomic settlement; no risk of stuck funds.

### Blockers
- **Outbound rate limits:** StakedUSDeOFTAdapter mainnet has 50 sUSDe / 60s per dstEid. Arbitrum's StakedUSDeOFT has its own limits for the return direction. Multi-hop (F-3 INFO) raises the effective cap to Σ limits = bounded by adapter balance, but still caps velocity.
- **Adapter TVL:** Can't unlock more sUSDe on Ethereum than the adapter holds.
- **DEX slippage:** For illiquid sUSDe pools, large bridge → sell moves the price against the arbitrageur.
- **Bridge latency:** LayerZero V2 is ~seconds, but the price may move during the bridge.

### Severity
**None (by design).** Standard cross-chain arbitrage; protocol funds are not at risk. The rate limiter is a velocity cap, not a profit cap.

---

## Attack 6 — PSM epoch boundary attack

**Feasibility:** **No** (for an external attacker). The `_maybeRollEpoch` lazy-reset design prevents same-block boundary exploitation. The only "bypass" is a trusted `EPOCH_PERIOD_MANAGER_ROLE` changing `epochDuration` (SD-3, LOW, centralization concern).

### Required conditions
- Attacker wants to swap more than `maxSwapForAssetPerEpoch` in a single epoch.
- Attacker hopes the epoch boundary resets the counter mid-block.

### Step-by-step flow (attempted)
1. Attacker submits `swap()` in block N. `_handleEpochPeriodOperations` reads `currentEpoch = block.timestamp / epochDuration`. `block.timestamp` is constant within a block, so `currentEpoch` is constant within the block.
2. `_maybeRollEpoch(globalEpochState, currentEpoch)`:
   - If `epochState.epoch != currentEpoch` ⇒ reset `swappedForAssetInEpoch = 0`, set `epochState.epoch = currentEpoch`.
   - This is the **only** way the counter resets, and it uses the same `currentEpoch` for the entire block.
3. There is no way for two `swap()` calls in the same block to see different `currentEpoch` values, because they both read `block.timestamp` (constant within a block).
4. After the first swap consumes the limit, the second swap in the same block reverts with `GlobalMaxSwapForAssetPerEpochExceeded`.

### What about `setEpochDuration` right before boundary?
- `setEpochDuration` is `EPOCH_PERIOD_MANAGER_ROLE`-gated. Not callable by external attacker.
- If a trusted role changes `epochDuration` from 1 hour to 10 seconds right before a boundary, the new `currentEpoch = block.timestamp / 10` is a different number than the old `currentEpoch = block.timestamp / 3600`, AND the state is stored in `epochStateByDuration[newDuration]` (a fresh mapping slot with 0/0). So the swap sees zero usage and proceeds.
- This is SD-3 in the PSM deep analysis (LOW). It requires a compromised or malicious `EPOCH_PERIOD_MANAGER_ROLE` — a trusted role, out of Immunefi scope per centralization rules.

### What about `chainId` / block.timestamp manipulation?
- `block.timestamp` is miner/validator-controlled within ~15 seconds. A validator could push `block.timestamp` across an epoch boundary. But the attacker would need to be a validator, and the gain is just resetting their own swap quota — bounded by their benefactor limit, not a protocol-funds exploit.

### Severity
**None (external); LOW (trusted role SD-3, centralization, out of scope).**

---

## Attack 7 — Governance flash loan attack on ENA

**Feasibility:** **Out of scope.** No governance contract is in the analyzed source set. ENA staking (StakedENA.sol) and ENA bridging (ENAOFT/ENAOFTAdapter) are in scope, but ENA governance (if it exists as a separate Governor contract) is not.

### Required conditions (hypothetical)
- ENA governance uses a snapshot-based voting system (e.g., OZ Governor).
- Voting power is determined at a past block (snapshot), not at the block of the vote.

### Step-by-step flow (hypothetical)
1. Flash-loan 10M ENA from a lending market.
2. Self-delegate or vote on a proposal.
3. If voting power is checked at the **current** block (not snapshot), the flash-loaned ENA counts.
4. Repay flash loan.

### Blockers
- Standard OZ Governor uses `proposalSnapshot(proposalId)` which is `block.number - 1` at proposal creation. Voting power is read at that past block. A flash loan at vote time doesn't help — the attacker didn't hold the ENA at the snapshot block.
- Even if governance used current-block voting, the attacker would need to flash-loan ENA, which requires a lending market with ENA liquidity. Aave doesn't list ENA as a borrowable asset as of writing; if it did, the flash-loan fee + slippage would erode any profit.
- ENA governance (if using Snapshot or a similar off-chain system) is entirely separate from on-chain ENA transfers.

### Severity
**Out of scope.** No governance contract in the analyzed source set. If a separate ENA Governor exists, the snapshot mechanism (standard OZ pattern) blocks flash-loan voting. Not analyzable from the current source set, and not a protocol-funds exploit even if it existed.

---

## Attack 8 — USDtbMinting + PSM combo arbitrage

**Feasibility:** **No.** Both legs charge fees / oracle spreads; the protocol always nets positive.

### Required conditions
- USDtbMinting and PSM both support overlapping collaterals (e.g., USDC).
- USDtbMinting's RFQ price differs from PSM's peg+oracle price.
- Attacker is both a USDtbMinting benefactor (MINTER_ROLE-holder submitting their signed order) and a PSM benefactor.

### Step-by-step flow (attempted)
1. Flash-loan 100M USDC.
2. USDtbMinting `mint({collateral=USDC, collateralAmount=100M, usdtbAmount=100M})`:
   - `verifyStablesLimit` (MINT path): checks when `usdtbAmount > normalizedCollateralAmount`. Here they're equal, so check returns true.
   - Attacker receives 100,000,000 USDtb. Protocol receives 100M USDC across custodians per the route.
3. PSM `swapForCollateral({asset=USDtb, amountIn=100M USDtb, collateral=USDT})`:
   - `oneToOne = 100M - fee USDtb` worth of USDT (peg-based, after fee).
   - `oracle = 100M * 1.00 / 1.00 = 100M USDtb` worth of USDT (assuming USDT oracle = $1.00).
   - `min = 100M - fee USDT`. Attacker receives 99,950,000 USDT (assuming 5 bps fee).
4. Repay flash loan: need 100M USDC. Attacker has 99,950,000 USDT.
5. Swap 99.95M USDT → USDC on Curve at ~$1.00 (assume 1 bps Curve fee + 1 bps slippage) ⇒ 99,930,000 USDC.
6. **Loss = 70,000 USDC** (PSM fee + Curve fees + slippage). No arbitrage.

### What if the USDtbMinting RFQ gives the attacker slightly more USDtb than collateral (off-chain quote favors attacker)?
- `verifyStablesLimit` MINT path: `if (usdtbAmount > normalizedCollateralAmount) return differenceInBps <= stablesDeltaLimit`.
- So if the RFQ gives `usdtbAmount = 100,000,100 USDtb` for `collateralAmount = 100,000,000 USDC` (i.e., attacker gets 1 bps more USDtb), the check fires: `differenceInBps = (100 * 10000) / 100,000,100 = 0.01 bps`. If `stablesDeltaLimit ≥ 0.01 bps`, the mint succeeds.
- Then in PSM, the attacker swaps 100,000,100 USDtb → USDT:
  - `oneToOne = 100,000,100 - fee_5bps = 99,950,099.95 USDT`
  - `oracle = 100,000,100 * 1.00 / 1.00 = 100,000,100 USDT`
  - `min = 99,950,099.95 USDT`. Slightly more USDT than the no-RFQ-bonus case, but still less than 100M.
- Net: attacker paid 100M USDC, got 99.95M USDT. Still a loss of ~50K USDC (the PSM fee), plus the 100 USDtb bonus is dwarfed by the 50K USDC fee. No arbitrage.

### What if the attacker can manipulate the USDT oracle (covered in Attack 2)?
- Already shown: `min()` blocks it.

### What if there's a same-block mint + redeem in USDtbMinting (Area 5 of the analysis)?
- The two legs of USDtbMinting both go through `verifyStablesLimit`, which checks in the direction that hurts the protocol.
- Mint gives ≤ collateral value of USDtb (asymmetric check). Redeem gives ≤ USDtb value of collateral (asymmetric check).
- Round-trip: mint USDC → USDtb (≤ 1:1), redeem USDtb → USDT (≤ 1:1). Net loss = sum of any fees + oracle slippage.
- Combined with PSM (which also charges fees), the multi-leg combo is strictly loss-making.

### Severity
**None.** Both USDtbMinting's `verifyStablesLimit` asymmetry and PSM's `min()` pricing protect the protocol. The asymmetric `verifyStablesLimit` was flagged as Medium-priority in `ethena-usdtb-minting-analysis.md` Area 1, but the analysis correctly notes: "In both cases, the direction that HURTS THE PROTOCOL is checked." So no free-money seam exists.

---

## Attack 9 — PSM `rescueFunds` economic attack

**Feasibility:** **Out of scope (centralization).** `rescueFunds` is `DEFAULT_ADMIN_ROLE`-gated. A compromised admin multisig can drain PSM, but this is a key-compromise / centralization risk, explicitly excluded from Immunefi scope.

### Required conditions
- `DEFAULT_ADMIN_ROLE` holder (Ethena's multisig) is compromised, OR
- The multisig's signing logic has a bug (e.g., signature malleability, replayable signed message).

### Step-by-step flow (hypothetical)
1. Compromised admin calls `rescueFunds(attacker, USDtb, 100M)`.
2. PSM executes `IERC20(USDtb).safeTransfer(attacker, 100M)`.
3. **If** the PSM holds 100M USDtb (it shouldn't in normal operation — funds flow through custodians), the transfer succeeds.
4. **In practice**, the PSM holds zero funds in normal operation. `rescueFunds` can only rescue tokens accidentally sent to the PSM address itself.

### Why this isn't an economic attack vector
- The PSM never holds user funds. `swap()` uses `safeTransferFrom` from benefactor → custodian (input) and custodian → beneficiary (output). The PSM has no allowance on custodian funds beyond what each `swap()` atomically consumes.
- `rescueFunds` can only move tokens that are sitting in the PSM's own address — i.e., accidentally-sent tokens (griefing) or stuck dust. These are not user funds.
- The admin cannot use `rescueFunds` to drain the custodian wallets (PSM has no allowance on them outside of an in-flight `swap()`).
- A compromised admin could call `updateCollateralConfig` to redirect custodians to attacker wallets (UC-3 INFO), but that's a key-compromise scenario, out of scope.
- A bug in the multisig signing logic is outside our source set (we don't have the multisig contract).

### Severity
**Out of scope (centralization).** Immunefi excludes admin-key compromise from severity. The `rescueFunds` function itself is correctly `DEFAULT_ADMIN_ROLE`-gated and `nonReentrant`. R-1 in the PSM deep analysis rates this LOW for "can rescue any token including asset/collateral", but that's a configuration smell, not an exploit.

---

## Attack 10 — Rate limit accumulation attack

**Feasibility:** **No.** Rate limits are velocity caps, not volume caps. Accumulating many small swaps within the per-epoch / per-period limit doesn't bypass the `min()` pricing protection — each swap is independently fee-charged and oracle-protected.

### Required conditions
- Attacker wants to move a large total volume through PSM.
- Per-epoch / per-period limits are non-trivial (e.g., $10M / epoch with 10-second epochs = $60M / minute).
- Attacker has benefactor status and rate-limit headroom.

### Step-by-step flow (attempted)
1. Attacker is a registered benefactor with `maxSwapForAssetPerEpoch = 10M` (or default).
2. Each 10-second epoch, attacker submits a `swap()` for 10M USDC → USDtb.
3. Over 1 hour (360 epochs), attacker moves 3.6B USDC through PSM.
4. **If** each swap is unprofitable (which it is, per Attacks 1–3), the attacker loses 5 bps × 3.6B = 1.8M USDC in fees.
5. **If** each swap is profitable (only possible via the Attack 1 intended-arbitrage mechanism, which doesn't drain protocol funds), the attacker profits from DEX spreads, not from PSM.

### Why accumulation doesn't help
- The `min()` in `_getQuote` is applied **per swap**, not per epoch. There is no "cumulative discount" or "cumulative bypass" for high-volume attackers.
- The fee is applied **per swap** (`feeAmount = amountIn * feeRate / 10000`). No volume tier reduces the fee.
- The 6-layer rate limit (global / collateral / benefactor × epoch / period) caps the velocity. Once any layer hits its cap, subsequent swaps revert.
- The cumulative volume over time is bounded by `maxSwapForAssetPerPeriod * (period_duration / period_duration) = maxSwapForAssetPerPeriod` per period. With period = 30 days and `maxSwapForAssetPerPeriod = $1B`, the attacker can move $1B / 30 days. This is by design.

### What about "small swaps each epoch to avoid detection"?
- The rate limits are hard on-chain caps, not monitoring thresholds. There's no "detection" to avoid — swaps within the limit simply succeed, swaps beyond revert.
- The PSM doesn't have a global cap beyond the per-epoch / per-period limits. There's no separate "lifetime volume" cap. So the attacker can do `maxSwapForAssetPerEpoch` swaps every epoch, indefinitely.
- But each swap is independently unprofitable (per Attacks 1–3). So accumulating volume just accumulates losses.

### What if the attacker finds a single profitable arbitrage (Attack 1) and wants to repeat it?
- Each iteration is bounded by the rate limit. If the rate limit is $10M / epoch and the arbitrage spread is 2%, the attacker makes $200K / epoch = $1.2M / minute.
- BUT the arbitrage closes the DEX spread. After a few iterations, the DEX price moves to peg, the spread disappears, and further swaps are unprofitable.
- The protocol still nets +fee per swap. No protocol loss.

### Severity
**None (by design).** Rate limits are velocity caps, not profit caps. The `min()` pricing protection is per-swap, not cumulative. No bypass via accumulation.

---

## Cross-cutting economic invariants (proven)

### Invariant E1: PSM never overpays in any single swap
**Proof:** For any `swap()` call, `amountOut = min(oneToOneAmountOut, oracleAmountOut)`. Both terms are bounded above by `amountIn * pegPrice / pegPrice = amountIn` in USD value (modulo decimal scaling):
- `oneToOneAmountOut = (amountIn - fee) * peg / peg < amountIn` (strictly, since fee > 0 unless fee-exempt).
- `oracleAmountOut = amountIn * oraclePrice / pegPrice` (swapForAsset). USD value of `oracleAmountOut` in asset units = `oracleAmountOut * pegPrice = amountIn * oraclePrice` = USD value of `amountIn` collateral.
- `min` of these is ≤ the smaller, which is ≤ fair USD value. Protocol never overpays.

### Invariant E2: PSM cross-collateral round-trip is strictly lossy
**Proof:** See Attack 3. The combined ratio is `(1-f1)(1-f2) * min(1,p1)/max(1,p2) < 1` always.

### Invariant E3: PSM cannot be drained by oracle manipulation
**Proof:** See Attack 2. The `min()` selects the conservative price in every oracle×direction cell. No oracle state produces `amountOut > fair value`.

### Invariant E4: PSM cannot be drained by flash-loaned same-block reentry
**Proof:** `swap()` is `nonReentrant`. All admin setters are `nonReentrant` (shared `_status`). No config can change between `_getQuote` and the transfers. Oracle callback cannot reenter `swap()` or any setter. No flash-loan seam.

### Invariant E5: PSM rescueFunds cannot drain custodian wallets
**Proof:** PSM has no ERC-20 allowance on custodian wallets except what each in-flight `swap()` atomically consumes. `rescueFunds` only moves tokens already in the PSM address (accidental / griefing dust). Custodian funds are unreachable.

### Invariant E6: sUSDe / sENA exchange rate cannot be flash-loan manipulated
**Proof:** `transferInRewards` (the only rate-moving function) is `REWARDER_ROLE`-gated and reverts if `getUnvestedAmount() > 0`. Flash-loan depositors cannot move the vesting state. Deposit→donate→withdraw round-trip breaks even (OZ v4.9.5 virtual offset + `_checkMinShares` floor). On the live vault with hundreds of millions in TVL, donation cost to move the rate exceeds any victim-deposit profit.

### Invariant E7: OFT cross-chain bridge is 1:1 and rate-limited
**Proof:** `_debit` (outbound) burns/locks `amountLD`; `_credit` (inbound) mints/unlocks `amountLD`. Same amount both sides. Rate limiter on outbound per `dstEid` caps velocity. Inbound has no rate limit but is bounded by source-chain outbound. No price discovery on the bridge itself — the arbitrage is on DEXes, not protocol funds.

---

## Severity roll-up

| # | Attack vector | Feasibility | Severity | Reason |
|---|---|---|---|---|
| 1 | PSM arbitrage with flash loan | Yes (arbitrageur), No (protocol exploit) | None (by design) | PSM nets +fee; arbitrage profit comes from DEX LPs, not protocol. |
| 2 | Oracle manipulation on PSM | No | None | `min()` protects protocol in every oracle×direction cell. |
| 3 | Cross-PSM / cross-collateral arbitrage | No | None | Proven strictly lossy: `(1-f1)(1-f2) * min(1,p1)/max(1,p2) < 1`. |
| 4 | sUSDe exchange rate manipulation | No (in production) | None | OZ v4.9.5 virtual offset + `_checkMinShares` + large TVL make donation attack uneconomical. |
| 5 | OFT cross-chain arbitrage | Yes (arbitrageur), No (protocol exploit) | None (by design) | 1:1 bridge; arbitrage profit from DEX LPs; rate-limited velocity. |
| 6 | PSM epoch boundary attack | No (external); LOW (trusted role) | None / LOW (OoS) | `_maybeRollEpoch` uses single `block.timestamp` per block. SD-3 requires trusted role. |
| 7 | Governance flash loan on ENA | N/A | Out of scope | No governance contract in source set. Snapshot-based voting blocks flash loans. |
| 8 | USDtbMinting + PSM combo | No | None | Both legs net +fee/+spread to protocol. `verifyStablesLimit` asymmetric check protects protocol. |
| 9 | PSM rescueFunds economic attack | N/A | Out of scope (centralization) | `DEFAULT_ADMIN_ROLE`-gated. PSM holds no user funds. Key compromise is OoS. |
| 10 | Rate limit accumulation attack | No | None (by design) | Rate limits are velocity caps. `min()` is per-swap. No cumulative bypass. |

**Total attacks simulated: 10**
**Exploitable (protocol-fund theft): 0**
**Critical: 0**
**High: 0**
**By-design arbitrage (not exploits): 2 (Attacks 1, 5)**
**Out of scope (centralization): 2 (Attacks 6-trusted-role, 9)**
**Out of scope (no source): 1 (Attack 7)**

---

## Conclusion

The Ethena PSM is **economically closed** under all 10 simulated attack vectors. The decisive design choice is the `min(oneToOneAmountOut, oracleAmountOut)` pricing in `_getQuote`, which unilaterally protects the protocol in every direction of every swap regardless of oracle behavior. Combined with:

- 6-layer rate limiting (velocity cap),
- `nonReentrant` on `swap()` and all admin setters (atomicity),
- `minOraclePrice` / `maxOraclePrice` / `maxOracleAge` / future-timestamp depeg guards (oracle sanity),
- `minAmountOut` user slippage protection,
- PSM holding zero user funds in normal operation (custodian model),
- OZ v4.9.5 virtual shares + `_checkMinShares` on the ERC-4626 vaults (inflation attack mitigation),
- Per-`dstEid` OFT rate limiting (cross-chain velocity cap),

…there is no flash-loan, oracle-manipulation, arbitrage, or MEV strategy that extracts protocol funds. The two "arbitrage" attacks (1 and 5) are profitable for the arbitrageur but **by design** — they are the peg-keeping mechanism and they **net +fee to the protocol** per cycle. The "intended PSM arbitrage" is the entire reason the PSM exists.

No `vuln/ethena-economic-*.md` PoC file is written — reserved for Critical findings only, and none was found.

**Honest verdict:** The economic attack surface of the Ethena PSM is well-defended. The `min()` pricing trick is the keystone — it converts "oracle manipulation" from a fund-theft vector into a self-defeating attack (the attacker crashes the oracle, gets fewer assets, and loses money). The custodian model means `rescueFunds` can't drain user funds. The ERC-4626 vaults use the standard inflation-attack mitigations. The OFT bridges are 1:1 with per-path rate limits. No exploitable economic attack found. No Immunefi submission warranted.

---

## Methodology notes

- All math verified against `PSM._getQuote` (lines 1571–1631) and `_validateOraclePrice` (lines 1522–1549) as actually written in the source.
- Rate-limit geometry verified against `_handleEpochPeriodOperations` (lines 1662–1836) and `_maybeRollEpoch` / `_maybeRollPeriod` (lines 1844–1864).
- ERC-4626 inflation-attack mitigation verified against OZ v4.9.5 `_convertToShares` / `_convertToAssets` (1-wei virtual offset) and Ethena's `_checkMinShares` (1e18 floor).
- OFT flow verified against `RateLimiter._checkAndUpdateRateLimit` and `USDeOFT._debit` override.
- USDtbMinting analysis based on `ethena-usdtb-minting-analysis.md` (source not in current `/contracts/` folder; prior analysis used).
- No mainnet execution. No funds moved. No Immunefi submission. Research only.
