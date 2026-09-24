// SPDX-License-Identifier: GPL-3.0
pragma solidity 0.8.30;

/**
 * @title IOracleFeed
 * @notice Interface for oracle price feeds in the on-chain minting system
 * @dev This interface defines the standard API for oracle implementations that provide
 *      price data for collateral assets in the minting system.
 * @dev Implementations should ensure price accuracy, staleness checks, and proper error handling.
 * @dev The interface is designed to be compatible with various oracle providers including
 *      Chainlink, Pyth, and custom implementations.
 *
 * Key Requirements:
 * - Price validation and staleness checks
 * - Timestamp validation
 * - Proper error handling for invalid prices
 * - Event emission for price updates
 */
interface IOracleFeed {
    /**
     * @notice Error thrown when oracle price is invalid
     * @dev Used when price is zero, negative, or otherwise unusable
     */
    error InvalidOraclePrice();

    /**
     * @notice Error thrown when oracle address is invalid
     * @dev Used when oracle contract address is zero or invalid
     */
    error InvalidOracleAddress();

    /**
     * @notice Emitted when a new oracle price is received
     * @param price The validated price from the oracle
     */
    event OraclePrice(uint256 price);

    /**
     * @notice Gets the latest price and timestamp from the oracle
     * @dev This function should perform all necessary validation including:
     *      - Price validity checks
     *      - Staleness validation
     *      - Timestamp verification
     * @return price The validated price (18 decimals)
     * @return updatedAt Timestamp when the price was last updated
     * @dev Reverts with InvalidOraclePrice if price validation fails
     * @dev Should emit OraclePrice event with the validated price
     */
    function getPrice() external returns (uint256 price, uint256 updatedAt);
}
