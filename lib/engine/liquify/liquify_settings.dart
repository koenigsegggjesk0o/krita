// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// liquify_settings.dart — Size, range, and strength settings for the
// Liquify tool.
//
// Per docs/liquify_editandapply.txt:
//
//   "Press and drag with your pen to liquify your curves or drawings.
//    The degree of distortion and the affected area depend on the size,
//    range, and strength settings in the Liquify panel."
//
// The three sliders map to:
//   - **Size**     — the brush radius in world units. The brush affects
//     points within `size` of the pen tip.
//   - **Range**    — the falloff radius beyond `size`. Points within
//     `size + range` get a falloff-weighted influence; outside that
//     they're untouched. A larger range = softer, broader distortion.
//   - **Strength** — the maximum magnitude of the per-point
//     displacement as a fraction of the drag delta. 1.0 = full drag
//     applied to the centre of the brush; 0.1 = gentle.
//
// The settings are immutable value objects; the liquify panel mutates
// the active [LiquifySettings] via [LiquifySettingsModel] (a
// [ChangeNotifier]).

import 'dart:math' as math;

import 'package:flutter/foundation.dart';

/// Immutable liquify brush settings.
@immutable
class LiquifySettings {
  const LiquifySettings({
    this.size = 0.25,
    this.range = 0.35,
    this.strength = 0.6,
  });

  /// Brush radius in world units. Points within [size] of the pen tip
  /// get the full influence (modulated by [strength]).
  final double size;

  /// Falloff radius in world units beyond [size]. Points within
  /// `size + range` get a smoothstep falloff to 0; outside that they're
  /// untouched.
  final double range;

  /// Maximum per-point displacement as a fraction of the drag delta
  /// (0.0 = no effect, 1.0 = full drag applied at the brush centre).
  final double strength;

  /// Total reach of the brush in world units.
  double get reach => size + range;

  /// Copies with overrides.
  LiquifySettings copyWith({double? size, double? range, double? strength}) =>
      LiquifySettings(
        size: size ?? this.size,
        range: range ?? this.range,
        strength: strength ?? this.strength,
      );

  @override
  bool operator ==(Object other) {
    if (other is! LiquifySettings) return false;
    return other.size == size &&
        other.range == range &&
        other.strength == strength;
  }

  @override
  int get hashCode => Object.hash(size, range, strength);

  @override
  String toString() =>
      'LiquifySettings(size=$size, range=$range, strength=$strength)';
}

/// Mutable, observable liquify settings.
class LiquifySettingsModel extends ChangeNotifier {
  LiquifySettingsModel([LiquifySettings? initial])
      : _settings = initial ?? const LiquifySettings();

  LiquifySettings _settings;
  LiquifySettings get settings => _settings;
  double get size => _settings.size;
  double get range => _settings.range;
  double get strength => _settings.strength;
  double get reach => _settings.reach;

  void update(LiquifySettings next) {
    if (next == _settings) return;
    _settings = next;
    notifyListeners();
  }

  void setSize(double v) {
    final clamped = v.clamp(0.01, 5.0);
    if (clamped == _settings.size) return;
    _settings = _settings.copyWith(size: clamped);
    notifyListeners();
  }

  void setRange(double v) {
    final clamped = v.clamp(0.0, 5.0);
    if (clamped == _settings.range) return;
    _settings = _settings.copyWith(range: clamped);
    notifyListeners();
  }

  void setStrength(double v) {
    final clamped = v.clamp(0.0, 1.0);
    if (clamped == _settings.strength) return;
    _settings = _settings.copyWith(strength: clamped);
    notifyListeners();
  }

  /// Convenience preset: small detail brush.
  void presetDetail() => update(const LiquifySettings(
      size: 0.10, range: 0.10, strength: 0.4));

  /// Convenience preset: broad reshape brush.
  void presetBroad() => update(const LiquifySettings(
      size: 0.50, range: 0.60, strength: 0.8));

  /// Convenience preset: gentle comb brush.
  void presetComb() => update(const LiquifySettings(
      size: 0.35, range: 0.50, strength: 0.25));
}

/// Returns the falloff weight (0..1) for a point at distance [d] from the
/// brush centre, given settings [s]. Inside [s.size] the weight is 1;
/// between [s.size] and [s.reach] it smoothstep-falloffs to 0; beyond
/// [s.reach] it's 0. Smoothstep (3t² − 2t³) gives C¹ continuity at both
/// ends so the brush doesn't have a visible seam.
double liquifyFalloff(LiquifySettings s, double d) {
  if (d <= 0) return 1.0;
  if (d <= s.size) return 1.0;
  if (d >= s.reach) return 0.0;
  final t = (d - s.size) / s.range;
  // Smoothstep: 1 → 0 across [0, 1] using (1 − t)² (3 − 2(1 − t)).
  final k = 1.0 - t;
  return k * k * (3.0 - 2.0 * k);
}

/// Convenience constant — pi, re-exported so callers don't need to
/// import dart:math just for the brush radius conversion.
const double kLiquifyPi = math.pi;
