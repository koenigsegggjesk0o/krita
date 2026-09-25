// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// joystick2d.dart — View-based 2D joystick transform resolver.
//
// Implements the Feather 2D Joystick behaviour described in
// docs/transform_2djoystick.txt:
//
//   "The 2D Joystick moves, rotates, and scales objects based on the view
//    direction. It's very intuitive because it transforms as it appears."
//
// Behaviour summary (per the doc):
//   - Stick (centre circle) drag  → move. Drag in the direction the
//     object should travel; the stick can move outside the joystick
//     layout. When locked, only the 4 cardinal directions are allowed.
//   - Scale handle drag           → scale. Free / Width / Height handles
//     sit above and to the left of the stick. When locked, a single
//     uniform handle replaces them.
//   - Rotate handle drag          → rotate around the view normal
//     (camera forward), centred on the screen crosshair. When locked,
//     rotation snaps to 15° increments.
//
// All transforms are *view-based*: translations live in the camera's
// right/up plane, rotation is around the camera forward axis, and scale
// is along the camera right (width) / up (height) axes. The "scaling
// reference point is the center of the screen, marked with a crosshair."
//
// This resolver is stateless: it returns a [TransformDelta] for one
// pointer event. The caller (selection system) accumulates the deltas
// and applies them to the active selection.

import 'dart:math' as math;

import 'package:vector_math/vector_math_64.dart';

import 'package:feather_krita/engine/transform/transform_mode.dart';
import 'package:feather_krita/engine/transform/transform_resolver.dart';

/// 2D joystick resolver.
///
/// Construct with a [worldPerPixel] conversion (world units per screen
/// pixel at the selection's depth) and a [rotateSpeed] (radians of
/// rotation per unit of normalised handle drag).
class Joystick2dResolver implements TransformResolver {
  Joystick2dResolver({
    this.worldPerPixel = 0.01,
    this.rotateSpeed = math.pi * 2.0, // full turn for a unit drag
    this.scaleSpeed = 1.5, // up to 1.5x scale for a unit drag
  });

  /// World units per screen pixel at the selection depth. Used by move.
  final double worldPerPixel;

  /// Radians of rotation per unit of normalised handle drag.
  final double rotateSpeed;

  /// Scale multiplier per unit of normalised handle drag.
  final double scaleSpeed;

  @override
  JoystickKind get kind => JoystickKind.joystick2d;

  @override
  TransformDelta resolve({
    required Object mode,
    JoystickLock lock = JoystickLock.off,
    required Vector2 delta,
    required ViewFrame frame,
  }) {
    if (mode is! TransformMode2d) {
      throw ArgumentError(
          'Joystick2dResolver expects TransformMode2d, got $mode');
    }
    final d = lock == JoystickLock.on ? _applyLock(mode, delta) : delta;
    switch (mode) {
      case TransformMode2d.move:
        return _move(d, frame);
      case TransformMode2d.rotate:
        return _rotate(d, frame, lock);
      case TransformMode2d.freeScale:
        return _scale(d, frame, free: true);
      case TransformMode2d.widthScale:
        return _scale(d, frame, widthOnly: true);
      case TransformMode2d.heightScale:
        return _scale(d, frame, heightOnly: true);
    }
  }

  // ----- Move -----------------------------------------------------------

  TransformDelta _move(Vector2 d, ViewFrame frame) {
    // Drag right  → move along +camera-right.
    // Drag up     → move along +camera-up.
    final dx = d.x * frame.viewportWidth * 0.5 * worldPerPixel;
    final dy = -d.y * frame.viewportHeight * 0.5 * worldPerPixel;
    final t = frame.right * dx + frame.up * dy;
    return TransformDelta(translation: t);
  }

  // ----- Rotate ---------------------------------------------------------

  TransformDelta _rotate(Vector2 d, ViewFrame frame, JoystickLock lock) {
    // Rotate handle: the angle of the drag vector (relative to +X) maps
    // directly to the rotation angle around the view normal. Spinning
    // the handle = dragging it around the joystick centre.
    var angle = math.atan2(-d.y, d.x);
    // When the drag starts at angle 0 (handle resting on +X), we want
    // the rotation to be 0 too — so subtract the resting angle.
    // The handle's resting position is along +X (right of the stick).
    // angle is already relative to +X, so just scale it.
    angle *= rotateSpeed / (2 * math.pi); // normalise then re-scale
    if (lock == JoystickLock.on) {
      // Snap to 15° increments.
      const snap = 15.0 * math.pi / 180.0;
      angle = (angle / snap).round() * snap;
    }
    // Rotation axis = view normal (camera forward).
    final q = Quaternion.axisAngle(frame.forward, angle);
    return TransformDelta(
      rotation: q,
      pivot: frame.crosshairWorld,
    );
  }

  // ----- Scale ----------------------------------------------------------

  TransformDelta _scale(
    Vector2 d,
    ViewFrame frame, {
    bool free = false,
    bool widthOnly = false,
    bool heightOnly = false,
  }) {
    // Drag up = grow, drag down = shrink. We use the y-component for
    // uniform and height scale, x-component for width scale, and the
    // max(|dx|, |dy|) for free scale.
    double sx = 1.0, sy = 1.0, sz = 1.0;
    if (free) {
      final mag = math.max(d.x.abs(), (-d.y).abs());
      final factor = 1.0 + mag * scaleSpeed;
      sx = sy = sz = factor;
    } else if (widthOnly) {
      sx = 1.0 + d.x.abs() * scaleSpeed * (d.x >= 0 ? 1 : -1);
      // Width scale uses drag *direction*: right = grow, left = shrink.
      sx = 1.0 + d.x * scaleSpeed;
    } else if (heightOnly) {
      sy = 1.0 - d.y * scaleSpeed; // up (negative y) grows
    }
    return TransformDelta(
      scale: Vector3(sx, sy, sz),
      pivot: frame.crosshairWorld,
    );
  }

  // ----- Lock mode helper ----------------------------------------------

  /// Restricts a drag delta according to the lock rules:
  ///   - Move: only the dominant axis survives (4-direction move).
  ///   - Rotate: handled by the rotate path (15° snap).
  ///   - Scale: only y survives (uniform scale handle).
  Vector2 _applyLock(TransformMode2d mode, Vector2 d) {
    switch (mode) {
      case TransformMode2d.move:
        // 4-direction: keep only the dominant component.
        if (d.x.abs() >= d.y.abs()) {
          return Vector2(d.x, 0.0);
        } else {
          return Vector2(0.0, d.y);
        }
      case TransformMode2d.freeScale:
      case TransformMode2d.widthScale:
      case TransformMode2d.heightScale:
        // Locked = single uniform handle = use y only.
        return Vector2(0.0, d.y);
      case TransformMode2d.rotate:
        return d; // snap handled in _rotate
    }
  }
}
