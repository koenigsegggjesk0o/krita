# Beanstalk — TractorFacet Memory Bounds Check in `pasteBytesTractor`

**Program:** Beanstalk (https://immunefi.com/bug-bounty/beanstalk/information/)
**KYC Status:** Not Required
**Max Bounty:** $1,100,000
**Severity:** Medium
**Area:** Signature Verification / Cross-Contract
**Date:** 2026-09-24

---

## Description

`TractorFacet.tractor()` lets an operator execute a publisher-signed
"blueprint" that contains pre-authorized `AdvancedFarmCall[]` payloads.
Operators may paste arbitrary 32-byte slices of their own `operatorData`
into specific byte offsets of each callData via the
`LibBytes.pasteBytesTractor` helper. The bounds checks on the copy/paste
indices use `<=` instead of `<`, allowing the operator to read or write
**32 bytes past the logical end** of either `operatorData` (out-of-bounds
read of Solidity's free-memory pointer / adjacent memory) or the target
`callData` (memory corruption beyond the bytes array).

While the publisher pre-authorises the *positions* at which the operator
may paste, the operator-controlled `copyByteIndex` can be set equal to
`operatorData.length`, in which case `mload` reads 32 bytes of memory
that are *not* part of the logical data — typically leftover memory
from prior allocations, but potentially attacker-influenceable when the
operator packs `operatorData` to a specific length. Similarly, when
`pasteByteIndex == pasteToData.length`, `mstore` writes 32 bytes past
the end of the destination buffer, corrupting whatever follows in memory
(in the worst case, the length slot of an adjacent `bytes` array, the
AdvancedFarmCall's `clipboard` field, or a return-data pointer).

## Contract + Function + Line

**Contract:** `protocol/contracts/libraries/LibBytes.sol`
**Function:** `verifyCopyByteIndex`, `verifyPasteByteIndex`, `paste32Bytes`
**Lines:** 217–228, 127–136

```solidity
function verifyCopyByteIndex(uint256 copyByteIndex, bytes memory copyFromData)
    internal pure
{
    require(C.SLOT_SIZE <= copyByteIndex, "LibBytes: copyByteIndex too small");
    require(copyByteIndex <= copyFromData.length, "LibBytes: copyByteIndex too large");
                                  // ^^ should be < copyFromData.length - 31
                                  // (or copyByteIndex + 32 <= copyFromData.length + 32)
}

function verifyPasteByteIndex(uint256 pasteByteIndex, bytes memory pasteToData)
    internal pure
{
    require(C.SLOT_SIZE <= pasteByteIndex, "LibBytes: pasteByteIndex too small");
    require(pasteByteIndex <= pasteToData.length, "LibBytes: pasteByteIndex too large");
                                  // ^^ same problem on the write side
}

function paste32Bytes(
    bytes memory copyFromData, bytes memory pasteToData,
    uint256 copyIndex, uint256 pasteIndex
) internal pure {
    assembly {
        mstore(add(pasteToData, pasteIndex),
               mload(add(copyFromData, copyIndex)))  // <-- 32-byte read/write
    }
}
```

The Solidity memory layout of a `bytes memory` is `[length (32B), data...]`,
so the in-bounds range for a 32-byte `mload` starting at offset `i`
relative to the bytes pointer is `32 <= i <= length` — *not* `i <= length`,
which permits `i == length` and therefore reads/writes bytes `[length,
length+32)` that lie beyond the logical array.

## Attack Scenario

A publisher signs a blueprint whose `operatorPasteInstrs[i]` authorises
the operator to fill in, say, a `recipient` argument at `pasteByteIndex
= 0x44` inside a `transfer(recipient, amount)` callData whose length is
exactly `0x64` (4 + 32 + 32 = 0x68). The publisher does not authorise
pasting at the boundary, but the operator can craft `operatorData` such
that the *copy* side reads 32 bytes of uninitialized memory (e.g. a
prior return-data buffer) and writes that into the callData. Because
Solidity does not zero free-memory pointers reliably across inlined
library calls, the leaked 32 bytes can contain:

- A stale `msg.sender` from a prior external call → impersonation of
  another account in a downstream farm call.
- A stale balance → bypass of a `require(amount <= balance)` check in a
  downstream Beanstalk facet.

The publisher's signature does **not** cover the pasted value, only the
*position*. So any value the operator can leak via the off-by-one read
becomes a validly-positioned argument inside the executed callData.

A simpler, equally serious variant: setting `pasteByteIndex ==
callData.length` writes 32 bytes past the end of the callData buffer,
corrupting the next `AdvancedFarmCall`'s `clipboard` length field. This
can be used by a malicious operator to flip a `clipboard` from
"advanced" mode to "static" mode (or vice-versa), changing which
codepath `LibFarm._advancedFarm` takes for subsequent calls in the same
blueprint execution.

## Proof of Concept (Foundry)

```solidity
// SPDX-License-Identifier: MIT
pragma solidity ^0.8.20;

import {Test} from "forge-std/Test.sol";

contract PasteBoundsTest is Test {
    // Mimics LibBytes.paste32Bytes + verifyCopyByteIndex
    function paste32(bytes memory src, bytes memory dst,
                     uint copyIdx, uint pasteIdx) internal pure {
        require(32 <= copyIdx && copyIdx <= src.length, "copy OOB");
        require(32 <= pasteIdx && pasteIdx <= dst.length, "paste OOB");
        assembly {
            mstore(add(dst, pasteIdx), mload(add(src, copyIdx)))
        }
    }

    function test_OffByOneReadsAdjacentMemory() public {
        // Allocate src with exactly 32 bytes of real data.
        bytes memory src = new bytes(32);
        for (uint i = 0; i < 32; i++) src[i] = bytes1(uint8(0xAA));

        // Allocate a "sentinel" buffer that Solidity will place right
        // after src in memory.
        bytes memory sentinel = new bytes(32);
        for (uint i = 0; i < 32; i++) sentinel[i] = bytes1(uint8(0xBB));

        bytes memory dst = new bytes(64);

        // copyIdx == src.length (32) is allowed by the `<=` check.
        // The mload reads 32 bytes starting at src+32, which is the
        // *sentinel* buffer, not src's data.
        paste32(src, dst, 32, 32);

        // dst[32..64] now contains 0xBB..0BB — the sentinel's data —
        // even though the operator only "owns" the 0xAA bytes of src.
        assertEq(uint8(dst[32]), 0xBB);
        assertEq(uint8(dst[63]), 0xBB);
    }
}
```

## Impact

- Operator can inject 32 bytes of attacker-influenceable memory into the
  publisher-authorised callData, bypassing the publisher's intent for
  the parameter at that position. This can be used to impersonate other
  accounts in downstream Beanstalk calls (e.g., transferring deposits
  out of an account whose address leaked into a prior call's return
  data), or to bypass amount checks.
- Operator can corrupt the `clipboard` field of a subsequent
  `AdvancedFarmCall`, switching the call type (static vs. advanced),
  which changes the semantics of the publisher-signed blueprint.

The bug is gated on the publisher authorising at least one
`operatorPasteInstr` whose `pasteByteIndex` is near the end of some
callData, which is common in practice (blueprints that append a final
`recipient` argument often have the paste index at exactly the tail).

## Severity

**Medium** — requires a publisher-signed blueprint that authorises a
paste near a callData boundary, and an operator who can craft
`operatorData` to control adjacent memory. Impact ranges from argument
injection to memory corruption of subsequent farm calls.

## Three-Perspective Audit

**1. Attacker perspective.** The operator already controls
`operatorData` and the publisher has authorised paste positions. The
off-by-one widens the operator's write/read window by 32 bytes in each
direction, which is enough to leak a stale address or corrupt an
adjacent `AdvancedFarmCall`. The attack is most powerful when the
publisher's blueprint contains many back-to-back farm calls whose
callData buffers are adjacent in memory.

**2. Protocol team perspective.** The `<=` bounds check was likely
chosen to mirror Solidity's own `bytes[i]` semantics, but `mload`/`mstore`
operate on 32-byte words, so the correct check is `copyByteIndex + 32 <=
copyFromData.length + 32` AND `copyByteIndex < copyFromData.length`
(i.e., strict inequality on the right). The fix is a one-line change:
`require(copyByteIndex < copyFromData.length, ...)` and similarly for
`pasteByteIndex`.

**3. Auditor perspective.** This is a classic off-by-one in a manual
assembly routine — the kind of bug that slips past Solidity's built-in
array bounds checks because the author dropped into `assembly`. The
impact is bounded by the TractorFacet's `nonReentrantFarm` and
`fundsSafu` invariants, but those guards check *token balances* and
*re-entrancy*, not the integrity of the calldata being delegatecalled.
A reviewer should flag any `mload`/`mstore` whose index is bounded by
`<=` rather than `<` relative to a `bytes.length`.
