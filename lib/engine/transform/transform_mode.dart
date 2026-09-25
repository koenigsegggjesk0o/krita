// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// transform_mode.dart — Enumerations for the Feather transform tool.
//
// The Feather transform UI exposes two distinct joysticks (the 2D view-based
// joystick and the 3D world-axis joystick). Each one can be in a number of
// sub-modes that determine what dragging its handle does. The enums below
// mirror the modes documented in:
//   - docs/transform_interface.txt
//   - docs/transform_2djoystick.txt
//   - docs/transform_3djoystick.txt
//
// `TransformMode2d` covers the six labelled buttons of the 2D joystick panel
// (Move, Rotate, Free Scale, Width Scale, Height Scale, and the implicit
// "Lock" toggle that restricts the others). `TransformMode3d` covers the
// seven labelled handles of the 3D joystick (three axis-move cones, three
// axis-rotate arcs, and the central trackball free-rotate sphere).

/// Sub-modes of the 2D joystick.
///
/// The 2D joystick moves / rotates / scales objects based on the *view*
/// direction: "It's very intuitive because it transforms as it appears."
/// All 2D-joystick transforms happen in the camera's right/up plane and the
/// rotation axis is the camera's forward (view normal) direction.
enum TransformMode2d {
  /// Drag the central stick to move the selection in the view plane.
  /// Direction of the drag in screen space maps to camera-right × camera-up.
  move,

  /// Spin the rotate handle (right of the stick) to rotate the selection
  /// around the view normal (camera forward), centred on the screen
  /// crosshair.
  rotate,

  /// Drag the corner scale handle to scale uniformly in both width and
  /// height around the screen crosshair.
  freeScale,

  /// Drag the horizontal scale handle to scale only along the view's
  /// horizontal axis (width).
  widthScale,

  /// Drag the vertical scale handle to scale only along the view's
  /// vertical axis (height).
  heightScale,
}

/// Sub-modes of the 3D joystick.
///
/// The 3D joystick moves / rotates objects based on the *global* XYZ axes.
/// Red = X axis, green = Y axis, blue = Z axis. The cones drive translations
/// along the world axes, the arcs drive rotations around them, and the
/// central sphere drives a trackball free-rotate around the object's
/// invisible centre.
enum TransformMode3d {
  /// Drag the red cone — translate along world +X / -X.
  moveX,

  /// Drag the green cone — translate along world +Y / -Y.
  moveY,

  /// Drag the blue cone — translate along world +Z / -Z.
  moveZ,

  /// Spin the red arc — rotate around the world X axis.
  rotateX,

  /// Spin the green arc — rotate around the world Y axis.
  rotateY,

  /// Spin the blue arc — rotate around the world Z axis.
  rotateZ,

  /// Drag the central sphere — free rotate (trackball) around the object's
  /// centre.
  freeRotate,
}

/// Lock state of the 2D joystick.
///
/// "Locking the joystick restricts movement and rotation values. It's useful
/// for precise transformations." When locked:
///   - Move is restricted to the four cardinal directions (up / down / left /
///     right).
///   - A single uniform scale handle replaces the per-axis handles.
///   - Rotation snaps to 15-degree increments.
enum JoystickLock {
  /// Free joystick — full directional movement, per-axis scale, free
  /// rotation.
  off,

  /// Locked joystick — 4-direction move, uniform scale, 15°-snap rotation.
  on,
}

/// Which joystick is currently active. Switching to 3D is itself a labelled
/// button of the 2D joystick panel.
enum JoystickKind {
  /// View-based 2D joystick.
  joystick2d,

  /// World-axis 3D joystick.
  joystick3d,
}

/// Returns the human-readable label for a [TransformMode2d], as shown on the
/// 2D joystick panel.
String label2d(TransformMode2d mode) {
  switch (mode) {
    case TransformMode2d.move:
      return 'Move';
    case TransformMode2d.rotate:
      return 'Rotate';
    case TransformMode2d.freeScale:
      return 'Free Scale';
    case TransformMode2d.widthScale:
      return 'Width Scale';
    case TransformMode2d.heightScale:
      return 'Height Scale';
  }
}

/// Returns the human-readable label for a [TransformMode3d], as shown on the
/// 3D joystick panel.
String label3d(TransformMode3d mode) {
  switch (mode) {
    case TransformMode3d.moveX:
      return 'Move to X Axis';
    case TransformMode3d.moveY:
      return 'Move to Y Axis';
    case TransformMode3d.moveZ:
      return 'Move to Z Axis';
    case TransformMode3d.rotateX:
      return 'Rotate by X Axis';
    case TransformMode3d.rotateY:
      return 'Rotate by Y Axis';
    case TransformMode3d.rotateZ:
      return 'Rotate by Z Axis';
    case TransformMode3d.freeRotate:
      return 'Free Rotate';
  }
}
