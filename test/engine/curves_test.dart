// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// curves_test.dart — unit tests for Curve3D implementations and stroke
// smoothing.

import 'dart:math' as math;

import 'package:feather_krita/core/math/math.dart';
import 'package:feather_krita/engine/curves/bezier_curve3d.dart';
import 'package:feather_krita/engine/curves/catmull_rom_curve3d.dart';
import 'package:feather_krita/engine/curves/curve3d.dart';
import 'package:feather_krita/engine/curves/nurbs_curve3d.dart';
import 'package:feather_krita/engine/curves/stroke3d.dart';
import 'package:feather_krita/engine/curves/stroke_smoother.dart';
import 'package:flutter_test/flutter_test.dart';

void expectVec(Vector3 a, Vector3 b, {double tol = 1e-6}) {
  expect(a.x, closeTo(b.x, tol));
  expect(a.y, closeTo(b.y, tol));
  expect(a.z, closeTo(b.z, tol));
}

void main() {
  group('BezierCurve3D', () {
    test('evaluateLocal interpolates the endpoints at t=0 and t=1', () {
      final c = BezierCurve3D.cubic(
        Vector3(0, 0, 0),
        Vector3(1, 1, 0),
        Vector3(2, 1, 0),
        Vector3(3, 0, 0),
      );
      expectVec(c.evaluateLocal(0), Vector3(0, 0, 0));
      expectVec(c.evaluateLocal(1), Vector3(3, 0, 0));
    });

    test('derivative at endpoints follows the control polygon', () {
      final c = BezierCurve3D.cubic(
        Vector3(0, 0, 0),
        Vector3(1, 1, 0),
        Vector3(2, 1, 0),
        Vector3(3, 0, 0),
      );
      // C'(0) = 3 * (P1 - P0).
      final d0 = c.derivativeLocal(0);
      expectVec(d0, Vector3(3, 3, 0), tol: 1e-6);
      // C'(1) = 3 * (P3 - P2).
      final d1 = c.derivativeLocal(1);
      expectVec(d1, Vector3(3, -3, 0), tol: 1e-6);
    });

    test('split produces two halves whose join is the original', () {
      final c = BezierCurve3D.cubic(
        Vector3(0, 0, 0),
        Vector3(0, 2, 0),
        Vector3(2, 2, 0),
        Vector3(2, 0, 0),
      );
      final (left, right) = c.split(0.5);
      // Left half at t=1 == right half at t=0 == original at t=0.5.
      final jointL = left.evaluateLocal(1);
      final jointR = right.evaluateLocal(0);
      final mid = c.evaluateLocal(0.5);
      expectVec(jointL, mid, tol: 1e-6);
      expectVec(jointR, mid, tol: 1e-6);
    });

    test('split halves share endpoints with the original curve', () {
      final c = BezierCurve3D.cubic(
        Vector3(0, 0, 0),
        Vector3(1, 1, 0),
        Vector3(2, -1, 0),
        Vector3(3, 0, 0),
      );
      final (left, right) = c.split(0.5);
      expectVec(left.evaluateLocal(0), c.evaluateLocal(0), tol: 1e-6);
      expectVec(right.evaluateLocal(1), c.evaluateLocal(1), tol: 1e-6);
    });

    test('reverse traces the curve backwards', () {
      final c = BezierCurve3D.cubic(
        Vector3(0, 0, 0),
        Vector3(1, 1, 0),
        Vector3(2, 1, 0),
        Vector3(3, 0, 0),
      );
      final r = c.reverse();
      expectVec(r.evaluateLocal(0), c.evaluateLocal(1), tol: 1e-6);
      expectVec(r.evaluateLocal(1), c.evaluateLocal(0), tol: 1e-6);
    });

    test('degree elevation preserves the curve shape', () {
      final quad = BezierCurve3D.quadratic(
        Vector3(0, 0, 0),
        Vector3(1, 2, 0),
        Vector3(2, 0, 0),
      );
      final cubic = quad.elevateDegree();
      expect(cubic.degree, 3);
      expectVec(cubic.evaluateLocal(0), quad.evaluateLocal(0), tol: 1e-6);
      expectVec(cubic.evaluateLocal(0.5), quad.evaluateLocal(0.5), tol: 1e-6);
      expectVec(cubic.evaluateLocal(1), quad.evaluateLocal(1), tol: 1e-6);
    });

    test('flatten emits a polyline within the requested tolerance', () {
      final c = BezierCurve3D.cubic(
        Vector3(0, 0, 0),
        Vector3(0, 1, 0),
        Vector3(1, 1, 0),
        Vector3(1, 0, 0),
      );
      final flat = c.flatten(tolerance: 1e-3);
      expect(flat.length, greaterThanOrEqualTo(2));
      expectVec(flat.first.point, Vector3(0, 0, 0));
      expectVec(flat.last.point, Vector3(1, 0, 0));
    });
  });

  group('CatmullRomCurve3D', () {
    test('domain spans the segment count for open splines', () {
      final c = CatmullRomCurve3D(controlPoints: [
        Vector3(0, 0, 0),
        Vector3(1, 1, 0),
        Vector3(2, 0, 0),
        Vector3(3, 1, 0),
      ]);
      final (tMin, tMax) = c.domain();
      expect(tMin, 0);
      expect(tMax, 3);
    });

    test('closed spline domain wraps the full control point count', () {
      final c = CatmullRomCurve3D(
        controlPoints: [
          Vector3(1, 0, 0),
          Vector3(0, 1, 0),
          Vector3(-1, 0, 0),
          Vector3(0, -1, 0),
        ],
        closed: true,
      );
      final (tMin, tMax) = c.domain();
      expect(tMin, 0);
      expect(tMax, 4);
    });

    test('spline output is bounded by the control point range', () {
      final pts = [
        Vector3(0, 0, 0),
        Vector3(1, 2, 0),
        Vector3(2, 0, 0),
        Vector3(3, 2, 0),
      ];
      final c = CatmullRomCurve3D(controlPoints: pts);
      final (tMin, tMax) = c.domain();
      var minY = double.infinity;
      var maxY = double.negativeInfinity;
      for (var i = 0; i <= 32; i++) {
        final t = tMin + (tMax - tMin) * i / 32;
        final p = c.evaluateLocal(t);
        minY = math.min(minY, p.y);
        maxY = math.max(maxY, p.y);
      }
      // The spline should sit within or close to the y range of the
      // control polygon (which is [0, 2]). Catmull-Rom can overshoot
      // slightly; allow 1.0 unit of slack.
      expect(minY, greaterThan(-1.0));
      expect(maxY, lessThan(3.0));
    });

    test('toBezierSegments produces one cubic per span', () {
      final pts = [
        Vector3(0, 0, 0),
        Vector3(1, 1, 0),
        Vector3(2, 0, 0),
        Vector3(3, 1, 0),
      ];
      final c = CatmullRomCurve3D(controlPoints: pts);
      final segs = c.toBezierSegments();
      expect(segs.length, pts.length - 1);
      // Each segment must start where the previous ended.
      for (var i = 1; i < segs.length; i++) {
        expectVec(segs[i].evaluateLocal(0),
            segs[i - 1].evaluateLocal(1), tol: 1e-6);
      }
    });

    test('simplify reduces or maintains the control point count', () {
      final pts = [
        Vector3(0, 0, 0),
        Vector3(1, 0.001, 0),
        Vector3(2, 0, 0),
        Vector3(3, 0.001, 0),
        Vector3(4, 0, 0),
      ];
      final c = CatmullRomCurve3D(controlPoints: pts);
      final s = c.simplify(0.01);
      expect(s.controlPoints.length, lessThanOrEqualTo(pts.length));
    });

    test('tension is clamped to [0, 1]', () {
      final c = CatmullRomCurve3D(
        controlPoints: [Vector3.zero(), Vector3(1, 0, 0)],
        tension: 5.0,
      );
      expect(c.tension, 1.0);
    });
  });

  group('NurbsCurve3D', () {
    test('interpolating factory honours the control points at the ends', () {
      final pts = [
        Vector3(0, 0, 0),
        Vector3(1, 1, 0),
        Vector3(2, 0, 0),
        Vector3(3, 1, 0),
      ];
      final n = NurbsCurve3D.interpolating(pts, degree: 3);
      final (tMin, tMax) = n.domain();
      expectVec(n.evaluateLocal(tMin), pts.first, tol: 1e-6);
      expectVec(n.evaluateLocal(tMax - 1e-9), pts.last, tol: 1e-6);
    });

    test('a clamped NURBS with weights=1 interpolates control polygon', () {
      // A degree-1 clamped NURBS interpolates the control polygon.
      final pts = [
        Vector3(0, 0, 0),
        Vector3(1, 0, 0),
        Vector3(1, 1, 0),
      ];
      final n = NurbsCurve3D.interpolating(pts, degree: 1);
      // At u=0.5 (within the first knot span, which goes from
      // knots[1]=0 to knots[2]=1), the linear B-spline gives the
      // midpoint of p0 and p1.
      final mid = n.evaluateLocal(0.5);
      expect(mid.x, closeTo(0.5, 1e-6));
      expect(mid.y, closeTo(0.0, 1e-6));
    });

    test('insertKnot preserves the shape exactly', () {
      final pts = [
        Vector3(0, 0, 0),
        Vector3(1, 2, 0),
        Vector3(2, 2, 0),
        Vector3(3, 0, 0),
      ];
      final n = NurbsCurve3D.interpolating(pts, degree: 3);
      final (tMin, tMax) = n.domain();
      final samplePoint = tMin + (tMax - tMin) * 0.37;
      final before = n.evaluateLocal(samplePoint);
      final refined = n.insertKnot(samplePoint, times: 1);
      final after = refined.evaluateLocal(samplePoint);
      expectVec(before, after, tol: 1e-6);
      expect(refined.positions.length, greaterThan(n.positions.length));
    });

    test('reverse swaps the endpoints', () {
      final pts = [
        Vector3(0, 0, 0),
        Vector3(1, 1, 0),
        Vector3(2, 0, 0),
        Vector3(3, 1, 0),
      ];
      final n = NurbsCurve3D.interpolating(pts, degree: 3);
      final r = n.reverse();
      final (tMin, tMax) = n.domain();
      final (rMin, rMax) = r.domain();
      expectVec(r.evaluateLocal(rMin), n.evaluateLocal(tMax - 1e-9),
          tol: 1e-6);
      expectVec(r.evaluateLocal(rMax - 1e-9), n.evaluateLocal(tMin),
          tol: 1e-6);
    });

    test('copy produces an equivalent curve', () {
      final pts = [
        Vector3(0, 0, 0),
        Vector3(1, 1, 0),
        Vector3(2, 0, 0),
        Vector3(3, 1, 0),
      ];
      final n = NurbsCurve3D.interpolating(pts, degree: 3);
      final c = n.copy();
      expect(c.positions.length, n.positions.length);
      expect(c.knots.length, n.knots.length);
      expect(c.degree, n.degree);
    });
  });

  group('Stroke3D', () {
    test('polylineLength sums segment distances in world space', () {
      final s = Stroke3D(
        samples: [
          StrokeSample3D(position: Vector3(0, 0, 0), time: 0),
          StrokeSample3D(position: Vector3(3, 4, 0), time: 0.1),
          StrokeSample3D(position: Vector3(3, 4, 12), time: 0.2),
        ],
      );
      // 3-4-5 triangle = 5; then vertical 12 = 12. Total = 17.
      expect(s.polylineLength(), closeTo(17, 1e-6));
    });

    test('toCatmullRom carries the sample positions as control points', () {
      final pts = [
        Vector3(0, 0, 0),
        Vector3(1, 1, 0),
        Vector3(2, 0, 0),
        Vector3(3, 1, 0),
      ];
      final s = Stroke3D(
        samples: pts
            .map((p) => StrokeSample3D(position: p.clone()))
            .toList(),
      );
      final c = s.toCatmullRom();
      expect(c.controlPoints.length, pts.length);
      for (var i = 0; i < pts.length; i++) {
        expectVec(c.controlPoints[i], pts[i], tol: 1e-6);
      }
    });

    test('toBezier returns a curve with positive length', () {
      final s = Stroke3D(
        samples: [
          StrokeSample3D(position: Vector3(0, 0, 0)),
          StrokeSample3D(position: Vector3(1, 1, 0)),
          StrokeSample3D(position: Vector3(2, 0, 0)),
          StrokeSample3D(position: Vector3(3, 1, 0)),
        ],
      );
      final b = s.toBezier();
      expect(b.length(), greaterThan(0));
    });
  });

  group('StrokeSmoother', () {
    List<StrokeSample3D> _zigzag() => [
          StrokeSample3D(position: Vector3(0, 0, 0)),
          StrokeSample3D(position: Vector3(1, 10, 0)),
          StrokeSample3D(position: Vector3(2, 0, 0)),
          StrokeSample3D(position: Vector3(3, 10, 0)),
          StrokeSample3D(position: Vector3(4, 0, 0)),
        ];

    test('movingAverage reduces the amplitude of high-frequency jitter', () {
      final samples = _zigzag();
      final smoothed = StrokeSmoother.smooth(
        samples,
        strength: 1.0,
        algorithm: StrokeSmoothingAlgorithm.movingAverage,
      );
      // The peaks (samples[1] and samples[3]) must be lower after
      // smoothing.
      expect(smoothed[1].position.y, lessThan(samples[1].position.y));
      expect(smoothed[3].position.y, lessThan(samples[3].position.y));
    });

    test('gaussian smoothing preserves the sample count', () {
      final samples = _zigzag();
      final smoothed = StrokeSmoother.smooth(
        samples,
        strength: 0.7,
        algorithm: StrokeSmoothingAlgorithm.gaussian,
      );
      expect(smoothed.length, samples.length);
    });

    test('stableStrokes uses only past samples (causal)', () {
      final samples = _zigzag();
      final smoothed = StrokeSmoother.smooth(
        samples,
        strength: 0.5,
        algorithm: StrokeSmoothingAlgorithm.stableStrokes,
      );
      // The first sample is preserved (no past to smooth against).
      expectVec(smoothed.first.position, samples.first.position,
          tol: 1e-9);
      // Subsequent samples are an exponential blend of the previous
      // smoothed position and the current sample — so the smoothed
      // value always lies between the two.
      for (var i = 1; i < samples.length; i++) {
        final lo = math.min(smoothed[i - 1].position.y, samples[i].position.y);
        final hi = math.max(smoothed[i - 1].position.y, samples[i].position.y);
        expect(smoothed[i].position.y, greaterThanOrEqualTo(lo - 1e-6));
        expect(smoothed[i].position.y, lessThanOrEqualTo(hi + 1e-6));
      }
    });

    test('catmullRomFit smooths without changing sample count', () {
      final samples = _zigzag();
      final smoothed = StrokeSmoother.smooth(
        samples,
        strength: 0.5,
        algorithm: StrokeSmoothingAlgorithm.catmullRomFit,
      );
      expect(smoothed.length, samples.length);
    });

    test('strength = 0 leaves the input untouched', () {
      final samples = _zigzag();
      final smoothed = StrokeSmoother.smooth(
        samples,
        strength: 0.0,
        algorithm: StrokeSmoothingAlgorithm.gaussian,
      );
      for (var i = 0; i < samples.length; i++) {
        expectVec(smoothed[i].position, samples[i].position, tol: 1e-9);
      }
    });

    test('radiusFor maps 0..1 onto 0..maxRadius', () {
      expect(StrokeSmoother.radiusFor(0), 0);
      expect(StrokeSmoother.radiusFor(1), StrokeSmoother.maxRadius);
      expect(StrokeSmoother.radiusFor(0.5),
          closeTo(StrokeSmoother.maxRadius / 2, 1e-9));
    });

    test('empty input returns empty output', () {
      final out = StrokeSmoother.smooth(
        <StrokeSample3D>[],
        strength: 0.5,
      );
      expect(out, isEmpty);
    });
  });

  group('Curve3D base-class helpers', () {
    test('length() and boundingBox() are non-negative', () {
      final c = BezierCurve3D.cubic(
        Vector3(0, 0, 0),
        Vector3(0, 1, 0),
        Vector3(1, 1, 0),
        Vector3(1, 0, 0),
      );
      expect(c.length(), greaterThan(0));
      final aabb = c.boundingBox();
      expect(aabb.min.x, lessThanOrEqualTo(0));
      expect(aabb.max.x, greaterThanOrEqualTo(1));
    });

    test('closestPoint finds the endpoint near an off-curve probe', () {
      final c = BezierCurve3D.cubic(
        Vector3(0, 0, 0),
        Vector3(0, 0, 0),
        Vector3(1, 0, 0),
        Vector3(1, 0, 0),
      );
      // A straight line from (0,0,0) to (1,0,0). Probe near (1.2, 0, 0).
      final result = c.closestPoint(Vector3(1.2, 0, 0), resolution: 32);
      expect(result.t, closeTo(1.0, 1e-2));
      expect(result.point.x, closeTo(1.0, 1e-2));
    });

    test('applyTransform composes a translation into the curve', () {
      final c = BezierCurve3D.cubic(
        Vector3(0, 0, 0),
        Vector3(1, 1, 0),
        Vector3(2, 1, 0),
        Vector3(3, 0, 0),
      );
      final before = c.sampleAt(0);
      c.applyTransform(Matrix4.identity()..setTranslation(Vector3(10, 0, 0)));
      final after = c.sampleAt(0);
      expectVec(after, before + Vector3(10, 0, 0), tol: 1e-6);
    });
  });
}
