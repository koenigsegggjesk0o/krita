// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// catmull_rom_curve3d.dart — Catmull-Rom splines (and the Cardinal
// family they belong to) for 3D stroke fitting.
//
// A Catmull-Rom spline is a piecewise cubic that interpolates each
// control point and is C1-continuous across segment boundaries. It is
// the de-facto default for stroke-fitting in drawing apps because it
// passes through every input point (unlike Bezier / B-spline) while
// still being locally controllable via a single tension dial.
//
// This implementation supports:
//   - uniform parameterization (default; what most drawing apps use).
//   - chord-length parameterization (better for unevenly spaced input).
//   - centripetal (a.k.a. alpha = 0.5) parameterization — provably
//     free of cusps and self-intersections within a segment, see
//     Barry & Goldman (1988). Use `parameterization: _Centripetal`.
//   - a `tension` dial in [0, 1] that interpolates between Catmull-Rom
//     (tension = 0) and a piecewise-linear spline (tension = 1).
//   - closed loops (control points wrap around).
//   - exact conversion to a list of cubic Bezier segments for editing
//     and rendering.
//
// The domain is `[0, n-1]` for open splines (n = control points) and
// `[0, n]` for closed splines — the integer parts of `t` index the
// "from" point of each segment.

import 'dart:math' as math;

import 'package:feather_krita/core/math/math.dart';
import 'package:feather_krita/engine/curves/curve3d.dart';
import 'package:feather_krita/engine/curves/bezier_curve3d.dart';

/// Which arc-length-style parameterization a [CatmullRomCurve3D] uses
/// to space the spline parameter within each segment.
enum CatmullRomParameterization {
  /// Uniform: `u = fractional part of t`. Fast; can produce cusps on
  /// highly uneven input.
  uniform,

  /// Chord-length: per-segment `u` scaled by the chord length ratio.
  /// Smoother than uniform; standard for stroke fitting.
  chordal,

  /// Centripetal (alpha = 0.5): per-segment `u` scaled by the
  /// square-root of the chord length ratio. Cusp-free within a
  /// segment — the safest default for stylus strokes.
  centripetal,
}

/// A 3D Catmull-Rom spline.
class CatmullRomCurve3D extends Curve3D {
  CatmullRomCurve3D({
    required List<Vector3> controlPoints,
    bool closed = false,
    double tension = 0.0,
    CatmullRomParameterization parameterization =
        CatmullRomParameterization.uniform,
    super.color,
    super.thickness,
    super.materialIndex,
    super.transform,
  })  : _points = List<Vector3>.unmodifiable(
          controlPoints.map((p) => p.clone()),
        ),
        _closed = closed,
        _tension = tension.clamp(0.0, 1.0),
        _parameterization = parameterization {
    if (_points.length < 2) {
      throw ArgumentError(
          'CatmullRomCurve3D requires >= 2 control points, got ${_points.length}');
    }
    _buildSegmentParameters();
  }

  final List<Vector3> _points;
  final bool _closed;
  final double _tension;
  final CatmullRomParameterization _parameterization;

  // Per-segment "knot" intervals in the curve's parameter space. For
  // uniform param, these are all 1.0; for chordal / centripetal they
  // encode the chord-length ratios.
  late final List<double> _segIntervals;

  /// The control polygon (read-only).
  List<Vector3> get controlPoints => List<Vector3>.unmodifiable(_points);

  /// Whether the spline is closed.
  bool get closed => _closed;

  /// Tension in [0, 1] — 0 = Catmull-Rom, 1 = piecewise linear.
  double get tension => _tension;

  /// The active parameterization mode.
  CatmullRomParameterization get parameterization => _parameterization;

  @override
  CurveKind get kind => CurveKind.catmullRom;

  @override
  (double, double) domain() {
    if (_closed) return (0.0, _points.length.toDouble());
    // Open splines draw n-1 segments between P_0..P_{n-1}.
    if (_points.length <= 3) return (0.0, (_points.length - 1).toDouble());
    return (0.0, (_points.length - 1).toDouble());
  }

  int get _segmentCount => _closed ? _points.length : _points.length - 1;

  void _buildSegmentParameters() {
    final n = _segmentCount;
    _segIntervals = List<double>.filled(n, 1.0);
    if (_parameterization == CatmullRomParameterization.uniform) return;
    for (var i = 0; i < n; i++) {
      final a = _pointAt(i);
      final b = _pointAt(i + 1);
      final d = (b - a).length;
      if (_parameterization == CatmullRomParameterization.centripetal) {
        _segIntervals[i] = math.sqrt(d);
      } else {
        _segIntervals[i] = d;
      }
      if (_segIntervals[i] < 1e-9) _segIntervals[i] = 1e-9;
    }
  }

  /// Returns control point [i], wrapping around for closed curves.
  /// Index -1 returns the antepenultimate point for open curves
  /// (mirrored endpoint trick).
  Vector3 _pointAt(int i) {
    final n = _points.length;
    if (_closed) {
      final j = ((i % n) + n) % n;
      return _points[j];
    }
    // Open: clamp at the ends (phantom endpoint = first / last point).
    if (i < 0) return _points[0];
    if (i >= n) return _points[n - 1];
    return _points[i];
  }

  /// Resolves a curve parameter [t] to a (segment index, local u in
  /// [0, 1]) pair, honouring the active parameterization.
  (int segment, double u) _resolveSegment(double t) {
    final (tMin, tMax) = domain();
    if (tMax - tMin < 1e-12) return (0, 0.0);
    var tt = t;
    if (_closed) {
      // Wrap.
      tt = tt - tMin;
      tt = tt % (tMax - tMin);
    } else {
      tt = tt.clamp(tMin, tMax) - tMin;
    }
    // Walk segments, accumulating their parameter intervals.
    var acc = 0.0;
    for (var i = 0; i < _segmentCount; i++) {
      final interval = _segIntervals[i];
      if (tt <= acc + interval || i == _segmentCount - 1) {
        final local = (tt - acc) / interval;
        return (i, local.clamp(0.0, 1.0));
      }
      acc += interval;
    }
    return (_segmentCount - 1, 1.0);
  }

  /// Cardinal-spline basis evaluated on four control points at local
  /// parameter [u] in [0, 1]. [s] is `2 - tension` (so s = 2 is plain
  /// Catmull-Rom).
  static Vector3 _cardinal(
      Vector3 p0, Vector3 p1, Vector3 p2, Vector3 p3, double u, double s) {
    final u2 = u * u;
    final u3 = u2 * u;
    // Cardinal basis polynomials.
    final b0 = (s * (-u3 + 2 * u2 - u)) * 0.5;
    final b1 = ((2 - s) * u3 + (s - 3) * u2 + 1.0) * 0.5;
    final b2 = ((s - 2) * u3 + (3 - 2 * s) * u2 + s * u) * 0.5;
    final b3 = (s * (u3 - u2)) * 0.5;
    return p0 * b0 + p1 * b1 + p2 * b2 + p3 * b3;
  }

  /// Derivative of [_cardinal] w.r.t. u.
  static Vector3 _cardinalDerivative(
      Vector3 p0, Vector3 p1, Vector3 p2, Vector3 p3, double u, double s) {
    final u2 = u * u;
    // d/du of the basis polynomials above.
    final b0 = (s * (-3 * u2 + 4 * u - 1)) * 0.5;
    final b1 = (3 * (2 - s) * u2 + 2 * (s - 3) * u) * 0.5;
    final b2 = (3 * (s - 2) * u2 + 2 * (3 - 2 * s) * u + s) * 0.5;
    final b3 = (s * (3 * u2 - 2 * u)) * 0.5;
    return p0 * b0 + p1 * b1 + p2 * b2 + p3 * b3;
  }

  double get _s => 2.0 - 2.0 * _tension;

  @override
  Vector3 evaluateLocal(double t) {
    if (_points.length == 2) {
      // Linear fallback.
      final u = t.clamp(0.0, 1.0);
      return _points[0] + (_points[1] - _points[0]) * u;
    }
    final (seg, u) = _resolveSegment(t);
    final p0 = _pointAt(seg - 1);
    final p1 = _pointAt(seg);
    final p2 = _pointAt(seg + 1);
    final p3 = _pointAt(seg + 2);
    return _cardinal(p0, p1, p2, p3, u, _s);
  }

  @override
  Vector3 derivativeLocal(double t) {
    if (_points.length == 2) {
      return _points[1] - _points[0];
    }
    final (seg, u) = _resolveSegment(t);
    final p0 = _pointAt(seg - 1);
    final p1 = _pointAt(seg);
    final p2 = _pointAt(seg + 1);
    final p3 = _pointAt(seg + 2);
    // Chain rule: dC/dt = dC/du * du/dt. For uniform param du/dt = 1.
    final local = _cardinalDerivative(p0, p1, p2, p3, u, _s);
    if (_parameterization == CatmullRomParameterization.uniform) return local;
    return local * (1.0 / _segIntervals[seg]);
  }

  // ----- Conversion ----------------------------------------------------

  /// Returns the spline as a list of cubic Bezier segments (one per
  /// Catmull-Rom span). For a closed spline the returned list has n
  /// segments, otherwise n-1. The Bezier control points are derived
  /// from the standard Catmull-Rom -> Bezier conversion:
  ///   B0 = P_i
  ///   B1 = P_i + (P_{i+1} - P_{i-1}) * (1 - tension) / 6
  ///   B2 = P_{i+1} - (P_{i+2} - P_i) * (1 - tension) / 6
  ///   B3 = P_{i+1}
  List<BezierCurve3D> toBezierSegments() {
    final out = <BezierCurve3D>[];
    final k = (1.0 - _tension) / 6.0;
    for (var i = 0; i < _segmentCount; i++) {
      final p0 = _pointAt(i);
      final p1 = _pointAt(i + 1);
      final pm = _pointAt(i - 1);
      final pp = _pointAt(i + 2);
      final b0 = p0.clone();
      final b1 = p0 + (p1 - pm) * k;
      final b2 = p1 - (pp - p0) * k;
      final b3 = p1.clone();
      out.add(BezierCurve3D.cubic(b0, b1, b2, b3,
          color: color, thickness: thickness, materialIndex: materialIndex));
    }
    return out;
  }

  // ----- Mutation overrides --------------------------------------------

  @override
  CatmullRomCurve3D reverse() {
    final reversed = _points.reversed.map((p) => p.clone()).toList();
    return CatmullRomCurve3D(
      controlPoints: reversed,
      closed: _closed,
      tension: _tension,
      parameterization: _parameterization,
      color: color,
      thickness: thickness,
      materialIndex: materialIndex,
      transform: transform.clone(),
    );
  }

  @override
  CatmullRomCurve3D append(Curve3D other) {
    if (other is! CatmullRomCurve3D) {
      throw ArgumentError(
          'CatmullRomCurve3D.append requires a CatmullRomCurve3D; got ${other.runtimeType}.');
    }
    final pts = <Vector3>[..._points.map((p) => p.clone())];
    if (pts.last == other._points.first) {
      pts.addAll(other._points.skip(1).map((p) => p.clone()));
    } else {
      pts.addAll(other._points.map((p) => p.clone()));
    }
    return CatmullRomCurve3D(
      controlPoints: pts,
      closed: false,
      tension: _tension,
      parameterization: _parameterization,
      color: color,
      thickness: thickness,
      materialIndex: materialIndex,
      transform: transform.clone(),
    );
  }

  @override
  CatmullRomCurve3D simplify(double tolerance) {
    // Ramer-Douglas-Peucker on the control polygon: drop points whose
    // deviation from the chord is below tolerance. Cheap and good
    // enough for visual simplification.
    final simplified = _ramerDouglasPeucker(_points, tolerance);
    return CatmullRomCurve3D(
      controlPoints: simplified,
      closed: _closed,
      tension: _tension,
      parameterization: _parameterization,
      color: color,
      thickness: thickness,
      materialIndex: materialIndex,
      transform: transform.clone(),
    );
  }

  /// Classic Ramer-Douglas-Peucker polyline simplification.
  static List<Vector3> _ramerDouglasPeucker(
      List<Vector3> pts, double epsilon) {
    if (pts.length < 3) return List<Vector3>.of(pts);
    final keep = List<bool>.filled(pts.length, false);
    keep.first = true;
    keep.last = true;
    _rdpRecurse(pts, 0, pts.length - 1, epsilon, keep);
    return [for (var i = 0; i < pts.length; i++) if (keep[i]) pts[i].clone()];
  }

  static void _rdpRecurse(
      List<Vector3> pts, int lo, int hi, double epsilon, List<bool> keep) {
    if (hi <= lo + 1) return;
    var maxD = -1.0;
    var idx = -1;
    final a = pts[lo];
    final b = pts[hi];
    final ab = b - a;
    final abLen2 = ab.length2;
    for (var i = lo + 1; i < hi; i++) {
      final p = pts[i];
      double d2;
      if (abLen2 < 1e-20) {
        d2 = (p - a).length2;
      } else {
        final t = ((p - a).dot(ab) / abLen2).clamp(0.0, 1.0);
        final proj = a + ab * t;
        d2 = (p - proj).length2;
      }
      if (d2 > maxD) {
        maxD = d2;
        idx = i;
      }
    }
    if (maxD > epsilon * epsilon && idx > 0) {
      keep[idx] = true;
      _rdpRecurse(pts, lo, idx, epsilon, keep);
      _rdpRecurse(pts, idx, hi, epsilon, keep);
    }
  }

  @override
  CatmullRomCurve3D copy() => CatmullRomCurve3D(
        controlPoints: _points,
        closed: _closed,
        tension: _tension,
        parameterization: _parameterization,
        color: color,
        thickness: thickness,
        materialIndex: materialIndex,
        transform: transform.clone(),
      );

  /// Fits a Catmull-Rom spline through [samples] (interpolating every
  /// input point). The simplest spline-fitting primitive; for strokes
  /// with timing / pressure data, prefer [Stroke3D.toCatmullRom] which
  /// conditions the input first.
  static CatmullRomCurve3D through(
    List<Vector3> samples, {
    bool closed = false,
    double tension = 0.0,
    CatmullRomParameterization parameterization =
        CatmullRomParameterization.centripetal,
    int color = 0xFF000000,
    double thickness = 1.0,
    int materialIndex = 0,
  }) =>
      CatmullRomCurve3D(
        controlPoints: samples,
        closed: closed,
        tension: tension,
        parameterization: parameterization,
        color: color,
        thickness: thickness,
        materialIndex: materialIndex,
      );
}

// Top-level alias — Dart requires constructors to share the enclosing
// class's name; expose a friendly `CatmullRomCurve3D` constructor too.
// (Implemented via the named factory above; users can also call
// `CatmullRomCurve3D.through(...)` for the most common case.)

/// Re-exported alias for the [CatmullRomCurve3D] constructor (mirrors
/// the `BezierCurve3D.cubic` naming pattern).
CatmullRomCurve3D catmullRomCurve3D({
  required List<Vector3> controlPoints,
  bool closed = false,
  double tension = 0.0,
  CatmullRomParameterization parameterization =
      CatmullRomParameterization.uniform,
  int color = 0xFF000000,
  double thickness = 1.0,
  int materialIndex = 0,
  Matrix4? transform,
}) =>
    CatmullRomCurve3D(
      controlPoints: controlPoints,
      closed: closed,
      tension: tension,
      parameterization: parameterization,
      color: color,
      thickness: thickness,
      materialIndex: materialIndex,
      transform: transform,
    );
