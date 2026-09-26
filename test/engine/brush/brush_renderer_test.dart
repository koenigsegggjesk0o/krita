// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// brush_renderer_test.dart — v57-C File 2 tests for BrushRenderer.
//
// Validates the new [BrushRenderer.projectStroke] polyline-projection
// API that the host's [_buildCanvasScene] now delegates to (replacing
// the inline world→screen projection loop).
//
// The tests use a synthetic [CameraContext] with a known projection
// function (orthographic-style: identity matrix + NDC = world, with
// clipW = 1.0 for in-front points) so the expected screen coordinates
// are computable by hand. The host's real projection (perspective via
// OrbitCamera.viewProjectionMatrix) is exercised end-to-end only via
// flutter analyze — pumping MainScreen requires engine + FFI init
// (out of test scope, same constraint as v56-B / v57-C GAP 0).

import 'dart:ui' show Color, Offset;

import 'package:feather_krita/engine/brush/brush_renderer.dart';
import 'package:feather_krita/models/stroke.dart';
import 'package:flutter_test/flutter_test.dart';
import 'package:vector_math/vector_math_64.dart';

/// Builds a synthetic [CameraContext] whose `project` maps world (x, y, z)
/// to screen (x, y) via a simple viewport mapping (clipW = 1.0 for
/// in-front points where z <= 0, -1.0 for behind-camera where z > 0).
/// The viewport is [width] x [height]; world (0, 0) maps to the viewport
/// center; world x in [-1, 1] maps to [0, width]; world y in [-1, 1]
/// maps to [height, 0] (Y flipped, screen-down = +Y).
CameraContext _orthoCam(double width, double height, {Vector3? cameraPos}) {
  return CameraContext(
    cameraPos: cameraPos ?? Vector3(0, 0, -5),
    project: (world) {
      // Behind-camera cull: world z > 0 (camera looks down -Z).
      if (world.z > 0) return (Offset.zero, -1.0);
      final sx = (world.x * 0.5 + 0.5) * width;
      final sy = (1.0 - (world.y * 0.5 + 0.5)) * height;
      return (Offset(sx, sy), 1.0);
    },
  );
}

StrokePoint _p(double x, double y, double z) =>
    StrokePoint(position: Vector3(x, y, z));

Stroke _stroke(List<StrokePoint> pts) => Stroke(id: 1, points: pts);

void main() {
  group('BrushRenderer.projectStroke', () {
    test('empty stroke → empty list', () {
      final cam = _orthoCam(800, 600);
      final renderer = BrushRenderer();
      expect(renderer.projectStroke(_stroke([]), cam), isEmpty);
    });

    test('single-point stroke → one projected offset (no cull)', () {
      final cam = _orthoCam(800, 600);
      final renderer = BrushRenderer();
      // World (0, 0, -1) is in front of the camera (z < 0).
      final s = _stroke([_p(0, 0, -1)]);
      final out = renderer.projectStroke(s, cam);
      expect(out.length, 1);
      // World (0, 0) → viewport center (400, 300).
      expect(out[0], const Offset(400, 300));
    });

    test('multi-point stroke projects each in order', () {
      final cam = _orthoCam(800, 600);
      final renderer = BrushRenderer();
      final s = _stroke([
        _p(-1, 1, -1), // top-left of viewport
        _p(1, -1, -1), // bottom-right of viewport
        _p(0, 0, -1), // center
      ]);
      final out = renderer.projectStroke(s, cam);
      expect(out.length, 3);
      // World (-1, 1) → (0, 0) top-left.
      expect(out[0], const Offset(0, 0));
      // World (1, -1) → (800, 600) bottom-right.
      expect(out[1], const Offset(800, 600));
      // World (0, 0) → (400, 300) center.
      expect(out[2], const Offset(400, 300));
    });

    test('culls points behind the camera (clipW <= 0)', () {
      final cam = _orthoCam(800, 600);
      final renderer = BrushRenderer();
      final s = _stroke([
        _p(0, 0, -1), // in front (kept)
        _p(0, 0, 1), // behind camera (culled)
        _p(0, 0, -2), // in front (kept)
      ]);
      final out = renderer.projectStroke(s, cam);
      expect(out.length, 2);
      // First and third points kept.
      expect(out[0], const Offset(400, 300));
      expect(out[1], const Offset(400, 300));
    });

    test('respects stroke transform (projects world positions, not local)', () {
      final cam = _orthoCam(800, 600);
      final renderer = BrushRenderer();
      // Local points (0, 0, -1) and (1, 0, -1), translated by (1, 0, 0).
      // World positions: (1, 0, -1) and (2, 0, -1).
      final s = Stroke(
        id: 1,
        points: [
          _p(0, 0, -1),
          _p(1, 0, -1),
        ],
        transform: Matrix4.identity()..setTranslation(Vector3(1, 0, 0)),
      );
      final out = renderer.projectStroke(s, cam);
      expect(out.length, 2);
      // World (1, 0, -1) → ((1*0.5+0.5)*800, (1-(0*0.5+0.5))*600) = (600, 300).
      expect(out[0], const Offset(600, 300));
      // World (2, 0, -1) → ((2*0.5+0.5)*800, 300) = (800, 300).
      expect(out[1], const Offset(800, 300));
    });

    test('all points behind camera → empty list', () {
      final cam = _orthoCam(800, 600);
      final renderer = BrushRenderer();
      final s = _stroke([
        _p(0, 0, 1),
        _p(1, 0, 1),
      ]);
      final out = renderer.projectStroke(s, cam);
      expect(out, isEmpty);
    });
  });

  group('BrushRenderer.sortBackToFront', () {
    test('sorts segments ascending by depth (far first)', () {
      // RenderedSegment.depth is negative (farther = more negative).
      // sortBackToFront sorts ascending, so the most-negative depth
      // (farthest) comes first.
      final segments = [
        RenderedSegment(
            a0: Offset.zero,
            a1: Offset.zero,
            b0: Offset.zero,
            b1: Offset.zero,
            color: const Color(0xFFFFFFFF),
            depth: -5, // nearest
            additive: false),
        RenderedSegment(
            a0: Offset.zero,
            a1: Offset.zero,
            b0: Offset.zero,
            b1: Offset.zero,
            color: const Color(0xFFFFFFFF),
            depth: -10, // farthest
            additive: false),
        RenderedSegment(
            a0: Offset.zero,
            a1: Offset.zero,
            b0: Offset.zero,
            b1: Offset.zero,
            color: const Color(0xFFFFFFFF),
            depth: -7, // middle
            additive: false),
      ];
      final sorted = BrushRenderer.sortBackToFront(segments);
      expect(sorted.map((s) => s.depth).toList(), [-10, -7, -5]);
    });
  });
}
