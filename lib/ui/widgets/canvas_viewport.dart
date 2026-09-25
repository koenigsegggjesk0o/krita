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
//
// ── 3D shading ────────────────────────────────────────────────────────────
// Strokes are painted as faux-3D tubes — no full rasterizer, no mesh
// tessellation, just a smarter CustomPainter. For every stroke we:
//
//   1. compute a per-vertex screen-space normal (perpendicular of the
//      local tangent, smoothed across the polyline),
//   2. compute a per-vertex lit sign from `dot(normal, -lightDir)` —
//      i.e. which side of the tube faces the light source,
//   3. draw THREE offset polylines:
//        • a base pass at the stroke's own color & width,
//        • a "shadow" pass offset to the unlit side, darker & thinner,
//        • a "highlight" pass offset to the lit side, brighter & thinner.
//      The three passes overlap with round caps/joins so the visible
//      cross-section reads as a shaded cylinder (Lambert falloff).
//   4. drop a soft shadow on the ground plane (or an offset blob if
//      no ground Y is provided), skewed along the light direction,
//   5. optionally taper width with per-vertex depth (perspective).
//
// Materials:
//   • [CanvasMaterial.flat]    — legacy solid color (no shading).
//   • [CanvasMaterial.shaded]  — the 3-pass Lambert tube (default).
//   • [CanvasMaterial.glow]    — additive blend + blurred bloom halo.
//   • [CanvasMaterial.guide]   — translucent ribbon with grid hatch
//                                (used to render Guide3D surfaces).

import 'dart:math' as math;
import 'dart:ui' as ui;

import 'package:flutter/gestures.dart'
    show
        kPrimaryButton,
        kSecondaryButton,
        kMiddleMouseButton,
        PointerScrollEvent,
        PointerSignalEvent;
import 'package:flutter/material.dart';
import 'package:flutter/services.dart' show HardwareKeyboard, LogicalKeyboardKey;

import '../theme/feather_colors.dart';

// ── Overlay primitives ───────────────────────────────────────────────────
//
// Engine subsystems ([GizmoRenderer], [SelectionRenderer], [LiquifyRenderer])
// emit screen-space draw-lists of pure-data primitives (lines, polylines,
// circles, triangles) so the engine stays decoupled from a paint backend.
// The host adapts those draw-lists into the [CanvasOverlay] value type
// below and hands them to [CanvasScene.overlayPrimitives]; the painter
// draws them on top of the strokes / selection bounds.
//
// Mirroring the engine's primitive kinds here (rather than importing the
// engine) keeps the UI package hermetic — the same architecture the rest
// of [CanvasScene] follows.

/// One drawable overlay primitive, in canvas pixel space.
abstract class CanvasOverlay {
  /// ARGB colour of the primitive.
  int get color;
  /// Stroke width for lines / outlines (ignored for filled shapes).
  double get strokeWidth;
}

/// A line segment overlay.
class CanvasOverlayLine implements CanvasOverlay {
  const CanvasOverlayLine(this.a, this.b, this.color, {this.strokeWidth = 2.0});
  final Offset a;
  final Offset b;
  @override
  final int color;
  @override
  final double strokeWidth;
}

/// A polyline (chain of segments) overlay.
class CanvasOverlayPolyline implements CanvasOverlay {
  const CanvasOverlayPolyline(this.points, this.color, {this.strokeWidth = 2.0});
  final List<Offset> points;
  @override
  final int color;
  @override
  final double strokeWidth;
}

/// A filled or outlined circle overlay.
class CanvasOverlayCircle implements CanvasOverlay {
  const CanvasOverlayCircle(this.center, this.radius, this.color,
      {this.filled = false, this.strokeWidth = 2.0});
  final Offset center;
  final double radius;
  final bool filled;
  @override
  final int color;
  @override
  final double strokeWidth;
}

/// A filled triangle overlay.
class CanvasOverlayTriangle implements CanvasOverlay {
  const CanvasOverlayTriangle(this.a, this.b, this.c, this.color,
      {this.strokeWidth = 0.0});
  final Offset a;
  final Offset b;
  final Offset c;
  @override
  final int color;
  @override
  final double strokeWidth;
}

/// Material kind for 3D stroke shading.
enum CanvasMaterial {
  /// Flat solid color (legacy behaviour, no shading).
  flat,

  /// Lambert-shaded tube — darker on the shadow side, brighter on the
  /// light side. The default for brush strokes.
  shaded,

  /// Additive bloom — bright core plus a soft halo, blended on top of
  /// the canvas with [BlendMode.plus].
  glow,

  /// Reads as the scene background: paints with the canvas background
  /// color so the stroke visually disappears into the paper. Maps to
  /// Feather's "Cutout" material.
  cutout,

  /// Translucent surface with grid hatch — used by Guide3D meshes.
  guide,
}

class CanvasStroke {
  const CanvasStroke({
    required this.points,
    required this.color,
    this.width = 4,
    this.tapered = true,
    this.material = CanvasMaterial.shaded,
    /// Per-point world-space depth (camera-Z), one entry per point in
    /// [points]. When provided, the painter modulates per-vertex width
    /// (closer = thicker) and scales the drop-shadow skew. When empty,
    /// a uniform width is used.
    this.depths = const [],
  });

  final List<Offset> points;
  final Color color;
  final double width;
  final bool tapered;
  final CanvasMaterial material;
  final List<double> depths;
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
    /// Direction light TRAVELS in screen space (not necessarily
    /// normalized — the painter will normalize). Default: light comes
    /// from the top-left, traveling towards the bottom-right.
    this.lightDir = const Offset(0.7, 0.7),
    /// Ambient term in 0..1 — floor for the shadow side. 0.35 means
    /// the unlit side is at most 35% as bright as the base color.
    this.ambient = 0.35,
    /// Diffuse term in 0..1 — ceiling lift for the lit side. 0.65
    /// means the highlight climbs 65% of the way from the base color
    /// to pure white.
    this.diffuse = 0.65,
    /// Ground-plane Y in scene-local pixels. When non-null, strokes
    /// cast a proper projected shadow onto this line, skewed along
    /// [lightDir]. When null, the painter falls back to a soft offset
    /// blob shadow under each stroke.
    this.groundY,
    /// When true, the painter draws a visible ground-plane band at
    /// [groundY] AND projects stroke shadows onto it. When false, no
    /// ground plane is drawn and strokes cast no shadow at all (per
    /// the Stage panel's "Ground plane" toggle: OFF = no ground, no
    /// shadows). Defaults to false so legacy callers see no behaviour
    /// change unless they opt in.
    this.showGroundPlane = false,
    /// User-picked background color override. When null, the painter
    /// uses the palette's default canvas background. The Cutout
    /// material also uses this color so a cutout stroke visually
    /// disappears into whatever background the artist chose (not just
    /// the palette default).
    this.backgroundColor,
    /// Glow halo size multiplier in 0..1 — scales the outer-blur radius
    /// of the glow material's bloom halo. 0.5 (default) reproduces the
    /// legacy look; 0 = no halo, 1 = maximum bloom. Driven by the
    /// Stage panel's "Glow area" slider.
    this.glowArea = 0.5,
    /// Real Krita paint layer — an RGBA8 raster composited by the
    /// brush engine's `generateDab` path (via `KritaCanvasController`
    /// on the host). Drawn on top of the 3D stroke ribbons so the
    /// actual dab pixels are visible. Null when the real engine is
    /// unavailable (the procedural / fallback path keeps using the
    /// 3D polyline rendering only).
    this.paintLayer,
    /// Overlay primitives rendered on top of the strokes (gizmo handles,
    /// selection highlight, liquify brush preview). Built by the host
    /// from the engine's [GizmoRenderer] / [SelectionRenderer] /
    /// [LiquifyRenderer] draw-lists and adapted into [CanvasOverlay]
    /// values so the UI package stays decoupled from the engine. Empty by
    /// default (no overlay to draw).
    this.overlayPrimitives = const [],
  });

  final List<CanvasStroke> strokes;
  final Color activeColor;
  final CanvasStroke? previewStroke;
  final Rect? selectionBounds;
  final double zoom;
  final Offset pan;
  final bool showGrid;
  final bool renderMode;
  final Offset lightDir;
  final double ambient;
  final double diffuse;
  final double? groundY;
  final bool showGroundPlane;
  final Color? backgroundColor;
  final double glowArea;

  /// The real-Krita dab composite layer (host-side rasterized from the
  /// [KritaCanvasController] backing buffer). Null on the fallback
  /// engine or before the first dab is painted.
  final ui.Image? paintLayer;

  /// Overlay primitives drawn on top of the strokes (gizmo / selection /
  /// liquify preview). Drawn after [selectionBounds] in screen space
  /// (no pan / zoom — they're already in canvas pixels).
  final List<CanvasOverlay> overlayPrimitives;
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
    this.onTapSelect,
    this.onLiquifyDrag,
    // --- Desktop mouse wiring (loop-keyboard-shortcuts-mouse) ------------
    // Right-click drag → orbit (defaults to [onPan] when [onOrbit] is null,
    // since the host's [onPan] handler already orbits the camera).
    // Middle-click drag → pan the orbit centre (translate the camera +
    // target together). Scroll wheel → zoom (delegates to [onZoom]).
    // Ctrl + scroll → adjust brush size by a signed delta in millimetres.
    // All four are optional: when null, the corresponding mouse gesture is
    // a no-op (touch / stylus handling is unaffected).
    this.onOrbit,
    this.onCameraPan,
    this.onBrushSizeDelta,
    this.paperSeed = 7,
  });

  final CanvasScene scene;
  final VoidCallback? onStrokeStart;
  final ValueChanged<Offset>? onStrokeUpdate;
  final VoidCallback? onStrokeEnd;
  final ValueChanged<Offset>? onPan;
  final ValueChanged<double>? onZoom;

  /// Fired on a single-finger tap (select gesture). Accepted but not yet
  /// routed from the gesture surface — the host wires it to its
  /// tap-select handler, but the viewport currently only emits pan /
  /// stroke / scale gestures. Plumbing the tap detection is a follow-up.
  final ValueChanged<Offset>? onTapSelect;

  /// Fired on every pan-update while the Liquify tool is active. The
  /// host (lib/screens/main_screen.dart) routes the screen position +
  /// drag delta to its [_onLiquifyDrag] handler, which forwards the drag
  /// to [LiquifyEngine.applyDrag] AND tracks the cursor so a live
  /// [LiquifyRenderer] brush preview can be drawn on top of the strokes
  /// (see [CanvasScene.overlayPrimitives]). Single-finger drag in the
  /// Liquify tool = liquify drag; two-finger pinch still orbits the
  /// camera (routed through [onPan] / [onZoom] via onScaleStart).
  final void Function(Offset screenPos, Offset dragDelta)? onLiquifyDrag;

  /// Right-mouse-button drag → orbit the camera. Falls back to [onPan]
  /// when null (the host's [onPan] handler orbits by default), so the
  /// right-button orbit path works without explicit wiring.
  final ValueChanged<Offset>? onOrbit;

  /// Middle-mouse-button drag → pan the orbit centre (translate the
  /// camera + target together). Distinct from [onPan] which orbits.
  final ValueChanged<Offset>? onCameraPan;

  /// Ctrl + scroll wheel → adjust the brush size by a signed delta in
  /// millimetres (positive = grow, negative = shrink). The host clamps
  /// and mirrors the new size into the brush engine.
  final ValueChanged<double>? onBrushSizeDelta;

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
  bool _isLiquifyDragging = false;
  Offset? _lastPan;

  // ----- Desktop mouse state (loop-keyboard-shortcuts-mouse) -------------
  //
  // The GestureDetector below only routes single-finger / touch / stylus
  // drags. For mouse devices we layer a [Listener] on top that:
  //   * routes right-button drag → [widget.onOrbit] (falls back to
  //     [widget.onPan] which orbits) and suppresses the GestureDetector,
  //   * routes middle-button drag → [widget.onCameraPan] (real pan) and
  //     suppresses the GestureDetector,
  //   * routes the scroll wheel → [widget.onZoom] (zoom) or, when Ctrl is
  //     held, [widget.onBrushSizeDelta] (brush size ±),
  //   * lets left-button / touch / stylus drags fall through to the
  //     GestureDetector so the existing draw / liquify / orbit paths are
  //     unchanged. When Space is held, left-button drag is redirected to
  //     orbit (per Feather's "Space = Pan mode (hold)" shortcut) by
  //     setting [_suppressGesture] for the duration of the drag.
  int _mouseButtons = 0; // bitmask of currently-held mouse buttons
  bool _suppressGesture = false; // true while a right/middle drag owns the gesture
  Offset? _mouseDragLast;

  @override
  Widget build(BuildContext context) {
    final palette = _palette(context);
    return RepaintBoundary(
      // Outer Listener captures desktop mouse events that the
      // GestureDetector below can't distinguish (right / middle button,
      // scroll wheel, Ctrl-modified scroll). When a right / middle drag
      // begins — or a left drag begins while Space is held — the Listener
      // sets [_suppressGesture] so the GestureDetector's onPan* callbacks
      // become no-ops for that drag (the GestureDetector would otherwise
      // treat a right-button drag exactly like a left-button drag and
      // start drawing).
      child: Listener(
        behavior: HitTestBehavior.opaque,
        onPointerDown: _onPointerDown,
        onPointerMove: _onPointerMove,
        onPointerUp: _onPointerUp,
        onPointerCancel: _onPointerUp,
        onPointerSignal: _onPointerSignal,
        child: GestureDetector(
          behavior: HitTestBehavior.opaque,
          onPanStart: (d) {
            // Two-finger / right-button pans; one-finger draws. We can't
            // distinguish at GestureDetector level without custom logic,
            // so we expose both: if onStrokeStart is null, treat as pan.
            if (_suppressGesture) return; // owned by the Listener.
            if (widget.onStrokeStart != null) {
              _isDrawing = true;
              _livePoints
                ..clear()
                ..add(d.localPosition);
              widget.onStrokeStart?.call();
              setState(() {});
            } else if (widget.onLiquifyDrag != null) {
              // GAP 3 FIX: route single-finger drag to the liquify engine
              // when the liquify tool is active (host wires onLiquifyDrag
              // only in that mode). Two-finger gestures still fall through
              // to onScaleStart → onPan / onZoom so the camera can orbit.
              _isLiquifyDragging = true;
              _lastPan = d.localPosition;
              widget.onLiquifyDrag!(d.localPosition, Offset.zero);
            } else {
              _isPanning = true;
              _lastPan = d.localPosition;
            }
          },
          onPanUpdate: (d) {
            if (_suppressGesture) return; // owned by the Listener.
            if (_isDrawing) {
              _livePoints.add(d.localPosition);
              widget.onStrokeUpdate?.call(d.localPosition);
              setState(() {});
            } else if (_isLiquifyDragging) {
              final delta = d.localPosition - (_lastPan ?? d.localPosition);
              _lastPan = d.localPosition;
              widget.onLiquifyDrag!(d.localPosition, delta);
            } else if (_isPanning) {
              final delta = d.localPosition - (_lastPan ?? d.localPosition);
              _lastPan = d.localPosition;
              widget.onPan?.call(delta);
            }
          },
          onPanEnd: (_) {
            if (_suppressGesture) return; // owned by the Listener.
            if (_isDrawing) {
              _isDrawing = false;
              _livePoints.clear();
              widget.onStrokeEnd?.call();
              setState(() {});
            } else if (_isLiquifyDragging) {
              _isLiquifyDragging = false;
              _lastPan = null;
            } else if (_isPanning) {
              _isPanning = false;
              _lastPan = null;
            }
          },
          onScaleStart: (d) {
            if (_suppressGesture) return; // owned by the Listener.
            _isPanning = true;
            _lastPan = d.focalPoint;
          },
          onScaleUpdate: (d) {
            if (_suppressGesture) return; // owned by the Listener.
            final delta = d.focalPoint - (_lastPan ?? d.focalPoint);
            _lastPan = d.focalPoint;
            widget.onPan?.call(delta);
            if ((d.scale - 1.0).abs() > 0.001) {
              widget.onZoom?.call(d.scale);
            }
          },
          onScaleEnd: (_) {
            if (_suppressGesture) return; // owned by the Listener.
            _isPanning = false;
            _lastPan = null;
          },
          // Tap-to-select: fires when a single finger lands and lifts
          // without significant drag. Used by the Select tool's tap-select
          // (host routes the screen point to its [SelectionSystem]) and by
          // Loft mode's tap-to-pick-curve flow (host routes the point to
          // its loft selection list). The gesture arena resolves tap vs
          // pan: a quick tap wins, a drag wins pan.
          onTapUp: widget.onTapSelect == null
              ? null
              : (details) => widget.onTapSelect!(details.localPosition),
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
      ),
    );
  }

  // ----- Desktop mouse routing (loop-keyboard-shortcuts-mouse) -----------

  void _onPointerDown(PointerDownEvent event) {
    // Track the held buttons so _onPointerMove can route multi-button
    // drags (e.g. chording left + right). Touch / stylus events carry a
    // buttons bitmask of 0 (touch) or kPrimaryButton (stylus); only real
    // mouse events set the secondary / tertiary bits.
    _mouseButtons = event.buttons;
    final isRight = (event.buttons & kSecondaryButton) != 0;
    final isMiddle = (event.buttons & kMiddleMouseButton) != 0;
    final spaceHeld = HardwareKeyboard.instance
        .isLogicalKeyPressed(LogicalKeyboardKey.space);
    // A left-button drag while Space is held orbits (per the
    // "Space = Pan mode (hold)" Feather shortcut). We treat it the same
    // as a right-button drag: route to the orbit callback and suppress
    // the GestureDetector's draw path.
    final leftOrbits = (event.buttons & kPrimaryButton) != 0 && spaceHeld;
    if (isRight || isMiddle || leftOrbits) {
      _suppressGesture = true;
      _mouseDragLast = event.position;
    }
  }

  void _onPointerMove(PointerMoveEvent event) {
    if (!_suppressGesture) return;
    final last = _mouseDragLast;
    if (last == null) return;
    final delta = event.position - last;
    _mouseDragLast = event.position;
    final isMiddle = (_mouseButtons & kMiddleMouseButton) != 0;
    if (isMiddle) {
      // Middle-button drag → real pan (translate the orbit centre).
      widget.onCameraPan?.call(delta);
    } else {
      // Right-button drag (or left + Space) → orbit. Falls back to
      // [onPan] when the host hasn't wired [onOrbit] explicitly — the
      // host's [onPan] handler orbits by default.
      final orbit = widget.onOrbit ?? widget.onPan;
      orbit?.call(delta);
    }
  }

  void _onPointerUp(PointerEvent event) {
    _mouseButtons = 0;
    _suppressGesture = false;
    _mouseDragLast = null;
  }

  void _onPointerSignal(PointerSignalEvent event) {
    if (event is! PointerScrollEvent) return;
    final ctrl = HardwareKeyboard.instance.isControlPressed ||
        HardwareKeyboard.instance.isMetaPressed;
    if (ctrl) {
      // Ctrl + scroll → brush size ±. Map scroll-up (negative dy) to
      // grow, scroll-down to shrink, in 2 mm increments per notch.
      final delta = -event.scrollDelta.dy.sign * 2.0;
      widget.onBrushSizeDelta?.call(delta);
      return;
    }
    // Plain scroll → zoom. Each notch adjusts the distance by ~10%.
    // scrollDelta.dy is positive for scroll-down (wheel toward user)
    // which should zoom OUT (factor > 1 = farther).
    final factor = 1.0 + (event.scrollDelta.dy * 0.002);
    if ((factor - 1.0).abs() < 1e-6) return;
    widget.onZoom?.call(factor);
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
    // 1. Background — paper. Use the user-picked background color when
    //    the Stage panel set one; otherwise the palette default.
    final bg = scene.backgroundColor ?? palette.canvasBackground;
    final bgPaint = Paint()..color = bg;
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

    // 3b. Ground plane — visible band + horizontal rule at the project
    //     line. Drawn OUTSIDE the pan/zoom transform so it stays stable
    //     on screen regardless of camera orbit / pan. Strokes' projected
    //     shadows land on this line (see [_paintDropShadow]).
    if (scene.showGroundPlane) {
      _paintGroundPlane(canvas, size);
    }

    // 4. Strokes — drop shadows first (so they sit beneath every tube),
    //    then tubes / glow / guides on top.
    canvas.save();
    canvas.translate(scene.pan.dx, scene.pan.dy);
    canvas.scale(scene.zoom);
    // Pass 1: shadows only.
    for (final s in scene.strokes) {
      _paintDropShadow(canvas, s, size);
    }
    // Pass 2: the strokes themselves.
    for (final s in scene.strokes) {
      _paintStroke(canvas, s);
    }
    // 5. Live preview.
    if (isDrawing && livePoints.length >= 2) {
      final live = CanvasStroke(
        points: livePoints,
        color: scene.activeColor,
        width: 4,
      );
      _paintDropShadow(canvas, live, size);
      _paintStroke(canvas, live);
    } else if (scene.previewStroke != null) {
      _paintDropShadow(canvas, scene.previewStroke!, size);
      _paintStroke(canvas, scene.previewStroke!);
    }
    canvas.restore();

    // 5b. Real Krita dab composite layer — drawn on top of the 3D
    //     strokes so the actual `generateDab` pixels are visible. The
    //     layer is rasterized from the host-side [KritaCanvasController]
    //     backing buffer (1:1 with the viewport in logical pixels), so
    //     we draw it at the canvas origin without any pan/zoom. Null on
    //     the fallback engine — in that case the 3D polyline rendering
    //     above is the only paint path (current behaviour preserved).
    final paintLayer = scene.paintLayer;
    if (paintLayer != null) {
      canvas.drawImage(
        paintLayer,
        Offset.zero,
        Paint()..filterQuality = FilterQuality.low,
      );
    }

    // 6. Selection bounds.
    if (scene.selectionBounds != null) {
      _paintSelection(canvas, scene.selectionBounds!);
    }

    // 7. Overlay primitives (gizmo / selection / liquify brush preview).
    //    These come from the host's adaptation of the engine renderers'
    //    draw-lists (see [GizmoRenderer], [SelectionRenderer],
    //    [LiquifyRenderer]). They're already in canvas pixels — drawn
    //    outside the pan/zoom transform.
    if (scene.overlayPrimitives.isNotEmpty) {
      _paintOverlays(canvas, scene.overlayPrimitives);
    }
  }

  // ── Stroke dispatch ──────────────────────────────────────────────────

  void _paintStroke(Canvas canvas, CanvasStroke stroke) {
    if (stroke.points.length < 2) return;
    // Render-mode toggle (Stage panel → Environment → Render Mode):
    //   ON  → strokes render with their assigned material (shaded /
    //         glow / cutout / guide). The default "artistic" look.
    //   OFF → strokes render flat (shadeless), regardless of their
    //         assigned material. Useful for previewing the raw
    //         silhouette / occluder set without 3D shading.
    //
    // We honour the per-stroke `material` only when render mode is on;
    // otherwise we force [CanvasMaterial.flat]. The flat material keeps
    // the stroke's own colour (no shading, no halo) so the artist sees
    // the unmodified paint.
    final effective = scene.renderMode ? stroke.material : CanvasMaterial.flat;
    switch (effective) {
      case CanvasMaterial.flat:
        _paintFlat(canvas, stroke);
        break;
      case CanvasMaterial.shaded:
        _paintShadedTube(canvas, stroke, additive: false);
        break;
      case CanvasMaterial.glow:
        _paintGlowHalo(canvas, stroke);
        _paintShadedTube(canvas, stroke, additive: true);
        break;
      case CanvasMaterial.cutout:
        _paintCutout(canvas, stroke);
        break;
      case CanvasMaterial.guide:
        _paintGuideRibbon(canvas, stroke);
        break;
    }
  }

  /// Cutout: paints the stroke with the canvas background color so it
  /// reads as the scene background. No drop shadow, no shading — the
  /// stroke visually disappears into the paper. Uses the user-picked
  /// [CanvasScene.backgroundColor] when set, so a cutout stroke
  /// vanishes into whatever background the artist chose (not just the
  /// palette default).
  void _paintCutout(Canvas canvas, CanvasStroke stroke) {
    final path = _strokePath(stroke);
    final bg = scene.backgroundColor ?? palette.canvasBackground;
    final paint = Paint()
      ..color = bg
      ..style = PaintingStyle.stroke
      ..strokeWidth = stroke.width
      ..strokeCap = StrokeCap.round
      ..strokeJoin = StrokeJoin.round;
    canvas.drawPath(path, paint);
  }

  // ── Flat (legacy) ────────────────────────────────────────────────────

  void _paintFlat(Canvas canvas, CanvasStroke stroke) {
    final path = _strokePath(stroke);
    final paint = Paint()
      ..color = stroke.color
      ..style = PaintingStyle.stroke
      ..strokeWidth = stroke.width
      ..strokeCap = StrokeCap.round
      ..strokeJoin = StrokeJoin.round;
    canvas.drawPath(path, paint);
    if (stroke.tapered) {
      canvas.drawPath(
        path,
        Paint()
          ..color = stroke.color.withValues(alpha: 0.6)
          ..style = PaintingStyle.stroke
          ..strokeWidth = stroke.width * 0.5
          ..strokeCap = StrokeCap.round,
      );
    }
  }

  // ── 3D shaded tube (Lambert) ─────────────────────────────────────────

  /// Draws a stroke as a faux-3D tube: a base pass plus a darker
  /// "shadow" pass offset to the unlit side and a brighter "highlight"
  /// pass offset to the lit side. When [additive] is true (used by the
  /// glow material) the passes blend with [BlendMode.plus].
  void _paintShadedTube(
    Canvas canvas,
    CanvasStroke stroke, {
    required bool additive,
  }) {
    final pts = stroke.points;
    final n = pts.length;
    if (n < 2) return;

    final geom = _strokeGeometry(stroke);
    if (geom == null) return;
    final normals = geom.normals;
    final litSigns = geom.litSigns;
    final halfWidths = geom.halfWidths;

    // Base color analysis → derive shadow & highlight colors.
    final base = HSVColor.fromColor(stroke.color);
    final shadowCol = HSVColor.fromAHSV(
      stroke.color.a,
      base.hue,
      (base.saturation * 0.85).clamp(0.0, 1.0),
      (base.value * scene.ambient).clamp(0.0, 1.0),
    ).toColor();
    final lightCol = HSVColor.fromAHSV(
      stroke.color.a,
      base.hue,
      (base.saturation * 0.55).clamp(0.0, 1.0),
      (base.value + (1.0 - base.value) * scene.diffuse).clamp(0.0, 1.0),
    ).toColor();
    final coreCol = HSVColor.fromAHSV(
      stroke.color.a,
      base.hue,
      base.saturation,
      (base.value + (1.0 - base.value) * 0.5).clamp(0.0, 1.0),
    ).toColor();

    // Pass 1: shadow side (drawn first so the base pass overlaps it).
    final shadowPath = _offsetPolyline(pts, normals, litSigns, halfWidths,
        side: -1.0, factor: 0.55);
    canvas.drawPath(
      shadowPath,
      Paint()
        ..color = shadowCol
        ..style = PaintingStyle.stroke
        ..strokeWidth = stroke.width * 0.9
        ..strokeCap = StrokeCap.round
        ..strokeJoin = StrokeJoin.round
        ..blendMode = additive ? BlendMode.plus : BlendMode.srcOver,
    );

    // Pass 2: base tube (full width, base color).
    final basePath = _strokePath(stroke);
    canvas.drawPath(
      basePath,
      Paint()
        ..color = stroke.color
        ..style = PaintingStyle.stroke
        ..strokeWidth = stroke.width
        ..strokeCap = StrokeCap.round
        ..strokeJoin = StrokeJoin.round
        ..blendMode = additive ? BlendMode.plus : BlendMode.srcOver,
    );

    // Pass 3: highlight on the lit side.
    final lightPath = _offsetPolyline(pts, normals, litSigns, halfWidths,
        side: 1.0, factor: 0.5);
    canvas.drawPath(
      lightPath,
      Paint()
        ..color = lightCol
        ..style = PaintingStyle.stroke
        ..strokeWidth = stroke.width * 0.45
        ..strokeCap = StrokeCap.round
        ..strokeJoin = StrokeJoin.round
        ..blendMode = additive ? BlendMode.plus : BlendMode.srcOver,
    );

    // Pass 4: a thin specular core for the "tube catch-light" feel.
    if (stroke.tapered) {
      final corePath = _offsetPolyline(pts, normals, litSigns, halfWidths,
          side: 1.0, factor: 0.25);
      canvas.drawPath(
        corePath,
        Paint()
          ..color = coreCol.withValues(alpha: 0.85)
          ..style = PaintingStyle.stroke
          ..strokeWidth = stroke.width * 0.18
          ..strokeCap = StrokeCap.round
          ..strokeJoin = StrokeJoin.round
          ..blendMode = additive ? BlendMode.plus : BlendMode.srcOver,
      );
    }
  }

  // ── Glow material ────────────────────────────────────────────────────

  /// Draws a soft bloom halo around the stroke using a heavily blurred,
  /// additively-blended wider pass. The actual bright core is drawn by
  /// [_paintShadedTube] with `additive: true`.
  ///
  /// The halo's blur radius scales with [CanvasScene.glowArea] (driven
  /// by the Stage panel's "Glow area" slider): 0 = no halo, 0.5 = the
  /// legacy look, 1.0 = maximum bloom. The mid halo is always narrower
  /// so the bright core reads at any setting.
  void _paintGlowHalo(Canvas canvas, CanvasStroke stroke) {
    final path = _strokePath(stroke);
    final base = HSVColor.fromColor(stroke.color);
    final haloCol = HSVColor.fromAHSV(
      stroke.color.a * 0.6,
      base.hue,
      (base.saturation * 0.6).clamp(0.0, 1.0),
      1.0,
    ).toColor();
    // glowArea 0..1 → outer blur radius 0..12, mid blur radius 0..4.
    final outerBlur = 12.0 * scene.glowArea.clamp(0.0, 1.0);
    final midBlur = 4.0 * scene.glowArea.clamp(0.0, 1.0);
    // Outer wide blur (skipped entirely when glowArea collapses to 0).
    if (outerBlur > 0.01) {
      canvas.drawPath(
        path,
        Paint()
          ..color = haloCol
          ..style = PaintingStyle.stroke
          ..strokeWidth = stroke.width * 4.0
          ..strokeCap = StrokeCap.round
          ..strokeJoin = StrokeJoin.round
          ..blendMode = BlendMode.plus
          ..maskFilter = MaskFilter.blur(BlurStyle.normal, outerBlur),
      );
    }
    // Mid halo — narrower, brighter.
    if (midBlur > 0.01) {
      canvas.drawPath(
        path,
        Paint()
          ..color = haloCol.withValues(alpha: 0.9)
          ..style = PaintingStyle.stroke
          ..strokeWidth = stroke.width * 2.0
          ..strokeCap = StrokeCap.round
          ..strokeJoin = StrokeJoin.round
          ..blendMode = BlendMode.plus
          ..maskFilter = MaskFilter.blur(BlurStyle.normal, midBlur),
      );
    }
  }

  // ── Guide3D ribbon ───────────────────────────────────────────────────

  /// Renders the stroke as a translucent surface ribbon with a faint
  /// grid hatch — the look of a Guide3D helper plane.
  void _paintGuideRibbon(Canvas canvas, CanvasStroke stroke) {
    final pts = stroke.points;
    final n = pts.length;
    if (n < 2) return;
    final geom = _strokeGeometry(stroke);
    if (geom == null) return;
    final normals = geom.normals;
    final litSigns = geom.litSigns;
    final halfWidths = geom.halfWidths;

    final base = HSVColor.fromColor(stroke.color);

    // Build the ribbon polygon (offset both sides of the centreline).
    final ribbon = Path();
    final top = <Offset>[];
    final bot = <Offset>[];
    for (int i = 0; i < n; i++) {
      final hw = halfWidths[i];
      top.add(pts[i] + normals[i] * hw);
      bot.add(pts[i] - normals[i] * hw);
    }
    ribbon.moveTo(top.first.dx, top.first.dy);
    for (final p in top.skip(1)) {
      ribbon.lineTo(p.dx, p.dy);
    }
    for (int i = bot.length - 1; i >= 0; i--) {
      ribbon.lineTo(bot[i].dx, bot[i].dy);
    }
    ribbon.close();

    // Translucent fill.
    final fillCol = HSVColor.fromAHSV(
      0.18,
      base.hue,
      base.saturation,
      base.value,
    ).toColor();
    canvas.drawPath(
      ribbon,
      Paint()
        ..color = fillCol
        ..style = PaintingStyle.fill
        ..blendMode = BlendMode.srcOver,
    );

    // Edges — slightly brighter so the surface silhouette reads.
    final edgeCol = HSVColor.fromAHSV(
      0.55,
      base.hue,
      base.saturation,
      (base.value + 0.15).clamp(0.0, 1.0),
    ).toColor();
    final edgePaint = Paint()
      ..color = edgeCol
      ..style = PaintingStyle.stroke
      ..strokeWidth = 1.0
      ..strokeJoin = StrokeJoin.round;
    canvas.drawPath(ribbon, edgePaint);

    // Grid hatch: short cross-ribbons at regular intervals along the
    // centreline. The hatch direction is the local normal.
    final hatchPaint = Paint()
      ..color = edgeCol.withValues(alpha: 0.35)
      ..style = PaintingStyle.stroke
      ..strokeWidth = 0.75;
    const hatchEvery = 24.0;
    var walked = 0.0;
    for (int i = 1; i < n; i++) {
      final seg = pts[i] - pts[i - 1];
      final segLen = seg.distance;
      if (segLen < 1e-6) continue;
      walked += segLen;
      if (walked >= hatchEvery) {
        walked = 0.0;
        final hw = halfWidths[i];
        final nr = normals[i];
        final p = pts[i];
        canvas.drawLine(
          p - nr * hw,
          p + nr * hw,
          hatchPaint,
        );
      }
    }

    // Soft lit edge on the lit side (carry-over of the 3D feel).
    final litEdge = _offsetPolyline(pts, normals, litSigns, halfWidths,
        side: 1.0, factor: 0.85);
    canvas.drawPath(
      litEdge,
      Paint()
        ..color = edgeCol.withValues(alpha: 0.5)
        ..style = PaintingStyle.stroke
        ..strokeWidth = 1.2
        ..strokeCap = StrokeCap.round,
    );
  }

  // ── Drop shadow ──────────────────────────────────────────────────────

  /// Drops a shadow under a stroke.
  ///
  /// Behaviour is governed by the Stage panel's "Ground plane" toggle
  /// (→ [CanvasScene.showGroundPlane]):
  ///   * OFF (default): no shadow at all. Strokes still render normally,
  ///     just without any drop shadow — matches the task contract
  ///     ("OFF → no ground plane, no shadows").
  ///   * ON: a true plan projection of the stroke onto the ground line
  ///     along [CanvasScene.lightDir], drawn as a soft blurred polyline.
  ///     The ground line is at 80% of the viewport height (computed in
  ///     [_groundLineY]) unless the host passed an explicit
  ///     [CanvasScene.groundY].
  ///
  /// Flat, cutout & guide materials never cast a 3D shadow (they're
  /// non-3D ribbons).
  void _paintDropShadow(Canvas canvas, CanvasStroke stroke, Size size) {
    // Ground plane OFF → no shadow at all (per the task contract).
    if (!scene.showGroundPlane) return;
    if (stroke.material == CanvasMaterial.flat ||
        stroke.material == CanvasMaterial.cutout ||
        stroke.material == CanvasMaterial.guide) {
      // Flat, cutout & guide materials don't cast a 3D shadow.
      return;
    }
    if (stroke.points.length < 2) return;

    final base = HSVColor.fromColor(stroke.color);
    final shadowCol = HSVColor.fromAHSV(
      0.30,
      base.hue,
      (base.saturation * 0.4).clamp(0.0, 1.0),
      0.05,
    ).toColor();
    final shadowPaint = Paint()
      ..color = shadowCol
      ..style = PaintingStyle.stroke
      ..strokeCap = StrokeCap.round
      ..strokeJoin = StrokeJoin.round
      ..strokeWidth = stroke.width * 1.15
      ..maskFilter = const MaskFilter.blur(BlurStyle.normal, 3.0);

    // Ground plane ON → project onto the ground line along lightDir.
    // Use the host's explicit groundY when set; otherwise compute a
    // stable line at 80% of the viewport height (in scene-local coords
    // so it lands at the right screen y given pan / zoom).
    final groundY = scene.groundY ?? _groundLineY(size);

    // Project every point onto the ground line along the light
    // direction. Solving for the intersection of the ray
    //   (P + t * lightDir) .y == groundY
    // gives t = (groundY - P.y) / lightDir.y. If lightDir.y is ~0 we
    // fall back to a vertical drop.
    final lLen = scene.lightDir.distance;
    final ld = lLen < 1e-6 ? const Offset(0.7, 0.7) : scene.lightDir / lLen;
    final dy = ld.dy.abs() < 1e-3 ? 1.0 : ld.dy;
    final projected = <Offset>[];
    for (final p in stroke.points) {
      final t = (groundY - p.dy) / dy;
      projected.add(Offset(p.dx + ld.dx * t, groundY));
    }
    if (projected.length < 2) return;
    final path = Path()..moveTo(projected.first.dx, projected.first.dy);
    for (final p in projected.skip(1)) {
      path.lineTo(p.dx, p.dy);
    }
    canvas.drawPath(path, shadowPaint);
  }

  /// Computes the ground-line Y in scene-local (transformed) pixels so
  /// that it lands at 80% of the viewport height in screen pixels,
  /// accounting for the current pan / zoom. Used as a fallback when
  /// the host doesn't pass an explicit [CanvasScene.groundY].
  double _groundLineY(Size size) {
    if (scene.zoom == 0.0) return size.height * 0.8;
    return (size.height * 0.8 - scene.pan.dy) / scene.zoom;
  }

  /// Draws the visible ground plane — a translucent band + a hairline
  /// rule at the project line — so the artist can see where shadows
  /// will land. Drawn OUTSIDE the pan/zoom transform so the plane stays
  /// stable on screen regardless of camera orbit / pan.
  void _paintGroundPlane(Canvas canvas, Size size) {
    final y = size.height * 0.8;
    // Translucent band below the line (reads as "floor").
    canvas.drawRect(
      Rect.fromLTRB(0, y, size.width, size.height),
      Paint()
        ..color = palette.textPrimary.withValues(alpha: 0.05)
        ..style = PaintingStyle.fill,
    );
    // Hairline rule at the project line.
    canvas.drawLine(
      Offset(0, y),
      Offset(size.width, y),
      Paint()
        ..color = palette.textPrimary.withValues(alpha: 0.22)
        ..strokeWidth = 0.75,
    );
  }

  // ── Geometry helpers ─────────────────────────────────────────────────

  /// Builds a [Path] for a stroke's centreline with an optional
  /// screen-space [offset] applied to every vertex.
  Path _strokePath(CanvasStroke stroke, {Offset offset = Offset.zero}) {
    final path = Path();
    if (stroke.points.isEmpty) return path;
    final first = stroke.points.first + offset;
    path.moveTo(first.dx, first.dy);
    for (final p in stroke.points.skip(1)) {
      final q = p + offset;
      path.lineTo(q.dx, q.dy);
    }
    return path;
  }

  /// Computes per-vertex tangent, normal, lit-sign and half-width.
  /// Returns null if the stroke is degenerate (all coincident points).
  ({
    List<Offset> tangents,
    List<Offset> normals,
    List<double> litSigns,
    List<double> halfWidths,
  })? _strokeGeometry(CanvasStroke stroke) {
    final pts = stroke.points;
    final n = pts.length;
    if (n < 2) return null;

    // Normalize the light direction.
    final lLen = scene.lightDir.distance;
    final lightDir = lLen < 1e-6
        ? const Offset(0.7, 0.7)
        : scene.lightDir / lLen;

    // Per-vertex tangents — averaged from the two adjacent segments at
    // interior vertices so the offset polylines stay smooth.
    final tangents = <Offset>[];
    for (int i = 0; i < n; i++) {
      Offset t;
      if (i == 0) {
        t = pts[1] - pts[0];
      } else if (i == n - 1) {
        t = pts[n - 1] - pts[n - 2];
      } else {
        t = (pts[i + 1] - pts[i - 1]);
      }
      final len = t.distance;
      t = len < 1e-6 ? const Offset(1, 0) : t / len;
      tangents.add(t);
    }

    // Left-hand perpendicular normals.
    final normals =
        tangents.map((t) => Offset(-t.dy, t.dx)).toList(growable: false);

    // Lit sign: +1 if the normal points TOWARDS the light source
    // (i.e. opposite to the direction light travels), -1 otherwise.
    final litSigns = <double>[];
    for (final nrm in normals) {
      // dot(n, -lightDir) > 0 → facing the light.
      final s = -(nrm.dx * lightDir.dx + nrm.dy * lightDir.dy);
      litSigns.add(s >= 0 ? 1.0 : -1.0);
    }

    // Per-vertex half-width — perspective modulation when depths are
    // provided. The reference depth is the stroke's mean depth so the
    // average width matches [CanvasStroke.width].
    final halfWidths = <double>[];
    if (stroke.depths.isEmpty) {
      final half = stroke.width * 0.5;
      for (int i = 0; i < n; i++) {
        halfWidths.add(half);
      }
    } else {
      double mean = 0;
      var count = 0;
      for (final d in stroke.depths) {
        if (d > 0.0) {
          mean += d;
          count++;
        }
      }
      mean = count > 0 ? mean / count : 1.0;
      for (int i = 0; i < n; i++) {
        final d = i < stroke.depths.length ? stroke.depths[i] : mean;
        final scale = d > 0.0 ? (mean / d).clamp(0.3, 3.0) : 1.0;
        halfWidths.add(stroke.width * 0.5 * scale);
      }
    }

    return (
      tangents: tangents,
      normals: normals,
      litSigns: litSigns,
      halfWidths: halfWidths,
    );
  }

  /// Builds a polyline by offsetting each vertex along the local
  /// normal. [side] selects which side of the centreline (+1 = along
  /// the normal, -1 = against it); [factor] scales the offset relative
  /// to the per-vertex half-width.
  Path _offsetPolyline(
    List<Offset> pts,
    List<Offset> normals,
    List<double> litSigns,
    List<double> halfWidths, {
    required double side,
    required double factor,
  }) {
    final path = Path();
    if (pts.isEmpty) return path;
    for (int i = 0; i < pts.length; i++) {
      // The "lit side" of the tube is `+litSign * normal`; the shadow
      // side is the opposite. We multiply by `side` so callers can
      // request the lit side (+1) or shadow side (-1).
      final dir = side * litSigns[i];
      final off = normals[i] * (dir * halfWidths[i] * factor);
      final p = pts[i] + off;
      if (i == 0) {
        path.moveTo(p.dx, p.dy);
      } else {
        path.lineTo(p.dx, p.dy);
      }
    }
    return path;
  }

  // ── Background helpers ───────────────────────────────────────────────

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

  /// Paints the overlay primitives (gizmo / selection highlight / liquify
  /// brush preview) on top of the strokes. The host adapts the engine
  /// renderers' draw-lists into [CanvasOverlay] values; this method just
  /// dispatches on the concrete subtype and draws each primitive with a
  /// fresh [Paint] (no shared mutable state).
  void _paintOverlays(Canvas canvas, List<CanvasOverlay> overlays) {
    for (final o in overlays) {
      final paint = Paint()
        ..color = Color(o.color)
        ..strokeWidth = o.strokeWidth
        ..strokeCap = StrokeCap.round
        ..strokeJoin = StrokeJoin.round
        ..style = PaintingStyle.stroke;
      if (o is CanvasOverlayLine) {
        canvas.drawLine(o.a, o.b, paint);
      } else if (o is CanvasOverlayPolyline) {
        if (o.points.length < 2) continue;
        final path = Path()..moveTo(o.points.first.dx, o.points.first.dy);
        for (final p in o.points.skip(1)) {
          path.lineTo(p.dx, p.dy);
        }
        canvas.drawPath(path, paint);
      } else if (o is CanvasOverlayCircle) {
        if (o.filled) {
          paint.style = PaintingStyle.fill;
        }
        canvas.drawCircle(o.center, o.radius, paint);
      } else if (o is CanvasOverlayTriangle) {
        final path = Path()
          ..moveTo(o.a.dx, o.a.dy)
          ..lineTo(o.b.dx, o.b.dy)
          ..lineTo(o.c.dx, o.c.dy)
          ..close();
        paint.style = PaintingStyle.fill;
        canvas.drawPath(path, paint);
      }
    }
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
