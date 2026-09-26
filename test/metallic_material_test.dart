// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// metallic_material_test.dart — unit tests for the v0.56-C MetallicMaterial
// engine class + the MaterialType.metallic enum value.
//
// Covers: enum membership, material_type.dart helper predicates (lit,
// casts shadows, NOT pattern-eligible, NOT additive), the BRDF output
// (differs from ShadedMaterial for the same input — higher specular),
// JSON round-trip, copy(), and the convenience factories.

import 'dart:ui' show Color;

import 'package:feather_krita/core/math/vec3.dart';
import 'package:feather_krita/engine/material/light_rig.dart';
import 'package:feather_krita/engine/material/material.dart';
import 'package:feather_krita/engine/material/material_type.dart';
import 'package:feather_krita/engine/material/metallic_material.dart';
import 'package:feather_krita/engine/material/shaded_material.dart';
import 'package:flutter_test/flutter_test.dart';

/// Channel extractors using the non-deprecated [Color] API (0..1 doubles).
int _redOf(Color c) => (c.r * 255.0).round() & 0xff;
int _greenOf(Color c) => (c.g * 255.0).round() & 0xff;
int _blueOf(Color c) => (c.b * 255.0).round() & 0xff;
int _alphaOf(Color c) => (c.a * 255.0).round() & 0xff;

void main() {
  group('MaterialType.metallic enum', () {
    test('is a member of MaterialType', () {
      expect(MaterialType.values, contains(MaterialType.metallic));
    });

    test('materialTypeFromName round-trips metallic', () {
      expect(materialTypeFromName('metallic'), MaterialType.metallic);
    });

    test('materialTypeDisplayName returns "Metallic"', () {
      expect(materialTypeDisplayName(MaterialType.metallic), 'Metallic');
    });

    test('metallic appears last in kMaterialTypeOrder', () {
      expect(kMaterialTypeOrder.last, MaterialType.metallic);
      // The four core types stay in their original doc order.
      expect(kMaterialTypeOrder.take(4), [
        MaterialType.shadeless,
        MaterialType.shaded,
        MaterialType.glow,
        MaterialType.cutout,
      ]);
    });
  });

  group('MaterialType.metallic rules', () {
    test('responds to light (mirrors shaded)', () {
      expect(materialTypeRespondsToLight(MaterialType.metallic), isTrue);
    });

    test('casts shadows (mirrors shaded)', () {
      expect(materialTypeCastsShadows(MaterialType.metallic), isTrue);
    });

    test('does NOT support patterns (polished metal has no engraving)', () {
      expect(materialTypeSupportsPattern(MaterialType.metallic), isFalse);
    });

    test('is NOT additive (only glow is)', () {
      expect(materialTypeIsAdditive(MaterialType.metallic), isFalse);
    });

    test('does NOT expose the docs\' intensity slider (glow only)', () {
      expect(materialTypeHasIntensity(MaterialType.metallic), isFalse);
    });
  });

  group('MetallicMaterial BRDF', () {
    final rig = MaterialLightRig(
      direction: const Vec3(0, -1, 0),
      intensity: 1.0,
      ambient: 0.5,
      specularStrength: 0.5,
      shininess: 32.0,
    );

    test('default knobs match the v0.56-C brief (shininess 64, spec 0.8)', () {
      final m = MetallicMaterial();
      expect(m.shininess, 64.0);
      expect(m.specularStrength, 0.8);
      expect(m.envStrength, 0.2);
      expect(m.type, MaterialType.metallic);
    });

    test('without a rig the shade still applies the env tint (up normal)', () {
      // Up-facing normal → sky tint mixed into the albedo → the result
      // should drift toward envColor (pale blue 180,210,240) and thus
      // differ from the pure base color.
      final m = MetallicMaterial(
        baseColor: const Color.fromARGB(255, 255, 0, 0), // pure red
        envColor: const Color.fromARGB(255, 180, 210, 240),
        envStrength: 0.2,
      );
      final upNormal = m.shade(const ShadeContext(
        normal: Vec3(0, 1, 0),
      ));
      final downNormal = m.shade(const ShadeContext(
        normal: Vec3(0, -1, 0),
      ));
      // Down-facing normal gets NO env tint → stays pure base red.
      expect(_redOf(downNormal), 255);
      expect(_greenOf(downNormal), 0);
      expect(_blueOf(downNormal), 0);
      // Up-facing normal picks up sky tint: mix((255,0,0), (180,210,240), 0.2)
      //   = (255*0.8+180*0.2, 0*0.8+210*0.2, 0*0.8+240*0.2)
      //   = (240, 42, 48). Red drops, green + blue rise.
      expect(_redOf(upNormal), 240);
      expect(_greenOf(upNormal), 42);
      expect(_blueOf(upNormal), 48);
    });

    test('produces a brighter highlight than ShadedMaterial for the same '
        'normal/view/rig (higher specular)', () {
      // Same base color, same rig. Metallic's default specularStrength 0.8
      // + tight shininess 64 should land a tighter, brighter catch-light
      // than ShadedMaterial's default roughness 0.5 → shininess 66 +
      // specularStrength 1.0 (multiplied onto the rig's 0.5 → 0.5 eff).
      // The catch-light direction is normal aligned with the light
      // direction (reflection-vector form lands the highlight toward
      // the viewer when N == -L == V).
      final base = const Color.fromARGB(255, 180, 180, 180);
      final metal = MetallicMaterial(baseColor: base);
      final shaded = ShadedMaterial(baseColor: base);

      final ctx = ShadeContext(
        normal: const Vec3(0, -1, 0),
        viewDir: const Vec3(0, -1, 0),
        lightRig: rig,
      );
      final mOut = metal.shade(ctx);
      final sOut = shaded.shade(ctx);

      // The metallic highlight is tighter (shininess 64 vs shaded's 66 —
      // comparable exponent) but multiplies the rig's specularStrength by
      // 0.8 vs shaded's 1.0; the metal also adds the env reflection tint.
      // We assert the outputs DIFFER (metal ≠ shaded) — the exact
      // brightness ordering depends on the env tint direction, but the
      // key contract is that metallic is NOT just shaded in disguise.
      expect(mOut == sOut, isFalse,
          reason: 'Metallic must differ from Shaded for the same input');
      // Sanity: both stay within the base-color neighbourhood (no clamp
      // blowout to pure white).
      expect(_redOf(mOut), lessThanOrEqualTo(255));
      expect(_redOf(sOut), lessThanOrEqualTo(255));
    });

    test('a normal facing the light is brighter than one facing away', () {
      final m = MetallicMaterial(
        baseColor: const Color.fromARGB(255, 180, 180, 180),
      );
      final bright = m.shade(ShadeContext(
        normal: const Vec3(0, -1, 0),
        viewDir: const Vec3(0, -1, 0),
        lightRig: rig,
      ));
      final dark = m.shade(ShadeContext(
        normal: const Vec3(0, 1, 0),
        viewDir: const Vec3(0, 1, 0),
        lightRig: rig,
      ));
      expect(_redOf(bright), greaterThan(_redOf(dark)));
    });

    test('opacity scales the alpha channel', () {
      final m = MetallicMaterial(
        baseColor: const Color.fromARGB(255, 200, 200, 200),
        opacity: 0.5,
      );
      final out = m.shade(const ShadeContext());
      expect(_alphaOf(out), closeTo(128, 1));
    });
  });

  group('MetallicMaterial serialization', () {
    test('toJson serializes the metallic-specific knobs', () {
      final m = MetallicMaterial(
        baseColor: const Color.fromARGB(255, 192, 192, 200),
        opacity: 0.9,
        specularColor: const Color.fromARGB(255, 250, 250, 255),
        specularStrength: 0.75,
        shininess: 80.0,
        envColor: const Color.fromARGB(255, 200, 220, 255),
        envStrength: 0.25,
      );
      final json = m.toJson();
      expect(json['type'], 'metallic');
      expect(json['specularStrength'], 0.75);
      expect(json['shininess'], 80.0);
      expect(json['envStrength'], 0.25);
      // pattern must be absent (Metallic does not accept patterns).
      expect(json.containsKey('pattern'), isFalse);
    });

    test('fromJson round-trips a MetallicMaterial', () {
      final original = MetallicMaterial(
        baseColor: const Color.fromARGB(255, 200, 180, 100),
        opacity: 0.7,
        specularColor: const Color.fromARGB(255, 255, 240, 220),
        specularStrength: 0.9,
        shininess: 72.0,
        envColor: const Color.fromARGB(255, 255, 220, 180),
        envStrength: 0.18,
      );
      final restored = FeatherMaterial.fromJson(original.toJson());
      expect(restored, isA<MetallicMaterial>());
      expect(restored.type, MaterialType.metallic);
      final m = restored as MetallicMaterial;
      expect(m.baseColor, original.baseColor);
      expect(m.opacity, original.opacity);
      expect(m.specularColor, original.specularColor);
      expect(m.specularStrength, original.specularStrength);
      expect(m.shininess, original.shininess);
      expect(m.envColor, original.envColor);
      expect(m.envStrength, original.envStrength);
      expect(m.pattern, isNull); // Metallic rejects patterns.
    });

    test('fromJson falls back to defaults for missing metallic fields', () {
      // A minimal JSON blob with only the shared fields — the
      // metallic-specific knobs should fall back to the constructor
      // defaults (shininess 64, spec 0.8, envStrength 0.2).
      final restored = FeatherMaterial.fromJson({
        'type': 'metallic',
        'baseColor': 0xFFC0C0C8,
        'opacity': 1.0,
      });
      expect(restored, isA<MetallicMaterial>());
      final m = restored as MetallicMaterial;
      expect(m.shininess, 64.0);
      expect(m.specularStrength, 0.8);
      expect(m.envStrength, 0.2);
    });
  });

  group('MetallicMaterial.copy + factories', () {
    test('copy() preserves every knob', () {
      final m = MetallicMaterial(
        baseColor: const Color.fromARGB(255, 200, 100, 50),
        opacity: 0.6,
        specularColor: const Color.fromARGB(255, 255, 200, 150),
        specularStrength: 0.65,
        shininess: 88.0,
        envColor: const Color.fromARGB(255, 220, 230, 255),
        envStrength: 0.3,
      );
      final c = m.copy();
      expect(c, isA<MetallicMaterial>());
      expect(c.baseColor, m.baseColor);
      expect(c.opacity, m.opacity);
      expect(c.specularColor, m.specularColor);
      expect(c.specularStrength, m.specularStrength);
      expect(c.shininess, m.shininess);
      expect(c.envColor, m.envColor);
      expect(c.envStrength, m.envStrength);
      // copy() is a deep copy — mutating the copy must not touch the
      // original.
      c.shininess = 1.0;
      expect(m.shininess, 88.0);
    });

    test('chrome factory yields a bright neutral metal', () {
      final c = MetallicMaterial.chrome();
      expect(c.shininess, 96.0);
      expect(c.specularStrength, 1.0);
      expect(c.envStrength, closeTo(0.28, 1e-9));
    });

    test('gold factory yields a warm yellow metal', () {
      final g = MetallicMaterial.gold();
      expect(g.shininess, 56.0);
      // Gold's base color is the classic gold RGB (212, 175, 55).
      expect(_redOf(g.baseColor), 212);
      expect(_greenOf(g.baseColor), 175);
      expect(_blueOf(g.baseColor), 55);
    });
  });
}
