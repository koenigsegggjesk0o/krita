// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// joystick3d.dart — World-axis-based 3D joystick transform resolver.
//
// Implements the Feather 3D Joystick behaviour described in
// docs/transform_3djoystick.txt:
//
//   "The 3D Joystick moves or rotates objects based on the global XYZ
//    axes. The global axes appear differently depending on the view
//    direction, and so does the 3D Joystick."
//
//   - Red / green / blue cone  → translate along world X / Y / Z.
//   - Red / green / blue arc   → rotate around world X / Y / Z. The
//     rotation center point is the invisible center of the selected
//     object.
//   - Central sphere           → free rotate (trackball) around the
//     object's center.
//
// In perfect views (front / side / top), only two of the three cones are
// usable — that's a natural consequence of the view direction (one axis
// points straight at the camera and so its cone is hidden behind the
// sphere). This resolver exposes [usableAxes] so the gizmo renderer can
// skip drawing the unusable cone.
//
// Like the 2D resolver, this is stateless: each call returns one
// [TransformDelta]. The caller accumulates them.

import 'dart:math' as math;

import 'package:vector_math/vector_math_64.dart';

import 'package:feather_krita/engine/transform/trackball.dart';
import 'package:feather_krita/engine/transform/transform_mode.dart';
import 'package:feather_krita/engine/transform/transform_resolver.dart';

/// 3D joystick resolver.
///
/// Construct with a [worldPerPixel] conversion (for axis translations)
/// and a [rotateSpeed] (radians per unit of arc drag). The trackball used
/// for free rotate is constructed from [trackballRadiusPx].
class Joystick3dResolver implements TransformResolver {
  Joystick3dResolver({
    this.worldPerPixel = 0.01,
    this.rotateSpeed = math.pi * 2.0,
    this.trackballRadiusPx = 80.0,
  });

  final double worldPerPixel;
  final double rotateSpeed;
  final double trackballRadiusPx;

  @override
  JoystickKind get kind => JoystickKind.joystick3d;

  @override
  TransformDelta resolve({
    required Object mode,
    JoystickLock lock = JoystickLock.off, // unused for 3D
    required Vector2 delta,
    required ViewFrame frame,
  }) {
    if (mode is! TransformMode3d) {
      throw ArgumentError(
          'Joystick3dResolver expects TransformMode3d, got $mode');
    }
    switch (mode) {
      case TransformMode3d.moveX:
        return _translateAlong(Vector3(1, 0, 0), delta, frame);
      case TransformMode3d.moveY:
        return _translateAlong(Vector3(0, 1, 0), delta, frame);
      case TransformMode3d.moveZ:
        return _translateAlong(Vector3(0, 0, 1), delta, frame);
      case TransformMode3d.rotateX:
        return _rotateAround(Vector3(1, 0, 0), delta, frame);
      case TransformMode3d.rotateY:
        return _rotateAround(Vector3(0, 1, 0), delta, frame);
      case TransformMode3d.rotateZ:
        return _rotateAround(Vector3(0, 0, 1), delta, frame);
      case TransformMode3d.freeRotate:
        return _freeRotate(delta, frame);
    }
  }

  // ----- Axis translation ----------------------------------------------

  /// Translates the selection along a world [axis]. The drag delta is
  /// measured in the screen plane; we project it onto the axis's screen
  /// projection (drag in the direction the axis appears to point on
  /// screen). This matches Feather's "drag the cone along the axis"
  /// behaviour: the cone is rendered at the axis tip, so dragging it
  /// along the axis's screen projection moves the object along that
  /// world axis.
  TransformDelta _translateAlong(
      Vector3 axis, Vector2 delta, ViewFrame frame) {
    // Project the world axis into screen space: x_screen = axis · right,
    // y_screen = -(axis · up) (screen Y is flipped).
    final sx = axis.dot(frame.right);
    final sy = -axis.dot(frame.up);
    final screenDir = Vector2(sx, sy);
    final len2 = screenDir.length2;
    if (len2 < 1e-6) {
      // Axis points along the view normal — cone is unusable.
      return TransformDelta.zero;
    }
    // Project the drag delta onto the axis's screen direction.
    final t = (delta.x * screenDir.x + delta.y * screenDir.y) / len2;
    // Convert "screen units" to world units. delta is in [-1, 1], so we
    // scale by viewport half-size × worldPerPixel.
    final worldDist = t *
        math.sqrt(len2) *
        frame.viewportWidth *
        0.5 *
        worldPerPixel *
        math.sqrt(len2);
    final translation = axis * worldDist;
    return TransformDelta(translation: translation);
  }

  // ----- Axis rotation -------------------------------------------------

  /// Rotates the selection around a world [axis]. The drag delta is
  /// measured in the screen plane perpendicular to the axis (the arc's
  /// plane). The signed angle is the drag's component along the arc's
  /// tangent.
  TransformDelta _rotateAround(
      Vector3 axis, Vector2 delta, ViewFrame frame) {
    // The arc lives in the plane perpendicular to the axis. Build two
    // in-plane screen directions: the axis's screen projection (call it
    // "radial") and a perpendicular ("tangent").
    final sx = axis.dot(frame.right);
    final sy = -axis.dot(frame.up);
    final radial = Vector2(sx, sy);
    if (radial.length2 < 1e-6) {
      // Axis aligned with view — arc degenerates to a circle, drag
      // direction doesn't matter, use full delta magnitude.
      final angle = delta.length * rotateSpeed /
          (frame.viewportWidth * 0.5);
      final sign = (delta.x + delta.y) >= 0 ? 1.0 : -1.0;
      return TransformDelta(
        rotation: Quaternion.axisAngle(axis, sign * angle),
        pivot: frame.crosshairWorld,
      );
    }
    final tangent = Vector2(-radial.y, radial.x)..normalize();
    // Signed drag along the tangent direction = rotation angle.
    final signed = delta.x * tangent.x + delta.y * tangent.y;
    final angle = signed * rotateSpeed;
    return TransformDelta(
      rotation: Quaternion.axisAngle(axis, angle),
      pivot: frame.crosshairWorld,
    );
  }

  // ----- Free rotate (trackball) ---------------------------------------

  TransformDelta _freeRotate(Vector2 delta, ViewFrame frame) {
    final tb = Trackball(radius: trackballRadiusPx);
    final px = delta.x * frame.viewportWidth * 0.5;
    final py = delta.y * frame.viewportHeight * 0.5;
    final q = tb.drag(const Offset2.zero(), Offset2(px, py));
    // The trackball emits rotations in screen space (z = out of screen).
    // Re-express as a world-space quaternion by rotating the screen-z
    // axis into world space (camera forward is -z, so out-of-screen is
    // -forward).
    final screenZ = -frame.forward;
    final (axis, angle) = _toAxisAngle(q);
    if (angle.abs() < 1e-6) return TransformDelta.zero;
    // The axis returned by Trackball is in screen coords (x=right,
    // y=up, z=out). Map to world.
    final worldAxis =
        frame.right * axis.x + frame.up * axis.y + screenZ * axis.z;
    if (worldAxis.length2 < 1e-9) return TransformDelta.zero;
    worldAxis.normalize();
    return TransformDelta(
      rotation: Quaternion.axisAngle(worldAxis, angle),
      pivot: frame.crosshairWorld,
    );
  }

  (Vector3, double) _toAxisAngle(Quaternion q) {
    final w = q.w.clamp(-1.0, 1.0);
    final angle = 2 * math.acos(w);
    final s = math.sqrt(1 - w * w);
    if (s < 1e-6) return (Vector3(1, 0, 0), 0);
    return (Vector3(q.x / s, q.y / s, q.z / s), angle);
  }

  // ----- Perfect-view helpers ------------------------------------------

  /// Returns the set of axes whose cones are usable in the current view.
  /// An axis is usable when it is NOT aligned with the view normal
  /// (|axis · forward| < 0.99). In perfect front/side/top views exactly
  /// one axis becomes unusable.
  Set<TransformMode3d> usableAxes(ViewFrame frame) {
    final out = <TransformMode3d>{};
    final axes = <TransformMode3d, Vector3>{
      TransformMode3d.moveX: Vector3(1, 0, 0),
      TransformMode3d.moveY: Vector3(0, 1, 0),
      TransformMode3d.moveZ: Vector3(0, 0, 1),
    };
    axes.forEach((mode, axis) {
      if (axis.dot(frame.forward).abs() < 0.99) {
        out.add(mode);
      }
    });
    return out;
  }
}
