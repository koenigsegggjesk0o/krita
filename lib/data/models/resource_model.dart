// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// resource_model.dart — Persistence model for an imported resource.
//
// A resource is any external asset the user has imported into a project:
// an image (brush tip, reference photo, texture), a 3D model (OBJ, glTF,
// STL), or a saved guide template (a parametric guide surface the user
// wants to keep around across projects).
//
// The model tracks the on-disk path, a stable id, display name, kind,
// optional thumbnail path, and import timestamp. The actual bytes are
// always streamed from disk by [ResourceRepository] — only the path is
// persisted.

/// Kind of imported resource.
enum ResourceKind {
  /// A 2D image (PNG, JPEG, brush tip, reference photo, texture).
  image,

  /// A 3D model (OBJ, glTF, STL, FBX).
  model,

  /// A saved guide-surface template.
  guideTemplate,

  /// A Krita brush preset (.kpp).
  brushPreset,

  /// Anything else.
  other,
}

/// Persistence model for an imported resource.
class ResourceModel {
  const ResourceModel({
    required this.id,
    required this.kind,
    required this.name,
    required this.path,
    this.mimeType = 'application/octet-stream',
    this.sizeBytes = 0,
    this.importedAt,
    this.thumbnailPath,
    this.metadata = const <String, String>{},
  });

  /// Stable identifier (a monotonic counter or a content hash).
  final String id;

  /// What kind of resource this is.
  final ResourceKind kind;

  /// Display name (without extension by convention).
  final String name;

  /// Absolute path on disk.
  final String path;

  /// MIME type (best-effort, derived from the file extension by the
  /// repository at import time).
  final String mimeType;

  /// File size in bytes.
  final int sizeBytes;

  /// Import timestamp (UTC).
  final DateTime? importedAt;

  /// Optional thumbnail PNG path.
  final String? thumbnailPath;

  /// Free-form key/value metadata (e.g. image dimensions, model
  /// triangle count, guide type + params).
  final Map<String, String> metadata;

  Map<String, dynamic> toJson() => {
        'id': id,
        'kind': kind.name,
        'name': name,
        'path': path,
        'mimeType': mimeType,
        'sizeBytes': sizeBytes,
        'importedAt': importedAt?.toIso8601String(),
        if (thumbnailPath != null) 'thumbnailPath': thumbnailPath,
        'metadata': metadata,
      };

  factory ResourceModel.fromJson(Map<String, dynamic> json) {
    final metadata = <String, String>{};
    final rawMeta = json['metadata'];
    if (rawMeta is Map) {
      rawMeta.forEach((k, v) {
        metadata[k.toString()] = v.toString();
      });
    }
    return ResourceModel(
      id: (json['id'] as String?) ?? '',
      kind: ResourceKind.values.firstWhere(
        (k) => k.name == (json['kind'] as String?),
        orElse: () => ResourceKind.other,
      ),
      name: (json['name'] as String?) ?? 'Resource',
      path: (json['path'] as String?) ?? '',
      mimeType: (json['mimeType'] as String?) ?? 'application/octet-stream',
      sizeBytes: (json['sizeBytes'] as num?)?.toInt() ?? 0,
      importedAt: json['importedAt'] is String
          ? DateTime.tryParse(json['importedAt'] as String)
          : null,
      thumbnailPath: json['thumbnailPath'] as String?,
      metadata: metadata,
    );
  }

  ResourceModel copyWith({
    String? name,
    String? path,
    String? mimeType,
    int? sizeBytes,
    DateTime? importedAt,
    String? thumbnailPath,
    Map<String, String>? metadata,
  }) =>
      ResourceModel(
        id: id,
        kind: kind,
        name: name ?? this.name,
        path: path ?? this.path,
        mimeType: mimeType ?? this.mimeType,
        sizeBytes: sizeBytes ?? this.sizeBytes,
        importedAt: importedAt ?? this.importedAt,
        thumbnailPath: thumbnailPath ?? this.thumbnailPath,
        metadata: metadata ?? this.metadata,
      );

  @override
  bool operator ==(Object other) =>
      identical(this, other) ||
      other is ResourceModel &&
          other.id == id &&
          other.kind == kind &&
          other.name == name &&
          other.path == path &&
          other.mimeType == mimeType &&
          other.sizeBytes == sizeBytes &&
          other.importedAt == importedAt &&
          other.thumbnailPath == thumbnailPath &&
          _mapEq(other.metadata, metadata);

  @override
  int get hashCode => Object.hash(
        id,
        kind,
        name,
        path,
        mimeType,
        sizeBytes,
        importedAt,
        thumbnailPath,
        Object.hashAll(metadata.entries),
      );
}

bool _mapEq<K, V>(Map<K, V> a, Map<K, V> b) {
  if (a.length != b.length) return false;
  for (final entry in a.entries) {
    if (b[entry.key] != entry.value) return false;
  }
  return true;
}
