# Finding #2 — Uncaught `ArithmeticException` on `int256`/`uint256 → long` truncation in ERC-20 transfer & HBAR-approve translators

**Severity:** Low (self-inflicted gas griefing / robustness; no fund loss)
**Component:** HTS ERC-20 redirect transfers; HAS `hbarApprove`
**Immunefi scope:** `hiero-consensus-node` / `hedera-smart-contract-service-impl`
**Repo / commit:** `hashgraph/hedera-services` @ `51f889b` (v0.79.0-SNAPSHOT)

## Contract + function + line

### (a) ERC-20 transfer / transferFrom

File:
`hedera-node/hedera-smart-contract-service-impl/src/main/java/com/hedera/node/app/service/contract/impl/exec/systemcontracts/hts/transfer/Erc20TransfersTranslator.java`

```
L70   if (attempt.isSelector(ERC_20_TRANSFER)) {
L71       final var call = Erc20TransfersTranslator.ERC_20_TRANSFER.decodeCall(...);
L72       return callFrom(null, call.get(0), call.get(1), attempt, false);
L73   } else {
L74       final var call = Erc20TransfersTranslator.ERC_20_TRANSFER_FROM.decodeCall(...);
L75       return callFrom(call.get(0), call.get(1), call.get(2), attempt, true);
L76   }
...
L90   amount.longValueExact(),   // <-- BigInteger → long, throws ArithmeticException if overflow
```

`ERC_20_TRANSFER = "transfer(address,uint256)"`, `ERC_20_TRANSFER_FROM = "transferFrom(address,address,uint256)"`
(both `CallVia.PROXY`).

### (b) HBAR approve (classic + proxy)

File:
`hedera-node/hedera-smart-contract-service-impl/src/main/java/com/hedera/node/app/service/contract/impl/exec/systemcontracts/has/hbarapprove/HbarApproveTranslator.java`

```
L38   HBAR_APPROVE = "hbarApprove(address,address,int256)"
L32   HBAR_APPROVE_PROXY = "hbarApprove(address,int256)"   // CallVia.PROXY
...
L108   .amount(amount.longValueExact())   // <-- BigInteger → long, throws ArithmeticException if overflow
```

## Description

Both translators decode an ABI `uint256` / `int256` amount into a `java.math.BigInteger` (via headlong) and then
narrow it to a `long` with `BigInteger.longValueExact()`. `longValueExact()` throws
`ArithmeticException("BigInteger out of long range")` when the value does not fit in a signed 64-bit long
(i.e. `amount > 2^63 - 1` for `uint256`, or `amount < -2^63` / `amount > 2^63 - 1` for `int256`).

That `ArithmeticException` is **not** caught locally; it propagates out of the translator's `callFrom(...)`,
which runs inside `AbstractNativeSystemContract.computeFully`'s attempt-translation try/catch:

File:
`hedera-node/hedera-smart-contract-service-impl/src/main/java/com/hedera/node/app/service/contract/impl/exec/systemcontracts/common/AbstractNativeSystemContract.java`

```
L181   } catch (final Exception ignore) {
L182       // Input that cannot be translated to an executable call, for any
L183       // reason, halts the frame and consumes all remaining gas
L184       return haltResult(INVALID_OPERATION, frame.getRemainingGas());
L185   }
```

So an out-of-range amount is translated into an `INVALID_OPERATION` exceptional halt that **consumes all remaining
gas** in the frame, instead of a clean, cheap revert with a precise status code (e.g. `INVALID_AMOUNT`).

## Attack scenario

1. Attacker contract `A` holds some HTS fungible token (or targets any deployed HTS token via its long-zero
   redirect address).
2. `A` calls `IERC20(token).transfer(recipient, type(uint256).max)` (or any value `> 2^63 - 1`).
   - This is routed through the token proxy → `redirectForToken` → `HTS.ERC_20_TRANSFER`.
   - `Erc20TransfersTranslator.callFrom` executes `amount.longValueExact()` on `type(uint256).max`.
   - `ArithmeticException` is thrown; `computeFully` catches it and returns
     `haltResult(INVALID_OPERATION, frame.getRemainingGas())`.
3. The entire frame's remaining gas is consumed and the outer transaction reverts.

For `hbarApprove`, the same holds for an `int256` amount outside the `long` range.

### PoC sketch

```solidity
// SPDX-License-Identifier: MIT
pragma solidity ^0.8.0;

interface IERC20Like { function transfer(address to, uint256 amount) external returns (bool); }

contract HederaInt256Grief {
    // token = long-zero address of an HTS fungible token (0x000...0XXX)
    function grief(address token, address victim) external {
        // type(uint256).max > 2^63-1  ==> ArithmeticException in Erc20TransfersTranslator
        IERC20Like(token).transfer(victim, type(uint256).max);
    }
}
```

Expected on-chain behavior: the call exceptional-halts with `INVALID_OPERATION` and burns all gas supplied to the
frame. No token moves.

## Impact

- **Theft / unauthorized transfer:** None. The exception fires *before* any synthetic `CryptoTransfer` body is
  built or dispatched; no state changes occur.
- **Fund loss:** None. The payer loses only the gas they themselves supplied.
- **Gas griefing:** Self-inflicted only — the attacker pays for the gas they burn. It is *not* a vector to burn a
  third party's gas beyond what that third party already authorized for the frame.
- **Robustness / UX:** The error surface is inconsistent with the rest of the HTS/HAS API, which returns explicit
  `ResponseCodeEnum` status codes for bad amounts (e.g. `INVALID_AMOUNT`, `NEGATIVE_ALLOWANCE_AMOUNT`). A caller
  gets an opaque `INVALID_OPERATION` + total-gas-consumption instead of a clean revert, which can mask bugs in
  calling contracts (a contract that tries `transfer(…, type(uint256).max)` as a sentinel gets a hard halt instead
  of a `false`/revert it can branch on).

### Why `hbarApprove` is the slightly more interesting case

`hbarApprove(address owner, address spender, int256 amount)` lets a *contract* set an HBAR crypto-allowance with
an owner-chosen `int256` amount. `CryptoApproveAllowanceHandler.pureChecks` already rejects *negative* amounts
(`NEGATIVE_ALLOWANCE_AMOUNT`, L106), but a *positive* amount `> 2^63 - 1` never reaches that check — it dies at
`longValueExact()` first. So the truncation boundary is enforced by an exception rather than by a deliberate
validation, which is fragile if future refactors move/change the `pureChecks` ordering.

## Severity rationale

Low. No asset can be moved, frozen, or burned; no third-party gas can be consumed beyond what the payer authorized.
The only effect is an opaque all-gas-consumed halt for an out-of-range amount, paid for by the caller. It does
not meet the CRITICAL/HIGH threshold of the Hedera Immunefi program. It is a code-quality/robustness defect
worth a one-line fix.

## Suggested fix

Validate the amount range explicitly and return a clean status code instead of relying on `longValueExact()`:

```java
// Erc20TransfersTranslator
final BigInteger amountBig = (BigInteger) call.get(idx);
if (amountBig.signum() < 0 || amountBig.compareTo(BigInteger.valueOf(Long.MAX_VALUE)) > 0) {
    // build a call that reverts with INVALID_AMOUNT instead of throwing
}
final long amount = amountBig.longValue();
```

```java
// HbarApproveTranslator.cryptoApproveTransactionBody
if (amount.signum() < 0 || amount.compareTo(BigInteger.valueOf(Long.MAX_VALUE)) > 0) {
    throw new HandleException(INVALID_AMOUNT);   // caught cleanly by computeFully's HandleException branch
}
final long value = amount.longValue();
```

(Using `HandleException` is preferable to `ArithmeticException`, because `computeFully` L158–180 maps a
`HandleException` to a clean `revertResult(status, …)` that does *not* consume all remaining gas.)

## Three-perspective audit

**Defender (Hedera team) perspective.**
This is a latent robustness papercut, not a security hole. The `catch (Exception ignore)` at
`AbstractNativeSystemContract` L181 makes the failure safe (no state change, no crash), but it also makes it
noisy: every out-of-range amount consumes all remaining gas and logs nothing useful. A targeted `HandleException`
+ explicit range check would make the API consistent with the rest of HTS/HAS and remove the all-gas-burn
penalty. Low priority, but trivial to fix.

**Attacker perspective.**
No exploitable primitive. The amount is validated (by exception) before any dispatch, so there is no integer
truncation that silently changes semantics — `2^63` does **not** wrap to a small/valid amount and authorize an
unexpected transfer/approve. The only outcome is the attacker burning their own gas. An attacker cannot use this
to grief a *different* user (e.g. a DEX pair) beyond causing that user's transaction to revert — but only if the
attacker already controls the input to that transaction, in which case they could simply revert it anyway.

**Protocol-economics perspective.**
The all-gas-consumption penalty for a malformed amount is disproportionate and inconsistent with EVM norms, where
`transfer(to, type(uint256).max)` on a real ERC-20 either succeeds or reverts with the gas actually used. Hedera's
HTS-as-ERC-20 surface is meant to be a drop-in EVM replacement; surprising callers with an `INVALID_OPERATION`
halt + gas burn degrades composability. Worth fixing for EVM-compatibility parity even though it is not a
security issue.

## References

- EIP-20 `transfer(address,uint256)` / `transferFrom(address,address,uint256)`.
- HIP-719 token redirect proxy (`HRC719TokenProxy.sol` → `0x167`).
- HIP-904 `hbarApprove`.
- `AbstractNativeSystemContract.computeFully` exception funnel (L158–185, L321–353).
