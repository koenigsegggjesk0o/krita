// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// input_state.dart — global input/tool state for Feather-Krita.
//
// Tracks WHICH tool the user is holding (Draw / Draw Shape / Erase /
// Vacuum / Select / Deselect), the active 3D guide, whether the mirror
// assistance is on, the last reported stylus pressure / tilt, and whether
// the current input comes from the Apple Pencil or a finger.
//
// The Apple-Pencil "Double Tap" and "Squeeze" gestures (see
// interfaceandgestures_applepencil.txt) flip between paired tools —
// Draw ⇄ Draw Shape, Erase ⇄ Vacuum, Select ⇄ Deselect. To support that
// the tools are grouped into [ToolPair]s and the input state knows how to
// toggle within a pair.
//
// Depends on: lib/core/math/

import 'package:flutter/foundation.dart';

import 'package:feather_krita/core/math/math.dart';

/// The primary tools exposed in the top-right Tool Menu.
///
/// The mirror, clipboard and stage-panel entries in the Tool Menu open
/// overlays rather than switching the active drawing tool, so they live
/// outside this enum (see [OverlayKind]).
enum FeatherTool {
  draw,
  drawShape,
  erase,
  vacuum,
  select,
  deselect,
}

/// The three tool pairs the Apple Pencil Double-Tap / Squeeze-drag flips
/// between (per the Apple Pencil docs).
enum ToolPair { drawShape, eraseVacuum, selectDeselect }

extension ToolPairX on ToolPair {
  /// The pair that contains [tool], or `null`.
  static ToolPair? pairFor(FeatherTool tool) {
    switch (tool) {
      case FeatherTool.draw:
      case FeatherTool.drawShape:
        return ToolPair.drawShape;
      case FeatherTool.erase:
      case FeatherTool.vacuum:
        return ToolPair.eraseVacuum;
      case FeatherTool.select:
      case FeatherTool.deselect:
        return ToolPair.selectDeselect;
    }
  }

  /// The other member of the pair containing [tool].
  static FeatherTool? flip(FeatherTool tool) {
    switch (tool) {
      case FeatherTool.draw:
        return FeatherTool.drawShape;
      case FeatherTool.drawShape:
        return FeatherTool.draw;
      case FeatherTool.erase:
        return FeatherTool.vacuum;
      case FeatherTool.vacuum:
        return FeatherTool.erase;
      case FeatherTool.select:
        return FeatherTool.deselect;
      case FeatherTool.deselect:
        return FeatherTool.select;
    }
  }
}

/// Overlays reachable from the Tool Menu (Mirror is modelled here too so
/// the squeeze-menu / shortcut layer can toggle it).
enum OverlayKind { mirror, clipboard, stagePanel, none }

/// Which physical input source produced the most recent event.
enum InputDevice { stylus, touch, mouse }

/// A snapshot of the most recent stylus sample.
class StylusSample {
  StylusSample({
    this.pressure = 0.0,
    Vector2? tilt,
    this.azimuthRadians = 0.0,
    this.altitudeRadians = 0.0,
    this.twistRadians = 0.0,
  }) : tilt = tilt ?? Vector2.zero();

  /// Normalized pressure in [0, 1].
  final double pressure;

  /// tilt-x / tilt-y in radians.
  final Vector2 tilt;

  /// Stylus azimuth around the screen normal (radians).
  final double azimuthRadians;

  /// Stylus altitude above the screen plane (radians).
  final double altitudeRadians;

  /// Barrel-roll twist (Apple Pencil Pro), radians.
  final double twistRadians;

  static final StylusSample idle = StylusSample();
}

/// Global input state.
class InputState extends ChangeNotifier {
  InputState({
    FeatherTool tool = FeatherTool.draw,
    this.activeGuideId,
    this.mirrorEnabled = false,
    this.fingerPenActive = false,
  }) : _tool = tool;

  FeatherTool _tool;
  FeatherTool get activeTool => _tool;
  set activeTool(FeatherTool value) {
    if (_tool == value) return;
    _tool = value;
    notifyListeners();
  }

  /// The id of the active 3D guide the user is drawing onto, or `null`
  /// when drawing in free space.
  String? activeGuideId;
  set activeGuide(String? id) {
    activeGuideId = id;
    notifyListeners();
  }

  /// Whether the mirror assistance is on (per assistance/mirror.txt).
  bool mirrorEnabled;

  /// The Finger-Pen toggle from the Apple Pencil docs — when on, finger
  /// touches are interpreted as pen strokes and navigation is locked.
  bool fingerPenActive;

  /// The device that produced the most recent sample.
  InputDevice lastDevice = InputDevice.touch;

  /// The most recent stylus sample (pressure / tilt / azimuth / altitude /
  /// twist). Updated by [apple_pencil_handler.dart].
  StylusSample stylus = StylusSample.idle;

  /// The currently open overlay, if any.
  OverlayKind openOverlay = OverlayKind.none;

  /// Quick-flip the active tool within its pair — the action wired to the
  /// Apple Pencil Double-Tap (and the default squeeze action).
  FeatherTool flipToolPair() {
    final flipped = ToolPairX.flip(_tool);
    if (flipped != null) activeTool = flipped;
    return _tool;
  }

  /// Toggle the mirror assistance.
  void toggleMirror() {
    mirrorEnabled = !mirrorEnabled;
    notifyListeners();
  }

  /// Toggle Finger-Pen mode.
  void toggleFingerPen() {
    fingerPenActive = !fingerPenActive;
    notifyListeners();
  }

  /// Update the stylus sample. Pass [device] too so consumers can react
  /// to the source switch (e.g. locking finger navigation when the pen
  /// touches down).
  void updateStylus(StylusSample sample, InputDevice device) {
    stylus = sample;
    lastDevice = device;
    notifyListeners();
  }
}
