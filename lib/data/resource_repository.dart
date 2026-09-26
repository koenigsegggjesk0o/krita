// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// resource_repository.dart — Imported-resource management.
//
// A resource is any external asset the user has imported into the app:
// an image (brush tip, reference photo, texture), a 3D model (OBJ,
// glTF, STL), or a saved guide template. The repository owns the
// resource index file (a JSON manifest at `feather_resources/index.json`)
// and the resources/ directory where the imported bytes live.
//
// Resources are content-addressed: the file's SHA-256 hash is the id,
// so re-importing the same file is a no-op. The manifest tracks the
// display name, kind, MIME type, size, and import timestamp.

import 'dart:convert';
import 'dart:io';

import 'package:feather_krita/data/models/resource_model.dart';
import 'package:feather_krita/io/app_dirs.dart' show appDataRoot;

/// Imported-resource manager.
class ResourceRepository {
  ResourceRepository();

  /// Root directory for imported resources (created on demand).
  Directory get resourcesDir {
    final dir = Directory(
        '${appDataRoot().path}${Platform.pathSeparator}feather_resources');
    if (!dir.existsSync()) dir.createSync(recursive: true);
    return dir;
  }

  /// Path of the JSON manifest.
  File get _manifestFile =>
      File('${resourcesDir.path}${Platform.pathSeparator}index.json');

  /// Loads the manifest, returning an empty list when missing or
  /// corrupt.
  Future<List<ResourceModel>> list() async {
    final file = _manifestFile;
    if (!file.existsSync()) return const <ResourceModel>[];
    try {
      final json = await file.readAsString();
      final decoded = jsonDecode(json);
      if (decoded is! List) return const <ResourceModel>[];
      return decoded
          .whereType<Map<String, dynamic>>()
          .map(ResourceModel.fromJson)
          .toList();
    } catch (_) {
      return const <ResourceModel>[];
    }
  }

  /// Writes [resources] back to the manifest atomically (temp file +
  /// rename).
  Future<void> _writeManifest(List<ResourceModel> resources) async {
    final json = const JsonEncoder.withIndent('  ')
        .convert(resources.map((r) => r.toJson()).toList());
    final tmp = File('${_manifestFile.path}.tmp');
    await tmp.writeAsString(json, flush: true);
    await tmp.rename(_manifestFile.path);
  }

  /// Imports [source] into the resources directory, returning the
  /// resulting [ResourceModel]. Re-importing the same file returns
  /// the existing entry unchanged (content-addressed).
  Future<ResourceModel> importFile(
    File source, {
    required ResourceKind kind,
    String? displayName,
    Map<String, String> metadata = const <String, String>{},
  }) async {
    if (!source.existsSync()) {
      throw FileSystemException('Source file not found', source.path);
    }
    final bytes = await source.readAsBytes();
    final id = _contentHash(bytes);
    final existing = (await list()).where((r) => r.id == id).firstOrNull;
    if (existing != null) return existing;

    final name = displayName ?? _basename(source.uri.pathSegments.last);
    final ext = source.uri.pathSegments.last.split('.').last.toLowerCase();
    final mimeType = _mimeTypeFor(ext, kind);
    final stored = File('${resourcesDir.path}${Platform.pathSeparator}$id.$ext');
    await stored.writeAsBytes(bytes, flush: true);

    final model = ResourceModel(
      id: id,
      kind: kind,
      name: name,
      path: stored.path,
      mimeType: mimeType,
      sizeBytes: bytes.length,
      importedAt: DateTime.timestamp(),
      metadata: metadata,
    );
    final next = [...await list(), model];
    await _writeManifest(next);
    return model;
  }

  /// Returns the resource with [id], or `null` when not present.
  Future<ResourceModel?> byId(String id) async =>
      (await list()).where((r) => r.id == id).firstOrNull;

  /// Removes a resource: deletes the stored file (when present) and
  /// removes the manifest entry. Returns true when something was
  /// removed.
  Future<bool> remove(String id) async {
    final current = await list();
    final match = current.where((r) => r.id == id).firstOrNull;
    if (match == null) return false;
    final file = File(match.path);
    if (file.existsSync()) {
      await file.delete();
    }
    final next = current.where((r) => r.id != id).toList();
    await _writeManifest(next);
    return true;
  }

  /// Renames a resource (display name only — file path is unchanged).
  Future<ResourceModel?> rename(String id, String newName) async {
    final current = await list();
    var updated = <ResourceModel>[];
    ResourceModel? out;
    for (final r in current) {
      if (r.id == id) {
        out = r.copyWith(name: newName);
        updated.add(out);
      } else {
        updated.add(r);
      }
    }
    if (out == null) return null;
    await _writeManifest(updated);
    return out;
  }

  /// Returns the resources of [kind], in import order.
  Future<List<ResourceModel>> byKind(ResourceKind kind) async =>
      (await list()).where((r) => r.kind == kind).toList();

  /// Indexes the Krita stock brush presets extracted by
  /// [KritaResources.ensureImported] into the manifest WITHOUT copying
  /// bytes — the .kpp files already live under [resourcesDir] at
  /// `paintoppresets/`, so this registers a manifest entry pointing at
  /// the existing path. Each .kpp gets kind = [ResourceKind.brushPreset],
  /// a content-hash id (so re-indexing is idempotent + dedupes), and the
  /// existing on-disk path.
  ///
  /// Called by [KritaResources] after a successful first-run extraction
  /// so the rest of the app can query "which stock presets are on disk?"
  /// via [byKind] / [byId] instead of re-scanning the directory tree.
  /// Returns the number of newly-registered entries (0 when the presets
  /// dir is missing or already fully indexed).
  Future<int> indexExtractedPresets() async {
    final presetsDir = Directory(
        '${resourcesDir.path}${Platform.pathSeparator}paintoppresets');
    if (!presetsDir.existsSync()) return 0;
    final existing = await list();
    final existingIds = existing.map((r) => r.id).toSet();
    var added = 0;
    final entries = <ResourceModel>[...existing];
    await for (final entity in presetsDir.list(recursive: true)) {
      if (entity is! File) continue;
      if (!entity.path.toLowerCase().endsWith('.kpp')) continue;
      final bytes = await entity.readAsBytes();
      final id = _contentHash(bytes);
      if (existingIds.contains(id)) continue;
      final name = _basename(entity.uri.pathSegments.last);
      entries.add(ResourceModel(
        id: id,
        kind: ResourceKind.brushPreset,
        name: name,
        path: entity.path,
        mimeType: 'application/x-krita-brushpreset',
        sizeBytes: bytes.length,
        importedAt: DateTime.timestamp(),
      ));
      added++;
    }
    if (added > 0) await _writeManifest(entries);
    return added;
  }

  String _basename(String fileName) {
    final dot = fileName.lastIndexOf('.');
    return dot > 0 ? fileName.substring(0, dot) : fileName;
  }

  String _mimeTypeFor(String ext, ResourceKind kind) {
    switch (ext) {
      case 'png':
        return 'image/png';
      case 'jpg':
      case 'jpeg':
        return 'image/jpeg';
      case 'webp':
        return 'image/webp';
      case 'obj':
        return 'model/obj';
      case 'gltf':
        return 'model/gltf+json';
      case 'glb':
        return 'model/gltf-binary';
      case 'stl':
        return 'model/stl';
      case 'kpp':
        return 'application/x-krita-brushpreset';
      default:
        return kind == ResourceKind.image
            ? 'image/*'
            : (kind == ResourceKind.model ? 'model/*' : 'application/octet-stream');
    }
  }

  /// Returns a 16-char content-addressed id for [bytes].
  ///
  /// Implemented inline as FNV-1a (64-bit) plus the byte length, then
  /// hex-encoded — collision-resistant enough for the resource index
  /// (a user importing ~10^9 distinct files would have a non-trivial
  /// collision probability; in practice the index stays in the
  /// thousands). Avoids the `crypto` package dependency.
  String _contentHash(List<int> bytes) {
    var hash = 0xCBF29CE484222325; // FNV-1a 64-bit offset basis
    const prime = 0x100000001B3; // FNV-1a 64-bit prime
    for (final b in bytes) {
      hash = (hash ^ b) * prime & 0xFFFFFFFFFFFFFFFF;
    }
    hash ^= bytes.length;
    return hash.toRadixString(16).padLeft(16, '0').substring(0, 16);
  }
}
