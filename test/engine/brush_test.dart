// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// brush_test.dart — unit tests for the brush engine (procedural fallback).

import 'package:feather_krita/engine/brush/brush_engine.dart';
import 'package:feather_krita/engine/brush/brush_preset.dart';
import 'package:feather_krita/engine/brush/brush_settings.dart';
import 'package:feather_krita/ffi/krita_bindings.dart';
import 'package:feather_krita/models/stroke.dart';
import 'package:flutter_test/flutter_test.dart';

import 'package:vector_math/vector_math_64.dart';

StrokePoint _p(double x, double y, [double pressure = 0.5]) =>
    StrokePoint(position: Vector3(x, y, 0), pressure: pressure);

void main() {
  group('BrushEngine.beginStroke', () {
    test('begins an active stroke and seeds the pending list', () {
      final engine = ProceduralBrushEngine();
      expect(engine.isStrokeActive, isFalse);
      engine.beginStroke(_p(0, 0));
      expect(engine.isStrokeActive, isTrue);
    });

    test('beginStroke resets any in-progress stroke', () {
      final engine = ProceduralBrushEngine(
        settings: const BrushSettings(smoothing: 0.0),
      );
      engine.beginStroke(_p(0, 0));
      engine.addPoint(_p(1, 1));
      engine.addPoint(_p(2, 2));
      // Re-begin — the previous stroke must be discarded. After adding
      // a single follow-up point the new stroke contains exactly two
      // samples (the re-begin seed + the new point), with no leftover
      // from the original (0,0)→(2,2) stroke.
      engine.beginStroke(_p(10, 10));
      engine.addPoint(_p(11, 11));
      final stroke = engine.endStroke();
      expect(stroke, isNotNull);
      expect(stroke!.points.length, 2);
      expect(stroke.points.first.position.x, 10);
      expect(stroke.points.last.position.x, 11);
    });
  });

  group('BrushEngine.addPoint', () {
    test('addPoint without beginStroke is a no-op', () {
      final engine = ProceduralBrushEngine();
      engine.addPoint(_p(5, 5));
      expect(engine.endStroke(), isNull);
    });

    test('addPoint applies EMA smoothing at high smoothing', () {
      final engine = ProceduralBrushEngine(
        settings: const BrushSettings(smoothing: 0.9),
      );
      engine.beginStroke(_p(0, 0));
      // Add a point that is far from the start. With smoothing 0.9,
      // alpha = 1 - 0.9*0.85 = 0.235. The smoothed position is mostly
      // the previous (origin) with a small step toward (100, 100).
      engine.addPoint(_p(100, 100));
      final stroke = engine.endStroke();
      expect(stroke, isNotNull);
      final smoothed = stroke!.points.last.position;
      // The smoothed position must be closer to (0,0) than to (100,100).
      expect(smoothed.x, lessThan(50));
      expect(smoothed.y, lessThan(50));
    });

    test('addPoint at smoothing=0 records the raw position', () {
      final engine = ProceduralBrushEngine(
        settings: const BrushSettings(smoothing: 0.0),
      );
      engine.beginStroke(_p(0, 0));
      engine.addPoint(_p(42, 17));
      final stroke = engine.endStroke();
      expect(stroke, isNotNull);
      final p = stroke!.points.last.position;
      expect(p.x, 42);
      expect(p.y, 17);
    });
  });

  group('BrushEngine.endStroke', () {
    test('endStroke returns null when fewer than two samples accumulated', () {
      final engine = ProceduralBrushEngine();
      engine.beginStroke(_p(0, 0));
      expect(engine.endStroke(), isNull);
      expect(engine.isStrokeActive, isFalse);
    });

    test('endStroke returns a Stroke with the engine color and size', () {
      final engine = ProceduralBrushEngine(
        settings: const BrushSettings(size: 12.0),
        color: 0xFF112233,
      );
      engine.beginStroke(_p(0, 0));
      engine.addPoint(_p(1, 0));
      final stroke = engine.endStroke();
      expect(stroke, isNotNull);
      expect(stroke!.color, 0xFF112233);
      expect(stroke.thickness, 12.0);
      expect(stroke.brushType, BrushType.basic);
    });

    test('endStroke sets the engine inactive', () {
      final engine = ProceduralBrushEngine();
      engine.beginStroke(_p(0, 0));
      engine.addPoint(_p(1, 0));
      engine.endStroke();
      expect(engine.isStrokeActive, isFalse);
    });

    test('endStroke with default brush type and explicit type override', () {
      final engine = ProceduralBrushEngine();
      engine.beginStroke(_p(0, 0));
      engine.addPoint(_p(1, 0));
      final stroke = engine.endStroke(brushType: BrushType.pencil);
      expect(stroke!.brushType, BrushType.pencil);
    });
  });

  group('BrushEngine settings & preset', () {
    test('default settings size = 8 and color = black', () {
      final engine = ProceduralBrushEngine();
      expect(engine.settings.size, 8.0);
      expect(engine.color, 0xFF000000);
    });

    test('custom settings and color are honoured', () {
      final engine = ProceduralBrushEngine(
        settings: const BrushSettings(size: 25.0, opacity: 0.5),
        color: 0xFFFF0000,
      );
      expect(engine.settings.size, 25.0);
      expect(engine.settings.opacity, 0.5);
      expect(engine.color, 0xFFFF0000);
    });

    test('settings can be swapped at runtime', () {
      final engine = ProceduralBrushEngine();
      engine.settings = const BrushSettings(size: 99.0);
      expect(engine.settings.size, 99.0);
    });

    test('preset can be attached and read back', () {
      final engine = ProceduralBrushEngine();
      final preset = FeatherBrushPreset(
        name: 'tester',
        color: 0xFF00FF00,
        thickness: 4.0,
      );
      engine.preset = preset;
      expect(engine.preset, isNotNull);
      expect(engine.preset!.name, 'tester');
      expect(engine.preset!.color, 0xFF00FF00);
    });
  });

  group('BrushSettings', () {
    test('sizeAtPressure ramps from minSize to size when target = size', () {
      const s = BrushSettings(
        size: 10.0,
        minSize: 2.0,
        pressureEnabled: true,
        pressureTarget: PressureTarget.size,
      );
      expect(s.sizeAtPressure(0.0), 2.0);
      expect(s.sizeAtPressure(1.0), 10.0);
      expect(s.sizeAtPressure(0.5), 6.0);
    });

    test('opacityAtPressure ramps with pressure when target = opacity', () {
      const s = BrushSettings(
        opacity: 0.8,
        pressureEnabled: true,
        pressureTarget: PressureTarget.opacity,
      );
      expect(s.opacityAtPressure(0.0), 0.0);
      expect(s.opacityAtPressure(1.0), 0.8);
    });

    test('pressure disabled returns the constant', () {
      const s = BrushSettings(
        size: 7.0,
        pressureEnabled: false,
      );
      expect(s.sizeAtPressure(0.0), 7.0);
      expect(s.sizeAtPressure(1.0), 7.0);
    });

    test('copyWith overrides only the specified fields', () {
      const s = BrushSettings(size: 5, opacity: 0.5);
      final modified = s.copyWith(size: 12);
      expect(modified.size, 12);
      expect(modified.opacity, 0.5);
    });

    test('JSON round-trip preserves all fields', () {
      const s = BrushSettings(
        size: 17.0,
        opacity: 0.42,
        flow: 0.9,
        spacing: 0.2,
        hardness: 0.6,
        smoothing: 0.3,
        pressureEnabled: true,
        pressureTarget: PressureTarget.opacity,
        minSize: 1.5,
      );
      final restored = BrushSettings.fromJson(s.toJson());
      expect(restored.size, s.size);
      expect(restored.opacity, s.opacity);
      expect(restored.flow, s.flow);
      expect(restored.spacing, s.spacing);
      expect(restored.hardness, s.hardness);
      expect(restored.smoothing, s.smoothing);
      expect(restored.pressureEnabled, s.pressureEnabled);
      expect(restored.pressureTarget, s.pressureTarget);
      expect(restored.minSize, s.minSize);
    });
  });

  group('ProceduralBrushEngine.generateDab', () {
    test('produces a non-empty dab with the configured size', () {
      final engine = ProceduralBrushEngine(
        settings: const BrushSettings(size: 4.0, hardness: 0.5),
      );
      final dab = engine.generateDab(const BrushInput(
        x: 0,
        y: 0,
        pressure: 0.5,
      ));
      expect(dab, isNotNull);
      expect(dab!.isNotEmpty, isTrue);
      // The dab size is approximately size*2*2 = 16 pixels per side.
      expect(dab.width, greaterThan(0));
      expect(dab.height, greaterThan(0));
      expect(dab.pixels.length, dab.stride * dab.height);
    });

    test('dab uses the engine color in its RGB channels', () {
      final engine = ProceduralBrushEngine(
        settings: const BrushSettings(size: 4.0),
        color: 0xFFFF0000, // pure red
      );
      final dab = engine.generateDab(const BrushInput(x: 0, y: 0, pressure: 1));
      // Pixel (cx, cy) should be the brush color: R=255, G=0, B=0.
      final cx = dab!.width ~/ 2;
      final cy = dab.height ~/ 2;
      final i = (cy * dab.stride + cx * 4);
      expect(dab.pixels[i], 255); // R
      expect(dab.pixels[i + 1], 0); // G
      expect(dab.pixels[i + 2], 0); // B
    });

    test('dispose is a no-op and safe to call', () {
      final engine = ProceduralBrushEngine();
      engine.dispose(); // Should not throw.
    });
  });

  group('BrushType helpers', () {
    test('brushTypeName round-trips via brushTypeFromName', () {
      for (final t in BrushType.values) {
        expect(brushTypeFromName(brushTypeName(t)), t);
      }
    });

    test('brushTypeFromName falls back to basic for unknown names', () {
      expect(brushTypeFromName(null), BrushType.basic);
      expect(brushTypeFromName(''), BrushType.basic);
      expect(brushTypeFromName('not-a-brush'), BrushType.basic);
    });
  });
}
