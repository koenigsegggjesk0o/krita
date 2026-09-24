# Tenor Labs — FOT Loan Tokens Break Liquidation Through DelayedLiquidationGate

**Area**: Liquidation — DelayedLiquidationGate / onLiquidate
**Severity**: Low
**Status**: New (not previously submitted)

---

## Description

`DelayedLiquidationGate.onLiquidate()` pulls `repaidUnits` of the loan token
from the liquidator via `SafeTransferLib.safeTransferFrom(market.loanToken,
sender, address(this), repaidUnits)` and then approves Midnight for the same
amount via `IERC20(market.loanToken).forceApprove(address(MORPHO_MIDNIGHT),
repaidUnits)`.  Midnight subsequently pulls `repaidUnits` from the gate.

If the loan token charges a fee on transfer (FOT), the gate receives
`repaidUnits − fotFee` but has approved Midnight for `repaidUnits`.
Midnight's `safeTransferFrom(gate, midnight, repaidUnits)` then reverts
because the gate's balance is insufficient.  **Every** liquidation through
the gate fails, making all positions on the market unliquidatable.

This is distinct from the `OracleWithValidation` deviation asymmetry (the
already-known Low): it affects a completely different code path (the
liquidation callback, not the oracle) and has a different failure mode
(hard revert vs. stale price).

---

## Contract / Function / Line

| Item | Value |
|---|---|
| Contract | `DelayedLiquidationGate` |
| File | `src/gates/DelayedLiquidationGate.sol` |
| Function | `onLiquidate()` — lines 103–145 |
| Problematic lines | 139–142 (`safeTransferFrom` then `forceApprove` for `repaidUnits`) |
| Midnight pull | `lib/midnight/src/Midnight.sol` line 755 (`safeTransferFrom(loanToken, payer, address(this), repaidUnits)`) |

---

## Attack Scenario

1. A Tenor market is created with an FOT loan token (e.g., a custom token
   with a 1% transfer fee) and `DelayedLiquidationGate` as
   `liquidatorGate`.
2. A borrower becomes unhealthy.
3. After the grace period, a liquidator calls `gate.liquidate(...)` with
   `repaidUnits = 100e18`.
4. Inside `onLiquidate`:
   * `safeTransferFrom(loanToken, sender, gate, 100e18)` — gate receives
     `99e18` (1% FOT).
   * `forceApprove(Midnight, 100e18)`.
5. Midnight calls `safeTransferFrom(loanToken, gate, midnight, 100e18)` —
   reverts (gate only has `99e18`).
6. The entire liquidation reverts.  The position cannot be liquidated
   through the gate.

Since the gate is the market's `liquidatorGate`, `Midnight.liquidate`
requires `gate.canLiquidate(msg.sender)` → `msg.sender == gate`.  No other
path can liquidate the position.  The borrower's bad debt accrues
indefinitely with no liquidation possible.

---

## PoC (Foundry)

```solidity
// SPDX-License-Identifier: BUSL-1.1
pragma solidity ^0.8.24;

import {Test} from "forge-std/Test.sol";
import {DelayedLiquidationGate} from "@gates/DelayedLiquidationGate.sol";
import {IDelayedLiquidationGate} from "@gates/interfaces/IDelayedLiquidationGate.sol";
import {Midnight} from "@midnight/Midnight.sol";
import {enableDefaultLltvs} from "../helpers/LltvHelper.sol";
import {IMidnight, Market, CollateralParams} from "@midnight/interfaces/IMidnight.sol";
import {IdLib} from "@midnight/libraries/IdLib.sol";
import {MockERC20} from "../helpers/mocks/MockERC20.sol";
import {Oracle} from "../helpers/Oracle.sol";
import {LIQUIDATION_CURSOR} from "../helpers/MaxLifLib.sol";

contract FeeOnTransferToken {
    string public name; string public symbol; uint8 public decimals = 18;
    uint256 public totalSupply; uint256 public feeBps;
    mapping(address => uint256) public balanceOf;
    mapping(address => mapping(address => uint256)) public allowance;
    constructor(string memory n, string memory s, uint256 _feeBps) {
        name = n; symbol = s; feeBps = _feeBps;
    }
    function mint(address to, uint256 amount) external {
        balanceOf[to] += amount; totalSupply += amount;
    }
    function approve(address spender, uint256 amount) external returns (bool) {
        allowance[msg.sender][spender] = amount; return true;
    }
    function transfer(address to, uint256 amount) external returns (bool) {
        uint256 fee = amount * feeBps / 10000;
        balanceOf[msg.sender] -= amount;
        balanceOf[to] += amount - fee;
        return true;
    }
    function transferFrom(address from, address to, uint256 amount) external returns (bool) {
        uint256 fee = amount * feeBps / 10000;
        allowance[from][msg.sender] -= amount;
        balanceOf[from] -= amount;
        balanceOf[to] += amount - fee;
        return true;
    }
}

contract FOTLiquidationPoC is Test {
    Midnight midnight;
    DelayedLiquidationGate gate;
    FeeOnTransferToken fotLoanToken;
    MockERC20 collateralToken;
    Oracle oracle;
    address liquidator = makeAddr("liquidator");
    address borrower = makeAddr("borrower");
    Market market;
    bytes32 marketId;
    uint256 constant GRACE_PERIOD = 1 hours;
    uint256 constant LIQUIDATION_PERIOD = 2 hours;
    uint256 constant LLTV = 0.77e18;

    function setUp() public {
        midnight = new Midnight();
        enableDefaultLltvs(midnight);
        midnight.setFeeClaimer(address(this));
        fotLoanToken = new FeeOnTransferToken("FOT", "FOT", 100); // 1% fee
        collateralToken = new MockERC20("COL", "COL", 18);
        oracle = new Oracle();
        oracle.setPrice(1e36);
        gate = new DelayedLiquidationGate(address(midnight), GRACE_PERIOD, LIQUIDATION_PERIOD, 1 minutes);
        CollateralParams[] memory c = new CollateralParams[](1);
        c[0] = CollateralParams({
            token: address(collateralToken), lltv: LLTV,
            liquidationCursor: LIQUIDATION_CURSOR, oracle: address(oracle)
        });
        market = Market({
            chainId: block.chainid, midnight: address(midnight),
            loanToken: address(fotLoanToken), collateralParams: c,
            maturity: block.timestamp + 30 days, rcfThreshold: 0,
            enterGate: address(0), liquidatorGate: address(gate)
        });
        midnight.touchMarket(market);
        marketId = IdLib.toId(market);
    }

    function test_fot_breaks_liquidation() public {
        uint256 repaidUnits = 100e18;
        fotLoanToken.mint(liquidator, 200e18);
        vm.prank(liquidator);
        fotLoanToken.approve(address(gate), type(uint256).max);

        // Simulate the gate's onLiquidate pull:
        vm.prank(address(gate));
        fotLoanToken.transferFrom(liquidator, address(gate), repaidUnits);

        uint256 gateBal = fotLoanToken.balanceOf(address(gate));
        assertEq(gateBal, 99e18, "gate received less due to FOT");

        // Midnight tries to pull repaidUnits from gate — reverts.
        vm.prank(address(gate));
        fotLoanToken.approve(address(midnight), repaidUnits);

        vm.expectRevert();
        vm.prank(address(midnight));
        fotLoanToken.transferFrom(address(gate), address(midnight), repaidUnits);
    }
}
```

---

## Impact

* Markets with FOT loan tokens and `DelayedLiquidationGate` become
  **completely unliquidatable** — every liquidation attempt reverts at
  Midnight's final token pull.
* Bad debt accrues indefinitely, socializing ever-larger losses to lenders.
* The issue is silent at market creation time (no FOT check in the gate or
  factory).

**Mitigating factor**: Standard Morpho Blue / Midnight markets use
non-FOT tokens (USDC, WETH, DAI, etc.), so this only affects markets that
deliberately choose an FOT loan token.  Tenor's `OracleWithValidation` and
gate factories do not enforce non-FOT, but the practical risk is low.

---

## Severity Justification

**Low.**  The issue only manifests with FOT loan tokens, which are uncommon
in Morpho-ecosystem markets.  No standard deployment is affected.  But the
gate's `onLiquidate` does not guard against FOT, and a market that
inadvertently uses an FOT token would have all liquidations broken with no
on-chain warning.

---

## Three-Perspective Audit

### 1. Attacker perspective
No active exploitation is needed — the bug manifests whenever any
liquidator attempts to liquidate on an FOT-token market.  An attacker could
grief lenders by creating a market with an FOT token (if they have market
creation privileges), but market creation is typically gated.

### 2. Protocol perspective
The gate should either (a) document that FOT loan tokens are unsupported
(or (b) measure the actual received amount and adjust the approval
accordingly: `uint256 received = balanceAfter − balanceBefore; forceApprove(
Midnight, received)`).  Option (b) would also handle rebasing tokens that
might reduce the received amount.

### 3. Auditor perspective
The root cause is the assumption that `safeTransferFrom(sender, gate,
repaidUnits)` delivers exactly `repaidUnits` to the gate.  This assumption
is also present in several callback contracts (`BorrowMidnightRenewalCallback`,
`BorrowBlueToMidnightCallback`, etc.) that approve `repayBudget` after
receiving `sellerAssets`.  Those callbacks receive tokens from Midnight's
take (which uses `safeTransferFrom`), so FOT would also cause shortfalls
there.  The gate is the most critical instance because it blocks all
liquidations.
