# Sky Audit — Rate Accumulator Liveness Brick & Governance Recovery Deadlock

> Target: `SUsds` (Savings USDS / sUSDS — the Sky "SavingsSky" yield vault)
> Repo: `makerdao/sdai`, branch `susds`, file `src/SUsds.sol`
> Bounty: Sky (Immunefi, no-KYC, max $10M)
> Severity: **LOW** (governance-misconfiguration-triggered liveness DoS; no direct fund theft; recoverable only via UUPS upgrade)

---

## 1. Description

`SUsds` self-manages its own rate accumulator (`chi`, `rho`, `ssr`) instead of delegating to the
legacy `Pot`. The `drip()` function compounds `chi` forward and mints the accrued yield via
`vat.suck` + `usdsJoin.exit`:

```solidity
// src/SUsds.sol
function drip() public returns (uint256 nChi) {                       // L214
    (uint256 chi_, uint256 rho_) = (chi, rho);
    uint256 diff;
    if (block.timestamp > rho_) {
        nChi = _rpow(ssr, block.timestamp - rho_) * chi_ / RAY;        // L218  CHECKED mul → reverts on overflow
        uint256 totalSupply_ = totalSupply;
        diff = totalSupply_ * nChi / RAY - totalSupply_ * chi_ / RAY;   // L220
        vat.suck(address(vow), address(this), diff * RAY);             // L221
        usdsJoin.exit(address(this), diff);                            // L222
        chi = uint192(nChi);                                           // L223
        rho = uint64(block.timestamp);                                 // L224
    } else { nChi = chi_; }
    emit Drip(nChi, diff);
}
```

The multiplication on **L218** is **checked** (Solidity 0.8 default — no `unchecked` block).
`_rpow(ssr, dt)` grows exponentially in `dt`; once `_rpow(ssr, dt) * chi_` exceeds `type(uint256).max`,
`drip()` reverts.

Because `deposit` (L352), `mint` (L371), `withdraw` (L390) and `redeem` (L403) **all** compute their
share/asset amounts as `... drip() ...`, a reverting `drip()` freezes **every** user operation:
no deposits, no mints, no withdrawals, no redeems. sUSDS holds billions of USD value, so this is a
full liveness outage of the savings product.

The same checked multiplication also appears in the public view helpers
`convertToShares` (L335), `convertToAssets` (L340), `previewMint` (L367), `previewWithdraw` (L386),
`totalAssets` (L331), `maxWithdraw` (L382). They revert identically — see
`sky-vuln-integer-precision.md` for the oracle-degradation angle.

### Why governance cannot simply lower `ssr` to recover

`file("ssr", data)` is gated by:

```solidity
function file(bytes32 what, uint256 data) external auth {              // L203
    if (what == "ssr") {
        require(data >= RAY, "SUsds/wrong-ssr-value");                 // L205  NOTE: no upper bound
        require(rho == block.timestamp, "SUsds/chi-not-up-to-date");   // L206  requires drip() to be current
        ssr = data;
    } ...
}
```

`rho` is only ever advanced to `block.timestamp` inside `drip()` (L224). If `drip()` reverts, `rho`
is frozen at a past timestamp, so `rho == block.timestamp` is **permanently false** and `file("ssr", …)`
always reverts with `SUsds/chi-not-up-to-date`. Governance is therefore **locked out of the only
non-upgrade remediation path**. The vault can only be unblocked by a `ward` calling
`upgradeTo`/`upgradeToAndCall` (UUPS, `_authorizeUpgrade` is `auth`-gated, L132) to ship a new
implementation that, e.g., clamps `dt`, bounds `ssr`, or skips the overflowing compounding.

### Enabling condition: `file` has no upper bound on `ssr`

The deployment script *does* bound the rate at init:

```solidity
// deploy/SUsdsInit.sol
require(cfg.ssr >= RAY && cfg.ssr <= RATES_ONE_HUNDRED_PCT, "SUsdsInit/ssr-out-of-boundaries");
```

where `RATES_ONE_HUNDRED_PCT = 1000000021979553151239153027` (~100% APR, per-second compounded).
**The contract-level `file("ssr")` does not replicate this cap** — it only enforces `data >= RAY`
(L205). A ward can therefore set `ssr` to any value `>= RAY` at any time after deployment, with no
sanity ceiling. A fat-fingered governance call (e.g. one extra zero, or a per-second rate mistakenly
entered as an APR) is sufficient to brick the vault within seconds.

---

## 2. Contract / Function / Line

| Item | Location |
|---|---|
| `drip()` checked-mul overflow | `src/SUsds.sol` `drip()` **L218** |
| `deposit/mint/withdraw/redeem` hard-depend on `drip()` | L353, L372, L391, L404 |
| `file("ssr")` requires stale `rho` (recovery deadlock) | `src/SUsds.sol` `file()` **L206** |
| `file("ssr")` has no upper bound | `src/SUsds.sol` `file()` **L205** |
| `_rpow` (exponential growth source) | `src/SUsds.sol` `_rpow()` **L160** |

---

## 3. Attack Scenario

1. (Precondition) A `ward` (governance multisig / Sky governance exec) calls `drip()` then
   `file("ssr", 10**40)` — a value `>= RAY` that passes the only contract-level check. Realistic
   vectors: a decimal-place mistake in a governance spell, an SSR-APR value entered where a
   per-second RAY is expected, or a compromised/misconfigured ward key. There is **no on-chain
   ceiling** to reject it.
2. Two seconds elapse (no keeper needed; `drip` is permissionless but reverting now).
3. Any user calls `deposit`/`redeem`/etc. → internally calls `drip()` →
   `_rpow(1e40, 2) * chi_` = `~1e53 * 1e27` = `~1e80` > `type(uint256).max` → revert.
4. **All** savings-vault operations are now blocked for every user.
5. Governance attempts `file("ssr", RAY)` to restore a sane rate → reverts (`rho != block.timestamp`).
6. The only recovery is a UUPS `upgradeTo(newImpl)` by a ward, requiring off-chain multisig
   coordination, code review, and deployment — during which user funds remain locked.

Even without a governance error, the *same* revert is reachable (in principle) if `drip()` is never
called for an extremely long interval relative to `ssr`; at the live ~5–6% SSR the timescale is
millennia, so the realistic trigger is the `ssr` misconfiguration above.

---

## 4. PoC (Foundry)

Self-contained test using minimal inline mocks so it compiles against the cloned repo without
relying on the (separately corrupted — see `sky-vuln-cross-contract.md`) `UsdsMock`.

```solidity
// SPDX-License-Identifier: UNLICENSED
pragma solidity ^0.8.21;

import "forge-std/Test.sol";
import { SUsds } from "src/SUsds.sol";
import { ERC1967Proxy } from "@openzeppelin/contracts/proxy/ERC1967/ERC1967Proxy.sol";

interface VatLike { function hope(address) external; function suck(address,address,uint256) external; }
interface UsdsLike { function transfer(address,uint256) external; function transferFrom(address,address,uint256) external; function mint(address,uint256) external; }
interface UsdsJoinLike { function vat() external view returns (address); function usds() external view returns (address); function exit(address,uint256) external; }

// Minimal fakes ----------------------------------------------------------
contract Vat is VatLike {
    function hope(address) external {}
    function suck(address,address,uint256) external {} // creates dai; always succeeds
}
contract Usds is UsdsLike {
    mapping(address=>uint256) public balanceOf;
    function mint(address to,uint256 v) external { balanceOf[to]+=v; }
    function transfer(address to,uint256 v) external { balanceOf[msg.sender]-=v; balanceOf[to]+=v; }
    function transferFrom(address f,address t,uint256 v) external { balanceOf[f]-=v; balanceOf[t]+=v; }
}
contract UsdsJoin is UsdsJoinLike {
    Vat public immutable vat; Usds public immutable usds;
    constructor(Vat v, Usds u){ vat=v; usds=u; }
    function exit(address usr,uint256 wad) external { usds.mint(usr, wad); }
}

contract RateBrickPoC is Test {
    Vat vat; Usds usds; UsdsJoin join; SUsds s; address ward = address(0xbeef);

    function setUp() public {
        vat = new Vat(); usds = new Usds(); join = new UsdsJoin(vat, usds);
        SUsds imp = new SUsds(address(join), address(0xc0de));
        s = SUsds(address(new ERC1967Proxy(address(imp), abi.encodeCall(SUsds.initialize, ()))));
        // make `ward` the sole admin
        vm.prank(address(this)); // initial ward from initialize()
        s.rely(ward);
        vm.prank(address(this)); s.deny(address(this));
    }

    function testBrick() public {
        // governance sets an absurd (but contract-legal) per-second ssr
        vm.prank(ward); s.file("ssr", 10**40);

        vm.warp(block.timestamp + 2);

        // 1) drip() reverts on checked-mul overflow
        vm.expectRevert(); s.drip();

        // 2) every user op hard-depends on drip() -> all frozen
        vm.expectRevert(); s.deposit(1e18, address(this));
        vm.expectRevert(); s.redeem(0, address(this), address(this));

        // 3) recovery deadlock: cannot lower ssr because rho is stale
        vm.prank(ward); vm.expectRevert(); s.file("ssr", 1e27);

        // 4) only a UUPS upgrade (ward-gated) can recover
        // SUsds newImp = new SUsds(address(join), address(0xc0de));
        // vm.prank(ward); s.upgradeTo(address(newImp));
    }
}
```

Expected: `drip()`, `deposit`, `redeem`, and the recovery `file("ssr", 1e27)` all revert; funds
locked until an upgrade.

---

## 5. Impact

- **Fund safety:** No direct theft. Principal + accrued yield remain in the contract and are
  recoverable after a governance upgrade. Rounding integrity is not violated.
- **Liveness:** Total outage of the sUSDS savings product (deposit/mint/withdraw/redeem) for all
  users until a ward ships and authorizes a UUPS upgrade. For a multi-billion-USD TVL vault this is
  a material availability event and breaks all downstream integrations that route through these
  functions.
- **Recovery:** Requires off-chain governance coordination (multisig + spell + review). No
  non-upgrade remediation exists because `file("ssr")` is deadlocked by the stale `rho`.

---

## 6. Severity

**LOW.** Reachable only via a `ward` action (governance misconfiguration) or an unrealistic
multi-millennia drip gap; no attacker-controllable external trigger; no fund loss; recoverable via
upgrade. Elevated to "noteworthy" because (a) the blast radius is the entire savings product and
(b) the `file("ssr")` ↔ `drip()` coupling creates a genuine remediation deadlock with no
non-upgrade escape hatch. If the ward set were ever compromised, this same primitive lets an
attacker cheaply hold the vault hostage (extortion surface), which is the main reason it is more
than purely informational.

---

## 7. Three-Perspective Audit

**Smart-contract logic view.** The checked multiplication is *correct* for safety — it never
silently wraps. The defect is the *lack of a graceful path* when compounding overflows: there is no
clamp on `dt`, no cap on `ssr`, and `file("ssr")`'s `rho == block.timestamp` precondition is
mutually exclusive with a reverting `drip()`. The `uint192 chi` downcast (L223) is genuinely safe
(verified: `nChi` is bounded by `type(uint256).max / RAY ≈ 1.15e50 < type(uint192).max ≈ 6.28e57`),
so that is not the failure point — the raw `uint256` multiply is.

**Economic / risk view.** SSR is the system's savings-yield knob; an unbounded `file("ssr")` gives
governance the power to instantly mint unbacked yield (`diff` scales with `ssr`) or to brick the
vault. The init script's `RATES_ONE_HUNDRED_PCT` cap exists precisely because someone recognized the
danger — but it lives in deploy code, not in the contract, so it does not protect post-deployment
changes. Mirroring that cap (or a more conservative ceiling) into `file` is the minimal hardening.

**Operational / integration view.** sUSDS is treated as a yield-bearing collateral asset by many
protocols; a `drip()` revert propagates into every `convertToAssets`-based oracle and every
deposit/withdraw path. During the outage, integrations that attempt rebasing/accounting will revert,
which can cascade into liquidation-market freezes. The remediation requiring an *upgrade* (not a
parameter tweak) lengthens downtime materially versus a design where `file("ssr")` could always run.

### Recommended fix (sketch)
- Add an upper bound in `file`: `require(data >= RAY && data <= MAX_SSR, ...)` with `MAX_SSR` ≈ the
  per-second rate for a sane APR ceiling.
- Make `drip()` overflow-tolerant for the `nChi` step (e.g. cap the effective `dt`, or compute
  `nChi` with the same overflow guards the Pot uses and fall back to a bounded accrual) so a
  transiently-stale `rho` cannot brick operations.
- Decouple `file("ssr")` from `rho == block.timestamp`, or add a ward-only `forceDrip`/recovery
  entrypoint so governance can always lower `ssr` even when the normal compounding path reverts.
