// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// transform_delta_test.dart — unit tests for the TransformDelta value
// object (the output of every joystick resolver).
//
// Covers the zero constant, isZero predicate, translation / rotation /
// scale composition, and pivot inheritance.

import 'dart:math' as math;

import 'package:feather_krita/engine/transform/transform_resolver.dart';
import 'package:flutter_test/flutter_test.dart';
import 'package:vector_math/vector_math_64.dart';

void main() {
  group('TransformDelta.zero', () {
    test('isZero is true for the constant', () {
      expect(TransformDelta.zero.isZero, isTrue);
    });

    test('a delta with any field set is not zero', () {
      final t = TransformDelta(translation: Vector3.zero());
      expect(t.isZero, isFalse);
    });
  });

  group('TransformDelta.compose — translation', () {
    test('two translations add', () {
      final a = TransformDelta(translation: Vector3(1, 2, 3));
      final b = TransformDelta(translation: Vector3(10, 20, 30));
      final c = a.compose(b);
      expect(c.translation, isNotNull);
      expect(c.translation!.x, closeTo(11, 1e-9));
      expect(c.translation!.y, closeTo(22, 1e-9));
      expect(c.translation!.z, closeTo(33, 1e-9));
    });

    test('composing with zero returns the non-zero translation', () {
      final a = TransformDelta(translation: Vector3(5, 0, 0));
      final c = a.compose(TransformDelta.zero);
      expect(c.translation, isNotNull);
      expect(c.translation!.x, 5);
      // Symmetric: zero ∘ a.
      final d = TransformDelta.zero.compose(a);
      expect(d.translation, isNotNull);
      expect(d.translation!.x, 5);
    });
  });

  group('TransformDelta.compose — rotation', () {
    test('two rotations compose as other * this', () {
      // 90° about Z, then another 90° about Z = 180° about Z.
      final q1 = Quaternion.axisAngle(Vector3(0, 0, 1), math.pi / 2);
      final q2 = Quaternion.axisAngle(Vector3(0, 0, 1), math.pi / 2);
      final aT = TransformDelta(rotation: q1);
      final bT = TransformDelta(rotation: q2);
      final c = aT.compose(bT);
      expect(c.rotation, isNotNull);
      // Composed rotation maps +X to -X.
      final v = c.rotation!.rotate(Vector3(1, 0, 0));
      expect(v.x, closeTo(-1.0, 1e-6));
      expect(v.y, closeTo(0.0, 1e-6));
    });

    test('composing a rotation with zero preserves the rotation', () {
      final q = Quaternion.axisAngle(Vector3(0, 0, 1), math.pi / 4);
      final a = TransformDelta(rotation: q);
      final c = a.compose(TransformDelta.zero);
      expect(c.rotation, isNotNull);
      // Same axis & angle.
      expect(c.rotation!.w, closeTo(q.w, 1e-9));
    });
  });

  group('TransformDelta.compose — scale', () {
    test('two scales multiply component-wise', () {
      final a = TransformDelta(scale: Vector3(2, 3, 4));
      final b = TransformDelta(scale: Vector3(5, 6, 7));
      final c = a.compose(b);
      expect(c.scale, isNotNull);
      expect(c.scale!.x, closeTo(10, 1e-9));
      expect(c.scale!.y, closeTo(18, 1e-9));
      expect(c.scale!.z, closeTo(28, 1e-9));
    });

    test('composing scale with zero returns the non-zero scale', () {
      final a = TransformDelta(scale: Vector3(2, 2, 2));
      final c = TransformDelta.zero.compose(a);
      expect(c.scale, isNotNull);
      expect(c.scale!.x, 2);
    });
  });

  group('TransformDelta.compose — pivot', () {
    test('pivot is inherited from `this` when present', () {
      final pivot = Vector3(1, 2, 3);
      final a = TransformDelta(pivot: pivot);
      const b = TransformDelta();
      final c = a.compose(b);
      expect(c.pivot, isNotNull);
      expect(c.pivot!.x, 1);
      expect(c.pivot!.y, 2);
      expect(c.pivot!.z, 3);
    });

    test('pivot falls back to `other` when `this` is null', () {
      final pivot = Vector3(4, 5, 6);
      const a = TransformDelta();
      final b = TransformDelta(pivot: pivot);
      final c = a.compose(b);
      expect(c.pivot, isNotNull);
      expect(c.pivot!.x, 4);
    });
  });
}
