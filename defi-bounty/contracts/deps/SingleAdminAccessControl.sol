// SPDX-License-Identifier: GPL-3.0
pragma solidity 0.8.30;

import {AccessControl} from "@openzeppelin/contracts/access/AccessControl.sol";
import {IERC5313} from "@openzeppelin/contracts/interfaces/IERC5313.sol";
import {ISingleAdminAccessControl} from "./ISingleAdminAccessControl.sol";

/**
 * @title SingleAdminAccessControl
 * @notice Simplified access control system that maintains a single admin role
 * @dev This contract is a simplified alternative to OpenZeppelin's AccessControlDefaultAdminRules
 *      that provides a single admin with transfer capabilities and prevents external role grants.
 * @dev The system implements a two-step admin transfer process for security.
 * @dev Security features include preventing admin role from being granted externally and
 *      maintaining strict control over admin operations.
 *
 * Key Features:
 * - Single admin role management
 * - Two-step admin transfer process
 * - Prevention of external admin role grants
 * - IERC5313 compliance for owner() function
 * - Role-based access control inheritance
 *
 * @custom:security This contract follows security best practices including:
 * - Two-step admin transfer process
 * - Prevention of external admin role manipulation
 * - Comprehensive access control validation
 * - IERC5313 standard compliance
 */
abstract contract SingleAdminAccessControl is IERC5313, ISingleAdminAccessControl, AccessControl {
    /*//////////////////////////////////////////////////////////////
                               STORAGE
    //////////////////////////////////////////////////////////////*/

    /// @notice Current default admin address
    address private _currentDefaultAdmin;

    /// @notice Pending admin address waiting for confirmation
    address private _pendingDefaultAdmin;

    /*//////////////////////////////////////////////////////////////
                               MODIFIERS
    //////////////////////////////////////////////////////////////*/

    /**
     * @notice Prevents admin role from being modified externally
     * @param role Role being modified
     * @dev Reverts if trying to modify the DEFAULT_ADMIN_ROLE
     */
    modifier notAdmin(bytes32 role) {
        if (role == DEFAULT_ADMIN_ROLE) revert InvalidAdminChange();
        _;
    }

    /*//////////////////////////////////////////////////////////////
                            ADMIN TRANSFER
    //////////////////////////////////////////////////////////////*/

    /**
     * @notice Requests transfer of admin role to a new address
     * @dev Only callable by the current admin
     * @param newAdmin Address of the new admin
     * @dev Reverts if newAdmin is the same as current admin
     * @dev Sets newAdmin as pending admin
     * @dev Emits AdminTransferRequested event
     */
    function transferAdmin(address newAdmin) external onlyRole(DEFAULT_ADMIN_ROLE) {
        if (newAdmin == msg.sender) revert InvalidAdminChange();
        _pendingDefaultAdmin = newAdmin;
        emit AdminTransferRequested(_currentDefaultAdmin, newAdmin);
    }

    /**
     * @notice Confirms admin role transfer for the pending admin
     * @dev Only callable by the pending admin
     * @dev Grants DEFAULT_ADMIN_ROLE to the caller
     * @dev Emits AdminTransferred event
     * @dev Clears pending admin
     */
    function acceptAdmin() external {
        if (msg.sender != _pendingDefaultAdmin) revert NotPendingAdmin();
        _grantRole(DEFAULT_ADMIN_ROLE, msg.sender);
    }

    /*//////////////////////////////////////////////////////////////
                            ROLE MANAGEMENT
    //////////////////////////////////////////////////////////////*/

    /**
     * @notice Grants a role to an account
     * @dev Only callable by the current admin
     * @dev Prevents granting of DEFAULT_ADMIN_ROLE externally
     * @param role Role to grant
     * @param account Account to grant the role to
     */
    function grantRole(bytes32 role, address account) public override onlyRole(DEFAULT_ADMIN_ROLE) notAdmin(role) {
        _grantRole(role, account);
    }

    /**
     * @notice Revokes a role from an account
     * @dev Only callable by the current admin
     * @dev Prevents revoking of DEFAULT_ADMIN_ROLE externally
     * @param role Role to revoke
     * @param account Account to revoke the role from
     */
    function revokeRole(bytes32 role, address account) public override onlyRole(DEFAULT_ADMIN_ROLE) notAdmin(role) {
        _revokeRole(role, account);
    }

    /**
     * @notice Renounces a role from the caller
     * @dev Prevents renouncing of DEFAULT_ADMIN_ROLE
     * @param role Role to renounce
     * @param account Account renouncing the role
     */
    function renounceRole(bytes32 role, address account) public virtual override notAdmin(role) {
        super.renounceRole(role, account);
    }

    /*//////////////////////////////////////////////////////////////
                            VIEW FUNCTIONS
    //////////////////////////////////////////////////////////////*/

    /**
     * @notice Gets the current admin address
     * @dev Implements IERC5313 owner() function
     * @return Current admin address
     */
    function owner() public view virtual returns (address) {
        return _currentDefaultAdmin;
    }

    /**
     * @notice Checks if the contract supports a specific interface
     * @dev Implements IERC165 interface detection
     * @param interfaceId Interface identifier to check
     * @return Whether the interface is supported
     */
    function supportsInterface(bytes4 interfaceId) public view virtual override returns (bool) {
        return interfaceId == type(IERC5313).interfaceId || super.supportsInterface(interfaceId);
    }

    /*//////////////////////////////////////////////////////////////
                            INTERNAL FUNCTIONS
    //////////////////////////////////////////////////////////////*/

    /**
     * @notice Internal function to grant roles with admin transfer logic
     * @dev Overrides OpenZeppelin's _grantRole to handle admin transfers
     * @dev Prevents external admin role grants
     * @param role Role to grant
     * @param account Account to grant the role to
     * @return Whether the role was granted successfully
     */
    function _grantRole(bytes32 role, address account) internal override returns (bool) {
        if (role == DEFAULT_ADMIN_ROLE) {
            emit AdminTransferred(_currentDefaultAdmin, account);
            _revokeRole(DEFAULT_ADMIN_ROLE, _currentDefaultAdmin);
            _currentDefaultAdmin = account;
            delete _pendingDefaultAdmin;
        }
        return super._grantRole(role, account);
    }
}
