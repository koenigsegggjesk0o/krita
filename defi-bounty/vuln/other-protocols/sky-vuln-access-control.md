# Sky Audit — Access Control & Upgradability

> Target: `SUsds` (sUSDS, mainnet L1) and `SUsds` L2 (`src/l2/SUsds.sol`)
> Repo: `makerdao/sdai` — branch `susds`
> Bounty: Sky (Immunefi, no-KYC, max $10M)
> Severity: **INFORMATIONAL** (trust/centralisation assumptions; no unauthorised path found)

---

## 1. Description

### 1A. `file("ssr")` has no upper bound (governance-safety gap)

```solidity
// src/SUsds.sol  file()  L203
function file(bytes32 what, uint256 data) external auth {
    if (what == "ssr") {
        require(data >= RAY, "SUsds/wrong-ssr-value");   // L205  lower bound only
        require(rho == block.timestamp, "SUsds/chi-not-up-to-date");
        ssr = data;
    } else revert("SUsds/file-unrecognized-param");
}
```

The deploy/init script bounds `ssr` at `RATES_ONE_HUNDRED_PCT`
(`~100% APR`, `1000000021979553151239153027`) — see `deploy/SUsdsInit.sol`. The contract itself does
not. A ward can therefore set `ssr` to any `≥ RAY` value post-deployment. This is the **enabling
condition** for the rate-accumulator liveness brick (`sky-vuln-rate-accumulator.md`): an
unbounded `ssr` lets a single ward call freeze the entire vault within seconds, with no
non-upgrade recovery. It also lets a ward instantly mint unbacked yield (`diff ∝ ssr`,
`drip()` L220–222), inflating sUSDS claims against the `vow`'s debt capacity. Treated separately
because it is an *access-control surface* (the missing parameter ceiling), not a math bug.

### 1B. UUPS upgrade is instant, ward-only, no timelock

```solidity
// src/SUsds.sol  L132
function _authorizeUpgrade(address newImplementation) internal override auth {}
```

Any single ward can replace the implementation atomically. There is no on-chain timelock, no
multisig threshold inside the contract, and no upgrade-delay. This is consistent with MakerDAO/Sky's
governance model (wards are themselves governance-controlled multisigs/spells), so it is a stated
trust assumption rather than a defect — but it is the sole recovery lever for the brick in 1A, and
it is also the most powerful privileged action in the system (a malicious/compromised ward can
redirect all logic). Worth recording for the trust model.

### 1C. `initialize()` is single-shot and not front-runnable (verified)

```solidity
// src/SUsds.sol  L121
function initialize() initializer external {
    __UUPSUpgradeable_init();
    chi = uint192(RAY); rho = uint64(block.timestamp); ssr = RAY;
    vat.hope(address(usdsJoin));
    wards[msg.sender] = 1;            // deployer becomes the first ward
    emit Rely(msg.sender);
}
```

`initializer` (OpenZeppelin) prevents re-initialisation; the implementation constructor calls
`_disableInitializers()` (L111) so the implementation itself cannot be initialised. Deployment
creates the `ERC1967Proxy` with `abi.encodeCall(SUsds.initialize, ())` in the proxy constructor
(`deploy/SUsdsDeploy.sol`), so `initialize` runs in the same transaction as proxy creation — not
front-runnable. Owner is then transferred deployer→governance via `ScriptTools.switchOwner`. Access
control of initialisation is sound.

### 1D. L2 `SUsds` is a flat, ward-minted ERC-20

`src/l2/SUsds.sol` is a non-yield-bearing ERC-20 (no `chi`/`rho`/`ssr`/`drip`, no ERC-4626).
`mint` is `auth`-gated (L157):

```solidity
function mint(address to, uint256 value) external auth { ... }   // L157
```

On L2, a ward (the L1→L2 bridge/messenger) can mint unbounded supply. This is the standard bridged-
representation trust model: a compromised bridge/ward can inflate L2 sUSDS to any amount. There is
no supply cap, no per-mint ceiling, and no pause. The L2 token also does **not** accrue yield, so
holders bridged to L2 forfeit SSR accrual for the duration of their L2 holding — a product-design
note, not a bug.

### 1E. `deny`/`rely` are ward-only and symmetric

`rely`/`deny` (L193–201) are `auth`-only; the ward set is self-managing. No unauthorised privilege
escalation path was found. There is no `renounceOwnership`-style self-lockout hazard and no
two-step ownership (a ward can be instantly removed by another ward), which is the intended
MakerDAO pattern.

---

## 2. Contract / Function / Line

| Issue | Location |
|---|---|
| `file("ssr")` no upper bound | `src/SUsds.sol` `file()` L203–210 (L205) |
| UUPS instant ward-only upgrade | `src/SUsds.sol` `_authorizeUpgrade` L132 |
| `initialize` soundness | `src/SUsds.sol` L121–130; impl ctor L111; deploy `SUsdsDeploy.sol` |
| L2 ward-minted flat token | `src/l2/SUsds.sol` `mint` L157; no yield logic (no `drip`/`chi`) |
| `rely`/`deny` | `src/SUsds.sol` L193–201 |

---

## 3. Attack Scenario

- **1A:** A ward sets `ssr` to a harmful value (fat-finger or compromise) → vault bricked / unbacked
  yield minted. Requires ward access. See `sky-vuln-rate-accumulator.md` for the full chain.
- **1B:** A compromised ward calls `upgradeTo(maliciousImpl)` → full takeover (steal all assets,
  mint arbitrary shares). Requires ward access. No timelock means no public window to react.
- **1D:** A compromised L2 bridge ward calls `mint(attacker, huge)` → L2 sUSDS inflated; redeemers
  against the bridge could be left unredeemable if the bridge backs L2 sUSDS 1:1 with locked L1.
  Requires L2 ward/bridge compromise.

All scenarios are gated on ward access; none is reachable by an unprivileged user.

---

## 4. PoC (Foundry)

```solidity
// SPDX-License-Identifier: UNLICENSED
pragma solidity ^0.8.21;

import "forge-std/Test.sol";
import { SUsds } from "src/SUsds.sol";
import { ERC1967Proxy } from "@openzeppelin/contracts/proxy/ERC1967/ERC1967Proxy.sol";

contract AccessControlPoC is Test {
    SUsds s; address ward = address(0xbeef);
    // setUp(): deploy proxy+initialize, rely(ward), deny(this)

    function test_nonWardCannotFile() public {
        vm.prank(address(0xdead));
        vm.expectRevert("SUsds/not-authorized");
        s.file("ssr", 2e27);
    }

    function test_wardCanSetUnboundedSsr() public {
        // no ceiling rejects even an absurd value
        vm.prank(ward);
        s.file("ssr", 10**40);
        assertEq(s.ssr(), 10**40);
    }

    function test_wardCanUpgradeInstantly() public {
        SUsds newImpl = new SUsds(address(0x1), address(0x2));
        vm.prank(ward);
        s.upgradeTo(address(newImpl));           // no delay, no timelock
        assertEq(s.getImplementation(), address(newImpl));
    }

    function test_reinitialiseBlocked() public {
        vm.expectRevert();                       // Initializable: already initialized
        s.initialize();
    }
}
```

---

## 5. Impact

No unauthorised access path. The risk is the *blast radius* of ward power: an unbounded `ssr`
(parameter) and instant upgradeability (code) both let a single ward brick or take over the vault
with no on-chain delay. Impact is bounded by the trust placed in the ward set (governance multisig),
which is the documented model.

---

## 6. Severity

**INFORMATIONAL** (trust-model/centralisation notes). 1A is the access-control enabler of the LOW
rate-accumulator brick and is cross-referenced there; on its own it is a parameter-ceiling
hardening recommendation.

---

## 7. Three-Perspective Audit

**Smart-contract logic view.** All privileged entrypoints are correctly `auth`-gated; `rely`/`deny`
are symmetric and self-managing; initialisation is single-shot and atomic with proxy creation. No
privilege-escalation or unauthorised-write bug exists. The only logic-level gap is the missing
`ssr` ceiling in `file` (a parameter-validation omission, not an authz flaw).

**Economic / risk view.** Ward power is maximal: set rates, mint yield, upgrade code. The system's
safety therefore reduces entirely to the integrity and operational discipline of the ward set
(governance). Adding a timelock and an `ssr` ceiling would convert several "ward can break things
instantly" risks into "ward can attempt, public can react" risks without changing the trust root.

**Operational / integration view.** Integrators should price sUSDS assuming the implementation can
change (verify `getImplementation()` / monitor upgrades) and that `ssr` can move sharply. For L2
sUSDS, integrators should treat supply as bridge-backed and not assume yield accrual.

### Recommended fix (sketch)
- `file("ssr")`: add `require(data <= RATES_ONE_HUNDRED_PCT, "SUsds/ssr-too-high")` (or a tighter
  ceiling) to mirror the init-script cap inside the contract.
- Wrap `upgradeTo`/`upgradeToAndCall` behind a timelock (or document explicitly that the ward set is
  itself timelocked governance).
- For L2 `SUsds`: consider a per-mint ceiling or a pause, and document explicitly that L2 sUSDS is
  non-yielding and bridge-supply-backed.
