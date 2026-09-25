// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// selection_renderer.dart — Renders the selection highlight (bounding
// box, primary handle, hover indicator).
//
// Like [GizmoDrawList] in the transform module, this produces a pure
// draw-list of screen-space primitives that the canvas can paint. It
// does NOT touch a [Canvas] directly.
//
// Visual design (Feather conventions):
//   - Active strokes  → bright outline + bounding box.
//   - Hovered stroke  → softer outline (no bbox).
//   - Primary stroke  → filled corner handles on the bounding box.
//   - Inactive        → nothing.
//
// The bounding box is the screen-space AABB of the projected world-space
// bounding boxes of all active strokes. When the selection is empty the
// draw-list is empty.

import 'package:vector_math/vector_math_64.dart';

import 'package:feather_krita/engine/selection/selection_state.dart';
import 'package:feather_krita/engine/transform/gizmo_renderer.dart';

/// Output of a single selection render pass.
class SelectionDrawList {
  SelectionDrawList(this.primitives, {this.bounds});
  final List<GizmoPrimitive> primitives;
  /// The screen-space bounding rectangle of the selection, or `null`
  /// when nothing is selected.
  final Aabb2? bounds;
}

/// Screen-space AABB (min/max in pixels).
class Aabb2 {
  Aabb2(this.min, this.max);
  final Vector2 min;
  final Vector2 max;
  Vector2 get center => (min + max) * 0.5;
  Vector2 get size => max - min;
}

/// Colour palette for selection visuals.
class SelectionColors {
  const SelectionColors._();
  static const int activeOutline = 0xFFFFC107; // amber
  static const int hoverOutline = 0xFFFFE082; // soft amber
  static const int bboxOutline = 0xFFFFC107;
  static const int handle = 0xFFFFC107;
  static const int handlePrimary = 0xFFFF6F00; // deep amber for primary
}

/// Renders the selection highlight for one frame.
class SelectionRenderer {
  SelectionRenderer({
    required this.strokesById,
    required this.state,
    required this.viewProjection,
    required this.viewportWidth,
    required this.viewportHeight,
    this.handleSize = 8.0,
  });

  /// Lookup from stroke ID → stroke (used to fetch geometry).
  final Map<int, dynamic> strokesById;
  final SelectionState state;
  final Matrix4 viewProjection;
  final double viewportWidth;
  final double viewportHeight;
  final double handleSize;

  SelectionDrawList build() {
    if (state.isEmpty) return SelectionDrawList(const []);
    final out = <GizmoPrimitive>[];
    var minX = double.infinity, minY = double.infinity;
    var maxX = double.negativeInfinity, maxY = double.negativeInfinity;

    // Per-stroke outlines.
    for (final id in state.active) {
      final stroke = strokesById[id];
      if (stroke == null) continue;
      final pts = _projectStroke(stroke);
      if (pts.isEmpty) continue;
      final isPrimary = state.primary == id;
      final color = isPrimary
          ? SelectionColors.handlePrimary
          : SelectionColors.activeOutline;
      out.add(GizmoPolyline(pts + [pts.first], color, width: 2.0));
      for (final p in pts) {
        if (p.x < minX) minX = p.x;
        if (p.y < minY) minY = p.y;
        if (p.x > maxX) maxX = p.x;
        if (p.y > maxY) maxY = p.y;
      }
    }

    // Hovered stroke (softer outline).
    if (state.hover != null && !state.active.contains(state.hover)) {
      final stroke = strokesById[state.hover];
      if (stroke != null) {
        final pts = _projectStroke(stroke);
        if (pts.isNotEmpty) {
          out.add(GizmoPolyline(pts + [pts.first],
              SelectionColors.hoverOutline,
              width: 1.5));
        }
      }
    }

    if (minX == double.infinity) {
      return SelectionDrawList(out);
    }

    // Bounding box.
    final min = Vector2(minX, minY);
    final max = Vector2(maxX, maxY);
    final corners = [
      min,
      Vector2(max.x, min.y),
      max,
      Vector2(min.x, max.y),
    ];
    out.add(GizmoPolyline(corners + [corners.first],
        SelectionColors.bboxOutline,
        width: 1.5));

    // Corner handles.
    final primary = state.primary;
    for (final c in corners) {
      final color = primary != null
          ? SelectionColors.handlePrimary
          : SelectionColors.handle;
      out.add(GizmoCircle(c, handleSize, color, filled: true));
    }

    return SelectionDrawList(out, bounds: Aabb2(min, max));
  }

  /// Projects a stroke's points to screen space. [stroke] is `dynamic`
  /// so this module doesn't depend on [Stroke] directly (keeps the
  /// module testable in isolation), but it expects the duck-typed
  /// `transform`, `points` (with `.position`), and `isVisible`.
  List<Vector2> _projectStroke(dynamic stroke) {
    if (stroke.isVisible != true) return const [];
    final out = <Vector2>[];
    final transform = stroke.transform as Matrix4;
    final points = stroke.points as List;
    for (final p in points) {
      final pos = p.position as Vector3;
      final world = transform.transform3(pos.clone());
      final ndc = viewProjection.transform3(world.clone());
      if (ndc.z >= 1.0 || ndc.z <= -1.0) continue;
      final sx = (ndc.x * 0.5 + 0.5) * viewportWidth;
      final sy = (1.0 - (ndc.y * 0.5 + 0.5)) * viewportHeight;
      out.add(Vector2(sx, sy));
    }
    return out;
  }
}
