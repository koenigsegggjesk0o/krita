# Verification Report — Berachain IncentivesCollector (Medium severity)

**Task ID:** `berachain-verify-medium`
**Audited repo:** `/home/z/berachain/` (commit `70e392fc`, 2026-08-11 per vuln files)
**Source under test:** `src/pol/IncentivesCollector.sol`
**Tooling:** Foundry v1.8.3, solc 0.8.26, OZ 5.1.0, solady v0.1.x
**PoC project:** `/home/z/fkr-step1/defi-bounty/vuln/other-protocols/poc-berachain-medium/`

---

## Summary

| # | Bug | Severity | PoC | Verdict |
|---|---|---|---|---|
| 1 | IncentivesCollector missing `nonReentrant` on `_claim` | Medium | `test/ReentrancyPoC.t.sol` (2 tests, PASS) | **CONFIRMED** |
| 2 | Free incentive tokens when `totalStake == 0` | Medium | `test/FreeTokensPoC.t.sol` (2 tests, PASS) | **CONFIRMED** |

Both bugs reproduce against the actual Berachain `IncentivesCollector.sol` source
(copied verbatim, only import paths re-rooted for an isolated Foundry project).

```text
$ forge test
Ran 2 tests for test/FreeTokensPoC.t.sol:FreeTokensPoC
[PASS] test_freeClaimWhenLstAdapterRateIsZero() (gas: 297620)
[PASS] test_freeClaimWhenTotalStakeZero() (gas: 502804)
Ran 2 tests for test/ReentrancyPoC.t.sol:ReentrancyPoC
[PASS] test_baselineNoReentrancy() (gas: 668448)
[PASS] test_reenterClaimBeforePayoutUpdate() (gas: 751855)
4 tests passed; 0 failed; 0 skipped
```

---

## Bug 1 — Missing `nonReentrant` on `_claim`

### Claim
`IncentivesCollector._claim` performs multiple external calls
(`safeTransferFrom`, `forceApprove`, `lstAdapter.stake()`,
`vault.receiveRewards()`, `safeTransfer` to a caller-controlled
`_recipient`) but is not guarded by a `nonReentrant` modifier. Sibling
contracts `BGTIncentiveDistributor.claim` and `RewardVault.*` are
`nonReentrant`. `FeeCollector.claimFees` has the same gap.

### Code verification (against `/home/z/berachain/src/pol/IncentivesCollector.sol`)

| Line | Code | Notes |
|---|---|---|
| 23 | `contract IncentivesCollector is IIncentivesCollector, PausableUpgradeable, AccessControlUpgradeable, UUPSUpgradeable` | **No** `ReentrancyGuardUpgradeable` in inheritance chain |
| 149 | `function claimFees(address _recipient, address[] calldata _feeTokens) external whenNotPaused` | No `nonReentrant` |
| 154 | `function claim(address _recipient, address[] calldata _incentiveTokens) external whenNotPaused` | No `nonReentrant` |
| 167–211 | `_claim(...)` internal body | 5+ external calls; recipient-controlled; re-enterable via callback-capable incentive token |
| 169 | `IERC20(WBERA).safeTransferFrom(msg.sender, address(this), payoutAmount);` | Pulls WBERA from caller |
| 188 | `uint256 lstAmount = lstAdapter.stake(amounts[i]);` | External call to adapter (governance-controlled but extensible) |
| 199–207 | Sweep loop: `safeTransfer(_recipient, bal)` for each `_incentiveTokens[i]` | Re-entry vector: a callback-capable token (ERC-777 / ERC-1363) can re-enter `claim` |
| 210 | `if (queuedPayoutAmount != 0) _setPayoutAmount();` | **Runs at END of `_claim`** — re-entrant call sees stale `payoutAmount` |

**Confirmed by grep** (`/home/z/berachain/src/pol/IncentivesCollector.sol`):
```
$ grep -nE "ReentrancyGuard|nonReentrant" src/pol/IncentivesCollector.sol
(no matches)
```
For comparison, `BGTIncentiveDistributor.sol` line 6 imports
`ReentrancyGuardUpgradeable`, line 21–27 lists it in the `is` clause, line 67
calls `__ReentrancyGuard_init()`, and line 163 decorates `claim` with
`nonReentrant`. `RewardVault.sol` has 11 `nonReentrant` decorators. The
discrepancy is real.

### PoC

**File:** `test/ReentrancyPoC.t.sol`
**Attacker contract:** `src/mocks/MaliciousCallbackToken.sol` — an ERC-20
whose `transfer(to, amount)` overrides `super.transfer` and then, when
`msg.sender == address(ic)`, calls `ic.claim(address(this), reentryTokens)`.

**Test scenario (`test_reenterClaimBeforePayoutUpdate`):**

1. WBERA vault seeded with 100 WBERA stake so `_splitAmount` distributes.
2. IncentivesCollector pre-funded with 2 000 USDC (incentive token).
3. Governance queues a payout INCREASE: 1 WBERA → 100 WBERA.
4. Attacker (`MaliciousCallbackToken`) funded with 2 WBERA and approves IC.
5. Attacker arms the malicious token to re-enter `claim` with `[usdc]`.
6. Attacker calls `ic.claim(mal, [mal, usdc])`.
7. Outer `_claim`:
   - pulls **1 WBERA** (old payout) from `mal`;
   - distributes 1 WBERA to vault;
   - sweep loop i=0: transfers `mal` (0 balance) → `mal.transfer` fires callback.
8. Callback re-enters `ic.claim(mal, [usdc])` (the `ReentrySucceeded` event fires,
   `CallbackFired` reports `payoutUsedByReentry = 1e18` — the OLD rate).
9. Re-entrant `_claim`:
   - pulls **ANOTHER 1 WBERA** (still old payout, because
     `_setPayoutAmount` hasn't run yet);
   - distributes 1 WBERA to vault;
   - sweeps 2 000 USDC to `mal`;
   - at the end, runs `_setPayoutAmount` → `payoutAmount = 100 WBERA`,
     `queuedPayoutAmount = 0`.
10. Outer `_claim` resumes sweep: i=1 (USDC), balance is now 0, transfers 0.
11. Outer `_claim` reaches `_setPayoutAmount` check: `queuedPayoutAmount == 0`,
    no-op.

**Assertions (all PASS):**

| Assertion | Expected | Got |
|---|---|---|
| Attacker WBERA spent | 2 WBERA (1 outer + 1 re-entrant, both at OLD rate) | ✅ 2 WBERA |
| Attacker received USDC | 2 000 USDC (swept by re-entrant call) | ✅ 2 000 USDC |
| `payoutAmount` after | 100 WBERA (queued update eventually applied) | ✅ 100 WBERA |
| `queuedPayoutAmount` after | 0 (cleared by re-entrant call's `_setPayoutAmount`) | ✅ 0 |
| WBERA vault received | 102 WBERA (100 seed + 1 outer + 1 re-entrant) | ✅ 102 WBERA |
| IC leftover WBERA | 0 (both payouts fully distributed) | ✅ 0 |

**Baseline test (`test_baselineNoReentrancy`)** shows that without
re-entrancy, the same attacker would pay 1 + 100 = **101 WBERA** for two
USDC batches (the second claim forces the NEW rate). With re-entrancy, the
attacker pays 1 + 1 = **2 WBERA** for the same two batches — **saves 99 WBERA
per re-entrant claim** when a payout INCREASE is queued.

### Three-perspective re-verification

**1. Attacker perspective.** The re-entrancy window is real and exploitable
*if and only if* the attacker can register a callback-capable token
(ERC-777 / ERC-1363) as an incentive token, OR if any LST adapter's `stake`
implementation calls back. Incentive-token registration happens in the
`RewardVaultFactory` via `whitelistRewardToken` (governance-gated), so an
attacker needs governance to whitelist their malicious token — non-trivial
but not impossible (a "legitimate-looking" token could later be revealed as
callback-capable). When combined with a queued payout INCREASE, the attacker
extracts an extra claim at the stale low rate. The dollar value of the saved
payout equals `(newPayout − oldPayout)` per re-entrant claim.

**2. Protocol / governance perspective.** The contract relies on the
assumption that all incentive tokens and LST adapters are well-behaved
ERC-20s. This is fragile: new incentive tokens or adapters added by managers
could introduce callbacks (intentionally or via a token upgrade). Adding
`nonReentrant` is a one-line, zero-cost hardening. The pattern is already
used by `BGTIncentiveDistributor` and `RewardVault` — same codebase — so the
omission is an oversight, not a design choice.

**3. Auditor / defence-in-depth perspective.** The exploit requires a
confluence of conditions: (a) callback-capable token listed as an incentive,
(b) governance has queued a payout change, (c) attacker monitors the queue
and strikes before the next legitimate claim. The probability of (a) is low
today (whitelist is curated) but rises over time as more tokens are added.
The probability of (b)+(c) is non-trivial because payout changes are
governance events that an attacker can monitor on-chain. The fix
(`nonReentrant` modifier) is mechanical and risk-free. Severity **Medium**
is appropriate — the vulnerability is real and exploitable under realistic
conditions, but the attacker needs a malicious token to be whitelisted
(which is gated).

### Verdict: **CONFIRMED** (Medium)

---

## Bug 2 — Free incentive tokens when `totalStake == 0`

### Claim
When `wberaStakerVault.totalAssets() == 0` and all LST vaults are empty (or
their adapter returns `getRate() == 0`), `_splitAmount` short-circuits and
returns an all-zero array. `_claim` still pulls `payoutAmount` of WBERA from
the caller but never distributes it to any vault. The caller can then
include `WBERA` itself in the `_incentiveTokens` array and the sweep loop
will transfer the **entire** WBERA balance (including the just-paid payout)
back to the caller. Net cost: 0. Net gain: every accumulated incentive
token in the contract.

### Code verification (against `/home/z/berachain/src/pol/IncentivesCollector.sol`)

| Line | Code | Notes |
|---|---|---|
| 169 | `IERC20(WBERA).safeTransferFrom(msg.sender, address(this), payoutAmount);` | Pulls WBERA from caller — **unconditional** |
| 170 | `uint256[] memory amounts = _splitAmount(payoutAmount);` | Returns all-zeros when `totalStake == 0` |
| 223–277 | `_splitAmount` body | See line 254 short-circuit |
| 230 | `stakes[0] = IERC4626(wberaStakerVault).totalAssets();` | WBERA vault stake |
| 236 | `uint256 stake = vault.totalAssets();` | LST vault stake |
| 240 | `uint256 value = (stake * rate) / 1e18;` | `rate == 0` → `value == 0` (LST contribution nullified) |
| 254 | `if (totalStake == 0 || amount == 0) { return amounts; }` | **All-zeros return; payout NOT distributed** |
| 250–253 | Developer comment | "We are aware of this issue and does not consider it a problem given totalStake being 0 is not practically, possible situation as all the LST vaults are deployed with initial supply." — this is the only defence |
| 177–179 | `forceApprove(wberaStakerVault, amounts[0]); receiveRewards(amounts[0]);` | `amounts[0] == 0` → no-op distribution; vault receives nothing |
| 199–207 | Sweep loop: `safeTransfer(_recipient, balanceOf(this))` for each `_incentiveTokens[i]` | **No filter** — caller can include `WBERA` itself, sweeping the just-paid payout back |

**Key observation:** there is no check preventing `WBERA` from appearing in
`_incentiveTokens`. The sweep loop transfers the contract's *entire balance*
of each listed token — including WBERA — to `_recipient`.

### PoC

**File:** `test/FreeTokensPoC.t.sol`

**Test scenario (`test_freeClaimWhenTotalStakeZero`):**

1. Default `setUp` state: `wberaVault.totalAssets() == 0` and
   `ic.lstStakerVaultsLength() == 0`. Therefore `totalStake == 0`.
2. IncentivesCollector pre-funded with 10 000 USDC and 5 000 HONEY
   (simulating accrued incentives redirected from reward vaults).
3. Attacker funded with exactly `payoutAmount` (1 WBERA) and approves IC.
4. Attacker calls `ic.claim(attacker, [WBERA, USDC, HONEY])`.
5. `_claim`:
   - pulls 1 WBERA from attacker — IC now holds 1 WBERA;
   - `_splitAmount` returns all-zeros (totalStake == 0);
   - `receiveRewards(0)` is a no-op — vault gets nothing;
   - sweep loop:
     - i=0: WBERA balanceOf(IC) = 1 WBERA → `transfer(attacker, 1e18)`
       (attacker recovers their payout);
     - i=1: USDC balanceOf(IC) = 10 000 USDC → `transfer(attacker, 10_000e18)`;
     - i=2: HONEY balanceOf(IC) = 5 000 HONEY → `transfer(attacker, 5_000e18)`.

**Assertions (all PASS):**

| Assertion | Expected | Got |
|---|---|---|
| Attacker WBERA balance after | 1 WBERA (full payout recovered) | ✅ 1 WBERA |
| Attacker USDC balance after | 10 000 USDC (stolen) | ✅ 10 000 USDC |
| Attacker HONEY balance after | 5 000 HONEY (stolen) | ✅ 5 000 HONEY |
| IC USDC balance after | 0 (drained) | ✅ 0 |
| IC HONEY balance after | 0 (drained) | ✅ 0 |
| IC WBERA balance after | 0 (no leftover) | ✅ 0 |
| `wberaVault.totalAssets()` after | 0 (vault received NOTHING) | ✅ 0 |

**Net cost to attacker: 0 WBERA. Net gain: 15 000 incentive tokens.**

### Three-perspective re-verification

**1. Attacker perspective.** The attack is permissionless and costs only
gas. The attacker monitors `wberaVault.totalAssets()` (a public view
computation) and the LST adapter rates. The moment `totalStake` collapses to
zero, the attacker strikes. The WBERA recovery vector (including WBERA in
the incentive-token list) is **not** mentioned in the developer comment and
amplifies the impact from "payout stuck in contract" to "incentives stolen
for free". The attacker can repeat the attack each time incentives accrue
while `totalStake == 0`, draining the contract continuously.

**2. Protocol perspective.** The developer comment acknowledges the
`totalStake == 0` edge case but dismisses it as "not practically possible"
because "all the LST vaults are deployed with initial supply". This defence
is insufficient for two reasons:
   - **WBERA vault starts seeded but is drainable.** The
     `IncentivesCollectorDeployer` seeds `wberaStakerVault` with
     `INITIAL_DEPOSIT_AMOUNT = 10e18` WBERA (line 17 of
     `IncentivesCollectorDeployer.sol`), with shares minted to governance.
     Governance can withdraw (7-day cooldown via `WBERAStakerVault.queueWithdraw`),
     reducing `totalAssets()` to 0. This is realistic during migration,
     launch wind-down, or extreme market events.
   - **LST adapter `getRate() == 0` nullifies LST stake.** Even if LST vaults
     hold stake, a misconfigured, paused, or freshly-deployed adapter whose
     rate oracle hasn't initialised will report `rate == 0`. Line 240:
     `value = (stake * rate) / 1e18` becomes 0. The LST stake is silently
     ignored, contributing 0 to `totalStake`.

   When either condition coincides with `wberaVault.totalAssets() == 0`,
   `totalStake` collapses to 0 and the free-claim path opens. The WBERA
   recovery vector turns this into a direct theft of all incentive tokens,
   not just a "stuck payout".

**3. Auditor perspective.** Two minimal fixes resolve this:
   1. **Exclude WBERA from the incentive-token sweep** (or burn excess WBERA
      when `totalStake == 0`):
      ```solidity
      for (uint256 i; i < _incentiveTokens.length;) {
          address token = _incentiveTokens[i];
          if (token == WBERA) { unchecked { ++i; } continue; } // never sweep payout token
          ...
      }
      ```
   2. **Refund the payout to `msg.sender`** when `_splitAmount` returns
      all-zeros, so the caller cannot profit:
      ```solidity
      uint256[] memory amounts = _splitAmount(payoutAmount);
      if (_allZero(amounts)) {
          IERC20(WBERA).safeTransfer(msg.sender, payoutAmount); // refund
          // still sweep incentive tokens? Or revert? Decision for the team.
      }
      ```
   Alternatively, revert when `totalStake == 0`. Given that legitimate
   claims also don't make sense when there are no stakers to receive the
   payout, reverting is the cleanest fix.

### Verdict: **CONFIRMED** (Medium)

The developer comment proves awareness of the edge case, but the dismissal
("not practically possible") is incorrect, and the WBERA-recovery amplification
vector (which turns a stuck-payout into free-token theft) is not addressed at
all.

---

## Submission Drafts

### Bug 1 submission draft

> **Title:** `IncentivesCollector._claim` lacks `nonReentrant`, allowing
> stale-payout re-entrancy via callback-capable incentive tokens
>
> **Severity:** Medium
>
> **Summary:** `IncentivesCollector.claim` and `claimFees` are not guarded
> by `nonReentrant` despite performing multiple external calls — including
> `safeTransfer` of caller-listed incentive tokens to a caller-controlled
> recipient. Sibling contracts `BGTIncentiveDistributor.claim` and
> `RewardVault.{stake,withdraw,accountIncentives,...}` all use
> `nonReentrant`. A callback-capable incentive token (ERC-777 / ERC-1363)
> can re-enter `_claim` during the sweep loop, before `_setPayoutAmount`
> runs. If governance has queued a payout INCREASE, the re-entrant call
> pulls WBERA at the stale (lower) rate, allowing the attacker to claim
> multiple incentive-token batches at the old rate before the increase
> takes effect.
>
> **PoC:** `test/ReentrancyPoC.t.sol::test_reenterClaimBeforePayoutUpdate`
> — attacker pays 2 WBERA (1 outer + 1 re-entrant, both at old rate of
> 1 WBERA) for two USDC batches; baseline (without re-entrancy) would cost
> 1 + 100 = 101 WBERA. Attacker saves 99 WBERA per re-entrant claim when
> governance queues a 1→100 WBERA payout increase.
>
> **Fix:** Add `ReentrancyGuardUpgradeable` to the inheritance chain, call
> `__ReentrancyGuard_init()` in `initialize`, and decorate both `claim` and
> `claimFees` with `nonReentrant`. Same fix for `FeeCollector.claimFees`.

### Bug 2 submission draft

> **Title:** `IncentivesCollector._claim` lets attacker steal all incentive
> tokens for free when `totalStake == 0`
>
> **Severity:** Medium
>
> **Summary:** When `wberaVault.totalAssets() == 0` and no LST vault
> contributes stake (either because no LST vaults are configured, or
> because their adapters return `getRate() == 0`), `_splitAmount`
> short-circuits to an all-zero array (line 254 of
> `src/pol/IncentivesCollector.sol`). `_claim` still pulls `payoutAmount`
> of WBERA from the caller but never distributes it to any vault. The
> caller can include `WBERA` itself in the `_incentiveTokens` array — the
> sweep loop has no filter on the payout token — and recover the just-paid
> payout, while also sweeping every other accrued incentive token. Net
> cost to the attacker: 0 WBERA. Net gain: the entire incentive treasury.
>
> The inline developer comment acknowledges the `totalStake == 0` edge case
> but dismisses it as "not practically possible" because "LST vaults are
> deployed with initial supply". This defence is incorrect: (a) the WBERA
> vault is drainable (governance can withdraw its initial-deposit shares
> via the 7-day cooldown), and (b) an LST adapter returning `getRate() == 0`
> nullifies that vault's contribution even if it holds stake (line 240:
> `value = (stake * rate) / 1e18` → 0).
>
> **PoC:** `test/FreeTokensPoC.t.sol::test_freeClaimWhenTotalStakeZero` —
> with `wberaVault.totalAssets() == 0` and no LST vaults, attacker calls
> `ic.claim(attacker, [WBERA, USDC, HONEY])` and recovers their 1 WBERA
> payout while stealing 10 000 USDC and 5 000 HONEY. WBERA vault receives
> 0 payout.
>
> **Fix (two-part):**
> 1. Filter `WBERA` out of the incentive-token sweep (never sweep the
>    payout token).
> 2. Revert when `totalStake == 0` (no stakers to receive payout → no
>    legitimate reason to claim) OR refund the payout to `msg.sender` when
>    `_splitAmount` returns all-zeros.

---

## Files produced

- `/home/z/fkr-step1/defi-bounty/vuln/other-protocols/VERIFICATION_berachain_medium.md` (this file)
- `/home/z/fkr-step1/defi-bounty/vuln/other-protocols/poc-berachain-medium/` (standalone Foundry project)
  - `foundry.toml`, `remappings.txt`
  - `src/IncentivesCollector.sol` (verbatim copy of Berachain source, only import paths re-rooted)
  - `src/IIncentivesCollector.sol`, `src/IPOLErrors.sol`, `src/IWBERAStakerVault.sol`,
    `src/IStakerVault.sol`, `src/ILSTAdapter.sol`, `src/IStakerVaultWithdrawalRequest.sol`,
    `src/IWBERAStakerVaultWithdrawalRequest.sol`, `src/IStakingRewardsErrors.sol`,
    `src/Utils.sol` (verbatim copies)
  - `src/mocks/WBERAToken.sol`, `src/mocks/SimpleERC20.sol`,
    `src/mocks/MockWBERAStakerVault.sol`, `src/mocks/MockLST.sol` (incl. `MockLSTStakerVault`, `MockLSTAdapter`),
    `src/mocks/MaliciousCallbackToken.sol`
  - `test/IncentivesCollectorHarness.sol` (shared harness; deploys IC behind UUPS proxy, etches WBERA at 0x69...69)
  - `test/FreeTokensPoC.t.sol` (Bug 2 PoC; 2 tests)
  - `test/ReentrancyPoC.t.sol` (Bug 1 PoC; 2 tests)

## Reproduction

```bash
cd /home/z/fkr-step1/defi-bounty/vuln/other-protocols/poc-berachain-medium
export PATH=/home/z/.foundry/bin:$PATH
forge test -vv
# 4 tests passed; 0 failed; 0 skipped
```
