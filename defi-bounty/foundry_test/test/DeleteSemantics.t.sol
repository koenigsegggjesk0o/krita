// SPDX-License-Identifier: MIT
pragma solidity 0.8.30;

// ============================================================================
// DeleteSemantics.t.sol
// ----------------------------------------------------------------------------
// ISOLATED Solidity language-semantics test (no PSM dependency).
//
// Verifies the foundational claim underlying the PSM removeBenefactor bug:
//   "delete on a struct (or a struct field) does NOT clear nested mappings;
//    mappings are immutable under delete; only value-type members are reset."
//
// This test intentionally uses a tiny standalone contract that mirrors the
// *shape* of PSM's BenefactorState / BenefactorConfig (struct-in-struct with
// mappings at both levels) so the result is unambiguously about the LANGUAGE,
// not about any particular contract implementation.
//
// Run:
//   forge test -vvvv --match-contract DeleteSemantics
// ============================================================================

import {Test} from "forge-std/Test.sol";

contract DeleteSemanticsHarness {
    // -- mirrors IPSM.BenefactorConfig shape: value types + 2 mappings --
    struct Inner {
        uint256 value;
        mapping(address => bool) flag;
        mapping(address => uint256) score;
    }

    // -- mirrors IPSM.BenefactorState shape: nested struct + 1 direct mapping --
    struct Outer {
        Inner inner;
        mapping(address => uint256) directMapping;
    }

    // NOTE: `internal` (not `public`) because Solidity forbids auto-getters
    // for mappings containing nested mapping types (recursive type error).
    // The explicit `check(...)` getter below exposes the four fields we need.
    mapping(address => Outer) internal data;

    function set(address key) external {
        data[key].inner.value = 42;
        data[key].inner.flag[msg.sender] = true;
        data[key].inner.score[msg.sender] = 7;
        data[key].directMapping[msg.sender] = 100;
    }

    // PSM.removeBenefactor analogue: delete the nested config struct.
    function removeInner(address key) external {
        delete data[key].inner; // does this clear flag/score mappings?
    }

    // Stronger analogue: delete the whole top-level value.
    function removeWhole(address key) external {
        delete data[key]; // does this clear directMapping + nested mappings?
    }

    // Single-key delete carve-out: `delete mapping[k]` IS allowed for one key.
    function deleteDirectMapping(address key, address who) external {
        delete data[key].directMapping[who];
    }

    function check(address key, address who)
        external
        view
        returns (uint256 value, bool flag, uint256 score, uint256 direct)
    {
        return (
            data[key].inner.value,
            data[key].inner.flag[who],
            data[key].inner.score[who],
            data[key].directMapping[who]
        );
    }
}

contract DeleteSemantics is Test {
    DeleteSemanticsHarness internal h;
    address internal key = makeAddr("key");
    address internal who = makeAddr("who");

    function setUp() public {
        h = new DeleteSemanticsHarness();
        vm.prank(who);
        h.set(key);
    }

    // ---- Test 1: delete nested struct -> value reset, mappings PERSIST ----
    function test_deleteInnerStruct_valueResets_mappingsPersist() public {
        // Pre-condition: all four fields are set
        (uint256 v, bool f, uint256 s, uint256 d) = h.check(key, who);
        assertEq(v, 42, "pre: value set");
        assertTrue(f, "pre: flag set");
        assertEq(s, 7, "pre: score set");
        assertEq(d, 100, "pre: direct set");

        // Act: delete the nested Inner struct (PSM's removeBenefactor pattern)
        h.removeInner(key);

        // Assert: only the value-type field reset; both mappings persisted.
        (v, f, s, d) = h.check(key, who);
        assertEq(v, 0, "POST: value MUST be reset (value type)");
        assertTrue(f, "CONFIRMED BUG: inner.flag mapping PERSISTS after delete struct");
        assertEq(s, 7, "CONFIRMED BUG: inner.score mapping PERSISTS after delete struct");
        assertEq(d, 100, "POST: directMapping untouched (delete was on inner struct only)");
    }

    // ---- Test 2: delete whole top-level value -> mappings STILL persist ----
    function test_deleteWhole_valueResets_allMappingsPersist() public {
        h.removeWhole(key);

        (uint256 v, bool f, uint256 s, uint256 d) = h.check(key, who);
        assertEq(v, 0, "POST: value MUST be reset (value type)");
        assertTrue(f, "CONFIRMED BUG: nested flag mapping PERSISTS after delete whole");
        assertEq(s, 7, "CONFIRMED BUG: nested score mapping PERSISTS after delete whole");
        assertEq(d, 100, "CONFIRMED BUG: top-level directMapping PERSISTS after delete whole");
    }

    // ---- Test 3: individual key delete DOES work (sanity / contrast) ----
    function test_deleteIndividualKey_clearsValue() public {
        // Solidity carve-out: `delete mapping[k]` for a single key IS allowed
        // and is the only correct way to clear a single mapping slot.
        h.deleteDirectMapping(key, who);

        (, bool f, uint256 s, uint256 d) = h.check(key, who);
        assertTrue(f, "flag still set (we didn't touch it)");
        assertEq(s, 7, "score still set (we didn't touch it)");
        assertEq(d, 0, "individual-key delete DID clear directMapping[who]");
    }
}
