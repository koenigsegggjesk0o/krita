// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// krita_brush_controller.dart — High-level brush control surface.
//
// Wraps the low-level [KritaNativeLibrary] and owns one native brush
// context handle. Exposes the brush operations the editor needs:
//   * Preset loading ([loadPreset]) and directory scanning
//     ([scanPresetFamilies] / [scannedPresets]).
//   * Scalar brush setters (size, color, opacity, spacing, smudge, flow,
//     hardness) and their reflection getters.
//   * Paintop-settings param editing with REPLACE-OR-APPEND semantics
//     ([setParam], [presetParams]) — the live param editor surface.
//   * Sensor-curve editing ([getCurve], [setCurve]).
//   * Dab generation ([generateDab]).
//   * Engine introspection ([engineVersion], [currentPaintopId],
//     [isEraserPreset]).
//
// The class is NOT re-entrant: callers must serialize brush operations
// (the editor does this by keeping a single instance on the UI isolate
// and gating access through [EditorState]).
//
// For backward compatibility the public type alias `KritaBrushEngine`
// is preserved (it points to this class). Existing imports
// `package:feather_krita/ffi/krita_bindings.dart` keep working through
// a re-export shim.

import 'dart:ffi';
import 'dart:typed_data';

import 'package:ffi/ffi.dart';

import 'krita_bindings.dart';
import 'krita_fallback.dart' show KritaBrushBackend;
import 'krita_param_map.dart';

/// High-level Krita brush controller. Owns a native brush context handle
/// and exposes the brush operations the Feather-Krita editor needs.
///
/// Implements [KritaBrushBackend] so the [KritaEngine] lifecycle manager
/// can return either this class or [KritaFallbackEngine] behind the
/// same type.
///
/// Typical usage:
/// ```dart
/// final brush = KritaBrushController();
/// if (!brush.loadPreset('/path/to/brush.kpp')) {
///   print(brush.lastError());
/// }
/// brush.size = 32;
/// brush.color = const BrushColor(0, 0, 0);
/// final dab = brush.generateDab(BrushInput(x: 10, y: 20, pressure: 0.8));
/// brush.dispose();
/// ```
class KritaBrushController implements KritaBrushBackend {
  /// Constructs a controller that resolves the C ABI on a freshly
  /// loaded [DynamicLibrary]. Throws [StateError] when the library
  /// cannot be opened or [krita_brush_init] returns a null handle.
  ///
  /// Prefer [KritaBrushController.withLibrary] when the caller already
  /// holds a [DynamicLibrary] (e.g. the [KritaEngine] lifecycle object
  /// loads once and shares it across brushes).
  KritaBrushController() : this.withLibrary(loadKritaBridge());

  /// Constructs a controller that resolves the C ABI on [lib] and
  /// allocates a fresh brush handle. Throws [StateError] when
  /// `krita_brush_init` returns a null handle.
  KritaBrushController.withLibrary(DynamicLibrary lib)
      : _native = KritaNativeLibrary(lib) {
    final handle = _native.init();
    if (handle == nullptr) {
      throw StateError('krita_brush_init returned a null handle');
    }
    _handle = handle;
  }

  final KritaNativeLibrary _native;
  // ignore: prefer_final_fields
  late Pointer<Void> _handle;
  bool _disposed = false;

  /// Returns the raw native handle (for the canvas / stroke session
  /// controllers that need to dispatch on the same context). The handle
  /// is owned by this controller and is invalidated by [dispose].
  Pointer<Void> get nativeHandle {
    _checkAlive();
    return _handle;
  }

  /// Returns the underlying native library wrapper (used by the canvas
  /// controller to share the same symbol table).
  KritaNativeLibrary get nativeLibrary => _native;

  // ----- Preset loading -------------------------------------------------

  /// Loads a Krita brush preset (.kpp) from [path] through the REAL
  /// engine's container parser (ZIP KoStore / legacy PNG zTXt / bare
  /// XML). Returns `true` on success. On failure [lastError] holds the
  /// engine's diagnostic.
  ///
  /// After a successful load the scalar getters ([currentSize],
  /// [currentOpacity], [currentSpacing], [currentHardness], [currentFlow],
  /// [currentSmudge], [isEraserPreset], [currentPresetName],
  /// [currentPaintopId]) and [presetParams] reflect the preset's values.
  @override
  bool loadPreset(String path) {
    _checkAlive();
    final pathPtr = path.toNativeUtf8();
    try {
      return _native.loadPreset(_handle, pathPtr) == 0;
    } finally {
      calloc.free(pathPtr);
    }
  }

  /// Scans [dir] recursively for .kpp presets through the REAL engine's
  /// own container parser and memoizes name + paintop family per preset
  /// on the native handle. Returns the number of presets found, or a
  /// negative code on bad arguments / missing directory. Read the parsed
  /// entries back with [scannedPresets].
  int scanPresetFamilies(String dir) {
    if (dir.isEmpty) {
      throw ArgumentError.value(dir, 'dir', 'must not be empty');
    }
    _checkAlive();
    final dirPtr = dir.toNativeUtf8();
    try {
      return _native.presetScan(_handle, dirPtr);
    } finally {
      calloc.free(dirPtr);
    }
  }

  /// The preset entries recorded by the last [scanPresetFamilies] call,
  /// in scan order (sorted by path). Each entry carries the
  /// engine-parsed display name, declared paintop family and the
  /// absolute file path (feed it to [loadPreset]).
  List<KritaPresetInfo> scannedPresets() {
    _checkAlive();
    final n = _native.presetCount(_handle);
    if (n <= 0) return const <KritaPresetInfo>[];
    return List<KritaPresetInfo>.generate(n, (i) {
      String read(Pointer<Utf8> Function(Pointer<Void>, int) fn) {
        final ptr = fn(_handle, i);
        return ptr == nullptr ? '' : ptr.toDartString();
      }
      return KritaPresetInfo(
        name: read(_native.presetNameAt),
        family: read(_native.presetFamilyAt),
        path: read(_native.presetPathAt),
      );
    });
  }

  // ----- Param map of the loaded preset ---------------------------------

  /// The number of paintop-settings-level parameters captured from the
  /// last successfully loaded preset (0 before the first load, after a
  /// failed load, or on the fallback bridge).
  int get presetParamCount {
    _checkAlive();
    return _native.presetParamCount(_handle);
  }

  /// The parameter NAME at [index] (document order) from the last
  /// successfully loaded preset, or `null` when out of range.
  String? presetParamNameAt(int index) {
    _checkAlive();
    final ptr = _native.presetParamNameAt(_handle, index);
    return ptr == nullptr ? null : ptr.toDartString();
  }

  /// The parameter VALUE at [index] (document order) from the last
  /// successfully loaded preset, or `null` when out of range.
  String? presetParamValueAt(int index) {
    _checkAlive();
    final ptr = _native.presetParamValueAt(_handle, index);
    return ptr == nullptr ? null : ptr.toDartString();
  }

  /// The loaded preset's paintop-settings-level parameters as an
  /// insertion-ordered [KritaParamMap] (replace-or-append semantics).
  /// Empty on the fallback bridge and before the first successful
  /// [loadPreset]. The returned map is a snapshot — edits to it do not
  /// affect the engine's map of record. Use [setParam] for live edits.
  @override
  KritaParamMap presetParams() {
    _checkAlive();
    final n = _native.presetParamCount(_handle);
    final out = KritaParamMap();
    if (n <= 0) return out;
    for (var i = 0; i < n; i++) {
      final namePtr = _native.presetParamNameAt(_handle, i);
      final valuePtr = _native.presetParamValueAt(_handle, i);
      if (namePtr == nullptr) continue;
      out.set(namePtr.toDartString(),
          valuePtr == nullptr ? '' : valuePtr.toDartString());
    }
    return out;
  }

  /// Sets a paintop-settings-level parameter of the loaded preset by
  /// NAME (live param editing). The real bridge records the (name,
  /// value) pair in its param map of record with REPLACE-OR-APPEND
  /// semantics (visible through [presetParams]) and applies the live
  /// effect for consumed keys — opacity, flow, hardness, softness,
  /// spacing, smudge, eraser. Unknown names are still recorded and
  /// return `true`.
  ///
  /// Returns `false` when the bridge rejects the edit (no preset loaded,
  /// empty name) OR when the loaded engine artifact predates the
  /// `krita_brush_set_param` export (symbol lookup throws
  /// [ArgumentError], caught and reported as "not supported"). Throws
  /// [ArgumentError] for an empty [name] before touching the engine.
  @override
  bool setParam(String name, String value) {
    if (name.trim().isEmpty) {
      throw ArgumentError.value(name, 'name', 'must not be empty');
    }
    _checkAlive();
    final namePtr = name.toNativeUtf8();
    final valuePtr = value.toNativeUtf8();
    try {
      return _native.setParam(_handle, namePtr, valuePtr) != 0;
    } on ArgumentError {
      // Engine artifact predates krita_brush_set_param.
      return false;
    } finally {
      calloc.free(namePtr);
      calloc.free(valuePtr);
    }
  }

  /// Reads the raw sensor-curve XML recorded on the loaded preset's
  /// param map for [key] (curve editing). Curve keys are the
  /// paintop-settings entries named "<CurveOption>Sensor"
  /// ("FlowSensor", "SizeSensor", ...) whose values are dynamic-sensor
  /// params XML. Returns `null` when the key is absent, the arguments
  /// are bad, no preset is loaded, the loaded bridge is a
  /// portable/fallback build, or the engine artifact predates the
  /// `get_curve` export.
  String? getCurve(String key) {
    if (key.trim().isEmpty) return null;
    _checkAlive();
    try {
      final keyPtr = key.toNativeUtf8();
      try {
        final ptr = _native.getCurve(_handle, keyPtr);
        return ptr == nullptr ? null : ptr.toDartString();
      } finally {
        calloc.free(keyPtr);
      }
    } on ArgumentError {
      // Engine artifact predates krita_brush_get_curve.
      return null;
    }
  }

  /// Records a validated sensor-curve XML for [key] on the loaded
  /// preset's param map (replace-or-append, same policy as [setParam]).
  /// Returns [CurveEditStatus.recorded] on success,
  /// [CurveEditStatus.rejected] when the engine rejects the payload or
  /// has no preset loaded, and [CurveEditStatus.unsupported] on
  /// portable/fallback bridges or engine artifacts that predate the
  /// `set_curve` export. Throws [ArgumentError] for empty Dart-side
  /// arguments before touching the engine.
  CurveEditStatus setCurve(String key, String curveXml) {
    if (key.trim().isEmpty) {
      throw ArgumentError.value(key, 'key', 'must not be empty');
    }
    if (curveXml.trim().isEmpty) {
      throw ArgumentError.value(curveXml, 'curveXml', 'must not be empty');
    }
    _checkAlive();
    final keyPtr = key.toNativeUtf8();
    final xmlPtr = curveXml.toNativeUtf8();
    try {
      final rc = _native.setCurve(_handle, keyPtr, xmlPtr);
      if (rc == 1) return CurveEditStatus.recorded;
      if (rc == -1) return CurveEditStatus.rejected;
      return CurveEditStatus.unsupported;
    } on ArgumentError {
      // Engine artifact predates krita_brush_set_curve.
      return CurveEditStatus.unsupported;
    } finally {
      calloc.free(keyPtr);
      calloc.free(xmlPtr);
    }
  }

  // ----- Scalar reflection getters --------------------------------------

  /// The brush diameter currently set on the native engine (px).
  @override
  double get currentSize {
    _checkAlive();
    return _native.getSize(_handle);
  }

  /// The master opacity currently set on the native engine (0..1).
  @override
  double get currentOpacity {
    _checkAlive();
    return _native.getOpacity(_handle);
  }

  /// The dab spacing currently set on the native engine (0..5).
  @override
  double get currentSpacing {
    _checkAlive();
    return _native.getSpacing(_handle);
  }

  /// The hardness currently set on the native engine (0..1).
  @override
  double get currentHardness {
    _checkAlive();
    return _native.getHardness(_handle);
  }

  /// The flow (per-dab application rate) currently set on the native
  /// engine (0..1, default 1.0).
  @override
  double get currentFlow {
    _checkAlive();
    return _native.getFlow(_handle);
  }

  /// The smudge rate currently set on the native engine (0..1).
  @override
  double get currentSmudge {
    _checkAlive();
    return _native.getSmudge(_handle);
  }

  /// Whether the loaded preset is an eraser preset (CompositeOp=erase /
  /// Krita/erase / EraserMode). Stroke compositing should switch to
  /// destination-out while this is true.
  @override
  bool get isEraserPreset {
    _checkAlive();
    return _native.getEraser(_handle);
  }

  /// The name of the last successfully loaded preset, or '' if none.
  @override
  String get currentPresetName {
    _checkAlive();
    final ptr = _native.getPresetName(_handle);
    return ptr == nullptr ? '' : ptr.toDartString();
  }

  /// The loaded preset's declared paintop family ("paintbrush",
  /// "eraser", "spray", ...), or '' when no preset is loaded or the
  /// preset omits the attribute.
  @override
  String get currentPaintopId {
    _checkAlive();
    final ptr = _native.getPaintopId(_handle);
    return ptr == nullptr ? '' : ptr.toDartString();
  }

  /// The loaded bridge's self-identification string, verbatim from
  /// `krita_brush_version` (e.g. "FeatherBridge-Krita/2.0 (real engine
  /// 5.3.4)" on the real bridge). Returns `null` when the loaded
  /// engine artifact predates the export or reports an empty string.
  @override
  String? engineVersion() {
    _checkAlive();
    try {
      final ptr = _native.version();
      if (ptr == nullptr) return null;
      final s = ptr.toDartString();
      return s.isEmpty ? null : s;
    } on ArgumentError {
      // Engine artifact predates krita_brush_version.
      return null;
    }
  }

  // ----- Scalar setters -------------------------------------------------

  /// Sets the brush diameter in pixels.
  @override
  set size(double pixels) {
    _checkAlive();
    _native.setSize(_handle, pixels);
  }

  /// Sets the brush color (packed ARGB).
  @override
  set color(BrushColor c) {
    _checkAlive();
    _native.setColor(_handle, c.packed);
  }

  /// Sets the master opacity in [0, 1].
  @override
  set opacity(double value) {
    _checkAlive();
    _native.setOpacity(_handle, value.clamp(0.0, 1.0));
  }

  /// Sets the dab spacing in [0, 5] (fraction of brush diameter).
  @override
  set spacing(double value) {
    _checkAlive();
    _native.setSpacing(_handle, value.clamp(0.0, 5.0));
  }

  /// Sets the smudge ratio in [0, 1].
  @override
  set smudge(double value) {
    _checkAlive();
    _native.setSmudge(_handle, value.clamp(0.0, 1.0));
  }

  /// Sets the per-dab flow in [0, 1].
  @override
  set flow(double value) {
    _checkAlive();
    _native.setFlow(_handle, value.clamp(0.0, 1.0));
  }

  /// Sets the brush hardness in [0, 1].
  @override
  set hardness(double value) {
    _checkAlive();
    _native.setHardness(_handle, value.clamp(0.0, 1.0));
  }

  // ----- Dab generation -------------------------------------------------

  /// Generates a single dab image for [input]. The returned [BrushDab]
  /// owns a copy of the pixel data, so callers may hold it
  /// indefinitely. Returns an empty dab if the engine rejects the input
  /// (e.g. zero pressure with skip-on-zero enabled).
  @override
  BrushDab generateDab(BrushInput input) {
    _checkAlive();
    final inputPtr = calloc<BrushInputNative>();
    final dabPtr = calloc<BrushDabNative>();
    try {
      input.writeTo(inputPtr.ref);
      dabPtr.ref.width = 0;
      dabPtr.ref.height = 0;
      dabPtr.ref.stride = 0;
      dabPtr.ref.reserved = 0;
      dabPtr.ref.pixels = nullptr;

      final ok = _native.generateDab(_handle, inputPtr, dabPtr);
      if (!ok) {
        return BrushDab(width: 0, height: 0, stride: 0, pixels: Uint8List(0));
      }
      final w = dabPtr.ref.width;
      final h = dabPtr.ref.height;
      final stride = dabPtr.ref.stride == 0 ? w * 4 : dabPtr.ref.stride;
      final pixelsPtr = dabPtr.ref.pixels;
      if (w <= 0 || h <= 0 || pixelsPtr == nullptr) {
        return BrushDab(width: 0, height: 0, stride: 0, pixels: Uint8List(0));
      }
      final byteCount = stride * h;
      // Copy the native pixel buffer into a Dart-owned Uint8List so the
      // data remains valid after we release the native dab.
      final pixels = Uint8List.fromList(pixelsPtr.asTypedList(byteCount));

      // Release the native buffer now that we have our own copy.
      _native.releaseDab(_handle, dabPtr);
      dabPtr.ref.pixels = nullptr;

      return BrushDab(width: w, height: h, stride: stride, pixels: pixels);
    } finally {
      calloc.free(inputPtr);
      if (dabPtr.ref.pixels != nullptr) {
        _native.releaseDab(_handle, dabPtr);
      }
      calloc.free(dabPtr);
    }
  }

  /// Resets the internal stroke state (call between strokes so the next
  /// dab is treated as the start of a fresh stroke).
  @override
  void cleanup() {
    _checkAlive();
    _native.cleanup(_handle);
  }

  /// Returns the most recent error message produced by the engine, or
  /// '' when there is none.
  @override
  String lastError() {
    _checkAlive();
    final ptr = _native.lastError(_handle);
    return ptr == nullptr ? '' : ptr.toDartString();
  }

  /// Releases the native brush handle. After this the instance is
  /// unusable. Safe to call more than once.
  @override
  void dispose() {
    if (_disposed) return;
    _disposed = true;
    _native.cleanup(_handle);
    _native.destroy(_handle);
  }

  void _checkAlive() {
    if (_disposed) {
      throw StateError('KritaBrushController has been disposed');
    }
  }
}

/// Backward-compatibility alias. The original high-level brush engine
/// class was named `KritaBrushEngine` and lived in
/// `lib/ffi/krita_bindings.dart`; the refactor moved it here as
/// [KritaBrushController]. Existing call sites keep compiling through
/// this alias and the re-export shim at `lib/ffi/krita_bindings.dart`.
typedef KritaBrushEngine = KritaBrushController;
