// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// SCAFFOLD — software rasterizer for a FUTURE high-quality headless
// render path. NOT YET WIRED to any live path (AUDIT_FINAL gap #4):
// the canvas viewport's CustomPainter handles live faux-3D tube
// rendering, and PNG export uses RepaintBoundary pixel capture. Wiring
// this would regress WYSIWYG unless the canvas also moves to mesh
// rendering. Kept as a forward-looking scaffold for a future
// WebGL/Impeller/headless-export backend; see worklog v54-B.
//
// lighting.dart — Light system for the Feather-Krita software renderer.
//
// Provides:
//   - [LightType]  — ambient / directional / point / spot
//   - [Light]      — a single light source (color, intensity, attenuation)
//   - [LightRig]   — a collection of lights plus ambient and exposure
//   - [LightContribution] — the per-fragment accumulated lighting state
//     that the fragment shaders consume
//
// Conventions:
//   - Light DIRECTIONS point FROM the light TOWARD the scene (the OpenGL
//     convention). For a directional light, this is a unit vector.
//   - Light COLORS are 0xAARRGGBB ints in 0–255 space (the fragment
//     shaders rescale to 0–1).
//   - Intensities are in arbitrary units, scaled by 1/pi internally for
//     physically-plausible diffuse response.
//
// All math is plain Dart / vector_math so this module can be unit-tested
// in isolation and reused by both the rasterizer-side shaders and the
// raycast-side picking / hit-tests.

import 'dart:math' as math;
import 'dart:typed_data';

import 'package:vector_math/vector_math_64.dart';

/// Light categories the renderer understands.
enum LightType {
  /// Flat ambient floor. Adds a constant to every fragment. Always
  /// rendered first by the shaders.
  ambient,

  /// Directional (sun) light — infinite distance, parallel rays, no
  /// attenuation.
  directional,

  /// Point light — emits in all directions from a position in world
  /// space, attenuates with distance.
  point,

  /// Spot light — point light with a cone of effect. Defined by a
  /// direction, inner cone angle, and outer cone angle.
  spot,
}

/// A single light source.
class Light {
  Light({
    required this.type,
    required this.color,
    this.intensity = 1.0,
    this.direction,
    this.position,
    this.range = double.infinity,
    this.innerConeAngle = math.pi * 0.25,
    this.outerConeAngle = math.pi * 0.35,
    this.castShadows = false,
  })  : assert(intensity >= 0),
        assert(range > 0) {
    if (direction != null) {
      direction = direction!.normalized();
    }
  }

  /// What kind of light this is.
  final LightType type;

  /// ARGB int (0xAARRGGBB) color. Alpha is treated as the color's
  /// "weight" multiplier in [LightContribution.add] (255 = full).
  int color;

  /// Brightness multiplier. Default 1.0.
  double intensity;

  /// For [LightType.directional] and [LightType.spot] — the direction
  /// FROM the light TOWARD the scene. Must be unit length; the
  /// constructor normalizes if not.
  Vector3? direction;

  /// For [LightType.point] and [LightType.spot] — world-space position
  /// of the light source.
  Vector3? position;

  /// Maximum effective range of point / spot lights. Beyond this the
  /// attenuation is zero.
  double range;

  /// Spot inner cone half-angle (radians). Full intensity inside.
  final double innerConeAngle;

  /// Spot outer cone half-angle (radians). Zero intensity outside.
  final double outerConeAngle;

  /// Whether this light should produce shadows. Reserved — the software
  /// rasterizer does not currently render shadow maps.
  final bool castShadows;

  /// Returns the (r, g, b) color of this light scaled by intensity, in
  /// 0–1 linear space. The alpha channel is folded in.
  Float64List rgbScaled() {
    final r = ((color >> 16) & 0xff) / 255.0;
    final g = ((color >> 8) & 0xff) / 255.0;
    final b = (color & 0xff) / 255.0;
    final a = ((color >> 24) & 0xff) / 255.0;
    final k = intensity * a;
    return Float64List.fromList([r * k, g * k, b * k]);
  }

  /// Returns the effective light direction at world point [p]. For
  /// directional lights this is just [direction]. For point/spot lights
  /// it is (position - p) normalized. Returns null for ambient.
  Vector3? directionAt(Vector3 p) {
    switch (type) {
      case LightType.ambient:
        return null;
      case LightType.directional:
        return direction;
      case LightType.point:
      case LightType.spot:
        final pos = position;
        if (pos == null) return null;
        return (pos - p)..normalize();
    }
  }

  /// Returns the attenuation factor (0..1) for a fragment at distance
  /// [distance] from the light source. Directional lights return 1
  /// always. Ambient returns 1. Point / spot use the inverse-square
  /// falloff with a [range] cutoff (smoothstep).
  double attenuationAt(double distance) {
    switch (type) {
      case LightType.ambient:
      case LightType.directional:
        return 1.0;
      case LightType.point:
      case LightType.spot:
        if (distance >= range) return 0.0;
        // Inverse-square with a softening term to avoid the singularity
        // at distance=0. Matches the formula used by glTF.
        final d2 = distance * distance + 0.01;
        final inv = 1.0 / d2;
        final cutoff = range > 0 ? (1.0 - (distance / range)).clamp(0.0, 1.0) : 1.0;
        return (inv * cutoff).clamp(0.0, 1.0);
    }
  }

  /// For [LightType.spot] only — returns the cone attenuation (0..1)
  /// given the angle (radians) between the spot direction and the
  /// vector from the light to the fragment. Returns 1 for non-spot
  /// lights.
  double spotAttenuation(double angle) {
    if (type != LightType.spot) return 1.0;
    if (angle <= innerConeAngle) return 1.0;
    if (angle >= outerConeAngle) return 0.0;
    final t =
        (angle - innerConeAngle) / (outerConeAngle - innerConeAngle);
    // Smoothstep for a soft edge.
    return 1.0 - t * t * (3.0 - 2.0 * t);
  }
}

/// A collection of lights plus global ambient + exposure.
///
/// The rasterizer's shaded shader takes one of these and accumulates the
/// contribution of every light at each fragment.
class LightRig {
  LightRig({
    List<Light>? lights,
    this.ambientColor = 0xff1a1a22,
    this.ambientIntensity = 0.25,
    this.exposure = 1.0,
  }) : lights = lights ?? <Light>[];

  /// The non-ambient lights in the scene. Ambient is stored separately
  /// because there's only ever one ambient floor.
  final List<Light> lights;

  /// Global ambient color (ARGB).
  int ambientColor;

  /// Global ambient intensity multiplier (default 0.25 — a gentle floor
  /// so unlit faces aren't pure black).
  double ambientIntensity;

  /// Tonemap exposure applied at the end of [LightContribution.resolve].
  /// 1.0 = no change. Higher values brighten the output.
  double exposure;

  /// Adds a light to the rig.
  void add(Light light) => lights.add(light);

  /// Removes the first light equal to [light] (identity check).
  void remove(Light light) => lights.remove(light);

  /// Convenience: adds a directional (sun) light.
  Light addDirectional(
          {required int color,
          required Vector3 direction,
          double intensity = 1.0}) =>
      _add(Light(
          type: LightType.directional,
          color: color,
          direction: direction,
          intensity: intensity));

  /// Convenience: adds a point light.
  Light addPoint(
          {required int color,
          required Vector3 position,
          double intensity = 1.0,
          double range = 10.0}) =>
      _add(Light(
          type: LightType.point,
          color: color,
          position: position,
          intensity: intensity,
          range: range));

  /// Convenience: adds a spot light.
  Light addSpot(
          {required int color,
          required Vector3 position,
          required Vector3 direction,
          double intensity = 1.0,
          double range = 10.0,
          double innerDeg = 30.0,
          double outerDeg = 45.0}) =>
      _add(Light(
          type: LightType.spot,
          color: color,
          position: position,
          direction: direction,
          intensity: intensity,
          range: range,
          innerConeAngle: innerDeg * math.pi / 180.0,
          outerConeAngle: outerDeg * math.pi / 180.0));

  Light _add(Light l) {
    lights.add(l);
    return l;
  }

  /// Builds a [LightContribution] ready for the fragment shaders.
  /// [position] and [normal] are the fragment's world-space attributes;
  /// [viewDir] points from the fragment toward the camera.
  LightContribution sample(Vector3 position, Vector3 normal,
      Vector3 viewDir) {
    final out = LightContribution.empty()
      ..ambientR = ((ambientColor >> 16) & 0xff) / 255.0 * ambientIntensity
      ..ambientG = ((ambientColor >> 8) & 0xff) / 255.0 * ambientIntensity
      ..ambientB = (ambientColor & 0xff) / 255.0 * ambientIntensity;
    for (final light in lights) {
      final dir = light.directionAt(position);
      if (dir == null) continue;
      final distance = (light.position != null)
          ? (light.position! - position).length
          : 0.0;
      final att = light.attenuationAt(distance);
      if (att <= 0.0) continue;
      final ndl = normal.dot(dir).clamp(0.0, 1.0);
      if (ndl <= 0.0) continue;
      double cone = 1.0;
      if (light.type == LightType.spot && light.direction != null) {
        final cosA = (-dir).dot(light.direction!);
        cone = light.spotAttenuation(math.acos(cosA.clamp(-1.0, 1.0)));
        if (cone <= 0.0) continue;
      }
      final rgb = light.rgbScaled();
      final k = ndl * att * cone;
      out.diffuseR += rgb[0] * k;
      out.diffuseG += rgb[1] * k;
      out.diffuseB += rgb[2] * k;
      // Specular half-vector trick.
      final half = (dir + viewDir)..normalize();
      final ndh = math.pow(normal.dot(half).clamp(0.0, 1.0), 64.0);
      out.specularR += rgb[0] * ndh * att * cone;
      out.specularG += rgb[1] * ndh * att * cone;
      out.specularB += rgb[2] * ndh * att * cone;
    }
    return out;
  }
}

/// The per-fragment lighting state — the shaded shader's only input
/// besides the material color.
class LightContribution {
  LightContribution.empty()
      : ambientR = 0,
        ambientG = 0,
        ambientB = 0,
        diffuseR = 0,
        diffuseG = 0,
        diffuseB = 0,
        specularR = 0,
        specularG = 0,
        specularB = 0;

  double ambientR, ambientG, ambientB;
  double diffuseR, diffuseG, diffuseB;
  double specularR, specularG, specularB;

  /// Resolves the contribution into a packed ARGB color (255 alpha),
  /// applying tonemap exposure and a simple Reinhard tonemap to keep
  /// HDR contributions in gamut.
  int resolve({
    required int baseColor,
    required double specularStrength,
    required double exposure,
  }) {
    final br = ((baseColor >> 16) & 0xff) / 255.0;
    final bg = ((baseColor >> 8) & 0xff) / 255.0;
    final bb = (baseColor & 0xff) / 255.0;
    final r = (br * (ambientR + diffuseR) + specularR * specularStrength) *
        exposure;
    final g = (bg * (ambientG + diffuseG) + specularG * specularStrength) *
        exposure;
    final b = (bb * (ambientB + diffuseB) + specularB * specularStrength) *
        exposure;
    return 0xff000000 |
        (_tonemap(r) << 16) |
        (_tonemap(g) << 8) |
        _tonemap(b);
  }

  static int _tonemap(double v) {
    // Reinhard: x / (1 + x), rescaled to 0–255.
    final mapped = v / (1.0 + v);
    return (mapped.clamp(0.0, 1.0) * 255.0).round();
  }

  /// Scales all channels by [s]. Returns this for chaining.
  LightContribution scale(double s) {
    ambientR *= s;
    ambientG *= s;
    ambientB *= s;
    diffuseR *= s;
    diffuseG *= s;
    diffuseB *= s;
    specularR *= s;
    specularG *= s;
    specularB *= s;
    return this;
  }
}
