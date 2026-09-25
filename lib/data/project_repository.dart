// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// project_repository.dart — .feather project file I/O.
//
// Reads / writes [ProjectModel] documents to the user's projects
// directory. Two on-disk formats are supported:
//
//   - JSON mode  (extension `.feather`): a single UTF-8 JSON document.
//     Compatible with the legacy [FeatherProjectDocument] writer in
//     lib/io/feather_project.dart — the repository reads either shape
//     and writes the new model's shape.
//   - Binary mode (extension `.featherb`): a length-prefixed binary
//     container produced by [FeatherProjectBinary] in lib/io/. Smaller
//     and faster to load for large projects; not human-readable.
//
// Both formats carry the same logical content (scene + camera + brush +
// metadata); the repository picks one based on the file extension and
// falls back to JSON when the binary format's magic header is absent.

import 'dart:convert';
import 'dart:io';
import 'dart:typed_data';

import 'package:feather_krita/data/models/project_model.dart';
import 'package:feather_krita/io/app_dirs.dart';
import 'package:feather_krita/io/feather_project.dart'
    show FeatherProjectBinary;

/// Project file I/O.
class ProjectRepository {
  ProjectRepository({Directory? baseDir}) : _baseDir = baseDir;

  final Directory? _baseDir;

  /// Root directory projects are saved to / loaded from. Defaults to
  /// `~/feather_projects` (created on demand).
  Directory get projectsDir {
    final dir = _baseDir ?? Directory('${appDataRoot().path}'
        '${Platform.pathSeparator}'
        'feather_projects');
    if (!dir.existsSync()) dir.createSync(recursive: true);
    return dir;
  }

  /// Suggests a fresh project path for [name] under [projectsDir] with
  /// the given [format].
  String suggestPath(String name, {ProjectFileFormat format = ProjectFileFormat.json}) {
    final safe = name.replaceAll(RegExp(r'[\\/:*?"<>|]'), '_');
    final ext = format == ProjectFileFormat.binary ? 'featherb' : 'feather';
    final sep = Platform.pathSeparator;
    final cleanDir =
        projectsDir.path.endsWith(sep) ? projectsDir.path : '${projectsDir.path}$sep';
    return '$cleanDir$safe.$ext';
  }

  /// Saves [model] to [model.filePath] (or to [suggestPath] when null).
  /// Returns the path written to.
  Future<String> save(ProjectModel model, {ProjectFileFormat? format}) async {
    final path = model.filePath ?? suggestPath(model.name, format: format ?? ProjectFileFormat.json);
    final file = File(path);
    await file.parent.create(recursive: true);
    final detectedFormat = format ??
        (path.toLowerCase().endsWith('.featherb')
            ? ProjectFileFormat.binary
            : ProjectFileFormat.json);
    if (detectedFormat == ProjectFileFormat.binary) {
      final bytes = FeatherProjectBinary.toBytes(model);
      await file.writeAsBytes(bytes, flush: true);
    } else {
      final json = const JsonEncoder.withIndent('  ').convert(model.toJson());
      await file.writeAsString(json, flush: true);
    }
    return path;
  }

  /// Loads a project from [path]. The format is auto-detected from the
  /// file's first 4 bytes (binary magic) or extension.
  Future<ProjectModel> load(String path) async {
    final file = File(path);
    if (!await file.exists()) {
      throw FileSystemException('Project file not found', path);
    }
    final bytes = await file.readAsBytes();
    if (FeatherProjectBinary.isBinary(bytes)) {
      return FeatherProjectBinary.fromBytes(bytes).copyWith(filePath: path);
    }
    final json = utf8.decode(bytes, allowMalformed: true);
    final decoded = jsonDecode(json);
    if (decoded is! Map<String, dynamic>) {
      throw const FormatException('Project root must be a JSON object.');
    }
    return ProjectModel.fromJson(decoded).copyWith(filePath: path);
  }

  /// Lists all `.feather` / `.featherb` files in [projectsDir], newest
  /// first by file mtime.
  Future<List<File>> listProjects() async {
    if (!await projectsDir.exists()) return const <File>[];
    final files = <File>[];
    await for (final entity in projectsDir.list()) {
      if (entity is! File) continue;
      final lower = entity.path.toLowerCase();
      if (lower.endsWith('.feather') || lower.endsWith('.featherb')) {
        files.add(entity);
      }
    }
    files.sort((a, b) => b.statSync().modified.compareTo(a.statSync().modified));
    return files;
  }

  /// Deletes [path] if it exists. Returns true when a file was deleted.
  Future<bool> delete(String path) async {
    final file = File(path);
    if (!await file.exists()) return false;
    await file.delete();
    return true;
  }

  /// Atomically renames a project file from [from] to [to]. Useful when
  /// the user renames a project from inside the app.
  Future<void> rename(String from, String to) async {
    final src = File(from);
    if (!await src.exists()) {
      throw FileSystemException('Source project not found', from);
    }
    await src.rename(to);
  }

  /// Reads the thumbnail bytes embedded in a project file (when
  /// present), without parsing the whole document.
  Future<Uint8List?> readThumbnail(String path) async {
    final file = File(path);
    if (!await file.exists()) return null;
    final bytes = await file.readAsBytes();
    if (FeatherProjectBinary.isBinary(bytes)) {
      return FeatherProjectBinary.readThumbnail(bytes);
    }
    return null; // JSON format carries thumbnail in a sidecar file.
  }
}

/// On-disk format for a .feather project.
enum ProjectFileFormat {
  /// Human-readable JSON (`.feather`).
  json,

  /// Length-prefixed binary container (`.featherb`).
  binary,
}
