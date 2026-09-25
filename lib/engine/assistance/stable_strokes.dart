// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// stable_strokes.dart — the Stable Strokes assistance.
//
// Real-time (CAUSAL) stroke stabilization, as documented in
// `assistance/stablestrokes.txt`. Unlike `lib/utils/stroke_smoother.dart`
// (a post-capture symmetric moving average), Stable Strokes runs WHILE
// the pen is down: it buffers recent samples and emits a delayed,
// smoothed point so the live ribbon the artist sees is steady rather
// than jittery.
//
// Design:
//   * A short FIFO of the last N raw samples (window grows with the
//     user-set intensity).
//   * Each emitted output is the weighted centroid of the window
//     (centre-weighted Gaussian) — a one-sided causal filter, so there
//     is a small lag proportional to the window size.
//   * Latency compensation: an optional one-step linear extrapolation
//     pulls the emitted point back toward the pen's most recent
//     position, recovering perceived responsiveness without
//     re-introducing jitter.
//   * The "Squeeze Menu" interaction (tap = toggle, slide = intensity)
//     maps a 0–1 vertical drag onto the intensity slider.
//
// Depends on: lib/core/math/

import 'dart:math' as math;

import 'package:feather_krita/core/math/math.dart';

/// A raw stylus sample feeding the stabilizer.
class StableSample {
  StableSample({
    required this.position,
    this.pressure = 0.5,
    Vector2? tilt,
    this.time = 0.0,
  }) : tilt = tilt ?? Vector2.zero();

  final Vector3 position;
  final double pressure;
  final Vector2 tilt;
  final double time;
}

/// The stabilized output.
class StableOutput {
  StableOutput({
    required this.position,
    required this.pressure,
    required this.tilt,
    required this.time,
    required this.confidence,
  });

  final Vector3 position;
  final double pressure;
  final Vector2 tilt;
  final double time;

  /// 0–1 — how full the buffer is. Low confidence at the start of a
  /// stroke means the consumer may want to suppress the dab until the
  /// filter has warmed up.
  final double confidence;
}

/// Configuration.
class StableStrokesConfig {
  const StableStrokesConfig({
    this.enabled = false,
    this.intensity = 0.5,
    this.maxWindow = 12,
    this.extrapolation = 0.5,
    this.minConfidence = 0.4,
  });

  /// Whether Stable Stroke is on.
  final bool enabled;

  /// 0–1 smoothing intensity (maps to a window radius 1..maxWindow).
  final double intensity;

  /// Maximum buffer size at intensity = 1.
  final int maxWindow;

  /// 0–1 how aggressively to extrapolate toward the latest sample
  /// (latency compensation). 0 = pure lag, 1 = no lag (but jittery).
  final double extrapolation;

  /// Below this buffer-fill the first emitted points are marked low
  /// confidence.
  final double minConfidence;
}

/// The live stabilizer.
class StableStrokes {
  StableStrokes({StableStrokesConfig? config})
      : _config = config ?? const StableStrokesConfig();

  StableStrokesConfig _config;
  StableStrokesConfig get config => _config;

  void setConfig(StableStrokesConfig next) {
    _config = next;
    if (_buffer.length > _windowSize()) _buffer.removeRange(0, _buffer.length - _windowSize());
  }

  /// Toggle on/off (Squeeze Menu tap action).
  void toggle() => _config = StableStrokesConfig(
        enabled: !_config.enabled,
        intensity: _config.intensity,
        maxWindow: _config.maxWindow,
        extrapolation: _config.extrapolation,
        minConfidence: _config.minConfidence,
      );

  /// Set intensity from a 0–1 value (Squeeze Menu slide action).
  void setIntensity(double intensity) {
    final clamped = clampDouble(intensity, 0.0, 1.0);
    _config = StableStrokesConfig(
      enabled: _config.enabled || clamped > 0,
      intensity: clamped,
      maxWindow: _config.maxWindow,
      extrapolation: _config.extrapolation,
      minConfidence: _config.minConfidence,
    );
  }

  final List<StableSample> _buffer = [];

  int _windowSize() {
    if (!_config.enabled) return 1;
    final r = (_config.intensity * _config.maxWindow).round().clamp(1, _config.maxWindow);
    return r;
  }

  /// Push a new sample and return the stabilized output (or `null` if
  /// the buffer is still warming up).
  StableOutput? push(StableSample sample) {
    if (!_config.enabled) {
      return StableOutput(
        position: sample.position.clone(),
        pressure: sample.pressure,
        tilt: sample.tilt.clone(),
        time: sample.time,
        confidence: 1.0,
      );
    }
    _buffer.add(sample);
    final window = _windowSize();
    while (_buffer.length > window) {
      _buffer.removeAt(0);
    }
    if (_buffer.length < 2) return null;

    final out = _weightedCentroid(_buffer);

    // Latency compensation: extrapolate toward the newest sample.
    final extra = _config.extrapolation;
    if (extra > 0 && _buffer.length >= 2) {
      final newest = _buffer.last.position;
      final delta = newest - out.position;
      out.position.add(delta * extra);
    }

    final confidence =
        clampDouble(_buffer.length / window, 0.0, 1.0);
    return StableOutput(
      position: out.position,
      pressure: out.pressure,
      tilt: out.tilt,
      time: sample.time,
      confidence: confidence,
    );
  }

  /// End the stroke — flush any buffered samples so the tail of the
  /// ribbon isn't truncated.
  List<StableOutput> flush() {
    final out = <StableOutput>[];
    while (_buffer.length > 1) {
      _buffer.removeAt(0);
      if (_buffer.length >= 2) {
        final o = _weightedCentroid(_buffer);
        out.add(StableOutput(
          position: o.position,
          pressure: o.pressure,
          tilt: o.tilt,
          time: _buffer.last.time,
          confidence: 1.0,
        ));
      }
    }
    _buffer.clear();
    return out;
  }

  /// Reset the stabilizer for a new stroke.
  void reset() => _buffer.clear();

  // ----- Weighting ------------------------------------------------------

  _Centroid _weightedCentroid(List<StableSample> buf) {
    final n = buf.length;
    final weights = List<double>.filled(n, 0);
    var sumW = 0.0;
    final sigma = math.max(1.0, n / 3.0);
    final center = (n - 1) / 2.0;
    for (var i = 0; i < n; i++) {
      final d = (i - center) / sigma;
      final w = math.exp(-d * d);
      weights[i] = w;
      sumW += w;
    }
    final pos = Vector3.zero();
    var pressure = 0.0;
    var tx = 0.0, ty = 0.0;
    for (var i = 0; i < n; i++) {
      final s = buf[i];
      final w = weights[i] / sumW;
      pos.add(s.position * w);
      pressure += s.pressure * w;
      tx += s.tilt.x * w;
      ty += s.tilt.y * w;
    }
    return _Centroid(pos, pressure, Vector2(tx, ty));
  }
}

class _Centroid {
  _Centroid(this.position, this.pressure, this.tilt);
  final Vector3 position;
  final double pressure;
  final Vector2 tilt;
}
