// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// brush_renderer.dart — renders a 3D stroke as a material-shaded ribbon.
//
// Takes a [Stroke] (lib/models/stroke.dart — a list of 3D sample points
// with pressure) and a [FeatherMaterial], and produces a back-to-front
// list of [RenderedSegment] quads the canvas painter can draw. Each
// segment is:
//   - perspective-correct width (thickness × pressure × clipW scale),
//   - shaded through the material's [shade] (Shadeless = flat color,
//     Shaded = Lambert + Phong via the light rig, Glow = additive,
//     Cutout = background sample),
//   - optionally pattern-modulated along the stroke's arc-length UV.
//
// The view-facing normal N = tangent × viewDir (the scene pipeline's
// ribbon convention) feeds the Shaded material; the renderer itself
// stays material-agnostic and just builds the [ShadeContext] per
// segment. Camera projection is supplied by [CameraContext.project] so
// this module doesn't duplicate the scene pipeline's matrix math.

import 'dart:ui' show Color, Offset;

import 'package:vector_math/vector_math_64.dart';

import 'package:feather_krita/core/math/vec3.dart' as core
    show Vec3;
import 'package:feather_krita/engine/material/light_rig.dart';
import 'package:feather_krita/engine/material/material.dart';
import 'package:feather_krita/engine/brush/brush_pattern.dart';
import 'package:feather_krita/models/stroke.dart';

/// One shaded ribbon segment, ready for the canvas painter.
class RenderedSegment {
  const RenderedSegment({
    required this.a0,
    required this.a1,
    required this.b0,
    required this.b1,
    required this.color,
    required this.depth,
    required this.additive,
  });

  /// Quad corners (screen space). a0/a1 = one long edge, b0/b1 = the
  /// other, paired by index along the stroke.
  final Offset a0, a1, b0, b1;

  /// Final shaded color (material + lighting + pattern resolved).
  final Color color;

  /// Camera-space depth for back-to-front sorting.
  final double depth;

  /// Whether to composite additively (Glow).
  final bool additive;
}

/// Minimal camera context: world→screen projection + the camera's world
/// position (for view-direction math). [project] returns the screen
/// point AND the clip-space w (perspective divisor) so the renderer can
/// do perspective-correct width scaling.
class CameraContext {
  const CameraContext({
    required this.cameraPos,
    required this.project,
    this.referenceDepth = 6.0,
    this.pixelsPerMm = 4.0,
  });

  /// World-space camera position.
  final Vector3 cameraPos;

  /// Projects a world point to screen space, returning (Offset, clipW).
  final (Offset, double) Function(Vector3 world) project;

  /// The orbit distance the width scale is anchored at (matches the
  /// scene pipeline's `kReferenceDepth` contract — default view = 1.0×).
  final double referenceDepth;

  /// Screen pixels per millimeter at the reference depth.
  final double pixelsPerMm;
}

/// Renders strokes into material-shaded ribbon segments.
class BrushRenderer {
  BrushRenderer({this.patternScale = 0.125});

  /// Pattern UV tiling factor (cells per mm of arc length).
  final double patternScale;

  /// Builds the rendered segments for [stroke]. [light] is required for
  /// Shaded materials; ignored by Shadeless/Glow/Cutout. Segments are
  /// returned in stroke order (caller sorts by [RenderedSegment.depth]
  /// back-to-front if needed).
  List<RenderedSegment> renderStroke(
    Stroke stroke,
    FeatherMaterial material,
    CameraContext cam, {
    MaterialLightRig? light,
    BrushPattern? pattern,
  }) {
    if (stroke.length < 2) return const <RenderedSegment>[];
    final out = <RenderedSegment>[];
    var arcLen = 0.0;
    for (var i = 0; i < stroke.length - 1; i++) {
      final p0 = stroke.worldPosition(i);
      final p1 = stroke.worldPosition(i + 1);
      final tangent = p1 - p0;
      final segLen = tangent.length;
      if (segLen < 1e-6) continue;
      final tDir = tangent / segLen;
      final mid = (p0 + p1) * 0.5;
      final view = cam.cameraPos - mid;
      final viewLen = view.length;
      if (viewLen < 1e-6) continue;
      final vDir = view / viewLen;

      // View-facing ribbon normal: tangent × view (then normalize).
      final nVm = tDir.cross(vDir);
      if (nVm.length2 < 1e-12) continue;
      nVm.normalize();

      // Perspective-correct width: thickness (mm) × pressure ×
      // (referenceDepth / clipW) × pixelsPerMm.
      final press = stroke.points[i].pressure;
      final (s0, w0) = cam.project(p0);
      final (s1, w1) = cam.project(p1);
      final clipW = (w0 + w1) * 0.5;
      if (clipW <= 0) continue;
      final widthScale = (cam.referenceDepth / clipW).clamp(0.12, 5.0);
      final widthPx = stroke.thickness *
          press *
          widthScale *
          cam.pixelsPerMm *
          0.5; // half-width for the offset

      // Screen-space ribbon normal (perpendicular to screen tangent).
      final st = s1 - s0;
      final stLen = st.distance;
      if (stLen < 1e-6) continue;
      final sNormal = Offset(-st.dy, st.dx) / stLen * widthPx;

      // Shade the segment through the material.
      final normal = MaterialLightRig.toVec3(nVm);
      final viewDir = MaterialLightRig.toVec3(vDir);
      final worldPos = core.Vec3(mid.x, mid.y, mid.z);
      final ctx = ShadeContext(
        normal: normal,
        viewDir: viewDir,
        worldPos: worldPos,
        lightRig: light,
        patternU: arcLen * patternScale,
        patternV: 0.0,
      );
      final color = material.shade(ctx);

      out.add(RenderedSegment(
        a0: s0 + sNormal,
        a1: s1 + sNormal,
        b0: s0 - sNormal,
        b1: s1 - sNormal,
        color: color,
        depth: -(cam.cameraPos - mid).length, // farther = more negative
        additive: material.isAdditive,
      ));
      arcLen += segLen;
    }
    return out;
  }

  /// Sorts [segments] back-to-front by depth (ascending = far first).
  static List<RenderedSegment> sortBackToFront(
          Iterable<RenderedSegment> segments) =>
      [...segments]..sort((a, b) => a.depth.compareTo(b.depth));
}
