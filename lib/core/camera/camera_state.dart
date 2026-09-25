// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// camera_state.dart — observable camera state for the Feather UI.
//
// Wraps an [OrbitCamera] in a [ChangeNotifier] so widgets can rebuild when
// the user navigates. Exposes the navigation-relevant flags called out in
// the docs:
//
//   * current view label (the nearest standard view / "Free"),
//   * projection mode (perspective ⇄ orthographic, toggled by the
//     three-finger double-tap gesture),
//   * orbit-point pinned flag (tap-and-hold to pin / unpin),
//   * focal length in mm (the three-finger swipe FOV control),
//
// plus a small history for the "reset view" fallback the docs describe:
// "If the orbit point is not pinned, tap-and-hold in empty space to reset
// the view."
//
// Depends on: lib/core/math/, lib/core/camera/orbit_camera.dart

import 'package:flutter/foundation.dart';

import 'package:feather_krita/core/camera/orbit_camera.dart';
import 'package:feather_krita/core/math/math.dart';

/// Human-readable label for the current framing.
enum ViewLabel { free, front, back, left, right, top, bottom }

extension ViewLabelX on ViewLabel {
  String get display => switch (this) {
        ViewLabel.free => 'Free',
        ViewLabel.front => 'Front',
        ViewLabel.back => 'Back',
        ViewLabel.left => 'Left',
        ViewLabel.right => 'Right',
        ViewLabel.top => 'Top',
        ViewLabel.bottom => 'Bottom',
      };

  static ViewLabel fromStandard(StandardView v) => switch (v) {
        StandardView.front => ViewLabel.front,
        StandardView.back => ViewLabel.back,
        StandardView.left => ViewLabel.left,
        StandardView.right => ViewLabel.right,
        StandardView.top => ViewLabel.top,
        StandardView.bottom => ViewLabel.bottom,
      };
}

/// Observable wrapper around [OrbitCamera].
class CameraState extends ChangeNotifier {
  CameraState({OrbitCamera? camera}) : _camera = camera ?? OrbitCamera() {
    _saveResetBookmark();
  }

  OrbitCamera _camera;
  OrbitCamera get camera => _camera;

  /// Replace the underlying camera (e.g. when loading a project).
  void replaceWith(OrbitCamera next) {
    _camera = next;
    _saveResetBookmark();
    notifyListeners();
  }

  // ----- Derived, UI-facing properties ---------------------------------

  /// The projection mode the three-finger double-tap toggles.
  ProjectionMode get projection => _camera.projection;
  bool get isOrthographic => _camera.projection == ProjectionMode.orthographic;

  /// Whether a tap-and-hold has pinned the orbit point.
  bool get orbitPointPinned => _camera.orbitPointPinned;
  Vector3? get orbitPoint => _camera.orbitPoint;

  /// The user-facing focal length (mm).
  double get focalLengthMm => _camera.focalLengthMm;

  /// The nearest standard view, or [ViewLabel.free] when the user has
  /// rotated away from any of the six snaps.
  ViewLabel get currentView {
    final nearest = _camera.nearestStandardView();
    return _camera.viewDistanceTo(nearest) < 0.02
        ? ViewLabelX.fromStandard(nearest)
        : ViewLabel.free;
  }

  // ----- Mutators (each calls notifyListeners) -------------------------

  void orbit(double dy, double dp) {
    _camera.orbit(dy, dp);
    notifyListeners();
  }

  void pan(double dxPx, double dyPx, double vw, double vh) {
    _camera.pan(dxPx, dyPx, vw, vh);
    notifyListeners();
  }

  void zoom(double factor) {
    _camera.zoom(factor);
    notifyListeners();
  }

  /// Three-finger swipe FOV control: a positive [deltaMm] widens the FOV
  /// (shorter focal length), a negative one narrows it.
  void adjustFocalLength(double deltaMm) {
    _camera.setFocalLengthMm(_camera.focalLengthMm - deltaMm);
    notifyListeners();
  }

  /// Pin the orbit point (tap-and-hold on a curve / grid).
  void pinOrbitPoint(Vector3 point) {
    _camera.setOrbitPoint(point);
    _saveResetBookmark();
    notifyListeners();
  }

  /// Unpin the orbit point (tap-and-hold in empty space).
  void unpinOrbitPoint() {
    _camera.clearOrbitPoint();
    notifyListeners();
  }

  /// Toggle perspective ⇄ orthographic (three-finger double-tap).
  ProjectionMode toggleProjection() {
    final next = _camera.toggleProjection();
    notifyListeners();
    return next;
  }

  /// One-finger double-tap: snap to the nearest standard view.
  StandardView snapToNearestView() {
    final v = _camera.nearestStandardView();
    _camera.snapToView(v);
    notifyListeners();
    return v;
  }

  /// Snap to an explicit standard view (used by the view menu).
  void snapToView(StandardView view) {
    _camera.snapToView(view);
    notifyListeners();
  }

  // ----- Reset-view bookmark -------------------------------------------

  late OrbitCamera _resetBookmark;

  void _saveResetBookmark() {
    _resetBookmark = _camera.copy();
  }

  /// "If the orbit point is not pinned, tap-and-hold in empty space to
  /// reset the view" — restores the bookmarked framing captured the last
  /// time the orbit point was pinned (or at construction).
  void resetView() {
    final pinned = _camera.orbitPointPinned;
    _camera = _resetBookmark.copy();
    if (!pinned) {
      _camera.clearOrbitPoint();
    }
    notifyListeners();
  }
}
