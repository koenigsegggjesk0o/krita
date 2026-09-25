// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// vec3_test.dart — unit tests for the immutable 3D vector type.

import 'dart:math' as math;

import 'package:feather_krita/core/math/vec3.dart';
import 'package:flutter_test/flutter_test.dart';

const double _eps = 1e-9;

void expectVec(Vec3 actual, Vec3 expected, {double tol = _eps}) {
  expect(actual.x, closeTo(expected.x, tol));
  expect(actual.y, closeTo(expected.y, tol));
  expect(actual.z, closeTo(expected.z, tol));
}

void main() {
  group('Vec3 construction', () {
    test('default constructor stores components', () {
      const v = Vec3(1, 2, 3);
      expect(v.x, 1);
      expect(v.y, 2);
      expect(v.z, 3);
    });

    test('named constructors produce expected values', () {
      expect(const Vec3.zero(), const Vec3(0, 0, 0));
      expect(const Vec3.unitX(), const Vec3(1, 0, 0));
      expect(const Vec3.unitY(), const Vec3(0, 1, 0));
      expect(const Vec3.unitZ(), const Vec3(0, 0, 1));
      expect(const Vec3.all(7), const Vec3(7, 7, 7));
      expect(const Vec3.one(), const Vec3(1, 1, 1));
    });

    test('fromList copies the first three entries', () {
      final v = Vec3.fromList([1.5, 2.5, 3.5, 4.5]);
      expect(v, const Vec3(1.5, 2.5, 3.5));
    });
  });

  group('Vec3 arithmetic', () {
    test('add subtract multiply divide', () {
      const a = Vec3(1, 2, 3);
      const b = Vec3(10, 20, 30);
      expect(a + b, const Vec3(11, 22, 33));
      expect(b - a, const Vec3(9, 18, 27));
      expect(a * 2, const Vec3(2, 4, 6));
      expect(b / 10, const Vec3(1, 2, 3));
      expect(-a, const Vec3(-1, -2, -3));
    });

    test('Hadamard product and component-wise divide', () {
      const a = Vec3(2, 3, 4);
      const b = Vec3(5, 6, 7);
      expect(a.multiply(b), const Vec3(10, 18, 28));
      expect(b.divide(a), const Vec3(2.5, 2.0, 7.0 / 4.0));
    });

    test('equality and hashCode obey value semantics', () {
      const a = Vec3(1, 2, 3);
      const b = Vec3(1, 2, 3);
      expect(a == b, isTrue);
      expect(a.hashCode, b.hashCode);
      expect(const Vec3(1, 2, 3) == const Vec3(3, 2, 1), isFalse);
    });
  });

  group('Vec3 geometry', () {
    test('dot product is symmetric and bilinear', () {
      const a = Vec3(1, 2, 3);
      const b = Vec3(4, 5, 6);
      expect(a.dot(b), 32);
      expect(b.dot(a), 32);
      expect(a.dot(const Vec3.zero()), 0);
    });

    test('cross product follows the right-hand rule', () {
      expect(const Vec3.unitX().cross(const Vec3.unitY()),
          const Vec3.unitZ());
      expect(const Vec3.unitY().cross(const Vec3.unitZ()),
          const Vec3.unitX());
      expect(const Vec3.unitZ().cross(const Vec3.unitX()),
          const Vec3.unitY());
      // Anti-commutativity.
      final ab = const Vec3(1, 2, 3).cross(const Vec3(4, 5, 6));
      final ba = const Vec3(4, 5, 6).cross(const Vec3(1, 2, 3));
      expect(ab, -ba);
    });

    test('length and length2 are consistent', () {
      const v = Vec3(3, 4, 0);
      expect(v.length2, 25);
      expect(v.length, 5);
    });

    test('distanceTo and distanceToSquared', () {
      const a = Vec3(0, 0, 0);
      const b = Vec3(3, 4, 0);
      expect(a.distanceToSquared(b), 25);
      expect(a.distanceTo(b), 5);
    });

    test('normalized returns a unit vector or zero', () {
      const v = Vec3(0, 3, 4);
      final n = v.normalized();
      expect(n.length, closeTo(1.0, _eps));
      expectVec(n, const Vec3(0, 0.6, 0.8));
      expect(const Vec3.zero().normalized(), const Vec3.zero());
    });

    test('lerp interpolates endpoints and midpoint', () {
      const a = Vec3(0, 0, 0);
      const b = Vec3(10, 20, 30);
      expect(a.lerp(b, 0), a);
      expect(a.lerp(b, 1), b);
      expect(a.lerp(b, 0.5), const Vec3(5, 10, 15));
    });

    test('angleTo returns 0 for parallel, pi/2 for perpendicular', () {
      expect(const Vec3.unitX().angleTo(const Vec3.unitX()),
          closeTo(0, _eps));
      expect(const Vec3.unitX().angleTo(const Vec3.unitY()),
          closeTo(math.pi / 2, 1e-9));
      expect(const Vec3.unitX().angleTo(const Vec3(-1, 0, 0)),
          closeTo(math.pi, 1e-9));
    });

    test('projectOnto drops a vector onto another', () {
      const a = Vec3(1, 2, 3);
      const axis = Vec3.unitX();
      final p = a.projectOnto(axis);
      expectVec(p, const Vec3(1, 0, 0));
      // Projecting onto zero is zero.
      expect(a.projectOnto(const Vec3.zero()), const Vec3.zero());
    });

    test('reflect mirrors a vector about a unit normal', () {
      // A vector hitting a flat floor (normal = +Y) reflects Y.
      const v = Vec3(1, -1, 0);
      final r = v.reflect(const Vec3.unitY());
      expectVec(r, const Vec3(1, 1, 0));
      // Pure vertical drop flips entirely.
      final r2 = const Vec3(0, -2, 0).reflect(const Vec3.unitY());
      expectVec(r2, const Vec3(0, 2, 0));
    });

    test('rotateAround rotates by an explicit angle around an axis', () {
      // +X rotated +90° around +Z becomes +Y.
      final r = const Vec3.unitX()
          .rotateAround(const Vec3.unitZ(), math.pi / 2);
      expectVec(r, const Vec3.unitY());
      // Rotating around an axis parallel to the vector is a no-op.
      final noOp = const Vec3(2, 0, 0)
          .rotateAround(const Vec3.unitX(), math.pi / 4);
      expectVec(noOp, const Vec3(2, 0, 0));
    });

    test('abs, min, max, copyWith behave component-wise', () {
      const a = Vec3(-1, 2, -3);
      expect(a.abs(), const Vec3(1, 2, 3));
      expect(const Vec3(1, 5, 3).min(const Vec3(4, 2, 6)),
          const Vec3(1, 2, 3));
      expect(const Vec3(1, 5, 3).max(const Vec3(4, 2, 6)),
          const Vec3(4, 5, 6));
      expect(a.copyWith(y: 99), const Vec3(-1, 99, -3));
    });

    test('xy drops the z component and toList round-trips', () {
      const v = Vec3(7, 8, 9);
      expect(v.xy.x, 7);
      expect(v.xy.y, 8);
      expect(v.toList(), [7, 8, 9]);
    });
  });
}
