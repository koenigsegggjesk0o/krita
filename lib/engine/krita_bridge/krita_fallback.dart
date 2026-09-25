// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// krita_fallback.dart — Fallback brush engine for when real Krita is not
// available.
//
// When the native `krita_bridge` library cannot be loaded (missing DLL /
// .so, wrong architecture, headless test environment, sandbox that
// blocks `DynamicLibrary.open`), the editor still needs to paint. This
// engine provides the SAME brush-control surface as [KritaBrushController]
// but implements dab generation synthetically (a soft radial-gradient
// stamp with smoothstep falloff — the same shape the editor's old
// `syntheticDab()` helper produced).
//
// The fallback is HONEST about its capabilities:
//   * [engineVersion] reports "FeatherBridge-Fallback/1.0".
//   * [presetParamCount] is always 0 (no real preset parsing).
//   * [setParam] always returns `false` (live param editing not
//     available — the capability-probe contract from krita_bridge.h).
//   * [getCurve] / [setCurve] return null / [CurveEditStatus.unsupported].
//   * [scanPresetFamilies] returns 0 (no real container parser).
//   * [generateDab] produces a real soft-circle dab whose size /
//     opacity / flow / hardness track the scalar setters, so the
//     editor's brush sliders still visibly affect the canvas.
//
// The fallback engine implements the [KritaBrushBackend] interface so
// the [KritaEngine] lifecycle manager can return either a real
// [KritaBrushController] or a [KritaFallbackEngine] behind the same
// type, and the editor can paint without caring which one is live.

import 'dart:io';
import 'dart:math' as math;
import 'dart:typed_data';

import 'krita_bindings.dart';
import 'krita_param_map.dart';

/// Abstract brush-backend contract implemented by both the real
/// [KritaBrushController] and the synthetic [KritaFallbackEngine]. The
/// [KritaEngine] lifecycle manager returns one of these; the editor
/// paints through this interface without caring which backend is live.
abstract class KritaBrushBackend {
  /// The backend's self-identification string (e.g.
  /// "FeatherBridge-Krita/2.0 (real engine 5.3.4)" on the real bridge,
  /// "FeatherBridge-Fallback/1.0" on the fallback).
  String? engineVersion();

  /// Loads a .kpp preset. Returns `true` on success.
  bool loadPreset(String path);

  /// Sets a paintop-settings param by name (replace-or-append). Returns
  /// `true` when the engine accepted the edit.
  bool setParam(String name, String value);

  /// The parsed param map of the loaded preset (snapshot).
  KritaParamMap presetParams();

  /// Generates one dab for [input].
  BrushDab generateDab(BrushInput input);

  /// Resets stroke state (call between strokes).
  void cleanup();

  /// The most recent engine error message, or ''.
  String lastError();

  /// Releases backend resources.
  void dispose();

  /// The brush diameter (px).
  double get currentSize;
  /// The master opacity (0..1).
  double get currentOpacity;
  /// The dab spacing (0..5).
  double get currentSpacing;
  /// The hardness (0..1).
  double get currentHardness;
  /// The per-dab flow (0..1).
  double get currentFlow;
  /// The smudge rate (0..1).
  double get currentSmudge;
  /// True when the active brush is an eraser.
  bool get isEraserPreset;
  /// The active preset's display name.
  String get currentPresetName;
  /// The active preset's paintop family id.
  String get currentPaintopId;

  /// Sets the brush diameter.
  set size(double pixels);
  /// Sets the brush color.
  set color(BrushColor c);
  /// Sets the master opacity.
  set opacity(double value);
  /// Sets the dab spacing.
  set spacing(double value);
  /// Sets the smudge rate.
  set smudge(double value);
  /// Sets the per-dab flow.
  set flow(double value);
  /// Sets the brush hardness.
  set hardness(double value);
}

/// Synthetic fallback brush engine. Implements [KritaBrushBackend] with
/// pure-Dart dab generation (no native library). See the file header
/// for the capability contract.
class KritaFallbackEngine implements KritaBrushBackend {
  /// Constructs a fallback engine with sensible defaults (size 32,
  /// opaque black, opacity 1.0, hardness 0.5, flow 1.0, spacing 0.1).
  KritaFallbackEngine();

  double _size = 32.0;
  double _opacity = 1.0;
  double _spacing = 0.1;
  double _hardness = 0.5;
  double _flow = 1.0;
  double _smudge = 0.0;
  bool _eraser = false;
  BrushColor _color = const BrushColor(0, 0, 0);
  String _presetName = '';
  String _paintopId = '';
  String _lastError = '';
  final KritaParamMap _params = KritaParamMap();
  bool _disposed = false;

  @override
  String? engineVersion() => 'FeatherBridge-Fallback/1.0';

  @override
  bool loadPreset(String path) {
    // The fallback has no real container parser. We record the file
    // stem as the preset name so the UI shows something honest, and we
    // seed a minimal param map (the engine's known defaults) so the
    // inspector is not empty.
    if (path.isEmpty) {
      _lastError = 'fallback: empty preset path';
      return false;
    }
    final stem = path
        .split(Platform.pathSeparator)
        .last
        .split('/')
        .last
        .replaceAll('.kpp', '')
        .replaceAll('.KPP', '');
    _presetName = stem.isEmpty ? 'Fallback' : stem;
    _paintopId = 'paintbrush';
    _eraser = stem.toLowerCase().contains('eraser');
    _params
      ..clear()
      ..set('Krita/opacity', '${(_opacity * 100).round()}')
      ..set('FlowValue', '$_flow')
      ..set('hardness', '$_hardness')
      ..set('brush_spacing', '$_spacing');
    return true;
  }

  @override
  bool setParam(String name, String value) {
    if (name.isEmpty) return false;
    // The fallback records every setParam in its local map (so the
    // inspector shows the recorded value) but applies only the scalar
    // keys it actually consumes for dab generation. Returns false to
    // honor the capability-probe contract (the C ABI's set_param
    // returns 0 on portable/fallback bridges).
    _params.set(name, value);
    final spec = KritaKnownParams.lookup(name);
    if (spec == null) return false;
    switch (spec) {
      case KritaKnownParams.opacity:
        final v = double.tryParse(value) ?? _opacity * 100;
        _opacity = (v / 100.0).clamp(0.0, 1.0);
      case KritaKnownParams.flow:
        _flow = (double.tryParse(value) ?? _flow).clamp(0.0, 1.0);
      case KritaKnownParams.hardness:
        _hardness = (double.tryParse(value) ?? _hardness).clamp(0.0, 1.0);
      case KritaKnownParams.softness:
        final s = (double.tryParse(value) ?? 0.0).clamp(0.0, 1.0);
        _hardness = 1.0 - s;
      case KritaKnownParams.spacing:
        _spacing = (double.tryParse(value) ?? _spacing).clamp(0.0, 5.0);
      case KritaKnownParams.smudge:
        _smudge = (double.tryParse(value) ?? _smudge).clamp(0.0, 1.0);
      case KritaKnownParams.eraser:
        _eraser = value == 'true' ||
            value == '1' ||
            value.toLowerCase() == 'erase';
    }
    return false; // capability-probe contract: fallback reports "not supported"
  }

  @override
  KritaParamMap presetParams() => _params.copy();

  @override
  BrushDab generateDab(BrushInput input) {
    _checkAlive();
    final p = input.pressure.clamp(0.0, 1.0);
    if (p <= 0.0) {
      return BrushDab(width: 0, height: 0, stride: 0, pixels: Uint8List(0));
    }
    final radius = (_size / 2.0).clamp(1.0, 256.0);
    final r = radius.ceil();
    final size = r * 2;
    final px = Uint8List(size * size * 4);
    final cr = _color.r;
    final cg = _color.g;
    final cb = _color.b;
    final hardness = _hardness.clamp(0.0, 1.0);
    final flow = _flow.clamp(0.0, 1.0);
    final opacity = _opacity.clamp(0.0, 1.0);
    final alphaScale = (flow * opacity * p).clamp(0.0, 1.0);
    // Hardness controls the softness of the falloff: hardness 1.0 =
    // hard disk (full alpha inside the radius, zero outside); hardness
    // 0.0 = smooth gaussian-like falloff.
    for (var y = 0; y < size; y++) {
      for (var x = 0; x < size; x++) {
        final dx = x - r + 0.5;
        final dy = y - r + 0.5;
        final d = math.sqrt(dx * dx + dy * dy) / r;
        if (d > 1.0) continue; // outside the dab
        double falloff;
        if (hardness >= 0.999) {
          // Hard disk: full alpha inside the radius.
          falloff = 1.0;
        } else {
          // Soft falloff: linear from 1 at center to 0 at edge, with
          // hardness biasing the cutoff toward the edge.
          final soft = (1.0 - d).clamp(0.0, 1.0);
          final biased = soft / (soft + (1.0 - soft) * (1.0 - hardness) * 4.0);
          falloff = biased.isNaN ? 1.0 : biased;
        }
        final a = (falloff * alphaScale * 255.0).round().clamp(0, 255);
        if (_eraser) {
          // Eraser dabs carry full alpha so destination-out removes
          // paint; the host compositor handles the subtraction.
          final i = (y * size + x) * 4;
          px[i] = 0;
          px[i + 1] = 0;
          px[i + 2] = 0;
          px[i + 3] = a;
        } else {
          final i = (y * size + x) * 4;
          px[i] = cr;
          px[i + 1] = cg;
          px[i + 2] = cb;
          px[i + 3] = a;
        }
      }
    }
    return BrushDab(width: size, height: size, stride: size * 4, pixels: px);
  }

  @override
  void cleanup() {
    // No stroke state to reset on the fallback.
  }

  @override
  String lastError() => _lastError;

  @override
  void dispose() {
    _disposed = true;
  }

  void _checkAlive() {
    if (_disposed) {
      throw StateError('KritaFallbackEngine has been disposed');
    }
  }

  @override
  double get currentSize => _size;
  @override
  double get currentOpacity => _opacity;
  @override
  double get currentSpacing => _spacing;
  @override
  double get currentHardness => _hardness;
  @override
  double get currentFlow => _flow;
  @override
  double get currentSmudge => _smudge;
  @override
  bool get isEraserPreset => _eraser;
  @override
  String get currentPresetName => _presetName;
  @override
  String get currentPaintopId => _paintopId;

  @override
  set size(double pixels) => _size = pixels.clamp(0.0, 4096.0);
  @override
  set color(BrushColor c) => _color = c;
  @override
  set opacity(double value) => _opacity = value.clamp(0.0, 1.0);
  @override
  set spacing(double value) => _spacing = value.clamp(0.0, 5.0);
  @override
  set smudge(double value) => _smudge = value.clamp(0.0, 1.0);
  @override
  set flow(double value) => _flow = value.clamp(0.0, 1.0);
  @override
  set hardness(double value) => _hardness = value.clamp(0.0, 1.0);
}
