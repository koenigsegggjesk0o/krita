// SPDX-License-Identifier: GPL-3.0
pragma solidity 0.8.30;

import {EnumerableSet} from "@openzeppelin/contracts/utils/structs/EnumerableSet.sol";
import {IPSM} from "./IPSM.sol";

/**
 * @title CollateralStateMap
 * @notice Enumerable map from collateral address to CollateralState.
 * @dev Combines an EnumerableSet of keys with a mapping to CollateralState, providing
 *      O(1) existence checks, O(1) value access, and O(n) enumeration. Replaces the
 *      sentinel address(0) pattern for collateral existence checks.
 */
library CollateralStateMap {
    using EnumerableSet for EnumerableSet.AddressSet;

    struct Map {
        EnumerableSet.AddressSet _keys;
        mapping(address => IPSM.CollateralState) _values;
    }

    function add(Map storage map, address key, IPSM.CollateralConfig calldata config) internal {
        map._keys.add(key);
        map._values[key].config = config;
    }

    function remove(Map storage map, address key) internal {
        map._keys.remove(key);
        delete map._values[key].config;
    }

    function contains(Map storage map, address key) internal view returns (bool) {
        return map._keys.contains(key);
    }

    function length(Map storage map) internal view returns (uint256) {
        return map._keys.length();
    }

    function at(Map storage map, uint256 index) internal view returns (address) {
        return map._keys.at(index);
    }

    function get(Map storage map, address key) internal view returns (IPSM.CollateralState storage) {
        return map._values[key];
    }
}
