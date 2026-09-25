// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// sphere.dart — Immutable bounding sphere in 3D.
//
// Stores a [center] and [radius]. Provides containment, sphere-sphere
// and sphere-AABB intersection, a tight two-sphere merge, and a
// transform that scales the radius by the largest axis scale of [m].

import 'dart:math' as math;

import 'aabb.dart';
import 'mat4.dart';
import 'vec3.dart';

/// An immutable bounding sphere.
class Sphere {
  /// Sphere center.
  final Vec3 center;

  /// Sphere radius.
  final double radius;

  /// Constructs a sphere from [center] and [radius].
  const Sphere(this.center, this.radius);

  /// A zero-radius sphere at the origin.
  const Sphere.empty()
      : center = const Vec3.zero(),
        radius = 0.0;

  /// Constructs an approximate bounding sphere from [points]: the
  /// centroid is the average, and the radius is the distance to the
  /// farthest point. This is the Ritter seed, not the minimal sphere.
  factory Sphere.fromPoints(Iterable<Vec3> points) {
    final list = points.toList();
    if (list.isEmpty) return const Sphere.empty();
    var c = const Vec3.zero();
    for (final p in list) {
      c = c + p;
    }
    c = c / list.length.toDouble();
    var r = 0.0;
    for (final p in list) {
      final d = c.distanceTo(p);
      if (d > r) r = d;
    }
    return Sphere(c, r);
  }

  /// True when [p] lies inside (or on) the sphere.
  bool contains(Vec3 p) => center.distanceToSquared(p) <= radius * radius;

  /// True when [other] overlaps this sphere.
  bool intersects(Sphere other) {
    final r = radius + other.radius;
    return center.distanceToSquared(other.center) <= r * r;
  }

  /// True when the sphere overlaps [box].
  bool intersectsAabb(Aabb box) => box.intersectsSphere(center, radius);

  /// Returns the smallest sphere enclosing both this and [other].
  /// When one sphere already contains the other, that sphere is
  /// returned unchanged.
  Sphere merge(Sphere other) {
    final d = center.distanceTo(other.center);
    if (d + other.radius <= radius) return this;
    if (d + radius <= other.radius) return other;
    final newR = (d + radius + other.radius) * 0.5;
    final dir = (other.center - center).normalized();
    final newC = center + dir * (newR - radius);
    return Sphere(newC, newR);
  }

  /// Transforms the sphere by [m]. The center is transformed as a
  /// point; the radius is scaled by the largest axis scale of [m]'s
  /// upper-left 3x3.
  Sphere transformed(Mat4 m) {
    final newC = m.transformPoint(center);
    final sx = Vec3(m.m00, m.m10, m.m20).length;
    final sy = Vec3(m.m01, m.m11, m.m21).length;
    final sz = Vec3(m.m02, m.m12, m.m22).length;
    final s = math.max(sx, math.max(sy, sz));
    return Sphere(newC, radius * s);
  }

  @override
  bool operator ==(Object other) =>
      other is Sphere && center == other.center && radius == other.radius;

  @override
  int get hashCode => Object.hash(center, radius);

  @override
  String toString() => 'Sphere(center: $center, radius: $radius)';
}
