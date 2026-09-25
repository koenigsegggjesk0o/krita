// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// vec2.dart — Immutable 2D vector for UV coordinates and screen-space
// positions.
//
// All arithmetic operators return new instances; the receiver is never
// mutated. The type is suitable for UV pairs, screen coordinates, and
// the 2D helpers used by the 3D engine (barycentric projection, etc.).

import 'dart:math' as math;

import 'math_utils.dart';

/// An immutable 2D vector with double-precision components.
class Vec2 {
  /// X component (often "u" for UVs).
  final double x;

  /// Y component (often "v" for UVs).
  final double y;

  /// Constructs a vector with components [x] and [y].
  const Vec2(this.x, this.y);

  /// Zero vector.
  const Vec2.zero() : x = 0.0, y = 0.0;

  /// Unit vector along +X.
  const Vec2.unitX() : x = 1.0, y = 0.0;

  /// Unit vector along +Y.
  const Vec2.unitY() : x = 0.0, y = 1.0;

  /// Constructs a vector with both components equal to [v].
  const Vec2.all(double v) : x = v, y = v;

  /// Constructs a vector from a [List] of at least two doubles.
  factory Vec2.fromList(List<double> v) => Vec2(v[0], v[1]);

  /// Component-wise addition.
  Vec2 operator +(Vec2 o) => Vec2(x + o.x, y + o.y);

  /// Component-wise subtraction.
  Vec2 operator -(Vec2 o) => Vec2(x - o.x, y - o.y);

  /// Scalar multiplication.
  Vec2 operator *(double s) => Vec2(x * s, y * s);

  /// Scalar division. Dividing by zero yields infinity/NaN as per IEEE
  /// 754 — callers should ensure [s] is non-zero.
  Vec2 operator /(double s) => Vec2(x / s, y / s);

  /// Negation.
  Vec2 operator -() => Vec2(-x, -y);

  /// Exact equality on both components.
  @override
  bool operator ==(Object other) =>
      other is Vec2 && x == other.x && y == other.y;

  @override
  int get hashCode => Object.hash(x, y);

  /// Component-wise multiplication (Hadamard product).
  Vec2 multiply(Vec2 o) => Vec2(x * o.x, y * o.y);

  /// Component-wise division.
  Vec2 divide(Vec2 o) => Vec2(x / o.x, y / o.y);

  /// Dot product.
  double dot(Vec2 o) => x * o.x + y * o.y;

  /// Squared length.
  double get length2 => x * x + y * y;

  /// Length (magnitude).
  double get length => math.sqrt(length2);

  /// Euclidean distance to [o].
  double distanceTo(Vec2 o) => (this - o).length;

  /// Squared distance to [o].
  double distanceToSquared(Vec2 o) => (this - o).length2;

  /// Returns a unit-length copy. Returns zero when this vector is too
  /// small to normalize safely.
  Vec2 normalized() {
    final l = length;
    if (l < epsilon) return const Vec2.zero();
    return Vec2(x / l, y / l);
  }

  /// Linear interpolation toward [o] by [t] (0 = this, 1 = [o]).
  Vec2 lerp(Vec2 o, double t) => Vec2(x + (o.x - x) * t, y + (o.y - y) * t);

  /// Signed angle (radians) from this vector to [o], measured
  /// counter-clockwise. The result is in (-PI, PI].
  double angleTo(Vec2 o) {
    final cross = x * o.y - y * o.x;
    final d = dot(o);
    return math.atan2(cross, d);
  }

  /// Returns the perpendicular vector rotated 90° counter-clockwise.
  Vec2 perpendicular() => Vec2(-y, x);

  /// Rotates this vector by [radians] counter-clockwise.
  Vec2 rotated(double radians) {
    final c = math.cos(radians);
    final s = math.sin(radians);
    return Vec2(x * c - y * s, x * s + y * c);
  }

  /// Returns a copy with [x] and/or [y] overridden.
  Vec2 copyWith({double? x, double? y}) => Vec2(x ?? this.x, y ?? this.y);

  /// Components as a growable list.
  List<double> toList() => [x, y];

  @override
  String toString() => 'Vec2($x, $y)';
}
