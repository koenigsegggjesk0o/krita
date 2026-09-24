# DEVIL'S ADVOCATE — Maximum-Skepticism Challenge of Two Ethena Bug Submissions

**Task ID:** eth-devils-advocate
**Agent:** Opus (Devil's Advocate — explicitly adversarial to in-house conclusions)
**Date:** 2026-09-24
**Mission:** Find every reason these bugs DON'T matter, are NOT exploitable, will be REJECTED, or are INFLATED. Honesty over bounty hopes.

**Bugs challenged:**
1. PSM `removeBenefactor` mapping persistence (`SUBMISSION_FINAL.md`, $25k–$75k ask)
2. TON bounce silent-drop in tsUSDe vault + USDe admin (`SUBMISSION_TON_DRAFT.md`, $100k ask, $10k–$75k fallback)

**Prior in-house verdicts being challenged:**
- Judge verdict (DEBATE_judge_verdict.md): PSM = HIGH, ~$40k modal, 92% acceptance
- TON verifier (VERIFICATION_ton_bounce.md): TON = HIGH-to-Critical, submit as Critical

---

## Executive Summary (Verdict First)

| Bug | In-house claim | Devil's advocate verdict | Submit? |
|-----|----------------|--------------------------|---------|
| PSM `removeBenefactor` mapping persistence | HIGH ($25k–$75k, modal $40k) | **REAL bug, INFLATED severity.** Realistic triage = Medium ($5k–$15k). 35% rejection risk. | **Submit, but only as Medium with High ask.** Honest framing improves triage goodwill. |
| TON bounce silent-drop | HIGH-to-Critical ($10k–$100k) | **REAL bug, OVERSTATED impact.** Self-griefing only — no attacker profit. Realistic triage = Medium (griefing) OR rejected as "user error." 45% rejection risk. | **Submit only if combined with a stronger TON finding.** Otherwise hold. |

**Headline weaknesses found:**
- PSM bug requires a 4-step precondition chain (2 admin actions + 1 prior compromise + 1 key retention). Immunefi's "leaked keys" exclusion is a live rejection risk.
- TON bug is **self-griefing only** — the attacker can ONLY lose their own funds. No theft of OTHER users' funds is demonstrated. Immunefi's "permanent freezing of funds" is typically interpreted as attack-by-external-party, not user-error.
- Both PoCs have artifact issues: PSM PoC uses `type(uint128).max` for all limits (removes friction); TON PoC has no sandbox deployment (only step-by-step scenario with approximate gas numbers).
- Neither bug has a "smoking gun" impact on OTHER users' funds.

---

## Perspective 1: "Bug exists but no impact"

### PSM bug

**Challenge:** Maybe the mappings persist but are never used in a harmful way?

**Investigation:**
- Read `_validateBenefactor` (PSM.sol L1493–1506) — confirmed: it DOES check `delegatedSigners[msg.sender] != DelegatedSignerStatus.ACCEPTED` (L1498) and `approvedBeneficiaries[order.beneficiary]` (L1502).
- Read `swap()` flow (L325–332) — confirmed: collateral IS pulled from `order.benefactor` and asset IS sent to `order.beneficiary`.
- An attacker who is `msg.sender`, who was previously ACCEPTED as delegated signer, who is named as `order.beneficiary`, and who is in `approvedBeneficiaries` — passes `_validateBenefactor` and gets asset tokens.

**Verdict:** Bug exists AND has impact **if precondition chain holds**. This is NOT a "no-impact" case. Persistence is genuinely exploitable.

**But:** The "no impact" angle still applies partially. The mappings only matter if:
- (a) Admin uses `removeBenefactor` (not just `disableBenefactor`)
- (b) Admin later re-adds the SAME address (not a fresh address)
- (c) The attacker was previously ACCEPTED (not just PENDING)
- (d) The attacker's key was not rotated during the remove→re-add window

If any of (a)–(d) is false, the persisted mappings are inert. The bug has impact **conditional on operator choices**.

**Severity adjustment:** No change to bug validity; reduces likelihood.

### TON bug

**Challenge:** Maybe the bounce is dropped but the vault has retry or refund we missed?

**Investigation:**
- Grepped the vault disassembly for `reclaim`, `reverse`, `refund`, `0xd53276db` (excesses opcode). NO matches found.
- Grepped for `reclaimDeposit`, `reverseBounce`, `pendingOperations`. NO matches.
- Read the vault's `fun_0` dispatch — every opcode in the table is a forward-action handler, not a recovery handler.
- The LayerZero V2 retry (`forceAbort`/`nilify`) applies ONLY to cross-chain packets, not internal TON messages.

**Verdict:** No retry exists in the vault. The bug DOES cause permanent fund loss when a bounce occurs.

**But:** The "no impact" angle still applies in a different way: **a bounce must actually occur**. In normal operation, with proper gas attachment, no bounce occurs. The bug is latent — it only fires under failure conditions.

**Severity adjustment:** No change to bug validity; reduces likelihood significantly.

---

## Perspective 2: "Operational reality"

### PSM bug — Does Ethena ever remove + re-add the same benefactor?

**Investigation (without on-chain access — flagged as limitation):**
- The submission does NOT cite a single on-chain instance of `removeBenefactor` being followed by `addBenefactor` on the same address.
- The submission does NOT cite Ethena docs, runbooks, or operational policies.
- The "7-day warp" in the PoC is illustrative, not derived from observation.

**Operational reality arguments AGAINST the bug:**
1. **Institutional benefactors are typically multisigs or corporate wallets.** Re-using the same address after a security incident is unusual — standard practice is to deploy a fresh multisig with new signers.
2. **`disableBenefactor` is the natural "incident response" function** (sets `isActive = false` without `delete`). If Ethena wanted to temporarily pause a benefactor, they would use `disableBenefactor`, not `removeBenefactor`.
3. **`removeBenefactor` reads as a "permanent exit" function** (uses `delete`). If Ethena permanently exits a partner, they would NOT re-onboard the same address — they'd onboard a new partner with a fresh address.
4. **No evidence** that Ethena's `BENEFACTOR_MANAGER_ROLE` holder has ever called `removeBenefactor` at all, let alone followed by `addBenefactor` on the same address.

**Operational reality arguments FOR the bug:**
1. Address reuse IS plausible for partners that retain legal entity identity but rotate keys internally.
2. The contract gives the admin no signal that re-add is unsafe — so the "avoid this pattern" mitigation requires undocumented policy.
3. Ethena's PSM is only 6 weeks old (added 10 Aug 2026) — operational patterns are not yet established.

**Verdict:** Operational reality **substantially weakens** the bug. Without evidence that Ethena actually performs remove→re-add on the same address, the bug is conditional on a hypothetical operator choice. The judge acknowledged this (D3 "operational mitigation") but only reduced severity by one tier (Critical→High). The devil's advocate view is that this should reduce severity by **two tiers** (Critical→Medium), because the operator choice is non-default and undocumented patterns cannot be assumed.

**Severity adjustment:** High → Medium.

### TON bug — Does Ethena's vault actually experience bounces?

**Investigation:**
- TON storage-rent economics: jetton wallets that haven't received TON in a while can't pay rent and bounce incoming messages. This IS a known TON phenomenon.
- However: standard TON wallets (Tonkeeper, Tonhub) automatically attach sufficient gas for jetton transfers. Manual under-attachment requires a custom script.
- Ethena's vault has been live since 20 May 2025 — ~16 months at time of analysis. If bounces had been a recurring problem, Ethena would have noticed (probably) and either fixed the vault or added a `reclaimDeposit` function.

**Arguments AGAINST the bug being operationally relevant:**
1. The bug fires only on bounce. Bounce requires gas miscalculation, wallet depletion, or network congestion.
2. UI defaults (Tonkeeper, Tonhub) attach generous gas. Manual under-attachment is non-default.
3. Ethena's vault has 16 months of production usage without (publicly known) bounce-loss incidents.
4. The verification report itself estimates "Probability of any single deposit/withdrawal bouncing: LOW (~1%)" — and even this 1% is unverified.

**Arguments FOR the bug:**
1. TON jetton wallets DO get depleted (rent is real).
2. The bug is latent — it might have already fired for some user without Ethena noticing (no event emitted).
3. "16 months without known incident" is weak evidence — silent fund loss with no event is exactly the kind of bug that goes unnoticed.

**Verdict:** Operational reality **moderately weakens** the bug. The bug is real but its production-relevance is unproven. The verifier's "near-certain within 1 year" estimate is speculative.

**Severity adjustment:** Critical/High → Medium.

---

## Perspective 3: "Immunefi triager will reject"

### PSM bug — Rejection risks

**Rejection pattern A: "Attacks requiring leaked keys/credentials"**
- Ethena's scope explicitly excludes this.
- The bug requires the attacker to be a previously-ACCEPTED delegated signer — i.e., to have held (or still hold) the benefactor's delegated key.
- The in-house defense (D1 in judge verdict): "the leaked key is a precondition, not the attack" — relying on Immunefi's narrow interpretation.
- **Devil's advocate:** Immunefi triagers are NOT uniform in interpretation. Some triagers apply the "leaked keys" exclusion broadly: "if the attacker needed a key they didn't legitimately control at exploit time, it's leaked keys." The judge's "narrow interpretation" is a hopeful reading, not a guaranteed one.
- **Probability of rejection on this ground: ~20%.**

**Rejection pattern B: "Attacks requiring privileged addresses without modifications"**
- The bug requires `BENEFACTOR_MANAGER_ROLE` to call `removeBenefactor` and `addBenefactor`.
- The in-house defense (D2): "the admin is the victim, not the attacker" — the admin's actions are intended to be safe.
- **Devil's advocate:** Some triagers interpret "privileged addresses" as "any bug requiring admin action to manifest." Under this reading, the bug requires admin action to enable, so it's out of scope.
- **Probability of rejection on this ground: ~10%.**

**Rejection pattern C: "Not exploitable by external party"**
- An external party (someone who is NOT a previously-accepted delegated signer) CANNOT exploit this bug.
- The bug requires the attacker to have been previously trusted by the benefactor.
- **Devil's advocate:** Immunefi's standard for "external party" is strict. If only previously-trusted parties can exploit, the bug is closer to "insider attack" than "external attack."
- **Probability of rejection on this ground: ~15%.**

**Rejection pattern D: "Not a bug — intended behavior"**
- The NatSpec for `removeBenefactor` says "Removes a benefactor from the system" — does NOT say "clears all configuration."
- Ethena could argue: "the contract works as documented; the admin is responsible for understanding Solidity's `delete` semantics."
- **Devil's advocate:** This is a weak defense (the `disable` vs `remove` divergence rebuts it), but Ethena might still try.
- **Probability of rejection on this ground: ~5%.**

**Combined rejection probability (union of A–D, with overlap): ~35%.**

This is significantly higher than the judge's 8% rejection estimate. The judge's optimistic number assumes all four defense angles fail; the devil's advocate view is that ANY ONE succeeding triggers rejection.

### TON bug — Rejection risks

**Rejection pattern A: "User error" — insufficient gas**
- The most realistic trigger (per verifier) is "user attaches insufficient TON."
- Triager response: "User attached insufficient gas → user error → out of scope."
- This is a STRONG rejection angle. Most Immunefi triagers view "user attached wrong gas" as user-side responsibility, not a contract bug.
- **Probability of rejection on this ground: ~30%.**

**Rejection pattern B: "Self-griefing — no attacker profit"**
- The attacker can ONLY lose their own funds. No external party can directly profit.
- Immunefi's "permanent freezing of funds" is typically interpreted as attack-by-external-party-against-victim-funds.
- Self-griefing is usually classified as Medium (griefing) — see Ethena's scope: "Medium: Griefing (no profit motive but damage to users/protocol)."
- **But:** self-griefing with no damage to others might be sub-Medium.
- **Probability of rejection on this ground: ~20%.**

**Rejection pattern C: "Requires specific conditions"**
- Bounce must occur — non-default condition.
- Triager response: "Bug requires network congestion or user-side failure to manifest; not a direct contract defect."
- **Probability of rejection on this ground: ~10%.**

**Rejection pattern D: "Theoretical — no demonstrated production impact"**
- No actual bounce-loss incident has been observed in 16 months of production.
- Triager response: "If this were a real bug, we'd have seen it by now."
- **Devil's advocate:** This is weak — silent fund loss with no event is hard to observe — but some triagers buy it.
- **Probability of rejection on this ground: ~5%.**

**Combined rejection probability (union): ~50%.**

This is dramatically higher than the verifier's implicit rejection estimate (verifier focused on scope rejection and didn't quantify user-error rejection). The TON bug has a coin-flip chance of rejection.

---

## Perspective 4: "TVM-specific" (TON bug only)

**Challenge:** TON bounce is actually RARE in practice? Maybe LayerZero V2 handles this? Is "self-triggered bounce" even in scope?

**Investigation:**

1. **TON bounce frequency:** TON's bounce mechanism is well-documented and DOES fire in production. Common triggers:
   - Insufficient gas (user miscalculation)
   - Wallet depletion (storage rent)
   - Network congestion raising forward fees
   - Receiver code failure
   The verifier's 1%-per-transaction estimate is plausible but unverified.

2. **LayerZero V2 retry:** Only applies to cross-chain packets routed through the Endpoint. The vault's internal TON messages (deposit processing, withdrawal) are NOT LayerZero packets — they're standalone TON messages. LayerZero retry does NOT help.

3. **User-initiated retry:** A user who notices their tsUSDe didn't arrive could try sending another deposit message. BUT:
   - The first deposit's USDe is already locked in the vault.
   - The user has no way to "reverse" the first deposit.
   - The user would have to send a SECOND deposit (with proper gas) to get tsUSDe — losing the first deposit's USDe permanently.
   - This is NOT a retry mechanism; it's "send more money to maybe get what you already paid for."

4. **Scope of "permanent freezing of OWN funds via self-triggered bounce":**
   - Ethena's scope says "Permanent freezing of funds" is Critical-tier.
   - But Immunefi's standard interpretation of "permanent freezing" typically requires:
     - An external attacker can freeze VICTIM funds, OR
     - A code bug causes funds to be permanently locked WITHOUT user action.
   - "User can lock their OWN funds by deliberately under-paying gas" is closer to "user error" than "permanent freezing."
   - The verifier's "permanent freezing" framing is hopeful but not standard.

**Verdict:** TVM-specific perspective reveals that:
- The bug IS real (bounce handling is missing).
- The bug ISN'T directly attacker-profitable.
- "Self-triggered permanent freezing" is a borderline scope question — Immunefi could go either way.
- The "near-certain within 1 year" probability estimate is speculative.

**Severity adjustment:** Critical → Medium (griefing).

---

## Perspective 5: "Severity inflation"

### PSM bug — Are we inflating HIGH to Critical for user satisfaction?

**In-house claim:** "HIGH (with Critical consideration)"

**Devil's advocate:** The bug is more realistically **Medium**.

**Why Medium, not High:**
- High per Ethena's scope = "Theft of unclaimed yield" / "Temporary freezing of funds."
- This bug doesn't fit High's definition cleanly:
  - It's not "theft of unclaimed yield" (it's theft of principal).
  - It's not "temporary freezing" (it's permanent if it occurs).
- The bug is closer to Critical's "Direct theft of user funds (in-motion)" — BUT gated by 4 preconditions.
- Immunefi's standard triage practice: bugs with multi-step precondition chains involving admin actions + prior compromise typically score **one tier below their literal impact**.
- Critical impact + 4-precondition chain = High (per judge) or Medium (per stricter triage).

**Realistic Ethena team classification:**
- Ethena would likely classify this as **Medium** because:
  - The bug requires their admin to do something specific (remove + re-add same address).
  - Their admin can avoid the bug by using fresh addresses.
  - The "leaked keys" precondition is uncomfortable for them (they'd argue it's an operational issue, not a contract issue).
- They might offer $5k–$15k as Medium.
- They MIGHT upgrade to High ($25k–$50k) if the triager is sympathetic to the "silent revocation failure" argument.

**Comparable Immunefi precedents (from public reports):**
- Bugs requiring admin misconfiguration typically land at Medium.
- Bugs requiring prior compromise typically land at Medium or are rejected.
- Bugs with end-to-end PoC fund theft typically land at High (if preconditions are minimal) or Medium (if preconditions are heavy).
- This bug's precondition chain is HEAVY → Medium is the modal outcome.

**Severity adjustment:** High → Medium (modal); High is the optimistic ceiling.

### TON bug — Are we inflating to Critical for user satisfaction?

**In-house claim:** "HIGH-to-Critical"

**Devil's advocate:** The bug is more realistically **Medium (griefing)** or **rejected**.

**Why Medium or rejected, not High:**
- High per Ethena's scope = "Temporary freezing of funds."
- This bug causes PERMANENT freezing — which is Critical-tier impact.
- BUT — Critical-tier impact requires (per Immunefi standard interpretation) that the bug be exploitable by an external attacker against victim funds.
- Self-griefing doesn't qualify as Critical impact.
- "Permanent freezing of OWN funds by deliberate under-payment" is closer to Medium (griefing).
- Ethena's scope explicitly lists "Griefing (no profit motive but damage to users/protocol)" as Medium.

**Realistic Ethena team classification:**
- They would likely classify as **Medium (griefing)** OR reject as "user error."
- They might offer $5k–$15k as Medium.
- They MIGHT upgrade to High if they accept that "user wallet depletion" is not user error.

**Severity adjustment:** Critical/High → Medium (griefing) modal.

---

## Perspective 6: "Foundry test artifact" (PSM bug)

**Challenge:** Maybe the PoC uses unrealistic parameters?

**Investigation of `PoC_removeBenefactor.t.sol`:**

1. **Rate limits set to `type(uint128).max` (line 74):**
   ```
   uint128 constant LARGE = type(uint128).max; // no rate-limit friction
   ```
   Used for ALL rate limit fields:
   - `maxSwapForAssetPerEpoch`
   - `maxSwapForCollateralPerEpoch`
   - `defaultBenefactorMaxSwapForAssetPerEpoch`
   - `defaultBenefactorMaxSwapForCollateralPerEpoch`
   - `maxSwapForAssetPerPeriod`
   - `maxSwapForCollateralPerPeriod`
   - `defaultBenefactorMaxSwapForAssetPerPeriod`
   - `defaultBenefactorMaxSwapForCollateralPerPeriod`

   **Realism:** NO production PSM would set rate limits to `type(uint128).max`. Ethena's institutional PSM benefactors would have $5M–$10M per-epoch limits, not $3.4×10^38. The PoC removes ALL rate-limit friction to demonstrate the bug cleanly, but this DOES inflate the apparent impact.

   **Impact on bug validity:** The bug still works with realistic limits — but the drain is BOUNDED by the benefactor's per-epoch limit, not unbounded. The "single-tx eight-figure theft" claim depends on the benefactor having $5M–$10M+ limits; for smaller benefactors, the drain is proportionally smaller.

2. **Fees set to 0 (line 140–141):**
   ```
   defaultSwapForAssetFee: 0, // 0 fee for simplicity
   defaultSwapForCollateralFee: 0,
   ```
   **Realism:** Production PSMs typically charge 5–50 bps fees. The PoC's 0-fee setup means the attacker drains 100% of `SWAP_AMOUNT`. With a 50 bps fee, the attacker drains 99.5% — still bad, but slightly less dramatic.

   **Impact on bug validity:** Minimal — fees don't prevent the attack, just reduce proceeds by ~0.5–5%.

3. **Test swap amount = 1,000 tokens (line 75):**
   ```
   uint128 constant SWAP_AMOUNT = 1_000e18; // 1000 stablecoins
   ```
   **Realism:** This is a symbolic amount, not a production-scale drain. It proves the mechanism but doesn't demonstrate $5M+ impact.

   **Impact on bug validity:** The mechanism is proven; the impact scale is asserted, not demonstrated.

4. **7-day warp (line 285):**
   ```
   vm.warp(block.timestamp + 7 days);
   ```
   **Realism:** The warp is illustrative — the bug is time-invariant. A 1-minute or 1-year gap exhibits the same persistence. This is NOT a realism issue.

5. **Custodian wallets pre-funded with 1,000,000 tokens (lines 86–87):**
   ```
   asset.mint(assetSendCustodian, 1_000_000e18);
   collateral.mint(collateralSendCustodian, 1_000_000e18);
   ```
   **Realism:** Production custodian wallets would have much larger balances (Ethena-scale TVL). This doesn't inflate the bug — it just provides sufficient liquidity for the test swap.

6. **Test setup uses MockERC20 and MockOracleFeed:**
   **Realism:** These are standard test mocks. The actual asset (USDtb) and oracle (likely a custom feed) would have different decimals/precision, but the bug mechanism is the same.

**Verdict:** The PoC is **mechanistically correct but parametrically inflated**. The "1,000 token drain" is symbolic; the "single-tx eight-figure theft" claim is asserted, not demonstrated. A more realistic PoC would use:
- $5M per-epoch limit (not `type(uint128).max`)
- 25 bps fee (not 0)
- $5M swap amount (not $1k)

But even with realistic params, the bug still works — just bounded by per-epoch limits.

**Severity adjustment:** No change to bug validity; reduces impact ceiling from "unbounded" to "per-epoch-limit-bounded."

---

## Perspective 7: "Code intent"

### PSM bug — Did Ethena intend for mappings to persist?

**Investigation:**

1. **NatSpec for `removeBenefactor` (PSM.sol L638–644):**
   ```
   * @notice Removes a benefactor from the system
   * @dev Only callable by benefactor managers
   * @param benefactor Address of the benefactor to remove
   * @dev Reverts if benefactor is not active
   * @dev Emits BenefactorRemoved event on success
   ```
   - Says "Removes a benefactor from the system" — ambiguous.
   - Does NOT say "clears all configuration" or "wipes all delegated signers."
   - Does NOT say "preserves mappings for audit trail."
   - **Verdict:** NatSpec is silent on persistence — neither confirms nor denies intent.

2. **Contrast with `disableBenefactor` (PSM.sol L604–610):**
   ```
   * @notice Disables a benefactor
   * @dev Only callable by benefactor disablers
   * @param benefactor Address of the benefactor
   * @dev Reverts if not active
   * @dev Emits BenefactorDisabled event on success
   ```
   - Also silent on persistence.
   - **But the CODE differs:** `disableBenefactor` uses `isActive = false` (no `delete`); `removeBenefactor` uses `delete .config`.
   - The deliberate code divergence is the strongest intent evidence — `delete` signals "permanent cleanup intent."

3. **No git history available** (verified: contracts directory has no `.git`). Cannot check commit messages or PR discussion for intent clues.

4. **No `audit trail` NatSpec or comment** suggesting persistence was intentional.

5. **Could `delete` have been chosen specifically to preserve mappings?** No — `delete` on a struct with mappings is a documented no-op for the mappings. If the developer KNEW this and wanted to preserve mappings, they would have used `isActive = false` (like `disableBenefactor`). Using `delete` suggests the developer EXPECTED it to clear everything — which is the bug.

**Verdict:** Code intent evidence SUPPORTS the bug claim. The `delete` keyword was likely chosen with the expectation of full cleanup; the cleanup silently fails for mappings. Ethena would have a hard time arguing "intended behavior" given the `disable` vs `remove` divergence.

**But:** Ethena COULD argue "we knew mappings persist; `delete` was for the value-type fields only; mappings are intentionally preserved for audit trail." This is a weak argument (no NatSpec, no comment, no event) but not impossible.

**Severity adjustment:** No change — intent evidence supports the bug.

### TON bug — Did Ethena intend for bounces to be dropped?

**Investigation:**

1. **Framework source comment** (`contractMain.fc`):
   ```
   ;;; ================================================================
   ;; The base main function for LayerZero Endpoint, UltraLightNode, and OApp
   ;;; ================================================================
   ```
   - The framework's `if (txnIsBounced()) { return (); }` is explicitly scoped to "LayerZero Endpoint, UltraLightNode, and OApp" — contracts with LayerZero's protocol-level retry.
   - The vault and admin are NOT LayerZero Endpoint/UltraLightNode/OApp — they're standalone staking contracts.
   - **Verdict:** The framework comment SCOPE-LIMITS the bounce-drop pattern to LayerZero contracts. The vault/admin inherited it WITHOUT being in that scope. This is an oversight, not intent.

2. **Contrast with jetton masters:**
   - `usde_jetton_master` and `tsusde_jetton_master` implement PROPER TEP-74 bounce handling (subtract from total_supply).
   - Same team, same ecosystem, same security requirements.
   - **Verdict:** The jetton masters' proper bounce handling proves the team KNOWS how to handle bounces correctly. The vault/admin not doing so is a divergence — likely an oversight from inheriting the LayerZero framework.

3. **Could the bounce-drop be intentional for the vault?**
   - Argument: "The vault records deposits in `c4` BEFORE sending tsUSDe. If the tsUSDe send bounces, the vault's `c4` still says 'deposited.' Maybe Ethena intended this so they can manually reconcile later."
   - Rebuttal: There's no `reconcile` or `refund` function in the opcode table. If Ethena intended manual reconciliation, they would have added an admin function to do it.
   - **Verdict:** Not intentional — the vault has no reconciliation mechanism.

**Verdict:** Code intent evidence SUPPORTS the bug claim. The bounce-drop is an oversight from inheriting the LayerZero framework without override.

**Severity adjustment:** No change — intent evidence supports the bug.

---

## Perspective 8: "Alternative explanation"

### PSM bug — Could there be something we missed?

**Investigation:**

1. **Could `delete .config` actually clear mappings in Solidity 0.8.30?**
   - No. This is documented Solidity behavior, unchanged across all 0.8.x versions. The Solidity docs explicitly state: "delete a has no effect on mappings (as mappings are not enumerable)."
   - Verified against Solidity 0.8.30 release notes — no changes to `delete` semantics.

2. **Could there be a constructor/init that resets mappings?**
   - Constructor (PSM.sol L178–245): only initializes global config, custodian addresses, and role grants. Does NOT reset per-benefactor state.
   - No `init()` or `reinitialize()` function found.
   - **Verdict:** No reset mechanism exists.

3. **Could there be an upgrade that fixes this?**
   - The contract uses `SingleAdminAccessControl` (not OpenZeppelin Upgradeable) — NOT a proxy pattern.
   - No `upgradeTo` or `initialize` pattern detected.
   - The contract at `0x73E35C5c35A274E34AdE6EB13cC7f62aEE323728` is immutable (barring an unusual proxy pattern not visible in the source).
   - **Verdict:** No upgrade path. Bug is permanent in deployed code.

4. **Could `orderNonceInvalidator` (also in `BenefactorState`) save us?**
   - `orderNonceInvalidator` lives in `BenefactorState`, NOT in `BenefactorConfig`. `removeBenefactor` only `delete`s `.config` — so `orderNonceInvalidator` ALSO persists.
   - This means the attacker can't replay old nonces — but they can use NEW nonces (any nonce not yet used).
   - **Verdict:** Nonce invalidation doesn't save us; the attacker just uses a fresh nonce.

5. **Could `_validateOrder` (called in swap) block the attack?**
   - `_validateOrder` checks expiry, nonce, chain ID — not benefactor state.
   - The attacker controls all order fields; they can craft a valid order.
   - **Verdict:** `_validateOrder` doesn't block the attack.

6. **Could `isSwapEnabled = false` save us?**
   - `swap()` reverts if `!isSwapEnabled` (line 269).
   - But this is a global pause — if swap is enabled (which it must be for normal operation), the attack works.
   - **Verdict:** Only helps if Ethena has globally paused swaps (unlikely in normal operation).

**Verdict:** No alternative explanation found. The bug is real and unaffected by any missed mechanism.

### TON bug — Could there be something we missed?

**Investigation:**

1. **Could the vault have a hidden bounce handler we missed?**
   - Grepped the full disassembly for `bounce`, `reverse`, `refund`, `reclaim`, `0xd53276db`, `0xFFFFFFFF` (bounce marker).
   - The only bounce-related code is in `fun_0` (the `s0 POP` branch) and `initTxnContext` (which sets the bounce flag).
   - **Verdict:** No hidden bounce handler.

2. **Could the vault's `c4` storage auto-reverse on bounce?**
   - TON storage (`c4`) is NOT auto-reversed on bounce. The contract's code must explicitly reverse it.
   - The vault's bounce branch is `s0 POP` — no storage modification.
   - **Verdict:** No auto-reversal.

3. **Could the user's wallet bounce trigger a different code path?**
   - When a sub-transfer bounces (e.g., user's USDe wallet bounces), the jetton wallet sends `excesses` (0xd53276db) back to the vault.
   - The vault's opcode table does NOT include `0xd53276db` (verified by grep).
   - The vault would either throw (THROW 261) or silently drop the excesses message.
   - **Verdict:** No alternative path; the excesses is also dropped.

4. **Could LayerZero V2's `forceAbort`/`nilify` help?**
   - These apply ONLY to cross-chain packets routed through the LayerZero Endpoint.
   - The vault's internal deposit/withdrawal messages are NOT LayerZero packets.
   - **Verdict:** LayerZero retry doesn't help.

5. **Could the vault's `c4` accounting be eventually consistent (i.e., self-corrects over time)?**
   - No. `c4` is only modified by explicit code in the dispatch handlers. There's no background reconciliation process.
   - **Verdict:** No self-correction.

**Verdict:** No alternative explanation found. The bug is real and unaffected by any missed mechanism.

---

## Perspective 9: "Market reality"

### PSM bug — Is Ethena still using this PSM in production?

**Investigation (without on-chain access — flagged as limitation):**

1. **PSM is the NEWEST contract** (added to Immunefi scope 10 Aug 2026, ~6 weeks before analysis date).
2. **TVL is unknown** (Ethena's Immunefi scope lists TVL as "?").
3. **`isSwapEnabled` might be false initially** — newly deployed PSMs often start paused and are enabled later.
4. **No evidence of active swap volume** — would need Etherscan or Dune query to verify.

**Devil's advocate interpretation:**
- If PSM is paused (`isSwapEnabled = false`), the bug is moot — `swap()` reverts.
- If PSM is live but has low TVL, the bug's impact is bounded.
- If PSM is live with significant TVL, the bug matters.
- Without on-chain verification, we CANNOT confirm the bug is currently exploitable.

**Risk:** Submitting a bug for a contract that's paused or has $0 TVL would look amateurish. The triager might respond: "This contract isn't active yet; we'll fix the bug before enabling."

**Severity adjustment:** Conditional on PSM being live with non-trivial TVL. If PSM is paused, the bug's current severity is $0.

**Recommendation:** Before submitting, verify on Etherscan that:
- `isSwapEnabled()` returns true
- At least one `SwapExecuted` event has been emitted
- The PSM holds non-trivial asset tokens

### TON bug — Is the TON vault still active?

**Investigation:**

1. **TON vault has been live since 20 May 2025** — ~16 months at analysis time.
2. **TVL is unknown** (Ethena's scope lists TON targets as "?").
3. **No evidence of recent deposit/withdrawal activity** — would need TonScan or similar to verify.

**Devil's advocate interpretation:**
- If TON vault has $0 TVL (decommissioned), the bug is moot.
- If TON vault has low TVL, the bug's impact is bounded.
- If TON vault has meaningful TVL, the bug matters.
- Ethena's TON deployment is explicitly listed as MEDIUM priority — suggesting lower TVL than mainnet.

**Risk:** Submitting a TON bug for a low-TVL vault might be classified as "low impact" even if accepted.

**Severity adjustment:** Conditional on TON vault TVL. Without on-chain verification, severity is uncertain.

**Recommendation:** Before submitting, verify on TonScan that:
- The vault's USDe jetton wallet has non-trivial USDe balance
- The vault's tsUSDe jetton master has non-trivial total_supply
- Recent deposit/withdrawal transactions exist

---

## Perspective 10: "Submission strategy"

### PSM bug — Should we submit even if real?

**Arguments FOR submitting:**
1. The bug IS technically real (Solidity `delete` semantics are documented and confirmed).
2. The PoC IS end-to-end (4 tests passing, including fund drain).
3. The `disable` vs `remove` divergence IS strong intent evidence.
4. The bug defeats an incident-response function — this is a recognized bug class.
5. Expected value: even at Medium ($5k–$15k) × 65% acceptance = $3k–$10k EV.
6. Submitting establishes the researcher's track record with Immunefi.

**Arguments AGAINST submitting:**
1. 35% rejection risk (devil's advocate estimate, vs 8% judge estimate).
2. The precondition chain (4 conditions, 2 admin actions, 1 prior compromise) is HEAVY.
3. The "leaked keys" exclusion is a live rejection risk.
4. The "admin error" framing could lead to Medium-not-High triage.
5. Risk of being labeled "low quality reporter" if rejected.
6. Reputation damage if Ethena disputes.

**Honest assessment:** The expected value IS positive (estimated $3k–$10k EV), but the variance is high. The bug is real but the precondition chain is the kind of thing that triages inconsistently.

**Submission recommendation: SUBMIT, but:**
- Lead with HONEST Medium-as-High framing, not Critical.
- Acknowledge the precondition chain upfront.
- Pre-empt the "leaked keys" and "admin error" defenses.
- Quote the Solidity docs verbatim.
- Quote the `disable` vs `remove` divergence prominently.
- DO NOT over-claim "single-tx eight-figure theft" — use "per-epoch-limit-bounded theft" instead.

### TON bug — Should we submit even if real?

**Arguments FOR submitting:**
1. The bug IS technically real (disassembly is unambiguous).
2. The pattern divergence with jetton masters IS strong evidence.
3. The bug causes permanent fund loss when triggered.
4. Expected value: even at Medium ($5k–$15k) × 50% acceptance = $2.5k–$7.5k EV.
5. TON-specific bugs are rare; this could establish TON expertise.

**Arguments AGAINST submitting:**
1. 50% rejection risk (devil's advocate estimate).
2. Self-griefing only — no attacker profit.
3. The "user error" rejection is STRONG (user attached insufficient gas).
4. No actual production incident in 16 months.
5. No sandbox PoC (only step-by-step scenario).
6. Combined with PSM bug, risks "two weak bugs = low-quality researcher" perception.

**Honest assessment:** The expected value is marginally positive but the rejection risk is high. The bug is real but the impact framing is weak. Submitting alone is risky; submitting alongside a stronger TON finding would be safer.

**Submission recommendation: HOLD unless:**
- A stronger TON finding is found to bundle with this one, OR
- On-chain verification confirms meaningful TON vault TVL, OR
- A sandbox PoC is built (eliminating the "theoretical" rejection angle).

If submitting alone: lead with HONEST Medium (griefing) framing, not Critical. Acknowledge self-griefing upfront. Don't over-claim "permanent freezing of funds" without addressing the self-griefing angle.

---

## Final Honest Verdict Per Bug

### Bug 1: PSM `removeBenefactor` mapping persistence

**Validity:** REAL — Solidity `delete` semantics are documented and confirmed. The bug is a code defect, not a design dispute.

**Severity (devil's advocate, realistic):**
- Modal: **Medium** ($5k–$15k) — precondition chain is heavy, "leaked keys" exclusion is live.
- Optimistic: **High** ($25k–$50k) — if triager accepts "silent revocation failure" as High.
- Pessimistic: **Rejected** — if triager applies "leaked keys" exclusion broadly.
- Critical: ~5% probability — only if triager weights end-to-end PoC fund theft heavily.

**Submit recommendation: SUBMIT, with honest framing.**
- Submit as **Medium with High consideration** (not High with Critical consideration).
- Acknowledge the 4-precondition chain.
- Pre-empt "leaked keys" and "admin error" defenses.
- Quote Solidity docs and `disable` vs `remove` divergence prominently.
- Adjust impact claim to "per-epoch-limit-bounded" (not "unbounded eight-figure").

**Adjusted bounty expectation:**
- Expected value (probability-weighted): ~$8k–$12k
- Modal outcome: Medium, $5k–$15k
- Rejection probability: ~35% (vs in-house judge's 8%)
- Acceptance probability: ~65% (vs in-house judge's 92%)

### Bug 2: TON bounce silent-drop

**Validity:** REAL — disassembly is unambiguous. The `s0 POP` bounce branch is a no-op. The pattern divergence with jetton masters is confirmed.

**Severity (devil's advocate, realistic):**
- Modal: **Medium (griefing)** ($5k–$15k) OR **rejected** as user error.
- Optimistic: **High** ($10k–$25k) — if triager accepts "user wallet depletion" as not-user-error.
- Pessimistic: **Rejected** — "user attached insufficient gas = user error."
- Critical: ~5% probability — only if triager accepts "permanent freezing of own funds" as Critical.

**Submit recommendation: HOLD unless bundled with stronger TON finding.**
- If submitting alone: lead with HONEST Medium (griefing) framing, not Critical.
- Acknowledge self-griefing upfront.
- Don't over-claim "permanent freezing of funds" without addressing the self-griefing angle.
- Ideally: build a sandbox PoC first (eliminates "theoretical" rejection angle).
- Ideally: verify on-chain that the TON vault has meaningful TVL.

**Adjusted bounty expectation:**
- Expected value (probability-weighted): ~$2k–$5k
- Modal outcome: Medium, $5k–$15k OR rejected
- Rejection probability: ~50% (vs in-house verifier's implicit ~10%)
- Acceptance probability: ~50% (vs in-house verifier's implicit ~90%)

---

## Key Weaknesses Found (Summary)

### Bug 1 (PSM) weaknesses:
1. **4-step precondition chain** (admin remove + admin re-add + prior compromise + key retention) — too many preconditions for clean Critical.
2. **"Leaked keys" exclusion is a live rejection risk** — the attacker must have been previously trusted.
3. **"Privileged addresses" exclusion is a live rejection risk** — admin must perform 2 actions.
4. **No on-chain evidence** that Ethena ever performs remove→re-add on same address.
5. **PoC uses unrealistic parameters** (`type(uint128).max` for all rate limits, 0 fees).
6. **Impact bounded by per-epoch limits** — "single-tx eight-figure theft" assumes benefactor has $5M+ limits (unverified).
7. **PSM is only 6 weeks old** — might not be live yet (need to verify `isSwapEnabled`).

### Bug 2 (TON) weaknesses:
1. **Self-griefing only** — no attacker profit; no theft of OTHER users' funds demonstrated.
2. **"User error" rejection is STRONG** — user attached insufficient gas is classic user-side responsibility.
3. **No actual production incident in 16 months** — silent fund loss is hard to observe, but absence of incident is still evidence.
4. **No sandbox PoC** — only step-by-step scenario with approximate gas numbers.
5. **Gas calculations are estimates** — exact TON amounts are unverified.
6. **TON vault TVL is unknown** — might be too low to justify Critical.
7. **"Permanent freezing of OWN funds" is borderline scope** — Immunefi typically requires external-attacker-against-victim-funds.

---

## Overall Confidence

**PSM bug:**
- Confidence that the bug is REAL: **95%** (Solidity semantics are documented; code is unambiguous).
- Confidence that the bug is EXPLOITABLE end-to-end: **90%** (PoC passes).
- Confidence that the bug will be ACCEPTED: **65%** (down from in-house 92%).
- Confidence that the bug will be triaged as High or above: **35%** (down from in-house 60%).
- **Overall confidence in submission: MODERATE.** Submit with honest framing.

**TON bug:**
- Confidence that the bug is REAL: **95%** (disassembly is unambiguous).
- Confidence that the bug is EXPLOITABLE: **80%** (conditional on bounce occurring).
- Confidence that the bug will be ACCEPTED: **50%** (down from in-house ~90%).
- Confidence that the bug will be triaged as High or above: **20%** (down from in-house ~50%).
- **Overall confidence in submission: LOW.** Hold unless bundled with stronger TON finding.

**Combined recommendation:**
- Submit PSM bug first (higher acceptance probability, cleaner impact framing).
- HOLD TON bug until either (a) a stronger TON finding is found to bundle with, OR (b) a sandbox PoC is built, OR (c) on-chain verification confirms meaningful TON vault TVL.
- DO NOT submit both simultaneously — if both are rejected, the researcher gets a "low quality" reputation hit. Stagger submissions.

---

## Closing Note

The in-house team has done excellent technical work verifying both bugs. The bugs ARE real. The disagreement is purely about **severity** and **submission strategy** — not about technical correctness.

The in-house verdicts (judge for PSM, verifier for TON) are optimistic on triage outcomes. The devil's advocate view is that:
- PSM bug triage will land at Medium-not-High more often than in-house estimates suggest.
- TON bug triage has a coin-flip rejection risk that in-house estimates underweight.

**Honesty over bounty hopes:** If the bugs are real but weak, say so. Submitting weak bugs as Critical/High risks reputation damage and dispute cost. Submitting weak bugs as Medium with honest framing builds trust with the triage team — which pays off on future, stronger submissions.

---

*End of devil's advocate challenge. Verdict: PSM bug = real, submit as Medium-with-High-consideration; TON bug = real, hold unless bundled with stronger finding.*
