// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// mat4.dart — Immutable 4x4 transformation matrix.
//
// Storage is row-major with named fields [m00]..[m33] where [mRC] is the
// entry at row R, column C. Vectors are treated as column vectors, so a
// point is transformed as `result = M * (p, 1)`. Translation lives in
// the rightmost column (m03, m13, m23) following the OpenGL convention.
//
// Provides: identity, translation, scale, axis rotations, perspective,
// orthographic, and look-at constructors, plus multiply, inverse,
// transpose, determinant, and point/vector/direction transforms.

import 'dart:math' as math;

import 'math_utils.dart';
import 'vec3.dart';
import 'vec4.dart';

/// An immutable 4x4 matrix of doubles.
class Mat4 {
  /// Row 0.
  final double m00, m01, m02, m03;

  /// Row 1.
  final double m10, m11, m12, m13;

  /// Row 2.
  final double m20, m21, m22, m23;

  /// Row 3.
  final double m30, m31, m32, m33;

  /// Constructs a matrix from all 16 entries in row-major order.
  const Mat4(
      this.m00,
      this.m01,
      this.m02,
      this.m03,
      this.m10,
      this.m11,
      this.m12,
      this.m13,
      this.m20,
      this.m21,
      this.m22,
      this.m23,
      this.m30,
      this.m31,
      this.m32,
      this.m33);

  /// Identity matrix.
  const Mat4.identity()
      : m00 = 1.0, m01 = 0.0, m02 = 0.0, m03 = 0.0,
        m10 = 0.0, m11 = 1.0, m12 = 0.0, m13 = 0.0,
        m20 = 0.0, m21 = 0.0, m22 = 1.0, m23 = 0.0,
        m30 = 0.0, m31 = 0.0, m32 = 0.0, m33 = 1.0;

  /// All-zero matrix.
  const Mat4.zero()
      : m00 = 0.0, m01 = 0.0, m02 = 0.0, m03 = 0.0,
        m10 = 0.0, m11 = 0.0, m12 = 0.0, m13 = 0.0,
        m20 = 0.0, m21 = 0.0, m22 = 0.0, m23 = 0.0,
        m30 = 0.0, m31 = 0.0, m32 = 0.0, m33 = 0.0;

  /// Returns the entry at [row], [col] (0-indexed, 0..3).
  double entry(int row, int col) {
    if (row == 0) {
      return col == 0 ? m00 : col == 1 ? m01 : col == 2 ? m02 : m03;
    } else if (row == 1) {
      return col == 0 ? m10 : col == 1 ? m11 : col == 2 ? m12 : m13;
    } else if (row == 2) {
      return col == 0 ? m20 : col == 1 ? m21 : col == 2 ? m22 : m23;
    }
    return col == 0 ? m30 : col == 1 ? m31 : col == 2 ? m32 : m33;
  }

  /// Matrix-matrix multiplication: `this * [o]`.
  Mat4 multiply(Mat4 o) => Mat4(
        m00 * o.m00 + m01 * o.m10 + m02 * o.m20 + m03 * o.m30,
        m00 * o.m01 + m01 * o.m11 + m02 * o.m21 + m03 * o.m31,
        m00 * o.m02 + m01 * o.m12 + m02 * o.m22 + m03 * o.m32,
        m00 * o.m03 + m01 * o.m13 + m02 * o.m23 + m03 * o.m33,
        m10 * o.m00 + m11 * o.m10 + m12 * o.m20 + m13 * o.m30,
        m10 * o.m01 + m11 * o.m11 + m12 * o.m21 + m13 * o.m31,
        m10 * o.m02 + m11 * o.m12 + m12 * o.m22 + m13 * o.m32,
        m10 * o.m03 + m11 * o.m13 + m12 * o.m23 + m13 * o.m33,
        m20 * o.m00 + m21 * o.m10 + m22 * o.m20 + m23 * o.m30,
        m20 * o.m01 + m21 * o.m11 + m22 * o.m21 + m23 * o.m31,
        m20 * o.m02 + m21 * o.m12 + m22 * o.m22 + m23 * o.m32,
        m20 * o.m03 + m21 * o.m13 + m22 * o.m23 + m23 * o.m33,
        m30 * o.m00 + m31 * o.m10 + m32 * o.m20 + m33 * o.m30,
        m30 * o.m01 + m31 * o.m11 + m32 * o.m21 + m33 * o.m31,
        m30 * o.m02 + m31 * o.m12 + m32 * o.m22 + m33 * o.m32,
        m30 * o.m03 + m31 * o.m13 + m32 * o.m23 + m33 * o.m33,
      );

  /// Matrix-matrix multiplication alias for [multiply].
  Mat4 operator *(Mat4 o) => multiply(o);

  /// Exact component-wise equality.
  @override
  bool operator ==(Object other) =>
      other is Mat4 &&
      m00 == other.m00 &&
      m01 == other.m01 &&
      m02 == other.m02 &&
      m03 == other.m03 &&
      m10 == other.m10 &&
      m11 == other.m11 &&
      m12 == other.m12 &&
      m13 == other.m13 &&
      m20 == other.m20 &&
      m21 == other.m21 &&
      m22 == other.m22 &&
      m23 == other.m23 &&
      m30 == other.m30 &&
      m31 == other.m31 &&
      m32 == other.m32 &&
      m33 == other.m33;

  @override
  int get hashCode => Object.hash(
        m00, m01, m02, m03,
        m10, m11, m12, m13,
        m20, m21, m22, m23,
        m30, m31, m32, m33,
      );

  /// Transpose.
  Mat4 transposed() => Mat4(
        m00, m10, m20, m30,
        m01, m11, m21, m31,
        m02, m12, m22, m32,
        m03, m13, m23, m33,
      );

  /// Determinant via cofactor expansion along the first row.
  double determinant() {
    final a232 = m22 * m33 - m23 * m32;
    final a132 = m21 * m33 - m23 * m31;
    final a122 = m21 * m32 - m22 * m31;
    final a112 = m20 * m33 - m23 * m30;
    final a102 = m20 * m32 - m22 * m30;
    final a012 = m20 * m31 - m21 * m30;
    return m00 * (m11 * a232 - m12 * a132 + m13 * a122) -
        m01 * (m10 * a232 - m12 * a112 + m13 * a102) +
        m02 * (m10 * a132 - m11 * a112 + m13 * a012) -
        m03 * (m10 * a122 - m11 * a102 + m12 * a012);
  }

  /// Returns the inverse, or `null` when the matrix is singular.
  ///
  /// Uses the cofactor/adjugate method (general 4x4, not affine-only).
  Mat4? inverted() {
    final a00 = m11 * m22 * m33 - m11 * m32 * m23 - m12 * m21 * m33 +
        m12 * m31 * m23 + m13 * m21 * m32 - m13 * m31 * m22;
    final a01 = -m01 * m22 * m33 + m01 * m32 * m23 + m02 * m21 * m33 -
        m02 * m31 * m23 - m03 * m21 * m32 + m03 * m31 * m22;
    final a02 = m01 * m12 * m33 - m01 * m32 * m13 - m02 * m11 * m33 +
        m02 * m31 * m13 + m03 * m11 * m32 - m03 * m31 * m12;
    final a03 = -m01 * m12 * m23 + m01 * m22 * m13 + m02 * m11 * m23 -
        m02 * m21 * m13 - m03 * m11 * m22 + m03 * m21 * m12;
    final a10 = -m10 * m22 * m33 + m10 * m32 * m23 + m12 * m20 * m33 -
        m12 * m30 * m23 - m13 * m20 * m32 + m13 * m30 * m22;
    final a11 = m00 * m22 * m33 - m00 * m32 * m23 - m02 * m20 * m33 +
        m02 * m30 * m23 + m03 * m20 * m32 - m03 * m30 * m22;
    final a12 = -m00 * m12 * m33 + m00 * m32 * m13 + m02 * m10 * m33 -
        m02 * m30 * m13 - m03 * m10 * m32 + m03 * m30 * m12;
    final a13 = m00 * m12 * m23 - m00 * m22 * m13 - m02 * m10 * m23 +
        m02 * m20 * m13 + m03 * m10 * m22 - m03 * m20 * m12;
    final a20 = m10 * m21 * m33 - m10 * m31 * m23 - m11 * m20 * m33 +
        m11 * m30 * m23 + m13 * m20 * m31 - m13 * m30 * m21;
    final a21 = -m00 * m21 * m33 + m00 * m31 * m23 + m01 * m20 * m33 -
        m01 * m30 * m23 - m03 * m20 * m31 + m03 * m30 * m21;
    final a22 = m00 * m11 * m33 - m00 * m31 * m13 - m01 * m10 * m33 +
        m01 * m30 * m13 + m03 * m10 * m31 - m03 * m30 * m11;
    final a23 = -m00 * m11 * m23 + m00 * m21 * m13 + m01 * m10 * m23 -
        m01 * m20 * m13 - m03 * m10 * m21 + m03 * m20 * m11;
    final a30 = -m10 * m21 * m32 + m10 * m31 * m22 + m11 * m20 * m32 -
        m11 * m30 * m22 - m12 * m20 * m31 + m12 * m30 * m21;
    final a31 = m00 * m21 * m32 - m00 * m31 * m22 - m01 * m20 * m32 +
        m01 * m30 * m22 + m02 * m20 * m31 - m02 * m30 * m21;
    final a32 = -m00 * m11 * m32 + m00 * m31 * m12 + m01 * m10 * m32 -
        m01 * m30 * m12 - m02 * m10 * m31 + m02 * m30 * m11;
    final a33 = m00 * m11 * m22 - m00 * m21 * m12 - m01 * m10 * m22 +
        m01 * m20 * m12 + m02 * m10 * m21 - m02 * m20 * m11;

    final det = m00 * a00 + m10 * a01 + m20 * a02 + m30 * a03;
    if (det.abs() < epsilon) return null;
    final invDet = 1.0 / det;
    return Mat4(
      a00 * invDet, a01 * invDet, a02 * invDet, a03 * invDet,
      a10 * invDet, a11 * invDet, a12 * invDet, a13 * invDet,
      a20 * invDet, a21 * invDet, a22 * invDet, a23 * invDet,
      a30 * invDet, a31 * invDet, a32 * invDet, a33 * invDet,
    );
  }

  /// Transforms a point (w = 1) including translation and perspective
  /// divide. The divide is skipped when w is ~0 or ~1.
  Vec3 transformPoint(Vec3 p) {
    final x = m00 * p.x + m01 * p.y + m02 * p.z + m03;
    final y = m10 * p.x + m11 * p.y + m12 * p.z + m13;
    final z = m20 * p.x + m21 * p.y + m22 * p.z + m23;
    final w = m30 * p.x + m31 * p.y + m32 * p.z + m33;
    if (w.abs() < epsilon || w == 1.0) return Vec3(x, y, z);
    return Vec3(x / w, y / w, z / w);
  }

  /// Transforms a vector (w = 0): applies the upper-left 3x3 (rotation
  /// and scale) without translation and without perspective divide.
  Vec3 transformVector(Vec3 v) => Vec3(
        m00 * v.x + m01 * v.y + m02 * v.z,
        m10 * v.x + m11 * v.y + m12 * v.z,
        m20 * v.x + m21 * v.y + m22 * v.z,
      );

  /// Transforms a direction applying the rotation part only (scale
  /// removed). The upper-left 3x3 columns are normalized to form a
  /// pure rotation; this is correct for TRS matrices with positive
  /// scale.
  Vec3 transformDirection(Vec3 v) {
    final cx = Vec3(m00, m10, m20).normalized();
    final cy = Vec3(m01, m11, m21).normalized();
    final cz = Vec3(m02, m12, m22).normalized();
    return cx * v.x + cy * v.y + cz * v.z;
  }

  /// Transforms a full [Vec4] (no perspective divide).
  Vec4 transformVec4(Vec4 v) => Vec4(
        m00 * v.x + m01 * v.y + m02 * v.z + m03 * v.w,
        m10 * v.x + m11 * v.y + m12 * v.z + m13 * v.w,
        m20 * v.x + m21 * v.y + m22 * v.z + m23 * v.w,
        m30 * v.x + m31 * v.y + m32 * v.z + m33 * v.w,
      );

  /// Translation component (m03, m13, m23).
  Vec3 get translation => Vec3(m03, m13, m23);

  /// Scale component as the length of each basis column. Negative
  /// scales are reported positive.
  Vec3 get scale => Vec3(
        Vec3(m00, m10, m20).length,
        Vec3(m01, m11, m21).length,
        Vec3(m02, m12, m22).length,
      );

  /// Right basis vector (column 0).
  Vec3 get right => Vec3(m00, m10, m20);

  /// Up basis vector (column 1).
  Vec3 get up => Vec3(m01, m11, m21);

  /// Forward basis vector (column 2).
  Vec3 get forward => Vec3(m02, m12, m22);

  // ---- Static constructors -------------------------------------------

  /// Translation by ([tx], [ty], [tz]).
  static Mat4 makeTranslation(double tx, double ty, double tz) => Mat4(
        1.0, 0.0, 0.0, tx,
        0.0, 1.0, 0.0, ty,
        0.0, 0.0, 1.0, tz,
        0.0, 0.0, 0.0, 1.0,
      );

  /// Non-uniform scale by ([sx], [sy], [sz]).
  static Mat4 makeScale(double sx, double sy, double sz) => Mat4(
        sx, 0.0, 0.0, 0.0,
        0.0, sy, 0.0, 0.0,
        0.0, 0.0, sz, 0.0,
        0.0, 0.0, 0.0, 1.0,
      );

  /// Rotation about the X axis by [angle] radians.
  static Mat4 makeRotationX(double angle) {
    final c = math.cos(angle);
    final s = math.sin(angle);
    return Mat4(
      1.0, 0.0, 0.0, 0.0,
      0.0, c, -s, 0.0,
      0.0, s, c, 0.0,
      0.0, 0.0, 0.0, 1.0,
    );
  }

  /// Rotation about the Y axis by [angle] radians.
  static Mat4 makeRotationY(double angle) {
    final c = math.cos(angle);
    final s = math.sin(angle);
    return Mat4(
      c, 0.0, s, 0.0,
      0.0, 1.0, 0.0, 0.0,
      -s, 0.0, c, 0.0,
      0.0, 0.0, 0.0, 1.0,
    );
  }

  /// Rotation about the Z axis by [angle] radians.
  static Mat4 makeRotationZ(double angle) {
    final c = math.cos(angle);
    final s = math.sin(angle);
    return Mat4(
      c, -s, 0.0, 0.0,
      s, c, 0.0, 0.0,
      0.0, 0.0, 1.0, 0.0,
      0.0, 0.0, 0.0, 1.0,
    );
  }

  /// Perspective projection mapping to NDC z in [-1, 1].
  ///
  /// [fovYRadians] is the full vertical field of view, [aspect] is
  /// width/height, and [near]/[far] are positive distances to the
  /// clip planes.
  static Mat4 makePerspective(
      double fovYRadians, double aspect, double near, double far) {
    final f = 1.0 / math.tan(fovYRadians * 0.5);
    final nf = 1.0 / (near - far);
    return Mat4(
      f / aspect, 0.0, 0.0, 0.0,
      0.0, f, 0.0, 0.0,
      0.0, 0.0, (far + near) * nf, 2.0 * far * near * nf,
      0.0, 0.0, -1.0, 0.0,
    );
  }

  /// Orthographic projection mapping to NDC z in [-1, 1].
  static Mat4 makeOrthographic(
      double left, double right, double bottom, double top, double near,
      double far) {
    final rl = 1.0 / (right - left);
    final tb = 1.0 / (top - bottom);
    final fn = 1.0 / (far - near);
    return Mat4(
      2.0 * rl, 0.0, 0.0, -(right + left) * rl,
      0.0, 2.0 * tb, 0.0, -(top + bottom) * tb,
      0.0, 0.0, -2.0 * fn, -(far + near) * fn,
      0.0, 0.0, 0.0, 1.0,
    );
  }

  /// View matrix looking from [eye] toward [target] with [up] as the
  /// reference up direction. The resulting matrix transforms
  /// world-space points into view space.
  static Mat4 makeLookAt(Vec3 eye, Vec3 target, Vec3 up) {
    final f = (target - eye).normalized();
    final s = f.cross(up.normalized()).normalized();
    final u = s.cross(f);
    return Mat4(
      s.x, s.y, s.z, -s.dot(eye),
      u.x, u.y, u.z, -u.dot(eye),
      -f.x, -f.y, -f.z, f.dot(eye),
      0.0, 0.0, 0.0, 1.0,
    );
  }

  @override
  String toString() => 'Mat4(\n'
      '  $m00, $m01, $m02, $m03,\n'
      '  $m10, $m11, $m12, $m13,\n'
      '  $m20, $m21, $m22, $m23,\n'
      '  $m30, $m31, $m32, $m33,\n)';
}
