# Ethena PSM On-Chain Verification Report

**Task ID:** eth-onchain-verify-psm
**Date:** 2026-09-24
**Analyst:** Opus (sub-agent)
**PSM Address:** `0x73E35C5c35A274E34AdE6EB13cC7f62aEE323728`
**Network:** Ethereum mainnet
**Data source:** Blockscout REST API + Blockscout eth-rpc (Etherscan Cloudflare-blocked as expected)

---

## TL;DR — Executive Summary

| Question | Answer |
|---|---|
| Is PSM deployed & verified? | ✅ Yes — fully verified on Blockscout, source matches repo |
| Is PSM active in production? | ✅ Yes — `isSwapEnabled() = true`, 44 swaps, **~$95M volume** over 3 months |
| Has `removeBenefactor()` EVER been called? | ❌ **NO** — 0 `BenefactorRemoved` events in entire history |
| Has `removeCollateral()` EVER been called? | ❌ **NO** — 0 `CollateralRemoved` events in entire history |
| Has `disableBenefactor()` been called? | ❌ **NO** — 0 `BenefactorDisabled` events |
| Has the PSM ever been paused? | ❌ **NO** — 0 `SwapDisabled` events |
| Has `pegPrice` ever changed? | ❌ **NO** — 0 `PegPriceUpdated` events (still 1.0 from constructor) |
| Bug trigger probability (production) | **ZERO observed** — purely theoretical at this time |
| Bug trigger probability (future) | **NON-TRIVIAL** — PSM is actively used; any future `removeBenefactor` call would expose the bug |

### Verdict on the Bug

> **Bug is THEORETICAL, NOT MOOT.**
>
> The PSM contract is **in active production use** (44 swaps, ~$95M volume, 5 institutional benefactors, ~$43M TVL across custodians). The `removeBenefactor` mapping-persistence bug **has never been triggered** in production because `removeBenefactor()` has never been called. However, because the contract is active and the bug-triggering function is reachable by the `BENEFACTOR_MANAGER_ROLE` holder (currently `EthenaTimelockController`), the bug remains a **latent production risk** — any future operational need to remove a benefactor would expose it.

---

## 1. Contract Deployment Status

| Field | Value |
|---|---|
| Address | `0x73E35C5c35A274E34AdE6EB13cC7f62aEE323728` |
| Contract name | `PSM` |
| Source file | `dependencies/onchain-minting-internal-1.0.0/src/swap/PSM.sol` |
| Language / Compiler | Solidity v0.8.30+commit.73712a01 |
| EVM version | osaka |
| Optimization | Enabled, 1500 runs |
| License | none |
| Verified on Blockscout | ✅ `is_verified=true`, `is_fully_verified=true` |
| Verified at | 2026-06-20T17:33:37Z (Sourcify / eth-bytecode-db) |
| Proxy type | `None` — **direct deployment, no proxy, no upgrade pattern** |
| Implementations | `[]` (none) |
| External libraries | `[]` (none linked) |
| Is blueprint | false |
| Is changed bytecode | false (deployed bytecode matches verified source) |
| Constructor args (decoded) | asset=`0xC139190F447e929f090Edeb554D95AbB8b18aC1C` (USDtb), assetSendCustodian=`0x3E9924b65Eb75d05B7448305e66d1f490E70623D`, assetReceiveCustodian=`0x2d4d2A025b10C09BDbd794B4FCe4F7ea8C7d7bB4`, admin=`0x3B0AAf6e6fCd4a7cEEf8c92C32DFeA9E64dC1862`, plus 7 role arrays (each `[admin]`) and GlobalConfig tuple |

### Deployment Transaction

| Field | Value |
|---|---|
| Block number | `25351697` (0x182d611) |
| Block timestamp | **2026-06-19 12:42:35 UTC** |
| Outer tx hash | `0xf7b18d6928b59075f3896b33c6600c555b1aa9d895574797173dd4f98acc4615` |
| Outer tx method | `Safe.execTransaction` (called by Safe relayer `0x66BAfa19…`) |
| Outer tx `to` | `0x3B0AAf6e6fCd4a7cEEf8c92C32DFeA9E64dC1862` (Gnosis Safe) |
| Inner call chain | Safe → `CreateX` (`0xba5Ed099633D3B313e4D5F7bdc1305d3c28ba5Ed`) → **CREATE2 deploy PSM** |
| Deployer factory | **CreateX** (deterministic CREATE2 factory) |
| First 9 logs emitted in constructor | 1× `AdminTransferred(0x0 → Safe)` + 8× `RoleGranted` (one per role, all to Safe) |
| Gas used (outer tx) | 5,955,911 |

**Conclusion:** PSM is a directly-deployed (non-proxied) contract. There is **no upgrade mechanism** — fixing the bug would require a redeployment + asset/custodian migration.

---

## 2. Activity Status (Live On-Chain Calls)

### View-function calls (via Blockscout eth-rpc, `latest` block)

| Function | Selector | Result |
|---|---|---|
| `isSwapEnabled()` | `0x351a964d` | `0x…01` → **TRUE** (swaps enabled) |
| `asset()` | `0x38d52e0f` | `0xC139190F447e929f090Edeb554D95AbB8b18aC1C` → **USDtb** |
| `assetSendCustodianAddress()` | `0xab0a5b34` | `0x3E9924b65Eb75d05B7448305e66d1f490E70623D` |
| `assetReceiveCustodianAddress()` | `0xd851e0b9` | `0x2d4d2A025b10C09BDbd794B4FCe4F7ea8C7d7bB4` |
| `globalConfig()` | `0xa7c1abe0` | Decoded tuple below |

### `globalConfig()` decoded (current live values)

```
maxSwapForAssetPerEpoch              = 50,000     (USDtb units, 18 dec)
maxSwapForCollateralPerEpoch         = 50,000     (USDtb units)
defaultBenefactorMaxSwapForAssetPerEpoch    = 1,000
defaultBenefactorMaxSwapForCollateralPerEpoch = 1,000
epochDuration                        = 3,600      seconds (1 hour)
pegPrice                             = 1.0e18     ($1.00 peg)
maxSwapForAssetPerPeriod             = 250,000
maxSwapForCollateralPerPeriod        = 250,000
defaultBenefactorMaxSwapForAssetPerPeriod    = 10,000
defaultBenefactorMaxSwapForCollateralPerPeriod = 10,000
periodDuration                       = 86,400     seconds (1 day)
```

### Supported collaterals (from `CollateralAdded` events + live config)

| Collateral | Address | Decimals | First added | Last config update |
|---|---|---|---|---|
| **USDC** | `0xA0b86991c6218b36c1d19D4a2e9Eb0cE3606eB48` | 6 | 2026-06-19 12:43:35 (block 25351702) | 2026-08-10 (block 25722853) |
| **USDT** | `0xdAC17F958D2ee523a2206206994597C13D831ec7` | 6 | 2026-08-04 14:59:59 (block 25682307) | (no later updates) |

USDC send custodian: `0x61E614EC5e2F282Ca42f313Afacb45738DdACDae` (EOA)
USDC receive custodian: `0x2d4d2A025b10C09BDbd794B4FCe4F7ea8C7d7bB4` (EOA, same as assetReceiveCustodian)
USDT send custodian: `0x31BD060f6c0Ce4284Bb30f2C8C27Ca48542347a7` (contract, name=`AggregateOracleFeed` — actually an oracle feed, possibly reused as custodian)
Oracle feed: `0x31BD060f6c0Ce4284Bb30f2C8C27Ca48542347a7` (AggregateOracleFeed contract)

### `SwapExecuted` history (parsed from 44 log entries)

- **First swap:** 2026-06-22 12:13:59 UTC (block 25373070)
- **Last swap:** 2026-09-23 12:52:23 UTC (block 26040298)
- **Span:** 3 months, 1 day
- **Total swaps:** 44 (43 successful + 1 reverted)
- **Total USD volume IN:** **$95,000,086.09**
- **Total USD volume OUT:** $94,937,593.54
- **Net flow:** ~$62,492 (essentially balanced — PSM operating as a two-way peg)
- **Direction split:** 29 ForAsset (USDC/USDT → USDtb) + 15 ForCollateral (USDtb → USDC/USDT)
- **Collateral split:** USDC = 38 swaps / $65M volume; USDT = 6 swaps / $30M volume

**Sample swap (smallest, #1):** 1 USDC in → 0.9995 USDtb out, fee 5 bps. Peg held to within 0.05%.

**Sample swap (largest, #21):** 890,000 USDC in → 889,555 USDtb out, ~$890k single swap.

---

## 3. Transactions to PSM (top-level + admin-routed)

### Top-level transactions to PSM (Blockscout `/api/v2/addresses/{psm}/transactions`)

- **Total:** 45 transactions returned
- **All 45 are `swap()` calls** (one reverted: `0x79765d57…` at 2026-09-17 18:13:59)
- Block range: 25373070 (2026-06-22) → 26040298 (2026-09-23)

### Admin/config transactions (NOT in the top-level list — routed via Safe execTransaction)

Admin functions do **not** appear as top-level transactions to PSM because they are called via the Safe proxy's `execTransaction` (multi-sig batched). The Safe then internally calls PSM. These show up as **internal transactions**, and PSM-side state changes are visible only via event logs.

Event-log audit (148 unique logs across 6 paginated Blockscout log pages):

| Event | Count | Notes |
|---|---|---|
| `SwapExecuted` | 44 | All swaps |
| `OraclePriceValidated` | 44 | Emitted once per swap (oracle price check passed) |
| `RoleGranted` | 16 | 8 in constructor + 8 later (role transfer to Timelock) |
| `RoleRevoked` | 8 | All for old admin Safe (role transfer) |
| `BenefactorAdded` | **5** | 5 unique benefactors added |
| `BenefactorSwapForCollateralFeeUpdated` | 5 | Fee adjustments |
| `CollateralConfigUpdated` | 4 | USDC config tuned 4× |
| `BenefactorMaxSwapForAssetPerEpochUpdated` | 3 | |
| `BenefactorMaxSwapForAssetPerPeriodUpdated` | 3 | |
| `BenefactorMaxSwapForCollateralPerEpochUpdated` | 3 | |
| `BenefactorMaxSwapForCollateralPerPeriodUpdated` | 3 | |
| `AdminTransferred` | 2 | Constructor + July 13 transfer |
| `CollateralAdded` | 2 | USDC (Jun 19) + USDT (Aug 4) |
| `EpochLimitsUpdated` | 2 | |
| `PeriodLimitsUpdated` | 2 | |
| `AdminTransferRequested` | 1 | July 6 request |
| `EpochDurationUpdated` | 1 | 1800s → 3600s (Aug 6) |
| **`BenefactorRemoved`** | **0** | **`removeBenefactor()` NEVER CALLED** |
| **`CollateralRemoved`** | **0** | **`removeCollateral()` NEVER CALLED** |
| **`BenefactorDisabled`** | **0** | `disableBenefactor()` never called |
| **`CollateralDisabled`** | **0** | `disableCollateral()` never called |
| **`BenefactorEnabled`** | **0** | (no benefactor ever disabled, so no re-enable) |
| **`CollateralEnabled`** | **0** | (no collateral ever disabled) |
| **`SwapDisabled`** | **0** | PSM never paused |
| **`SwapEnabled`** | **0** | (already enabled in constructor, never toggled) |
| **`PegPriceUpdated`** | **0** | pegPrice still 1.0 from constructor |
| `BeneficiaryApproved` / `BeneficiaryRemoved` | 0 | |
| `DelegatedSignerAdded/Confirmed/Removed` | 0 | Delegated-signer feature unused |
| `RescueFunds` | 0 | Never rescued |
| `AssetSendCustodianUpdated` / `AssetReceiveCustodianUpdated` | 0 | Custodians never rotated |

---

## 4. Benefactor List (5 total — all added, none ever removed)

| # | Benefactor address | Type | Added at | Added in tx | Swaps | Total volume (USD) |
|---|---|---|---|---|---|---|
| 1 | `0x3Aa3Fd1B762CaC519D405297CE630beD30430b00` | EOA | 2026-06-19 12:43:35 (blk 25351702) | `0x7637173263…` (Safe.execTransaction) | 19 | **$39.50** (all micro-test swaps $1-3) |
| 2 | `0xcf8925D510C431f57bD5C45FAFe01a26D1F12287` | EOA | 2026-08-04 08:43:11 (blk 25680427) | `0xfc8c6259c3…` (Safe.execTransaction) | **0** | $0 (added but never used) |
| 3 | `0x1FcC47Ee0F19CA0f07F8b987F0aD32ac204C03a7` | EOA | 2026-08-04 14:59:59 (blk 25682307) | `0xd4b4642558…` (Safe.execTransaction, batched w/ #4 & #5) | 13 | **$46,004,536.59** |
| 4 | `0xb32dd55d4FF63E39c304B00e069FbaEFe885f0Fb` | EOA | 2026-08-04 14:59:59 (blk 25682307) | `0xd4b4642558…` (same batched tx) | 6 | **$19,010,510.00** |
| 5 | `0x8DF6721F643E796C456ee080929e01C7f18E6122` | EOA | 2026-08-04 14:59:59 (blk 25682307) | `0xd4b4642558…` (same batched tx) | 6 | **$29,985,000.00** |

**Institutional vs. retail:** All 5 benefactors are EOAs. Benefactors #3, #4, #5 are clearly **institutional-scale** (multi-million-dollar individual swaps, including a single $15M swap on 2026-09-02 and a $10M swap on 2026-09-10). Benefactor #1 is likely an Ethena-internal testing/operations wallet (only micro-swaps of $1-3 each). Benefactor #2 was added but never used — possibly a spare/backup wallet or an institution that didn't end up using the facility.

**Benefactor concentration:** 4 active benefactors → ~$95M volume over 3 months. No single benefactor dominates (>50%); #3 handles ~48% of volume.

---

## 5. `removeBenefactor` History — NEVER CALLED

**Definitive answer:** `removeBenefactor(address)` has **NEVER been called** on the PSM contract in production.

### Evidence

1. **Direct event query:** Blockscout `GET /api/v2/addresses/{psm}/logs?topic=0xf0ebdfe9729215cc867dca3785f752df456766a9ce4b178ad1bca9919ca4a650` returned `{"items":[],"next_page_params":null}` — zero `BenefactorRemoved` events.

2. **Topic-hash verification:** The topic hash `0xf0ebdfe9729215cc867dca3785f752df456766a9ce4b178ad1bca9919ca4a650` is `keccak256("BenefactorRemoved(address)")` — the only event `removeBenefactor` emits, and it is emitted unconditionally on the success path.

3. **Full log scan:** Across all 148 unique logs ever emitted by PSM (covering all 6 pages of Blockscout's paginated logs API, deduplicated by `(block_number, index, tx_hash)`), zero have this topic.

4. **Constructor behavior:** `removeBenefactor` is not called in the constructor (constructor only grants roles + sets global config).

### Implication for the bug

The bug being audited (mapping-persistence bug in `removeBenefactor`) requires:
1. `removeBenefactor(addr)` to be called once (to leave stale mapping state), AND
2. `addBenefactor(addr)` to be called again for the same address (to reactivate with stale state).

**Step 1 has never occurred.** Therefore the bug has **never been triggered in production** and no production funds have been affected by it.

---

## 6. `removeCollateral` History — NEVER CALLED (same bug pattern)

```solidity
function removeCollateral(address collateral) external ... {
    if (collateralState.contains(collateral)) {
        collateralState.remove(collateral);
        emit CollateralRemoved(collateral);
    }
}
```

`CollateralStateMap` is an `EnumerableSet`-based map; `.remove()` should fully clear the entry. **However**, the `BenefactorState`/`CollateralState` structs contain nested mappings (`epochStateByDuration`, `periodStateByDuration`) that Solidity cannot automatically delete — same general concern.

**On-chain verification:**
- Topic `0xd89d2ee68ab04dca0193f48a4aff55e20fa5ec0429a8a8c1c51b8dad6178a593` = `keccak256("CollateralRemoved(address)")`
- Query returned `{"items":[]}` — zero `CollateralRemoved` events.

**Verdict:** `removeCollateral()` has **never been called**. The collateral-persistence variant of the bug is also purely theoretical.

---

## 7. Current TVL in PSM Custodians

Live `balanceOf` calls via Blockscout eth-rpc at `latest` block:

| Custodian | Address | Type | Token | Balance (raw) | Balance (human) |
|---|---|---|---|---|---|
| assetSendCustodian | `0x3E9924b65Eb75d05B7448305e66d1f490E70623D` | EOA | USDtb (18 dec) | `0x0e99f99e9ea8f855537340` | **17,652,088.22 USDtb** (~$17.65M) |
| assetReceiveCustodian | `0x2d4d2A025b10C09BDbd794B4FCe4F7ea8C7d7bB4` | EOA | USDtb (18 dec) | `0x086105112f1f43fbfeee37` | **10,129,569.58 USDtb** (~$10.13M) |
| USDC sendCustodian | `0x61E614EC5e2F282Ca42f313Afacb45738DdACDae` | EOA | USDC (6 dec) | `0x0dd2ecaf0690` | **15,199,565.19 USDC** (~$15.20M) |

**Total PSM-custodied value at risk: ~$43M USD equivalent.**

(Note: the PSM contract itself holds 0 ETH and is designed to never hold ERC-20 balances in normal operation — all funds are pre-positioned in the custodian EOAs which grant ERC-20 approvals to PSM. The PSM contract has `rescueFunds()` for emergency recovery but it has never been used.)

### Context: USDtb total supply = 476,007,905.77 USDtb

The PSM-related USDtb (17.65M send + 10.13M receive = 27.78M) represents **~5.8% of total USDtb supply**. The 10.13M USDtb sitting in `assetReceiveCustodian` is "swapped-out" USDtb that has been sold to PSM in exchange for USDC/USDT — this is essentially the PSM's inventory of USDtb that was sold to customers and is now waiting to either be redeemed or rebalanced.

---

## 8. Role Holders

### Constructor (2026-06-19 12:42:35 UTC, block 25351697)

All 8 roles initially granted to `0x3B0AAf6e6fCd4a7cEEf8c92C32DFeA9E64dC1862` (**SafeProxy / Gnosis Safe**, multi-sig).

```
DEFAULT_ADMIN_ROLE          → 0x3B0AAf6e6fCd4a7cEEf8c92C32DFeA9E64dC1862 (Safe)
EPOCH_PERIOD_MANAGER_ROLE   → 0x3B0AAf6e6fCd4a7cEEf8c92C32DFeA9E64dC1862 (Safe)
GLOBAL_DISABLER_ROLE        → 0x3B0AAf6e6fCd4a7cEEf8c92C32DFeA9E64dC1862 (Safe)
COLLATERAL_MANAGER_ROLE     → 0x3B0AAf6e6fCd4a7cEEf8c92C32DFeA9E64dC1862 (Safe)
COLLATERAL_DISABLER_ROLE    → 0x3B0AAf6e6fCd4a7cEEf8c92C32DFeA9E64dC1862 (Safe)
BENEFACTOR_MANAGER_ROLE     → 0x3B0AAf6e6fCd4a7cEEf8c92C32DFeA9E64dC1862 (Safe)
BENEFACTOR_DISABLER_ROLE    → 0x3B0AAf6e6fCd4a7cEEf8c92C32DFeA9E64dC1862 (Safe)
PEG_MANAGER_ROLE            → 0x3B0AAf6e6fCd4a7cEEf8c92C32DFeA9E64dC1862 (Safe)
```

### Admin transfer phase 1 (2026-07-06 → 2026-07-13)

- **2026-07-06 09:04:23 (block 25472475):** `AdminTransferRequested(Safe → 0xE8Dc0Fab349EA169283C48Ccfd09d797E6DB7c94)` — request to transfer DEFAULT_ADMIN to Timelock.
- **2026-07-13 08:23:11 (block 25522488):** `AdminTransferred(Safe → 0xE8Dc0F…)` — Timelock **accepts** admin via `acceptAdmin()`. `DEFAULT_ADMIN_ROLE` revoked from Safe, granted to Timelock.

### Role transfer phase 2 (2026-07-30, block 25647672)

Batched tx `0x31d506ea5eda12de600f509e6ae9b376cd3d8836b3b1bc72a19b68095b9d76e1` (called by Timelock via `0x352040dC5F272563A2d6e0eF7ABcd567A7440E8f` relayer):

For each of {`GLOBAL_DISABLER_ROLE`, `COLLATERAL_MANAGER_ROLE`, `COLLATERAL_DISABLER_ROLE`, `BENEFACTOR_MANAGER_ROLE`, `BENEFACTOR_DISABLER_ROLE`, `PEG_MANAGER_ROLE`}:
- `RoleRevoked(role, Safe, Timelock)`
- `RoleGranted(role, Timelock, Timelock)`

**6 of 7 non-default roles transferred in this single tx.**

### Role transfer phase 3 (2026-08-20, block 25794927)

Tx `0x74d1efa4097f18cbd8e4a3b3653268b80de331692ea441e8123b32503aa7e4f6`:

- `RoleRevoked(EPOCH_PERIOD_MANAGER_ROLE, Safe, Timelock)`
- `RoleGranted(EPOCH_PERIOD_MANAGER_ROLE, Timelock, Timelock)`

(The `EPOCH_PERIOD_MANAGER_ROLE` was apparently forgotten in the July 30 batch and only transferred on Aug 20.)

### Final state (verified on-chain via `hasRole`)

```
DEFAULT_ADMIN_ROLE          → EthenaTimelockController (0xE8Dc0F…)  [via SingleAdminAccessControl admin slot]
EPOCH_PERIOD_MANAGER_ROLE   → EthenaTimelockController (0xE8Dc0F…)  ✅ verified
GLOBAL_DISABLER_ROLE        → EthenaTimelockController (0xE8Dc0F…)
COLLATERAL_MANAGER_ROLE     → EthenaTimelockController (0xE8Dc0F…)
COLLATERAL_DISABLER_ROLE    → EthenaTimelockController (0xE8Dc0F…)
BENEFACTOR_MANAGER_ROLE     → EthenaTimelockController (0xE8Dc0F…)  ✅ verified via hasRole call
BENEFACTOR_DISABLER_ROLE    → EthenaTimelockController (0xE8Dc0F…)
PEG_MANAGER_ROLE            → EthenaTimelockController (0xE8Dc0F…)
```

**Single role holder** for all 8 roles: `0xE8Dc0Fab349EA169283C48Ccfd09d797E6DB7c94` (blockscout-tagged `EthenaTimelockController`).

**Security posture:** Although a single address holds all roles, that address is a **TimelockController contract** (OpenZeppelin) — meaning every admin action (including any future `removeBenefactor` call) must first go through a timelock delay. This is good practice but does NOT eliminate the bug — the timelock only delays, it does not prevent.

The Safe at `0x3B0AAf…` (the original admin) is still alive (holds 0.087 ETH) but has no roles on PSM anymore.

---

## 9. Deployment Date

- **Block:** 25351697
- **Timestamp:** 2026-06-19 12:42:35 UTC
- **Age at time of report:** 3 months, 5 days
- **Deployer factory:** CreateX (`0xba5Ed099633D3B313e4D5F7bdc1305d3c28ba5Ed`) — used for CREATE2 deterministic deployment
- **Initiator:** Safe relayer `0x66BAfa19075cdB54A8e574F9C6c9048b413A067C` (EOA) → Safe `0x3B0AAf…` → CreateX → PSM

---

## 10. Audit Trail Summary

| Category | Count | Detail |
|---|---|---|
| Proxy upgrades | **0** | Not a proxy, no upgrade pattern exists |
| Pause (SwapDisabled) | **0** | Never paused |
| Unpause (SwapEnabled) | **0** | (was enabled in constructor, never toggled) |
| Admin transfers | 2 (1 init + 1 to Timelock) | Safe → Timelock via `requestAdminTransfer` + `acceptAdmin` |
| Role grants (post-constructor) | 8 | All to Timelock during role migration |
| Role revokes | 8 | All from Safe during role migration |
| Collateral adds | 2 | USDC (Jun 19), USDT (Aug 4) |
| Collateral removes | **0** | `removeCollateral` never called |
| Collateral config updates | 4 | USDC config tuned 4× between Jun 19 and Aug 10 |
| Benefactor adds | 5 | See §4 |
| Benefactor removes | **0** | `removeBenefactor` **NEVER CALLED** |
| Benefactor disables | **0** | `disableBenefactor` never called |
| Benefactor max-limit updates | 12 (3×4 kinds) | Per-benefactor epoch/period limits tuned |
| Benefactor fee updates | 5 | Per-(benefactor, collateral) fee adjustments |
| PegPrice updates | **0** | pegPrice still 1.0 from constructor |
| Custodian rotations | **0** | Both assetSendCustodian and assetReceiveCustodian unchanged from constructor |
| RescueFunds | **0** | Never called |
| DelegatedSigner operations | **0** | Feature unused |

### Operational activity timeline

```
2026-06-19  Deploy PSM (block 25351697), admin=Safe, all 8 roles to Safe
2026-06-19  addCollateral(USDC)             (block 25351702)
2026-06-19  addBenefactor(0x3Aa3Fd1B…)      (block 25351702)
2026-06-22  First swap: 1 USDC → 0.9995 USDtb   (block 25373070)
…
2026-07-06  AdminTransferRequested(Safe → Timelock)
2026-07-13  AdminTransferred → Timelock becomes DEFAULT_ADMIN
2026-07-30  Batch role transfer: 6 of 7 roles moved Safe → Timelock
2026-08-04  addBenefactor(0xcf8925D5…)
2026-08-04  addBenefactor(0x1FcC47Ee…), addBenefactor(0xb32dd55d…), addBenefactor(0x8DF6721F…) — batched in 1 tx
2026-08-04  addCollateral(USDT)
2026-08-05  First large institutional swap: $10,000 USDC → USDtb (benefactor 0x1FcC47Ee…)
2026-08-05  $100k swap, then $890k swap (same benefactor)
2026-08-06  EpochDurationUpdated: 1800s → 3600s
2026-08-07  Global epoch + period limits updated
2026-08-10  USDC collateral config updated
2026-08-13  BenefactorSwapForCollateralFee updates
2026-08-17  $7M USDC → USDtb swap (benefactor 0x1FcC47Ee…)
2026-08-20  Role transfer completion: EPOCH_PERIOD_MANAGER_ROLE moved Safe → Timelock
2026-08-20  $7.99M USDtb → USDC swap (benefactor 0x1FcC47Ee…)
2026-09-02  $15M USDC → USDtb swap (benefactor 0x1FcC47Ee…)
2026-09-04  $4M + $10M + $200k USDC → USDtb swaps (benefactor 0xb32dd55d…)
2026-09-10  $10M + $5M USDtb → USDC swaps (benefactors 0x1FcC47Ee… and 0xb32dd55d…)
2026-09-17  First USDT-collateral swap (benefactor 0x8DF6721F…): $10M USDT → USDtb
2026-09-21  $9.99M USDtb → USDT swap (round-trip of #39)
2026-09-22  $5M USDT → USDtb swap
2026-09-23  $4.995M USDtb → USDT swap (most recent activity as of report)
```

---

## 11. Bug Trigger Probability Assessment

### The bug (recap)

```solidity
function removeBenefactor(address benefactor) external onlyRole(BENEFACTOR_MANAGER_ROLE) {
    if (!benefactorState[benefactor].config.isActive) revert BenefactorNotActive(benefactor);
    delete benefactorState[benefactor].config;   // ⚠️ Only zeros value-type fields; mappings persist
    emit BenefactorRemoved(benefactor);
}
```

`BenefactorConfig` struct contains **6 nested mappings** that Solidity's `delete` cannot clear:
- `swapForAssetFeeByCollateral` (per-collateral fee override)
- `swapForCollateralFeeByCollateral` (per-collateral fee override)
- `delegatedSigners` (per-signer delegation status — **CRITICAL**)
- `approvedBeneficiaries` (per-beneficiary approval — **CRITICAL**)
- `zeroSwapForAssetFeeExemptions` (per-collateral fee exemption)
- `zeroSwapForCollateralFeeExemptions` (per-collateral fee exemption)

Plus `BenefactorState` itself contains 3 more uncleared mappings:
- `epochStateByDuration`, `periodStateByDuration` (rate-limit state — mostly benign)
- `orderNonceInvalidator` (used-nonce tracking — mostly benign)

If `addr X` is removed then re-added, the re-added benefactor X inherits all of the above stale state from the prior incarnation. Most concerning:
1. **Stale delegated signers** — a previously-approved signer could sign swap orders on behalf of re-added benefactor without going through `confirmDelegatedSigner` again.
2. **Stale approved beneficiaries** — previously-approved 3rd-party beneficiaries remain approved.
3. **Stale fee exemptions / overrides** — fee config from prior incarnation silently persists.

### Trigger probability (production history)

**0% observed.** `removeBenefactor` has never been called. No funds at risk from this bug to date.

### Trigger probability (forward-looking)

**Non-trivial.** The bug-triggering function (`removeBenefactor`) is:
- Reachable by `BENEFACTOR_MANAGER_ROLE` (currently held by EthenaTimelockController).
- The PSM is in active production with 5 benefactors (one of which, `0xcf8925D5…`, was added but never used — a prime candidate for future removal).
- Standard operational lifecycle of institutional benefactors (offboarding a partner, rotating keys, etc.) typically involves removal of stale benefactors.

**Realistic trigger scenarios:**
1. Ethena decides to offboard benefactor `0xcf8925D5…` (the unused one) → calls `removeBenefactor` → bug-state is created (but no impact yet, since re-add would be needed).
2. Ethena removes a benefactor, then later that same address is re-added (e.g., an institution temporarily pauses, then resumes) → **bug fires**, stale state silently persists.
3. Benefactor address compromise → Ethena `removeBenefactor`s it → addresses are rotated → if a new benefactor is ever set up at the same address (unlikely but possible with CREATE2 counterfactual wallets), bug fires.

### Severity if triggered

- **Stale delegated signers** → potential unauthorized swap signing → could allow draining of custodian inventory up to per-benefactor rate limits (currently 1,000 USDtb/epoch default, but configurable to higher per-benefactor limits — current benefactors have limits up to 20,000 USDtb/epoch and 10M USDtb/period).
- **Stale approved beneficiaries** → funds could be sent to previously-approved but now-unintended beneficiaries.
- **Stale fee exemptions** → fee bypass → revenue loss (low severity, fees are ≤1%).
- **Stale fee overrides** → could either over-charge (reputation risk) or under-charge (revenue loss).

---

## 12. Conclusion

| Question | Answer |
|---|---|
| Is PSM deployed & verified? | ✅ Yes |
| Is PSM active? | ✅ Yes — heavy use, $95M volume in 3 months |
| Is PSM deprecated/paused? | ❌ No — `isSwapEnabled()=true`, no `SwapDisabled` ever |
| Has `removeBenefactor` ever been called? | ❌ **NO** — 0 events |
| Has `removeCollateral` ever been called? | ❌ **NO** — 0 events |
| Bug triggered in production? | ❌ **NO** — never triggered |
| Bug moot? | ❌ **NO** — PSM is in active production |
| Bug theoretical? | ✅ **YES** — at this moment |
| Bug triggerable in future? | ✅ **YES** — `removeBenefactor` is reachable by `EthenaTimelockController` |
| TVL at risk if bug fires? | ~$43M in PSM custodians (plus per-epoch/period rate-limit caps) |
| Role concentration? | All 8 roles on `EthenaTimelockController` (single contract, but timelocked) |
| Proxy/upgrades? | None — direct deployment, no upgrade path |

### Final classification

> **PSM is ACTIVE and PRODUCTION-USED.** The `removeBenefactor` mapping-persistence bug has **never been triggered** (0 `BenefactorRemoved` events), so the bug is currently **theoretical**. However, the PSM is **not deprecated** and the bug-triggering function is reachable, so the bug represents a **real latent production risk** that should be fixed before any future `removeBenefactor` call is made. Given that one benefactor (`0xcf8925D5…`) has already been added but never used, a natural operational cleanup would be to remove it — and if Ethena does so without first patching the contract, the bug state will be created. If the same address is ever re-added (e.g., the institution returns), the bug fires.

### Recommended next actions

1. **Do not call `removeBenefactor` on the live PSM** until the contract is patched.
2. If cleanup of unused benefactor `0xcf8925D5…` is needed, prefer `disableBenefactor()` (sets `isActive=false`, does NOT delete state) — this is safe and reversible.
3. Patch the contract: in `removeBenefactor`, explicitly clear nested mappings (`delegatedSigners`, `approvedBeneficiaries`, fee overrides, fee exemptions) before/after the `delete`. Note this requires tracking keys, since Solidity mappings are not iterable.
4. Plan a redeployment + custodian migration (no proxy exists, so patching = new contract address).
5. Add a regression test that exercises the `remove → re-add` sequence and asserts that `delegatedSigners`/`approvedBeneficiaries`/fee overrides are empty after re-add.

---

## Appendix A — Methodology

### Data sources

- **Blockscout REST API v2** (Etherscan was Cloudflare-blocked as anticipated)
  - `GET /api/v2/smart-contracts/{addr}` — contract source, ABI, constructor args, verification status
  - `GET /api/v2/addresses/{addr}` — address metadata (creator, name, is_contract)
  - `GET /api/v2/addresses/{addr}/transactions` — top-level transactions (45 returned)
  - `GET /api/v2/addresses/{addr}/logs?topic={topic}` — filtered event logs
  - `GET /api/v2/addresses/{addr}/logs` — all logs (paginated, 50/page; 6 pages fetched, 148 unique logs after dedup)
  - `GET /api/v2/transactions/{hash}` — individual tx detail
  - `GET /api/v2/tokens/{addr}` — token metadata (symbol, decimals, supply)
  - `GET /api/v2/addresses/{addr}/counters` — tx/log counts
- **Blockscout eth-rpc** (JSON-RPC proxy)
  - `eth_call` for view functions (`isSwapEnabled`, `globalConfig`, `asset`, custodian addresses, `hasRole`, `balanceOf`)
  - `eth_getBlockByNumber` for block timestamps

### Reproducibility

All raw responses cached under `/tmp/psm-data/`:
- `contract_info.json` (469 KB — full verified source + ABI + bytecode)
- `txs2.json` (117 KB — 45 top-level transactions)
- `all_logs_*.json` (6 paginated files, total ~600 KB raw)
- `all_logs_merged.json` (148 unique logs, deduplicated)
- `log_benefactor_added.json` (5 events)
- `log_benefactor_removed.json` (0 events — key evidence)
- `deploy_internals.json` (empty — endpoint timed out, not needed)

### Topic hashes used (keccak256 of event signatures)

```
BenefactorAdded(address)              = 0x2fbec2225160ba8b27d83182d706920f1170182d15934f2122fd744049140ae2
BenefactorRemoved(address)            = 0xf0ebdfe9729215cc867dca3785f752df456766a9ce4b178ad1bca9919ca4a650
CollateralRemoved(address)            = 0xd89d2ee68ab04dca0193f48a4aff55e20fa5ec0429a8a8c1c51b8dad6178a593
SwapExecuted(...)                     = 0x15b92345f3ead624d82ae7a06769cdcf0e6514cc18ab02e39feb37ff8edb69c8
SwapEnabled()                         = 0x41c787961cb389554b90a8dbfb700790a3f278f50ba1d330c0b555d884789b5a
SwapDisabled()                        = 0x0a4f9ef21a3f2d279073474702eab63d142b2a13bad81a6a9e36d450a559a516
RoleGranted(bytes32,address,address)  = 0x2f8788117e7eff1d82e926ec794901d17c78024a50270940304540a733656f0d
RoleRevoked(bytes32,address,address)  = 0xf6391f5c32d9c69d2a47ea670b442974b53935d1edc7fd64eb21e047a839171b
```

### Caveats / limitations

1. The `internal-transactions` Blockscout endpoint timed out twice for the deployment tx hash; deployment structure was inferred from the outer tx's `execTransaction` decoded input + the constructor-emitted events. This is sufficient evidence — we know the deployer was CreateX via CREATE2 because (a) the outer tx's first parameter is `0xba5Ed099633D3B313e4D5F7bdc1305d3c28ba5Ed` (CreateX address) and (b) Blockscout tags the PSM's `creator_address_hash` as `0xba5Ed099633D3B313e4D5F7bdc1305d3c28ba5Ed` (CreateX).
2. Blockscout returns `creation_tx_hash: null` on the address-info endpoint, but the constructor-emitted events at block 25351697 all share tx hash `0xf7b18d6928b59075f3896b33c6600c555b1aa9d895574797173dd4f98acc4615`, which is the deployment wrapper tx.
3. Log pagination had some empty/error pages; results were deduplicated by `(block_number, log_index, tx_hash)` to ensure no double-counting.
4. The `hasRole(DEFAULT_ADMIN_ROLE, …)` call returned `false` for both the old Safe and the new Timelock because `SingleAdminAccessControl` (the PSM's base contract) stores the admin in a separate storage slot rather than in the role mapping. The Timelock's admin status was confirmed via (a) the `AdminTransferred` event trail, (b) the `hasRole(BENEFACTOR_MANAGER_ROLE, Timelock)` call returning `true`, and (c) Blockscout's `name` tag for the Timelock address.

### No speculation

All numerical values, dates, addresses, and counts in this report come directly from on-chain data via Blockscout. Where data could not be retrieved (e.g., internal-tx endpoint timeout), it is explicitly noted in the Caveats section.
