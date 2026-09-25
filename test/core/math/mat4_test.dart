// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// mat4_test.dart — unit tests for the immutable 4x4 transformation matrix.

import 'dart:math' as math;

import 'package:feather_krita/core/math/mat4.dart';
import 'package:feather_krita/core/math/vec3.dart';
import 'package:flutter_test/flutter_test.dart';

const double _eps = 1e-9;

void expectMatClose(Mat4 a, Mat4 b, {double tol = 1e-7}) {
  for (var r = 0; r < 4; r++) {
    for (var c = 0; c < 4; c++) {
      expect(a.entry(r, c), closeTo(b.entry(r, c), tol));
    }
  }
}

void main() {
  group('Mat4 identity & basic accessors', () {
    test('identity multiplied is a no-op', () {
      const m = Mat4.identity();
      const other = Mat4(
          1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16);
      expect(m.multiply(other), other);
      expect(m * other, other);
    });

    test('entry() returns the (row, col) component', () {
      const m = Mat4(
          0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15);
      expect(m.entry(0, 0), 0);
      expect(m.entry(1, 1), 5);
      expect(m.entry(2, 2), 10);
      expect(m.entry(3, 3), 15);
      expect(m.entry(3, 0), 12);
    });
  });

  group('Mat4 multiplication & composition', () {
    test('translation followed by inverse translation cancels', () {
      final t = Mat4.makeTranslation(5, -3, 7);
      final tInv = t.inverted();
      expect(tInv, isNotNull);
      expectMatClose(t.multiply(tInv!), const Mat4.identity());
    });

    test('two translations compose additively', () {
      final a = Mat4.makeTranslation(1, 2, 3);
      final b = Mat4.makeTranslation(10, 20, 30);
      final ab = a.multiply(b);
      expect(ab.translation, const Vec3(11, 22, 33));
    });

    test('rotation by 2pi returns identity (within fp tolerance)', () {
      final r = Mat4.makeRotationZ(2 * math.pi);
      expectMatClose(r, const Mat4.identity(), tol: 1e-9);
    });

    test('scale matrix extracts scale via .scale getter', () {
      final s = Mat4.makeScale(2, 3, 4);
      expect(s.scale, const Vec3(2, 3, 4));
    });
  });

  group('Mat4 transform', () {
    test('transformPoint applies translation', () {
      final t = Mat4.makeTranslation(10, 20, 30);
      final p = t.transformPoint(const Vec3(1, 2, 3));
      expect(p, const Vec3(11, 22, 33));
    });

    test('transformVector ignores translation', () {
      final t = Mat4.makeTranslation(10, 20, 30);
      final v = t.transformVector(const Vec3(1, 2, 3));
      expect(v, const Vec3(1, 2, 3));
    });

    test('transformPoint with scale scales the point', () {
      final s = Mat4.makeScale(2, 4, 8);
      final p = s.transformPoint(const Vec3(1, 1, 1));
      expect(p, const Vec3(2, 4, 8));
    });

    test('transformDirection removes scale from the columns', () {
      final s = Mat4.makeScale(2, 3, 4);
      // The columns are unitized, so a (1,1,1) direction is preserved.
      final d = s.transformDirection(const Vec3(1, 1, 1));
      expect(d.length, closeTo(math.sqrt(3.0), 1e-9));
    });
  });

  group('Mat4 inverse & determinant', () {
    test('determinant of identity is 1', () {
      expect(const Mat4.identity().determinant(), 1);
    });

    test('determinant of a pure scale is the product of the scale', () {
      expect(Mat4.makeScale(2, 3, 4).determinant(), 24);
    });

    test('singular matrix returns null inverse', () {
      const zero = Mat4.zero();
      expect(zero.inverted(), isNull);
    });

    test('inverse of a rotation is its transpose', () {
      final r = Mat4.makeRotationY(math.pi / 3);
      final inv = r.inverted();
      expect(inv, isNotNull);
      expectMatClose(inv!, r.transposed());
    });

    test('inverse composed with original yields identity', () {
      final m = Mat4.makeRotationX(0.4)
          .multiply(Mat4.makeTranslation(1, 2, 3))
          .multiply(Mat4.makeScale(2, 2, 2));
      final inv = m.inverted();
      expect(inv, isNotNull);
      expectMatClose(m.multiply(inv!), const Mat4.identity(), tol: 1e-7);
    });
  });

  group('Mat4 static constructors', () {
    test('makePerspective maps the near plane centre to z = -1', () {
      final p = Mat4.makePerspective(math.pi / 2, 1.0, 1.0, 100.0);
      // Origin at the near plane (z = -1 in view space).
      final ndc = p.transformPoint(const Vec3(0, 0, -1));
      expect(ndc.x, closeTo(0, _eps));
      expect(ndc.y, closeTo(0, _eps));
      expect(ndc.z, closeTo(-1, 1e-7));
    });

    test('makePerspective maps the far plane centre to z = +1', () {
      final p = Mat4.makePerspective(math.pi / 2, 1.0, 1.0, 100.0);
      final ndc = p.transformPoint(const Vec3(0, 0, -100));
      expect(ndc.z, closeTo(1, 1e-7));
    });

    test('makeLookAt places the camera at eye looking toward target', () {
      final view = Mat4.makeLookAt(
        const Vec3(0, 0, 5),
        const Vec3(0, 0, 0),
        const Vec3.unitY(),
      );
      // Eye is mapped to the origin in view space.
      final eyeInView = view.transformPoint(const Vec3(0, 0, 5));
      expectVec(eyeInView, const Vec3.zero(), tol: 1e-7);
      // Target sits along -Z in view space.
      final targetInView = view.transformPoint(const Vec3(0, 0, 0));
      expect(targetInView.z, lessThan(0));
    });

    test('makeOrthographic maps corners to NDC', () {
      final o = Mat4.makeOrthographic(-1, 1, -1, 1, 0, 10);
      expectVec(o.transformPoint(const Vec3(-1, -1, 0)),
          const Vec3(-1, -1, -1), tol: 1e-7);
      expectVec(o.transformPoint(const Vec3(1, 1, -10)),
          const Vec3(1, 1, 1), tol: 1e-7);
    });
  });
}

void expectVec(Vec3 actual, Vec3 expected, {double tol = 1e-9}) {
  expect(actual.x, closeTo(expected.x, tol));
  expect(actual.y, closeTo(expected.y, tol));
  expect(actual.z, closeTo(expected.z, tol));
}
