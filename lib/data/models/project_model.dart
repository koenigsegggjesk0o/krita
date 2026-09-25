// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// project_model.dart — Persistence model for a .feather project.
//
// This is the on-disk schema (de)serialized by [ProjectRepository]. It
// wraps the live scene/camera/brush slices produced by the Riverpod
// notifiers with project-level metadata (name, timestamps, thumbnail,
// format version) and a [ProjectSceneModel] / [ProjectCameraModel] /
// [ProjectBrushModel] triplet that capture only the persistent fields.
//
// Distinct from the runtime [SceneState] / [CameraState] / [BrushState]
// value types in lib/state/ — those carry transient UI flags (hover,
// showGrid, etc.) that should never be written to disk.

import 'package:feather_krita/models/stroke.dart' show Stroke;

/// On-disk project format version. Bumped whenever the schema changes
/// in a backward-incompatible way. The repository rejects newer-format
/// files with a [FormatException] so the user is told the file was
/// saved by a newer app version.
const int kProjectModelVersion = 1;

/// Top-level project model.
class ProjectModel {
  const ProjectModel({
    this.version = kProjectModelVersion,
    required this.name,
    required this.scene,
    required this.camera,
    required this.brush,
    this.created,
    this.modified,
    this.thumbnail,
    this.filePath,
  });

  /// Format version.
  final int version;

  /// Display name (without extension).
  final String name;

  /// Scene block (strokes + guides + layers + environment).
  final ProjectSceneModel scene;

  /// Camera block.
  final ProjectCameraModel camera;

  /// Brush block.
  final ProjectBrushModel brush;

  /// Creation timestamp (UTC).
  final DateTime? created;

  /// Last-modified timestamp (UTC).
  final DateTime? modified;

  /// Optional PNG thumbnail bytes.
  final List<int>? thumbnail;

  /// Absolute path of the file the project was loaded from / saved to.
  final String? filePath;

  Map<String, dynamic> toJson() => {
        'version': version,
        'name': name,
        'scene': scene.toJson(),
        'camera': camera.toJson(),
        'brush': brush.toJson(),
        'created': created?.toIso8601String(),
        'modified': modified?.toIso8601String(),
        if (thumbnail != null) 'thumbnailLength': thumbnail!.length,
      };

  factory ProjectModel.fromJson(Map<String, dynamic> json) {
    final version = (json['version'] as num?)?.toInt() ?? 1;
    if (version > kProjectModelVersion) {
      throw FormatException(
        'Project saved by a newer app version '
        '(format v$version, supported v$kProjectModelVersion).',
      );
    }
    return ProjectModel(
      version: version,
      name: (json['name'] as String?) ?? 'Untitled',
      scene: ProjectSceneModel.fromJson(
        (json['scene'] as Map?)?.cast<String, dynamic>() ?? {},
      ),
      camera: ProjectCameraModel.fromJson(
        (json['camera'] as Map?)?.cast<String, dynamic>() ?? {},
      ),
      brush: ProjectBrushModel.fromJson(
        (json['brush'] as Map?)?.cast<String, dynamic>() ?? {},
      ),
      created: _parseDate(json['created']),
      modified: _parseDate(json['modified']),
      // Thumbnail bytes are streamed separately by the repository
      // because they may live in a sidecar bin chunk.
      thumbnail: null,
      filePath: json['filePath'] as String?,
    );
  }

  ProjectModel copyWith({
    String? name,
    ProjectSceneModel? scene,
    ProjectCameraModel? camera,
    ProjectBrushModel? brush,
    DateTime? created,
    DateTime? modified,
    List<int>? thumbnail,
    String? filePath,
  }) =>
      ProjectModel(
        version: version,
        name: name ?? this.name,
        scene: scene ?? this.scene,
        camera: camera ?? this.camera,
        brush: brush ?? this.brush,
        created: created ?? this.created,
        modified: modified ?? this.modified,
        thumbnail: thumbnail ?? this.thumbnail,
        filePath: filePath ?? this.filePath,
      );
}

DateTime? _parseDate(dynamic v) {
  if (v is String) return DateTime.tryParse(v);
  return null;
}

/// Scene block of a project.
class ProjectSceneModel {
  const ProjectSceneModel({
    this.strokes = const <Stroke>[],
    this.guideSurfaceName = 'Sphere',
    this.guideShapeParams = const <String, num>{},
    this.textureWidth = 2048,
    this.textureHeight = 2048,
    this.mirrorX = false,
    this.mirrorY = false,
    this.mirrorZ = false,
    this.lightAzimuthDeg,
    this.lightElevationDeg,
    this.lightIntensity,
  });

  final List<Stroke> strokes;
  final String guideSurfaceName;
  final Map<String, num> guideShapeParams;
  final int textureWidth;
  final int textureHeight;
  final bool mirrorX;
  final bool mirrorY;
  final bool mirrorZ;
  final double? lightAzimuthDeg;
  final double? lightElevationDeg;
  final double? lightIntensity;

  Map<String, dynamic> toJson() => {
        'strokes': strokes.map((s) => s.toJson()).toList(),
        'guideSurface': guideSurfaceName,
        if (guideShapeParams.isNotEmpty)
          'guideShape': {'params': guideShapeParams},
        'texture': {'width': textureWidth, 'height': textureHeight},
        'mirror': {'x': mirrorX, 'y': mirrorY, 'z': mirrorZ},
        if (lightAzimuthDeg != null &&
            lightElevationDeg != null &&
            lightIntensity != null)
          'light': {
            'azimuthDeg': lightAzimuthDeg,
            'elevationDeg': lightElevationDeg,
            'intensity': lightIntensity,
          },
      };

  factory ProjectSceneModel.fromJson(Map<String, dynamic> json) {
    final strokesList = (json['strokes'] as List? ?? [])
        .whereType<Map<String, dynamic>>()
        .map(Stroke.fromJson)
        .toList();
    final guideShape =
        (json['guideShape'] as Map?)?.cast<String, dynamic>() ?? {};
    final params = (guideShape['params'] as Map?)?.cast<String, num>() ?? {};
    final texture = (json['texture'] as Map?)?.cast<String, dynamic>() ?? {};
    final mirror = (json['mirror'] as Map?)?.cast<String, dynamic>() ?? {};
    final light = (json['light'] as Map?)?.cast<String, dynamic>() ?? {};
    return ProjectSceneModel(
      strokes: strokesList,
      guideSurfaceName: (json['guideSurface'] as String?) ?? 'Sphere',
      guideShapeParams: Map<String, num>.from(params),
      textureWidth: (texture['width'] as num?)?.toInt() ?? 2048,
      textureHeight: (texture['height'] as num?)?.toInt() ?? 2048,
      mirrorX: (mirror['x'] as bool?) ?? false,
      mirrorY: (mirror['y'] as bool?) ?? false,
      mirrorZ: (mirror['z'] as bool?) ?? false,
      lightAzimuthDeg: (light['azimuthDeg'] as num?)?.toDouble(),
      lightElevationDeg: (light['elevationDeg'] as num?)?.toDouble(),
      lightIntensity: (light['intensity'] as num?)?.toDouble(),
    );
  }
}

/// Camera block of a project.
class ProjectCameraModel {
  const ProjectCameraModel({
    this.yaw = 0.0,
    this.pitch = -0.4,
    this.distance = 6.0,
    this.targetX = 0.0,
    this.targetY = 0.0,
    this.targetZ = 0.0,
    this.projection = 'perspective',
    this.fovYRadians = 1.0471975511965976,
  });

  final double yaw;
  final double pitch;
  final double distance;
  final double targetX;
  final double targetY;
  final double targetZ;
  final String projection;
  final double fovYRadians;

  Map<String, dynamic> toJson() => {
        'yaw': yaw,
        'pitch': pitch,
        'distance': distance,
        'target': [targetX, targetY, targetZ],
        'projection': projection,
        'fovYRadians': fovYRadians,
      };

  factory ProjectCameraModel.fromJson(Map<String, dynamic> json) {
    final target = (json['target'] as List?) ?? const [0.0, 0.0, 0.0];
    return ProjectCameraModel(
      yaw: (json['yaw'] as num?)?.toDouble() ?? 0.0,
      pitch: (json['pitch'] as num?)?.toDouble() ?? -0.4,
      distance: (json['distance'] as num?)?.toDouble() ?? 6.0,
      targetX: target.isNotEmpty ? (target[0] as num).toDouble() : 0.0,
      targetY: target.length > 1 ? (target[1] as num).toDouble() : 0.0,
      targetZ: target.length > 2 ? (target[2] as num).toDouble() : 0.0,
      projection: (json['projection'] as String?) ?? 'perspective',
      fovYRadians:
          (json['fovYRadians'] as num?)?.toDouble() ?? 1.0471975511965976,
    );
  }
}

/// Brush block of a project.
class ProjectBrushModel {
  const ProjectBrushModel({
    this.presetName = 'Basic Round',
    this.size = 32.0,
    this.opacity = 1.0,
    this.color = 0xFF1A1A1A,
    this.hardness = 0.8,
    this.flow = 1.0,
  });

  final String presetName;
  final double size;
  final double opacity;
  final int color;
  final double hardness;
  final double flow;

  Map<String, dynamic> toJson() => {
        'preset': presetName,
        'size': size,
        'opacity': opacity,
        'color': color,
        'hardness': hardness,
        'flow': flow,
      };

  factory ProjectBrushModel.fromJson(Map<String, dynamic> json) =>
      ProjectBrushModel(
        presetName: (json['preset'] as String?) ?? 'Basic Round',
        size: (json['size'] as num?)?.toDouble() ?? 32.0,
        opacity: (json['opacity'] as num?)?.toDouble() ?? 1.0,
        color: (json['color'] as num?)?.toInt() ?? 0xFF1A1A1A,
        hardness: (json['hardness'] as num?)?.toDouble() ?? 0.8,
        flow: (json['flow'] as num?)?.toDouble() ?? 1.0,
      );
}
