// SPDX-License-Identifier: GPL-3.0
pragma solidity 0.8.30;

import {ReentrancyGuard} from "@openzeppelin/contracts/utils/ReentrancyGuard.sol";
import {SafeERC20} from "@openzeppelin/contracts/token/ERC20/utils/SafeERC20.sol";
import {SafeCast} from "@openzeppelin/contracts/utils/math/SafeCast.sol";
import {IERC20} from "@openzeppelin/contracts/token/ERC20/IERC20.sol";
import {IERC20Metadata} from "@openzeppelin/contracts/token/ERC20/extensions/IERC20Metadata.sol";
import {IPSM} from "./IPSM.sol";
import {CollateralStateMap} from "./CollateralStateMap.sol";
import {IOracleFeed} from "../oracle/IOracleFeed.sol";
import {SingleAdminAccessControl} from "../access/SingleAdminAccessControl.sol";

/**
 * @title PSM
 * @notice Peg Stability Module enabling bidirectional atomic swaps between an asset token
 *         and one or more collateral tokens (assumed to be stablecoins).
 * @dev Derived from OnChainMinting.sol. All infrastructure (RBAC, epoch/period rate-limiting,
 *      oracle validation, slippage protection, delegated signers, reentrancy guard, fee system)
 *      is identical. The only change is replacing asset.mint/burnFrom with ERC-20 safeTransferFrom
 *      calls against dedicated custodian wallets. The PSM contract itself never holds funds
 *      in normal operation.
 *
 * Key Features:
 * - Epoch and period-based swap limits with dual-layer rate limiting
 * - Configurable fee structures per collateral and benefactor
 * - Oracle price validation with depeg protection
 * - Delegated signer support for benefactors
 * - Multi-role access control system
 * - Emergency pause functionality
 *
 * Inventory model:
 *   - assetSendCustodianAddress: holds the asset to send out; grants capped ERC-20 approval to this contract.
 *   - assetReceiveCustodianAddress: receives the asset flowing in; no approval required.
 *   - CollateralConfig.sendCustodianAddress: holds the collateral to send out; grants capped ERC-20 approval.
 *   - CollateralConfig.receiveCustodianAddress: receives the collateral flowing in; no approval required.
 *
 * swapForAsset  (collateral → asset):
 *   collateral.safeTransferFrom(benefactor, collateralConfig.receiveCustodianAddress, amountIn)
 *   asset.safeTransferFrom(assetSendCustodianAddress, beneficiary, amountOut)
 *
 * swapForCollateral (asset → collateral):
 *   asset.safeTransferFrom(benefactor, assetReceiveCustodianAddress, amountIn)
 *   collateral.safeTransferFrom(collateralConfig.sendCustodianAddress, beneficiary, amountOut)
 *
 * @custom:security This contract follows security best practices including:
 * - Reentrancy protection
 * - Access control validation
 * - Input validation and sanitization
 * - Oracle price staleness checks
 * - Epoch and period-based rate limiting
 */
contract PSM is IPSM, SingleAdminAccessControl, ReentrancyGuard {
    using SafeERC20 for IERC20;
    using SafeCast for uint256;
    using CollateralStateMap for CollateralStateMap.Map;

    /*//////////////////////////////////////////////////////////////
                               CONSTANTS
    //////////////////////////////////////////////////////////////*/

    /// @notice Role for managing epoch and period limits and global configuration
    bytes32 public constant EPOCH_PERIOD_MANAGER_ROLE = keccak256("EPOCH_PERIOD_MANAGER_ROLE");

    /// @notice Role for emergency global disabling of swapping
    bytes32 public constant GLOBAL_DISABLER_ROLE = keccak256("GLOBAL_DISABLER_ROLE");

    /// @notice Role for managing collateral assets and their configuration
    bytes32 public constant COLLATERAL_MANAGER_ROLE = keccak256("COLLATERAL_MANAGER_ROLE");

    /// @notice Role for disabling collateral assets in emergency situations
    bytes32 public constant COLLATERAL_DISABLER_ROLE = keccak256("COLLATERAL_DISABLER_ROLE");

    /// @notice Role for managing benefactors and their configuration
    bytes32 public constant BENEFACTOR_MANAGER_ROLE = keccak256("BENEFACTOR_MANAGER_ROLE");

    /// @notice Role for disabling benefactors in emergency situations
    bytes32 public constant BENEFACTOR_DISABLER_ROLE = keccak256("BENEFACTOR_DISABLER_ROLE");

    /// @notice Role for managing the peg enforcement setting
    bytes32 public constant PEG_MANAGER_ROLE = keccak256("PEG_MANAGER_ROLE");

    /// @notice Basis points for fee calculations (10000 = 100%)
    uint128 private constant BASIS_POINTS = 10_000;

    /// @notice Maximum fee allowed in basis points (100 = 1%)
    uint128 private constant MAX_FEE = 100;

    /// @notice 1 * 10^18
    uint128 private constant ONE_ETHER = 1e18;

    /// @notice Minimum allowed epoch duration (10 seconds)
    /// @dev Epochs are intended to track shorter time periods, such as 10 seconds, while periods track longer durations
    uint256 private constant MIN_EPOCH_DURATION = 10;

    /// @notice Maximum allowed epoch duration (24 hours)
    uint256 private constant MAX_EPOCH_DURATION = 24 hours;

    /// @notice Minimum allowed period duration (10 seconds)
    /// @dev Periods are intended to track longer time periods, such as 24 hours, complementing the shorter epoch-based limits
    uint256 private constant MIN_PERIOD_DURATION = 10;

    /// @notice Maximum allowed period duration (30 days)
    uint256 private constant MAX_PERIOD_DURATION = 30 days;

    /// @notice Minimum allowed oracle age (10 seconds)
    uint256 private constant MIN_ORACLE_AGE = 10;

    /// @notice Maximum allowed oracle age, one day and one minute
    uint256 private constant MAX_ORACLE_AGE = 1441 minutes;

    /// @notice Maximum allowed future timestamp tolerance (15 seconds)
    /// @dev Allows for minor clock drift and intrablock timing differences with Pyth feeds
    uint256 private constant MAX_FUTURE_TIMESTAMP_TOLERANCE = 15;

    /// @notice Maximum allowed peg price to prevent overflow in calculations (1000 * 10^18 = $1000)
    /// @dev This prevents overflow when pegPrice is multiplied with amountIn and decimal scaling factors
    /// @dev Calculated to ensure: amountIn * pegPrice * (10 ** decimals) < type(uint256).max
    uint128 private constant MAX_PEG_PRICE = 1000e18;

    /*//////////////////////////////////////////////////////////////
                               STORAGE
    //////////////////////////////////////////////////////////////*/

    /// @notice Asset token — plain ERC-20.
    IERC20 public immutable asset;

    /// @notice Decimals of the asset token.
    uint8 public immutable assetDecimals;

    /// @notice Wallet that sends asset out (swapForAsset); must grant ERC-20 approval to this contract.
    address public assetSendCustodianAddress;

    /// @notice Wallet that receives asset in (swapForCollateral); no approval required.
    address public assetReceiveCustodianAddress;

    /// @notice Whether swapping is currently enabled.
    bool public isSwapEnabled;

    /// @notice Global rate-limit and configuration state.
    GlobalState public globalState;

    /// @notice Per-collateral configuration and rate-limit state, with enumerable keys.
    CollateralStateMap.Map private collateralState;

    /// @notice Per-benefactor configuration and rate-limit state.
    mapping(address => BenefactorState) private benefactorState;

    /*//////////////////////////////////////////////////////////////
                               MODIFIERS
    //////////////////////////////////////////////////////////////*/

    /**
     * @notice Validates that an address is not the zero address
     * @param addr Address to validate
     * @dev Reverts with InvalidAddress if the address is zero
     */
    modifier onlyValidAddress(address addr) {
        if (addr == address(0)) revert InvalidAddress(addr);
        _;
    }

    /*//////////////////////////////////////////////////////////////
                             CONSTRUCTOR
    //////////////////////////////////////////////////////////////*/

    /**
     * @notice Initializes the PSM contract
     * @param _asset Address of the asset token (plain ERC-20)
     * @param _assetSendCustodian Address of the wallet that sends asset out (must grant ERC-20 approval)
     * @param _assetReceiveCustodian Address of the wallet that receives asset in
     * @param _globalConfig Global configuration parameters
     * @param _admin Address of the default admin
     * @param _epochManagers Array of addresses to grant epoch period manager role
     * @param _globalDisablers Array of addresses to grant global disabler role
     * @param _collateralManagers Array of addresses to grant collateral manager role
     * @param _collateralDisablers Array of addresses to grant collateral disabler role
     * @param _benefactorManagers Array of addresses to grant benefactor manager role
     * @param _benefactorDisablers Array of addresses to grant benefactor disabler role
     * @param _pegMaintainers Array of addresses to grant peg manager role
     * @dev Reverts if any address is zero or if epoch/period duration is invalid
     * @dev Grants appropriate roles to the provided addresses
     * @dev Sets initial state with swapping enabled
     */
    constructor(
        address _asset,
        address _assetSendCustodian,
        address _assetReceiveCustodian,
        GlobalConfig memory _globalConfig,
        address _admin,
        address[] memory _epochManagers,
        address[] memory _globalDisablers,
        address[] memory _collateralManagers,
        address[] memory _collateralDisablers,
        address[] memory _benefactorManagers,
        address[] memory _benefactorDisablers,
        address[] memory _pegMaintainers
    )
        onlyValidAddress(_asset)
        onlyValidAddress(_assetSendCustodian)
        onlyValidAddress(_assetReceiveCustodian)
        onlyValidAddress(_admin)
    {
        if (_globalConfig.epochDuration < MIN_EPOCH_DURATION) {
            revert EpochDurationTooShort(_globalConfig.epochDuration, MIN_EPOCH_DURATION);
        }
        if (_globalConfig.epochDuration > MAX_EPOCH_DURATION) {
            revert EpochDurationTooLong(_globalConfig.epochDuration, MAX_EPOCH_DURATION);
        }
        if (_globalConfig.defaultBenefactorMaxSwapForAssetPerEpoch == 0) {
            revert InvalidAmount(_globalConfig.defaultBenefactorMaxSwapForAssetPerEpoch);
        }
        if (_globalConfig.defaultBenefactorMaxSwapForCollateralPerEpoch == 0) {
            revert InvalidAmount(_globalConfig.defaultBenefactorMaxSwapForCollateralPerEpoch);
        }
        if (_globalConfig.pegPrice == 0) revert InvalidAmount(_globalConfig.pegPrice);
        if (_globalConfig.pegPrice > MAX_PEG_PRICE) revert InvalidPegPrice(_globalConfig.pegPrice, MAX_PEG_PRICE);
        if (_globalConfig.periodDuration < MIN_PERIOD_DURATION) {
            revert PeriodDurationTooShort(_globalConfig.periodDuration, MIN_PERIOD_DURATION);
        }
        if (_globalConfig.periodDuration > MAX_PERIOD_DURATION) {
            revert PeriodDurationTooLong(_globalConfig.periodDuration, MAX_PERIOD_DURATION);
        }
        if (_globalConfig.defaultBenefactorMaxSwapForAssetPerPeriod == 0) {
            revert InvalidAmount(_globalConfig.defaultBenefactorMaxSwapForAssetPerPeriod);
        }
        if (_globalConfig.defaultBenefactorMaxSwapForCollateralPerPeriod == 0) {
            revert InvalidAmount(_globalConfig.defaultBenefactorMaxSwapForCollateralPerPeriod);
        }

        asset = IERC20(_asset);
        // Assumes a well-behaved token (decimals <= 18); abnormally high decimals cause _getQuote overflow (DoS, no fund loss)
        assetDecimals = IERC20Metadata(_asset).decimals();
        assetSendCustodianAddress = _assetSendCustodian;
        assetReceiveCustodianAddress = _assetReceiveCustodian;
        isSwapEnabled = true;
        globalState.config = _globalConfig;

        _grantRole(DEFAULT_ADMIN_ROLE, _admin);
        _grantRoleToAddresses(EPOCH_PERIOD_MANAGER_ROLE, _epochManagers);
        _grantRoleToAddresses(GLOBAL_DISABLER_ROLE, _globalDisablers);
        _grantRoleToAddresses(COLLATERAL_MANAGER_ROLE, _collateralManagers);
        _grantRoleToAddresses(COLLATERAL_DISABLER_ROLE, _collateralDisablers);
        _grantRoleToAddresses(BENEFACTOR_MANAGER_ROLE, _benefactorManagers);
        _grantRoleToAddresses(BENEFACTOR_DISABLER_ROLE, _benefactorDisablers);
        _grantRoleToAddresses(PEG_MANAGER_ROLE, _pegMaintainers);
    }

    /*//////////////////////////////////////////////////////////////
                               EXTERNAL
    //////////////////////////////////////////////////////////////*/

    /**
     * @notice Executes a swap order
     * @dev This is the main function for processing swap orders in the system
     * @param order The order to execute containing all necessary parameters
     * @dev swapForAsset  (isSwapForAsset=true):  collateral → asset
     *      swapForCollateral (isSwapForAsset=false): asset → collateral
     * @dev Reverts if swapping is disabled
     * @dev Reverts if order validation fails (expiry, nonce, chain ID)
     * @dev Reverts if benefactor validation fails (not active, unauthorized caller)
     * @dev Reverts if collateral validation fails (not active, oracle depeg detected)
     * @dev Reverts if epoch or period limits are exceeded at any level
     * @dev Reverts if amountOut is below minAmountOut (slippage guard)
     * @dev Emits SwapExecuted event on success
     * @dev Uses reentrancy protection to prevent attacks
     */
    function swap(Order calldata order) external override nonReentrant {
        if (!isSwapEnabled) revert SwapDisabledError();

        // ========= ORDER VALIDATION =========

        _validateOrder(order);

        bool _isSwapForAsset = order.isSwapForAsset;

        // ========= COLLATERAL VALIDATION =========

        CollateralState storage _collateralState = collateralState.get(order.collateral);
        CollateralConfig storage _collateralConfig = _collateralState.config;

        if (!_collateralConfig.isActive) revert CollateralNotSupported(order.collateral);

        (uint256 oraclePrice, uint256 updatedAt) = IOracleFeed(_collateralConfig.oracleFeed).getPrice();
        _validateOraclePrice(_collateralConfig, order.collateral, _isSwapForAsset, oraclePrice, updatedAt);

        // ========= BENEFACTOR VALIDATION =========

        BenefactorState storage _benefactorState = benefactorState[order.benefactor];
        _validateBenefactor(order, _benefactorState);

        // ========= PRICING & FEES =========

        // Load benefactor config parameters for fee calculations (avoiding redundant SLOADs)
        BenefactorConfig storage _benefactorConfig = _benefactorState.config;
        bool zeroSwapForAssetFeeExempt = _benefactorConfig.zeroSwapForAssetFeeExemptions[order.collateral];
        bool zeroSwapForCollateralFeeExempt = _benefactorConfig.zeroSwapForCollateralFeeExemptions[order.collateral];
        uint128 customSwapForAssetFee = _benefactorConfig.swapForAssetFeeByCollateral[order.collateral];
        uint128 customSwapForCollateralFee = _benefactorConfig.swapForCollateralFeeByCollateral[order.collateral];

        // Get expected quote for this order using pre-loaded config
        (uint128 feeAmount, uint128 amountOut) = _getQuote(
            order.collateral,
            _collateralConfig,
            order.amountIn,
            _isSwapForAsset,
            zeroSwapForAssetFeeExempt,
            zeroSwapForCollateralFeeExempt,
            customSwapForAssetFee,
            customSwapForCollateralFee,
            oraclePrice
        );

        // Verify slippage protection: expected amount out should meet minimum expectation
        if (amountOut < order.minAmountOut) revert InsufficientAmountOut(order.minAmountOut, amountOut);

        // ========= EFFECTS: EPOCHS & LIMITS =========

        _handleEpochPeriodOperations(order, _benefactorState, _collateralState, globalState, amountOut);

        _benefactorState.orderNonceInvalidator[order.nonce] = true;

        // ========= INTERACTIONS: TRANSFERS =========

        if (_isSwapForAsset) {
            IERC20(order.collateral)
                .safeTransferFrom(order.benefactor, _collateralConfig.receiveCustodianAddress, order.amountIn);
            asset.safeTransferFrom(assetSendCustodianAddress, order.beneficiary, amountOut);
        } else {
            asset.safeTransferFrom(order.benefactor, assetReceiveCustodianAddress, order.amountIn);
            IERC20(order.collateral)
                .safeTransferFrom(_collateralConfig.sendCustodianAddress, order.beneficiary, amountOut);
        }

        emit SwapExecuted(msg.sender, order.benefactor, order.beneficiary, order, amountOut, feeAmount);
    }

    /**
     * @notice Update the asset send custodian wallet address
     * @dev Only callable by the default admin
     * @param newCustodian New custodian address; must have granted ERC-20 approval to this contract
     * @dev Reverts if newCustodian is the zero address
     * @dev Emits AssetSendCustodianUpdated event if the address changes
     */
    function setAssetSendCustodian(address newCustodian)
        external
        override
        nonReentrant
        onlyRole(DEFAULT_ADMIN_ROLE)
        onlyValidAddress(newCustodian)
    {
        if (benefactorState[newCustodian].config.isActive) {
            revert CustodianBenefactorConflict(newCustodian);
        }
        address old = assetSendCustodianAddress;
        if (old != newCustodian) {
            assetSendCustodianAddress = newCustodian;
            emit AssetSendCustodianUpdated(old, newCustodian);
        }
    }

    /**
     * @notice Update the asset receive custodian wallet address
     * @dev Only callable by the default admin
     * @param newCustodian New custodian address
     * @dev Reverts if newCustodian is the zero address
     * @dev Emits AssetReceiveCustodianUpdated event if the address changes
     */
    function setAssetReceiveCustodian(address newCustodian)
        external
        override
        nonReentrant
        onlyRole(DEFAULT_ADMIN_ROLE)
        onlyValidAddress(newCustodian)
    {
        if (benefactorState[newCustodian].config.isActive) {
            revert CustodianBenefactorConflict(newCustodian);
        }
        address old = assetReceiveCustodianAddress;
        if (old != newCustodian) {
            assetReceiveCustodianAddress = newCustodian;
            emit AssetReceiveCustodianUpdated(old, newCustodian);
        }
    }

    /**
     * @notice Sets the peg price for the asset; should only be called if the asset has
     *         permanently depegged to a new equilibrium that the protocol will uphold
     * @dev Only callable by peg managers
     * @param _pegPrice New peg price in USD (18 decimals, 1e18 = $1.00)
     * @dev Reverts if pegPrice is zero or exceeds MAX_PEG_PRICE
     * @dev Emits PegPriceUpdated event on success
     */
    function setPegPrice(uint128 _pegPrice) external override nonReentrant onlyRole(PEG_MANAGER_ROLE) {
        if (_pegPrice == 0) revert InvalidAmount(_pegPrice);
        if (_pegPrice > MAX_PEG_PRICE) revert InvalidPegPrice(_pegPrice, MAX_PEG_PRICE);
        if (globalState.config.pegPrice != _pegPrice) {
            uint128 oldPrice = globalState.config.pegPrice;
            globalState.config.pegPrice = _pegPrice;
            emit PegPriceUpdated(oldPrice, _pegPrice);
        }
    }

    /**
     * @notice Rescues funds accidentally sent to this contract
     * @dev Only callable by the default admin
     * @dev The PSM should hold no funds in normal operation; this is for recovery only
     * @param recipient Address to receive the rescued funds
     * @param token Address of the token to rescue
     * @param amount Amount to rescue
     * @dev Reverts if amount is zero
     * @dev Emits RescueFunds event on success
     */
    function rescueFunds(address recipient, address token, uint128 amount)
        external
        override
        nonReentrant
        onlyRole(DEFAULT_ADMIN_ROLE)
        onlyValidAddress(recipient)
        onlyValidAddress(token)
    {
        if (amount == 0) revert InvalidAmount(amount);
        IERC20(token).safeTransfer(recipient, amount);
        emit RescueFunds(recipient, token, amount);
    }

    /**
     * @notice Enables swap functionality
     * @dev Only callable by the default admin
     * @dev Reverts if already enabled
     * @dev Emits SwapEnabled event on success
     */
    function enableSwap() external override nonReentrant onlyRole(DEFAULT_ADMIN_ROLE) {
        if (isSwapEnabled) revert SwapAlreadyEnabled();
        isSwapEnabled = true;
        emit SwapEnabled();
    }

    /**
     * @notice Disables swap functionality
     * @dev Only callable by global disablers
     * @dev Reverts if already disabled
     * @dev Emits SwapDisabled event on success
     */
    function disableSwap() external override nonReentrant onlyRole(GLOBAL_DISABLER_ROLE) {
        if (!isSwapEnabled) revert SwapAlreadyDisabled();
        isSwapEnabled = false;
        emit SwapDisabled();
    }

    /**
     * @notice Enables a collateral
     * @dev Only callable by the default admin
     * @param collateral Address of the collateral
     * @dev Reverts if already enabled
     * @dev Emits CollateralEnabled event on success
     */
    function enableCollateral(address collateral)
        external
        override
        nonReentrant
        onlyRole(DEFAULT_ADMIN_ROLE)
        onlyValidAddress(collateral)
    {
        if (!collateralState.contains(collateral)) {
            revert CollateralNotSupported(collateral);
        }
        IPSM.CollateralState storage state = collateralState.get(collateral);
        if (state.config.isActive) revert CollateralAlreadyEnabled(collateral);
        state.config.isActive = true;
        emit CollateralEnabled(collateral);
    }

    /**
     * @notice Disables a collateral
     * @dev Only callable by collateral disablers
     * @param collateral Address of the collateral
     * @dev Reverts if not active
     * @dev Emits CollateralDisabled event on success
     */
    function disableCollateral(address collateral) external override nonReentrant onlyRole(COLLATERAL_DISABLER_ROLE) {
        if (!collateralState.get(collateral).config.isActive) revert CollateralNotActive();
        collateralState.get(collateral).config.isActive = false;
        emit CollateralDisabled(collateral);
    }

    /**
     * @notice Adds a new collateral to the system
     * @dev Only callable by collateral managers
     * @param collateral Address of the collateral token
     * @param config Configuration for the collateral including custodian, oracle, fees, and limits
     * @dev Reverts if collateral already exists
     * @dev Reverts if configuration is invalid
     * @dev Reverts if config.decimals does not match the token's actual decimals
     * @dev Emits CollateralAdded event on success
     */
    function addCollateral(address collateral, CollateralConfig calldata config)
        external
        override
        nonReentrant
        onlyValidAddress(collateral)
        onlyRole(COLLATERAL_MANAGER_ROLE)
    {
        if (collateralState.contains(collateral)) revert CollateralAlreadyExists(collateral);
        _validateCollateralConfig(config);

        uint8 tokenDecimals = IERC20Metadata(collateral).decimals();
        if (config.decimals != tokenDecimals) revert DecimalsMismatch(collateral, config.decimals, tokenDecimals);

        if (benefactorState[config.sendCustodianAddress].config.isActive) {
            revert CustodianBenefactorConflict(config.sendCustodianAddress);
        }
        if (benefactorState[config.receiveCustodianAddress].config.isActive) {
            revert CustodianBenefactorConflict(config.receiveCustodianAddress);
        }

        collateralState.add(collateral, config);
        emit CollateralAdded(collateral, config);
    }

    /**
     * @notice Removes a collateral from the system
     * @dev Only callable by collateral managers
     * @param collateral Address of the collateral to remove
     * @dev No-ops if collateral doesn't exist
     * @dev Emits CollateralRemoved event only if collateral was successfully removed
     */
    function removeCollateral(address collateral)
        external
        override
        nonReentrant
        onlyValidAddress(collateral)
        onlyRole(COLLATERAL_MANAGER_ROLE)
    {
        if (collateralState.contains(collateral)) {
            collateralState.remove(collateral);
            emit CollateralRemoved(collateral);
        }
    }

    /**
     * @notice Updates configuration for a collateral. Can be called even if the collateral
     *         is not active in preparation for reactivation.
     * @dev Only callable by collateral managers
     * @param collateral Address of the collateral
     * @param config New configuration; custodian rotation is supported here
     * @dev Reverts if collateral doesn't exist
     * @dev Reverts if configuration is invalid
     * @dev Reverts if config.decimals differs from the existing decimals (immutable after add)
     * @dev Emits CollateralConfigUpdated event on success
     */
    function updateCollateralConfig(address collateral, CollateralConfig calldata config)
        external
        override
        nonReentrant
        onlyValidAddress(collateral)
        onlyRole(COLLATERAL_MANAGER_ROLE)
    {
        if (!collateralState.contains(collateral)) revert CollateralNotSupported(collateral);
        CollateralState storage state = collateralState.get(collateral);
        CollateralConfig storage collateralCfg = state.config;
        _validateCollateralConfig(config);

        if (config.decimals != collateralCfg.decimals) {
            revert DecimalsCannotChange(collateral, collateralCfg.decimals, config.decimals);
        }

        if (benefactorState[config.sendCustodianAddress].config.isActive) {
            revert CustodianBenefactorConflict(config.sendCustodianAddress);
        }
        if (benefactorState[config.receiveCustodianAddress].config.isActive) {
            revert CustodianBenefactorConflict(config.receiveCustodianAddress);
        }

        bool wasActive = collateralCfg.isActive;
        CollateralConfig memory storedConfig = config;
        storedConfig.isActive = wasActive;
        state.config = storedConfig;
        emit CollateralConfigUpdated(collateral, storedConfig);
    }

    /**
     * @notice Enables a benefactor
     * @dev Only callable by the default admin
     * @param benefactor Address of the benefactor
     * @dev Reverts if already enabled
     * @dev Emits BenefactorEnabled event on success
     */
    function enableBenefactor(address benefactor)
        external
        override
        nonReentrant
        onlyRole(DEFAULT_ADMIN_ROLE)
        onlyValidAddress(benefactor)
    {
        if (benefactorState[benefactor].config.isActive) {
            revert BenefactorAlreadyEnabled(benefactor);
        }
        if (_isCustodian(benefactor)) revert CustodianBenefactorConflict(benefactor);
        benefactorState[benefactor].config.isActive = true;
        emit BenefactorEnabled(benefactor);
    }

    /**
     * @notice Disables a benefactor
     * @dev Only callable by benefactor disablers
     * @param benefactor Address of the benefactor
     * @dev Reverts if not active
     * @dev Emits BenefactorDisabled event on success
     */
    function disableBenefactor(address benefactor) external override nonReentrant onlyRole(BENEFACTOR_DISABLER_ROLE) {
        if (!benefactorState[benefactor].config.isActive) revert BenefactorNotActive(benefactor);
        benefactorState[benefactor].config.isActive = false;
        emit BenefactorDisabled(benefactor);
    }

    /**
     * @notice Adds a new benefactor to the system
     * @dev Only callable by benefactor managers
     * @param benefactor Address of the benefactor
     * @dev Reverts if benefactor already exists
     * @dev Emits BenefactorAdded event on success
     */
    function addBenefactor(address benefactor)
        external
        override
        nonReentrant
        onlyValidAddress(benefactor)
        onlyRole(BENEFACTOR_MANAGER_ROLE)
    {
        BenefactorConfig storage benefactorConfig = benefactorState[benefactor].config;
        if (benefactorConfig.isActive) revert BenefactorAlreadyExists(benefactor);
        if (_isCustodian(benefactor)) revert CustodianBenefactorConflict(benefactor);
        benefactorState[benefactor].config.isActive = true;
        emit BenefactorAdded(benefactor);
    }

    /**
     * @notice Removes a benefactor from the system
     * @dev Only callable by benefactor managers
     * @param benefactor Address of the benefactor to remove
     * @dev Reverts if benefactor is not active
     * @dev Emits BenefactorRemoved event on success
     */
    function removeBenefactor(address benefactor) external override nonReentrant onlyRole(BENEFACTOR_MANAGER_ROLE) {
        if (!benefactorState[benefactor].config.isActive) revert BenefactorNotActive(benefactor);
        delete benefactorState[benefactor].config;
        emit BenefactorRemoved(benefactor);
    }

    /**
     * @notice Sets the maximum asset swappable per epoch for a benefactor (swapForAsset direction)
     * @dev Only callable by benefactor managers
     * @param benefactor Address of the benefactor
     * @param limit New limit in asset-token units; 0 falls back to the global default
     * @dev Emits BenefactorMaxSwapForAssetPerEpochUpdated event if limit changes
     */
    function setBenefactorMaxSwapForAssetPerEpoch(address benefactor, uint128 limit)
        external
        override
        nonReentrant
        onlyRole(BENEFACTOR_MANAGER_ROLE)
        onlyValidAddress(benefactor)
    {
        BenefactorConfig storage config = benefactorState[benefactor].config;
        if (config.maxSwapForAssetPerEpoch != limit) {
            uint128 old = config.maxSwapForAssetPerEpoch;
            config.maxSwapForAssetPerEpoch = limit;
            emit BenefactorMaxSwapForAssetPerEpochUpdated(benefactor, old, limit);
        }
    }

    /**
     * @notice Sets the maximum asset swappable per epoch for a benefactor (swapForCollateral direction)
     * @dev Only callable by benefactor managers
     * @param benefactor Address of the benefactor
     * @param limit New limit in asset-token units; 0 falls back to the global default
     * @dev Emits BenefactorMaxSwapForCollateralPerEpochUpdated event if limit changes
     */
    function setBenefactorMaxSwapForCollateralPerEpoch(address benefactor, uint128 limit)
        external
        override
        nonReentrant
        onlyRole(BENEFACTOR_MANAGER_ROLE)
        onlyValidAddress(benefactor)
    {
        BenefactorConfig storage config = benefactorState[benefactor].config;
        if (config.maxSwapForCollateralPerEpoch != limit) {
            uint128 old = config.maxSwapForCollateralPerEpoch;
            config.maxSwapForCollateralPerEpoch = limit;
            emit BenefactorMaxSwapForCollateralPerEpochUpdated(benefactor, old, limit);
        }
    }

    /**
     * @notice Sets a custom swapForAsset fee for a benefactor and collateral
     * @dev Only callable by benefactor managers
     * @param benefactor Address of the benefactor
     * @param collateral Address of the collateral
     * @param fee Fee in basis points (0 to use the default from collateral config)
     * @dev Reverts if fee exceeds MAX_FEE
     * @dev Emits BenefactorSwapForAssetFeeUpdated event if fee changes
     */
    function setBenefactorSwapForAssetFee(address benefactor, address collateral, uint128 fee)
        external
        override
        nonReentrant
        onlyRole(BENEFACTOR_MANAGER_ROLE)
        onlyValidAddress(benefactor)
        onlyValidAddress(collateral)
    {
        if (fee > MAX_FEE) revert InvalidBenefactorFee(fee);
        BenefactorConfig storage config = benefactorState[benefactor].config;
        if (config.swapForAssetFeeByCollateral[collateral] != fee) {
            uint128 oldFee = config.swapForAssetFeeByCollateral[collateral];
            config.swapForAssetFeeByCollateral[collateral] = fee;
            emit BenefactorSwapForAssetFeeUpdated(benefactor, collateral, oldFee, fee);
        }
    }

    /**
     * @notice Sets a custom swapForCollateral fee for a benefactor and collateral
     * @dev Only callable by benefactor managers
     * @param benefactor Address of the benefactor
     * @param collateral Address of the collateral
     * @param fee Fee in basis points (0 to use the default from collateral config)
     * @dev Reverts if fee exceeds MAX_FEE
     * @dev Emits BenefactorSwapForCollateralFeeUpdated event if fee changes
     */
    function setBenefactorSwapForCollateralFee(address benefactor, address collateral, uint128 fee)
        external
        override
        nonReentrant
        onlyRole(BENEFACTOR_MANAGER_ROLE)
        onlyValidAddress(benefactor)
        onlyValidAddress(collateral)
    {
        if (fee > MAX_FEE) revert InvalidBenefactorFee(fee);
        BenefactorConfig storage config = benefactorState[benefactor].config;
        if (config.swapForCollateralFeeByCollateral[collateral] != fee) {
            uint128 oldFee = config.swapForCollateralFeeByCollateral[collateral];
            config.swapForCollateralFeeByCollateral[collateral] = fee;
            emit BenefactorSwapForCollateralFeeUpdated(benefactor, collateral, oldFee, fee);
        }
    }

    /**
     * @notice Sets zero swapForAsset fee exemption for a benefactor and collateral
     * @dev Only callable by benefactor managers
     * @param benefactor Address of the benefactor
     * @param collateral Address of the collateral
     * @param exempt Whether to grant zero-fee exemption for swapForAsset operations
     * @dev Emits BenefactorZeroSwapForAssetFeeExemptionUpdated event if exemption changes
     */
    function setBenefactorZeroSwapForAssetFeeExemption(address benefactor, address collateral, bool exempt)
        external
        override
        nonReentrant
        onlyRole(BENEFACTOR_MANAGER_ROLE)
        onlyValidAddress(benefactor)
        onlyValidAddress(collateral)
    {
        BenefactorConfig storage config = benefactorState[benefactor].config;
        if (config.zeroSwapForAssetFeeExemptions[collateral] != exempt) {
            config.zeroSwapForAssetFeeExemptions[collateral] = exempt;
            emit BenefactorZeroSwapForAssetFeeExemptionUpdated(benefactor, collateral, exempt);
        }
    }

    /**
     * @notice Sets zero swapForCollateral fee exemption for a benefactor and collateral
     * @dev Only callable by benefactor managers
     * @param benefactor Address of the benefactor
     * @param collateral Address of the collateral
     * @param exempt Whether to grant zero-fee exemption for swapForCollateral operations
     * @dev Emits BenefactorZeroSwapForCollateralFeeExemptionUpdated event if exemption changes
     */
    function setBenefactorZeroSwapForCollateralFeeExemption(address benefactor, address collateral, bool exempt)
        external
        override
        nonReentrant
        onlyRole(BENEFACTOR_MANAGER_ROLE)
        onlyValidAddress(benefactor)
        onlyValidAddress(collateral)
    {
        BenefactorConfig storage config = benefactorState[benefactor].config;
        if (config.zeroSwapForCollateralFeeExemptions[collateral] != exempt) {
            config.zeroSwapForCollateralFeeExemptions[collateral] = exempt;
            emit BenefactorZeroSwapForCollateralFeeExemptionUpdated(benefactor, collateral, exempt);
        }
    }

    /**
     * @notice Sets the maximum asset swappable per period for a benefactor (swapForAsset direction)
     * @dev Only callable by benefactor managers
     * @param benefactor Address of the benefactor
     * @param limit New limit in asset-token units; 0 falls back to the global default
     * @dev Emits BenefactorMaxSwapForAssetPerPeriodUpdated event if limit changes
     */
    function setBenefactorMaxSwapForAssetPerPeriod(address benefactor, uint128 limit)
        external
        override
        nonReentrant
        onlyRole(BENEFACTOR_MANAGER_ROLE)
        onlyValidAddress(benefactor)
    {
        BenefactorConfig storage config = benefactorState[benefactor].config;
        if (config.maxSwapForAssetPerPeriod != limit) {
            uint128 old = config.maxSwapForAssetPerPeriod;
            config.maxSwapForAssetPerPeriod = limit;
            emit BenefactorMaxSwapForAssetPerPeriodUpdated(benefactor, old, limit);
        }
    }

    /**
     * @notice Sets the maximum asset swappable per period for a benefactor (swapForCollateral direction)
     * @dev Only callable by benefactor managers
     * @param benefactor Address of the benefactor
     * @param limit New limit in asset-token units; 0 falls back to the global default
     * @dev Emits BenefactorMaxSwapForCollateralPerPeriodUpdated event if limit changes
     */
    function setBenefactorMaxSwapForCollateralPerPeriod(address benefactor, uint128 limit)
        external
        override
        nonReentrant
        onlyRole(BENEFACTOR_MANAGER_ROLE)
        onlyValidAddress(benefactor)
    {
        BenefactorConfig storage config = benefactorState[benefactor].config;
        if (config.maxSwapForCollateralPerPeriod != limit) {
            uint128 old = config.maxSwapForCollateralPerPeriod;
            config.maxSwapForCollateralPerPeriod = limit;
            emit BenefactorMaxSwapForCollateralPerPeriodUpdated(benefactor, old, limit);
        }
    }

    /**
     * @notice Sets a delegated signer for the caller. Can be called even if not active
     *         to allow inactive benefactors to manage their delegated signers.
     * @dev Callable by any address; signer status is set to PENDING until the signer calls
     *      confirmDelegatedSigner. Once confirmed (ACCEPTED), the signer gains full swap
     *      execution authority on behalf of the benefactor — it can submit any valid swap
     *      order, consume benefactor nonces, spend benefactor token allowances, and direct
     *      output to any benefactor-approved beneficiary. Treat an accepted delegated signer
     *      with the same trust level as the benefactor account itself.
     * @param signer Address of the delegated signer
     * @dev Emits DelegatedSignerAdded event on success
     */
    function setDelegatedSigner(address signer) external override nonReentrant onlyValidAddress(signer) {
        BenefactorConfig storage config = benefactorState[msg.sender].config;
        config.delegatedSigners[signer] = DelegatedSignerStatus.PENDING;
        emit DelegatedSignerAdded(signer, msg.sender);
    }

    /**
     * @notice Confirms a delegated signer request, granting full swap execution authority
     *         for the benefactor. By calling this function the signer acknowledges it will
     *         be able to submit swaps, consume nonces, and spend token allowances on behalf
     *         of the benefactor — subject only to the benefactor's configured limits and
     *         approved beneficiaries. Revoke via removeDelegatedSigner when no longer needed.
     * @dev Only callable by the pending signer; requires the benefactor to be active
     * @param benefactor Address of the benefactor who added this signer
     * @dev Signer status is set to ACCEPTED
     * @dev Reverts if benefactor is not active
     * @dev Reverts if caller's status is not PENDING
     * @dev Emits DelegatedSignerConfirmed event on success
     */
    function confirmDelegatedSigner(address benefactor) external override nonReentrant onlyValidAddress(benefactor) {
        BenefactorConfig storage config = benefactorState[benefactor].config;
        if (!config.isActive) revert BenefactorNotActive(benefactor);
        if (config.delegatedSigners[msg.sender] != DelegatedSignerStatus.PENDING) {
            revert DelegationNotAuthorized(msg.sender);
        }
        config.delegatedSigners[msg.sender] = DelegatedSignerStatus.ACCEPTED;
        emit DelegatedSignerConfirmed(msg.sender, benefactor);
    }

    /**
     * @notice Removes a delegated signer. Can be called even if not active to remove
     *         an unintended signer while in a state that prevents them from signing.
     * @dev Only callable by the benefactor (msg.sender)
     * @param signer Address of the signer to remove
     * @dev Signer status is set to REJECTED
     * @dev Reverts if signer is already REJECTED
     * @dev Emits DelegatedSignerRemoved event on success
     */
    function removeDelegatedSigner(address signer) external override nonReentrant {
        BenefactorConfig storage config = benefactorState[msg.sender].config;
        if (config.delegatedSigners[signer] == DelegatedSignerStatus.REJECTED) {
            revert DelegationNotAuthorized(signer);
        }
        config.delegatedSigners[signer] = DelegatedSignerStatus.REJECTED;
        emit DelegatedSignerRemoved(signer, msg.sender);
    }

    /**
     * @notice Sets approval status for a beneficiary
     * @dev Callable by any benefactor, even if not active
     * @param beneficiary Address of the beneficiary
     * @param approved Whether to approve the beneficiary
     * @dev Emits BeneficiaryApproved or BeneficiaryRemoved event based on approval status
     * @dev NOTE: Benefactors are always implicitly approved as their own beneficiary.
     *      Calling this function for `beneficiary == msg.sender` is allowed but unnecessary.
     *      This function is primarily for approving third-party beneficiaries.
     */
    function setApprovedBeneficiary(address beneficiary, bool approved)
        external
        override
        nonReentrant
        onlyValidAddress(beneficiary)
    {
        BenefactorConfig storage config = benefactorState[msg.sender].config;
        if (config.approvedBeneficiaries[beneficiary] != approved) {
            config.approvedBeneficiaries[beneficiary] = approved;
            if (approved) {
                emit BeneficiaryApproved(msg.sender, beneficiary);
            } else {
                emit BeneficiaryRemoved(msg.sender, beneficiary);
            }
        }
    }

    /**
     * @notice Sets global epoch limits for both swap directions
     * @dev Only callable by epoch period managers
     * @param maxSwapForAssetPerEpoch New global maximum asset swappable out per epoch
     * @param maxSwapForCollateralPerEpoch New global maximum asset swappable in per epoch
     * @dev Emits EpochLimitsUpdated event if limits change
     */
    function setGlobalEpochLimits(uint128 maxSwapForAssetPerEpoch, uint128 maxSwapForCollateralPerEpoch)
        external
        override
        nonReentrant
        onlyRole(EPOCH_PERIOD_MANAGER_ROLE)
    {
        GlobalConfig storage config = globalState.config;
        if (
            config.maxSwapForAssetPerEpoch != maxSwapForAssetPerEpoch
                || config.maxSwapForCollateralPerEpoch != maxSwapForCollateralPerEpoch
        ) {
            uint128 oldAsset = config.maxSwapForAssetPerEpoch;
            uint128 oldCollateral = config.maxSwapForCollateralPerEpoch;
            config.maxSwapForAssetPerEpoch = maxSwapForAssetPerEpoch;
            config.maxSwapForCollateralPerEpoch = maxSwapForCollateralPerEpoch;
            emit EpochLimitsUpdated(oldAsset, oldCollateral, maxSwapForAssetPerEpoch, maxSwapForCollateralPerEpoch);
        }
    }

    /**
     * @notice Sets the default maximum asset swappable per epoch for benefactors (swapForAsset direction)
     * @dev Only callable by epoch period managers
     * @param limit New default limit in asset-token units; must be non-zero
     * @dev Emits DefaultBenefactorMaxSwapForAssetPerEpochUpdated event if limit changes
     */
    function setDefaultBenefactorMaxSwapForAssetPerEpoch(uint128 limit)
        external
        override
        nonReentrant
        onlyRole(EPOCH_PERIOD_MANAGER_ROLE)
    {
        if (limit == 0) revert InvalidAmount(limit);
        if (globalState.config.defaultBenefactorMaxSwapForAssetPerEpoch != limit) {
            uint128 old = globalState.config.defaultBenefactorMaxSwapForAssetPerEpoch;
            globalState.config.defaultBenefactorMaxSwapForAssetPerEpoch = limit;
            emit DefaultBenefactorMaxSwapForAssetPerEpochUpdated(old, limit);
        }
    }

    /**
     * @notice Sets the default maximum asset swappable per epoch for benefactors (swapForCollateral direction)
     * @dev Only callable by epoch period managers
     * @param limit New default limit in asset-token units; must be non-zero
     * @dev Emits DefaultBenefactorMaxSwapForCollateralPerEpochUpdated event if limit changes
     */
    function setDefaultBenefactorMaxSwapForCollateralPerEpoch(uint128 limit)
        external
        override
        nonReentrant
        onlyRole(EPOCH_PERIOD_MANAGER_ROLE)
    {
        if (limit == 0) revert InvalidAmount(limit);
        if (globalState.config.defaultBenefactorMaxSwapForCollateralPerEpoch != limit) {
            uint128 old = globalState.config.defaultBenefactorMaxSwapForCollateralPerEpoch;
            globalState.config.defaultBenefactorMaxSwapForCollateralPerEpoch = limit;
            emit DefaultBenefactorMaxSwapForCollateralPerEpochUpdated(old, limit);
        }
    }

    /**
     * @notice Sets the epoch duration for the system
     * @dev Only callable by epoch period managers
     * @param newDuration New epoch duration in seconds
     * @dev Uses separate epoch state mappings per duration to prevent epoch collisions
     * @dev Automatically isolates epochs when duration changes; current usage effectively resets
     * @dev Emits EpochDurationUpdated event on success
     */
    function setEpochDuration(uint256 newDuration) external override nonReentrant onlyRole(EPOCH_PERIOD_MANAGER_ROLE) {
        if (newDuration < MIN_EPOCH_DURATION) revert EpochDurationTooShort(newDuration, MIN_EPOCH_DURATION);
        if (newDuration > MAX_EPOCH_DURATION) revert EpochDurationTooLong(newDuration, MAX_EPOCH_DURATION);
        GlobalConfig storage config = globalState.config;
        if (config.epochDuration != newDuration) {
            uint256 oldDuration = config.epochDuration;
            config.epochDuration = newDuration;
            emit EpochDurationUpdated(oldDuration, newDuration);
        }
    }

    /**
     * @notice Sets global period limits for both swap directions
     * @dev Only callable by epoch period managers
     * @param maxSwapForAssetPerPeriod New global maximum asset swappable out per period
     * @param maxSwapForCollateralPerPeriod New global maximum asset swappable in per period
     * @dev Emits PeriodLimitsUpdated event if limits change
     */
    function setGlobalPeriodLimits(uint128 maxSwapForAssetPerPeriod, uint128 maxSwapForCollateralPerPeriod)
        external
        override
        nonReentrant
        onlyRole(EPOCH_PERIOD_MANAGER_ROLE)
    {
        GlobalConfig storage config = globalState.config;
        if (
            config.maxSwapForAssetPerPeriod != maxSwapForAssetPerPeriod
                || config.maxSwapForCollateralPerPeriod != maxSwapForCollateralPerPeriod
        ) {
            uint128 oldAsset = config.maxSwapForAssetPerPeriod;
            uint128 oldCollateral = config.maxSwapForCollateralPerPeriod;
            config.maxSwapForAssetPerPeriod = maxSwapForAssetPerPeriod;
            config.maxSwapForCollateralPerPeriod = maxSwapForCollateralPerPeriod;
            emit PeriodLimitsUpdated(oldAsset, oldCollateral, maxSwapForAssetPerPeriod, maxSwapForCollateralPerPeriod);
        }
    }

    /**
     * @notice Sets the default maximum asset swappable per period for benefactors (swapForAsset direction)
     * @dev Only callable by epoch period managers
     * @param limit New default limit in asset-token units; must be non-zero
     * @dev Emits DefaultBenefactorMaxSwapForAssetPerPeriodUpdated event if limit changes
     */
    function setDefaultBenefactorMaxSwapForAssetPerPeriod(uint128 limit)
        external
        override
        nonReentrant
        onlyRole(EPOCH_PERIOD_MANAGER_ROLE)
    {
        if (limit == 0) revert InvalidAmount(limit);
        if (globalState.config.defaultBenefactorMaxSwapForAssetPerPeriod != limit) {
            uint128 old = globalState.config.defaultBenefactorMaxSwapForAssetPerPeriod;
            globalState.config.defaultBenefactorMaxSwapForAssetPerPeriod = limit;
            emit DefaultBenefactorMaxSwapForAssetPerPeriodUpdated(old, limit);
        }
    }

    /**
     * @notice Sets the default maximum asset swappable per period for benefactors (swapForCollateral direction)
     * @dev Only callable by epoch period managers
     * @param limit New default limit in asset-token units; must be non-zero
     * @dev Emits DefaultBenefactorMaxSwapForCollateralPerPeriodUpdated event if limit changes
     */
    function setDefaultBenefactorMaxSwapForCollateralPerPeriod(uint128 limit)
        external
        override
        nonReentrant
        onlyRole(EPOCH_PERIOD_MANAGER_ROLE)
    {
        if (limit == 0) revert InvalidAmount(limit);
        if (globalState.config.defaultBenefactorMaxSwapForCollateralPerPeriod != limit) {
            uint128 old = globalState.config.defaultBenefactorMaxSwapForCollateralPerPeriod;
            globalState.config.defaultBenefactorMaxSwapForCollateralPerPeriod = limit;
            emit DefaultBenefactorMaxSwapForCollateralPerPeriodUpdated(old, limit);
        }
    }

    /**
     * @notice Sets the period duration for the system
     * @dev Only callable by epoch period managers
     * @param newDuration New period duration in seconds
     * @dev Uses separate period state mappings per duration to prevent period collisions
     * @dev Automatically isolates periods when duration changes; current usage effectively resets
     * @dev Emits PeriodDurationUpdated event on success
     */
    function setPeriodDuration(uint256 newDuration) external override nonReentrant onlyRole(EPOCH_PERIOD_MANAGER_ROLE) {
        if (newDuration < MIN_PERIOD_DURATION) revert PeriodDurationTooShort(newDuration, MIN_PERIOD_DURATION);
        if (newDuration > MAX_PERIOD_DURATION) revert PeriodDurationTooLong(newDuration, MAX_PERIOD_DURATION);
        GlobalConfig storage config = globalState.config;
        if (config.periodDuration != newDuration) {
            uint256 oldDuration = config.periodDuration;
            config.periodDuration = newDuration;
            emit PeriodDurationUpdated(oldDuration, newDuration);
        }
    }

    /*//////////////////////////////////////////////////////////////
                               PUBLIC
    //////////////////////////////////////////////////////////////*/

    /**
     * @notice Gets a quote for the amount out and fee based on the amount in and current configuration
     * @param benefactor Address of the benefactor (used for per-benefactor fee overrides)
     * @param collateral Address of the collateral token
     * @param amountIn Amount of collateral (for swapForAsset) or asset (for swapForCollateral)
     * @param isSwapForAsset Whether the quote is for a swapForAsset (true) or swapForCollateral (false)
     * @return feeAmount The calculated fee amount (denominated in the amountIn token)
     * @return amountOut The calculated amount out after fees and pricing adjustments
     * @dev Validates oracle price to ensure the quote matches actual execution behavior
     * @dev Reverts if amountIn is below BASIS_POINTS or collateral is not active
     */
    function getQuote(address benefactor, address collateral, uint128 amountIn, bool isSwapForAsset)
        external
        nonReentrant
        returns (uint128 feeAmount, uint128 amountOut)
    {
        if (amountIn < BASIS_POINTS) {
            if (isSwapForAsset) revert InvalidCollateralAmount(amountIn);
            revert InvalidAssetAmount(amountIn);
        }

        CollateralConfig memory _collateralConfig = collateralState.get(collateral).config;
        if (!_collateralConfig.isActive) revert CollateralNotSupported(collateral);

        BenefactorConfig storage _benefactorConfig = benefactorState[benefactor].config;
        bool zeroSwapForAssetFeeExempt = _benefactorConfig.zeroSwapForAssetFeeExemptions[collateral];
        bool zeroSwapForCollateralFeeExempt = _benefactorConfig.zeroSwapForCollateralFeeExemptions[collateral];
        uint128 customSwapForAssetFee = _benefactorConfig.swapForAssetFeeByCollateral[collateral];
        uint128 customSwapForCollateralFee = _benefactorConfig.swapForCollateralFeeByCollateral[collateral];

        // Fetch and validate oracle price to ensure quote matches execution behavior
        (uint256 oraclePrice, uint256 updatedAt) = IOracleFeed(_collateralConfig.oracleFeed).getPrice();
        _validateOraclePrice(_collateralConfig, collateral, isSwapForAsset, oraclePrice, updatedAt);

        return _getQuote(
            collateral,
            _collateralConfig,
            amountIn,
            isSwapForAsset,
            zeroSwapForAssetFeeExempt,
            zeroSwapForCollateralFeeExempt,
            customSwapForAssetFee,
            customSwapForCollateralFee,
            oraclePrice
        );
    }

    /**
     * @notice Gets the basic configuration for a benefactor
     * @param benefactor Address of the benefactor
     * @return isActive Whether the benefactor is active
     * @return maxSwapForAssetPerEpoch Maximum asset swappable out per epoch for the benefactor
     * @return maxSwapForCollateralPerEpoch Maximum asset swappable in per epoch for the benefactor
     * @return maxSwapForAssetPerPeriod Maximum asset swappable out per period for the benefactor
     * @return maxSwapForCollateralPerPeriod Maximum asset swappable in per period for the benefactor
     */
    function getBenefactorConfig(address benefactor)
        external
        view
        returns (
            bool isActive,
            uint128 maxSwapForAssetPerEpoch,
            uint128 maxSwapForCollateralPerEpoch,
            uint128 maxSwapForAssetPerPeriod,
            uint128 maxSwapForCollateralPerPeriod
        )
    {
        BenefactorConfig storage config = benefactorState[benefactor].config;
        return (
            config.isActive,
            config.maxSwapForAssetPerEpoch,
            config.maxSwapForCollateralPerEpoch,
            config.maxSwapForAssetPerPeriod,
            config.maxSwapForCollateralPerPeriod
        );
    }

    /**
     * @notice Gets the effective fee rates for a benefactor and collateral
     * @param benefactor Address of the benefactor
     * @param collateral Address of the collateral
     * @return swapForAssetFee Effective fee in basis points for swapForAsset (0 if exempt)
     * @return swapForCollateralFee Effective fee in basis points for swapForCollateral (0 if exempt)
     * @dev Returns 0 for fees if the benefactor has a zero-fee exemption for that direction
     */
    function getBenefactorFeesForCollateral(address benefactor, address collateral)
        external
        view
        returns (uint128 swapForAssetFee, uint128 swapForCollateralFee)
    {
        BenefactorConfig storage benefactorCfg = benefactorState[benefactor].config;
        CollateralConfig storage collateralCfg = collateralState.get(collateral).config;

        // swapForAsset fee: 0 if exempt, custom fee if set, otherwise default collateral fee
        if (benefactorCfg.zeroSwapForAssetFeeExemptions[collateral]) {
            swapForAssetFee = 0;
        } else {
            swapForAssetFee = benefactorCfg.swapForAssetFeeByCollateral[collateral];
            if (swapForAssetFee == 0) swapForAssetFee = collateralCfg.defaultSwapForAssetFee;
        }

        // swapForCollateral fee: 0 if exempt, custom fee if set, otherwise default collateral fee
        if (benefactorCfg.zeroSwapForCollateralFeeExemptions[collateral]) {
            swapForCollateralFee = 0;
        } else {
            swapForCollateralFee = benefactorCfg.swapForCollateralFeeByCollateral[collateral];
            if (swapForCollateralFee == 0) swapForCollateralFee = collateralCfg.defaultSwapForCollateralFee;
        }
    }

    /**
     * @notice Gets the delegated signer status for a benefactor and signer
     * @param benefactor Address of the benefactor
     * @param signer Address of the signer
     * @return DelegatedSignerStatus Current status of the signer (REJECTED, PENDING, or ACCEPTED)
     */
    function getDelegatedSignerStatus(address benefactor, address signer)
        external
        view
        returns (DelegatedSignerStatus)
    {
        return benefactorState[benefactor].config.delegatedSigners[signer];
    }

    /**
     * @notice Checks if a beneficiary is effectively approved by a benefactor
     * @param benefactor Address of the benefactor
     * @param beneficiary Address of the beneficiary
     * @return Whether the beneficiary is approved (explicitly or implicitly)
     * @dev Returns true when benefactor == beneficiary (implicit self-approval) or when
     *      the beneficiary has been explicitly approved via setApprovedBeneficiary.
     *      This mirrors the approval logic applied during swap execution.
     */
    function isApprovedBeneficiary(address benefactor, address beneficiary) external view returns (bool) {
        if (benefactor == beneficiary) return true;
        return benefactorState[benefactor].config.approvedBeneficiaries[beneficiary];
    }

    /**
     * @notice Gets the current epoch totals for global swapForAsset and swapForCollateral volume
     * @return globalSwappedForAsset Total asset amount sent out (swapForAsset) in the current epoch
     * @return globalSwappedForCollateral Total asset amount received (swapForCollateral) in the current epoch
     * @dev Returns 0 if the current epoch is different from the stored epoch
     */
    function getGlobalEpochTotals()
        external
        view
        returns (uint128 globalSwappedForAsset, uint128 globalSwappedForCollateral)
    {
        uint256 currentEpoch = _getCurrentEpoch(globalState);
        uint256 epochDuration = globalState.config.epochDuration;
        EpochState storage epochState = globalState.epochStateByDuration[epochDuration];
        if (epochState.epoch == currentEpoch) {
            globalSwappedForAsset = epochState.swappedForAssetInEpoch;
            globalSwappedForCollateral = epochState.swappedForCollateralInEpoch;
        }
    }

    /**
     * @notice Gets the current epoch totals for a specific collateral
     * @param collateral Address of the collateral
     * @return swappedForAsset Total asset amount sent out using this collateral in the current epoch
     * @return swappedForCollateral Total asset amount received for this collateral in the current epoch
     * @dev Returns 0 if the current epoch is different from the stored epoch
     */
    function getCollateralEpochTotals(address collateral)
        external
        view
        returns (uint128 swappedForAsset, uint128 swappedForCollateral)
    {
        uint256 currentEpoch = _getCurrentEpoch(globalState);
        uint256 epochDuration = globalState.config.epochDuration;
        EpochState storage epochState = collateralState.get(collateral).epochStateByDuration[epochDuration];
        if (epochState.epoch == currentEpoch) {
            swappedForAsset = epochState.swappedForAssetInEpoch;
            swappedForCollateral = epochState.swappedForCollateralInEpoch;
        }
    }

    /**
     * @notice Gets the current epoch totals for a specific benefactor
     * @param benefactor Address of the benefactor
     * @return swappedForAsset Total asset amount sent out by this benefactor in the current epoch
     * @return swappedForCollateral Total asset amount received by this benefactor in the current epoch
     * @dev Returns 0 if the current epoch is different from the stored epoch
     */
    function getBenefactorEpochTotal(address benefactor)
        external
        view
        returns (uint128 swappedForAsset, uint128 swappedForCollateral)
    {
        uint256 currentEpoch = _getCurrentEpoch(globalState);
        uint256 epochDuration = globalState.config.epochDuration;
        EpochState storage epochState = benefactorState[benefactor].epochStateByDuration[epochDuration];
        if (epochState.epoch == currentEpoch) {
            swappedForAsset = epochState.swappedForAssetInEpoch;
            swappedForCollateral = epochState.swappedForCollateralInEpoch;
        }
    }

    /**
     * @notice Gets the current period totals for global swapForAsset and swapForCollateral volume
     * @return globalSwappedForAsset Total asset amount sent out (swapForAsset) in the current period
     * @return globalSwappedForCollateral Total asset amount received (swapForCollateral) in the current period
     * @dev Returns 0 if the current period is different from the stored period
     */
    function getGlobalPeriodTotals()
        external
        view
        returns (uint128 globalSwappedForAsset, uint128 globalSwappedForCollateral)
    {
        uint256 currentPeriod = _getCurrentPeriod(globalState);
        uint256 periodDuration = globalState.config.periodDuration;
        PeriodState storage periodState = globalState.periodStateByDuration[periodDuration];
        if (periodState.period == currentPeriod) {
            globalSwappedForAsset = periodState.swappedForAssetInPeriod;
            globalSwappedForCollateral = periodState.swappedForCollateralInPeriod;
        }
    }

    /**
     * @notice Gets the current period totals for a specific collateral
     * @param collateral Address of the collateral
     * @return swappedForAsset Total asset amount sent out using this collateral in the current period
     * @return swappedForCollateral Total asset amount received for this collateral in the current period
     * @dev Returns 0 if the current period is different from the stored period
     */
    function getCollateralPeriodTotals(address collateral)
        external
        view
        returns (uint128 swappedForAsset, uint128 swappedForCollateral)
    {
        uint256 currentPeriod = _getCurrentPeriod(globalState);
        uint256 periodDuration = globalState.config.periodDuration;
        PeriodState storage periodState = collateralState.get(collateral).periodStateByDuration[periodDuration];
        if (periodState.period == currentPeriod) {
            swappedForAsset = periodState.swappedForAssetInPeriod;
            swappedForCollateral = periodState.swappedForCollateralInPeriod;
        }
    }

    /**
     * @notice Gets the current period totals for a specific benefactor
     * @param benefactor Address of the benefactor
     * @return swappedForAsset Total asset amount sent out by this benefactor in the current period
     * @return swappedForCollateral Total asset amount received by this benefactor in the current period
     * @dev Returns 0 if the current period is different from the stored period
     */
    function getBenefactorPeriodTotal(address benefactor)
        external
        view
        returns (uint128 swappedForAsset, uint128 swappedForCollateral)
    {
        uint256 currentPeriod = _getCurrentPeriod(globalState);
        uint256 periodDuration = globalState.config.periodDuration;
        PeriodState storage periodState = benefactorState[benefactor].periodStateByDuration[periodDuration];
        if (periodState.period == currentPeriod) {
            swappedForAsset = periodState.swappedForAssetInPeriod;
            swappedForCollateral = periodState.swappedForCollateralInPeriod;
        }
    }

    /**
     * @notice Gets the global configuration
     * @return GlobalConfig struct containing global configuration parameters
     */
    function globalConfig() external view returns (GlobalConfig memory) {
        return globalState.config;
    }

    /**
     * @notice Gets the configuration for a specific collateral
     * @param collateral Address of the collateral
     * @return CollateralConfig struct containing collateral configuration
     */
    function collateralConfig(address collateral) external view returns (CollateralConfig memory) {
        return collateralState.get(collateral).config;
    }

    /**
     * @notice Gets the default maximum asset swappable per epoch for benefactors (swapForAsset direction)
     * @return Default maximum asset swappable out per epoch
     */
    function defaultBenefactorMaxSwapForAssetPerEpoch() external view returns (uint128) {
        return globalState.config.defaultBenefactorMaxSwapForAssetPerEpoch;
    }

    /**
     * @notice Gets the default maximum asset swappable per epoch for benefactors (swapForCollateral direction)
     * @return Default maximum asset swappable in per epoch
     */
    function defaultBenefactorMaxSwapForCollateralPerEpoch() external view returns (uint128) {
        return globalState.config.defaultBenefactorMaxSwapForCollateralPerEpoch;
    }

    /**
     * @notice Gets the default maximum asset swappable per period for benefactors (swapForAsset direction)
     * @return Default maximum asset swappable out per period
     */
    function defaultBenefactorMaxSwapForAssetPerPeriod() external view returns (uint128) {
        return globalState.config.defaultBenefactorMaxSwapForAssetPerPeriod;
    }

    /**
     * @notice Gets the default maximum asset swappable per period for benefactors (swapForCollateral direction)
     * @return Default maximum asset swappable in per period
     */
    function defaultBenefactorMaxSwapForCollateralPerPeriod() external view returns (uint128) {
        return globalState.config.defaultBenefactorMaxSwapForCollateralPerPeriod;
    }

    /**
     * @notice Gets the timestamp when the current epoch ends
     * @return Timestamp when the current epoch ends
     * @dev Calculated as (currentEpoch + 1) * epochDuration
     */
    function getEpochEndTimestamp() external view override returns (uint256) {
        uint256 currentEpoch = _getCurrentEpoch(globalState);
        return (currentEpoch + 1) * globalState.config.epochDuration;
    }

    /**
     * @notice Gets the timestamp when the current period ends
     * @return Timestamp when the current period ends
     * @dev Calculated as (currentPeriod + 1) * periodDuration
     */
    function getPeriodEndTimestamp() external view override returns (uint256) {
        uint256 currentPeriod = _getCurrentPeriod(globalState);
        return (currentPeriod + 1) * globalState.config.periodDuration;
    }

    /*//////////////////////////////////////////////////////////////
                               INTERNAL
    //////////////////////////////////////////////////////////////*/

    /**
     * @notice Returns true if addr is registered as an asset or collateral custodian.
     * @dev Checks the two asset custodians and iterates the registered collateral set.
     */
    function _isCustodian(address addr) internal view returns (bool) {
        if (addr == assetSendCustodianAddress || addr == assetReceiveCustodianAddress) return true;
        uint256 len = collateralState.length();
        for (uint256 i = 0; i < len;) {
            CollateralConfig storage cfg = collateralState.get(collateralState.at(i)).config;
            if (addr == cfg.sendCustodianAddress || addr == cfg.receiveCustodianAddress) return true;
            unchecked {
                ++i;
            }
        }
        return false;
    }

    /**
     * @notice Grants a role to multiple addresses
     * @param role Role to grant
     * @param addresses Array of addresses to grant the role to
     * @dev Reverts if any address is zero
     * @dev Internal function used in constructor
     */
    function _grantRoleToAddresses(bytes32 role, address[] memory addresses) internal {
        for (uint256 i = 0; i < addresses.length;) {
            if (addresses[i] == address(0)) revert InvalidAddress(addresses[i]);
            _grantRole(role, addresses[i]);
            unchecked {
                ++i;
            }
        }
    }

    /**
     * @notice Validates an order for basic requirements
     * @param order The order to validate
     * @dev Checks amounts, expiry, and chain ID
     * @dev Reverts if amountIn is below BASIS_POINTS, minAmountOut is zero, order is expired, or chain ID mismatches
     */
    function _validateOrder(Order calldata order) internal view {
        if (order.amountIn < BASIS_POINTS) {
            if (order.isSwapForAsset) revert InvalidCollateralAmount(order.amountIn);
            revert InvalidAssetAmount(order.amountIn);
        }
        if (order.minAmountOut == 0) revert InvalidAssetAmount(0);
        if (order.expiry < block.timestamp) revert OrderExpired(order.expiry, block.timestamp);
        if (order.chainId != block.chainid) revert InvalidChainId(order.chainId, block.chainid);
    }

    /**
     * @notice Validates benefactor permissions and beneficiary approval
     * @param order The order to validate
     * @param _benefactorState Storage reference to benefactor state
     * @dev Reverts if benefactor is not active
     * @dev Reverts if nonce has already been used
     * @dev Reverts if caller is not the benefactor or an accepted delegated signer
     * @dev IMPORTANT: Benefactors are always implicitly approved as their own beneficiary.
     *      If `order.benefactor == order.beneficiary` the beneficiary approval check is bypassed.
     *      For all other beneficiaries, explicit approval via `setApprovedBeneficiary` is required.
     */
    function _validateBenefactor(Order calldata order, BenefactorState storage _benefactorState) internal view {
        if (!_benefactorState.config.isActive) revert BenefactorNotActive(order.benefactor);
        if (_benefactorState.orderNonceInvalidator[order.nonce]) revert InvalidNonce(order.nonce);
        if (
            msg.sender != order.benefactor
                && _benefactorState.config.delegatedSigners[msg.sender] != DelegatedSignerStatus.ACCEPTED
        ) {
            revert DelegationNotAuthorized(msg.sender);
        }
        if (order.benefactor != order.beneficiary && !_benefactorState.config.approvedBeneficiaries[order.beneficiary])
        {
            revert BeneficiaryNotApproved(order.beneficiary);
        }
    }

    /**
     * @notice Validates oracle price for a collateral
     * @param _collateralConfig Configuration of the collateral
     * @param collateral Address of the collateral
     * @param isSwapForAsset Whether this is a swapForAsset operation
     * @param price Oracle price in 18-decimal USD
     * @param updatedAt Timestamp when the oracle price was last updated
     * @dev Reverts if price is zero
     * @dev Reverts if updatedAt is too far in the future (exceeds MAX_FUTURE_TIMESTAMP_TOLERANCE)
     * @dev Reverts if price data is stale (older than maxOracleAge)
     * @dev Reverts if swapForAsset and price is below minOraclePrice (depeg protection)
     * @dev Reverts if swapForCollateral and price is above maxOraclePrice (depeg protection)
     * @dev Emits OraclePriceValidated event on success
     */
    function _validateOraclePrice(
        CollateralConfig memory _collateralConfig,
        address collateral,
        bool isSwapForAsset,
        uint256 price,
        uint256 updatedAt
    ) internal {
        if (price == 0) revert InvalidOraclePrice(collateral, _collateralConfig.oracleFeed, price);

        if (updatedAt > block.timestamp + MAX_FUTURE_TIMESTAMP_TOLERANCE) {
            revert FuturePriceDetected(block.timestamp, updatedAt);
        }

        uint256 timeDiff = updatedAt > block.timestamp ? 0 : block.timestamp - updatedAt;
        if (timeDiff > _collateralConfig.maxOracleAge) revert OraclePriceTooOld(updatedAt);

        if (isSwapForAsset) {
            if (price < _collateralConfig.minOraclePrice) {
                revert OracleSwapForAssetDepegDetected(_collateralConfig.minOraclePrice, price);
            }
        } else {
            if (price > _collateralConfig.maxOraclePrice) {
                revert OracleSwapForCollateralDepegDetected(_collateralConfig.maxOraclePrice, price);
            }
        }

        emit OraclePriceValidated(collateral, price);
    }

    /**
     * @notice Calculates the output amount and fee for a swap using pre-loaded config
     * @param collateral Address of the collateral token
     * @param _collateralConfig Configuration of the collateral
     * @param amountIn Amount of tokens being provided (collateral for swapForAsset, asset for swapForCollateral)
     * @param isSwapForAsset Whether this is a swapForAsset (true) or swapForCollateral (false) operation
     * @param zeroSwapForAssetFeeExempt Whether the benefactor has zero swapForAsset fee exemption
     * @param zeroSwapForCollateralFeeExempt Whether the benefactor has zero swapForCollateral fee exemption
     * @param customSwapForAssetFee Custom swapForAsset fee in basis points (0 = use collateral default)
     * @param customSwapForCollateralFee Custom swapForCollateral fee in basis points (0 = use collateral default)
     * @param oraclePrice Current oracle price in 18-decimal USD
     * @return feeAmount The calculated fee amount (denominated in the amountIn token)
     * @return amountOut The calculated amount out after fees and pricing adjustments
     * @dev Pricing: computes two amounts and takes the minimum to protect the protocol in both directions:
     *      1. oneToOneAmountOut — based on pegPrice, using netAmountIn (after fee)
     *      2. oracleAmountOut  — based on live oracle price, using gross amountIn (no explicit fee)
     *      amountOut = min(oneToOneAmountOut, oracleAmountOut)
     *      When oracle ≈ peg, the fee-reduced peg path always wins (fee collected).
     *      When oracle diverges, the oracle path wins and the price difference absorbs the cost.
     */
    function _getQuote(
        address collateral,
        CollateralConfig memory _collateralConfig,
        uint128 amountIn,
        bool isSwapForAsset,
        bool zeroSwapForAssetFeeExempt,
        bool zeroSwapForCollateralFeeExempt,
        uint128 customSwapForAssetFee,
        uint128 customSwapForCollateralFee,
        uint256 oraclePrice
    ) internal view returns (uint128 feeAmount, uint128 amountOut) {
        // Calculate fee on the amount in
        bool feeExempt = isSwapForAsset ? zeroSwapForAssetFeeExempt : zeroSwapForCollateralFeeExempt;
        if (!feeExempt) {
            uint128 feeRate = isSwapForAsset ? customSwapForAssetFee : customSwapForCollateralFee;
            if (feeRate == 0) {
                feeRate = isSwapForAsset
                    ? _collateralConfig.defaultSwapForAssetFee
                    : _collateralConfig.defaultSwapForCollateralFee;
            }
            feeAmount = (amountIn * feeRate) / BASIS_POINTS;
        }

        // Calculate amount out based on operation type
        uint128 netAmountIn = amountIn - feeAmount;
        uint256 pegPrice = globalState.config.pegPrice;

        // 1. Calculate 1:1 amount out (no oracle — pure peg-based USD exchange, after fee)
        uint128 oneToOneAmountOut;
        if (isSwapForAsset) {
            // netAmountIn collateral → assets at pegPrice
            oneToOneAmountOut = (uint256(netAmountIn) * ONE_ETHER * (10 ** assetDecimals)
                    / (pegPrice * (10 ** _collateralConfig.decimals)))
            .toUint128();
        } else {
            // netAmountIn assets → collateral at pegPrice
            oneToOneAmountOut = (uint256(netAmountIn) * pegPrice * (10 ** _collateralConfig.decimals)
                    / (ONE_ETHER * (10 ** assetDecimals)))
            .toUint128();
        }

        // 2. Calculate oracle-based amount out using gross amountIn (no explicit fee deducted)
        if (oraclePrice == 0) revert InvalidOraclePrice(collateral, _collateralConfig.oracleFeed, oraclePrice);
        uint128 oracleAmountOut;
        if (isSwapForAsset) {
            // gross amountIn collateral * oraclePrice / pegPrice = assets
            oracleAmountOut = (uint256(amountIn) * oraclePrice * (10 ** assetDecimals)
                    / (pegPrice * (10 ** _collateralConfig.decimals)))
            .toUint128();
        } else {
            // gross amountIn assets * pegPrice / oraclePrice = collateral
            oracleAmountOut = (uint256(amountIn) * pegPrice * (10 ** _collateralConfig.decimals)
                    / (oraclePrice * (10 ** assetDecimals)))
            .toUint128();
        }

        // 3. Take the lesser of the two (protects the protocol in both directions).
        //    When collateral depegs below peg on swapForAsset, oracle path gives fewer assets.
        //    When collateral goes above peg on swapForCollateral, oracle path gives less collateral.
        amountOut = oneToOneAmountOut < oracleAmountOut ? oneToOneAmountOut : oracleAmountOut;
    }

    /**
     * @notice Returns the current epoch number based on block.timestamp and epoch duration
     * @param _globalState Storage reference to global state
     * @return Current epoch number (block.timestamp / epochDuration)
     */
    function _getCurrentEpoch(GlobalState storage _globalState) internal view returns (uint256) {
        return block.timestamp / _globalState.config.epochDuration;
    }

    /**
     * @notice Returns the current period number based on block.timestamp and period duration
     * @param _globalState Storage reference to global state
     * @return Current period number (block.timestamp / periodDuration)
     */
    function _getCurrentPeriod(GlobalState storage _globalState) internal view returns (uint256) {
        return block.timestamp / _globalState.config.periodDuration;
    }

    /**
     * @notice Handles all epoch and period state updates and limit enforcement for a swap
     * @param order The swap order being executed
     * @param _benefactorState Storage reference to benefactor state
     * @param _collateralState Storage reference to collateral state
     * @param _globalState Storage reference to global state
     * @param amountOut The asset-denominated output amount (used for swapForAsset limit tracking)
     * @dev For swapForAsset: limits are tracked against amountOut (asset tokens going out)
     * @dev For swapForCollateral: limits are tracked against order.amountIn (asset tokens coming in)
     * @dev Rolls epoch/period state forward automatically if a new epoch/period has started
     */
    function _handleEpochPeriodOperations(
        Order calldata order,
        BenefactorState storage _benefactorState,
        CollateralState storage _collateralState,
        GlobalState storage _globalState,
        uint128 amountOut
    ) internal {
        uint256 _currentEpoch = _getCurrentEpoch(_globalState);
        uint256 _epochDuration = _globalState.config.epochDuration;

        EpochState storage _globalEpochState = _globalState.epochStateByDuration[_epochDuration];
        EpochState storage _collateralEpochState = _collateralState.epochStateByDuration[_epochDuration];
        EpochState storage _benefactorEpochState = _benefactorState.epochStateByDuration[_epochDuration];

        _maybeRollEpoch(_globalEpochState, _currentEpoch);
        _maybeRollEpoch(_collateralEpochState, _currentEpoch);
        _maybeRollEpoch(_benefactorEpochState, _currentEpoch);

        uint256 _currentPeriod = _getCurrentPeriod(_globalState);
        uint256 _periodDuration = _globalState.config.periodDuration;

        PeriodState storage _globalPeriodState = _globalState.periodStateByDuration[_periodDuration];
        PeriodState storage _collateralPeriodState = _collateralState.periodStateByDuration[_periodDuration];
        PeriodState storage _benefactorPeriodState = _benefactorState.periodStateByDuration[_periodDuration];

        _maybeRollPeriod(_globalPeriodState, _currentPeriod);
        _maybeRollPeriod(_collateralPeriodState, _currentPeriod);
        _maybeRollPeriod(_benefactorPeriodState, _currentPeriod);

        if (order.isSwapForAsset) {
            _validateGlobalEpochLimits(
                _globalEpochState.swappedForAssetInEpoch,
                _globalState.config.maxSwapForAssetPerEpoch,
                amountOut,
                _currentEpoch,
                _epochDuration,
                true
            );
            _validateCollateralEpochLimits(
                order.collateral,
                _collateralEpochState.swappedForAssetInEpoch,
                _collateralState.config.maxSwapForAssetPerEpoch,
                amountOut,
                _currentEpoch,
                _epochDuration,
                true
            );
            uint128 benefactorMaxSwapForAssetPerEpoch = _benefactorState.config.maxSwapForAssetPerEpoch;
            if (benefactorMaxSwapForAssetPerEpoch == 0) {
                benefactorMaxSwapForAssetPerEpoch = _globalState.config.defaultBenefactorMaxSwapForAssetPerEpoch;
            }
            _validateBenefactorEpochLimits(
                order.benefactor,
                _benefactorEpochState.swappedForAssetInEpoch,
                benefactorMaxSwapForAssetPerEpoch,
                amountOut,
                _currentEpoch,
                _epochDuration,
                true
            );

            _validateGlobalPeriodLimits(
                _globalPeriodState.swappedForAssetInPeriod,
                _globalState.config.maxSwapForAssetPerPeriod,
                amountOut,
                _currentPeriod,
                _periodDuration,
                true
            );
            _validateCollateralPeriodLimits(
                order.collateral,
                _collateralPeriodState.swappedForAssetInPeriod,
                _collateralState.config.maxSwapForAssetPerPeriod,
                amountOut,
                _currentPeriod,
                _periodDuration,
                true
            );
            uint128 benefactorMaxSwapForAssetPerPeriod = _benefactorState.config.maxSwapForAssetPerPeriod;
            if (benefactorMaxSwapForAssetPerPeriod == 0) {
                benefactorMaxSwapForAssetPerPeriod = _globalState.config.defaultBenefactorMaxSwapForAssetPerPeriod;
            }
            _validateBenefactorPeriodLimits(
                order.benefactor,
                _benefactorPeriodState.swappedForAssetInPeriod,
                benefactorMaxSwapForAssetPerPeriod,
                amountOut,
                _currentPeriod,
                _periodDuration,
                true
            );

            unchecked {
                _globalEpochState.swappedForAssetInEpoch += amountOut;
                _collateralEpochState.swappedForAssetInEpoch += amountOut;
                _benefactorEpochState.swappedForAssetInEpoch += amountOut;
                _globalPeriodState.swappedForAssetInPeriod += amountOut;
                _collateralPeriodState.swappedForAssetInPeriod += amountOut;
                _benefactorPeriodState.swappedForAssetInPeriod += amountOut;
            }
        } else {
            _validateGlobalEpochLimits(
                _globalEpochState.swappedForCollateralInEpoch,
                _globalState.config.maxSwapForCollateralPerEpoch,
                order.amountIn,
                _currentEpoch,
                _epochDuration,
                false
            );
            _validateCollateralEpochLimits(
                order.collateral,
                _collateralEpochState.swappedForCollateralInEpoch,
                _collateralState.config.maxSwapForCollateralPerEpoch,
                order.amountIn,
                _currentEpoch,
                _epochDuration,
                false
            );
            uint128 benefactorMaxSwapForCollateralPerEpoch = _benefactorState.config.maxSwapForCollateralPerEpoch;
            if (benefactorMaxSwapForCollateralPerEpoch == 0) {
                benefactorMaxSwapForCollateralPerEpoch =
                _globalState.config.defaultBenefactorMaxSwapForCollateralPerEpoch;
            }
            _validateBenefactorEpochLimits(
                order.benefactor,
                _benefactorEpochState.swappedForCollateralInEpoch,
                benefactorMaxSwapForCollateralPerEpoch,
                order.amountIn,
                _currentEpoch,
                _epochDuration,
                false
            );

            _validateGlobalPeriodLimits(
                _globalPeriodState.swappedForCollateralInPeriod,
                _globalState.config.maxSwapForCollateralPerPeriod,
                order.amountIn,
                _currentPeriod,
                _periodDuration,
                false
            );
            _validateCollateralPeriodLimits(
                order.collateral,
                _collateralPeriodState.swappedForCollateralInPeriod,
                _collateralState.config.maxSwapForCollateralPerPeriod,
                order.amountIn,
                _currentPeriod,
                _periodDuration,
                false
            );
            uint128 benefactorMaxSwapForCollateralPerPeriod = _benefactorState.config.maxSwapForCollateralPerPeriod;
            if (benefactorMaxSwapForCollateralPerPeriod == 0) {
                benefactorMaxSwapForCollateralPerPeriod =
                _globalState.config.defaultBenefactorMaxSwapForCollateralPerPeriod;
            }
            _validateBenefactorPeriodLimits(
                order.benefactor,
                _benefactorPeriodState.swappedForCollateralInPeriod,
                benefactorMaxSwapForCollateralPerPeriod,
                order.amountIn,
                _currentPeriod,
                _periodDuration,
                false
            );

            unchecked {
                _globalEpochState.swappedForCollateralInEpoch += order.amountIn;
                _collateralEpochState.swappedForCollateralInEpoch += order.amountIn;
                _benefactorEpochState.swappedForCollateralInEpoch += order.amountIn;
                _globalPeriodState.swappedForCollateralInPeriod += order.amountIn;
                _collateralPeriodState.swappedForCollateralInPeriod += order.amountIn;
                _benefactorPeriodState.swappedForCollateralInPeriod += order.amountIn;
            }
        }
    }

    /**
     * @notice Rolls epoch state forward if a new epoch has started
     * @param epochState Storage reference to the epoch state to potentially roll
     * @param currentEpoch The current epoch number
     * @dev Resets swappedForAssetInEpoch and swappedForCollateralInEpoch to zero on rollover
     */
    function _maybeRollEpoch(EpochState storage epochState, uint256 currentEpoch) internal {
        if (epochState.epoch != currentEpoch) {
            epochState.epoch = currentEpoch;
            epochState.swappedForAssetInEpoch = 0;
            epochState.swappedForCollateralInEpoch = 0;
        }
    }

    /**
     * @notice Rolls period state forward if a new period has started
     * @param periodState Storage reference to the period state to potentially roll
     * @param currentPeriod The current period number
     * @dev Resets swappedForAssetInPeriod and swappedForCollateralInPeriod to zero on rollover
     */
    function _maybeRollPeriod(PeriodState storage periodState, uint256 currentPeriod) internal {
        if (periodState.period != currentPeriod) {
            periodState.period = currentPeriod;
            periodState.swappedForAssetInPeriod = 0;
            periodState.swappedForCollateralInPeriod = 0;
        }
    }

    /**
     * @notice Validates global epoch limits for a swap direction
     * @param currentTotal Current usage in the epoch
     * @param max Maximum allowed in the epoch
     * @param amount Amount being requested
     * @param currentEpoch Current epoch number
     * @param epochDuration Epoch duration in seconds
     * @param isSwapForAsset Whether this is a swapForAsset operation
     * @dev Reverts if adding amount would exceed max
     */
    function _validateGlobalEpochLimits(
        uint128 currentTotal,
        uint128 max,
        uint128 amount,
        uint256 currentEpoch,
        uint256 epochDuration,
        bool isSwapForAsset
    ) internal pure {
        if (currentTotal + amount > max) {
            if (isSwapForAsset) {
                revert GlobalMaxSwapForAssetPerEpochExceeded(amount, currentTotal, max, currentEpoch, epochDuration);
            } else {
                revert GlobalMaxSwapForCollateralPerEpochExceeded(
                    amount, currentTotal, max, currentEpoch, epochDuration
                );
            }
        }
    }

    /**
     * @notice Validates collateral-specific epoch limits for a swap direction
     * @param collateral Address of the collateral asset
     * @param currentTotal Current usage in the epoch for this collateral
     * @param max Maximum allowed in the epoch for this collateral
     * @param amount Amount being requested
     * @param currentEpoch Current epoch number
     * @param epochDuration Epoch duration in seconds
     * @param isSwapForAsset Whether this is a swapForAsset operation
     * @dev Reverts if adding amount would exceed max
     */
    function _validateCollateralEpochLimits(
        address collateral,
        uint128 currentTotal,
        uint128 max,
        uint128 amount,
        uint256 currentEpoch,
        uint256 epochDuration,
        bool isSwapForAsset
    ) internal pure {
        if (currentTotal + amount > max) {
            if (isSwapForAsset) {
                revert CollateralMaxSwapForAssetPerEpochExceeded(
                    collateral, amount, currentTotal, max, currentEpoch, epochDuration
                );
            } else {
                revert CollateralMaxSwapForCollateralPerEpochExceeded(
                    collateral, amount, currentTotal, max, currentEpoch, epochDuration
                );
            }
        }
    }

    /**
     * @notice Validates benefactor-specific epoch limits for a swap direction
     * @param benefactor Address of the benefactor
     * @param currentTotal Current usage in the epoch for this benefactor
     * @param max Maximum allowed in the epoch for this benefactor
     * @param amount Amount being requested
     * @param currentEpoch Current epoch number
     * @param epochDuration Epoch duration in seconds
     * @param isSwapForAsset Whether this is a swapForAsset operation
     * @dev Reverts if adding amount would exceed max
     */
    function _validateBenefactorEpochLimits(
        address benefactor,
        uint128 currentTotal,
        uint128 max,
        uint128 amount,
        uint256 currentEpoch,
        uint256 epochDuration,
        bool isSwapForAsset
    ) internal pure {
        if (currentTotal + amount > max) {
            if (isSwapForAsset) {
                revert BenefactorMaxSwapForAssetPerEpochExceeded(
                    benefactor, amount, currentTotal, max, currentEpoch, epochDuration
                );
            } else {
                revert BenefactorMaxSwapForCollateralPerEpochExceeded(
                    benefactor, amount, currentTotal, max, currentEpoch, epochDuration
                );
            }
        }
    }

    /**
     * @notice Validates global period limits for a swap direction
     * @param currentTotal Current usage in the period
     * @param max Maximum allowed in the period
     * @param amount Amount being requested
     * @param currentPeriod Current period number
     * @param periodDuration Period duration in seconds
     * @param isSwapForAsset Whether this is a swapForAsset operation
     * @dev Reverts if adding amount would exceed max
     */
    function _validateGlobalPeriodLimits(
        uint128 currentTotal,
        uint128 max,
        uint128 amount,
        uint256 currentPeriod,
        uint256 periodDuration,
        bool isSwapForAsset
    ) internal pure {
        if (currentTotal + amount > max) {
            if (isSwapForAsset) {
                revert GlobalMaxSwapForAssetPerPeriodExceeded(amount, currentTotal, max, currentPeriod, periodDuration);
            } else {
                revert GlobalMaxSwapForCollateralPerPeriodExceeded(
                    amount, currentTotal, max, currentPeriod, periodDuration
                );
            }
        }
    }

    /**
     * @notice Validates collateral-specific period limits for a swap direction
     * @param collateral Address of the collateral asset
     * @param currentTotal Current usage in the period for this collateral
     * @param max Maximum allowed in the period for this collateral
     * @param amount Amount being requested
     * @param currentPeriod Current period number
     * @param periodDuration Period duration in seconds
     * @param isSwapForAsset Whether this is a swapForAsset operation
     * @dev Reverts if adding amount would exceed max
     */
    function _validateCollateralPeriodLimits(
        address collateral,
        uint128 currentTotal,
        uint128 max,
        uint128 amount,
        uint256 currentPeriod,
        uint256 periodDuration,
        bool isSwapForAsset
    ) internal pure {
        if (currentTotal + amount > max) {
            if (isSwapForAsset) {
                revert CollateralMaxSwapForAssetPerPeriodExceeded(
                    collateral, amount, currentTotal, max, currentPeriod, periodDuration
                );
            } else {
                revert CollateralMaxSwapForCollateralPerPeriodExceeded(
                    collateral, amount, currentTotal, max, currentPeriod, periodDuration
                );
            }
        }
    }

    /**
     * @notice Validates benefactor-specific period limits for a swap direction
     * @param benefactor Address of the benefactor
     * @param currentTotal Current usage in the period for this benefactor
     * @param max Maximum allowed in the period for this benefactor
     * @param amount Amount being requested
     * @param currentPeriod Current period number
     * @param periodDuration Period duration in seconds
     * @param isSwapForAsset Whether this is a swapForAsset operation
     * @dev Reverts if adding amount would exceed max
     */
    function _validateBenefactorPeriodLimits(
        address benefactor,
        uint128 currentTotal,
        uint128 max,
        uint128 amount,
        uint256 currentPeriod,
        uint256 periodDuration,
        bool isSwapForAsset
    ) internal pure {
        if (currentTotal + amount > max) {
            if (isSwapForAsset) {
                revert BenefactorMaxSwapForAssetPerPeriodExceeded(
                    benefactor, amount, currentTotal, max, currentPeriod, periodDuration
                );
            } else {
                revert BenefactorMaxSwapForCollateralPerPeriodExceeded(
                    benefactor, amount, currentTotal, max, currentPeriod, periodDuration
                );
            }
        }
    }

    /**
     * @notice Validates collateral configuration parameters
     * @param config Configuration to validate
     * @dev Reverts if sendCustodianAddress, receiveCustodianAddress, or oracleFeed is zero
     * @dev Reverts if fees exceed MAX_FEE
     * @dev Reverts if maxOracleAge is outside [MIN_ORACLE_AGE, MAX_ORACLE_AGE]
     * @dev Reverts if maxOraclePrice is zero or minOraclePrice is zero or exceeds maxOraclePrice
     * @dev Reverts if decimals is zero
     */
    function _validateCollateralConfig(CollateralConfig calldata config) internal pure {
        if (config.sendCustodianAddress == address(0)) revert InvalidAddress(config.sendCustodianAddress);
        if (config.receiveCustodianAddress == address(0)) revert InvalidAddress(config.receiveCustodianAddress);
        if (config.oracleFeed == address(0)) revert InvalidAddress(config.oracleFeed);
        if (config.defaultSwapForAssetFee > MAX_FEE) revert InvalidSwapForAssetFee(config.defaultSwapForAssetFee);
        if (config.defaultSwapForCollateralFee > MAX_FEE) {
            revert InvalidSwapForCollateralFee(config.defaultSwapForCollateralFee);
        }
        if (config.maxOracleAge < MIN_ORACLE_AGE || config.maxOracleAge > MAX_ORACLE_AGE) {
            revert InvalidOracleAge(config.maxOracleAge);
        }
        if (config.maxOraclePrice == 0) revert InvalidOraclePriceThreshold(config.maxOraclePrice);
        if (config.minOraclePrice == 0 || config.minOraclePrice > config.maxOraclePrice) {
            revert InvalidOraclePriceThreshold(config.minOraclePrice);
        }
        if (config.decimals == 0) revert InvalidDecimals(config.decimals);
    }
}
