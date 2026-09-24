# Ethena OFT Contracts — Deep LayerZero Security Analysis

**Analyst:** Opus (DeFi security researcher, LayerZero cross-chain specialist)
**Task ID:** eth-oft-deep
**Date:** 2026-09-25
**Scope:** 6 Ethena OFT contracts + 3 dependency libs sourced via Blockscout multi-file API
**Solidity:** 0.8.22 (Ethena OFTs) / 0.8.20 (LayerZero OApp) / 0.8.19 (StakedUSDe innerToken)
**LayerZero:** V2 (Endpoint V2 @ 0x1a44076050125825900e736c501f859c50fE728c on mainnet)

---

## Executive Summary

All 6 Ethena OFT contracts and 3 dependency libraries were read line-by-line, alongside
their LayerZero V2 OApp/OFT base classes (`OFTCore`, `OFT`, `OFTAdapter`, `OAppReceiver`,
`OAppCore`, `OAppSender`, `OFTMsgCodec`) and the innerToken `StakedUSDe` / `StakedUSDeV2`.

**No CRITICAL or HIGH severity vulnerability was found.** The integration follows the
canonical LayerZero V2 OFT pattern, the rate limiter is correctly wired into `_debit`,
the peer/endpoint checks in `lzReceive` are inherited unchanged from `OAppReceiver`,
and the message codec is the standard `OFTMsgCodec` (no custom encoding that could be
tampered with). Replay protection is delegated to the Endpoint V2 (per-path nonce
uniqueness via GUID), and the OFT does not override `nextNonce` (returns 0 = no
additional OApp-level ordering, which is the default and safe).

The most noteworthy findings are **Low** severity robustness gaps in the
`StakedUSDeOFTAdapter._credit` failure-handling path (the highest-TV L contract at
~$416M) and a CEI smell in `StakedUSDeOFT.redistributeBlackListedFunds`. The remaining
items are **Informational** design notes (per-path rate-limit multi-hop bypass,
cross-chain blacklist lag, payable `lzReceive` with no `receive()`).

No PoC file is written because no critical bug was identified. Findings are documented
below with severity, trigger conditions, and impact.

### Severity roll-up

| # | Finding | Severity | Contract |
|---|---------|----------|----------|
| F-1 | `abi.decode` of revert data before `success` check in `_credit` | Low | StakedUSDeOFTAdapter |
| F-2 | `redistributeBlackListedFunds` temporarily un-blacklists during `_transfer` | Low | StakedUSDeOFT |
| F-3 | Per-path rate limit bypass via multi-hop routing | Informational | All (RateLimiter) |
| F-4 | Cross-chain blacklist lag (per-chain mapping, no sync) | Informational | StakedUSDeOFT, StakedUSDeOFTAdapter |
| F-5 | No inbound rate limit on `_credit` (griefing bounded by adapter balance) | Informational | All Adapters |
| F-6 | Only `FULL_RESTRICTED_STAKER_ROLE` checked; soft-blacklist recipients cause message stick | Informational | StakedUSDeOFTAdapter |
| F-7 | `_setRateLimits` does not reset `amountInFlight`; admin can lift cap instantly | Informational | RateLimiter |
| F-8 | `setRateLimiter` accepts `address(0)` (disables rateLimiter role; safe) | Informational | USDeOFT, USDeOFTAdapter, ENAOFT, ENAOFTAdapter, StakedUSDeOFT* |
| F-9 | `lzReceive` is `payable`; contract has no `receive()` → native dust stuck | Informational | All |
| F-10 | Owner-redirect path reverts if `owner()` is on `FULL_RESTRICTED_STAKER_ROLE` | Informational | StakedUSDeOFT, StakedUSDeOFTAdapter |

---

## Contracts & Dependencies Analyzed

### Primary scope (6 Ethena OFT contracts in `contracts/`)

| # | Contract | File | Loc | Mainnet addr | TVL |
|---|----------|------|-----|--------------|-----|
| 1 | StakedUSDeOFTAdapter | `contracts/StakedUSDeOFTAdapter.sol` | 57 | 0x211Cc4DD073734dA055fbF44a2b4667d5E5fE5d2 | ~$416M |
| 2 | StakedUSDeOFT | `contracts/StakedUSDeOFT.sol` | 112 | (L2 only — Arbitrum 0x211Cc4…e5d2 via CREATE2) | — |
| 3 | USDeOFT | `contracts/USDeOFT.sol` | 72 | (L2 only — Arbitrum 0x5d3a1Ff2…2ef34) | — |
| 4 | USDeOFTAdapter | `contracts/USDeOFTAdapter.sol` | 70 | 0x5d3a1Ff2b6BAb83b63cd9AD0787074081a52ef34 | — |
| 5 | ENAOFT | `contracts/ENAOFT.sol` | 72 | (L2 only — Arbitrum 0x58538e6A…C0133) | — |
| 6 | ENAOFTAdapter | `contracts/ENAOFTAdapter.sol` | 70 | 0x58538e6A46E07434d7E7375Bc268D3cb839C0133 | — |

### Dependency libs sourced (saved to `contracts/libs/`)

Blockscout's `/api/v2/smart-contracts/<addr>` returns the full multi-file source tree in
`additional_sources`. The Ethena OFT contracts import from local `../libs/` paths
(custom Ethena libs, NOT the upstream LayerZero `@layerzerolabs/` packages).

| File | Bytes | Source |
|------|-------|--------|
| `contracts/libs/RateLimiter.sol` | 2 912 | mainnet StakedUSDeOFTAdapter additional_sources |
| `contracts/libs/OFTOwnable2StepAdapter.sol` | 2 658 | mainnet StakedUSDeOFTAdapter additional_sources |
| `contracts/libs/OFTOwnable2Step.sol` | 2 667 | Arbitrum USDeOFT additional_sources |
| `contracts/libs/OFTCore.sol` | 18 910 | mainnet StakedUSDeOFTAdapter additional_sources |
| `contracts/libs/OFTAdapter.sol` | 4 799 | mainnet StakedUSDeOFTAdapter additional_sources |
| `contracts/libs/OFT.sol` | 3 230 | Arbitrum USDeOFT additional_sources |
| `contracts/libs/OFTMsgCodec.sol` | 2 906 | both mainnet + Arbitrum |
| `contracts/libs/OAppReceiver.sol` | 5 016 | mainnet StakedUSDeOFTAdapter additional_sources |
| `contracts/libs/ERC20.sol` | 11 143 | Arbitrum USDeOFT additional_sources (OZ v5) |

### InnerToken verification (read for cross-checking adapter behavior)

| Token | Address | Used by |
|-------|---------|---------|
| StakedUSDeV2 (sUSDe) | 0x9D39A5DE30e57443BfF2A8307A4256c8797A3497 | StakedUSDeOFTAdapter (constructor arg confirmed) |
| USDe | 0x4c9EDD5852cd905f086C759E8383e09bff1E68B3 | USDeOFTAdapter |
| ENA | 0x57e114B691Db790C35207b2e685D4A43181e6061 | ENAOFTAdapter |

The `StakedUSDe` (V1) base contract was fetched from sUSDe's `additional_sources`
(`contracts/StakedUSDe.sol`, 10 238 bytes). It uses OZ v4.9 `_beforeTokenTransfer` hook
(NOT v5 `_update`) and `SingleAdminAccessControl` for `hasRole` support. Confirms the
adapter's low-level `hasRole(bytes32,address)` call signature matches.

### Rate-limit configuration (decoded from constructor args)

| Adapter | dstEid | Limit | Window | Note |
|---------|--------|-------|--------|------|
| StakedUSDeOFTAdapter | 30217 (Linea) | 50e18 sUSDe | 60 s | very tight |
| StakedUSDeOFTAdapter | 30181 (Arbitrum) | 50e18 sUSDe | 60 s | very tight |
| USDeOFTAdapter | 30217 (Linea) | 50e18 USDe | 60 s | tight |
| USDeOFTAdapter | 30181 (Arbitrum) | 50e18 USDe | 60 s | tight |
| ENAOFTAdapter | 30181, 30217, 30110, 30102, 30111, 30151, 30177, 30214, 30260, 30255, 30183 | 10e24 ENA each | 3 600 s | 10M ENA/hr per chain |

---

## LayerZero Message Flow (canonical, applies to all 6 contracts)

```
SENDER (src chain)                                RECEIVER (dst chain)
─────────────────────                            ─────────────────────
User calls OFT.send(_sendParam, _fee, _refund)
   │
   ▼
OFTCore.send
   ├─ _debit(amountLD, minAmountLD, dstEid)          [VIRTUAL override chain]
   │     │
   │     ├─ Ethena's RateLimiter._checkAndUpdateRateLimit(dstEid, amountLD)
   │     │     └─ reverts RateLimitExceeded if amount > amountCanBeSent
   │     │     └─ updates rl.amountInFlight, rl.lastUpdated   (CEI ✓)
   │     │
   │     └─ super._debit  →  OFTAdapter._debit   (mainnet adapter model)
   │           ├─ _debitView  →  _removeDust(amountLD)
   │           └─ innerToken.safeTransferFrom(msg.sender, address(this), amountSentLD)
   │                                [LOCKS tokens inside the adapter]
   │           — OR —
   │           └─ OFT._debit  →  _burn(msg.sender, amountSentLD)   (L2 OFT model)
   │                                [BURNS tokens on L2]
   │
   ├─ _buildMsgAndOptions
   │     └─ OFTMsgCodec.encode(to, _toSD(amountReceivedLD), composeMsg)
   │           → abi.encodePacked(bytes32 to, uint64 amountSD, [bytes32 sender, composeMsg])
   │
   └─ _lzSend  →  endpoint.send{value: msg.value}(MessagingParams{dstEid, peer, message, options, payInLzToken}, refundAddress)
                   │
                   │  LayerZero Endpoint V2 handles:
                   │   - message lib signs packet
                   │   - DVN verifies
                   │   - executor delivers
                   │
                   ▼
                                                              endpoint.lzReceive(origin, guid, message, executor, extraData)
                                                                 │
                                                                 ▼
                                                              OAppReceiver.lzReceive
                                                                 ├─ require msg.sender == endpoint      [OnlyEndpoint]
                                                                 ├─ require peers[origin.srcEid] == origin.sender  [OnlyPeer]
                                                                 │
                                                                 ▼
                                                              OFTCore._lzReceive
                                                                 ├─ toAddress = message.sendTo().bytes32ToAddress()
                                                                 ├─ amountLD = _toLD(message.amountSD())
                                                                 ├─ amountReceivedLD = _credit(toAddress, amountLD, origin.srcEid)
                                                                 │       │
                                                                 │       ├─ [StakedUSDeOFTAdapter override]
                                                                 │       │    (bool ok, bytes data) = innerToken.call("hasRole", FULL_RESTRICTED_STAKER_ROLE, to)
                                                                 │       │    bool isBlack = abi.decode(data, (bool))     ← F-1: decoded before success check
                                                                 │       │    if (!ok || isBlack)  →  super._credit(owner(), amountLD, srcEid)
                                                                 │       │    else                 →  super._credit(to, amountLD, srcEid)
                                                                 │       │
                                                                 │       ├─ [StakedUSDeOFT override]
                                                                 │       │    if (blackList[to])  →  super._credit(owner(), amountLD, srcEid)
                                                                 │       │    else                →  super._credit(to, amountLD, srcEid)
                                                                 │       │
                                                                 │       └─ [default OFTAdapter._credit]  →  innerToken.safeTransfer(to, amountLD)  [UNLOCKS]
                                                                 │          [default OFT._credit]        →  _mint(to, amountLD)                     [MINTS on L2]
                                                                 │
                                                                 ├─ if composeMsg  →  endpoint.sendCompose(to, guid, 0, composeMsg)
                                                                 └─ emit OFTReceived(guid, srcEid, to, amountReceivedLD)
```

### Invariant (cross-chain mint/burn balance)

For the **adapter model** (mainnet): `adapter.innerToken.balanceOf(adapter)` ≥ Σ(synthetic supply on all L2s).
Equality holds iff every lock on mainnet was credited on some L2 and every unlock was debited on some L2.

For the **OFT model** (L2): `L2_totalSupply` = Σ(inbound credits) − Σ(outbound debits).

These invariants are preserved by the LayerZero Endpoint V2's message integrity (DVN-verified) and
the symmetric `amountSD` encoding. No mint-without-burn or burn-without-mint path exists in the code.

---

## Contract 1: `StakedUSDeOFTAdapter.sol` (57 lines, $416M TVL — HIGHEST VALUE)

### Purpose
Mainnet canonical contract that wraps `StakedUSDeV2` (sUSDe) into the LayerZero OFT
mesh using the **lock/unlock adapter pattern**. Outbound: pulls sUSDe from sender via
`safeTransferFrom` and locks it inside the adapter. Inbound: unlocks sUSDe from the
adapter's balance and sends to the recipient — UNLESS the recipient is on the
`FULL_RESTRICTED_STAKER_ROLE` blacklist on the innerToken, in which case funds are
redirected to `owner()`.

### Inheritance chain
```
StakedUSDeOFTAdapter
  └─ USDeOFTAdapter                       [adds RateLimiter hook on _debit + rateLimiter role]
       └─ OFTOwnable2StepAdapter          [2-step Ownable, blocks renounceOwnership]
            └─ OFTAdapter (LayerZero)     [lock/unlock: safeTransferFrom / safeTransfer]
                 └─ OFTCore (LayerZero)   [send / lzReceive / _debit / _credit / OFTMsgCodec]
                      └─ OApp (LayerZero)
                           ├─ OAppSender
                           ├─ OAppReceiver
                           └─ OAppCore    [endpoint immutable, peers mapping, setPeer onlyOwner]
       └─ RateLimiter                     [Ethena-custom, per-dstEid sliding window]
```

### Line-by-line analysis

| Lines | Area | Notes |
|-------|------|-------|
| 9 | `is USDeOFTAdapter` | Inherits the USDe adapter's rate limiter, OFTAdapter lock/unlock, and 2-step Ownable. Note: StakedUSDeOFTAdapter does NOT override `_debit` — the rate limit hook inherited from `USDeOFTAdapter._debit` applies. ✅ |
| 12 | `FULL_RESTRICTED_STAKER_ROLE` constant | `keccak256("FULL_RESTRICTED_STAKER_ROLE")` — matches the role in `StakedUSDe.sol` (verified by reading sUSDe source). ✅ |
| 14 | `RedistributeFunds` event | Emitted on redirect. Index by `user` (the would-be recipient). ✅ |
| 23-28 | Constructor | Pure passthrough to `USDeOFTAdapter`. No additional state. ✅ |
| 38-56 | `_credit` override | **See F-1 below** — the failure-handling path has a robustness gap. |

### Finding F-1 — `abi.decode` of revert data before `success` check (Low)

**Location:** `StakedUSDeOFTAdapter._credit`, lines 44-47

```solidity
(bool success, bytes memory data) = address(innerToken).call(
    abi.encodeWithSignature("hasRole(bytes32,address)", FULL_RESTRICTED_STAKER_ROLE, _to)
);
bool isBlackListed = abi.decode(data, (bool));   // ← decoded BEFORE success check

if (!success || isBlackListed) {
    emit RedistributeFunds(_to, _amountLD);
    return super._credit(owner(), _amountLD, _srcEid);
} else {
    return super._credit(_to, _amountLD, _srcEid);
}
```

**The bug:** `abi.decode(data, (bool))` is executed unconditionally, even when `success == false`.
Solidity 0.8's `abi.decode` reverts on insufficient data length. Three failure modes:

| `success` | `data` length | `abi.decode(data,(bool))` result | Outer `if` branch taken |
|-----------|---------------|-----------------------------------|------------------------|
| `true` | ≥ 32 bytes (normal) | correct bool | correct |
| `true` | < 32 bytes (e.g., innerToken is EOA or has bare fallback returning nothing) | **reverts** — `lzReceive` reverts — message stuck | n/a |
| `false` | 0 bytes (revert with no reason, `revert()`) | **reverts** — `lzReceive` reverts — message stuck | n/a |
| `false` | ≥ 32 bytes (revert with `Error(string)`) | first 32 bytes = `0x08c379a0…` (Error selector) → non-zero → `isBlackListed = true` | **redirect to owner** (silent fund redirection) |

**Trigger conditions (in practice, dormant):**
- `innerToken` (sUSDe at `0x9D39A5DE30e57443BfF2A8307A4256c8797A3497`) is `StakedUSDeV2`
  which inherits `SingleAdminAccessControl` → `AccessControl.hasRole` is a pure view
  that never reverts and always returns exactly 32 bytes. So `success == true, data == 32 bytes`
  in all normal operation.
- The bug only manifests if:
  1. `innerToken` is upgraded to a contract whose `hasRole` reverts (e.g., a paused state, a storage corruption, a token migration that changes the function selector).
  2. `innerToken` is somehow replaced with an EOA (deployment-time mistake).
  3. The `hasRole(bytes32,address)` selector is shadowed by a fallback that consumes the call without returning 32 bytes.

**Impact:**
- **Silent fund redirection:** if `hasRole` reverts with a non-empty reason, ALL cross-chain
  receives during the affected window are silently redirected to `owner()`. Users lose
  their sUSDe without consent and must petition the owner for manual recovery. The
  `RedistributeFunds` event is emitted but the user may not be monitoring.
- **Message stick:** if `hasRole` reverts with empty data, ALL inbound LayerZero messages
  to this adapter revert and get retried indefinitely by the executor (costing gas)
  until the innerToken is fixed.

**Why this matters at $416M TVL:** Even a low-probability event affecting the
innerToken's `hasRole` could trigger mass silent redirection of user funds to the owner.
The intended "fail-closed for compliance" behavior is sound, but the implementation
should branch on `success` BEFORE decoding, and decode inside a try/catch or with a
length guard. Recommended fix:

```solidity
bool isBlackListed;
if (success && data.length >= 32) {
    isBlackListed = abi.decode(data, (bool));
} else if (!success) {
    // fail-closed: treat as blacklisted → redirect to owner
    isBlackListed = true;
} else {
    // success but malformed return — should never happen for a real AccessControl
    revert("StakedUSDeOFTAdapter: malformed hasRole return");
}
```

**Severity: Low.** Requires innerToken misbehavior (trusted contract). Dormant in
production. But the failure mode is silent fund redirection at scale, so worth fixing.

**Not a Critical:** No attacker-controlled path to trigger this. The innerToken is a
fixed, audited, non-upgradeable contract. The bug is a defensive-coding gap, not an
exploitable vulnerability.

### Cross-chain interaction analysis for this contract

1. **Replay protection:** Inherited from `OAppReceiver.lzReceive` — checks
   `msg.sender == endpoint` and `peers[srcEid] == origin.sender`. Endpoint V2 enforces
   per-path GUID uniqueness. ✅
2. **Message tampering:** `OFTMsgCodec` encoding is `abi.encodePacked(bytes32, uint64,
   [bytes32, bytes])`. DVN-verified by the endpoint. Relayer cannot tamper. ✅
3. **Decimal mismatch:** `sharedDecimals() == 6` (OFT default), sUSDe `decimals() == 18`,
   so `decimalConversionRate == 1e12`. `_removeDust` strips sub-1e12 amounts. All
   Ethena chains use 18 decimals for sUSDe, so no cross-chain decimal issue. ✅
4. **Adapter balance cap:** Inbound `_credit` reverts if `adapter.balanceOf(sUSDe) <
   amountLD`. This naturally bounds inbound flow to what was previously locked
   outbound. ✅ (See F-5 for griefing analysis.)
5. **Blacklist enforcement:** The adapter only checks `FULL_RESTRICTED_STAKER_ROLE` on
   the innerToken. `SOFT_RESTRICTED_STAKER_ROLE` is NOT checked by the adapter, but
   `StakedUSDe._beforeTokenTransfer` only reverts on `FULL_RESTRICTED_STAKER_ROLE`
   anyway (soft blacklist is only enforced in `_deposit` / `_withdraw`, not in plain
   transfers). So the adapter's check is consistent with the innerToken's transfer
   semantics. ✅
6. **Native ETH:** No `receive()` function. `lzReceive` is `payable` but the contract
   never uses `msg.value`. ETH sent via `lzReceive` would be stuck. (See F-9.)

### Severity rating for Contract 1
- F-1: **Low** (robustness gap, dormant in production, requires innerToken misbehavior).
- No Critical / High / Medium.

---

## Contract 2: `StakedUSDeOFT.sol` (112 lines, L2-side OFT — mints/burns)

### Purpose
The L2-side OFT for sUSDe. Uses the **mint/burn pattern** (NOT the adapter lock/unlock).
On inbound: mints synthetic sUSDe to the recipient. On outbound: burns the sender's
synthetic sUSDe. Maintains its own local `blackList` mapping (separate from any role
on a staked token) and a `blackLister` role that can update the list alongside `owner`.

### Inheritance chain
```
StakedUSDeOFT
  └─ USDeOFT  (adds RateLimiter hook on _debit + rateLimiter role)
       └─ OFTOwnable2Step  (2-step Ownable, blocks renounceOwnership)
            └─ OFT (LayerZero)  [mint/burn: _burn on debit, _mint on credit]
                 └─ OFTCore, ERC20 (OZ v5 — uses _update hook, not _beforeTokenTransfer)
```

Note: the OFT path uses OZ v5 `ERC20` whose `_update(address from, address to, uint256
value)` hook is overridden here. This is DIFFERENT from the mainnet sUSDe innerToken
which uses OZ v4.9 `_beforeTokenTransfer`. The two models are consistent in effect
(both block transfers involving blacklisted addresses) but the hook signature differs.

### Line-by-line analysis

| Lines | Area | Notes |
|-------|------|-------|
| 9 | `is USDeOFT` | Inherits rate limiter + OFT mint/burn + 2-step Ownable. ✅ |
| 11-14 | `blackLister`, `blackList` mapping | L2-local blacklist. Independent from mainnet's `FULL_RESTRICTED_STAKER_ROLE` on sUSDe. ✅ (See F-4 for cross-chain lag.) |
| 22-24 | Errors | `BlackListed`, `NotBlackListed`, `OnlyBlackLister`. ✅ |
| 34-40 | Constructor | Pure passthrough to `USDeOFT`. ✅ |
| 46-49 | `setBlackLister` | `onlyOwner`. No zero-address check — owner could set `blackLister = address(0)`, which would lock out the role (only owner could then update the list). Acceptable. ✅ |
| 56-60 | `updateBlackList` | `msg.sender == blackLister || msg.sender == owner()`. No event-indexed old value. ✅ |
| 70-82 | `_credit` override | Checks `blackList[_to]`. If blacklisted, redirects to `owner()` via `super._credit`. **Only checks recipient**, not sender — sender is on the source chain and is enforced by the source chain's own `_debit → _burn → _update` check. ✅ (by design) |
| 90-94 | `_update` override | Checks BOTH `_from` and `_to` against `blackList`. This hook is called by `_mint`, `_burn`, and `_transfer`. So local mints/burns/transfers all enforce the blacklist. ✅ |
| 101-111 | `redistributeBlackListedFunds` | **See F-2 below** — CEI smell. |

### Finding F-2 — `redistributeBlackListedFunds` temporarily un-blacklists during `_transfer` (Low)

**Location:** `StakedUSDeOFT.redistributeBlackListedFunds`, lines 101-111

```solidity
function redistributeBlackListedFunds(address _from, uint256 _amount) external onlyOwner {
    if (!blackList[_from]) revert NotBlackListed();

    blackList[_from] = false;                    // ← state mutation #1
    _transfer(_from, owner(), _amount);          // ← external-ish call (no callback though)
    blackList[_from] = true;                     // ← state mutation #2

    emit RedistributeFunds(_from, _amount);
}
```

**The smell:** CEI is violated — state is mutated, then `_transfer` is called, then state
is mutated again. The `_transfer` calls `_update` which is overridden to check `blackList`.
Without the temporary un-blacklist, the `_update` check would revert (because `_from` is
blacklisted). The temporary un-blacklist is the only way to make the transfer succeed.

**Why it's Low, not higher:**
- `_transfer` (OZ v5 `ERC20._transfer`) does NOT trigger any callback. It calls `_update`,
  which is overridden here to only check `blackList` and call `super._update` (a pure
  storage update). No external calls, no reentrancy vector.
- The recipient (`owner()`) is not invoked during `_transfer`.
- The OZ v5 ERC20 does not have ERC-777-style hooks.
- If `_transfer` reverts (e.g., `_from` has insufficient balance), the entire transaction
  reverts, including the `blackList[_from] = false` mutation. So no state corruption.
- The only "window" where `_from` is un-blacklisted is during the internal `_update`
  call, which is non-reentrant. An off-chain observer simulating the tx could see the
  intermediate state, but cannot act on it within the same tx.

**Recommended fix (defense-in-depth):** Use a sentinel or a dedicated internal
`_transferUnchecked` that skips the blacklist check, instead of mutating the blacklist:

```solidity
function redistributeBlackListedFunds(address _from, uint256 _amount) external onlyOwner {
    if (!blackList[_from]) revert NotBlackListed();
    _update(_from, owner(), _amount);   // bypass _transfer's _update override? No — _update itself checks.
    // Better: introduce an internal _transferWithoutBlacklistCheck that calls super._update directly.
    emit RedistributeFunds(_from, _amount);
}
```

Or move the re-blacklist to a `finally`-style pattern using try/catch (Solidity 0.8+):

```solidity
blackList[_from] = false;
(bool ok,) = address(this).call(abi.encodeWithSignature("_transfer(address,address,uint256)", _from, owner(), _amount));
blackList[_from] = true;
if (!ok) revert();
```

**Severity: Low.** No exploit path identified. Pure code-smell / defense-in-depth gap.

### Other observations for Contract 2

- **`_credit` redirects to `owner()`.** If `owner()` is itself on the blacklist,
  `super._credit(owner(), ...)` → `_mint(owner(), ...)` → `_update(address(0), owner(),
  ...)` → reverts `BlackListed(owner)`. The lzReceive then reverts and the message is
  stuck. Edge case (owner is typically a multisig, never blacklisted). (F-10)
- **`_update` checks `address(0)` against `blackList`** — `blackList[address(0)]` is
  always `false` (default), so mints/burns (which use `address(0)` as the from/to) are
  not blocked by the blacklist check on the zero address. ✅
- **`blackLister` is mutable by owner.** A compromised owner could set `blackLister` to
  an attacker, who could then blacklist arbitrary users. Standard trusted-role risk.

### Severity rating for Contract 2
- F-2: **Low** (CEI smell, no exploit).
- F-4, F-10: **Informational**.
- No Critical / High / Medium.

---

## Contract 3: `USDeOFT.sol` (72 lines, L2-side OFT for USDe)

### Purpose
L2-side OFT for plain USDe. Identical structure to `StakedUSDeOFT` but WITHOUT the
blacklist logic. Mints on inbound, burns on outbound. Rate-limited per dstEid.

### Line-by-line analysis

| Lines | Area | Notes |
|-------|------|-------|
| 10 | `is OFTOwnable2Step, RateLimiter` | Multiple inheritance. `OFTOwnable2Step` extends `OFT` (which extends `OFTCore`, `ERC20`). `RateLimiter` is a separate abstract contract with its own storage (`rateLimits` mapping). No storage collision (OFT uses `_balances`, `_allowances`, `_totalSupply`; RateLimiter uses `rateLimits`). ✅ |
| 12-18 | `rateLimiter` address + event + error | Separate role for rate-limit admin. Allows a dedicated bot to adjust limits without full owner privileges. ✅ |
| 28-36 | Constructor | Calls `OFTOwnable2Step` constructor + `_setRateLimits`. ✅ |
| 42-45 | `setRateLimiter` | `onlyOwner`. No zero-address validation — see F-8. ✅ |
| 51-54 | `setRateLimits` | `msg.sender == rateLimiter || msg.sender == owner()`. Calls internal `_setRateLimits`. ✅ |
| 64-71 | `_debit` override | Calls `_checkAndUpdateRateLimit(dstEid, amountLD)` BEFORE `super._debit`. ✅ CEI: rate-limit state is updated inside `_checkAndUpdateRateLimit`, then `super._debit` (which calls `_burn`) executes. If `_burn` reverts (e.g., insufficient balance), the entire tx reverts including the rate-limit update. ✅ |

### Observations
- The rate limiter hook is on `_debit` only (outbound). Inbound `_credit` uses the
  default `OFT._credit` which just calls `_mint`. No rate limit on inbound. (F-5)
- `_burn(msg.sender, amountSentLD)` in `OFT._debit` enforces the sender's balance. A
  blacklisted user (if this contract had a blacklist — it doesn't) couldn't burn. ✅
- `decimalConversionRate == 1e12` (USDe has 18 decimals, sharedDecimals is 6). ✅

### Severity rating for Contract 3
- F-7, F-8: **Informational**.
- No Critical / High / Medium / Low.

---

## Contract 4: `USDeOFTAdapter.sol` (70 lines, mainnet adapter for USDe)

### Purpose
Mainnet canonical contract wrapping `USDe` into the LayerZero OFT mesh using the
lock/unlock adapter pattern. Identical structure to `StakedUSDeOFTAdapter` but WITHOUT
the blacklist-redirect override on `_credit`. Just adds the rate-limiter hook on `_debit`
and the `rateLimiter` role.

### Line-by-line analysis

| Lines | Area | Notes |
|-------|------|-------|
| 10 | `is OFTOwnable2StepAdapter, RateLimiter` | Multiple inheritance. `OFTOwnable2StepAdapter` extends `OFTAdapter` (which has `innerToken` immutable, `safeTransferFrom` / `safeTransfer`). ✅ |
| 27-34 | Constructor | `OFTOwnable2StepAdapter(_token, _lzEndpoint, _delegate)` + `_setRateLimits`. ✅ |
| 40-43 | `setRateLimiter` | `onlyOwner`. ✅ |
| 49-52 | `setRateLimits` | `rateLimiter || owner`. ✅ |
| 62-69 | `_debit` override | Rate limit then `super._debit` (which is `OFTAdapter._debit` → `safeTransferFrom`). ✅ |

### Observations
- No `_credit` override — uses `OFTAdapter._credit` which does `innerToken.safeTransfer(_to,
  _amountLD)`. If `_to` is a non-standard contract that reverts on transfer, the message
  sticks. ✅ (standard adapter behavior).
- No blacklist logic — USDe is a plain ERC20 without `hasRole`. ✅
- `innerToken` is `USDe` at `0x4c9EDD5852cd905f086C759E8383e09bff1E68B3`. ✅

### Severity rating for Contract 4
- F-7, F-8, F-9: **Informational**.
- No Critical / High / Medium / Low.

---

## Contract 5: `ENAOFT.sol` (72 lines, L2-side OFT for ENA)

### Purpose
L2-side OFT for the ENA token. **Byte-for-byte identical to `USDeOFT.sol`** except for
the contract name and the natdoc comments. Same inheritance, same rate-limiter hook,
same constructor signature.

### Line-by-line analysis
Identical to Contract 3 (`USDeOFT`). See above.

### Observations
- ENA is the Ethena governance token, not a yield-bearing vault. So cross-chain ENA
  is a 1:1 representation (no yield loss concern like sUSDe).
- Deployed with 11 destination rate limits (10M ENA / hour each). This is a much wider
  rate-limit mesh than USDe/sUSDe (only 2 destinations).

### Severity rating for Contract 5
- Same as Contract 3. No Critical / High / Medium / Low.

---

## Contract 6: `ENAOFTAdapter.sol` (70 lines, mainnet adapter for ENA)

### Purpose
Mainnet canonical contract wrapping `ENA` into the LayerZero OFT mesh. **Byte-for-byte
identical to `USDeOFTAdapter.sol`** except for the contract name.

### Line-by-line analysis
Identical to Contract 4 (`USDeOFTAdapter`). See above.

### Observations
- `innerToken` is `ENA` at `0x57e114B691Db790C35207b2e685D4A43181e6061`. ✅
- 11 rate-limit configs at construction (vs 2 for USDe/sUSDe adapters).

### Severity rating for Contract 6
- Same as Contract 4. No Critical / High / Medium / Low.

---

## Dependency: `RateLimiter.sol` (80 lines) — Ethena-custom

### Purpose
Per-`dstEid` sliding-window rate limiter with linear decay. Hooked into `_debit` by all
6 Ethena OFT contracts. NOT the LayerZero standard (Ethena wrote their own).

### Line-by-line analysis

| Lines | Area | Notes |
|-------|------|-------|
| 5-10 | `RateLimit` struct | `amountInFlight` (current debt), `lastUpdated` (timestamp), `limit`, `window`. All `uint256`. ✅ |
| 12-16 | `RateLimitConfig` struct | Used for batch config. `dstEid` is `uint32`, `limit` and `window` are `uint256`. ✅ |
| 18 | `rateLimits` mapping | `mapping(uint32 dstEid => RateLimit) public`. Anyone can read. ✅ |
| 24-29 | `getAmountCanBeSent` | View helper. Returns `(currentAmountInFlight, amountCanBeSent)`. ✅ |
| 31-42 | `_setRateLimits` | **See F-7.** `unchecked` loop. Sets `limit` and `window` only; does NOT reset `amountInFlight` or `lastUpdated`. ✅ (intentional — preserves in-flight debt across config changes) |
| 44-62 | `_amountCanBeSent` | Sliding window with linear decay: `decay = limit * timeSinceLastDeposit / window`. If `timeSinceLastDeposit >= window`, full `limit` is available. ✅ |
| 50 | `block.timestamp - _lastUpdated` | Solidity 0.8 reverts on underflow. If `block.timestamp < _lastUpdated` (chain reorg or non-monotonic timestamp), all sends to that dstEid revert until timestamp catches up. Practically impossible on mainnet/L2s. ✅ |
| 57 | `decay = (limit * timeSinceLastDeposit) / window` | Rounds DOWN. Favors the protocol (slightly slower refill). ✅ |
| 58 | `currentAmountInFlight = amountInFlight <= decay ? 0 : amountInFlight - decay` | Avoids underflow. ✅ |
| 60 | `amountCanBeSent = limit <= currentAmountInFlight ? 0 : limit - currentAmountInFlight` | If limit was lowered below current debt, blocks all sends until debt decays. ✅ |
| 64-79 | `_checkAndUpdateRateLimit` | Reads storage, computes, reverts if `_amount > amountCanBeSent`, then updates storage. ✅ CEI: check before state mutation. If revert, state unchanged. |

### Finding F-7 — `_setRateLimits` does not reset `amountInFlight`; admin can lift cap instantly (Informational)

**Behavior:**
- If the admin lowers `limit` below current `amountInFlight`, the next `_checkAndUpdateRateLimit`
  computes `amountCanBeSent = 0`, blocking all sends until decay brings `currentAmountInFlight`
  below `limit`. ✅ (correct, safe)
- If the admin RAISES `limit`, the next `_checkAndUpdateRateLimit` computes
  `amountCanBeSent = newLimit - currentAmountInFlight`, which could be large immediately.
  The admin can effectively bypass the rate limit by raising it. ✅ (by design — emergency response)
- The admin CANNOT reset `amountInFlight` to 0 via `_setRateLimits` alone. To reset, they
  would need to wait for decay, or deploy a new contract. There is no `resetRateLimit`
  function. This is a minor operational gap (e.g., after a compromise, the admin can't
  instantly clear the in-flight debt).

**Severity: Informational.** The rate limiter is a soft cap that the admin can lift. This
is appropriate for a protocol that needs emergency response capability. The fact that
`amountInFlight` is preserved across config changes is correct (prevents admin from
clearing debt to allow a burst).

### Finding F-3 — Per-path rate limit bypass via multi-hop routing (Informational)

**The bypass:**
The rate limiter is keyed by `(contract, dstEid)`. A user who wants to send `X` from
chain A to chain B but is constrained by the A→B rate limit can route via chain C:

```
A --[A→C limit on A]--> C --[C→B limit on C]--> B
```

If `A→C` limit on A and `C→B` limit on C are both higher than `A→B` limit on A, the user
moves more per unit time than the direct A→B limit allows.

**Concrete example (using deployed configs):**
- StakedUSDeOFTAdapter on mainnet has `rateLimits[30181 (Arb)] = 50e18/60s` and
  `rateLimits[30217 (Linea)] = 50e18/60s`.
- A user wanting to move 100 sUSDe from mainnet to Arbitrum in 60s can:
  1. Send 50 sUSDe mainnet→Arbitrum (uses 50/50 of mainnet's A→Arb limit).
  2. Send 50 sUSDe mainnet→Linea (uses 50/50 of mainnet's A→Linea limit).
  3. From Linea, send 50 sUSDe Linea→Arbitrum (uses Linea's L→Arb limit, which is
     configured separately on Linea's StakedUSDeOFT).
- Result: 100 sUSDe reached Arbitrum in 60s, vs 50 if direct-only.

**Why it's only Informational:**
- Each hop is still rate-limited by its own per-path config. The bypass only works if
  there exists a path with higher limits.
- The total outbound flow from any single chain is bounded by `Σ(rateLimits[dstEid])` —
  i.e., the sum of all configured rate limits. For StakedUSDeOFTAdapter, that's
  `50 + 50 = 100 sUSDe / 60s` total outbound.
- The adapter's token balance is a hard cap on cumulative outbound (can't lock more than
  the adapter holds). So even with multi-hop, the total cross-chain TVL movement is
  bounded.
- To truly cap total outbound per epoch, the protocol would need a GLOBAL rate limiter
  (not per-dstEid). This is a design choice, not a bug.

**Severity: Informational.** No fix needed unless the protocol specifically wants to cap
total outbound flow.

### Rate-limit bypass via "self-send"
Sending tokens to oneself cross-chain (A→B with `to = self`) does NOT bypass the rate
limiter. The `_debit` on A is still called, the rate limit on A→B is still consumed, and
the tokens are locked on A. The `_credit` on B mints to self. To retrieve them, the user
must send B→A, consuming B's A-rate limit. No bypass. ✅

### Rate-limit bypass via wrapping
Wrapping (e.g., deposit USDe into sUSDe on mainnet, then send sUSDe cross-chain) does
NOT bypass the sUSDe rate limiter — the sUSDe rate limit applies to the sUSDe send. The
USDe rate limit is separate (applies to USDe sends). So wrapping doesn't help bypass. ✅

### Chain boundary (different block times)
The rate limiter uses `block.timestamp` (in seconds), which is consistent across EVM
chains (all use Unix time). Different block times (12s on mainnet, 2s on Arbitrum, 0.5s
on Linea) don't affect the rate limit calculation — it's wall-clock based, not
block-based. ✅

---

## Dependency: `OFTOwnable2Step.sol` and `OFTOwnable2StepAdapter.sol` (76 lines each)

### Purpose
2-step ownership transfer wrapper around LayerZero's `OFT` / `OFTAdapter`. Ethena
copied this from OZ v5 `Ownable2Step` because the LayerZero base contracts use OZ v5
`Ownable` (single-step) and Ethena wanted the safer 2-step pattern. Also blocks
`renounceOwnership` (Ethena never wants the contracts to be ownerless).

### Line-by-line analysis (both files identical except for base class)

| Lines | Area | Notes |
|-------|------|-------|
| 21 | `_pendingOwner` | Private. ✅ |
| 24 | `CantRenounceOwnership` error | ✅ |
| 29-34 | Constructor | Routes to base constructor + `Ownable(_delegate)`. The delegate becomes the initial owner. ✅ |
| 38-41 | `pendingOwner` | View. ✅ |
| 46-49 | `transferOwnership` | `onlyOwner`. Sets `_pendingOwner`, does NOT transfer yet. Emits `OwnershipTransferStarted`. ✅ |
| 55-58 | `_transferOwnership` | Internal. Deletes `_pendingOwner` then calls `super._transferOwnership`. ✅ |
| 63-69 | `acceptOwnership` | Public. Checks `pendingOwner() == msg.sender`. Calls `_transferOwnership(sender)`. ✅ |
| 72-74 | `renounceOwnership` | `view` override (not `pure`!) that always reverts. The `view` modifier is unusual but valid — it makes the function callable from `staticcall` contexts (where it would still revert). ✅ |

### Observations
- The 2-step pattern prevents accidental ownership transfer to a wrong address (the
  wrong address can't accept). ✅
- `renounceOwnership` is blocked, so the contract always has an owner. ✅
- No way to transfer ownership to `address(0)` because `acceptOwnership` requires
  `msg.sender == pendingOwner`, and `address(0)` can't be `msg.sender`. ✅

### Severity
No issues. ✅

---

## Cross-cutting Findings (apply to multiple contracts)

### F-4 — Cross-chain blacklist lag (Informational)

**Affected:** `StakedUSDeOFT`, `StakedUSDeOFTAdapter`.

**Behavior:** Each chain maintains its own blacklist:
- `StakedUSDeOFT` (L2) has a local `blackList` mapping.
- `StakedUSDeOFTAdapter` (mainnet) queries the innerToken's `FULL_RESTRICTED_STAKER_ROLE`.

There is NO automatic synchronization between these lists. A user blacklisted on mainnet
is NOT automatically blacklisted on L2s (and vice versa). The protocol must call
`updateBlackList` on each L2's `StakedUSDeOFT` and `addToBlacklist` on mainnet's
`StakedUSDe` separately.

**Impact:** A user blacklisted on mainnet but not yet on L2 can still move/transfer their
synthetic sUSDe on L2. They can also send from L2 to another L2. They CANNOT:
- Send from mainnet to L2 (blocked by `StakedUSDe._beforeTokenTransfer` on the
  `safeTransferFrom` in the adapter's `_debit`).
- Receive on mainnet (the adapter's `_credit` redirects to owner if the recipient is on
  `FULL_RESTRICTED_STAKER_ROLE`).

So the lag is one-directional: a mainnet-blacklisted user with L2 funds can still move
them on L2 until the L2 blacklist is updated. This is a known compliance gap, not a code
bug. The protocol's off-chain ops must push blacklist updates to all chains promptly.

**Severity: Informational.** Operational/compliance concern.

### F-5 — No inbound rate limit on `_credit` (Informational)

**Affected:** All 6 contracts (the rate limiter is only hooked in `_debit`).

**Behavior:** Inbound `_credit` is unlimited. An attacker who acquires tokens on chain B
can send them all to chain A in a single message (subject only to chain B's outbound
rate limit and the attacker's balance).

**Why it's not exploitable:**
- For the adapter model (mainnet): `_credit` calls `innerToken.safeTransfer(_to,
  amountLD)`. If the adapter's balance < amountLD, the transfer reverts. So inbound is
  bounded by `adapter.balanceOf(innerToken)`, which equals `cumulative_locked -
  cumulative_unlocked`. Since `cumulative_unlocked ≤ cumulative_locked` (you can't
  unlock more than was locked), the adapter always has enough — UNLESS the invariant is
  broken by a bug (none found).
- For the OFT model (L2): `_credit` calls `_mint(_to, amountLD)`. Minting is unlimited
  (no supply cap). But the mint only happens in response to a valid LayerZero message
  from a peer, which requires a corresponding burn/lock on the source chain. So the L2
  total supply is bounded by `Σ(inbound from mainnet) + Σ(inbound from other L2s)`,
  which is itself bounded by the source chains' outbound rate limits.

**Griefing vector (low-impact):** If chain B's outbound rate limit is removed (admin
error or compromise), an attacker with tokens on B can spam chain A with messages that
exceed chain A's adapter balance. The messages revert and get retried, costing the
executor gas. But the attacker's tokens on B are already burned (debit), so they lose
value. Not profitable.

**Severity: Informational.** Design choice. The outbound rate limit is the primary
throttle; inbound is naturally bounded by the adapter balance / source-chain burn.

### F-6 — StakedUSDeOFTAdapter only checks FULL_RESTRICTED_STAKER_ROLE (Informational)

The adapter's `_credit` queries `hasRole(FULL_RESTRICTED_STAKER_ROLE, _to)` on the
innerToken. It does NOT check `SOFT_RESTRICTED_STAKER_ROLE`. However, `StakedUSDe._beforeTokenTransfer`
also only reverts on `FULL_RESTRICTED_STAKER_ROLE` (soft blacklist is only enforced in
`_deposit` / `_withdraw`, not in plain `transfer`). So the adapter's check is consistent
with the innerToken's transfer-time enforcement.

If a recipient is on `SOFT_RESTRICTED_STAKER_ROLE`, the adapter credits them normally
(no redirect), and the innerToken's `transfer` succeeds (soft blacklist doesn't block
transfers). ✅

**Severity: Informational.** Consistent behavior.

### F-8 — `setRateLimiter` accepts `address(0)` (Informational)

`setRateLimiter(address(0))` sets `rateLimiter = address(0)`. Then `setRateLimits` checks
`msg.sender != rateLimiter && msg.sender != owner()`. With `rateLimiter = address(0)`:
- `msg.sender != address(0)` is always true (msg.sender is always a real address).
- So the check becomes `true && msg.sender != owner()`, i.e., only owner can call.
- This effectively disables the rateLimiter role, leaving only owner. Safe.

No zero-address check needed. ✅

**Severity: Informational.**

### F-9 — `lzReceive` is `payable`; contracts have no `receive()` (Informational)

`OAppReceiver.lzReceive` is `payable`. The Ethena OFT contracts don't override this.
The contracts have no `receive()` function. If the LayerZero executor forwards native
ETH to `lzReceive` (e.g., for compose gas), the ETH is held by the contract but never
used or withdrawn. There's no `rescueETH` function.

In practice, LayerZero V2's executor pays for gas separately and doesn't forward ETH to
the OApp via `lzReceive`. So this is dormant. But if the OApp ever uses `lzCompose` with
native gas forwarding, the ETH could accumulate.

**Severity: Informational.** Minor fund-sticking risk.

### F-10 — Owner-redirect path reverts if `owner()` is blacklisted (Informational)

In both `StakedUSDeOFT._credit` and `StakedUSDeOFTAdapter._credit`, the redirect target
is `owner()`. If `owner()` is itself blacklisted:
- `StakedUSDeOFT`: `super._credit(owner(), ...)` → `_mint(owner(), ...)` → `_update(0,
  owner(), ...)` → reverts `BlackListed(owner)`.
- `StakedUSDeOFTAdapter`: `super._credit(owner(), ...)` → `innerToken.safeTransfer(owner(),
  ...)` → `StakedUSDe._beforeTokenTransfer(adapter, owner, ...)` → reverts.

The lzReceive reverts, message stuck. Edge case — owner is typically a multisig that is
never blacklisted. But worth noting.

**Severity: Informational.**

---

## LayerZero-specific vulnerability classes — checklist

| Class | Status | Notes |
|-------|--------|-------|
| 1. Cross-chain message replay | ✅ Safe | Endpoint V2 enforces per-path GUID uniqueness. `OAppReceiver.lzReceive` checks `msg.sender == endpoint` and `peers[srcEid] == origin.sender`. `nextNonce` returns 0 (default, no OApp-level ordering — safe, Endpoint handles uniqueness). |
| 2. Rate limit bypass | ⚠️ F-3 (Info) | Per-path rate limit can be bypassed via multi-hop routing. Bounded by total outbound rate and adapter balance. No code fix needed; design choice. |
| 3. Message tampering | ✅ Safe | `OFTMsgCodec` uses `abi.encodePacked(bytes32, uint64, [bytes32, bytes])`. DVN-verified by Endpoint. Relayer cannot modify payload. |
| 4. Endpoint interaction bugs | ✅ Safe | Ethena OFTs don't call `endpoint` directly — they use `OAppSender._lzSend` and `OAppReceiver.lzReceive` abstractions. No custom endpoint interaction. |
| 5. OFT credit/debit imbalance | ✅ Safe | Each `_debit` (burn/lock) has a corresponding `_credit` (mint/unlock) on the peer. Amounts are symmetric (after `_removeDust`). Invariant: `adapter.balanceOf(innerToken) == Σ L2 supplies`. No mint-without-burn path found. |
| 6. Adapter pattern bugs | ✅ Safe | `safeTransferFrom` in `_debit` requires user approval. `safeTransfer` in `_credit` bounded by adapter balance. No balance/allowance issues. `OFTAdapter` warns about fee-on-transfer tokens — USDe/sUSDe/ENA are all plain ERC20 (no fees), so safe. |
| 7. Decimal mismatch across chains | ✅ Safe | `sharedDecimals == 6`, all Ethena tokens use 18 decimals. `decimalConversionRate == 1e12`. `_removeDust` strips sub-1e12. No L2 uses different decimals. |
| 8. Blocking/unblocking logic | ⚠️ F-4 (Info) | Per-chain blacklist, no auto-sync. A mainnet-blacklisted user can still move L2 funds until L2 blacklist is updated. Compliance gap, not code bug. |
| 9. Native ETH handling | ✅ Safe | No `receive()`. Contracts don't use `msg.value`. `lzReceive` is payable but ETH is not forwarded into the OApp logic. No reentrancy via native ETH. |
| 10. Upgradeable proxy | ✅ N/A | Ethena OFT contracts are NOT upgradeable (no UUPS/transparent proxy). `StakedUSDe` (the innerToken) is also not upgradeable. `StakedENA` is upgradeable but out of scope. |

---

## Comparison: Ethena OFT vs LayerZero OFT standard

| Aspect | LayerZero standard | Ethena custom | Difference |
|--------|--------------------|---------------|------------|
| Base class | `OFT` / `OFTAdapter` | `OFTOwnable2Step` / `OFTOwnable2StepAdapter` (wraps standard) | Adds 2-step ownership, blocks renounce |
| Rate limiting | None | Custom `RateLimiter` per-dstEid sliding window | Ethena adds outbound throttle |
| Blacklist | None | Custom in `StakedUSDeOFT*` | Ethena adds compliance redirect |
| Message codec | `OFTMsgCodec` | Same (no override) | Identical |
| `lzReceive` validation | `OAppReceiver` default | Same (no override) | Identical |
| `nextNonce` | returns 0 | Same (no override) | Identical (Endpoint handles uniqueness) |
| Peer management | `setPeer` onlyOwner | Same | Identical |
| `msgInspector` | Optional, settable | Not used (defaults to address(0)) | Identical (disabled) |

The Ethena customizations are purely additive (rate limiter + blacklist + 2-step ownable).
They do NOT modify the LayerZero message flow, codec, or endpoint interaction. This is
the correct pattern — customizations should be additive, not invasive.

---

## Recommendations (defense-in-depth, not critical fixes)

1. **F-1 (Low):** In `StakedUSDeOFTAdapter._credit`, check `success` before calling
   `abi.decode`. Decode inside a length guard. This eliminates the silent-redirect and
   message-stick failure modes if the innerToken's `hasRole` ever misbehaves.
2. **F-2 (Low):** In `StakedUSDeOFT.redistributeBlackListedFunds`, replace the temporary
   un-blacklist pattern with an internal `_transferUnchecked` that calls `super._update`
   directly, bypassing the blacklist check. This eliminates the CEI smell.
3. **F-3 (Info):** If the protocol wants to cap TOTAL outbound per epoch (not just per-path),
   add a global rate limiter alongside the per-path one. Otherwise, document the multi-hop
   bypass as an accepted design tradeoff.
4. **F-4 (Info):** Document the cross-chain blacklist lag in the protocol's compliance
   runbook. Ensure off-chain ops push blacklist updates to all chains within a tight SLA.
5. **F-9 (Info):** Add a `rescueETH` function (admin-gated) to sweep accidental native
   ETH sent via `lzReceive`.
6. **F-10 (Info):** Add a `require(!blackList[owner()], "owner blacklisted")` check in
   the redirect path, or redirect to a dedicated `redistributionAddress` instead of
   `owner()`.

---

## Conclusion

The Ethena OFT contracts are **well-designed and follow the canonical LayerZero V2 OFT
pattern** with additive customizations (rate limiter, blacklist, 2-step ownership). No
critical, high, or medium severity vulnerability was identified. The two Low findings
(F-1, F-2) are robustness/CEI gaps that are dormant in production but worth fixing as
defense-in-depth. The Informational findings are design tradeoffs (per-path rate limit,
cross-chain blacklist lag) that are acceptable given the protocol's operational model.

The highest-value contract (`StakedUSDeOFTAdapter`, $416M TVL) has the most interesting
finding (F-1), but the trigger condition (innerToken `hasRole` reverting) is not
attacker-controllable and requires the trusted sUSDe contract to misbehave. The finding
is documented for defense-in-depth, not as an exploitable vulnerability.

**No PoC file is written** because no critical bug was found. Per the task brief: "If no
bug found, say so honestly."
