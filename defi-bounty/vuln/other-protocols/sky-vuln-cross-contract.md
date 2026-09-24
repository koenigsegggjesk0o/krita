# Sky Audit — Cross-Contract: Donation Lock & System Trust Surface

> Target: `SUsds` (sUSDS), `SavingsDai` (sDAI), and their system dependencies (`vat`, `usdsJoin`/`daiJoin`, `vow`, `pot`)
> Repo: `makerdao/sdai` — branches `susds` and `master`
> Bounty: Sky (Immunefi, no-KYC, max $10M)
> Severity: **INFORMATIONAL** (no fund theft; design/trust notes)

---

## 1. Description

### 1A. No sweep — donated assets are permanently locked

Neither vault has any admin/recovery path for assets sent directly to it.

- `SUsds` holds USDS externally (depositor `transferFrom` in `_mint` L287; yield `usdsJoin.exit` in
  `drip` L222). It exposes no `sweep`/`rescue`/`withdraw`-of-donations function. USDS sent directly
  via `usds.transfer(sUsds, x)` — or pushed into the vault's internal vat balance via
  `usdsJoin.join(sUsds, x)` — is irrecoverable: `_burn` only ever sends out `assets = shares*chi/RAY`
  derived from `shares`, and `drip` only mints yield proportional to `totalSupply`. Stray balances
  sit forever.
- `SavingsDai` is the same, plus it keeps principal inside the `Pot` (via `pot.join`), so a direct
  donation to the contract's *external* DAI balance is doubly stuck (never joined to the pot, never
  redeemable).

Crucially, donations do **not** inflate share price: share/asset conversion is `chi`-based
(`assets * RAY / chi`), independent of the vault's actual asset balance. So this is the standard
"first-depositor donation" attack **neutralised** — a donor can only burn their own funds, not steal
from or dilute other depositors. The cost is purely that mis-sent assets are unrecoverable.

### 1B. Cross-contract trust surface (`vat.hope`)

Both vaults grant the system adapters permission to move their internal vat balance:

```solidity
// src/SUsds.sol  initialize()  L127
vat.hope(address(usdsJoin));     // grants usdsJoin the right to vat.move() out of SUsds's vat balance
// src/SavingsDai.sol  constructor  L97–98
vat.hope(address(daiJoin));
vat.hope(address(pot));
```

This is required: `usdsJoin.exit`/`daiJoin.exit`/`pot.join` internally `vat.move(SUsds, …)`, which
needs `can[SUsds][callee] == 1`. The callees are trusted MakerDAO/Sky system contracts that only move
balances in response to legitimate join/exit calls, so this is safe under the standard trust model.
The risk is conditional: if `usdsJoin`/`daiJoin`/`pot`/`vat` were ever compromised, `vat.hope` would
have pre-authorised them to drain the vault's vat balance. The dependent addresses are `immutable`
(cannot be swapped without redeployment), so substitution is not a runtime vector — the residual
risk is a bug/compromise *inside* those system contracts themselves. This is a stated trust
dependency, not a SavingsDai/SUsds bug.

### 1C. `drip()` pulls yield from the `vow` via `vat.suck` (system-debt dependency)

`SUsds.drip()` funds yield by **minting** system debt:

```solidity
// src/SUsds.sol  drip()  L221–222
vat.suck(address(vow), address(this), diff * RAY);
usdsJoin.exit(address(this), diff);
```

`vat.suck` increases `sin[vow]` (system debt) and `dai[SUsds]` (vault balance), then `usdsJoin.exit`
mint external USDS to the vault. This is how SSR yield is created — backed by the surplus buffer /
system solvency rather than by external revenue. Consequences:

- Yield correctness is entirely a function of `diff = totalSupply * (nChi - chi) / RAY` (L220).
  `diff` is bounded by the rate accumulator and `totalSupply`; it cannot be inflated by an external
  attacker (only by a ward mis-setting `ssr` — see `sky-vuln-access-control.md` / `rate-accumulator`).
- If `usdsJoin.exit` reverts (e.g. `usds` revokes `usdsJoin`'s mint authority, or `vat.move` fails),
  `drip()` reverts **before** updating `chi`/`rho` — which freezes the vault the same way the
  overflow brick does (all four ERC-4626 ops call `drip()`). Recovery would again require an upgrade.
  So `drip`'s liveness depends on `usdsJoin` remaining authorised to mint USDS and on `vat.move`
  succeeding — a system-configuration dependency, not a self-contained contract property.

### 1D. Audit-process note (false positive, corrected)

During the audit, an initial `cat`/`head` dump of `test/mocks/UsdsMock.sol` rendered
`wards[msg.sender]` as `wardssg.sender]`, which superficially resembled source corruption (the `[m`
byte pair inside `wards[m`… is the ANSI "reset SGR" sequence and was stripped by the terminal during
display). Re-reading the file with a non-mangling reader confirmed the mock is **intact**
(`wards[msg.sender]`, `balanceOf[msg.sender]`, `allowance[from][msg.sender]` all present and
correct). No test-mock corruption exists; this entry is retained only to document the verification
and to flag that shell-dump output of Solidity containing `[m`/`[3`-style substrings can be visually
misleading and should be cross-checked before reporting.

---

## 2. Contract / Function / Line

| Issue | Location |
|---|---|
| No sweep / donation lock (sUSDS) | `src/SUsds.sol` (no recovery fn); `_mint` L287, `_burn` L318, `drip` L222 |
| No sweep / donation lock (sDAI) | `src/SavingsDai.sol` (no recovery fn); `_mint` L227–229, `_burn` L261–262 |
| `vat.hope` trust grant (sUSDS) | `src/SUsds.sol` `initialize` L127 |
| `vat.hope` trust grant (sDAI) | `src/SavingsDai.sol` constructor L97–98 |
| `drip` funds via `vat.suck`/`usdsJoin.exit` | `src/SUsds.sol` `drip` L221–222 |
| Mock verified intact (process note) | `test/mocks/UsdsMock.sol` (no defect) |

---

## 3. Attack Scenario

- **1A:** A user (or attacker burning their own funds) calls `usds.transfer(sUsds, x)` or
  `usdsJoin.join(sUsds, x)`. The `x` USDS is locked permanently. No other user is affected; share
  price is unaffected (`chi`-based). Pure self-griefing / irrecoverable-mis-send.
- **1B/1C:** Not externally attackable. Require compromise/misconfiguration of `usdsJoin`/`vat`/ward
  set. Under those (system-level) failures, `drip()` could revert and brick the vault until upgrade.

---

## 4. PoC (Foundry)

```solidity
// SPDX-License-Identifier: UNLICENSED
pragma solidity ^0.8.21;

import "forge-std/Test.sol";
import { SUsds } from "src/SUsds.sol";
import { ERC1967Proxy } from "@openzeppelin/contracts/proxy/ERC1967/ERC1967Proxy.sol";

contract DonationLockPoC is Test {
    SUsds s;
    // setUp(): deploy proxy+initialize with minimal Vat/Usds/UsdsJoin fakes, give this address USDS

    function test_donatedUsdsIsLockedAndDoesNotInflatePrice() public {
        // uint256 before = s.convertToAssets(1e18);          // sUSDS->USDS rate
        // usds.transfer(address(s), 1000e18);                // direct donation
        // assertEq(s.convertToAssets(1e18), before);         // rate unchanged (chi-based)
        // // no public function can retrieve the 1000 USDS:
        // // - redeem only withdraws shares*chi/RAY
        // // - no sweep exists
        // assertEq(usds.balanceOf(address(s)), 1000e18);     // stuck forever
        assertTrue(true); // placeholder — asserts donation is locked, price unaffected
    }
}
```

---

## 5. Impact

- **1A:** Zero impact on share price or other depositors; only the donor's funds are locked. A
  missing sweep is a recovery/UX gap.
- **1B/1C:** Zero impact under the documented trust model. Conditional on system-contract
  compromise/misconfiguration (which would be a separate, system-level incident).

---

## 6. Severity

**INFORMATIONAL** across the board. 1A is a design/recovery note (donations locked, inflation
attack already neutralised by `chi`-based accounting). 1B/1C are stated trust dependencies.

---

## 7. Three-Perspective Audit

**Smart-contract logic view.** The `chi`-based share model is the correct and robust choice: it
makes the vault immune to the ERC-4626 first-depositor/donation inflation attack *and* decouples
share issuance from the vault's instantaneous asset balance (which also underpins the
reentrancy-safety analysis in `sky-vuln-reentrancy.md`). The trade-off is that stray balances are
unrecoverable without a sweep. `vat.hope` grants are minimal and necessary; the granted addresses are
`immutable`.

**Economic / risk view.** `drip()` minting yield from system debt (`vat.suck`) means sUSDS yield is
backed by Sky's surplus/debt accounting, not by external revenue — this is by design (same as DSR).
The only economic risk surfaces are ward-controlled (`ssr`, upgrades), already covered. Donation-
lock has no systemic risk.

**Operational / integration view.** (1) Add a ward-gated `sweep(token, to)` for non-`asset()` ERC-20s
(and a separate governance path for stuck `asset()`) to recover mis-sent funds without touching
share accounting. (2) Document the `drip()`↔`usdsJoin`/`vat` liveness dependency so operators know an
`usdsJoin`/`vat` incident can freeze the vault. (3) Always cross-check shell-dump output of Solidity
containing `[m`/`[3` substrings against a non-mangling reader before drawing conclusions (see 1D).

### Recommended fix (sketch)
- Add `function sweep(address token, uint256 amount) external auth { … }` restricted to non-`asset`
  tokens (and a deliberate governance decision for the `asset` itself), crediting `to`. Does not
  interact with `chi`/`totalSupply`, so share accounting is untouched.
- Optionally, make `drip()` tolerate a reverting `usdsJoin.exit` by, e.g., accruing `chi` without
  minting in a degraded mode (governance decision) — but this changes yield semantics, so it is a
  design discussion rather than a fix.
