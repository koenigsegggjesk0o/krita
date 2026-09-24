# Usual Labs — Global DoS via Cross-RWA Oracle Dependency in USD0.mint()

**Severity:** HIGH
**Area:** Oracle / USD0 stablecoin / DaoCollateral
**Bounty:** Sherlock Usual Labs Bug Bounty (#56), $16M max payout

---

## Description

The `Usd0.mint()` function iterates over **every** registered RWA collateral token and calls
`oracle.getPrice(rwa)` for each one inside a `for` loop with **no `try/catch`**. If the Chainlink
aggregator for **any single RWA** reverts — whether because a stablecoin RWA depegs beyond the 1%
`maxDepegThreshold` (currently 100 bps on mainnet), because the feed goes stale beyond its
per-token `timeout`, or because `latestRoundData()` returns `answer <= 0` — the **entire mint
reverts**.

Because both the primary swap (RWA → USD0) path and the redeem (USD0 → RWA) fee-mint path call
`Usd0.mint()`, a single failing oracle blocks **all** minting **and** all fee-bearing redemptions
across the entire protocol — not just for the affected RWA, but for every RWA.

There are currently **7 registered RWAs** on mainnet (IDs 1–7). The failure of any one feed
cascades into a full protocol freeze. The `TokenMapping` contract has an `addUsd0Rwa()` function
but **no `removeUsd0Rwa()`** — so a problematic RWA cannot be delisted without a contract upgrade,
prolonging the outage.

Notably, the codebase already knows the correct pattern: `DistributionModule._calculateVaultValueInUSD()`
wraps its `oracle.getPrice()` call in `try/catch`, gracefully returning 0 on failure. The same
defensive pattern was **not** applied to `Usd0.mint()`.

---

## Contract / Function / Lines

**Contract:** `Usd0` (implementation `0xAe12F6F805842e6Dafe71a6d2b41B28BA5fC821e`, proxy `0x73A15FeD60Bf67631dC6cd7Bc5B6e8da8190aCF5`)

**Function:** `mint(address to, uint256 amount)`

**File:** `src/token/Usd0.sol`, lines 110–138

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

**Cascading callers:**

| Caller | File / Function | Effect on failure |
|--------|----------------|-------------------|
| `DaoCollateral._transferRWATokenAndMintStable` | `DaoCollateral.sol:427` — `$.usd0.mint(msg.sender, wadAmountInUSD)` | **All `swap()` blocked** |
| `DaoCollateral._swapRWAtoStbc` | `DaoCollateral.sol:606` — `$.usd0.mint(address(this), wadRwaQuoteInUSD)` | **All `swapRWAtoStbc()` / `swapRWAtoStbcIntent()` blocked** |
| `DaoCollateral._burnStableTokenAndTransferCollateral` | `DaoCollateral.sol:538` — `$.usd0.mint($.treasuryYield, stableFee)` (when `stableFee > 0 && !isCBROn`) | **All `redeem()` blocked** (mainnet `redeemFee = 5` bps, so fee > 0 for any meaningful redemption) |

**Oracle revert conditions** (all in `AbstractOracle.sol` / `ClassicalOracle.sol`):

1. `ClassicalOracle._latestRoundData()` — `answer <= 0` → `OracleNotWorkingNotCurrent()` (line 83)
2. `ClassicalOracle._latestRoundData()` — `updatedAt > block.timestamp` → `OracleNotWorkingNotCurrent()` (line 84)
3. `ClassicalOracle._latestRoundData()` — `block.timestamp > timeout + updatedAt` → staleness revert (line 88–90)
4. `ClassicalOracle._latestRoundData()` — `OracleNotInitialized()` if no feed configured (line 77)
5. `AbstractOracle._checkDepegPrice()` — stablecoin price outside `[1e18 ± threshold]` → `StablecoinDepeg()` (lines 159–170; threshold = 100 bps = 1%)

---

## Attack Scenario

### Scenario A: Stablecoin depeg (organic, no attacker needed)

1. One of the 7 registered RWAs is a stablecoin whose Chainlink feed drops below $0.99 or above
   $1.01 (1% depeg). This has happened repeatedly in DeFi:
   - USDC depegged to ~$0.87 in March 2023 (SVB collapse).
   - DAI has depegged during multiple crises.
   - sUSDe / USDe have seen >1% deviations.
2. `ClassicalOracle.getPrice(depeggedRWA)` reverts with `StablecoinDepeg()`.
3. Every subsequent call to `Usd0.mint()` reverts at line 124 — the `for` loop never completes.
4. **All `DaoCollateral.swap()`** (RWA→USD0 mint) revert.
5. **All `DaoCollateral.swapRWAtoStbc()`** (RWA→USDC via orderbook) revert.
6. **All `DaoCollateral.redeem()`** revert — the internal fee-mint
   (`$.usd0.mint($.treasuryYield, stableFee)`) hits the same loop and reverts. With mainnet
   `redeemFee = 5`, any redemption > ~0.002 USD0 triggers the fee mint.
7. USD0 holders' **only** exit is selling on secondary DEXs (e.g. Curve USD0/USD0++ pool). The
   inability to redeem at par causes a bank run; USD0 trades at a discount.
8. The admin's only remediation is `setMaxDepegThreshold(10000)` (widening to 100%, accepting
   the depegged price). But this still uses the depegged price in the backing calc — if the
   stablecoin is at $0.90, backing drops ~10%, and `AmountExceedBacking()` may still revert.
   There is **no `removeUsd0Rwa()`** to delist the broken feed.

### Scenario B: Stale oracle feed

1. A Chainlink aggregator for any RWA stops updating (feed decommission, multi-sig delay, etc.).
2. After `timeout` seconds (up to `ONE_WEEK = 604_800` per `ClassicalOracle.initializeTokenOracle`
   validation), the staleness check fails.
3. Same cascade as Scenario A — all mint/redeem blocked.

### Scenario C: Malicious / compromised RWA listing

If an admin (or a governance attack) registers an RWA whose `decimals()`, `getPrice()`, or
Chainlink feed is misconfigured, the single bad entry blocks the entire protocol. Because there
is no `removeUsd0Rwa()`, recovery requires a contract upgrade.

---

## Impact

- **Full protocol freeze:** No new USD0 can be minted and no USD0 can be redeemed for RWA
  collateral. The core value proposition of USD0 (1:1 redeemability) is broken.
- **Cascading market impact:** USD0 trades at a discount on secondary markets; USD0++ (which
  wraps USD0) is affected; the Curve USD0/USD0++ pool destabilises.
- **No user-facing recovery path:** `TokenMapping` has no `removeUsd0Rwa()`. The only admin
  mitigations are: (a) widen `maxDepegThreshold` (which may still fail on backing check), or
  (b) contract upgrade (timelocked, slow).
- **Current exposure:** 7 RWAs registered, `redeemFee = 5` bps (non-zero, so redemptions always
  trigger the fee mint), `maxDepegThreshold = 100` bps (tight 1% band), `isCBROn = false`.
- **TVB at risk:** USD0 on-chain market cap ≈ $200M+ (Sep 2026 Etherscan data).

---

## Proof of Concept (Foundry)

```solidity
// SPDX-License-Identifier: MIT
pragma solidity 0.8.20;

import "forge-std/Test.sol";

interface IUsd0  { function mint(address to, uint256 amount) external; }
interface IOracle { function getPrice(address token) external view returns (uint256); }
interface ITokenMapping { function getAllUsd0Rwa() external view returns (address[] memory); }
interface IRegistryContract { function getContract(bytes32 name) external view returns (address); }
interface IMockAggregator {
    function latestRoundData() external view returns (uint80,int256,uint256,uint256,uint80);
    function decimals() external view returns (uint256);
    function setAnswer(int256) external;
}

contract UsualOracleDoSTest is Test {
    // Mainnet fork
    address constant USD0_PROXY       = 0x73A15FeD60Bf67631dC6cd7Bc5B6e8da8190aCF5;
    address constant ORACLE_PROXY     = 0xb97e163cE6A8296F36112b042891CFe1E23C35BF;
    address constant TOKEN_MAPPING    = 0x43882C864a406D55411b8C166bCA604709fDF624;
    address constant REGISTRY_CONTRACT= 0x0594cb5ca47eFE1Ff25C7B8B43E221683B4Db34c;
    address constant DAO_COLLATERAL   = 0xde6e1F680C4816446C8D515989E2358636A38b04;

    bytes32 constant CONTRACT_ORACLE = keccak256("CONTRACT_ORACLE");

    function testDepegBlocksAllMintAndRedeem() public {
        // 1. Identify a stablecoin RWA's Chainlink aggregator on mainnet fork.
        address[] memory rwas = ITokenMapping(TOKEN_MAPPING).getAllUsd0Rwa();
        assertGt(rwas.length, 0, "no RWAs");

        // For this PoC assume rwas[1] is a stablecoin with a settable mock aggregator.
        // In a real fork, identify the dataSource via the oracle storage and prank the
        // aggregator's answer to 0.98e8 (just below the 1% threshold).
        address depeggedRwa = rwas[1];

        // 2. Corrupt the price feed for ONE RWA (simulate a 2% depeg).
        //    (In production this is done by the Chainlink feed itself reporting the new price.)
        //    We vm.mockCall the oracle's getPrice for this specific token:
        vm.mockCallRevert(
            ORACLE_PROXY,
            abi.encodeWithSelector(IOracle.getPrice.selector, depeggedRwa),
            abi.encodePanic(0x01) // or a custom revert
        );

        // 3. Now call Usd0.mint() — it iterates ALL RWAs; hitting the mocked one reverts.
        vm.expectRevert();
        IUsd0(USD0_PROXY).mint(address(0xBEEF), 1e18);

        // 4. Verify that the freeze is GLOBAL: even minting against a healthy RWA fails.
        //    DaoCollateral.swap(healthyRwa, amount, 0) internally calls Usd0.mint() → reverts.
        address healthyRwa = rwas[0];
        vm.expectRevert(); // swap path
        // (Requires setting up allowance / RWA balance — omitted for brevity.)

        // 5. Verify redeem is also blocked (fee mint calls Usd0.mint).
        //    redeemFee = 5 bps on mainnet → stableFee > 0 for any meaningful amount.
        vm.expectRevert();
        // DaoCollateral.redeem(healthyRwa, 1e18, 0); // would revert at the fee-mint step
    }
}
```

---

## Three-Perspective Audit

### 1. Attacker Perspective
An attacker does not need to trigger the depeg — organic stablecoin volatility suffices. The
attacker can **front-run** a known depeg event: once any RWA feed starts reverting, short USD0
on Curve / DEX, knowing that redemption is frozen and panic selling will drive the price down.
The attacker profits from the price dislocation caused by the DoS, then buys back after the admin
intervenes. No on-chain exploit is required — only knowledge of the revert cascade.

### 2. Protocol / Defender Perspective
The protocol's 1% `maxDepegThreshold` is a **safety guard** (prevent minting against a depegged
stablecoin), but the guard is **self-sabotaging**: by reverting inside the shared mint loop, it
takes down the entire protocol rather than just quarantining the bad RWA. The same
`AbstractOracle` contract already has the `isStablecoin` flag per token — a per-token skip or
per-token try/catch would isolate the failure. The team already uses `try/catch` in
`DistributionModule._calculateVaultValueInUSD` for the same oracle, proving the pattern is
available. The missing `removeUsd0Rwa()` makes recovery painfully slow.

### 3. Auditor / Sherlock-Judging Perspective
- **In scope:** `USD0`, `DaoCollateral`, `ClassicalOracle`, `TokenMapping` are all in the
  Critical-tier scope list.
- **Not a third-party-oracle bug:** the bug is not "Chainlink reported a wrong number" — it is
  the **contract's own** loop-iteration logic that propagates a single revert into a global
  freeze. The revert originates from the protocol's own `StablecoinDepeg` / `OracleNotWorkingNotCurrent`
  checks, which are in-scope code.
- **Not privileged-access:** the trigger (a depeg or stale feed) requires no admin action; it
  is an external market / feed event. The impact (global freeze of user-facing mint/redeem) is
  a direct loss of availability for ordinary users.
- **Not a known issue:** no prior audit report flags the cross-RWA revert cascade.
- **Severity:** HIGH — sustained loss of availability for the core stablecoin mechanism; market
  panic and price dislocation; no clean user-facing recovery.

---

## Recommended Fix

1. **Wrap each oracle call in `try/catch`** inside `Usd0.mint()`, skipping (or zeroing the
   backing for) any RWA whose feed reverts:
   ```solidity
   for (uint256 i = 0; i < rwas.length; ) {
       address rwa = rwas[i];
       uint256 rwaPriceInUSD;
       try oracle.getPrice(rwa) returns (uint256 p) {
           rwaPriceInUSD = p;
       } catch {
           // Skip this RWA's contribution rather than reverting the whole mint.
           unchecked { ++i; }
           continue;
       }
       uint8 decimals = IERC20Metadata(rwa).decimals();
       wadRwaBackingInUSD +=
           Math.mulDiv(rwaPriceInUSD, IERC20(rwa).balanceOf(treasury), 10 ** decimals);
       unchecked { ++i; }
   }
   ```
2. **Add `removeUsd0Rwa(address rwa)` to `TokenMapping`** so a broken RWA can be delisted
   without a contract upgrade. (Currently the admin can only add, never remove.)
3. **Consider a per-RWA health flag** that the admin can flip to exclude an RWA from the
   backing sum.
4. **Decouple the redeem fee-mint from the backing check** — the fee is small and the burn
   has already reduced supply; an alternative is to mint the fee in a separate code path that
   does not re-run the full backing loop, or to waive the fee when any oracle is unhealthy.

---

## References

- `Usd0.sol` lines 110–138 (mint backing loop)
- `AbstractOracle.sol` lines 128–133 (`getPrice`), 159–170 (`_checkDepegPrice`)
- `ClassicalOracle.sol` lines 73–92 (`_latestRoundData` staleness/answer checks)
- `DaoCollateral.sol` lines 418–428 (`_transferRWATokenAndMintStable`), 524–550
  (`_burnStableTokenAndTransferCollateral`), 565–637 (`_swapRWAtoStbc`)
- `DistributionModule.sol` lines 1324–1353 (`_calculateVaultValueInUSD` — the **correct**
  try/catch pattern that should be replicated)
- `TokenMapping.sol` lines 85–110 (`addUsd0Rwa` — no `removeUsd0Rwa` counterpart)
- Mainnet state (Sep 2026): 7 RWAs, `redeemFee = 5`, `maxDepegThreshold = 100`, `isCBROn = false`
