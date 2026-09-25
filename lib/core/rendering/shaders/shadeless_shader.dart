// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// Export-only: software rasterizer for high-quality PNG export.
// Not used in real-time (too slow for 30 fps); the canvas viewport's
// CustomPainter handles live rendering. This subsystem is reserved
// for the export pipeline (rasterize scene -> framebuffer -> PNG)
// and for future headless render paths.
//
// shadeless_shader.dart — Flat-color / unlit fragment shader.
//
// Returns the material's base color * vertex color * texture sample
// (if any). No lighting, no specular. This is the fastest shader and
// the right choice for UI overlays, picking IDs, debug visualizations,
// and any material that already has its lighting baked into the texture
// (e.g. lightmapped guide surfaces).
//
// Variant flags:
//   - [tintMode] controls how the per-vertex color is combined with the
//     material's base color (multiply / replace / add).
//   - [premultiplyAlpha] folds the texture's alpha into its RGB before
//     returning — needed when the framebuffer is in premultiplied
//     alpha format.

import '../fragment_shader.dart';

/// How the per-vertex color is combined with the material's base color
/// in [ShadelessShader].
enum ShadelessTintMode {
  /// `material * vertex * texture` — the default. Both colors modulate
  /// the texture.
  multiply,

  /// `vertex * texture` — the material's base color is ignored. Used
  /// by stroke ribbons that bake their color into per-vertex data.
  replace,

  /// `material + vertex * 0.5 * texture` — additive tint. Used by the
  /// highlight overlay on the active-stroke indicator.
  additive,
}

/// The shadeless (unlit) shader.
class ShadelessShader implements FragmentShader {
  const ShadelessShader({
    this.tintMode = ShadelessTintMode.multiply,
    this.premultiplyAlpha = false,
  });

  /// Controls how [Material.baseColor] and [Fragment.vertexColor] are
  /// combined. See [ShadelessTintMode].
  final ShadelessTintMode tintMode;

  /// If true, the texture's alpha is folded into its RGB before output
  /// (premultiplied alpha). Default false (straight alpha).
  final bool premultiplyAlpha;

  @override
  int shade(Fragment fragment, Material material, ShaderContext context) {
    var color = material.baseColor;
    switch (tintMode) {
      case ShadelessTintMode.multiply:
        color = _multiply(color, fragment.vertexColor);
        break;
      case ShadelessTintMode.replace:
        color = fragment.vertexColor;
        break;
      case ShadelessTintMode.additive:
        color = _addScaled(color, fragment.vertexColor, 0.5);
        break;
    }
    // Sample the texture if present.
    final tex = material.texture;
    if (tex != null) {
      final t = context.sampler.sample(
        tex,
        fragment.uv.x,
        fragment.uv.y,
        filter: material.textureFilter,
      );
      color = _multiply(color, t);
      if (premultiplyAlpha) {
        color = _premultiply(color);
      }
    }
    // Apply opacity. The blend mode handles final compositing; we just
    // fold the material-wide opacity into the alpha channel.
    if (material.opacity < 1.0) {
      final a = ((color >> 24) & 0xff) * material.opacity;
      color = (a.round().clamp(0, 255) << 24) | (color & 0x00ffffff);
    }
    return color;
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

  static int _addScaled(int a, int b, double s) {
    final ar = (a >> 16) & 0xff;
    final ag = (a >> 8) & 0xff;
    final ab = a & 0xff;
    final aa = (a >> 24) & 0xff;
    final br = (b >> 16) & 0xff;
    final bg = (b >> 8) & 0xff;
    final bb = b & 0xff;
    final ba = (b >> 24) & 0xff;
    return ((aa + ba * s).round().clamp(0, 255) << 24) |
        ((ar + br * s).round().clamp(0, 255) << 16) |
        ((ag + bg * s).round().clamp(0, 255) << 8) |
        (ab + bb * s).round().clamp(0, 255);
  }

  static int _premultiply(int c) {
    final a = (c >> 24) & 0xff;
    final r = (c >> 16) & 0xff;
    final g = (c >> 8) & 0xff;
    final b = c & 0xff;
    final k = a / 255.0;
    return (a << 24) |
        ((r * k).round() << 16) |
        ((g * k).round() << 8) |
        (b * k).round();
  }
}
