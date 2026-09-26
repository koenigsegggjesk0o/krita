// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// aabb_test.dart — unit tests for the immutable Aabb type.
//
// Covers construction (from points / center+extent / empty), containment,
// box-box intersection, sphere-box intersection, expand/merge, the slab
// ray intersection, and transform re-fit.

import 'package:feather_krita/core/math/aabb.dart';
import 'package:feather_krita/core/math/mat4.dart';
import 'package:feather_krita/core/math/ray.dart';
import 'package:feather_krita/core/math/vec3.dart';
import 'package:flutter_test/flutter_test.dart';

const double _eps = 1e-9;

void expectVec(Vec3 actual, Vec3 expected, {double tol = _eps}) {
  expect(actual.x, closeTo(expected.x, tol));
  expect(actual.y, closeTo(expected.y, tol));
  expect(actual.z, closeTo(expected.z, tol));
}

void main() {
  group('Aabb construction', () {
    test('fromPoints tight-fits the input', () {
      final b = Aabb.fromPoints([
        const Vec3(-1, -2, -3),
        const Vec3(1, 2, 3),
        const Vec3(0, 0, 0),
      ]);
      expectVec(b.min, const Vec3(-1, -2, -3));
      expectVec(b.max, const Vec3(1, 2, 3));
      expectVec(b.center, const Vec3.zero());
      expectVec(b.size, const Vec3(2, 4, 6));
    });

    test('fromCenterExtent places center and half-extents correctly', () {
      final b = Aabb.fromCenterExtent(const Vec3(5, 0, 0), const Vec3(2, 4, 6));
      expectVec(b.center, const Vec3(5, 0, 0));
      expectVec(b.min, const Vec3(4, -2, -3));
      expectVec(b.max, const Vec3(6, 2, 3));
    });

    test('empty box reports isEmpty and ignores containment', () {
      const empty = Aabb.empty();
      expect(empty.isEmpty, isTrue);
      expect(empty.contains(const Vec3.zero()), isFalse);
    });
  });

  group('Aabb containment and intersection', () {
    test('contains is inclusive on every face', () {
      const b = Aabb(Vec3.zero(), Vec3(1, 1, 1));
      expect(b.contains(const Vec3.zero()), isTrue);
      expect(b.contains(const Vec3(1, 1, 1)), isTrue);
      expect(b.contains(const Vec3(0.5, 0.5, 0.5)), isTrue);
      expect(b.contains(const Vec3(1.0001, 0, 0)), isFalse);
      expect(b.contains(const Vec3(-0.0001, 0, 0)), isFalse);
    });

    test('intersects requires overlap on every axis', () {
      const a = Aabb(Vec3.zero(), Vec3(2, 2, 2));
      const overlapping = Aabb(Vec3(1, 1, 1), Vec3(3, 3, 3));
      const disjoint = Aabb(Vec3(3, 3, 3), Vec3(4, 4, 4));
      const touching = Aabb(Vec3(2, 0, 0), Vec3(4, 2, 2));
      expect(a.intersects(overlapping), isTrue);
      expect(a.intersects(disjoint), isFalse);
      // Touching on a face counts as overlap (boundary inclusive).
      expect(a.intersects(touching), isTrue);
    });

    test('intersectsSphere uses closest-point distance', () {
      const b = Aabb(Vec3.zero(), Vec3(2, 2, 2));
      // Sphere centred at a corner, radius slightly larger than zero → hit.
      expect(b.intersectsSphere(const Vec3(2, 2, 2), 0.5), isTrue);
      // Sphere far away from the box → miss.
      expect(b.intersectsSphere(const Vec3(10, 10, 10), 0.5), isFalse);
      // Sphere centred inside the box → always hit.
      expect(b.intersectsSphere(const Vec3(1, 1, 1), 0.0), isTrue);
    });

    test('containsBox is true only when other is fully inside', () {
      const outer = Aabb(Vec3.zero(), Vec3(10, 10, 10));
      const inner = Aabb(Vec3(2, 2, 2), Vec3(8, 8, 8));
      const straddler = Aabb(Vec3(8, 8, 8), Vec3(12, 12, 12));
      expect(outer.containsBox(inner), isTrue);
      expect(outer.containsBox(straddler), isFalse);
    });
  });

  group('Aabb expand and merge', () {
    test('expand grows to include the new point', () {
      const b = Aabb(Vec3.zero(), Vec3(1, 1, 1));
      final grown = b.expand(const Vec3(3, 0, 0));
      expectVec(grown.min, const Vec3.zero());
      expectVec(grown.max, const Vec3(3, 1, 1));
      // Original is unchanged (immutable).
      expectVec(b.max, const Vec3(1, 1, 1));
    });

    test('merge returns the union of two boxes', () {
      const a = Aabb(Vec3.zero(), Vec3(1, 1, 1));
      const b = Aabb(Vec3(2, 2, 2), Vec3(3, 3, 3));
      final u = a.merge(b);
      expectVec(u.min, const Vec3.zero());
      expectVec(u.max, const Vec3(3, 3, 3));
    });
  });

  group('Aabb ray intersection (slab method)', () {
    test('axis-aligned ray hits the near face', () {
      const b = Aabb(Vec3(-1, -1, -1), Vec3(1, 1, 1));
      final ray = Ray.normalized(const Vec3(0, 0, -5), const Vec3(0, 0, 1));
      final t = b.intersectRay(ray);
      expect(t, isNotNull);
      expect(t!, closeTo(4.0, 1e-6));
      expectVec(ray.pointAt(t), const Vec3(0, 0, -1));
    });

    test('parallel ray outside the slab misses', () {
      const b = Aabb(Vec3(-1, -1, -1), Vec3(1, 1, 1));
      // Ray runs along +X at y=5, never crossing the box.
      final ray = Ray.normalized(const Vec3(-5, 5, 0), const Vec3(1, 0, 0));
      expect(b.intersectRay(ray), isNull);
    });

    test('ray starting inside the box hits a far face', () {
      const b = Aabb(Vec3(-1, -1, -1), Vec3(1, 1, 1));
      final ray = Ray.normalized(const Vec3.zero(), const Vec3(0, 0, 1));
      final t = b.intersectRay(ray);
      expect(t, isNotNull);
      expect(t!, closeTo(1.0, 1e-6));
    });

    test('ray pointing away from the box misses', () {
      const b = Aabb(Vec3(-1, -1, -1), Vec3(1, 1, 1));
      final ray = Ray.normalized(const Vec3(0, 0, 5), const Vec3(0, 0, 1));
      expect(b.intersectRay(ray), isNull);
    });
  });

  group('Aabb transformed', () {
    test('translation moves the corners', () {
      const b = Aabb(Vec3.zero(), Vec3(1, 1, 1));
      final t = Mat4.makeTranslation(10, 20, 30);
      final moved = b.transformed(t);
      expectVec(moved.min, const Vec3(10, 20, 30));
      expectVec(moved.max, const Vec3(11, 21, 31));
    });

    test('uniform scale grows the extents', () {
      const b = Aabb(Vec3(-1, -1, -1), Vec3(1, 1, 1));
      final t = Mat4.makeScale(2.0, 2.0, 2.0);
      final grown = b.transformed(t);
      expectVec(grown.min, const Vec3(-2, -2, -2));
      expectVec(grown.max, const Vec3(2, 2, 2));
    });
  });

  group('Aabb equality', () {
    test('value semantics', () {
      const a = Aabb(Vec3.zero(), Vec3(1, 1, 1));
      const b = Aabb(Vec3.zero(), Vec3(1, 1, 1));
      expect(a, b);
      expect(a.hashCode, b.hashCode);
    });
  });
}
