# Cap — Swapper Sweeps Pre-Existing Token Balance to Caller

**Protocol:** Cap (cap-labs-dev)
**Bounty:** $1,000,000 USDC (top-tier)
**Sherlock Bounty URL:** https://audits.sherlock.xyz/bug-bounties/114
**Source:** https://github.com/cap-labs-dev/cap-contracts
**Severity:** MEDIUM
**Area:** Cross-contract interaction / ERC-4626 donation & leftover sweep
**Status:** NOT SUBMITTED — local audit finding only.

---

## 1. Description

`Swapper._swap` measures the output of a swap as the contract's **entire**
post-swap balance of the destination token, rather than the *delta* produced
by the swap:

```solidity
// contracts/swapper/Swapper.sol:92-107
function _swap(address _fromToken, address _toToken, uint256 _amountIn, uint256 _minAmountOut)
    private
    returns (uint256 amountOut)
{
    IERC20Metadata(_fromToken).safeTransferFrom(msg.sender, address(this), _amountIn);
    _executeSwap(_fromToken, _toToken, _amountIn);
    amountOut = IERC20Metadata(_toToken).balanceOf(address(this));   // <-- BUG
    if (amountOut < _minAmountOut) revert SlippageExceeded(amountOut, _minAmountOut);
    if (amountOut == 0) revert NoAmountOut();

    IERC20Metadata(_toToken).safeTransfer(msg.sender, amountOut);    // <-- drains entire balance
    if (IERC20Metadata(_fromToken).balanceOf(address(this)) > 0) {
        IERC20Metadata(_fromToken).safeTransfer(msg.sender, IERC20Metadata(_fromToken).balanceOf(address(this)));
    }
    emit Swap(msg.sender, _fromToken, _toToken, _amountIn, amountOut);
}
```

The correct pattern records `balanceBefore`, runs the swap, and returns
`balanceAfter - balanceBefore`. Using the raw `balanceOf` pulls in:

1. Any tokens that were donated / mistakenly sent to the `Swapper` address.
2. Any residual balance from a previous swap that reverted *after* the router
   call but before the sweep — possible if a malicious or non-standard router
   leaves the contract holding the input token mid-flight and a separate call
   then triggers a swap on the same `_toToken`.
3. Tokens that the protocol intentionally parks on the Swapper (e.g. fee
   accumulations) — none are documented, but the contract is upgradeable and
   admin-controlled, so future configs may do so.

Because the caller can choose `_fromToken` and `_toToken` freely (subject to
admin-set `swapInfo`), the first swapper to call after a donation (or a
leftover-creating event) walks away with the entire pre-existing balance of
`_toToken` for the cost of a 1-wei swap.

### Variant that does not require a donation

If the admin ever configures `swapInfo[X][X]` (same-token "swap", e.g. as a
no-op router for rebasing tokens), then a caller swapping `X -> X` gets back
their `_amountIn` **plus** the entire pre-existing `X` balance of the
contract. The same-token configuration is not explicitly rejected anywhere.

---

## 2. Contract, Function, and Lines

| Field | Value |
|---|---|
| Contract | `Swapper` |
| File | `contracts/swapper/Swapper.sol` |
| Function | `_swap` (private, called by both `swap` overloads) |
| Lines | 92–107 (specifically line 98) |

---

## 3. Attack Scenario

### Path A — donation / mistaken transfer
1. Some user (or the protocol itself during a migration) accidentally sends
   50,000 USDC directly to the `Swapper` proxy address via
   `IERC20(USDC).transfer(swapper, 50_000e6)`.
2. Attacker (anyone) calls `Swapper.swap(USDC, USDC, 1, 0)` — assuming the
   admin has configured `swapInfo[USDC][USDC]`, or more generally
   `Swapper.swap(WETH, USDC, 1, 0)` where the WETH→USDC route produces ~1 USDC
   of output.
3. `amountOut = IERC20(USDC).balanceOf(swapper) = 50,001 USDC`.
4. `Swapper` transfers 50,001 USDC to the attacker.
5. Attacker net: `+50,000 USDC` (minus the 1-wei input).

### Path B — leftover from a reverting downstream call
1. The swap router configured in `swapInfo[TokenA][TokenB]` is a custom
   contract that, on this specific call, transfers TokenB into the Swapper
   and then reverts in a later step (e.g. a fee-on-transfer accounting
   mismatch). The revert unwinds the *router's* state but **the
   `safeTransfer` of TokenB into the Swapper already happened inside the
   router's `_executeSwap` call** — wait, actually a revert would bubble up
   and unwind the Swapper's tx too. This path requires a router that
   partially succeeds; not all DEX routers are atomic in this sense when
   wrapped behind a custom `swapInfo.data` payload.
2. More concretely: a `swapInfo` payload that calls a multi-hop router which
   itself transfers the output to `address(this)` before reverting on a
   later hop. Because `_executeSwap` uses a low-level `router.call(data)`
   and only checks `success`, a router that returns `success=true` but has
   routed tokens through an intermediate that left dust on the Swapper will
   have its dust swept on the next call.

### Path C — `_fromToken == _toToken`
1. Admin configures `swapInfo[USDC][USDC]` with a no-op router (or any router
   that "passes through" USDC).
2. Attacker calls `swap(USDC, USDC, 1, 0)`.
3. The 1 USDC is pulled in, "swapped" (no-op), and then
   `amountOut = balanceOf(USDC) = pre-existing + 1` is sent back to the
   attacker.

---

## 4. Proof of Concept (Forge-style)

```solidity
// SPDX-License-Identifier: UNLICENSED
pragma solidity ^0.8.28;

import "forge-std/Test.sol";
import {Swapper} from "contracts/swapper/Swapper.sol";
import {IERC20} from "@openzeppelin/contracts/token/ERC20/IERC20.sol";

contract CapSwapperSweepPoC is Test {
    Swapper swapper;
    IERC20 usdc; // 6-dec
    IERC20 weth; // 18-dec
    address victim = address(0V1C);
    address attacker = address(0xB0B);

    function setUp() public {
        swapper = Swapper(payable(DEPLOYED_SWAPPER));
        usdc = IERC20(USDC);
        weth = IERC20(WETH);
        // admin has already configured swapInfo[WETH][USDC] with a Uniswap router.
    }

    function testSweepDonation() public {
        // 1. Victim mistakenly sends 50k USDC directly to the Swapper.
        deal(address(usdc), victim, 50_000e6);
        vm.startPrank(victim);
        usdc.transfer(address(swapper), 50_000e6);
        vm.stopPrank();

        // 2. Attacker swaps 1 wei of WETH -> USDC.
        deal(address(weth), attacker, 1);
        vm.startPrank(attacker);
        weth.approve(address(swapper), 1);
        uint256 out = swapper.swap(address(weth), address(usdc), 1, 0);
        vm.stopPrank();

        // 3. Attacker receives the entire 50,000 USDC + the 1-wei swap output.
        assertGt(usdc.balanceOf(attacker), 50_000e6);
        assertEq(usdc.balanceOf(address(swapper)), 0);
    }
}
```

---

## 5. Impact

- Direct loss of any token balance that ends up parked on the `Swapper`
  contract. The contract is upgradeable, admin-controlled, and intended as a
  generic swap router for the protocol's fee auction, gelato sweeper, and
  vault flows — any of those callers can mistakenly route tokens through it,
  and any external user can subsequently drain the balance.
- The attack is **permissionless** — `swap` has no access control.
- Severity is bounded by "how much value ends up on the Swapper," which in
  steady-state operation is zero (the function sweeps itself). But any
  operational mistake (wrong `swapInfo` config, misdirected
  `safeTransfer` from another Cap contract, donation-for-griefing) becomes
  immediately and fully exploitable, with no recovery path.

---

## 6. Severity: **MEDIUM**

- Not HIGH because: in normal operation the contract self-sweeps and holds no
  balance; exploitation requires a misconfiguration or external mistake to
  seed the balance first.
- Not LOW because: the drain is total (not dust), permissionless, and the
  contract is a long-lived upgradeable proxy that multiple admin flows route
  through — the likelihood of *some* balance landing on it over time is
  non-trivial.

---

## 7. Three-Perspective Audit

### 7.1 Protocol / Business-logic perspective
The Swapper is a thin wrapper around an admin-configured router payload. The
"swap output = my full balance" pattern was almost certainly copied from a
tutorial or an earlier single-purpose swapper where the contract never held
balances between calls. In the Cap context — multi-asset vault, fractional
reserve, gelato automation — the Swapper is one of several contracts that may
incidentally receive tokens, so the pattern is unsafe.

### 7.2 Mechanism / Reentrancy perspective
The function pulls `_fromToken` *before* calling the router, then reads
`_toToken` balance *after*. If the router is malicious or re-entrant, it could
donate `_toToken` to the Swapper mid-call and the `balanceOf` read would
include the donation. Because the router address is admin-configured (not
user-supplied per-call), this is a trusted-router assumption — but the
pattern still violates the principle that swap output should be measured, not
inferred from state.

### 7.3 Operational / Threat-model perspective
Cap uses this Swapper from `CapSweeper`, `CapInterestHarvester`, and the
`FractionalReserve.divest` path. Each of those callers moves real value
(USDC, wstETH, LBTC) through the Swapper. An operational mistake that sends
tokens to the Swapper outside of `_swap` (e.g. a `safeTransfer` to the wrong
address in a vault migration) is immediately irrecoverable. The fix is cheap
and the downside is permanent loss, so the cost/benefit clearly favors
measuring the delta.

---

## 8. Suggested Fix

```diff
 function _swap(address _fromToken, address _toToken, uint256 _amountIn, uint256 _minAmountOut)
     private
     returns (uint256 amountOut)
 {
+    uint256 toBalanceBefore = IERC20Metadata(_toToken).balanceOf(address(this));
     IERC20Metadata(_fromToken).safeTransferFrom(msg.sender, address(this), _amountIn);
     _executeSwap(_fromToken, _toToken, _amountIn);
-    amountOut = IERC20Metadata(_toToken).balanceOf(address(this));
+    amountOut = IERC20Metadata(_toToken).balanceOf(address(this)) - toBalanceBefore;
     if (amountOut < _minAmountOut) revert SlippageExceeded(amountOut, _minAmountOut);
     if (amountOut == 0) revert NoAmountOut();

     IERC20Metadata(_toToken).safeTransfer(msg.sender, amountOut);
     ...
 }
```

And, defensively, reject same-token swaps at the entry point:

```diff
+    if (_fromToken == _toToken) revert SameToken();
```

Additionally, consider exposing a `rescue(address token)` admin-gated function
so that mistakenly-sent tokens can be recovered without relying on a swap
call.
