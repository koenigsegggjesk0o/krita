// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// guide3d_type.dart — Creation-mode taxonomy for Feather-style 3D Guides.
//
// Feather 3D exposes four ways to bring a 3D Guide into the scene:
//
//   - Drawn     : the artist sketches with the pen; a translucent surface is
//                 generated perpendicular to the current viewing angle, its
//                 on-screen width scaled by the active Field of View.
//   - Lofted    : two or more already-drawn curves are connected in sequence
//                 to skin a surface between them (tension slider controls the
//                 Catmull-Rom interpolation sharpness).
//   - Primitive : a ready-made Cube, Pyramid, Sphere or Tube is dropped at the
//                 world origin (segment slider controls tessellation).
//   - Bent      : an existing guide is deformed along a freshly drawn bend
//                 path. The deformation anchors on the guide's orange
//                 starting line and can be repeated.
//
// Each enum value maps to a [Guide3D] subclass-ish builder in this package.
// The names are persisted verbatim in the .feather document so the order
// and spelling here are part of the file-format contract.

/// The four creation modes a [Guide3D] can originate from.
///
/// See the package-level docs in `guide3d.dart` for the per-mode behavior.
enum Guide3DType {
  /// Pen-drawn translucent surface, perpendicular to the viewing angle.
  drawn,

  /// Surface skinned across two or more curves selected in sequence.
  lofted,

  /// Ready-made primitive (cube / pyramid / sphere / tube) at the origin.
  primitive,

  /// Existing guide deformed along a drawn bend path.
  bent;

  /// Human-readable name matching the Feather docs ("Draw", "Loft", …).
  String get displayName {
    switch (this) {
      case Guide3DType.drawn:
        return 'Draw';
      case Guide3DType.lofted:
        return 'Loft';
      case Guide3DType.primitive:
        return 'Primitives';
      case Guide3DType.bent:
        return 'Bend';
    }
  }

  /// Stable lowercase identifier used in serialization.
  String get wireName => name;

  /// Short doc string for inspector tooltips.
  String get helpText {
    switch (this) {
      case Guide3DType.drawn:
        return 'Pen-drawn surface perpendicular to the viewing angle. '
            'Width scales with the camera Field of View.';
      case Guide3DType.lofted:
        return 'Skin a surface across two or more curves selected in '
            'sequence. Tension slider controls interpolation sharpness.';
      case Guide3DType.primitive:
        return 'Ready-made Cube, Pyramid, Sphere or Tube at the world '
            'origin. Segment slider controls tessellation.';
      case Guide3DType.bent:
        return 'Bend an existing guide along a drawn path. Deformation '
            'starts from the orange starting line and can be repeated.';
    }
  }
}

/// Parses a [Guide3DType] from a stored wire name or display name.
///
/// Accepts the enum [Guide3DType.name] ("drawn", "lofted", …), the
/// [Guide3DType.displayName] ("Draw", "Loft", …) and a few common
/// aliases ("bend" / "primitive"). The lookup is case-insensitive and
/// tolerates surrounding whitespace. Returns `null` for unknown spellings
/// so callers can decide whether to fall back to a default or surface an
/// error — unlike [guide3DTypeFromName], which silently falls back to
/// [Guide3DType.drawn].
Guide3DType? guide3DTypeFromNameStrict(String? raw) {
  switch (raw?.trim().toLowerCase()) {
    case 'drawn':
    case 'draw':
      return Guide3DType.drawn;
    case 'lofted':
    case 'loft':
      return Guide3DType.lofted;
    case 'primitive':
    case 'primitives':
      return Guide3DType.primitive;
    case 'bent':
    case 'bend':
      return Guide3DType.bent;
    default:
      return null;
  }
}

/// Tolerant variant of [guide3DTypeFromNameStrict] that falls back to
/// [Guide3DType.drawn] for unknown spellings. Used by the load path where
/// a corrupt document should still open instead of throwing.
Guide3DType guide3DTypeFromName(String? raw) =>
    guide3DTypeFromNameStrict(raw) ?? Guide3DType.drawn;
