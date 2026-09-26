// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// plane_test.dart — unit tests for the immutable Plane type.
//
// Covers construction (point+normal, three-points), side-of-plane
// predicates, distance, ray/segment intersection, and transform.

import 'dart:math' as math;

import 'package:feather_krita/core/math/mat4.dart';
import 'package:feather_krita/core/math/plane.dart';
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
  group('Plane construction', () {
    test('fromPointNormal stores a unit normal and the correct distance', () {
      final p = Plane.fromPointNormal(
          const Vec3(1, 2, 3), const Vec3(0, 2, 0));
      expectVec(p.normal, const Vec3(0, 1, 0));
      expect(p.distance, closeTo(2.0, _eps));
    });

    test('fromThreePoints follows the right-hand rule', () {
      // CCW triangle in the XY plane → normal = +Z.
      final p = Plane.fromThreePoints(
        const Vec3(0, 0, 0),
        const Vec3(1, 0, 0),
        const Vec3(0, 1, 0),
      );
      expectVec(p.normal, const Vec3(0, 0, 1));
      expect(p.distance, closeTo(0.0, _eps));
    });

    test('degenerate three-points input returns a zero-normal plane', () {
      // All three points collinear → cross product is zero → normalized() = zero.
      final p = Plane.fromThreePoints(
        const Vec3(0, 0, 0),
        const Vec3(1, 0, 0),
        const Vec3(2, 0, 0),
      );
      expectVec(p.normal, const Vec3.zero());
    });
  });

  group('Plane side predicates', () {
    const plane = Plane(Vec3(0, 1, 0), 5.0); // y == 5

    test('pointInFront / pointBehind / containsPoint', () {
      expect(plane.pointInFront(const Vec3(0, 6, 0)), isTrue);
      expect(plane.pointBehind(const Vec3(0, 4, 0)), isTrue);
      expect(plane.containsPoint(const Vec3(0, 5, 0)), isTrue);
      expect(plane.containsPoint(const Vec3(0, 5.0001, 0)), isFalse);
    });

    test('distanceToPoint is signed', () {
      expect(plane.distanceToPoint(const Vec3(0, 10, 0)), closeTo(5.0, _eps));
      expect(plane.distanceToPoint(const Vec3(0, 0, 0)), closeTo(-5.0, _eps));
      expect(plane.distanceToPoint(const Vec3(0, 5, 0)), closeTo(0.0, _eps));
    });
  });

  group('Plane ray intersection', () {
    const plane = Plane(Vec3(0, 1, 0), 0.0); // the XZ plane (y = 0)

    test('orthogonal ray hits the plane', () {
      final ray = Ray.normalized(const Vec3(0, 5, 0), const Vec3(0, -1, 0));
      final hit = plane.intersectRay(ray);
      expect(hit, isNotNull);
      expectVec(hit!, const Vec3(0, 0, 0));
    });

    test('parallel ray misses (returns null)', () {
      final ray = Ray.normalized(const Vec3(0, 5, 0), const Vec3(1, 0, 0));
      expect(plane.intersectRay(ray), isNull);
    });

    test('ray pointing away from the plane misses', () {
      final ray = Ray.normalized(const Vec3(0, 5, 0), const Vec3(0, 1, 0));
      expect(plane.intersectRay(ray), isNull);
    });
  });

  group('Plane segment intersection', () {
    const plane = Plane(Vec3(0, 1, 0), 0.0);

    test('segment crossing the plane returns the crossing point', () {
      final hit = plane.intersectSegment(
          const Vec3(0, -1, 0), const Vec3(0, 1, 0));
      expect(hit, isNotNull);
      expectVec(hit!, const Vec3(0, 0, 0));
    });

    test('segment on one side misses', () {
      expect(
          plane.intersectSegment(
              const Vec3(0, 1, 0), const Vec3(0, 2, 0)),
          isNull);
    });

    test('segment parallel to the plane misses', () {
      expect(
          plane.intersectSegment(
              const Vec3(0, 1, 0), const Vec3(1, 1, 0)),
          isNull);
    });
  });

  group('Plane transformed', () {
    test('translation moves the plane along its normal', () {
      const plane = Plane(Vec3(0, 1, 0), 0.0); // y = 0
      final t = Mat4.makeTranslation(0, 5, 0);
      final moved = plane.transformed(t);
      expectVec(moved.normal, const Vec3(0, 1, 0));
      // A point that was at y=0 is now at y=5, so distance becomes 5.
      expect(moved.distance, closeTo(5.0, _eps));
    });

    test('rotation around Z keeps a Y-plane normal consistent', () {
      const plane = Plane(Vec3(0, 1, 0), 0.0);
      final t = Mat4.makeRotationZ(math.pi / 2);
      final rotated = plane.transformed(t);
      // +Y rotated +90° around Z becomes -X (within tolerance).
      expectVec(rotated.normal, const Vec3(-1, 0, 0), tol: 1e-9);
      expect(rotated.distance, closeTo(0.0, _eps));
    });
  });

  group('Plane equality', () {
    test('value semantics on normal and distance', () {
      const a = Plane(Vec3(0, 1, 0), 5.0);
      const b = Plane(Vec3(0, 1, 0), 5.0);
      expect(a, b);
      expect(a.hashCode, b.hashCode);
    });
  });
}
