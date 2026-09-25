// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// paint_extensions.dart — Flutter Paint/Canvas extensions for the
// software renderer.
//
// Provides convenience methods on Flutter's [Canvas] and [Paint] that
// bridge the pure-Dart rasterizer output into the standard Flutter
// 2D pipeline:
//
//   - [CanvasExtensions.drawImageCovered] — draw an [ui.Image] scaled
//     to fill a rect (the canonical "blit the framebuffer to screen"
//     step).
//   - [CanvasExtensions.drawTriangle3D] — debug helper that draws a
//     wireframe triangle from three NDC points. Used by the gizmo
//     overlay.
//   - [CanvasExtensions.drawMesh3D] — debug helper that draws an
//     entire mesh's wireframe.
//   - [CanvasExtensions.drawCurve3D] — draws a 3D Bézier / Catmull-Rom
//     curve as a series of line segments after projecting each control
//     point through a [Camera].
//   - [PaintExtensions.makeGlowPaint] — builds a [Paint] configured for
//     additive-blend glow (matches the [GlowShader] look).
//   - [PaintExtensions.makeShadedPaint] — builds a [Paint] configured
//     for the shaded (lit) look.
//
// These extensions are the seam between the 3D engine and Flutter's 2D
// `CustomPainter` API. They are intentionally side-effect free aside
// from the obvious Canvas mutations.

import 'dart:math' as math;
import 'dart:ui' as ui;

import 'package:vector_math/vector_math_64.dart';

import 'vertex_processor.dart';

/// Extensions on Flutter's [Canvas] for the 3D engine.
extension CanvasExtensions on ui.Canvas {
  /// Draws [image] scaled to fill [rect], preserving aspect ratio by
  /// cropping (cover semantics). Used by the renderer's present step
  /// to blit the framebuffer onto the visible canvas area.
  void drawImageCovered(ui.Image image, ui.Rect rect, ui.Paint paint) {
    final srcW = image.width.toDouble();
    final srcH = image.height.toDouble();
    final dstW = rect.width;
    final dstH = rect.height;
    final scaleX = dstW / srcW;
    final scaleY = dstH / srcH;
    final scale = math.max(scaleX, scaleY);
    final scaledW = srcW * scale;
    final scaledH = srcH * scale;
    final dx = rect.left + (dstW - scaledW) * 0.5;
    final dy = rect.top + (dstH - scaledH) * 0.5;
    final src = ui.Rect.fromLTWH(0, 0, srcW, srcH);
    final dst = ui.Rect.fromLTWH(dx, dy, scaledW, scaledH);
    drawImageRect(image, src, dst, paint);
  }

  /// Draws a wireframe triangle from three screen-space points. Used
  /// by the gizmo overlay and the wireframe material debug view.
  void drawTriangle3D(
    ui.Offset a,
    ui.Offset b,
    ui.Offset c,
    ui.Paint paint,
  ) {
    final path = ui.Path()
      ..moveTo(a.dx, a.dy)
      ..lineTo(b.dx, b.dy)
      ..lineTo(c.dx, c.dy)
      ..close();
    drawPath(path, paint);
  }

  /// Draws a 3D mesh as a wireframe. [positions] are object-space
  /// vertex positions, [indices] is a flat triangle index buffer,
  /// [model] is the object-to-world transform, [camera] provides the
  /// view-projection. Only vertices that survive the near-plane clip
  /// are drawn (others are skipped silently).
  void drawMesh3D(
    List<Vector3> positions,
    List<int> indices,
    Matrix4 model,
    Camera camera,
    ui.Paint paint,
  ) {
    final vp = camera.viewProjection;
    final mvp = vp * model;
    final halfW = camera.viewportWidth * 0.5;
    final halfH = camera.viewportHeight * 0.5;
    final screen = List<ui.Offset?>.filled(positions.length, null);
    for (var i = 0; i < positions.length; i++) {
      final p = positions[i];
      final clip = mvp * Vector4(p.x, p.y, p.z, 1.0);
      if (clip.w <= 1e-5) {
        screen[i] = null;
        continue;
      }
      final invW = 1.0 / clip.w;
      final sx = halfW + clip.x * invW * halfW;
      final sy = halfH - clip.y * invW * halfH;
      screen[i] = ui.Offset(sx, sy);
    }
    final path = ui.Path();
    for (var i = 0; i < indices.length; i += 3) {
      final a = screen[indices[i]];
      final b = screen[indices[i + 1]];
      final c = screen[indices[i + 2]];
      if (a == null || b == null || c == null) continue;
      path
        ..moveTo(a.dx, a.dy)
        ..lineTo(b.dx, b.dy)
        ..lineTo(c.dx, c.dy)
        ..close();
    }
    drawPath(path, paint);
  }

  /// Draws a 3D curve as a series of line segments. [points] are
  /// world-space control points; [camera] projects them to screen
  /// space. Points behind the near plane are skipped (the line is
  /// broken at that point — no perspective-correct clipping is done
  /// here; for that, use the rasterizer's curve primitive).
  void drawCurve3D(
    List<Vector3> points,
    Camera camera,
    ui.Paint paint, {
    bool closed = false,
  }) {
    if (points.isEmpty) return;
    final vp = camera.viewProjection;
    final halfW = camera.viewportWidth * 0.5;
    final halfH = camera.viewportHeight * 0.5;
    final path = ui.Path();
    var started = false;
    for (final p in points) {
      final clip = vp * Vector4(p.x, p.y, p.z, 1.0);
      if (clip.w <= 1e-5) {
        started = false;
        continue;
      }
      final invW = 1.0 / clip.w;
      final sx = halfW + clip.x * invW * halfW;
      final sy = halfH - clip.y * invW * halfH;
      if (!started) {
        path.moveTo(sx, sy);
        started = true;
      } else {
        path.lineTo(sx, sy);
      }
    }
    if (closed && started) path.close();
    drawPath(path, paint);
  }

  /// Draws a 3D Bézier curve of arbitrary degree using the de Casteljau
  /// algorithm at [segments] subdivisions. Cheaper than rasterizing
  /// a triangle strip and good enough for stroke-preview overlays.
  void drawBezier3D(
    List<Vector3> controlPoints,
    Camera camera,
    ui.Paint paint, {
    int segments = 32,
  }) {
    if (controlPoints.length < 2) return;
    final sampled = <Vector3>[];
    for (var i = 0; i <= segments; i++) {
      final t = i / segments;
      sampled.add(_deCasteljau(controlPoints, t));
    }
    drawCurve3D(sampled, camera, paint);
  }

  static Vector3 _deCasteljau(List<Vector3> pts, double t) {
    if (pts.length == 1) return pts.first.clone();
    final next = <Vector3>[];
    for (var i = 0; i < pts.length - 1; i++) {
      next.add(pts[i] + (pts[i + 1] - pts[i]) * t);
    }
    return _deCasteljau(next, t);
  }
}

/// Extensions on Flutter's [Paint] for the 3D engine.
extension PaintExtensions on ui.Paint {
  /// Returns a [Paint] configured for additive-blend glow, matching
  /// the [GlowShader] look.
  static ui.Paint makeGlowPaint({
    int color = 0xffffffff,
    double strokeWidth = 2.0,
    ui.BlendMode blendMode = ui.BlendMode.plus,
  }) {
    return ui.Paint()
      ..color = ui.Color(color)
      ..style = ui.PaintingStyle.stroke
      ..strokeWidth = strokeWidth
      ..strokeCap = ui.StrokeCap.round
      ..strokeJoin = ui.StrokeJoin.round
      ..blendMode = blendMode
      ..isAntiAlias = true;
  }

  /// Returns a [Paint] configured for the shaded (lit) look — opaque
  /// stroke with rounded joins.
  static ui.Paint makeShadedPaint({
    int color = 0xff8a8a90,
    double strokeWidth = 1.0,
  }) {
    return ui.Paint()
      ..color = ui.Color(color)
      ..style = ui.PaintingStyle.fill
      ..strokeWidth = strokeWidth
      ..strokeCap = ui.StrokeCap.round
      ..strokeJoin = ui.StrokeJoin.round
      ..blendMode = ui.BlendMode.srcOver
      ..isAntiAlias = true;
  }

  /// Returns a [Paint] configured for wireframe debug rendering.
  static ui.Paint makeWireframePaint({
    int color = 0xff00ff88,
    double strokeWidth = 0.5,
  }) {
    return ui.Paint()
      ..color = ui.Color(color)
      ..style = ui.PaintingStyle.stroke
      ..strokeWidth = strokeWidth
      ..strokeJoin = ui.StrokeJoin.miter
      ..blendMode = ui.BlendMode.srcOver
      ..isAntiAlias = true;
  }

  /// Returns a [Paint] configured for blitting a framebuffer image to
  /// the canvas. Uses nearest-neighbor filtering by default — the
  /// framebuffer is already at the canvas's resolution so we don't
  /// want any extra smoothing.
  static ui.Paint makeBlitPaint({bool smooth = false}) {
    return ui.Paint()
      ..filterQuality = smooth ? ui.FilterQuality.low : ui.FilterQuality.none
      ..blendMode = ui.BlendMode.srcOver
      ..isAntiAlias = false;
  }
}
