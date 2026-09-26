// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// gesture_detector.dart — multi-touch gesture recognizer for Feather.
//
// ── STATUS: SCAFFOLD — NOT YET WIRED (v0.54-D) ──────────────────────────
// This recognizer is fully implemented and unit-testable in isolation, but
// it is NOT mounted in the live canvas. The production canvas viewport
// (lib/ui/widgets/canvas_viewport.dart) uses Flutter's stock `GestureDetector`
// for the draw / pan / zoom / orbit flows. Wiring this detector as a
// replacement is a larger lift than the v0.54-D Apple-Pencil task and
// would risk regressing the live draw / liquify / orbit paths (which the
// stock GestureDetector + the viewport's custom `Listener` together own).
//
// The detector's value-adds — stylus-vs-finger disambiguation, 3-finger
// FOV / projection-toggle, tap-and-hold resolve-orbit — remain
// forward-looking. The Apple Pencil wiring (double-tap / squeeze / tilt)
// added in v0.54-D does NOT depend on this detector: tilt is fed from
// Flutter's own stylus `PointerEvent` stream (see
// `_feedStylusSample` in canvas_viewport.dart) and the hardware
// double-tap / squeeze arrive via the native iOS EventChannel
// (apple_pencil_channel.dart). When palm-rejection / stylus-finger
// disambiguation becomes a priority, mount a `Listener` that maps
// `PointerEvent`s into `GesturePointer`s and dispatch the `GestureEvent`
// stream from the host — but that is a separate, dedicated task.
// ─────────────────────────────────────────────────────────────────────────
//
// Sits between the raw pointer stream and the camera / drawing pipelines.
// It classifies the active pointer set into one of the documented
// gesture families and emits typed [GestureEvent]s the host can dispatch:
//
//   1 finger  + stylus / finger-pen  → draw strokes (not handled here —
//                                       the host wires draw events to the
//                                       stroke manager).
//   1 finger  + touch                → orbit (turntable) — but the
//                                       Feather docs also describe this
//                                       as the drawing finger, so this
//                                       detector reports it as a
//                                       [DrawEvent] AND the host decides
//                                       whether to treat it as orbit
//                                       based on the active tool.
//   2 fingers                       → pinch-zoom + pinch-swipe-pan.
//   3 fingers, double-tap           → toggle perspective / orthographic.
//   3 fingers, vertical swipe       → FOV adjustment.
//
// The detector is UI-framework agnostic — the host feeds it
// [GesturePointer]s via [pointerDown]/[pointerMove]/[pointerUp] and
// reads events from the [events] sink. The Flutter binding is a thin
// `Listener` widget that maps `PointerEvent`s into [GesturePointer]s.
//
// Depends on: lib/core/math/

import 'dart:async';
import 'dart:math' as math;

import 'package:feather_krita/core/math/math.dart';

/// A tracked pointer, framework-agnostic.
class GesturePointer {
  GesturePointer({
    required this.id,
    required this.position,
    required this.timestamp,
    required this.kind,
    this.pressure = 0.0,
    Vector2? tilt,
    this.azimuth = 0.0,
    this.altitude = 0.0,
    this.twist = 0.0,
  }) : tilt = tilt ?? Vector2.zero();

  final int id;
  final PointerKind kind;
  final Duration timestamp;
  final double pressure;
  final Vector2 tilt;
  final double azimuth;
  final double altitude;
  final double twist;
  GesturePosition position;
}

/// Position record shared with the camera controller.
class GesturePosition {
  const GesturePosition(this.x, this.y);
  final double x;
  final double y;
  GesturePosition operator -(GesturePosition o) =>
      GesturePosition(x - o.x, y - o.y);
  double get distance => math.sqrt(x * x + y * y);
  static const GesturePosition zero = GesturePosition(0, 0);
}

enum PointerKind { touch, stylus, mouse }

/// Tagged union of recognized gestures.
sealed class GestureEvent {
  const GestureEvent();
}

/// A drawing sample (1-finger with stylus / finger-pen).
class DrawEvent extends GestureEvent {
  const DrawEvent({
    required this.pointerId,
    required this.position,
    required this.pressure,
    required this.tilt,
    required this.azimuth,
    required this.altitude,
    required this.twist,
    required this.down,
  });
  final int pointerId;
  final GesturePosition position;
  final double pressure;
  final Vector2 tilt;
  final double azimuth;
  final double altitude;
  final double twist;
  final bool down;
}

/// Orbit (turntable) request — fired for 1-finger touch swipes that the
/// host has decided should navigate (e.g. when no draw tool is active).
class OrbitEvent extends GestureEvent {
  const OrbitEvent(this.deltaXDx, this.deltaYDy);
  final double deltaXDx;
  final double deltaYDy;
}

/// Pinch-zoom — multiplicative ratio on the camera distance.
class ZoomEvent extends GestureEvent {
  const ZoomEvent(this.factor);
  final double factor;
}

/// Pinch-swipe-pan — cooperative two-finger drag.
class PanEvent extends GestureEvent {
  const PanEvent(this.dxPx, this.dyPx);
  final double dxPx;
  final double dyPx;
}

/// Three-finger vertical swipe — FOV adjustment in millimetres.
class FovEvent extends GestureEvent {
  const FovEvent(this.deltaMm);
  final double deltaMm;
}

/// One-finger double-tap — snap to nearest standard view.
class SnapViewEvent extends GestureEvent {
  const SnapViewEvent();
}

/// Three-finger double-tap — toggle perspective / orthographic.
class ToggleProjectionEvent extends GestureEvent {
  const ToggleProjectionEvent();
}

/// Tap-and-hold — request the host to resolve the orbit point.
class TapHoldEvent extends GestureEvent {
  const TapHoldEvent(this.position);
  final GesturePosition position;
}

/// Configuration knobs for the recognizer.
class GestureDetectorConfig {
  const GestureDetectorConfig({
    this.doubleTapMaxIntervalMs = 280,
    this.doubleTapMaxDistancePx = 24,
    this.threeFingerDoubleTapMaxIntervalMs = 560,
    this.threeFingerMaxDriftPx = 18,
    this.fovMmPerPixel = 2.0,
    this.orbitSensitivity = 0.008,
    this.tapHoldDurationMs = 320,
    this.tapHoldMaxDriftPx = 10,
  });

  final int doubleTapMaxIntervalMs;
  final double doubleTapMaxDistancePx;
  final int threeFingerDoubleTapMaxIntervalMs;
  final double threeFingerMaxDriftPx;
  final double fovMmPerPixel;
  final double orbitSensitivity;
  final int tapHoldDurationMs;
  final double tapHoldMaxDriftPx;
}

/// The recognizer.
class FeatherGestureDetector {
  FeatherGestureDetector({
    this.config = const GestureDetectorConfig(),
    this.stylusDraws = true,
  });

  final GestureDetectorConfig config;

  /// When true, a single stylus contact produces [DrawEvent]s rather
  /// than orbit events (the Feather default: "navigate with your fingers,
  /// draw with your pen").
  final bool stylusDraws;

  final StreamController<GestureEvent> _controller =
      StreamController<GestureEvent>.broadcast();
  Stream<GestureEvent> get events => _controller.stream;

  final Map<int, GesturePointer> _pointers = {};
  final Map<int, GesturePointer> _initial = {};
  double _lastPinchDistance = 0;
  GesturePosition? _lastCentroid;
  GesturePosition? _lastTapPos;
  Duration? _lastTapTime;
  int _threeFingerTaps = 0;
  Duration? _threeFingerFirstTap;
  GesturePosition? _tapHoldAnchor;
  Duration? _tapHoldStart;
  bool _tapHoldFired = false;

  bool get hasPointers => _pointers.isNotEmpty;
  int get pointerCount => _pointers.length;

  void pointerDown(GesturePointer p) {
    _pointers[p.id] = p;
    _initial[p.id] = p;
    if (_pointers.length == 1) {
      _tapHoldStart = p.timestamp;
      _tapHoldAnchor = p.position;
      _tapHoldFired = false;
    } else {
      _tapHoldStart = null;
      _tapHoldAnchor = null;
    }
    if (_pointers.length == 2) {
      _lastPinchDistance = _pinchDistance();
      _lastCentroid = _centroid();
    }
    if (_isStylusDraw(p)) {
      _controller.add(DrawEvent(
        pointerId: p.id,
        position: p.position,
        pressure: p.pressure,
        tilt: p.tilt,
        azimuth: p.azimuth,
        altitude: p.altitude,
        twist: p.twist,
        down: true,
      ));
    }
  }

  void pointerMove(GesturePointer p) {
    final prev = _pointers[p.id];
    if (prev == null) return;
    _pointers[p.id] = p;

    // Cancel tap-and-hold on drift.
    if (_pointers.length == 1 && _tapHoldAnchor != null && !_tapHoldFired) {
      if ((p.position - _tapHoldAnchor!).distance >
          config.tapHoldMaxDriftPx) {
        _tapHoldStart = null;
        _tapHoldAnchor = null;
      }
    }

    switch (_pointers.length) {
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

  void pointerUp(GesturePointer p) {
    final was = _pointers.length;
    if (_isStylusDraw(p)) {
      _controller.add(DrawEvent(
        pointerId: p.id,
        position: p.position,
        pressure: 0.0,
        tilt: p.tilt,
        azimuth: p.azimuth,
        altitude: p.altitude,
        twist: p.twist,
        down: false,
      ));
    }
    _pointers.remove(p.id);
    _initial.remove(p.id);
    if (was == 1) {
      final drift = _tapHoldAnchor == null
          ? 0.0
          : (p.position - _tapHoldAnchor!).distance;
      if (!_tapHoldFired && drift <= config.doubleTapMaxDistancePx) {
        _handleOneFingerTap(p);
      }
      _tapHoldStart = null;
      _tapHoldAnchor = null;
    } else if (was == 3 && _pointers.length < 3) {
      _maybeThreeFingerTap(p.timestamp);
    }
  }

  void dispose() {
    _controller.close();
  }

  // ----- Per-count handlers --------------------------------------------

  void _handleOneFingerMove(GesturePointer prev, GesturePointer now) {
    final delta = now.position - prev.position;
    if (_isStylusDraw(now)) {
      _controller.add(DrawEvent(
        pointerId: now.id,
        position: now.position,
        pressure: now.pressure,
        tilt: now.tilt,
        azimuth: now.azimuth,
        altitude: now.altitude,
        twist: now.twist,
        down: true,
      ));
      return;
    }
    // Touch one-finger: the Feather docs treat this as the rotate swipe,
    // but the host may re-interpret it as drawing when Finger-Pen is on.
    _controller
        .add(OrbitEvent(delta.x * config.orbitSensitivity, delta.y * config.orbitSensitivity));
  }

  void _handleTwoFingerMove() {
    final pinch = _pinchDistance();
    final centroid = _centroid();
    if (pinch > 0 && _lastPinchDistance > 0) {
      final ratio = pinch / _lastPinchDistance;
      if ((ratio - 1.0).abs() > 1e-3) {
        _controller.add(ZoomEvent(1.0 / ratio));
      }
    }
    if (_lastCentroid != null) {
      final pan = centroid - _lastCentroid!;
      if (pan.distance > 0.5) {
        _controller.add(PanEvent(pan.x, pan.y));
      }
    }
    _lastPinchDistance = pinch;
    _lastCentroid = centroid;
  }

  void _handleThreeFingerMove() {
    final v = _averageVerticalDelta();
    if (v.abs() < 0.5) return;
    _controller.add(FovEvent(v * config.fovMmPerPixel));
  }

  double _averageVerticalDelta() {
    var sum = 0.0;
    var n = 0;
    for (final e in _pointers.entries) {
      final init = _initial[e.key];
      if (init == null) continue;
      sum += e.value.position.y - init.position.y;
      n++;
    }
    return n == 0 ? 0 : sum / n;
  }

  void _handleOneFingerTap(GesturePointer p) {
    final now = p.timestamp;
    if (_lastTapPos != null &&
        _lastTapTime != null &&
        (now - _lastTapTime!).inMilliseconds <=
            config.doubleTapMaxIntervalMs &&
        (p.position - _lastTapPos!).distance <=
            config.doubleTapMaxDistancePx) {
      _controller.add(const SnapViewEvent());
      _lastTapPos = null;
      _lastTapTime = null;
      return;
    }
    _lastTapPos = p.position;
    _lastTapTime = now;
  }

  void _maybeThreeFingerTap(Duration now) {
    // Require minimal drift across all three pointers.
    for (final e in _pointers.entries) {
      final init = _initial[e.key];
      if (init == null) continue;
      if ((e.value.position - init.position).distance >
          config.threeFingerMaxDriftPx) {
        _threeFingerTaps = 0;
        _threeFingerFirstTap = null;
        return;
      }
    }
    if (_threeFingerFirstTap == null) {
      _threeFingerFirstTap = now;
      _threeFingerTaps = 1;
      return;
    }
    if ((now - _threeFingerFirstTap!).inMilliseconds <=
        config.threeFingerDoubleTapMaxIntervalMs) {
      _threeFingerTaps++;
      if (_threeFingerTaps >= 2) {
        _controller.add(const ToggleProjectionEvent());
        _threeFingerTaps = 0;
        _threeFingerFirstTap = null;
      }
    } else {
      _threeFingerFirstTap = now;
      _threeFingerTaps = 1;
    }
  }

  /// Called per frame by the host to fire the tap-and-hold event once
  /// the threshold elapses.
  void tick(Duration now) {
    if (_tapHoldStart == null || _tapHoldFired || _tapHoldAnchor == null) {
      return;
    }
    if ((now - _tapHoldStart!).inMilliseconds >= config.tapHoldDurationMs) {
      _tapHoldFired = true;
      _controller.add(TapHoldEvent(_tapHoldAnchor!));
    }
  }

  // ----- Helpers -------------------------------------------------------

  bool _isStylusDraw(GesturePointer p) =>
      stylusDraws && p.kind == PointerKind.stylus;

  double _pinchDistance() {
    final pts = _pointers.values.toList();
    if (pts.length < 2) return 0;
    return (pts[0].position - pts[1].position).distance;
  }

  GesturePosition _centroid() {
    if (_pointers.isEmpty) return GesturePosition.zero;
    var x = 0.0, y = 0.0;
    for (final p in _pointers.values) {
      x += p.position.x;
      y += p.position.y;
    }
    return GesturePosition(x / _pointers.length, y / _pointers.length);
  }

  /// Reset all transient tracking.
  void reset() {
    _pointers.clear();
    _initial.clear();
    _lastPinchDistance = 0;
    _lastCentroid = null;
    _lastTapPos = null;
    _lastTapTime = null;
    _threeFingerTaps = 0;
    _threeFingerFirstTap = null;
    _tapHoldAnchor = null;
    _tapHoldStart = null;
    _tapHoldFired = false;
  }
}
