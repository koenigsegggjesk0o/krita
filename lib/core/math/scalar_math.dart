// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// scalar_math.dart — Pure scalar helpers used by the curve / NURBS engine.
//
// These functions are deliberately stateless and free of `vector_math`
// imports so they can be unit-tested in isolation and reused from
// anywhere in the engine without dragging in the heavier vector types.
//
// Provides:
//   - `factorial` (memoised, up to 20!).
//   - `binomial` (memoised Pascal triangle).
//   - `bernstein(i, n, t)` — i-th Bernstein basis polynomial of degree n.
//   - `horner(coeffs, t)` — Horner-scheme polynomial evaluation.
//   - `hornerDerivative(coeffs, t)` — first derivative via Horner.
//   - `trapezoidal` / `simpson` — closed Newton-Cotes quadrature.
//   - `gaussianWeight(x, sigma)` — Gaussian kernel value (unnormalised).
//   - `smootherstep` — Ken Perlin's 6t^5-15t^4+10t^3 C2 smoothstep.
//   - `wrapAngle` — fold an angle into [-pi, pi].
//
// v0.57-D runtime-wiring status (honest):
// ----------------------------------------
// PARTIALLY consumed via the core/math barrel (lib/core/math/math.dart):
//   - `gaussianWeight` is called by lib/engine/curves/stroke_smoother.dart
//     (stroke smoothing kernel).
//   - `smootherstep` is called by lib/engine/curves/curve_renderer.dart
//     (stroke tapering).
// Both consumers are in lib/engine/curves/ (OUT OF SCOPE for v0.57-D), so
// they were not touched — but they ARE live runtime callers, proving the
// barrel re-export + the two functions work end-to-end.
//
// v0.57-D NEW runtime consumer (in-scope): `wrapAngle` is now called by
// lib/screens/main_screen.dart's light-rig azimuth slider callback to fold
// the azimuth into [-pi, pi] (canonical serialisation + equality). This is
// the one natural in-scope consumer — the host owns the raw azimuth float.
//
// The remaining functions (factorial, binomial, bernstein, horner,
// hornerDerivative, simpson, trapezoidal, solveLinear, solveQuadratic) have
// NO in-scope consumer today. Their natural consumers are the curve/NURBS
// engine (lib/engine/curves/ — OUT OF SCOPE) + a future polynomial-fit UI.
// No inline duplicates of these functions exist in the in-scope files
// (main_screen.dart, krita_resources.dart), so there is nothing to replace.
// Left as a tested math library scaffold for the curves engine + future tasks.

import 'dart:math' as math;

/// Cache of `factorial(n)` for `n` in [0, 20]. 20! is the largest value
/// that fits exactly in a double; anything beyond saturates.
final List<double> _factorialCache = _buildFactorialCache(20);

List<double> _buildFactorialCache(int max) {
  final out = List<double>.filled(max + 1, 1.0);
  for (var i = 1; i <= max; i++) {
    out[i] = out[i - 1] * i;
  }
  return out;
}

/// Memoised factorial. Returns 1 for `n <= 0`. Saturates at `n > 20`
/// (returns 20! — callers in this codebase never ask for more).
double factorial(int n) {
  if (n <= 0) return 1.0;
  if (n >= _factorialCache.length) return _factorialCache.last;
  return _factorialCache[n];
}

/// Memoised binomial coefficient `C(n, k)` via Pascal's triangle.
/// Returns 0 for `k < 0` or `k > n`. Saturates for `n > 60`.
int binomial(int n, int k) {
  if (k < 0 || k > n) return 0;
  if (n == 0 || k == 0 || k == n) return 1;
  // Symmetry — pick the smaller side.
  if (k > n - k) k = n - k;
  var result = 1;
  for (var i = 0; i < k; i++) {
    result = result * (n - i) ~/ (i + 1);
  }
  return result;
}

/// The i-th Bernstein basis polynomial of degree [n] evaluated at [t].
///
/// `B_{i,n}(t) = C(n, i) * (1-t)^(n-i) * t^i`. Defined for t in [0,1]
/// but mathematically valid for any real t. Returns 0 if `i` is out of
/// `[0, n]`.
double bernstein(int i, int n, double t) {
  if (i < 0 || i > n) return 0.0;
  final c = binomial(n, i).toDouble();
  final u = 1.0 - t;
  return c * math.pow(u, n - i) * math.pow(t, i);
}

/// Horner-scheme evaluation of a polynomial with coefficients ordered
/// ascending: `p(t) = c0 + c1*t + c2*t^2 + ...`.
double horner(List<double> coeffs, double t) {
  if (coeffs.isEmpty) return 0.0;
  var r = coeffs.last;
  for (var i = coeffs.length - 2; i >= 0; i--) {
    r = r * t + coeffs[i];
  }
  return r;
}

/// First derivative of the polynomial `p(t) = sum(coeffs[i] * t^i)`,
/// evaluated at [t], also via Horner on the derived coefficients.
double hornerDerivative(List<double> coeffs, double t) {
  if (coeffs.length < 2) return 0.0;
  var r = coeffs.last * (coeffs.length - 1);
  for (var i = coeffs.length - 2; i >= 1; i--) {
    r = r * t + coeffs[i] * i;
  }
  return r;
}

/// Composite Simpson's 1/3 rule over [a, b] with [n] subintervals (n
/// must be even). [f] is sampled at n+1 points.
double simpson(double Function(double) f, double a, double b, {int n = 64}) {
  if (n.isOdd) n += 1;
  final h = (b - a) / n;
  var sum = f(a) + f(b);
  for (var i = 1; i < n; i++) {
    final x = a + i * h;
    sum += (i.isEven ? 2.0 : 4.0) * f(x);
  }
  return sum * h / 3.0;
}

/// Composite trapezoidal rule over [a, b] with [n] subintervals.
double trapezoidal(double Function(double) f, double a, double b, {int n = 64}) {
  final h = (b - a) / n;
  var sum = 0.5 * (f(a) + f(b));
  for (var i = 1; i < n; i++) {
    sum += f(a + i * h);
  }
  return sum * h;
}

/// Unnormalised Gaussian `exp(-x^2 / (2*sigma^2))`. The peak value at
/// x=0 is 1.0; the kernel integrates to `sigma * sqrt(2*pi)`. Callers
/// that need a probability density should divide by that factor.
double gaussianWeight(double x, double sigma) {
  final s = sigma == 0 ? 1e-12 : sigma;
  return math.exp(-(x * x) / (2.0 * s * s));
}

/// Ken Perlin's C2-continuous smoothstep: 6t^5 - 15t^4 + 10t^3.
/// Clamps [t] to [0,1] first. Useful for stroke tapering where a
/// simple `t*t*(3-2t)` shows a visible kink in derivative.
double smootherstep(double t) {
  if (t <= 0.0) return 0.0;
  if (t >= 1.0) return 1.0;
  return t * t * t * (t * (t * 6.0 - 15.0) + 10.0);
}

/// Folds an arbitrary angle (in radians) into the canonical `[-pi, pi]`
/// range. Used by twist / spiral strokes.
double wrapAngle(double a) {
  const twoPi = 2.0 * math.pi;
  var x = a % twoPi;
  if (x < -math.pi) x += twoPi;
  if (x > math.pi) x -= twoPi;
  return x;
}

/// Solves `a*x + b = 0` for x with a tiny-epsilon guard against a==0.
/// Returns `null` when the equation is degenerate.
double? solveLinear(double a, double b, {double epsilon = 1e-12}) {
  if (a.abs() < epsilon) return null;
  return -b / a;
}

/// Robust quadratic formula. Returns real roots in ascending order;
/// empty list if the discriminant is negative or the equation is
/// degenerate (a==0 and b==0).
List<double> solveQuadratic(double a, double b, double c,
    {double epsilon = 1e-12}) {
  if (a.abs() < epsilon) {
    final r = solveLinear(b, c, epsilon: epsilon);
    return r == null ? const <double>[] : <double>[r];
  }
  final disc = b * b - 4.0 * a * c;
  if (disc < 0.0) return const <double>[];
  final sq = math.sqrt(disc);
  // Numerically stable form (avoids catastrophic cancellation).
  final q = -0.5 * (b + (b >= 0 ? sq : -sq));
  final r1 = q / a;
  final r2 = c / q;
  return r1 < r2 ? <double>[r1, r2] : <double>[r2, r1];
}
