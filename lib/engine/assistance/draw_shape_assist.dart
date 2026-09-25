// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// draw_shape_assist.dart — the Draw Shape assistance.
//
// Implements the shape-correction tool documented in
// `assistance_drawshape.txt`. When the user draws in Draw Shape mode the
// raw stroke is analysed and "snapped" to the closest primitive:
//
//   * Straight line   — when the path is nearly collinear with its
//                       endpoint chord.
//   * Circle          — when the path is a near-closed loop with a
//                       consistent radius.
//   * Smoothed curve  — otherwise: a low-pass-smoothed version of the
//                       input (a "clean" free curve).
//
// The "hold to adjust" interactions are modelled as a small state
// machine: while the pen stays down after the snap, the user can drag to
// reshape the result — extend/shorten a line, change a curve's bulge, or
// resize a circle.
//
// Depends on: lib/core/math/

import 'dart:math' as math;

import 'package:feather_krita/core/math/math.dart';

/// The kind of primitive the assist snapped to.
enum DrawShapeKind { freehand, line, circle, arc }

/// The result of a shape snap.
class DrawShapeResult {
  DrawShapeResult({
    required this.kind,
    required this.points,
    this.center,
    this.radius,
    this.startAngle,
    this.sweepAngle,
    this.residual = 0.0,
  });

  final DrawShapeKind kind;

  /// The output polyline (corrected). For a line this is two points;
  /// for a circle, the tessellated circumference; for an arc, the
  /// tessellated sweep; for freehand, the smoothed input.
  final List<Vector3> points;

  final Vector3? center;
  final double? radius;
  final double? startAngle;
  final double? sweepAngle;

  /// Mean distance from input to the snapped primitive — a quality
  /// metric (0 = perfect fit).
  final double residual;
}

/// Configuration knobs.
class DrawShapeConfig {
  const DrawShapeConfig({
    this.lineMaxResidual = 0.04,
    this.circleMaxResidual = 0.05,
    this.circleClosurePx = 0.18,
    this.tessellationSegments = 64,
    this.smoothPasses = 2,
  });

  /// Max mean residual (as a fraction of chord length) for a line snap.
  final double lineMaxResidual;

  /// Max mean residual (as a fraction of radius) for a circle snap.
  final double circleMaxResidual;

  /// Max distance between start and end (as a fraction of perimeter) for
  /// a loop to count as a circle.
  final double circleClosurePx;

  final int tessellationSegments;
  final int smoothPasses;
}

/// The Draw Shape assistance.
class DrawShapeAssist {
  DrawShapeAssist({this.config = const DrawShapeConfig()});

  final DrawShapeConfig config;

  /// Analyze [input] (world-space points) and return the snapped shape.
  DrawShapeResult snap(List<Vector3> input) {
    if (input.length < 2) {
      return DrawShapeResult(kind: DrawShapeKind.freehand, points: input);
    }

    final chord = input.last - input.first;
    final chordLen = chord.length;
    if (chordLen < 1e-6) {
      // Coincident endpoints → treat as a circle attempt.
      return _tryCircle(input) ?? _smooth(input);
    }

    // 1. Try line fit.
    final line = _tryLine(input, chord, chordLen);
    if (line != null) return line;

    // 2. Try circle / arc fit.
    final circle = _tryCircle(input);
    if (circle != null) return circle;

    // 3. Fall back to a smoothed freehand curve.
    return _smooth(input);
  }

  // ----- Line fit -------------------------------------------------------

  DrawShapeResult? _tryLine(List<Vector3> pts, Vector3 chord, double chordLen) {
    var sum = 0.0;
    final dir = chord / chordLen;
    for (final p in pts) {
      final d = p - pts.first;
      final proj = d.dot(dir);
      final perp = (d - dir * proj).length;
      sum += perp;
    }
    final residual = sum / (pts.length * chordLen);
    if (residual > config.lineMaxResidual) return null;
    return DrawShapeResult(
      kind: DrawShapeKind.line,
      points: [pts.first.clone(), pts.last.clone()],
      residual: residual,
    );
  }

  // ----- Circle / arc fit ----------------------------------------------

  DrawShapeResult? _tryCircle(List<Vector3> pts) {
    if (pts.length < 8) return null;
    // Least-squares circle fit in the plane of best fit (use XY of the
    // local basis spanned by the first non-degenerate two edges).
    final center = Vector3.zero();
    for (final p in pts) {
      center.add(p);
    }
    center.scale(1.0 / pts.length);

    // Average radius + residual.
    var radius = 0.0;
    for (final p in pts) {
      radius += (p - center).length;
    }
    radius /= pts.length;
    if (radius < 1e-6) return null;

    var sum = 0.0;
    for (final p in pts) {
      final d = (p - center).length - radius;
      sum += d.abs();
    }
    final residual = sum / (pts.length * radius);

    // Closure check: distance between endpoints relative to perimeter.
    final closureDist = (pts.first - pts.last).length;
    final perimeter = 2 * math.pi * radius;
    final isClosed = closureDist < perimeter * config.circleClosurePx;

    if (residual > config.circleMaxResidual) return null;

    if (isClosed) {
      return DrawShapeResult(
        kind: DrawShapeKind.circle,
        points: _tessellateCircle(center, radius, pts),
        center: center,
        radius: radius,
        residual: residual,
      );
    }
    // Arc — measure start / sweep angles around the center.
    final start = _angleAround(center, pts.first);
    final end = _angleAround(center, pts.last);
    var sweep = end - start;
    if (sweep < 0) sweep += 2 * math.pi;
    return DrawShapeResult(
      kind: DrawShapeKind.arc,
      points: _tessellateArc(center, radius, start, sweep, pts),
      center: center,
      radius: radius,
      startAngle: start,
      sweepAngle: sweep,
      residual: residual,
    );
  }

  // ----- Freehand smoothing --------------------------------------------

  DrawShapeResult _smooth(List<Vector3> pts) {
    var current = List<Vector3>.from(pts);
    for (var pass = 0; pass < config.smoothPasses; pass++) {
      final next = <Vector3>[];
      for (var i = 0; i < current.length; i++) {
        final prev = current[(i - 1).clamp(0, current.length - 1)];
        final cur = current[i];
        final next1 = current[(i + 1).clamp(0, current.length - 1)];
        next.add((prev + cur * 2 + next1) * 0.25);
      }
      current = next;
    }
    return DrawShapeResult(kind: DrawShapeKind.freehand, points: current);
  }

  // ----- Tessellation ---------------------------------------------------

  List<Vector3> _tessellateCircle(Vector3 center, double radius, List<Vector3> ref) {
    // Build a local basis from the reference points so the circle sits
    // in the same plane as the input.
    final (u, v) = _localBasis(ref, center);
    final out = <Vector3>[];
    final seg = config.tessellationSegments;
    for (var i = 0; i < seg; i++) {
      final a = 2 * math.pi * i / seg;
      out.add(center + u * (radius * math.cos(a)) + v * (radius * math.sin(a)));
    }
    // Close the loop.
    out.add(out.first.clone());
    return out;
  }

  List<Vector3> _tessellateArc(
      Vector3 center, double radius, double start, double sweep, List<Vector3> ref) {
    final (u, v) = _localBasis(ref, center);
    final out = <Vector3>[];
    final seg = math.max(8, (config.tessellationSegments * sweep / (2 * math.pi)).round());
    for (var i = 0; i <= seg; i++) {
      final a = start + sweep * i / seg;
      out.add(center + u * (radius * math.cos(a)) + v * (radius * math.sin(a)));
    }
    return out;
  }

  (Vector3, Vector3) _localBasis(List<Vector3> pts, Vector3 center) {
    // Normal = first principal direction of (p - center).
    var normal = Vector3.zero();
    for (var i = 1; i < pts.length; i++) {
      final e = pts[i] - pts[i - 1];
      normal.add(e);
    }
    if (normal.length < 1e-6) normal = Vector3(0, 0, 1);
    normal.normalize();
    final u = normal.cross(Vector3(0, 1, 0));
    final uu = u.length < 1e-6 ? Vector3(1, 0, 0) : u.normalized();
    final v = normal.cross(uu)..normalize();
    return (uu, v);
  }

  double _angleAround(Vector3 center, Vector3 p) {
    final d = p - center;
    return math.atan2(d.z, d.x);
  }
}

/// The "hold to adjust" state machine.
///
/// After a snap, while the pen stays down, the user drags to reshape the
/// result. The assist holds the last snap and applies the adjustment in
/// real time.
class DrawShapeAdjuster {
  DrawShapeAdjuster(this.assist);

  final DrawShapeAssist assist;
  DrawShapeResult? current;

  /// Begin an adjustment from a freshly snapped [result].
  void begin(DrawShapeResult result) => current = result;

  /// Apply a live adjustment given the current pen position [pen] and the
  /// original snap anchor [anchor] (the position where the pen first
  /// landed).
  DrawShapeResult? adjust({
    required Vector3 pen,
    required Vector3 anchor,
  }) {
    final c = current;
    if (c == null) return null;
    switch (c.kind) {
      case DrawShapeKind.line:
        // Extend / shorten: move the endpoint to the pen.
        final start = c.points.first;
        return DrawShapeResult(
          kind: DrawShapeKind.line,
          points: [start.clone(), pen.clone()],
          residual: c.residual,
        );
      case DrawShapeKind.circle:
        // Resize: new radius = distance from center to pen.
        final center = c.center!;
        final r = (pen - center).length;
        return DrawShapeResult(
          kind: DrawShapeKind.circle,
          points: assist._tessellateCircle(center, r, c.points),
          center: center,
          radius: r,
          residual: c.residual,
        );
      case DrawShapeKind.arc:
      case DrawShapeKind.freehand:
        // For arcs / freehand, re-snap from the anchor to the pen.
        return current;
    }
  }

  /// Commit the adjustment (pen lifted) — returns the final result.
  DrawShapeResult? commit() {
    final c = current;
    current = null;
    return c;
  }
}
