// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// metallic_material.dart — polished-metal BRDF (v0.56-C).
//
// Metallic was a UI-only affordance from v0.55-B (the picker exposed a
// 5th chip but the host mapped it to [CanvasMaterial.shaded]). This
// file is the REAL engine material that backs the chip.
//
// The BRDF is intentionally cheap (no Fresnel, no microfacet sampling):
//
//   albedo   = mix(baseColor, envColor, 0.2 * clamp(normal.y, 0, 1))
//   diffuse  = albedo * lambert(N, L)             // ambient-floored
//   specular = specularColor * phong(N, V, shininess) * specularStrength
//   result   = diffuse + specular
//
// Three knobs differentiate it from [ShadedMaterial]:
//   1. [shininess] defaults to 64.0 — a tight Phong highlight (vs
//      ShadedMaterial's roughness-derived 4..128 range with default
//      roughness 0.5 → shininess 66). The high exponent keeps the
//      highlight as a crisp catch-light, the signature metal look.
//   2. [specularStrength] defaults to 0.8 — the highlight reads as a
//      bright reflection of the light source, not a soft sheen (vs
//      ShadedMaterial's default 1.0 multiplier on the rig's own
//      specularStrength 0.35 → effective ~0.35). Metallic multiplies
//      the rig's specular on top of that, so the highlight is markedly
//      brighter than shaded for the same scene.
//   3. [envColor] is a sky-color tint mixed into the albedo based on
//      the surface normal's Y component. Up-facing fragments pick up a
//      pale-blue sky reflection; down-facing fragments stay pure base
//      color. This is the cheap environment-probe stand-in: a real
//      probe would sample a cubemap, but Feather has no scene cubemap,
//      so we fake the sky with a single tint driven by normal.y.
//
// [shade] mirrors [ShadedMaterial.shade]'s structure: drive the rig's
// shininess + specularStrength from the material (saved + restored so
// the rig is not mutated across calls), evaluate, scale albedo by the
// diffuse term, add the specular highlight, then optionally modulate
// by a procedural pattern (the base class forces pattern null for
// Metallic — [materialTypeSupportsPattern] returns false — but the
// pattern modulation branch stays for parity with ShadedMaterial).
//
// Patterns are NOT supported on Metallic (polished metal has no
// engraving in the Feather doc model). The base [FeatherMaterial]
// constructor nulls [pattern] via [materialTypeSupportsPattern].

import 'dart:ui' show Color;

import 'package:feather_krita/engine/material/material.dart';
import 'package:feather_krita/engine/material/material_type.dart';
import 'package:feather_krita/engine/material/pattern_generator.dart';

/// A polished-metal material: Lambert diffuse + high-spec Phong +
/// a sky-color environment reflection tint driven by the surface
/// normal's Y component.
///
/// See the file header for the full BRDF derivation.
class MetallicMaterial extends FeatherMaterial {
  MetallicMaterial({
    super.baseColor = const Color.fromARGB(255, 192, 192, 200),
    super.opacity = 1.0,
    this.specularColor = const Color.fromARGB(255, 255, 255, 255),
    this.specularStrength = 0.8,
    this.shininess = 64.0,
    this.envColor = const Color.fromARGB(255, 180, 210, 240),
    this.envStrength = 0.2,
  })  : assert(specularStrength >= 0),
        assert(shininess >= 0),
        assert(envStrength >= 0 && envStrength <= 1),
        super(type: MaterialType.metallic);

  /// Specular highlight color (defaults to white — a clean metal
  /// reflects the light source without tinting it).
  Color specularColor;

  /// Multiplier on the rig's specular term. Defaults to 0.8 (a strong
  /// but not blown-out highlight). The effective specular brightness
  /// is `rig.specularStrength * specularStrength`, so a default rig
  /// (0.35) yields ~0.28 — already brighter than ShadedMaterial's
  /// default ~0.35 because of the tighter [shininess] highlight.
  double specularStrength;

  /// Phong shininess exponent (higher = tighter highlight). Defaults
  /// to 64.0 — a tight metal catch-light.
  double shininess;

  /// Environment tint color mixed into the albedo for up-facing
  /// fragments. Defaults to a pale sky blue (180, 210, 240).
  Color envColor;

  /// Maximum fraction of [envColor] mixed into the albedo. Defaults to
  /// 0.2 (20% sky tint at normal.y == 1.0). The actual mix weight is
  /// `envStrength * clamp(normal.y, 0, 1)` — down-facing fragments
  /// (normal.y < 0) get NO reflection tint, matching the intuition
  /// that the sky is above.
  double envStrength;

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
      // Environment reflection tint: blend the base albedo toward
      // [envColor] based on how much the normal points up. Down-facing
      // normals get no tint (sky is above, not below).
      final ny = normal.normalized().y.clamp(0.0, 1.0);
      final tint = (envStrength * ny).clamp(0.0, 1.0);
      final ar = r * (1.0 - tint) + envColor.r * tint;
      final ag = g * (1.0 - tint) + envColor.g * tint;
      final ab = b * (1.0 - tint) + envColor.b * tint;

      // Drive the rig's shininess + specularStrength from this material
      // (saved + restored so the rig is not mutated across calls —
      // matches [ShadedMaterial.shade]).
      final savedShininess = rig.shininess;
      final savedSpec = rig.specularStrength;
      rig.shininess = shininess;
      rig.specularStrength = savedSpec * specularStrength;
      final lit = rig.evaluate(normal, viewDir);
      rig.shininess = savedShininess;
      rig.specularStrength = savedSpec;

      // Diffuse scales the (env-tinted) albedo.
      r = ar * lit.diffuse;
      g = ag * lit.diffuse;
      b = ab * lit.diffuse;
      // Specular adds a highlight of [specularColor].
      r += specularColor.r * lit.specular;
      g += specularColor.g * lit.specular;
      b += specularColor.b * lit.specular;
    } else {
      // No rig bound: still apply the env reflection tint so a metallic
      // stroke reads as metal even in offline preview / unlit passes.
      final ny = normal?.normalized().y.clamp(0.0, 1.0) ?? 0.0;
      final tint = (envStrength * ny).clamp(0.0, 1.0);
      r = r * (1.0 - tint) + envColor.r * tint;
      g = g * (1.0 - tint) + envColor.g * tint;
      b = b * (1.0 - tint) + envColor.b * tint;
    }

    // Pattern modulation — parity with ShadedMaterial. The base class
    // forces pattern null for Metallic (materialTypeSupportsPattern is
    // false), so this branch is dead in practice; kept so a future
    // rule change does not silently drop the modulation.
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
    ..['shininess'] = shininess
    ..['envColor'] = envColor.toARGB32()
    ..['envStrength'] = envStrength;

  @override
  MetallicMaterial copy() => MetallicMaterial(
        baseColor: baseColor,
        opacity: opacity,
        specularColor: specularColor,
        specularStrength: specularStrength,
        shininess: shininess,
        envColor: envColor,
        envStrength: envStrength,
      );

  /// Convenience: a bright chrome look (white base, full specular).
  static MetallicMaterial chrome() => MetallicMaterial(
        baseColor: const Color.fromARGB(255, 220, 222, 228),
        specularStrength: 1.0,
        shininess: 96.0,
        envStrength: 0.28,
      );

  /// Convenience: a warm gold look (yellow base, soft highlight).
  static MetallicMaterial gold() => MetallicMaterial(
        baseColor: const Color.fromARGB(255, 212, 175, 55),
        specularColor: const Color.fromARGB(255, 255, 240, 200),
        specularStrength: 0.7,
        shininess: 56.0,
        envColor: const Color.fromARGB(255, 255, 220, 160),
        envStrength: 0.22,
      );
}
