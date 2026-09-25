// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// krita_dab_renderer.dart — Dab rendering for the Krita brush engine.
//
// The brush controller's [KritaBrushController.generateDab] produces one
// [BrushDab] for one [BrushInput] sample. Real strokes are sequences of
// dabs interpolated along the pointer path with spacing, pressure
// ramping, and per-dab color/alpha modulation. This file wraps the
// single-dab call into a higher-level stroke rasterizer:
//
//   * [renderDab] — render one dab at a canvas position and composite
//     it onto a [KritaCanvasController] (the host-side composite path).
//   * [renderStroke] — render a sequence of dabs along a polyline,
//     applying brush spacing, pressure interpolation, and per-dab
//     alpha (flow * opacity * pressure). Returns the dirty region.
//   * [renderStrokeToBuffer] — same as [renderStroke] but onto a raw
//     Uint8List buffer (for offscreen export, GIF frame rendering, etc.).
//
// The renderer does NOT replace the native stroke-session ABI
// (`krita_stroke_*`): that path runs a REAL stroke through Krita's
// KisPaintOpRegistry with the engine's own spacing / interpolation. This
// renderer is the host-side composite path used by the v1 dab-compositing
// engine (still the default) and the fallback engine, so the editor can
// paint even when the stroke-session ABI is not available.

import 'dart:math' as math;

import 'krita_bindings.dart';
import 'krita_canvas_controller.dart';
import 'krita_fallback.dart';

/// One sample in a stroke polyline.
class KritaStrokePoint {
  /// Constructs a stroke point.
  const KritaStrokePoint({
    required this.x,
    required this.y,
    this.pressure = 1.0,
    this.tiltX = 0.0,
    this.tiltY = 0.0,
    this.time = 0.0,
  });

  /// Canvas-space X in pixels.
  final double x;

  /// Canvas-space Y in pixels.
  final double y;

  /// Normalized pressure in [0, 1].
  final double pressure;

  /// X tilt in degrees.
  final double tiltX;

  /// Y tilt in degrees.
  final double tiltY;

  /// Time in seconds since stroke start.
  final double time;
}

/// The dirty region produced by a [KritaDabRenderer.renderStroke] call.
class KritaDirtyRect {
  /// Constructs a dirty rect.
  const KritaDirtyRect({
    required this.x,
    required this.y,
    required this.width,
    required this.height,
  });

  /// Left edge in canvas pixel space.
  final int x;

  /// Top edge in canvas pixel space.
  final int y;

  /// Width in pixels.
  final int width;

  /// Height in pixels.
  final int height;

  /// True when the rect is empty (no dabs were rendered).
  bool get isEmpty => width <= 0 || height <= 0;

  @override
  String toString() =>
      'KritaDirtyRect($x, $y, ${width}x$height)';
}

/// Renders brush dabs onto a canvas (or a raw buffer). Stateless — every
/// method takes the brush + canvas it should operate on, so one
/// renderer instance can serve any number of strokes.
class KritaDabRenderer {
  /// Constructs a dab renderer. Stateless — no per-instance state.
  const KritaDabRenderer();

  /// Renders one dab from [brush] for [input] and composites it onto
  /// [canvas] at the input's (x, y) position. Returns the dab that was
  /// generated (empty when the engine rejected the input). [mode] and
  /// [alpha] are passed through to [KritaCanvasController.paintDab].
  BrushDab renderDab(
    KritaBrushBackend brush,
    KritaCanvasController canvas,
    BrushInput input, {
    KritaBlendMode mode = KritaBlendMode.normal,
    double alpha = 1.0,
  }) {
    final dab = brush.generateDab(input);
    if (dab.isEmpty) return dab;
    canvas.paintDab(dab, input.x, input.y, mode: mode, alpha: alpha);
    return dab;
  }

  /// Renders a stroke along [points] using [brush] and composites the
  /// dabs onto [canvas]. Dab spacing is read from [KritaBrushController.currentSpacing]
  /// (a fraction of the brush diameter); pressure is interpolated
  /// linearly between samples. Per-dab alpha is `flow * opacity *
  /// pressure` (clamped to [0, 1]).
  ///
  /// Returns the union dirty rect of all rendered dabs (in canvas pixel
  /// space). When [points] has fewer than 2 entries the renderer still
  /// emits one dab at the first point (a single tap).
  KritaDirtyRect renderStroke(
    KritaBrushBackend brush,
    KritaCanvasController canvas,
    List<KritaStrokePoint> points, {
    KritaBlendMode mode = KritaBlendMode.normal,
  }) {
    if (points.isEmpty) return const KritaDirtyRect(x: 0, y: 0, width: 0, height: 0);
    brush.cleanup();
    var minX = 0x7FFFFFFF;
    var minY = 0x7FFFFFFF;
    var maxX = -1;
    var maxY = -1;
    void mark(BrushDab dab, double cx, double cy) {
      if (dab.isEmpty) return;
      final halfW = dab.width / 2.0;
      final halfH = dab.height / 2.0;
      final left = (cx - halfW).floor();
      final top = (cy - halfH).floor();
      final right = (cx + halfW).ceil();
      final bottom = (cy + halfH).ceil();
      if (left < minX) minX = left;
      if (top < minY) minY = top;
      if (right > maxX) maxX = right;
      if (bottom > maxY) maxY = bottom;
    }

    final spacing = brush.currentSpacing.clamp(0.02, 5.0);
    final diameter = brush.currentSize.clamp(1.0, 4096.0);
    final stepPx = (diameter * spacing).clamp(0.5, 4096.0);
    final flow = brush.currentFlow.clamp(0.0, 1.0);
    final opacity = brush.currentOpacity.clamp(0.0, 1.0);
    final erase = brush.isEraserPreset || mode == KritaBlendMode.erase;
    final blend = erase ? KritaBlendMode.erase : mode;

    if (points.length == 1) {
      final p = points.first;
      final dab = brush.generateDab(BrushInput(
        x: p.x,
        y: p.y,
        pressure: p.pressure,
        tiltX: p.tiltX,
        tiltY: p.tiltY,
        time: p.time,
      ));
      final a = (flow * opacity * p.pressure).clamp(0.0, 1.0);
      if (dab.isNotEmpty) {
        canvas.paintDab(dab, p.x, p.y, mode: blend, alpha: a);
        mark(dab, p.x, p.y);
      }
    } else {
      for (var i = 1; i < points.length; i++) {
        final a = points[i - 1];
        final b = points[i];
        final dx = b.x - a.x;
        final dy = b.y - a.y;
        final segLen = math.sqrt(dx * dx + dy * dy);
        final n = (segLen / stepPx).ceil();
        for (var s = 0; s <= n; s++) {
          if (s == 0 && i > 1) continue; // skip duplicate joints
          final t = n == 0 ? 0.0 : s / n;
          final px = a.x + dx * t;
          final py = a.y + dy * t;
          final pp = a.pressure + (b.pressure - a.pressure) * t;
          final pt = a.time + (b.time - a.time) * t;
          final dab = brush.generateDab(BrushInput(
            x: px,
            y: py,
            pressure: pp,
            tiltX: a.tiltX + (b.tiltX - a.tiltX) * t,
            tiltY: a.tiltY + (b.tiltY - a.tiltY) * t,
            time: pt,
          ));
          final alpha = (flow * opacity * pp).clamp(0.0, 1.0);
          if (dab.isNotEmpty) {
            canvas.paintDab(dab, px, py, mode: blend, alpha: alpha);
            mark(dab, px, py);
          }
        }
      }
    }
    if (maxX < 0) return const KritaDirtyRect(x: 0, y: 0, width: 0, height: 0);
    return KritaDirtyRect(
      x: minX,
      y: minY,
      width: maxX - minX,
      height: maxY - minY,
    );
  }

  /// Convenience: renders a single dab for a press/tap event (no
  /// motion). Equivalent to [renderDab] with a default [BrushInput]
  /// built from a position + pressure.
  BrushDab renderTap(
    KritaBrushBackend brush,
    KritaCanvasController canvas,
    double x,
    double y, {
    double pressure = 1.0,
    KritaBlendMode mode = KritaBlendMode.normal,
  }) {
    return renderDab(
      brush,
      canvas,
      BrushInput(x: x, y: y, pressure: pressure),
      mode: mode,
    );
  }
}
