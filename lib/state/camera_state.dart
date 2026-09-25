// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// camera_state.dart — Pure Riverpod state for the orbit camera.
//
// Mirrors the user-facing slice of [CameraController] as an immutable
// value type: yaw / pitch / distance / target / FOV / projection mode /
// orbit point. The legacy [CameraController] (a [ChangeNotifier]) still
// owns the live damping animation; this file is the data slice the
// chrome widgets (camera reset button, ortho/persp toggle, FOV slider)
// subscribe to via Riverpod.
//
// Pure value type — emits through a Riverpod [Notifier].

import 'package:vector_math/vector_math_64.dart' show Vector3;

/// Projection mode.
enum CameraProjection {
  /// Perspective (default).
  perspective,

  /// Orthographic.
  orthographic,
}

/// Pure camera state. All angular values are in radians.
class CameraState {
  const CameraState({
    this.yaw = 0.0,
    this.pitch = -0.4,
    this.distance = 6.0,
    this.target,
    this.fovYRadians = 1.0471975511965976, // 60 degrees
    this.near = 0.05,
    this.far = 1000.0,
    this.projection = CameraProjection.perspective,
    this.orthoScale = 4.0,
    this.minDistance = 0.5,
    this.maxDistance = 200.0,
    this.minPitch = -1.55,
    this.maxPitch = 1.55,
  });

  /// Yaw (horizontal rotation) — target value.
  final double yaw;

  /// Pitch (vertical rotation) — target value.
  final double pitch;

  /// Distance from the orbit target.
  final double distance;

  /// Orbit target point (world space). `null` is treated as the origin
  /// by [effectiveTarget] so the const default constructor stays const.
  final Vector3? target;

  /// Vertical field of view (perspective only).
  final double fovYRadians;

  /// Near clip plane distance.
  final double near;

  /// Far clip plane distance.
  final double far;

  /// Active projection.
  final CameraProjection projection;

  /// Half-height of the orthographic frustum (orthographic only).
  final double orthoScale;

  /// Min / max distance clamps.
  final double minDistance;
  final double maxDistance;

  /// Min / max pitch clamps.
  final double minPitch;
  final double maxPitch;

  /// The orbit target, defaulting to the origin when unset.
  Vector3 get effectiveTarget => target ?? Vector3.zero();

  /// True when projection is orthographic.
  bool get isOrthographic => projection == CameraProjection.orthographic;

  /// Field of view in degrees.
  double get fovYDegrees => fovYRadians * 180.0 / 3.141592653589793;

  CameraState copyWith({
    double? yaw,
    double? pitch,
    double? distance,
    Vector3? target,
    double? fovYRadians,
    double? near,
    double? far,
    CameraProjection? projection,
    double? orthoScale,
    double? minDistance,
    double? maxDistance,
    double? minPitch,
    double? maxPitch,
  }) =>
      CameraState(
        yaw: yaw ?? this.yaw,
        pitch: pitch ?? this.pitch,
        distance: distance ?? this.distance,
        target: target ?? this.target,
        fovYRadians: fovYRadians ?? this.fovYRadians,
        near: near ?? this.near,
        far: far ?? this.far,
        projection: projection ?? this.projection,
        orthoScale: orthoScale ?? this.orthoScale,
        minDistance: minDistance ?? this.minDistance,
        maxDistance: maxDistance ?? this.maxDistance,
        minPitch: minPitch ?? this.minPitch,
        maxPitch: maxPitch ?? this.maxPitch,
      );

  /// Returns a copy with the pitch clamped to [minPitch, maxPitch].
  CameraState clamped() => copyWith(
        pitch: pitch.clamp(minPitch, maxPitch),
        distance: distance.clamp(minDistance, maxDistance),
      );

  /// Resets to the default orbit pose.
  CameraState reset() => const CameraState();

  Map<String, dynamic> toJson() => {
        'yaw': yaw,
        'pitch': pitch,
        'distance': distance,
        'target': [
          effectiveTarget.x,
          effectiveTarget.y,
          effectiveTarget.z,
        ],
        'fovYRadians': fovYRadians,
        'near': near,
        'far': far,
        'projection': projection.name,
        'orthoScale': orthoScale,
        'minDistance': minDistance,
        'maxDistance': maxDistance,
        'minPitch': minPitch,
        'maxPitch': maxPitch,
      };

  @override
  bool operator ==(Object other) =>
      identical(this, other) ||
      other is CameraState &&
          other.yaw == yaw &&
          other.pitch == pitch &&
          other.distance == distance &&
          _vecEq(other.target, target) &&
          other.fovYRadians == fovYRadians &&
          other.near == near &&
          other.far == far &&
          other.projection == projection &&
          other.orthoScale == orthoScale &&
          other.minDistance == minDistance &&
          other.maxDistance == maxDistance &&
          other.minPitch == minPitch &&
          other.maxPitch == maxPitch;

  @override
  int get hashCode => Object.hash(
        yaw,
        pitch,
        distance,
        target == null ? 0 : Object.hash(target!.x, target!.y, target!.z),
        fovYRadians,
        near,
        far,
        projection,
        orthoScale,
        minDistance,
        maxDistance,
        minPitch,
        maxPitch,
      );
}

bool _vecEq(Vector3? a, Vector3? b) {
  if (a == null && b == null) return true;
  if (a == null || b == null) return false;
  return a.x == b.x && a.y == b.y && a.z == b.z;
}
