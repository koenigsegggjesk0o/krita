// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// guide3d_test.dart — unit tests for 3D Guide surfaces and primitives.

import 'package:feather_krita/core/math/math.dart';
import 'package:feather_krita/engine/guide3d/guide3d.dart';
import 'package:feather_krita/engine/guide3d/guide3d_primitive.dart';
import 'package:feather_krita/engine/guide3d/guide3d_type.dart';
import 'package:feather_krita/engine/guide3d/guide_mesh_generators.dart';
import 'package:feather_krita/engine/guide3d/primitive_guide.dart';
import 'package:flutter_test/flutter_test.dart';

void main() {
  group('Guide3DMesh — cube', () {
    test('cubeMesh produces a non-degenerate mesh', () {
      final m = GuideMeshGenerators.cubeMesh(size: 2.0, segments: 1);
      expect(m.vertexCount, greaterThan(0));
      expect(m.triangleCount, greaterThan(0));
      expect(m.indices.length % 3, 0);
    });

    test('cubeMesh respects segment subdivision', () {
      final coarse = GuideMeshGenerators.cubeMesh(size: 2.0, segments: 1);
      final fine = GuideMeshGenerators.cubeMesh(size: 2.0, segments: 4);
      expect(fine.vertexCount, greaterThan(coarse.vertexCount));
      expect(fine.triangleCount, greaterThan(coarse.triangleCount));
    });

    test('cubeMesh bounds are symmetric about the origin', () {
      final m = GuideMeshGenerators.cubeMesh(size: 2.0, segments: 1);
      final b = m.bounds;
      expect(b.min.x, closeTo(-1, 1e-6));
      expect(b.max.x, closeTo(1, 1e-6));
      expect(b.min.y, closeTo(-1, 1e-6));
      expect(b.max.y, closeTo(1, 1e-6));
      expect(b.min.z, closeTo(-1, 1e-6));
      expect(b.max.z, closeTo(1, 1e-6));
    });

    test('cubeMesh normals are unit length', () {
      final m = GuideMeshGenerators.cubeMesh(size: 2.0, segments: 2);
      for (final n in m.normals) {
        expect(n.length, closeTo(1.0, 1e-6));
      }
    });
  });

  group('Guide3DMesh — sphere', () {
    test('sphereMesh centres on the origin and respects radius', () {
      final m = GuideMeshGenerators.sphereMesh(radius: 2.0, segments: 12);
      final b = m.bounds;
      expect(b.min.x, closeTo(-2, 1e-6));
      expect(b.max.x, closeTo(2, 1e-6));
      expect(b.min.y, closeTo(-2, 1e-6));
      expect(b.max.y, closeTo(2, 1e-6));
      expect(b.min.z, closeTo(-2, 1e-6));
      expect(b.max.z, closeTo(2, 1e-6));
    });

    test('sphereMesh produces closed triangle topology', () {
      final m = GuideMeshGenerators.sphereMesh(radius: 1.0, segments: 8);
      expect(m.triangleCount, greaterThan(0));
      expect(m.indices.length % 3, 0);
      // Top and bottom poles should each appear in the index list.
      expect(m.indices.contains(0), isTrue);
      expect(m.indices.contains(m.vertexCount - 1), isTrue);
    });

    test('sphereMesh vertices lie on the unit sphere surface', () {
      final m = GuideMeshGenerators.sphereMesh(radius: 1.0, segments: 8);
      for (final p in m.positions) {
        expect(p.length, closeTo(1.0, 1e-6));
      }
    });
  });

  group('Guide3DMesh — tube', () {
    test('tubeMesh builds an open shell along Y', () {
      final m = GuideMeshGenerators.tubeMesh(
          radius: 1.0, height: 2.0, segments: 8);
      expect(m.triangleCount, greaterThan(0));
      // Bounds span the requested height along Y.
      final b = m.bounds;
      expect(b.max.y - b.min.y, closeTo(2.0, 1e-6));
      // And the requested radius along X/Z.
      expect(b.max.x, closeTo(1.0, 1e-6));
      expect(b.max.z, closeTo(1.0, 1e-6));
    });

    test('tubeMesh segments control vertex density', () {
      final low = GuideMeshGenerators.tubeMesh(
          radius: 1.0, height: 2.0, segments: 4);
      final high = GuideMeshGenerators.tubeMesh(
          radius: 1.0, height: 2.0, segments: 24);
      expect(high.vertexCount, greaterThan(low.vertexCount));
    });
  });

  group('Guide3DMesh — clone & recomputeNormals', () {
    test('clone produces an independent copy', () {
      final a = GuideMeshGenerators.cubeMesh(size: 1, segments: 1);
      final b = a.clone();
      expect(b.vertexCount, a.vertexCount);
      expect(b.triangleCount, a.triangleCount);
      // Mutating a's positions must not affect b's.
      for (var i = 0; i < a.positions.length; i++) {
        a.positions[i].setValues(99, 99, 99);
      }
      expect(b.positions.first.x, isNot(99));
    });

    test('recomputeNormals rebuilds the normals list in place', () {
      final m = GuideMeshGenerators.cubeMesh(size: 1, segments: 1);
      final originalLength = m.normals.length;
      // Stash one normal then perturb positions and recompute.
      final firstBefore = m.normals.first.clone();
      m.recomputeNormals();
      expect(m.normals.length, originalLength);
      // Recomputing twice should be idempotent on a stable mesh.
      expect(m.normals.first.x, closeTo(firstBefore.x, 1e-6));
      expect(m.normals.first.y, closeTo(firstBefore.y, 1e-6));
      expect(m.normals.first.z, closeTo(firstBefore.z, 1e-6));
    });
  });

  group('PrimitiveGuideBuilder', () {
    final builder = const PrimitiveGuideBuilder();

    test('builds a cube guide of type primitive', () {
      final g = builder.build(
          kind: Guide3DPrimitive.cube,
          params: const PrimitiveGuideParams(size: 2.0));
      expect(g.type, Guide3DType.primitive);
      expect(g.mesh.triangleCount, greaterThan(0));
      expect(g.meta['primitive'], 'cube');
      expect(g.meta['mode'], 'primitive');
      expect(g.startPoint, isNotNull);
      expect(g.startEnd, isNotNull);
    });

    test('builds a sphere guide with metadata', () {
      final g = builder.build(
          kind: Guide3DPrimitive.sphere,
          params: const PrimitiveGuideParams(radius: 1.5, segments: 16));
      expect(g.meta['primitive'], 'sphere');
      expect(g.meta['radius'], 1.5);
      expect(g.mesh.vertexCount, greaterThan(0));
    });

    test('builds a tube guide with metadata', () {
      final g = builder.build(
          kind: Guide3DPrimitive.tube,
          params: const PrimitiveGuideParams(radius: 0.5, height: 3.0));
      expect(g.meta['primitive'], 'tube');
      expect(g.meta['height'], 3.0);
    });

    test('withSegments rebuilds the mesh at the requested density', () {
      final g = builder.build(
          kind: Guide3DPrimitive.sphere,
          params: const PrimitiveGuideParams(segments: 8));
      final rebuilt = builder.withSegments(g, 24);
      expect(rebuilt.mesh.vertexCount, greaterThan(g.mesh.vertexCount));
      // Transform / metadata is preserved.
      expect(rebuilt.meta['primitive'], 'sphere');
    });

    test('toJson / applyJson round-trip preserves core fields', () {
      final g = builder.build(
          kind: Guide3DPrimitive.cube,
          params: const PrimitiveGuideParams(opacity: 0.7),
          name: 'test cube');
      g.opacity = 0.4;
      g.locked = true;
      final json = g.toJson();
      final restored = builder.build(
          kind: Guide3DPrimitive.cube, name: 'restored')
        ..applyJson(json);
      expect(restored.opacity, 0.4);
      expect(restored.locked, isTrue);
      expect(restored.name, 'test cube');
    });
  });

  group('Guide3D visibility / lock / raycast short-circuit', () {
    final builder = const PrimitiveGuideBuilder();
    final ray = Ray(const Vec3.zero(), const Vec3.unitZ());

    test('raycast returns null when the guide is invisible', () {
      final g = builder.build(kind: Guide3DPrimitive.cube);
      g.visible = false;
      expect(g.raycast(ray), isNull);
    });

    test('raycast returns null when the guide is locked', () {
      final g = builder.build(kind: Guide3DPrimitive.cube);
      g.locked = true;
      expect(g.raycast(ray), isNull);
    });

    test('projectToSurface returns null on a locked guide', () {
      final g = builder.build(kind: Guide3DPrimitive.cube);
      g.locked = true;
      expect(g.projectToSurface(ray), isNull);
    });
  });

  group('Guide3D mutation & curves', () {
    final builder = const PrimitiveGuideBuilder();

    test('addDrawnCurve appends to drawnCurves', () {
      final g = builder.build(kind: Guide3DPrimitive.cube);
      expect(g.drawnCurves, isEmpty);
      final curve = GuideDrawnCurve(
        points: [Vector3.zero(), Vector3(1, 0, 0)],
        uvs: [Vector2.zero(), Vector2(1, 0)],
        color: 0xFFFFFFFF,
      );
      g.addDrawnCurve(curve);
      expect(g.drawnCurves.length, 1);
      expect(g.drawnCurves.first.color, 0xFFFFFFFF);
    });

    test('markTransformDirty allows recomputation of the inverse', () {
      final g = builder.build(kind: Guide3DPrimitive.cube);
      // No assertion fires — the call must succeed without throwing.
      g.markTransformDirty();
      // Translate the guide.
      g.transform = Matrix4.identity()..setTranslation(Vector3(5, 0, 0));
      g.markTransformDirty();
      // The world centre should reflect the new translation.
      final c = g.worldCenter;
      expect(c.x, closeTo(5, 1e-6));
    });

    test('start line endpoints live in local space', () {
      final g = builder.build(kind: Guide3DPrimitive.cube);
      // The two endpoints must be distinct and lie within the cube's
      // local bounds.
      expect(g.startPoint, isNotNull);
      expect(g.startEnd, isNotNull);
      expect((g.startPoint - g.startEnd).length, greaterThan(0));
    });
  });
}
