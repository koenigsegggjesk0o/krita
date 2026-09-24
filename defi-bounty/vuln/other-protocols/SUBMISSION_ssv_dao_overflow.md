# Submission Draft — SSV DAO `networkTotalEarningsSSV()` uint64 Overflow

## Title

SSV DAO earnings overflow in `networkTotalEarningsSSV()` — permanent DoS of all SSV-side operations

## Severity

Critical

## Summary

`ProtocolLib.networkTotalEarningsSSV()` computes the SSV DAO's total accumulated
earnings using pure `uint64` arithmetic with no intermediate widening. When the
product `blockDiff × networkFee_raw × daoValidatorCount` exceeds `type(uint64).max`,
the function reverts with Solidity 0.8 panic 0x11 (arithmetic overflow). This
view function is on every SSV-side state-mutating path, so once the threshold is
crossed, all SSV cluster operations (register, remove, liquidate, reactivate,
migrate), the DAO fee update, and the DAO earnings withdrawal permanently revert.
There is no recovery path: setting the fee to 0 overflows, removing validators
overflows, and withdrawing earnings overflows.

## Vulnerability Detail

`ProtocolLib.sol` lines 97–99:

```solidity
function networkTotalEarningsSSV(StorageProtocol storage sp) internal view returns (PackedSSV) {
    return PackedSSV.wrap(
        PackedSSV.unwrap(sp.daoBalance)
        + (uint64(block.number) - sp.daoIndexBlockNumber)
          * PackedSSV.unwrap(sp.networkFee)     // uint64
          * sp.daoValidatorCount                // uint32 → uint64
    );
}
```

All operands are `uint64` or narrower (`uint32` promoted to `uint64`). The
multiplication `blockDiff × fee_raw × daoValidatorCount` is performed in
`uint64` with Solidity 0.8 overflow checks. There is no intermediate widening
to `uint128` or `uint256`.

This contrasts with the ETH-side equivalent `networkTotalEarnings()` (L84–90),
which uses `uint128` intermediates and an explicit `_safeUint64` cast — the
safe pattern. The SSV-side function is the only outlier, indicating an oversight.

The function is called by:
- `updateDAOEarningsSSV()` (ProtocolLib:74) → called by `updateDAOSSV()` (ProtocolLib:128) → called by `SSVClusters.liquidateSSV` (L107), `SSVClusters.migrateClusterToETH` (L285), `SSVValidators._bulkRemoveValidator` SSV branch (L246)
- `updateNetworkFeeSSV()` (ProtocolLib:54) → called by `SSVDAO.updateNetworkFeeSSV` (L44)
- `withdrawNetworkSSVEarnings()` directly (SSVDAO:56)
- `SSVViews` read path (SSVViews:499)

## Impact

Once the overflow threshold is crossed (e.g. 1 845 blocks ≈ 6.1 h at 1 SSV/block
fee and 100k validators, or ~25.6 days at 0.01 SSV/block and 100k validators):

1. **SSV cluster operations permanently revert** — `liquidateSSV`,
   `migrateClusterToETH` (non-liquidated), `removeValidator`/`bulkRemoveValidator`
   (SSV branch) all call `updateDAOSSV` → overflow.
2. **DAO fee update permanently reverts** — `updateNetworkFeeSSV(0)` (the
   "stop the bleeding" action) calls `updateDAOEarningsSSV` → overflow.
3. **DAO earnings withdrawal permanently reverts** — `withdrawNetworkSSVEarnings`
   calls `networkTotalEarningsSSV()` directly → overflow.
4. **SSV views revert** — `SSVViews` read path calls `networkTotalEarningsSSV()`.

There is no recovery path. SSV cluster owners cannot liquidate or migrate their
clusters. The DAO cannot withdraw accumulated SSV fees. Operators cannot
withdraw SSV earnings (the operator snapshot update path also depends on
SSV-side arithmetic).

The trigger is **elapsed time** — no attacker action is required. The SSV-to-ETH
migration window is the highest-risk period: `daoValidatorCount` is still high
while SSV operations become infrequent.

## Proof of Concept

Foundry PoC at `/home/z/ssv/test/foundry/POC_SSVDAOOverflow.t.sol` (5 tests,
all pass):

```solidity
// Setup: 1 SSV/block fee (raw=1e11), 100_000 SSV validators, daoBalance=0
// Overflow threshold: blockDiff * 1e11 * 1e5 > 2^64-1  →  blockDiff > 1844.67

function test_OverflowBlocksUpdateNetworkFeeSSV() public {
    vm.roll(block.number + 1845);  // blockDiff = 1845 → overflow
    vm.expectRevert();
    dao.updateNetworkFeeSSV(0);    // DAO owner tries to lower fee to 0
    // Reverts with: panic: arithmetic underflow or overflow (0x11)
}
```

Full test output:

```
[PASS] test_NoOverflowAtSafeBlockDiff() (gas: 62761)
[PASS] test_OverflowBlocksLiquidateSSV() (gas: 120553)
[PASS] test_OverflowBlocksUpdateNetworkFeeSSV() (gas: 34879)
[PASS] test_OverflowBlocksWithdrawNetworkSSVEarnings() (gas: 56809)
[PASS] test_OverflowThresholdExact() (gas: 849)
```

All three SSV-side write paths (`updateNetworkFeeSSV`, `withdrawNetworkSSVEarnings`,
`liquidateSSV`) revert with `panic 0x11` at blockDiff = 1845. The positive control
(blockDiff = 1844) succeeds, confirming the exact threshold.

## Recommendation

Widen the intermediate computation to `uint256` (matching the ETH-side pattern):

```solidity
function networkTotalEarningsSSV(StorageProtocol storage sp) internal view returns (PackedSSV) {
    uint256 blockDiff = uint256(block.number) - sp.daoIndexBlockNumber;
    uint256 earningsDelta = blockDiff
        * uint256(PackedSSV.unwrap(sp.networkFee))
        * uint256(sp.daoValidatorCount);
    uint256 total = uint256(PackedSSV.unwrap(sp.daoBalance)) + earningsDelta;
    require(total <= type(uint64).max, "SSV DAO earnings overflow");
    return PackedSSV.wrap(uint64(total));
}
```

Additionally, add a permissionless `syncSSVEarnings()` function to reset
`daoIndexBlockNumber` without a cluster operation, preventing `blockDiff` from
growing unbounded during SSV inactivity.
