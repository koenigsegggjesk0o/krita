// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// quaternion_test.dart — unit tests for the immutable quaternion type.

import 'dart:math' as math;

import 'package:feather_krita/core/math/mat3.dart';
import 'package:feather_krita/core/math/quaternion.dart';
import 'package:feather_krita/core/math/vec3.dart';
import 'package:flutter_test/flutter_test.dart';

const double _eps = 1e-9;

void expectVec(Vec3 a, Vec3 b, {double tol = _eps}) {
  expect(a.x, closeTo(b.x, tol));
  expect(a.y, closeTo(b.y, tol));
  expect(a.z, closeTo(b.z, tol));
}

void expectQuat(Quaternion a, Quaternion b, {double tol = 1e-7}) {
  expect(a.x, closeTo(b.x, tol));
  expect(a.y, closeTo(b.y, tol));
  expect(a.z, closeTo(b.z, tol));
  expect(a.w, closeTo(b.w, tol));
}

void main() {
  group('Quaternion identity & basic facts', () {
    test('identity has zero vector part and unit scalar', () {
      const q = Quaternion.identity();
      expect(q.x, 0);
      expect(q.y, 0);
      expect(q.z, 0);
      expect(q.w, 1);
      expect(q.length, 1);
    });

    test('identity rotates nothing', () {
      const v = Vec3(1, 2, 3);
      expectVec(const Quaternion.identity().rotateVector(v), v);
    });

    test('length2 / length / dot are consistent', () {
      final q = Quaternion(1, 2, 3, 4);
      expect(q.length2, 30);
      expect(q.length, closeTo(math.sqrt(30), _eps));
      expect(q.dot(Quaternion(1, 2, 3, 4)), 30);
    });
  });

  group('Quaternion fromAxisAngle', () {
    test('half-angle decomposition matches the textbook formula', () {
      final q = Quaternion.fromAxisAngle(const Vec3.unitZ(), math.pi / 2);
      // cos(pi/4) = sqrt(2)/2; sin(pi/4) = sqrt(2)/2; axis (0,0,1).
      expect(q.w, closeTo(math.sqrt(0.5), _eps));
      expect(q.z, closeTo(math.sqrt(0.5), _eps));
      expect(q.x, 0);
      expect(q.y, 0);
    });

    test('a 2pi rotation is identity (up to sign)', () {
      final q = Quaternion.fromAxisAngle(const Vec3.unitY(), 2 * math.pi);
      // For a 2pi rotation, cos(pi) = -1, so w = -1. The quaternion
      // represents the same rotation as identity (q and -q are
      // equivalent); we check the vector part is zero and the rotation
      // acts as identity on a sample vector.
      expect(q.x, closeTo(0, _eps));
      expect(q.y, closeTo(0, _eps));
      expect(q.z, closeTo(0, _eps));
      expect(q.w.abs(), closeTo(1, _eps));
      expectVec(
          q.rotateVector(const Vec3(1, 2, 3)), const Vec3(1, 2, 3),
          tol: 1e-7);
    });

    test('rotates a vector by the requested angle', () {
      final q = Quaternion.fromAxisAngle(const Vec3.unitZ(), math.pi / 2);
      final r = q.rotateVector(const Vec3.unitX());
      expectVec(r, const Vec3.unitY(), tol: 1e-7);
    });

    test('axis is normalized internally before use', () {
      // Pass an un-normalised axis; expect the same rotation as the
      // normalised version.
      final a = Quaternion.fromAxisAngle(const Vec3(0, 2, 0), math.pi);
      final b = Quaternion.fromAxisAngle(const Vec3.unitY(), math.pi);
      expectQuat(a, b, tol: 1e-9);
    });
  });

  group('Quaternion fromEuler', () {
    test('pure x-axis euler equals fromAxisAngle on X', () {
      final e = Quaternion.fromEuler(math.pi / 4, 0, 0);
      final a = Quaternion.fromAxisAngle(const Vec3.unitX(), math.pi / 4);
      expectQuat(e, a, tol: 1e-9);
    });

    test('euler XYZ composition matches the matrix order', () {
      final q = Quaternion.fromEuler(math.pi / 2, 0, 0);
      // 90° about X takes +Y to +Z.
      expectVec(
          q.rotateVector(const Vec3.unitY()), const Vec3.unitZ(),
          tol: 1e-7);
    });

    test('euler zero is identity', () {
      expectQuat(Quaternion.fromEuler(0, 0, 0),
          const Quaternion.identity());
    });
  });

  group('Quaternion slerp', () {
    test('slerp at 0 returns the first quaternion', () {
      final a = Quaternion.fromAxisAngle(const Vec3.unitZ(), 0);
      final b = Quaternion.fromAxisAngle(const Vec3.unitZ(), math.pi / 2);
      expectQuat(a.slerp(b, 0), a, tol: 1e-7);
    });

    test('slerp at 1 returns the second quaternion', () {
      final a = Quaternion.fromAxisAngle(const Vec3.unitZ(), 0);
      final b = Quaternion.fromAxisAngle(const Vec3.unitZ(), math.pi / 2);
      expectQuat(a.slerp(b, 1), b, tol: 1e-7);
    });

    test('slerp at 0.5 is the half-angle rotation', () {
      final a = const Quaternion.identity();
      final b = Quaternion.fromAxisAngle(const Vec3.unitZ(), math.pi);
      final mid = a.slerp(b, 0.5);
      final expected = Quaternion.fromAxisAngle(
          const Vec3.unitZ(), math.pi / 2);
      expectQuat(mid, expected, tol: 1e-7);
    });

    test('slerp takes the shorter arc', () {
      // Two quaternions that represent the same rotation but with
      // opposite sign should slerp through identity.
      final a = Quaternion.fromAxisAngle(const Vec3.unitX(), 0.001);
      final b = Quaternion(-a.x, -a.y, -a.z, -a.w);
      final mid = a.slerp(b, 0.5);
      expect(mid.length, closeTo(1.0, 1e-6));
    });
  });

  group('Quaternion rotateVector & toMatrix', () {
    test('rotateVector matches matrix multiplication', () {
      final q = Quaternion.fromAxisAngle(const Vec3.unitY(), math.pi / 3);
      const v = Vec3(1, 2, 3);
      final qv = q.rotateVector(v);
      final mv = q.toMatrix().transform(v);
      expectVec(qv, mv, tol: 1e-7);
    });

    test('toMatrix of identity is the identity matrix', () {
      final m = const Quaternion.identity().toMatrix();
      expectMat3(m, const Mat3.identity(), tol: 1e-9);
    });

    test('conjugate undoes a unit-quaternion rotation', () {
      final q = Quaternion.fromAxisAngle(const Vec3.unitZ(), math.pi / 5);
      const v = Vec3(2, -3, 4);
      final rotated = q.rotateVector(v);
      final back = q.conjugate().rotateVector(rotated);
      expectVec(back, v, tol: 1e-7);
    });

    test('inverse of a unit quaternion equals conjugate', () {
      final q = Quaternion.fromAxisAngle(const Vec3.unitX(), 0.7);
      final inv = q.inverse();
      final conj = q.conjugate();
      expectQuat(inv, conj, tol: 1e-7);
    });
  });
}

void expectMat3(Mat3 a, Mat3 b, {double tol = _eps}) {
  for (var r = 0; r < 3; r++) {
    for (var c = 0; c < 3; c++) {
      expect(a.entry(r, c), closeTo(b.entry(r, c), tol));
    }
  }
}
