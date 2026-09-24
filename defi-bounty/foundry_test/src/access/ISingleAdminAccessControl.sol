// SPDX-License-Identifier: MIT
pragma solidity 0.8.30;

/**
 * @title ISingleAdminAccessControl
 * @notice Interface for single admin access control system
 * @dev This interface defines the events and errors for a simplified access control system
 *      that maintains a single admin role with transfer capabilities.
 * @dev The system prevents admin role from being granted externally and provides
 *      a two-step process for admin transfers.
 */
interface ISingleAdminAccessControl {
    /**
     * @notice Error thrown when trying to make an invalid admin change
     * @dev Used to prevent unauthorized admin modifications
     */
    error InvalidAdminChange();

    /**
     * @notice Error thrown when trying to accept admin role without being pending
     * @dev Ensures only the pending admin can accept the role
     */
    error NotPendingAdmin();

    /**
     * @notice Emitted when admin role is transferred to a new address
     * @param oldAdmin Address of the previous admin
     * @param newAdmin Address of the new admin
     */
    event AdminTransferred(address indexed oldAdmin, address indexed newAdmin);

    /**
     * @notice Emitted when admin transfer is requested
     * @param oldAdmin Address of the current admin
     * @param newAdmin Address of the pending admin
     */
    event AdminTransferRequested(address indexed oldAdmin, address indexed newAdmin);
}
