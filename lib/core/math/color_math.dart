// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// color_math.dart — Linear color-space math for the engine.
//
// Colors are [Vec3] RGB triples in the linear [0, 1] range. sRGB
// conversion follows the IEC 61966-2-1 transfer function. Tone
// mapping supports Reinhard and the Narkowicz ACES approximation.
// Alpha is handled out-of-band by the caller (e.g. via [Vec4]).

import 'dart:math' as math;

import 'math_utils.dart';
import 'vec3.dart';

/// Tone-mapping operator selector.
enum ToneMapMode {
  /// Reinhard global: `L' = L / (1 + L)` applied per-channel.
  reinhard,

  /// Narkowicz ACES filmic approximation.
  aces,
}

double _srgbToLinearChannel(double c) {
  if (c <= 0.04045) return c / 12.92;
  return math.pow((c + 0.055) / 1.055, 2.4).toDouble();
}

double _linearToSrgbChannel(double c) {
  if (c <= 0.0031308) return c * 12.92;
  return 1.055 * math.pow(c, 1.0 / 2.4).toDouble() - 0.055;
}

/// Converts an sRGB-encoded color to linear light. Input and output
/// are in [0, 1] (values outside the range are handled but may
/// overshoot).
Vec3 toLinear(Vec3 srgb) => Vec3(
      _srgbToLinearChannel(srgb.x),
      _srgbToLinearChannel(srgb.y),
      _srgbToLinearChannel(srgb.z),
    );

/// Converts a linear-light color to sRGB encoding.
Vec3 toSRGB(Vec3 linear) => Vec3(
      _linearToSrgbChannel(linear.x),
      _linearToSrgbChannel(linear.y),
      _linearToSrgbChannel(linear.z),
    );

/// Linear interpolation between two linear-space colors by [t].
Vec3 mix(Vec3 a, Vec3 b, double t) => a.lerp(b, t);

/// Component-wise (modulative) color multiply in linear space.
Vec3 multiply(Vec3 a, Vec3 b) => a.multiply(b);

/// Component-wise color add in linear space.
Vec3 add(Vec3 a, Vec3 b) => a + b;

/// Rec. 709 luminance of a linear-space color.
double luminance(Vec3 linear) =>
    0.2126 * linear.x + 0.7152 * linear.y + 0.0722 * linear.z;

/// Scales a linear color so its luminance maps through the Reinhard
/// curve `L / (1 + L)`. Black returns black.
Vec3 _toneMapReinhard(Vec3 linear) {
  final l = luminance(linear);
  if (l < epsilon) return const Vec3.zero();
  final scale = l / (1.0 + l);
  return linear * (scale / l);
}

/// Applies the Narkowicz ACES filmic approximation per channel and
/// clamps to [0, 1].
Vec3 _toneMapAces(Vec3 linear) {
  const a = 2.51, b = 0.03, c = 2.43, d = 0.59, e = 0.14;
  double f(double x) {
    final denom = x * (c * x + d) + e;
    if (denom.abs() < epsilon) return 0.0;
    return clampDouble((x * (a * x + b)) / denom, 0.0, 1.0);
  }

  return Vec3(f(linear.x), f(linear.y), f(linear.z));
}

/// Tone maps a linear HDR color to LDR ([0, 1]) using [mode]
/// (default: [ToneMapMode.aces]).
Vec3 toneMap(Vec3 linear, {ToneMapMode mode = ToneMapMode.aces}) {
  switch (mode) {
    case ToneMapMode.reinhard:
      return _toneMapReinhard(linear);
    case ToneMapMode.aces:
      return _toneMapAces(linear);
  }
}
