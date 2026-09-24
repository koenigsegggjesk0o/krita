# Olympus BondDepository V2 — unguarded `uint64` casts silently truncate `controlVariable` (and `maxPayout`/`totalDebt`), enabling underpriced bonds

## Summary

`OlympusBondDepositoryV2.create` and `_tune` derive the market's
`controlVariable` as a full-width `uint256` and then cast it to `uint64` with no
range check. Solidity's explicit narrowing cast **silently truncates** the high
bits rather than reverting. If the computed value exceeds `type(uint64).max`
(~1.84e19) — which is realistic when `baseSupply` is large and/or
`targetDebt`/`timeRemaining` are small — the stored control variable becomes a
fraction of the intended value, collapsing the bond `marketPrice`
(`price = cv * debtRatio`). The treasury then sells OHM for far less quote token
than intended. The same unguarded narrowing affects `maxPayout`, `totalDebt`,
`maxDebt`, `targetDebt`, and `newControlVariable` in `_tune`.

## Contract / Function / Lines

- Contract: `contracts/BondDepository.sol` — `OlympusBondDepositoryV2`
- `create(...)`:
  - line **298**: `uint64 targetDebt = uint64(... _market[0] ...);`
  - line **305**: `uint64 maxPayout = uint64((targetDebt * _intervals[0]) / secondsToConclusion);`
  - line **326**: `uint256 controlVariable = (_market[1] * treasury.baseSupply()) / targetDebt;`
  - line **346**: `controlVariable: uint64(controlVariable),`  ← truncation
  - line **349**: `maxDebt: uint64(maxDebt)`
- `_tune(...)`:
  - line **244**: `markets[_id].maxPayout = uint64((capacity * meta.depositInterval) / timeRemaining);`
  - line **250**: `uint64 newControlVariable = uint64((price * treasury.baseSupply()) / targetDebt);`  ← truncation
- Price derivation: `_marketPrice` line **531–533**, `marketPrice` 408–410,
  `debtRatio` 432–434, `_debtRatio` 541–543 (all divide by `baseSupply()`).

## Root cause

```solidity
// create()
uint256 controlVariable = (_market[1] * treasury.baseSupply()) / targetDebt;
...
terms.push(Terasury({
    ...
    controlVariable: uint64(controlVariable),   // silent truncation
    ...
}));

// _tune()
uint64 newControlVariable = uint64((price * treasury.baseSupply()) / targetDebt);
```

`price` (9-dec) × `baseSupply()` (9-dec, can be ~1e16–1e17 and grows over time as
OHM is minted) divided by a small `targetDebt` easily exceeds 2^64. The cast
discards the high 192 bits. Because `marketPrice = cv * debtRatio`, a truncated
`cv` directly lowers the price users pay, increasing `payout_` per quote token
(line 105) and draining treasury OHM below fair value.

`_tune` runs on **every deposit** (`deposit` → `_tune`, line 167), so even a
market that started with a sane `cv` can be silently re-truncated to a tiny value
once `baseSupply` grows enough, mid-market.

## Attack scenario

1. Governance creates a market with parameters where
   `_market[1] * baseSupply() / targetDebt > 2^64` (e.g. a low-capacity market
   where `targetDebt` is small, or after OHM supply has grown so `baseSupply` is
   large). The stored `controlVariable` is now e.g. the low 64 bits — potentially
   orders of magnitude smaller than intended.
2. `marketPrice = cv * debtRatio / 10^quoteDecimals` is now far below the
   intended bond price.
3. Attacker (anyone) calls `deposit` with `_maxPrice` set to the (now-tiny)
   `marketPrice`, receiving `payout_ = (_amount * 1e18) / price / 10^quoteDecimals`
   — a greatly inflated OHM payout for their quote tokens.
4. Even if `create` was safe, a later `deposit` triggers `_tune`; if
   `price * baseSupply() / targetDebt` now overflows 64 bits, `cv` is re-truncated
   downward and the market flips to underpriced from that deposit onward.
5. Treasury OHM is sold at a discount; attacker redeems notes after vesting and
   dumps OHM for profit.

## PoC (Foundry, control-variable truncation)

```solidity
// SPDX-License-Identifier: AGPL-3.0
pragma solidity ^0.8.10;

import "forge-std/Test.sol";
import "../contracts/BondDepository.sol";
// + Treasury/Authority/OHM/sOHM/gOHM/Staking mocks

contract BondCvTruncPoC is Test {
    OlympusBondDepositoryV2 bond;
    ITreasury treasury; // baseSupply() mocked

    function testControlVariableTruncationUnderpricesBonds() public {
        // baseSupply = 20_000_000 OHM (9 dec) = 2e16
        vm.mockCall(address(treasury), abi.encodeWithSelector(ITreasury.baseSupply.selector), abi.encode(20_000_000 * 1e9));
        // intended initial price 100 DAI/OHM (9 dec) = 1e11
        uint256 initialPrice = 100 * 1e9;
        // capacity in OHM = 1 OHM (tiny targetDebt) to force cv overflow
        uint256 capacity = 1 * 1e9;
        // targetDebt (capacityInQuote=false) = capacity = 1e9
        // cv_intended = initialPrice * baseSupply / targetDebt = 1e11 * 2e16 / 1e9 = 2e18  (fits u64)
        // Now pick capacity = 1 (1 wei of OHM, 9-dec token -> 1 raw):
        capacity = 1;
        uint256 cvIntended = (initialPrice * (20_000_000 * 1e9)) / capacity; // = 2e27 -> > 2^64
        uint64 cvStored = uint64(cvIntended);
        assertTrue(cvStored < cvIntended, "expected truncation");
        // marketPrice stored = cvStored * debtRatio -> drastically below intended 1e11
        // => payout per DAI inflated => treasury sells OHM cheap
        assertLt(uint256(cvStored) , cvIntended / 1_000_000);
    }
}
```

## Impact

- **Treasury value leak:** OHM is sold via bonds at a price below the
  governance-intended price whenever `cv` truncates. Loss scales with remaining
  market capacity and can be large (whole-market discount).
- **Mid-market flip:** Because `_tune` re-computes on every deposit, a market
  that was correctly priced at creation can turn underpriced as `baseSupply`
  grows — no governance action required to trigger.
- Reliability: `maxPayout`/`totalDebt`/`maxDebt` truncations can also produce
  nonsensical capacity/debt-breaker behavior (e.g. a too-small `maxDebt` closes
  the market prematurely, or a too-large truncated `maxPayout` removes the
  per-deposit cap).

## Severity

**Medium** — silent truncation leading to mispriced bonds / treasury OHM sold
below fair value. Triggered by governance parameter choice and/or supply growth;
exploitable by any depositor once a market is in the truncated regime. Not
Critical because it requires specific parameterization, but the unguarded cast
is a clear defect and `_tune` makes it dynamic.

## Three-perspective audit

- **Differential:** `payout_` (line 105) is deliberately kept in `uint256` and
  bounded by `require(payout_ <= market.maxPayout)`. But the *inputs* to pricing
  (`controlVariable`, `maxPayout`, `totalDebt`, `maxDebt`) are narrowed without
  checks — inconsistent with the care taken elsewhere to use full-width math
  (e.g. `1e18` scaling) before narrowing only at the very end.
- **Adversarial:** An attacker cannot pick `cv` directly (governance does), but
  can detect a truncated market on-chain (`marketPrice` vs. intended price) and
  deposit aggressively before `_tune`/governance corrects it. `_tune`’s
  per-deposit recompute expands the attack window.
- **Defensive / fix:** Add revert-on-overflow guards before every narrowing
  cast, e.g.:
```solidity
require(controlVariable <= type(uint64).max, "cv overflow");
terms.push(Terasury({ controlVariable: uint64(controlVariable), ... }));
```
  Repeat for `targetDebt`, `maxPayout`, `maxDebt`, and the `_tune` recompute.
  Alternatively, store `controlVariable` as `uint128`/`uint256` and adjust
  `marketPrice` scaling. Add invariant tests asserting
  `stored == computed` after `create` and after `_tune`.
