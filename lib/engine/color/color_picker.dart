// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// color_picker.dart — HSV color-wheel model.
//
// Docs (brushes_color.txt + brushes_interface.txt):
//   - Color Wheel: "Spin the wheel to change the Hue (H), and use the
//     square to adjust saturation (S) and brightness (V)."
//   - Hex Code: "Tap the hex code to open the input pad for entering a
//     hex code."
//   - Active Color drag: "drag up, down, left, or right to adjust the
//     current color's saturation and brightness."
//
// This is the pure DATA model behind the wheel — no Flutter widget (the
// widget lives in lib/ui/widgets/color_wheel.dart). It owns the HSV
// state, the wheel-angle ↔ hue mapping, the SV-square ↔ (S, V) mapping,
// and the hex-code ↔ ARGB conversions. Real HSV↔RGB math (no Flutter
// HSVColor dependency) so the model is unit-testable in isolation.

import 'dart:math' as math;

import 'package:feather_krita/core/math/math_utils.dart';

/// HSV color model with hex-code and ARGB conversions.
class HsvColorModel {
  HsvColorModel({
    this.hue = 0.0,
    this.saturation = 0.0,
    this.value = 1.0,
    this.alpha = 1.0,
  })  : assert(hue >= 0 && hue < 360),
        assert(saturation >= 0 && saturation <= 1),
        assert(value >= 0 && value <= 1),
        assert(alpha >= 0 && alpha <= 1);

  /// Hue in degrees [0, 360).
  double hue;

  /// Saturation in [0, 1].
  double saturation;

  /// Brightness (value) in [0, 1].
  double value;

  /// Alpha in [0, 1].
  double alpha;

  HsvColorModel copyWith({
          double? hue, double? saturation, double? value, double? alpha}) =>
      HsvColorModel(
        hue: hue ?? this.hue,
        saturation: saturation ?? this.saturation,
        value: value ?? this.value,
        alpha: alpha ?? this.alpha,
      );

  // --- wheel / square mapping -----------------------------------------

  /// Sets the hue from a wheel angle in degrees [0, 360).
  void spinWheelTo(double degrees) {
    var d = degrees % 360.0;
    if (d < 0.0) d += 360.0;
    hue = d;
  }

  /// Spins the hue by [deltaDegrees] (negative = counter-clockwise).
  void spinWheel(double deltaDegrees) => spinWheelTo(hue + deltaDegrees);

  /// Returns the (saturation, value) pair as a square UV in [0, 1]².
  ({double s, double v}) get squareUv => (s: saturation, v: value);

  /// Sets saturation + value from a square UV tap/drag. [uv] components
  /// are clamped to [0, 1].
  void setSquareUv(({double s, double v}) uv) {
    saturation = clampDouble(uv.s, 0.0, 1.0);
    value = clampDouble(uv.v, 0.0, 1.0);
  }

  // --- conversions -----------------------------------------------------

  /// Converts to an ARGB int (0xAARRGGBB) via real HSV→RGB.
  int toArgb() {
    final c = value * saturation;
    final hp = hue / 60.0;
    final x = c * (1 - (hp % 2 - 1).abs());
    final m = value - c;
    double r, g, b;
    if (hp < 1) {
      r = c;
      g = x;
      b = 0;
    } else if (hp < 2) {
      r = x;
      g = c;
      b = 0;
    } else if (hp < 3) {
      r = 0;
      g = c;
      b = x;
    } else if (hp < 4) {
      r = 0;
      g = x;
      b = c;
    } else if (hp < 5) {
      r = x;
      g = 0;
      b = c;
    } else {
      r = c;
      g = 0;
      b = x;
    }
    final ri = ((r + m) * 255.0).round() & 0xff;
    final gi = ((g + m) * 255.0).round() & 0xff;
    final bi = ((b + m) * 255.0).round() & 0xff;
    final ai = (alpha * 255.0).round() & 0xff;
    return (ai << 24) | (ri << 16) | (gi << 8) | bi;
  }

  /// Builds the model from an ARGB int via real RGB→HSV.
  factory HsvColorModel.fromArgb(int argb) {
    final a = ((argb >> 24) & 0xff) / 255.0;
    final r = ((argb >> 16) & 0xff) / 255.0;
    final g = ((argb >> 8) & 0xff) / 255.0;
    final b = (argb & 0xff) / 255.0;
    final max = math.max(r, math.max(g, b));
    final min = math.min(r, math.min(g, b));
    final d = max - min;
    double h;
    if (d == 0) {
      h = 0;
    } else if (max == r) {
      h = 60.0 * (((g - b) / d) % 6.0);
    } else if (max == g) {
      h = 60.0 * (((b - r) / d) + 2.0);
    } else {
      h = 60.0 * (((r - g) / d) + 4.0);
    }
    if (h < 0) h += 360.0;
    final s = max == 0 ? 0.0 : d / max;
    return HsvColorModel(hue: h, saturation: s, value: max, alpha: a);
  }

  /// Returns "#RRGGBB" (alpha == 1) or "#AARRGGBB" (alpha < 1).
  String toHex() {
    final argb = toArgb();
    final a = (argb >> 24) & 0xff;
    final r = (argb >> 16) & 0xff;
    final g = (argb >> 8) & 0xff;
    final b = argb & 0xff;
    String h(int v) => v.toRadixString(16).padLeft(2, '0').toUpperCase();
    return alpha >= 1.0
        ? '#${h(r)}${h(g)}${h(b)}'
        : '#${h(a)}${h(r)}${h(g)}${h(b)}';
  }

  /// Parses "#RRGGBB", "#AARRGGBB", "RRGGBB", or "AARRGGBB".
  factory HsvColorModel.fromHex(String input) {
    var s = input.trim();
    if (s.startsWith('#')) s = s.substring(1);
    if (s.length == 6) s = 'FF$s';
    final parsed = int.tryParse(s, radix: 16);
    if (parsed == null) {
      return HsvColorModel(hue: 0, saturation: 0, value: 0);
    }
    return HsvColorModel.fromArgb(parsed);
  }

  @override
  String toString() =>
      'Hsv(h=${hue.toStringAsFixed(1)}, s=${saturation.toStringAsFixed(2)}, '
      'v=${value.toStringAsFixed(2)}, a=${alpha.toStringAsFixed(2)})';
}
