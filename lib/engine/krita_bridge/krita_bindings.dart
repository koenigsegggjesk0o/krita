// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// krita_bindings.dart — Low-level Dart FFI bindings to the Krita C++ brush
// engine.
//
// This file is the THIN layer between Dart and the native `krita_bridge`
// shared library (krita_bridge_real.cpp). It contains ONLY:
//   * The `dart:ffi` Struct mirrors of the C ABI structs (BrushInput /
//     BrushDab) — the layouts MUST match `krita_bridge.h`.
//   * The Dart-side immutable value types (BrushInput, BrushDab,
//     BrushColor, BrushInputFlags, KritaPresetInfo, CurveEditStatus).
//   * The C function-pointer typedefs (Native + Dart variants).
//   * The [loadKritaBridge] DynamicLibrary loader.
//   * The [KritaNativeLibrary] class that looks up every export of the
//     C ABI once and exposes them as typed Dart function fields.
//
// The HIGH-LEVEL brush engine (preset loading, param editing, dab
// generation orchestration, error reporting) lives in
// [KritaBrushController] (`krita_brush_controller.dart`). Engine lifecycle
// (init / probe / shutdown / version / capabilities) lives in
// [KritaEngine] (`krita_engine.dart`). The fallback engine is
// [KritaFallbackEngine] (`krita_fallback.dart`).
//
// Refactor note: this file used to live at `lib/ffi/krita_bindings.dart`
// and bundled the high-level `KritaBrushEngine` class in the same file.
// It has been split into a low-level bindings layer (this file) and a
// high-level controller layer (`krita_brush_controller.dart`). A
// re-export shim at `lib/ffi/krita_bindings.dart` preserves the old
// import path so existing call sites keep compiling.

import 'dart:ffi';
import 'dart:io';
import 'dart:typed_data';

import 'package:ffi/ffi.dart';

// ---------------------------------------------------------------------------
// Native struct mirrors — these layouts MUST match `krita_bridge.h`.
// ---------------------------------------------------------------------------

/// Mirrors `BrushInput` in `krita_bridge.h`.
///
/// Layout: 6 doubles + 2 floats + 2 int32 (flags + explicit padding) =
/// 64 bytes. The trailing `_padding` field keeps the ABI stable across
/// platforms with different native alignment rules.
final class BrushInputNative extends Struct {
  /// X position in device-independent pixels relative to the dab origin.
  @Double()
  external double x;

  /// Y position in device-independent pixels relative to the dab origin.
  @Double()
  external double y;

  /// Normalized pressure in range [0, 1] (1.0 == max pressure).
  @Double()
  external double pressure;

  /// X tilt in degrees, range [-90, 90].
  @Double()
  external double tiltX;

  /// Y tilt in degrees, range [-90, 90].
  @Double()
  external double tiltY;

  /// Time in seconds since stroke start.
  @Double()
  external double time;

  /// Velocity along X axis (px/s).
  @Float()
  external double velocityX;

  /// Velocity along Y axis (px/s).
  @Float()
  external double velocityY;

  /// Bitfield of [BrushInputFlags] values.
  @Int32()
  external int flags;

  /// Reserved for alignment / future use. Always written as 0.
  @Int32()
  // ignore: unused_field
  external int _padding;
}

/// Mirrors `BrushDab` in `krita_bridge.h`.
///
/// The pixel buffer is allocated on the native side and owned by the
/// bridge — the high-level controller copies the data out via
/// `asTypedList` and then calls `krita_brush_release_dab` to free it.
final class BrushDabNative extends Struct {
  /// Width of the dab image in pixels.
  @Int32()
  external int width;

  /// Height of the dab image in pixels.
  @Int32()
  external int height;

  /// Number of bytes per pixel row (== width * 4 for RGBA8, but the
  /// bridge may pad rows in the future — always honor this field).
  @Int32()
  external int stride;

  /// Reserved for alignment / future use.
  @Int32()
  external int reserved;

  /// Pointer to width * height * 4 bytes of RGBA8 pixel data, or NULL
  /// when [width] / [height] are 0.
  external Pointer<Uint8> pixels;
}

// ---------------------------------------------------------------------------
// High-level Dart value types.
// ---------------------------------------------------------------------------

/// Bit flags packed inside [BrushInputNative.flags].
class BrushInputFlags {
  const BrushInputFlags._(this.value);
  final int value;

  /// No flags set (plain brush dab).
  static const int kNone = 0;

  /// Eraser mode (subtract alpha).
  static const int kEraser = 1 << 0;

  /// Smudge mode (pick + smear existing color).
  static const int kSmudge = 1 << 1;

  /// Mirror the dab position across the X axis.
  static const int kMirrorX = 1 << 2;

  /// Mirror the dab position across the Y axis.
  static const int kMirrorY = 1 << 3;

  /// Inverted pressure (light touch == full pressure).
  static const int kInverted = 1 << 4;

  /// Constructs a flag set from boolean components.
  factory BrushInputFlags({
    bool eraser = false,
    bool smudge = false,
    bool mirrorX = false,
    bool mirrorY = false,
    bool inverted = false,
  }) {
    var v = kNone;
    if (eraser) v |= kEraser;
    if (smudge) v |= kSmudge;
    if (mirrorX) v |= kMirrorX;
    if (mirrorY) v |= kMirrorY;
    if (inverted) v |= kInverted;
    return BrushInputFlags._(v);
  }

  /// True when the eraser bit is set.
  bool get eraser => (value & kEraser) != 0;

  /// True when the smudge bit is set.
  bool get smudge => (value & kSmudge) != 0;

  /// True when the X-axis mirror bit is set.
  bool get mirrorX => (value & kMirrorX) != 0;

  /// True when the Y-axis mirror bit is set.
  bool get mirrorY => (value & kMirrorY) != 0;

  /// True when the inverted-pressure bit is set.
  bool get inverted => (value & kInverted) != 0;
}

/// Immutable Dart-side description of one brush input sample.
class BrushInput {
  /// X position in device-independent pixels relative to the dab origin.
  final double x;

  /// Y position in device-independent pixels relative to the dab origin.
  final double y;

  /// Normalized pressure in [0, 1].
  final double pressure;

  /// X tilt in degrees, range [-90, 90].
  final double tiltX;

  /// Y tilt in degrees, range [-90, 90].
  final double tiltY;

  /// Time in seconds since stroke start.
  final double time;

  /// Velocity along X axis (px/s).
  final double velocityX;

  /// Velocity along Y axis (px/s).
  final double velocityY;

  /// Bitfield of [BrushInputFlags] values.
  final BrushInputFlags flags;

  /// Constructs a brush input sample. [pressure] defaults to 0.5; the
  /// other optional fields default to zero / no flags.
  const BrushInput({
    required this.x,
    required this.y,
    this.pressure = 0.5,
    this.tiltX = 0.0,
    this.tiltY = 0.0,
    this.time = 0.0,
    this.velocityX = 0.0,
    this.velocityY = 0.0,
    this.flags = const BrushInputFlags._(BrushInputFlags.kNone),
  });

  /// Returns a copy of this input with the given fields replaced.
  BrushInput copyWith({
    double? x,
    double? y,
    double? pressure,
    double? tiltX,
    double? tiltY,
    double? time,
    double? velocityX,
    double? velocityY,
    BrushInputFlags? flags,
  }) =>
      BrushInput(
        x: x ?? this.x,
        y: y ?? this.y,
        pressure: pressure ?? this.pressure,
        tiltX: tiltX ?? this.tiltX,
        tiltY: tiltY ?? this.tiltY,
        time: time ?? this.time,
        velocityX: velocityX ?? this.velocityX,
        velocityY: velocityY ?? this.velocityY,
        flags: flags ?? this.flags,
      );

  /// Writes this input into the native struct. The pressure is clamped
  /// to [0, 1] (out-of-range values fall back to 1.0 in the engine, same
  /// policy as the stroke-session ABI).
  ///
  /// Public so the high-level [KritaBrushController] (a different
  /// library) can marshal inputs into the native struct.
  void writeTo(BrushInputNative native) {
    native.x = x;
    native.y = y;
    native.pressure = pressure.clamp(0.0, 1.0);
    native.tiltX = tiltX;
    native.tiltY = tiltY;
    native.time = time;
    native.velocityX = velocityX;
    native.velocityY = velocityY;
    native.flags = flags.value;
    native._padding = 0;
  }
}

/// Immutable Dart-side description of a generated dab.
class BrushDab {
  /// Width of the dab image in pixels.
  final int width;

  /// Height of the dab image in pixels.
  final int height;

  /// Number of bytes per pixel row (== width * 4 for RGBA8, but the
  /// bridge may pad rows — always honor this when indexing [pixels]).
  final int stride;

  /// RGBA8 (straight alpha, R,G,B,A byte order) pixel data. Length is
  /// [stride] * [height] bytes.
  final Uint8List pixels;

  /// Constructs a dab. Callers usually get a dab back from
  /// [KritaBrushController.generateDab] (or [KritaFallbackEngine]).
  const BrushDab({
    required this.width,
    required this.height,
    required this.stride,
    required this.pixels,
  });

  /// True when the dab carries no pixels (the engine rejected the input
  /// or produced an empty stamp, e.g. zero pressure with skip-on-zero).
  bool get isEmpty => width == 0 || height == 0 || pixels.isEmpty;

  /// True when the dab carries pixels (inverse of [isEmpty]).
  bool get isNotEmpty => !isEmpty;
}

/// RGBA color value packed as 0xAARRGGBB.
class BrushColor {
  /// Alpha channel (0 == fully transparent, 255 == fully opaque).
  final int a;

  /// Red channel (0..255).
  final int r;

  /// Green channel (0..255).
  final int g;

  /// Blue channel (0..255).
  final int b;

  /// Constructs a color. [a] defaults to 255 (fully opaque).
  const BrushColor(this.r, this.g, this.b, [this.a = 255]);

  /// The color packed as 0xAARRGGBB (the format the C ABI expects).
  int get packed => (a << 24) | (r << 16) | (g << 8) | b;

  /// Inverse of [packed]: unpacks a 0xAARRGGBB integer into a color.
  static BrushColor fromPacked(int packed) => BrushColor(
        (packed >> 16) & 0xff,
        (packed >> 8) & 0xff,
        packed & 0xff,
        (packed >> 24) & 0xff,
      );
}

/// Outcome of a sensor-curve edit (milestone (i) curve ABI).
enum CurveEditStatus {
  /// The (key, xml) pair was recorded on the engine's param map of
  /// record (replace-or-append) — visible through preset params and
  /// [KritaBrushController.getCurve].
  recorded,

  /// The engine rejected the XML payload (validation) or the edit could
  /// not be applied (no preset loaded). The map is untouched either way.
  rejected,

  /// The loaded bridge cannot edit curves: portable/fallback bridges
  /// return 0 unconditionally, and engine artifacts that predate the
  /// get_curve/set_curve export fail the symbol lookup. The UI hides
  /// curve editing in this state.
  unsupported,
}

/// One preset entry from an engine-side directory scan
/// ([KritaBrushController.scanPresetFamilies]).
class KritaPresetInfo {
  /// Constructs a preset descriptor.
  const KritaPresetInfo({
    required this.name,
    required this.family,
    required this.path,
  });

  /// Engine-parsed display name (root `name` attribute, else file name).
  final String name;

  /// Declared paintop family (root `paintopid` / `<Paintop id>`), '' when
  /// the preset omits it.
  final String family;

  /// Absolute file path (loadable through [KritaBrushController.loadPreset]).
  final String path;

  @override
  String toString() => 'KritaPresetInfo($name, family=$family)';
}

// ---------------------------------------------------------------------------
// C function-pointer typedefs. Each ABI export has a Native (C signature)
// and a Dart (marshalled) variant.
// ---------------------------------------------------------------------------

typedef _KritaBrushInitNative = Pointer<Void> Function();
typedef _KritaBrushInitDart = Pointer<Void> Function();

typedef _KritaBrushDestroyNative = Void Function(Pointer<Void> handle);
typedef _KritaBrushDestroyDart = void Function(Pointer<Void> handle);

typedef _KritaBrushLoadPresetNative = Int32 Function(
    Pointer<Void> handle, Pointer<Utf8> path);
typedef _KritaBrushLoadPresetDart = int Function(
    Pointer<Void> handle, Pointer<Utf8> path);

typedef _KritaBrushSetSizeNative = Void Function(
    Pointer<Void> handle, Double size);
typedef _KritaBrushSetSizeDart = void Function(
    Pointer<Void> handle, double size);

typedef _KritaBrushSetColorNative = Void Function(
    Pointer<Void> handle, Uint32 argb);
typedef _KritaBrushSetColorDart = void Function(
    Pointer<Void> handle, int argb);

typedef _KritaBrushSetOpacityNative = Void Function(
    Pointer<Void> handle, Double opacity);
typedef _KritaBrushSetOpacityDart = void Function(
    Pointer<Void> handle, double opacity);

typedef _KritaBrushSetSpacingNative = Void Function(
    Pointer<Void> handle, Double spacing);
typedef _KritaBrushSetSpacingDart = void Function(
    Pointer<Void> handle, double spacing);

typedef _KritaBrushSetSmudgeNative = Void Function(
    Pointer<Void> handle, Double smudge);
typedef _KritaBrushSetSmudgeDart = void Function(
    Pointer<Void> handle, double smudge);

typedef _KritaBrushSetFlowNative = Void Function(
    Pointer<Void> handle, Double flow);
typedef _KritaBrushSetFlowDart = void Function(
    Pointer<Void> handle, double flow);

typedef _KritaBrushSetHardnessNative = Void Function(
    Pointer<Void> handle, Double hardness);
typedef _KritaBrushSetHardnessDart = void Function(
    Pointer<Void> handle, double hardness);

typedef _KritaBrushGenerateDabNative = Bool Function(Pointer<Void> handle,
    Pointer<BrushInputNative> input, Pointer<BrushDabNative> outDab);
typedef _KritaBrushGenerateDabDart = bool Function(Pointer<Void> handle,
    Pointer<BrushInputNative> input, Pointer<BrushDabNative> outDab);

typedef _KritaBrushReleaseDabNative = Void Function(
    Pointer<Void> handle, Pointer<BrushDabNative> dab);
typedef _KritaBrushReleaseDabDart = void Function(
    Pointer<Void> handle, Pointer<BrushDabNative> dab);

typedef _KritaBrushCleanupNative = Void Function(Pointer<Void> handle);
typedef _KritaBrushCleanupDart = void Function(Pointer<Void> handle);

typedef _KritaBrushLastErrorNative = Pointer<Utf8> Function(
    Pointer<Void> handle);
typedef _KritaBrushLastErrorDart = Pointer<Utf8> Function(
    Pointer<Void> handle);

typedef _KritaBrushVersionNative = Pointer<Utf8> Function();
typedef _KritaBrushVersionDart = Pointer<Utf8> Function();

typedef _KritaPresetScanNative = Int32 Function(
    Pointer<Void> handle, Pointer<Utf8> dir);
typedef _KritaPresetScanDart = int Function(
    Pointer<Void> handle, Pointer<Utf8> dir);

typedef _KritaPresetCountNative = Int32 Function(Pointer<Void> handle);
typedef _KritaPresetCountDart = int Function(Pointer<Void> handle);

typedef _KritaPresetEntryNative = Pointer<Utf8> Function(
    Pointer<Void> handle, Int32 index);
typedef _KritaPresetEntryDart = Pointer<Utf8> Function(
    Pointer<Void> handle, int index);

typedef _KritaPresetParamCountNative = Int32 Function(Pointer<Void> handle);
typedef _KritaPresetParamCountDart = int Function(Pointer<Void> handle);

typedef _KritaPresetParamEntryNative = Pointer<Utf8> Function(
    Pointer<Void> handle, Int32 index);
typedef _KritaPresetParamEntryDart = Pointer<Utf8> Function(
    Pointer<Void> handle, int index);

typedef _KritaBrushSetParamNative = Int32 Function(
    Pointer<Void> handle, Pointer<Utf8> name, Pointer<Utf8> value);
typedef _KritaBrushSetParamDart = int Function(
    Pointer<Void> handle, Pointer<Utf8> name, Pointer<Utf8> value);

typedef _KritaBrushGetCurveNative = Pointer<Utf8> Function(
    Pointer<Void> handle, Pointer<Utf8> key);
typedef _KritaBrushGetCurveDart = Pointer<Utf8> Function(
    Pointer<Void> handle, Pointer<Utf8> key);

typedef _KritaBrushSetCurveNative = Int32 Function(
    Pointer<Void> handle, Pointer<Utf8> key, Pointer<Utf8> curveXml);
typedef _KritaBrushSetCurveDart = int Function(
    Pointer<Void> handle, Pointer<Utf8> key, Pointer<Utf8> curveXml);

typedef _KritaBrushGetDoubleNative = Double Function(Pointer<Void> handle);
typedef _KritaBrushGetDoubleDart = double Function(Pointer<Void> handle);

typedef _KritaBrushGetEraserNative = Bool Function(Pointer<Void> handle);
typedef _KritaBrushGetEraserDart = bool Function(Pointer<Void> handle);

typedef _KritaBrushGetStringNative = Pointer<Utf8> Function(
    Pointer<Void> handle);
typedef _KritaBrushGetStringDart = Pointer<Utf8> Function(
    Pointer<Void> handle);

// ---------------------------------------------------------------------------
// Dynamic library loader.
// ---------------------------------------------------------------------------

/// Loads the `krita_bridge` native library for the current platform.
///
/// On Android and iOS the library is bundled inside the APK / app bundle
/// and the OS loader resolves it from the bare name. On desktop platforms
/// we try a set of candidate paths next to the executable / in the bundle
/// / via `LD_LIBRARY_PATH` and return the first one that opens.
///
/// Throws [StateError] when no candidate opens on a desktop platform, or
/// [UnsupportedError] on a platform the bridge does not support.
DynamicLibrary loadKritaBridge() {
  if (Platform.isAndroid || Platform.isIOS) {
    return DynamicLibrary.open('libkrita_bridge.so');
  }
  if (Platform.isWindows) {
    const candidates = <String>[
      'krita_bridge.dll',
      'lib\\krita_bridge.dll',
      '.\\krita_bridge.dll',
    ];
    for (final c in candidates) {
      try {
        return DynamicLibrary.open(c);
      } on ArgumentError {
        // continue
      }
    }
    throw StateError('krita_bridge.dll not found in search path');
  }
  if (Platform.isLinux) {
    const candidates = <String>[
      'lib/libkrita_bridge.so',
      'libkrita_bridge.so',
      './libkrita_bridge.so',
      'assets/native/linux/libkrita_bridge.so',
      'assets/native/libkrita_bridge.so',
      '../assets/native/linux/libkrita_bridge.so',
    ];
    for (final c in candidates) {
      try {
        return DynamicLibrary.open(c);
      } on ArgumentError {
        // continue
      } on OSError {
        // continue
      }
    }
    throw StateError('libkrita_bridge.so not found. Set LD_LIBRARY_PATH or '
        'place the .so next to the executable.');
  }
  if (Platform.isMacOS) {
    return DynamicLibrary.open('libkrita_bridge.dylib');
  }
  throw UnsupportedError('Krita bridge not supported on this platform');
}

// ---------------------------------------------------------------------------
// KritaNativeLibrary — typed symbol lookup table for the C ABI.
// ---------------------------------------------------------------------------

/// Resolves every export of the `krita_bridge` C ABI on a [DynamicLibrary]
/// and exposes them as typed Dart function fields.
///
/// Symbol lookup failures on individual exports are tolerated: the field
/// is left `null` and the high-level controller treats the missing symbol
/// as "this capability is not available on this bridge build" (the
/// portable / fallback bridges, and older engine artifacts that predate a
/// given ABI addition, intentionally do not export every symbol). This
/// mirrors the capability-probe contract documented in `krita_bridge.h`.
class KritaNativeLibrary {
  /// Constructs a native library wrapper by resolving every symbol on
  /// [lib]. The resolution is lazy per-symbol (each `late final` field
  /// does its own `lookupFunction` on first access) so a missing symbol
  /// does not crash construction — it only fails when the field is read.
  KritaNativeLibrary(this.lib);

  /// The underlying dynamic library handle.
  final DynamicLibrary lib;

  /// `krita_brush_init` — allocate a brush context.
  late final _KritaBrushInitDart init =
      lib.lookupFunction<_KritaBrushInitNative, _KritaBrushInitDart>(
          'krita_brush_init');

  /// `krita_brush_destroy` — release a brush context.
  late final _KritaBrushDestroyDart destroy =
      lib.lookupFunction<_KritaBrushDestroyNative, _KritaBrushDestroyDart>(
          'krita_brush_destroy');

  /// `krita_brush_load_preset` — load a .kpp preset.
  late final _KritaBrushLoadPresetDart loadPreset = lib
      .lookupFunction<_KritaBrushLoadPresetNative, _KritaBrushLoadPresetDart>(
          'krita_brush_load_preset');

  late final _KritaBrushSetSizeDart setSize =
      lib.lookupFunction<_KritaBrushSetSizeNative, _KritaBrushSetSizeDart>(
          'krita_brush_set_size');
  late final _KritaBrushSetColorDart setColor =
      lib.lookupFunction<_KritaBrushSetColorNative, _KritaBrushSetColorDart>(
          'krita_brush_set_color');
  late final _KritaBrushSetOpacityDart setOpacity = lib
      .lookupFunction<_KritaBrushSetOpacityNative, _KritaBrushSetOpacityDart>(
          'krita_brush_set_opacity');
  late final _KritaBrushSetSpacingDart setSpacing = lib
      .lookupFunction<_KritaBrushSetSpacingNative, _KritaBrushSetSpacingDart>(
          'krita_brush_set_spacing');
  late final _KritaBrushSetSmudgeDart setSmudge =
      lib.lookupFunction<_KritaBrushSetSmudgeNative, _KritaBrushSetSmudgeDart>(
          'krita_brush_set_smudge');
  late final _KritaBrushSetFlowDart setFlow =
      lib.lookupFunction<_KritaBrushSetFlowNative, _KritaBrushSetFlowDart>(
          'krita_brush_set_flow');
  late final _KritaBrushSetHardnessDart setHardness = lib
      .lookupFunction<_KritaBrushSetHardnessNative, _KritaBrushSetHardnessDart>(
          'krita_brush_set_hardness');

  late final _KritaBrushGenerateDabDart generateDab = lib.lookupFunction<
      _KritaBrushGenerateDabNative,
      _KritaBrushGenerateDabDart>('krita_brush_generate_dab');
  late final _KritaBrushReleaseDabDart releaseDab = lib
      .lookupFunction<_KritaBrushReleaseDabNative, _KritaBrushReleaseDabDart>(
          'krita_brush_release_dab');
  late final _KritaBrushCleanupDart cleanup =
      lib.lookupFunction<_KritaBrushCleanupNative, _KritaBrushCleanupDart>(
          'krita_brush_cleanup');
  late final _KritaBrushLastErrorDart lastError = lib
      .lookupFunction<_KritaBrushLastErrorNative, _KritaBrushLastErrorDart>(
          'krita_brush_last_error');

  /// `krita_brush_version` — bridge self-identification string. Resolved
  /// lazily; the controller catches [ArgumentError] on engines that
  /// predate the export.
  late final _KritaBrushVersionDart version =
      lib.lookupFunction<_KritaBrushVersionNative, _KritaBrushVersionDart>(
          'krita_brush_version');

  late final _KritaPresetScanDart presetScan =
      lib.lookupFunction<_KritaPresetScanNative, _KritaPresetScanDart>(
          'krita_brush_preset_scan');
  late final _KritaPresetCountDart presetCount =
      lib.lookupFunction<_KritaPresetCountNative, _KritaPresetCountDart>(
          'krita_brush_preset_count');
  late final _KritaPresetEntryDart presetNameAt =
      lib.lookupFunction<_KritaPresetEntryNative, _KritaPresetEntryDart>(
          'krita_brush_preset_name');
  late final _KritaPresetEntryDart presetFamilyAt =
      lib.lookupFunction<_KritaPresetEntryNative, _KritaPresetEntryDart>(
          'krita_brush_preset_family');
  late final _KritaPresetEntryDart presetPathAt =
      lib.lookupFunction<_KritaPresetEntryNative, _KritaPresetEntryDart>(
          'krita_brush_preset_path');

  late final _KritaPresetParamCountDart presetParamCount = lib.lookupFunction<
      _KritaPresetParamCountNative,
      _KritaPresetParamCountDart>('krita_brush_preset_param_count');
  late final _KritaPresetParamEntryDart presetParamNameAt = lib.lookupFunction<
      _KritaPresetParamEntryNative,
      _KritaPresetParamEntryDart>('krita_brush_preset_param_name');
  late final _KritaPresetParamEntryDart presetParamValueAt = lib.lookupFunction<
      _KritaPresetParamEntryNative,
      _KritaPresetParamEntryDart>('krita_brush_preset_param_value');

  late final _KritaBrushSetParamDart setParam =
      lib.lookupFunction<_KritaBrushSetParamNative, _KritaBrushSetParamDart>(
          'krita_brush_set_param');
  late final _KritaBrushGetCurveDart getCurve =
      lib.lookupFunction<_KritaBrushGetCurveNative, _KritaBrushGetCurveDart>(
          'krita_brush_get_curve');
  late final _KritaBrushSetCurveDart setCurve =
      lib.lookupFunction<_KritaBrushSetCurveNative, _KritaBrushSetCurveDart>(
          'krita_brush_set_curve');

  late final _KritaBrushGetDoubleDart getSize =
      lib.lookupFunction<_KritaBrushGetDoubleNative, _KritaBrushGetDoubleDart>(
          'krita_brush_get_size');
  late final _KritaBrushGetDoubleDart getOpacity = lib
      .lookupFunction<_KritaBrushGetDoubleNative, _KritaBrushGetDoubleDart>(
          'krita_brush_get_opacity');
  late final _KritaBrushGetDoubleDart getSpacing = lib
      .lookupFunction<_KritaBrushGetDoubleNative, _KritaBrushGetDoubleDart>(
          'krita_brush_get_spacing');
  late final _KritaBrushGetDoubleDart getHardness = lib
      .lookupFunction<_KritaBrushGetDoubleNative, _KritaBrushGetDoubleDart>(
          'krita_brush_get_hardness');
  late final _KritaBrushGetDoubleDart getFlow =
      lib.lookupFunction<_KritaBrushGetDoubleNative, _KritaBrushGetDoubleDart>(
          'krita_brush_get_flow');
  late final _KritaBrushGetDoubleDart getSmudge =
      lib.lookupFunction<_KritaBrushGetDoubleNative, _KritaBrushGetDoubleDart>(
          'krita_brush_get_smudge');

  late final _KritaBrushGetEraserDart getEraser =
      lib.lookupFunction<_KritaBrushGetEraserNative, _KritaBrushGetEraserDart>(
          'krita_brush_get_eraser');
  late final _KritaBrushGetStringDart getPresetName = lib.lookupFunction<
      _KritaBrushGetStringNative,
      _KritaBrushGetStringDart>('krita_brush_get_preset_name');
  late final _KritaBrushGetStringDart getPaintopId = lib.lookupFunction<
      _KritaBrushGetStringNative,
      _KritaBrushGetStringDart>('krita_brush_get_paintop_id');

  /// Tries to resolve [symbol] and returns `true` when present. Used by
  /// the engine capability probe ([KritaEngine.capabilities]) to detect
  /// which ABI additions the loaded bridge actually exports.
  bool hasSymbol(String symbol) {
    try {
      lib.lookup(symbol);
      return true;
    } on ArgumentError {
      return false;
    } on UnsupportedError {
      return false;
    }
  }
}
