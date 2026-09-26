// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// guide_snap_wiring_test.dart — v0.57-B wiring tests for guide_snap.dart.
//
// lib/engine/guide3d/guide_snap.dart was dead code before v0.57-B: the
// [Guide3DSnap] engine existed but no runtime caller invoked it. v0.57-B
// wires it into [MainScreen._onStrokeUpdate]: when the GuidePanel's
// "Snap to guide" toggle ([MainScreen._guideSnap]) is ON, the world
// sample is projected onto the nearest guide surface within
// [Guide3DSnap.maxDistance] before being fed to the stroke stabilizer.
//
// Pumping the full [MainScreen] widget requires engine + FFI init (the
// boot probe) and is out of scope for a unit test — the same constraint
// v56-B's [test/undo_oom_test.dart] ran into. These tests therefore
// follow the v56-B pattern: they pin the wiring contract via the
// @visibleForTesting constant [MainScreen.kGuideSnapMaxDistance] and
// exercise the snap engine against a real guide geometry (a sphere
// primitive built by [PrimitiveGuideBuilder], the same builder the host
// uses for the GuidePanel's Primitives mode).

import 'package:feather_krita/core/math/math.dart';
import 'package:feather_krita/engine/guide3d/guide3d_primitive.dart';
import 'package:feather_krita/engine/guide3d/guide_manager.dart';
import 'package:feather_krita/engine/guide3d/guide_snap.dart';
import 'package:feather_krita/engine/guide3d/primitive_guide.dart';
import 'package:feather_krita/screens/main_screen.dart';
import 'package:flutter_test/flutter_test.dart';

void main() {
  group('MainScreen.kGuideSnapMaxDistance (v0.57-B wiring contract)', () {
    test('is exposed as a @visibleForTesting constant', () {
      // Pin the constant so a future refactor can't silently drift the
      // snap radius. The host constructs `_guideSnapHelper` with this
      // value (lib/screens/main_screen.dart `_guideSnapHelper = Guide3DSnap(
      // maxDistance: kGuideSnapMaxDistance)`), so changing the constant
      // here is the single source of truth for the snap radius.
      expect(MainScreen.kGuideSnapMaxDistance, 0.25);
    });

    test('matches the Guide3DSnap engine default', () {
      // The engine's own default (lib/engine/guide3d/guide_snap.dart
      // `Guide3DSnap({this.maxDistance = 0.25, ...})`) must match the
      // host's pinned constant — otherwise the host would be snapping at
      // a different radius than the engine's documented default.
      final engineDefault = Guide3DSnap().maxDistance;
      expect(engineDefault, MainScreen.kGuideSnapMaxDistance);
    });
  });

  group('Guide3DSnap.snapPoint (engine exercised via the runtime path)', () {
    // Build a unit sphere at the origin — the same geometry the host's
    // GuidePanel → insertPrimitive path produces for the "Sphere" shape.
    // Radius 1.0, identity transform → surface points are at distance 1.0
    // from the origin on every axis.
    final builder = const PrimitiveGuideBuilder();
    final sphere = builder.build(
      kind: Guide3DPrimitive.sphere,
      params: const PrimitiveGuideParams(radius: 1.0, segments: 16),
    );

    test('snaps a world point just outside the surface onto the surface', () {
      // A point 0.1 world units above the sphere's north pole (radius 1.0
      // → pole at (0,1,0); query at (0, 1.1, 0) is 0.1 outside). Within
      // the 0.25 snap radius → should snap onto (0, 1, 0).
      final snap = Guide3DSnap(maxDistance: MainScreen.kGuideSnapMaxDistance);
      final result = snap.snapPoint([sphere], Vector3(0.0, 1.1, 0.0));
      expect(result, isNotNull);
      expect(result!.point.x, closeTo(0.0, 1e-6));
      expect(result.point.y, closeTo(1.0, 1e-6));
      expect(result.point.z, closeTo(0.0, 1e-6));
      expect(result.guideId, 0);
    });

    test('returns null when the world point is outside the snap radius', () {
      // A point 5.0 units above the origin — 4.0 units outside the
      // sphere surface, well beyond the 0.25 snap radius. Should return
      // null so the host falls through to the original world sample
      // (no behaviour change vs. snap-OFF).
      final snap = Guide3DSnap(maxDistance: MainScreen.kGuideSnapMaxDistance);
      final result = snap.snapPoint([sphere], Vector3(0.0, 5.0, 0.0));
      expect(result, isNull);
    });

    test('skips invisible + locked guides (matches the runtime contract)', () {
      // The host's [_screenToWorld] raycasts every visible+unlocked guide;
      // [Guide3DSnap.snapPoint] honours the same flags. Verify both.
      final snap = Guide3DSnap(maxDistance: MainScreen.kGuideSnapMaxDistance);

      // Invisible guide — snap should skip it.
      final invisible = builder.build(
        kind: Guide3DPrimitive.sphere,
        params: const PrimitiveGuideParams(radius: 1.0, segments: 16),
      );
      invisible.visible = false;
      expect(
        snap.snapPoint([invisible], Vector3(0.0, 1.1, 0.0)),
        isNull,
        reason: 'invisible guides must not be snapped to',
      );

      // Locked guide — snap should skip it.
      final locked = builder.build(
        kind: Guide3DPrimitive.sphere,
        params: const PrimitiveGuideParams(radius: 1.0, segments: 16),
      );
      locked.locked = true;
      expect(
        snap.snapPoint([locked], Vector3(0.0, 1.1, 0.0)),
        isNull,
        reason: 'locked guides must not be snapped to',
      );
    });

    test('picks the nearest guide when several are in range', () {
      // Two spheres: one at the origin (radius 1.0), one offset by 3 units
      // along X (also radius 1.0). A query at (0, 1.1, 0) is 0.1 from the
      // first sphere's surface and ~3.1 from the second's — the first
      // should win (guideId 0).
      final offsetSphere = builder.build(
        kind: Guide3DPrimitive.sphere,
        params: const PrimitiveGuideParams(radius: 1.0, segments: 16),
      );
      offsetSphere.transform = Matrix4.translation(Vector3(3.0, 0.0, 0.0));
      offsetSphere.markTransformDirty();

      final snap = Guide3DSnap(maxDistance: MainScreen.kGuideSnapMaxDistance);
      final result =
          snap.snapPoint([sphere, offsetSphere], Vector3(0.0, 1.1, 0.0));
      expect(result, isNotNull);
      expect(result!.guideId, 0);
      expect(result.point.y, closeTo(1.0, 1e-6));
    });
  });

  group('Guide3DManager + Guide3DSnap (runtime composition)', () {
    test('manager.guides feeds snapPoint the same way the host wires it', () {
      // The host wiring is:
      //   final snap = _guideSnapHelper.snapPoint(_guides.guides, world);
      // Verify that composition works end-to-end with a real manager.
      final manager = Guide3DManager();
      final sphere = const PrimitiveGuideBuilder().build(
        kind: Guide3DPrimitive.sphere,
        params: const PrimitiveGuideParams(radius: 1.0, segments: 16),
      );
      manager.add(sphere);

      final snap = Guide3DSnap(maxDistance: MainScreen.kGuideSnapMaxDistance);
      final result = snap.snapPoint(manager.guides, Vector3(0.0, 1.1, 0.0));
      expect(result, isNotNull);
      expect(result!.point.y, closeTo(1.0, 1e-6));

      manager.clear();
      // After clearing, no guides → snap returns null.
      expect(snap.snapPoint(manager.guides, Vector3(0.0, 1.1, 0.0)), isNull);
    });
  });
}
