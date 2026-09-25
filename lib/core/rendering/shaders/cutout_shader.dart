// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// cutout_shader.dart — Background image / color shader for the 3D
// preview's "behind the scene" plate.
//
// Renders either a flat background color (when no texture is set) or a
// fullscreen background image (when a texture is provided). The shader
// is special-cased in the renderer: it bypasses the depth test and
// writes depth=1.0 (far) so every subsequent triangle wins the depth
// test against it. This keeps the background as a true "back plate"
// rather than a textured fullscreen quad that might occlude the scene.
//
// The shader is also responsible for the gradient sky option — when
// [Material.texture] is null but the material's [Material.baseColor]
// has alpha < 255, the shader lerps from [baseColor] (top) to
// [Material.emissiveColor] (bottom) across the framebuffer height. This
// is the cheap-and-cheerful "Feather 3D studio gradient" look.

import '../fragment_shader.dart';

/// Background plate shader.
class CutoutShader implements FragmentShader {
  const CutoutShader({this.gradient = true});

  /// If true (default), the shader draws a vertical gradient when no
  /// texture is bound. Top color = [Material.baseColor], bottom color =
  /// [Material.emissiveColor].
  final bool gradient;

  @override
  int shade(Fragment fragment, Material material, ShaderContext context) {
    final tex = material.texture;
    if (tex != null) {
      // Fullscreen texture — UV is already in [0,1] from the fullscreen
      // quad. Just sample and apply opacity.
      final t = context.sampler.sample(
        tex,
        fragment.uv.x,
        fragment.uv.y,
        filter: material.textureFilter,
      );
      var c = _multiply(t, material.baseColor);
      if (material.opacity < 1.0) {
        final a = ((c >> 24) & 0xff) * material.opacity;
        c = (a.round().clamp(0, 255) << 24) | (c & 0x00ffffff);
      }
      return c;
    }
    if (!gradient) {
      // Solid color.
      var c = material.baseColor;
      if (material.opacity < 1.0) {
        final a = ((c >> 24) & 0xff) * material.opacity;
        c = (a.round().clamp(0, 255) << 24) | (c & 0x00ffffff);
      }
      return c;
    }
    // Vertical gradient: top = baseColor, bottom = emissiveColor.
    final h = context.viewportHeight > 0 ? context.viewportHeight : 1.0;
    final yT = (fragment.y / h).clamp(0.0, 1.0);
    return _lerp(material.baseColor, material.emissiveColor, yT);
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

  static int _lerp(int a, int b, double t) {
    if (t <= 0.0) return a;
    if (t >= 1.0) return b;
    final ar = (a >> 16) & 0xff;
    final ag = (a >> 8) & 0xff;
    final ab = a & 0xff;
    final aa = (a >> 24) & 0xff;
    final br = (b >> 16) & 0xff;
    final bg = (b >> 8) & 0xff;
    final bb = b & 0xff;
    final ba = (b >> 24) & 0xff;
    final r = (ar + (br - ar) * t).round();
    final g = (ag + (bg - ag) * t).round();
    final bl = (ab + (bb - ab) * t).round();
    final al = (aa + (ba - aa) * t).round();
    return (al << 24) | (r << 16) | (g << 8) | bl;
  }
}
