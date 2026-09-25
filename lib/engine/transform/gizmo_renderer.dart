// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// gizmo_renderer.dart — Builds the draw-list for the on-screen transform
// gizmo (2D joystick stick + handles, 3D joystick cones + arcs + sphere).
//
// This module is pure data: it produces a list of [GizmoPrimitive]s
// (lines, triangles, circles) in *screen space* that the Flutter
// CustomPainter (or any other backend) can render. It does NOT paint
// pixels itself — keeping the geometry separate from the paint backend
// matches the architecture of `lib/core/scene/scene.dart` and lets
// the gizmo be unit-tested without a Canvas.
//
// Visual design follows the Feather docs:
//   - 2D joystick: central stick (circle) + scale handles (above / left)
//     + rotate handle (right). Lock mode replaces per-axis handles with
//     one uniform handle.
//   - 3D joystick: central sphere + three cones (red/green/blue) along
//     world axes + three arcs (red/green/blue) perpendicular to those
//     axes. Cones / arcs are hidden in perfect views when their axis is
//     parallel to the view normal.

import 'dart:math' as math;

import 'package:vector_math/vector_math_64.dart';

import 'package:feather_krita/engine/transform/transform_mode.dart';
import 'package:feather_krita/engine/transform/transform_resolver.dart';

/// One drawable primitive of the gizmo, in screen-space pixels relative
/// to the joystick centre.
abstract class GizmoPrimitive {
  /// ARGB colour of the primitive.
  final int color;
  /// Stroke width for lines / outlines, or fill alpha for triangles.
  final double strokeWidth;
  const GizmoPrimitive({required this.color, this.strokeWidth = 2.0});
}

/// A line segment.
class GizmoLine extends GizmoPrimitive {
  final Vector2 a, b;
  const GizmoLine(this.a, this.b, int color, {double width = 2.0})
      : super(color: color, strokeWidth: width);
}

/// A polyline (chain of segments).
class GizmoPolyline extends GizmoPrimitive {
  final List<Vector2> points;
  const GizmoPolyline(this.points, int color, {double width = 2.0})
      : super(color: color, strokeWidth: width);
}

/// A filled or outlined circle.
class GizmoCircle extends GizmoPrimitive {
  final Vector2 center;
  final double radius;
  final bool filled;
  const GizmoCircle(this.center, this.radius, int color,
      {this.filled = false, double width = 2.0})
      : super(color: color, strokeWidth: width);
}

/// A filled triangle (used for cones).
class GizmoTriangle extends GizmoPrimitive {
  final Vector2 a, b, c;
  const GizmoTriangle(this.a, this.b, this.c, int color)
      : super(color: color);
}

/// The full draw-list for one frame of the gizmo.
class GizmoDrawList {
  GizmoDrawList(this.primitives);
  final List<GizmoPrimitive> primitives;
}

/// Colour palette for the gizmo (ARGB packed).
class GizmoColors {
  const GizmoColors._();
  static const int red = 0xFFE53935; // X axis
  static const int green = 0xFF43A047; // Y axis
  static const int blue = 0xFF1E88E5; // Z axis
  static const int stick = 0xFFECEFF1; // 2D joystick stick
  static const int stickActive = 0xFF212121; // stick when pressed
  static const int handle = 0xFFFFB74D; // scale / rotate handles
  static const int outline = 0xFFB0BEC5; // neutral outline
}

/// Renders the 2D joystick's geometry.
///
/// [centre] is the joystick centre in screen pixels. [radius] is the
/// outer ring radius. [stickOffset] is the current stick displacement
/// (pixels, can exceed [radius] — the doc says "the stick can move
/// outside the joystick layout"). [lock] toggles the locked layout.
class Joystick2dGizmoRenderer {
  Joystick2dGizmoRenderer({
    required this.centre,
    required this.radius,
    Vector2? stickOffset,
    this.lock = JoystickLock.off,
    this.activeHandle,
  }) : stickOffset = stickOffset ?? Vector2.zero();

  final Vector2 centre;
  final double radius;
  final Vector2 stickOffset;
  final JoystickLock lock;
  final TransformMode2d? activeHandle;

  GizmoDrawList build() {
    final out = <GizmoPrimitive>[];
    // Outer ring.
    out.add(GizmoCircle(centre, radius, GizmoColors.outline, width: 1.5));
    // Crosshair (rotation / scale reference point = screen centre).
    out.add(GizmoLine(
        centre - Vector2(6, 0), centre + Vector2(6, 0), GizmoColors.outline,
        width: 1.0));
    out.add(GizmoLine(
        centre - Vector2(0, 6), centre + Vector2(0, 6), GizmoColors.outline,
        width: 1.0));
    // Stick (centre circle).
    final stickPos = centre + stickOffset;
    final stickColor =
        activeHandle == TransformMode2d.move ? GizmoColors.stickActive : GizmoColors.stick;
    out.add(GizmoCircle(stickPos, radius * 0.18, stickColor, filled: true));
    // Scale handles.
    if (lock == JoystickLock.on) {
      // Single uniform handle above the stick.
      out.add(GizmoCircle(centre + Vector2(0, -radius * 0.85),
          radius * 0.10, GizmoColors.handle, filled: true));
    } else {
      // Width handle (left), Height handle (above), Free handle (upper-left).
      out.add(GizmoCircle(centre + Vector2(-radius * 0.85, 0),
          radius * 0.10, GizmoColors.handle, filled: true));
      out.add(GizmoCircle(centre + Vector2(0, -radius * 0.85),
          radius * 0.10, GizmoColors.handle, filled: true));
      out.add(GizmoCircle(centre + Vector2(-radius * 0.6, -radius * 0.6),
          radius * 0.10, GizmoColors.handle, filled: true));
    }
    // Rotate handle (right of stick).
    out.add(GizmoCircle(centre + Vector2(radius * 0.85, 0),
        radius * 0.10, GizmoColors.handle, filled: true));
    return GizmoDrawList(out);
  }
}

/// Renders the 3D joystick's geometry.
///
/// [centre] is the joystick centre in screen pixels. [radius] is the
/// sphere radius. [frame] provides the camera basis so the cones / arcs
/// can be projected correctly. [usableAxes] (typically from
/// [Joystick3dResolver.usableAxes]) hides cones that aren't reachable
/// in the current view.
class Joystick3dGizmoRenderer {
  Joystick3dGizmoRenderer({
    required this.centre,
    required this.radius,
    required this.frame,
    this.usableAxes = const {
      TransformMode3d.moveX,
      TransformMode3d.moveY,
      TransformMode3d.moveZ,
    },
    this.activeHandle,
  });

  final Vector2 centre;
  final double radius;
  final ViewFrame frame;
  final Set<TransformMode3d> usableAxes;
  final TransformMode3d? activeHandle;

  GizmoDrawList build() {
    final out = <GizmoPrimitive>[];
    // Central sphere (free-rotate handle).
    final sphereColor = activeHandle == TransformMode3d.freeRotate
        ? GizmoColors.handle
        : GizmoColors.stick;
    out.add(GizmoCircle(centre, radius * 0.30, sphereColor, filled: true));
    out.add(GizmoCircle(centre, radius * 0.30, GizmoColors.outline,
        width: 1.5));
    // Cones + arcs for each axis.
    _emitAxis(out, TransformMode3d.moveX, Vector3(1, 0, 0), GizmoColors.red);
    _emitAxis(out, TransformMode3d.moveY, Vector3(0, 1, 0), GizmoColors.green);
    _emitAxis(out, TransformMode3d.moveZ, Vector3(0, 0, 1), GizmoColors.blue);
    return GizmoDrawList(out);
  }

  void _emitAxis(List<GizmoPrimitive> out, TransformMode3d mode,
      Vector3 axis, int color) {
    final usable = usableAxes.contains(mode);
    // Project axis tip into screen space.
    final sx = axis.dot(frame.right);
    final sy = -axis.dot(frame.up);
    final tip = centre + Vector2(sx, sy) * radius;
    if (!usable) {
      // Axis aligned with view normal — degenerate: draw a small dot.
      out.add(GizmoCircle(tip, radius * 0.06, color, filled: true));
      return;
    }
    // Cone: a triangle at the axis tip.
    final dir = (tip - centre)..normalize();
    final perp = Vector2(-dir.y, dir.x);
    final coneBase = tip - dir * radius * 0.15;
    final coneWidth = radius * 0.08;
    out.add(GizmoTriangle(
      coneBase + perp * coneWidth,
      coneBase - perp * coneWidth,
      tip,
      color,
    ));
    // Axis line from centre to cone base.
    out.add(GizmoLine(centre, coneBase, color, width: 2.0));
    // Arc (perpendicular to axis). Sample a circle in the axis's
    // perpendicular plane and project it.
    final arcPoints = <Vector2>[];
    final radial = Vector3(axis.y, axis.z, axis.x); // any non-parallel vec
    final tangent = axis.cross(radial)..normalize();
    final bitangent = axis.cross(tangent)..normalize();
    final steps = 24;
    for (var i = 0; i <= steps; i++) {
      final t = i / steps * 2 * math.pi;
      final p3 = (tangent * math.cos(t) + bitangent * math.sin(t)) * radius;
      final px = p3.dot(frame.right);
      final py = -p3.dot(frame.up);
      arcPoints.add(centre + Vector2(px, py));
    }
    out.add(GizmoPolyline(arcPoints, color, width: 1.5));
  }
}
