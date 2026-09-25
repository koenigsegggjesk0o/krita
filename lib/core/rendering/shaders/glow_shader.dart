// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// glow_shader.dart — Emissive additive-blend shader for stroke highlights
// and guide-surface accent lines.
//
// Returns a flat emissive color (optionally modulated by a texture mask
// and a per-vertex color tint) that the rasterizer composites using
// [BlendMode3D.additive]. Used for:
//   - Active-stroke highlights (the "this is the stroke you're drawing
//     right now" glow).
//   - Snap indicators on the guide surface.
//   - Decorative emissive accents on the 3D preview.
//
// The shader can also produce a soft radial falloff based on the
// fragment's distance from the triangle's centroid — useful for the
// "soft glow" brush preset.

import 'dart:math' as math;

import '../fragment_shader.dart';

/// Emissive additive-blend shader.
class GlowShader implements FragmentShader {
  const GlowShader({
    this.falloff = 0.0,
    this.pulseHz = 0.0,
    this.useTextureAsMask = true,
  });

  /// If > 0, multiplies the emissive color by a radial falloff based on
  /// the fragment's barycentric distance from the triangle's centroid.
  /// 0 = uniform glow (default). 1 = strong falloff.
  final double falloff;

  /// If > 0, pulses the emissive intensity with a sine at this frequency.
  /// Used for the "active" stroke highlight.
  final double pulseHz;

  /// If true (default), the texture is sampled as a multiplicative mask
  /// on the emissive color. If false, the texture is ignored.
  final bool useTextureAsMask;

  @override
  int shade(Fragment fragment, Material material, ShaderContext context) {
    var emissive = material.emissiveColor;
    // Modulate by the material's base color (lets the artist tint the
    // glow without changing the emissive color directly).
    emissive = _multiply(emissive, material.baseColor);
    // Modulate by per-vertex color (so stroke ribbons can fade out at
    // the ends).
    emissive = _multiply(emissive, fragment.vertexColor);

    // Texture mask.
    final tex = material.texture;
    if (tex != null && useTextureAsMask) {
      final t = context.sampler.sample(
        tex,
        fragment.uv.x,
        fragment.uv.y,
        filter: material.textureFilter,
      );
      // Use only the texture's luminance + alpha as a mask.
      final tr = (t >> 16) & 0xff;
      final tg = (t >> 8) & 0xff;
      final tb = t & 0xff;
      final ta = (t >> 24) & 0xff;
      final lum = (0.299 * tr + 0.587 * tg + 0.114 * tb).round();
      final mask = (lum * ta) ~/ 255;
      emissive = _scale(emissive, mask);
    }

    // Intensity.
    var intensity = material.emissiveIntensity;
    if (pulseHz > 0.0) {
      // Pulse: 0.7..1.0 sine.
      final p = 0.85 + 0.15 * math.sin(2 * math.pi * pulseHz * context.timeSeconds);
      intensity *= p;
    }
    if (intensity != 1.0) {
      emissive = _scale(emissive, (intensity * 255).round().clamp(0, 255));
    }

    // Falloff (cheap — uses the barycentric-distance-to-centroid
    // approximation: distance from the UV center).
    if (falloff > 0.0) {
      final du = fragment.uv.x - 0.5;
      final dv = fragment.uv.y - 0.5;
      final d = math.sqrt(du * du + dv * dv) * 2.0; // 0 at center, ~1 at edge
      final k = (1.0 - d.clamp(0.0, 1.0) * falloff).clamp(0.0, 1.0);
      emissive = _scale(emissive, (k * 255).round().clamp(0, 255));
    }

    // Opacity.
    if (material.opacity < 1.0) {
      final a = ((emissive >> 24) & 0xff) * material.opacity;
      emissive = (a.round().clamp(0, 255) << 24) | (emissive & 0x00ffffff);
    }
    return emissive;
  }

  static int _multiply(int a, int b) {
    final ar = (a >> 16) & 0xff;
    final ag = (a >> 8) & 0xff;
    final ab = a & 0xff;
    final aa = (a >> 24) & 0xff;
    final br = (b >> 16) & 0xff;
    final bg = (b >> 8) & 0xff;
    final bb = b & 0xff;
    final ba = (b >> 24) & 0xff;
    return ((aa * ba) ~/ 255) << 24 |
        ((ar * br) ~/ 255) << 16 |
        ((ag * bg) ~/ 255) << 8 |
        (ab * bb) ~/ 255;
  }

  static int _scale(int c, int k255) {
    if (k255 == 255) return c;
    final r = ((c >> 16) & 0xff) * k255 ~/ 255;
    final g = ((c >> 8) & 0xff) * k255 ~/ 255;
    final b = (c & 0xff) * k255 ~/ 255;
    final a = ((c >> 24) & 0xff) * k255 ~/ 255;
    return (a << 24) | (r << 16) | (g << 8) | b;
  }
}
