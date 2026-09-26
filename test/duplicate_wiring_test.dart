// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// duplicate_wiring_test.dart — v0.57-C tests for the duplicate wiring.
//
// Validates the DuplicateEngine surface that the host
// (lib/screens/main_screen.dart `_shortcutDuplicate`) wires to Ctrl+D:
//   * [DuplicateEngine.duplicate] — plain in-place copy (the wired mode).
//   * [DuplicateEngine.duplicateByView] — symmetric-by-view mirror.
//   * [DuplicateEngine.duplicateByMirror] — symmetric-by-mirror.
//
// The host's `_shortcutDuplicate` method itself requires pumping MainScreen
// (FFI native library + scene graph) and is NOT unit-testable in isolation
// — same constraint as the brush_renderer_test.dart note (line 14-16).
// Instead, these tests pin the DuplicateEngine contract the host depends on
// (return shape, ID bumping, geometry preservation, mirror math) so the
// wiring is regression-safe even without an end-to-end pump.

import 'package:feather_krita/engine/selection/duplicate.dart';
import 'package:feather_krita/models/stroke.dart';
import 'package:flutter_test/flutter_test.dart';
import 'package:vector_math/vector_math_64.dart';

Stroke _stroke({
  required int id,
  List<StrokePoint> points = const [],
  int color = 0xFF60A5FA,
  double thickness = 8.0,
  Matrix4? transform,
}) {
  return Stroke(
    id: id,
    color: color,
    thickness: thickness,
    points: points,
    transform: transform ?? Matrix4.identity(),
  );
}

StrokePoint _p(double x, double y, double z) =>
    StrokePoint(position: Vector3(x, y, z));

void main() {
  group('DuplicateEngine.duplicate (host-wired Ctrl+D mode)', () {
    test('returns one copy per input stroke with unique bumped IDs', () {
      final engine = DuplicateEngine(nextId: 100);
      final inputs = [
        _stroke(id: 1, points: [_p(0, 0, 0), _p(1, 0, 0)]),
        _stroke(id: 2, points: [_p(0, 1, 0), _p(1, 1, 0)]),
        _stroke(id: 3, points: [_p(0, 2, 0), _p(1, 2, 0)]),
      ];
      final result = engine.duplicate(inputs);
      expect(result.strokes.length, 3);
      expect(result.strokes.map((s) => s.id).toList(), [100, 101, 102]);
      // nextId bumped past the assigned IDs.
      expect(engine.nextId, 103);
    });

    test('preserves stroke geometry (points + color + thickness)', () {
      final engine = DuplicateEngine(nextId: 50);
      final original = _stroke(
        id: 7,
        points: [_p(1, 2, 3), _p(4, 5, 6)],
        color: 0xFFABCDEF,
        thickness: 12.5,
      );
      final result = engine.duplicate([original]);
      expect(result.strokes.length, 1);
      final copy = result.strokes.single;
      // ID is new.
      expect(copy.id, 50);
      // Geometry + style preserved.
      expect(copy.color, 0xFFABCDEF);
      expect(copy.thickness, 12.5);
      expect(copy.length, 2);
      expect(copy.points[0].position, Vector3(1, 2, 3));
      expect(copy.points[1].position, Vector3(4, 5, 6));
    });

    test('does NOT mutate the original stroke', () {
      final engine = DuplicateEngine(nextId: 200);
      final original = _stroke(
        id: 5,
        points: [_p(0, 0, 0), _p(1, 0, 0)],
      );
      engine.duplicate([original]);
      // Original keeps its ID + points.
      expect(original.id, 5);
      expect(original.length, 2);
      expect(original.points[0].position, Vector3(0, 0, 0));
    });

    test('preserves the stroke transform (duplicates in same position)', () {
      // Per docs/selection_duplicate.txt: "duplicated curves are in the
      // same position as the original." This is the host's contract for
      // Ctrl+D — the duplicate must overlap the original until the user
      // moves it.
      final engine = DuplicateEngine(nextId: 10);
      final original = _stroke(
        id: 1,
        points: [_p(0, 0, 0), _p(1, 0, 0)],
        transform: Matrix4.identity()..setTranslation(Vector3(5, 5, 5)),
      );
      final result = engine.duplicate([original]);
      final copy = result.strokes.single;
      // Copy's world position matches the original's (same transform).
      expect(copy.worldPosition(0), original.worldPosition(0));
      expect(copy.worldPosition(1), original.worldPosition(1));
    });

    test('handles empty input (returns 0 copies, message still formatted)', () {
      final engine = DuplicateEngine(nextId: 1);
      final result = engine.duplicate([]);
      expect(result.strokes, isEmpty);
      expect(result.count, 0);
      // Message format: "0 curves duplicated" — pinned for the future
      // toast UI (per docs/selection_duplicate.txt: "A message will
      // appear with the number of duplicated curves").
      expect(result.message, '0 curves duplicated');
    });

    test('message singular/plural ("curve" vs "curves")', () {
      final engine = DuplicateEngine(nextId: 1);
      expect(engine.duplicate([_stroke(id: 0)]).message, '1 curve duplicated');
      expect(
        engine.duplicate([_stroke(id: 0), _stroke(id: 0)]).message,
        '2 curves duplicated',
      );
    });

    test('nextId sync contract — host adopts the bumped counter', () {
      // The host (_shortcutDuplicate) sets _duplicateEngine.nextId =
      // _nextStrokeId before the call, then adopts _nextStrokeId =
      // _duplicateEngine.nextId after. Verify the bump is exactly
      // +N (one per duplicated stroke).
      final engine = DuplicateEngine(nextId: 1000);
      engine.duplicate([_stroke(id: 1), _stroke(id: 2), _stroke(id: 3)]);
      expect(engine.nextId, 1003);
      engine.duplicate([_stroke(id: 4)]);
      expect(engine.nextId, 1004);
    });
  });

  group('DuplicateEngine.duplicateByView (symmetric-by-view)', () {
    test('mirrors each stroke across the view-right plane (origin = 0)', () {
      // viewRight = (1, 0, 0) → mirror plane is the YZ plane (normal = X).
      // A point at (3, 0, 0) reflects to (-3, 0, 0).
      final engine = DuplicateEngine(nextId: 100);
      final s = _stroke(
        id: 1,
        points: [_p(3, 0, 0), _p(3, 4, 5)],
      );
      final result =
          engine.duplicateByView([s], Vector3(1, 0, 0));
      expect(result.strokes.length, 1);
      final mirror = result.strokes.single;
      expect(mirror.id, 100);
      // World positions: (3,0,0) → (-3,0,0); (3,4,5) → (-3,4,5).
      expect(mirror.worldPosition(0), Vector3(-3, 0, 0));
      expect(mirror.worldPosition(1), Vector3(-3, 4, 5));
    });

    test('mirror plane through a non-zero origin', () {
      // Origin = (1, 0, 0): a point at (3, 0, 0) reflects to (-1, 0, 0)
      // (the plane x=1 is the mirror; 3 is 2 units right of 1, so the
      // reflection is 2 units left of 1 = -1).
      final engine = DuplicateEngine(nextId: 1);
      final s = _stroke(id: 0, points: [_p(3, 7, 9)]);
      final result = engine.duplicateByView(
        [s],
        Vector3(1, 0, 0),
        origin: Vector3(1, 0, 0),
      );
      final mirror = result.strokes.single;
      expect(mirror.worldPosition(0), Vector3(-1, 7, 9));
    });

    test('handles non-axis-aligned viewRight (45° in XY plane)', () {
      // viewRight = (1, 1, 0) normalized → mirror plane normal is (1,1,0).
      // A point at (1, 0, 0) reflects across the plane normal to (1,1,0)/√2
      // to... let's verify: the Householder reflection of (1,0,0) across
      // the plane whose normal is n=(1,1,0)/√2 is:
      //   p - 2*(p·n)*n = (1,0,0) - 2*(1/√2)*(1/√2, 1/√2, 0)
      //                 = (1,0,0) - (1, 1, 0) = (0, -1, 0).
      final engine = DuplicateEngine(nextId: 1);
      final s = _stroke(id: 0, points: [_p(1, 0, 0)]);
      final result =
          engine.duplicateByView([s], Vector3(1, 1, 0));
      final mirror = result.strokes.single;
      expect(mirror.worldPosition(0).x, closeTo(0, 1e-6));
      expect(mirror.worldPosition(0).y, closeTo(-1, 1e-6));
      expect(mirror.worldPosition(0).z, closeTo(0, 1e-6));
    });

    test('message format includes "by view"', () {
      final engine = DuplicateEngine(nextId: 1);
      final result = engine.duplicateByView(
        [_stroke(id: 0, points: [_p(1, 0, 0)])],
        Vector3(1, 0, 0),
      );
      expect(result.message, contains('by view'));
      expect(result.message, contains('1 curve'));
    });
  });

  group('DuplicateEngine.duplicateByMirror (symmetric-by-mirror)', () {
    test('one axis (x) → one mirror copy per stroke', () {
      final engine = DuplicateEngine(nextId: 10);
      final s = _stroke(id: 1, points: [_p(2, 3, 4)]);
      final result = engine.duplicateByMirror(
        [s],
        const MirrorAxes(x: true),
      );
      expect(result.strokes.length, 1);
      final mirror = result.strokes.single;
      expect(mirror.id, 10);
      // Sign vector (-1, 1, 1): point (2, 3, 4) → (-2, 3, 4).
      expect(mirror.worldPosition(0), Vector3(-2, 3, 4));
    });

    test('two axes (x + y) → three mirror copies per stroke', () {
      // signVectors for {x, y}: (-1,1,1), (1,-1,1), (-1,-1,1) — 3 signs.
      final engine = DuplicateEngine(nextId: 100);
      final s = _stroke(id: 1, points: [_p(2, 3, 4)]);
      final result = engine.duplicateByMirror(
        [s],
        const MirrorAxes(x: true, y: true),
      );
      expect(result.strokes.length, 3);
      final worldPos = result.strokes.map((s) => s.worldPosition(0)).toSet();
      expect(worldPos, contains(Vector3(-2, 3, 4))); // (-1, 1, 1)
      expect(worldPos, contains(Vector3(2, -3, 4))); // (1, -1, 1)
      expect(worldPos, contains(Vector3(-2, -3, 4))); // (-1, -1, 1)
    });

    test('three axes (x + y + z) → seven mirror copies per stroke', () {
      // signVectors for {x, y, z}: all non-identity sign combos = 7.
      final engine = DuplicateEngine(nextId: 1);
      final s = _stroke(id: 0, points: [_p(1, 2, 3)]);
      final result = engine.duplicateByMirror(
        [s],
        const MirrorAxes(x: true, y: true, z: true),
      );
      expect(result.strokes.length, 7);
      // IDs are unique.
      final ids = result.strokes.map((s) => s.id).toSet();
      expect(ids.length, 7);
    });

    test('no axes enabled → empty result + honest message', () {
      final engine = DuplicateEngine(nextId: 1);
      final result = engine.duplicateByMirror(
        [_stroke(id: 0, points: [_p(1, 0, 0)])],
        const MirrorAxes(), // all axes off
      );
      expect(result.strokes, isEmpty);
      expect(result.message, contains('Mirror is off'));
    });

    test('mirror origin shifts the reflection pivot', () {
      // MirrorAxes origin = (1, 0, 0): a point at (3, 0, 0) with x-axis
      // mirror reflects to (2*1 - 3, 0, 0) = (-1, 0, 0).
      final engine = DuplicateEngine(nextId: 1);
      final s = _stroke(id: 0, points: [_p(3, 0, 0)]);
      final result = engine.duplicateByMirror(
        [s],
        const MirrorAxes(x: true), // default origin = (0,0,0)
      );
      // With default origin (0,0,0): point (3,0,0) → (-3, 0, 0).
      expect(result.strokes.single.worldPosition(0), Vector3(-3, 0, 0));

      // Now with origin (1, 0, 0).
      final engine2 = DuplicateEngine(nextId: 1);
      final s2 = _stroke(id: 0, points: [_p(3, 0, 0)]);
      final result2 = engine2.duplicateByMirror(
        [s2],
        MirrorAxes(x: true, origin: Vector3(1, 0, 0)),
      );
      expect(result2.strokes.single.worldPosition(0), Vector3(-1, 0, 0));
    });
  });

  group('MirrorAxes.signVectors', () {
    test('no axes → empty list (no mirrors)', () {
      expect(const MirrorAxes().signVectors(), isEmpty);
    });

    test('one axis → 1 sign vector', () {
      final sv = const MirrorAxes(x: true).signVectors();
      expect(sv.length, 1);
      expect(sv.single, Vector3(-1, 1, 1));
    });

    test('two axes → 3 sign vectors (excluding identity)', () {
      final sv = const MirrorAxes(x: true, y: true).signVectors();
      expect(sv.length, 3);
      // Identity (1,1,1) must NOT be present.
      expect(sv.any((v) => v.x == 1 && v.y == 1 && v.z == 1), isFalse);
    });

    test('three axes → 7 sign vectors (all non-identity)', () {
      final sv = const MirrorAxes(x: true, y: true, z: true).signVectors();
      expect(sv.length, 7);
      expect(sv.any((v) => v.x == 1 && v.y == 1 && v.z == 1), isFalse);
    });

    test('anyEnabled getter', () {
      expect(const MirrorAxes().anyEnabled, isFalse);
      expect(const MirrorAxes(x: true).anyEnabled, isTrue);
      expect(const MirrorAxes(z: true).anyEnabled, isTrue);
    });

    test('originPoint defaults to zero when origin is null', () {
      expect(const MirrorAxes().originPoint, Vector3.zero());
      expect(
        MirrorAxes(origin: Vector3(1, 2, 3)).originPoint,
        Vector3(1, 2, 3),
      );
    });
  });

  group('DuplicateResult', () {
    test('count getter matches strokes.length', () {
      final engine = DuplicateEngine(nextId: 1);
      final r = engine.duplicate([
        _stroke(id: 0, points: [_p(0, 0, 0)]),
        _stroke(id: 0, points: [_p(1, 0, 0)]),
      ]);
      expect(r.count, r.strokes.length);
      expect(r.count, 2);
    });

    test('strokes list is mutable-tolerant (caller can mutate freely)', () {
      // The returned list is a fresh growable list — caller can sort /
      // mutate without affecting the engine's internal state. The
      // engine's nextId was bumped during the duplicate() call (that's
      // expected + permanent — duplicates keep their IDs); clearing the
      // returned list does NOT roll back the ID bump.
      final engine = DuplicateEngine(nextId: 1);
      final r = engine.duplicate([_stroke(id: 0, points: [_p(0, 0, 0)])]);
      final idAfterDuplicate = engine.nextId; // 2 — bumped by 1 stroke.
      r.strokes.clear();
      expect(r.strokes, isEmpty);
      // Engine's nextId is unaffected by the post-call list mutation.
      expect(engine.nextId, idAfterDuplicate);
    });
  });

  group('Host wiring contract (main_screen._shortcutDuplicate)', () {
    // These tests pin the contract that the host's _shortcutDuplicate
    // method depends on. The host:
    //   1. Reads _selectedStrokes().
    //   2. Syncs _duplicateEngine.nextId = _nextStrokeId.
    //   3. Calls _duplicateEngine.duplicate(selected).
    //   4. Adopts _nextStrokeId = _duplicateEngine.nextId.
    //   5. Adds result.strokes to _strokes.
    //   6. Sets selection to result.strokes' IDs.

    test('nextId is bumped by exactly the number of duplicated strokes', () {
      // Contract: the host's _nextStrokeId sync (step 4) is correct iff
      // DuplicateEngine.bump == inputs.length.
      for (final n in [1, 2, 5, 10]) {
        final engine = DuplicateEngine(nextId: 500);
        final inputs = List.generate(n, (i) => _stroke(id: i + 1));
        engine.duplicate(inputs);
        expect(engine.nextId, 500 + n, reason: 'n=$n');
      }
    });

    test('every duplicated stroke ID is unique within one call', () {
      // Contract: the host's selection update (step 6) maps IDs to
      // strokes — IDs must be unique so the selection maps back to the
      // right strokes.
      final engine = DuplicateEngine(nextId: 1000);
      final inputs = List.generate(10, (i) => _stroke(id: i + 1));
      final r = engine.duplicate(inputs);
      final ids = r.strokes.map((s) => s.id).toSet();
      expect(ids.length, 10);
    });

    test('duplicated stroke IDs do NOT collide with original IDs', () {
      // Contract: the host syncs _duplicateEngine.nextId = _nextStrokeId
      // before the call, so the next assigned duplicate ID is always
      // >= _nextStrokeId (which is > every existing stroke ID). Verify
      // the engine honors this by starting nextId high.
      final engine = DuplicateEngine(nextId: 5000);
      final original = _stroke(id: 1, points: [_p(0, 0, 0)]);
      final r = engine.duplicate([original]);
      expect(r.strokes.single.id, greaterThan(original.id));
    });

    test('duplicates are pre-selected for further manipulation', () {
      // Contract: the host sets the active selection to the duplicates'
      // IDs (step 6). Verify the IDs the host would extract from the
      // result match the duplicates (not the originals).
      final engine = DuplicateEngine(nextId: 100);
      final originals = [
        _stroke(id: 1, points: [_p(0, 0, 0)]),
        _stroke(id: 2, points: [_p(1, 0, 0)]),
      ];
      final r = engine.duplicate(originals);
      final duplicateIds = r.strokes.map((s) => s.id).toSet();
      // The host would call _selectionModel.setActive(duplicateIds).
      expect(duplicateIds, {100, 101});
      // Originals are NOT in the new selection.
      expect(duplicateIds.any((id) => id == 1 || id == 2), isFalse);
    });
  });
}
