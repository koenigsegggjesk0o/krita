// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// selection_test.dart — unit tests for the selection state model and
// the tap/box/lasso selection system.

import 'package:feather_krita/engine/selection/selection.dart';
import 'package:feather_krita/engine/selection/selection_state.dart';
import 'package:feather_krita/models/stroke.dart';
import 'package:flutter_test/flutter_test.dart';

import 'package:vector_math/vector_math_64.dart';

Stroke _stroke(int id, {bool visible = true}) => Stroke(
      id: id,
      points: [
        StrokePoint(position: Vector3(id.toDouble(), 0, 0)),
        StrokePoint(position: Vector3(id.toDouble() + 1, 0, 0)),
      ],
      isVisible: visible,
    );

void main() {
  group('SelectionState', () {
    test('default state is empty with no hover or primary', () {
      const s = SelectionState();
      expect(s.isEmpty, isTrue);
      expect(s.count, 0);
      expect(s.hover, isNull);
      expect(s.primary, isNull);
    });

    test('withActive sets primary to the last id', () {
      const s = SelectionState();
      final next = s.withActive([1, 2, 3]);
      expect(next.active, [1, 2, 3]);
      expect(next.primary, 3);
    });

    test('withActive on empty list clears primary', () {
      const s = SelectionState(active: [1, 2], primary: 2);
      final next = s.withActive(const <int>[]);
      expect(next.isEmpty, isTrue);
      expect(next.primary, isNull);
    });

    test('withHover / withPrimary update only the requested field', () {
      const s = SelectionState(active: [1]);
      final h = s.withHover(5);
      expect(h.hover, 5);
      expect(h.active, [1]);
      final p = s.withPrimary(1);
      expect(p.primary, 1);
    });

    test('withPrimary rejects ids not in the active set', () {
      const s = SelectionState(active: [1, 2]);
      final p = s.withPrimary(99);
      expect(p, same(s)); // returns the receiver unchanged.
    });

    test('equality is value-based', () {
      const a = SelectionState(active: [1, 2], hover: 3, primary: 2);
      const b = SelectionState(active: [1, 2], hover: 3, primary: 2);
      expect(a, b);
      expect(a.hashCode, b.hashCode);
    });

    test('isActive / isHover / isPrimary predicates', () {
      const s = SelectionState(active: [1, 2], hover: 2, primary: 1);
      expect(s.isActive(1), isTrue);
      expect(s.isActive(99), isFalse);
      expect(s.isHover(2), isTrue);
      expect(s.isPrimary(1), isTrue);
    });
  });

  group('SelectionModel', () {
    test('starts empty', () {
      final m = SelectionModel();
      expect(m.isEmpty, isTrue);
      expect(m.active, isEmpty);
      expect(m.primary, isNull);
    });

    test('setActive replaces the active set and notifies', () {
      final m = SelectionModel();
      var notified = 0;
      m.addListener(() => notified++);
      m.setActive([1, 2, 3]);
      expect(m.active, [1, 2, 3]);
      expect(m.primary, 3);
      expect(notified, 1);
    });

    test('add appends and updates primary', () {
      final m = SelectionModel()..setActive([1]);
      m.add(2);
      expect(m.active, [1, 2]);
      expect(m.primary, 2);
    });

    test('add is a no-op for an existing id (no notification)', () {
      final m = SelectionModel()..setActive([1]);
      var notified = 0;
      m.addListener(() => notified++);
      m.add(1);
      expect(notified, 0);
      expect(m.active, [1]);
    });

    test('remove drops the id and re-elects the last remaining as primary', () {
      final m = SelectionModel()..setActive([1, 2, 3]);
      m.remove(2);
      expect(m.active, [1, 3]);
      expect(m.primary, 3);
      m.remove(3);
      expect(m.active, [1]);
      expect(m.primary, 1);
    });

    test('clear empties the active set', () {
      final m = SelectionModel()..setActive([1, 2]);
      m.clear();
      expect(m.isEmpty, isTrue);
      expect(m.primary, isNull);
    });

    test('toggle adds when absent and removes when present', () {
      final m = SelectionModel()..setActive([1]);
      m.toggle(2);
      expect(m.active, [1, 2]);
      m.toggle(1);
      expect(m.active, [2]);
    });

    test('setHover updates hover without disturbing active', () {
      final m = SelectionModel()..setActive([1]);
      m.setHover(5);
      expect(m.hover, 5);
      expect(m.active, [1]);
    });

    test('setPrimary promotes an active id', () {
      final m = SelectionModel()..setActive([1, 2, 3]);
      m.setPrimary(1);
      expect(m.primary, 1);
    });

    test('reconcile drops stale ids', () {
      final m = SelectionModel()..setActive([1, 2, 3]);
      m.reconcile([_stroke(1), _stroke(3)]);
      expect(m.active, [1, 3]);
    });
  });

  group('SelectionSystem — single-stroke operations', () {
    late List<Stroke> strokes;
    late SelectionSystem sys;

    setUp(() {
      strokes = [_stroke(1), _stroke(2), _stroke(3)];
      sys = SelectionSystem(
        model: SelectionModel(),
        strokes: () => strokes,
      );
    });

    test('select with replace resets the active set', () {
      sys.select(1);
      expect(sys.model.active, [1]);
      sys.select(2);
      expect(sys.model.active, [2]);
    });

    test('select with add accumulates ids', () {
      sys.select(1);
      sys.select(2, mode: SelectMode.add);
      expect(sys.model.active, [1, 2]);
    });

    test('select with subtract removes an id', () {
      sys.selectAll();
      sys.select(2, mode: SelectMode.subtract);
      expect(sys.model.active, [1, 3]);
    });

    test('select with toggle flips membership', () {
      sys.select(1);
      sys.select(1, mode: SelectMode.toggle);
      expect(sys.model.isEmpty, isTrue);
      sys.select(1, mode: SelectMode.toggle);
      expect(sys.model.active, [1]);
    });

    test('deselect clears the active set', () {
      sys.selectAll();
      sys.deselect();
      expect(sys.model.isEmpty, isTrue);
    });

    test('selectAll selects every stroke id', () {
      sys.selectAll();
      expect(sys.model.active.toSet(), {1, 2, 3});
    });

    test('invert flips the selection', () {
      sys.select(1);
      sys.invert();
      expect(sys.model.active.toSet(), {2, 3});
    });

    test('selectedStrokes returns the Stroke objects in the active set', () {
      sys.select(1);
      sys.select(3, mode: SelectMode.add);
      final selected = sys.selectedStrokes();
      expect(selected.length, 2);
      expect(selected.map((s) => s.id).toSet(), {1, 3});
    });
  });

  group('SelectionSystem — tap-select', () {
    test('tapSelectSync selects the stroke whose segment is under the ray', () {
      final strokes = [_stroke(1), _stroke(2)];
      final sys = SelectionSystem(
        model: SelectionModel(),
        strokes: () => strokes,
      );
      // Ray origin right on stroke 1's segment (which spans x=1..2 at
      // y=0, z=0). The default worldTapRadius (0.05) covers the hit.
      sys.tapSelectSync(
        Vector3(1.5, 0, 0),
        Vector3(0, 0, 1),
        worldTapRadius: 0.1,
      );
      expect(sys.model.active, [1]);
    });

    test('tapSelectSync with no hit clears the selection in replace mode', () {
      final strokes = [_stroke(1)];
      final sys = SelectionSystem(
        model: SelectionModel()..setActive([1]),
        strokes: () => strokes,
      );
      sys.tapSelectSync(
        Vector3(100, 100, 100),
        Vector3(0, 0, 1),
        worldTapRadius: 0.1,
      );
      expect(sys.model.isEmpty, isTrue);
    });

    test('tapSelectSync in add mode does not deselect on miss', () {
      final strokes = [_stroke(1)];
      final sys = SelectionSystem(
        model: SelectionModel()..setActive([1]),
        strokes: () => strokes,
      );
      sys.tapSelectSync(
        Vector3(100, 100, 100),
        Vector3(0, 0, 1),
        mode: SelectMode.add,
        worldTapRadius: 0.1,
      );
      expect(sys.model.active, [1]);
    });

    test('invisible strokes are never hit', () {
      final strokes = [_stroke(1, visible: false)];
      final sys = SelectionSystem(
        model: SelectionModel(),
        strokes: () => strokes,
      );
      sys.tapSelectSync(
        Vector3(1.5, 0, 0),
        Vector3(0, 0, 1),
        worldTapRadius: 0.1,
      );
      expect(sys.model.isEmpty, isTrue);
    });
  });

  group('SelectionSystem — box / lasso', () {
    test('boxSelect selects strokes whose projection lands inside the box', () {
      // Identity view-projection: world coords == NDC. Stroke 1 has
      // points at x=1 and x=2. With a viewport of 100x100, NDC x=1
      // maps to screen x=100, x=2 maps to 150 — only x=1 is inside
      // [0, 100].
      final strokes = [_stroke(1), _stroke(2)];
      final sys = SelectionSystem(
        model: SelectionModel(),
        strokes: () => strokes,
      );
      sys.boxSelect(
        Vector2(0, 0),
        Vector2(110, 110),
        Matrix4.identity(),
        viewportWidth: 100,
        viewportHeight: 100,
      );
      expect(sys.model.active, [1]);
    });

    test('lassoSelect selects strokes inside the polygon', () {
      final strokes = [_stroke(1), _stroke(5)];
      final sys = SelectionSystem(
        model: SelectionModel(),
        strokes: () => strokes,
      );
      // Lasso covering screen x in [-10, 10], y in [-10, 10]. With
      // identity view-projection and a 100x100 viewport, stroke 1's
      // first point at world (1,0,0) maps to screen (100, 50) — outside
      // the lasso. Stroke 5's first point at (5,0,0) maps to (300, 50)
      // — also outside. Both should miss.
      sys.lassoSelect(
        [
          Vector2(-10, -10),
          Vector2(10, -10),
          Vector2(10, 10),
          Vector2(-10, 10),
        ],
        Matrix4.identity(),
        viewportWidth: 100,
        viewportHeight: 100,
      );
      expect(sys.model.isEmpty, isTrue);
    });

    test('lassoSelect with fewer than 3 points is a no-op', () {
      final strokes = [_stroke(1)];
      final sys = SelectionSystem(
        model: SelectionModel(),
        strokes: () => strokes,
      );
      sys.lassoSelect(
        [Vector2(0, 0), Vector2(1, 1)],
        Matrix4.identity(),
        viewportWidth: 100,
        viewportHeight: 100,
      );
      expect(sys.model.isEmpty, isTrue);
    });
  });
}
