// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// mat3.dart — Immutable 3x3 matrix for normal transforms and rotation
// extraction.
//
// Storage is row-major with named fields [m00]..[m22] where [mRC] is
// the entry at row R, column C. Vectors are column vectors: `result =
// M * v`. Provides multiply, transpose, determinant, inverse, axis
// rotations, scale, quaternion conversion, and the
// inverse-transpose (normal matrix) helper used for non-uniformly
// scaled geometry.

import 'dart:math' as math;

import 'math_utils.dart';
import 'mat4.dart';
import 'quaternion.dart';
import 'vec3.dart';

/// An immutable 3x3 matrix of doubles.
class Mat3 {
  /// Row 0.
  final double m00, m01, m02;

  /// Row 1.
  final double m10, m11, m12;

  /// Row 2.
  final double m20, m21, m22;

  /// Constructs from all 9 entries in row-major order.
  const Mat3(
      this.m00,
      this.m01,
      this.m02,
      this.m10,
      this.m11,
      this.m12,
      this.m20,
      this.m21,
      this.m22);

  /// Identity.
  const Mat3.identity()
      : m00 = 1.0, m01 = 0.0, m02 = 0.0,
        m10 = 0.0, m11 = 1.0, m12 = 0.0,
        m20 = 0.0, m21 = 0.0, m22 = 1.0;

  /// All-zero.
  const Mat3.zero()
      : m00 = 0.0, m01 = 0.0, m02 = 0.0,
        m10 = 0.0, m11 = 0.0, m12 = 0.0,
        m20 = 0.0, m21 = 0.0, m22 = 0.0;

  /// Returns the entry at [row], [col] (0-indexed, 0..2).
  double entry(int row, int col) {
    if (row == 0) {
      return col == 0 ? m00 : col == 1 ? m01 : m02;
    } else if (row == 1) {
      return col == 0 ? m10 : col == 1 ? m11 : m12;
    }
    return col == 0 ? m20 : col == 1 ? m21 : m22;
  }

  /// Matrix-matrix multiplication: `this * [o]`.
  Mat3 multiply(Mat3 o) => Mat3(
        m00 * o.m00 + m01 * o.m10 + m02 * o.m20,
        m00 * o.m01 + m01 * o.m11 + m02 * o.m21,
        m00 * o.m02 + m01 * o.m12 + m02 * o.m22,
        m10 * o.m00 + m11 * o.m10 + m12 * o.m20,
        m10 * o.m01 + m11 * o.m11 + m12 * o.m21,
        m10 * o.m02 + m11 * o.m12 + m12 * o.m22,
        m20 * o.m00 + m21 * o.m10 + m22 * o.m20,
        m20 * o.m01 + m21 * o.m11 + m22 * o.m21,
        m20 * o.m02 + m21 * o.m12 + m22 * o.m22,
      );

  /// Alias for [multiply].
  Mat3 operator *(Mat3 o) => multiply(o);

  /// Exact component-wise equality.
  @override
  bool operator ==(Object other) =>
      other is Mat3 &&
      m00 == other.m00 &&
      m01 == other.m01 &&
      m02 == other.m02 &&
      m10 == other.m10 &&
      m11 == other.m11 &&
      m12 == other.m12 &&
      m20 == other.m20 &&
      m21 == other.m21 &&
      m22 == other.m22;

  @override
  int get hashCode => Object.hash(
        m00, m01, m02,
        m10, m11, m12,
        m20, m21, m22,
      );

  /// Transpose.
  Mat3 transposed() => Mat3(
        m00, m10, m20,
        m01, m11, m21,
        m02, m12, m22,
      );

  /// Determinant.
  double determinant() =>
      m00 * (m11 * m22 - m12 * m21) -
      m01 * (m10 * m22 - m12 * m20) +
      m02 * (m10 * m21 - m11 * m20);

  /// Returns the inverse, or `null` when singular.
  Mat3? inverted() {
    final det = determinant();
    if (det.abs() < epsilon) return null;
    final invDet = 1.0 / det;
    return Mat3(
      (m11 * m22 - m12 * m21) * invDet,
      (m02 * m21 - m01 * m22) * invDet,
      (m01 * m12 - m02 * m11) * invDet,
      (m12 * m20 - m10 * m22) * invDet,
      (m00 * m22 - m02 * m20) * invDet,
      (m02 * m10 - m00 * m12) * invDet,
      (m10 * m21 - m11 * m20) * invDet,
      (m01 * m20 - m00 * m21) * invDet,
      (m00 * m11 - m01 * m10) * invDet,
    );
  }

  /// Transforms a [Vec3] as a column vector (no translation).
  Vec3 transform(Vec3 v) => Vec3(
        m00 * v.x + m01 * v.y + m02 * v.z,
        m10 * v.x + m11 * v.y + m12 * v.z,
        m20 * v.x + m21 * v.y + m22 * v.z,
      );

  /// Extracts the rotation as a quaternion via Shepperd's method.
  /// See [Quaternion.fromMatrix].
  Quaternion toQuaternion() => Quaternion.fromMatrix(this);

  // ---- Static constructors -------------------------------------------

  /// Diagonal scale by ([sx], [sy], [sz]).
  static Mat3 makeScale(double sx, double sy, double sz) => Mat3(
        sx, 0.0, 0.0,
        0.0, sy, 0.0,
        0.0, 0.0, sz,
      );

  /// Rotation about the X axis by [angle] radians.
  static Mat3 makeRotationX(double angle) {
    final c = math.cos(angle);
    final s = math.sin(angle);
    return Mat3(
      1.0, 0.0, 0.0,
      0.0, c, -s,
      0.0, s, c,
    );
  }

  /// Rotation about the Y axis by [angle] radians.
  static Mat3 makeRotationY(double angle) {
    final c = math.cos(angle);
    final s = math.sin(angle);
    return Mat3(
      c, 0.0, s,
      0.0, 1.0, 0.0,
      -s, 0.0, c,
    );
  }

  /// Rotation about the Z axis by [angle] radians.
  static Mat3 makeRotationZ(double angle) {
    final c = math.cos(angle);
    final s = math.sin(angle);
    return Mat3(
      c, -s, 0.0,
      s, c, 0.0,
      0.0, 0.0, 1.0,
    );
  }

  /// Extracts the upper-left 3x3 of [m].
  factory Mat3.fromMat4(Mat4 m) => Mat3(
        m.m00, m.m01, m.m02,
        m.m10, m.m11, m.m12,
        m.m20, m.m21, m.m22,
      );

  /// Computes the normal matrix (inverse-transpose) of the upper-left
  /// 3x3 of [m]. Falls back to the transposed upper 3x3 when [m] is
  /// not invertible.
  static Mat3 normalFromMat4(Mat4 m) {
    final upper = Mat3.fromMat4(m);
    final inv = upper.inverted();
    if (inv == null) return upper.transposed();
    return inv.transposed();
  }

  @override
  String toString() => 'Mat3(\n'
      '  $m00, $m01, $m02,\n'
      '  $m10, $m11, $m12,\n'
      '  $m20, $m21, $m22,\n)';
}
