// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// transform_test.dart — unit tests for the 2D/3D joystick transform
// resolvers and the Shoemake trackball.

import 'dart:math' as math;

import 'package:feather_krita/engine/transform/joystick2d.dart';
import 'package:feather_krita/engine/transform/joystick3d.dart';
import 'package:feather_krita/engine/transform/trackball.dart';
import 'package:feather_krita/engine/transform/transform_mode.dart';
import 'package:feather_krita/engine/transform/transform_resolver.dart';
import 'package:flutter_test/flutter_test.dart';

import 'package:vector_math/vector_math_64.dart';

ViewFrame _frame({
  Vector3? right,
  Vector3? up,
  Vector3? forward,
  double width = 800,
  double height = 600,
  Vector3? crosshair,
}) =>
    ViewFrame(
      right: right ?? Vector3(1, 0, 0),
      up: up ?? Vector3(0, 1, 0),
      forward: forward ?? Vector3(0, 0, -1),
      viewportWidth: width,
      viewportHeight: height,
      crosshairWorld: crosshair ?? Vector3.zero(),
    );

void main() {
  group('Joystick2dResolver — move', () {
    final r = Joystick2dResolver(worldPerPixel: 0.01);

    test('drag right moves along +camera-right', () {
      final d = r.resolve(
        mode: TransformMode2d.move,
        delta: Vector2(1, 0),
        frame: _frame(),
      );
      expect(d.translation, isNotNull);
      // Drag right by full delta = viewport*0.5*0.01 = 4 world units.
      expect(d.translation!.x, closeTo(4.0, 1e-6));
      expect(d.translation!.y, 0);
      expect(d.translation!.z, 0);
    });

    test('drag up moves along -camera-up (screen-y inverted)', () {
      final d = r.resolve(
        mode: TransformMode2d.move,
        delta: Vector2(0, 1),
        frame: _frame(),
      );
      // Per the resolver's convention, delta.y=+1 ("up") is negated
      // before mapping to the world up vector.
      expect(d.translation!.y, closeTo(-3.0, 1e-6));
    });

    test('locked move keeps only the dominant axis', () {
      final d = r.resolve(
        mode: TransformMode2d.move,
        lock: JoystickLock.on,
        delta: Vector2(0.6, 0.4),
        frame: _frame(),
      );
      expect(d.translation!.x, greaterThan(0));
      expect(d.translation!.y, 0);
    });

    test('move returns no rotation or scale', () {
      final d = r.resolve(
        mode: TransformMode2d.move,
        delta: Vector2(0.5, 0.5),
        frame: _frame(),
      );
      expect(d.rotation, isNull);
      expect(d.scale, isNull);
    });
  });

  group('Joystick2dResolver — rotate', () {
    final r = Joystick2dResolver();

    test('rotation is around the view normal', () {
      final frame = _frame(forward: Vector3(0, 0, -1));
      final d = r.resolve(
        mode: TransformMode2d.rotate,
        delta: Vector2(0, -1), // atan2(1, 0) = pi/2 → non-zero angle
        frame: frame,
      );
      expect(d.rotation, isNotNull);
      expect(d.pivot, frame.crosshairWorld);
      // The axis of the rotation should be the view normal.
      final axis = _axisOf(d.rotation!);
      expect(axis.x, closeTo(0, 1e-6));
      expect(axis.y, closeTo(0, 1e-6));
      expect(axis.z.abs(), closeTo(1, 1e-6));
    });

    test('locked rotate snaps to 15 degree increments', () {
      final r2 = Joystick2dResolver();
      final d = r2.resolve(
        mode: TransformMode2d.rotate,
        lock: JoystickLock.on,
        delta: Vector2(0.1, 0.05),
        frame: _frame(),
      );
      final angle = _angleOf(d.rotation!);
      // The snapped angle must be a multiple of 15° in radians.
      final snap = 15.0 * math.pi / 180.0;
      expect((angle / snap).round() * snap, closeTo(angle, 1e-6));
    });
  });

  group('Joystick2dResolver — scale', () {
    final r = Joystick2dResolver();

    test('freeScale grows on upward drag', () {
      final d = r.resolve(
        mode: TransformMode2d.freeScale,
        delta: Vector2(0, -1),
        frame: _frame(),
      );
      expect(d.scale, isNotNull);
      expect(d.scale!.x, greaterThan(1.0));
      expect(d.scale!.x, d.scale!.y);
      expect(d.scale!.x, d.scale!.z);
    });

    test('widthScale changes only the x component', () {
      final d = r.resolve(
        mode: TransformMode2d.widthScale,
        delta: Vector2(0.5, 0),
        frame: _frame(),
      );
      expect(d.scale!.x, greaterThan(1.0));
      expect(d.scale!.y, 1.0);
      expect(d.scale!.z, 1.0);
    });

    test('heightScale changes only the y component', () {
      final d = r.resolve(
        mode: TransformMode2d.heightScale,
        delta: Vector2(0, -0.5),
        frame: _frame(),
      );
      expect(d.scale!.y, greaterThan(1.0));
      expect(d.scale!.x, 1.0);
      expect(d.scale!.z, 1.0);
    });

    test('scale pivot is the crosshair', () {
      final frame = _frame(crosshair: Vector3(1, 2, 3));
      final d = r.resolve(
        mode: TransformMode2d.freeScale,
        delta: Vector2(0, -0.5),
        frame: frame,
      );
      expect(d.pivot, Vector3(1, 2, 3));
    });
  });

  group('Joystick2dResolver — misc', () {
    test('rejects a 3D mode with an ArgumentError', () {
      final r = Joystick2dResolver();
      expect(
        () => r.resolve(
          mode: TransformMode3d.moveX,
          delta: Vector2(1, 0),
          frame: _frame(),
        ),
        throwsArgumentError,
      );
    });

    test('kind reports joystick2d', () {
      expect(Joystick2dResolver().kind, JoystickKind.joystick2d);
    });
  });

  group('Joystick3dResolver — translate', () {
    final r = Joystick3dResolver(worldPerPixel: 0.01);

    test('moveX translates along the world X axis', () {
      final d = r.resolve(
        mode: TransformMode3d.moveX,
        delta: Vector2(1, 0),
        frame: _frame(),
      );
      expect(d.translation, isNotNull);
      expect(d.translation!.x, greaterThan(0));
      expect(d.translation!.y, 0);
      expect(d.translation!.z, 0);
    });

    test('moveY translates along the world Y axis', () {
      final d = r.resolve(
        mode: TransformMode3d.moveY,
        delta: Vector2(0, -1),
        frame: _frame(),
      );
      expect(d.translation, isNotNull);
      expect(d.translation!.y, greaterThan(0));
      expect(d.translation!.x, 0);
      expect(d.translation!.z, 0);
    });

    test('moveZ translates along the world Z axis when viewable', () {
      // Frame where Z is visible on screen (forward = -Y so the screen
      // shows the XZ plane).
      final frame = _frame(
        right: Vector3(1, 0, 0),
        up: Vector3(0, 0, 1),
        forward: Vector3(0, -1, 0),
      );
      final d = r.resolve(
        mode: TransformMode3d.moveZ,
        delta: Vector2(0, 1),
        frame: frame,
      );
      expect(d.translation, isNotNull);
      expect(d.translation!.z, isNot(0));
    });

    test('axis aligned with view normal returns zero translation', () {
      // X axis aligned with the view normal (forward = +X).
      final frame = _frame(
        right: Vector3(0, 1, 0),
        up: Vector3(0, 0, 1),
        forward: Vector3(1, 0, 0),
      );
      final d = r.resolve(
        mode: TransformMode3d.moveX,
        delta: Vector2(1, 1),
        frame: frame,
      );
      expect(d.isZero, isTrue);
    });
  });

  group('Joystick3dResolver — rotate', () {
    final r = Joystick3dResolver();

    test('rotateX produces a rotation about the X axis', () {
      final d = r.resolve(
        mode: TransformMode3d.rotateX,
        delta: Vector2(0, 1),
        frame: _frame(),
      );
      expect(d.rotation, isNotNull);
      final axis = _axisOf(d.rotation!);
      expect(axis.x.abs(), closeTo(1, 1e-6));
      expect(axis.y, closeTo(0, 1e-6));
      expect(axis.z, closeTo(0, 1e-6));
    });

    test('rotateY produces a rotation about the Y axis', () {
      final frame = _frame(
        right: Vector3(1, 0, 0),
        up: Vector3(0, 0, 1),
        forward: Vector3(0, -1, 0),
      );
      final d = r.resolve(
        mode: TransformMode3d.rotateY,
        delta: Vector2(1, 0),
        frame: frame,
      );
      expect(d.rotation, isNotNull);
      final axis = _axisOf(d.rotation!);
      expect(axis.y.abs(), closeTo(1, 1e-6));
    });

    test('rotateZ produces a rotation about the Z axis', () {
      final d = r.resolve(
        mode: TransformMode3d.rotateZ,
        delta: Vector2(1, 0),
        frame: _frame(),
      );
      expect(d.rotation, isNotNull);
      final axis = _axisOf(d.rotation!);
      expect(axis.z.abs(), closeTo(1, 1e-6));
    });

    test('rejects a 2D mode with an ArgumentError', () {
      expect(
        () => r.resolve(
          mode: TransformMode2d.move,
          delta: Vector2(1, 0),
          frame: _frame(),
        ),
        throwsArgumentError,
      );
    });
  });

  group('Joystick3dResolver — free rotate & usable axes', () {
    final r = Joystick3dResolver(trackballRadiusPx: 80);

    test('freeRotate returns a quaternion on a real drag', () {
      final d = r.resolve(
        mode: TransformMode3d.freeRotate,
        delta: Vector2(0.5, -0.5),
        frame: _frame(),
      );
      expect(d.rotation, isNotNull);
      // The rotation magnitude is non-zero.
      expect(_angleOf(d.rotation!).abs(), greaterThan(0));
    });

    test('freeRotate on a zero drag returns the identity delta', () {
      final d = r.resolve(
        mode: TransformMode3d.freeRotate,
        delta: Vector2.zero(),
        frame: _frame(),
      );
      expect(d.isZero, isTrue);
    });

    test('usableAxes excludes the axis aligned with the view normal', () {
      // Forward = +X means the X-axis cone is hidden.
      final frame = _frame(
        right: Vector3(0, 1, 0),
        up: Vector3(0, 0, 1),
        forward: Vector3(1, 0, 0),
      );
      final axes = r.usableAxes(frame);
      expect(axes.contains(TransformMode3d.moveX), isFalse);
      expect(axes.contains(TransformMode3d.moveY), isTrue);
      expect(axes.contains(TransformMode3d.moveZ), isTrue);
    });

    test('kind reports joystick3d', () {
      expect(r.kind, JoystickKind.joystick3d);
    });
  });

  group('Trackball', () {
    test('zero drag returns identity', () {
      const tb = Trackball(radius: 80);
      final q = tb.drag(const Offset2.zero(), const Offset2.zero());
      // Identity quaternion: (0, 0, 0, 1).
      expect(q.x, 0);
      expect(q.y, 0);
      expect(q.z, 0);
      expect(q.w, closeTo(1, 1e-6));
    });

    test('drag produces a rotation in the screen plane', () {
      const tb = Trackball(radius: 80);
      final q = tb.drag(const Offset2.zero(), const Offset2(10, 0));
      // Pure horizontal drag rotates around the Y axis (screen up).
      expect(q.length, closeTo(1.0, 1e-6));
      expect(q.y.abs(), greaterThan(0));
    });

    test('velocity uses dt to scale the per-frame drag', () {
      const tb = Trackball(radius: 80);
      final q = tb.velocity(const Offset2(100, 0), 0.016);
      // dt > 0 → rotation is non-identity.
      expect(q.length, closeTo(1.0, 1e-6));
      expect(q.y.abs(), greaterThan(0));
    });

    test('velocity with dt=0 returns identity', () {
      const tb = Trackball(radius: 80);
      final q = tb.velocity(const Offset2(100, 0), 0);
      expect(q.w, closeTo(1, 1e-6));
    });
  });

  group('TransformDelta', () {
    test('zero delta is isZero', () {
      expect(TransformDelta.zero.isZero, isTrue);
    });

    test('compose accumulates translations', () {
      final a = TransformDelta(translation: Vector3(1, 0, 0));
      final b = TransformDelta(translation: Vector3(0, 2, 0));
      final c = a.compose(b);
      expect(c.translation, Vector3(1, 2, 0));
    });

    test('compose multiplies scales', () {
      final a = TransformDelta(scale: Vector3(2, 2, 2));
      final b = TransformDelta(scale: Vector3(3, 3, 3));
      final c = a.compose(b);
      expect(c.scale, Vector3(6, 6, 6));
    });
  });
}

Vector3 _axisOf(Quaternion q) {
  final w = q.w.clamp(-1.0, 1.0);
  final s = math.sqrt(1 - w * w);
  if (s < 1e-6) return Vector3(1, 0, 0);
  return Vector3(q.x / s, q.y / s, q.z / s);
}

double _angleOf(Quaternion q) {
  final w = q.w.clamp(-1.0, 1.0);
  return 2 * math.acos(w);
}
