// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// color_picker_test.dart — v0.57-C tests for HsvColorModel.
//
// Validates the HSV↔ARGB↔hex round-trips + the wheel/square mapping
// helpers exposed by lib/engine/color/color_picker.dart. These are the
// surfaces the host (lib/screens/main_screen.dart `_setActiveColorFromHex`)
// now depends on for the hex-code input pad contract (per
// brushes_color.txt "Tap the hex code to open the input pad for entering
// a hex code").

import 'dart:math' as math;

import 'package:feather_krita/engine/color/color_picker.dart';
import 'package:flutter_test/flutter_test.dart';

void main() {
  group('HsvColorModel defaults + assertions', () {
    test('default constructor yields opaque white (V=1, A=1)', () {
      final m = HsvColorModel();
      expect(m.hue, 0.0);
      expect(m.saturation, 0.0);
      expect(m.value, 1.0);
      expect(m.alpha, 1.0);
      // White = 0xFFFFFFFF.
      expect(m.toArgb(), 0xFFFFFFFF);
    });

    test('clamps via assertions (hue range, sat/val/alpha range)', () {
      // Within-range construction succeeds.
      HsvColorModel(hue: 359.99, saturation: 1.0, value: 0.0, alpha: 0.0);
      // Out-of-range throws (assertion).
      expect(() => HsvColorModel(hue: 360.0), throwsA(isA<Object>()));
      expect(() => HsvColorModel(hue: -0.01), throwsA(isA<Object>()));
      expect(() => HsvColorModel(saturation: 1.01), throwsA(isA<Object>()));
      expect(() => HsvColorModel(value: -0.01), throwsA(isA<Object>()));
      expect(() => HsvColorModel(alpha: 1.01), throwsA(isA<Object>()));
    });
  });

  group('HsvColorModel HSV→ARGB (toArgb)', () {
    test('pure red (H=0, S=1, V=1) → 0xFFFF0000', () {
      expect(HsvColorModel(hue: 0, saturation: 1, value: 1).toArgb(),
          0xFFFF0000);
    });

    test('pure green (H=120, S=1, V=1) → 0xFF00FF00', () {
      expect(HsvColorModel(hue: 120, saturation: 1, value: 1).toArgb(),
          0xFF00FF00);
    });

    test('pure blue (H=240, S=1, V=1) → 0xFF0000FF', () {
      expect(HsvColorModel(hue: 240, saturation: 1, value: 1).toArgb(),
          0xFF0000FF);
    });

    test('yellow (H=60, S=1, V=1) → 0xFFFF00', () {
      expect(HsvColorModel(hue: 60, saturation: 1, value: 1).toArgb(),
          0xFFFFFF00);
    });

    test('cyan (H=180, S=1, V=1) → 0xFF00FFFF', () {
      expect(HsvColorModel(hue: 180, saturation: 1, value: 1).toArgb(),
          0xFF00FFFF);
    });

    test('magenta (H=300, S=1, V=1) → 0xFFFF00FF', () {
      expect(HsvColorModel(hue: 300, saturation: 1, value: 1).toArgb(),
          0xFFFF00FF);
    });

    test('grey (S=0, V=0.5) → 0xFF808080 (mid-grey)', () {
      // V=0.5, S=0 → R=G=B=0.5*255≈128 = 0x80.
      expect(HsvColorModel(hue: 0, saturation: 0, value: 0.5).toArgb(),
          0xFF808080);
    });

    test('alpha channel is packed into the high byte', () {
      final argb = HsvColorModel(
              hue: 0, saturation: 1, value: 1, alpha: 0.5)
          .toArgb();
      expect((argb >> 24) & 0xff, 128); // 0.5*255 rounded = 128.
      expect(argb & 0xFFFFFF, 0xFF0000); // red component.
    });
  });

  group('HsvColorModel ARGB→HSV (fromArgb round-trip)', () {
    test('red round-trips through fromArgb', () {
      final m = HsvColorModel.fromArgb(0xFFFF0000);
      expect(m.hue, closeTo(0, 1e-6));
      expect(m.saturation, closeTo(1, 1e-6));
      expect(m.value, closeTo(1, 1e-6));
      expect(m.alpha, closeTo(1, 1e-6));
    });

    test('green round-trips through fromArgb', () {
      final m = HsvColorModel.fromArgb(0xFF00FF00);
      expect(m.hue, closeTo(120, 1e-6));
      expect(m.saturation, closeTo(1, 1e-6));
      expect(m.value, closeTo(1, 1e-6));
    });

    test('blue round-trips through fromArgb', () {
      final m = HsvColorModel.fromArgb(0xFF0000FF);
      expect(m.hue, closeTo(240, 1e-6));
      expect(m.saturation, closeTo(1, 1e-6));
      expect(m.value, closeTo(1, 1e-6));
    });

    test('grey (S=0) round-trips with hue=0 fallback', () {
      // When S=0, hue is undefined — fromArgb returns hue=0.
      // 0x80 = 128; 128/255 ≈ 0.50196 (NOT exactly 0.5 — quantization).
      final m = HsvColorModel.fromArgb(0xFF808080);
      expect(m.hue, 0);
      expect(m.saturation, 0);
      expect(m.value, closeTo(128 / 255, 1e-6));
    });

    test('toArgb ∘ fromArgb = identity for primary colours', () {
      for (final argb in [0xFFFF0000, 0xFF00FF00, 0xFF0000FF, 0xFFFFFFFF, 0xFF000000]) {
        expect(HsvColorModel.fromArgb(argb).toArgb(), argb);
      }
    });
  });

  group('HsvColorModel hex conversions', () {
    test('toHex: opaque red → "#FF0000"', () {
      expect(HsvColorModel(hue: 0, saturation: 1, value: 1).toHex(),
          '#FF0000');
    });

    test('toHex: semi-transparent red → "#80FF0000"', () {
      final m = HsvColorModel(hue: 0, saturation: 1, value: 1, alpha: 0x80 / 255);
      expect(m.toHex(), '#80FF0000');
    });

    test('fromHex accepts "#RRGGBB"', () {
      final m = HsvColorModel.fromHex('#FF0000');
      expect(m.toArgb(), 0xFFFF0000);
      expect(m.alpha, 1.0);
    });

    test('fromHex accepts "#AARRGGBB"', () {
      final m = HsvColorModel.fromHex('#80FF0000');
      expect(m.toArgb(), 0x80FF0000);
      expect(m.alpha, closeTo(0x80 / 255, 1e-3));
    });

    test('fromHex accepts bare "RRGGBB" (no hash)', () {
      expect(HsvColorModel.fromHex('00FF00').toArgb(), 0xFF00FF00);
    });

    test('fromHex accepts bare "AARRGGBB" (no hash)', () {
      expect(HsvColorModel.fromHex('80FF0000').toArgb(), 0x80FF0000);
    });

    test('fromHex trims whitespace', () {
      expect(HsvColorModel.fromHex('  #FF0000  ').toArgb(), 0xFFFF0000);
    });

    test('fromHex → toHex round-trips for opaque colours', () {
      // Red, green, blue, white round-trip exactly.
      for (final hex in ['#FF0000', '#00FF00', '#0000FF', '#FFFFFF', '#000000']) {
        expect(HsvColorModel.fromHex(hex).toHex(), hex);
      }
    });

    test('fromHex sentinel: unparseable input → alpha=0 (caller can detect)', () {
      // This is the contract the host (main_screen._setActiveColorFromHex)
      // relies on to detect invalid input without a nullable return type.
      final garbage = HsvColorModel.fromHex('not-a-hex');
      expect(garbage.alpha, 0.0);
      expect(garbage.toArgb() >> 24, 0); // high byte = 0 → invalid.
    });

    test('fromHex sentinel: empty string → alpha=0', () {
      expect(HsvColorModel.fromHex('').alpha, 0.0);
    });

    test('fromHex does NOT silently return opaque black for valid black', () {
      // "#000000" is valid opaque black — must NOT be the sentinel.
      final black = HsvColorModel.fromHex('#000000');
      expect(black.alpha, 1.0); // opaque, not sentinel.
      expect(black.toArgb(), 0xFF000000);
    });
  });

  group('HsvColorModel wheel-angle ↔ hue mapping', () {
    test('spinWheelTo sets hue modulo 360', () {
      final m = HsvColorModel();
      m.spinWheelTo(45);
      expect(m.hue, 45);
      m.spinWheelTo(360);
      expect(m.hue, 0);
      m.spinWheelTo(720);
      expect(m.hue, 0);
      m.spinWheelTo(-90);
      expect(m.hue, 270); // -90 + 360 = 270.
    });

    test('spinWheel increments hue by delta (negative = CCW)', () {
      final m = HsvColorModel(hue: 10);
      m.spinWheel(20);
      expect(m.hue, 30);
      m.spinWheel(-40);
      expect(m.hue, 350); // 30-40=-10 → 350.
    });

    test('squareUv ↔ setSquareUv round-trips S and V', () {
      final m = HsvColorModel(hue: 180, saturation: 0.3, value: 0.7);
      final uv = m.squareUv;
      expect(uv.s, 0.3);
      expect(uv.v, 0.7);
      m.setSquareUv((s: 0.9, v: 0.1));
      expect(m.saturation, 0.9);
      expect(m.value, 0.1);
    });

    test('setSquareUv clamps out-of-range UV to [0, 1]', () {
      final m = HsvColorModel();
      m.setSquareUv((s: 1.5, v: -0.5));
      expect(m.saturation, 1.0);
      expect(m.value, 0.0);
    });
  });

  group('HsvColorModel.copyWith + toString', () {
    test('copyWith overrides only the supplied fields', () {
      final m = HsvColorModel(hue: 30, saturation: 0.5, value: 0.8, alpha: 0.6);
      final m2 = m.copyWith(hue: 90);
      expect(m2.hue, 90);
      expect(m2.saturation, 0.5);
      expect(m2.value, 0.8);
      expect(m2.alpha, 0.6);
    });

    test('toString mentions h/s/v/a fields', () {
      final s = HsvColorModel(hue: 1, saturation: 0.5, value: 0.5, alpha: 0.5).toString();
      expect(s.contains('h='), true);
      expect(s.contains('s='), true);
      expect(s.contains('v='), true);
      expect(s.contains('a='), true);
    });
  });

  group('Host wiring contract (main_screen._setActiveColorFromHex)', () {
    // These tests pin the contract that the host's
    // `_setActiveColorFromHex` helper depends on. They are intentionally
    // minimal — full HSV↔ARGB coverage is above.

    test('valid hex → ARGB high byte != 0 (host accepts)', () {
      for (final hex in ['#FF0000', '#00FF00', '#0000FF', '#FFFFFF', '#808080']) {
        final argb = HsvColorModel.fromHex(hex).toArgb();
        expect(argb >> 24, 0xFF, reason: '$hex should be opaque');
      }
    });

    test('invalid hex → ARGB high byte == 0 (host rejects)', () {
      for (final hex in ['', 'garbage', '#GGGGGG', 'zzz', '   ', '12345']) {
        final argb = HsvColorModel.fromHex(hex).toArgb();
        expect(argb >> 24, 0, reason: '$hex should be the sentinel (alpha=0)');
      }
    });
  });
}
