// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// transform_state.dart — Pure Riverpod state for the transform joystick.
//
// Holds the live transform mode (move / rotate / scale), the active axis
// constraint, the per-axis lock flags, and whether the on-screen joystick
// is the 2D (planar) or 3D (full 6-DOF) variant.
//
// This is the pure value-type companion to the legacy [JoystickMode] /
// [TransformMode] enums already used by the engine. Coexists with them —
// the legacy enums live on the engine side, this file adds the missing
// axis-constraint and lock state plus the joystick-shape flag the chrome
// needs to render the right handle layout.
//
// Pure value type — emits through a Riverpod [Notifier].

import 'package:feather_krita/engine/stroke_manager.dart' show TransformMode;

/// Which axes the joystick currently drives. The 2D joystick cycles
/// through these one at a time; the 3D joystick can have several
/// enabled at once.
enum TransformAxis {
  /// No axis constrained (free transform).
  none,

  /// X axis only.
  x,

  /// Y axis only.
  y,

  /// Z axis only.
  z,

  /// XY plane.
  xy,

  /// XZ plane.
  xz,

  /// YZ plane.
  yz,
}

/// Joystick visual layout.
enum JoystickType {
  /// Planar 2D handle — drag maps to a single plane (default for the
  /// touch-first UI).
  twoD,

  /// Full 6-DOF 3D handle — three orthogonal rings + a center grip.
  threeD,
}

/// Pure transform state.
class TransformState {
  const TransformState({
    this.mode = TransformMode.move,
    this.axis = TransformAxis.none,
    this.lockX = false,
    this.lockY = false,
    this.lockZ = false,
    this.uniformScale = true,
    this.joystickType = JoystickType.twoD,
    this.snapToGrid = false,
    this.gridSize = 0.25,
    this.snapAngleDeg = 15.0,
  });

  /// Active transform mode.
  final TransformMode mode;

  /// Active axis constraint.
  final TransformAxis axis;

  /// Per-axis lock — when true, that axis is frozen even when not
  /// otherwise constrained.
  final bool lockX;
  final bool lockY;
  final bool lockZ;

  /// When true (default), scale mode preserves aspect ratio.
  final bool uniformScale;

  /// Visual layout of the on-screen joystick.
  final JoystickType joystickType;

  /// Whether transforms snap to a grid of size [gridSize].
  final bool snapToGrid;

  /// Grid cell size in world units (used when [snapToGrid] is true).
  final double gridSize;

  /// Rotation snap increment in degrees (used when [snapToGrid] is true).
  final double snapAngleDeg;

  /// True when [a] is allowed by the current axis + lock state.
  bool axisAllowed(TransformAxis a) {
    switch (a) {
      case TransformAxis.x:
        return !lockX;
      case TransformAxis.y:
        return !lockY;
      case TransformAxis.z:
        return !lockZ;
      case TransformAxis.xy:
        return !lockX && !lockY;
      case TransformAxis.xz:
        return !lockX && !lockZ;
      case TransformAxis.yz:
        return !lockY && !lockZ;
      case TransformAxis.none:
        return true;
    }
  }

  /// Whether any axis is currently locked.
  bool get anyLocked => lockX || lockY || lockZ;

  TransformState copyWith({
    TransformMode? mode,
    TransformAxis? axis,
    bool? lockX,
    bool? lockY,
    bool? lockZ,
    bool? uniformScale,
    JoystickType? joystickType,
    bool? snapToGrid,
    double? gridSize,
    double? snapAngleDeg,
  }) =>
      TransformState(
        mode: mode ?? this.mode,
        axis: axis ?? this.axis,
        lockX: lockX ?? this.lockX,
        lockY: lockY ?? this.lockY,
        lockZ: lockZ ?? this.lockZ,
        uniformScale: uniformScale ?? this.uniformScale,
        joystickType: joystickType ?? this.joystickType,
        snapToGrid: snapToGrid ?? this.snapToGrid,
        gridSize: gridSize ?? this.gridSize,
        snapAngleDeg: snapAngleDeg ?? this.snapAngleDeg,
      );

  Map<String, dynamic> toJson() => {
        'mode': mode.name,
        'axis': axis.name,
        'lockX': lockX,
        'lockY': lockY,
        'lockZ': lockZ,
        'uniformScale': uniformScale,
        'joystickType': joystickType.name,
        'snapToGrid': snapToGrid,
        'gridSize': gridSize,
        'snapAngleDeg': snapAngleDeg,
      };

  @override
  bool operator ==(Object other) =>
      identical(this, other) ||
      other is TransformState &&
          other.mode == mode &&
          other.axis == axis &&
          other.lockX == lockX &&
          other.lockY == lockY &&
          other.lockZ == lockZ &&
          other.uniformScale == uniformScale &&
          other.joystickType == joystickType &&
          other.snapToGrid == snapToGrid &&
          other.gridSize == gridSize &&
          other.snapAngleDeg == snapAngleDeg;

  @override
  int get hashCode => Object.hash(
        mode,
        axis,
        lockX,
        lockY,
        lockZ,
        uniformScale,
        joystickType,
        snapToGrid,
        gridSize,
        snapAngleDeg,
      );
}
