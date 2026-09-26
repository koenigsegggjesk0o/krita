// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// material.dart — base material class for Feather 3D curves.
//
// Feather's docs (brushes_materials.txt): "All curves drawn with Feather
// are 3D curves that respond to light. Use materials to add depth and
// vibrancy to your work."
//
// Every curve carries one [FeatherMaterial]. The five concrete kinds —
// Shadeless, Shaded, Glow, Cutout, Metallic — live in sibling files and
// implement [shade]. This base class holds the shared state (type,
// baseColor, opacity, optional pattern) and the JSON dispatch that
// reconstructs the right subclass from a saved project.
//
// A [ShadeContext] bundles everything a material needs to evaluate one
// fragment: the surface normal + view direction (for Shaded's lighting),
// the optional light rig, the background sampler (for Cutout), and the
// pattern UV (for Shadeless/Shaded pattern modulation). All vectors are
// core immutable [Vec3] so the material layer is pure and testable.

import 'dart:ui' show Color;

import 'package:feather_krita/core/math/vec3.dart';
import 'package:feather_krita/engine/material/cutout_material.dart';
import 'package:feather_krita/engine/material/glow_material.dart';
import 'package:feather_krita/engine/material/light_rig.dart';
import 'package:feather_krita/engine/material/material_type.dart';
import 'package:feather_krita/engine/material/metallic_material.dart';
import 'package:feather_krita/engine/material/pattern_generator.dart';
import 'package:feather_krita/engine/material/shadeless_material.dart';
import 'package:feather_krita/engine/material/shaded_material.dart';

/// Everything a [FeatherMaterial] needs to evaluate one fragment.
class ShadeContext {
  const ShadeContext({
    this.normal,
    this.viewDir,
    this.worldPos,
    this.lightRig,
    this.backgroundSampler,
    this.patternU = 0.0,
    this.patternV = 0.0,
  });

  /// Surface normal at the fragment (unit, core Vec3). Null for
  /// materials that ignore lighting (Shadeless, Glow, Cutout).
  final Vec3? normal;

  /// Direction from the fragment toward the camera (unit, core Vec3).
  /// Null for non-lit materials.
  final Vec3? viewDir;

  /// Fragment world position (used by Cutout background sampling).
  final Vec3? worldPos;

  /// The active lighting rig (Shaded only).
  final MaterialLightRig? lightRig;

  /// Background sampler: returns the scene background ARGB int at a
  /// world position (Cutout material). Null when no background is bound.
  final int Function(Vec3 worldPos)? backgroundSampler;

  /// Pattern texture coordinate at the fragment (Shadeless/Shaded).
  final double patternU;
  final double patternV;
}

/// Base class for the four Feather materials.
abstract class FeatherMaterial {
  FeatherMaterial({
    required this.type,
    this.baseColor = const Color.fromARGB(255, 240, 240, 240),
    this.opacity = 1.0,
    PatternSettings? pattern,
  }) : pattern = materialTypeSupportsPattern(type) ? pattern : null;

  /// Which of the four Feather material types this is.
  final MaterialType type;

  /// The curve's base color (alpha channel is owned by [opacity]).
  Color baseColor;

  /// Curve opacity in [0, 1] (docs: the brush's opacity slider).
  double opacity;

  /// Optional procedural pattern. Forced null for Glow/Cutout/Metallic
  /// by the docs' rule (only Shadeless and Shaded accept patterns).
  PatternSettings? pattern;

  /// Whether this material is drawn with additive blending.
  bool get isAdditive => materialTypeIsAdditive(type);

  /// Whether this material reacts to scene lighting.
  bool get respondsToLight => materialTypeRespondsToLight(type);

  /// Evaluates the material at one fragment, returning the final ARGB
  /// color the renderer should paint. Subclasses implement the per-type
  /// math (flat / lit / emissive / background).
  Color shade(ShadeContext ctx);

  /// Returns a deep copy of this material.
  FeatherMaterial copy();

  /// Serializes the shared fields. Subclasses add their own fields and
  /// call [super.toJson] then merge.
  Map<String, dynamic> toJson() => {
        'type': type.name,
        'baseColor': baseColor.toARGB32(),
        'opacity': opacity,
        if (pattern != null) 'pattern': pattern!.toJson(),
      };

  /// Reconstructs the right subclass from JSON. Dispatches on `type`.
  /// The four subclass imports below close a benign type-cycle (Dart
  /// resolves mutual class references); no top-level state depends on it.
  static FeatherMaterial fromJson(Map<String, dynamic> json) {
    final type = materialTypeFromName(json['type'] as String?);
    final color = Color(
        (json['baseColor'] as num?)?.toInt() ?? 0xFFF0F0F0);
    final opacity = (json['opacity'] as num?)?.toDouble() ?? 1.0;
    final patternJson = json['pattern'] as Map<String, dynamic>?;
    final pattern =
        patternJson != null ? PatternSettings.fromJson(patternJson) : null;
    switch (type) {
      case MaterialType.shadeless:
        return ShadelessMaterial(
            baseColor: color, opacity: opacity, pattern: pattern);
      case MaterialType.shaded:
        return ShadedMaterial(
            baseColor: color, opacity: opacity, pattern: pattern);
      case MaterialType.glow:
        return GlowMaterial(baseColor: color, opacity: opacity);
      case MaterialType.cutout:
        return CutoutMaterial(opacity: opacity);
      case MaterialType.metallic:
        // Metallic does not accept a pattern (materialTypeSupportsPattern
        // is false); the base constructor already nulled [pattern]. We
        // also restore the metallic-specific knobs from JSON if present,
        // falling back to the defaults.
        return MetallicMaterial(
          baseColor: color,
          opacity: opacity,
          specularColor: _readColor(json['specularColor'],
              const Color.fromARGB(255, 255, 255, 255)),
          specularStrength:
              (json['specularStrength'] as num?)?.toDouble() ?? 0.8,
          shininess: (json['shininess'] as num?)?.toDouble() ?? 64.0,
          envColor: _readColor(
              json['envColor'], const Color.fromARGB(255, 180, 210, 240)),
          envStrength: (json['envStrength'] as num?)?.toDouble() ?? 0.2,
        );
    }
  }
}

/// Decodes a 0xAARRGGBB int from JSON into a [Color], returning [fallback]
/// when the value is missing or not an int.
Color _readColor(Object? v, Color fallback) {
  if (v is int) return Color(v);
  return fallback;
}

/// Packs an ARGB int from 0..1 linear channels.
int packArgb(double a, double r, double g, double b) {
  int c(double v) => (v.clamp(0.0, 1.0) * 255.0).round() & 0xff;
  return (c(a) << 24) | (c(r) << 16) | (c(g) << 8) | c(b);
}
