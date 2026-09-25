// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// curve3d.dart — Abstract base class for all 3D curves in Feather-Krita.
//
// A [Curve3D] is a parametric curve C: [tMin, tMax] -> R^3 carrying the
// visual properties (color, thickness, materialIndex) needed to render
// it as lit 3D geometry. The base class provides every operation that
// can be expressed purely in terms of `evaluate` / `derivative` — arc
// length, adaptive flattening, bounding box, closest point, curvature —
// and delegates representation-specific mutations (reverse, append,
// simplify, degree change) to concrete subclasses.
//
// The base class also owns a [transform] matrix (local -> parent) that
// is composed into the public sampling methods so transforms can be
// applied without mutating the underlying control points. Subclasses
// sample in *local* space and let the base handle the world transform.

import 'package:feather_krita/core/math/math.dart';
import 'package:feather_krita/engine/curves/curve_math.dart';

/// Discriminator used by [CurveSerializer] to round-trip subclasses.
enum CurveKind { base, bezier, catmullRom, nurbs }

/// A flat sample produced by [Curve3D.flatten] / [Curve3D.subdivide].
class CurveSample {
  const CurveSample({
    required this.t,
    required this.point,
    this.tangent,
    this.normal,
    this.curvature = 0.0,
  });

  /// Curve parameter at this sample.
  final double t;

  /// World-space position (transform already applied).
  final Vector3 point;

  /// Unit tangent at the sample, or `null` if not computed.
  final Vector3? tangent;

  /// Unit normal at the sample (perpendicular to tangent), or `null`.
  final Vector3? normal;

  /// Discrete curvature at the sample (1 / radius of osculating
  /// circle). 0 means straight.
  final double curvature;

  CurveSample copyWith({
    double? t,
    Vector3? point,
    Vector3? tangent,
    Vector3? normal,
    double? curvature,
  }) =>
      CurveSample(
        t: t ?? this.t,
        point: point ?? this.point,
        tangent: tangent ?? this.tangent,
        normal: normal ?? this.normal,
        curvature: curvature ?? this.curvature,
      );
}

/// Abstract parametric 3D curve.
abstract class Curve3D {
  Curve3D({
    this.color = 0xFF000000,
    this.thickness = 1.0,
    this.materialIndex = 0,
    Matrix4? transform,
  }) : transform = transform ?? Matrix4.identity();

  /// ARGB-packed stroke color (0xAARRGGBB).
  int color;

  /// Base thickness in world units (at unit pressure). Subclasses may
  /// store per-sample pressure separately.
  double thickness;

  /// Index into the scene's material table — picks shading params
  /// (roughness, metalness, emissive) when the curve is rendered as a
  /// tube / ribbon mesh.
  int materialIndex;

  /// Local-to-parent transform. Composed into [sampleAt] / [derivativeAt]
  /// so callers never see raw local-space coordinates from the public
  /// API.
  Matrix4 transform;

  /// Discriminator for (de)serialization. Subclasses override.
  CurveKind get kind => CurveKind.base;

  /// The curve's parameter domain. Default is `[0, 1]`; subclasses
  /// with explicit domains (e.g. NURBS knot spans) override.
  (double tMin, double tMax) domain() => (0.0, 1.0);

  /// Evaluates the curve at parameter [t] in LOCAL space (transform
  /// not applied). Subclasses must implement.
  Vector3 evaluateLocal(double t);

  /// First derivative of the curve at [t] in LOCAL space. Subclasses
  /// may override for analytic derivatives; the default uses a central
  /// finite difference which is plenty accurate for visual work.
  Vector3 derivativeLocal(double t) {
    final (tMin, tMax) = domain();
    final span = tMax - tMin;
    final h = 1e-5 * span.abs().clamp(1e-3, 1.0);
    return (evaluateLocal(t + h) - evaluateLocal(t - h)) * (0.5 / h);
  }

  // ----- Public sampling (transform-aware) -----------------------------

  /// Samples the curve at parameter [t] in WORLD space.
  Vector3 sampleAt(double t) =>
      transform.transform3(evaluateLocal(t).clone());

  /// First derivative at [t] in WORLD space (transform applied to the
  /// local derivative; note: only the linear part of the matrix is
  /// meaningful for direction vectors).
  Vector3 derivativeAt(double t) {
    final local = derivativeLocal(t);
    return _transformDirection(transform, local);
  }

  /// Returns the unit tangent at [t] (world space). Falls back to the
  /// chord direction if the derivative is degenerate.
  Vector3 tangentAt(double t) {
    final d = derivativeAt(t);
    if (d.length2 < 1e-20) {
      final (tMin, tMax) = domain();
      final span = (tMax - tMin).abs();
      if (span < 1e-12) return Vector3(1, 0, 0);
      final p0 = sampleAt(tMin);
      final p1 = sampleAt(tMax);
      final chord = p1 - p0;
      if (chord.length2 < 1e-20) return Vector3(1, 0, 0);
      return chord..normalize();
    }
    return d..normalize();
  }

  /// Discrete curvature at parameter [t] (world space). Uses the
  /// analytic formula `|C' x C''| / |C'|^3` via finite differences.
  double curvatureAt(double t) {
    final (tMin, tMax) = domain();
    final span = tMax - tMin;
    final h = 1e-4 * span.abs().clamp(1e-3, 1.0);
    return CurveMath.curvatureAt(sampleAt, t, dt: h);
  }

  // ----- Length & sampling ---------------------------------------------

  /// Total arc length of the curve. Uses adaptive subdivision + chord
  /// accumulation; cached after the first call (callers must call
  /// [invalidateLength] after mutating the curve).
  double length() {
    if (_cachedLength != null) return _cachedLength!;
    final flat = flatten(tolerance: 1e-3);
    var sum = 0.0;
    for (var i = 1; i < flat.length; i++) {
      sum += (flat[i].point - flat[i - 1].point).length;
    }
    _cachedLength = sum;
    return sum;
  }

  double? _cachedLength;

  /// Resets the cached length / arc-length table. Subclasses that
  /// mutate their control points must call this.
  void invalidateLength() {
    _cachedLength = null;
    _arcTable = null;
  }

  ArcLengthTable? _arcTable;

  /// Builds (or returns the cached) arc-length reparameterisation
  /// table with [samples] slices.
  ArcLengthTable arcLengthTable({int samples = 128}) {
    if (_arcTable != null && _arcTable!.entries.length == samples + 1) {
      return _arcTable!;
    }
    final (tMin, tMax) = domain();
    _arcTable = CurveMath.arcLengthTable(sampleAt,
        samples: samples, t0: tMin, t1: tMax);
    return _arcTable!;
  }

  /// Returns the parameter `t` whose arc length is [s] (in [0, length]).
  double parameterForArcLength(double s) =>
      arcLengthTable().parameterAt(s.clamp(0.0, length()));

  /// Returns `n+1` evenly arc-length-spaced samples along the curve.
  List<CurveSample> resample(int n) {
    if (n < 1) return const <CurveSample>[];
    final table = arcLengthTable();
    final total = table.total;
    final out = <CurveSample>[];
    for (var i = 0; i <= n; i++) {
      final s = total * i / n;
      final t = table.parameterAt(s);
      out.add(CurveSample(t: t, point: sampleAt(t)));
    }
    return out;
  }

  // ----- Flattening & subdivision --------------------------------------

  /// Flattens the curve into a polyline of [CurveSample]s such that
  /// the maximum chord deviation is below [tolerance] (in world units).
  /// Uses recursive midpoint flatness testing.
  List<CurveSample> flatten({double tolerance = 0.1, int maxDepth = 16}) {
    final (tMin, tMax) = domain();
    final ts = CurveMath.adaptiveSubdivide(
      sampleAt,
      tMin,
      tMax,
      tolerance: tolerance,
      maxDepth: maxDepth,
    );
    final out = <CurveSample>[];
    for (final t in ts) {
      out.add(CurveSample(
        t: t,
        point: sampleAt(t),
        tangent: tangentAt(t),
      ));
    }
    return out;
  }

  /// Subdivides the curve into a list of [n] sub-curves of equal
  /// parameter span. The base implementation builds a polyline of
  /// [CurveSample]s; subclasses that support true subdivision (e.g.
  /// Bezier.split) override to return a list of `Curve3D` instances.
  ///
  /// The default returns the flattened samples grouped into [n]
  /// chunks (a faithful approximation suitable for ribbon rendering).
  List<List<CurveSample>> subdivide(int n, {double tolerance = 0.1}) {
    if (n < 1) n = 1;
    final flat = flatten(tolerance: tolerance);
    final out = <List<CurveSample>>[];
    final per = flat.length / n;
    for (var i = 0; i < n; i++) {
      final lo = (i * per).floor();
      final hi = i == n - 1 ? flat.length : ((i + 1) * per).ceil() + 1;
      out.add(flat.sublist(lo, hi.clamp(0, flat.length)));
    }
    return out;
  }

  // ----- Bounding box --------------------------------------------------

  /// Computes the world-space AABB of the curve by sampling at
  /// `2 * resolution + 1` parameter values. Default resolution is 32
  /// which is plenty for cubic beziers / short splines; callers with
  /// very long, wiggly curves should bump it up.
  Aabb3 boundingBox({int resolution = 32}) {
    final (tMin, tMax) = domain();
    final out = Aabb3();
    for (var i = 0; i <= resolution; i++) {
      final t = tMin + (tMax - tMin) * i / resolution;
      out.hullPoint(sampleAt(t));
    }
    // Pad by the thickness so culling is conservative — hull each
    // corner of the expanded box.
    final pad = thickness * 0.5;
    if (pad > 0) {
      final mn = out.min;
      final mx = out.max;
      out.hullPoint(mn - Vector3.all(pad));
      out.hullPoint(mx + Vector3.all(pad));
      out.hullPoint(Vector3(mn.x - pad, mn.y - pad, mx.z + pad));
      out.hullPoint(Vector3(mn.x - pad, mx.y + pad, mn.z - pad));
      out.hullPoint(Vector3(mx.x + pad, mn.y - pad, mn.z - pad));
      out.hullPoint(Vector3(mn.x - pad, mx.y + pad, mx.z + pad));
      out.hullPoint(Vector3(mx.x + pad, mn.y - pad, mx.z + pad));
      out.hullPoint(Vector3(mx.x + pad, mx.y + pad, mn.z - pad));
    }
    return out;
  }

  // ----- Closest point -------------------------------------------------

  /// Finds the parameter `t` of the closest point on the curve to
  /// [p] (world space). Uses a coarse uniform scan (resolution samples)
  /// followed by Newton refinement.
  ({double t, Vector3 point, double distance}) closestPoint(Vector3 p,
      {int resolution = 64}) {
    final (tMin, tMax) = domain();
    var bestT = tMin;
    var bestD2 = double.infinity;
    for (var i = 0; i <= resolution; i++) {
      final t = tMin + (tMax - tMin) * i / resolution;
      final d2 = (sampleAt(t) - p).length2;
      if (d2 < bestD2) {
        bestD2 = d2;
        bestT = t;
      }
    }
    // Newton refinement on `f(t) = (C(t) - p) . C'(t) = 0`.
    final span = tMax - tMin;
    final h = 1e-4 * span.abs().clamp(1e-3, 1.0);
    var t = bestT;
    for (var iter = 0; iter < 8; iter++) {
      final c = sampleAt(t);
      final d = derivativeAt(t);
      final f = (c - p).dot(d);
      final d2 = (sampleAt(t + h) - sampleAt(t - h)).scaled(0.5 / h);
      final fPrime = d.dot(d) + (c - p).dot(d2);
      if (fPrime.abs() < 1e-12) break;
      final step = f / fPrime;
      t = (t - step).clamp(tMin, tMax);
      if (step.abs() < 1e-7 * span.abs()) break;
    }
    final point = sampleAt(t);
    return (t: t, point: point, distance: (point - p).length);
  }

  // ----- Mutation API (subclass-implemented) ---------------------------

  /// Applies a [Matrix4] to the curve. The base implementation
  /// composes the matrix into [transform]; subclasses that want to
  /// bake the transform into their control points instead should
  /// override.
  void applyTransform(Matrix4 m) {
    transform = m * transform;
    invalidateLength();
  }

  /// Reverses the curve's parameter direction (start becomes end and
  /// vice versa). Subclasses must override.
  Curve3D reverse();

  /// Appends [other] to the end of this curve and returns the merged
  /// result. The two curves must share an endpoint; otherwise a gap
  /// is left. Subclasses must override.
  Curve3D append(Curve3D other);

  /// Simplifies the curve (reduces control point count) while keeping
  /// deviation under [tolerance]. Subclasses must override.
  Curve3D simplify(double tolerance);

  /// Returns a deep copy of this curve.
  Curve3D copy();

  // ----- Helpers --------------------------------------------------------

  /// Transforms a direction vector by [m], ignoring translation.
  static Vector3 _transformDirection(Matrix4 m, Vector3 v) {
    final out = Vector3.zero();
    final s = m.storage;
    // Column-major Matrix4 storage. Direction = upper-left 3x3.
    out.x = s[0] * v.x + s[4] * v.y + s[8] * v.z;
    out.y = s[1] * v.x + s[5] * v.y + s[9] * v.z;
    out.z = s[2] * v.x + s[6] * v.y + s[10] * v.z;
    return out;
  }

  @override
  String toString() =>
      '$runtimeType(color=0x${color.toRadixString(16).padLeft(8, '0')}, '
      'thickness=$thickness, material=$materialIndex)';
}

