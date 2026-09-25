// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// orbit_camera.dart — Feather orbit (turntable) camera model.
//
// Pure 3D camera state, no Flutter bindings. Mirrors the navigation
// vocabulary documented in `interfaceandgestures_navigation.txt`:
//
//   * yaw / pitch / distance / target — the turntable pose.
//   * FOV expressed BOTH as an angle (radians) AND as a 35mm-equivalent
//     focal length in millimetres (Feather exposes 10 mm – 500 mm to the
//     user via the three-finger swipe gesture).
//   * An optional PINNED ORBIT POINT — the navigation centre. When pinned
//     the camera orbits a user-chosen world point instead of [target];
//     unpinning falls back to [target]. The orbit point is also the DOF
//     focus point.
//   * Perspective ⇄ orthographic toggle (three-finger double-tap).
//   * snapToView() — double-tap with one finger snaps to the nearest of
//     the six standard orthographic views (Front / Back / Left / Right /
//     Top / Bottom) as the docs require.
//
// The model is fully serializable so it can round-trip through a project
// file (the scene pipeline restores the exact framing on open).
//
// Depends on: lib/core/math/

import 'dart:math' as math;

import 'package:feather_krita/core/math/math.dart';

/// The six standard orthographic views the "double-tap to perfect view"
/// gesture snaps to (see interfaceandgestures_navigation.txt).
enum StandardView { front, back, left, right, top, bottom }

/// Projection mode — toggled by the three-finger double-tap gesture.
enum ProjectionMode { perspective, orthographic }

/// Configuration range for the FOV slider.
///
/// Feather exposes the field of view as a 35mm-equivalent focal length in
/// the 10 mm – 500 mm range (three-finger swipe up = wider FOV / shorter
/// focal length, swipe down = narrower FOV / longer focal length).
class FovRange {
  const FovRange({this.minMm = 10.0, this.maxMm = 500.0, this.defaultMm = 50.0});

  /// Shortest focal length / widest field of view.
  final double minMm;

  /// Longest focal length / narrowest field of view.
  final double maxMm;

  /// Focal length applied when the camera is first constructed.
  final double defaultMm;

  static const FovRange feather = FovRange();
}

/// An orbit (turntable) camera.
///
/// The camera orbits a centre point — either [target] or, when pinned,
/// [orbitPoint]. State is plain data; the gesture handler in
/// `camera_controller.dart` mutates these fields directly.
class OrbitCamera {
  OrbitCamera({
    Vector3? target,
    this.yaw = 0.0,
    this.pitch = -0.35,
    this.distance = 6.0,
    double focalLengthMm = 50.0,
    this.near = 0.05,
    this.far = 1000.0,
    this.projection = ProjectionMode.perspective,
    FovRange? fovRange,
    this.minDistance = 0.25,
    this.maxDistance = 400.0,
    this.minPitch = -1.55,
    this.maxPitch = 1.55,
  })  : _target = target ?? Vector3.zero(),
        _orbitPoint = null,
        _orbitPinned = false,
        _focalMm = focalLengthMm,
        fovRange = fovRange ?? FovRange.feather {
    _focalMm = clampDouble(_focalMm, this.fovRange.minMm, this.fovRange.maxMm);
  }

  // ----- Pose -----------------------------------------------------------

  /// Yaw (turntable rotation around the world Y axis) in radians.
  double yaw;

  /// Pitch (vertical tilt) in radians, clamped to [minPitch]/[maxPitch].
  double pitch;

  /// Distance from the orbit centre to the eye.
  double distance;

  /// The fallback orbit centre (used when no orbit point is pinned).
  Vector3 _target;
  Vector3 get target => _target;
  set target(Vector3 v) => _target = v.clone();

  /// The pinned orbit point — the navigation centre / DOF focus.
  Vector3? _orbitPoint;
  Vector3? get orbitPoint => _orbitPoint;
  bool _orbitPinned;
  bool get orbitPointPinned => _orbitPinned;

  /// Effective orbit centre: the pinned point if set, otherwise [target].
  Vector3 get orbitCenter =>
      (_orbitPinned && _orbitPoint != null) ? _orbitPoint! : _target;

  // ----- Projection -----------------------------------------------------

  ProjectionMode projection;
  final FovRange fovRange;

  /// 35mm-equivalent focal length in millimetres (Feather's user-facing
  /// FOV control). Internally converted to a vertical FOV angle.
  double _focalMm;
  double get focalLengthMm => _focalMm;

  /// Sets the focal length, clamping to [fovRange]. Returns the clamped
  /// value actually applied.
  double setFocalLengthMm(double mm) {
    _focalMm = clampDouble(mm, fovRange.minMm, fovRange.maxMm);
    return _focalMm;
  }

  /// Vertical field of view in radians, derived from the focal length.
  ///
  /// The 35mm-equivalent vertical FOV assumes a 24 mm-tall frame:
  ///   fovY = 2 * atan(24 / (2 * f))
  double get fovYRadians {
    final f = _focalMm <= 0 ? fovRange.defaultMm : _focalMm;
    return 2.0 * math.atan(24.0 / (2.0 * f));
  }

  double near;
  double far;

  final double minDistance;
  final double maxDistance;
  final double minPitch;
  final double maxPitch;

  // ----- Derived geometry ----------------------------------------------

  /// World-space eye position derived from the current pose.
  Vector3 get position {
    final cp = math.cos(pitch);
    return orbitCenter +
        Vector3(
          distance * cp * math.cos(yaw),
          distance * math.sin(pitch),
          distance * cp * math.sin(yaw),
        );
  }

  /// World-space forward (eye → orbit centre), normalized.
  Vector3 get forward => (orbitCenter - position)..normalize();

  /// World-space right vector.
  Vector3 get right {
    final f = forward;
    return f.cross(Vector3(0, 1, 0))..normalize();
  }

  /// World-space up vector.
  Vector3 get up => right.cross(forward)..normalize();

  /// The view (world → camera) matrix.
  Matrix4 get viewMatrix =>
      VectorMathUtils.lookAt(position, orbitCenter, Vector3(0, 1, 0));

  /// Perspective projection matrix for the given viewport aspect.
  Matrix4 _perspective(double aspect) =>
      VectorMathUtils.makeViewMatrix(fovYRadians, aspect, near, far);

  /// Orthographic projection matrix. The visible half-height is derived
  /// from the current distance so that zooming behaves consistently
  /// between the two projection modes.
  Matrix4 _orthographic(double aspect) {
    final halfH = distance * math.tan(fovYRadians * 0.5);
    final halfW = halfH * aspect;
    final m = Matrix4.zero();
    final inv = 1.0 / (far - near);
    m.setValues(
      1.0 / halfW, 0, 0, 0,
      0, 1.0 / halfH, 0, 0,
      0, 0, 2.0 * inv, 0,
      0, 0, -(far + near) * inv, 1,
    );
    return m;
  }

  /// Projection matrix for the given viewport aspect, honouring the
  /// current [projection] mode.
  Matrix4 projectionMatrix(double aspect) => projection == ProjectionMode.orthographic
      ? _orthographic(aspect)
      : _perspective(aspect);

  /// Combined view-projection matrix.
  Matrix4 viewProjectionMatrix(double aspect) =>
      projectionMatrix(aspect) * viewMatrix;

  // ----- Mutators -------------------------------------------------------

  /// Orbit (turntable) by [deltaYaw] / [deltaPitch] radians.
  void orbit(double deltaYaw, double deltaPitch) {
    yaw += deltaYaw;
    while (yaw > math.pi) {
      yaw -= 2 * math.pi;
    }
    while (yaw < -math.pi) {
      yaw += 2 * math.pi;
    }
    pitch = clampDouble(pitch + deltaPitch, minPitch, maxPitch);
  }

  /// Pan the orbit centre by a screen-space delta. The eye follows so the
  /// view translates without rotating.
  void pan(double dxPx, double dyPx, double viewportW, double viewportH) {
    final worldPerPixel =
        2.0 * distance * math.tan(fovYRadians * 0.5) / viewportH;
    final move = right * (-dxPx * worldPerPixel) + up * (dyPx * worldPerPixel);
    _target = _target + move;
    if (_orbitPinned && _orbitPoint != null) {
      _orbitPoint = _orbitPoint! + move;
    }
  }

  /// Zoom by a multiplicative factor on the current distance.
  void zoom(double factor) {
    distance = clampDouble(distance * factor, minDistance, maxDistance);
  }

  /// Pin the orbit point to a world position (tap-and-hold on a curve /
  /// grid per the docs). Also doubles as the DOF focus point.
  void setOrbitPoint(Vector3 point) {
    _orbitPoint = point.clone();
    _orbitPinned = true;
  }

  /// Unpin the orbit point (tap-and-hold in empty space per the docs).
  void clearOrbitPoint() {
    _orbitPoint = null;
    _orbitPinned = false;
  }

  /// Toggle the projection mode and return the new mode.
  ProjectionMode toggleProjection() {
    projection = projection == ProjectionMode.perspective
        ? ProjectionMode.orthographic
        : ProjectionMode.perspective;
    return projection;
  }

  /// Snap the camera to a named standard view.
  void snapToView(StandardView view) {
    switch (view) {
      case StandardView.front:
        yaw = 0.0;
        pitch = 0.0;
        break;
      case StandardView.back:
        yaw = math.pi;
        pitch = 0.0;
        break;
      case StandardView.left:
        yaw = -kHalfPi;
        pitch = 0.0;
        break;
      case StandardView.right:
        yaw = kHalfPi;
        pitch = 0.0;
        break;
      case StandardView.top:
        yaw = 0.0;
        pitch = kHalfPi * 0.999;
        break;
      case StandardView.bottom:
        yaw = 0.0;
        pitch = -kHalfPi * 0.999;
        break;
    }
  }

  /// Pick the closest of the six standard views to the current pose —
  /// used by the "double-tap to perfect view" gesture.
  StandardView nearestStandardView() {
    var best = StandardView.front;
    var bestScore = double.infinity;
    for (final v in StandardView.values) {
      final score = viewDistanceTo(v);
      if (score < bestScore) {
        bestScore = score;
        best = v;
      }
    }
    return best;
  }

  /// A scalar "distance" between the current pose and [view] (radians of
  /// yaw + pitch combined). Used by the "double-tap to perfect view"
  /// gesture to pick the closest snap target, and by the UI to decide
  /// whether the current framing reads as a named standard view.
  double viewDistanceTo(StandardView view) {
    final (ty, tp) = switch (view) {
      StandardView.front => (0.0, 0.0),
      StandardView.back => (math.pi, 0.0),
      StandardView.left => (-kHalfPi, 0.0),
      StandardView.right => (kHalfPi, 0.0),
      StandardView.top => (0.0, kHalfPi * 0.999),
      StandardView.bottom => (0.0, -kHalfPi * 0.999),
    };
    var dy = (yaw - ty).abs();
    if (dy > math.pi) dy = 2 * math.pi - dy;
    return dy + (pitch - tp).abs();
  }

  // ----- Serialization --------------------------------------------------

  Map<String, dynamic> toJson() => {
        'target': [_target.x, _target.y, _target.z],
        'yaw': yaw,
        'pitch': pitch,
        'distance': distance,
        'focalMm': _focalMm,
        'near': near,
        'far': far,
        'projection': projection.name,
        'orbitPoint': _orbitPoint == null
            ? null
            : [_orbitPoint!.x, _orbitPoint!.y, _orbitPoint!.z],
        'orbitPinned': _orbitPinned,
      };

  factory OrbitCamera.fromJson(Map<String, dynamic> json) {
    final t = (json['target'] as List).cast<num>();
    final cam = OrbitCamera(
      target: Vector3(t[0].toDouble(), t[1].toDouble(), t[2].toDouble()),
      yaw: (json['yaw'] as num?)?.toDouble() ?? 0.0,
      pitch: (json['pitch'] as num?)?.toDouble() ?? -0.35,
      distance: (json['distance'] as num?)?.toDouble() ?? 6.0,
      focalLengthMm: (json['focalMm'] as num?)?.toDouble() ?? 50.0,
      near: (json['near'] as num?)?.toDouble() ?? 0.05,
      far: (json['far'] as num?)?.toDouble() ?? 1000.0,
      projection: ProjectionMode.values.firstWhere(
        (m) => m.name == (json['projection'] as String? ?? 'perspective'),
        orElse: () => ProjectionMode.perspective,
      ),
    );
    if (json['orbitPinned'] == true) {
      final op = (json['orbitPoint'] as List).cast<num>();
      cam.setOrbitPoint(
          Vector3(op[0].toDouble(), op[1].toDouble(), op[2].toDouble()));
    }
    return cam;
  }

  OrbitCamera copy() => OrbitCamera.fromJson(toJson());
}
