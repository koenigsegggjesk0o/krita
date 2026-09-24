// SPDX-License-Identifier: GPL-3.0
pragma solidity 0.8.30;

import {Test} from "forge-std/Test.sol";
import {MockStakedUSDeOFT} from "./MockStakedUSDeOFT.sol";
import {MockComposeStore} from "./MockStakedUSDeOFT.sol";

/// @title PoC_ComposeRedirectInconsistency
/// @notice Demonstrates that in StakedUSDeOFT, when a cross-chain message is sent
///         WITH a composeMsg to a BLACKLISTED recipient:
///
///         1. _credit() redirects the token mint to owner() — owner gets the tokens.
///         2. BUT _lzReceive() still calls endpoint.sendCompose(toAddress, ...) —
///            the compose is queued for the BLACKLISTED recipient, NOT owner().
///         3. The OFTReceived event also logs toAddress (the blacklisted recipient),
///            not owner() — this is NF-3.
///
///         The compose-queue inconsistency (issue #2) means a blacklisted recipient
///         can call endpoint.lzCompose() to trigger the OApp's _lzCompose handler.
///         If a custom OFT extension implements _lzCompose and trusts the
///         amountReceivedLD encoded in the compose message, the blacklisted
///         recipient could trigger logic that assumes THEY received the tokens —
///         even though the tokens went to owner().
///
///         In the DEFAULT Ethena OFT, no _lzCompose is implemented, so the compose
///         is queued but never executed (lzCompose reverts). Impact is minimal
///         (phantom endpoint storage). Severity: Low (latent issue for custom
///         extensions).
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

    /// @notice Core test: blacklisted recipient's tokens go to owner, but the
    ///         compose is queued for the BLACKLISTED recipient.
    function test_ComposeQueuedForBlacklistedRecipientNotOwner() public {
        // simulate an inbound LZ message with composeMsg to a blacklisted recipient
        oft.lzReceive(GUID, blacklistedRecipient, AMOUNT, true, COMPOSE_MSG);

        // tokens were minted to OWNER, not the blacklisted recipient
        assertEq(oft.balanceOf(owner), AMOUNT, "owner received the redirected tokens");
        assertEq(oft.balanceOf(blacklistedRecipient), 0, "blacklisted recipient got 0 tokens");

        // BUT the compose was queued for the BLACKLISTED recipient
        MockComposeStore store = oft.composeStore();
        assertEq(store.composeLength(), 1, "one compose queued");
        assertEq(store.composeTo(0), blacklistedRecipient, "compose queued for blacklisted recipient (NOT owner)");
        assertEq(store.composeCount(blacklistedRecipient), 1, "blacklisted recipient has 1 compose pending");
        assertEq(store.composeCount(owner), 0, "owner has 0 composes pending -- compose NOT redirected");
    }

    /// @notice Contrast: non-blacklisted recipient gets BOTH tokens AND compose.
    function test_LegitRecipientGetsTokensAndCompose() public {
        oft.lzReceive(GUID, legitRecipient, AMOUNT, true, COMPOSE_MSG);

        assertEq(oft.balanceOf(legitRecipient), AMOUNT, "legit recipient received tokens");
        assertEq(oft.balanceOf(owner), 0, "owner got nothing");

        MockComposeStore store = oft.composeStore();
        assertEq(store.composeTo(0), legitRecipient, "compose queued for legit recipient");
    }

    /// @notice Non-composed messages to blacklisted recipients: no compose queued,
    ///         tokens still redirected. No inconsistency in this case.
    function test_NonComposedBlacklistedNoComposeIssue() public {
        oft.lzReceive(GUID, blacklistedRecipient, AMOUNT, false, "");

        assertEq(oft.balanceOf(owner), AMOUNT, "owner received redirected tokens");
        assertEq(oft.balanceOf(blacklistedRecipient), 0, "blacklisted got 0");

        MockComposeStore store = oft.composeStore();
        assertEq(store.composeLength(), 0, "no compose queued (non-composed message)");
    }

    /// @notice Double-credit risk: if a custom _lzCompose credits tokens based on
    ///         the amountReceivedLD in the compose message, the blacklisted
    ///         recipient could trigger _lzCompose and receive tokens AGAIN.
    ///         This test SIMULATES what a naive _lzCompose implementation would do.
    function test_SimulatedDoubleCreditViaCustomLzCompose() public {
        // Step 1: inbound message to blacklisted recipient (with compose)
        oft.lzReceive(GUID, blacklistedRecipient, AMOUNT, true, COMPOSE_MSG);

        // tokens went to owner
        assertEq(oft.balanceOf(owner), AMOUNT, "owner has tokens after redirect");

        // Step 2: SIMULATE a naive _lzCompose that credits toAddress based on
        //         the amountReceivedLD in the compose message.
        //         (In the default Ethena OFT, _lzCompose is NOT implemented,
        //          so this step would revert. But a custom extension COULD do this.)
        //
        // The blacklisted recipient calls lzCompose → _lzCompose is invoked →
        // _lzCompose credits toAddress = blacklistedRecipient with amountReceivedLD.
        //
        // This is a DOUBLE CREDIT: owner already got the tokens in _credit,
        // and now blacklistedRecipient ALSO gets tokens in _lzCompose.

        // We simulate the _lzCompose credit here (not in the mock, because the
        // default OFT doesn't implement it):
        // vm.prank(blacklistedRecipient);
        // oft.simulateLzCompose(GUID, AMOUNT);  // would credit blacklistedRecipient

        // Instead, just verify the preconditions for the double-credit:
        MockComposeStore store = oft.composeStore();
        assertTrue(store.composeCount(blacklistedRecipient) > 0, "blacklisted recipient CAN trigger lzCompose");
        assertEq(oft.balanceOf(blacklistedRecipient), 0, "blacklisted has 0 tokens (no _lzCompose yet)");

        // If _lzCompose were implemented to credit blacklistedRecipient:
        //   balanceOf[blacklistedRecipient] would become AMOUNT
        //   totalSupply would become 2 * AMOUNT (double mint!)
        // This is the latent double-credit risk.
    }
}
