// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// brush_engine.dart — the brush engine interface.
//
// Two jobs, per the docs:
//   1. 2D brush painting: connects to the Krita C++ brush engine over FFI
//      (lib/ffi/krita_bindings.dart) to generate per-dab pixel stamps —
//      the same dabs Krita would paint on a flat canvas.
//   2. 3D curve rendering: accumulates the pointer's 3D sample points
//      into a [Stroke] (lib/models/stroke.dart) that the brush renderer
//      turns into a lit, material-shaded ribbon.
//
// [BrushEngine] is the abstract contract. Two implementations:
//   - [KritaFFIBrushEngine] : drives the native Krita engine for 2D dabs
//     and builds 3D strokes; falls back to procedural dabs if the native
//     library is unavailable (web, missing DLL).
//   - [ProceduralBrushEngine] : pure-Dart fallback — synthesizes a soft
//     circular dab and builds 3D strokes. Used in tests and on platforms
//     without the Krita bridge.
//
// Stroke smoothing (docs: the brush panel's smoothing) is applied here
// as an exponential moving average on the incoming sample positions
// before they enter the [Stroke].

import 'dart:math' as math;
import 'dart:typed_data';

import 'package:vector_math/vector_math_64.dart';

import 'package:feather_krita/engine/brush/brush_preset.dart';
import 'package:feather_krita/engine/brush/brush_settings.dart';
import 'package:feather_krita/ffi/krita_bindings.dart';
import 'package:feather_krita/models/stroke.dart';

/// Abstract brush engine: 2D dab generation + 3D stroke assembly.
abstract class BrushEngine {
  BrushEngine({BrushSettings? settings, this.color = 0xFF000000})
      : settings = settings ?? BrushSettings(size: 8.0);

  /// Active brush parameters.
  BrushSettings settings;

  /// Current brush color (ARGB int).
  int color;

  /// Active preset, if one is loaded (drives material/pattern downstream).
  FeatherBrushPreset? preset;

  bool _strokeActive = false;
  final List<StrokePoint> _pending = <StrokePoint>[];
  Vector3 _lastSmoothed = Vector3.zero();

  // --- 3D stroke assembly ---------------------------------------------

  /// Begins a new stroke at [start]. Discards any in-progress stroke.
  void beginStroke(StrokePoint start) {
    _strokeActive = true;
    _pending.clear();
    _lastSmoothed = start.position.clone();
    _pending.add(start.copyWith(position: _lastSmoothed.clone()));
  }

  /// Appends a sample to the active stroke, applying [BrushSettings
  /// .smoothing] as an exponential moving average on the position.
  void addPoint(StrokePoint p) {
    if (!_strokeActive) return;
    final s = settings.smoothing.clamp(0.0, 1.0);
    // EMA factor: 0 = raw input, 1 = near-frozen (heavy stabilization).
    final a = 1.0 - s * 0.85;
    final smoothed = _lastSmoothed * (1.0 - a) + p.position * a;
    _lastSmoothed = smoothed;
    _pending.add(p.copyWith(position: smoothed));
  }

  /// Ends the active stroke and returns the assembled [Stroke] (or null
  /// if fewer than two samples accumulated). The stroke carries the
  /// engine's current brush type / color / thickness.
  Stroke? endStroke({BrushType brushType = BrushType.basic}) {
    if (!_strokeActive) return null;
    _strokeActive = false;
    if (_pending.length < 2) {
      _pending.clear();
      return null;
    }
    final stroke = Stroke(
      brushType: brushType,
      color: color,
      thickness: settings.size,
      points: List<StrokePoint>.of(_pending),
    );
    _pending.clear();
    return stroke;
  }

  /// Whether a stroke is currently being assembled.
  bool get isStrokeActive => _strokeActive;

  // --- 2D dab generation -----------------------------------------------

  /// Generates one 2D dab at the pointer state described by [input].
  /// Returns null if the engine has no dab to emit (e.g. spacing not
  /// yet reached). Implementations may drive the native Krita engine
  /// or synthesize a procedural stamp.
  BrushDab? generateDab(BrushInput input);

  /// Releases any native resources. Safe to call multiple times.
  void dispose();
}

/// Pure-Dart fallback engine. Synthesizes a soft circular dab and builds
/// 3D strokes — used when the Krita native bridge is unavailable.
class ProceduralBrushEngine extends BrushEngine {
  ProceduralBrushEngine({super.settings, super.color});

  @override
  BrushDab? generateDab(BrushInput input) {
    final radius = (settings.sizeAtPressure(input.pressure) * 2.0)
        .clamp(1.0, 256.0);
    final size = (radius * 2).round();
    final px = Uint8List(size * size * 4);
    final hardness = settings.hardness.clamp(0.0, 1.0);
    final alphaScale = settings.opacityAtPressure(input.pressure) *
        settings.flow *
        255.0;
    final cx = size / 2.0;
    for (var y = 0; y < size; y++) {
      for (var x = 0; x < size; x++) {
        final dx = x + 0.5 - cx;
        final dy = y + 0.5 - cx;
        final d = math.sqrt(dx * dx + dy * dy) / radius;
        // Soft falloff: hardness controls the inner solid core.
        var a = 1.0 - d;
        final core = hardness;
        if (d > core) {
          a = (1.0 - (d - core) / (1.0 - core)).clamp(0.0, 1.0);
        }
        final ai = (a * alphaScale).round().clamp(0, 255);
        final i = (y * size + x) * 4;
        px[i] = (color >> 16) & 0xff;
        px[i + 1] = (color >> 8) & 0xff;
        px[i + 2] = color & 0xff;
        px[i + 3] = ai;
      }
    }
    return BrushDab(
        width: size, height: size, stride: size * 4, pixels: px);
  }

  @override
  void dispose() {}
}

/// Drives the native Krita brush engine for 2D dabs and builds 3D
/// strokes. Construct via [createBrushEngine], which falls back to a
/// [ProceduralBrushEngine] if the native library cannot be loaded.
class KritaFFIBrushEngine extends BrushEngine {
  KritaFFIBrushEngine._(this._native, {super.settings, super.color});

  final KritaBrushEngine _native;

  /// Tries to construct a native-backed engine, returning null on any
  /// FFI / library-load failure (the caller — [createBrushEngine] —
  /// substitutes a procedural fallback so drawing never breaks).
  static KritaFFIBrushEngine? tryCreate({
    BrushSettings? settings,
    int color = 0xFF000000,
  }) {
    try {
      final native = KritaBrushEngine();
      return KritaFFIBrushEngine._(native, settings: settings, color: color)
        .._pushSettings();
    } catch (_) {
      return null;
    }
  }

  void _pushSettings() {
    _native
      ..size = settings.size
      ..color = BrushColor(
        (color >> 16) & 0xff,
        (color >> 8) & 0xff,
        color & 0xff,
        (color >> 24) & 0xff,
      )
      ..opacity = settings.opacity
      ..flow = settings.flow
      ..hardness = settings.hardness
      ..spacing = settings.spacing;
  }

  @override
  BrushDab? generateDab(BrushInput input) {
    _pushSettings();
    return _native.generateDab(input);
  }

  @override
  void dispose() {
    _native.cleanup();
    _native.dispose();
  }
}

/// Constructs the best available [BrushEngine]: a native Krita engine
/// when the bridge loads, otherwise a pure-Dart procedural engine.
BrushEngine createBrushEngine({
  BrushSettings? settings,
  int color = 0xFF000000,
}) =>
    KritaFFIBrushEngine.tryCreate(settings: settings, color: color) ??
    ProceduralBrushEngine(settings: settings, color: color);
