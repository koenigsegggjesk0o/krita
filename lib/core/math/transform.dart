// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// transform.dart — Immutable TRS (translation/rotation/scale)
// composite transform.
//
// A [Transform] stores a position ([Vec3]), rotation ([Quaternion]),
// and scale ([Vec3]). It can compose to/from a [Mat4] and provides
// chainable, copy-returning mutators ([translated], [rotated],
// [scaledBy], [lookAt]) so transform chains read like a builder DSL.

import 'mat3.dart';
import 'mat4.dart';
import 'quaternion.dart';
import 'vec3.dart';

/// An immutable TRS transform.
class Transform {
  /// World position.
  final Vec3 position;

  /// World rotation.
  final Quaternion rotation;

  /// Local scale.
  final Vec3 scale;

  /// Constructs a transform. Defaults: identity position, identity
  /// rotation, unit scale.
  const Transform({
    this.position = const Vec3.zero(),
    this.rotation = const Quaternion.identity(),
    this.scale = const Vec3(1.0, 1.0, 1.0),
  });

  /// Identity transform.
  static const Transform identity = Transform();

  /// Composes this TRS into a [Mat4] equal to `T * R * S`.
  Mat4 toMatrix() {
    final r = rotation.toMatrix();
    return Mat4(
      r.m00 * scale.x, r.m01 * scale.y, r.m02 * scale.z, position.x,
      r.m10 * scale.x, r.m11 * scale.y, r.m12 * scale.z, position.y,
      r.m20 * scale.x, r.m21 * scale.y, r.m22 * scale.z, position.z,
      0.0, 0.0, 0.0, 1.0,
    );
  }

  /// Decomposes [m] into a [Transform]. Scale is read from the lengths
  /// of the upper-left 3x3 basis columns (signs lost); rotation is
  /// recovered from the orthonormalized upper-left 3x3.
  factory Transform.fromMatrix(Mat4 m) {
    final position = Vec3(m.m03, m.m13, m.m23);
    final sx = Vec3(m.m00, m.m10, m.m20).length;
    final sy = Vec3(m.m01, m.m11, m.m21).length;
    final sz = Vec3(m.m02, m.m12, m.m22).length;
    final sxSafe = sx < 1e-12 ? 1.0 : sx;
    final sySafe = sy < 1e-12 ? 1.0 : sy;
    final szSafe = sz < 1e-12 ? 1.0 : sz;
    final rotMat = Mat3(
      m.m00 / sxSafe, m.m01 / sySafe, m.m02 / szSafe,
      m.m10 / sxSafe, m.m11 / sySafe, m.m12 / szSafe,
      m.m20 / sxSafe, m.m21 / sySafe, m.m22 / szSafe,
    );
    return Transform(
      position: position,
      rotation: Quaternion.fromMatrix(rotMat),
      scale: Vec3(sx, sy, sz),
    );
  }

  /// Returns a transform with [position] translated by [delta] in
  /// world space.
  Transform translated(Vec3 delta) => Transform(
        position: position + delta,
        rotation: rotation,
        scale: scale,
      );

  /// Returns a transform with [delta] applied as a world-space
  /// rotation (pre-multiplied so `result * v` rotates [v] by [delta]
  /// after the existing rotation).
  Transform rotated(Quaternion delta) => Transform(
        position: position,
        rotation: delta * rotation,
        scale: scale,
      );

  /// Returns a transform with [scale] multiplied component-wise by
  /// [factor].
  Transform scaledBy(Vec3 factor) => Transform(
        position: position,
        rotation: rotation,
        scale: scale.multiply(factor),
      );

  /// Returns a transform at the same [position] and [scale] but
  /// rotated so the local +Z axis points toward [target]. [up] is the
  /// reference up direction (default world +Y).
  Transform lookAt(Vec3 target, {Vec3 up = const Vec3.unitY()}) {
    final forward = (target - position).normalized();
    if (forward.length2 < 1e-12) return this;
    final right = up.cross(forward).normalized();
    final newUp = forward.cross(right).normalized();
    // Rotation matrix columns: right, up, forward.
    final rotMat = Mat3(
      right.x, newUp.x, forward.x,
      right.y, newUp.y, forward.y,
      right.z, newUp.z, forward.z,
    );
    return Transform(
      position: position,
      rotation: Quaternion.fromMatrix(rotMat),
      scale: scale,
    );
  }

  /// Linearly interpolates position and scale, and slerps rotation,
  /// toward [other] by [t].
  Transform lerp(Transform other, double t) => Transform(
        position: position.lerp(other.position, t),
        rotation: rotation.slerp(other.rotation, t),
        scale: scale.lerp(other.scale, t),
      );

  @override
  bool operator ==(Object other) =>
      other is Transform &&
      position == other.position &&
      rotation == other.rotation &&
      scale == other.scale;

  @override
  int get hashCode => Object.hash(position, rotation, scale);

  @override
  String toString() =>
      'Transform(position: $position, rotation: $rotation, scale: $scale)';
}
