# DeXe Protocol — `GovPool.tryExecute` Executes Arbitrary Calls with GovPool Funds Inside a Reverted Frame

**Severity: LOW (informational, but worth documenting)**
**Area: Proposal execution / Cross-contract interaction**
**Contracts: GovPool, GovPoolExecute**

---

## 1. Description

`GovPool.tryExecute` is an **permissionless** `external` function that
simulates proposal execution:

```solidity
// GovPool.sol, lines 161–167
function tryExecute(ProposalAction[] calldata actions) external returns (bool) {
    try actions.tryExecute() {
        revert();
    } catch (bytes memory reason) {
        return abi.decode(reason, (bool));
    }
}
```

The library function `GovPoolExecute.tryExecute` performs low-level calls
**from the GovPool's context**:

```solidity
// GovPoolExecute.sol, lines 79–91
function tryExecute(IGovPool.ProposalAction[] calldata actions) external {
    uint256 length = actions.length;
    for (uint256 i = 0; i < length; i++) {
        (bool success, ) = actions[i].executor.call{value: actions[i].value}(actions[i].data);
        if (!success) {
            _throwRevert(false);
        }
    }
    _throwRevert(true);
}
```

Because the library function **always** reverts (via `_throwRevert`), the
state changes — including ETH transfers via `call{value: actions[i].value}`
— are rolled back by the `try/catch`. The function therefore behaves as a
read-only simulation and does **not** actually move funds.

**However**, two concerns remain:

1. **Gas griefing**: any caller can supply a large `actions` array with
   expensive `data` payloads. The GovPool executes them (consuming the
   caller's gas) and then reverts. This is a self-inflicted griefing vector
   (the caller pays), but it can be used to probe internal contract
   behaviour or to measure gas for a real attack.

2. **Reentrancy surface inside the reverted frame**: during each
   `executor.call`, the called contract receives control with
   `msg.sender == GovPool` and `msg.value == actions[i].value` (from the
   GovPool's balance). A malicious executor could re-enter the GovPool and
   call view functions to read state that is normally only accessible
   mid-execution. While the reentrant state changes are reverted, any
   **return data** captured by the attacker's contract persists in memory
   and can be used after `tryExecute` returns. This is an information-leak
   channel, not a fund-theft channel.

## 2. Contract + Function + Line

| Item | Location |
|------|----------|
| `GovPool.tryExecute` | `contracts/gov/GovPool.sol:161–167` |
| `GovPoolExecute.tryExecute` | `contracts/libs/gov/gov-pool/GovPoolExecute.sol:79–91` |
| `_throwRevert` | `contracts/libs/gov/gov-pool/GovPoolExecute.sol:111–116` |

## 3. Impact

No direct fund theft. The `try/catch` correctly rolls back state. The
residual risks are:

- **Gas griefing** (caller-inflicted, low impact).
- **State probing via reentrancy** — an attacker can observe GovPool state
  mid-simulation that would exist if a real proposal were executed, then
  use that information to craft a more effective real proposal. This is an
  reconnaissance aid for the validation-bypass attack described in
  `dexe-vuln-proposal-validation-bypass.md`.

## 4. Severity: **LOW**

Informational. No direct loss of funds.

## 5. Recommended Mitigation

- Consider gating `tryExecute` behind `onlyBABTHolder` (it is currently
  unrestricted) to reduce the reentrancy-probing surface.
- Document that `tryExecute` is a simulation-only function and must never
  be used as a security control.
