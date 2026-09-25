// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// guide_renderer.dart — Batches a [Guide3D] into GPU-ready render data.
//
// The renderer is the bridge between the logical guide (mesh + transform
// + opacity) and the canvas's draw layer. For each guide it produces:
//
//   - [GuideSurfaceBatch]   : translucent triangle strip (the surface
//                              itself), with opacity baked into the
//                              vertex colour and double-sided winding.
//   - [GuideGridBatch]      : iso-parametric line strip (the section grid
//                              Feather paints on every guide so the
//                              artist can see the surface curvature).
//   - [GuideStartLineBatch] : the orange starting line — a thick segment
//                              from [Guide3D.startPoint] to
//                              [Guide3D.startEnd] in world space.
//
// "Both sides drawable" contract: a guide is a translucent surface but a
// curve painted on the *far* side cannot be selected (the guide occludes
// it). [cullOccludedCurves] classifies each drawn curve as front-facing
// (selectable) or back-facing (occluded) against a camera position so the
// snap layer can de-select the occluded ones.

import 'package:feather_krita/core/math/math.dart';

import 'guide3d.dart';
import 'guide_uv.dart';

/// Vertex format pushed to the GPU. Position is pre-transformed to world
/// space; the canvas's vertex shader is expected to apply the view-
/// projection matrix only.
class GuideVertex {
  GuideVertex(this.position, this.color, this.uv);
  final Vector3 position;
  final Vector4 color; // RGBA, premultiplied alpha.
  final Vector2 uv;
}

/// One draw batch. [mode] selects the primitive type (triangles / lines).
class GuideBatch {
  GuideBatch({required this.mode, required this.vertices, required this.indices});
  final GuideBatchMode mode;
  final List<GuideVertex> vertices;
  final List<int> indices;
}

/// Primitive topology a [GuideBatch] is rendered with.
enum GuideBatchMode { triangles, lines }

/// Translucent surface batch + grid + orange start line for one guide.
class GuideRenderData {
  GuideRenderData({
    required this.surface,
    required this.grid,
    required this.startLine,
    required this.guideId,
  });
  final GuideBatch surface;
  final GuideBatch grid;
  final GuideBatch startLine;
  final int guideId;
}

/// Classification of a drawn curve against the camera — used to enforce
/// the "curves hidden by the guide cannot be selected" rule.
enum CurveVisibility { front, occluded, edgeOn }

/// Renders [Guide3D]s into [GuideRenderData] batches.
class Guide3DRenderer {
  Guide3DRenderer();

  /// Default translucent surface tint (Feather's pale blue).
  static const int defaultSurfaceColor = 0xFF8FB8FF;

  /// Orange used for the starting line.
  static const int orangeStartColor = 0xFFFF8A00;

  /// Grid line tint (slightly darker than the surface).
  static const int gridLineColor = 0xFFB8D0FF;

  /// Renders a single guide. The [guideId] is opaque to the renderer —
  /// it is stamped onto the [GuideRenderData] so the canvas can route
  /// pick results back to the manager.
  GuideRenderData? render(Guide3D guide, {int guideId = 0}) {
    if (!guide.visible || guide.mesh.positions.isEmpty) return null;
    final surface = _buildSurfaceBatch(guide);
    final grid = _buildGridBatch(guide);
    final startLine = _buildStartLineBatch(guide);
    return GuideRenderData(
      surface: surface,
      grid: grid,
      startLine: startLine,
      guideId: guideId,
    );
  }

  /// Renders every guide in [guides] in order. Guides with no mesh or
  /// [Guide3D.visible] = false are skipped.
  List<GuideRenderData> renderAll(List<Guide3D> guides) {
    final out = <GuideRenderData>[];
    for (var i = 0; i < guides.length; i++) {
      final data = render(guides[i], guideId: i);
      if (data != null) out.add(data);
    }
    return out;
  }

  // ----- Surface --------------------------------------------------------

  GuideBatch _buildSurfaceBatch(Guide3D guide) {
    final verts = <GuideVertex>[];
    final indices = <int>[];
    final baseColor = _unpackColor(defaultSurfaceColor);
    final tint = Vector4(
      baseColor.x,
      baseColor.y,
      baseColor.z,
      guide.opacity,
    );
    for (var i = 0; i < guide.mesh.positions.length; i++) {
      final world = guide.transform.transform3(guide.mesh.positions[i].clone());
      verts.add(GuideVertex(world, tint.clone(), guide.mesh.uvs[i].clone()));
    }
    indices.addAll(guide.mesh.indices);
    return GuideBatch(mode: GuideBatchMode.triangles, vertices: verts, indices: indices);
  }

  // ----- Grid -----------------------------------------------------------

  GuideBatch _buildGridBatch(Guide3D guide) {
    final verts = <GuideVertex>[];
    final indices = <int>[];
    final tint = Vector4(
      _unpackColor(gridLineColor).x,
      _unpackColor(gridLineColor).y,
      _unpackColor(gridLineColor).z,
      (guide.opacity * 0.8).clamp(0.0, 1.0),
    );
    // Use the UV grid when the mesh is laid out as a regular grid
    // (drawn / lofted). Fall back to the edge-walk for primitives.
    final rows = _inferRowCount(guide);
    final cols = _inferColCount(guide);
    List<GridLineEdge> edges;
    if (rows > 1 && cols > 1 && guide.mesh.positions.length == rows * cols) {
      edges = GuideUv.gridLineSegments(rows, cols, uLines: 8, vLines: 8);
    } else {
      edges = GuideUv.gridLineSegmentsFromIndices(guide.mesh.indices, stride: 1);
    }
    final indexMap = <int, int>{};
    for (final e in edges) {
      for (final v in [e.a, e.b]) {
        if (indexMap.containsKey(v)) continue;
        indexMap[v] = verts.length;
        final world = guide.transform.transform3(guide.mesh.positions[v].clone());
        verts.add(GuideVertex(world, tint.clone(), guide.mesh.uvs[v].clone()));
      }
      indices.addAll([indexMap[e.a]!, indexMap[e.b]!]);
    }
    return GuideBatch(mode: GuideBatchMode.lines, vertices: verts, indices: indices);
  }

  // ----- Orange start line ---------------------------------------------

  GuideBatch _buildStartLineBatch(Guide3D guide) {
    final a = guide.startLineWorldStart;
    final b = guide.startLineWorldEnd;
    final tint = _unpackColor(orangeStartColor);
    return GuideBatch(
      mode: GuideBatchMode.lines,
      vertices: [
        GuideVertex(a, Vector4(tint.x, tint.y, tint.z, 1.0), Vector2.zero()),
        GuideVertex(b, Vector4(tint.x, tint.y, tint.z, 1.0), Vector2(1.0, 0.0)),
      ],
      indices: [0, 1],
    );
  }

  // ----- Curve visibility (both sides drawable) ------------------------

  /// Classifies each drawn curve on [guide] against [cameraPosition].
  ///
  /// A curve is [CurveVisibility.front] when its average surface normal
  /// faces the camera; [CurveVisibility.occluded] when it faces away
  /// (the guide body is between the curve and the camera); and
  /// [CurveVisibility.edgeOn] when the normal is perpendicular to the
  /// view ray (degenerate — treated as front so the artist can still
  /// pick it).
  List<CurveVisibility> cullOccludedCurves(
    Guide3D guide,
    Vector3 cameraPosition,
  ) {
    final out = <CurveVisibility>[];
    for (final curve in guide.drawnCurves) {
      if (curve.points.isEmpty) {
        out.add(CurveVisibility.front);
        continue;
      }
      // Average the surface normal at each curve sample (looked up via
      // the stored UV) and dot with the view ray.
      var acc = Vector3.zero();
      var count = 0;
      for (var i = 0; i < curve.points.length; i++) {
        final local = curve.points[i];
        // Approximate the surface normal by finding the nearest
        // triangle's normal — cheaper than a UV lookup, sufficient for
        // visibility classification.
        final n = _approximateNormal(guide, local);
        if (n == null) continue;
        final worldN = VectorMathUtils.transformNormal(guide.transform, n)
            .normalized();
        acc.add(worldN);
        count++;
      }
      if (count == 0) {
        out.add(CurveVisibility.front);
        continue;
      }
      acc.scale(1.0 / count);
      final avgWorld = guide.transform.transform3(
        _avgPoint(curve.points).clone(),
      );
      final view = (avgWorld - cameraPosition)..normalize();
      final d = acc.dot(view);
      if (d > 0.2) {
        out.add(CurveVisibility.occluded);
      } else if (d < -0.2) {
        out.add(CurveVisibility.front);
      } else {
        out.add(CurveVisibility.edgeOn);
      }
    }
    return out;
  }

  Vector3 _avgPoint(List<Vector3> pts) {
    final out = Vector3.zero();
    for (final p in pts) {
      out.add(p);
    }
    out.scale(1.0 / pts.length);
    return out;
  }

  /// Approximates the surface normal at [localPoint] by finding the
  /// nearest triangle's normal in the guide mesh. Returns `null` when
  /// the mesh is empty.
  Vector3? _approximateNormal(Guide3D guide, Vector3 localPoint) {
    if (guide.mesh.indices.length < 3) return null;
    var bestDist = double.infinity;
    Vector3? bestNormal;
    final centroid = Vector3.zero();
    for (var i = 0; i < guide.mesh.indices.length; i += 3) {
      final v0 = guide.mesh.positions[guide.mesh.indices[i]];
      final v1 = guide.mesh.positions[guide.mesh.indices[i + 1]];
      final v2 = guide.mesh.positions[guide.mesh.indices[i + 2]];
      centroid
        ..setFrom(v0)
        ..add(v1)
        ..add(v2)
        ..scale(1.0 / 3.0);
      final d = (centroid - localPoint).length2;
      if (d < bestDist) {
        bestDist = d;
        final e1 = v1 - v0;
        final e2 = v2 - v0;
        bestNormal = e1.cross(e2)..normalize();
      }
    }
    return bestNormal;
  }

  // ----- Helpers --------------------------------------------------------

  /// Heuristic row count for the grid — assumes a regular rows × cols
  /// layout (drawn / lofted). Returns 0 when the mesh is not a grid.
  int _inferRowCount(Guide3D guide) {
    final n = guide.mesh.positions.length;
    final cols = _inferColCount(guide);
    if (cols == 0) return 0;
    return n ~/ cols;
  }

  int _inferColCount(Guide3D guide) {
    final n = guide.mesh.positions.length;
    // Find the largest divisor of n that is <= 256 and yields a row
    // count >= 2. Cheap and good enough for the grid heuristic.
    for (var c = mathMin(n, 256); c >= 2; c--) {
      if (n % c == 0 && n ~/ c >= 2) return c;
    }
    return 0;
  }

  Vector3 _unpackColor(int argb) {
    final r = ((argb >> 16) & 0xff) / 255.0;
    final g = ((argb >> 8) & 0xff) / 255.0;
    final b = (argb & 0xff) / 255.0;
    return Vector3(r, g, b);
  }
}

int mathMin(int a, int b) => a < b ? a : b;
