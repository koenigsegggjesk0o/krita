// SPDX-License-Identifier: UNLICENSED
pragma solidity ^0.8.22;

import { OFTOwnable2Step } from "../libs/OFTOwnable2Step.sol";
import { RateLimiter } from "../libs/RateLimiter.sol";

/**
 * @title ENAOFT
 */
contract ENAOFT is OFTOwnable2Step, RateLimiter {
    // Address of the rate limiter
    address public rateLimiter;

    // Event emitted when the rate limiter is set
    event RateLimiterSet(address indexed rateLimiter);

    // Error to be thrown when only the rate limiter is allowed to perform an action
    error OnlyRateLimiter();

    /**
     * @dev Constructor to initialize the ENAOFT contract.
     * @param _rateLimitConfigs An array of RateLimitConfig structures defining the rate limits.
     * @param _name The name of the token.
     * @param _symbol The symbol of the token.
     * @param _lzEndpoint Address of the LZ endpoint.
     * @param _delegate Address of the OApp delegate.
     */
    constructor(
        RateLimitConfig[] memory _rateLimitConfigs,
        string memory _name,
        string memory _symbol,
        address _lzEndpoint,
        address _delegate
    ) OFTOwnable2Step(_name, _symbol, _lzEndpoint, _delegate) {
        _setRateLimits(_rateLimitConfigs);
    }

    /**
     * @dev Sets the rate limiter contract address. Only callable by the owner.
     * @param _rateLimiter Address of the rate limiter contract.
     */
    function setRateLimiter(address _rateLimiter) external onlyOwner {
        rateLimiter = _rateLimiter;
        emit RateLimiterSet(_rateLimiter);
    }

    /**
     * @dev Sets the rate limits based on RateLimitConfig array. Only callable by the owner or the rate limiter.
     * @param _rateLimitConfigs An array of RateLimitConfig structures defining the rate limits.
     */
    function setRateLimits(RateLimitConfig[] calldata _rateLimitConfigs) external {
        if (msg.sender != rateLimiter && msg.sender != owner()) revert OnlyRateLimiter();
        _setRateLimits(_rateLimitConfigs);
    }

    /**
     * @dev Checks and updates the rate limit before initiating a token transfer.
     * @param _amountLD The amount of tokens to be transferred.
     * @param _minAmountLD The minimum amount of tokens expected to be received.
     * @param _dstEid The destination endpoint identifier.
     * @return amountSentLD The actual amount of tokens sent.
     * @return amountReceivedLD The actual amount of tokens received.
     */
    function _debit(
        uint256 _amountLD,
        uint256 _minAmountLD,
        uint32 _dstEid
    ) internal virtual override returns (uint256 amountSentLD, uint256 amountReceivedLD) {
        _checkAndUpdateRateLimit(_dstEid, _amountLD);
        return super._debit(_amountLD, _minAmountLD, _dstEid);
    }
}
