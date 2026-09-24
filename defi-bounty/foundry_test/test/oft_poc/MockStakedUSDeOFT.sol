// SPDX-License-Identifier: GPL-3.0
pragma solidity 0.8.30;

/// @title MockComposeStore
/// @notice Minimal endpoint mock that records sendCompose() calls.
contract MockComposeStore {
    struct ComposeEntry {
        address to;      // who the compose is queued for
        bytes32 guid;
        uint16 index;
        bytes message;
    }
    ComposeEntry[] public composes;
    mapping(address => uint256) public composeCount;

    function sendCompose(address _to, bytes32 _guid, uint16 _index, bytes calldata _message) external {
        composes.push(ComposeEntry({to: _to, guid: _guid, index: _index, message: _message}));
        composeCount[_to] += 1;
    }

    function composeLength() external view returns (uint256) {
        return composes.length;
    }

    function composeTo(uint256 i) external view returns (address) {
        return composes[i].to;
    }
}

/// @title MockStakedUSDeOFT
/// @notice Minimal mock replicating the _credit redirect + _lzReceive compose flow
///         in StakedUSDeOFT + OFTCore, WITHOUT the LayerZero endpoint/OApp stack.
///         Demonstrates that when the recipient is blacklisted:
///           - _credit redirects token mint to owner()
///           - BUT _lzReceive queues the compose for the ORIGINAL (blacklisted) recipient
contract MockStakedUSDeOFT {
    address public owner;
    mapping(address => bool) public blackList;
    MockComposeStore public composeStore;

    uint256 public totalSupply;
    mapping(address => uint256) public balanceOf;

    event RedistributeFunds(address indexed user, uint256 amount);
    event OFTReceived(bytes32 guid, address toAddress, uint256 amountReceivedLD);
    event ComposeQueued(address indexed to, bytes32 guid);

    constructor(address _owner) {
        owner = _owner;
        composeStore = new MockComposeStore();
    }

    function setBlackList(address user, bool status) external {
        blackList[user] = status;
    }

    /// @notice Replicates StakedUSDeOFT._credit: redirect to owner() if blacklisted
    function _credit(address to, uint256 amountLD) internal returns (uint256) {
        if (blackList[to]) {
            emit RedistributeFunds(to, amountLD);
            balanceOf[owner] += amountLD;
            totalSupply += amountLD;
            return amountLD; // minted to owner, but amountReceivedLD is returned
        } else {
            balanceOf[to] += amountLD;
            totalSupply += amountLD;
            return amountLD;
        }
    }

    /// @notice Replicates OFTCore._lzReceive: credits tokens THEN queues compose
    ///         for the ORIGINAL toAddress — NOT the actual recipient (owner).
    function lzReceive(bytes32 guid, address toAddress, uint256 amountSD, bool isComposed, bytes calldata composeMsg)
        external
    {
        // Step 1: credit (may redirect to owner if toAddress is blacklisted)
        uint256 amountReceivedLD = _credit(toAddress, amountSD);

        // Step 2: queue compose for the ORIGINAL toAddress (NOT owner!)
        if (isComposed) {
            // OFTComposeMsgCodec.encode(nonce, srcEid, amountReceivedLD, composeMsg)
            // simplified — just store the raw composeMsg
            composeStore.sendCompose(toAddress, guid, 0, composeMsg);
            emit ComposeQueued(toAddress, guid);
        }

        // Step 3: emit OFTReceived with toAddress (the original, NOT owner)
        emit OFTReceived(guid, toAddress, amountReceivedLD);
    }
}
