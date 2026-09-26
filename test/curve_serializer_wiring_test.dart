// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// curve_serializer_wiring_test.dart — v0.57-B wiring tests for
// curve_serializer.dart.
//
// lib/engine/curves/curve_serializer.dart (CurveSerializer) was dead code
// before v0.57-B: the serializer could round-trip a [Curve3D] to / from
// JSON + binary, but no runtime caller invoked it. v0.57-B wires it into
// the project save/load path ([MainScreen._shortcutSave] /
// [_shortcutOpen]): on save, each stroke's [Stroke3D] is converted to a
// [BezierCurve3D] via [Stroke3D.toBezier] and serialized via
// [CurveSerializer.toJson] into a `curves` block; on load, the block is
// deserialized via [CurveSerializer.fromJson] into a runtime
// `Map<int, Curve3D>` ([MainScreen._loadedCurves]) so the curves form
// survives a save/load round-trip (previously only the rendered Stroke
// polyline was persisted).
//
// The wiring logic lives in two pure @visibleForTesting static methods on
// [MainScreen] — [serializeStrokesAsCurves] and [deserializeCurvesFromJson]
// — so it is unit-testable without pumping the full [MainScreen] widget
// (which needs engine + FFI init). Mirrors the v56-B effectiveMaxUndoFor
// pattern.

import 'dart:convert';

import 'package:feather_krita/core/math/math.dart';
import 'package:feather_krita/engine/curves/bezier_curve3d.dart';
import 'package:feather_krita/engine/curves/curve3d.dart';
import 'package:feather_krita/engine/curves/curve_serializer.dart';
import 'package:feather_krita/engine/curves/stroke3d.dart';
import 'package:feather_krita/screens/main_screen.dart';
import 'package:flutter_test/flutter_test.dart';

void main() {
  group('MainScreen.kCurvesBlockKey (v0.57-B wiring contract)', () {
    test('is exposed as a @visibleForTesting constant', () {
      // Pin the key so a future refactor can't silently drift the
      // block name. The host's _shortcutSave writes the block under
      // this key; _shortcutOpen reads it back. Changing it here is
      // the single source of truth for the on-disk format.
      expect(MainScreen.kCurvesBlockKey, 'curves');
    });
  });

  group('MainScreen.serializeStrokesAsCurves (save path)', () {
    test('converts each Stroke3D to a CurveSerializer.toJson entry', () {
      // Build a Stroke3D with 4 samples — enough for toBezier to fit
      // a non-degenerate Bezier (the fitter pads <4 samples with
      // extrapolated points, but 4 gives a clean fit).
      final stroke3d = Stroke3D(samples: [
        StrokeSample3D(position: Vector3(0, 0, 0), time: 0),
        StrokeSample3D(position: Vector3(1, 1, 0), time: 0.1),
        StrokeSample3D(position: Vector3(2, 0, 0), time: 0.2),
        StrokeSample3D(position: Vector3(3, 1, 0), time: 0.3),
      ]);
      final stroke3Ds = <int, Stroke3D>{42: stroke3d};

      final json = MainScreen.serializeStrokesAsCurves(stroke3Ds);

      // One entry per stroke, keyed by the stroke id as a string.
      expect(json.length, 1);
      expect(json.containsKey('42'), isTrue);
      // The entry is a CurveSerializer.toJson map — it carries the
      // version, kind discriminator, colour, thickness, transform,
      // and a kind-specific data block.
      final entry = json['42'] as Map<String, dynamic>;
      expect(entry['v'], kCurveJsonVersion);
      expect(entry['kind'], CurveKind.bezier.name);
      expect(entry.containsKey('data'), isTrue);
      expect((entry['data'] as Map)['points'], isA<List>());
    });

    test('produces an empty map for an empty stroke3Ds input', () {
      expect(MainScreen.serializeStrokesAsCurves(<int, Stroke3D>{}), isEmpty);
    });
  });

  group('MainScreen.deserializeCurvesFromJson (load path)', () {
    test('round-trips a serialized Stroke3D back into a Curve3D', () {
      // The full save→load round-trip: Stroke3D → toBezier →
      // CurveSerializer.toJson → (embed in project JSON) →
      // CurveSerializer.fromJson → Curve3D. The restored Curve3D should
      // be a BezierCurve3D with the same kind + a non-empty control-
      // point list.
      final stroke3d = Stroke3D(samples: [
        StrokeSample3D(position: Vector3(0, 0, 0), time: 0),
        StrokeSample3D(position: Vector3(1, 2, 0), time: 0.1),
        StrokeSample3D(position: Vector3(2, 0, 0), time: 0.2),
        StrokeSample3D(position: Vector3(3, 2, 0), time: 0.3),
        StrokeSample3D(position: Vector3(4, 0, 0), time: 0.4),
      ]);
      final stroke3Ds = <int, Stroke3D>{7: stroke3d};

      // Save.
      final saved = MainScreen.serializeStrokesAsCurves(stroke3Ds);
      // Simulate the project JSON round-trip through the file system
      // (jsonEncode → jsonDecode) so the test exercises the real
      // serialization format, not just the in-memory Map.
      final encoded = jsonEncode({MainScreen.kCurvesBlockKey: saved});
      final decoded =
          (jsonDecode(encoded) as Map).cast<String, dynamic>();
      final curvesBlock =
          decoded[MainScreen.kCurvesBlockKey] as Map<String, dynamic>;

      // Load.
      final loaded = MainScreen.deserializeCurvesFromJson(curvesBlock);

      expect(loaded.length, 1);
      expect(loaded.containsKey(7), isTrue);
      final curve = loaded[7]!;
      expect(curve.kind, CurveKind.bezier);
      // The Bezier should have at least 2 control points (toBezier
      // fits a cubic Bezier per segment; 5 samples → ≥1 segment →
      // ≥4 control points).
      expect(curve, isA<BezierCurve3D>());
      final bez = curve as BezierCurve3D;
      expect(bez.controlPoints.length, greaterThanOrEqualTo(2));
    });

    test('skips malformed entries permissively', () {
      // The host's load path must not abort the whole project open
      // when one curve entry is malformed. [deserializeCurvesFromJson]
      // skips entries that: (a) have a non-integer key, (b) have a
      // non-Map value, or (c) throw during CurveSerializer.fromJson.
      final json = <String, dynamic>{
        '1': CurveSerializer.toJson(Stroke3D(samples: [
              StrokeSample3D(position: Vector3(0, 0, 0)),
              StrokeSample3D(position: Vector3(1, 1, 0)),
              StrokeSample3D(position: Vector3(2, 0, 0)),
              StrokeSample3D(position: Vector3(3, 1, 0)),
            ]).toBezier()),
        'not-an-int': CurveSerializer.toJson(Stroke3D(samples: [
              StrokeSample3D(position: Vector3(0, 0, 0)),
              StrokeSample3D(position: Vector3(1, 1, 0)),
              StrokeSample3D(position: Vector3(2, 0, 0)),
              StrokeSample3D(position: Vector3(3, 1, 0)),
            ]).toBezier()),
        '2': 'not-a-map',
        '3': <String, dynamic>{'bad': 'shape'},
      };

      final loaded = MainScreen.deserializeCurvesFromJson(json);

      // Only entry '1' is valid → loaded should have exactly one entry.
      expect(loaded.length, 1);
      expect(loaded.containsKey(1), isTrue);
      expect(loaded[1]!.kind, CurveKind.bezier);
    });

    test('returns an empty map for an empty input', () {
      expect(
          MainScreen.deserializeCurvesFromJson(<String, dynamic>{}), isEmpty);
    });
  });

  group('MainScreen.serializeStrokesAsCurves → deserializeCurvesFromJson '
      '(full round-trip)', () {
    test('multiple strokes survive a save/load round-trip', () {
      // Three strokes with different sample counts → different fitted
      // Bezier forms. All three should round-trip.
      final stroke3Ds = <int, Stroke3D>{
        10: Stroke3D(samples: [
          StrokeSample3D(position: Vector3(0, 0, 0), time: 0),
          StrokeSample3D(position: Vector3(1, 1, 0), time: 0.1),
          StrokeSample3D(position: Vector3(2, 0, 0), time: 0.2),
          StrokeSample3D(position: Vector3(3, 1, 0), time: 0.3),
        ]),
        20: Stroke3D(samples: [
          StrokeSample3D(position: Vector3(0, 5, 0), time: 0),
          StrokeSample3D(position: Vector3(1, 6, 0), time: 0.1),
          StrokeSample3D(position: Vector3(2, 5, 0), time: 0.2),
          StrokeSample3D(position: Vector3(3, 6, 0), time: 0.3),
          StrokeSample3D(position: Vector3(4, 5, 0), time: 0.4),
        ]),
        30: Stroke3D(samples: [
          StrokeSample3D(position: Vector3(0, 0, 5), time: 0),
          StrokeSample3D(position: Vector3(0, 0, 6), time: 0.1),
          StrokeSample3D(position: Vector3(0, 0, 7), time: 0.2),
          StrokeSample3D(position: Vector3(0, 0, 8), time: 0.3),
        ]),
      };

      final saved = MainScreen.serializeStrokesAsCurves(stroke3Ds);
      final loaded = MainScreen.deserializeCurvesFromJson(saved);

      expect(loaded.keys, {10, 20, 30});
      for (final id in [10, 20, 30]) {
        expect(loaded[id], isNotNull);
        expect(loaded[id]!.kind, CurveKind.bezier,
            reason: 'stroke $id should round-trip as a BezierCurve3D');
      }
    });
  });
}
