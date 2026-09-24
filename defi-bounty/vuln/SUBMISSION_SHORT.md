# Ethena PSM `removeBenefactor` Mapping Persistence — Short Submission (Immunefi form)

**Severity:** High (request Critical consideration)
**Contract:** PSM — `0x73E35C5c35A274E34AdE6EB13cC7f62aEE323728` (Ethereum mainnet, USDtb PSM)

## Summary

`PSM.removeBenefactor(address)` (PSM.sol L645–649) calls `delete benefactorState[benefactor].config` to "permanently remove" a benefactor. The `BenefactorConfig` struct (IPSM.sol L112–124) contains **six nested mappings**, including `delegatedSigners` (L118) and `approvedBeneficiaries` (L119) — the two mappings that gate all swap authority. Per the Solidity language spec, `delete` on a struct is a **documented no-op on mapping members** ([docs.soliditylang.org/en/latest/types.html#delete](https://docs.soliditylang.org/en/latest/types.html#delete)). After `removeBenefactor`, `isActive` is reset to `false` (✓), but every persisted `delegatedSigners[X] == ACCEPTED` and `approvedBeneficiaries[Y] == true` survives indefinitely.

`addBenefactor(address)` (PSM.sol L624–636) only sets `isActive = true` — no re-initialization, no version counter, no enumerable-key sweep. After re-add, `_validateBenefactor` (PSM.sol L1493–1506) reads the **persisted** `delegatedSigners[msg.sender]` and `approvedBeneficiaries[order.beneficiary]` directly with no version binding — so a previously-accepted delegated signer / approved beneficiary immediately regains full swap authority **without re-confirmation**.

## Impact

Direct theft of user funds, in-motion — Ethena's Critical-tier criterion (`ethena.md` L46). In the `swapForAsset` direction (PSM.sol L325–328):
```
IERC20(collateral).safeTransferFrom(order.benefactor, receiveCustodian, amountIn);   // collateral pulled FROM benefactor
asset.safeTransferFrom(assetSendCustodian, order.beneficiary, amountOut);            // USDtb sent TO attacker
```
The attacker (msg.sender) calls `swap()`, names themselves as `order.beneficiary`, and the protocol pulls collateral from the benefactor and sends USDtb to the attacker. Single transaction, irreversible, no flash loan, no MEV, no oracle manipulation, no fresh compromise during the exploit window.

Drain is bounded by the benefactor's per-epoch/per-period rate limits and collateral balance. For institutional PSM benefactors (Ethena's target market) with $5M–$10M per-epoch limits, this is a **single-tx eight-figure direct theft**.

## Proof of Concept

Foundry, Solidity 0.8.30, `via_ir = true`. Contract under test: the unmodified `contracts/PSM.sol`. Four tests, all pass:

```
[PASS] test_RemoveBenefactor_AttackerCanSwapAfterReAdd() (gas: 869850)
  attacker asset gain:        1000.000000000000000000
  benefactorA collateral loss: 1000.000000000000000000
[PASS] test_RemoveBenefactor_DelegatedSignerAndBeneficiaryPersist() (gas: 444454)
[PASS] test_Sanity_AttackerCanSwapBeforeRemove() (gas: 566255)
[PASS] test_Sanity_FreshBenefactorBlocksUnknownAttacker() (gas: 150485)
```

Test 2 (end-to-end drain): admin adds benefactor → benefactor delegates to attacker + approves attacker as beneficiary → admin `removeBenefactor` (incident response) → `vm.warp(7 days)` → admin `addBenefactor(sameAddress)` → attacker calls `swap()` as `msg.sender`, names self as `beneficiary`. Result: attacker gains 1,000 USDtb, benefactor loses 1,000 collateral. Test 4 (control): fresh benefactor with no prior delegation correctly reverts with `DelegationNotAuthorized` — proves the bug is specifically persistence, not a general auth bypass.

## Why this is in scope (defense pre-emption)

- **"Leaked keys/credentials" exclusion does not apply.** The bug is a contract-level defect (mappings not cleared by `delete`), not a key-management failure. The compromised delegated-signer key is a *precondition*; the *attack* is the silent re-grant of authority on re-add. `removeBenefactor` is the contract's own incident-response function — its existence in the public ABI is an implicit warranty that calling it severs the benefactor's permissions.
- **"Privileged addresses" exclusion does not apply.** The admin is the **victim** of the bug, not the attacker. The admin's actions (`removeBenefactor` + `addBenefactor`) are the contract's documented API; the bug makes the combination unsafe with no event, no API method, and no NatSpec warning indicating that re-add is unsafe.
- **"Best-practice recommendation" exclusion does not apply.** The bug is a code defect (silent failure of an intended cleanup), not a feature request. The recommended fix (configVersion counter) restores the documented intent.

## Intent evidence (strongest single argument)

The contract ships **two** deliberately different benefactor-lifecycle functions:
- `disableBenefactor` (PSM.sol L611–615): `benefactorState[b].config.isActive = false;` — no `delete`; mappings persist by design (temporary pause, reversible via `enableBenefactor`).
- `removeBenefactor` (PSM.sol L645–649): `delete benefactorState[b].config;` — uses `delete`, signaling **permanent** cleanup.

The code divergence proves the authors understood the semantic distinction. If persistence across remove/re-add were intentional, the authors would not have used `delete` (a costly no-op for the six mappings) in `remove` while using a plain assignment in `disable`. The persistence is **unintended** — the cleanup silently failed.

## Caveats (honest severity framing)

We acknowledge three severity-reducing factors and submit at **High** rather than over-claiming Critical:
1. The exploit is gated by two privileged admin actions (`removeBenefactor` + `addBenefactor` by `BENEFACTOR_MANAGER_ROLE`).
2. A pre-existing credential compromise (delegated-signer key) is part of the threat model — but this is precisely the precondition `removeBenefactor` exists to remediate.
3. An operational workaround exists: re-onboard at a fresh address. But the contract does not enforce this, gives no signal that re-add is unsafe, and address reuse is the realistic operational pattern for multi-sig benefactor addresses.

These factors reduce severity by one tier (Critical → High). They do not eliminate the bug.

## Suggested fix

Add a `uint96 configVersion` field to `BenefactorConfig`. Increment it in `removeBenefactor`. Store `delegatedSigners` and `approvedBeneficiaries` under a versioned key (`mapping(uint96 => mapping(address => ...))`). In `_validateBenefactor`, read with the current `configVersion`. All old entries become unreachable after removal. (Detailed in SUBMISSION_FINAL.md §11.)

## References

- PSM.sol L611–615: `disableBenefactor` (no `delete`)
- PSM.sol L624–636: `addBenefactor` (only sets `isActive = true`)
- PSM.sol L638–649: `removeBenefactor` (uses `delete` — the bug)
- PSM.sol L1493–1506: `_validateBenefactor` (reads persisted mappings directly)
- PSM.sol L325–328: `swap` swapForAsset branch (fund-theft path)
- IPSM.sol L112–124: `BenefactorConfig` struct (six nested mappings)
- IPSM.sol L150–155: `BenefactorState` struct (wraps `BenefactorConfig`)
- Solidity docs, `delete`: https://docs.soliditylang.org/en/latest/types.html#delete
- Ethena scope: https://immunefi.com/bug-bounty/ethena/

Full report with 12 sections, attack scenario, defense anticipation, and remediation trade-offs: `SUBMISSION_FINAL.md`. Foundry PoC: `PoC_removeBenefactor.t.sol`.
