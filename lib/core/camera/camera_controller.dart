// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// camera_controller.dart — touch gesture handler for the Feather orbit
// camera.
//
// Translates the raw multi-touch vocabulary documented in
// `interfaceandgestures_navigation.txt` into [OrbitCamera] mutations,
// surfaced through [CameraState]:
//
//   * Swipe (one finger)             → orbit (yaw / pitch).
//   * Double tap (one finger)        → snap to the nearest standard view.
//   * Pinch (two fingers)            → zoom.
//   * Pinch-swipe (two fingers)      → pan.
//   * Three-finger double tap        → toggle perspective / orthographic.
//   * Three-finger swipe (up / down) → adjust FOV (10 mm – 500 mm).
//   * Tap-and-hold                   → pin / unpin orbit point, or reset.
//
// The controller is intentionally UI-framework-agnostic at the input level:
// callers feed it normalized pointer records via [onPointerDown] /
// [onPointerMove] / [onPointerUp] and it mutates the [state]. The widget
// layer (a Flutter `Listener` / `RawGestureDetector`) is responsible for
// the actual pointer plumbing — see `gesture_detector.dart`.
//
// Depends on: lib/core/math/, lib/core/camera/

import 'dart:math' as math;

import 'package:feather_krita/core/camera/camera_state.dart';

/// A snapshot of one tracked pointer.
class PointerRecord {
  PointerRecord({
    required this.id,
    required this.position,
    required this.timestamp,
    this.kind = PointerKind.touch,
  });

  final int id;
  Offset2 position;
  final Duration timestamp;
  final PointerKind kind;

  Offset2 deltaFrom(PointerRecord other) => position - other.position;
}

/// Pointer source — used to keep the Apple Pencil draw path separate from
/// finger navigation.
enum PointerKind { touch, stylus, mouse }

/// A minimal 2D point alias so this module stays decoupled from
/// `dart:ui` (which `Offset` comes from). The interaction layer maps the
/// framework's `Offset` into this.
class Offset2 {
  const Offset2(this.dx, this.dy);
  final double dx;
  final double dy;
  Offset2 operator -(Offset2 o) => Offset2(dx - o.dx, dy - o.dy);
  Offset2 operator +(Offset2 o) => Offset2(dx + o.dx, dy + o.dy);
  double get distance => math.sqrt(dx * dx + dy * dy);
  double get distanceSquared => dx * dx + dy * dy;
  static const Offset2 zero = Offset2(0, 0);
}

/// Callback fired when a tap-and-hold is recognised — the caller is
/// expected to ray-cast the screen position and either pin the orbit
/// point (hit a curve / grid) or unpin it (hit empty space). The returned
/// future lets the handler await the hit-test before reporting.
typedef OrbitPointResolver = Future<bool> Function(Offset2 screenPos);

/// Touch gesture handler that drives a [CameraState].
class CameraGestureController {
  CameraGestureController(
    this.state, {
    this.viewportSize = const Size2(1024, 768),
    this.orbitSensitivity = 0.008,
    this.panSensitivity = 1.0,
    this.zoomSensitivity = 0.004,
    this.fovMmPerPixel = 2.0,
    this.doubleTapMaxIntervalMs = 280,
    this.doubleTapMaxDistancePx = 24,
    this.tapHoldDurationMs = 320,
    this.tapHoldMaxDriftPx = 10,
    this.resolveOrbitPoint,
  });

  final CameraState state;
  Size2 viewportSize;

  /// Radians per pixel of one-finger swipe.
  double orbitSensitivity;

  /// Multiplier on the pan speed.
  double panSensitivity;

  /// Log-scale zoom sensitivity per pixel of pinch delta.
  double zoomSensitivity;

  /// mm of focal-length change per pixel of three-finger swipe.
  double fovMmPerPixel;

  /// Double-tap timing / distance thresholds.
  final int doubleTapMaxIntervalMs;
  final double doubleTapMaxDistancePx;

  /// Tap-and-hold timing / drift thresholds.
  final int tapHoldDurationMs;
  final double tapHoldMaxDriftPx;

  /// Optional hit-test resolver used by the tap-and-hold gesture.
  final OrbitPointResolver? resolveOrbitPoint;

  final Map<int, PointerRecord> _pointers = {};
  final Map<int, PointerRecord> _initial = {};
  int _activeCount = 0;
  Offset2 _lastPanCentroid = Offset2.zero;
  double _lastPinchDistance = 0;
  Offset2? _lastTapPos;
  Duration? _lastTapTime;
  int _threeFingerTaps = 0;
  Duration? _threeFingerFirstTap;
  Duration? _tapHoldStart;
  Offset2? _tapHoldAnchor;
  bool _tapHoldFired = false;

  // ----- Viewport plumbing ---------------------------------------------

  /// Called when a pointer goes down.
  void onPointerDown(PointerRecord p) {
    _pointers[p.id] = p;
    _initial[p.id] = p;
    _activeCount = _pointers.length;
    if (_activeCount == 1) {
      _tapHoldStart = p.timestamp;
      _tapHoldAnchor = p.position;
      _tapHoldFired = false;
    } else {
      _tapHoldStart = null;
      _tapHoldAnchor = null;
    }
    if (_activeCount == 2) {
      _lastPinchDistance = _currentPinchDistance();
      _lastPanCentroid = _currentCentroid();
    }
  }

  /// Called when a pointer moves.
  void onPointerMove(PointerRecord p) {
    final prev = _pointers[p.id];
    if (prev == null) return;
    _pointers[p.id] = p;

    // Tap-and-hold drift cancellation for one-finger.
    if (_activeCount == 1 && _tapHoldAnchor != null && !_tapHoldFired) {
      final drift = (p.position - _tapHoldAnchor!).distance;
      if (drift > tapHoldMaxDriftPx) {
        _tapHoldStart = null;
        _tapHoldAnchor = null;
      }
    }

    switch (_activeCount) {
      case 1:
        _handleOneFingerMove(prev, p);
        break;
      case 2:
        _handleTwoFingerMove();
        break;
      case 3:
        _handleThreeFingerMove();
        break;
    }
  }

  /// Called when a pointer goes up.
  void onPointerUp(PointerRecord p) {
    final wasCount = _activeCount;
    _pointers.remove(p.id);
    _initial.remove(p.id);
    _activeCount = _pointers.length;

    if (wasCount == 1) {
      // Maybe a tap (or the second tap of a double tap).
      final drift =
          _tapHoldAnchor == null ? 0.0 : (p.position - _tapHoldAnchor!).distance;
      if (!_tapHoldFired && drift <= doubleTapMaxDistancePx) {
        _handleOneFingerTap(p);
      }
      _tapHoldStart = null;
      _tapHoldAnchor = null;
    } else if (wasCount == 3 && _activeCount < 3) {
      // Three-finger tap detection happens on the lift of the third
      // finger, gated by minimal total drift.
      _maybeThreeFingerTap(p.timestamp);
    }
  }

  // ----- Per-count handlers --------------------------------------------

  void _handleOneFingerMove(PointerRecord prev, PointerRecord now) {
    final delta = now.position - prev.position;
    state.orbit(delta.dx * orbitSensitivity, delta.dy * orbitSensitivity);
  }

  void _handleTwoFingerMove() {
    final pinch = _currentPinchDistance();
    final centroid = _currentCentroid();
    final panDelta = centroid - _lastPanCentroid;

    // Pinch zoom — multiplicative on the ratio of distances.
    if (pinch > 0 && _lastPinchDistance > 0) {
      final ratio = pinch / _lastPinchDistance;
      if ((ratio - 1.0).abs() > 1e-3) {
        state.zoom(1.0 / ratio);
      }
    }
    // Pinch-swipe pan — a cooperative drag of both fingers.
    if (panDelta.distance > 0.5) {
      state.pan(panDelta.dx * panSensitivity, panDelta.dy * panSensitivity,
          viewportSize.width, viewportSize.height);
    }
    _lastPinchDistance = pinch;
    _lastPanCentroid = centroid;
  }

  void _handleThreeFingerMove() {
    // Vertical three-finger swipe drives the FOV (mm). Horizontal swipe
    // is ignored by the docs.
    final vertical = _averageVerticalDelta();
    if (vertical.abs() < 0.5) return;
    // Swipe up → increase FOV → decrease focal length.
    state.adjustFocalLength(vertical * fovMmPerPixel);
  }

  double _averageVerticalDelta() {
    var sum = 0.0;
    var n = 0;
    for (final entry in _pointers.entries) {
      final init = _initial[entry.key];
      if (init == null) continue;
      sum += entry.value.position.dy - init.position.dy;
      n++;
    }
    return n == 0 ? 0 : sum / n;
  }

  // ----- Tap / hold detection ------------------------------------------

  void _handleOneFingerTap(PointerRecord p) {
    final now = p.timestamp;
    if (_lastTapPos != null &&
        _lastTapTime != null &&
        (now - _lastTapTime!).inMilliseconds <= doubleTapMaxIntervalMs &&
        (p.position - _lastTapPos!).distance <= doubleTapMaxDistancePx) {
      // Double tap → snap to nearest standard view.
      state.snapToNearestView();
      _lastTapPos = null;
      _lastTapTime = null;
      return;
    }
    _lastTapPos = p.position;
    _lastTapTime = now;
  }

  void _maybeThreeFingerTap(Duration now) {
    if (_threeFingerFirstTap == null) {
      _threeFingerFirstTap = now;
      _threeFingerTaps = 1;
      return;
    }
    if ((now - _threeFingerFirstTap!).inMilliseconds <=
        doubleTapMaxIntervalMs * 2) {
      _threeFingerTaps++;
      if (_threeFingerTaps >= 2) {
        state.toggleProjection();
        _threeFingerTaps = 0;
        _threeFingerFirstTap = null;
      }
    } else {
      _threeFingerFirstTap = now;
      _threeFingerTaps = 1;
    }
  }

  /// Drives the tap-and-hold gesture. Called every frame by the host
  /// widget; fires the orbit-point resolver once the hold threshold
  /// elapses.
  Future<void> tick(Duration now) async {
    if (_tapHoldStart == null || _tapHoldFired || _tapHoldAnchor == null) {
      return;
    }
    if ((now - _tapHoldStart!).inMilliseconds >= tapHoldDurationMs) {
      _tapHoldFired = true;
      final resolver = resolveOrbitPoint;
      if (resolver == null) return;
      final pinned = await resolver(_tapHoldAnchor!);
      if (pinned) {
        // The resolver is expected to have already pinned the point.
      } else {
        state.unpinOrbitPoint();
        // Per docs: tap-and-hold in empty space, with no pin, resets view.
        if (!state.orbitPointPinned) state.resetView();
      }
    }
  }

  // ----- Geometry helpers ----------------------------------------------

  double _currentPinchDistance() {
    final pts = _pointers.values.toList();
    if (pts.length < 2) return 0;
    return (pts[0].position - pts[1].position).distance;
  }

  Offset2 _currentCentroid() {
    if (_pointers.isEmpty) return Offset2.zero;
    var x = 0.0, y = 0.0;
    for (final p in _pointers.values) {
      x += p.position.dx;
      y += p.position.dy;
    }
    return Offset2(x / _pointers.length, y / _pointers.length);
  }

  /// Reset all transient gesture tracking (e.g. on window focus loss).
  void reset() {
    _pointers.clear();
    _initial.clear();
    _activeCount = 0;
    _lastPanCentroid = Offset2.zero;
    _lastPinchDistance = 0;
    _lastTapPos = null;
    _lastTapTime = null;
    _threeFingerTaps = 0;
    _threeFingerFirstTap = null;
    _tapHoldStart = null;
    _tapHoldAnchor = null;
    _tapHoldFired = false;
  }
}

/// Lightweight viewport size record.
class Size2 {
  const Size2(this.width, this.height);
  final double width;
  final double height;
}
