// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// shadeless_material.dart — flat-color material with optional pattern.
//
// Docs: "Shadeless — A basic material that does not respond to lighting
// or cast shadows. Patterns can be applied."
//
// [shade] returns the base color at full intensity, optionally modulated
// by the procedural pattern's coverage (the pattern "replaces" the base
// color where coverage is high, blended by the pattern's intensity). No
// lighting math runs — Shadeless ignores the light rig and the surface
// normal entirely.

import 'dart:ui' show Color;

import 'package:feather_krita/engine/material/material.dart';
import 'package:feather_krita/engine/material/material_type.dart';
import 'package:feather_krita/engine/material/pattern_generator.dart';

/// A flat, unlit material. The base color is output directly; an optional
/// [pattern] modulates it via coverage blending.
class ShadelessMaterial extends FeatherMaterial {
  ShadelessMaterial({
    super.baseColor = const Color.fromARGB(255, 240, 240, 240),
    super.opacity = 1.0,
    super.pattern,
    this.patternColor = const Color.fromARGB(255, 30, 30, 38),
    this.tintColor = const Color.fromARGB(255, 0, 0, 0),
    this.tintStrength = 0.0,
  }) : super(type: MaterialType.shadeless);

  /// The "pattern-on" color — the value the pattern paints where its
  /// coverage is 1. Defaults to a darkened base color so the pattern
  /// reads as etching/engraving on the curve.
  Color patternColor;

  /// Optional flat tint mixed into the base color (e.g. a warm tint).
  /// [tintStrength] in [0, 1] controls how far the base moves toward
  /// [tintColor]; 0 = no tint (pure base color).
  Color tintColor;

  /// Tint blend strength in [0, 1].
  double tintStrength;

  @override
  Color shade(ShadeContext ctx) {
    final tint = tintStrength.clamp(0.0, 1.0);
    var r = baseColor.r + (tintColor.r - baseColor.r) * tint;
    var g = baseColor.g + (tintColor.g - baseColor.g) * tint;
    var b = baseColor.b + (tintColor.b - baseColor.b) * tint;
    final pat = pattern;
    if (pat != null) {
      final cov = PatternGenerator(pat).sample(ctx.patternU, ctx.patternV);
      if (cov > 0.0) {
        // Blend toward patternColor by coverage.
        r = r + (patternColor.r - r) * cov;
        g = g + (patternColor.g - g) * cov;
        b = b + (patternColor.b - b) * cov;
      }
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
    ..['patternColor'] = patternColor.toARGB32()
    ..['tintColor'] = tintColor.toARGB32()
    ..['tintStrength'] = tintStrength;

  @override
  ShadelessMaterial copy() => ShadelessMaterial(
        baseColor: baseColor,
        opacity: opacity,
        pattern: pattern?.copyWith(),
        patternColor: patternColor,
        tintColor: tintColor,
        tintStrength: tintStrength,
      );

  /// Convenience: a shadeless material with just a flat [color] and no
  /// pattern (the docs' "basic material" default).
  static ShadelessMaterial flat(Color color) =>
      ShadelessMaterial(baseColor: color);

  /// Convenience copy with a bound pattern (replaces any existing one).
  ShadelessMaterial withPattern(PatternSettings p) => copy()
    ..pattern = p
    ..syncPattern();

  /// Re-applies the docs' pattern-eligibility rule (Shadeless always
  /// allows patterns, so this is a no-op kept for symmetry with the
  /// base class and for future rule changes).
  void syncPattern() {
    // Shadeless always supports patterns — nothing to clamp.
  }
}
