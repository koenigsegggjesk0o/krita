# Midas — `RedemptionVault` / `RedemptionVaultWithSwapper` tokenOut allowance over-decrement

**Protocol**: Midas (Sherlock bounty #122, up to $500,000 USDC)
**Repo**: `github.com/midas-apps/contracts` (cloned to `/home/z/usual-midas-contracts`)
**Severity**: Low–Medium
**Area**: Integer precision / accounting, cross-contract interaction

---

## 1. Description

`RedemptionVault._redeemInstant` decrements the per-token `tokensConfig[tokenOut].allowance`
by the **gross** `amountTokenOut` (the USD-equivalent of the full `amountMTokenIn`,
*including the fee portion*) even though the fee is collected in **mToken**, not in
`tokenOut`. Only the **net** `amountTokenOutWithoutFee` is ever transferred to the user.

This is inconsistent with the request-redemption path (`_approveRequest`), which
correctly decrements the allowance by the net `amountTokenOutWithoutFee` — the exact
amount actually pulled from `requestRedeemer` and sent to the user.

The inconsistency is confirmed by comparing the two code paths:

```
// _redeemInstant  (line 616) — GROSS
_requireAndUpdateAllowance(tokenOutCopy, amountTokenOut);

// _approveRequest (line 534) — NET  (correct)
_requireAndUpdateAndUpdateAllowance(request.tokenOut, amountTokenOutWithoutFee);
```

`amountTokenOut` is derived from the full `amountMTokenIn` (line 598-604), whereas
`amountTokenOutWithoutFee` is derived from `calcResult.amountMTokenWithoutFee`
(line 606-609). The fee is already taken in mToken at lines 618-624, so the gross
`tokenOut` figure is never actually dispensed — yet it is charged against the
allowance.

### Aggravation in `RedemptionVaultWithSwapper`

The swapper variant makes the bug worse. When the vault lacks `tokenOut` liquidity it
swaps mToken1 → mToken2 and redeems on a **second** vault (`mTbillRedemptionVault`).
The `tokenOut` the user receives in that path is dispensed by the *other* vault, not
by this one. Nevertheless this vault still calls:

```solidity
_requireAndUpdateAllowance(tokenOutCopy, amountTokenOut);   // line 165 — GROSS, pre-swap
```

…before the swap branch is even taken. The other vault independently decrements its
own `tokenOut` allowance inside its `redeemInstant`. Two vaults' allowances are
therefore consumed for a single token flow, and this vault's allowance is consumed for
tokens it never held or sent.

---

## 2. Contract / function / line

| File | Function | Line(s) |
|------|----------|---------|
| `contracts/RedemptionVault.sol` | `_redeemInstant` | 616 (`_requireAndUpdateAllowance(tokenOutCopy, amountTokenOut)`) |
| `contracts/RedemptionVault.sol` | `_approveRequest` (correct reference) | 534 (`_requireAndUpdateAllowance(request.tokenOut, amountTokenOutWithoutFee)`) |
| `contracts/abstract/ManageableVault.sol` | `_requireAndUpdateAllowance` | 519-528 |
| `contracts/RedemptionVaultWithSwapper.sol` | `_redeemInstant` | 165 (gross, before swap path) + 182-192 (swap path) |

---

## 3. Attack scenario

1. Admin sets `tokensConfig[USDC].allowance = 1_000_000e18` (base-18) and
   `instantFee = 1%` (100 bps).
2. Users redeem mToken for USDC. Each redeem decrements the USDC allowance by ~101% of
   the net USDC actually sent (gross includes the 1% fee that was taken in mToken, not
   USDC).
3. After ~990k USDC of *actual* outflows the allowance hits zero prematurely and every
   subsequent `redeemInstant` reverts with `"MV: exceed allowance"`, even though the
   vault / `requestRedeemer` still holds ample USDC.
4. In the `RedemptionVaultWithSwapper` case, every swap-path redeem additionally burns
   this vault's `tokenOut` allowance for tokens the vault never dispensed, accelerating
   the depletion and blocking **direct** (non-swap) redemptions on this vault even when
   it has the token balance to serve them.

Because `safeBulkApproveRequest(uint256[])` (the only "permissionless-looking" approve
path) is in fact gated by `onlyVaultAdmin` through its public overload, an attacker
cannot trigger arbitrary approves to work around the depletion; the only relief is an
admin `changeTokenAllowance` call.

---

## 4. Proof of Concept (Foundry, sketch)

```solidity
// SPDX-License-Identifier: MIT
pragma solidity 0.8.9;

import "forge-std/Test.sol";
// imports for MidasAccessControl, mToken, DataFeed mocks, DepositVault, RedemptionVault …

contract MidasAllowanceOverDecrementTest is Test {
    MidasAccessControl ac;
    MockUSDC usdc;            // 6 decimals
    MockMTok mTok;           // 18 decimals, mToken
    MockFeed mTokFeed;       // returns 1.05e18 (mToken = $1.05)
    MockFeed usdcFeed;       // returns 1e18
    RedemptionVault rv;

    function setUp() public {
        ac = new MidasAccessControl(); ac.initialize();
        usdc = new MockUSDC(); mTok = new MockMTok(); 
        mTokFeed = new MockFeed(1.05e18); usdcFeed = new MockFeed(1e18);
        // deploy + initialize RedemptionVault with instantFee = 100 (1%), allowance = 1_000_000e18
        // add USDC payment token, greenlist attacker, mint mTok to attacker, approve rv
    }

    function test_allowance_depleted_by_gross_not_net() public {
        uint256 snap = vm.snapshot();
        // redeem 100_000e18 mToken worth (~105_000 USDC) repeatedly until allowance exhausted
        uint256 redeemed;
        while (rv.tokensConfig(address(usdc)).allowance() > 0) {
            (bool ok,) = address(rv).call(
                abi.encodeWithSignature(
                    "redeemInstant(address,uint256,uint256)",
                    address(usdc), 100_000e18, 0));
            if (!ok) break;
            redeemed += 100_000e18;
        }
        uint256 netUsdcActuallySent = usdc.balanceOf(address(this));
        uint256 allowanceLeft = rv.tokensConfig(address(usdc)).allowance();
        // Assert: allowance hit 0 BEFORE 1_000_000 USDC of net outflows,
        // proving the gross (incl. fee) was charged.
        assertLt(netUsdcActuallySent, 1_000_000e6, "net outflow should be < allowance cap");
        assertEq(allowanceLeft, 0, "allowance exhausted early");
        vm.revertTo(snap);
    }
}
```

Run: `forge test --match-test test_allowance_depleted_by_gross_not_net -vv`.

---

## 5. Impact

* **Fund loss**: none directly. The over-decrement is conservative (caps are hit
  sooner, never later), so no user can withdraw more than intended.
* **Availability / DoS**: once the allowance is exhausted, all instant redemptions for
  that `tokenOut` revert (`"MV: exceed allowance"`) until an admin calls
  `changeTokenAllowance`. In the Swapper variant, direct (non-swap) redemptions are
  blocked even when the vault holds the tokens, because the allowance was burned by
  swap-path flows that the vault never dispensed.
* **Bounty alignment**: Moonwell/Midas severity rules list *"smart contracts
  inoperable due to a lock of funds"* and *"griefing"* under Medium. The 1-hour
  exploit-window assumption and admin's ability to top up the allowance cap the
  real-world window, hence Low–Medium rather than High.

---

## 6. Three-perspective audit

**Exploitability**
No private key, no price oracle, no special role is required to *trigger* the
depletion — ordinary user redemptions accumulate it. An attacker can accelerate it by
front-running many small redeems, but cannot amplify the per-call over-charge beyond
the configured fee %.

**Economic impact**
Bounded by the fee percentage (typically 0.5–2%): the allowance is consumed
`1/(1-fee)` × faster than intended. There is no direct fund theft; the cost is
operational (more frequent admin top-ups) plus a temporary redemption freeze if the
admin is slow to react. TVL at risk during the freeze is the un-redeemable balance,
recoverable once allowance is replenished.

**Fix recommendation**
In `_redeemInstant` (both `RedemptionVault` and `RedemptionVaultWithSwapper`), change:

```diff
- _requireAndUpdateAllowance(tokenOutCopy, amountTokenOut);
+ _requireAndUpdateAllowance(tokenOutCopy, amountTokenOutWithoutFee);
```

In `RedemptionVaultWithSwapper._redeemInstant`, additionally skip the allowance
decrement entirely when the swap path is taken, since the dispensed `tokenOut` comes
from `mTbillRedemptionVault` (which already enforces its own allowance):

```solidity
if (contractTokenOutBalance >= amountTokenOutWithoutFee.convertFromBase18(tokenDecimals)) {
    _requireAndUpdateAllowance(tokenOutCopy, amountTokenOutWithoutFee); // direct path only
    mToken.burn(user, calcResult.amountMTokenWithoutFee);
} else {
    // swap path: do NOT touch this vault's tokenOut allowance
    ...
}
```

---

## 7. References

* `_requireAndUpdateAllowance`: `contracts/abstract/ManageableVault.sol:519-528`
* Gross vs net discrepancy: `contracts/RedemptionVault.sol:616` vs `:534`
* Swapper swap path: `contracts/RedemptionVaultWithSwapper.sol:165-192`
