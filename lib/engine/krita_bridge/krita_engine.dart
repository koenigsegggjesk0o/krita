// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// krita_engine.dart — High-level Krita engine lifecycle.
//
// This is the top-level entry point for the Krita brush engine. It owns
// the engine-wide state (the loaded [DynamicLibrary], the capability
// probe result, the active brush backend) and exposes a small lifecycle
// surface: init / probe / shutdown / version / capabilities.
//
// The editor's [EditorState] constructs one [KritaEngine] at startup.
// [KritaEngine.init] probes the native library (hang-proof, via
// [probeKritaEngine] in io/engine_probe.dart), and:
//   * on success — returns a REAL [KritaBrushController] backend;
//   * on failure — returns a [KritaFallbackEngine] backend (the editor
//     keeps painting with synthetic dabs, no behavior regression vs.
//     the old build's synthetic-dab fallback path).
//
// The editor paints through the [KritaBrushBackend] interface returned
// by [KritaEngine.backend], so it does not care which backend is live.
//
// Capability probing: the engine exposes [capabilities] which reports
// which ABI additions the loaded bridge actually exports (live param
// editing, curve editing, stroke sessions, preset scanning). The UI
// gates advanced panels on these flags — e.g. the curve editor hides
// itself when [KritaEngineCapabilities.curveEditing] is false.

import 'dart:ffi';
import 'dart:io' show Platform;

import 'package:ffi/ffi.dart';

import 'krita_bindings.dart';
import 'krita_brush_controller.dart';
import 'krita_fallback.dart';

/// Engine capability flags. Reflect which ABI additions the loaded
/// bridge actually exports. Portable / fallback bridges report `false`
/// across the board (the capability-probe contract from krita_bridge.h).
class KritaEngineCapabilities {
  /// Constructs a capability snapshot.
  const KritaEngineCapabilities({
    required this.isRealEngine,
    required this.libraryPath,
    required this.versionString,
    required this.liveParamEditing,
    required this.curveEditing,
    required this.strokeSessions,
    required this.presetScanning,
    required this.presetParamMap,
  });

  /// True when the real native `krita_bridge` library is loaded and a
  /// brush handle was allocated. False for the fallback engine.
  final bool isRealEngine;

  /// The library path / name that loaded, or '' on the fallback.
  final String libraryPath;

  /// The bridge self-identification string (verbatim from
  /// `krita_brush_version`), or '' when the engine predates the export.
  final String versionString;

  /// True when `krita_brush_set_param` is exported (live param editing
  /// available). False on the fallback and on engine artifacts that
  /// predate the export.
  final bool liveParamEditing;

  /// True when `krita_brush_get_curve` / `krita_brush_set_curve` are
  /// exported (sensor-curve editing available).
  final bool curveEditing;

  /// True when `krita_stroke_begin` / `krita_stroke_move` / etc. are
  /// exported (real stroke-session ABI available). The host uses the
  /// v1 dab-compositing path when this is false.
  final bool strokeSessions;

  /// True when `krita_brush_preset_scan` is exported (directory
  /// scanning through the engine's container parser).
  final bool presetScanning;

  /// True when `krita_brush_preset_param_count` / `_name` / `_value`
  /// are exported (raw preset param map enumeration).
  final bool presetParamMap;

  /// Convenience: true when ALL capability flags are set (the real
  /// engine with the latest ABI).
  bool get fullFeatured =>
      isRealEngine &&
      liveParamEditing &&
      curveEditing &&
      strokeSessions &&
      presetScanning &&
      presetParamMap;

  @override
  String toString() => 'KritaEngineCapabilities(real=$isRealEngine, '
      'version="$versionString", paramEdit=$liveParamEditing, '
      'curves=$curveEditing, strokes=$strokeSessions, '
      'scan=$presetScanning, paramMap=$presetParamMap)';
}

/// Outcome of [KritaEngine.probe] / [KritaEngine.init].
enum KritaEngineStatus {
  /// The real native library loaded and a brush handle was allocated.
  realEngine,

  /// The native library could not be loaded; the fallback engine is in
  /// use. The editor still paints (synthetic dabs).
  fallback,

  /// The probe timed out (the library load hung — the class of bug
  /// [probeKritaEngine] guards against). The editor uses the fallback.
  probeTimeout,

  /// The platform is not supported (neither the real library nor the
  /// fallback should run here — e.g. a web build).
  unsupportedPlatform,
}

/// Top-level Krita engine lifecycle. Owns the loaded [DynamicLibrary]
/// (when present) and the active [KritaBrushBackend]. Construct one
/// instance at app startup, call [init], paint through [backend], and
/// call [shutdown] on app exit.
class KritaEngine {
  DynamicLibrary? _lib;
  KritaBrushBackend? _backend;
  KritaEngineCapabilities? _capabilities;
  KritaEngineStatus _status = KritaEngineStatus.fallback;
  String _lastError = '';

  /// Whether [init] has been called and succeeded (either real or
  /// fallback). False before [init] and after [shutdown].
  bool get isInitialized => _backend != null;

  /// The loaded [DynamicLibrary] backing the real engine, or `null` on
  /// the fallback / before [init] / after [shutdown]. Callers that want
  /// to share the library with other native controllers (e.g. a stroke
  /// session controller) read it from here.
  DynamicLibrary? get library => _lib;

  /// The active brush backend. Throws [StateError] before [init] or
  /// after [shutdown]. Real [KritaBrushController] when the native
  /// library loaded, [KritaFallbackEngine] otherwise.
  KritaBrushBackend get backend {
    final b = _backend;
    if (b == null) {
      throw StateError('KritaEngine not initialized — call init() first');
    }
    return b;
  }

  /// The engine status from the last [init] / [probe].
  KritaEngineStatus get status => _status;

  /// The capability snapshot from the last [init] / [probe]. Null
  /// before [init].
  KritaEngineCapabilities? get capabilities => _capabilities;

  /// The most recent error message (from the probe / init / shutdown),
  /// or '' when there is none.
  String get lastError => _lastError;

  /// Probes the native library WITHOUT allocating a brush handle. Same
  /// capability detection as [init] but does not switch the active
  /// backend. Returns the probed status. Safe to call before [init] or
  /// after [shutdown]; does not mutate engine state beyond caching the
  /// probe result.
  ///
  /// Pass a pre-opened [DynamicLibrary] when the caller already holds
  /// one (e.g. the editor's existing loader). When omitted, the engine
  /// tries [loadKritaBridge] and catches any failure.
  KritaEngineStatus probe({DynamicLibrary? preopened}) {
    final lib = preopened ?? _tryLoad();
    if (lib == null) {
      _status = KritaEngineStatus.fallback;
      _capabilities = const KritaEngineCapabilities(
        isRealEngine: false,
        libraryPath: '',
        versionString: '',
        liveParamEditing: false,
        curveEditing: false,
        strokeSessions: false,
        presetScanning: false,
        presetParamMap: false,
      );
      return _status;
    }
    _capabilities = _probeCapabilities(lib);
    _status = KritaEngineStatus.realEngine;
    return _status;
  }

  /// Initializes the engine: probes the native library, allocates a
  /// brush handle on success, and falls back to [KritaFallbackEngine]
  /// on failure. Returns the resulting status. Idempotent — calling
  /// [init] twice reuses the already-initialized backend (call
  /// [shutdown] first to force a re-init).
  ///
  /// Pass [preopened] when the caller already holds a [DynamicLibrary]
  /// (e.g. the editor's existing loader that also feeds the stroke
  /// session controller).
  KritaEngineStatus init({DynamicLibrary? preopened}) {
    if (_backend != null) return _status;
    final lib = preopened ?? _tryLoad();
    if (lib == null) {
      _backend = KritaFallbackEngine();
      _status = KritaEngineStatus.fallback;
      _capabilities = const KritaEngineCapabilities(
        isRealEngine: false,
        libraryPath: '',
        versionString: 'FeatherBridge-Fallback/1.0',
        liveParamEditing: false,
        curveEditing: false,
        strokeSessions: false,
        presetScanning: false,
        presetParamMap: false,
      );
      return _status;
    }
    try {
      _lib = lib;
      _backend = KritaBrushController.withLibrary(lib);
      _capabilities = _probeCapabilities(lib);
      _status = KritaEngineStatus.realEngine;
    } catch (e) {
      _lastError = '$e';
      _backend = KritaFallbackEngine();
      _status = KritaEngineStatus.fallback;
      _capabilities = const KritaEngineCapabilities(
        isRealEngine: false,
        libraryPath: '',
        versionString: 'FeatherBridge-Fallback/1.0',
        liveParamEditing: false,
        curveEditing: false,
        strokeSessions: false,
        presetScanning: false,
        presetParamMap: false,
      );
    }
    return _status;
  }

  /// Releases the active backend and (when present) the loaded
  /// [DynamicLibrary]. After this the engine is in the same state as
  /// before [init]; calling [init] again re-probes.
  void shutdown() {
    _backend?.dispose();
    _backend = null;
    _lib = null;
    _capabilities = null;
    _status = KritaEngineStatus.fallback;
    _lastError = '';
  }

  /// Convenience: returns the bridge version string from the active
  /// backend (real or fallback). Null when the engine is not
  /// initialized.
  String? version() => _backend?.engineVersion();

  DynamicLibrary? _tryLoad() {
    try {
      return loadKritaBridge();
    } catch (e) {
      _lastError = '$e';
      return null;
    }
  }

  KritaEngineCapabilities _probeCapabilities(DynamicLibrary lib) {
    final native = KritaNativeLibrary(lib);
    String version = '';
    try {
      final ptr = native.version();
      if (ptr != nullptr) {
        version = ptr.toDartString();
      }
    } on ArgumentError {
      // Engine artifact predates krita_brush_version.
    }
    return KritaEngineCapabilities(
      isRealEngine: true,
      libraryPath: _describeLib(lib),
      versionString: version,
      liveParamEditing: native.hasSymbol('krita_brush_set_param'),
      curveEditing: native.hasSymbol('krita_brush_get_curve') &&
          native.hasSymbol('krita_brush_set_curve'),
      strokeSessions: native.hasSymbol('krita_stroke_begin') &&
          native.hasSymbol('krita_stroke_move'),
      presetScanning: native.hasSymbol('krita_brush_preset_scan'),
      presetParamMap: native.hasSymbol('krita_brush_preset_param_count'),
    );
  }

  String _describeLib(DynamicLibrary lib) {
    // DynamicLibrary does not expose its path; we report the platform's
    // default name so the UI can show something useful.
    if (Platform.isWindows) return 'krita_bridge.dll';
    if (Platform.isMacOS) return 'libkrita_bridge.dylib';
    return 'libkrita_bridge.so';
  }
}
