// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// eraser_mirror_test.dart — v57-C GAP 0 + File 1 tests.
//
// Validates the two pieces of the mirror-erase wiring without pumping
// the full MainScreen widget (which needs engine + FFI init):
//
//   1. [EraserEngine.erasePointsInPlace] — the in-place erase API the
//      host's `_eraseAtWorld` delegates to. Verifies:
//       - strokes that lose points but stay >= minSurvivingPoints are
//         reported in `mutated` (and their `points` list is mutated).
//       - strokes that drop below `minSurvivingPoints` are reported in
//         `dropped` (and are NOT removed from the input list — the
//         caller owns removal).
//       - the `skip` callback protects strokes (3D-guide isolation
//         contract).
//       - strokes with no points in radius are unchanged.
//
//   2. [MirrorAssist.reflectPoints] applied to a single world point —
//      the exact pattern `_eraseAt` uses to derive mirrored erase
//      positions. Verifies:
//       - 1 axis → 2 copies (identity + 1 mirror).
//       - 2 axes → 4 copies (identity + 3 mirrors).
//       - the identity copy comes first (so skipping `i == 0` skips
//         the already-erased original position — the contract the
//         `_eraseAt` mirror loop relies on).
//       - a single-axis mirror flips the expected component (e.g.
//         MirrorAxis.x flips the x component).
//
//   3. An integration-style test that COMBINES the two: erase at the
//      original position AND at each mirrored position, then assert
//      strokes near BOTH the original and the mirrored erase point
//      lost samples. This is the behaviour the v57-C GAP 0 fix
//      delivers (`_eraseAt` calls `_eraseAtWorld` for the screen-
//      derived world pos AND for each mirrored world pos).

import 'package:feather_krita/engine/assistance/mirror_assist.dart';
import 'package:feather_krita/engine/brush/eraser_engine.dart';
import 'package:feather_krita/models/stroke.dart';
import 'package:flutter_test/flutter_test.dart';
import 'package:vector_math/vector_math_64.dart';

StrokePoint _p(double x, double y, double z) =>
    StrokePoint(position: Vector3(x, y, z));

Stroke _stroke(List<StrokePoint> points, {int id = 1}) =>
    Stroke(id: id, points: points);

void main() {
  group('EraserEngine.erasePointsInPlace', () {
    test('mutates surviving strokes and reports them in `mutated`', () {
      final engine = EraserEngine();
      // 5 points along X axis at y=0, z=0. Erase at (2, 0, 0) with
      // radius 0.5 — should remove only the point at x=2.
      final s = _stroke([
        _p(0, 0, 0),
        _p(1, 0, 0),
        _p(2, 0, 0),
        _p(3, 0, 0),
        _p(4, 0, 0),
      ]);
      final res = engine.erasePointsInPlace([s], Vector3(2, 0, 0), 0.5);
      expect(res.dropped, isEmpty);
      expect(res.mutated, [s]);
      expect(s.points.length, 4);
      expect(s.points.map((p) => p.position.x), [0.0, 1.0, 3.0, 4.0]);
    });

    test('reports strokes that drop below minSurvivingPoints in `dropped` '
        'and leaves them in the input list (caller removes)', () {
      final engine = EraserEngine(minSurvivingPoints: 2);
      // 2-point stroke; erase at (0, 0, 0) with radius 0.5 — both
      // points are within radius (distance 0 and 1, the latter > 0.5).
      // Actually: point at (0,0,0) is distance 0 (erased). Point at
      // (1,0,0) is distance 1 (> 0.5, kept). After erase: 1 point left
      // — below minSurvivingPoints=2, so reported in `dropped`.
      final s = _stroke([_p(0, 0, 0), _p(1, 0, 0)]);
      final res = engine.erasePointsInPlace([s], Vector3(0, 0, 0), 0.5);
      expect(res.mutated, isEmpty);
      expect(res.dropped, [s]);
      // The stroke is NOT removed from the input list — the caller
      // owns removal (so it can also clean up auxiliary records like
      // the Stroke3D / material maps).
      expect(s.points.length, 1);
    });

    test('`skip` callback protects strokes (3D-guide isolation)', () {
      final engine = EraserEngine();
      final protected = _stroke([_p(0, 0, 0), _p(1, 0, 0)]);
      final target = _stroke([_p(0, 0, 0), _p(1, 0, 0)], id: 2);
      final res = engine.erasePointsInPlace(
        [protected, target],
        Vector3(0, 0, 0),
        0.5,
        skip: (s) => s.id == 1, // protect the first stroke
      );
      // Protected stroke is unchanged.
      expect(protected.points.length, 2);
      // Target stroke lost its (0,0,0) point; 1 point left → dropped
      // (minSurvivingPoints default = 2).
      expect(res.dropped, [target]);
      expect(target.points.length, 1);
    });

    test('strokes with no points in radius are unchanged and unreported', () {
      final engine = EraserEngine();
      final s = _stroke([_p(0, 0, 0), _p(1, 0, 0)]);
      final res = engine.erasePointsInPlace([s], Vector3(100, 100, 100), 0.5);
      expect(res.mutated, isEmpty);
      expect(res.dropped, isEmpty);
      expect(s.points.length, 2);
      expect(res.anyChange, isFalse);
    });

    test('respects stroke transform (erase in world space, not local)', () {
      final engine = EraserEngine();
      // Stroke points are at local (0,0,0) and (1,0,0) but the stroke
      // is translated by (10, 0, 0). World positions are (10,0,0) and
      // (11,0,0). Erasing at world (10.5, 0, 0) with radius 0.5 should
      // remove the (10,0,0) point (distance 0.5, within radius) but
      // not the (11,0,0) point (distance 0.5, on the boundary — <=
      // radius so also erased). Use radius 0.4 to test the boundary.
      final s = Stroke(
        id: 1,
        points: [_p(0, 0, 0), _p(1, 0, 0)],
        transform: Matrix4.identity()..setTranslation(Vector3(10, 0, 0)),
      );
      final res = engine.erasePointsInPlace([s], Vector3(10, 0, 0), 0.4);
      // Only the (10,0,0) world point is within 0.4 of (10,0,0).
      // (11,0,0) is at distance 1.0 — kept. After erase: 1 point →
      // dropped (minSurvivingPoints=2).
      expect(res.dropped, [s]);
      expect(s.points.length, 1);
    });
  });

  group('MirrorAssist.reflectPoints (mirror-erase pattern)', () {
    test('single axis (X) → 2 copies, identity first, x flipped in mirror', () {
      final assist = MirrorAssist(activeAxes: const {MirrorAxis.x});
      final copies = assist.reflectPoints([Vector3(3, 7, -2)]);
      expect(copies.length, 2);
      // Identity first.
      expect(copies[0].first, Vector3(3, 7, -2));
      // Mirror: x flipped, y/z unchanged.
      expect(copies[1].first, Vector3(-3, 7, -2));
    });

    test('two axes (X, Y) → 4 copies, identity first', () {
      final assist =
          MirrorAssist(activeAxes: const {MirrorAxis.x, MirrorAxis.y});
      final copies = assist.reflectPoints([Vector3(1, 2, 3)]);
      expect(copies.length, 4);
      expect(copies[0].first, Vector3(1, 2, 3)); // identity
      expect(copies[1].first, Vector3(-1, 2, 3)); // -X
      expect(copies[2].first, Vector3(1, -2, 3)); // -Y
      expect(copies[3].first, Vector3(-1, -2, 3)); // -X -Y
    });

    test('disabled assist returns a single identity copy', () {
      final assist = MirrorAssist(); // no axes
      final copies = assist.reflectPoints([Vector3(1, 2, 3)]);
      expect(copies.length, 1);
      expect(copies[0].first, Vector3(1, 2, 3));
    });
  });

  group('mirror-erase integration (EraserEngine + MirrorAssist)', () {
    test('erasing at the original AND each mirrored position removes points '
        'from strokes near both the original and the mirror', () {
      // Simulates the _eraseAt flow: erase at world, then erase at
      // each mirrored world position. Strokes near EITHER should
      // lose samples.
      final engine = EraserEngine(minSurvivingPoints: 1);
      final assist = MirrorAssist(activeAxes: const {MirrorAxis.x});
      // Two strokes, each with points FAR from the erase target so
      // only the near point is removed:
      //   rightStroke: (5,0,0) [target] and (50,0,0) [far survivor].
      //   leftStroke:  (-5,0,0) [mirror target] and (-50,0,0) [far].
      final rightStroke = _stroke([
        _p(5, 0, 0),
        _p(50, 0, 0),
      ], id: 1);
      final leftStroke = _stroke([
        _p(-5, 0, 0),
        _p(-50, 0, 0),
      ], id: 2);
      final strokes = [rightStroke, leftStroke];

      // Erase at (5, 0, 0) — removes (5,0,0) from rightStroke.
      var res = engine.erasePointsInPlace(strokes, Vector3(5, 0, 0), 0.5);
      expect(res.mutated, [rightStroke]);
      expect(rightStroke.points.length, 1);
      expect(rightStroke.points.first.position.x, 50.0);
      // leftStroke untouched (no points within 0.5 of (5,0,0)).
      expect(leftStroke.points.length, 2);

      // Mirror-erase: reflectPoints returns identity + mirror. Skip
      // i=0 (already erased above). Erase at the mirror (-5, 0, 0).
      final copies = assist.reflectPoints([Vector3(5, 0, 0)]);
      expect(copies.length, 2);
      for (var i = 1; i < copies.length; i++) {
        final mirrored = copies[i].first;
        res = engine.erasePointsInPlace(strokes, mirrored, 0.5);
      }
      // The mirror pass hit leftStroke: (-5,0,0) is within 0.5 of
      // the mirrored erase point (-5,0,0). (-50,0,0) is far — kept.
      expect(leftStroke.points.length, 1);
      expect(leftStroke.points.first.position.x, -50.0);
    });

    test('no mirror assist → only the original position is erased', () {
      final engine = EraserEngine(minSurvivingPoints: 1);
      final assist = MirrorAssist(); // disabled
      final rightStroke = _stroke([
        _p(5, 0, 0),
        _p(50, 0, 0),
      ], id: 1);
      final leftStroke = _stroke([
        _p(-5, 0, 0),
        _p(-50, 0, 0),
      ], id: 2);
      final strokes = [rightStroke, leftStroke];

      // Erase at (5, 0, 0) — removes (5,0,0) from rightStroke.
      engine.erasePointsInPlace(strokes, Vector3(5, 0, 0), 0.5);
      // Mirror-erase loop: reflectPoints returns only the identity
      // (disabled assist), so the loop body (i in 1..length) doesn't
      // execute. leftStroke is untouched.
      final copies = assist.reflectPoints([Vector3(5, 0, 0)]);
      expect(copies.length, 1);
      // ignore: unused_local_variable — verifies the loop body never runs.
      var loopRuns = 0;
      for (var i = 1; i < copies.length; i++) {
        loopRuns++;
        engine.erasePointsInPlace(strokes, copies[i].first, 0.5);
      }
      expect(loopRuns, 0);
      expect(rightStroke.points.length, 1);
      expect(leftStroke.points.length, 2); // unchanged
    });
  });
}
