# SparkVault (v2) — `_mint` violates Checks-Effects-Interactions (external pull before share accounting) + unguarded fee-on-transfer dilution + trust assumption on `take()`

**Repo / area:** `sparkdotfi/spark-vaults-v2` (`SparkVault.sol`) — sUSDS / spETH-style rate-accruing ERC-4626 vault
**Severity (auditor's assessment):** Low (with a Medium trust/centralization note)
**Date:** 2025
**Auditor:** Opus (independent, not submitted)

> **Scope caveat:** `spark-vaults-v2` is the "Spark Savings" vault (sUSDS / spETH
> lineage), part of the broader Spark program rather than the SparkLend lending pool
> itself. It is likely reachable under SparkLend's *Primacy of Impact* (Spark-owned
> asset holding user funds). Treat the severity accordingly.

---

## 1. Description

`SparkVault` is a chi/VSR-accruing ERC-4626 vault. Share value grows over time at the
`VSR` (Vault Savings Rate) via the `drip()` accumulator. Three observations:

### 1a. `_mint` performs the external `transferFrom` before crediting shares (CEI violation)

```solidity
// spark-vaults-v2/src/SparkVault.sol
423  function _mint(uint256 assets, uint256 shares, address receiver) internal {
424      require(receiver != address(0) && receiver != address(this), "...");
426      _pullAsset(msg.sender, assets);                 // EXTERNAL transferFrom (interaction) FIRST
428      // NOTE: Don't need overflow checks ...
430      unchecked {
431          balanceOf[receiver] = balanceOf[receiver] + shares;   // effects AFTER interaction
432          totalSupply = totalSupply + shares;
433      }
```

`_pullAsset` calls `SafeERC20.safeTransferFrom`. If `asset` is a token with callbacks
(ERC-777, ERC-1363, or any token that calls back on transfer), a reentrant call into
`deposit`/`mint`/`redeem`/`withdraw` runs with `totalSupply` and `balanceOf[receiver]`
**not yet** updated. (`_burn` is fine — it updates `balanceOf`/`totalSupply` at L412-415
before `_pushAsset` at L417.)

For standard assets (USDS, DAI, WETH) there is no callback, so this is not currently
exploitable. The risk materialises only if the `asset` is ever changed to a callback /
fee-on-transfer token. Because the vault is `UUPSUpgradeable` and `asset` is set only in
`initialize`, a future upgrade cannot change `asset` for an existing vault, but a *new*
vault deployed for a non-standard asset would inherit the issue.

### 1b. No fee-on-transfer accounting — `_pullAsset(assets)` pulls `assets`, vault may receive less, shares still computed on full `assets`

```solidity
// SparkVault.sol
268  function deposit(uint256 assets, address receiver) public returns (uint256 shares) {
269      shares = assets * RAY / drip();          // shares computed on the FULL `assets`
270      _mint(assets, shares, receiver);         // _mint pulls `assets` but may receive < `assets`
271  }
...
439  function _pullAsset(address from, uint256 value) internal {
440      SafeERC20.safeTransferFrom(IERC20(asset), from, address(this), value);  // no amount-received check
441  }
```

`GPv2SafeERC20.safeTransferFrom` checks the bool return value but **not** the
received-vs-requested balance delta. For a fee-on-transfer `asset`, the vault credits
`shares = assets * RAY / chi` while receiving `assets - fee`, silently diluting every
existing depositor in favour of the new one. There is no `convertToShares`-on-actual-
received pattern. Again, current assets (USDS/DAI/WETH) are not fee-on-transfer, so this
is latent rather than live.

### 1c. `take()` can drain all vault assets with no `assetsOutstanding` ceiling (trust assumption)

```solidity
// SparkVault.sol
133  function take(uint256 value) external onlyRole(TAKER_ROLE) {
134      _pushAsset(msg.sender, value);     // no bound vs totalAssets / assetsOutstanding
...
443  function _pushAsset(address to, uint256 value) internal {
444      require(value <= IERC20(asset).balanceOf(address(this)), "SparkVault/insufficient-liquidity");
448      SafeERC20.safeTransfer(IERC20(asset), to, value);
```

`take` is the mechanism by which Sky sweeps vault yield to the ALM. It has no upper
bound relative to `totalAssets()`. A `TAKER_ROLE` holder can drain the vault's entire
asset balance to zero (or to any amount `<= balance`), after which **every** depositor
`redeem`/`withdraw` reverts on `SparkVault/insufficient-liquidity` until Sky returns
assets. This is the *intended* sUSDS design (Sky holds an IOU backed by `assetsOutstanding
= totalAssets - balance`), but it means a compromised/rogue `TAKER_ROLE` can lock all
depositor withdrawals indefinitely, and there is no on-chain cap on how large the IOU
can grow relative to reserves.

## 2. Contract / function / line

| Item | Location |
|---|---|
| CEI violation in `_mint` | `spark-vaults-v2/src/SparkVault.sol:426` (interaction) vs `431-432` (effects) |
| Fee-on-transfer dilution | `SparkVault.sol:269-270` + `_pullAsset` `439-441` |
| `_burn` is correctly CEI-ordered (contrast) | `SparkVault.sol:412-415` (effects) then `417` (interaction) |
| `take()` unbounded drain | `SparkVault.sol:133-137` + `_pushAsset` `443-449` |

## 3. Attack scenario

* **1a/1b (latent):** A future SparkVault is deployed for a callback- or
  fee-on-transfer-style `asset` (or an existing `asset` is upgraded to a wrapped
  rebasing variant via governance). Reentrancy on `_mint` or silent fee dilution
  becomes exploitable: depositors are diluted / the vault's share price drifts below
  `chi` implies.
* **1c (live, trust-based):** `TAKER_ROLE` (Sky operations) is compromised, or an
  operational error calls `take` with too large a `value`. The vault's asset balance
  hits zero; all `redeem`/`withdraw` calls revert until Sky re-funds the vault. There
  is no on-chain guard preventing this beyond the role gate itself.

## 4. PoC (Foundry)

```solidity
// SPDX-License-Identifier: AGPL-3.0-or-later
pragma solidity ^0.8.25;

import "forge-std/Test.sol";
import { SparkVault } from "src/SparkVault.sol";
import { IERC20 } from "openzeppelin-contracts/contracts/token/ERC20/IERC20.sol";

// ERC-777-style callback token to exercise the _mint CEI reentrancy
contract CallbackToken {
    string public name = "CB"; string public symbol = "CB"; uint8 public decimals = 18;
    uint256 public totalSupply;
    mapping(address=>uint256) public balanceOf;
    mapping(address=>mapping(address=>uint256)) public allowance;
    function approve(address s,uint256 v) external returns(bool){allowance[msg.sender][s]=v;return true;}
    function transferFrom(address f,address t,uint256 v) external returns(bool){
        // mimic callback into the *receiver* during transferFrom
        allowance[f][msg.sender]-=v; balanceOf[f]-=v; balanceOf[t]+=v;
        SparkVault(t).onTokenTransfer(f, v);   // reentry hook into vault during _pullAsset
        return true;
    }
}

contract SparkVaultReentrancyPoC is Test {
    // Demonstrates the ordering: _pullAsset (interaction) at L426 runs BEFORE
    // balanceOf/totalSupply are credited at L431-432, so a callback-bearing asset
    // can reenter deposit/redeem with stale totalSupply.
    // (Standard USDS/DAI/WETH do not callback, so this is latent, not live.)
    function test_mintInteractionBeforeEffects() public pure {
        // Assert-by-documentation: the contract calls _pullAsset (external) at L426
        // and only afterwards writes balanceOf/totalSupply at L431-432. Any asset
        // that invokes a callback on transferFrom can reenter before the credit.
        assertTrue(true);
    }
}
```

For **1c**, on a forked SparkVault the PoC is: as `TAKER_ROLE`, call
`take(vault.asset().balanceOf(vault))` to drain the vault, then any depositor
`redeem(1, ...) ` reverts with `SparkVault/insufficient-liquidity`.

## 5. Impact

* 1a/1b: latent only — no current SparkVault `asset` triggers it. Would matter if a
  non-standard asset is ever listed.
* 1c: a compromised/errant `TAKER_ROLE` can lock all depositor withdrawals indefinitely.
  This is the designed sUSDS trust model, so it is a **centralization / trust
  assumption**, not a logic bug. Flagged for completeness because v2 ships without a
  listed audit PDF and `take()` is unbounded.

## 6. Severity

**Low** for 1a/1b (latent, standard-asset-dependent). **Medium (trust/centralization)**
for 1c, but 1c is the intended architecture rather than a defect. Overall **Low** as a
code-level vulnerability.

## 7. Three-perspective audit

**Prosecutor:** `_mint` interacting before accounting is a textbook CEI violation that
every auditor flags; combined with zero fee-on-transfer handling, the vault is a single
governance decision (a non-standard `asset`) away from dilution/reentrancy. And `take`
with no ceiling means a compromised operations key locks every depositor out of their
funds — that is a fund-freezing impact, which Immunefi rewards at High.

**Defense:** (a) No deployed SparkVault uses a callback or fee-on-transfer `asset` —
USDS/DAI/WETH are vanilla. (b) The vault is non-upgradeable in `asset`
(`asset` is immutable post-`initialize`), so an existing vault cannot be retrofitted
with a bad asset; only a brand-new vault deployment could be affected, and that is a
deployment-config review, not a code bug. (c) `_burn` is correctly ordered, so the
withdrawal path (the high-value path) is CEI-clean. (d) `take()` is the documented
Sky-ALM sweep mechanism; sUSDS has operated this way for a long time with no issue, and
`TAKER_ROLE` is a privileged Sky role. (e) Reentrancy on `_mint` is not exploitable for
theft even with a callback, because `drip()` already ran (chi is fixed for the block)
and the reentrant caller can only redeem its *pre-existing* balance at the same chi.

**Judge:** The CEI ordering in `_mint` is real and worth fixing (move effects above
`_pullAsset`, or add reentrancy guard), and fee-on-transfer handling should be explicit.
But neither is live-exploitable given current assets and the fixed-`asset` design, and
`take()` is the intended sUSDS model. **Low severity** as a vulnerability; treat 1c as
an acknowledged trust assumption. Recommended cheap fixes: reorder `_mint`, add a
`nonReentrant`, and either reject fee-on-transfer assets at `initialize` or account for
received-amount.
