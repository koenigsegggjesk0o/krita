// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// project_state.dart — Pure Riverpod state for the open project.
//
// Holds metadata about the currently-open .feather document: name,
// created / modified timestamps, thumbnail bytes, dirty flag, and
// auto-save configuration. The actual document bytes / strokes / camera
// live in [SceneState] / [CameraState]; this file holds the wrapper
// metadata the title bar, file menu, and auto-save scheduler read.
//
// Pure value type — emits through a Riverpod [Notifier].

/// Auto-save configuration.
class AutoSaveConfig {
  const AutoSaveConfig({
    this.enabled = false,
    this.intervalSeconds = 120,
    this.keepLastN = 5,
  });

  /// Whether auto-save is on.
  final bool enabled;

  /// Seconds between auto-save ticks.
  final int intervalSeconds;

  /// How many recent auto-save snapshots to keep on disk.
  final int keepLastN;

  AutoSaveConfig copyWith({
    bool? enabled,
    int? intervalSeconds,
    int? keepLastN,
  }) =>
      AutoSaveConfig(
        enabled: enabled ?? this.enabled,
        intervalSeconds: intervalSeconds ?? this.intervalSeconds,
        keepLastN: keepLastN ?? this.keepLastN,
      );

  @override
  bool operator ==(Object other) =>
      identical(this, other) ||
      other is AutoSaveConfig &&
          other.enabled == enabled &&
          other.intervalSeconds == intervalSeconds &&
          other.keepLastN == keepLastN;

  @override
  int get hashCode => Object.hash(enabled, intervalSeconds, keepLastN);
}

/// Pure project metadata state.
class ProjectState {
  const ProjectState({
    this.name = 'Untitled',
    this.filePath,
    this.created,
    this.modified,
    this.thumbnailBytes,
    this.isDirty = false,
    this.autoSave = const AutoSaveConfig(),
    this.lastAutoSaveAt,
  });

  /// Display name (without extension).
  final String name;

  /// Absolute path of the file the project was last saved to, or
  /// `null` when the project has never been saved.
  final String? filePath;

  /// Creation timestamp (UTC). `null` until the project is first saved.
  final DateTime? created;

  /// Last-modified timestamp (UTC). `null` until the project is first
  /// saved.
  final DateTime? modified;

  /// Optional PNG thumbnail bytes (used by the recent-projects list).
  final List<int>? thumbnailBytes;

  /// True when there are unsaved changes.
  final bool isDirty;

  /// Auto-save configuration.
  final AutoSaveConfig autoSave;

  /// Timestamp of the last successful auto-save, or `null`.
  final DateTime? lastAutoSaveAt;

  /// Suggested file name (name + '.feather').
  String get fileName => '$name.feather';

  /// True when the project has been saved at least once.
  bool get hasPath => filePath != null && filePath!.isNotEmpty;

  ProjectState copyWith({
    String? name,
    String? filePath,
    DateTime? created,
    DateTime? modified,
    List<int>? thumbnailBytes,
    bool? isDirty,
    AutoSaveConfig? autoSave,
    DateTime? lastAutoSaveAt,
  }) =>
      ProjectState(
        name: name ?? this.name,
        filePath: filePath ?? this.filePath,
        created: created ?? this.created,
        modified: modified ?? this.modified,
        thumbnailBytes: thumbnailBytes ?? this.thumbnailBytes,
        isDirty: isDirty ?? this.isDirty,
        autoSave: autoSave ?? this.autoSave,
        lastAutoSaveAt: lastAutoSaveAt ?? this.lastAutoSaveAt,
      );

  /// Marks the project as dirty with an updated modification time.
  ProjectState markDirty() =>
      copyWith(isDirty: true, modified: DateTime.timestamp());

  /// Marks the project as saved at [path] with a fresh modification
  /// time and an optional new thumbnail.
  ProjectState markSaved(
    String path, {
    List<int>? thumbnail,
  }) =>
      ProjectState(
        name: _basename(path),
        filePath: path,
        created: created ?? DateTime.timestamp(),
        modified: DateTime.timestamp(),
        thumbnailBytes: thumbnail ?? thumbnailBytes,
        isDirty: false,
        autoSave: autoSave,
        lastAutoSaveAt: lastAutoSaveAt,
      );

  /// Marks the most recent auto-save tick.
  ProjectState markAutoSaved() =>
      copyWith(lastAutoSaveAt: DateTime.timestamp());

  Map<String, dynamic> toJson() => {
        'name': name,
        'filePath': filePath,
        'created': created?.toIso8601String(),
        'modified': modified?.toIso8601String(),
        'isDirty': isDirty,
        'autoSave': {
          'enabled': autoSave.enabled,
          'intervalSeconds': autoSave.intervalSeconds,
          'keepLastN': autoSave.keepLastN,
        },
        'lastAutoSaveAt': lastAutoSaveAt?.toIso8601String(),
      };

  @override
  bool operator ==(Object other) =>
      identical(this, other) ||
      other is ProjectState &&
          other.name == name &&
          other.filePath == filePath &&
          other.created == created &&
          other.modified == modified &&
          other.isDirty == isDirty &&
          other.autoSave == autoSave &&
          other.lastAutoSaveAt == lastAutoSaveAt &&
          _listEq(other.thumbnailBytes, thumbnailBytes);

  @override
  int get hashCode => Object.hash(
        name,
        filePath,
        created,
        modified,
        isDirty,
        autoSave,
        lastAutoSaveAt,
        thumbnailBytes == null ? 0 : Object.hashAll(thumbnailBytes!),
      );
}

bool _listEq<T>(List<T>? a, List<T>? b) {
  if (a == null && b == null) return true;
  if (a == null || b == null) return false;
  if (a.length != b.length) return false;
  for (var i = 0; i < a.length; i++) {
    if (a[i] != b[i]) return false;
  }
  return true;
}

String _basename(String path) {
  final sep = path.contains('\\') ? '\\' : '/';
  final last = path.split(sep).last;
  return last.endsWith('.feather')
      ? last.substring(0, last.length - '.feather'.length)
      : last;
}
