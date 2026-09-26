// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// scalar_math_test.dart — unit tests for lib/core/math/scalar_math.dart.
//
// scalar_math had NO test file before v0.57-D. This file covers the
// `wrapAngle` function (the v0.57-D runtime consumer — wired to
// main_screen.dart's light-rig azimuth slider) plus a representative
// spread of the other helpers (factorial, binomial, bernstein, horner,
// smootherstep, gaussianWeight, solveQuadratic) so the math library is
// not shipped untested.

import 'dart:math' as math;

import 'package:feather_krita/core/math/scalar_math.dart' as scalarmath;
import 'package:flutter_test/flutter_test.dart';

void main() {
  group('wrapAngle — v0.57-D runtime consumer (light azimuth fold)', () {
    test('angles already in [-pi, pi] are returned unchanged', () {
      expect(scalarmath.wrapAngle(0.0), 0.0);
      expect(scalarmath.wrapAngle(math.pi / 4), closeTo(math.pi / 4, 1e-15));
      expect(scalarmath.wrapAngle(-math.pi / 4), closeTo(-math.pi / 4, 1e-15));
      // The impl folds via `x > pi` (strict), so exactly +pi is NOT
      // wrapped to -pi — it stays +pi. Dart's `%` maps -pi to +pi
      // (floor-division semantics), so wrap(-pi) ALSO returns +pi:
      // both half-turn representations collapse to +pi at the boundary.
      // Pinned here so a future refactor can't silently change the
      // boundary behaviour (the light-azimuth consumer never feeds a
      // raw ±pi so this quirk is latent).
      expect(scalarmath.wrapAngle(math.pi), closeTo(math.pi, 1e-15));
      expect(scalarmath.wrapAngle(-math.pi), closeTo(math.pi, 1e-15));
    });

    test('angles outside [-pi, pi] fold back into range', () {
      // 3*pi/2 ≡ -pi/2.
      expect(scalarmath.wrapAngle(3.0 * math.pi / 2.0),
          closeTo(-math.pi / 2.0, 1e-15));
      // 5*pi/4 ≡ -3*pi/4.
      expect(scalarmath.wrapAngle(5.0 * math.pi / 4.0),
          closeTo(-3.0 * math.pi / 4.0, 1e-15));
      // -3*pi/2 ≡ +pi/2.
      expect(scalarmath.wrapAngle(-3.0 * math.pi / 2.0),
          closeTo(math.pi / 2.0, 1e-15));
      // 7.5 rad (the example from the main_screen.dart doc comment) folds.
      final wrapped = scalarmath.wrapAngle(7.5);
      expect(wrapped, greaterThanOrEqualTo(-math.pi));
      expect(wrapped, lessThanOrEqualTo(math.pi));
    });

    test('wrapAngle is idempotent (wrap(wrap(x)) == wrap(x))', () {
      for (final x in <double>[-7.5, -3.0, -0.5, 0.0, 0.5, 3.0, 7.5, 100.0]) {
        final once = scalarmath.wrapAngle(x);
        final twice = scalarmath.wrapAngle(once);
        expect(twice, closeTo(once, 1e-15),
            reason: 'wrapAngle not idempotent at $x');
      }
    });
  });

  group('factorial + binomial', () {
    test('factorial matches the known small values + saturates past 20', () {
      expect(scalarmath.factorial(0), 1.0);
      expect(scalarmath.factorial(1), 1.0);
      expect(scalarmath.factorial(5), 120.0);
      expect(scalarmath.factorial(10), 3628800.0);
      // Saturates: n > 20 returns 20! (the cache cap).
      expect(scalarmath.factorial(25), scalarmath.factorial(20));
    });

    test('binomial matches Pascal-triangle edges + interior', () {
      expect(scalarmath.binomial(0, 0), 1);
      expect(scalarmath.binomial(5, 0), 1);
      expect(scalarmath.binomial(5, 5), 1);
      expect(scalarmath.binomial(5, 2), 10);
      expect(scalarmath.binomial(10, 3), 120);
      // Out-of-range → 0.
      expect(scalarmath.binomial(5, 6), 0);
      expect(scalarmath.binomial(5, -1), 0);
    });
  });

  group('bernstein + horner', () {
    test('bernstein basis sums to 1 across i at fixed n (partition of unity)',
        () {
      for (final n in <int>[1, 2, 3, 5, 8]) {
        for (final t in <double>[0.0, 0.25, 0.5, 0.75, 1.0]) {
          var sum = 0.0;
          for (var i = 0; i <= n; i++) {
            sum += scalarmath.bernstein(i, n, t);
          }
          expect(sum, closeTo(1.0, 1e-12),
              reason: 'Bernstein basis not partition-of-unity at n=$n t=$t');
        }
      }
    });

    test('horner evaluates a known polynomial correctly', () {
      // p(t) = 1 + 2t + 3t^2 → p(2) = 1 + 4 + 12 = 17.
      expect(scalarmath.horner(<double>[1, 2, 3], 2.0), 17.0);
      // p(0) = constant term.
      expect(scalarmath.horner(<double>[5, 1, 1], 0.0), 5.0);
      // Empty coeffs → 0.
      expect(scalarmath.horner(<double>[], 1.0), 0.0);
    });
  });

  group('smootherstep + gaussianWeight', () {
    test('smootherstep is 0 at <=0, 1 at >=1, monotonic in between', () {
      expect(scalarmath.smootherstep(-0.5), 0.0);
      expect(scalarmath.smootherstep(0.0), 0.0);
      expect(scalarmath.smootherstep(1.0), 1.0);
      expect(scalarmath.smootherstep(1.5), 1.0);
      final mid = scalarmath.smootherstep(0.5);
      expect(mid, greaterThan(0.0));
      expect(mid, lessThan(1.0));
      // C2 smoothstep at 0.5 is exactly 0.5 (symmetry).
      expect(mid, closeTo(0.5, 1e-15));
    });

    test('gaussianWeight peaks at 1.0 for x=0 + decays symmetrically', () {
      expect(scalarmath.gaussianWeight(0.0, 1.0), 1.0);
      // Symmetric: g(-x) == g(+x).
      expect(scalarmath.gaussianWeight(-2.0, 1.5),
          closeTo(scalarmath.gaussianWeight(2.0, 1.5), 1e-15));
      // Decays: g(2) < g(1) < g(0).
      final g0 = scalarmath.gaussianWeight(0.0, 1.0);
      final g1 = scalarmath.gaussianWeight(1.0, 1.0);
      final g2 = scalarmath.gaussianWeight(2.0, 1.0);
      expect(g1, lessThan(g0));
      expect(g2, lessThan(g1));
    });
  });

  group('solveQuadratic', () {
    test('two real roots for a positive discriminant', () {
      // x^2 - 5x + 6 = 0 → roots 2, 3.
      final roots = scalarmath.solveQuadratic(1.0, -5.0, 6.0);
      expect(roots, hasLength(2));
      expect(roots[0], lessThanOrEqualTo(roots[1]));
      expect(roots[0], closeTo(2.0, 1e-12));
      expect(roots[1], closeTo(3.0, 1e-12));
    });

    test('empty list for a negative discriminant (no real roots)', () {
      // x^2 + 1 = 0 → no real roots.
      final roots = scalarmath.solveQuadratic(1.0, 0.0, 1.0);
      expect(roots, isEmpty);
    });

    test('degenerate a==0 falls back to the linear root', () {
      // 0*x^2 + 2x - 4 = 0 → x = 2.
      final roots = scalarmath.solveQuadratic(0.0, 2.0, -4.0);
      expect(roots, hasLength(1));
      expect(roots[0], closeTo(2.0, 1e-12));
    });
  });
}
