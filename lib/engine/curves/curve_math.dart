// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// curve_math.dart — Curve math utilities shared by all Curve3D subclasses.
//
// The classic 3D-curve primitives — de Casteljau evaluation, chord-length
// parameterization, adaptive subdivision, arc-length tables, discrete
// curvature — are all generic over a parametric evaluator
// `Vector3 Function(double t)`. They live here, not on the base
// `Curve3D` class, so they can be reused by both the abstract base and
// the free-form spline / NURBS classes that don't inherit it.
//
// All algorithms are textbook-faithful and use the standard numeric
// safeguards (epsilon guards, recursion-depth limits, linear interp
// fallbacks when a curvature estimate degenerates).

import 'dart:math' as math;

import 'package:feather_krita/core/math/math.dart';

/// A sample of an arc-length reparameterisation table: a sorted list of
/// `(t, s)` pairs where `t` is the curve parameter and `s` is the
/// cumulative arc length up to that `t`.
class ArcLengthTable {
  ArcLengthTable(this.entries)
      : assert(entries.isNotEmpty, 'ArcLengthTable requires >= 1 entry'),
        assert(_isSorted(entries), 'ArcLengthTable entries must be sorted by t');

  final List<({double t, double s})> entries;

  /// Total arc length of the curve (s at the final sample).
  double get total => entries.last.s;

  static bool _isSorted(List<({double t, double s})> e) {
    for (var i = 1; i < e.length; i++) {
      if (e[i].t < e[i - 1].t) return false;
    }
    return true;
  }

  /// Returns the parameter `t` whose arc length matches [s] (clamped to
  /// `[0, total]`). Linear search + linear interp; table sizes are
  /// small (<= 256 samples) so this is plenty fast.
  double parameterAt(double s) {
    if (s <= 0.0) return entries.first.t;
    if (s >= total) return entries.last.t;
    // Binary search for the bracketing pair.
    var lo = 0;
    var hi = entries.length - 1;
    while (hi - lo > 1) {
      final mid = (lo + hi) >> 1;
      if (entries[mid].s < s) {
        lo = mid;
      } else {
        hi = mid;
      }
    }
    final a = entries[lo];
    final b = entries[hi];
    final ds = b.s - a.s;
    if (ds < 1e-12) return a.t;
    final alpha = (s - a.s) / ds;
    return a.t + (b.t - a.t) * alpha;
  }

  /// Inverse of [parameterAt]: returns the cumulative arc length at
  /// parameter [t] (clamped to the table's t-range).
  double arcLengthAt(double t) {
    if (t <= entries.first.t) return 0.0;
    if (t >= entries.last.t) return total;
    var lo = 0;
    var hi = entries.length - 1;
    while (hi - lo > 1) {
      final mid = (lo + hi) >> 1;
      if (entries[mid].t < t) {
        lo = mid;
      } else {
        hi = mid;
      }
    }
    final a = entries[lo];
    final b = entries[hi];
    final dt = b.t - a.t;
    if (dt < 1e-12) return a.s;
    final alpha = (t - a.t) / dt;
    return a.s + (b.s - a.s) * alpha;
  }
}

/// Static curve math utilities. Pure functions; safe to call from any
/// isolate.
class CurveMath {
  const CurveMath._();

  // ----- de Casteljau ---------------------------------------------------

  /// Evaluates a Bezier curve defined by [points] at parameter [t]
  /// (typically in [0, 1]) using the de Casteljau algorithm. Mutates
  /// nothing; allocates `O(n^2)` temporary storage but n is small
  /// (typically <= 6 control points).
  static Vector3 deCasteljau(List<Vector3> points, double t) {
    if (points.isEmpty) return Vector3.zero();
    if (points.length == 1) return points.first.clone();
    final u = 1.0 - t;
    // Work on a scratch copy so the caller's list is untouched.
    final b = List<Vector3>.generate(points.length, (i) => points[i].clone());
    final n = b.length - 1;
    for (var r = 1; r <= n; r++) {
      for (var i = 0; i <= n - r; i++) {
        b[i] = b[i] * u + b[i + 1] * t;
      }
    }
    return b[0];
  }

  /// First derivative of a Bezier curve of degree `n = points.length - 1`
  /// at [t]. Uses the well-known identity: the derivative of a degree-n
  /// Bezier is `n * sum_{i=0}^{n-1} (P_{i+1} - P_i) * B_{i,n-1}(t)`.
  static Vector3 deCasteljauDerivative(List<Vector3> points, double t) {
    if (points.length < 2) return Vector3.zero();
    final n = points.length - 1;
    final diff = List<Vector3>.generate(
        n, (i) => points[i + 1] - points[i]);
    return deCasteljau(diff, t) * n.toDouble();
  }

  /// Second derivative of a Bezier curve at [t]. Used by curvature
  /// estimation.
  static Vector3 deCasteljauSecondDerivative(List<Vector3> points, double t) {
    if (points.length < 3) return Vector3.zero();
    final n = points.length - 1;
    final diff = List<Vector3>.generate(
        n - 1, (i) => points[i + 2] - points[i + 1] * 2.0 + points[i]);
    return deCasteljau(diff, t) * (n * (n - 1));
  }

  /// Splits a Bezier curve defined by [points] at parameter [t] into
  /// two Bezier curves that exactly represent the same shape. Uses the
  /// de Casteljau subdivision. Returns `(left, right)` where `left`
  /// covers `[0, t]` and `right` covers `[t, 1]`.
  static (List<Vector3> left, List<Vector3> right) deCasteljauSplit(
      List<Vector3> points, double t) {
    if (points.length < 2) {
      return (<Vector3>[points.first.clone()], <Vector3>[points.first.clone()]);
    }
    final n = points.length;
    final u = 1.0 - t;
    final scratch = List<Vector3>.generate(n, (i) => points[i].clone());
    final left = List<Vector3>.filled(n, Vector3.zero(), growable: true);
    final right = List<Vector3>.filled(n, Vector3.zero(), growable: true);
    left[0] = scratch[0].clone();
    right[n - 1] = scratch[n - 1].clone();
    for (var r = 1; r < n; r++) {
      for (var i = 0; i < n - r; i++) {
        scratch[i] = scratch[i] * u + scratch[i + 1] * t;
      }
      left[r] = scratch[0].clone();
      right[n - 1 - r] = scratch[n - 1 - r].clone();
    }
    return (left, right);
  }

  // ----- Parameterization helpers ---------------------------------------

  /// Chord-length parameterization of [points]. Returns a list of `t`
  /// values in `[0, 1]` where the spacing between consecutive t's is
  /// proportional to the Euclidean distance between consecutive
  /// points. This is the standard input conditioning step for spline
  /// fitting (Catmull-Rom, least-squares B-spline, etc.).
  static List<double> chordLengthParameterize(List<Vector3> points) {
    if (points.isEmpty) return const <double>[];
    if (points.length == 1) return const <double>[0.0];
    final ts = List<double>.filled(points.length, 0.0);
    var total = 0.0;
    for (var i = 1; i < points.length; i++) {
      total += (points[i] - points[i - 1]).length;
      ts[i] = total;
    }
    if (total < 1e-12) {
      // Degenerate input: distribute uniformly.
      for (var i = 0; i < ts.length; i++) {
        ts[i] = i.toDouble() / (ts.length - 1);
      }
      return ts;
    }
    for (var i = 0; i < ts.length; i++) {
      ts[i] /= total;
    }
    return ts;
  }

  // ----- Arc length -----------------------------------------------------

  /// Builds an [ArcLengthTable] by sampling [evaluate] at [samples]+1
  /// parameter values uniformly in `[t0, t1]`. Uses cumulative
  /// chord distance (good enough for visual work; the underlying
  /// integral is `|C'(t)| dt`).
  static ArcLengthTable arcLengthTable(
      Vector3 Function(double) evaluate,
      {int samples = 128,
      double t0 = 0.0,
      double t1 = 1.0}) {
    final entries = <({double t, double s})>[];
    var prev = evaluate(t0);
    var s = 0.0;
    entries.add((t: t0, s: 0.0));
    for (var i = 1; i <= samples; i++) {
      final t = t0 + (t1 - t0) * i / samples;
      final p = evaluate(t);
      s += (p - prev).length;
      entries.add((t: t, s: s));
      prev = p;
    }
    return ArcLengthTable(entries);
  }

  /// Computes the arc length of [evaluate] over `[t0, t1]` by Gauss-
  /// Legendre quadrature with 5 nodes (sufficient for smooth splines;
  /// for highly oscillatory curves call [arcLengthTable] and read
  /// `.total`).
  static double arcLength(Vector3 Function(double) evaluate,
      {double t0 = 0.0, double t1 = 1.0}) {
    // 5-point Gauss-Legendre nodes/weights on [-1, 1].
    const nodes = [
      0.0000000000000000,
      -0.5384693101056831,
      0.5384693101056831,
      -0.9061798459386640,
      0.9061798459386640,
    ];
    const weights = [
      0.5688888888888889,
      0.4786286704993665,
      0.4786286704993665,
      0.2369268850561891,
      0.2369268850561891,
    ];
    final half = 0.5 * (t1 - t0);
    final mid = 0.5 * (t1 + t0);
    var sum = 0.0;
    for (var i = 0; i < nodes.length; i++) {
      final t = mid + half * nodes[i];
      // |C'(t)| via central difference.
      final h = 1e-5 * (t1 - t0).abs().clamp(1e-6, 1.0);
      final d = (evaluate(t + h) - evaluate(t - h)) * (0.5 / h);
      sum += weights[i] * d.length;
    }
    return half * sum;
  }

  // ----- Adaptive subdivision -------------------------------------------

  /// Recursively subdivides `[t0, t1]` until the chord between
  /// `evaluate(t0)` and `evaluate(t1)` is within [tolerance] of the
  /// midpoint sample. Returns the list of `t` breakpoints (excluding
  /// `t1`; caller should append it). [maxDepth] caps the recursion so
  /// a degenerate / C1-discontinuous curve can't blow the stack.
  static List<double> adaptiveSubdivide(
      Vector3 Function(double) evaluate,
      double t0,
      double t1, {
        double tolerance = 0.25,
        int maxDepth = 16,
      }) {
    final out = <double>[t0];
    _subdivideRecursive(out, evaluate, t0, t1, tolerance, maxDepth, 0);
    return out;
  }

  static void _subdivideRecursive(
      List<double> out,
      Vector3 Function(double) evaluate,
      double t0,
      double t1,
      double tolerance,
      int maxDepth,
      int depth) {
    if (depth >= maxDepth) {
      out.add(t1);
      return;
    }
    final p0 = evaluate(t0);
    final p1 = evaluate(t1);
    final mid = 0.5 * (t0 + t1);
    final pmid = evaluate(mid);
    // Distance from the midpoint to the chord — flatness metric.
    final chord = p1 - p0;
    final chordLen2 = chord.length2;
    double d2;
    if (chordLen2 < 1e-20) {
      // Degenerate chord — use direct distance to midpoint.
      d2 = (pmid - p0).length2;
    } else {
      final rel = (pmid - p0).dot(chord) / chordLen2;
      final proj = p0 + chord * rel.clamp(0.0, 1.0);
      d2 = (pmid - proj).length2;
    }
    if (d2 <= tolerance * tolerance) {
      out.add(t1);
      return;
    }
    _subdivideRecursive(out, evaluate, t0, mid, tolerance, maxDepth, depth + 1);
    _subdivideRecursive(out, evaluate, mid, t1, tolerance, maxDepth, depth + 1);
  }

  // ----- Curvature ------------------------------------------------------

  /// Discrete curvature at the middle point of three samples, using
  /// the circumcircle formula: `k = 4 * area / (|a-b| * |b-c| * |c-a|)`.
  /// Returns 0 for nearly-collinear samples (no curvature).
  static double curvature(Vector3 a, Vector3 b, Vector3 c) {
    final ab = b - a;
    final bc = c - b;
    final ca = a - c;
    final lab = ab.length;
    final lbc = bc.length;
    final lca = ca.length;
    final denom = lab * lbc * lca;
    if (denom < 1e-12) return 0.0;
    // 2 * area = |(b-a) x (c-a)|; area2 = 2*area = |cross|; triangle area = |cross|/2.
    final cross = ab.cross(c - a);
    final area2 = cross.length; // = 2 * area
    // curvature = 4 * area / (lab * lbc * lca).
    return 2.0 * area2 / denom;
  }

  /// Continuous curvature of a parametric curve at parameter [t]
  /// using the standard formula:
  ///   k = |C'(t) x C''(t)| / |C'(t)|^3
  /// Returns 0 when C'(t) is near-zero.
  static double curvatureAt(
      Vector3 Function(double) evaluate, double t,
      {double? dt}) {
    final h = dt ?? 1e-4 * (1.0 + t.abs());
    final c0 = evaluate(t - h);
    final c1 = evaluate(t);
    final c2 = evaluate(t + h);
    final d1 = (c2 - c0) * (0.5 / h); // C'(t)
    final d2 = (c2 - c1 * 2.0 + c0) * (1.0 / (h * h)); // C''(t)
    final speed = d1.length;
    if (speed < 1e-12) return 0.0;
    return d1.cross(d2).length / (speed * speed * speed);
  }

  // ----- Misc -----------------------------------------------------------

  /// Returns a unit vector perpendicular to [v]. Picks the smallest-
  /// component axis to cross against, which is the numerically safest
  /// choice. Returns `Vector3(1,0,0)` if [v] is the zero vector.
  static Vector3 perpendicularTo(Vector3 v) {
    final ax = v.x.abs();
    final ay = v.y.abs();
    final az = v.z.abs();
    Vector3 other;
    if (ax <= ay && ax <= az) {
      other = Vector3(1, 0, 0);
    } else if (ay <= az) {
      other = Vector3(0, 1, 0);
    } else {
      other = Vector3(0, 0, 1);
    }
    final out = v.cross(other);
    if (out.length2 < 1e-20) return Vector3(1, 0, 0);
    return out..normalize();
  }

  /// Resolves the Frenet frame at a curve point given the previous
  /// frame (parallel transport, more stable than pure Frenet which
  /// flips at inflection points). Returns the new tangent / normal /
  /// binormal triple.
  static (Vector3 tangent, Vector3 normal, Vector3 binormal) parallelTransport(
      Vector3 prevTangent,
      Vector3 prevNormal,
      Vector3 newTangent) {
    var t = newTangent;
    if (t.length2 < 1e-20) t = prevTangent.clone();
    t = t.normalized();
    // Rotation axis = prevTangent x newTangent.
    var axis = prevTangent.cross(t);
    final sinTheta = axis.length;
    final cosTheta = prevTangent.dot(t);
    Vector3 n;
    if (sinTheta < 1e-9) {
      // Tangent barely moved — keep the previous normal (rotation is
      // either identity or 180°, in which case any perpendicular is
      // valid).
      n = prevNormal.clone();
    } else {
      axis = axis / sinTheta;
      // Rodrigues' rotation of prevNormal around `axis` by theta.
      final theta = math.atan2(sinTheta, cosTheta);
      final k = axis;
      n = (prevNormal * math.cos(theta)) +
          (k.cross(prevNormal) * math.sin(theta)) +
          (k * (k.dot(prevNormal) * (1.0 - math.cos(theta))));
    }
    n = n.normalized();
    // Re-orthogonalise against the new tangent to fight drift.
    n = (n - t * n.dot(t))..normalize();
    final b = t.cross(n);
    return (t, n, b);
  }
}
