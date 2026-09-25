// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// vec3.dart — Immutable 3D vector, the workhorse type of the engine.
//
// Provides the full operator set and geometric helpers required by the
// 3D sketching engine: dot, cross, normalize, lerp, angleTo,
// projectOnto, reflect, rotateAround. All operations return new
// instances; the receiver is never mutated.

import 'dart:math' as math;

import 'math_utils.dart';
import 'vec2.dart';

/// An immutable 3D vector with double-precision components.
class Vec3 {
  /// X component.
  final double x;

  /// Y component.
  final double y;

  /// Z component.
  final double z;

  /// Constructs a vector with components [x], [y], [z].
  const Vec3(this.x, this.y, this.z);

  /// Zero vector.
  const Vec3.zero() : x = 0.0, y = 0.0, z = 0.0;

  /// Unit vector along +X.
  const Vec3.unitX() : x = 1.0, y = 0.0, z = 0.0;

  /// Unit vector along +Y.
  const Vec3.unitY() : x = 0.0, y = 1.0, z = 0.0;

  /// Unit vector along +Z.
  const Vec3.unitZ() : x = 0.0, y = 0.0, z = 1.0;

  /// Vector with all components equal to [v].
  const Vec3.all(double v) : x = v, y = v, z = v;

  /// Unit-scaled vector (1, 1, 1).
  const Vec3.one() : x = 1.0, y = 1.0, z = 1.0;

  /// Constructs from a [List] of at least three doubles.
  factory Vec3.fromList(List<double> v) => Vec3(v[0], v[1], v[2]);

  /// Promotes a [Vec2] to 3D by appending [z].
  factory Vec3.fromVec2(Vec2 v, double z) => Vec3(v.x, v.y, z);

  /// Component-wise addition.
  Vec3 operator +(Vec3 o) => Vec3(x + o.x, y + o.y, z + o.z);

  /// Component-wise subtraction.
  Vec3 operator -(Vec3 o) => Vec3(x - o.x, y - o.y, z - o.z);

  /// Scalar multiplication.
  Vec3 operator *(double s) => Vec3(x * s, y * s, z * s);

  /// Scalar division.
  Vec3 operator /(double s) => Vec3(x / s, y / s, z / s);

  /// Negation.
  Vec3 operator -() => Vec3(-x, -y, -z);

  /// Exact equality on all components.
  @override
  bool operator ==(Object other) =>
      other is Vec3 && x == other.x && y == other.y && z == other.z;

  @override
  int get hashCode => Object.hash(x, y, z);

  /// Component-wise multiplication (Hadamard product).
  Vec3 multiply(Vec3 o) => Vec3(x * o.x, y * o.y, z * o.z);

  /// Component-wise division.
  Vec3 divide(Vec3 o) => Vec3(x / o.x, y / o.y, z / o.z);

  /// Dot product.
  double dot(Vec3 o) => x * o.x + y * o.y + z * o.z;

  /// Right-handed cross product: this × [o].
  Vec3 cross(Vec3 o) =>
      Vec3(y * o.z - z * o.y, z * o.x - x * o.z, x * o.y - y * o.x);

  /// Squared length.
  double get length2 => x * x + y * y + z * z;

  /// Length (magnitude).
  double get length => math.sqrt(length2);

  /// Euclidean distance to [o].
  double distanceTo(Vec3 o) => (this - o).length;

  /// Squared distance to [o].
  double distanceToSquared(Vec3 o) => (this - o).length2;

  /// Returns a unit-length copy. Returns zero when this vector is too
  /// small to normalize safely.
  Vec3 normalized() {
    final l = length;
    if (l < epsilon) return const Vec3.zero();
    return Vec3(x / l, y / l, z / l);
  }

  /// Linear interpolation toward [o] by [t].
  Vec3 lerp(Vec3 o, double t) =>
      Vec3(x + (o.x - x) * t, y + (o.y - y) * t, z + (o.z - z) * t);

  /// Unsigned angle (radians) between this and [o], clamped to a valid
  /// acos domain.
  double angleTo(Vec3 o) {
    final d = normalized().dot(o.normalized());
    return math.acos(clampDouble(d, -1.0, 1.0));
  }

  /// Projects this vector onto [o]. The result is parallel to [o].
  /// Returns zero when [o] is degenerate.
  Vec3 projectOnto(Vec3 o) {
    final denom = o.dot(o);
    if (denom.abs() < epsilon) return const Vec3.zero();
    return o * (dot(o) / denom);
  }

  /// Reflects this vector about the unit [normal] using the formula
  /// `r = v - 2*(v·n)*n`. [normal] is normalized internally.
  Vec3 reflect(Vec3 normal) {
    final n = normal.normalized();
    return this - n * (2.0 * dot(n));
  }

  /// Rotates this vector around a unit [axis] by [angle] radians using
  /// Rodrigues' rotation formula. [axis] is normalized internally.
  Vec3 rotateAround(Vec3 axis, double angle) {
    final a = axis.normalized();
    final c = math.cos(angle);
    final s = math.sin(angle);
    final k = a.dot(this);
    return this * c + a.cross(this) * s + a * (k * (1.0 - c));
  }

  /// Component-wise absolute value.
  Vec3 abs() => Vec3(x.abs(), y.abs(), z.abs());

  /// Component-wise minimum between this and [o].
  Vec3 min(Vec3 o) =>
      Vec3(math.min(x, o.x), math.min(y, o.y), math.min(z, o.z));

  /// Component-wise maximum between this and [o].
  Vec3 max(Vec3 o) =>
      Vec3(math.max(x, o.x), math.max(y, o.y), math.max(z, o.z));

  /// Drops the Z component, returning the (x, y) pair.
  Vec2 get xy => Vec2(x, y);

  /// Returns a copy with components overridden.
  Vec3 copyWith({double? x, double? y, double? z}) =>
      Vec3(x ?? this.x, y ?? this.y, z ?? this.z);

  /// Components as a growable list.
  List<double> toList() => [x, y, z];

  @override
  String toString() => 'Vec3($x, $y, $z)';
}
