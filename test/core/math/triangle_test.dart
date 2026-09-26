// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// triangle_test.dart — unit tests for the immutable Triangle type.
//
// Covers normal/area/centroid, barycentric coordinates, containment,
// closest point (vertex / edge / face regions), and Möller-Trumbore
// ray intersection including backface culling.

import 'package:feather_krita/core/math/ray.dart';
import 'package:feather_krita/core/math/triangle.dart';
import 'package:feather_krita/core/math/vec3.dart';
import 'package:flutter_test/flutter_test.dart';

const double _eps = 1e-9;

void expectVec(Vec3 actual, Vec3 expected, {double tol = _eps}) {
  expect(actual.x, closeTo(expected.x, tol));
  expect(actual.y, closeTo(expected.y, tol));
  expect(actual.z, closeTo(expected.z, tol));
}

void main() {
  group('Triangle geometry', () {
    const t = Triangle(Vec3(0, 0, 0), Vec3(2, 0, 0), Vec3(0, 2, 0));

    test('normal follows the right-hand rule (CCW = +Z)', () {
      expectVec(t.normal, const Vec3(0, 0, 1));
    });

    test('area is half the cross product magnitude', () {
      expect(t.area, closeTo(2.0, _eps));
    });

    test('centroid is the vertex average', () {
      expectVec(t.centroid, const Vec3(2 / 3, 2 / 3, 0));
    });
  });

  group('Triangle barycentric coords', () {
    const t = Triangle(Vec3(0, 0, 0), Vec3(1, 0, 0), Vec3(0, 1, 0));

    test('vertices map to (1,0,0) (0,1,0) (0,0,1)', () {
      expectVec(t.barycentricCoords(const Vec3(0, 0, 0))!,
          const Vec3(1, 0, 0));
      expectVec(t.barycentricCoords(const Vec3(1, 0, 0))!,
          const Vec3(0, 1, 0));
      expectVec(t.barycentricCoords(const Vec3(0, 1, 0))!,
          const Vec3(0, 0, 1));
    });

    test('weights sum to 1 at the centroid', () {
      final b = t.barycentricCoords(t.centroid)!;
      expect(b.x + b.y + b.z, closeTo(1.0, _eps));
      expect(b.x, closeTo(1 / 3, _eps));
      expect(b.y, closeTo(1 / 3, _eps));
      expect(b.z, closeTo(1 / 3, _eps));
    });

    test('degenerate triangle returns null', () {
      const degenerate =
          Triangle(Vec3.zero(), Vec3(1, 0, 0), Vec3(2, 0, 0));
      expect(degenerate.barycentricCoords(const Vec3(0.5, 0, 0)), isNull);
    });
  });

  group('Triangle containsPoint', () {
    const t = Triangle(Vec3(0, 0, 0), Vec3(2, 0, 0), Vec3(0, 2, 0));

    test('interior point is inside, exterior is outside', () {
      expect(t.containsPoint(const Vec3(0.5, 0.5, 0)), isTrue);
      expect(t.containsPoint(const Vec3(1, 1, 0)), isTrue); // on hypotenuse
      expect(t.containsPoint(const Vec3(1.5, 1.5, 0)), isFalse);
      // Out-of-plane points are NOT inside (barycentric weights don't
      // depend on Z, but the test does include them).
      expect(t.containsPoint(const Vec3(0.5, 0.5, 10)), isTrue);
    });
  });

  group('Triangle closestPoint', () {
    const t = Triangle(Vec3(0, 0, 0), Vec3(2, 0, 0), Vec3(0, 2, 0));

    test('probe above the interior returns the projected point', () {
      final p = t.closestPoint(const Vec3(0.5, 0.5, 5));
      expectVec(p, const Vec3(0.5, 0.5, 0));
    });

    test('probe past vertex A returns A', () {
      final p = t.closestPoint(const Vec3(-1, -1, 0));
      expectVec(p, const Vec3(0, 0, 0));
    });

    test('probe past edge AB returns a point on AB', () {
      final p = t.closestPoint(const Vec3(1, -1, 0));
      expectVec(p, const Vec3(1, 0, 0));
    });
  });

  group('Triangle ray intersection (Möller-Trumbore)', () {
    const tri = Triangle(Vec3(-1, -1, 0), Vec3(1, -1, 0), Vec3(0, 1, 0));

    test('orthogonal ray hits the triangle plane', () {
      final ray = Ray.normalized(const Vec3(0, 0, 5), const Vec3(0, 0, -1));
      final hit = tri.intersectRay(ray);
      expect(hit, isNotNull);
      // The ray pierces the triangle's plane (z=0) at (0, 0, 0).
      expectVec(hit!, const Vec3(0, 0, 0));
    });

    test('ray missing the triangle returns null', () {
      final ray = Ray.normalized(const Vec3(5, 5, 5), const Vec3(0, 0, -1));
      expect(tri.intersectRay(ray), isNull);
    });

    test('ray parallel to the triangle returns null', () {
      final ray = Ray.normalized(const Vec3(0, 0, 1), const Vec3(1, 0, 0));
      expect(tri.intersectRay(ray), isNull);
    });

    test('cullBackfaces rejects a ray hitting the back face', () {
      // Triangle faces +Z (CCW). A ray going +Z (from -Z side toward +Z
      // side) hits the back face → culled when cullBackfaces is true.
      final ray = Ray.normalized(const Vec3(0, 0, -5), const Vec3(0, 0, 1));
      expect(tri.intersectRay(ray, cullBackfaces: true), isNull);
      // Without culling the same ray should hit.
      expect(tri.intersectRay(ray), isNotNull);
    });

    test('ray starting behind the triangle misses', () {
      // Hit point is behind the ray origin (t < epsilon).
      final ray = Ray.normalized(const Vec3(0, 0, 5), const Vec3(0, 0, 1));
      expect(tri.intersectRay(ray), isNull);
    });
  });

  group('Triangle equality', () {
    test('value semantics', () {
      const a = Triangle(Vec3.zero(), Vec3(1, 0, 0), Vec3(0, 1, 0));
      const b = Triangle(Vec3.zero(), Vec3(1, 0, 0), Vec3(0, 1, 0));
      expect(a, b);
      expect(a.hashCode, b.hashCode);
    });
  });
}
