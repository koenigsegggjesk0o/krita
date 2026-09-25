// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// vec4.dart — Immutable 4D vector used for homogeneous coordinates,
// RGBA colors, and quaternion-like payloads.
//
// Quaternion math lives in quaternion.dart; this type is the raw 4-tuple
// used wherever a 4-component value is needed without rotation
// semantics (e.g. packed colors, projected clip-space positions).

import 'dart:math' as math;

import 'math_utils.dart';
import 'vec3.dart';

/// An immutable 4D vector.
class Vec4 {
  /// X component.
  final double x;

  /// Y component.
  final double y;

  /// Z component.
  final double z;

  /// W component (homogeneous weight or alpha).
  final double w;

  /// Constructs a vector with components [x], [y], [z], [w].
  const Vec4(this.x, this.y, this.z, this.w);

  /// Zero vector.
  const Vec4.zero() : x = 0.0, y = 0.0, z = 0.0, w = 0.0;

  /// Vector with all components equal to [v].
  const Vec4.all(double v) : x = v, y = v, z = v, w = v;

  /// Homogeneous identity (0, 0, 0, 1).
  const Vec4.identity() : x = 0.0, y = 0.0, z = 0.0, w = 1.0;

  /// Promotes a [Vec3] with a homogeneous [w] (default 1.0).
  factory Vec4.fromVec3(Vec3 v, [double w = 1.0]) =>
      Vec4(v.x, v.y, v.z, w);

  /// Constructs from a [List] of at least four doubles.
  factory Vec4.fromList(List<double> v) => Vec4(v[0], v[1], v[2], v[3]);

  /// Component-wise addition.
  Vec4 operator +(Vec4 o) => Vec4(x + o.x, y + o.y, z + o.z, w + o.w);

  /// Component-wise subtraction.
  Vec4 operator -(Vec4 o) => Vec4(x - o.x, y - o.y, z - o.z, w - o.w);

  /// Scalar multiplication.
  Vec4 operator *(double s) => Vec4(x * s, y * s, z * s, w * s);

  /// Scalar division.
  Vec4 operator /(double s) => Vec4(x / s, y / s, z / s, w / s);

  /// Negation.
  Vec4 operator -() => Vec4(-x, -y, -z, -w);

  /// Exact equality on all components.
  @override
  bool operator ==(Object other) =>
      other is Vec4 &&
      x == other.x &&
      y == other.y &&
      z == other.z &&
      w == other.w;

  @override
  int get hashCode => Object.hash(x, y, z, w);

  /// Component-wise multiplication (Hadamard product).
  Vec4 multiply(Vec4 o) => Vec4(x * o.x, y * o.y, z * o.z, w * o.w);

  /// Component-wise division.
  Vec4 divide(Vec4 o) => Vec4(x / o.x, y / o.y, z / o.z, w / o.w);

  /// Dot product.
  double dot(Vec4 o) => x * o.x + y * o.y + z * o.z + w * o.w;

  /// Squared length.
  double get length2 => x * x + y * y + z * z + w * w;

  /// Length (magnitude).
  double get length => math.sqrt(length2);

  /// Returns a unit-length copy. Returns zero when too small to
  /// normalize safely.
  Vec4 normalized() {
    final l = length;
    if (l < epsilon) return const Vec4.zero();
    return Vec4(x / l, y / l, z / l, w / l);
  }

  /// Linear interpolation toward [o] by [t].
  Vec4 lerp(Vec4 o, double t) => Vec4(
      x + (o.x - x) * t,
      y + (o.y - y) * t,
      z + (o.z - z) * t,
      w + (o.w - w) * t);

  /// Returns the (x, y, z) triple as a [Vec3], ignoring [w].
  Vec3 get xyz => Vec3(x, y, z);

  /// Perspective divide: returns (x/w, y/w, z/w) as a [Vec3]. When [w]
  /// is near zero, returns the raw (x, y, z) to avoid NaNs.
  Vec3 get perspectiveDivide =>
      w.abs() < epsilon ? Vec3(x, y, z) : Vec3(x / w, y / w, z / w);

  /// Returns a copy with components overridden.
  Vec4 copyWith({double? x, double? y, double? z, double? w}) =>
      Vec4(x ?? this.x, y ?? this.y, z ?? this.z, w ?? this.w);

  /// Components as a growable list.
  List<double> toList() => [x, y, z, w];

  @override
  String toString() => 'Vec4($x, $y, $z, $w)';
}
