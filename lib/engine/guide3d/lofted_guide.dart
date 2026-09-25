// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// lofted_guide.dart — Loft-mode builder (2+ curves → skinned surface).
//
// Feather docs (3dguide_loft.txt):
//
//   "Select the curves you want to connect in sequence. When you select
//    two or more curves, a preview of the 3D Guide will appear
//    immediately."
//
//   "Tension Slider — Use the slider on the left side of the screen to
//    adjust how much the guide will bend. Slide up for smoother curves
//    and slide down for sharper bends."
//
// Implementation: the selected curves (each a polyline of Vector3) are
// resampled to a common arc-length count, then skinned with
// [GuideMeshGenerators.loftMesh] using a Catmull-Rom interpolation whose
// sharpness is controlled by the tension slider. A live preview path
// ([LoftPreview]) is exposed so the UI can render the in-progress surface
// before the artist taps Done.
//
// The orange starting line is the first cross-edge of the first selected
// curve — Feather draws it so the artist can locate the bend anchor for
// a subsequent Bend-mode pass.

import 'package:feather_krita/core/math/math.dart';

import 'guide3d.dart';
import 'guide3d_type.dart';
import 'guide_mesh_generators.dart';

/// Tunables for the Loft-mode builder.
class LoftGuideParams {
  const LoftGuideParams({
    this.tension = 0.5,
    this.samplesPerCurve = 32,
    this.opacity = 0.5,
  });

  /// 0 = sharp (linear-ish), 1 = smooth (full Catmull-Rom). Matches the
  /// Feather tension slider: slide up for smoother, down for sharper.
  final double tension;

  /// How many vertices to sample along each curve. Higher = smoother
  /// surface at the cost of more triangles.
  final int samplesPerCurve;

  /// Default opacity for the new guide.
  final double opacity;
}

/// A live, in-progress loft preview. Returned by
/// [LoftedGuideBuilder.buildPreview] while the artist is still selecting
/// curves; converted to a full [Guide3D] by [build] once Done is tapped.
class LoftPreview {
  LoftPreview({
    required this.curves,
    required this.params,
    required this.mesh,
    required this.startPoint,
    required this.startEnd,
    required this.ready,
  });

  /// Curves selected so far (in selection order).
  final List<List<Vector3>> curves;

  /// Params used to build this preview.
  final LoftGuideParams params;

  /// Tessellated preview mesh (rebuilt on every selection change).
  final Guide3DMesh mesh;

  /// Local-space orange starting line endpoints.
  final Vector3 startPoint;
  final Vector3 startEnd;

  /// False until two or more curves have been selected — matches the
  /// Feather docs ("When you select two or more curves, a preview …
  /// will appear immediately").
  final bool ready;
}

/// Builds a [Guide3D] of type [Guide3DType.lofted] from a sequence of
/// curves.
class LoftedGuideBuilder {
  const LoftedGuideBuilder();

  /// Builds a live preview from the curves selected so far.
  ///
  /// Returns a [LoftPreview] with [LoftPreview.ready] = false when fewer
  /// than two curves are selected. The preview's mesh is rebuilt on every
  /// call so the UI can re-render as the tension slider moves.
  LoftPreview buildPreview({
    required List<List<Vector3>> curves,
    LoftGuideParams params = const LoftGuideParams(),
  }) {
    if (curves.length < 2) {
      return LoftPreview(
        curves: curves.map((c) => List<Vector3>.of(c)).toList(),
        params: params,
        mesh: Guide3DMesh(
            positions: [Vector3.zero()],
            uvs: [Vector2.zero()],
            indices: const []),
        startPoint: Vector3.zero(),
        startEnd: Vector3.zero(),
        ready: false,
      );
    }
    final mesh = GuideMeshGenerators.loftMesh(
      curves: curves,
      samplesPerCurve: params.samplesPerCurve,
      tension: params.tension,
    );
    final start = _firstCurveStartEdge(mesh, params.samplesPerCurve);
    return LoftPreview(
      curves: curves.map((c) => List<Vector3>.of(c)).toList(),
      params: params,
      mesh: mesh,
      startPoint: start.first,
      startEnd: start.second,
      ready: true,
    );
  }

  /// Finalises a lofted guide from the selected curves. Called when the
  /// artist taps Done in the bottom context menu.
  Guide3D build({
    required List<List<Vector3>> curves,
    LoftGuideParams params = const LoftGuideParams(),
    String? name,
  }) {
    if (curves.length < 2) {
      return _fallbackGuide(params, name);
    }
    final mesh = GuideMeshGenerators.loftMesh(
      curves: curves,
      samplesPerCurve: params.samplesPerCurve,
      tension: params.tension,
    );
    final start = _firstCurveStartEdge(mesh, params.samplesPerCurve);
    // Stash the source curves in meta so a reload can rebuild the mesh.
    final meta = <String, dynamic>{
      'mode': 'loft',
      'tension': params.tension,
      'samplesPerCurve': params.samplesPerCurve,
      'curveCount': curves.length,
      'curves': curves
          .map((c) => c
              .map((p) => <double>[p.x, p.y, p.z])
              .toList())
          .toList(),
    };
    return Guide3D(
      type: Guide3DType.lofted,
      mesh: mesh,
      startPoint: start.first,
      startEnd: start.second,
      opacity: params.opacity,
      name: name,
      meta: meta,
    );
  }

  /// Restores a lofted guide from JSON. Rebuilds the mesh from the
  /// stored curves and tension.
  Guide3D fromJson(Map<String, dynamic> json) {
    final meta = (json['meta'] as Map?)?.cast<String, dynamic>() ?? {};
    final rawCurves = (meta['curves'] as List?)
            ?.map((c) => (c as List)
                .map((p) => Vector3(
                      (p as List)[0] as double,
                      p[1] as double,
                      p[2] as double,
                    ))
                .toList())
            .toList() ??
        <List<Vector3>>[];
    final params = LoftGuideParams(
      tension: (meta['tension'] as num?)?.toDouble() ?? 0.5,
      samplesPerCurve:
          (meta['samplesPerCurve'] as num?)?.toInt() ?? 32,
      opacity: (json['opacity'] as num?)?.toDouble() ?? 0.5,
    );
    final guide = build(
      curves: rawCurves,
      params: params,
      name: json['name'] as String?,
    );
    guide.applyJson(json);
    return guide;
  }

  /// Rebuilds a lofted guide with a new tension value. Used by the
  /// tension slider's "apply" step after Done has been tapped.
  Guide3D withTension(Guide3D guide, double tension) {
    final curves = _extractCurves(guide);
    final samples = (guide.meta['samplesPerCurve'] as num?)?.toInt() ?? 32;
    return build(
      curves: curves,
      params: LoftGuideParams(
        tension: tension,
        samplesPerCurve: samples,
        opacity: guide.opacity,
      ),
      name: guide.name,
    )..transform = guide.transform.clone();
  }

  /// Returns the first cross-edge of the first curve — the orange
  /// starting line for a lofted guide.
  ///
  /// The loft mesh lays out vertices curve-major (curve 0 first, then
  /// curve 1, …) with [samplesPerCurve] vertices per curve. The first
  /// cross-edge of the surface is the edge connecting vertex 0 and
  /// vertex (samplesPerCurve) — i.e. the start of the first selected
  /// curve, spanned across to the second curve. Feather draws this edge
  /// in orange.
  ({Vector3 first, Vector3 second}) _firstCurveStartEdge(
      Guide3DMesh mesh, int samplesPerCurve) {
    if (mesh.positions.length < samplesPerCurve + 1) {
      final p = mesh.positions.isEmpty ? Vector3.zero() : mesh.positions.first;
      return (first: p.clone(), second: p.clone());
    }
    return (
      first: mesh.positions[0].clone(),
      second: mesh.positions[samplesPerCurve].clone(),
    );
  }

  List<List<Vector3>> _extractCurves(Guide3D guide) {
    final raw = guide.meta['curves'] as List?;
    if (raw == null) return const [];
    return raw
        .map((c) => (c as List)
            .map((p) => Vector3(
                  (p as List)[0] as double,
                  p[1] as double,
                  p[2] as double,
                ))
            .toList())
        .toList();
  }

  Guide3D _fallbackGuide(LoftGuideParams params, String? name) {
    final a = [Vector3(-0.5, 0, -0.5), Vector3(0.5, 0, -0.5)];
    final b = [Vector3(-0.5, 0, 0.5), Vector3(0.5, 0, 0.5)];
    final mesh = GuideMeshGenerators.loftMesh(
      curves: [a, b],
      samplesPerCurve: params.samplesPerCurve,
      tension: params.tension,
    );
    final start = _firstCurveStartEdge(mesh, params.samplesPerCurve);
    return Guide3D(
      type: Guide3DType.lofted,
      mesh: mesh,
      startPoint: start.first,
      startEnd: start.second,
      opacity: params.opacity,
      name: name,
      meta: const <String, dynamic>{'mode': 'loft', 'fallback': true},
    );
  }
}
