// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// quaternion.dart — Immutable unit-quaternion type for 3D rotations.
//
// Convention: (x, y, z, w) where w is the scalar part and (x, y, z) is
// the vector part. Rotations compose right-to-left (Hamilton product)
// so `q2 * q1` applies q1 first, then q2 — matching the matrix
// convention `M2 * M1 * v`.
//
// Provides: fromAxisAngle, fromEuler, fromMatrix, toMatrix, slerp,
// normalize, conjugate, inverse, multiply, rotateVector, dot, angle,
// axis.

import 'dart:math' as math;

import 'math_utils.dart';
import 'mat3.dart';
import 'vec3.dart';

/// An immutable quaternion.
class Quaternion {
  /// Vector part X.
  final double x;

  /// Vector part Y.
  final double y;

  /// Vector part Z.
  final double z;

  /// Scalar part.
  final double w;

  /// Constructs a quaternion from its components.
  const Quaternion(this.x, this.y, this.z, this.w);

  /// Identity rotation (no rotation).
  const Quaternion.identity() : x = 0.0, y = 0.0, z = 0.0, w = 1.0;

  /// Constructs a rotation of [angle] radians about [axis]. [axis] is
  /// normalized internally.
  factory Quaternion.fromAxisAngle(Vec3 axis, double angle) {
    final a = axis.normalized();
    final half = angle * 0.5;
    final s = math.sin(half);
    return Quaternion(a.x * s, a.y * s, a.z * s, math.cos(half));
  }

  /// Constructs from Euler angles [x], [y], [z] (radians) in extrinsic
  /// XYZ order: the rotation about X is applied first, then Y, then Z
  /// in the world frame.
  factory Quaternion.fromEuler(double x, double y, double z) {
    final qx = Quaternion.fromAxisAngle(const Vec3.unitX(), x);
    final qy = Quaternion.fromAxisAngle(const Vec3.unitY(), y);
    final qz = Quaternion.fromAxisAngle(const Vec3.unitZ(), z);
    return qz * qy * qx;
  }

  /// Constructs from a rotation [Mat3] using Shepperd's branch method.
  /// Handles all trace/diagonal cases for numerical stability.
  factory Quaternion.fromMatrix(Mat3 m) {
    final trace = m.m00 + m.m11 + m.m22;
    if (trace > 0.0) {
      final s = math.sqrt(trace + 1.0) * 2.0; // s = 4 * w
      return Quaternion(
        (m.m21 - m.m12) / s,
        (m.m02 - m.m20) / s,
        (m.m10 - m.m01) / s,
        0.25 * s,
      );
    } else if (m.m00 > m.m11 && m.m00 > m.m22) {
      final s = math.sqrt(1.0 + m.m00 - m.m11 - m.m22) * 2.0; // s = 4 * x
      return Quaternion(
        0.25 * s,
        (m.m01 + m.m10) / s,
        (m.m02 + m.m20) / s,
        (m.m21 - m.m12) / s,
      );
    } else if (m.m11 > m.m22) {
      final s = math.sqrt(1.0 + m.m11 - m.m00 - m.m22) * 2.0; // s = 4 * y
      return Quaternion(
        (m.m01 + m.m10) / s,
        0.25 * s,
        (m.m12 + m.m21) / s,
        (m.m02 - m.m20) / s,
      );
    } else {
      final s = math.sqrt(1.0 + m.m22 - m.m00 - m.m11) * 2.0; // s = 4 * z
      return Quaternion(
        (m.m02 + m.m20) / s,
        (m.m12 + m.m21) / s,
        0.25 * s,
        (m.m10 - m.m01) / s,
      );
    }
  }

  /// Returns the equivalent rotation as a [Mat3]. Assumes a unit
  /// quaternion; non-unit quaternions are normalized first.
  Mat3 toMatrix() {
    final n = normalized();
    final xx = n.x * n.x;
    final yy = n.y * n.y;
    final zz = n.z * n.z;
    final xy = n.x * n.y;
    final xz = n.x * n.z;
    final yz = n.y * n.z;
    final wx = n.w * n.x;
    final wy = n.w * n.y;
    final wz = n.w * n.z;
    return Mat3(
      1.0 - 2.0 * (yy + zz), 2.0 * (xy - wz), 2.0 * (xz + wy),
      2.0 * (xy + wz), 1.0 - 2.0 * (xx + zz), 2.0 * (yz - wx),
      2.0 * (xz - wy), 2.0 * (yz + wx), 1.0 - 2.0 * (xx + yy),
    );
  }

  /// Hamilton product: `this * [o]`. Applies [o] first, then `this`.
  Quaternion operator *(Quaternion o) => Quaternion(
        w * o.x + x * o.w + y * o.z - z * o.y,
        w * o.y - x * o.z + y * o.w + z * o.x,
        w * o.z + x * o.y - y * o.x + z * o.w,
        w * o.w - x * o.x - y * o.y - z * o.z,
      );

  /// Exact component equality.
  @override
  bool operator ==(Object other) =>
      other is Quaternion &&
      x == other.x &&
      y == other.y &&
      z == other.z &&
      w == other.w;

  @override
  int get hashCode => Object.hash(x, y, z, w);

  /// Squared length.
  double get length2 => x * x + y * y + z * z + w * w;

  /// Length.
  double get length => math.sqrt(length2);

  /// Dot product with [o].
  double dot(Quaternion o) => x * o.x + y * o.y + z * o.z + w * o.w;

  /// Unit-length copy. Returns identity when too small to normalize.
  Quaternion normalized() {
    final l = length;
    if (l < epsilon) return const Quaternion.identity();
    return Quaternion(x / l, y / l, z / l, w / l);
  }

  /// Conjugate (-x, -y, -z, w). For unit quaternions this is also the
  /// inverse.
  Quaternion conjugate() => Quaternion(-x, -y, -z, w);

  /// Multiplicative inverse. For unit quaternions equals [conjugate].
  /// Returns identity when too small to invert.
  Quaternion inverse() {
    final l2 = length2;
    if (l2 < epsilon) return const Quaternion.identity();
    return Quaternion(-x / l2, -y / l2, -z / l2, w / l2);
  }

  /// Rotates vector [v] by this quaternion. Uses the optimized form
  /// `v' = v + w*t + cross(u, t)` where `t = 2 * cross(u, v)` and
  /// `u` is the vector part. Assumes a unit quaternion.
  Vec3 rotateVector(Vec3 v) {
    final u = Vec3(x, y, z);
    final t = u.cross(v) * 2.0;
    return v + t * w + u.cross(t);
  }

  /// Spherical linear interpolation toward [o] by [t]. Takes the
  /// shortest arc (negates [o] when the dot product is negative) and
  /// falls back to normalized linear interpolation when the two
  /// quaternions are very close.
  Quaternion slerp(Quaternion o, double t) {
    var dot = this.dot(o);
    var b = o;
    if (dot < 0.0) {
      b = Quaternion(-o.x, -o.y, -o.z, -o.w);
      dot = -dot;
    }
    if (dot > 0.9995) {
      // Linear interpolation when nearly parallel.
      return Quaternion(
            x + (b.x - x) * t,
            y + (b.y - y) * t,
            z + (b.z - z) * t,
            w + (b.w - w) * t,
          ).normalized();
    }
    final theta0 = math.acos(clampDouble(dot, -1.0, 1.0));
    final theta = theta0 * t;
    final sinTheta0 = math.sin(theta0);
    final s0 = math.cos(theta) - dot * math.sin(theta) / sinTheta0;
    final s1 = math.sin(theta) / sinTheta0;
    return Quaternion(
      s0 * x + s1 * b.x,
      s0 * y + s1 * b.y,
      s0 * z + s1 * b.z,
      s0 * w + s1 * b.w,
    );
  }

  /// Returns the rotation axis (unit vector) and angle (radians). The
  /// axis defaults to +X when the rotation is identity.
  (Vec3 axis, double angle) toAxisAngle() {
    final cw = w.clamp(-1.0, 1.0);
    final a = 2.0 * math.acos(cw);
    final s = math.sqrt(1.0 - cw * cw);
    if (s < epsilon) {
      return (const Vec3.unitX(), 0.0);
    }
    return (Vec3(x / s, y / s, z / s), a);
  }

  @override
  String toString() => 'Quaternion($x, $y, $z, $w)';
}
