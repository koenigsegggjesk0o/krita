// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// mirror_assist_test.dart — unit tests for the Mirror drawing assistance.
//
// Covers axis toggling, the 2^N copy count, the X/Y/Z reflection matrices,
// UV-flipping via reflectPoint / reflectPoints, origin-offset reflection,
// LiveMirror streaming, and JSON round-trip.

import 'package:feather_krita/engine/assistance/mirror_assist.dart';
import 'package:flutter_test/flutter_test.dart';
import 'package:vector_math/vector_math_64.dart';

void expectVec(Vector3 actual, Vector3 expected, {double tol = 1e-9}) {
  expect(actual.x, closeTo(expected.x, tol));
  expect(actual.y, closeTo(expected.y, tol));
  expect(actual.z, closeTo(expected.z, tol));
}

void main() {
  group('MirrorAssist axis toggling', () {
    test('starts disabled (empty active set)', () {
      final m = MirrorAssist();
      expect(m.isEnabled, isFalse);
      expect(m.activeAxes, isEmpty);
      expect(m.copyCount, 1);
    });

    test('toggle adds then removes an axis', () {
      final m = MirrorAssist();
      final afterOn = m.toggle(MirrorAxis.x);
      expect(afterOn, {MirrorAxis.x});
      expect(m.isEnabled, isTrue);
      final afterOff = m.toggle(MirrorAxis.x);
      expect(afterOff, isEmpty);
      expect(m.isEnabled, isFalse);
    });

    test('enable / disable / clear mutate the set in place', () {
      final m = MirrorAssist();
      m.enable(MirrorAxis.x);
      m.enable(MirrorAxis.y);
      expect(m.activeAxes, {MirrorAxis.x, MirrorAxis.y});
      m.disable(MirrorAxis.x);
      expect(m.activeAxes, {MirrorAxis.y});
      m.clear();
      expect(m.activeAxes, isEmpty);
    });
  });

  group('MirrorAssist copyCount', () {
    test('2^N for N active axes', () {
      final m = MirrorAssist();
      expect(m.copyCount, 1); // 2^0
      m.enable(MirrorAxis.x);
      expect(m.copyCount, 2); // 2^1
      m.enable(MirrorAxis.y);
      expect(m.copyCount, 4); // 2^2
      m.enable(MirrorAxis.z);
      expect(m.copyCount, 8); // 2^3
    });
  });

  group('MirrorAssist reflection matrices', () {
    test('X axis flips only the X component', () {
      final m = MirrorAssist(activeAxes: {MirrorAxis.x});
      final copies = m.reflectPoint(Vector3(1, 2, 3));
      expect(copies.length, 2);
      expectVec(copies[0], Vector3(1, 2, 3)); // identity
      expectVec(copies[1], Vector3(-1, 2, 3)); // X-flipped
    });

    test('Y axis flips only the Y component', () {
      final m = MirrorAssist(activeAxes: {MirrorAxis.y});
      final copies = m.reflectPoint(Vector3(1, 2, 3));
      expect(copies.length, 2);
      expectVec(copies[1], Vector3(1, -2, 3));
    });

    test('Z axis flips only the Z component', () {
      final m = MirrorAssist(activeAxes: {MirrorAxis.z});
      final copies = m.reflectPoint(Vector3(1, 2, 3));
      expect(copies.length, 2);
      expectVec(copies[1], Vector3(1, 2, -3));
    });

    test('two axes produce four copies (identity + X + Y + XY)', () {
      final m = MirrorAssist(activeAxes: {MirrorAxis.x, MirrorAxis.y});
      final copies = m.reflectPoint(Vector3(1, 2, 3));
      expect(copies.length, 4);
      // The set of results must include all four sign combinations of
      // X and Y (Z stays positive).
      final xs = copies.map((p) => p.x).toSet();
      final ys = copies.map((p) => p.y).toSet();
      expect(xs, {1.0, -1.0});
      expect(ys, {2.0, -2.0});
      for (final p in copies) {
        expect(p.z, 3.0);
      }
    });
  });

  group('MirrorAssist origin offset', () {
    test('reflection through a non-world origin', () {
      final m = MirrorAssist(
        activeAxes: {MirrorAxis.x},
        origin: Vector3(5, 0, 0),
      );
      final copies = m.reflectPoint(Vector3(6, 0, 0));
      // Point at x=6 reflected through x=5 should land at x=4.
      expectVec(copies[0], Vector3(6, 0, 0));
      expectVec(copies[1], Vector3(4, 0, 0));
    });
  });

  group('MirrorAssist reflectPoints', () {
    test('disabled returns a single copy equal to the input', () {
      final m = MirrorAssist();
      final input = [Vector3(1, 0, 0), Vector3(2, 0, 0)];
      final out = m.reflectPoints(input);
      expect(out.length, 1);
      expect(out.first.length, 2);
    });

    test('enabled returns one list per reflection transform', () {
      final m = MirrorAssist(activeAxes: {MirrorAxis.x});
      final input = [Vector3(1, 0, 0), Vector3(2, 0, 0)];
      final out = m.reflectPoints(input);
      expect(out.length, 2);
      expectVec(out[1][0], Vector3(-1, 0, 0));
      expectVec(out[1][1], Vector3(-2, 0, 0));
    });
  });

  group('MirrorAssist LiveMirror', () {
    test('sample reflects a point into every copy', () {
      final assist = MirrorAssist(activeAxes: {MirrorAxis.x, MirrorAxis.y});
      final live = LiveMirror(assist);
      expect(live.copyCount, 4);
      final out = live.sample(Vector3(1, 2, 3));
      expect(out.length, 4);
      // The first copy is the identity.
      expectVec(out.first, Vector3(1, 2, 3));
    });

    test('refresh picks up axis changes', () {
      final assist = MirrorAssist(activeAxes: {MirrorAxis.x});
      final live = LiveMirror(assist);
      expect(live.copyCount, 2);
      assist.enable(MirrorAxis.y);
      live.refresh();
      expect(live.copyCount, 4);
    });
  });

  group('MirrorAssist serialization', () {
    test('toJson / fromJson round-trip preserves axes and origin', () {
      final m = MirrorAssist(
        activeAxes: {MirrorAxis.x, MirrorAxis.z},
        origin: Vector3(1, 2, 3),
      );
      final json = m.toJson();
      final restored = MirrorAssist.fromJson(json);
      expect(restored.activeAxes, {MirrorAxis.x, MirrorAxis.z});
      expectVec(restored.origin!, Vector3(1, 2, 3));
    });

    test('copy produces an equivalent assist', () {
      final m = MirrorAssist(activeAxes: {MirrorAxis.y});
      final c = m.copy();
      expect(c.activeAxes, m.activeAxes);
      expect(c.origin, m.origin);
    });
  });
}
