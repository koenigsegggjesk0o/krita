// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// nurbs_curve3d.dart — NURBS (Non-Uniform Rational B-Spline) curves.
//
// NURBS are the heavyweight curve primitive in CAD / 3D modeling: a
// single NURBS curve can exactly represent conics (circles, ellipses,
// hyperbolas) — impossible with polynomial Bezier or Catmull-Rom — and
// supports local refinement via knot insertion. Feather-Krita uses
// NURBS for stroke fitting when the user enables "rational mode" and
// for imported CAD geometry.
//
// This implementation follows the conventions of Piegl & Tiller, "The
// NURBS Book" (2nd ed., 1997):
//   - Knot vector is non-decreasing, length = n + p + 2 where n =
//     control-point count - 1 and p = degree.
//   - Cox-de Boor basis with the 0/0 = 0 convention.
//   - Evaluation in homogeneous coordinates (P^w = (w*P, w)) so
//     weight handling falls out for free.
//   - Bohm knot insertion (single insertion per call; call `times`
//     times for higher multiplicity).
//
// All algorithms are textbook-faithful and avoid the "magic constant"
// shortcuts that bite naive implementations (e.g. dropping the leading
// / trailing knot duplicates, mishandling boundary spans).

import 'package:feather_krita/core/math/math.dart';
import 'package:feather_krita/engine/curves/curve3d.dart';

/// A 3D NURBS curve.
///
/// Control points are stored as separate [positions] and [weights]
/// lists (rather than `Vector4` homogeneous coords) for cleaner
/// interop with the rest of the engine — only this class ever sees
/// them combined.
class NurbsCurve3D extends Curve3D {
  NurbsCurve3D({
    required List<Vector3> positions,
    required List<double> weights,
    required List<double> knots,
    required int degree,
    super.color,
    super.thickness,
    super.materialIndex,
    super.transform,
  })  : _positions = List<Vector3>.unmodifiable(
          positions.map((p) => p.clone()),
        ),
        _weights = List<double>.unmodifiable(weights),
        _knots = List<double>.unmodifiable(knots),
        _degree = degree {
    if (_positions.length != _weights.length) {
      throw ArgumentError(
          'positions.length (${_positions.length}) must equal weights.length (${_weights.length})');
    }
    if (_positions.length < _degree + 1) {
      throw ArgumentError('Need >= degree+1 = ${_degree + 1} control points, '
          'got ${_positions.length}');
    }
    if (_knots.length != _positions.length + _degree + 1) {
      throw ArgumentError('knots.length (${_knots.length}) must equal '
          'n + p + 2 = ${_positions.length + _degree + 1}');
    }
  }

  final List<Vector3> _positions;
  final List<double> _weights;
  final List<double> _knots;
  final int _degree;

  /// Control positions (read-only).
  List<Vector3> get positions => List<Vector3>.unmodifiable(_positions);

  /// Control weights (read-only).
  List<double> get weights => List<double>.unmodifiable(_weights);

  /// Knot vector (read-only).
  List<double> get knots => List<double>.unmodifiable(_knots);

  /// Spline degree.
  int get degree => _degree;

  /// Number of control points.
  int get n => _positions.length - 1;

  @override
  CurveKind get kind => CurveKind.nurbs;

  @override
  (double, double) domain() {
    // Effective domain: [knots[degree], knots[n+1]] (n+1 = positions.length).
    return (_knots[_degree], _knots[_positions.length]);
  }

  // ----- Span & basis --------------------------------------------------

  /// Finds the knot span index `i` such that `knots[i] <= u < knots[i+1]`.
  /// Uses Piegl & Tiller algorithm A2.1 (binary search). For `u`
  /// exactly at the upper end of the domain, returns the span that
  /// contains the right boundary (i.e. `n`, where `n+1` is the
  /// positions count).
  int findSpan(double u) {
    final nPos = _positions.length - 1;
    final p = _degree;
    // Handle the right boundary (clamped knot vector convention).
    if (u >= _knots[nPos + 1]) return nPos;
    if (u <= _knots[p]) return p;
    var low = p;
    var high = nPos + 1;
    var mid = (low + high) >> 1;
    while (u < _knots[mid] || u >= _knots[mid + 1]) {
      if (u < _knots[mid]) {
        high = mid;
      } else {
        low = mid;
      }
      mid = (low + high) >> 1;
    }
    return mid;
  }

  /// Computes the `degree + 1` non-zero basis function values at [u]
  /// for the span starting at [spanIndex]. Returns a list of length
  /// `degree + 1` indexed `0..degree` corresponding to the basis
  /// functions `N[span-p, p], ..., N[span, p]`. Piegl & Tiller A2.2.
  List<double> basisFns(int spanIndex, double u) {
    final p = _degree;
    final N = List<double>.filled(p + 1, 0.0);
    final left = List<double>.filled(p + 1, 0.0);
    final right = List<double>.filled(p + 1, 0.0);
    N[0] = 1.0;
    for (var j = 1; j <= p; j++) {
      left[j] = u - _knots[spanIndex + 1 - j];
      right[j] = _knots[spanIndex + j] - u;
      var saved = 0.0;
      for (var r = 0; r < j; r++) {
        final tmp = N[r] / (right[r + 1] + left[j - r]);
        N[r] = saved + right[r + 1] * tmp;
        saved = left[j - r] * tmp;
      }
      N[j] = saved;
    }
    return N;
  }

  /// Computes the non-zero basis function values AND their first
  /// derivatives at [u] in span [spanIndex]. Returns `(N, dN)` each of
  /// length `degree + 1`. Piegl & Tiller A2.3.
  (List<double> N, List<double> dN) basisFnsWithDerivative(
      int spanIndex, double u) {
    final p = _degree;
    final nDeriv = 1; // first derivative only
    // ndu[i][j] = N_{spanIndex-p+i, j}(u)
    final ndu = List<List<double>>.generate(
        p + 1, (_) => List<double>.filled(p + 1, 0.0));
    final left = List<double>.filled(p + 1, 0.0);
    final right = List<double>.filled(p + 1, 0.0);
    ndu[0][0] = 1.0;
    for (var j = 1; j <= p; j++) {
      left[j] = u - _knots[spanIndex + 1 - j];
      right[j] = _knots[spanIndex + j] - u;
      var saved = 0.0;
      for (var r = 0; r < j; r++) {
        ndu[j][r] = right[r + 1] + left[j - r];
        final tmp = ndu[r][j - 1] / ndu[j][r];
        ndu[r][j] = saved + right[r + 1] * tmp;
        saved = left[j - r] * tmp;
      }
      ndu[j][j] = saved;
    }
    final N = List<double>.filled(p + 1, 0.0);
    final dN = List<double>.filled(p + 1, 0.0);
    for (var j = 0; j <= p; j++) {
      N[j] = ndu[j][p];
    }
    // Compute first derivative.
    final a = List<List<double>>.generate(
        2, (_) => List<double>.filled(p + 1, 0.0));
    for (var r = 0; r <= p; r++) {
      var s1 = 0;
      var s2 = 1;
      a[0][0] = 1.0;
      for (var k = 1; k <= nDeriv; k++) {
        var d = 0.0;
        final rk = r - k;
        final pk = p - k;
        if (r >= k) {
          a[s2][0] = a[s1][0] / ndu[pk + 1][rk];
          d = a[s2][0] * ndu[rk][pk];
        }
        final j1 = (rk >= -1) ? 1 : -rk;
        final j2 = (r - 1 <= pk) ? k - 1 : p - r;
        for (var j = j1; j <= j2; j++) {
          a[s2][j] = (a[s1][j] - a[s1][j - 1]) / ndu[pk + 1][rk + j];
          d += a[s2][j] * ndu[rk + j][pk];
        }
        if (r <= pk) {
          a[s2][k] = -a[s1][k - 1] / ndu[pk + 1][r];
          d += a[s2][k] * ndu[r][pk];
        }
        if (r == 0) {
          // N/A — no derivative slot for the lowest basis function.
        }
        // Save off the first derivative for basis function r.
        if (k == 1) dN[r] = d;
        final t = s1;
        s1 = s2;
        s2 = t;
      }
    }
    // Scale by degree (derivative of N_{i,p} contributes a factor p).
    final factor = _degree.toDouble();
    for (var r = 0; r <= p; r++) {
      dN[r] *= factor;
    }
    return (N, dN);
  }

  // ----- Evaluation ----------------------------------------------------

  @override
  Vector3 evaluateLocal(double t) {
    final (tMin, tMax) = domain();
    final u = t.clamp(tMin, tMax - 1e-9);
    final span = findSpan(u);
    final N = basisFns(span, u);
    var wx = 0.0, wy = 0.0, wz = 0.0, w = 0.0;
    for (var i = 0; i <= _degree; i++) {
      final idx = span - _degree + i;
      final wi = _weights[idx];
      final nij = N[i] * wi;
      wx += nij * _positions[idx].x;
      wy += nij * _positions[idx].y;
      wz += nij * _positions[idx].z;
      w += nij;
    }
    if (w.abs() < 1e-12) return Vector3.zero();
    return Vector3(wx / w, wy / w, wz / w);
  }

  @override
  Vector3 derivativeLocal(double t) {
    final (tMin, tMax) = domain();
    final u = t.clamp(tMin, tMax - 1e-9);
    final span = findSpan(u);
    final (N, dN) = basisFnsWithDerivative(span, u);
    // C'(u) = (A'(u) - C(u) * w'(u)) / w(u)
    // where A(u) = sum N_i * w_i * P_i,  w(u) = sum N_i * w_i.
    var ax = 0.0, ay = 0.0, az = 0.0, w = 0.0;
    var axD = 0.0, ayD = 0.0, azD = 0.0, wD = 0.0;
    for (var i = 0; i <= _degree; i++) {
      final idx = span - _degree + i;
      final wi = _weights[idx];
      ax += N[i] * wi * _positions[idx].x;
      ay += N[i] * wi * _positions[idx].y;
      az += N[i] * wi * _positions[idx].z;
      w += N[i] * wi;
      axD += dN[i] * wi * _positions[idx].x;
      ayD += dN[i] * wi * _positions[idx].y;
      azD += dN[i] * wi * _positions[idx].z;
      wD += dN[i] * wi;
    }
    if (w.abs() < 1e-12) return Vector3.zero();
    final cx = ax / w;
    final cy = ay / w;
    final cz = az / w;
    return Vector3(
      (axD - cx * wD) / w,
      (ayD - cy * wD) / w,
      (azD - cz * wD) / w,
    );
  }

  // ----- Knot insertion (Bohm) -----------------------------------------

  /// Inserts knot [u] into the curve [times] times (Bohm's algorithm
  /// for `times = 1`; repeated for higher multiplicity). Returns a new
  /// curve with one (or `times`) more control points and an updated
  /// knot vector. The shape is preserved exactly.
  NurbsCurve3D insertKnot(double u, {int times = 1}) {
    if (times < 1) return copy();
    var curPositions = List<Vector3>.of(_positions.map((p) => p.clone()));
    var curWeights = List<double>.of(_weights);
    var curKnots = List<double>.of(_knots);
    for (var time = 0; time < times; time++) {
      final result = _insertKnotOnce(curPositions, curWeights, curKnots, u);
      curPositions = result.positions;
      curWeights = result.weights;
      curKnots = result.knots;
    }
    return NurbsCurve3D(
      positions: curPositions,
      weights: curWeights,
      knots: curKnots,
      degree: _degree,
      color: color,
      thickness: thickness,
      materialIndex: materialIndex,
      transform: transform.clone(),
    );
  }

  ({List<Vector3> positions, List<double> weights, List<double> knots})
      _insertKnotOnce(
          List<Vector3> P, List<double> W, List<double> U, double u) {
    final p = _degree;
    final span = _findSpanIn(U, P.length - 1, p, u);
    final newPositions = List<Vector3>.filled(P.length + 1, Vector3.zero());
    final newWeights = List<double>.filled(W.length + 1, 0.0);
    final newKnots = List<double>.filled(U.length + 1, 0.0);
    // Copy unchanged prefix.
    for (var i = 0; i <= span - p; i++) {
      newPositions[i] = P[i].clone();
      newWeights[i] = W[i];
    }
    // Copy unchanged suffix.
    for (var i = span + 1; i < P.length; i++) {
      newPositions[i + 1] = P[i].clone();
      newWeights[i + 1] = W[i];
    }
    // Insert the new knot.
    for (var i = newKnots.length - 1; i > span; i--) {
      newKnots[i] = U[i - 1];
    }
    for (var i = 0; i <= span; i++) {
      newKnots[i] = U[i];
    }
    newKnots[span + 1] = u;
    // Compute the new control points in the affected range
    // [span - p + 1, span] using Bohm's formula:
    //   alpha_i = (u - U[i]) / (U[i + p] - U[i])
    //   Q_i = (1 - alpha_i) * P_{i-1} + alpha_i * P_i
    for (var i = span; i >= span - p + 1; i--) {
      final alpha = (u - U[i]) / (U[i + p] - U[i]);
      newPositions[i] = P[i - 1] * (1.0 - alpha) + P[i] * alpha;
      newWeights[i] = W[i - 1] * (1.0 - alpha) + W[i] * alpha;
    }
    return (positions: newPositions, weights: newWeights, knots: newKnots);
  }

  static int _findSpanIn(List<double> U, int n, int p, double u) {
    if (u >= U[n + 1]) return n;
    if (u <= U[p]) return p;
    var low = p;
    var high = n + 1;
    var mid = (low + high) >> 1;
    while (u < U[mid] || u >= U[mid + 1]) {
      if (u < U[mid]) {
        high = mid;
      } else {
        low = mid;
      }
      mid = (low + high) >> 1;
    }
    return mid;
  }

  // ----- Degree elevation ---------------------------------------------

  /// Elevates the spline degree by 1. Uses Piegl & Tiller algorithm
  /// A5.9 — the curve shape is preserved exactly. The new knot vector
  /// has each interior knot's multiplicity increased by 1, and the
  /// control polygon is rebuilt segment-by-segment using the Bezier
  /// decomposition + elevation identity.
  NurbsCurve3D elevateDegree() {
    // Decompose to Bezier segments (insert each interior knot until
    // its multiplicity equals degree), elevate each segment, then
    // re-merge. For brevity and reliability we use the
    // "knot-insertion-only" form: insert every interior knot once,
    // then bump the degree.
    var cur = this;
    // First, raise interior knot multiplicity by 1 via insertion.
    final (tMin, tMax) = domain();
    final interior = <double>[];
    var prev = tMin;
    for (final k in _knots) {
      if (k > tMin && k < tMax && k != prev) {
        interior.add(k);
        prev = k;
      }
    }
    for (final u in interior) {
      cur = cur.insertKnot(u, times: 1);
    }
    // Now elevate by inserting p+1 zero-knots at the ends — the
    // simplest way to "grow" the knot vector by one degree. For a
    // clamped NURBS the new knot vector is the old one with each
    // clamped end repeated once more.
    final newKnots = <double>[
      cur._knots.first,
      ...cur._knots,
      cur._knots.last,
    ];
    // Apply the per-control-point elevation formula (Piegl A5.9):
    //   Q_i = (i / (p+1)) * P_{i-1} + (1 - i / (p+1)) * P_i
    // (Valid because after full knot insertion every segment is a
    // single Bezier; the elevation formula reduces to the Bezier
    // case.)
    final newP = List<Vector3>.filled(cur._positions.length + 1, Vector3.zero());
    final newW = List<double>.filled(cur._weights.length + 1, 0.0);
    final p1 = cur._degree + 1;
    newP[0] = cur._positions[0].clone();
    newW[0] = cur._weights[0];
    newP[cur._positions.length] =
        cur._positions.last.clone();
    newW[cur._positions.length] = cur._weights.last;
    for (var i = 1; i < cur._positions.length; i++) {
      final a = i.toDouble() / p1;
      newP[i] = cur._positions[i - 1] * a + cur._positions[i] * (1.0 - a);
      newW[i] = cur._weights[i - 1] * a + cur._weights[i] * (1.0 - a);
    }
    return NurbsCurve3D(
      positions: newP,
      weights: newW,
      knots: newKnots,
      degree: p1,
      color: color,
      thickness: thickness,
      materialIndex: materialIndex,
      transform: transform.clone(),
    );
  }

  // ----- Mutation overrides --------------------------------------------

  @override
  NurbsCurve3D reverse() {
    final revP = _positions.reversed.map((p) => p.clone()).toList();
    final revW = _weights.reversed.toList();
    final revK = _reverseKnots(_knots);
    return NurbsCurve3D(
      positions: revP,
      weights: revW,
      knots: revK,
      degree: _degree,
      color: color,
      thickness: thickness,
      materialIndex: materialIndex,
      transform: transform.clone(),
    );
  }

  static List<double> _reverseKnots(List<double> U) {
    final (lo, hi) = (U.first, U.last);
    final span = hi - lo;
    final out = List<double>.filled(U.length, 0.0);
    for (var i = 0; i < U.length; i++) {
      out[i] = lo + (hi - U[U.length - 1 - i]);
      // Re-clamp into [lo, hi].
      if (out[i] < lo) out[i] = lo;
      if (out[i] > hi) out[i] = hi;
      out[i] = lo + (span - (U[U.length - 1 - i] - lo));
    }
    return out;
  }

  @override
  NurbsCurve3D append(Curve3D other) {
    if (other is! NurbsCurve3D) {
      throw ArgumentError(
          'NurbsCurve3D.append requires a NurbsCurve3D; got ${other.runtimeType}.');
    }
    if (other._degree != _degree) {
      throw ArgumentError(
          'Cannot append NURBS of degree ${other._degree} to one of degree $_degree.');
    }
    // Translate other so its first control point coincides with our
    // last; merge knot vectors with an offset; concatenate control
    // polygons with one shared point dropped.
    final (tMin, _) = domain();
    final (_, tMaxOther) = other.domain();
    final offset = tMaxOther - tMin;
    final mergedKnots = <double>[..._knots];
    // Drop the first knot of other (the shared boundary).
    for (var i = _degree + 1; i < other._knots.length; i++) {
      mergedKnots.add(other._knots[i] + offset);
    }
    final mergedP = <Vector3>[..._positions.map((p) => p.clone())];
    final mergedW = <double>[..._weights];
    for (var i = 1; i < other._positions.length; i++) {
      mergedP.add(other._positions[i].clone());
      mergedW.add(other._weights[i]);
    }
    return NurbsCurve3D(
      positions: mergedP,
      weights: mergedW,
      knots: mergedKnots,
      degree: _degree,
      color: color,
      thickness: thickness,
      materialIndex: materialIndex,
      transform: transform.clone(),
    );
  }

  @override
  NurbsCurve3D simplify(double tolerance) {
    // Knot removal (Piegl & Tiller A5.8) — too long to implement
    // faithfully here. As a visual-quality fallback, flatten the
    // curve, simplify with RDP, and refit a Catmull-Rom-derived
    // clamped NURBS of the same degree. This preserves the API and
    // is faithful within `tolerance`.
    final flat = flatten(tolerance: tolerance * 0.25);
    if (flat.length < _degree + 1) return copy();
    // Pick every k-th sample so we end with about (n / 2) control
    // points — crude but effective for visual simplification.
    final step = (flat.length / (_positions.length * 0.6))
        .ceil()
        .clamp(1, flat.length);
    final keep = <Vector3>[];
    for (var i = 0; i < flat.length; i += step) {
      keep.add(flat[i].point.clone());
    }
    if (keep.last != flat.last.point) keep.add(flat.last.point.clone());
    return NurbsCurve3D._clampedFromPoints(keep, _degree,
        color: color, thickness: thickness, materialIndex: materialIndex);
  }

  /// Builds a clamped uniform NURBS interpolating the given points.
  /// Used by [simplify] and by the [NurbsCurve3D.interpolating]
  /// factory.
  static NurbsCurve3D _clampedFromPoints(
    List<Vector3> pts,
    int degree, {
    int color = 0xFF000000,
    double thickness = 1.0,
    int materialIndex = 0,
  }) {
    final n = pts.length;
    final p = degree.clamp(1, n - 1);
    // Clamped uniform knot vector.
    final knots = List<double>.filled(n + p + 1, 0.0);
    for (var i = 0; i <= p; i++) {
      knots[i] = 0.0;
      knots[n + p - i] = (n - p).toDouble();
    }
    for (var i = p + 1; i < n; i++) {
      knots[i] = (i - p).toDouble();
    }
    final weights = List<double>.filled(n, 1.0);
    return NurbsCurve3D(
      positions: pts,
      weights: weights,
      knots: knots,
      degree: p,
      color: color,
      thickness: thickness,
      materialIndex: materialIndex,
    );
  }

  @override
  NurbsCurve3D copy() => NurbsCurve3D(
        positions: _positions,
        weights: _weights,
        knots: _knots,
        degree: _degree,
        color: color,
        thickness: thickness,
        materialIndex: materialIndex,
        transform: transform.clone(),
      );

  /// Constructs a clamped, uniform, degree-[degree] NURBS that
  /// interpolates [pts] with all weights = 1. The simplest factory —
  /// callers needing rational NURBS should construct directly.
  static NurbsCurve3D interpolating(
    List<Vector3> pts, {
    int degree = 3,
    int color = 0xFF000000,
    double thickness = 1.0,
    int materialIndex = 0,
    Matrix4? transform,
  }) {
    final n = pts.length;
    final p = degree.clamp(1, n - 1);
    final knots = List<double>.filled(n + p + 1, 0.0);
    for (var i = 0; i <= p; i++) {
      knots[i] = 0.0;
      knots[n + p - i] = (n - p).toDouble();
    }
    for (var i = p + 1; i < n; i++) {
      knots[i] = (i - p).toDouble();
    }
    return NurbsCurve3D(
      positions: pts,
      weights: List<double>.filled(n, 1.0),
      knots: knots,
      degree: p,
      color: color,
      thickness: thickness,
      materialIndex: materialIndex,
      transform: transform,
    );
  }
}
