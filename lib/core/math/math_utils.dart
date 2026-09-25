// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// math_utils.dart — Core math constants and helper functions for the
// Feather-Krita 3D engine.
//
// Pure Dart. No external dependencies. All angles are in radians unless
// noted otherwise. Functions are top-level so callers can `import
// 'math_utils.dart'` and use them directly without a class prefix.

import 'dart:math' as math;

/// Pi.
const double PI = math.pi;

/// Two times Pi (a full circle in radians).
const double TWO_PI = 2.0 * math.pi;

/// Half Pi (a quarter circle in radians).
const double HALF_PI = math.pi / 2.0;

/// Multiply a degree value by this to convert to radians.
const double DEG2RAD = math.pi / 180.0;

/// Multiply a radian value by this to convert to degrees.
const double RAD2DEG = 180.0 / math.pi;

/// Default comparison tolerance for double-precision float math.
const double epsilon = 1e-9;

/// Converts [degrees] to radians.
double degToRad(double degrees) => degrees * DEG2RAD;

/// Converts [radians] to degrees.
double radToDeg(double radians) => radians * RAD2DEG;

/// Clamps [value] to the inclusive range [lower]-[upper].
double clampDouble(double value, double lower, double upper) {
  if (value < lower) return lower;
  if (value > upper) return upper;
  return value;
}

/// Linearly interpolates between [a] and [b] by [t] (0 returns [a], 1
/// returns [b]). [t] is not clamped.
double lerp(double a, double b, double t) => a + (b - a) * t;

/// Linearly interpolates between angles [a] and [b] (radians), taking the
/// shortest path around the circle.
double lerpAngle(double a, double b, double t) {
  var delta = (b - a) % TWO_PI;
  if (delta > PI) delta -= TWO_PI;
  if (delta < -PI) delta += TWO_PI;
  return a + delta * t;
}

/// Smoothstep: returns 0 below [edge0], 1 above [edge1], and a smooth
/// Hermite interpolation in between.
double smoothstep(double edge0, double edge1, double x) {
  final t = clampDouble((x - edge0) / (edge1 - edge0), 0.0, 1.0);
  return t * t * (3.0 - 2.0 * t);
}

/// Remaps [value] from the input range [inMin]-[inMax] to the output
/// range [outMin]-[outMax]. Returns [outMin] when the input range is
/// degenerate.
double remap(double value, double inMin, double inMax, double outMin,
    double outMax) {
  if ((inMax - inMin).abs() < epsilon) return outMin;
  final t = (value - inMin) / (inMax - inMin);
  return outMin + t * (outMax - outMin);
}

/// Returns 1.0 for positive [value], -1.0 for negative, and 0.0 for zero
/// (or NaN).
double sign(double value) {
  if (value > 0.0) return 1.0;
  if (value < 0.0) return -1.0;
  return 0.0;
}

/// Moves [current] toward [target] by at most [maxDelta]. Never
/// overshoots. Returns [target] when within [maxDelta].
double approach(double current, double target, double maxDelta) {
  if ((target - current).abs() <= maxDelta) return target;
  return current + sign(target - current) * maxDelta;
}

/// "Ping-pongs" [value] inside [0, [length]) so it folds back at both
/// ends. Useful for back-and-forth animation timing. Returns 0 when
/// [length] is non-positive.
double pingPong(double value, double length) {
  if (length < epsilon) return 0.0;
  var t = value % (2.0 * length);
  if (t < 0.0) t += 2.0 * length;
  return t < length ? t : 2.0 * length - t;
}

/// Returns true when [a] and [b] are within [tol] of each other.
bool equalsApprox(double a, double b, {double tol = 1e-6}) =>
    (a - b).abs() <= tol;

/// Inverse square root (1 / sqrt([x])). Returns 0 when [x] is not
/// positive to avoid producing infinities or NaN.
double invSqrt(double x) {
  if (x <= 0.0) return 0.0;
  return 1.0 / math.sqrt(x);
}
