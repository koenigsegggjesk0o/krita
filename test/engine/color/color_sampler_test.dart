// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// color_sampler_test.dart — unit tests for the eyedropper.
//
// Covers nearest-stroke colour sampling (with pick radius), single-stroke
// sampling, hidden-stroke rejection, bilinear reference-image sampling
// at corners and interior, nearest-neighbour samplePixel, and the
// degenerate-buffer / out-of-range cases.

import 'dart:typed_data';

import 'package:feather_krita/engine/color/color_sampler.dart';
import 'package:feather_krita/models/stroke.dart';
import 'package:flutter_test/flutter_test.dart';
import 'package:vector_math/vector_math_64.dart';

Stroke _stroke(int id, List<Vector3> pts, {int color = 0xFF000000, bool visible = true}) =>
    Stroke(
      id: id,
      color: color,
      isVisible: visible,
      points: pts.map((p) => StrokePoint(position: p)).toList(),
    );

int _alpha(int packed) => (packed >> 24) & 0xff;
int _red(int packed) => (packed >> 16) & 0xff;
int _green(int packed) => (packed >> 8) & 0xff;
int _blue(int packed) => packed & 0xff;

void main() {
  group('ColorSampler.sampleFromCurves', () {
    test('returns the colour of the nearest visible stroke within radius', () {
      final s1 = _stroke(1, [Vector3(0, 0, 0), Vector3(1, 0, 0)],
          color: 0xFFAA0000);
      final s2 = _stroke(2, [Vector3(5, 0, 0), Vector3(6, 0, 0)],
          color: 0xFF00BB00);
      final sampler = ColorSampler(defaultPickRadius: 0.5);
      final color = sampler.sampleFromCurves([s1, s2], Vector3(0.4, 0, 0));
      expect(color, 0xFFAA0000);
    });

    test('returns null when no stroke is within the pick radius', () {
      final s = _stroke(1, [Vector3(0, 0, 0), Vector3(1, 0, 0)],
          color: 0xFFAA0000);
      final sampler = ColorSampler(defaultPickRadius: 0.1);
      expect(sampler.sampleFromCurves([s], Vector3(5, 5, 5)), isNull);
    });

    test('skips hidden strokes', () {
      final hidden = _stroke(1, [Vector3(0, 0, 0)], color: 0xFFAA0000, visible: false);
      final sampler = ColorSampler(defaultPickRadius: 1.0);
      expect(sampler.sampleFromCurves([hidden], Vector3.zero()), isNull);
    });

    test('empty stroke list returns null', () {
      final sampler = ColorSampler();
      expect(sampler.sampleFromCurves(<Stroke>[], Vector3.zero()), isNull);
    });
  });

  group('ColorSampler.sampleFromStroke', () {
    test('returns the colour when any sample is within radius', () {
      final s = _stroke(1, [Vector3(0, 0, 0), Vector3(1, 0, 0)], color: 0xFF112233);
      final sampler = ColorSampler();
      expect(sampler.sampleFromStroke(s, Vector3(0.6, 0, 0)), 0xFF112233);
    });

    test('returns null for a hidden stroke', () {
      final s = _stroke(1, [Vector3(0, 0, 0)], visible: false);
      expect(ColorSampler().sampleFromStroke(s, Vector3.zero()), isNull);
    });
  });

  group('ColorSampler.sampleFromImage', () {
    test('a 1x1 RGBA image returns that pixel for every UV', () {
      // 1x1 image: R=10, G=20, B=30, A=255.
      final rgba = Uint8List.fromList([10, 20, 30, 255]);
      final sampler = ColorSampler();
      final c = sampler.sampleFromImage(rgba, 1, 1, 0.5, 0.5);
      expect(_red(c), 10);
      expect(_green(c), 20);
      expect(_blue(c), 30);
      expect(_alpha(c), 255);
    });

    test('a 2x2 image returns distinct corner colours', () {
      // 2x2 RGBA: (0,0)=red, (1,0)=green, (0,1)=blue, (1,1)=white.
      final rgba = Uint8List.fromList([
        255, 0, 0, 255,   0, 255, 0, 255,
        0, 0, 255, 255,   255, 255, 255, 255,
      ]);
      final sampler = ColorSampler();
      // Top-left corner (u=0, v=0).
      final tl = sampler.sampleFromImage(rgba, 2, 2, 0.0, 0.0);
      expect(_red(tl), 255);
      expect(_green(tl), 0);
      expect(_blue(tl), 0);
      // Bottom-right corner (u=1, v=1).
      final br = sampler.sampleFromImage(rgba, 2, 2, 1.0, 1.0);
      expect(_red(br), 255);
      expect(_green(br), 255);
      expect(_blue(br), 255);
    });

    test('empty / zero-dimension buffer returns opaque black', () {
      final sampler = ColorSampler();
      expect(sampler.sampleFromImage(Uint8List(0), 0, 0, 0.5, 0.5),
          0xFF000000);
    });

    test('rgbOnly promotes alpha to opaque', () {
      final rgb = Uint8List.fromList([10, 20, 30]);
      final sampler = ColorSampler();
      final c = sampler.sampleFromImage(rgb, 1, 1, 0, 0, rgbOnly: true);
      expect(_alpha(c), 255);
      expect(_red(c), 10);
    });
  });

  group('ColorSampler.samplePixel', () {
    test('nearest-neighbour returns the exact pixel', () {
      final rgba = Uint8List.fromList([
        255, 0, 0, 255,   0, 255, 0, 255,
        0, 0, 255, 255,   255, 255, 255, 255,
      ]);
      final sampler = ColorSampler();
      // Pixel (1, 0) = green.
      final c = sampler.samplePixel(rgba, 2, 1, 0);
      expect(_green(c), 255);
      expect(_red(c), 0);
      // Pixel (0, 1) = blue.
      final d = sampler.samplePixel(rgba, 2, 0, 1);
      expect(_blue(d), 255);
    });

    test('out-of-range index returns opaque black', () {
      final sampler = ColorSampler();
      expect(sampler.samplePixel(Uint8List(0), 1, 0, 0), 0xFF000000);
    });
  });
}
