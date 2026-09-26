// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// lightrig_test.dart — verifies the canvas painter consumes
// [MaterialLightRig.evaluate] for its per-vertex lit-sign decision
// (honesty-gap 2 fix).
//
// Before v0.53 the painter (_CanvasPainter._strokeGeometry in
// lib/ui/widgets/canvas_viewport.dart) computed the lit sign with an
// inline 2D Lambert: `sign(dot(normal, -lightDir))`. That calc ignored
// the material light rig's ambient / intensity / color and diverged
// from the Shaded material's lighting model. The fix routes the
// lit-sign decision through the top-level [litSignFromRig] helper,
// which calls [MaterialLightRig.evaluate] so all strokes use the same
// lighting as the Shaded material + BrushRenderer.
//
// These tests verify:
//   1. [litSignFromRig] returns +1.0 when the normal faces the light
//      and -1.0 when it faces away — proving the helper derives the
//      sign from the rig's key-light direction (not a hardcoded or
//      legacy 2D direction).
//   2. [litSignFromRig] actually calls [MaterialLightRig.evaluate]
//      (mock the rig via a counting subclass, assert the call count).
//   3. Flipping the rig's key-light direction flips the lit sign —
//      proving the painter now respects the rig's lighting rather than
//      a fixed direction.

import 'package:feather_krita/core/math/vec3.dart';
import 'package:feather_krita/engine/material/light_rig.dart';
import 'package:feather_krita/ui/widgets/canvas_viewport.dart';
import 'package:flutter_test/flutter_test.dart';

/// A [MaterialLightRig] subclass that counts calls to [evaluate] so
/// tests can assert the painter's lit-sign helper actually invokes the
/// rig (not some inline fallback).
class _CountingRig extends MaterialLightRig {
  _CountingRig({required Vec3 direction})
      : super(direction: direction, ambient: 0.35);

  /// Number of times [evaluate] has been called since construction.
  int evaluateCalls = 0;

  @override
  LightResult evaluate(Vec3 normal, Vec3 viewDir) {
    evaluateCalls++;
    return super.evaluate(normal, viewDir);
  }
}

void main() {
  // Out-of-screen view direction — the convention the painter's
  // _strokeGeometry helper uses (a 2D tube cross-section has no z, so
  // the view vector points along +z out of the screen plane).
  const view = Vec3(0.0, 0.0, 1.0);

  group('litSignFromRig — painter consumes MaterialLightRig.evaluate', () {
    test('returns +1 when the normal faces the light, -1 when it faces away', () {
      // Light travels straight down: direction = (0, -1, 0) means the
      // light source is above (+y) and rays travel toward -y. A surface
      // normal pointing up (0, 1, 0) faces the source → lit (+1). A
      // normal pointing down (0, -1, 0) faces away → shadow (-1).
      final rig = MaterialLightRig(
        direction: const Vec3(0.0, -1.0, 0.0),
        ambient: 0.35,
      );

      expect(
        litSignFromRig(rig, const Vec3(0.0, 1.0, 0.0), view),
        1.0,
        reason: 'normal facing the light should be on the lit side',
      );
      expect(
        litSignFromRig(rig, const Vec3(0.0, -1.0, 0.0), view),
        -1.0,
        reason: 'normal facing away from the light should be on the shadow side',
      );
    });

    test('actually calls MaterialLightRig.evaluate (mock rig, assert called)', () {
      final rig = _CountingRig(direction: const Vec3(0.0, -1.0, 0.0));
      expect(rig.evaluateCalls, 0, reason: 'fresh rig should have zero calls');

      // One lit-sign computation → exactly one evaluate() call.
      final sign = litSignFromRig(rig, const Vec3(0.0, 1.0, 0.0), view);
      expect(rig.evaluateCalls, 1,
          reason: 'litSignFromRig must route through rig.evaluate');
      expect(sign, 1.0);

      // A second call with a different normal → second evaluate() call.
      litSignFromRig(rig, const Vec3(0.0, -1.0, 0.0), view);
      expect(rig.evaluateCalls, 2,
          reason: 'each litSignFromRig call maps to one evaluate() call');
    });

    test('respects the rig\'s key-light direction (flipping the rig flips the sign)', () {
      // Rig A: light from +x (direction = -x, rays travel toward -x).
      // Normal (1, 0, 0) points toward +x (toward the source) → lit.
      final rigA = MaterialLightRig(
        direction: const Vec3(-1.0, 0.0, 0.0),
        ambient: 0.35,
      );
      expect(litSignFromRig(rigA, const Vec3(1.0, 0.0, 0.0), view), 1.0);
      expect(litSignFromRig(rigA, const Vec3(-1.0, 0.0, 0.0), view), -1.0);

      // Rig B: light from -x (direction = +x, rays travel toward +x).
      // Now normal (1, 0, 0) points away from the source → shadow.
      // Flipping the rig's direction must flip the lit-sign decision.
      final rigB = MaterialLightRig(
        direction: const Vec3(1.0, 0.0, 0.0),
        ambient: 0.35,
      );
      expect(litSignFromRig(rigB, const Vec3(1.0, 0.0, 0.0), view), -1.0,
          reason: 'flipped rig: normal (+x) now faces away → shadow');
      expect(litSignFromRig(rigB, const Vec3(-1.0, 0.0, 0.0), view), 1.0,
          reason: 'flipped rig: normal (-x) now faces the light → lit');
    });

    test('lit sign is independent of view direction sign (diffuse-only decision)', () {
      // The lit-sign decision is driven by the diffuse term
      // (n · (-L)), not the specular term (which depends on the view
      // vector). Flipping the view direction must NOT flip the lit
      // sign — only the specular highlight would change.
      final rig = MaterialLightRig(
        direction: const Vec3(0.0, -1.0, 0.0),
        ambient: 0.35,
      );
      const normal = Vec3(0.0, 1.0, 0.0); // faces the light

      expect(litSignFromRig(rig, normal, const Vec3(0.0, 0.0, 1.0)), 1.0);
      expect(litSignFromRig(rig, normal, const Vec3(0.0, 0.0, -1.0)), 1.0,
          reason: 'view direction does not affect the diffuse lit-sign');
    });
  });
}
