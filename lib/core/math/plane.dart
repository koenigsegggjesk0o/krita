// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// plane.dart — Immutable 3D plane.
//
// Representation: a unit [normal] and a signed [distance] from the
// origin. Points [p] on the plane satisfy `normal.dot(p) == distance`.
// [pointInFront] returns true when `normal.dot(p) > distance`.

import 'mat4.dart';
import 'ray.dart';
import 'vec3.dart';

/// An immutable plane in 3D space.
class Plane {
  /// Unit normal.
  final Vec3 normal;

  /// Signed distance from origin: `normal.dot(p) == distance` for
  /// points on the plane.
  final double distance;

  /// Constructs a plane from a unit [normal] and signed [distance].
  const Plane(this.normal, this.distance);

  /// Constructs a plane through [point] with the given [normal]. The
  /// normal is normalized internally.
  factory Plane.fromPointNormal(Vec3 point, Vec3 normal) {
    final n = normal.normalized();
    return Plane(n, n.dot(point));
  }

  /// Constructs a plane through three points (counter-clockwise
  /// winding produces a normal following the right-hand rule).
  factory Plane.fromThreePoints(Vec3 a, Vec3 b, Vec3 c) {
    final n = (b - a).cross(c - a).normalized();
    return Plane(n, n.dot(a));
  }

  /// True when [p] lies on the positive side of the plane.
  bool pointInFront(Vec3 p) => normal.dot(p) > distance;

  /// True when [p] lies on the negative side of the plane.
  bool pointBehind(Vec3 p) => normal.dot(p) < distance;

  /// True when [p] is on the plane within [tol].
  bool containsPoint(Vec3 p, {double tol = 1e-6}) =>
      (normal.dot(p) - distance).abs() < tol;

  /// Signed distance from [p] to the plane. Positive on the normal
  /// side, negative behind.
  double distanceToPoint(Vec3 p) => normal.dot(p) - distance;

  /// Returns the intersection point of [ray] with this plane, or
  /// `null` when the ray is parallel to the plane or the hit is
  /// behind the origin (within [epsilon]).
  Vec3? intersectRay(Ray ray, {double epsilon = 1e-9}) {
    final denom = normal.dot(ray.direction);
    if (denom.abs() < epsilon) return null;
    final t = (distance - normal.dot(ray.origin)) / denom;
    if (t < epsilon) return null;
    return ray.pointAt(t);
  }

  /// Returns the intersection point of segment [a]-[b] with this
  /// plane, or `null` when the segment is parallel or does not cross
  /// the plane within its extent.
  Vec3? intersectSegment(Vec3 a, Vec3 b, {double epsilon = 1e-9}) {
    final d = b - a;
    final denom = normal.dot(d);
    if (denom.abs() < epsilon) return null;
    final t = (distance - normal.dot(a)) / denom;
    if (t < 0.0 || t > 1.0) return null;
    return a + d * t;
  }

  /// Transforms the plane by [m]. The translation is applied to a
  /// point on the plane; the normal is transformed as a direction
  /// (rotation only, scale removed).
  Plane transformed(Mat4 m) {
    final pointOnPlane = normal * distance;
    return Plane.fromPointNormal(
        m.transformPoint(pointOnPlane), m.transformDirection(normal));
  }

  @override
  bool operator ==(Object other) =>
      other is Plane && normal == other.normal && distance == other.distance;

  @override
  int get hashCode => Object.hash(normal, distance);

  @override
  String toString() => 'Plane(normal: $normal, distance: $distance)';
}
