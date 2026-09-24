// SPDX-License-Identifier: GPL-3.0
pragma solidity 0.8.30;

import {RateLimiter} from "src/oft_libs/RateLimiter.sol";

/// @title MockRateLimitedOFT
/// @notice Minimal mock that replicates the rate-limit + dust-removal interaction
///         found in USDeOFT._debit / ENAOFT._debit without the full LayerZero stack.
///         decimalConversionRate = 1e12 (18-decimal local, 6-decimal shared).
contract MockRateLimitedOFT is RateLimiter {
    uint256 public constant DECIMAL_CONVERSION_RATE = 1e12; // 18 - 6

    uint256 public totalSupply;
    mapping(address => uint256) public balanceOf;
    mapping(address => uint256) public burnCalls; // track _burn invocations

    event RateLimitConsumed(uint32 dstEid, uint256 amountConsumed, uint256 remainingBudget);
    event TokensBurned(address from, uint256 amount); // actual burn (post-dust)

    constructor(RateLimitConfig[] memory configs) {
        _setRateLimits(configs);
    }

    function mint(address to, uint256 amount) external {
        balanceOf[to] += amount;
        totalSupply += amount;
    }

    /// @notice Replicates USDeOFT._debit → _checkAndUpdateRateLimit(amountLD) then
    ///         OFT._debit → _debitView → _removeDust(amountLD) → _burn(amountSentLD)
    function debit(uint32 dstEid, uint256 amountLD, uint256 minAmountLD)
        external
        returns (uint256 amountSentLD, uint256 amountReceivedLD)
    {
        // ---- USDeOFT._debit: rate-limit check uses the RAW amountLD (pre-dust) ----
        (uint256 currentInFlight, uint256 canSend) =
            _amountCanBeSent(rateLimits[dstEid].amountInFlight, rateLimits[dstEid].lastUpdated, rateLimits[dstEid].limit, rateLimits[dstEid].window);
        _checkAndUpdateRateLimit(dstEid, amountLD);
        emit RateLimitConsumed(dstEid, amountLD, canSend > amountLD ? canSend - amountLD : 0);

        // ---- OFT._debit → _debitView: dust removal happens AFTER rate-limit check ----
        amountSentLD = _removeDust(amountLD);
        amountReceivedLD = amountSentLD;
        if (amountReceivedLD < minAmountLD) revert("SlippageExceeded");

        // ---- OFT._debit: burn the dust-removed amount (could be 0!) ----
        if (amountSentLD > 0) {
            require(balanceOf[msg.sender] >= amountSentLD, "insufficient balance");
            balanceOf[msg.sender] -= amountSentLD;
            totalSupply -= amountSentLD;
        }
        burnCalls[msg.sender] += 1;
        emit TokensBurned(msg.sender, amountSentLD);
    }

    function _removeDust(uint256 amountLD) internal pure returns (uint256) {
        return (amountLD / DECIMAL_CONVERSION_RATE) * DECIMAL_CONVERSION_RATE;
    }

    // expose for tests
    function amountCanBeSent(uint32 dstEid) external view returns (uint256, uint256) {
        RateLimit memory rl = rateLimits[dstEid];
        return _amountCanBeSent(rl.amountInFlight, rl.lastUpdated, rl.limit, rl.window);
    }
}
