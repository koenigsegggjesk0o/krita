// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// shaded_material.dart — Lambert + Phong lit material.
//
// Docs: "Shaded — Responds to lighting and casts shadows. Patterns can
// be applied."
//
// [shade] runs the material light rig's Lambert diffuse + Phong specular
// (see light_rig.dart), multiplies the base color by the diffuse term,
// adds the specular highlight, then optionally modulates the result by
// the procedural pattern's coverage. This is the only material that
// consults [ShadeContext.normal], [ShadeContext.viewDir], and
// [ShadeContext.lightRig].

import 'dart:ui' show Color;

import 'package:feather_krita/engine/material/material.dart';
import 'package:feather_krita/engine/material/material_type.dart';
import 'package:feather_krita/engine/material/pattern_generator.dart';

/// A lit material: Lambert diffuse + Phong specular, ambient-floored.
class ShadedMaterial extends FeatherMaterial {
  ShadedMaterial({
    super.baseColor = const Color.fromARGB(255, 200, 200, 210),
    super.opacity = 1.0,
    super.pattern,
    this.specularColor = const Color.fromARGB(255, 255, 255, 255),
    this.specularStrength = 1.0,
    this.roughness = 0.5,
  })  : assert(specularStrength >= 0),
        assert(roughness >= 0 && roughness <= 1),
        super(type: MaterialType.shaded);

  /// Specular highlight color (defaults to white).
  Color specularColor;

  /// Multiplier on the rig's specular term (0 = matte, >1 = sharper
  /// highlights). Independent of the rig's own specularStrength so a
  /// preset can dial highlights per-material without touching the scene.
  double specularStrength;

  /// Surface roughness in [0, 1] (0 = mirror, 1 = chalky). Maps to a
  /// Phong shininess exponent when the material drives its own light
  /// evaluation: shininess = lerp(128, 4, roughness).
  double roughness;

  /// The Phong shininess exponent derived from [roughness].
  double get shininess {
    final r = roughness.clamp(0.0, 1.0);
    // 0 roughness → 128 (tight), 1 roughness → 4 (broad).
    return 128.0 - (128.0 - 4.0) * r;
  }

  @override
  Color shade(ShadeContext ctx) {
    final normal = ctx.normal;
    final viewDir = ctx.viewDir;
    final rig = ctx.lightRig;

    // Base RGB in 0..1.
    var r = baseColor.r;
    var g = baseColor.g;
    var b = baseColor.b;

    if (normal != null && viewDir != null && rig != null) {
      // Drive the rig's shininess from the material's roughness so the
      // highlight tightness is per-material, not per-scene.
      final savedShininess = rig.shininess;
      final savedSpec = rig.specularStrength;
      rig.shininess = shininess;
      rig.specularStrength = savedSpec * specularStrength;
      final lit = rig.evaluate(normal, viewDir);
      rig.shininess = savedShininess;
      rig.specularStrength = savedSpec;
      // Diffuse scales the base color.
      r *= lit.diffuse;
      g *= lit.diffuse;
      b *= lit.diffuse;
      // Specular adds a highlight of [specularColor].
      r += specularColor.r * lit.specular;
      g += specularColor.g * lit.specular;
      b += specularColor.b * lit.specular;
    }

    // Optional pattern: darken the lit base where coverage is high
    // (pattern reads as engraving on a shaded surface).
    final pat = pattern;
    if (pat != null) {
      final cov = PatternGenerator(pat).sample(ctx.patternU, ctx.patternV);
      r *= 1.0 - cov * 0.5;
      g *= 1.0 - cov * 0.5;
      b *= 1.0 - cov * 0.5;
    }

    return Color.fromARGB(
      (opacity.clamp(0.0, 1.0) * 255.0).round(),
      (r.clamp(0.0, 1.0) * 255.0).round(),
      (g.clamp(0.0, 1.0) * 255.0).round(),
      (b.clamp(0.0, 1.0) * 255.0).round(),
    );
  }

  @override
  Map<String, dynamic> toJson() => super.toJson()
    ..['specularColor'] = specularColor.toARGB32()
    ..['specularStrength'] = specularStrength
    ..['roughness'] = roughness;

  @override
  ShadedMaterial copy() => ShadedMaterial(
        baseColor: baseColor,
        opacity: opacity,
        pattern: pattern?.copyWith(),
        specularColor: specularColor,
        specularStrength: specularStrength,
        roughness: roughness,
      );

  /// Convenience: a fully matte shaded material (no specular).
  static ShadedMaterial matte(Color color) => ShadedMaterial(
        baseColor: color,
        specularStrength: 0.0,
        roughness: 1.0,
      );
}
