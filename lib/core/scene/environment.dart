// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// environment.dart — the scene environment settings.
//
// Models the Environment Tab of the Stage Panel (see `stagepanel.txt`).
// The Environment Tab controls the world the artist draws into:
//
//   * Background colour (a flat colour behind the 3D scene).
//   * Background image (an optional reference / sky plate).
//   * Lighting direction (azimuth + elevation, the "sun position") and
//     intensity.
//   * World settings: ground-grid visibility, grid spacing, axis gizmo,
//     depth-of-field toggle + focus distance, fog density.
//
// The lighting direction is the same sun-position parametrization used by
// `lib/engine/material/light_rig.dart`; this struct is the serializable counterpart
// the scene graph persists.
//
// Depends on: lib/core/math/

import 'dart:ui' show Color;

import 'dart:math' as math;

import 'package:feather_krita/core/math/math.dart';

/// Lighting model — a single directional sun.
class EnvironmentLight {
  EnvironmentLight({
    this.azimuthDegrees = 45.0,
    this.elevationDegrees = 55.0,
    this.intensity = 1.0,
    this.ambient = 0.25,
    this.color = const Color(0xffffffff),
  });

  /// Horizontal angle around the scene (degrees, 0 = +X, CCW positive).
  double azimuthDegrees;

  /// Elevation above the horizon (degrees, -90…+90).
  double elevationDegrees;

  /// Diffuse intensity multiplier (0…2).
  double intensity;

  /// Ambient floor (0…1) so shadowed faces aren't pure black.
  double ambient;

  /// Light tint.
  Color color;

  /// World-space unit direction pointing FROM the light TOWARD the scene
  /// (the convention `lib/core/scene/scene.dart` uses).
  Vector3 get direction {
    final az = azimuthDegrees * kDegToRad;
    final el = elevationDegrees * kDegToRad;
    // Sun position on the unit sphere.
    final sx = math.cos(el) * math.cos(az);
    final sy = math.sin(el);
    final sz = math.cos(el) * math.sin(az);
    // Direction from sun to scene is the negation.
    return Vector3(-sx, -sy, -sz)..normalize();
  }
}

/// World / scene-display settings.
class WorldSettings {
  WorldSettings({
    this.showGroundGrid = true,
    this.gridSpacing = 1.0,
    this.gridSubdivisions = 10,
    this.showAxisGizmo = true,
    this.depthOfField = false,
    this.dofFocusDistance = 6.0,
    this.dofAperture = 0.05,
    this.fogEnabled = false,
    this.fogDensity = 0.02,
    this.fogColor = const Color(0xff000000),
  });

  /// Whether the ground grid overlay is visible.
  bool showGroundGrid;

  /// World units between major grid lines.
  double gridSpacing;

  /// Minor lines per major cell.
  int gridSubdivisions;

  /// Whether the XYZ axis gizmo is shown.
  bool showAxisGizmo;

  /// Depth-of-field toggle (focus point = camera orbit point).
  bool depthOfField;

  /// Focus distance in world units.
  double dofFocusDistance;

  /// Aperture size — larger = stronger blur.
  double dofAperture;

  /// Distance-fog toggle.
  bool fogEnabled;

  /// Fog exponential density.
  double fogDensity;

  /// Fog tint.
  Color fogColor;
}

/// A reference image plate attached to the environment (background or
/// skybox-style plate).
class EnvironmentImage {
  EnvironmentImage({this.assetPath, this.opacity = 1.0, this.fit = ImageFit.cover});

  /// Filesystem / asset path. `null` clears the plate.
  String? assetPath;

  /// 0–1 opacity.
  double opacity;

  /// How the image is fitted to the viewport.
  ImageFit fit;
}

enum ImageFit { cover, contain, stretch, center }

/// The full environment block.
class Environment {
  Environment({
    Color? backgroundColor,
    EnvironmentImage? backgroundImage,
    EnvironmentLight? light,
    WorldSettings? world,
  })  : backgroundColor = backgroundColor ?? const Color(0xff1a1a1f),
        backgroundImage = backgroundImage ?? EnvironmentImage(),
        light = light ?? EnvironmentLight(),
        world = world ?? WorldSettings();

  Color backgroundColor;
  EnvironmentImage backgroundImage;
  EnvironmentLight light;
  WorldSettings world;

  Environment copy() => Environment(
        backgroundColor: backgroundColor,
        backgroundImage: EnvironmentImage(
          assetPath: backgroundImage.assetPath,
          opacity: backgroundImage.opacity,
          fit: backgroundImage.fit,
        ),
        light: EnvironmentLight(
          azimuthDegrees: light.azimuthDegrees,
          elevationDegrees: light.elevationDegrees,
          intensity: light.intensity,
          ambient: light.ambient,
          color: light.color,
        ),
        world: WorldSettings(
          showGroundGrid: world.showGroundGrid,
          gridSpacing: world.gridSpacing,
          gridSubdivisions: world.gridSubdivisions,
          showAxisGizmo: world.showAxisGizmo,
          depthOfField: world.depthOfField,
          dofFocusDistance: world.dofFocusDistance,
          dofAperture: world.dofAperture,
          fogEnabled: world.fogEnabled,
          fogDensity: world.fogDensity,
          fogColor: world.fogColor,
        ),
      );

  Map<String, dynamic> toJson() => {
        'backgroundColor': backgroundColor.toARGB32(),
        'backgroundImage': {
          'assetPath': backgroundImage.assetPath,
          'opacity': backgroundImage.opacity,
          'fit': backgroundImage.fit.name,
        },
        'light': {
          'azimuth': light.azimuthDegrees,
          'elevation': light.elevationDegrees,
          'intensity': light.intensity,
          'ambient': light.ambient,
          'color': light.color.toARGB32(),
        },
        'world': {
          'showGrid': world.showGroundGrid,
          'gridSpacing': world.gridSpacing,
          'gridSubdivisions': world.gridSubdivisions,
          'showGizmo': world.showAxisGizmo,
          'dof': world.depthOfField,
          'dofFocus': world.dofFocusDistance,
          'dofAperture': world.dofAperture,
          'fog': world.fogEnabled,
          'fogDensity': world.fogDensity,
          'fogColor': world.fogColor.toARGB32(),
        },
      };

  factory Environment.fromJson(Map<String, dynamic> json) {
    final env = Environment();
    final bg = json['backgroundColor'];
    if (bg is int) env.backgroundColor = Color(bg);
    final bi = json['backgroundImage'] as Map<String, dynamic>?;
    if (bi != null) {
      env.backgroundImage = EnvironmentImage(
        assetPath: bi['assetPath'] as String?,
        opacity: (bi['opacity'] as num?)?.toDouble() ?? 1.0,
        fit: ImageFit.values.firstWhere(
          (f) => f.name == (bi['fit'] as String? ?? 'cover'),
          orElse: () => ImageFit.cover,
        ),
      );
    }
    final l = json['light'] as Map<String, dynamic>?;
    if (l != null) {
      env.light = EnvironmentLight(
        azimuthDegrees: (l['azimuth'] as num?)?.toDouble() ?? 45.0,
        elevationDegrees: (l['elevation'] as num?)?.toDouble() ?? 55.0,
        intensity: (l['intensity'] as num?)?.toDouble() ?? 1.0,
        ambient: (l['ambient'] as num?)?.toDouble() ?? 0.25,
        color: Color((l['color'] as int?) ?? 0xffffffff),
      );
    }
    final w = json['world'] as Map<String, dynamic>?;
    if (w != null) {
      env.world = WorldSettings(
        showGroundGrid: w['showGrid'] as bool? ?? true,
        gridSpacing: (w['gridSpacing'] as num?)?.toDouble() ?? 1.0,
        gridSubdivisions: (w['gridSubdivisions'] as int?) ?? 10,
        showAxisGizmo: w['showGizmo'] as bool? ?? true,
        depthOfField: w['dof'] as bool? ?? false,
        dofFocusDistance: (w['dofFocus'] as num?)?.toDouble() ?? 6.0,
        dofAperture: (w['dofAperture'] as num?)?.toDouble() ?? 0.05,
        fogEnabled: w['fog'] as bool? ?? false,
        fogDensity: (w['fogDensity'] as num?)?.toDouble() ?? 0.02,
        fogColor: Color((w['fogColor'] as int?) ?? 0xff000000),
      );
    }
    return env;
  }
}
