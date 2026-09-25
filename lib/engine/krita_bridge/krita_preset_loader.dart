// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// krita_preset_loader.dart — Preset loading for the Krita brush engine.
//
// The REAL preset-loading path runs entirely inside the C bridge
// (`krita_brush_load_preset` in krita_bridge_real.cpp): it unpacks the
// .kpp container (ZIP KoStore OR legacy PNG-with-zTXt OR bare XML),
// hands the extracted preset XML to Krita's own `KisBrush::fromXML`
// / paintop-settings parser, and records the parsed `(name, value)`
// pairs on the engine's param map of record. The Dart side only feeds
// it a path and reads the parsed result back.
//
// This file wraps that flow with a higher-level surface:
//   * [loadPreset] — load a .kpp into a [KritaBrushController], then
//     snapshot the parsed params into a [KritaLoadedPreset] record.
//   * [scanDirectory] — scan a directory tree for .kpp presets through
//     the engine's own container parser and return per-preset metadata.
//   * [applyParamMap] — apply a [KritaParamMap] of live edits to a
//     loaded brush (the live param editor surface).
//   * [parsePresetNameFromPath] / [looksLikeKppPath] — small pure-Dart
//     helpers for the file picker (no engine roundtrip needed).
//
// The loader does NOT re-implement the .kpp container unpacking in
// Dart — that path is CI-proven inside the C bridge and re-implementing
// it would diverge from the engine's own parser. The Dart side trusts
// the bridge's parse and only adds orchestration + UI metadata.

import 'dart:io';

import 'krita_bindings.dart';
import 'krita_brush_controller.dart';
import 'krita_param_map.dart';

/// A loaded preset snapshot: the file path, the engine-parsed display
/// name, the declared paintop family, and the parsed param map.
class KritaLoadedPreset {
  /// Constructs a loaded-preset snapshot.
  const KritaLoadedPreset({
    required this.path,
    required this.name,
    required this.paintopId,
    required this.params,
    required this.isEraser,
  });

  /// Absolute file path the preset was loaded from.
  final String path;

  /// Engine-parsed display name (root `name` attribute, else file
  /// stem). Empty when the bridge could not parse one.
  final String name;

  /// Declared paintop family ("paintbrush", "eraser", "spray", ...).
  /// Empty when the preset omits the attribute.
  final String paintopId;

  /// The parsed paintop-settings param map (replace-or-append
  /// semantics). Empty when the bridge is the portable/fallback build.
  final KritaParamMap params;

  /// True when the preset is an eraser preset (CompositeOp=erase /
  /// Krita/erase / EraserMode).
  final bool isEraser;

  @override
  String toString() =>
      'KritaLoadedPreset($name, paintop=$paintopId, '
      'params=${params.length}, eraser=$isEraser)';
}

/// Outcome of a [KritaPresetLoader.scanDirectory] call.
class KritaPresetScanResult {
  /// Constructs a scan result.
  const KritaPresetScanResult({
    required this.directory,
    required this.presets,
    required this.engineCode,
  });

  /// The directory that was scanned.
  final String directory;

  /// The parsed preset entries, in scan order (sorted by path).
  final List<KritaPresetInfo> presets;

  /// The raw return code from `krita_brush_preset_scan` (>= 0 is the
  /// preset count, -1 bad arguments, -2 missing directory). Useful for
  /// diagnostics.
  final int engineCode;

  /// True when the scan succeeded (engineCode >= 0).
  bool get ok => engineCode >= 0;

  @override
  String toString() =>
      'KritaPresetScanResult(${presets.length} presets, code=$engineCode)';
}

/// High-level preset loader. Wraps [KritaBrushController.loadPreset] /
/// [KritaBrushController.scanPresetFamilies] with snapshot helpers.
///
/// The loader is stateless — every method takes the brush controller
/// it should operate on, so one loader instance can serve multiple
/// brushes (or the same brush across preset swaps).
class KritaPresetLoader {
  /// Constructs a preset loader. Stateless — no per-instance state.
  const KritaPresetLoader();

  /// Loads the .kpp preset at [path] into [brush] through the REAL
  /// engine's container parser, then snapshots the engine-parsed
  /// metadata into a [KritaLoadedPreset]. Returns `null` when the load
  /// fails (call [KritaBrushController.lastError] on [brush] for the
  /// diagnostic).
  ///
  /// On success the returned snapshot holds a fresh [KritaParamMap]
  /// copy — edits to it do not affect the engine's map of record. Use
  /// [applyParamMap] to push live edits back into the engine.
  KritaLoadedPreset? loadPreset(KritaBrushController brush, String path) {
    if (!brush.loadPreset(path)) return null;
    return KritaLoadedPreset(
      path: path,
      name: brush.currentPresetName,
      paintopId: brush.currentPaintopId,
      params: brush.presetParams(),
      isEraser: brush.isEraserPreset,
    );
  }

  /// Scans [directory] recursively for .kpp presets through the REAL
  /// engine's container parser and returns per-preset metadata. The
  /// scan runs engine-side (the same container parser
  /// `krita_brush_load_preset` uses), so unparsable files are skipped
  /// silently — every entry in the result is a loadable preset.
  ///
  /// Pass [brush] only when you already hold a brush controller; the
  /// loader allocates a throwaway brush for the scan otherwise (the
  /// scan does not need a preset loaded, just a handle).
  KritaPresetScanResult scanDirectory(String directory,
      {KritaBrushController? brush}) {
    final owner = brush;
    KritaBrushController? temp;
    var code = 0;
    List<KritaPresetInfo> entries;
    if (owner != null) {
      code = owner.scanPresetFamilies(directory);
      entries = code >= 0 ? owner.scannedPresets() : const <KritaPresetInfo>[];
    } else {
      temp = KritaBrushController();
      try {
        code = temp.scanPresetFamilies(directory);
        entries =
            code >= 0 ? temp.scannedPresets() : const <KritaPresetInfo>[];
      } finally {
        temp.dispose();
      }
    }
    return KritaPresetScanResult(
      directory: directory,
      presets: entries,
      engineCode: code,
    );
  }

  /// Applies a [KritaParamMap] of live edits to [brush] (the live param
  /// editor surface). Each entry is pushed through
  /// [KritaBrushController.setParam] in insertion order. Returns the
  /// number of entries the engine actually accepted (recorded on its
  /// map of record with replace-or-append semantics).
  ///
  /// Entries the engine rejects (no preset loaded, or the bridge
  /// predates the `krita_brush_set_param` export) are counted as
  /// rejected — the caller can compare the returned count to
  /// [KritaParamMap.length] to detect partial failures.
  int applyParamMap(KritaBrushController brush, KritaParamMap edits) {
    if (edits.isEmpty) return 0;
    var accepted = 0;
    for (final entry in edits.entries) {
      if (brush.setParam(entry.key, entry.value)) accepted++;
    }
    return accepted;
  }

  /// Returns the preset display name parsed from a file path: the file
  /// stem with `_` replaced by spaces, title-cased. Used by the file
  /// picker to show a friendly name without round-tripping through the
  /// engine (the engine's own parse may differ — `currentPresetName`
  /// after a real load is authoritative).
  String parsePresetNameFromPath(String path) {
    final stem = path
        .split(Platform.pathSeparator)
        .last
        .split('/')
        .last
        .replaceAll('.kpp', '')
        .replaceAll('.KPP', '');
    if (stem.isEmpty) return 'Untitled';
    final spaced = stem.replaceAll('_', ' ');
    if (spaced.isEmpty) return stem;
    final first = spaced[0].toUpperCase();
    return first + spaced.substring(1);
  }

  /// True when [path] looks like a Krita preset file (.kpp extension,
  /// case-insensitive). Used by the file picker to filter.
  bool looksLikeKppPath(String path) {
    final lower = path.toLowerCase();
    return lower.endsWith('.kpp');
  }
}
