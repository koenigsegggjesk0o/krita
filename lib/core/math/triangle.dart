// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// triangle.dart — Immutable 3D triangle.
//
// Provides geometric queries used by the sketching engine's picking,
// snapping, and tessellation passes: normal, area, centroid,
// barycentric coordinates, closest point (Ericson), containment test,
// and ray intersection (Möller-Trumbore).

import 'math_utils.dart';
import 'ray.dart';
import 'vec3.dart';

/// An immutable triangle in 3D space.
class Triangle {
  /// First vertex.
  final Vec3 a;

  /// Second vertex.
  final Vec3 b;

  /// Third vertex.
  final Vec3 c;

  /// Constructs a triangle from three vertices.
  const Triangle(this.a, this.b, this.c);

  /// Unit normal following the right-hand rule for CCW winding.
  Vec3 get normal => (b - a).cross(c - a).normalized();

  /// Centroid (average of vertices).
  Vec3 get centroid => (a + b + c) / 3.0;

  /// Area (half the magnitude of the edge cross product).
  double get area => (b - a).cross(c - a).length * 0.5;

  /// Barycentric coordinates of [p] relative to this triangle. The
  /// returned [Vec3] holds weights (a, b, c) that sum to 1. Returns
  /// `null` for degenerate triangles.
  Vec3? barycentricCoords(Vec3 p) {
    final v0 = b - a;
    final v1 = c - a;
    final v2 = p - a;
    final d00 = v0.dot(v0);
    final d01 = v0.dot(v1);
    final d11 = v1.dot(v1);
    final d20 = v2.dot(v0);
    final d21 = v2.dot(v1);
    final denom = d00 * d11 - d01 * d01;
    if (denom.abs() < epsilon) return null;
    final inv = 1.0 / denom;
    final v = (d11 * d20 - d01 * d21) * inv;
    final w = (d00 * d21 - d01 * d20) * inv;
    final u = 1.0 - v - w;
    return Vec3(u, v, w);
  }

  /// True when [p] lies inside (or on the boundary of) the triangle,
  /// using barycentric coordinates with tolerance [tol].
  bool containsPoint(Vec3 p, {double tol = 1e-6}) {
    final bary = barycentricCoords(p);
    if (bary == null) return false;
    return bary.x >= -tol && bary.y >= -tol && bary.z >= -tol;
  }

  /// Closest point on the triangle to [p] (Ericson, Real-Time
  /// Collision Detection §5.1.5). Handles vertices, edges, and the
  /// interior face correctly.
  Vec3 closestPoint(Vec3 p) {
    final ab = b - a;
    final ac = c - a;
    final ap = p - a;
    final d1 = ab.dot(ap);
    final d2 = ac.dot(ap);
    if (d1 <= 0.0 && d2 <= 0.0) return a; // vertex region A

    final bp = p - b;
    final d3 = ab.dot(bp);
    final d4 = ac.dot(bp);
    if (d3 >= 0.0 && d4 <= d3) return b; // vertex region B

    final vc = d1 * d4 - d3 * d2;
    if (vc <= 0.0 && d1 >= 0.0 && d3 <= 0.0) {
      final v = d1 / (d1 - d3);
      return a + ab * v; // edge AB
    }

    final cp = p - c;
    final d5 = ab.dot(cp);
    final d6 = ac.dot(cp);
    if (d6 >= 0.0 && d5 <= d6) return c; // vertex region C

    final vb = d5 * d2 - d1 * d6;
    if (vb <= 0.0 && d2 >= 0.0 && d6 <= 0.0) {
      final w = d2 / (d2 - d6);
      return a + ac * w; // edge AC
    }

    final va = d3 * d6 - d5 * d4;
    if (va <= 0.0 && (d4 - d3) >= 0.0 && (d5 - d6) >= 0.0) {
      final denom2 = (d4 - d3) + (d5 - d6);
      if (denom2 > epsilon) {
        final w = (d4 - d3) / denom2;
        return b + (c - b) * w; // edge BC
      }
    }

    final denomSum = va + vb + vc;
    if (denomSum.abs() < epsilon) return a; // degenerate fallback
    final denom = 1.0 / denomSum;
    final v = vb * denom;
    final w = vc * denom;
    return a + ab * v + ac * w; // interior face
  }

  /// Ray-triangle intersection (Möller-Trumbore). Returns the hit
  /// point, or `null` when the ray misses, is parallel, or hits
  /// behind the origin. Set [cullBackfaces] to reject triangles
  /// facing away from the ray direction.
  Vec3? intersectRay(Ray ray,
      {double epsilon = 1e-9, bool cullBackfaces = false}) {
    final edge1 = b - a;
    final edge2 = c - a;
    final h = ray.direction.cross(edge2);
    final aa = edge1.dot(h);
    if (aa > -epsilon && aa < epsilon) return null; // parallel
    if (cullBackfaces && aa < 0.0) return null;
    final f = 1.0 / aa;
    final s = ray.origin - a;
    final u = f * s.dot(h);
    if (u < 0.0 || u > 1.0) return null;
    final q = s.cross(edge1);
    final v = f * ray.direction.dot(q);
    if (v < 0.0 || u + v > 1.0) return null;
    final t = f * edge2.dot(q);
    if (t < epsilon) return null; // behind origin
    return ray.pointAt(t);
  }

  @override
  bool operator ==(Object other) =>
      other is Triangle && a == other.a && b == other.b && c == other.c;

  @override
  int get hashCode => Object.hash(a, b, c);

  @override
  String toString() => 'Triangle($a, $b, $c)';
}
