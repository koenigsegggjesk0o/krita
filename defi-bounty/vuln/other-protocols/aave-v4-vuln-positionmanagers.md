# Aave V4 Audit — Position Managers, SignatureGateway, TokenizationSpoke, Oracle

## Summary

Deep audit of:
- `src/position-manager/{GiverPositionManager,TakerPositionManager,
  ConfigPositionManager,SignatureGateway,NativeTokenGateway,
  PositionManagerBase,PositionManagerIntentBase}.sol`
- `src/spoke/TokenizationSpoke.sol` (ERC-4626 wrapper + EIP-712 permits)
- `src/spoke/AaveOracle.sol`
- `src/utils/{IntentConsumer,NoncesKeyed,Multicall,Rescuable,ExtSload}.sol`
- `src/position-manager/libraries/EIP712Hash.sol`
- `src/spoke/libraries/EIP712Hash.sol`

**No Critical or High severity bug was identified.**  EIP-712 domain
separation is correct, keyed nonces prevent replay, `multicall` correctly
disables itself in `NativeTokenGateway` to prevent `msg.value` reuse, and
the ERC-4626 rounding invariants hold.

Below are the lower-severity observations.

---

## Finding L-7: `NativeTokenGateway.receive()` cannot reject forced native (selfdestruct / coinbase)

- **Severity**: Low
- **Contract / Function / Line**: `src/position-manager/NativeTokenGateway.sol`,
  `receive`, lines 33–35; `Rescuable.rescueNative`, `src/utils/Rescuable.sol`
  line 25.

### Description

```solidity
receive() external payable {
  require(msg.sender == NATIVE_TOKEN_WRAPPER, UnsupportedAction());
}
```

`receive()` only fires on ordinary transfers; native sent via
`SELFDESTRUCT` or block-producer coinbase payments bypasses `receive()`
entirely and lands in the contract anyway.  Such stuck native is only
recoverable via `rescueNative` (owner-gated through
`PositionManagerBase._rescueGuardian → owner()`).

### Attack scenario / Impact

* Griefing: an attacker force-sends native to the gateway.  The funds are
  not stolen (only the owner can rescue), but they are locked until the
  owner acts.
* No fund loss; low operational nuisance.

### 3-perspective audit

1. **Exploitability**: None beyond griefing.
2. **Economic impact**: Zero (owner can always rescue).
3. **Fix recommendation**: None required; the `Rescuable` pattern is the
   correct mitigation.

---

## Finding L-8: `TokenizationSpoke.permit` consumes the nonce before signature verification

- **Severity**: Informational (revert rolls back state)
- **Contract / Function / Line**: `src/spoke/TokenizationSpoke.sol`,
  `permit`, lines 197–221.

### Description

```solidity
bytes32 digest = _hashTypedData(
  keccak256(
    abi.encode(
      EIP712Hash.PERMIT_TYPEHASH,
      owner, spender, value,
      _useNonce({owner: owner, key: PERMIT_NONCE_NAMESPACE}),   // ← side effect
      deadline
    )
  )
);
require(owner == ECDSA.recover({hash: digest, v: v, r: r, s: s}), InvalidSignature());
```

`_useNonce` is invoked *inside* the `keccak256(abi.encode(...))` argument
list, i.e. before the `require` that checks the signature.  An invalid
signature therefore increments `nonces[owner][0]` and immediately reverts,
rolling back the increment.  This is **safe** (Solidity reverts undo all
state) but reads as a foot-gun; reviewers of similar code patterns
sometimes mistake it for a nonce-burn griefing vector.

### 3-perspective audit

1. **Exploitability**: None — the revert restores the nonce.
2. **Economic impact**: None.
3. **Fix recommendation**: Hoist `_useNonce` to *after* the signature
   check for clarity; behaviour is identical.

---

## Finding L-9: `TokenizationSpoke.renounceAllowance` lets any spender zero their *own* allowance from an arbitrary owner

- **Severity**: Informational
- **Contract / Function / Line**: `src/spoke/TokenizationSpoke.sol`,
  `renounceAllowance`, lines 229–234.

### Description

```solidity
function renounceAllowance(address owner) external override {
  if (allowance({owner: owner, spender: msg.sender}) == 0) return;
  _approve({owner: owner, spender: msg.sender, value: 0});
}
```

`msg.sender` is the spender; the function only ever writes
`_allowances[owner][msg.sender] = 0`.  It cannot touch any other spender's
allowance and is therefore a safe self-service renunciation.  Listed only
because the signature `renounceAllowance(address owner)` superficially
looks like it might let a caller clear *someone else's* allowance — it
does not.

### 3-perspective audit

1. **Exploitability**: None.
2. **Economic impact**: None.
3. **Fix recommendation**: None; behaviour is correct.

---

## Finding L-10: `AaveOracle` does not validate price-feed staleness or roundness

- **Severity**: Low (explicitly out-of-scope per Sherlock bounty terms)
- **Contract / Function / Line**: `src/spoke/AaveOracle.sol`,
  `_getSourcePrice`, lines 80–88.

### Description

```solidity
int256 price = source.latestAnswer();
require(price > 0, InvalidPrice(reserveId));
return uint256(price);
```

Only a non-zero check is performed.  No staleness window, no
`latestRoundData` round-number check, no heartbeat enforcement.  A frozen
or stale Chainlink feed would return a stale `latestAnswer` and silently
propagate into `toValue` / liquidation math.

### Attack scenario / Impact

* The Sherlock scope explicitly states "Chainlink feeds are assumed to
  operate correctly … out of scope".  Reporting this only for completeness.
* If a feed ever goes stale in production, every price-dependent operation
  (borrow HF check, liquidation bonus, risk-premium recomputation) would
  use a stale price until the feed recovers.

### 3-perspective audit

1. **Exploitability**: Out of scope.
2. **Economic impact**: Out of scope; documented for risk-team awareness.
3. **Fix recommendation**: Optional defensive `latestRoundData` +
   `updatedAt` staleness check, gated by a configurable heartbeat, even
   though the bounty excludes oracle misbehaviour.

---

## Areas verified clean (no bug found)

- **EIP-712 domain separation**: each of `Spoke`, `TokenizationSpoke`,
  `SignatureGateway`, `TakerPositionManager`, `ConfigPositionManager`
  overrides `_domainNameAndVersion` with a distinct name, so a signature
  valid for one contract cannot be replayed on another.
- **Keyed nonces (ERC-4337 style)**: `_useCheckedNonce` enforces
  `keyNonce == _pack(key, _nonces[owner][key]++)`, preventing both
  replay and cross-key confusion.  `PERMIT_NONCE_NAMESPACE = 0` in
  `TokenizationSpoke` keeps permit nonces in the same stream as
  `*WithSig` nonces — but each intent's hash binds the full packed
  nonce, so collisions are impossible.
- **`multicall` `msg.value` reuse**: `NativeTokenGateway._multicallEnabled`
  returns `false`, blocking the inherited `Multicall.multicall`.  All other
  position managers disable `msg.value` paths, so `delegatecall` reuse is
  not exploitable.
- **`SignatureGateway.borrowWithSig` / `withdrawWithSig`**: borrowed /
  withdrawn underlying is always forwarded to `params.onBehalfOf` (the
  signer), never to `msg.sender`.  A relayer cannot intercept funds.
- **`GiverPositionManager.repayOnBehalfOf`** correctly forbids
  `amount == type(uint256).max` (which would otherwise transfer the user's
  full balance via `safeTransferFrom` and then be capped inside
  `Spoke.repay`, stranding the remainder in the GiverPositionManager).
- **`TakerPositionManager.withdrawOnBehalfOf` allowance debit**: proved
  `suppliedAssetsBefore − suppliedAssetsAfter ≥ withdrawnAmount` for every
  share-price `R ≥ 0`, so the spender can never withdraw more than their
  allowance permits (the debit is conservative).
- **ERC-4626 rounding invariants in `TokenizationSpoke`**:
  - `deposit`: `previewDeposit(assets)` (floor shares) == shares minted by
    `Hub.add` (floor shares). ✓
  - `mint`: `previewMint(shares)` (ceil assets) ⇒ `Hub.add` mints exactly
    `shares` because `toAddedSharesDown(toAddedAssetsUp(shares)) == shares`
    for all share prices ≥ 1 (always true post-fee-minting). ✓
  - `withdraw`: `previewWithdraw(assets)` (ceil shares) == shares burned by
    `Hub.remove` (ceil shares). ✓
  - `redeem`: `previewRedeem(shares)` (floor assets) ⇒ `Hub.remove` debits
    ≤ shares (no user-side underflow). ✓
- **`ConfigPositionManager` delegated permissions**: every `*OnBehalfOf`
  path re-checks `_getPermissions(spoke, delegator=onBehalfOf,
  delegatee=msg.sender).canX()` — no permission escalation.
- **`renouncePositionManagerRole` / `renounceAllowance`**: only allow a
  manager/spender to revoke *their own* relationship; cannot grief others.
