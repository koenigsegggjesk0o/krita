// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// trackball.dart — Virtual-trackball math: 2D drag → 3D rotation quaternion.
//
// Implements the classic Shoemake trackball (Ken Shoemake, "Animating
// Rotation with Quaternion Curves", 1985). A 2D pointer drag inside the
// virtual sphere is mapped to a 3D rotation by pretending the pointer
// sits on the surface (or just inside) of an imaginary sphere centred on
// the screen, then computing the rotation that takes the previous sphere
// point to the current one.
//
// This is the math behind the Feather 3D joystick's central sphere ("Free
// Rotate — tap and drag the center sphere of the 3D Joystick for free
// rotation. You can rotate the selected curve or resource like a
// trackball, and the rotation center point is the invisible center of the
// selected object.")
//
// Properties of this implementation:
//   - Drags inside the sphere (|p| <= radius * sqrt(2)/2) follow the sphere
//     surface exactly.
//   - Drags outside that circle are projected onto the sphere's equator
//     (z = 0) so the rotation stays well-conditioned near the edge — the
//     original Shoemake trick.
//   - The output is a unit [Quaternion] that can be composed with other
//     rotations or fed straight into `Stroke.applyRotation`.
//   - Stateless: every call returns the rotation for a single (prev →
//     current) pair; the caller accumulates them by quaternion
//     multiplication.

import 'dart:math' as math;

import 'package:vector_math/vector_math_64.dart';

import 'package:feather_krita/utils/vector_math_utils.dart';

/// Stateless virtual-trackball mapper.
///
/// Construct with the screen-space [radius] of the virtual sphere (the
/// trackball's circle in pixels) and (optionally) a [sensitivity] factor
/// that scales the resulting rotation angle.
class Trackball {
  const Trackball({
    required this.radius,
    this.sensitivity = 1.0,
  });

  /// Screen-space radius of the virtual sphere in pixels. Drags within
  /// `radius * sqrt(2)/2` of the centre are mapped onto the sphere's
  /// surface; drags beyond that are clamped to the equator.
  final double radius;

  /// Multiplier on the rotation angle. Defaults to 1.0 (pure Shoemake).
  final double sensitivity;

  /// Maps a 2D drag (from [prev] to [current], both in pixels, relative to
  /// the trackball centre) into a 3D rotation quaternion.
  ///
  /// The rotation axis lives in the screen plane (xy), the rotation angle
  /// is proportional to the arc length on the virtual sphere.
  Quaternion drag(Offset2 prev, Offset2 current) {
    final p0 = _projectToSphere(prev);
    final p1 = _projectToSphere(current);
    // Cross product gives the rotation axis (in screen space, z out of
    // the screen); dot gives cos(angle).
    var axis = p0.cross(p1);
    final dot = p0.dot(p1);
    if (axis.length2 < 1e-12) {
      // Pointer barely moved or moved along the same great circle.
      return Quaternion.identity();
    }
    // Angle on the sphere surface = atan2(|cross|, dot), but we use the
    // simpler arc formulation: angle = asin(|cross|) which is accurate
    // for the small angles typical of pointer drags.
    var angle = math.asin(axis.length.clamp(0.0, 1.0));
    // Shoemake's trick: extend angle when points are on the sphere
    // surface (this makes large drags feel right).
    if (dot < 0) {
      angle = math.pi - angle;
    }
    angle *= sensitivity;
    axis.normalize();
    if (axis.length2 < 1e-12) return Quaternion.identity();
    return Quaternion.axisAngle(axis, angle);
  }

  /// Maps a 2D velocity (pixels/sec) into an instantaneous angular
  /// velocity quaternion (per-second rotation). Useful for inertia.
  Quaternion velocity(Offset2 v, double dt) {
    if (dt <= 0) return Quaternion.identity();
    final perFrame = Offset2(v.x * dt, v.y * dt);
    return drag(const Offset2.zero(), perFrame);
  }

  /// Projects a screen-space point onto the virtual sphere surface.
  ///
  /// Returns a [Vector3] in "trackball space" where +X is right, +Y is up,
  /// +Z is out of the screen toward the viewer. The point is normalised
  /// so |p| = 1.
  Vector3 _projectToSphere(Offset2 p) {
    final r = radius;
    if (r <= 0) return Vector3(0, 0, 1);
    final x = p.x / r;
    // Flip Y because screen coords grow downward but we want up = +Y.
    final y = -p.y / r;
    final d2 = x * x + y * y;
    const innerEdge = 0.5; // (sqrt(2)/2)^2
    double z;
    if (d2 <= innerEdge) {
      // Inside the inner circle — sphere surface.
      z = math.sqrt(1.0 - d2);
    } else {
      // Outside — hyperbolic sheet (Shoemake's recommendation) so the
      // rotation stays smooth past the equator. We use the simple
      // z = (r²/2)/|p| form which is the standard trackball fallback.
      final d = math.sqrt(d2);
      z = innerEdge / d;
    }
    final v = Vector3(x, y, z);
    return v..normalize();
  }
}

/// Two-dimensional point used by the trackball. Lives here (and not in
/// dart:ui) so the engine module has no Flutter dependency.
class Offset2 {
  const Offset2(this.x, this.y);
  const Offset2.zero() : x = 0, y = 0;
  final double x;
  final double y;

  Offset2 operator +(Offset2 o) => Offset2(x + o.x, y + o.y);
  Offset2 operator -(Offset2 o) => Offset2(x - o.x, y - o.y);
  Offset2 operator *(double s) => Offset2(x * s, y * s);

  double get length => math.sqrt(x * x + y * y);
  double get length2 => x * x + y * y;

  @override
  String toString() => 'Offset2($x, $y)';
}

/// Convenience helper: compose a sequence of trackball drag quaternions
/// into a single cumulative rotation. Newest first, so qTotal = q[n] *
/// q[n-1] * ... * q[0].
Quaternion composeTrackball(List<Quaternion> qs) {
  var out = Quaternion.identity();
  for (final q in qs) {
    out = q * out;
  }
  return out..normalize();
}

/// Returns the quaternion that rotates [from] onto [to] using the
/// shortest arc on the unit sphere. Same as
/// `VectorMathUtils.fromToRotation` but kept here for trackball-related
/// callers that want it in the same module.
Quaternion shortestArc(Vector3 from, Vector3 to) {
  return VectorMathUtils.fromToRotation(from.normalized(), to.normalized());
}
