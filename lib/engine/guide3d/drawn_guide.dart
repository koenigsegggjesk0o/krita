// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// drawn_guide.dart — Draw-mode builder (pen stroke → translucent surface).
//
// Feather docs (3dguide_draw.txt):
//
//   "Tap the Draw icon and set the bottom context menu to Draw. Then, use
//    your pen to draw on the screen, and the 3D Guide will be created.
//    The 3D Guide is generated perpendicular to your viewing angle and
//    varies based on your Field of View(FOV)."
//
// Concretely: every sample of the pen stroke carries (a) a world-space
// position on the camera's near plane (or a hit point in the scene) and
// (b) the camera's view direction at that sample. The guide is a ribbon
// whose centerline is the stroke and whose surface normal is the view
// direction. The cross-ribbon width is scaled by the camera FOV so a
// narrow FOV (zoomed-in) produces a thin guide and a wide FOV (fisheye)
// produces a broad one.
//
// The first cross-ribbon edge of the ribbon is the orange starting line —
// Feather draws it on every Draw-mode guide so the artist can locate the
// bend anchor for the Bend mode.

import 'dart:math' as math;

import 'package:feather_krita/core/math/math.dart';

import 'guide3d.dart';
import 'guide_mesh_generators.dart';
import 'guide3d_type.dart';

/// One sample of a pen stroke captured for Draw mode.
///
/// The [viewPoint] is where the stroke sits in world space (typically the
/// near-plane intersection of the camera ray at the sample). The
/// [viewDirection] is the camera's forward vector at that sample — the
/// generated surface will be perpendicular to it. [pressure] is the
/// stylus pressure (currently informational; future versions could scale
/// the ribbon width with pressure for tapered guides).
class DrawStrokeSample {
  const DrawStrokeSample({
    required this.viewPoint,
    required this.viewDirection,
    this.pressure = 0.5,
  });

  final Vector3 viewPoint;
  final Vector3 viewDirection;
  final double pressure;
}

/// Tunables for the Draw-mode builder.
class DrawGuideParams {
  const DrawGuideParams({
    this.baseWidth = 1.2,
    this.fovDegrees = 45.0,
    this.rows = 2,
    this.opacity = 0.5,
    this.smoothing = 0.5,
  });

  /// Cross-ribbon width at a 45° FOV. The actual width scales with
  /// tan(fov/2) so a wider FOV produces a wider guide.
  final double baseWidth;

  /// Camera Field of View in degrees. Feather ties the guide width to
  /// this value — the slider on the left of the screen can override it
  /// per-stroke.
  final double fovDegrees;

  /// Number of vertices across the ribbon. 2 = single quad strip; higher
  /// values let the ribbon bend in its cross-section.
  final int rows;

  /// Default opacity for the new guide.
  final double opacity;

  /// 0 = raw stroke, 1 = heavily smoothed. The centerline is passed
  /// through a Catmull-Rom smoother before tessellation so a jittery pen
  /// still produces a clean surface.
  final double smoothing;

  /// Effective width given the FOV. Width ∝ tan(fov/2).
  double get effectiveWidth {
    final tanHalf = math.tan((fovDegrees * kDegToRad) * 0.5);
    return baseWidth * tanHalf / math.tan(kDegToRad * 22.5);
  }
}

/// Builds a [Guide3D] of type [Guide3DType.drawn] from a pen stroke.
class DrawnGuideBuilder {
  const DrawnGuideBuilder();

  /// Builds a drawn guide from [samples].
  ///
  /// The samples are first smoothed (if [params.smoothing] > 0), then
  /// passed to [GuideMeshGenerators.ribbonMesh] with the view-normal
  /// averaged across the stroke. The ribbon's first cross-edge (vertices
  /// 0 → 1) becomes the orange starting line.
  Guide3D build({
    required List<DrawStrokeSample> samples,
    DrawGuideParams params = const DrawGuideParams(),
    String? name,
  }) {
    if (samples.length < 2) {
      return _fallbackGuide(params, name);
    }
    final centerline = _smoothCenterline(
      samples.map((s) => s.viewPoint.clone()).toList(),
      params.smoothing,
    );
    // Average view direction — the ribbon normal.
    final viewNormal = Vector3.zero();
    var count = 0;
    for (final s in samples) {
      final d = s.viewDirection;
      if (d.length2 > 1e-9) {
        viewNormal.add(d);
        count++;
      }
    }
    if (count == 0 || viewNormal.length2 < 1e-9) {
      viewNormal.setValues(0, 0, 1);
    } else {
      viewNormal.scale(1.0 / count);
    }
    viewNormal.normalize();

    final mesh = GuideMeshGenerators.ribbonMesh(
      centerline: centerline,
      width: params.effectiveWidth,
      viewNormal: viewNormal,
      rows: params.rows,
    );

    // Orange start line = first cross-edge of the ribbon (vertices 0, 1).
    final startA = mesh.positions.isNotEmpty ? mesh.positions[0].clone() : Vector3.zero();
    final startB = mesh.positions.length > 1
        ? mesh.positions[1].clone()
        : startA.clone();

    return Guide3D(
      type: Guide3DType.drawn,
      mesh: mesh,
      startPoint: startA,
      startEnd: startB,
      opacity: params.opacity,
      name: name,
      meta: <String, dynamic>{
        'mode': 'draw',
        'baseWidth': params.baseWidth,
        'fov': params.fovDegrees,
        'rows': params.rows,
        'smoothing': params.smoothing,
        'sampleCount': samples.length,
      },
    );
  }

  /// Restores a drawn guide from JSON. The mesh is rebuilt from the
  /// stored centerline (carried in [meta]) so a re-tessellation after a
  /// parameters change is possible.
  Guide3D fromJson(Map<String, dynamic> json) {
    final meta = (json['meta'] as Map?)?.cast<String, dynamic>() ?? {};
    final rawCenterline = (meta['centerline'] as List?)
            ?.map((e) => Vector3(
                  (e as List)[0] as double,
                  e[1] as double,
                  e[2] as double,
                ))
            .toList() ??
        <Vector3>[];
    final rawNormal = meta['viewNormal'] as List?;
    final viewNormal = rawNormal == null
        ? Vector3(0, 0, 1)
        : Vector3(
            (rawNormal[0] as num).toDouble(),
            (rawNormal[1] as num).toDouble(),
            (rawNormal[2] as num).toDouble(),
          );
    final params = DrawGuideParams(
      baseWidth: (meta['baseWidth'] as num?)?.toDouble() ?? 1.2,
      fovDegrees: (meta['fov'] as num?)?.toDouble() ?? 45.0,
      rows: (meta['rows'] as num?)?.toInt() ?? 2,
      opacity: (json['opacity'] as num?)?.toDouble() ?? 0.5,
      smoothing: (meta['smoothing'] as num?)?.toDouble() ?? 0.5,
    );
    final guide = build(
      samples: rawCenterline.isEmpty
          ? const []
          : rawCenterline
              .map((p) => DrawStrokeSample(
                    viewPoint: p,
                    viewDirection: viewNormal,
                  ))
              .toList(),
      params: params,
      name: json['name'] as String?,
    );
    guide.applyJson(json);
    return guide;
  }

  /// Serialises the centerline + view normal into [meta] so a reload can
  /// rebuild the mesh. Call after [build] and before persistence.
  static Map<String, dynamic> serializeMeta(
    Guide3D guide, {
    required List<Vector3> centerline,
    required Vector3 viewNormal,
  }) {
    return <String, dynamic>{
      ...guide.meta,
      'centerline': centerline
          .map((p) => <double>[p.x, p.y, p.z])
          .toList(),
      'viewNormal': [viewNormal.x, viewNormal.y, viewNormal.z],
    };
  }

  Guide3D _fallbackGuide(DrawGuideParams params, String? name) {
    final a = Vector3(-0.5, 0, 0);
    final b = Vector3(0.5, 0, 0);
    final mesh = GuideMeshGenerators.ribbonMesh(
      centerline: [a, b],
      width: params.effectiveWidth,
      viewNormal: Vector3(0, 0, 1),
      rows: params.rows,
    );
    return Guide3D(
      type: Guide3DType.drawn,
      mesh: mesh,
      startPoint: mesh.positions[0].clone(),
      startEnd: mesh.positions[1].clone(),
      opacity: params.opacity,
      name: name,
      meta: const <String, dynamic>{'mode': 'draw', 'fallback': true},
    );
  }

  /// Catmull-Rom smoothing of a polyline. [amount] in [0, 1] blends
  /// between the raw points and a four-neighbour Catmull-Rom pass —
  /// 0 returns the input unchanged, 1 produces the fully smoothed curve.
  static List<Vector3> _smoothCenterline(List<Vector3> pts, double amount) {
    if (pts.length < 4 || amount <= 0.0) return pts;
    final out = <Vector3>[];
    final n = pts.length;
    for (var i = 0; i < n; i++) {
      final p0 = pts[(i - 1).clamp(0, n - 1)];
      final p1 = pts[i];
      final p2 = pts[(i + 1).clamp(0, n - 1)];
      final p3 = pts[(i + 2).clamp(0, n - 1)];
      // Catmull-Rom evaluated at t=0.5 yields a centred blend of the
      // four neighbours; blend with the raw point by [amount].
      final cr = p1 * 0.5 +
          (p2 - p0) * 0.25 +
          (p0 * 2 - p1 * 5 + p2 * 4 - p3) * 0.125 +
          (-p0 + p1 * 3 - p2 * 3 + p3) * 0.0625;
      out.add(p1 + (cr - p1) * amount);
    }
    return out;
  }
}
