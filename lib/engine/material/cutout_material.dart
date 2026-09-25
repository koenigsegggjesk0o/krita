// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// cutout_material.dart — background-responding material.
//
// Docs: "Cutout — Responds to the background, making curves appear as
// the background color or image." (See also the World doc.) Cutout
// ignores lighting and patterns; the curve "disappears" into the world's
// background, reading as a hole/punch-out that shows whatever is behind
// the scene.
//
// [shade] delegates to the [ShadeContext.backgroundSampler]: if a
// background is bound it returns the sampled background color; otherwise
// it falls back to a flat [fallbackColor] (the scene's solid background
// tint). The renderer composites Cutout opaquely (no additive blend).

import 'dart:ui' show Color;

import 'package:feather_krita/core/math/vec3.dart';
import 'package:feather_krita/engine/material/material.dart';
import 'package:feather_krita/engine/material/material_type.dart';

/// A material that reads as the scene background.
class CutoutMaterial extends FeatherMaterial {
  CutoutMaterial({
    this.fallbackColor = const Color.fromARGB(255, 30, 30, 38),
    super.opacity = 1.0,
  }) : super(
          type: MaterialType.cutout,
          baseColor: fallbackColor,
          pattern: null, // docs: Cutout cannot take a pattern.
        );

  /// Solid background color used when no [ShadeContext.backgroundSampler]
  /// is bound (e.g. during offline material preview). Defaults to the
  /// app's dark scene-clear color.
  Color fallbackColor;

  /// Factory: a Cutout that reads as a flat solid [argb] background
  /// (the docs' "background color" case). The fallback color IS the
  /// background; no sampler is needed.
  factory CutoutMaterial.solid(int argb) =>
      CutoutMaterial(fallbackColor: Color.fromARGB(
        (argb >> 24) & 0xff,
        (argb >> 16) & 0xff,
        (argb >> 8) & 0xff,
        argb & 0xff,
      ));

  /// Builds a background sampler that always returns [argb], ignoring
  /// world position — the "background color" case from the docs. Hand
  /// this to a [ShadeContext] to make Cutout read as a solid color.
  static int Function(Vec3) solidSampler(int argb) => (_) => argb;

  @override
  Color shade(ShadeContext ctx) {
    final sampler = ctx.backgroundSampler;
    final pos = ctx.worldPos;
    final a = (opacity.clamp(0.0, 1.0) * 255.0).round();
    if (sampler != null && pos != null) {
      final argb = sampler(pos);
      // Cutout is opaque — force full alpha unless the material's own
      // opacity slider dims it.
      return Color.fromARGB(
        a,
        (argb >> 16) & 0xff,
        (argb >> 8) & 0xff,
        argb & 0xff,
      );
    }
    // No background bound: read as the solid fallback tint.
    return Color.fromARGB(
      a,
      (fallbackColor.r * 255.0).round(),
      (fallbackColor.g * 255.0).round(),
      (fallbackColor.b * 255.0).round(),
    );
  }

  @override
  Map<String, dynamic> toJson() =>
      super.toJson()..['fallbackColor'] = fallbackColor.toARGB32();

  @override
  CutoutMaterial copy() =>
      CutoutMaterial(fallbackColor: fallbackColor, opacity: opacity);

  /// Convenience copy with a new fallback/background color.
  CutoutMaterial withFallbackColor(Color c) =>
      CutoutMaterial(fallbackColor: c, opacity: opacity);

  /// Whether this Cutout will resolve to a solid color (no sampler
  /// bound at shade time). The renderer can short-circuit to the
  /// fallback color in that case.
  bool get isSolid => true; // ShadeContext's sampler is checked at shade.

  /// Returns a human-readable description for the material panel.
  String get description =>
      'Reads as the scene background (color or image). No patterns.';
}
