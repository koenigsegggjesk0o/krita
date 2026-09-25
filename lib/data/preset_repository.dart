// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// preset_repository.dart — Brush preset storage.
//
// Loads presets from three sources, in priority order:
//   1. Bundled .kpp assets shipped with the app (read-only).
//   2. The user's feather_presets directory (writable; user can drop
//      .kpp files here, or save custom presets through the UI).
//   3. The full Krita resource library (extracted on first run by
//      [KritaResources] into feather_resources/paintoppresets).
//
// Custom user-saved presets are written as a small JSON sidecar
// (`<id>.preset.json`) next to the original .kpp so the runtime
// [BrushPreset] type can be reconstructed cheaply on next launch
// without re-parsing the XML.

import 'dart:convert';
import 'dart:io';

import 'package:feather_krita/data/models/preset_model.dart';
import 'package:feather_krita/io/app_dirs.dart' show presetsDir;
import 'package:feather_krita/io/krita_resources.dart' show KritaResources;
import 'package:feather_krita/models/brush_preset.dart' show BrushPreset;

/// Brush preset storage.
class PresetRepository {
  PresetRepository();

  /// Returns all preset directories that should be scanned, in
  /// priority order. Missing directories are skipped (no error).
  List<Directory> searchDirs() {
    final dirs = <Directory>[presetsDir()];
    final krita = KritaResources.importedPresetsDirPath();
    if (krita != null) dirs.add(Directory(krita));
    return dirs.where((d) => d.existsSync()).toList();
  }

  /// Lists all presets found in any search directory.
  /// Files that fail to parse are skipped (errors collected into
  /// [errors] when provided).
  Future<List<BrushPreset>> listAll({
    void Function(String path, Object error)? errors,
  }) async {
    final out = <BrushPreset>[];
    final seenIds = <String>{};
    for (final dir in searchDirs()) {
      await for (final entity in dir.list(recursive: true)) {
        if (entity is! File) continue;
        final lower = entity.path.toLowerCase();
        if (!lower.endsWith('.kpp')) continue;
        try {
          final preset = await BrushPreset.loadFromFile(entity.path);
          if (seenIds.add(preset.id)) {
            out.add(preset);
          }
        } catch (e) {
          errors?.call(entity.path, e);
        }
      }
    }
    return out;
  }

  /// Loads a single preset by id, scanning all search directories.
  /// Returns `null` when no matching .kpp file is found.
  Future<BrushPreset?> loadById(String id) async {
    for (final dir in searchDirs()) {
      final candidate = File('${dir.path}${Platform.pathSeparator}$id.kpp');
      if (candidate.existsSync()) {
        return BrushPreset.loadFromFile(candidate.path);
      }
    }
    return null;
  }

  /// Saves a custom preset's JSON sidecar to the user presets
  /// directory. The .kpp itself is NOT written here — callers that
  /// want to persist a brand-new preset should write the .kpp first
  /// then call this for the JSON metadata.
  Future<File> saveSidecar(PresetModel model) async {
    final dir = presetsDir();
    final file = File('${dir.path}${Platform.pathSeparator}${model.id}.preset.json');
    final json = const JsonEncoder.withIndent('  ').convert(model.toJson());
    await file.writeAsString(json, flush: true);
    return file;
  }

  /// Loads the sidecar JSON for [id], when one exists.
  Future<PresetModel?> loadSidecar(String id) async {
    final dir = presetsDir();
    final file = File('${dir.path}${Platform.pathSeparator}$id.preset.json');
    if (!file.existsSync()) return null;
    final json = await file.readAsString();
    final decoded = jsonDecode(json);
    if (decoded is! Map<String, dynamic>) return null;
    return PresetModel.fromJson(decoded);
  }

  /// Toggles the favorite flag on a sidecar (creating one when the
  /// preset has no sidecar yet). Returns the updated model.
  Future<PresetModel> toggleFavorite(BrushPreset preset) async {
    final existing = await loadSidecar(preset.id);
    final model = existing ??
        PresetModel.fromBrushPreset(preset);
    final updated = model.copyWith(isFavorite: !model.isFavorite);
    await saveSidecar(updated);
    return updated;
  }

  /// Returns the list of preset ids the user has favorited.
  Future<List<String>> favoriteIds() async {
    final dir = presetsDir();
    final ids = <String>[];
    if (!dir.existsSync()) return ids;
    await for (final entity in dir.list()) {
      if (entity is! File) continue;
      final name = entity.uri.pathSegments.last;
      if (!name.endsWith('.preset.json')) continue;
      try {
        final json = await entity.readAsString();
        final decoded = jsonDecode(json);
        if (decoded is Map<String, dynamic> &&
            decoded['isFavorite'] == true) {
          final id = decoded['id'];
          if (id is String) ids.add(id);
        }
      } catch (_) {
        // Skip malformed sidecar.
      }
    }
    return ids;
  }

  /// Deletes the sidecar JSON for [id], if any. The original .kpp is
  /// NOT touched (callers should manage the .kpp separately).
  Future<bool> deleteSidecar(String id) async {
    final dir = presetsDir();
    final file = File('${dir.path}${Platform.pathSeparator}$id.preset.json');
    if (!file.existsSync()) return false;
    await file.delete();
    return true;
  }
}
