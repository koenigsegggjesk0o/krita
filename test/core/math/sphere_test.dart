// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// sphere_test.dart — unit tests for the immutable Sphere type.

import 'package:feather_krita/core/math/aabb.dart';
import 'package:feather_krita/core/math/mat4.dart';
import 'package:feather_krita/core/math/sphere.dart';
import 'package:feather_krita/core/math/vec3.dart';
import 'package:flutter_test/flutter_test.dart';

const double _eps = 1e-9;

void expectVec(Vec3 actual, Vec3 expected, {double tol = _eps}) {
  expect(actual.x, closeTo(expected.x, tol));
  expect(actual.y, closeTo(expected.y, tol));
  expect(actual.z, closeTo(expected.z, tol));
}

void main() {
  group('Sphere construction', () {
    test('fromPoints centres on the average and reaches the farthest', () {
      final s = Sphere.fromPoints([
        const Vec3(-1, 0, 0),
        const Vec3(1, 0, 0),
        const Vec3(0, 0, 0),
      ]);
      expectVec(s.center, const Vec3.zero());
      expect(s.radius, closeTo(1.0, _eps));
    });

    test('fromPoints on empty input returns the zero sphere', () {
      final s = Sphere.fromPoints(const <Vec3>[]);
      expectVec(s.center, const Vec3.zero());
      expect(s.radius, 0.0);
    });

    test('empty constructor is zero', () {
      const s = Sphere.empty();
      expectVec(s.center, const Vec3.zero());
      expect(s.radius, 0.0);
    });
  });

  group('Sphere containment', () {
    const s = Sphere(Vec3.zero(), 2.0);

    test('contains respects the radius inclusive', () {
      expect(s.contains(const Vec3(0, 0, 0)), isTrue);
      expect(s.contains(const Vec3(2, 0, 0)), isTrue);
      expect(s.contains(const Vec3(0, 1.5, 0)), isTrue);
      expect(s.contains(const Vec3(2.0001, 0, 0)), isFalse);
    });
  });

  group('Sphere-sphere intersection', () {
    test('overlapping spheres intersect', () {
      const a = Sphere(Vec3.zero(), 1.0);
      const b = Sphere(Vec3(0.5, 0, 0), 1.0);
      expect(a.intersects(b), isTrue);
    });

    test('disjoint spheres do not intersect', () {
      const a = Sphere(Vec3.zero(), 1.0);
      const b = Sphere(Vec3(5, 0, 0), 1.0);
      expect(a.intersects(b), isFalse);
    });

    test('tangent spheres (touching) intersect', () {
      const a = Sphere(Vec3.zero(), 1.0);
      const b = Sphere(Vec3(2, 0, 0), 1.0);
      expect(a.intersects(b), isTrue);
    });
  });

  group('Sphere-AABB intersection', () {
    test('sphere centred inside the box hits', () {
      const box = Aabb(Vec3(-1, -1, -1), Vec3(1, 1, 1));
      const s = Sphere(Vec3.zero(), 0.5);
      expect(s.intersectsAabb(box), isTrue);
    });

    test('sphere far from the box misses', () {
      const box = Aabb(Vec3(-1, -1, -1), Vec3(1, 1, 1));
      const s = Sphere(Vec3(10, 10, 10), 0.5);
      expect(s.intersectsAabb(box), isFalse);
    });

    test('sphere grazing a corner hits', () {
      const box = Aabb(Vec3(-1, -1, -1), Vec3(1, 1, 1));
      const s = Sphere(Vec3(2, 0, 0), 1.0);
      expect(s.intersectsAabb(box), isTrue);
    });
  });

  group('Sphere merge', () {
    test('merge returns the tight enclosure of two spheres', () {
      const a = Sphere(Vec3(-1, 0, 0), 0.5);
      const b = Sphere(Vec3(1, 0, 0), 0.5);
      final u = a.merge(b);
      expectVec(u.center, const Vec3.zero());
      expect(u.radius, closeTo(1.5, _eps));
    });

    test('merge with a contained sphere returns the outer', () {
      const outer = Sphere(Vec3.zero(), 5.0);
      const inner = Sphere(Vec3.zero(), 1.0);
      expect(outer.merge(inner), same(outer));
      expect(inner.merge(outer), same(outer));
    });
  });

  group('Sphere transformed', () {
    test('translation moves the centre, keeps the radius', () {
      const s = Sphere(Vec3.zero(), 2.0);
      final t = Mat4.makeTranslation(5, 0, 0);
      final moved = s.transformed(t);
      expectVec(moved.center, const Vec3(5, 0, 0));
      expect(moved.radius, closeTo(2.0, _eps));
    });

    test('uniform scale scales the radius by the largest axis', () {
      const s = Sphere(Vec3.zero(), 1.0);
      final t = Mat4.makeScale(2.0, 3.0, 4.0);
      final grown = s.transformed(t);
      // Largest axis scale = 4.
      expect(grown.radius, closeTo(4.0, _eps));
    });
  });

  group('Sphere equality', () {
    test('value semantics', () {
      const a = Sphere(Vec3.zero(), 1.0);
      const b = Sphere(Vec3.zero(), 1.0);
      expect(a, b);
      expect(a.hashCode, b.hashCode);
    });
  });
}
