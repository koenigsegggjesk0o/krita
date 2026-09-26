// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// assistance_wiring_test.dart — unit tests for the pure helpers in
// `lib/engine/assistance/assistance_wiring.dart` that bridge the four
// assistance subsystems to the live paint path.
//
// These tests cover the WIRING layer (how the host composes the existing
// engine APIs), not the underlying math (already covered by
// stable_strokes_test.dart, mirror_assist_test.dart,
// draw_shape_assist_test.dart, color_sampler_test.dart).

import 'dart:math' as math;

import 'package:feather_krita/engine/assistance/assistance_wiring.dart';
import 'package:feather_krita/engine/assistance/draw_shape_assist.dart';
import 'package:feather_krita/engine/assistance/mirror_assist.dart';
import 'package:feather_krita/engine/assistance/stable_strokes.dart';
import 'package:feather_krita/engine/curves/stroke3d.dart';
import 'package:feather_krita/models/stroke.dart';
import 'package:flutter_test/flutter_test.dart';
import 'package:vector_math/vector_math_64.dart';

Stroke3D _stroke(List<List<double>> pts,
    {int color = 0xFF0000FF, double thickness = 1.0}) {
  final samples = <StrokeSample3D>[];
  for (var i = 0; i < pts.length; i++) {
    samples.add(StrokeSample3D(
      position: Vector3(pts[i][0], pts[i][1], pts[i][2]),
      pressure: 0.8,
      time: i * 0.01,
    ));
  }
  return Stroke3D(
    samples: samples,
    color: color,
    thickness: thickness,
    source: 'test',
  );
}

Stroke _renderStroke(Stroke3D s, {int id = 1}) {
  final points = <StrokePoint>[];
  for (var i = 0; i < s.length; i++) {
    final p = s.worldPosition(i);
    points.add(StrokePoint(position: p, pressure: s.samples[i].pressure));
  }
  return Stroke(
    id: id,
    points: points,
    color: s.color,
    thickness: s.thickness,
  );
}

void main() {
  group('smoothStrokeSample — Stable Strokes wiring', () {
    test('disabled stabilizer returns the raw position unchanged', () {
      final s = StableStrokes(config: const StableStrokesConfig(enabled: false));
      final out = smoothStrokeSample(s, Vector3(1, 2, 3),
          pressure: 0.5, time: 0.1);
      expect(out, isNotNull);
      expect(out!.x, 1);
      expect(out.y, 2);
      expect(out.z, 3);
    });

    test('enabled stabilizer returns null on the first (warm-up) sample', () {
      final s = StableStrokes(config: const StableStrokesConfig(
        enabled: true,
        intensity: 0.5,
        maxWindow: 4,
      ));
      final out = smoothStrokeSample(s, Vector3(0, 0, 0));
      expect(out, isNull);
    });

    test('enabled stabilizer returns a smoothed position once warmed up', () {
      final s = StableStrokes(config: const StableStrokesConfig(
        enabled: true,
        intensity: 0.5,
        maxWindow: 4,
        extrapolation: 0.0,
      ));
      smoothStrokeSample(s, Vector3(0, 0, 0));
      final out = smoothStrokeSample(s, Vector3(2, 0, 0));
      expect(out, isNotNull);
      // Centroid of [0,0,0] and [2,0,0] → x in (0, 2).
      expect(out!.x, greaterThan(0));
      expect(out.x, lessThan(2));
      expect(out.y, 0);
      expect(out.z, 0);
    });
  });

  group('sampleColorAt — ColorSampler wiring', () {
    test('returns the colour of the nearest visible stroke', () {
      final strokes = [
        _renderStroke(_stroke([
          [0, 0, 0],
          [1, 0, 0],
        ], color: 0xFF0000FF)),
      ];
      final c = sampleColorAt(strokes, Vector3(0.5, 0, 0));
      expect(c, 0xFF0000FF);
    });

    test('returns null when no stroke is within the pick radius', () {
      final strokes = [
        _renderStroke(_stroke([
          [0, 0, 0],
          [1, 0, 0],
        ])),
      ];
      final c = sampleColorAt(strokes, Vector3(100, 100, 0), radius: 4.0);
      expect(c, isNull);
    });

    test('returns null on an empty stroke list', () {
      final c = sampleColorAt(<Stroke>[], Vector3(0, 0, 0));
      expect(c, isNull);
    });

    test('honours a custom radius', () {
      final strokes = [
        _renderStroke(_stroke([
          [0, 0, 0],
          [1, 0, 0],
        ], color: 0xFF00FF00)),
      ];
      // Point (3, 0, 0) is 2 units from the nearest sample (1, 0, 0).
      // Within radius 5 (clearly inside); outside radius 1.5 (clearly outside).
      expect(sampleColorAt(strokes, Vector3(3, 0, 0), radius: 5.0), 0xFF00FF00);
      expect(sampleColorAt(strokes, Vector3(3, 0, 0), radius: 1.5), isNull);
    });
  });

  group('mirrorStroke — MirrorAssist wiring', () {
    test('disabled assist returns no copies', () {
      final source = _stroke([
        [1, 0, 0],
        [2, 0, 0],
      ]);
      final copies = mirrorStroke(source, MirrorAssist());
      expect(copies, isEmpty);
    });

    test('single X axis produces one reflected copy with X negated', () {
      final source = _stroke([
        [1, 0, 0],
        [2, 1, 0],
      ]);
      final copies = mirrorStroke(source, MirrorAssist(activeAxes: {MirrorAxis.x}));
      expect(copies.length, 1);
      final c = copies.first;
      expect(c.length, 2);
      expect(c.worldPosition(0).x, closeTo(-1, 1e-9));
      expect(c.worldPosition(0).y, 0);
      expect(c.worldPosition(1).x, closeTo(-2, 1e-9));
      expect(c.worldPosition(1).y, closeTo(1, 1e-9));
    });

    test('two axes produce three copies (2^2 - 1 identity)', () {
      final source = _stroke([
        [1, 1, 1],
        [2, 2, 2],
      ]);
      final copies = mirrorStroke(
        source,
        MirrorAssist(activeAxes: {MirrorAxis.x, MirrorAxis.y}),
      );
      expect(copies.length, 3);
      // Each copy must carry the source's colour + thickness.
      for (final c in copies) {
        expect(c.color, source.color);
        expect(c.thickness, source.thickness);
      }
    });

    test('copies preserve pressure + time per sample', () {
      final source = _stroke([
        [0, 0, 0],
        [1, 0, 0],
      ]);
      final copies = mirrorStroke(source, MirrorAssist(activeAxes: {MirrorAxis.x}));
      expect(copies.length, 1);
      for (var i = 0; i < source.length; i++) {
        expect(copies.first.samples[i].pressure, source.samples[i].pressure);
        expect(copies.first.samples[i].time, source.samples[i].time);
      }
    });
  });

  group('snapStroke — DrawShapeAssist wiring', () {
    test('auto mode snaps a nearly-collinear stroke to a 2-point line', () {
      final source = _stroke([
        [0, 0, 0],
        [1, 0.001, 0],
        [2, 0, 0],
        [3, -0.001, 0],
        [4, 0, 0],
      ]);
      final snapped = snapStroke(source, DrawShapeAssist(), DrawShapeMode.auto);
      expect(snapped.length, 2);
      expect(snapped.worldPosition(0).x, closeTo(0, 1e-6));
      expect(snapped.worldPosition(1).x, closeTo(4, 1e-6));
    });

    test('line mode forces a 2-point line even for a curved input', () {
      final source = _stroke([
        [0, 0, 0],
        [1, 1, 0],
        [2, 0, 0],
        [3, 1, 0],
        [4, 0, 0],
      ]);
      final snapped = snapStroke(source, DrawShapeAssist(), DrawShapeMode.line);
      expect(snapped.length, 2);
      expect(snapped.worldPosition(0).x, 0);
      expect(snapped.worldPosition(1).x, 4);
    });

    test('circle mode forces a tessellated closed circle', () {
      // A noisy semi-circle around (5, 0, 5) in the XZ plane.
      final pts = <List<double>>[];
      for (var i = 0; i <= 12; i++) {
        final a = math.pi * i / 12;
        pts.add([5 + 2 * math.cos(a), 0, 5 + 2 * math.sin(a)]);
      }
      final source = _stroke(pts);
      final snapped = snapStroke(source, DrawShapeAssist(), DrawShapeMode.circle);
      // Tessellated circumference + closing point.
      expect(snapped.length, greaterThan(20));
      expect(snapped.closed, isTrue);
      // First and last points coincide (closed loop).
      final first = snapped.worldPosition(0);
      final last = snapped.worldPosition(snapped.length - 1);
      expect((first - last).length, lessThan(1e-3));
    });

    test('returns the source unchanged for a single-sample stroke', () {
      final source = Stroke3D(
        samples: [StrokeSample3D(position: Vector3(1, 2, 3))],
        color: 0xFF000000,
      );
      final snapped = snapStroke(source, DrawShapeAssist(), DrawShapeMode.auto);
      expect(identical(snapped, source), isTrue);
    });

    test('preserves the source colour + thickness on the snapped stroke', () {
      final source = _stroke([
        [0, 0, 0],
        [1, 0, 0],
      ], color: 0xFFABCDEF, thickness: 2.5);
      final snapped = snapStroke(source, DrawShapeAssist(), DrawShapeMode.line);
      expect(snapped.color, 0xFFABCDEF);
      expect(snapped.thickness, 2.5);
    });
  });
}
