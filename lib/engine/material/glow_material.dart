// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// glow_material.dart — emissive additive-blend material.
//
// Docs: "Glow — Responds to the glow area, adding a glowing effect to
// the curves. Does not respond to lighting, does not cast shadows, and
// patterns cannot be applied. You can adjust its intensity."
//
// [shade] returns the base color scaled by [intensity] (the docs'
// adjustable parameter), with [isAdditive] true so the renderer
// composites it additively. No lighting, no pattern, no shadows.

import 'dart:ui' show Color;

import 'package:feather_krita/core/math/math_utils.dart';
import 'package:feather_krita/engine/material/material.dart';
import 'package:feather_krita/engine/material/material_type.dart';

/// An emissive, additive-blend material.
class GlowMaterial extends FeatherMaterial {
  GlowMaterial({
    super.baseColor = const Color.fromARGB(255, 255, 230, 120),
    super.opacity = 1.0,
    this.intensity = 1.0,
    this.glowArea = 1.0,
    this.bloomRadius = 1.2,
  })  : assert(intensity >= 0),
        super(type: MaterialType.glow);

  /// Emission intensity (the docs' adjustable parameter). 0 = dark, 1 =
  /// full base color, >1 = HDR (the renderer tonemaps additively).
  double intensity;

  /// The docs' "glow area" — the fraction of the ribbon cross-section
  /// (0..1, center outward) that emits. 1 = full width glows; <1 = a
  /// hot core with a falloff to the edges.
  final double glowArea;

  /// Bloom radius multiplier (how far the glow bleeds past the ribbon
  /// in screen space). Consumed by the renderer's additive pass.
  final double bloomRadius;

  /// Soft falloff curve applied to [intensity] so the slider feels
  /// natural (0.5 maps to ~0.35 perceived). Pure function.
  double brightness() {
    final t = clampDouble(intensity, 0.0, 4.0);
    // Gamma 1.6 perceptual ramp — matches the scene pipeline's tonemap.
    return t > 1.0 ? 1.0 + (t - 1.0) * 0.5 : t;
  }

  /// Cross-ribbon glow weight at normalized offset [vAcross] in
  /// [-1, 1] (−1 = one edge, 0 = centerline, +1 = other edge). Used by
  /// the renderer to fade the ribbon edges when [glowArea] < 1.
  double crossSectionWeight(double vAcross) {
    final a = clampDouble(glowArea, 0.0, 1.0);
    final edge = a; // emit from center out to ±a
    final d = vAcross.abs();
    if (d >= edge) return 0.0;
    // Smooth falloff: 1 at center, 0 at the glow-area boundary.
    return 1.0 - (d / edge).clamp(0.0, 1.0);
  }

  /// Bloom falloff at normalized distance [t] from the ribbon edge
  /// (0 = on the edge, 1 = bloomRadius away). Smoothstep to zero.
  double bloomFalloff(double t) {
    final tt = clampDouble(t, 0.0, 1.0);
    final v = 1.0 - tt;
    return v * v * (3.0 - 2.0 * v);
  }

  @override
  Color shade(ShadeContext ctx) {
    final k = brightness();
    final r = (baseColor.r * k).clamp(0.0, 1.0);
    final g = (baseColor.g * k).clamp(0.0, 1.0);
    final b = (baseColor.b * k).clamp(0.0, 1.0);
    final a = opacity.clamp(0.0, 1.0);
    return Color.fromARGB(
      (a * 255.0).round(),
      (r * 255.0).round(),
      (g * 255.0).round(),
      (b * 255.0).round(),
    );
  }

  @override
  Map<String, dynamic> toJson() => super.toJson()
    ..['intensity'] = intensity
    ..['glowArea'] = glowArea
    ..['bloomRadius'] = bloomRadius;

  @override
  GlowMaterial copy() => GlowMaterial(
        baseColor: baseColor,
        opacity: opacity,
        intensity: intensity,
        glowArea: glowArea,
        bloomRadius: bloomRadius,
      );

  /// Convenience copy with a new intensity (the docs' slider).
  GlowMaterial withIntensity(double v) => copy()..intensity = v;
}
