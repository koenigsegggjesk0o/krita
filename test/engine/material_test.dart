// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// material_test.dart — unit tests for the four Feather materials and
// the lighting rig that drives Shaded.

import 'dart:ui' show Color;

import 'package:feather_krita/core/math/vec3.dart';
import 'package:feather_krita/engine/material/cutout_material.dart';
import 'package:feather_krita/engine/material/glow_material.dart';
import 'package:feather_krita/engine/material/light_rig.dart';
import 'package:feather_krita/engine/material/material.dart';
import 'package:feather_krita/engine/material/material_type.dart';
import 'package:feather_krita/engine/material/pattern_generator.dart';
import 'package:feather_krita/engine/material/shaded_material.dart';
import 'package:feather_krita/engine/material/shadeless_material.dart';
import 'package:flutter_test/flutter_test.dart';

void main() {
  group('MaterialType rules', () {
    test('shadeless and shaded support patterns; glow and cutout do not', () {
      expect(materialTypeSupportsPattern(MaterialType.shadeless), isTrue);
      expect(materialTypeSupportsPattern(MaterialType.shaded), isTrue);
      expect(materialTypeSupportsPattern(MaterialType.glow), isFalse);
      expect(materialTypeSupportsPattern(MaterialType.cutout), isFalse);
    });

    test('only shaded responds to light / casts shadows', () {
      expect(materialTypeRespondsToLight(MaterialType.shaded), isTrue);
      for (final t in [
        MaterialType.shadeless,
        MaterialType.glow,
        MaterialType.cutout,
      ]) {
        expect(materialTypeRespondsToLight(t), isFalse);
      }
    });

    test('only glow is additive', () {
      expect(materialTypeIsAdditive(MaterialType.glow), isTrue);
      expect(materialTypeIsAdditive(MaterialType.shadeless), isFalse);
      expect(materialTypeIsAdditive(MaterialType.shaded), isFalse);
      expect(materialTypeIsAdditive(MaterialType.cutout), isFalse);
    });

    test('materialTypeFromName round-trips every value', () {
      for (final t in MaterialType.values) {
        expect(materialTypeFromName(t.name), t);
      }
      expect(materialTypeFromName(null), MaterialType.shadeless);
      expect(materialTypeFromName('unknown'), MaterialType.shadeless);
    });
  });

  group('ShadelessMaterial', () {
    test('shade returns the base color at full alpha', () {
      final m = ShadelessMaterial(
        baseColor: Color.fromARGB(255, 200, 100, 50),
        opacity: 1.0,
      );
      final out = m.shade(const ShadeContext());
      expect(out.alpha, 255);
      expect(out.red, 200);
      expect(out.green, 100);
      expect(out.blue, 50);
    });

    test('opacity scales the alpha channel', () {
      final m = ShadelessMaterial(
        baseColor: Color.fromARGB(255, 0, 0, 0),
        opacity: 0.5,
      );
      final out = m.shade(const ShadeContext());
      expect(out.alpha, closeTo(128, 1));
    });

    test('tint moves the base color toward the tint color', () {
      final m = ShadelessMaterial(
        baseColor: Color.fromARGB(255, 0, 0, 0),
        tintColor: Color.fromARGB(255, 255, 255, 255),
        tintStrength: 0.5,
      );
      final out = m.shade(const ShadeContext());
      // 50% blend toward white -> gray ≈ 128.
      expect(out.red, closeTo(128, 1));
      expect(out.green, closeTo(128, 1));
      expect(out.blue, closeTo(128, 1));
    });

    test('ignores lighting context (no normal/viewDir needed)', () {
      final m = ShadelessMaterial(
        baseColor: Color.fromARGB(255, 10, 20, 30),
      );
      final noCtx = m.shade(const ShadeContext());
      final withCtx = m.shade(const ShadeContext(
        normal: Vec3(1, 0, 0),
        viewDir: Vec3(0, 1, 0),
      ));
      expect(noCtx, withCtx);
    });

    test('flat factory and copy preserve values', () {
      final m = ShadelessMaterial.flat(const Color.fromARGB(255, 5, 6, 7));
      final c = m.copy();
      expect(c.baseColor, m.baseColor);
      expect(c.opacity, m.opacity);
    });
  });

  group('ShadedMaterial', () {
    final rig = MaterialLightRig(
      direction: const Vec3(0, -1, 0),
      intensity: 1.0,
      ambient: 0.5,
      specularStrength: 0.5,
      shininess: 32.0,
    );

    test('a normal aligned with the light direction is brighter than one '
        'opposite to it', () {
      final m = ShadedMaterial(
        baseColor: Color.fromARGB(255, 200, 200, 200),
      );
      // Normal aligned with the light direction — the specular term
      // (reflection-vector form) lands a highlight toward the viewer.
      final bright = m.shade(ShadeContext(
        normal: const Vec3(0, -1, 0),
        viewDir: const Vec3(0, -1, 0),
        lightRig: rig,
      ));
      // Normal opposite the light direction — faces the light source so
      // lambert diffuse is maximal, but no specular highlight reaches
      // the viewer along this normal.
      final dark = m.shade(ShadeContext(
        normal: const Vec3(0, 1, 0),
        viewDir: const Vec3(0, 1, 0),
        lightRig: rig,
      ));
      expect(bright.red, greaterThan(dark.red));
    });

    test('without a rig the shade falls back to the base color', () {
      final m = ShadedMaterial(
        baseColor: Color.fromARGB(255, 100, 150, 200),
      );
      final out = m.shade(const ShadeContext());
      expect(out.red, 100);
      expect(out.green, 150);
      expect(out.blue, 200);
    });

    test('roughness maps to shininess (0→128, 1→4)', () {
      final mirror = ShadedMaterial(
        baseColor: Color.fromARGB(255, 255, 255, 255),
        roughness: 0.0,
      );
      final chalk = ShadedMaterial(
        baseColor: Color.fromARGB(255, 255, 255, 255),
        roughness: 1.0,
      );
      expect(mirror.shininess, 128);
      expect(chalk.shininess, 4);
    });

    test('matte factory disables specular', () {
      final m = ShadedMaterial.matte(const Color.fromARGB(255, 128, 128, 128));
      expect(m.specularStrength, 0.0);
      expect(m.roughness, 1.0);
    });

    test('MaterialLightRig.lambert floors at ambient when n·(-L) ≤ 0', () {
      final r = MaterialLightRig(
        direction: const Vec3(0, -1, 0),
        intensity: 1.0,
        ambient: 0.4,
      );
      // Normal facing the light source (opposite the light direction) —
      // n·(-L) = 1, so lambert returns ambient + (1-ambient) = 1.0.
      final toward = r.lambert(const Vec3(0, 1, 0));
      expect(toward, closeTo(1.0, 1e-6));
      // Normal aligned with the light direction (facing away from the
      // source) — n·(-L) = -1, clamped to 0, so lambert returns the
      // ambient floor.
      final away = r.lambert(const Vec3(0, -1, 0));
      expect(away, closeTo(0.4, 1e-6));
    });

    test('MaterialLightRig.phong returns 0 when n·L ≤ 0', () {
      final r = MaterialLightRig(
        direction: const Vec3(0, -1, 0),
        intensity: 1.0,
        specularStrength: 0.5,
        shininess: 16,
      );
      // Normal opposite the light direction: ndl ≤ 0, phong returns 0.
      final spec = r.phong(const Vec3(0, 1, 0), const Vec3(0, 1, 0));
      expect(spec, 0.0);
    });
  });

  group('GlowMaterial', () {
    test('brightness follows the intensity ramp', () {
      final m0 = GlowMaterial(intensity: 0.0);
      final m1 = GlowMaterial(intensity: 1.0);
      final m2 = GlowMaterial(intensity: 2.0);
      expect(m0.brightness(), 0.0);
      expect(m1.brightness(), 1.0);
      // >1 maps to 1 + (i-1)*0.5, so 2.0 -> 1.5.
      expect(m2.brightness(), 1.5);
    });

    test('shade scales the base color by brightness', () {
      final m = GlowMaterial(
        baseColor: Color.fromARGB(255, 255, 255, 255),
        intensity: 0.5,
      );
      final out = m.shade(const ShadeContext());
      // 0.5 intensity maps to 0.5 brightness.
      expect(out.red, closeTo(128, 1));
    });

    test('isAdditive is true (additive blending)', () {
      final m = GlowMaterial();
      expect(m.isAdditive, isTrue);
    });

    test('crossSectionWeight fades out beyond the glow area', () {
      final m = GlowMaterial(glowArea: 0.5);
      expect(m.crossSectionWeight(0.0), 1.0);
      expect(m.crossSectionWeight(0.5), 0.0);
      expect(m.crossSectionWeight(1.0), 0.0);
    });

    test('bloomFalloff is smoothstep from 1 to 0', () {
      final m = GlowMaterial();
      expect(m.bloomFalloff(0.0), 1.0);
      expect(m.bloomFalloff(1.0), 0.0);
      expect(m.bloomFalloff(0.5), closeTo(0.5, 1e-6));
    });

    test('withIntensity returns a copy with the new value', () {
      final m = GlowMaterial(intensity: 1.0);
      final m2 = m.withIntensity(2.5);
      expect(m2.intensity, 2.5);
      expect(m.intensity, 1.0); // original unchanged
    });
  });

  group('CutoutMaterial', () {
    test('uses the background sampler when bound', () {
      final m = CutoutMaterial();
      final out = m.shade(ShadeContext(
        worldPos: const Vec3(1, 2, 3),
        backgroundSampler: (_) => 0xFFAABBCC, // packed ARGB
      ));
      expect(out.alpha, 255);
      expect(out.red, 0xAA);
      expect(out.green, 0xBB);
      expect(out.blue, 0xCC);
    });

    test('falls back to the fallback color when no sampler is bound', () {
      final m = CutoutMaterial(
        fallbackColor: Color.fromARGB(255, 10, 20, 30),
      );
      final out = m.shade(const ShadeContext());
      expect(out.red, 10);
      expect(out.green, 20);
      expect(out.blue, 30);
    });

    test('solid factory packs the ARGB into the fallback color', () {
      final m = CutoutMaterial.solid(0xFF778899);
      expect(m.fallbackColor.red, 0x77);
      expect(m.fallbackColor.green, 0x88);
      expect(m.fallbackColor.blue, 0x99);
    });

    test('solidSampler ignores world position and returns the packed color', () {
      final sampler = CutoutMaterial.solidSampler(0xFF112233);
      expect(sampler(const Vec3(0, 0, 0)), 0xFF112233);
      expect(sampler(const Vec3(99, 99, 99)), 0xFF112233);
    });

    test('opacity scales the alpha channel', () {
      final m = CutoutMaterial(
        opacity: 0.5,
        fallbackColor: Color.fromARGB(255, 0, 0, 0),
      );
      final out = m.shade(const ShadeContext());
      expect(out.alpha, closeTo(128, 1));
    });
  });

  group('FeatherMaterial JSON dispatch', () {
    test('fromJson reconstructs each subclass by type', () {
      final cases = <FeatherMaterial>[
        ShadelessMaterial(),
        ShadedMaterial(),
        GlowMaterial(),
        CutoutMaterial(),
      ];
      for (final m in cases) {
        final json = m.toJson();
        final restored = FeatherMaterial.fromJson(json);
        expect(restored.type, m.type);
        expect(restored.opacity, m.opacity);
      }
    });

    test('CutoutMaterial is forced to have no pattern', () {
      final m = CutoutMaterial();
      expect(m.pattern, isNull);
    });

    test('GlowMaterial is forced to have no pattern', () {
      final m = GlowMaterial();
      expect(m.pattern, isNull);
    });

    test('patternGenerator coverage is deterministic and in [0, 1]', () {
      const settings = PatternSettings(
        type: PatternType.dot,
        scale: 4.0,
        intensity: 0.7,
      );
      final gen = PatternGenerator(settings);
      // Same input -> same output.
      final a = gen.sample(0.25, 0.25);
      final b = gen.sample(0.25, 0.25);
      expect(a, b);
      expect(a, greaterThanOrEqualTo(0.0));
      expect(a, lessThanOrEqualTo(1.0));
    });
  });

  group('packArgb helper', () {
    test('packs linear 0..1 channels into a 0xAARRGGBB int', () {
      expect(packArgb(1, 1, 0, 0), 0xFFFF0000);
      expect(packArgb(1, 0, 1, 0), 0xFF00FF00);
      expect(packArgb(1, 0, 0, 1), 0xFF0000FF);
      expect(packArgb(0, 1, 1, 1), 0x00FFFFFF);
    });

    test('clamps out-of-range channels to 0..1', () {
      expect(packArgb(2, 2, -1, 0), 0xFFFF0000);
    });
  });
}
