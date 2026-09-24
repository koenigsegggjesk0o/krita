# IncentivesCollector — Missing ReentrancyGuard on `_claim`

## Metadata

| Field | Value |
|---|---|
| **Severity** | Medium |
| **Area** | Proof-of-Liquidity / Incentives |
| **Contract** | `IncentivesCollector.sol` |
| **Function** | `_claim` (called by `claim` and `claimFees`) |
| **Line** | 167–211 |
| **File** | `src/pol/IncentivesCollector.sol` |
| **Audited commit** | `70e392fc` (2026-08-11) |

## Description

The `IncentivesCollector._claim` function performs multiple external calls —
`safeTransferFrom`, `forceApprove`, `lstAdapter.stake()`, `vault.receiveRewards()`,
and `safeTransfer` to a caller-controlled `_recipient` — yet is **not** guarded by a
`nonReentrant` modifier.

```solidity
function _claim(address _recipient, address[] calldata _incentiveTokens) internal {
    IERC20(WBERA).safeTransferFrom(msg.sender, address(this), payoutAmount);  // external
    uint256[] memory amounts = _splitAmount(payoutAmount);

    // — WBERA staker vault reward —
    IERC20(WBERA).forceApprove(wberaStakerVault, amounts[0]);
    IWBERAStakerVault(wberaStakerVault).receiveRewards(amounts[0]);           // external

    // — LST loop —
    for (uint256 i = 1; i < amounts.length;) {
        IStakerVault vault = IStakerVault(lstStakerVaults[i - 1]);
        if (amounts[i] > 0) {
            ILSTAdapter lstAdapter = ILSTAdapter(lstAdapters[address(vault)]);
            IERC20(WBERA).forceApprove(address(lstAdapter), amounts[i]);
            uint256 lstAmount = lstAdapter.stake(amounts[i]);                  // external, arbitrary
            IERC20(IERC4626(address(vault)).asset()).forceApprove(address(vault), lstAmount);
            vault.receiveRewards(lstAmount);                                   // external
        }
        ...
    }

    // — Incentive token sweep —
    for (uint256 i; i < _incentiveTokens.length;) {
        address token = _incentiveTokens[i];
        uint256 bal = IERC20(token).balanceOf(address(this));
        IERC20(token).safeTransfer(_recipient, bal);                          // external, callback
        ...
    }
    ...
}
```

Neither `claim()` nor `claimFees()` carries `nonReentrant`. Compare with
`BGTIncentiveDistributor.claim` and `RewardVault` which are both `nonReentrant`.

## Attack Scenario

1. An attacker deploys a malicious ERC-777-style token (or any token with a
   `tokensReceived` / `ERC1363` callback) and has it listed as an incentive token
   in a reward vault (incentive whitelisting is done by the factory owner).
2. The attacker calls `IncentivesCollector.claim(attackerContract, [maliciousToken, ...])`.
3. `payoutAmount` of WBERA is pulled from the attacker and split among vaults.
4. During the incentive-token sweep, `safeTransfer(maliciousToken, attackerContract, bal)`
   triggers the attacker's callback.
5. Inside the callback the attacker re-enters `claim()` **before**
   `_setPayoutAmount()` has run (queued payout not yet applied) or before other
   incentive tokens in the array have been swept.
6. The re-entrant call pays `payoutAmount` again but can sweep the *remaining*
   incentive tokens to a second recipient, or exploit the fact that
   `forceApprove` for the WBERA/LST vaults was already set, potentially causing
   double-approval confusion.

Additionally, `lstAdapter.stake()` is a call into an externally-managed adapter
contract. If the adapter's `stake` logic ever calls back (e.g., to notify the
vault or to swap via an AMM pool), the same re-entrancy window opens mid-loop
**before** `receiveRewards` is called, leaving `forceApprove` set and the split
amounts unspent.

## Proof of Concept (pseudo-Solidity)

```solidity
// Malicious incentive token with callback
contract CallbackToken is ERC777 {
    function transfer(address to, uint256 amt) public returns (bool) {
        // normal transfer…
        IIncentivesCollector(msg.sender).claim(address(this), new address[](0));
        // re-entry: payout not yet applied, remaining tokens swept
    }
}
```

## Impact

- **Direct theft:** unlikely because each `claim` call still costs `payoutAmount`
  of WBERA, but the payout-amount update (`_setPayoutAmount`) can be delayed,
  allowing multiple claims at a stale (potentially lower) rate.
- **State corruption:** `forceApprove` overwrites from re-entrant calls can
  cause the first `receiveRewards` / `stake` to pull a different amount than
  intended, mis-routing WBERA to vaults.
- **Standards violation:** the contract deviates from the CEI + nonReentrant
  pattern used by its sibling `BGTIncentiveDistributor` and `RewardVault`.

## Three-Perspective Audit

### 1. Attacker perspective
The re-entrancy window is real but monetising it requires (a) a callback-capable
token being registered as an incentive, or (b) a misbehaving LST adapter. The
most plausible profit vector is sweeping remaining incentive tokens in the
re-entrant call while the outer loop is paused inside a callback.

### 2. Protocol / governance perspective
The code relies on the assumption that all incentive tokens and LST adapters are
well-behaved ERC-20s. This is fragile — new incentive tokens or adapters added
by managers could introduce callbacks. Adding `nonReentrant` is a one-line,
zero-cost hardening.

### 3. Auditor / defence-in-depth perspective
`FeeCollector.claimFees` has the same pattern and also lacks `nonReentrant`.
Both contracts should adopt `ReentrancyGuardUpgradeable` (already imported by
sibling contracts) and annotate `claim`/`claimFees` with `nonReentrant`. No
behavioural change is required; the guard only prevents callback-based
re-entry.

## Recommendation

```solidity
+ import { ReentrancyGuardUpgradeable } from "...";

  contract IncentivesCollector is ..., ReentrancyGuardUpgradeable {
      ...
-     function claim(address _recipient, address[] calldata _incentiveTokens) external whenNotPaused {
+     function claim(address _recipient, address[] calldata _incentiveTokens)
+         external nonReentrant whenNotPaused {
          _claim(_recipient, _incentiveTokens);
      }
-     function claimFees(address _recipient, address[] calldata _feeTokens) external whenNotPaused {
+     function claimFees(address _recipient, address[] calldata _feeTokens)
+         external nonReentrant whenNotPaused {
          _claim(_recipient, _feeTokens);
      }
  }
```
