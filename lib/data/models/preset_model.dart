// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// preset_model.dart — Persistence model for a brush preset.
//
// The runtime brush-preset type lives in lib/models/brush_preset.dart and
// carries a lot of parse-time state (XML backing, file paths, thumbnail
// bytes). This persistence model is the lean JSON-friendly subset the
// [PresetRepository] writes to and reads from the user's preset
// directory. The repository converts between the two at I/O time.

import 'package:feather_krita/models/brush_preset.dart' show BrushPreset;

/// Persistence model for a brush preset.
class PresetModel {
  const PresetModel({
    required this.id,
    required this.name,
    required this.paintopId,
    this.settings = const <String, String>{},
    this.description = '',
    this.category = 'Default',
    this.isFavorite = false,
    this.thumbnailPath,
    this.sourcePath,
  });

  /// Stable identifier (file name without extension).
  final String id;

  /// Display name.
  final String name;

  /// Krita paintop ID (e.g. "basic_tip", "airbrush", "pencil").
  final String paintopId;

  /// Brush settings keyed by Krita setting name, with the value
  /// serialized as a string (matches [BrushSettingValue.value]).
  final Map<String, String> settings;

  /// Free-form description.
  final String description;

  /// Logical grouping ("Ink", "Pencil", ...).
  final String category;

  /// Whether the user has favorited the preset.
  final bool isFavorite;

  /// Path to the thumbnail PNG (kept external so the JSON stays small).
  final String? thumbnailPath;

  /// Path to the source .kpp file, when imported.
  final String? sourcePath;

  Map<String, dynamic> toJson() => {
        'id': id,
        'name': name,
        'paintopId': paintopId,
        'settings': settings,
        'description': description,
        'category': category,
        'isFavorite': isFavorite,
        if (thumbnailPath != null) 'thumbnailPath': thumbnailPath,
        if (sourcePath != null) 'sourcePath': sourcePath,
      };

  factory PresetModel.fromJson(Map<String, dynamic> json) {
    final settings = <String, String>{};
    final rawSettings = json['settings'];
    if (rawSettings is Map) {
      rawSettings.forEach((k, v) {
        settings[k.toString()] = v.toString();
      });
    }
    return PresetModel(
      id: (json['id'] as String?) ?? '',
      name: (json['name'] as String?) ?? 'Preset',
      paintopId: (json['paintopId'] as String?) ?? 'basic',
      settings: settings,
      description: (json['description'] as String?) ?? '',
      category: (json['category'] as String?) ?? 'Default',
      isFavorite: (json['isFavorite'] as bool?) ?? false,
      thumbnailPath: json['thumbnailPath'] as String?,
      sourcePath: json['sourcePath'] as String?,
    );
  }

  /// Builds the persistence model from a runtime [BrushPreset].
  factory PresetModel.fromBrushPreset(BrushPreset preset) => PresetModel(
        id: preset.id,
        name: preset.name,
        paintopId: preset.paintopId,
        settings: preset.settings
            .map((k, v) => MapEntry(k, v.value)),
        description: preset.description,
        category: preset.category,
        isFavorite: preset.isFavorite,
        sourcePath: preset.filePath,
      );

  /// Returns the runtime [BrushPreset] view of this model. The
  /// repository uses this when loading a preset back into the editor.
  BrushPreset toBrushPreset() => BrushPreset(
        id: id,
        name: name,
        paintopId: paintopId,
        description: description,
        isFavorite: isFavorite,
        category: category,
        filePath: sourcePath,
      );

  PresetModel copyWith({
    String? name,
    String? paintopId,
    Map<String, String>? settings,
    String? description,
    String? category,
    bool? isFavorite,
    String? thumbnailPath,
    String? sourcePath,
  }) =>
      PresetModel(
        id: id,
        name: name ?? this.name,
        paintopId: paintopId ?? this.paintopId,
        settings: settings ?? this.settings,
        description: description ?? this.description,
        category: category ?? this.category,
        isFavorite: isFavorite ?? this.isFavorite,
        thumbnailPath: thumbnailPath ?? this.thumbnailPath,
        sourcePath: sourcePath ?? this.sourcePath,
      );

  @override
  bool operator ==(Object other) =>
      identical(this, other) ||
      other is PresetModel &&
          other.id == id &&
          other.name == name &&
          other.paintopId == paintopId &&
          _mapEq(other.settings, settings) &&
          other.description == description &&
          other.category == category &&
          other.isFavorite == isFavorite &&
          other.thumbnailPath == thumbnailPath &&
          other.sourcePath == sourcePath;

  @override
  int get hashCode => Object.hash(
        id,
        name,
        paintopId,
        Object.hashAll(settings.entries),
        description,
        category,
        isFavorite,
        thumbnailPath,
        sourcePath,
      );
}

bool _mapEq<K, V>(Map<K, V> a, Map<K, V> b) {
  if (a.length != b.length) return false;
  for (final entry in a.entries) {
    if (b[entry.key] != entry.value) return false;
  }
  return true;
}
