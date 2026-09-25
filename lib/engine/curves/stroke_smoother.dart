// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// stroke_smoother.dart — Stroke smoothing for live and captured input.
//
// Real styli produce jittery strokes — hand tremor, sub-pixel pointer
// quantisation, the discrete sampling of pointer-move events. This
// module applies a smoothing pass that trades a small amount of
// latency / detail for a dramatic reduction in visible jitter.
//
// Four algorithms are provided, each with a distinct trade-off:
//   - [StrokeSmoothingAlgorithm.movingAverage]: symmetric box filter.
//     Zero phase lag at stroke ends (works on completed strokes); the
//     cheapest option. NOT suited to live drawing because it needs
//     future samples.
//   - [StrokeSmoothingAlgorithm.gaussian]: Gaussian-weighted window.
//     Smoother than the box filter for the same effective bandwidth,
//     at the same cost. Default for completed-stroke cleanup.
//   - [StrokeSmoothingAlgorithm.catmullRomFit]: fits a centripetal
//     Catmull-Rom spline through the samples and re-samples evenly.
//     Best visual quality at low sample counts; expensive.
//   - [StrokeSmoothingAlgorithm.stableStrokes]: one-sided exponential
//     smoothing with a tunable lag — the "Stabilizer" / "Smooth"
//     slider in Krita / Photoshop. Suitable for LIVE drawing because
//     it only uses past samples.
//
// All algorithms preserve the sample timestamps exactly (so replay
// speed matches capture speed) and only adjust positions / pressures
// / tilts. Pressure is smoothed with the same kernel as position so
// the stroke stays internally consistent.

import 'package:feather_krita/core/math/math.dart';
import 'package:feather_krita/engine/curves/catmull_rom_curve3d.dart';
import 'package:feather_krita/engine/curves/stroke3d.dart';

/// Smoothing algorithms supported by [StrokeSmoother].
enum StrokeSmoothingAlgorithm {
  /// Symmetric box (uniform-weight) moving average. Best for
  /// post-capture cleanup of completed strokes.
  movingAverage,

  /// Symmetric Gaussian-weighted window. Smoother than the box filter
  /// for the same effective bandwidth. Default.
  gaussian,

  /// Fit a centripetal Catmull-Rom spline through the samples and
  /// re-sample evenly along arc length. Best visual quality.
  catmullRomFit,

  /// One-sided exponential smoothing (a.k.a. Krita's "Stabilizer" /
  /// Photoshop's "Smooth"). Suitable for live drawing because it
  /// only uses past samples; produces a small lag proportional to
  /// the strength.
  stableStrokes,
}

/// Pure stroke-smoothing utility. Stateless; safe to call from any
/// isolate.
class StrokeSmoother {
  /// Maximum window radius (samples each side) at strength = 1.
  static const int maxRadius = 6;

  /// Maps a [0, 1] strength to a window radius in [0, maxRadius].
  static int radiusFor(double strength) {
    final s = strength.clamp(0.0, 1.0);
    return (s * maxRadius).round();
  }

  /// Smooths [samples] with the given [algorithm] at [strength] in
  /// `[0, 1]`. Returns a new list of the same length. The original
  /// list is never mutated.
  static List<StrokeSample3D> smooth(
    List<StrokeSample3D> samples, {
    double strength = 0.5,
    StrokeSmoothingAlgorithm algorithm = StrokeSmoothingAlgorithm.gaussian,
  }) {
    if (samples.isEmpty) return <StrokeSample3D>[];
    if (samples.length < 3 || strength <= 0.0) {
      return samples.map((s) => s.copy()).toList();
    }
    switch (algorithm) {
      case StrokeSmoothingAlgorithm.movingAverage:
        return _movingAverage(samples, radiusFor(strength));
      case StrokeSmoothingAlgorithm.gaussian:
        return _gaussian(samples, radiusFor(strength));
      case StrokeSmoothingAlgorithm.catmullRomFit:
        return _catmullRomFit(samples, strength);
      case StrokeSmoothingAlgorithm.stableStrokes:
        return _stableStrokes(samples, strength);
    }
  }

  // ----- Box filter ----------------------------------------------------

  static List<StrokeSample3D> _movingAverage(
      List<StrokeSample3D> samples, int radius) {
    if (radius == 0) return samples.map((s) => s.copy()).toList();
    final n = samples.length;
    final out = List<StrokeSample3D>.filled(n, samples.first.copy(),
        growable: false);
    for (var i = 0; i < n; i++) {
      final lo = i - radius < 0 ? 0 : i - radius;
      final hi = i + radius >= n ? n - 1 : i + radius;
      out[i] = _lerpWindow(samples, lo, hi);
    }
    return out;
  }

  // ----- Gaussian ------------------------------------------------------

  static List<StrokeSample3D> _gaussian(
      List<StrokeSample3D> samples, int radius) {
    if (radius == 0) return samples.map((s) => s.copy()).toList();
    // Pre-compute the Gaussian kernel for [-radius, radius].
    final sigma = radius / 2.0;
    final kernel = List<double>.generate(2 * radius + 1,
        (i) => gaussianWeight((i - radius).toDouble(), sigma));
    final kSum = kernel.reduce((a, b) => a + b);
    final n = samples.length;
    final out = List<StrokeSample3D>.filled(n, samples.first.copy(),
        growable: false);
    for (var i = 0; i < n; i++) {
      var sx = 0.0, sy = 0.0, sz = 0.0;
      var sp = 0.0;
      var stx = 0.0, sty = 0.0;
      var wsum = 0.0;
      for (var k = -radius; k <= radius; k++) {
        final j = (i + k).clamp(0, n - 1);
        final w = kernel[k + radius] / kSum;
        final s = samples[j];
        sx += s.position.x * w;
        sy += s.position.y * w;
        sz += s.position.z * w;
        sp += s.pressure * w;
        stx += s.tilt.x * w;
        sty += s.tilt.y * w;
        wsum += w;
      }
      final base = samples[i];
      out[i] = StrokeSample3D(
        position: Vector3(sx / wsum, sy / wsum, sz / wsum),
        pressure: sp / wsum,
        tilt: Vector2(stx / wsum, sty / wsum),
        time: base.time,
        uv: base.uv?.clone(),
        velocity: base.velocity,
      );
    }
    return out;
  }

  // ----- Catmull-Rom refit --------------------------------------------

  static List<StrokeSample3D> _catmullRomFit(
      List<StrokeSample3D> samples, double strength) {
    // Fit a centripetal Catmull-Rom through the sample positions,
    // evaluate it at the original (chord-length) parameter values,
    // and emit new samples. Pressure / tilt / time are kept from the
    // original samples (they're already at the right t).
    final pts = samples.map((s) => s.position.clone()).toList();
    // Add phantom endpoints by reflecting — needed so the spline
    // interpolates the actual first / last sample.
    if (pts.length >= 2) {
      pts.insert(0, pts[0] * 2.0 - pts[1]);
      pts.add(pts.last * 2.0 - pts[pts.length - 2]);
    }
    // Build a centripetal Catmull-Rom through the augmented list.
    final spline = CatmullRomCurve3D(
      controlPoints: pts,
      parameterization: CatmullRomParameterization.centripetal,
    );
    final n = samples.length;
    final out = List<StrokeSample3D>.filled(n, samples.first.copy(),
        growable: false);
    for (var i = 0; i < n; i++) {
      // t = i+1 (since we prepended a phantom). The spline domain
      // is [0, pts.length - 1] for an open spline.
      final t = (i + 1).toDouble();
      final smoothed = spline.evaluateLocal(t);
      // Blend toward the original position by (1 - strength) so
      // strength = 0 leaves the input untouched.
      final orig = samples[i].position;
      final blended = orig * (1.0 - strength) + smoothed * strength;
      out[i] = StrokeSample3D(
        position: blended,
        pressure: samples[i].pressure,
        tilt: samples[i].tilt.clone(),
        time: samples[i].time,
        uv: samples[i].uv?.clone(),
        velocity: samples[i].velocity,
      );
    }
    return out;
  }

  // ----- Stable Strokes (one-sided exponential) -----------------------

  static List<StrokeSample3D> _stableStrokes(
      List<StrokeSample3D> samples, double strength) {
    // Classic one-sided exponential smoothing:
    //   y[i] = alpha * x[i] + (1 - alpha) * y[i-1]
    // where alpha = 1 - strength. strength = 0 -> alpha = 1 (no
    // smoothing); strength = 1 -> alpha = 0 (frozen). We clamp alpha
    // to a minimum of 0.05 so the stroke eventually catches up.
    final alpha = (1.0 - strength).clamp(0.05, 1.0);
    final n = samples.length;
    final out = List<StrokeSample3D>.filled(n, samples.first.copy(),
        growable: false);
    if (n == 0) return out;
    out[0] = samples.first.copy();
    for (var i = 1; i < n; i++) {
      final prev = out[i - 1];
      final cur = samples[i];
      out[i] = StrokeSample3D(
        position: prev.position * (1.0 - alpha) + cur.position * alpha,
        pressure: prev.pressure * (1.0 - alpha) + cur.pressure * alpha,
        tilt: prev.tilt * (1.0 - alpha) + cur.tilt * alpha,
        time: cur.time,
        uv: cur.uv?.clone(),
        velocity: cur.velocity,
      );
    }
    return out;
  }

  // ----- Helpers -------------------------------------------------------

  /// Computes the average of [samples] over the inclusive window
  /// [lo]-[hi]. Position / pressure / tilt / time are averaged; uv
  /// and velocity are taken from the centre sample (these are
  /// discrete attributes that don't lend themselves to averaging).
  static StrokeSample3D _lerpWindow(
      List<StrokeSample3D> samples, int lo, int hi) {
    final span = hi - lo + 1;
    var sx = 0.0, sy = 0.0, sz = 0.0;
    var sp = 0.0;
    var stx = 0.0, sty = 0.0;
    var stime = 0.0;
    Vector2? uv;
    double? vel;
    var hasUv = true;
    for (var j = lo; j <= hi; j++) {
      final p = samples[j];
      sx += p.position.x;
      sy += p.position.y;
      sz += p.position.z;
      sp += p.pressure;
      stx += p.tilt.x;
      sty += p.tilt.y;
      stime += p.time;
      if (p.uv == null) {
        hasUv = false;
      } else {
        uv ??= Vector2.zero();
        uv.x += p.uv!.x;
        uv.y += p.uv!.y;
      }
      vel ??= p.velocity;
    }
    final center = samples[(lo + hi) >> 1];
    return StrokeSample3D(
      position: Vector3(sx / span, sy / span, sz / span),
      pressure: sp / span,
      tilt: Vector2(stx / span, sty / span),
      time: stime / span,
      uv: hasUv ? uv! / span.toDouble() : null,
      velocity: vel ?? center.velocity,
    );
  }
}

/// Extension providing a `smooth()` convenience on a list of samples
/// so callers can write `samples.smooth(strength: 0.5)` without naming
/// the [StrokeSmoother] class.
extension StrokeSampleSmoother on List<StrokeSample3D> {
  List<StrokeSample3D> smooth({
    double strength = 0.5,
    StrokeSmoothingAlgorithm algorithm = StrokeSmoothingAlgorithm.gaussian,
  }) =>
      StrokeSmoother.smooth(this,
          strength: strength, algorithm: algorithm);
}
