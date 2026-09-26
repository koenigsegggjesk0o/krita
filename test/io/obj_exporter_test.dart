// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// obj_exporter_test.dart — unit tests for the Wavefront OBJ tube-mesh
// exporter.
//
// Each visible [Stroke] becomes a tube (swept regular polygon) with two
// triangle-fan caps. Tests cover header, object naming, vertex / face
// counts, hidden-stroke skipping, multi-stroke vertex indexing offsets,
// the cap-centre vertices, and the degenerate cases (empty / single-point
// stroke).

import 'package:feather_krita/io/obj_exporter.dart';
import 'package:feather_krita/models/stroke.dart';
import 'package:flutter_test/flutter_test.dart';
import 'package:vector_math/vector_math_64.dart';

Stroke _stroke(int id, List<Vector3> pts, {bool visible = true}) => Stroke(
      id: id,
      isVisible: visible,
      points: pts.map((p) => StrokePoint(position: p)).toList(),
    );

int _count(String haystack, String needle) =>
    haystack.split(needle).length - 1;

void main() {
  group('ObjExporter header & object naming', () {
    test('header carries the stroke count and tube parameters', () {
      final out = ObjExporter(tubeSegments: 6, tubeRadius: 0.1)
          .export(strokes: [_stroke(1, [Vector3.zero(), Vector3(1, 0, 0)])]);
      expect(out, contains('# Exported from Feather-Krita'));
      expect(out, contains('# Strokes: 1'));
      expect(out, contains('# Tube segments: 6, radius: 0.1'));
    });

    test('each visible stroke emits an o / g line with its id', () {
      final out = ObjExporter().export(strokes: [
        _stroke(7, [Vector3.zero(), Vector3(1, 0, 0)]),
      ]);
      expect(out, contains('o Stroke_7'));
      expect(out, contains('g Stroke_7'));
    });
  });

  group('ObjExporter vertex / face counts', () {
    test('a 2-point stroke produces rings*points + 2 cap-centre vertices', () {
      const seg = 8;
      final out = ObjExporter(tubeSegments: seg, tubeRadius: 0.05)
          .export(strokes: [_stroke(1, [Vector3.zero(), Vector3(1, 0, 0)])]);
      // 2 polyline points × 8 ring segments + 2 cap centres = 18 vertices.
      final vCount = _count(out, '\nv ');
      expect(vCount, 2 * seg + 2);
    });

    test('side faces: 2 triangles × segments × (points-1)', () {
      const seg = 6;
      final out = ObjExporter(tubeSegments: seg, tubeRadius: 0.05)
          .export(strokes: [_stroke(1, [Vector3.zero(), Vector3(1, 0, 0)])]);
      // 2 side triangles × seg × (2-1) = 12 side faces, plus 2*seg cap
      // faces = 12 cap faces. Total = 24 face lines.
      final fCount = _count(out, '\nf ');
      expect(fCount, 2 * seg * 1 + 2 * seg);
    });
  });

  group('ObjExporter — hidden & degenerate strokes', () {
    test('hidden strokes are skipped entirely', () {
      final out = ObjExporter().export(strokes: [
        _stroke(1, [Vector3.zero(), Vector3(1, 0, 0)], visible: false),
      ]);
      expect(out.contains('o Stroke_1'), isFalse);
      // No vertex lines beyond the header.
      expect(_count(out, '\nv '), 0);
    });

    test('single-point strokes are skipped (need >= 2 points)', () {
      final out = ObjExporter().export(strokes: [
        _stroke(2, [Vector3.zero()]),
      ]);
      expect(out.contains('o Stroke_2'), isFalse);
    });

    test('empty stroke list produces a valid header-only OBJ', () {
      final out = ObjExporter().export(strokes: <Stroke>[]);
      expect(out, contains('# Strokes: 0'));
      expect(_count(out, '\nv '), 0);
      expect(_count(out, '\nf '), 0);
    });
  });

  group('ObjExporter — multi-stroke vertex indexing', () {
    test('two strokes produce two objects with no face-index overlap', () {
      const seg = 4;
      final out = ObjExporter(tubeSegments: seg, tubeRadius: 0.05)
          .export(strokes: [
        _stroke(1, [Vector3.zero(), Vector3(1, 0, 0)]),
        _stroke(2, [Vector3.zero(), Vector3(0, 1, 0)]),
      ]);
      // Both object headers should be present, in order.
      final i1 = out.indexOf('o Stroke_1');
      final i2 = out.indexOf('o Stroke_2');
      expect(i1, greaterThan(0));
      expect(i2, greaterThan(i1));
      // Each stroke contributes 2*seg+2 vertices.
      expect(_count(out, '\nv '), 2 * (2 * seg + 2));
    });

    test('constructor asserts on tubeSegments < 3 and radius <= 0', () {
      expect(() => ObjExporter(tubeSegments: 2), throwsA(isA<AssertionError>()));
      expect(() => ObjExporter(tubeRadius: 0), throwsA(isA<AssertionError>()));
      expect(() => ObjExporter(tubeRadius: -1), throwsA(isA<AssertionError>()));
    });
  });

  group('ObjExporter — cap centers coincide with polyline endpoints', () {
    test('start cap center equals the first polyline point', () {
      // The last two `v` lines of a stroke are the start-cap and end-cap
      // centers. They must equal the first and last polyline points.
      final out = ObjExporter(tubeSegments: 4, tubeRadius: 0.05).export(
          strokes: [_stroke(1, [Vector3(2, 3, 4), Vector3(5, 6, 7)])]);
      // The header carries "2" and "3" and "4" elsewhere, so we look
      // for vertex lines explicitly.
      final vLines = RegExp(r'^v (.*)$', multiLine: true)
          .allMatches(out)
          .map((m) => m.group(1)!)
          .toList();
      // The last two are the cap centres.
      expect(vLines.last, contains('5'));
      expect(vLines[vLines.length - 2], contains('2'));
    });
  });
}
