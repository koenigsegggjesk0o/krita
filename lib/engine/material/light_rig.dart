// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// light_rig.dart — material-facing lighting system for Feather.
//
// Feather's docs (brushes_materials.txt): "You can change the lighting
// direction or add various effects in the Environment Tab." The Shaded
// material "responds to lighting and casts shadows."
//
// This rig is the lighting model the MATERIAL layer consumes. It is
// deliberately simpler than the software rasterizer's full LightRig
// (lib/core/rendering/lighting.dart) — Feather's environment is a single
// key light plus an ambient floor, the same model the scene pipeline's
// legacy `kSceneKeyLight` established. The math here is REAL Lambert
// diffuse + REAL Phong specular (the reflection-vector R·V form, not
// the Blinn half-vector shortcut), so a Shaded curve reads as lit 3D
// geometry:
//
//   diffuse  = max(0, N · L) * lightColor * lightIntensity
//   specular = pow(max(0, R · V), shininess) * lightColor * specular
//   R        = reflect(-L, N) = 2*(N·L)*N - L
//
// Depends on lib/core/math (Vec3 + math_utils). Bridge helpers convert
// to/from vector_math's mutable Vector3 (the type the Stroke / scene
// pipeline uses) so the rig slots into both worlds.

import 'dart:math' as math;

import 'package:vector_math/vector_math_64.dart' show Vector3;

import 'package:feather_krita/core/math/math_utils.dart';
import 'package:feather_krita/core/math/vec3.dart';

/// One directional light for the material rig.
class MaterialLight {
  MaterialLight({
    required this.direction,
    this.color = const _Rgb(1.0, 1.0, 1.0),
    this.intensity = 1.0,
  }) : _dir = direction.normalized();

  /// Light direction, pointing FROM the light TOWARD the scene (the
  /// OpenGL convention). Stored normalized.
  Vec3 direction;
  Vec3 _dir;

  void setDirection(Vec3 d) {
    direction = d;
    _dir = d.normalized();
  }

  /// Unit light direction (cached).
  Vec3 get unit => _dir;

  /// Linear-RGB color in 0..1.
  _Rgb color;

  /// Diffuse intensity multiplier (0..1+).
  double intensity;
}

/// Simple linear-RGB triple in 0..1 (no alpha — light color has no
/// transparency).
class _Rgb {
  const _Rgb(this.r, this.g, this.b);
  final double r, g, b;
}

/// The result of evaluating the rig at one fragment.
class LightResult {
  const LightResult({this.diffuse = 0.0, this.specular = 0.0});
  final double diffuse;
  final double specular;
}

/// A key light + ambient floor, the Feather environment model.
class MaterialLightRig {
  MaterialLightRig({
    Vec3? direction,
    double intensity = 1.0,
    this.ambient = 0.62,
    this.ambientColor = const _Rgb(1.0, 1.0, 1.0),
    this.specularStrength = 0.35,
    this.shininess = 24.0,
  }) : key = MaterialLight(
          direction: direction ?? const Vec3(-0.35, -1.0, -0.55),
          intensity: intensity,
        );

  /// The single key light.
  final MaterialLight key;

  /// Ambient floor (0..1). Diffuse contributes the remainder up to 1.0,
  /// matching the scene pipeline's `kSceneAmbient` contract so a Shaded
  /// curve at the default rig reproduces the legacy look exactly.
  double ambient;

  /// Ambient color (linear RGB).
  _Rgb ambientColor;

  /// Specular term multiplier (0 = matte, 1 = strong highlight).
  double specularStrength;

  /// Phong shininess exponent (higher = tighter highlight).
  double shininess;

  void setKeyDirection(Vec3 d) => key.setDirection(d);
  void setKeyIntensity(double v) => key.intensity = v.clamp(0.0, 4.0);

  /// Lambert diffuse term for [normal] (unit) under the key light,
  /// ambient-floored and color-tinted.
  ///
  /// [key.unit] points FROM the light TOWARD the scene (the OpenGL
  /// convention), so the diffuse term is `N · (-L)`: a surface whose
  /// normal faces the light source reads as fully lit, while one facing
  /// away (normal aligned with L) reads as the ambient floor. Using
  /// `N · L` directly would invert the lighting — a back-facing surface
  /// would read as fully lit.
  double lambert(Vec3 normal) {
    final n = normal.normalized();
    final ndl = n.dot(-key.unit).clamp(0.0, 1.0);
    return (ambient + (1.0 - ambient) * key.intensity * ndl).clamp(0.0, 1.0);
  }

  /// Phong specular term for [normal] viewed from [viewDir] (both unit).
  /// Uses the reflection-vector form R·V raised to [shininess].
  double phong(Vec3 normal, Vec3 viewDir) {
    final n = normal.normalized();
    final v = viewDir.normalized();
    final l = key.unit;
    // Reflect the light direction about the normal: R = 2*(N·L)*N - L.
    final ndl = n.dot(l);
    if (ndl <= 0.0) return 0.0; // facing away — no specular
    final r = n * (2.0 * ndl) - l;
    final rdv = r.dot(v).clamp(0.0, 1.0);
    final spec = math.pow(rdv, shininess).toDouble();
    return (spec * key.intensity * specularStrength).clamp(0.0, 1.0);
  }

  /// Full BRDF evaluation: ambient + diffuse + specular, returned as a
  /// brightness pair the material multiplies its base color by.
  LightResult evaluate(Vec3 normal, Vec3 viewDir) {
    final diff = lambert(normal);
    final spec = phong(normal, viewDir);
    return LightResult(diffuse: diff, specular: spec);
  }

  // --- vector_math bridge ----------------------------------------------
  // The Stroke model + scene pipeline use vector_math's mutable Vector3;
  // the material layer uses core's immutable Vec3. These helpers keep the
  // boundary explicit and allocation-light (single conversion per shade).

  static Vec3 toVec3(Vector3 v) => Vec3(v.x, v.y, v.z);
  static Vector3 toVector3(Vec3 v) => Vector3(v.x, v.y, v.z);
  static Vec3 normalizeVec3(Vector3 v) => toVec3(v).normalized();
}

/// Converts a 0xAARRGGBB int to linear-RGB doubles in 0..1.
_Rgb argbToRgb(int argb) => _Rgb(
      ((argb >> 16) & 0xff) / 255.0,
      ((argb >> 8) & 0xff) / 255.0,
      (argb & 0xff) / 255.0,
    );

/// Convenience: clamp a brightness scalar into [0, 1].
double clampBrightness(double v) => clampDouble(v, 0.0, 1.0);
