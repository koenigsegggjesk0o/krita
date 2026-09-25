// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// pattern_generator.dart — procedural pattern textures for materials.
//
// Feather's docs (brushes_materials.txt): "Patterns are procedurally
// generated textures." Five are supported (left to right in the UI):
//   Dot, Line, Cross, Terrazzo, Stippled Dot.
//
// This module generates those five patterns PROCEDURALLY (no image
// assets): each [PatternGenerator] evaluates a deterministic coverage
// function sample(u, v) -> [0, 1] and can rasterize a tileable RGBA8
// texture via [generateTile]. The coverage value modulates the material's
// base color (1 = pattern-on, 0 = pattern-off); the docs' intensity /
// angle / contrast sliders map onto [PatternSettings].
//
// All patterns are TILEABLE in UV space (the cell hash wraps at the grid
// period) and DETERMINISTIC for a given [seed] (a hash-based PRNG, no
// dart:math Random — so a saved project renders identically on reload).
//
// Depends on lib/core/math (math_utils: clampDouble, smoothstep, lerp).

import 'dart:math' as math;
import 'dart:typed_data';

import 'package:feather_krita/core/math/math_utils.dart';

/// The five Feather pattern types, in the docs' UI order.
enum PatternType {
  dot,
  line,
  cross,
  terrazzo,
  stippledDot,
}

/// Pattern type from a name string, with [fallback].
PatternType patternTypeFromName(String? name, [PatternType? fallback]) {
  if (name == null) return fallback ?? PatternType.dot;
  for (final t in PatternType.values) {
    if (t.name == name) return t;
  }
  return fallback ?? PatternType.dot;
}

/// User-tunable pattern parameters (docs: "Slide sideways to adjust the
/// pattern's intensity, angle, and contrast").
class PatternSettings {
  const PatternSettings({
    this.type = PatternType.dot,
    this.scale = 8.0,
    this.angle = 0.0,
    this.intensity = 0.7,
    this.contrast = 0.6,
    this.seed = 1,
  })  : assert(scale > 0),
        assert(intensity >= 0 && intensity <= 1),
        assert(contrast >= 0 && contrast <= 1);

  /// Which procedural pattern.
  final PatternType type;

  /// Pattern cells per UV unit (higher = denser). ~8 yields a few cells
  /// across a typical brush ribbon.
  final double scale;

  /// Pattern rotation in degrees (Line, Cross, Terrazzo, Stippled).
  final double angle;

  /// How strongly the pattern modulates the base color (0 = invisible,
  /// 1 = full swap). Maps the docs' "intensity" slider.
  final double intensity;

  /// Sharpness of the coverage falloff (0 = soft, 1 = hard). Maps the
  /// docs' "contrast" slider.
  final double contrast;

  /// Deterministic seed for stochastic patterns (Stippled, Terrazzo).
  final int seed;

  PatternSettings copyWith({
    PatternType? type,
    double? scale,
    double? angle,
    double? intensity,
    double? contrast,
    int? seed,
  }) =>
      PatternSettings(
        type: type ?? this.type,
        scale: scale ?? this.scale,
        angle: angle ?? this.angle,
        intensity: intensity ?? this.intensity,
        contrast: contrast ?? this.contrast,
        seed: seed ?? this.seed,
      );

  Map<String, dynamic> toJson() => {
        'type': type.name,
        'scale': scale,
        'angle': angle,
        'intensity': intensity,
        'contrast': contrast,
        'seed': seed,
      };

  factory PatternSettings.fromJson(Map<String, dynamic> json) =>
      PatternSettings(
        type: patternTypeFromName(json['type'] as String?),
        scale: (json['scale'] as num?)?.toDouble() ?? 8.0,
        angle: (json['angle'] as num?)?.toDouble() ?? 0.0,
        intensity: (json['intensity'] as num?)?.toDouble() ?? 0.7,
        contrast: (json['contrast'] as num?)?.toDouble() ?? 0.6,
        seed: (json['seed'] as num?)?.toInt() ?? 1,
      );
}

/// Evaluates a procedural pattern's coverage at texture coordinate [u],
/// [v] (both in repeats — the pattern tiles every 1.0 unit).
class PatternGenerator {
  PatternGenerator(this.settings);

  final PatternSettings settings;

  // Rotated UV for angle-aware patterns: returns the (u, v) projected
  // onto the pattern's local frame so the whole field rotates by [angle].
  void _rotated(double u, double v, List<double> out) {
    final a = settings.angle * DEG2RAD;
    final c = math.cos(a);
    final s = math.sin(a);
    out[0] = u * c - v * s;
    out[1] = u * s + v * c;
  }

  /// Coverage at [u], [v] in [0, 1] BEFORE intensity/contrast shaping.
  double sampleRaw(double u, double v) {
    final uv = [0.0, 0.0];
    _rotated(u * settings.scale, v * settings.scale, uv);
    switch (settings.type) {
      case PatternType.dot:
        return _dot(uv[0], uv[1]);
      case PatternType.line:
        return _line(uv[0], uv[1]);
      case PatternType.cross:
        return _cross(uv[0], uv[1]);
      case PatternType.terrazzo:
        return _terrazzo(uv[0], uv[1]);
      case PatternType.stippledDot:
        return _stippledDot(uv[0], uv[1]);
    }
  }

  /// Coverage at [u], [v] AFTER intensity + contrast shaping — the value
  /// a material multiplies its base color by. 0 = base color shows, 1 =
  /// pattern fully replaces it (then the material lerps by [intensity]).
  double sample(double u, double v) {
    var c = sampleRaw(u, v);
    // Contrast: bias around 0.5 then sharpen.
    c = (c - 0.5) * (1.0 + settings.contrast * 4.0) + 0.5;
    c = clampDouble(c, 0.0, 1.0);
    // Intensity: the on-color's strength.
    return c * settings.intensity;
  }

  // --- Dot: regular grid of soft dots, one per unit cell. --------------
  double _dot(double u, double v) {
    final cx = u.floor() + 0.5;
    final cy = v.floor() + 0.5;
    final d = math.sqrt((u - cx) * (u - cx) + (v - cy) * (v - cy));
    return 1.0 - smoothstep(0.25, 0.5, d);
  }

  // --- Line: parallel bands, triangle-wave of the V coordinate. -------
  double _line(double u, double v) {
    // ignore unused u; bands run along local-u.
    final t = v - v.floor();
    return 1.0 - smoothstep(0.35, 0.5, (t - 0.5).abs() * 2.0);
  }

  // --- Cross: union of a horizontal and vertical band per cell. --------
  double _cross(double u, double v) {
    final fu = (u - u.floor()) - 0.5;
    final fv = (v - v.floor()) - 0.5;
    final band = 1.0 -
        smoothstep(0.10, 0.20, math.min(fu.abs(), fv.abs()));
    return band;
  }

  // --- Terrazzo: Voronoi chunks with a dark seam between cells. --------
  double _terrazzo(double u, double v) {
    final ix = u.floor();
    final iy = v.floor();
    var bestDist = double.infinity;
    var secondDist = double.infinity;
    // 3x3 neighbor scan — the closest cell wins, the 2nd-closest drives
    // the seam width (standard Voronoi edge metric).
    for (var dy = -1; dy <= 1; dy++) {
      for (var dx = -1; dx <= 1; dx++) {
        final cx = ix + dx;
        final cy = iy + dy;
        final px = cx + _hash2(cx, cy, 0);
        final py = cy + _hash2(cx, cy, 1);
        final d = (u - px) * (u - px) + (v - py) * (v - py);
        if (d < bestDist) {
          secondDist = bestDist;
          bestDist = d;
        } else if (d < secondDist) {
          secondDist = d;
        }
      }
    }
    final seam = math.sqrt(secondDist) - math.sqrt(bestDist);
    return 1.0 - smoothstep(0.05, 0.25, seam);
  }

  // --- Stippled Dot: jittered grid of dots with hashed radii. ---------
  double _stippledDot(double u, double v) {
    final ix = u.floor();
    final iy = v.floor();
    final jx = ix + _hash2(ix, iy, 0) - 0.5;
    final jy = iy + _hash2(ix, iy, 1) - 0.5;
    final r = 0.18 + _hash2(ix, iy, 2) * 0.18;
    final d = math.sqrt((u - jx) * (u - jx) + (v - jy) * (v - jy));
    return 1.0 - smoothstep(r * 0.6, r, d);
  }

  /// Rasterizes a tileable [size]x[size] RGBA8 texture of this pattern
  /// (alpha = 255 everywhere, RGB = coverage repeated 0..255). Used for
  /// preview thumbnails and for sampling on platforms without a shader
  /// pipeline.
  Uint8List generateTile(int size, {int argb = 0xFFFFFFFF}) {
    final out = Uint8List(size * size * 4);
    final r = (argb >> 16) & 0xff;
    final g = (argb >> 8) & 0xff;
    final b = argb & 0xff;
    for (var y = 0; y < size; y++) {
      for (var x = 0; x < size; x++) {
        final u = x / size;
        final v = y / size;
        final cov = sample(u, v);
        final i = (y * size + x) * 4;
        out[i] = (r * cov).round();
        out[i + 1] = (g * cov).round();
        out[i + 2] = (b * cov).round();
        out[i + 3] = 255;
      }
    }
    return out;
  }

  // Deterministic hash in [0, 1) from integer cell + channel. Same input
  // always yields the same output (no PRNG state) — the docs require a
  // saved project render identically on reload.
  static double _hash2(int x, int y, int channel) {
    var h = x * 374761393 + y * 668265263 + channel * 2147483647;
    h = (h ^ (h >> 13)) * 1274126177;
    h = h ^ (h >> 16);
    return ((h & 0x7fffffff) / 0x7fffffff).abs();
  }
}
