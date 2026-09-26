// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// vec2_test.dart — unit tests for the immutable Vec2 type.

import 'dart:math' as math;

import 'package:feather_krita/core/math/vec2.dart';
import 'package:flutter_test/flutter_test.dart';

const double _eps = 1e-9;

void main() {
  group('Vec2 construction', () {
    test('named constructors', () {
      expect(const Vec2.zero(), const Vec2(0, 0));
      expect(const Vec2.unitX(), const Vec2(1, 0));
      expect(const Vec2.unitY(), const Vec2(0, 1));
      expect(const Vec2.all(7), const Vec2(7, 7));
    });

    test('fromList takes the first two entries', () {
      expect(Vec2.fromList([1.5, 2.5, 3.5]), const Vec2(1.5, 2.5));
    });
  });

  group('Vec2 arithmetic', () {
    test('add / subtract / scale / negate', () {
      const a = Vec2(1, 2);
      const b = Vec2(10, 20);
      expect(a + b, const Vec2(11, 22));
      expect(b - a, const Vec2(9, 18));
      expect(a * 3, const Vec2(3, 6));
      expect(a / 2, const Vec2(0.5, 1));
      expect(-a, const Vec2(-1, -2));
    });

    test('Hadamard product and divide', () {
      expect(const Vec2(2, 3).multiply(const Vec2(4, 5)),
          const Vec2(8, 15));
      expect(const Vec2(8, 15).divide(const Vec2(2, 5)),
          const Vec2(4, 3));
    });

    test('equality and hashCode obey value semantics', () {
      const a = Vec2(1, 2);
      const b = Vec2(1, 2);
      expect(a, b);
      expect(a.hashCode, b.hashCode);
      expect(const Vec2(1, 2) == const Vec2(2, 1), isFalse);
    });
  });

  group('Vec2 geometry', () {
    test('dot product is symmetric', () {
      const a = Vec2(1, 2);
      const b = Vec2(3, 4);
      expect(a.dot(b), 11);
      expect(b.dot(a), 11);
      expect(a.dot(const Vec2.zero()), 0);
    });

    test('length and length2 are consistent', () {
      const v = Vec2(3, 4);
      expect(v.length2, 25);
      expect(v.length, 5);
    });

    test('distance / distanceSquared', () {
      expect(const Vec2(0, 0).distanceTo(const Vec2(3, 4)), 5);
      expect(const Vec2(0, 0).distanceToSquared(const Vec2(3, 4)), 25);
      expect(const Vec2(0, 0).distanceTo(const Vec2.zero()), 0);
    });

    test('normalized returns a unit vector or zero', () {
      final n = const Vec2(0, 5).normalized();
      expect(n.length, closeTo(1.0, _eps));
      expect(n.x, closeTo(0.0, _eps));
      expect(n.y, closeTo(1.0, _eps));
      expect(const Vec2.zero().normalized(), const Vec2.zero());
    });

    test('lerp interpolates endpoints and midpoint', () {
      const a = Vec2(0, 0);
      const b = Vec2(10, 20);
      expect(a.lerp(b, 0), a);
      expect(a.lerp(b, 1), b);
      expect(a.lerp(b, 0.5), const Vec2(5, 10));
    });

    test('angleTo is signed CCW', () {
      // +X to +Y is +π/2.
      expect(const Vec2.unitX().angleTo(const Vec2.unitY()),
          closeTo(math.pi / 2, 1e-9));
      // +Y to +X is -π/2.
      expect(const Vec2.unitY().angleTo(const Vec2.unitX()),
          closeTo(-math.pi / 2, 1e-9));
      // Parallel vectors have zero angle.
      expect(const Vec2(1, 1).angleTo(const Vec2(2, 2)), closeTo(0, 1e-9));
    });
  });
}
