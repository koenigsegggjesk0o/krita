// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// guide3d_primitive.dart — Primitive shape taxonomy for 3D Guides.
//
// Feather's Primitives panel offers four ready-made shapes that double as
// 3D Guides: Cube, Pyramid, Sphere and Tube. The Segment slider on the
// left of the screen reshapes them — slide up for more segments, down for
// fewer. The same Tube primitive with few segments reads as a faceted
// prism; with one segment it degenerates toward a plane ring; with many
// it becomes a smooth cylinder. Likewise the Sphere with a low segment
// count is a low-poly ball, and the Pyramid collapses to a triangle when
// segments drop to 3.
//
// These four values are persisted verbatim in the .feather document, so
// the order and the spelling here are part of the file-format contract.

/// The four primitive shapes a [Guide3D] of type
/// [Guide3DType.primitive] can take.
enum Guide3DPrimitive {
  /// Axis-aligned box centred on the origin.
  cube,

  /// Square-base pyramid, apex on +Y.
  pyramid,

  /// UV sphere centred on the origin.
  sphere,

  /// Open tube (cylinder shell) along the Y axis.
  tube;

  /// Human-readable name matching the Feather docs.
  String get displayName {
    switch (this) {
      case Guide3DPrimitive.cube:
        return 'Cube';
      case Guide3DPrimitive.pyramid:
        return 'Pyramid';
      case Guide3DPrimitive.sphere:
        return 'Sphere';
      case Guide3DPrimitive.tube:
        return 'Tube';
    }
  }

  /// Stable lowercase identifier used in serialization.
  String get wireName => name;

  /// Default segment count for the Segment slider's midpoint.
  ///
  /// Feather ships with the slider centred, so each primitive declares a
  /// sensible mid-poly default the editor can restore to.
  int get defaultSegments {
    switch (this) {
      case Guide3DPrimitive.cube:
        return 1; // cubes tessellate per-face; 1 = clean box.
      case Guide3DPrimitive.pyramid:
        return 4; // 4-sided base by default.
      case Guide3DPrimitive.sphere:
        return 24;
      case Guide3DPrimitive.tube:
        return 24;
    }
  }

  /// Minimum segment count the slider can express without collapsing the
  /// mesh to a degenerate shape.
  int get minSegments {
    switch (this) {
      case Guide3DPrimitive.cube:
        return 1;
      case Guide3DPrimitive.pyramid:
        return 3; // < 3 → no base area.
      case Guide3DPrimitive.sphere:
        return 4;
      case Guide3DPrimitive.tube:
        return 3;
    }
  }

  /// Hard upper cap so a runaway slider cannot request a pathological
  /// vertex count. Matches [GuideSurface.kMaxShapeSegments].
  static const int maxSegments = 512;

  /// Short doc string for inspector tooltips.
  String get helpText {
    switch (this) {
      case Guide3DPrimitive.cube:
        return 'Six-faced box. Segments subdivide each face into a grid '
            'so the surface stays drawable at any density.';
      case Guide3DPrimitive.pyramid:
        return 'Square-base pyramid. Segments set the base polygon count '
            '— 4 reads as a true pyramid, higher as a cone, lower as a '
            'triangular prism.';
      case Guide3DPrimitive.sphere:
        return 'UV sphere. Segments control both longitude and latitude '
            'tessellation.';
      case Guide3DPrimitive.tube:
        return 'Open cylinder shell. Segments control the side tessellation; '
            'low counts read as a faceted prism, high counts as a smooth '
            'tube.';
    }
  }
}

/// Parses a [Guide3DPrimitive] from a stored wire name or display name.
///
/// Accepts the enum [Guide3DPrimitive.name] ("cube", …), the
/// [Guide3DPrimitive.displayName] ("Cube", …) and a few common aliases
/// ("box" → cube, "cylinder" → tube, "cone" maps to nothing — Feather
/// expresses cones via the Pyramid with high segment count). The lookup
/// is case-insensitive and tolerates surrounding whitespace. Returns
/// `null` for unknown spellings.
Guide3DPrimitive? guide3DPrimitiveFromNameStrict(String? raw) {
  switch (raw?.trim().toLowerCase()) {
    case 'cube':
    case 'box':
      return Guide3DPrimitive.cube;
    case 'pyramid':
      return Guide3DPrimitive.pyramid;
    case 'sphere':
      return Guide3DPrimitive.sphere;
    case 'tube':
    case 'cylinder':
      return Guide3DPrimitive.tube;
    default:
      return null;
  }
}

/// Tolerant variant of [guide3DPrimitiveFromNameStrict] that falls back
/// to [Guide3DPrimitive.cube] for unknown spellings.
Guide3DPrimitive guide3DPrimitiveFromName(String? raw) =>
    guide3DPrimitiveFromNameStrict(raw) ?? Guide3DPrimitive.cube;

/// Clamps an arbitrary integer into the legal segment range for [p],
/// flooring at the primitive's [Guide3DPrimitive.minSegments] and capping
/// at [Guide3DPrimitive.maxSegments]. Non-finite or non-positive values
/// restore the primitive's [Guide3DPrimitive.defaultSegments].
int clampPrimitiveSegments(Guide3DPrimitive p, num? raw) {
  if (raw is! num || !raw.isFinite || raw <= 0) return p.defaultSegments;
  final i = raw.round();
  if (i < p.minSegments) return p.minSegments;
  if (i > Guide3DPrimitive.maxSegments) return Guide3DPrimitive.maxSegments;
  return i;
}
