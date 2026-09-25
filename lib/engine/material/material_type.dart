// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// material_type.dart — the four Feather material types.
//
// Feather's docs (brushes_materials.txt) define exactly four material
// types that a 3D curve can carry:
//   - Shadeless : no lighting, no shadows; flat color + optional pattern.
//   - Shaded    : responds to lighting, casts shadows; pattern allowed.
//   - Glow      : emissive glow, adjustable intensity, no lighting, no
//                 shadows, NO pattern (additive blend).
//   - Cutout    : responds to the background — curve reads as the world's
//                 background color or image.
//
// This file is the single source of truth for that enum and the rules
// the docs attach to each type (pattern eligibility, lighting response,
// shadow casting). The material implementations and the brush preset
// model consult these helpers so the rules stay in one place.

/// The four Feather material types.
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
/// Shaded allow patterns; Glow and Cutout do not).
bool materialTypeSupportsPattern(MaterialType type) =>
    type == MaterialType.shadeless || type == MaterialType.shaded;

/// Whether [type] responds to scene lighting (docs: Shaded only —
/// Shadeless, Glow, and Cutout all ignore the light rig).
bool materialTypeRespondsToLight(MaterialType type) =>
    type == MaterialType.shaded;

/// Whether [type] casts shadows (docs: Shaded only).
bool materialTypeCastsShadows(MaterialType type) =>
    type == MaterialType.shaded;

/// Whether [type] is drawn additively (docs: Glow blends additively).
bool materialTypeIsAdditive(MaterialType type) => type == MaterialType.glow;

/// The docs' UI order: Shadeless, Shaded, Glow, Cutout (the order they
/// appear in the Material Type tab).
const List<MaterialType> kMaterialTypeOrder = <MaterialType>[
  MaterialType.shadeless,
  MaterialType.shaded,
  MaterialType.glow,
  MaterialType.cutout,
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
  }
}

/// Whether [type] exposes the docs' intensity slider (Glow only).
bool materialTypeHasIntensity(MaterialType type) =>
    type == MaterialType.glow;
