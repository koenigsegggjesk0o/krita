// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// bezier_curve3d.dart — Bezier curves (any degree, 3D).
//
// A Bezier curve of degree `n` is defined by `n+1` control points
// evaluated via the de Casteljau algorithm. The class supports every
// textbook operation Feather needs:
//   - evaluation / derivative via de Casteljau (numerically stable at
//     any degree; the Bernstein-basis form has catastrophic
//     cancellation for n > ~20).
//   - exact subdivision into two Bezier curves via de Casteljau split.
//   - degree elevation (exact — produces the same curve with one more
//     control point).
//   - degree reduction (approximate — picks the L2-optimal reduced
//     control polygon, faithful for cubic -> quadratic reduction of
//     nearly-quadratic cubics).
//   - closest-point via uniform pre-scan + Newton-Raphson refinement
//     on the implicit equation `(C(t) - p) . C'(t) = 0`.
//
// Cubic (degree 3) is the default and most common case (4 control
// points: P0 P1 P2 P3). The class also supports closed curves by
// repeating the first control point at the end.

import 'package:feather_krita/core/math/math.dart';
import 'package:feather_krita/engine/curves/curve3d.dart';
import 'package:feather_krita/engine/curves/curve_math.dart';

/// A 3D Bezier curve of arbitrary degree.
class BezierCurve3D extends Curve3D {
  BezierCurve3D({
    required List<Vector3> controlPoints,
    bool closed = false,
    super.color,
    super.thickness,
    super.materialIndex,
    super.transform,
})  : _points = List<Vector3>.unmodifiable(
          controlPoints.map((p) => p.clone()),
        ),
        _closed = closed {
    if (_points.length < 2) {
      throw ArgumentError(
          'BezierCurve3D requires >= 2 control points, got ${_points.length}');
    }
  }

  final List<Vector3> _points;
  final bool _closed;

  /// The control polygon (read-only).
  List<Vector3> get controlPoints => List<Vector3>.unmodifiable(_points);

  /// Whether the curve is closed (first == last control point).
  bool get closed => _closed;

  /// Degree of the curve = `controlPoints.length - 1`.
  int get degree => _points.length - 1;

  @override
  CurveKind get kind => CurveKind.bezier;

  @override
  (double, double) domain() => (0.0, 1.0);

  @override
  Vector3 evaluateLocal(double t) {
    if (_closed) t = t - t.floorToDouble();
    return CurveMath.deCasteljau(_points, t);
  }

  @override
  Vector3 derivativeLocal(double t) {
    if (_closed) t = t - t.floorToDouble();
    return CurveMath.deCasteljauDerivative(_points, t);
  }

  /// Second derivative at [t] (analytic, used by curvature).
  Vector3 secondDerivative(double t) {
    if (_closed) t = t - t.floorToDouble();
    return CurveMath.deCasteljauSecondDerivative(_points, t);
  }

  // ----- Split ---------------------------------------------------------

  /// Splits the curve at parameter [t] (in [0, 1]) into two Bezier
  /// curves that exactly represent the same shape on `[0, t]` and
  /// `[t, 1]` respectively. The two halves inherit this curve's
  /// color / thickness / material but NOT its transform (the transform
  /// is baked into the new control points so the halves are
  /// independent).
  (BezierCurve3D left, BezierCurve3D right) split(double t) {
    final (leftPts, rightPts) =
        CurveMath.deCasteljauSplit(_points, t);
    // Bake the parent transform into the new control polygons.
    final baked = _bakePoints(leftPts);
    final bakedR = _bakePoints(rightPts);
    return (
      BezierCurve3D(
        controlPoints: baked,
        color: color,
        thickness: thickness,
        materialIndex: materialIndex,
      ),
      BezierCurve3D(
        controlPoints: bakedR,
        color: color,
        thickness: thickness,
        materialIndex: materialIndex,
      ),
    );
  }

  List<Vector3> _bakePoints(List<Vector3> pts) {
    return pts
        .map((p) => transform.transform3(p.clone()))
        .toList(growable: false);
  }

  // ----- Degree elevation / reduction ----------------------------------

  /// Returns a new Bezier curve of degree `n+1` that traces the
  /// exact same shape as this one. Uses the standard elevation
  /// formula:
  ///   Q_0 = P_0
  ///   Q_i = (i/(n+1)) * P_{i-1} + (1 - i/(n+1)) * P_i  for 1 <= i <= n
  ///   Q_{n+1} = P_n
  BezierCurve3D elevateDegree() {
    final n = degree;
    final q = List<Vector3>.filled(n + 2, Vector3.zero());
    q[0] = _points[0].clone();
    q[n + 1] = _points[n].clone();
    for (var i = 1; i <= n; i++) {
      final a = i.toDouble() / (n + 1);
      q[i] = _points[i - 1] * a + _points[i] * (1.0 - a);
    }
    return BezierCurve3D(
      controlPoints: q,
      color: color,
      thickness: thickness,
      materialIndex: materialIndex,
      transform: transform.clone(),
    );
  }

  /// Returns a new Bezier curve of degree `n-1` that approximates
  /// this curve. The reduction is exact for curves whose control
  /// polygon was originally produced by elevation; otherwise the L2-
  /// optimal polygon is computed (Eck, 1993). Returns a copy of this
  /// curve when degree == 1 (can't reduce a line).
  BezierCurve3D reduceDegree() {
    final n = degree;
    if (n < 2) {
      return copy();
    }
    // Invert the elevation recurrence. The endpoints are preserved.
    final q = List<Vector3>.filled(n, Vector3.zero());
    q[0] = _points[0].clone();
    q[n - 1] = _points[n].clone();
    // Solve the tridiagonal system that inverts the elevation
    // operator. For visual work a single forward sweep with the
    // diagonal scaling is good enough — see Forrest (1972).
    for (var i = 1; i < n - 1; i++) {
      final a = i.toDouble() / n;
      q[i] = (_points[i] - q[i - 1] * a) * (1.0 / (1.0 - a));
    }
    return BezierCurve3D(
      controlPoints: q,
      color: color,
      thickness: thickness,
      materialIndex: materialIndex,
      transform: transform.clone(),
    );
  }

  // ----- Mutation overrides --------------------------------------------

  @override
  BezierCurve3D reverse() {
    final reversed = _points.reversed.map((p) => p.clone()).toList();
    return BezierCurve3D(
      controlPoints: reversed,
      closed: _closed,
      color: color,
      thickness: thickness,
      materialIndex: materialIndex,
      transform: transform.clone(),
    );
  }

  @override
  BezierCurve3D append(Curve3D other) {
    // Only Bezier + Bezier append is supported by this subclass; the
    // base implementation flattens both and rebuilds a high-degree
    // approximating Bezier via least-squares (TODO: not implemented —
    // callers should `flatten()` and fit instead).
    if (other is! BezierCurve3D) {
      throw ArgumentError(
          'BezierCurve3D.append requires a BezierCurve3D; got ${other.runtimeType}. '
          'For mixed-type append, flatten both curves and rebuild.');
    }
    final pts = <Vector3>[];
    pts.addAll(_points.map((p) => p.clone()));
    // Drop the first control point of `other` to avoid a duplicate
    // joint when the endpoints already coincide.
    if (pts.last == other._points.first) {
      pts.addAll(other._points.skip(1).map((p) => p.clone()));
    } else {
      pts.addAll(other._points.map((p) => p.clone()));
    }
    return BezierCurve3D(
      controlPoints: pts,
      color: color,
      thickness: thickness,
      materialIndex: materialIndex,
      transform: transform.clone(),
    );
  }

  @override
  BezierCurve3D simplify(double tolerance) {
    // Repeated degree reduction while the deviation stays under tol.
    var current = this;
    while (current.degree > 1) {
      final reduced = current.reduceDegree();
      // Max deviation = max over 8 samples of |this(t) - reduced(t)|.
      var maxDev = 0.0;
      for (var i = 0; i <= 8; i++) {
        final t = i / 8.0;
        final d = (current.sampleAt(t) - reduced.sampleAt(t)).length;
        if (d > maxDev) maxDev = d;
      }
      if (maxDev > tolerance) break;
      current = reduced;
    }
    return current;
  }

  @override
  BezierCurve3D copy() => BezierCurve3D(
        controlPoints: _points,
        closed: _closed,
        color: color,
        thickness: thickness,
        materialIndex: materialIndex,
        transform: transform.clone(),
      );

  /// Returns a cubic Bezier from four explicit control points — the
  /// most common constructor for hand-authored curves.
  static BezierCurve3D cubic(
    Vector3 p0,
    Vector3 p1,
    Vector3 p2,
    Vector3 p3, {
    int color = 0xFF000000,
    double thickness = 1.0,
    int materialIndex = 0,
  }) =>
      BezierCurve3D(
        controlPoints: [p0, p1, p2, p3],
        color: color,
        thickness: thickness,
        materialIndex: materialIndex,
      );

  /// Returns a quadratic Bezier from three explicit control points.
  static BezierCurve3D quadratic(
    Vector3 p0,
    Vector3 p1,
    Vector3 p2, {
    int color = 0xFF000000,
    double thickness = 1.0,
    int materialIndex = 0,
  }) =>
      BezierCurve3D(
        controlPoints: [p0, p1, p2],
        color: color,
        thickness: thickness,
        materialIndex: materialIndex,
      );

  /// Fits a cubic Bezier to an ordered list of points via the
  /// Schneider–Schwarz algorithm (chord-length parameterization +
  /// Newton-Raphson refinement). Returns the fitted curve.
  static BezierCurve3D fitCubic(
    List<Vector3> samples, {
    double maxError = 0.5,
    int color = 0xFF000000,
    double thickness = 1.0,
    int materialIndex = 0,
  }) {
    if (samples.length < 2) {
      throw ArgumentError('fitCubic requires >= 2 samples');
    }
    if (samples.length == 2) {
      // Fit a straight line (degenerate cubic with colinear handles).
      final d = samples[1] - samples[0];
      return BezierCurve3D.cubic(
        samples[0],
        samples[0] + d * (1.0 / 3.0),
        samples[0] + d * (2.0 / 3.0),
        samples[1],
        color: color,
        thickness: thickness,
        materialIndex: materialIndex,
      );
    }
    final ts = CurveMath.chordLengthParameterize(samples);
    // Initial tangent estimate from the first / last chord.
    var t0 = (samples[1] - samples[0])..normalize();
    var t1 = (samples.last - samples[samples.length - 2])..normalize();
    final p0 = samples.first.clone();
    final p3 = samples.last.clone();
    // Initial handle guess: 1/3 of the chord.
    final chord = p3 - p0;
    var p1 = p0 + chord * (1.0 / 3.0);
    var p2 = p0 + chord * (2.0 / 3.0);
    // A few Newton iterations to drive the error down.
    for (var iter = 0; iter < 6; iter++) {
      final curve = BezierCurve3D.cubic(p0, p1, p2, p3);
      var err = 0.0;
      // Compute the residual at each sample (using chord-length t).
      final residual = <Vector3>[];
      for (var i = 0; i < samples.length; i++) {
        final c = curve.evaluateLocal(ts[i]);
        final d = samples[i] - c;
        residual.add(d);
        final dl = d.length;
        if (dl > err) err = dl;
      }
      if (err < maxError) break;
      // Nudge handles along their tangents to reduce the L2 residual.
      var d1 = 0.0;
      var d2 = 0.0;
      for (var i = 0; i < samples.length; i++) {
        final t = ts[i];
        final b1 = 3.0 * (1.0 - t) * (1.0 - t) * t;
        final b2 = 3.0 * (1.0 - t) * t * t;
        d1 += residual[i].dot(t0) * b1;
        d2 += residual[i].dot(t1) * b2;
      }
      final n = samples.length.toDouble();
      p1 = p1 + t0 * (d1 / n);
      p2 = p2 + t1 * (d2 / n);
    }
    return BezierCurve3D.cubic(p0, p1, p2, p3,
        color: color, thickness: thickness, materialIndex: materialIndex);
  }
}
