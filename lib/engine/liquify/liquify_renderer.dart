// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// liquify_renderer.dart — Renders the liquify brush preview (the on-screen
// circle that follows the pen) plus the range indicator (the soft outer
// ring showing the falloff extent).
//
// The preview is pure data — a [LiquifyDrawList] of screen-space
// primitives that the canvas can paint. This matches the architecture of
// `lib/engine/transform/gizmo_renderer.dart` and
// `lib/engine/selection/selection_renderer.dart`.
//
// Visual design:
//   - **Inner circle**  — solid outline at radius `size_px` = the
//     brush's full-influence radius.
//   - **Outer ring**    — dashed outline at radius `reach_px` = the
//     brush's falloff extent (size + range).
//   - **Fill**          — translucent tint inside the inner circle,
//     colour-coded by the active brush (Push = blue, Pinch = orange,
//     Comb = green) so the user can tell at a glance which brush is
//     active.
//   - **Drag arrow**    — when the user is mid-drag, an arrow from the
//     brush centre in the drag direction. Length scaled by strength.

import 'dart:math' as math;

import 'package:vector_math/vector_math_64.dart';

import 'package:feather_krita/engine/liquify/liquify_brush.dart';
import 'package:feather_krita/engine/liquify/liquify_settings.dart';
import 'package:feather_krita/engine/transform/gizmo_renderer.dart';

/// Output of a single liquify-render pass.
class LiquifyDrawList {
  LiquifyDrawList(this.primitives);
  final List<GizmoPrimitive> primitives;
}

/// Colour palette for the liquify brush preview.
class LiquifyColors {
  const LiquifyColors._();
  static const int push = 0xFF42A5F5; // blue
  static const int pinch = 0xFFFFA726; // orange
  static const int comb = 0xFF66BB6A; // green
  static const int innerFill = 0x33FFFFFF; // 20% white
  static const int dash = 0xFFB0BEC5; // neutral
  static const int arrow = 0xFFFFFFFF;
}

/// Returns the brush-coded colour for [type].
int liquifyColorFor(LiquifyBrushType type) {
  switch (type) {
    case LiquifyBrushType.push:
      return LiquifyColors.push;
    case LiquifyBrushType.pinch:
      return LiquifyColors.pinch;
    case LiquifyBrushType.comb:
      return LiquifyColors.comb;
  }
}

/// Renders the liquify brush preview for one frame.
class LiquifyRenderer {
  LiquifyRenderer({
    required this.centre,
    required this.settings,
    required this.brushType,
    this.dragDelta,
    this.worldToScreen = 100.0, // px per world unit
  });

  /// Brush centre in screen pixels.
  final Vector2 centre;

  /// Active liquify settings (drives the radii).
  final LiquifySettings settings;

  /// Active brush type (drives the colour).
  final LiquifyBrushType brushType;

  /// Current drag delta in screen pixels (null = not dragging).
  final Vector2? dragDelta;

  /// Conversion factor: screen pixels per world unit. The settings'
  /// `size` and `range` are in world units; this scales them to px.
  final double worldToScreen;

  LiquifyDrawList build() {
    final out = <GizmoPrimitive>[];
    final color = liquifyColorFor(brushType);
    final sizePx = settings.size * worldToScreen;
    final reachPx = settings.reach * worldToScreen;

    // Outer ring (dashed) — falloff extent.
    out.addAll(_dashedCircle(centre, reachPx, LiquifyColors.dash,
        dashCount: 32, width: 1.0));

    // Inner circle (solid) — full-influence radius.
    out.add(GizmoCircle(centre, sizePx, color, width: 1.5));

    // Translucent fill inside the inner circle. Bit-OR a 20% alpha
    // onto the colour so the tint is visibly the brush's own colour.
    final fill = (color & 0x00FFFFFF) | 0x33000000;
    out.add(GizmoCircle(centre, sizePx, fill, filled: true));

    // Drag arrow when dragging.
    final drag = dragDelta;
    if (drag != null && drag.length2 > 1.0) {
      final arrowEnd = centre + drag;
      out.add(GizmoLine(centre, arrowEnd, LiquifyColors.arrow, width: 2.0));
      // Arrowhead.
      final dir = (arrowEnd - centre)..normalize();
      final perp = Vector2(-dir.y, dir.x);
      const headLen = 8.0;
      const headWidth = 4.0;
      out.add(GizmoTriangle(
        arrowEnd,
        arrowEnd - dir * headLen + perp * headWidth,
        arrowEnd - dir * headLen - perp * headWidth,
        LiquifyColors.arrow,
      ));
    }

    return LiquifyDrawList(out);
  }

  /// Builds a dashed circle as a list of short polyline segments.
  List<GizmoPrimitive> _dashedCircle(
    Vector2 centre,
    double radius,
    int color, {
    int dashCount = 32,
    double width = 1.0,
  }) {
    if (radius <= 0) return const [];
    final out = <GizmoPrimitive>[];
    // Each dash = 1/2 of a segment; the other half is a gap.
    final step = 2 * math.pi / dashCount;
    for (var i = 0; i < dashCount; i++) {
      final a0 = i * step;
      final a1 = i * step + step * 0.5;
      final p0 = centre + Vector2(radius * math.cos(a0), radius * math.sin(a0));
      final p1 = centre + Vector2(radius * math.cos(a1), radius * math.sin(a1));
      out.add(GizmoLine(p0, p1, color, width: width));
    }
    return out;
  }
}
