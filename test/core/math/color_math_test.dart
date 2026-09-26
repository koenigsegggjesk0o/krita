// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// color_math_test.dart — unit tests for lib/core/math/color_math.dart.
//
// color_math had NO test file before v0.57-D. This file covers the sRGB
// ↔linear transfer functions (round-trip + the IEC 61966-2-1 boundary
// values), the linear-space mix/multiply/add helpers, the Rec. 709
// luminance weights, and both tone-map operators (Reinhard + ACES).
//
// v0.57-D honest note: color_math has NO runtime consumer in the in-scope
// files (see the file header doc). These tests pin the math so a future
// rendering/material wiring can rely on it.

import 'package:feather_krita/core/math/color_math.dart' as color_math;
import 'package:feather_krita/core/math/vec3.dart';
import 'package:flutter_test/flutter_test.dart';

void main() {
  group('sRGB ↔ linear transfer', () {
    test('toLinear + toSRGB round-trip recovers the original colour', () {
      for (final v in <double>[0.0, 0.1, 0.25, 0.5, 0.75, 0.9, 1.0]) {
        final srgb = Vec3(v, v, v);
        final back = color_math.toSRGB(color_math.toLinear(srgb));
        expect(back.x, closeTo(v, 1e-12),
            reason: 'round-trip failed at channel value $v');
        expect(back.y, closeTo(v, 1e-12));
        expect(back.z, closeTo(v, 1e-12));
      }
    });

    test('toLinear matches the IEC 61966-2-1 piecewise transfer', () {
      // Below the 0.04045 threshold: linear division by 12.92.
      expect(color_math.toLinear(const Vec3(0.0, 0.0, 0.0)),
          const Vec3(0.0, 0.0, 0.0));
      final low = color_math.toLinear(const Vec3(0.02, 0.02, 0.02));
      expect(low.x, closeTo(0.02 / 12.92, 1e-15));
      // Above the threshold: ((c + 0.055) / 1.055) ^ 2.4. At sRGB 1.0
      // the linear value is exactly 1.0.
      final high = color_math.toLinear(const Vec3(1.0, 1.0, 1.0));
      expect(high.x, closeTo(1.0, 1e-15));
      // A mid-grey sRGB ~0.5 maps to linear ~0.214 (well-known value).
      final mid = color_math.toLinear(const Vec3(0.5, 0.5, 0.5));
      expect(mid.x, closeTo(0.214041140482, 1e-6));
    });

    test('toSRGB is the inverse at the boundary points', () {
      // linear 0 → sRGB 0; linear 1 → sRGB 1.
      expect(color_math.toSRGB(const Vec3(0.0, 0.0, 0.0)),
          const Vec3(0.0, 0.0, 0.0));
      expect(color_math.toSRGB(const Vec3(1.0, 1.0, 1.0)).x, closeTo(1.0, 1e-15));
      // linear 0.0031308 threshold: below it sRGB = linear * 12.92.
      final low = color_math.toSRGB(const Vec3(0.001, 0.001, 0.001));
      expect(low.x, closeTo(0.001 * 12.92, 1e-15));
    });
  });

  group('linear-space combine helpers', () {
    test('mix is linear interpolation in linear space', () {
      const a = Vec3(0.0, 0.0, 0.0);
      const b = Vec3(1.0, 1.0, 1.0);
      expect(color_math.mix(a, b, 0.0), a);
      expect(color_math.mix(a, b, 1.0), b);
      final mid = color_math.mix(a, b, 0.5);
      expect(mid.x, closeTo(0.5, 1e-15));
    });

    test('multiply is component-wise (modulative)', () {
      const a = Vec3(0.5, 0.25, 0.75);
      const b = Vec3(0.8, 0.4, 0.2);
      final out = color_math.multiply(a, b);
      expect(out.x, closeTo(0.4, 1e-15));
      expect(out.y, closeTo(0.1, 1e-15));
      expect(out.z, closeTo(0.15, 1e-15));
    });

    test('add is component-wise', () {
      const a = Vec3(0.1, 0.2, 0.3);
      const b = Vec3(0.4, 0.5, 0.6);
      final out = color_math.add(a, b);
      expect(out.x, closeTo(0.5, 1e-15));
      expect(out.y, closeTo(0.7, 1e-15));
      expect(out.z, closeTo(0.9, 1e-15));
    });
  });

  group('luminance (Rec. 709)', () {
    test('black is 0, white is ~1, green dominates the weights', () {
      expect(color_math.luminance(const Vec3(0, 0, 0)), 0.0);
      expect(color_math.luminance(const Vec3(1, 1, 1)), closeTo(1.0, 1e-15));
      // Pure green (0.7152) is brighter than pure red (0.2126) or blue (0.0722).
      final lg = color_math.luminance(const Vec3(0, 1, 0));
      final lr = color_math.luminance(const Vec3(1, 0, 0));
      final lb = color_math.luminance(const Vec3(0, 0, 1));
      expect(lg, greaterThan(lr));
      expect(lr, greaterThan(lb));
      expect(lg, closeTo(0.7152, 1e-15));
      expect(lr, closeTo(0.2126, 1e-15));
      expect(lb, closeTo(0.0722, 1e-15));
    });
  });

  group('tone mapping', () {
    test('Reinhard maps L → L/(1+L) + black stays black', () {
      // Black maps to black.
      expect(
          color_math
              .toneMap(const Vec3(0, 0, 0), mode: color_math.ToneMapMode.reinhard),
          const Vec3(0, 0, 0));
      // A linear 1.0 grey maps to 0.5 (1/(1+1)).
      final out = color_math.toneMap(const Vec3(1, 1, 1),
          mode: color_math.ToneMapMode.reinhard);
      expect(out.x, closeTo(0.5, 1e-6));
      // HDR (>1) values are compressed toward 1 but never exceed it.
      final hdr = color_math.toneMap(const Vec3(10, 10, 10),
          mode: color_math.ToneMapMode.reinhard);
      expect(hdr.x, lessThan(1.0));
      expect(hdr.x, greaterThan(0.5));
    });

    test('ACES clamps to [0,1] + compresses HDR', () {
      // LDR in-range values stay roughly in range (ACES is non-linear).
      final ldr = color_math.toneMap(const Vec3(0.5, 0.5, 0.5),
          mode: color_math.ToneMapMode.aces);
      expect(ldr.x, greaterThanOrEqualTo(0.0));
      expect(ldr.x, lessThanOrEqualTo(1.0));
      // A moderate HDR (2.0) is compressed below 1.0 (≈0.915).
      final hdr = color_math.toneMap(const Vec3(2.0, 2.0, 2.0),
          mode: color_math.ToneMapMode.aces);
      expect(hdr.x, lessThan(1.0));
      expect(hdr.x, greaterThan(0.0));
      // Extreme HDR (20.0) saturates to exactly 1.0 (clamped).
      final extreme = color_math.toneMap(const Vec3(20.0, 20.0, 20.0),
          mode: color_math.ToneMapMode.aces);
      expect(extreme.x, lessThanOrEqualTo(1.0));
      expect(extreme.x, closeTo(1.0, 1e-12));
      // Black stays black.
      expect(
          color_math
              .toneMap(const Vec3(0, 0, 0), mode: color_math.ToneMapMode.aces),
          const Vec3(0, 0, 0));
    });

    test('default toneMap mode is ACES', () {
      // Omitting mode should match ACES explicitly.
      final implicit = color_math.toneMap(const Vec3(2.0, 2.0, 2.0));
      final explicit = color_math.toneMap(const Vec3(2.0, 2.0, 2.0),
          mode: color_math.ToneMapMode.aces);
      expect(implicit.x, closeTo(explicit.x, 1e-15));
    });
  });
}
