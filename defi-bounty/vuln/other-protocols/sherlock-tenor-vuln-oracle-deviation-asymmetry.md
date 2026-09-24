# Tenor Labs — `OracleWithValidation` Deviation Check Is Asymmetric, Allowing Primary Overpricing

**Protocol:** Tenor Labs
**Bounty:** $200,000 USDC
**Sherlock Bounty URL:** https://audits.sherlock.xyz/bug-bounties/407
**Source:** https://github.com/tenor-labs/tenor-contracts
**Severity:** LOW (informational / defense-in-depth)
**Area:** Oracle manipulation / Integer precision
**Status:** NOT SUBMITTED — local audit finding only.

---

## 1. Description

`OracleWithValidation.price()` compares the primary oracle's price
against a validation oracle and reverts if the deviation exceeds
`MAX_ORACLE_DEVIATION`. The deviation is computed as
`|primaryPrice - validationPrice|` and compared against
`primaryPrice * MAX_ORACLE_DEVIATION / 1e18`. Because the deviation is
scaled by the **primary** price (not the validation price, and not an
average), the allowed overpricing relative to the validation oracle is
asymmetric: the primary can be overpriced by up to `d / (1 - d)` relative
to the validation, but can only be underpriced by `d`.

```solidity
// contracts/src/oracles/OracleWithValidation.sol:64-79
function price() external view returns (uint256) {
    uint256 primaryPrice = PRIMARY_ORACLE.price();
    if (validationCheckPaused) return primaryPrice;

    try VALIDATION_ORACLE.price() returns (uint256 validationPrice) {
        uint256 absoluteDeviation =
            primaryPrice > validationPrice ? primaryPrice - validationPrice : validationPrice - primaryPrice;
        uint256 maxAllowedDeviation = primaryPrice.mulDivDown(MAX_ORACLE_DEVIATION, 1e18);

        if (absoluteDeviation > maxAllowedDeviation) revert ExcessiveOracleDeviation();
    } catch {
        if (REVERT_ON_VALIDATION_ORACLE_FAILURE) revert ValidationOracleFailure();
    }

    return primaryPrice;
}
```

The contract's own NatSpec acknowledges this:

> The deviation is scaled by the primary price, so a threshold d allows
> up to d / (1 - d) overpricing relative to the validation oracle
> (e.g. 5% configured allows ~5.26% effective).

So this is a **known, documented** asymmetry rather than an undocumented
bug. The reason it is still worth recording as an audit finding:

1. **`MAX_ORACLE_DEVIATION` is `immutable`** — set at construction, never
   adjustable without redeploying. A 5% configured deviation becomes a
   permanent 5.26% overpricing cap for the life of the market.
2. **The asymmetry direction favors the borrower.** If the primary
   oracle is manipulated *upward* (the most common flash-loan attack
   direction, since inflating collateral price lets a borrower extract
   more), the allowed overpricing is `d / (1 - d) > d`. If the primary
   is manipulated *downward* (which would harm the borrower, not help
   them), the allowed underpricing is only `d`. So the side of the
   deviation that an attacker would naturally target gets the looser
   bound.
3. **The validation oracle returning 0 is not caught by `try/catch`.**
   If `VALIDATION_ORACLE.price()` returns `0` (rather than reverting),
   `absoluteDeviation = primaryPrice` and `maxAllowedDeviation =
   primaryPrice * d`. The check `primaryPrice > primaryPrice * d`
   passes for any `d < 1`, so `price()` reverts with
   `ExcessiveOracleDeviation` even when
   `REVERT_ON_VALIDATION_ORACLE_FAILURE == false`. The contract's NatSpec
   acknowledges this too, but it means a validation oracle that
   silently degrades to returning 0 (e.g. a Chainlink feed that returns
   0 on error instead of reverting) takes the whole market down rather
   than falling back to primary-only mode.

---

## 2. Contract, Function, and Lines

| Field | Value |
|---|---|
| Contract | `OracleWithValidation` |
| File | `src/oracles/OracleWithValidation.sol` |
| Function | `price` |
| Lines | 64–79 (deviation check on lines 69–73) |
| Constants | `MAX_ORACLE_DEVIATION` (immutable, line 35), `REVERT_ON_VALIDATION_ORACLE_FAILURE` (immutable, line 39) |

---

## 3. Attack Scenario

### Asymmetric overpricing

1. Tenor deploys an `OracleWithValidation` for an LBTC market with
   `MAX_ORACLE_DEVIATION = 5%` (0.05e18).
2. The primary oracle is a Uniswap V3 TWAP that an attacker can
   manipulate within a single block via a large flash-loaned swap
   (typical for low-liquidity V3 pools with short TWAP windows).
3. The validation oracle is a Chainlink feed showing LBTC = $50,000.
4. The attacker manipulates the primary upward to $52,631.58 (a 5.26%
   increase, which is `d / (1 - d)` for `d = 0.05`).
5. `absoluteDeviation = 52631.58 - 50000 = 2631.58`.
6. `maxAllowedDeviation = 52631.58 * 0.05 = 2631.58`.
7. `2631.58 > 2631.58` is false, so the check passes. The protocol uses
   $52,631.58 as the LBTC price.
8. The attacker borrows against LBTC at the inflated price, extracting
   ~5.26% more than they should be able to. They repay after the
   manipulation unwinds.

The same manipulation *downward* (primary = $47,500, a 5% decrease)
would also pass — but underpricing collateral harms the borrower, so an
attacker would not target it. The asymmetry means the *dangerous*
direction gets the looser bound.

### Validation-oracle-returns-0 blackout

1. The validation oracle (e.g. a Redstone feed) has a bug where it
   returns `0` instead of reverting on stale data.
2. `REVERT_ON_VALIDATION_ORACLE_FAILURE` is `false` (the "resilient"
   configuration).
3. `price()` is called. `validationPrice = 0`. `absoluteDeviation =
   primaryPrice`. `maxAllowedDeviation = primaryPrice * 0.05`.
   `primaryPrice > primaryPrice * 0.05` → revert
   `ExcessiveOracleDeviation`.
4. Every Tenor market using this oracle is now frozen for borrows,
   liquidations, and migrations — even though the operator intended
   `REVERT_ON_VALIDATION_ORACLE_FAILURE = false` to mean "fall back to
   primary if validation breaks."

---

## 4. Proof of Concept (Forge-style)

```solidity
// SPDX-License-Identifier: UNLICENSED
pragma solidity 0.8.34;

import "forge-std/Test.sol";
import {OracleWithValidation} from "src/oracles/OracleWithValidation.sol";

contract MockOracle {
    uint256 public price;
    constructor(uint256 p) { price = p; }
    function setPrice(uint256 p) public { price = p; }
}

contract TenorOracleDeviationPoC is Test {
    OracleWithValidation o;
    MockOracle primary;
    MockOracle validation;

    function setUp() public {
        primary = new MockOracle(50_000e18);
        validation = new MockOracle(50_000e18);
        // 5% deviation, do NOT revert on validation failure
        o = new OracleWithValidation(primary, validation, 0.05e18, false, address(this));
    }

    function testAsymmetricOverpricing() public {
        // Primary up to 52631.58 (5.26% over validation) — passes.
        primary.setPrice(52_631.58e18);
        assertEq(o.price(), 52_631.58e18); // no revert

        // Primary up to 52631.59 — reverts.
        primary.setPrice(52_631.59e18);
        vm.expectRevert(OracleWithValidation.ExcessiveOracleDeviation.selector);
        o.price();
    }

    function testValidationZeroBlackout() public {
        validation.setPrice(0);
        // REVERT_ON_VALIDATION_ORACLE_FAILURE == false, but still reverts:
        vm.expectRevert(OracleWithValidation.ExcessiveOracleDeviation.selector);
        o.price();
    }
}
```

---

## 5. Impact

- **Asymmetric overpricing:** a manipulated primary oracle can overprice
  collateral by up to `d / (1 - d)` (e.g. 5.26% for a 5% threshold)
  without triggering the deviation revert. The extra borrowing capacity
  is extractable by an attacker who can flash-loan-manipulate the
  primary within a single block.
- **Validation-zero blackout:** a validation oracle that silently
  returns 0 freezes all markets using `OracleWithValidation`, even
  when the operator has configured `REVERT_ON_VALIDATION_ORACLE_FAILURE
  = false` precisely to avoid that outcome.

Both issues are bounded by the immutable `MAX_ORACLE_DEVIATION` and by
the validation oracle's behavior, and the contract's NatSpec documents
them. The practical risk depends on the primary oracle's manipulability
(Tenor's docs note that the primary should be manipulation-resistant,
e.g. Chainlink) and on the validation oracle's failure mode.

---

## 6. Severity: **LOW**

- Not MEDIUM because: the asymmetry is documented and the magnitude
  (`d / (1 - d) - d = d² / (1 - d)`, which is 0.26% for `d = 5%`) is
  small. The validation-zero blackout requires a misbehaving validation
  oracle, which is an operational failure mode rather than a direct
  exploit.
- Worth reporting because: (a) the asymmetry direction favors the
  attacker, (b) the validation-zero behavior contradicts the documented
  intent of `REVERT_ON_VALIDATION_ORACLE_FAILURE = false`, and (c)
  `MAX_ORACLE_DEVIATION` is immutable so misconfigurations are
  permanent.

---

## 7. Three-Perspective Audit

### 7.1 Protocol / Business-logic perspective
The deviation check exists to prevent the primary oracle from being
manipulated beyond a threshold. Scaling by the primary price is a
natural choice (the primary is the "live" price), but it creates the
asymmetry. Scaling by `max(primary, validation)` or by the validation
price would eliminate the overpricing slack at the cost of slightly
tighter underpricing bounds.

### 7.2 Oracle / Data-integrity perspective
Redstone and other non-Chainlink oracles sometimes return `0` on error
instead of reverting. The `try/catch` pattern only catches reverts, not
zero returns. A `require(validationPrice > 0, ...)` before the deviation
check would make the failure mode explicit and would allow the
`REVERT_ON_VALIDATION_ORACLE_FAILURE = false` path to actually fall
through to primary-only mode (by reverting in the `try` block and being
caught by `catch`).

Actually — wait. If we add `require(validationPrice > 0)` *inside* the
`try` block, it would revert, and the `catch` block would execute
(returning the primary price if
`REVERT_ON_VALIDATION_ORACLE_FAILURE == false`). That would correctly
implement the documented fallback behavior. Currently, the
`validationPrice == 0` case is **not** caught by `try/catch` (no
revert) and instead trips `ExcessiveOracleDeviation`, which is a
hard revert that bypasses the fallback entirely.

### 7.3 Operational / Threat-model perspective
Tenor's `OracleWithValidation` is deployed per-market with immutable
config. An operator who chooses `MAX_ORACLE_DEVIATION = 5%` and
`REVERT_ON_VALIDATION_ORACLE_FAILURE = false` is signing up for "allow
up to 5% deviation, and if validation breaks, fall back to primary."
They are *not* signing up for "allow up to 5.26% overpricing, and if
validation returns 0, freeze the market." The current behavior
contradicts the operator's mental model in both respects.

---

## 8. Suggested Fix

### Fix 1 — Symmetric deviation

```diff
-        uint256 absoluteDeviation =
-            primaryPrice > validationPrice ? primaryPrice - validationPrice : validationPrice - primaryPrice;
-        uint256 maxAllowedDeviation = primaryPrice.mulDivDown(MAX_ORACLE_DEVIATION, 1e18);
+        uint256 base = primaryPrice > validationPrice ? primaryPrice : validationPrice;
+        uint256 absoluteDeviation =
+            primaryPrice > validationPrice ? primaryPrice - validationPrice : validationPrice - primaryPrice;
+        uint256 maxAllowedDeviation = base.mulDivDown(MAX_ORACLE_DEVIATION, 1e18);
```

Or, more conservatively, scale by `validationPrice` so the validation
oracle is the "ground truth" and the primary must track it within
`d`:

```diff
-        uint256 maxAllowedDeviation = primaryPrice.mulDivDown(MAX_ORACLE_DEVIATION, 1e18);
+        uint256 maxAllowedDeviation = validationPrice.mulDivDown(MAX_ORACLE_DEVIATION, 1e18);
```

### Fix 2 — Treat validation `0` as a failure (so `try/catch` handles it)

```diff
     try VALIDATION_ORACLE.price() returns (uint256 validationPrice) {
+        if (validationPrice == 0) {
+            // Trigger the catch branch by reverting here.
+            revert ValidationOracleFailure();
+        }
         uint256 absoluteDeviation =
             primaryPrice > validationPrice ? primaryPrice - validationPrice : validationPrice - primaryPrice;
```

This makes `REVERT_ON_VALIDATION_ORACLE_FAILURE = false` actually mean
"fall back to primary on any validation failure, including silent
zero returns."
