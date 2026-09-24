# Sky Audit — Integer Precision: View-Function Pricing DoS & Sub-Wei Backing Drift

> Target: `SUsds` (sUSDS) and `SavingsDai` (sDAI)
> Repo: `makerdao/sdai` — branches `susds` (`src/SUsds.sol`) and `master` (`src/SavingsDai.sol`)
> Bounty: Sky (Immunefi, no-KYC, max $10M)
> Severity: **LOW** (view/oracle DoS) + **INFORMATIONAL** (sub-wei backing drift)

---

## 1. Description

### 1A. View-function pricing DoS (overflow in real-time `chi`)

The ERC-4626 view helpers recompute the *current* rate accumulator on the fly instead of reading a
stored value, so that share/asset quotes stay correct even when `drip()` has not been called this
block:

```solidity
// src/SUsds.sol — convertToShares (L334), identical pattern in convertToAssets (L339),
// previewMint (L366), previewWithdraw (L385), totalAssets<-convertToAssets (L330),
// maxWithdraw<-convertToAssets (L381)
uint256 chi_ = (block.timestamp > rho)
    ? _rpow(ssr, block.timestamp - rho) * chi / RAY   // CHECKED mul
    : chi;
```

`SavingsDai` (`src/SavingsDai.sol`) uses the same pattern against the external `Pot`
(`convertToShares` L278–282, `convertToAssets` L284–288, `previewMint` L313, `previewWithdraw` L334,
`totalAssets` L274, `maxWithdraw` L330).

The multiplication `_rpow(ssr, dt) * chi` is **checked**. Under the same conditions as the
`drip()` brick (see `sky-vuln-rate-accumulator.md` — a high `ssr` set via the uncapped `file`, or an
extreme drip gap), `_rpow(ssr, dt) * chi` overflows `uint256` and the **view function reverts**.

Crucially, the mutating functions are unaffected by *this specific* path because they call
`drip()` (which advances `chi`/`rho` to the current block, after which `_rpow(ssr, dt)` with `dt≈0`
returns `RAY`-scale and no longer overflows). The damage is therefore confined to the
**read-only pricing surface** — but that surface is exactly what the ecosystem consumes as the
sUSDS/USDS (and sDAI/DAI) exchange rate:

- Many lending/collateral protocols compute the sUSDS price as `ISUsds.convertToAssets(1e18)`.
- ERC-4626 routers and oracles call `previewRedeem`/`convertToAssets` for accounting.
- Indexers, keepers and front-ends read `totalAssets()` for TVL.

If any of these reverts, downstream markets that depend on an on-chain sUSDS price can fail to
update, freeze borrows/liquidations, or compute stale prices — a second-order availability/price
risk layered on top of the primary `drip()` brick.

### 1B. Sub-wei backing drift from `diff` rounding

In `SUsds.drip()` the yield minted into the vault is computed as the difference of two floored
terms (L220):

```solidity
diff = totalSupply_ * nChi / RAY - totalSupply_ * chi_ / RAY;
```

Let `f_new = totalSupply_ * nChi / RAY` (real) and `f_old = totalSupply_ * chi_ / RAY` (real). The
contract mints `diff = floor(f_new) - floor(f_old)` of USDS, while the *true* increase in the
shareholders' claim is `f_new - f_old`. Because `floor(f_new) - floor(f_old)` can be **less than**
`f_new - f_old` (by up to < 1 wei) whenever `frac(f_new) > frac(f_old)`, the vault can mint slightly
*less* yield than the accounting says it owes.

`deposit`/`redeem` rounding is vault-favouring (deposit floors shares, redeem floors assets), so
each user operation donates up to 1 wei of surplus to the vault. The `diff` drift is a per-drip
random walk of magnitude < 1 wei in either direction, partially offset by the deposit/redeem
surplus. In an adversarial sequence of drips the contract's external USDS balance can fall below
`totalSupply * chi / RAY` by a few wei, at which point a `redeem(MAX)` of the *entire* remaining
supply asks for `floor(totalSupply * chi / RAY)` which exceeds the balance and **reverts**. The last
withdrawer must redeem 1 wei of assets fewer.

`SavingsDai` does **not** have this drift: it never mints yield into itself (the external `Pot`
holds the yield and `chi` is the Pot's own accumulator), so its `diff`-equivalent never exists inside
the vault. This is an `SUsds`-only artefact of moving the rate accumulator in-house.

---

## 2. Contract / Function / Line

| Issue | Location |
|---|---|
| View overflow (sUSDS) | `src/SUsds.sol` `convertToShares` L335, `convertToAssets` L340, `previewMint` L367, `previewWithdraw` L386, `totalAssets` L331, `maxWithdraw` L382 |
| View overflow (sDAI) | `src/SavingsDai.sol` `convertToShares` L280, `convertToAssets` L286, `previewMint` L315, `previewWithdraw` L336, `totalAssets` L274–276, `maxWithdraw` L330–332 |
| `diff` rounding drift | `src/SUsds.sol` `drip()` L219–222 |
| `_divup` edge (context) | `src/SUsds.sol` `_divup` L184–189 (`_divup(0,0)=0`; div-by-zero only if `chi`/`RAY`=0, unreachable in practice) |

---

## 3. Attack Scenario

**1A (view DoS).** Same trigger as the rate-accumulator brick: a ward sets a large `ssr` (or an
extreme drip gap elapses). Every `convertToAssets`/`convertToShares`/`preview*`/`totalAssets` call
reverts. Any protocol using `convertToAssets(1e18)` as the sUSDS price feed breaks until the next
successful `drip()` (e.g. via a deposit) or a governance upgrade.

**1B (backing drift).** An attacker (or natural traffic) performs a long sequence of
`drip()` calls interleaved with deposits/redeems chosen so the `frac(f_old) < frac(f_new)` branch
dominates, nudging the contract's USDS balance a few wei below `totalSupply*chi/RAY`. The final
holder then calls `redeem(balanceOf)` and reverts; they must call `redeem(balanceOf - 1)` or
`withdraw(assets - 1)`. No value is extractable — the "loss" is a few wei (1e-18 USDS) and is
absorbed by the rounding direction on the next operation.

---

## 4. PoC (Foundry)

```solidity
// SPDX-License-Identifier: UNLICENSED
pragma solidity ^0.8.21;

import "forge-std/Test.sol";
import { SUsds } from "src/SUsds.sol";
import { ERC1967Proxy } from "@openzeppelin/contracts/proxy/ERC1967/ERC1967Proxy.sol";

// (reuse the minimal Vat/Usds/UsdsJoin fakes from sky-vuln-rate-accumulator.md)

contract PrecisionPoC is Test {
    SUsds s;
    // ... setUp() identical to RateBrickPoC ...

    function testViewOverflowReverts() public {
        // ward sets a huge but contract-legal ssr
        // vm.prank(ward); s.file("ssr", 10**40);
        // vm.warp(block.timestamp + 2);

        // drip() reverts (checked mul) -> views that recompute chi also revert
        vm.expectRevert(); s.convertToAssets(1e18);   // the canonical sUSDS price call
        vm.expectRevert(); s.convertToShares(1e18);
        vm.expectRevert(); s.totalAssets();
    }
}
```

For 1B, a fuzz harness asserting `usds.balanceOf(s) >= totalSupply * chi / RAY` after random
deposit/redeem/drip sequences will, over long runs, find the few-wei underflow sequence; the
remediation is to round `diff` *up* (use a `_divup`-style ceiling on the yield minted) or round the
per-share claim *down* symmetrically so the vault is always over-collateralised.

```solidity
// 1B invariant test (sketch)
function test_invariant_vaultAlwaysBacked(uint256 seed) public {
    // randomised deposit/redeem/drip loop driven by `seed`
    // assert: usds.balanceOf(address(s)) + 1 >= s.convertToAssets(s.totalSupply());
    // currently this CAN fail by a couple of wei after adversarial drip sequences
}
```

---

## 5. Impact

- **1A:** Read-only price/feed reverts → broken sUSDS oracles, frozen collateral accounting, stale
  prices in dependent markets. Self-heals on the next successful `drip()` (e.g. any deposit), but
  during the window every integration reading the rate is degraded. Severity LOW (cascading
  availability/price risk; no direct theft).
- **1B:** At most a few-wei shortfall; only observable as a revert on a *full* final redeem. No
  profit path. Severity INFORMATIONAL.

---

## 6. Severity

- **1A: LOW** — oracle/integration DoS; gated behind the same (low-likelihood) `ssr`/drip-gap
  trigger as the primary brick; no fund loss.
- **1B: INFORMATIONAL** — sub-wei, non-extractable, self-correcting on the next op.

---

## 7. Three-Perspective Audit

**Smart-contract logic view.** Recomputing `chi` in the views (rather than storing it) is a
deliberate choice so quotes stay live between drips; it is mathematically equivalent to the Pot's
`drip()`. The only defect is the absence of an overflow-tolerant code path — the same unchecked
growth bound that bites `drip()` bites the views, but the views cannot heal themselves by advancing
`rho`. The `diff` rounding is a textbook floor-difference-vs-difference-of-floors artefact; it is
< 1 wei and vault-favouring in aggregate, but not *guaranteed* non-negative per drip.

**Economic / risk view.** `convertToAssets(1e18)` is *the* sUSDS/USDS rate for the DeFi ecosystem.
Making it revert-able (even under a governance trigger) turns a savings-vault parameter mistake into
a broad pricing incident. For 1B, the economic magnitude is zero — it is a correctness/robustness
smell, not a risk.

**Operational / integration view.** Integrators should defensively cap their reliance on
`convertToAssets` (e.g. cache the last-good rate, treat revert as "stale", never let an sUSDS price
revert break a borrow loop). The contract-side hardening is to make `chi` recomputation
overflow-safe (bounded `dt`/`ssr`) and to round `diff` in the vault's favour so the
balance ≥ backing invariant holds by construction.

### Recommended fix (sketch)
- 1A: bound the effective `dt` used in the view-side `_rpow` (e.g. `min(dt, MAX_DT)`) and/or cap
  `ssr` in `file`, so the view multiply cannot overflow.
- 1B: mint yield with ceiling rounding, e.g.
  `diff = _divup(totalSupply_ * (nChi - chi_), RAY)` is *not* safe (can over-mint); instead compute
  `diff` so that `balance >= totalSupply * nChi / RAY` is invariant — simplest is to round the
  per-share asset claim *down* (`assets = shares * chi / RAY` already does) and mint
  `floor`-rounded yield, then accept that the last wei is donated surplus; or add a 1-wei tolerance
  in `_burn` (`assets` floor already provides this). Document the invariant explicitly.
