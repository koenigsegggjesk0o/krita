// SPDX-License-Identifier: GPL-3.0
pragma solidity 0.8.30;

/**
 * @title IPSM
 * @notice Interface for the PSM (Peg Stability Module) swap contract.
 * @dev Enables bidirectional atomic swaps between an asset token and one or more
 *      collateral tokens (assumed to be stablecoins). Derived from IOnChainMinting —
 *      replaces mint/burn semantics with ERC-20 safeTransferFrom pulls against
 *      custodian wallets. The PSM contract itself never holds funds in normal operation.
 */
interface IPSM {
    /* --------------- ENUMS --------------- */

    enum DelegatedSignerStatus {
        REJECTED,
        PENDING,
        ACCEPTED
    }

    /* --------------- STRUCTS --------------- */

    /**
     * @notice Represents a swap order.
     * @param isSwapForAsset True = swapForAsset (collateral → asset). False = swapForCollateral (asset → collateral).
     * @param expiry Timestamp when the order expires.
     * @param nonce Unique identifier to prevent replay attacks.
     * @param chainId Chain ID where the order should be executed.
     * @param benefactor Address that provides the input tokens.
     * @param beneficiary Address that receives the output tokens.
     * @param collateral Address of the collateral token (a stablecoin).
     * @param amountIn Amount of input tokens (collateral for swapForAsset; asset for swapForCollateral).
     * @param minAmountOut Minimum acceptable output amount (slippage guard).
     */
    struct Order {
        bool isSwapForAsset;
        uint120 expiry;
        uint128 nonce;
        uint256 chainId;
        address benefactor;
        address beneficiary;
        address collateral;
        uint128 amountIn;
        uint128 minAmountOut;
    }

    /**
     * @notice Global configuration parameters.
     * @param maxSwapForAssetPerEpoch  Maximum asset swappable out per epoch (global).
     * @param maxSwapForCollateralPerEpoch Maximum asset swappable in per epoch (global).
     * @param defaultBenefactorMaxSwapForAssetPerEpoch Default per-benefactor epoch limit for swapForAsset.
     * @param defaultBenefactorMaxSwapForCollateralPerEpoch Default per-benefactor epoch limit for swapForCollateral.
     * @param epochDuration Duration of each epoch in seconds.
     * @param pegPrice Asset peg price in USD (18 decimals, 1e18 = $1.00).
     * @param maxSwapForAssetPerPeriod Maximum asset swappable out per period (global).
     * @param maxSwapForCollateralPerPeriod Maximum asset swappable in per period (global).
     * @param defaultBenefactorMaxSwapForAssetPerPeriod Default per-benefactor period limit for swapForAsset.
     * @param defaultBenefactorMaxSwapForCollateralPerPeriod Default per-benefactor period limit for swapForCollateral.
     * @param periodDuration Duration of each period in seconds.
     */
    struct GlobalConfig {
        uint128 maxSwapForAssetPerEpoch;
        uint128 maxSwapForCollateralPerEpoch;
        uint128 defaultBenefactorMaxSwapForAssetPerEpoch;
        uint128 defaultBenefactorMaxSwapForCollateralPerEpoch;
        uint256 epochDuration;
        uint128 pegPrice;
        uint128 maxSwapForAssetPerPeriod;
        uint128 maxSwapForCollateralPerPeriod;
        uint128 defaultBenefactorMaxSwapForAssetPerPeriod;
        uint128 defaultBenefactorMaxSwapForCollateralPerPeriod;
        uint256 periodDuration;
    }

    /**
     * @notice Configuration for a supported collateral token.
     * @param sendCustodianAddress Wallet that sends collateral out (swapForCollateral); PSM pulls via ERC-20 approval.
     * @param receiveCustodianAddress Wallet that receives collateral in (swapForAsset); no approval required.
     * @param oracleFeed Oracle feed address for this collateral/USD price.
     * @param maxSwapForAssetPerEpoch Max asset out using this collateral per epoch.
     * @param maxSwapForCollateralPerEpoch Max asset in for this collateral per epoch.
     * @param minOraclePrice Minimum oracle price allowed (depeg guard for swapForAsset).
     * @param maxOraclePrice Maximum oracle price allowed (depeg guard for swapForCollateral).
     * @param maxOracleAge Maximum age of oracle price data in seconds.
     * @param isActive Whether this collateral is currently active.
     * @param decimals Token decimals (must match the token contract).
     * @param defaultSwapForAssetFee Default fee in basis points for swapForAsset.
     * @param defaultSwapForCollateralFee Default fee in basis points for swapForCollateral.
     * @param maxSwapForAssetPerPeriod Max asset out using this collateral per period.
     * @param maxSwapForCollateralPerPeriod Max asset in for this collateral per period.
     */
    struct CollateralConfig {
        address sendCustodianAddress;
        address receiveCustodianAddress;
        address oracleFeed;
        uint128 maxSwapForAssetPerEpoch;
        uint128 maxSwapForCollateralPerEpoch;
        uint128 minOraclePrice;
        uint128 maxOraclePrice;
        uint256 maxOracleAge;
        bool isActive;
        uint8 decimals;
        uint16 defaultSwapForAssetFee;
        uint16 defaultSwapForCollateralFee;
        uint128 maxSwapForAssetPerPeriod;
        uint128 maxSwapForCollateralPerPeriod;
    }

    /**
     * @notice Per-benefactor configuration.
     */
    struct BenefactorConfig {
        bool isActive;
        uint128 maxSwapForAssetPerEpoch;
        uint128 maxSwapForCollateralPerEpoch;
        mapping(address => uint128) swapForAssetFeeByCollateral;
        mapping(address => uint128) swapForCollateralFeeByCollateral;
        mapping(address => DelegatedSignerStatus) delegatedSigners;
        mapping(address => bool) approvedBeneficiaries;
        mapping(address => bool) zeroSwapForAssetFeeExemptions;
        mapping(address => bool) zeroSwapForCollateralFeeExemptions;
        uint128 maxSwapForAssetPerPeriod;
        uint128 maxSwapForCollateralPerPeriod;
    }

    struct PeriodState {
        uint256 period;
        uint128 swappedForAssetInPeriod;
        uint128 swappedForCollateralInPeriod;
    }

    struct EpochState {
        uint256 epoch;
        uint128 swappedForAssetInEpoch;
        uint128 swappedForCollateralInEpoch;
    }

    struct GlobalState {
        GlobalConfig config;
        mapping(uint256 => EpochState) epochStateByDuration;
        mapping(uint256 => PeriodState) periodStateByDuration;
    }

    struct CollateralState {
        CollateralConfig config;
        mapping(uint256 => EpochState) epochStateByDuration;
        mapping(uint256 => PeriodState) periodStateByDuration;
    }

    struct BenefactorState {
        BenefactorConfig config;
        mapping(uint256 => EpochState) epochStateByDuration;
        mapping(uint256 => PeriodState) periodStateByDuration;
        mapping(uint128 => bool) orderNonceInvalidator;
    }

    /* --------------- EVENTS --------------- */

    event SwapExecuted(
        address indexed orderExecutor,
        address indexed benefactor,
        address indexed beneficiary,
        Order order,
        uint128 amountOut,
        uint128 feeAmount
    );

    event SwapEnabled();
    event SwapDisabled();

    event AssetSendCustodianUpdated(address indexed oldCustodian, address indexed newCustodian);
    event AssetReceiveCustodianUpdated(address indexed oldCustodian, address indexed newCustodian);

    event CollateralAdded(address indexed collateral, CollateralConfig config);
    event CollateralRemoved(address indexed collateral);
    event CollateralEnabled(address indexed collateral);
    event CollateralDisabled(address indexed collateral);
    event CollateralConfigUpdated(address indexed collateral, CollateralConfig config);

    event RescueFunds(address indexed recipient, address indexed token, uint128 amount);

    event BenefactorAdded(address indexed benefactor);
    event BenefactorRemoved(address indexed benefactor);
    event BenefactorEnabled(address indexed benefactor);
    event BenefactorDisabled(address indexed benefactor);
    event BenefactorMaxSwapForAssetPerEpochUpdated(address indexed benefactor, uint128 oldLimit, uint128 newLimit);
    event BenefactorMaxSwapForCollateralPerEpochUpdated(address indexed benefactor, uint128 oldLimit, uint128 newLimit);
    event BenefactorMaxSwapForAssetPerPeriodUpdated(address indexed benefactor, uint128 oldLimit, uint128 newLimit);
    event BenefactorMaxSwapForCollateralPerPeriodUpdated(
        address indexed benefactor, uint128 oldLimit, uint128 newLimit
    );
    event BenefactorSwapForAssetFeeUpdated(
        address indexed benefactor, address indexed collateral, uint128 oldFee, uint128 newFee
    );
    event BenefactorSwapForCollateralFeeUpdated(
        address indexed benefactor, address indexed collateral, uint128 oldFee, uint128 newFee
    );
    event BenefactorZeroSwapForAssetFeeExemptionUpdated(
        address indexed benefactor, address indexed collateral, bool exempt
    );
    event BenefactorZeroSwapForCollateralFeeExemptionUpdated(
        address indexed benefactor, address indexed collateral, bool exempt
    );

    event DelegatedSignerAdded(address indexed signer, address indexed benefactor);
    event DelegatedSignerConfirmed(address indexed signer, address indexed benefactor);
    event DelegatedSignerRemoved(address indexed signer, address indexed benefactor);

    event BeneficiaryApproved(address indexed benefactor, address indexed beneficiary);
    event BeneficiaryRemoved(address indexed benefactor, address indexed beneficiary);

    event EpochLimitsUpdated(
        uint128 oldMaxSwapForAsset,
        uint128 oldMaxSwapForCollateral,
        uint128 newMaxSwapForAsset,
        uint128 newMaxSwapForCollateral
    );
    event DefaultBenefactorMaxSwapForAssetPerEpochUpdated(uint128 oldLimit, uint128 newLimit);
    event DefaultBenefactorMaxSwapForCollateralPerEpochUpdated(uint128 oldLimit, uint128 newLimit);
    event EpochDurationUpdated(uint256 oldDuration, uint256 newDuration);

    event PeriodLimitsUpdated(
        uint128 oldMaxSwapForAsset,
        uint128 oldMaxSwapForCollateral,
        uint128 newMaxSwapForAsset,
        uint128 newMaxSwapForCollateral
    );
    event DefaultBenefactorMaxSwapForAssetPerPeriodUpdated(uint128 oldLimit, uint128 newLimit);
    event DefaultBenefactorMaxSwapForCollateralPerPeriodUpdated(uint128 oldLimit, uint128 newLimit);
    event PeriodDurationUpdated(uint256 oldDuration, uint256 newDuration);

    event PegPriceUpdated(uint128 oldPrice, uint128 newPrice);
    event OraclePriceValidated(address indexed collateral, uint256 price);

    /* --------------- ERRORS --------------- */

    error SwapDisabledError();
    error SwapAlreadyEnabled();
    error SwapAlreadyDisabled();

    error InsufficientAmountOut(uint128 expected, uint128 actual);
    error FuturePriceDetected(uint256 currentTimestamp, uint256 oracleTimestamp);
    error InvalidOraclePrice(address collateral, address oracleFeed, uint256 price);
    error OraclePriceTooOld(uint256 updatedAt);
    error InvalidOracleAge(uint256 provided);
    error OracleSwapForAssetDepegDetected(uint256 minOraclePrice, uint256 actualPrice);
    error OracleSwapForCollateralDepegDetected(uint256 maxOraclePrice, uint256 actualPrice);
    error InvalidOraclePriceThreshold(uint128 provided);

    error InvalidBenefactorFee(uint128 provided);
    error InvalidSwapForAssetFee(uint128 provided);
    error InvalidSwapForCollateralFee(uint128 provided);
    error InvalidDecimals(uint8 provided);
    error DecimalsMismatch(address collateral, uint8 configDecimals, uint8 tokenDecimals);
    error DecimalsCannotChange(address collateral, uint8 existingDecimals, uint8 newDecimals);

    error EpochDurationTooShort(uint256 provided, uint256 minimum);
    error EpochDurationTooLong(uint256 provided, uint256 maximum);
    error PeriodDurationTooShort(uint256 provided, uint256 minimum);
    error PeriodDurationTooLong(uint256 provided, uint256 maximum);

    error InvalidAddress(address provided);
    error InvalidAmount(uint256 provided);
    error InvalidAssetAmount(uint128 provided);
    error InvalidCollateralAmount(uint128 provided);
    error InvalidNonce(uint128 provided);
    error InvalidChainId(uint256 provided, uint256 expected);
    error OrderExpired(uint120 expiry, uint256 currentTimestamp);
    error InvalidPegPrice(uint128 provided, uint128 maximum);

    error CollateralNotActive();
    error CollateralAlreadyExists(address collateral);
    error CollateralAlreadyEnabled(address collateral);
    error CollateralNotSupported(address collateral);

    error BenefactorAlreadyExists(address benefactor);
    error BenefactorAlreadyEnabled(address benefactor);
    error BenefactorNotActive(address benefactor);
    error BeneficiaryNotApproved(address beneficiary);
    error DelegationNotAuthorized(address caller);
    error CustodianBenefactorConflict(address addr);

    error GlobalMaxSwapForAssetPerEpochExceeded(
        uint128 requested, uint128 currentUsage, uint128 maxAllowed, uint256 currentEpoch, uint256 epochDuration
    );
    error GlobalMaxSwapForCollateralPerEpochExceeded(
        uint128 requested, uint128 currentUsage, uint128 maxAllowed, uint256 currentEpoch, uint256 epochDuration
    );
    error CollateralMaxSwapForAssetPerEpochExceeded(
        address collateral,
        uint128 requested,
        uint128 currentUsage,
        uint128 maxAllowed,
        uint256 currentEpoch,
        uint256 epochDuration
    );
    error CollateralMaxSwapForCollateralPerEpochExceeded(
        address collateral,
        uint128 requested,
        uint128 currentUsage,
        uint128 maxAllowed,
        uint256 currentEpoch,
        uint256 epochDuration
    );
    error BenefactorMaxSwapForAssetPerEpochExceeded(
        address benefactor,
        uint128 requested,
        uint128 currentUsage,
        uint128 maxAllowed,
        uint256 currentEpoch,
        uint256 epochDuration
    );
    error BenefactorMaxSwapForCollateralPerEpochExceeded(
        address benefactor,
        uint128 requested,
        uint128 currentUsage,
        uint128 maxAllowed,
        uint256 currentEpoch,
        uint256 epochDuration
    );

    error GlobalMaxSwapForAssetPerPeriodExceeded(
        uint128 requested, uint128 currentUsage, uint128 maxAllowed, uint256 currentPeriod, uint256 periodDuration
    );
    error GlobalMaxSwapForCollateralPerPeriodExceeded(
        uint128 requested, uint128 currentUsage, uint128 maxAllowed, uint256 currentPeriod, uint256 periodDuration
    );
    error CollateralMaxSwapForAssetPerPeriodExceeded(
        address collateral,
        uint128 requested,
        uint128 currentUsage,
        uint128 maxAllowed,
        uint256 currentPeriod,
        uint256 periodDuration
    );
    error CollateralMaxSwapForCollateralPerPeriodExceeded(
        address collateral,
        uint128 requested,
        uint128 currentUsage,
        uint128 maxAllowed,
        uint256 currentPeriod,
        uint256 periodDuration
    );
    error BenefactorMaxSwapForAssetPerPeriodExceeded(
        address benefactor,
        uint128 requested,
        uint128 currentUsage,
        uint128 maxAllowed,
        uint256 currentPeriod,
        uint256 periodDuration
    );
    error BenefactorMaxSwapForCollateralPerPeriodExceeded(
        address benefactor,
        uint128 requested,
        uint128 currentUsage,
        uint128 maxAllowed,
        uint256 currentPeriod,
        uint256 periodDuration
    );

    /* --------------- FUNCTIONS --------------- */

    function swap(Order calldata order) external;

    function rescueFunds(address recipient, address token, uint128 amount) external;

    function enableSwap() external;
    function disableSwap() external;

    function setAssetSendCustodian(address newCustodian) external;
    function setAssetReceiveCustodian(address newCustodian) external;

    function enableCollateral(address collateral) external;
    function disableCollateral(address collateral) external;
    function addCollateral(address collateral, CollateralConfig calldata config) external;
    function removeCollateral(address collateral) external;
    function updateCollateralConfig(address collateral, CollateralConfig calldata config) external;

    function enableBenefactor(address benefactor) external;
    function disableBenefactor(address benefactor) external;
    function addBenefactor(address benefactor) external;
    function removeBenefactor(address benefactor) external;

    function setBenefactorMaxSwapForAssetPerEpoch(address benefactor, uint128 limit) external;
    function setBenefactorMaxSwapForCollateralPerEpoch(address benefactor, uint128 limit) external;
    function setBenefactorSwapForAssetFee(address benefactor, address collateral, uint128 fee) external;
    function setBenefactorSwapForCollateralFee(address benefactor, address collateral, uint128 fee) external;
    function setBenefactorZeroSwapForAssetFeeExemption(address benefactor, address collateral, bool exempt) external;
    function setBenefactorZeroSwapForCollateralFeeExemption(address benefactor, address collateral, bool exempt)
        external;
    function setBenefactorMaxSwapForAssetPerPeriod(address benefactor, uint128 limit) external;
    function setBenefactorMaxSwapForCollateralPerPeriod(address benefactor, uint128 limit) external;

    /**
     * @notice Proposes `signer` as a delegated signer for the caller (benefactor).
     *         Once the signer calls confirmDelegatedSigner, it gains full swap execution
     *         authority on behalf of the benefactor — including submitting swaps, consuming
     *         nonces, and spending the benefactor's token allowances. Treat an accepted
     *         delegated signer with the same trust level as the benefactor account itself.
     */
    function setDelegatedSigner(address signer) external;

    /**
     * @notice Accepts a pending delegation request from `benefactor`, granting the caller
     *         full swap execution authority for that benefactor within its configured
     *         limits and approved beneficiaries. Revoke via removeDelegatedSigner.
     */
    function confirmDelegatedSigner(address benefactor) external;

    /**
     * @notice Revokes a delegated signer, immediately removing their ability to submit
     *         swaps on behalf of the benefactor.
     */
    function removeDelegatedSigner(address signer) external;

    function setApprovedBeneficiary(address beneficiary, bool approved) external;

    function setGlobalEpochLimits(uint128 maxSwapForAssetPerEpoch, uint128 maxSwapForCollateralPerEpoch) external;
    function setDefaultBenefactorMaxSwapForAssetPerEpoch(uint128 limit) external;
    function setDefaultBenefactorMaxSwapForCollateralPerEpoch(uint128 limit) external;
    function setEpochDuration(uint256 newDuration) external;

    function setGlobalPeriodLimits(uint128 maxSwapForAssetPerPeriod, uint128 maxSwapForCollateralPerPeriod) external;
    function setDefaultBenefactorMaxSwapForAssetPerPeriod(uint128 limit) external;
    function setDefaultBenefactorMaxSwapForCollateralPerPeriod(uint128 limit) external;
    function setPeriodDuration(uint256 newDuration) external;

    function setPegPrice(uint128 _pegPrice) external;

    function getQuote(address benefactor, address collateral, uint128 amountIn, bool isSwapForAsset)
        external
        returns (uint128 feeAmount, uint128 amountOut);

    function getBenefactorConfig(address benefactor)
        external
        view
        returns (
            bool isActive,
            uint128 maxSwapForAssetPerEpoch,
            uint128 maxSwapForCollateralPerEpoch,
            uint128 maxSwapForAssetPerPeriod,
            uint128 maxSwapForCollateralPerPeriod
        );

    function getBenefactorFeesForCollateral(address benefactor, address collateral)
        external
        view
        returns (uint128 swapForAssetFee, uint128 swapForCollateralFee);

    function getDelegatedSignerStatus(address benefactor, address signer) external view returns (DelegatedSignerStatus);

    function isApprovedBeneficiary(address benefactor, address beneficiary) external view returns (bool);

    function getGlobalEpochTotals()
        external
        view
        returns (uint128 globalSwappedForAsset, uint128 globalSwappedForCollateral);

    function getCollateralEpochTotals(address collateral)
        external
        view
        returns (uint128 swappedForAsset, uint128 swappedForCollateral);

    function getBenefactorEpochTotal(address benefactor)
        external
        view
        returns (uint128 swappedForAsset, uint128 swappedForCollateral);

    function getGlobalPeriodTotals()
        external
        view
        returns (uint128 globalSwappedForAsset, uint128 globalSwappedForCollateral);

    function getCollateralPeriodTotals(address collateral)
        external
        view
        returns (uint128 swappedForAsset, uint128 swappedForCollateral);

    function getBenefactorPeriodTotal(address benefactor)
        external
        view
        returns (uint128 swappedForAsset, uint128 swappedForCollateral);

    function globalConfig() external view returns (GlobalConfig memory);
    function collateralConfig(address collateral) external view returns (CollateralConfig memory);

    function defaultBenefactorMaxSwapForAssetPerEpoch() external view returns (uint128);
    function defaultBenefactorMaxSwapForCollateralPerEpoch() external view returns (uint128);
    function defaultBenefactorMaxSwapForAssetPerPeriod() external view returns (uint128);
    function defaultBenefactorMaxSwapForCollateralPerPeriod() external view returns (uint128);

    function getEpochEndTimestamp() external view returns (uint256);
    function getPeriodEndTimestamp() external view returns (uint256);
}
