// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// stroke3d.dart — User-drawn 3D stroke (the curves-package view).
//
// A [Stroke3D] is the *raw* input that a stylus / mouse produces while
// the user is drawing: an ordered list of [StrokeSample3D]s, each
// carrying a 3D position, normalized pressure, stylus tilt, and a
// timestamp (microseconds since the stroke started). It is the
// "uncurated" form of a stroke — straight from the pointer events to
// the engine — before smoothing, simplification, or curve-fitting.
//
// This class is distinct from [Stroke] in `lib/models/stroke.dart`:
//   - [Stroke] is the *document* model: a finished stroke stored in a
//     project, with a transform, an ID, brush-type metadata, etc.
//   - [Stroke3D] is the *capture* model: the live in-flight samples
//     being captured by the pointer handler, plus the operations to
//     clean them up (smooth / simplify / fit to a Curve3D) before they
//     get promoted to a [Stroke] for storage.
//
// The two are bridged by [Stroke3D.toBezier] / [toCatmullRom] /
// [toNurbs] producing a [Curve3D] that the renderer can draw and the
// serializer can persist.

import 'dart:math' as math;

import 'package:feather_krita/core/math/math.dart';
import 'package:feather_krita/engine/curves/curve3d.dart';
import 'package:feather_krita/engine/curves/bezier_curve3d.dart';
import 'package:feather_krita/engine/curves/catmull_rom_curve3d.dart';
import 'package:feather_krita/engine/curves/nurbs_curve3d.dart';
import 'package:feather_krita/engine/curves/stroke_smoother.dart';

/// One captured sample of a 3D stroke.
class StrokeSample3D {
  StrokeSample3D({
    required this.position,
    this.pressure = 0.5,
    Vector2? tilt,
    this.time = 0.0,
    this.uv,
    this.velocity,
  }) : tilt = tilt ?? Vector2.zero();

  /// 3D position (world space, before the stroke's transform).
  final Vector3 position;

  /// Normalized stylus pressure in [0, 1]. For mouse / touch input
  /// this is 0.5 (constant).
  final double pressure;

  /// Stylus tilt in degrees (X, Y).
  final Vector2 tilt;

  /// Time in seconds since the stroke started.
  final double time;

  /// Texture-space coordinate at the sample (when the sample was
  /// captured via a raycast against a guide surface).
  final Vector2? uv;

  /// Optional pre-computed speed in world units / second. `null` when
  /// the stroke smoother hasn't computed it yet.
  final double? velocity;

  /// Linearly interpolates between this sample and [other] by [t].
  StrokeSample3D lerp(StrokeSample3D other, double t) => StrokeSample3D(
        position: position * (1.0 - t) + other.position * t,
        pressure: pressure * (1.0 - t) + other.pressure * t,
        tilt: tilt * (1.0 - t) + other.tilt * t,
        time: time * (1.0 - t) + other.time * t,
        uv: uv == null || other.uv == null
            ? null
            : uv! * (1.0 - t) + other.uv! * t,
        velocity: velocity == null || other.velocity == null
            ? null
            : velocity! * (1.0 - t) + other.velocity! * t,
      );

  StrokeSample3D copy() => StrokeSample3D(
        position: position.clone(),
        pressure: pressure,
        tilt: tilt.clone(),
        time: time,
        uv: uv?.clone(),
        velocity: velocity,
      );

  Map<String, dynamic> toJson() => {
        'x': position.x,
        'y': position.y,
        'z': position.z,
        'p': pressure,
        'tx': tilt.x,
        'ty': tilt.y,
        't': time,
        if (uv != null) 'u': uv!.x,
        if (uv != null) 'v': uv!.y,
        if (velocity != null) 'vz': velocity,
      };

  factory StrokeSample3D.fromJson(Map<String, dynamic> json) =>
      StrokeSample3D(
        position: Vector3(
          (json['x'] as num).toDouble(),
          (json['y'] as num).toDouble(),
          (json['z'] as num).toDouble(),
        ),
        pressure: (json['p'] as num?)?.toDouble() ?? 0.5,
        tilt: Vector2(
          (json['tx'] as num?)?.toDouble() ?? 0.0,
          (json['ty'] as num?)?.toDouble() ?? 0.0,
        ),
        time: (json['t'] as num?)?.toDouble() ?? 0.0,
        uv: json['u'] == null || json['v'] == null
            ? null
            : Vector2(
                (json['u'] as num).toDouble(),
                (json['v'] as num).toDouble(),
              ),
        velocity: (json['vz'] as num?)?.toDouble(),
      );

  @override
  String toString() =>
      'StrokeSample3D(pos=$position, p=$pressure, t=$time, v=$velocity)';
}

/// Source device that produced a stroke — affects default smoothing.
enum StrokeInputDevice {
  /// Apple Pencil / Wacom / Surface Pen — full pressure + tilt.
  stylus,

  /// Mouse — constant pressure, no tilt.
  mouse,

  /// Finger / capacitive touch — coarse, no pressure.
  touch,
}

/// A 3D stroke: raw samples + capture metadata + clean-up + curve-fit
/// operations.
class Stroke3D {
  Stroke3D({
    List<StrokeSample3D>? samples,
    this.color = 0xFF000000,
    this.thickness = 4.0,
    this.materialIndex = 0,
    this.inputDevice = StrokeInputDevice.stylus,
    this.source = 'unknown',
    Matrix4? transform,
    this.closed = false,
  })  : samples = samples ?? <StrokeSample3D>[],
        transform = transform ?? Matrix4.identity();

  /// Ordered list of stroke samples (oldest first).
  final List<StrokeSample3D> samples;

  /// ARGB-packed stroke color.
  int color;

  /// Maximum stroke thickness in world units (at pressure = 1).
  double thickness;

  /// Material slot index for shading.
  int materialIndex;

  /// What kind of device captured this stroke.
  StrokeInputDevice inputDevice;

  /// Free-form source identifier — typically the platform-specific
  /// device name ("Apple Pencil", "Surface Pen", "Mouse", etc.).
  String source;

  /// Local-to-world transform applied to all samples when converting
  /// to a [Curve3D] (the sample positions themselves are stored
  /// untransformed for replay fidelity).
  Matrix4 transform;

  /// Whether the stroke is a closed loop (start == end).
  bool closed;

  /// Number of samples in the stroke.
  int get length => samples.length;

  /// True if the stroke has no samples.
  bool get isEmpty => samples.isEmpty;

  /// True if the stroke has at least one sample.
  bool get isNotEmpty => samples.isNotEmpty;

  /// Appends a sample. The first sample sets the stroke's t=0; later
  /// samples should have monotonically increasing [StrokeSample3D.time].
  void addSample(StrokeSample3D sample) {
    samples.add(sample);
    _cachedLength = null;
  }

  /// Appends many samples at once (bulk capture / replay).
  void addSamples(Iterable<StrokeSample3D> newSamples) {
    samples.addAll(newSamples);
    _cachedLength = null;
  }

  /// Removes all samples.
  void clear() {
    samples.clear();
    _cachedLength = null;
  }

  /// The world-space position of sample [i] (transform applied).
  Vector3 worldPosition(int i) =>
      transform.transform3(samples[i].position.clone());

  // ----- Length & bounds ----------------------------------------------

  double? _cachedLength;

  /// Total polyline length (sum of inter-sample distances in world
  /// space). Cached; cleared on [addSample] / [clear]. Named
  /// `polylineLength` to avoid clashing with the `length` getter that
  /// returns the sample count.
  double polylineLength() {
    if (_cachedLength != null) return _cachedLength!;
    if (samples.length < 2) {
      _cachedLength = 0.0;
      return 0.0;
    }
    var sum = 0.0;
    for (var i = 1; i < samples.length; i++) {
      sum += (worldPosition(i) - worldPosition(i - 1)).length;
    }
    if (closed && samples.length > 1) {
      sum += (worldPosition(0) - worldPosition(samples.length - 1)).length;
    }
    _cachedLength = sum;
    return sum;
  }

  /// World-space bounding box of the stroke samples, padded by
  /// `thickness/2` so culling is conservative.
  Aabb3 boundingBox() {
    final out = Aabb3();
    if (samples.isEmpty) {
      return out;
    }
    for (var i = 0; i < samples.length; i++) {
      out.hullPoint(worldPosition(i));
    }
    final pad = thickness * 0.5;
    if (pad > 0) {
      final mn = out.min;
      final mx = out.max;
      out.hullPoint(mn - Vector3.all(pad));
      out.hullPoint(mx + Vector3.all(pad));
    }
    return out;
  }

  /// Returns the average capture velocity (world units / second) over
  /// the stroke. 0 when the stroke has < 2 samples or no time delta.
  double averageVelocity() {
    if (samples.length < 2) return 0.0;
    final dt = samples.last.time - samples.first.time;
    if (dt < 1e-6) return 0.0;
    return polylineLength() / dt;
  }

  /// Computes per-sample velocity (world units / second) using forward
  /// differences. The first sample's velocity is the same as the
  /// second's. Returns a list of length [length].
  List<double> computeVelocities() {
    final out = List<double>.filled(samples.length, 0.0);
    if (samples.length < 2) return out;
    for (var i = 1; i < samples.length; i++) {
      final dx = (worldPosition(i) - worldPosition(i - 1)).length;
      final dt = samples[i].time - samples[i - 1].time;
      out[i] = dt > 1e-6 ? dx / dt : 0.0;
    }
    out[0] = out[1];
    return out;
  }

  // ----- Clean-up -----------------------------------------------------

  /// Returns a smoothed copy of this stroke. See [StrokeSmoother] for
  /// the algorithm details.
  Stroke3D smooth({
    double strength = 0.5,
    StrokeSmoothingAlgorithm algorithm = StrokeSmoothingAlgorithm.gaussian,
  }) {
    final smoothed = StrokeSmoother.smooth(samples,
        strength: strength, algorithm: algorithm);
    return Stroke3D(
      samples: smoothed,
      color: color,
      thickness: thickness,
      materialIndex: materialIndex,
      inputDevice: inputDevice,
      source: source,
      transform: transform.clone(),
      closed: closed,
    );
  }

  /// Returns a simplified copy of this stroke with samples whose
  /// deviation from the local chord is below [tolerance] removed.
  /// Uses Ramer-Douglas-Peucker. Pressure / tilt / time of the
  /// surviving samples are preserved exactly.
  Stroke3D simplify(double tolerance) {
    if (samples.length < 3) return copy();
    final positions = samples.map((s) => s.position.clone()).toList();
    final keep = List<bool>.filled(samples.length, false);
    keep.first = true;
    keep.last = true;
    _rdpRecurse(positions, 0, samples.length - 1, tolerance, keep);
    final out = <StrokeSample3D>[
      for (var i = 0; i < samples.length; i++)
        if (keep[i]) samples[i]
    ];
    return Stroke3D(
      samples: out,
      color: color,
      thickness: thickness,
      materialIndex: materialIndex,
      inputDevice: inputDevice,
      source: source,
      transform: transform.clone(),
      closed: closed,
    );
  }

  static void _rdpRecurse(
      List<Vector3> pts, int lo, int hi, double epsilon, List<bool> keep) {
    if (hi <= lo + 1) return;
    var maxD2 = -1.0;
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
      if (d2 > maxD2) {
        maxD2 = d2;
        idx = i;
      }
    }
    if (maxD2 > epsilon * epsilon && idx > 0) {
      keep[idx] = true;
      _rdpRecurse(pts, lo, idx, epsilon, keep);
      _rdpRecurse(pts, idx, hi, epsilon, keep);
    }
  }

  // ----- Curve fitting -------------------------------------------------

  /// Fits a Catmull-Rom spline through the (transformed) sample
  /// positions. The default [parameterization] is centripetal, which
  /// is cusp-free for uneven stylus input.
  CatmullRomCurve3D toCatmullRom({
    double tension = 0.0,
    CatmullRomParameterization parameterization =
        CatmullRomParameterization.centripetal,
  }) {
    final pts = samples.map((s) => s.position.clone()).toList();
    return CatmullRomCurve3D(
      controlPoints: pts,
      closed: closed,
      tension: tension,
      parameterization: parameterization,
      color: color,
      thickness: thickness,
      materialIndex: materialIndex,
      transform: transform.clone(),
    );
  }

  /// Fits a list of cubic Bezier segments to the samples using
  /// Schneider-style chord-length parameterization + Newton-Raphson
  /// refinement. The result is a single "composite" Bezier (the
  /// segments share endpoints). Set [maxError] higher for looser fit
  /// (fewer segments); lower for tighter fit (more segments).
  ///
  /// Pressure data is currently dropped — the Bezier's [thickness] is
  /// the stroke's nominal [thickness]. Per-vertex pressure variation
  /// is handled by the [CurveRenderer] when building the tube mesh.
  BezierCurve3D toBezier({double maxError = 0.5}) {
    if (samples.isEmpty) {
      return BezierCurve3D.cubic(
        Vector3.zero(),
        Vector3.zero(),
        Vector3.zero(),
        Vector3.zero(),
        color: color,
        thickness: thickness,
        materialIndex: materialIndex,
      );
    }
    if (samples.length < 4) {
      // Pad with extrapolated samples so fitCubic has at least 2.
      final pts = samples.map((s) => s.position).toList();
      while (pts.length < 4) {
        final last = pts.last;
        final prev = pts.length >= 2 ? pts[pts.length - 2] : last;
        pts.add(last + (last - prev));
      }
      return BezierCurve3D.fitCubic(pts,
          maxError: maxError,
          color: color,
          thickness: thickness,
          materialIndex: materialIndex);
    }
    // Adaptive segmentation: keep adding samples to the current
    // segment until fitCubic's error exceeds maxError, then start a
    // new segment.
    final segments = <List<Vector3>>[];
    var current = <Vector3>[samples.first.position];
    for (var i = 1; i < samples.length; i++) {
      current.add(samples[i].position);
      final test = BezierCurve3D.fitCubic(current, maxError: maxError);
      var maxDev = 0.0;
      final denom = (current.length - 1).clamp(1, 1 << 30);
      for (var j = 0; j < current.length; j++) {
        final t = j / denom;
        final dev = (test.sampleAt(t) - current[j]).length;
        if (dev > maxDev) maxDev = dev;
      }
      if (maxDev > maxError * 1.5 && current.length > 4) {
        // Finalize current segment without this sample.
        current.removeLast();
        segments.add(current);
        current = <Vector3>[samples[i - 1].position, samples[i].position];
      }
    }
    if (current.length >= 2) segments.add(current);
    // Fit each segment, then merge.
    var merged = BezierCurve3D.fitCubic(segments.first,
        maxError: maxError,
        color: color,
        thickness: thickness,
        materialIndex: materialIndex);
    for (var i = 1; i < segments.length; i++) {
      final seg = BezierCurve3D.fitCubic(segments[i],
          maxError: maxError,
          color: color,
          thickness: thickness,
          materialIndex: materialIndex);
      merged = merged.append(seg);
    }
    merged.applyTransform(transform);
    return merged;
  }

  /// Fits a clamped uniform NURBS of the given [degree] through the
  /// sample positions. Default degree 3 (cubic). For sparse input the
  /// degree is clamped to `samples - 1`.
  NurbsCurve3D toNurbs({int degree = 3}) {
    final pts = samples.map((s) => s.position.clone()).toList();
    final p = math.min(degree, pts.length - 1).clamp(1, 10);
    return NurbsCurve3D.interpolating(
      pts,
      degree: p,
      color: color,
      thickness: thickness,
      materialIndex: materialIndex,
      transform: transform.clone(),
    );
  }

  /// Generic curve-fit dispatcher. Convenience for code that doesn't
  /// care which spline family the stroke ends up as.
  Curve3D toCurve(CurveKind kind) {
    switch (kind) {
      case CurveKind.bezier:
        return toBezier();
      case CurveKind.catmullRom:
        return toCatmullRom();
      case CurveKind.nurbs:
        return toNurbs();
      case CurveKind.base:
        return toCatmullRom();
    }
  }

  // ----- Copy / serialize ---------------------------------------------

  Stroke3D copy() => Stroke3D(
        samples: samples.map((s) => s.copy()).toList(),
        color: color,
        thickness: thickness,
        materialIndex: materialIndex,
        inputDevice: inputDevice,
        source: source,
        transform: transform.clone(),
        closed: closed,
      );

  Map<String, dynamic> toJson() => {
        'samples': samples.map((s) => s.toJson()).toList(),
        'color': color,
        'thickness': thickness,
        'materialIndex': materialIndex,
        'inputDevice': inputDevice.name,
        'source': source,
        'transform': transform.storage.toList(),
        'closed': closed,
      };

  factory Stroke3D.fromJson(Map<String, dynamic> json) {
    final transformList = (json['transform'] as List?)
            ?.map((e) => (e as num).toDouble())
            .toList() ??
        Matrix4.identity().storage.toList();
    return Stroke3D(
      samples: ((json['samples'] as List?) ?? [])
          .map((e) => StrokeSample3D.fromJson(e as Map<String, dynamic>))
          .toList(),
      color: (json['color'] as num?)?.toInt() ?? 0xFF000000,
      thickness: (json['thickness'] as num?)?.toDouble() ?? 4.0,
      materialIndex: (json['materialIndex'] as num?)?.toInt() ?? 0,
      inputDevice: StrokeInputDevice.values.firstWhere(
        (d) => d.name == (json['inputDevice'] as String?),
        orElse: () => StrokeInputDevice.stylus,
      ),
      source: json['source'] as String? ?? 'unknown',
      transform: Matrix4.fromList(transformList),
      closed: json['closed'] as bool? ?? false,
    );
  }
}
