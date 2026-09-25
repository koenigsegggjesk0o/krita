// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// apple_pencil_handler.dart — Apple Pencil event bridge.
//
// Normalises the Apple-Pencil-specific signals documented in
// `interfaceandgestures_applepencil.txt`:
//
//   * Pressure, tilt, azimuth, altitude (from stylus pointer events).
//   * Barrel-roll twist (Apple Pencil Pro).
//   * Squeeze gesture (Apple Pencil Pro) → opens the Squeeze Menu by
//     default, or the user's customized action.
//   * Double-tap on the Pencil body (Pro + 2nd Gen) → toggles within the
//     active tool's pair (Draw ⇄ Draw Shape, Erase ⇄ Vacuum, Select ⇄
//     Deselect).
//   * Squeeze-drag → temporary tool switch within the pair while the pen
//     is down.
//
// The host platform feeds the handler through [onStylusSample],
// [onSqueeze], [onSqueezeRelease], [onDoubleTap]. The handler mutates the
// shared [InputState] and emits high-level [PencilAction]s the UI can
// subscribe to (e.g. to open the Squeeze Menu).
//
// Depends on: lib/core/math/, lib/core/interaction/input_state.dart

import 'dart:async';

import 'package:feather_krita/core/interaction/input_state.dart';
import 'package:feather_krita/core/math/math.dart';

/// High-level Pencil actions the UI may want to react to.
sealed class PencilAction {
  const PencilAction();
}

class SqueezeAction extends PencilAction {
  const SqueezeAction(this.position);
  final Vector2? position;
}

class SqueezeReleaseAction extends PencilAction {
  const SqueezeReleaseAction();
}

class DoubleTapAction extends PencilAction {
  const DoubleTapAction();
}

/// What the squeeze / double-tap hardware gestures are wired to.
enum PencilGestureBinding {
  /// Default for squeeze — open the radial Squeeze Menu.
  squeezeMenu,

  /// Default for double-tap — flip within the active tool's pair.
  toggleToolPair,

  /// Open the brush Injector (a Feather brush feature).
  injector,
}

/// Configuration for the two hardware gestures.
class PencilGestureConfig {
  const PencilGestureConfig({
    this.squeeze = PencilGestureBinding.squeezeMenu,
    this.doubleTap = PencilGestureBinding.toggleToolPair,
  });
  final PencilGestureBinding squeeze;
  final PencilGestureBinding doubleTap;
}

/// The Apple Pencil handler.
class ApplePencilHandler {
  ApplePencilHandler(this.input, {this.config = const PencilGestureConfig()});

  final InputState input;
  PencilGestureConfig config;

  final StreamController<PencilAction> _actions =
      StreamController<PencilAction>.broadcast();
  Stream<PencilAction> get actions => _actions.stream;

  /// Whether a squeeze is currently held (squeeze-drag mode).
  bool _squeezing = false;
  bool get isSqueezing => _squeezing;

  /// The tool that was active when the squeeze began — restored on
  /// release if the squeeze was used as a temporary switch.
  FeatherTool? _preSqueezeTool;

  // ----- Stylus samples ------------------------------------------------

  /// Push a normalized stylus sample (from a stylus pointer event).
  void onStylusSample({
    required double pressure,
    Vector2? tilt,
    double azimuth = 0.0,
    double altitude = 0.0,
    double twist = 0.0,
  }) {
    final sample = StylusSample(
      pressure: pressure.clamp(0.0, 1.0),
      tilt: tilt ?? Vector2.zero(),
      azimuthRadians: azimuth,
      altitudeRadians: altitude,
      twistRadians: twist,
    );
    input.updateStylus(sample, InputDevice.stylus);
  }

  /// Mark the stylus as lifted (pressure → 0).
  void onStylusUp() {
    input.updateStylus(StylusSample.idle, InputDevice.stylus);
    // End any squeeze-drag temporary switch.
    if (_squeezing) {
      _endSqueezeDrag();
    }
  }

  // ----- Hardware squeeze ----------------------------------------------

  /// The user squeezed the Pencil (Pro only).
  void onSqueeze({Vector2? position}) {
    _squeezing = true;
    switch (config.squeeze) {
      case PencilGestureBinding.squeezeMenu:
        _actions.add(SqueezeAction(position));
        break;
      case PencilGestureBinding.toggleToolPair:
        input.flipToolPair();
        break;
      case PencilGestureBinding.injector:
        // The host wires the Injector overlay.
        _actions.add(SqueezeAction(position));
        break;
    }
  }

  /// The squeeze was released.
  void onSqueezeRelease() {
    if (_squeezing && config.squeeze == PencilGestureBinding.squeezeMenu) {
      _actions.add(const SqueezeReleaseAction());
    }
    _endSqueezeDrag();
  }

  void _endSqueezeDrag() {
    _squeezing = false;
    if (_preSqueezeTool != null) {
      input.activeTool = _preSqueezeTool!;
      _preSqueezeTool = null;
    }
  }

  /// Begin a squeeze-drag (squeeze held while the pen is on the canvas):
  /// temporarily flip to the paired tool. The previous tool is restored
  /// when the squeeze ends or the pen lifts.
  void beginSqueezeDrag() {
    if (!_squeezing) _squeezing = true;
    _preSqueezeTool ??= input.activeTool;
    input.flipToolPair();
  }

  // ----- Hardware double-tap -------------------------------------------

  /// The user double-tapped the Pencil body.
  void onDoubleTap() {
    switch (config.doubleTap) {
      case PencilGestureBinding.toggleToolPair:
        input.flipToolPair();
        break;
      case PencilGestureBinding.squeezeMenu:
        _actions.add(const SqueezeAction(null));
        break;
      case PencilGestureBinding.injector:
        _actions.add(const DoubleTapAction());
        break;
    }
  }

  void dispose() {
    _actions.close();
  }
}
