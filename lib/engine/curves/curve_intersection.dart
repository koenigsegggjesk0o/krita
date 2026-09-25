// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// curve_intersection.dart — Intersection queries between curves and
// other primitives.
//
// Three operations are provided:
//   - [rayCurve]: closest hit(s) between a 3D ray and a [Curve3D].
//     Used for picking strokes in the 3D viewport.
//   - [curveCurve]: intersection(s) of two [Curve3D]s. Uses recursive
//     Bezier-style subdivision with bounding-box overlap tests —
//     robust to all curve types, not just polynomials.
//   - [curvePlane]: parameter values where a curve crosses a plane
//     (defined by a point and normal). Used for cross-section
//     preview and guide-surface trimming.
//
// All algorithms return a list of [CurveIntersection] records
// carrying the parameter on each participating curve, the world-space
// hit point, and the chord distance between the two hit parameters
// (zero for exact intersections).
//
// These are O(N * M) at worst (curve-curve with N and M control
// points) but the bounding-box pruning makes them effectively
// O(log N * log M) for clean inputs.

import 'dart:math' as math;

import 'package:feather_krita/core/math/math.dart';
import 'package:feather_krita/engine/curves/curve3d.dart';

/// A single intersection between two primitives.
class CurveIntersection {
  const CurveIntersection({
    required this.tA,
    required this.point,
    this.tB,
    this.distance = 0.0,
  });

  /// Parameter on the primary curve (the one passed as `curve` /
  /// `a`).
  final double tA;

  /// Parameter on the secondary curve (when this is a curve-curve
  /// intersection). `null` for ray-curve and curve-plane.
  final double? tB;

  /// World-space intersection point.
  final Vector3 point;

  /// For curve-curve: the chord distance between `a.sampleAt(tA)` and
  /// `b.sampleAt(tB)` — zero for exact intersections, positive for
  /// approximate (within `tolerance`). For ray-curve: the distance
  /// from the ray origin to the hit point. For curve-plane: 0.
  final double distance;

  @override
  String toString() =>
      'CurveIntersection(tA=$tA, tB=$tB, point=$point, dist=$distance)';
}

/// Static intersection utilities. Pure functions; safe to call from
/// any isolate.
class CurveIntersectionOps {
  const CurveIntersectionOps._();

  // ----- Ray-curve -----------------------------------------------------

  /// Finds all intersections between a ray (defined by [origin] and
  /// [direction]) and [curve] in the forward direction, sorted by
  /// distance from the ray origin.
  ///
  /// Algorithm: flatten the curve into a polyline, then for each
  /// segment compute the closest point to the ray. If the closest
  /// point lies on both the segment and the ray (distance below
  /// [tolerance]) and the segment "straddles" the ray's perpendicular
  /// plane, refine via Newton and record.
  ///
  /// Returns at most [maxResults] hits (the closest ones).
  static List<CurveIntersection> rayCurve(
    Vector3 origin,
    Vector3 direction,
    Curve3D curve, {
    double tolerance = 0.1,
    int maxResults = 16,
    int resolution = 256,
  }) {
    final flat = curve.flatten(tolerance: tolerance * 0.5);
    if (flat.length < 2) return const <CurveIntersection>[];

    final hits = <CurveIntersection>[];
    final dir = direction.normalized();

    for (var i = 1; i < flat.length; i++) {
      final a = flat[i - 1].point;
      final b = flat[i].point;
      // Closest points on the segment a-b and the ray.
      // Solve: min over s in [0,1], t >= 0 of |a + s*(b-a) - (origin + t*dir)|^2.
      final ab = b - a;
      final ao = a - origin;
      final abLen2 = ab.length2;
      if (abLen2 < 1e-20) continue;
      final dDotAb = dir.dot(ab);
      final dDotAo = dir.dot(ao);
      final abDotAo = ab.dot(ao);
      // Linear system:
      //   |dir.dir  -dir.ab| |t|   |-dir.ao|
      //   |ab.dir  -ab.ab  | |s| = |-ab.ao |
      final denom = dir.dot(dir) * (-abLen2) - (-dDotAb) * dDotAb;
      if (denom.abs() < 1e-12) continue;
      final t = (dir.dot(dir) * (-abDotAo) - (-dDotAb) * (-dDotAo)) /
          denom;
      final s =
          (dDotAb * (-abDotAo) - (-abLen2) * (-dDotAo)) / denom;
      if (s < 0.0 || s > 1.0) continue;
      if (t < 0.0) continue; // behind the ray
      final pSeg = a + ab * s;
      final pRay = origin + dir * t;
      final d = (pSeg - pRay).length;
      if (d > tolerance) continue;
      // Newton refinement on the parameter `tA` of the curve.
      final tALinear =
          flat[i - 1].t + (flat[i].t - flat[i - 1].t) * s;
      final tARefined = _refineRayHit(origin, dir, curve, tALinear);
      final point = curve.sampleAt(tARefined);
      final dist = (point - origin).dot(dir);
      hits.add(CurveIntersection(
        tA: tARefined,
        point: point,
        distance: dist,
      ));
    }
    hits.sort((a, b) => a.distance.compareTo(b.distance));
    if (hits.length > maxResults) {
      hits.removeRange(maxResults, hits.length);
    }
    return hits;
  }

  /// Newton-refines a ray-curve hit by minimizing `|C(t) - (o + t*d)|`.
  static double _refineRayHit(
      Vector3 origin, Vector3 dir, Curve3D curve, double t0) {
    final (tMin, tMax) = curve.domain();
    var t = t0.clamp(tMin, tMax);
    final o = origin;
    final d = dir;
    for (var iter = 0; iter < 8; iter++) {
      final c = curve.sampleAt(t);
      final cd = curve.derivativeAt(t);
      // f(t) = (C(t) - o) . d_perp, where d_perp = d - (d.d)d (component
      // perpendicular to the ray). For a straight ray we want the
      // perpendicular distance from C(t) to the ray, so f(t) = (C(t) - o) -
      // ((C(t) - o).d) d. Minimizing |f|^2 gives the closest point.
      final oc = c - o;
      final proj = oc.dot(d);
      final perp = oc - d * proj;
      final perpLen2 = perp.length2;
      if (perpLen2 < 1e-16) break;
      // d/dt |perp|^2 = 2 * perp . perp' = 2 * perp . (cd - (cd.d) d).
      final cdProj = cd.dot(d);
      final cdPerp = cd - d * cdProj;
      final f = perp.dot(cdPerp);
      // Second derivative (approximate): perp . cdPerp'.
      final h = 1e-5 * (tMax - tMin).abs().clamp(1e-3, 1.0);
      final cd2 = (curve.sampleAt(t + h) - curve.sampleAt(t - h))
          .scaled(0.5 / h);
      final cd2Proj = cd2.dot(d);
      final cd2Perp = cd2 - d * cd2Proj;
      final fPrime = cdPerp.dot(cdPerp) + perp.dot(cd2Perp);
      if (fPrime.abs() < 1e-12) break;
      final step = f / fPrime;
      t = (t - step).clamp(tMin, tMax);
      if (step.abs() < 1e-7 * (tMax - tMin).abs()) break;
    }
    return t;
  }

  // ----- Curve-curve ---------------------------------------------------

  /// Finds all intersections between [a] and [b] within [tolerance].
  ///
  /// Uses recursive subdivision with axis-aligned bounding box
  /// overlap tests: subdivide each curve at its midpoint, recurse on
  /// the 4 child pairs, and emit an intersection when both subcurves'
  /// bounding boxes are smaller than [tolerance] AND overlap.
  static List<CurveIntersection> curveCurve(
    Curve3D a,
    Curve3D b, {
    double tolerance = 0.01,
    int maxDepth = 24,
    int maxResults = 64,
  }) {
    final (aMin, aMax) = a.domain();
    final (bMin, bMax) = b.domain();
    final hits = <CurveIntersection>[];
    _curveCurveRecurse(
      a, b, aMin, aMax, bMin, bMax,
      tolerance, maxDepth, 0, hits,
    );
    // Deduplicate hits that are within `tolerance` of each other in
    // parameter space.
    final deduped = <CurveIntersection>[];
    for (final h in hits) {
      var isNew = true;
      for (final existing in deduped) {
        if ((existing.point - h.point).length < tolerance * 2.0) {
          isNew = false;
          break;
        }
      }
      if (isNew) deduped.add(h);
    }
    if (deduped.length > maxResults) {
      deduped.removeRange(maxResults, deduped.length);
    }
    return deduped;
  }

  static void _curveCurveRecurse(
    Curve3D a,
    Curve3D b,
    double aMin,
    double aMax,
    double bMin,
    double bMax,
    double tolerance,
    int maxDepth,
    int depth,
    List<CurveIntersection> out,
  ) {
    final aBox = _subCurveBounds(a, aMin, aMax);
    final bBox = _subCurveBounds(b, bMin, bMax);
    if (!_aabbIntersects(aBox, bBox)) return;
    final aSize = aBox.max - aBox.min;
    final bSize = bBox.max - bBox.min;
    final aSpan = math.max(aSize.x, math.max(aSize.y, aSize.z));
    final bSpan = math.max(bSize.x, math.max(bSize.y, bSize.z));
    if ((aSpan < tolerance && bSpan < tolerance) || depth >= maxDepth) {
      // Emit the midpoint as the intersection.
      final tA = 0.5 * (aMin + aMax);
      final tB = 0.5 * (bMin + bMax);
      final pA = a.sampleAt(tA);
      final pB = b.sampleAt(tB);
      final point = pA + (pB - pA) * 0.5;
      out.add(CurveIntersection(
        tA: tA,
        tB: tB,
        point: point,
        distance: (pA - pB).length,
      ));
      return;
    }
    final aMid = 0.5 * (aMin + aMax);
    final bMid = 0.5 * (bMin + bMax);
    _curveCurveRecurse(
        a, b, aMin, aMid, bMin, bMid, tolerance, maxDepth, depth + 1, out);
    _curveCurveRecurse(
        a, b, aMin, aMid, bMid, bMax, tolerance, maxDepth, depth + 1, out);
    _curveCurveRecurse(
        a, b, aMid, aMax, bMin, bMid, tolerance, maxDepth, depth + 1, out);
    _curveCurveRecurse(
        a, b, aMid, aMax, bMid, bMax, tolerance, maxDepth, depth + 1, out);
  }

  static Aabb3 _subCurveBounds(Curve3D curve, double tMin, double tMax) {
    final out = Aabb3();
    final samples = 8;
    for (var i = 0; i <= samples; i++) {
      final t = tMin + (tMax - tMin) * i / samples;
      out.hullPoint(curve.sampleAt(t));
    }
    return out;
  }

  /// AABB-AABB overlap test (inclusive on all axes). Returns true if
  /// [a] and [b] share any volume (or face / edge / corner).
  static bool _aabbIntersects(Aabb3 a, Aabb3 b) {
    final aMin = a.min;
    final aMax = a.max;
    final bMin = b.min;
    final bMax = b.max;
    if (aMax.x < bMin.x || bMax.x < aMin.x) return false;
    if (aMax.y < bMin.y || bMax.y < aMin.y) return false;
    if (aMax.z < bMin.z || bMax.z < aMin.z) return false;
    return true;
  }

  // ----- Curve-plane ---------------------------------------------------

  /// Finds all intersections between [curve] and the plane defined
  /// by [point] and [normal] (need not be unit length).
  ///
  /// Algorithm: sample finely, find sign changes of
  /// `f(t) = (C(t) - point) . normal`, refine by bisection.
  static List<CurveIntersection> curvePlane(
    Curve3D curve,
    Vector3 point,
    Vector3 normal, {
    int resolution = 128,
    double epsilon = 1e-6,
  }) {
    final (tMin, tMax) = curve.domain();
    final out = <CurveIntersection>[];
    var prevT = tMin;
    var prevF = (curve.sampleAt(tMin) - point).dot(normal);
    for (var i = 1; i <= resolution; i++) {
      final t = tMin + (tMax - tMin) * i / resolution;
      final f = (curve.sampleAt(t) - point).dot(normal);
      if (prevF * f < 0.0) {
        // Sign change — bisect.
        final hit = _bisectPlane(curve, point, normal, prevT, t,
            prevF, f, epsilon);
        if (hit != null) out.add(hit);
      } else if (prevF.abs() < epsilon) {
        // Exact touch at prevT.
        out.add(CurveIntersection(
          tA: prevT,
          point: curve.sampleAt(prevT),
        ));
      }
      prevT = t;
      prevF = f;
    }
    return out;
  }

  static CurveIntersection? _bisectPlane(
    Curve3D curve,
    Vector3 point,
    Vector3 normal,
    double tLo,
    double tHi,
    double fLo,
    double fHi,
    double epsilon,
  ) {
    var lo = tLo;
    var hi = tHi;
    var flo = fLo;
    for (var i = 0; i < 48; i++) {
      final mid = 0.5 * (lo + hi);
      final fm = (curve.sampleAt(mid) - point).dot(normal);
      if (fm.abs() < epsilon) {
        return CurveIntersection(
          tA: mid,
          point: curve.sampleAt(mid),
        );
      }
      if (flo * fm < 0.0) {
        hi = mid;
        fHi = fm;
      } else {
        lo = mid;
        flo = fm;
      }
    }
    final t = 0.5 * (lo + hi);
    return CurveIntersection(tA: t, point: curve.sampleAt(t));
  }

  // ----- Curve-self-intersection --------------------------------------

  /// Finds self-intersections of [curve] by running [curveCurve]
  /// against itself, skipping adjacent segments. Used by the editor
  /// to flag bad strokes.
  static List<CurveIntersection> selfIntersect(
    Curve3D curve, {
    double tolerance = 0.01,
  }) {
    final hits = <CurveIntersection>[];
    final flat = curve.flatten(tolerance: tolerance * 0.5);
    // Brute-force segment-segment check on the flattened polyline.
    // O(N^2); fine for visual-quality detection (N typically < 200).
    for (var i = 0; i < flat.length - 1; i++) {
      for (var j = i + 2; j < flat.length - 1; j++) {
        final hit = _segmentSegment(
          flat[i].point, flat[i + 1].point,
          flat[j].point, flat[j + 1].point,
        );
        if (hit == null) continue;
        final tA = flat[i].t +
            (flat[i + 1].t - flat[i].t) * hit.$1;
        final tB = flat[j].t +
            (flat[j + 1].t - flat[j].t) * hit.$2;
        hits.add(CurveIntersection(
          tA: tA,
          tB: tB,
          point: hit.$3,
          distance: 0.0,
        ));
      }
    }
    return hits;
  }

  /// Returns `(s, t, point)` if segments a-b and c-d intersect in
  /// their interiors, else null. s and t are in [0, 1].
  static (double, double, Vector3)? _segmentSegment(
      Vector3 a, Vector3 b, Vector3 c, Vector3 d) {
    final ab = b - a;
    final cd = d - c;
    final h = ab.cross(cd);
    final hLen2 = h.length2;
    if (hLen2 < 1e-16) return null; // parallel
    final ac = c - a;
    final s = ac.cross(cd).dot(h) / hLen2;
    final t = ac.cross(ab).dot(h) / hLen2;
    if (s < 0.0 || s > 1.0 || t < 0.0 || t > 1.0) return null;
    final p = a + ab * s;
    return (s, t, p);
  }
}
