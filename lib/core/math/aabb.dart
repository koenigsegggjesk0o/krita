// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// aabb.dart — Immutable axis-aligned bounding box in 3D.
//
// Stores [min] and [max] corners. Provides containment, intersection,
// ray intersection (slab method), expansion, merge, center/size
// accessors, and a transform that re-fits from the 8 transformed
// corners.

import 'mat4.dart';
import 'math_utils.dart';
import 'ray.dart';
import 'vec3.dart';

/// An immutable axis-aligned bounding box.
class Aabb {
  /// Minimum corner.
  final Vec3 min;

  /// Maximum corner.
  final Vec3 max;

  /// Constructs a box from explicit [min] and [max] corners.
  const Aabb(this.min, this.max);

  /// Empty (inverted) box ready for [expand] / [merge] accumulation.
  const Aabb.empty()
      : min = const Vec3(double.infinity, double.infinity, double.infinity),
        max = const Vec3(
            double.negativeInfinity, double.negativeInfinity,
            double.negativeInfinity);

  /// Constructs a box that tightly contains [points].
  factory Aabb.fromPoints(Iterable<Vec3> points) {
    var mn = const Vec3(
        double.infinity, double.infinity, double.infinity);
    var mx = const Vec3(double.negativeInfinity, double.negativeInfinity,
        double.negativeInfinity);
    for (final p in points) {
      mn = mn.min(p);
      mx = mx.max(p);
    }
    return Aabb(mn, mx);
  }

  /// Constructs a box from a [center] and full [extent] (size) vector.
  factory Aabb.fromCenterExtent(Vec3 center, Vec3 extent) {
    final half = extent * 0.5;
    return Aabb(center - half, center + half);
  }

  /// Center of the box.
  Vec3 get center => (min + max) * 0.5;

  /// Full size (max - min).
  Vec3 get size => max - min;

  /// Half extents.
  Vec3 get extents => size * 0.5;

  /// True when the box is degenerate (any min > max).
  bool get isEmpty =>
      min.x > max.x || min.y > max.y || min.z > max.z;

  /// True when [p] lies inside the box (boundaries inclusive).
  bool contains(Vec3 p) =>
      p.x >= min.x &&
      p.x <= max.x &&
      p.y >= min.y &&
      p.y <= max.y &&
      p.z >= min.z &&
      p.z <= max.z;

  /// True when [other] lies entirely within this box.
  bool containsBox(Aabb other) =>
      min.x <= other.min.x &&
      max.x >= other.max.x &&
      min.y <= other.min.y &&
      max.y >= other.max.y &&
      min.z <= other.min.z &&
      max.z >= other.max.z;

  /// True when [other] overlaps this box on every axis.
  bool intersects(Aabb other) =>
      min.x <= other.max.x &&
      max.x >= other.min.x &&
      min.y <= other.max.y &&
      max.y >= other.min.y &&
      min.z <= other.max.z &&
      max.z >= other.min.z;

  /// True when a sphere ([center], [radius]) overlaps the box.
  bool intersectsSphere(Vec3 center, double radius) {
    final cx = clampDouble(center.x, min.x, max.x);
    final cy = clampDouble(center.y, min.y, max.y);
    final cz = clampDouble(center.z, min.z, max.z);
    return center.distanceToSquared(Vec3(cx, cy, cz)) <= radius * radius;
  }

  /// Returns a box expanded to include [p].
  Aabb expand(Vec3 p) => Aabb(min.min(p), max.max(p));

  /// Returns the union of this box and [other].
  Aabb merge(Aabb other) =>
      Aabb(min.min(other.min), max.max(other.max));

  /// Returns a box transformed by [m], re-fitted from the 8 corners.
  Aabb transformed(Mat4 m) {
    final corners = <Vec3>[
      Vec3(min.x, min.y, min.z),
      Vec3(max.x, min.y, min.z),
      Vec3(min.x, max.y, min.z),
      Vec3(max.x, max.y, min.z),
      Vec3(min.x, min.y, max.z),
      Vec3(max.x, min.y, max.z),
      Vec3(min.x, max.y, max.z),
      Vec3(max.x, max.y, max.z),
    ].map(m.transformPoint);
    return Aabb.fromPoints(corners);
  }

  /// Ray-box intersection (slab method). Returns the nearest positive
  /// ray parameter [t] (world units when [ray.direction] is unit
  /// length), or `null` when the ray misses.
  double? intersectRay(Ray ray, {double epsilon = 1e-9}) {
    var tmin = double.negativeInfinity;
    var tmax = double.infinity;

    final r = _slab(ray.origin.x, ray.direction.x, min.x, max.x, tmin, tmax,
        epsilon);
    if (r == null) return null;
    tmin = r.$1;
    tmax = r.$2;

    final ry = _slab(ray.origin.y, ray.direction.y, min.y, max.y, tmin, tmax,
        epsilon);
    if (ry == null) return null;
    tmin = ry.$1;
    tmax = ry.$2;

    final rz = _slab(ray.origin.z, ray.direction.z, min.z, max.z, tmin, tmax,
        epsilon);
    if (rz == null) return null;
    tmin = rz.$1;
    tmax = rz.$2;

    if (tmax < epsilon) return null;
    return tmin > epsilon ? tmin : tmax;
  }

  static (double, double)? _slab(double o, double d, double lo, double hi,
      double tmin, double tmax, double epsilon) {
    if (d.abs() < epsilon) {
      if (o < lo || o > hi) return null;
      return (tmin, tmax);
    }
    var t1 = (lo - o) / d;
    var t2 = (hi - o) / d;
    if (t1 > t2) {
      final tmp = t1;
      t1 = t2;
      t2 = tmp;
    }
    final newMin = t1 > tmin ? t1 : tmin;
    final newMax = t2 < tmax ? t2 : tmax;
    if (newMin > newMax) return null;
    return (newMin, newMax);
  }

  @override
  bool operator ==(Object other) =>
      other is Aabb && min == other.min && max == other.max;

  @override
  int get hashCode => Object.hash(min, max);

  @override
  String toString() => 'Aabb(min: $min, max: $max)';
}
