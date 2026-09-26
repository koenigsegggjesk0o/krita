// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// guide_renderer_wiring_test.dart — v0.57-B wiring tests for
// guide_renderer.dart.
//
// lib/engine/guide3d/guide_renderer.dart (Guide3DRenderer) was dead code
// before v0.57-B: the renderer could batch a [Guide3D] into GPU-ready
// triangle / line strips, but no runtime caller invoked it. v0.57-B
// wires it into [MainScreen._buildGuideRibbonOverlay]: when the
// GuidePanel's "Show guide ribbon" toggle ([MainScreen._guideRibbon])
// is ON, every active guide is run through [Guide3DRenderer.renderAll]
// and the resulting world-space batches are adapted into screen-space
// [CanvasOverlay] primitives (translucent surface triangles + grid
// line segments + the orange starting line) that the canvas viewport
// draws on top of the strokes.
//
// Pumping the full [MainScreen] widget requires engine + FFI init and
// is out of scope for a unit test (same constraint as v56-B's undo_oom
// test + v0.57-B's guide_snap test). These tests therefore pin the
// wiring contract via @visibleForTesting constants on [MainScreen] and
// exercise the renderer against a real guide geometry built by
// [PrimitiveGuideBuilder] — the same builder the host's GuidePanel →
// insertPrimitive path uses for the "Sphere" / "Cube" shapes.

import 'package:feather_krita/core/math/math.dart';
import 'package:feather_krita/engine/guide3d/guide3d_primitive.dart';
import 'package:feather_krita/engine/guide3d/guide_renderer.dart';
import 'package:feather_krita/engine/guide3d/guide_manager.dart';
import 'package:feather_krita/engine/guide3d/primitive_guide.dart';
import 'package:feather_krita/screens/main_screen.dart';
import 'package:flutter_test/flutter_test.dart';

void main() {
  group('MainScreen guide-ribbon colour constants (v0.57-B wiring contract)',
      () {
    test('kGuideRibbonSurfaceColor is exposed + matches the engine default',
        () {
      // Pin the constant so a future refactor can't silently drift the
      // overlay colour. The host's [_buildGuideRibbonOverlay] stamps this
      // colour onto every surface triangle, so changing it here is the
      // single source of truth for the ribbon's translucent tint.
      expect(MainScreen.kGuideRibbonSurfaceColor, 0xFF8FB8FF);
      // And it must match the engine's own [Guide3DRenderer.defaultSurfaceColor]
      // so the overlay matches what a direct engine draw would produce.
      expect(MainScreen.kGuideRibbonSurfaceColor,
          Guide3DRenderer.defaultSurfaceColor);
    });

    test('kGuideRibbonStartLineColor is exposed + matches the engine default',
        () {
      expect(MainScreen.kGuideRibbonStartLineColor, 0xFFFF8A00);
      expect(MainScreen.kGuideRibbonStartLineColor,
          Guide3DRenderer.orangeStartColor);
    });
  });

  group('Guide3DRenderer.renderAll (engine exercised via the runtime path)',
      () {
    final builder = const PrimitiveGuideBuilder();

    test('batches a sphere guide into surface + grid + start-line batches', () {
      // The host wiring is:
      //   final renderer = Guide3DRenderer();
      //   final renderData = renderer.renderAll(_guides.guides);
      // Verify that composition produces the three batches per guide.
      final sphere = builder.build(
        kind: Guide3DPrimitive.sphere,
        params: const PrimitiveGuideParams(radius: 1.0, segments: 12),
      );
      final renderer = Guide3DRenderer();
      final data = renderer.renderAll([sphere]);

      expect(data.length, 1, reason: 'one guide → one render-data entry');
      final d0 = data.first;
      // Surface: triangle batch with non-empty vertex + index lists.
      expect(d0.surface.mode, GuideBatchMode.triangles);
      expect(d0.surface.vertices.length, greaterThan(0));
      expect(d0.surface.indices.length, greaterThanOrEqualTo(3));
      expect(d0.surface.indices.length % 3, 0,
          reason: 'triangle indices come in triples');
      // Grid: line batch.
      expect(d0.grid.mode, GuideBatchMode.lines);
      expect(d0.grid.indices.length % 2, 0,
          reason: 'line indices come in pairs');
      // Start line: exactly one segment (two indices).
      expect(d0.startLine.mode, GuideBatchMode.lines);
      expect(d0.startLine.indices.length, 2);
    });

    test('skips invisible guides (matches the runtime contract)', () {
      // The host's [_buildCanvasScene] projects every guide; the renderer
      // honours [Guide3D.visible]. Verify an invisible guide yields no
      // render data (renderAll filters it out).
      final invisible = builder.build(
        kind: Guide3DPrimitive.cube,
        params: const PrimitiveGuideParams(size: 2.0),
      );
      invisible.visible = false;
      final renderer = Guide3DRenderer();
      expect(renderer.renderAll([invisible]), isEmpty);
    });

    test('surface vertices are pre-transformed to world space', () {
      // The renderer's doc says "Position is pre-transformed to world
      // space" — verify a translated guide's surface vertices actually
      // carry the translation. This is the contract the host's
      // [_buildGuideRibbonOverlay] relies on: it projects the vertices
      // through the view-projection matrix WITHOUT applying the guide's
      // own transform (the renderer already did).
      final offset = Vector3(5.0, 0.0, 0.0);
      final guide = builder.build(
        kind: Guide3DPrimitive.sphere,
        params: const PrimitiveGuideParams(radius: 1.0, segments: 8),
      );
      guide.transform = Matrix4.translation(offset);
      guide.markTransformDirty();

      final renderer = Guide3DRenderer();
      final data = renderer.renderAll([guide]);
      expect(data, hasLength(1));
      // Every surface vertex should be near the offset (the sphere is
      // centred at the origin in local space, so world-space vertices
      // sit on a sphere of radius 1 around (5, 0, 0)).
      for (final v in data.first.surface.vertices) {
        final d = (v.position - offset).length;
        expect(d, closeTo(1.0, 1e-3),
            reason: 'vertex should be 1 unit from the guide origin');
      }
    });
  });

  group('Guide3DManager + Guide3DRenderer (runtime composition)', () {
    test('manager.guides feeds renderAll the same way the host wires it', () {
      // The host wiring is:
      //   final renderData = renderer.renderAll(_guides.guides);
      // Verify that composition works end-to-end with a real manager.
      final manager = Guide3DManager();
      manager.add(const PrimitiveGuideBuilder().build(
        kind: Guide3DPrimitive.cube,
        params: const PrimitiveGuideParams(size: 2.0),
      ));
      manager.add(const PrimitiveGuideBuilder().build(
        kind: Guide3DPrimitive.sphere,
        params: const PrimitiveGuideParams(radius: 1.5, segments: 12),
      ));

      final renderer = Guide3DRenderer();
      final data = renderer.renderAll(manager.guides);
      expect(data, hasLength(2),
          reason: 'two active guides → two render-data entries');

      // Clearing the manager empties the render output (matches the
      // host's empty-guides short-circuit).
      manager.clear();
      expect(renderer.renderAll(manager.guides), isEmpty);
    });
  });
}
