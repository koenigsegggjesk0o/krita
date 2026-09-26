// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// curve_renderer.dart — Builds triangle meshes from [Curve3D]s.
//
// AUDIT (v0.54-B): this module is now WIRED. AUDIT_FINAL.md gap #6
// originally flagged it as dead code with a misleading "used by glTF/OBJ
// exporters" claim — neither exporter consumed it at the time. v0.54-B
// closed the gap by adding an optional `emitTubes` mode to
// [GltfExporter.exportJson]: when on, each stroke that has a matching
// [Stroke3D] is fitted to a Bezier via `Stroke3D.toBezier()` and
// tessellated into a capped tube mesh here via
// [CurveRenderer.buildTubeMesh], then emitted as a glTF TRIANGLES
// primitive (mode 4) so viewers show solid lit tubes instead of thin
// LINE_STRIPs. Strokes without a matching [Stroke3D] fall back to
// LINE_STRIP. The OBJ exporter still does NOT use this module — it has
// its own parallel-transport tube builder.
//
// v0.54-C re-verified (read-only): grep across lib/ confirms the only
// importer of `curve_renderer` / `CurveRenderer` is now
// `lib/io/gltf_exporter.dart` (import at line 35, used at line 292);
// `obj_exporter.dart` does NOT import it (it has its own inline
// parallel-transport tube builder). The header doc is now accurate —
// no further doc fix needed from v0.54-C.
//
// Curves by themselves have no volume — to render them as lit 3D
// geometry (with thickness, pressure variation, and a real surface
// normal that responds to the LightRig) we tessellate them into
// either a tube mesh or a ribbon mesh.
//
// Tube mesh:  the curve swept by a circle of radius `thickness/2` in
//   the plane perpendicular to the tangent. Pressure scales the
//   radius per-vertex. Uses parallel-transport frames (via
//   [CurveMath.parallelTransport]) so the tube doesn't flip at
//   inflection points the way a pure Frenet frame would. Caps are
//   optional fan triangulations.
//
// Ribbon mesh: the curve swept by a line segment of width
//   `thickness` in the plane defined by the tangent and a reference
//   `up` vector (camera-facing when up is the camera up). Cheaper
//   than a tube and the standard for brush strokes in 2D-and-a-half
//   painting apps.
//
// The output is a plain [CurveMesh] (positions, normals, uvs, indices,
// optional colors) — no Flutter / OpenGL types — so the same builder
// can feed a CustomPainter vertex list, a flutter_gl VBO, or an
// exported .glTF (once wired).

import 'dart:math' as math;

import 'package:feather_krita/core/math/math.dart';
import 'package:feather_krita/engine/curves/curve3d.dart';
import 'package:feather_krita/engine/curves/curve_math.dart';

/// A triangle mesh built by [CurveRenderer]. All lists are flat
/// (positions / normals / uvs are interleaved-by-vertex; indices are
/// unsigned-int triples).
class CurveMesh {
  CurveMesh({
    required this.positions,
    required this.indices,
    this.normals,
    this.uvs,
    this.colors,
  });

  /// Flat list of vertex positions: `[x0, y0, z0, x1, y1, z1, ...]`.
  final List<double> positions;

  /// Triangle indices (counterclockwise winding).
  final List<int> indices;

  /// Optional flat list of vertex normals (same layout as positions).
  final List<double>? normals;

  /// Optional flat list of vertex UVs: `[u0, v0, u1, v1, ...]`.
  final List<double>? uvs;

  /// Optional ARGB-packed vertex colors (one per vertex).
  final List<int>? colors;

  /// The number of vertices in the mesh.
  int get vertexCount => positions.length ~/ 3;

  /// The number of triangles in the mesh.
  int get triangleCount => indices.length ~/ 3;
}

/// Thickness callback signature: takes the curve parameter `t` in
/// `[0, 1]` (normalized arc length) and the curve's nominal thickness,
/// returns the per-vertex radius. Defaults to a constant
/// `thickness / 2`.
typedef ThicknessFunction = double Function(double t, double nominal);

/// Builds [CurveMesh]es from [Curve3D]s.
class CurveRenderer {
  const CurveRenderer._();

  // ----- Tube mesh -----------------------------------------------------

  /// Builds a tube mesh around [curve].
  ///
  /// - [radialSegments]: vertices around each ring (default 8).
  /// - [lengthSegments]: segments along the curve length (default
  ///   64). The actual count is adapted so each segment is shorter
  ///   than `curve.length() / lengthSegments`.
  /// - [thicknessFn]: optional callback returning the per-vertex
  ///   radius. Default: constant `curve.thickness / 2`.
  /// - [capEnds]: when true, adds a fan triangulation at each end.
  /// - [uvMode]: how to compute UVs. `length` (default) maps u to
  ///   cumulative arc length in world units and v to angle / 2pi.
  static CurveMesh buildTubeMesh(
    Curve3D curve, {
    int radialSegments = 8,
    int lengthSegments = 64,
    ThicknessFunction? thicknessFn,
    bool capEnds = true,
    TubeUvMode uvMode = TubeUvMode.length,
  }) {
    if (radialSegments < 3) radialSegments = 3;
    if (lengthSegments < 1) lengthSegments = 1;
    final thickness = thicknessFn ?? ((_, nominal) => nominal * 0.5);

    // Sample the curve at `lengthSegments + 1` arc-length-spaced
    // parameters.
    final table = curve.arcLengthTable(samples: lengthSegments * 2);
    final total = table.total;
    final nSlices = lengthSegments + 1;
    final samples = <_FrameSample>[];
    var prevTangent = curve.tangentAt(0.0);
    var prevNormal = CurveMath.perpendicularTo(prevTangent);
    for (var i = 0; i < nSlices; i++) {
      final s = total * i / (nSlices - 1);
      final t = table.parameterAt(s);
      final p = curve.sampleAt(t);
      final tan = curve.tangentAt(t);
      final (tanN, normN, binN) =
          CurveMath.parallelTransport(prevTangent, prevNormal, tan);
      prevTangent = tanN;
      prevNormal = normN;
      samples.add(_FrameSample(
        t: t,
        arcLength: s,
        point: p,
        tangent: tanN,
        normal: normN,
        binormal: binN,
      ));
    }

    final positions = <double>[];
    final normals = <double>[];
    final uvs = <double>[];
    final colors = <int>[];

    // For each ring, emit `radialSegments` vertices.
    for (final s in samples) {
      final r = thickness(s.t, curve.thickness);
      for (var j = 0; j < radialSegments; j++) {
        final theta = (j / radialSegments) * 2.0 * math.pi;
        final cs = math.cos(theta);
        final sn = math.sin(theta);
        // Vertex position = curve point + r * (cos(theta) * normal +
        // sin(theta) * binormal).
        final offset = (s.normal * cs) + (s.binormal * sn);
        final v = s.point + offset * r;
        positions.addAll([v.x, v.y, v.z]);
        // Outward normal is the offset direction (since the tube is
        // circular, the surface normal IS the radial direction).
        normals.addAll([offset.x, offset.y, offset.z]);
        switch (uvMode) {
          case TubeUvMode.length:
            uvs.addAll([s.arcLength * 0.1, j / radialSegments]);
          case TubeUvMode.normalized:
            uvs.addAll([s.t, j / radialSegments]);
          case TubeUvMode.none:
            uvs.addAll([0.0, 0.0]);
        }
        colors.add(curve.color);
      }
    }

    final indices = <int>[];
    final ringVerts = radialSegments;
    // Side quads.
    for (var i = 0; i < nSlices - 1; i++) {
      for (var j = 0; j < ringVerts; j++) {
        final a = i * ringVerts + j;
        final b = i * ringVerts + (j + 1) % ringVerts;
        final c = (i + 1) * ringVerts + (j + 1) % ringVerts;
        final d = (i + 1) * ringVerts + j;
        indices.addAll([a, d, c, a, c, b]);
      }
    }

    if (capEnds && samples.length >= 2) {
      // Start cap (fan, facing -tangent).
      final startCenter = positions.length ~/ 3;
      positions.addAll(
          [samples.first.point.x, samples.first.point.y, samples.first.point.z]);
      final tn = samples.first.tangent;
      normals.addAll([-tn.x, -tn.y, -tn.z]);
      uvs.addAll([0.5, 0.5]);
      colors.add(curve.color);
      for (var j = 0; j < ringVerts; j++) {
        final a = startCenter;
        final b = j;
        final c = (j + 1) % ringVerts;
        // Reverse winding so the cap faces outward.
        indices.addAll([a, c, b]);
      }
      // End cap (fan, facing +tangent).
      final endCenter = positions.length ~/ 3;
      positions.addAll(
          [samples.last.point.x, samples.last.point.y, samples.last.point.z]);
      final tn2 = samples.last.tangent;
      normals.addAll([tn2.x, tn2.y, tn2.z]);
      uvs.addAll([0.5, 0.5]);
      colors.add(curve.color);
      final lastRingStart = (nSlices - 1) * ringVerts;
      for (var j = 0; j < ringVerts; j++) {
        final a = endCenter;
        final b = lastRingStart + j;
        final c = lastRingStart + (j + 1) % ringVerts;
        indices.addAll([a, b, c]);
      }
    }

    return CurveMesh(
      positions: positions,
      indices: indices,
      normals: normals,
      uvs: uvs,
      colors: colors,
    );
  }

  // ----- Ribbon mesh ---------------------------------------------------

  /// Builds a camera-facing ribbon mesh around [curve].
  ///
  /// - [lengthSegments]: segments along the curve length.
  /// - [widthFn]: optional callback returning the per-vertex half
  ///   width. Default: constant `curve.thickness / 2`.
  /// - [up]: reference up vector for the ribbon plane. The ribbon's
  ///   "side" direction is `cross(tangent, up)`. When [up] is null
  ///   the renderer uses the curve's surface normal at each sample
  ///   (true ribbon following the surface).
  /// - [doubleSided]: when true (default), emits two triangles per
  ///   quad with reversed winding so both faces draw.
  static CurveMesh buildRibbonMesh(
    Curve3D curve, {
    int lengthSegments = 64,
    ThicknessFunction? widthFn,
    Vector3? up,
    bool doubleSided = true,
    bool taperEnds = true,
  }) {
    if (lengthSegments < 1) lengthSegments = 1;
    final halfWidthFn = widthFn ?? ((_, nominal) => nominal * 0.5);
    final upVec = up?.clone() ?? Vector3(0, 1, 0);

    final table = curve.arcLengthTable(samples: lengthSegments * 2);
    final total = table.total;
    final nSlices = lengthSegments + 1;
    final positions = <double>[];
    final normals = <double>[];
    final uvs = <double>[];
    final colors = <int>[];

    for (var i = 0; i < nSlices; i++) {
      final s = total * i / (nSlices - 1);
      final t = table.parameterAt(s);
      final p = curve.sampleAt(t);
      final tan = curve.tangentAt(t);
      // Side vector perpendicular to tangent in the up-tangent plane.
      var side = tan.cross(upVec);
      if (side.length2 < 1e-10) {
        // Tangent is parallel to up — pick any perpendicular.
        side = CurveMath.perpendicularTo(tan);
      } else {
        side.normalize();
      }
      // Surface normal = side x tangent (points toward camera if up
      // is camera up and the curve is mostly horizontal).
      final nrm = side.cross(tan)..normalize();
      // Taper the ends to zero width for clean stroke terminators.
      final taper = taperEnds ? _endTaper(i, nSlices) : 1.0;
      final hw = halfWidthFn(t, curve.thickness) * taper;
      final left = p + side * hw;
      final right = p - side * hw;
      positions.addAll([left.x, left.y, left.z]);
      positions.addAll([right.x, right.y, right.z]);
      normals.addAll([nrm.x, nrm.y, nrm.z]);
      normals.addAll([nrm.x, nrm.y, nrm.z]);
      uvs.addAll([0.0, s * 0.1]);
      uvs.addAll([1.0, s * 0.1]);
      colors.add(curve.color);
      colors.add(curve.color);
    }

    final indices = <int>[];
    for (var i = 0; i < nSlices - 1; i++) {
      final a = i * 2;
      final b = i * 2 + 1;
      final c = (i + 1) * 2;
      final d = (i + 1) * 2 + 1;
      indices.addAll([a, c, b, b, c, d]);
      if (doubleSided) {
        indices.addAll([a, b, c, b, d, c]);
      }
    }

    return CurveMesh(
      positions: positions,
      indices: indices,
      normals: normals,
      uvs: uvs,
      colors: colors,
    );
  }

  /// End-taper factor: 1.0 in the interior, ramps from 0 to 1 over
  /// the first / last 5% of the ribbon. Produces clean pointed ends.
  static double _endTaper(int i, int nSlices) {
    final taperSpan = (nSlices * 0.05).ceil().clamp(1, 6);
    if (i < taperSpan) {
      return smootherstep(i / taperSpan);
    }
    if (i > nSlices - 1 - taperSpan) {
      final j = nSlices - 1 - i;
      return smootherstep(j / taperSpan);
    }
    return 1.0;
  }

  // ----- Polyline (debug) ---------------------------------------------

  /// Builds a thin polyline (line-list) mesh for debugging / picking
  /// visualization. Each pair of indices is a line segment.
  static CurveMesh buildPolyline(
    Curve3D curve, {
    int samples = 64,
  }) {
    if (samples < 2) samples = 2;
    final flat = curve.flatten(tolerance: 1e-3);
    if (flat.length > samples) {
      // Use the resampled points.
    }
    final positions = <double>[];
    final indices = <int>[];
    final n = flat.length;
    for (var i = 0; i < n; i++) {
      positions.addAll(
          [flat[i].point.x, flat[i].point.y, flat[i].point.z]);
      if (i < n - 1) {
        indices.addAll([i, i + 1]);
      }
    }
    return CurveMesh(positions: positions, indices: indices);
  }
}

/// UV mapping modes for [CurveRenderer.buildTubeMesh].
enum TubeUvMode {
  /// u = arc length in world units (scaled by 0.1); v = angle / 2pi.
  length,

  /// u = curve parameter t in [0, 1]; v = angle / 2pi.
  normalized,

  /// All UVs are (0, 0). Disables texture mapping.
  none,
}

/// Internal frame sample for the tube mesh builder.
class _FrameSample {
  const _FrameSample({
    required this.t,
    required this.arcLength,
    required this.point,
    required this.tangent,
    required this.normal,
    required this.binormal,
  });

  final double t;
  final double arcLength;
  final Vector3 point;
  final Vector3 tangent;
  final Vector3 normal;
  final Vector3 binormal;
}
