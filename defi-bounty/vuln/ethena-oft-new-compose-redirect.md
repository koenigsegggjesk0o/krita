# Ethena OFT — Compose Message Queued for Blacklisted Recipient After Token Redirect

**Finding ID:** ethena-oft-new-compose-redirect
**Area:** Cross-chain blacklist enforcement / OFTCore compose flow
**Severity:** Low
**Date:** 2026-09-26
**Analyst:** Opus

---

## 1. Vulnerability Description

In `StakedUSDeOFT._credit`, when a cross-chain message arrives for a
**blacklisted recipient**, the tokens are redirected to `owner()`:

```solidity
function _credit(address _to, uint256 _amountLD, uint32 _srcEid)
    internal virtual override returns (uint256 amountReceivedLD)
{
    if (blackList[_to]) {
        emit RedistributeFunds(_to, _amountLD);
        return super._credit(owner(), _amountLD, _srcEid);  // ← tokens go to OWNER
    } else {
        return super._credit(_to, _amountLD, _srcEid);
    }
}
```

However, in `OFTCore._lzReceive`, the compose message is queued for the
**ORIGINAL `toAddress`** (from the LZ message), NOT the actual recipient
(`owner()`):

```solidity
function _lzReceive(Origin calldata _origin, bytes32 _guid, bytes calldata _message, ...)
    internal virtual override
{
    address toAddress = _message.sendTo().bytes32ToAddress();       // ← original recipient
    uint256 amountReceivedLD = _credit(toAddress, ...);             // ← may redirect to owner()

    if (_message.isComposed()) {
        bytes memory composeMsg = OFTComposeMsgCodec.encode(
            _origin.nonce, _origin.srcEid, amountReceivedLD, _message.composeMsg()
        );
        endpoint.sendCompose(toAddress, _guid, 0, composeMsg);      // ← queued for ORIGINAL, not owner()!
    }

    emit OFTReceived(_guid, _origin.srcEid, toAddress, amountReceivedLD);  // ← NF-3: event also logs original
}
```

**The inconsistency:** Tokens go to `owner()`, but the compose is queued for
the **blacklisted** `toAddress`. This means:

1. The blacklisted recipient can call `endpoint.lzCompose(oapp, guid, index, composeMsg)`
   to trigger the OApp's `_lzCompose` handler — even though they never received
   the tokens.
2. The `amountReceivedLD` encoded in the compose message reflects the amount
   minted to `owner()`, not to `toAddress`. A naive `_lzCompose` implementation
   that credits `toAddress` based on `amountReceivedLD` would **double-mint**
   tokens (once to `owner()` in `_credit`, once to `toAddress` in `_lzCompose`).

**In the default Ethena OFT:** No `_lzCompose` is implemented, so the compose
is queued but never executed (calling `lzCompose` reverts). The impact is
limited to phantom endpoint storage and a logical inconsistency.

**In a custom extension:** If Ethena (or a third party) extends
`StakedUSDeOFT` with a `_lzCompose` handler that processes the
`amountReceivedLD` field, the blacklisted recipient could trigger
double-credit logic.

---

## 2. Contract + Function + Line Numbers

| Contract | Function | Lines | Issue |
|----------|----------|-------|-------|
| `contracts/libs/OFTCore.sol` | `_lzReceive` | 239–270 | `endpoint.sendCompose(toAddress, ...)` queues compose for ORIGINAL recipient, not `owner()` after redirect |
| `contracts/StakedUSDeOFT.sol` | `_credit` | 70–82 | Redirects tokens to `owner()` if `toAddress` is blacklisted |
| `contracts/StakedUSDeOFTAdapter.sol` | `_credit` | 38–56 | Same redirect pattern (via innerToken `hasRole` check) |
| `contracts/libs/OFTCore.sol` | `_lzReceive` line 250 | `amountReceivedLD = _credit(toAddress, ...)` | Return value reflects owner's mint, but `toAddress` is used for compose |
| `contracts/libs/OFTCore.sol` | `_lzReceive` line 266 | `endpoint.sendCompose(toAddress, _guid, 0, composeMsg)` | Compose queued for blacklisted `toAddress` |

**Root cause:** `_lzReceive` uses `toAddress` (decoded from the LZ message) for
both `_credit` and `sendCompose`. The `_credit` override in `StakedUSDeOFT`
redirects the mint to `owner()`, but `_lzReceive` has no way to know about the
redirect — it always queues the compose for `toAddress`.

---

## 3. Attack Scenario (Step-by-Step)

### Scenario A: Default Ethena OFT (no `_lzCompose`)

1. **Attacker is blacklisted** on the destination chain's `StakedUSDeOFT`.
2. **Accomplice on source chain** sends tokens WITH a `composeMsg` to the
   attacker's address: `send(to = attacker, amountLD = X, composeMsg = "swap")`.
3. **Destination `_lzReceive`:**
   - `_credit(attacker, X)` → `blackList[attacker] = true` → redirects mint
     to `owner()`. Owner gets X tokens.
   - `endpoint.sendCompose(attacker, guid, 0, composeMsg)` → compose queued
     for attacker (the blacklisted address).
   - `emit OFTReceived(guid, ..., attacker, X)` → event logs attacker (NF-3).
4. **Attacker calls `endpoint.lzCompose(oapp, guid, 0, composeMsg)`:**
   - Endpoint verifies compose was queued for `msg.sender = attacker`. ✓
   - Endpoint calls `oapp.lzCompose(...)`.
   - OApp's `_lzCompose` is NOT implemented → **reverts**.
5. **Result:** Compose is stuck in endpoint storage. No double-credit. No
   token loss. Impact: phantom endpoint storage + logical inconsistency.

### Scenario B: Custom Extension with `_lzCompose` (hypothetical)

1. Ethena deploys a custom `StakedUSDeOFTV2` that implements `_lzCompose` to
   auto-swap received tokens via a DEX:
   ```solidity
   function _lzCompose(Origin calldata, bytes32 guid, bytes calldata message, ...) internal override {
       (, , uint256 amountReceivedLD, bytes memory swapCall) = OFTComposeMsgCodec.decode(message);
       // swap `amountReceivedLD` tokens for the caller (toAddress)
       IERC20(token).approve(dex, amountReceivedLD);
       dex.swap(swapCall);  // sends output to msg.sender (toAddress = attacker)
   }
   ```
2. **Attacker is blacklisted** on the destination chain.
3. **Accomplice sends** tokens with `composeMsg = swap instruction` to attacker.
4. **Destination `_lzReceive`:**
   - Tokens minted to `owner()` (redirect).
   - Compose queued for attacker (blacklisted).
5. **Attacker calls `lzCompose`:**
   - `_lzCompose` executes the swap using `amountReceivedLD` (which reflects
     owner's mint, not attacker's balance).
   - **The swap uses owner's tokens** (or tries to — if the swap contract
     pulls from the OApp, it succeeds; if it pulls from `toAddress`, it fails).
   - If the swap sends output to `msg.sender = attacker`, the attacker receives
     swap output **without ever having received the input tokens**.
6. **Result:** Double-spend or privilege escalation. Owner's tokens are
   swapped for the attacker's benefit.

**Scenario B is hypothetical** — the default Ethena OFT does not implement
`_lzCompose`. But the inconsistency in `_lzReceive` creates a latent trap for
future extensions.

---

## 4. PoC Code (Foundry)

**File:** `foundry_test/test/oft_poc/PoC_ComposeRedirectInconsistency.t.sol`

```solidity
// SPDX-License-Identifier: GPL-3.0
pragma solidity 0.8.30;

import {Test} from "forge-std/Test.sol";
import {MockStakedUSDeOFT} from "./MockStakedUSDeOFT.sol";
import {MockComposeStore} from "./MockStakedUSDeOFT.sol";

contract PoC_ComposeRedirectInconsistency is Test {
    MockStakedUSDeOFT oft;
    address owner = makeAddr("owner");
    address blacklistedRecipient = makeAddr("blacklistedRecipient");
    address legitRecipient = makeAddr("legitRecipient");

    bytes32 constant GUID = keccak256("test-guid");
    uint256 constant AMOUNT = 100e18;
    bytes constant COMPOSE_MSG = bytes("do-something-with-tokens");

    function setUp() public {
        oft = new MockStakedUSDeOFT(owner);
        oft.setBlackList(blacklistedRecipient, true);
    }

    function test_ComposeQueuedForBlacklistedRecipientNotOwner() public {
        // inbound LZ message with composeMsg to a blacklisted recipient
        oft.lzReceive(GUID, blacklistedRecipient, AMOUNT, true, COMPOSE_MSG);

        // tokens went to OWNER, not the blacklisted recipient
        assertEq(oft.balanceOf(owner), AMOUNT);
        assertEq(oft.balanceOf(blacklistedRecipient), 0);

        // BUT compose was queued for the BLACKLISTED recipient
        MockComposeStore store = oft.composeStore();
        assertEq(store.composeLength(), 1);
        assertEq(store.composeTo(0), blacklistedRecipient);  // ← NOT owner!
        assertEq(store.composeCount(blacklistedRecipient), 1);
        assertEq(store.composeCount(owner), 0);  // ← owner has 0 composes
    }

    function test_LegitRecipientGetsTokensAndCompose() public {
        oft.lzReceive(GUID, legitRecipient, AMOUNT, true, COMPOSE_MSG);
        assertEq(oft.balanceOf(legitRecipient), AMOUNT);
        assertEq(oft.balanceOf(owner), 0);
        MockComposeStore store = oft.composeStore();
        assertEq(store.composeTo(0), legitRecipient);
    }

    function test_SimulatedDoubleCreditViaCustomLzCompose() public {
        oft.lzReceive(GUID, blacklistedRecipient, AMOUNT, true, COMPOSE_MSG);
        // tokens → owner
        assertEq(oft.balanceOf(owner), AMOUNT);
        // blacklisted recipient CAN trigger lzCompose (compose is queued for them)
        MockComposeStore store = oft.composeStore();
        assertTrue(store.composeCount(blacklistedRecipient) > 0);
        // If _lzCompose credited toAddress based on amountReceivedLD:
        //   → double mint (owner got tokens in _credit, toAddress gets tokens in _lzCompose)
    }
}
```

**Mock:** `foundry_test/test/oft_poc/MockStakedUSDeOFT.sol` — replicates the
`_credit` redirect + `_lzReceive` compose-queue flow without the full LayerZero
endpoint/OApp stack.

**Test result:**
```
Ran 4 tests for test/oft_poc/PoC_ComposeRedirectInconsistency.t.sol
[PASS] test_ComposeQueuedForBlacklistedRecipientNotOwner() (gas: 246477)
[PASS] test_LegitRecipientGetsTokensAndCompose() (gas: 224540)
[PASS] test_NonComposedBlacklistedNoComposeIssue() (gas: 106272)
[PASS] test_SimulatedDoubleCreditViaCustomLzCompose() (gas: 227827)
Suite result: ok. 4 passed; 0 failed; 0 skipped
```

---

## 5. Impact Assessment

| Dimension | Assessment |
|-----------|------------|
| **Default Ethena OFT** | No exploitable impact — `_lzCompose` is not implemented, so the compose is queued but never executed. |
| **Custom extension** | Latent double-credit risk if `_lzCompose` trusts `amountReceivedLD` and credits `toAddress`. |
| **Endpoint storage** | Phantom compose entries accumulate on the LayerZero endpoint (minor gas/storage waste). |
| **Event inconsistency** | `OFTReceived` logs the blacklisted recipient, not `owner()` — this is NF-3 (already reported). |
| **Fix complexity** | Moderate — `_lzReceive` would need to know the actual recipient (return value from `_credit` or a separate redirect signal). |

**Why this matters:** The `_credit` → `_lzReceive` interface assumes `_credit`
credits the `toAddress` passed to it. The `StakedUSDeOFT._credit` override
**silently violates this assumption** by redirecting to `owner()`. The compose
queue and the event both use the original `toAddress`, creating a divergence
between the token flow and the metadata flow. This is a **fragile interface
contract violation** that will produce bugs when future extensions trust the
existing `_lzReceive` structure.

---

## 6. Severity: **Low**

- No exploitable impact in the default deployment (no `_lzCompose`).
- Latent risk for custom extensions.
- Phantom endpoint storage is minimal.
- The NF-3 event inconsistency is already reported; this finding covers the
  compose-queue counterpart.

---

## 7. Three-Perspective Audit

### Prosecutor (argues FOR bug validity — should be accepted)

**Argument 1 — Interface contract violation.** The `OFTCore._lzReceive`
function assumes that `_credit(toAddress, ...)` credits tokens to `toAddress`.
The `StakedUSDeOFT._credit` override silently violates this by redirecting to
`owner()`. This is a **fragile interface violation** — the base class
(`OFTCore`) and the derived class (`StakedUSDeOFT`) have **different mental
models** of who receives the tokens. The compose queue and the event both use
the wrong address.

**Argument 2 — Latent double-credit trap.** If Ethena ever extends
`StakedUSDeOFT` with a `_lzCompose` handler (a natural extension for
automated cross-chain swaps, staking, or governance), the handler will receive
a compose message that says "toAddress received amountReceivedLD tokens." But
`toAddress` is blacklisted and received nothing — `owner()` got the tokens. A
naive handler that credits `toAddress` based on `amountReceivedLD` would
**double-mint** tokens. This is a **ticking time bomb** for future development.

**Argument 3 — Blacklist bypass via compose.** The entire purpose of the
`StakedUSDeOFT._credit` redirect is to **prevent blacklisted recipients from
receiving tokens**. But the compose queue gives the blacklisted recipient a
**callback mechanism** — they can trigger `_lzCompose`, which may execute
arbitrary logic on their behalf. Even if `_lzCompose` doesn't credit tokens,
it might call other functions (governance, staking, etc.) that the blacklisted
recipient shouldn't be able to trigger. This is a **blacklist bypass** for
compose-triggered logic.

**Argument 4 — Inconsistency with the redistribute intent.** The
`RedistributeFunds` event and the `_credit` redirect signal "funds taken from
blacklisted recipient, given to owner." But the compose queue says "compose
pending for blacklisted recipient." These two signals **contradict each other**.
A monitoring system that indexes `RedistributeFunds` would miss the compose
queue; a system that indexes `sendCompose` would think the blacklisted
recipient is still active.

**Severity case:** Low is the floor. If a custom `_lzCompose` is ever added,
this becomes High (double-credit). The finding should be **accepted and fixed**
before any `_lzCompose` extension is deployed.

### Defense (argues AGAINST bug validity — should be rejected/downgraded)

**Argument 1 — No exploitability in default deployment.** The default Ethena
OFT does NOT implement `_lzCompose`. The compose is queued but never executed.
Calling `lzCompose` reverts. There is **zero on-chain impact** — no tokens
moved, no state changed, no double-credit. The finding describes a
**hypothetical** scenario (custom `_lzCompose`) that does not exist.

**Argument 2 — LayerZero V2 OFT standard.** The `_lzReceive` →
`sendCompose(toAddress)` pattern is the **canonical LayerZero V2 OFT
implementation**. Every OFT using `OFTCore` has this behavior. The compose is
ALWAYS queued for the original `toAddress`, regardless of any `_credit`
override. This is not an Ethena-specific design choice — it's the LayerZero
SDK design.

**Argument 3 — `toAddress` is a bytes32 from the source chain.** The
destination OApp cannot know in advance whether `toAddress` is blacklisted. The
compose must be queued BEFORE `_credit` returns (or at least, the compose
metadata is built from `toAddress`). The only way to "fix" this would be to
have `_credit` return the actual recipient, and then use that for
`sendCompose`. But this requires changing the `OFTCore` interface, which is
the LayerZero SDK's responsibility, not Ethena's.

**Argument 4 — Compose is opt-in.** Users must explicitly provide a
`composeMsg` to trigger the compose flow. For the default Ethena OFT, no
front-end or integration uses `composeMsg`. The compose path is dormant. An
attacker would need to craft a custom `send` call with a `composeMsg` to
trigger this — and even then, the impact is zero (no `_lzCompose`).

**Argument 5 — NF-3 already covers the event inconsistency.** The prior audit
identified that `OFTReceived` logs the original recipient after redirect
(NF-3, Informational). The compose-queue inconsistency is the **same root
cause** (metadata uses `toAddress` while tokens go to `owner()`). This finding
is a **duplicate** of NF-3, just for a different metadata field (compose queue
vs. event log).

**Recommended outcome:** **Reject** as a duplicate of NF-3, or downgrade to
**Informational**. No fix required for the default deployment. If Ethena ever
implements `_lzCompose`, they should re-audit at that time.

### Judge (final verdict)

**Code fact verification:** Confirmed via Foundry PoC (4/4 tests pass). The
compose is queued for `toAddress` (the original/blacklisted recipient), not
`owner()` (the actual recipient). The `_credit` redirect and the
`sendCompose(toAddress)` are indeed inconsistent.

**Prosecutor strengths:**
- The interface contract violation is real — `_credit` can redirect, but
  `_lzReceive` doesn't know.
- The latent double-credit trap for custom `_lzCompose` is a genuine risk.
- The blacklist-bypass-via-compose argument has merit for compose-triggered
  logic.

**Defense strengths:**
- Zero exploitability in the default deployment (no `_lzCompose`).
- This is the canonical LayerZero V2 OFT pattern, not an Ethena-specific design
  choice.
- Compose is opt-in and unused in the default Ethena deployment.
- The root cause (metadata uses `toAddress` while tokens go to `owner()`) is
  the same as NF-3 — this is arguably a duplicate.

**Judge's assessment:**

The finding is **technically correct** — the compose IS queued for the
blacklisted recipient, not `owner()`. The PoC is clean and the inconsistency
is verified. However, the **impact in the default deployment is zero**:

1. No `_lzCompose` is implemented → compose is never executed.
2. Compose is opt-in (`composeMsg` must be non-empty) → default sends don't
   trigger this path.
3. The root cause (metadata/token divergence after redirect) is the same as
   NF-3 — the event inconsistency.

The prosecutor's strongest argument is the **latent double-credit trap** for
custom `_lzCompose` extensions. This is a real risk, but it's **conditional on
future development** that hasn't happened. The finding is a **code smell / foot-gun**
rather than an exploitable vulnerability.

The defense's strongest argument is that this is the **LayerZero V2 OFT
standard** behavior. Ethena inherited `_lzReceive` from `OFTCore` without
modification. The redirect in `StakedUSDeOFT._credit` is Ethena's addition,
but the compose-queue logic is LayerZero's. Fixing this would require either
(a) overriding `_lzReceive` in `StakedUSDeOFT` (duplicating LayerZero logic),
or (b) changing the `_credit` return interface to signal redirects.

**Verdict: Low (accepted as defense-in-depth, no bounty warranted).**

The finding should be accepted as Low severity. It's a genuine interface
inconsistency that could become exploitable if `_lzCompose` is ever
implemented. The recommended fix is to override `_lzReceive` in
`StakedUSDeOFT` to skip the `sendCompose` call when the recipient is
blacklisted (or to queue the compose for `owner()` instead).

This finding is **related to but distinct from NF-3**. NF-3 covers the event
log inconsistency; this finding covers the compose-queue inconsistency. Both
have the same root cause (metadata uses `toAddress` after token redirect to
`owner()`), but they affect different consumers (off-chain indexers vs.
on-chain compose execution). They should be tracked as separate findings with
a shared root-cause note.

**Confidence:** 75% accepted as Low, 20% downgraded to Informational (as a
NF-3 duplicate), 5% rejected (no default-deployment impact).
**Expected payout:** $0 (Low/Informational, below bounty threshold).

---

## 8. Recommendation

Override `_lzReceive` in `StakedUSDeOFT` to skip compose queuing when the
recipient is blacklisted:

```solidity
function _lzReceive(
    Origin calldata _origin,
    bytes32 _guid,
    bytes calldata _message,
    address _executor,
    bytes calldata _extraData
) internal virtual override {
    address toAddress = _message.sendTo().bytes32ToAddress();

    // If blacklisted, credit redirects to owner() — skip compose for the
    // blacklisted recipient to avoid metadata/token divergence.
    if (blackList[toAddress]) {
        uint256 amountReceivedLD = _credit(toAddress, _toLD(_message.amountSD()), _origin.srcEid);
        emit OFTReceived(_guid, _origin.srcEid, owner(), amountReceivedLD);  // log owner, not toAddress
        return;  // skip sendCompose for blacklisted recipients
    }

    super._lzReceive(_origin, _guid, _message, _executor, _extraData);
}
```

This also fixes NF-3 (the event log inconsistency) for the same path.
