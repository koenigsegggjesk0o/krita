// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// ray.dart — Immutable ray in 3D space.
//
// A ray has an [origin] and a [direction]. The direction is expected to
// be normalized for parameter [t] to be in world units; callers are
// responsible for normalization but the helpers here do not enforce it.

import 'mat4.dart';
import 'vec3.dart';

/// An immutable 3D ray.
class Ray {
  /// Ray origin.
  final Vec3 origin;

  /// Ray direction. Should be normalized for distance semantics.
  final Vec3 direction;

  /// Constructs a ray from [origin] and [direction].
  const Ray(this.origin, this.direction);

  /// A degenerate ray at the origin pointing along +Z.
  const Ray.zero()
      : origin = const Vec3.zero(),
        direction = const Vec3.unitZ();

  /// Constructs a ray with an explicitly normalized direction.
  factory Ray.normalized(Vec3 origin, Vec3 direction) =>
      Ray(origin, direction.normalized());

  /// Point at parameter [t]: `origin + direction * t`.
  Vec3 pointAt(double t) => origin + direction * t;

  /// Transforms the ray by [m]. The origin is transformed as a point
  /// and the direction as a vector (no translation), then re-normalized
  /// so the parameter stays in world units.
  Ray transformed(Mat4 m) =>
      Ray(m.transformPoint(origin), m.transformVector(direction).normalized());

  @override
  bool operator ==(Object other) =>
      other is Ray && origin == other.origin && direction == other.direction;

  @override
  int get hashCode => Object.hash(origin, direction);

  @override
  String toString() => 'Ray(origin: $origin, dir: $direction)';
}
