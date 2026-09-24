// SPDX-License-Identifier: MIT
pragma solidity 0.8.30;

import {IOracleFeed} from "../../src/oracle/IOracleFeed.sol";

/// @dev Deterministic oracle feed for tests. Returns whatever price/updatedAt the test sets.
contract MockOracleFeed is IOracleFeed {
    uint256 public price;
    uint256 public updatedAt;

    constructor(uint256 _price, uint256 _updatedAt) {
        price = _price;
        updatedAt = _updatedAt;
    }

    function setPrice(uint256 _price, uint256 _updatedAt) external {
        price = _price;
        updatedAt = _updatedAt;
    }

    function getPrice() external returns (uint256, uint256) {
        emit OraclePrice(price);
        return (price, updatedAt);
    }
}
