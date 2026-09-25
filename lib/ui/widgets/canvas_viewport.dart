// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// canvas_viewport.dart — 3D canvas viewport.
//
// The viewport is the heart of the editor: it owns the gesture surface,
// routes pen/touch events to the engine, and renders the scene via a
// CustomPainter. It draws:
//   * the paper-textured canvas (noise 4%),
//   * a faint floor grid + center crosshair,
//   * the active group's bounding box,
//   * live stroke preview while drawing,
//   * the joystick + crosshair overlay (driven by the parent).
//
// The real stroke / scene data lives in lib/engine; this widget only
// renders what it's given through [CanvasScene]. We don't import the
// engine to keep the UI package hermetic — the parent editor screen
// adapts the engine state into [CanvasScene].

import 'dart:math' as math;
import 'dart:ui' as ui;

import 'package:flutter/material.dart';

import '../theme/feather_colors.dart';

class CanvasStroke {
  const CanvasStroke({
    required this.points,
    required this.color,
    this.width = 4,
    this.tapered = true,
  });

  final List<Offset> points;
  final Color color;
  final double width;
  final bool tapered;
}

class CanvasScene {
  const CanvasScene({
    this.strokes = const [],
    this.activeColor = const Color(0xFF60A5FA),
    this.previewStroke,
    this.selectionBounds,
    this.zoom = 1.0,
    this.pan = Offset.zero,
    this.showGrid = true,
    this.renderMode = false,
  });

  final List<CanvasStroke> strokes;
  final Color activeColor;
  final CanvasStroke? previewStroke;
  final Rect? selectionBounds;
  final double zoom;
  final Offset pan;
  final bool showGrid;
  final bool renderMode;
}

class CanvasViewport extends StatefulWidget {
  const CanvasViewport({
    super.key,
    required this.scene,
    this.onStrokeStart,
    this.onStrokeUpdate,
    this.onStrokeEnd,
    this.onPan,
    this.onZoom,
    this.paperSeed = 7,
  });

  final CanvasScene scene;
  final VoidCallback? onStrokeStart;
  final ValueChanged<Offset>? onStrokeUpdate;
  final VoidCallback? onStrokeEnd;
  final ValueChanged<Offset>? onPan;
  final ValueChanged<double>? onZoom;
  final int paperSeed;

  @override
  State<CanvasViewport> createState() => _CanvasViewportState();
}

class _CanvasViewportState extends State<CanvasViewport> {
  // Track the live stroke's local points so we can paint the preview
  // immediately even before the parent has rebuilt with a new scene.
  final List<Offset> _livePoints = <Offset>[];
  bool _isDrawing = false;
  bool _isPanning = false;
  Offset? _lastPan;

  @override
  Widget build(BuildContext context) {
    final palette = _palette(context);
    return RepaintBoundary(
      child: GestureDetector(
        behavior: HitTestBehavior.opaque,
        onPanStart: (d) {
          // Two-finger / right-button pans; one-finger draws. We can't
          // distinguish at GestureDetector level without custom logic,
          // so we expose both: if onStrokeStart is null, treat as pan.
          if (widget.onStrokeStart != null) {
            _isDrawing = true;
            _livePoints
              ..clear()
              ..add(d.localPosition);
            widget.onStrokeStart?.call();
            setState(() {});
          } else {
            _isPanning = true;
            _lastPan = d.localPosition;
          }
        },
        onPanUpdate: (d) {
          if (_isDrawing) {
            _livePoints.add(d.localPosition);
            widget.onStrokeUpdate?.call(d.localPosition);
            setState(() {});
          } else if (_isPanning) {
            final delta = d.localPosition - (_lastPan ?? d.localPosition);
            _lastPan = d.localPosition;
            widget.onPan?.call(delta);
          }
        },
        onPanEnd: (_) {
          if (_isDrawing) {
            _isDrawing = false;
            _livePoints.clear();
            widget.onStrokeEnd?.call();
            setState(() {});
          } else if (_isPanning) {
            _isPanning = false;
            _lastPan = null;
          }
        },
        onScaleStart: (d) {
          _isPanning = true;
          _lastPan = d.focalPoint;
        },
        onScaleUpdate: (d) {
          final delta = d.focalPoint - (_lastPan ?? d.focalPoint);
          _lastPan = d.focalPoint;
          widget.onPan?.call(delta);
          if ((d.scale - 1.0).abs() > 0.001) {
            widget.onZoom?.call(d.scale);
          }
        },
        onScaleEnd: (_) {
          _isPanning = false;
          _lastPan = null;
        },
        child: ClipRect(
          child: CustomPaint(
            size: Size.infinite,
            painter: _CanvasPainter(
              scene: widget.scene,
              livePoints: _livePoints,
              isDrawing: _isDrawing,
              palette: palette,
              paperSeed: widget.paperSeed,
            ),
          ),
        ),
      ),
    );
  }

  FeatherPalette _palette(BuildContext context) =>
      Theme.of(context).brightness == Brightness.dark
          ? FeatherColors.dark
          : FeatherColors.light;
}

class _CanvasPainter extends CustomPainter {
  _CanvasPainter({
    required this.scene,
    required this.livePoints,
    required this.isDrawing,
    required this.palette,
    required this.paperSeed,
  });

  final CanvasScene scene;
  final List<Offset> livePoints;
  final bool isDrawing;
  final FeatherPalette palette;
  final int paperSeed;

  @override
  void paint(Canvas canvas, Size size) {
    // 1. Background — paper.
    final bgPaint = Paint()..color = palette.canvasBackground;
    canvas.drawRect(Offset.zero & size, bgPaint);
    _paintPaperNoise(canvas, size);

    // 2. Floor grid (perspective-ish, but kept simple).
    if (scene.showGrid) {
      _paintGrid(canvas, size);
    }

    // 3. Center crosshair.
    final cx = size.width / 2 + scene.pan.dx;
    final cy = size.height / 2 + scene.pan.dy;
    final crossPaint = Paint()
      ..color = palette.textPrimary.withValues(alpha: 0.4)
      ..strokeWidth = 1;
    canvas.drawLine(Offset(cx - 10, cy), Offset(cx + 10, cy), crossPaint);
    canvas.drawLine(Offset(cx, cy - 10), Offset(cx, cy + 10), crossPaint);

    // 4. Strokes.
    canvas.save();
    canvas.translate(scene.pan.dx, scene.pan.dy);
    canvas.scale(scene.zoom);
    for (final s in scene.strokes) {
      _paintStroke(canvas, s);
    }
    // 5. Live preview.
    if (isDrawing && livePoints.length >= 2) {
      _paintStroke(
        canvas,
        CanvasStroke(
          points: livePoints,
          color: scene.activeColor,
          width: 4,
        ),
      );
    } else if (scene.previewStroke != null) {
      _paintStroke(canvas, scene.previewStroke!);
    }
    canvas.restore();

    // 6. Selection bounds.
    if (scene.selectionBounds != null) {
      _paintSelection(canvas, scene.selectionBounds!);
    }
  }

  void _paintStroke(Canvas canvas, CanvasStroke stroke) {
    if (stroke.points.length < 2) return;
    final paint = Paint()
      ..color = stroke.color
      ..style = PaintingStyle.stroke
      ..strokeWidth = stroke.width
      ..strokeCap = StrokeCap.round
      ..strokeJoin = StrokeJoin.round;
    final path = Path()..moveTo(stroke.points.first.dx, stroke.points.first.dy);
    for (final p in stroke.points.skip(1)) {
      path.lineTo(p.dx, p.dy);
    }
    canvas.drawPath(path, paint);
    if (stroke.tapered) {
      final thin = Paint()
        ..color = stroke.color.withValues(alpha: 0.6)
        ..style = PaintingStyle.stroke
        ..strokeWidth = stroke.width * 0.5
        ..strokeCap = StrokeCap.round;
      canvas.drawPath(path, thin);
    }
  }

  void _paintGrid(Canvas canvas, Size size) {
    final cx = size.width / 2 + scene.pan.dx;
    final cy = size.height / 2 + scene.pan.dy;
    final step = 40.0 * scene.zoom;
    final paint = Paint()
      ..color = palette.textPrimary.withValues(alpha: 0.08)
      ..strokeWidth = 0.5;
    // Vertical lines.
    var x = cx % step;
    while (x < size.width) {
      canvas.drawLine(Offset(x, 0), Offset(x, size.height), paint);
      x += step;
    }
    // Horizontal lines.
    var y = cy % step;
    while (y < size.height) {
      canvas.drawLine(Offset(0, y), Offset(size.width, y), paint);
      y += step;
    }
  }

  void _paintPaperNoise(Canvas canvas, Size size) {
    // Deterministic noise — small dots scattered across the canvas at
    // 4% opacity. Cheap and gives the paper feel.
    final rng = math.Random(paperSeed);
    final paint = Paint()..color = palette.textPrimary.withValues(alpha: 0.04);
    for (int i = 0; i < 600; i++) {
      final x = rng.nextDouble() * size.width;
      final y = rng.nextDouble() * size.height;
      canvas.drawCircle(Offset(x, y), 0.6, paint);
    }
  }

  void _paintSelection(Canvas canvas, Rect bounds) {
    final paint = Paint()
      ..style = PaintingStyle.stroke
      ..strokeWidth = 1.5
      ..color = palette.accent;
    canvas.drawRect(bounds, paint);
    // Marching ants corners.
    final corner = Paint()
      ..style = PaintingStyle.stroke
      ..strokeWidth = 2
      ..color = palette.textPrimary;
    for (final c in [
      bounds.topLeft,
      bounds.topRight,
      bounds.bottomLeft,
      bounds.bottomRight,
    ]) {
      canvas.drawCircle(c, 3, corner);
    }
    // Glow.
    canvas.drawRect(
      bounds,
      Paint()
        ..style = PaintingStyle.stroke
        ..strokeWidth = 6
        ..color = palette.accent.withValues(alpha: 0.2)
        ..maskFilter = const MaskFilter.blur(BlurStyle.normal, 4),
    );
  }

  @override
  bool shouldRepaint(covariant _CanvasPainter old) =>
      old.scene != scene ||
      old.livePoints != livePoints ||
      old.isDrawing != isDrawing;
}

// Silence unused-import warning if [ui] is never referenced directly.
// (Kept for forward-compat with shader-based paper textures.)
// ignore: unused_element
ui.ImageFilter? _unusedKeepImport() => null;
