# Midas — `safeBulkApproveRequest(uint256[])` "permissionless" keeper is silently admin-gated

**Protocol**: Midas (Sherlock bounty #122, up to $500,000 USDC)
**Repo**: `github.com/midas-apps/contracts` (cloned to `/home/z/usual-midas-contracts`)
**Severity**: Low (functional / design inconsistency)
**Area**: Access control

---

## 1. Description

Both `DepositVault` and `RedemptionVault` expose a *parameterless* overload of
`safeBulkApproveRequest` that, by its signature and the absence of any modifier,
appears to be a **permissionless keeper** entry point — anyone can trigger batch
approval of pending requests at the *current* mToken rate (with the variation-tolerance
guard applied).

```solidity
// RedemptionVault.sol:346
function safeBulkApproveRequest(uint256[] calldata requestIds) external {
    uint256 currentMTokenRate = _getMTokenRate();
    safeBulkApproveRequest(requestIds, currentMTokenRate);   // delegates to public overload
}
```

However the public overload it delegates to is `onlyVaultAdmin`:

```solidity
// RedemptionVault.sol:431
function safeBulkApproveRequest(
    uint256[] calldata requestIds,
    uint256 newOutRate
) public onlyVaultAdmin {
    ...
}
```

Because Solidity preserves `msg.sender` across the internal call, the `onlyVaultAdmin`
check on the public overload is evaluated against the **original external caller**. A
non-admin caller therefore hits `revert` inside the public overload, despite the
external function advertising no access modifier.

The same pattern exists in `DepositVault.sol:354`.

The result is that the "permissionless" keeper is dead code for non-admins: it can only
ever be used by the vault admin (who could just as easily call the two-arg overload
with an off-chain-fetched rate). This is the *opposite* of the usual bug — here the
safeguard is *too* strict rather than too lax — so it is not directly exploitable for
theft. It is flagged because:

1. It contradicts the apparent intent (permissionless settlement of stale requests),
   meaning the protocol cannot rely on keepers to drain the pending-request queue if
   the admin is offline.
2. A future refactor that "fixes" the dead-code smell by removing the `onlyVaultAdmin`
   modifier on the public overload (to make the keeper work) would *create* a real
   vulnerability: anyone could force-settle requests at a moment of favourable
   mToken-rate movement within the tolerance window.

---

## 2. Contract / function / line

| File | Function | Line(s) |
|------|----------|---------|
| `contracts/RedemptionVault.sol` | `safeBulkApproveRequest(uint256[])` (external) | 346-349 |
| `contracts/RedemptionVault.sol` | `safeBulkApproveRequest(uint256[],uint256)` (public, gated) | 431-449 |
| `contracts/DepositVault.sol` | `safeBulkApproveRequest(uint256[])` (external) | 354-357 |
| `contracts/DepositVault.sol` | `safeBulkApproveRequest(uint256[],uint256)` (public, gated) | 424-442 |
| `contracts/abstract/ManageableVault.sol` | `onlyVaultAdmin` modifier | 138-141 |

---

## 3. Attack scenario

*Direct* exploitation is not possible because the safeguard (the public overload's
`onlyVaultAdmin`) does fire. The realistic risk is operational: a stuck request queue
that no keeper can clear, combined with a latent footgun for future maintainers.

Scenario showing the keeper is non-functional for non-admins:

1. User A creates a `redeemRequest` for 10 mToken. The request sits `Pending`.
2. The mToken rate moves 3% (within `variationTolerance`). The admin is offline.
3. A keeper bot calls `safeBulkApproveRequest([requestId])`.
4. Tx reverts: the internal call hits `onlyVaultAdmin` → `_onlyRole(REDEMPTION_VAULT_ADMIN_ROLE, keeperBot)` fails.
5. The request stays `Pending` indefinitely. User A cannot be settled until the admin
   returns, even though the rate is within tolerance and the keeper was willing to pay
   gas.

---

## 4. Proof of Concept (Foundry, sketch)

```solidity
function test_permissionless_keeper_reverts_for_non_admin() public {
    // setup: rv deployed, user greenlisted, user has mToken, redeemRequest created
    uint256 id = rv.redeemRequest(address(usdc), 10e18);

    vm.prank(keeperBot);                 // non-admin
    vm.expectRevert();                   // _onlyRole reverts
    rv.safeBulkApproveRequest(_arrayOf(id));

    // admin can still call the 1-arg version (proving it is admin-only in practice)
    vm.prank(vaultAdmin);
    rv.safeBulkApproveRequest(_arrayOf(id));
    assertEq(uint256(rv.redeemRequests(id).status), uint256(IRedemptionVault.RequestStatus.Processed));
}
```

---

## 5. Impact

* **Fund loss**: none.
* **Availability**: pending deposit/redeem requests cannot be settled by keepers; only
  the vault admin can move them. If the admin is unavailable, user funds remain
  escrowed in the vault (redeem) or un-minted (deposit) beyond the intended
  time-window.
* **Latent risk**: removing the redundant-looking `onlyVaultAdmin` on the public
  overload to "enable keepers" would expose the settlement to griefing / rate-timing
  attacks by arbitrary callers.

---

## 6. Three-perspective audit

**Exploitability**
No theft path. The severity is purely operational/availability plus a maintainer
footgun. It is reported because the code's *shape* implies permissionless settlement
that does not in fact work.

**Economic impact**
Bounded by the duration of admin unavailability: users whose requests are stuck cannot
access their funds (redeem) or their mToken (deposit) during that window. No
permanent loss as long as the admin eventually returns.

**Fix recommendation**
Pick one explicitly:
- **Option A (intended permissionless keeper):** remove `onlyVaultAdmin` from the
  public `safeBulkApproveRequest(uint256[],uint256)` overload, *and* add an explicit
  re-validation that `newOutRate` equals the live `_getMTokenRate()` when called via
  the 1-arg path, so that only the variation-tolerance check (not admin trust) gates
  settlement. Document that settlement is permissionless.
- **Option B (intended admin-only):** add `onlyVaultAdmin` directly to the 1-arg
  external overload and delete the misleading internal delegation, so the access model
  is obvious to readers and static-analysis tools.

Either option removes the current "looks permissionless, isn't" ambiguity.

---

## 7. References

* `DepositVault.sol:354-357`, `:424-442`
* `RedemptionVault.sol:346-349`, `:431-449`
* `ManageableVault.onlyVaultAdmin`: `contracts/abstract/ManageableVault.sol:138-141`
