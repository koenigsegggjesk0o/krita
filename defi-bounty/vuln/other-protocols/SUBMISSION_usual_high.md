# SUBMISSION: Usual Labs — Global DoS via Cross-RWA Oracle Revert Cascade in Usd0.mint()

**Bounty:** Sherlock Usual Labs Bug Bounty (#56)
**Severity:** HIGH
**Protocol:** Usual Labs (USD0 stablecoin)
**Date:** 2026-09-24

---

## Summary

The `Usd0.mint()` function iterates over **every** registered RWA collateral token and calls
`oracle.getPrice(rwa)` for each one inside a `for` loop with **no `try/catch`**. If the
Chainlink aggregator for **any single RWA** reverts — whether because a stablecoin RWA depegs
beyond the 1% `maxDepegThreshold`, the feed goes stale, or `latestRoundData()` returns an
invalid answer — the **entire `mint()` reverts**.

Because both the primary swap (RWA → USD0) path and the redeem (USD0 → RWA) fee-mint path
call `Usd0.mint()`, a single failing oracle blocks **all** minting **and** all fee-bearing
redemptions across the entire protocol — not just for the affected RWA, but for every RWA.

The `TokenMapping` contract has an `addUsd0Rwa()` function but **no `removeUsd0Rwa()`** — so
a problematic RWA cannot be delisted without a contract upgrade, prolonging the outage.

Notably, the codebase already knows the correct pattern: `DistributionModule._calculateVaultValueInUSD()`
wraps its `oracle.getPrice()` call in `try/catch`, gracefully returning 0 on failure. The same
defensive pattern was **not** applied to `Usd0.mint()`.

---

## Vulnerability Detail

### The vulnerable loop

**Contract:** `Usd0` (`src/token/Usd0.sol`, lines 110–138)

```solidity
function mint(address to, uint256 amount) public {
    if (amount == 0) { revert AmountIsZero(); }
    Usd0StorageV0 storage $ = _usd0StorageV0();
    $.registryAccess.onlyMatchingRole(USD0_MINT);
    IOracle oracle = IOracle($.registryContract.getContract(CONTRACT_ORACLE));
    address treasury = $.registryContract.getContract(CONTRACT_TREASURY);

    address[] memory rwas = $.tokenMapping.getAllUsd0Rwa();
    uint256 wadRwaBackingInUSD = 0;
    for (uint256 i = 0; i < rwas.length;) {
        address rwa = rwas[i];
        uint256 rwaPriceInUSD = uint256(oracle.getPrice(rwa));   // ← reverts if ANY feed fails
        uint8 decimals = IERC20Metadata(rwa).decimals();
        wadRwaBackingInUSD +=
            Math.mulDiv(rwaPriceInUSD, IERC20(rwa).balanceOf(treasury), 10 ** decimals);
        unchecked { ++i; }
    }
    if (totalSupply() + amount > wadRwaBackingInUSD) { revert AmountExceedBacking(); }
    _mint(to, amount);
}
```

The `oracle.getPrice(rwa)` call on line 124 is **not** wrapped in `try/catch`. A revert from
any single RWA's oracle propagates up and aborts the entire `mint()` — the loop never completes,
the backing sum is never checked, and no USD0 is minted.

### Oracle revert conditions

All of the following conditions cause `oracle.getPrice(rwa)` to revert, any one of which
triggers the cascade:

1. **`ClassicalOracle._latestRoundData()`** — `answer <= 0` → `OracleNotWorkingNotCurrent()` (line 83)
2. **`ClassicalOracle._latestRoundData()`** — `updatedAt > block.timestamp` → `OracleNotWorkingNotCurrent()` (line 84)
3. **`ClassicalOracle._latestRoundData()`** — `block.timestamp > timeout + updatedAt` → staleness revert (lines 88–90)
4. **`ClassicalOracle._latestRoundData()`** — `OracleNotInitialized()` if no feed configured (line 77)
5. **`AbstractOracle._checkDepegPrice()`** — stablecoin price outside `[1e18 ± threshold]` → `StablecoinDepeg()` (lines 159–170; threshold = 100 bps = 1%)

### Cascading callers

| Caller | File / Function | Effect on failure |
|--------|-----------------|-------------------|
| `DaoCollateral._transferRWATokenAndMintStable` | `DaoCollateral.sol:427` — `$.usd0.mint(msg.sender, wadAmountInUSD)` | **All `swap()` blocked** |
| `DaoCollateral._swapRWAtoStbc` | `DaoCollateral.sol:606` — `$.usd0.mint(address(this), wadRwaQuoteInUSD)` | **All `swapRWAtoStbc()` blocked** |
| `DaoCollateral._burnStableTokenAndTransferCollateral` | `DaoCollateral.sol:538` — `$.usd0.mint($.treasuryYield, stableFee)` | **All `redeem()` blocked** (mainnet `redeemFee = 5` bps, so fee > 0 for any amount ≥ 2000 wei) |

### No recovery path

- `TokenMapping` has `addUsd0Rwa()` but **no `removeUsd0Rwa()`** — a broken RWA cannot be delisted.
- `setMaxDepegThreshold(10000)` widens the band to 100% (bypasses the depeg check), but does **not** help with stale feeds, zero answers, or uninitialized feeds. And if the depegged price is still used, `AmountExceedBacking()` may still revert.
- `activateCBR(coefficient)` unblocks redeem (at a discount via `cbrCoef`) but does **not** unblock minting.
- A contract upgrade is timelocked (hours to days).

### The fix pattern already exists

`DistributionModule._calculateVaultValueInUSD()` (lines 1335–1340) already wraps the same
`oracle.getPrice()` call in `try/catch`:

```solidity
try $.oracle.getPrice(address($.iUsd0ppVault)) returns (uint256 price) {
    iUsd0ppPrice = price;
} catch {
    iUsd0ppPrice = 0;
}
```

---

## Impact

- **Full protocol freeze:** No new USD0 can be minted and no USD0 can be redeemed for RWA
  collateral. The core value proposition of USD0 (1:1 redeemability) is broken.
- **Cascading market impact:** USD0 trades at a discount on secondary markets; USD0++ (which
  wraps USD0) is affected; the Curve USD0/USD0++ pool destabilises.
- **No user-facing recovery path:** `TokenMapping` has no `removeUsd0Rwa()`. The only admin
  mitigations are (a) widen `maxDepegThreshold` (doesn't help stale/zero feeds), (b) activate
  CBR (helps redeem at a discount but not mint), or (c) contract upgrade (timelocked, slow).
- **Trigger is organic:** A stablecoin depeg beyond 1% is a real-world event that has happened
  repeatedly (USDC -13% in March 2023, DAI -3%, sUSDe deviations). No attacker action is needed.
- **Current exposure:** Multiple RWAs registered, `redeemFee = 5` bps (non-zero, so redemptions
  always trigger the fee mint), `maxDepegThreshold = 100` bps (tight 1% band), `isCBROn = false`.

---

## Proof of Concept

A fully compilable Foundry PoC is provided at:
`/home/z/usual-poc/test/UsualOracleDoS.t.sol`

The PoC deploys the **real** `Usd0`, `ClassicalOracle`, and `TokenMapping` contracts (not logic
copies) and uses mock Chainlink aggregators. 10 tests pass, including:

1. **`test_baseline_mintSucceedsWhenAllOraclesHealthy`** — 3 healthy RWAs, mint succeeds.
2. **`test_depegOneRwaBlocksEntireMint`** — depegging RWA2 to $0.95 causes `StablecoinDepeg()`
   to propagate through the loop and revert the entire `mint()`, even though RWA1 and RWA3
   are healthy and there is $3000 of combined backing.
3. **`test_depegFirstRwaAlsoBlocksEntireMint`** — depegging RWA1 (first in loop) also blocks.
4. **`test_depegLastRwaAlsoBlocksEntireMint`** — depegging RWA3 (last in loop) also blocks.
5. **`test_staleFeedBlocksEntireMint`** — a stale feed (`updatedAt` too old) triggers
   `OracleNotWorkingNotCurrent()` and blocks the entire mint.
6. **`test_aggregatorRevertBlocksEntireMint`** — an aggregator that itself reverts also cascades.
7. **`test_revertIsStablecoinDepeg_fromOracle`** — confirms the exact error is `StablecoinDepeg()`
   propagating uncaught from the oracle through `Usd0.mint()`.
8. **`test_fix_tryCatchPreventsCascade`** — a patched `Usd0` with `try/catch` (the
   `DistributionModule` pattern) skips the bad RWA and mints successfully against the healthy ones.

### Key trace (test 2):

```
Usd0::mint(recipient, 1e18)
  ├─ TokenMapping::getAllUsd0Rwa() → [rwa1, rwa2, rwa3]
  ├─ ClassicalOracle::getPrice(rwa1) → 1e18          ✅ healthy
  ├─ ClassicalOracle::getPrice(rwa2)
  │   └─ ← [Revert] StablecoinDepeg()   ($0.95 < $0.99 threshold)
  └─ ← [Revert] StablecoinDepeg()       ❌ entire mint aborts — NO try/catch
```

### How to run:

```bash
cd /home/z/usual-poc
export PATH=$PATH:$HOME/.foundry/bin
forge test -vvv
```

---

## Recommendation

Wrap each `oracle.getPrice()` call in `try/catch` inside `Usd0.mint()`, skipping any RWA whose
feed reverts (the same pattern already used in `DistributionModule._calculateVaultValueInUSD`):

```solidity
for (uint256 i = 0; i < rwas.length; ) {
    address rwa = rwas[i];
    uint256 rwaPriceInUSD;
    try oracle.getPrice(rwa) returns (uint256 p) {
        rwaPriceInUSD = p;
    } catch {
        unchecked { ++i; }
        continue;   // skip unhealthy RWA
    }
    uint8 decimals = IERC20Metadata(rwa).decimals();
    wadRwaBackingInUSD +=
        Math.mulDiv(rwaPriceInUSD, IERC20(rwa).balanceOf(treasury), 10 ** decimals);
    unchecked { ++i; }
}
```

Additionally:
- Add `removeUsd0Rwa(address rwa)` to `TokenMapping` so a broken RWA can be delisted without
  a contract upgrade.
- Consider decoupling the redeem fee-mint from the full backing loop (the fee is small and the
  burn has already reduced supply).

---

## References

- `Usd0.sol` lines 110–138 (mint backing loop — the vulnerable code)
- `AbstractOracle.sol` lines 128–170 (`getPrice`, `_checkDepegPrice`)
- `ClassicalOracle.sol` lines 73–92 (`_latestRoundData` staleness/answer checks)
- `DaoCollateral.sol` lines 418–428, 524–550, 565–637 (cascading callers)
- `DistributionModule.sol` lines 1324–1353 (the correct try/catch pattern)
- `TokenMapping.sol` lines 85–110 (`addUsd0Rwa` — no `removeUsd0Rwa`)
- `constants.sol` line 223 (`INITIAL_MAX_DEPEG_THRESHOLD = 100`), line 226 (`MAX_RWA_COUNT = 10`)
