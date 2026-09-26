// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// material_type.dart — the five Feather material types.
//
// Feather's docs (brushes_materials.txt) define four core material
// types that a 3D curve can carry:
//   - Shadeless : no lighting, no shadows; flat color + optional pattern.
//   - Shaded    : responds to lighting, casts shadows; pattern allowed.
//   - Glow      : emissive glow, adjustable intensity, no lighting, no
//                 shadows, NO pattern (additive blend).
//   - Cutout    : responds to the background — curve reads as the world's
//                 background color or image.
//
// A fifth type — Metallic — was added in v0.56-C as a real engine
// material (Lambert diffuse + high-spec Phong + environment reflection
// tint, see metallic_material.dart). It mirrors the lighting/shadow
// rules of Shaded (lit + casts shadows) but does NOT accept patterns
// (a polished metal surface has no engraving) and is drawn with a
// boosted specular + a sky-color reflection tint.
//
// This file is the single source of truth for that enum and the rules
// the docs attach to each type (pattern eligibility, lighting response,
// shadow casting). The material implementations and the brush preset
// model consult these helpers so the rules stay in one place.

/// The five Feather material types.
enum MaterialType {
  /// Flat color, ignores lighting and shadows. Patterns may be applied.
  shadeless,

  /// Lambert + Phong lit, casts shadows. Patterns may be applied.
  shaded,

  /// Emissive glow, additive blend, adjustable intensity. No lighting,
  /// no shadows, no patterns.
  glow,

  /// Reads as the scene background (color or image). No patterns.
  cutout,

  /// Polished metal: Lambert diffuse + high-spec Phong + environment
  /// reflection tint. Lit, casts shadows, NO patterns. See
  /// [MetallicMaterial] (v0.56-C).
  metallic,
}

/// Returns the [MaterialType] whose name matches [name], or [fallback].
MaterialType materialTypeFromName(String? name, [MaterialType? fallback]) {
  if (name == null) return fallback ?? MaterialType.shadeless;
  for (final t in MaterialType.values) {
    if (t.name == name) return t;
  }
  return fallback ?? MaterialType.shadeless;
}

/// Whether [type] accepts a procedural pattern (docs: only Shadeless and
/// Shaded allow patterns; Glow, Cutout, and Metallic do not).
bool materialTypeSupportsPattern(MaterialType type) =>
    type == MaterialType.shadeless || type == MaterialType.shaded;

/// Whether [type] responds to scene lighting (docs: Shaded only among the
/// four core types; Metallic mirrors Shaded — both run the Lambert +
/// Phong light rig). Shadeless, Glow, and Cutout all ignore the rig.
bool materialTypeRespondsToLight(MaterialType type) =>
    type == MaterialType.shaded || type == MaterialType.metallic;

/// Whether [type] casts shadows (docs: Shaded only among the four core
/// types; Metallic mirrors Shaded — both occlude the scene).
bool materialTypeCastsShadows(MaterialType type) =>
    type == MaterialType.shaded || type == MaterialType.metallic;

/// Whether [type] is drawn additively (docs: Glow blends additively).
bool materialTypeIsAdditive(MaterialType type) => type == MaterialType.glow;

/// The docs' UI order: Shadeless, Shaded, Glow, Cutout (the order they
/// appear in the Material Type tab). Metallic is appended at the end
/// (added in v0.56-C — not part of the original docs row).
const List<MaterialType> kMaterialTypeOrder = <MaterialType>[
  MaterialType.shadeless,
  MaterialType.shaded,
  MaterialType.glow,
  MaterialType.cutout,
  MaterialType.metallic,
];

/// Short human-readable label for the material panel.
String materialTypeDisplayName(MaterialType type) {
  switch (type) {
    case MaterialType.shadeless:
      return 'Shadeless';
    case MaterialType.shaded:
      return 'Shaded';
    case MaterialType.glow:
      return 'Glow';
    case MaterialType.cutout:
      return 'Cutout';
    case MaterialType.metallic:
      return 'Metallic';
  }
}

/// One-line description mirroring the docs.
String materialTypeDescription(MaterialType type) {
  switch (type) {
    case MaterialType.shadeless:
      return 'No lighting or shadows. Flat color, patterns allowed.';
    case MaterialType.shaded:
      return 'Responds to lighting, casts shadows. Patterns allowed.';
    case MaterialType.glow:
      return 'Emissive glow, adjustable intensity. No patterns.';
    case MaterialType.cutout:
      return 'Reads as the scene background color or image.';
    case MaterialType.metallic:
      return 'Polished metal: high specular + environment reflection.';
  }
}

/// Whether [type] exposes the docs' intensity slider (Glow only).
bool materialTypeHasIntensity(MaterialType type) =>
    type == MaterialType.glow;
