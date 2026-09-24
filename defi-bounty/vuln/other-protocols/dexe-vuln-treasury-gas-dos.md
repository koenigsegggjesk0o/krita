# DeXe Protocol — Unbounded Growth of Credit Withdrawal History Arrays (Gas Griefing / DoS)

**Severity: LOW**
**Area: DAO treasury / Gas consumption / Cross-contract interaction**
**Contracts: GovPoolCredit, GovPool**

---

## 1. Description

`GovPoolCredit.transferCreditAmount` appends to two unbounded arrays on every
withdrawal:

```solidity
// GovPoolCredit.sol, lines 65–68
creditInfo.tokenInfo[currentToken].timestamps.push(block.timestamp);
uint256[] storage history = creditInfo.tokenInfo[currentToken].cumulativeAmounts;
history.push(currentAmount + (history.length == 0 ? 0 : history[history.length - 1]));
```

The `_getCreditBalanceForToken` view performs an `upperBound` (binary search)
over these arrays:

```solidity
// GovPoolCredit.sol, lines 99–113
uint256 index = tokenInfo.timestamps.upperBound(_monthAgo());
```

`EnumerableSet.upperBound` is O(log n), but each withdrawal still costs
~20k gas for the `SSTORE` (zero-to-nonzero slot) plus 5k for subsequent
appends. After many withdrawals, the `timestamps` and `cumulativeAmounts`
arrays can grow to thousands of entries.

Critically, **`setCreditInfo` never prunes these arrays** — it only resets
`monthLimit`:

```solidity
// GovPoolCredit.sol, lines 30–44
function setCreditInfo(…) external {
    uint256 length = creditInfo.tokenList.length;
    for (uint256 i = 0; i < length; i++) {
        delete creditInfo.tokenInfo[creditInfo.tokenList[i]].monthLimit;
    }
    creditInfo.tokenList = tokens;
    for (uint256 i = 0; i < tokens.length; i++) {
        …
        creditInfo.tokenInfo[currentToken].monthLimit = amounts[i];
    }
}
```

So even if the DAO reconfigures the credit limits, the historical withdrawal
records accumulate indefinitely.

## 2. Contract + Function + Line

| Item | Location |
|------|----------|
| `transferCreditAmount` (push) | `contracts/libs/gov/gov-pool/GovPoolCredit.sol:46–72` |
| `_getCreditBalanceForToken` (binary search) | `contracts/libs/gov/gov-pool/GovPoolCredit.sol:94–113` |
| `setCreditInfo` (no pruning) | `contracts/libs/gov/gov-pool/GovPoolCredit.sol:20–44` |

## 3. Attack Scenario

The `monthlyWithdraw` path (validators internal proposal →
`GovValidators.monthlyWithdraw` → `GovPool.transferCreditAmount` →
`GovPoolCredit.transferCreditAmount`) can be called up to the monthly limit
per token. An attacker who controls a validator (or passes a validator
proposal) can make many small withdrawals, each appending to the arrays.
After thousands of withdrawals, every subsequent `transferCreditAmount` and
every `getCreditInfo` view call becomes expensive, potentially exceeding
block gas limits and **permanently bricking the credit withdrawal path**.

## 4. Impact

- **Gas-cost DoS** on the treasury withdrawal path.
- No direct fund theft.
- The arrays are per-token, so the attack must target a specific token, but
  it only takes one token to block that token's withdrawals.

## 5. Severity: **LOW**

Requires governance access (validator proposal) to trigger, so it is a
griefing vector from a semi-trusted role, not an external attacker.

## 6. Recommended Fix

- Prune entries older than 30 days inside `transferCreditAmount` (or
  periodically via a keeper).
- Cap the arrays at a reasonable length and revert if exceeded.
- Alternatively, replace the cumulative-arrays design with a circular
  buffer or a single `(lastResetTimestamp, withdrawnThisCycle)` pair per
  token.
