// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// vec4_test.dart — unit tests for the immutable Vec4 type.

import 'package:feather_krita/core/math/vec3.dart';
import 'package:feather_krita/core/math/vec4.dart';
import 'package:flutter_test/flutter_test.dart';

const double _eps = 1e-9;

void main() {
  group('Vec4 construction', () {
    test('named constructors', () {
      expect(const Vec4.zero(), const Vec4(0, 0, 0, 0));
      expect(const Vec4.all(7), const Vec4(7, 7, 7, 7));
      expect(const Vec4.identity(), const Vec4(0, 0, 0, 1));
    });

    test('fromVec3 promotes with the given w', () {
      expect(Vec4.fromVec3(const Vec3(1, 2, 3)),
          const Vec4(1, 2, 3, 1.0));
      expect(Vec4.fromVec3(const Vec3(1, 2, 3), 0.0),
          const Vec4(1, 2, 3, 0.0));
    });

    test('fromList takes the first four entries', () {
      expect(Vec4.fromList([1, 2, 3, 4, 5]), const Vec4(1, 2, 3, 4));
    });
  });

  group('Vec4 arithmetic', () {
    test('add / subtract / scale / negate', () {
      const a = Vec4(1, 2, 3, 4);
      const b = Vec4(10, 20, 30, 40);
      expect(a + b, const Vec4(11, 22, 33, 44));
      expect(b - a, const Vec4(9, 18, 27, 36));
      expect(a * 2, const Vec4(2, 4, 6, 8));
      expect(a / 2, const Vec4(0.5, 1, 1.5, 2));
      expect(-a, const Vec4(-1, -2, -3, -4));
    });

    test('Hadamard product and divide', () {
      expect(const Vec4(1, 2, 3, 4).multiply(const Vec4(2, 3, 4, 5)),
          const Vec4(2, 6, 12, 20));
      expect(const Vec4(2, 6, 12, 20).divide(const Vec4(2, 3, 4, 5)),
          const Vec4(1, 2, 3, 4));
    });

    test('dot product', () {
      expect(const Vec4(1, 2, 3, 4).dot(const Vec4(1, 1, 1, 1)), 10);
    });

    test('equality and hashCode obey value semantics', () {
      const a = Vec4(1, 2, 3, 4);
      const b = Vec4(1, 2, 3, 4);
      expect(a, b);
      expect(a.hashCode, b.hashCode);
      expect(const Vec4(1, 2, 3, 4) == const Vec4(4, 3, 2, 1), isFalse);
    });
  });

  group('Vec4 length and normalization', () {
    test('length2 and length are consistent', () {
      const v = Vec4(1, 1, 1, 1);
      expect(v.length2, 4);
      expect(v.length, 2);
    });

    test('normalized returns a unit vector or zero', () {
      final n = const Vec4(0, 0, 3, 4).normalized();
      expect(n.length, closeTo(1.0, _eps));
      expect(n.x, closeTo(0, _eps));
      expect(n.y, closeTo(0, _eps));
      expect(n.z, closeTo(0.6, _eps));
      expect(n.w, closeTo(0.8, _eps));
      // Zero vector → zero.
      expect(const Vec4.zero().normalized(), const Vec4.zero());
    });
  });

  group('Vec4 swizzle / promotion', () {
    test('xyz drops the w component', () {
      expect(const Vec4(1, 2, 3, 4).xyz, const Vec3(1, 2, 3));
    });

    test('perspectiveDivide divides by w', () {
      final p = const Vec4(2, 4, 6, 2).perspectiveDivide;
      expect(p.x, closeTo(1.0, _eps));
      expect(p.y, closeTo(2.0, _eps));
      expect(p.z, closeTo(3.0, _eps));
    });

    test('perspectiveDivide handles near-zero w gracefully', () {
      // w == 0 → returns raw (x, y, z) (no NaN).
      final p = const Vec4(1, 2, 3, 0).perspectiveDivide;
      expect(p.x, closeTo(1.0, _eps));
      expect(p.y, closeTo(2.0, _eps));
      expect(p.z, closeTo(3.0, _eps));
    });
  });

  group('Vec4 lerp and copyWith', () {
    test('lerp interpolates endpoints and midpoint', () {
      const a = Vec4(0, 0, 0, 0);
      const b = Vec4(10, 20, 30, 40);
      expect(a.lerp(b, 0), a);
      expect(a.lerp(b, 1), b);
      expect(a.lerp(b, 0.5), const Vec4(5, 10, 15, 20));
    });

    test('copyWith overrides only the named components', () {
      const v = Vec4(1, 2, 3, 4);
      expect(v.copyWith(w: 99), const Vec4(1, 2, 3, 99));
      expect(v.copyWith(x: 0, y: 0), const Vec4(0, 0, 3, 4));
    });

    test('toList round-trips the four components', () {
      expect(const Vec4(1, 2, 3, 4).toList(), [1, 2, 3, 4]);
    });
  });
}
