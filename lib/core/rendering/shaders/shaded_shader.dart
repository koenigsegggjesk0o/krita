// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// Export-only: software rasterizer for high-quality PNG export.
// Not used in real-time (too slow for 30 fps); the canvas viewport's
// CustomPainter handles live rendering. This subsystem is reserved
// for the export pipeline (rasterize scene -> framebuffer -> PNG)
// and for future headless render paths.
//
// shaded_shader.dart — Lambert diffuse + Phong specular + ambient shader.
//
// This is the default shader for 3D guide surfaces, stroke ribbons with
// normal data, and any object that should "live" in the Feather-Krita
// scene light rig. Lighting is accumulated by [LightRig.sample], then
// tonemapped via Reinhard in [LightContribution.resolve].
//
// Specular uses the Blinn-Phong half-vector model (cheaper than true
// Phong reflection and visually identical for the small specular
// exponents we use).

import '../fragment_shader.dart';

/// The standard lit shader (Lambert + Blinn-Phong).
class ShadedShader implements FragmentShader {
  const ShadedShader({this.specularExponent = 64.0, this.ambientBoost = 1.0});

  /// Blinn-Phong specular exponent. 64 = moderately glossy. Lower for
  /// matte, higher for shiny.
  final double specularExponent;

  /// Multiplier on the ambient term — lets the shader fake image-based
  /// lighting from a hemisphere term without paying for an envmap.
  final double ambientBoost;

  @override
  int shade(Fragment fragment, Material material, ShaderContext context) {
    final baseColor = material.baseColor;
    final vColor = fragment.vertexColor;
    var albedo = _multiply(baseColor, vColor);
    final tex = material.texture;
    if (tex != null) {
      final t = context.sampler.sample(
        tex,
        fragment.uv.x,
        fragment.uv.y,
        filter: material.textureFilter,
      );
      albedo = _multiply(albedo, t);
    }

    // If there's no light rig, fall back to a flat ambient.
    final rig = context.lights;
    if (rig == null) {
      // 50% albedo + 50% white ambient — keeps the shape readable.
      final ar = (albedo >> 16) & 0xff;
      final ag = (albedo >> 8) & 0xff;
      final ab = albedo & 0xff;
      final r = (ar * 0.5 + 255 * 0.25).clamp(0, 255).round();
      final g = (ag * 0.5 + 255 * 0.25).clamp(0, 255).round();
      final b = (ab * 0.5 + 255 * 0.25).clamp(0, 255).round();
      return _withAlpha(r, g, b, ((albedo >> 24) & 0xff) * material.opacity);
    }

    // Compute world-space normal — flip for back-facing fragments so
    // double-sided materials light correctly.
    var n = fragment.worldNormal;
    final viewDir = fragment.viewDir;
    if (viewDir.dot(n) < 0 && material.doubleSided) {
      n = -n;
    }

    final contribution = rig.sample(fragment.worldPosition, n, viewDir);
    if (ambientBoost != 1.0) {
      contribution.ambientR *= ambientBoost;
      contribution.ambientG *= ambientBoost;
      contribution.ambientB *= ambientBoost;
    }

    final lit = contribution.resolve(
      baseColor: albedo,
      specularStrength: material.specularStrength,
      exposure: context.exposure,
    );

    // Fold opacity into alpha.
    if (material.opacity < 1.0) {
      final a = ((lit >> 24) & 0xff) * material.opacity;
      return (a.round().clamp(0, 255) << 24) | (lit & 0x00ffffff);
    }
    return lit;
  }

  static int _withAlpha(int r, int g, int b, double a) {
    return (a.round().clamp(0, 255) << 24) |
        (r.clamp(0, 255) << 16) |
        (g.clamp(0, 255) << 8) |
        b.clamp(0, 255);
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
}
