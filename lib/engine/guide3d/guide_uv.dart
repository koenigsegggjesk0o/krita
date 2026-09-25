// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// guide_uv.dart — UV generation for 3D Guide surfaces.
//
// A 3D Guide is a translucent surface the artist draws curves ON. To map
// pen strokes onto the surface (and back-project world hits to texture
// space) every guide mesh ships with per-vertex UVs in [0, 1]².
//
// The generators below are topology-aware:
//
//   - [planarUv]      : project positions onto a plane (XY / XZ / YZ) and
//                       normalise the bounding box to [0, 1].
//   - [cylindricalUv] : wrap UVs around the Y axis (u = angle, v = height).
//   - [sphericalUv]   : classic spherical parametrisation (u = longitude,
//                       v = latitude).
//   - [ribbonUv]      : for drawn guides — u along the stroke's arc length,
//                       v across the ribbon's FOV-scaled width.
//   - [loftUv]        : for lofted guides — u across the cross-section
//                       curves (one per input curve), v along each curve.
//
// Additionally [gridLineSegments] derives the iso-parametric line strip a
// renderer paints on top of the translucent surface so the artist can see
// the section grid Feather draws on every guide.
//
// All generators are pure and side-effect free; they allocate fresh lists
// so a caller can swap UVs without copying the mesh.

import 'dart:math' as math;

import 'package:feather_krita/core/math/math.dart';

/// Axis a [planarUv] projection drops.
enum PlanarAxis { xy, xz, yz }

/// A pair of indices describing a line segment in a mesh's vertex array,
/// used by [gridLineSegments] to emit the iso-parametric grid.
class GridLineEdge {
  const GridLineEdge(this.a, this.b);
  final int a;
  final int b;
}

/// Pure UV generators for 3D Guide meshes.
class GuideUv {
  const GuideUv._();

  // ----- Planar ---------------------------------------------------------

  /// Generates planar UVs by projecting [positions] onto [axis] and
  /// normalising the bounding box to [0, 1]. Padding insets the UVs by
  /// [padding] (default 0.0) so the surface does not touch the texture
  /// border — useful for stroke painting where the brush bleeds outside
  /// the footprint.
  static List<Vector2> planarUv(
    List<Vector3> positions, {
    PlanarAxis axis = PlanarAxis.xz,
    double padding = 0.0,
  }) {
    if (positions.isEmpty) return const <Vector2>[];
    var minA = double.infinity;
    var maxA = double.negativeInfinity;
    var minB = double.infinity;
    var maxB = double.negativeInfinity;
    for (final p in positions) {
      final a = axis == PlanarAxis.xy
          ? p.x
          : (axis == PlanarAxis.xz ? p.x : p.y);
      final b = axis == PlanarAxis.xy
          ? p.y
          : (axis == PlanarAxis.xz ? p.z : p.z);
      if (a < minA) minA = a;
      if (a > maxA) maxA = a;
      if (b < minB) minB = b;
      if (b > maxB) maxB = b;
    }
    final spanA = (maxA - minA).abs();
    final spanB = (maxB - minB).abs();
    final invA = spanA < 1e-9 ? 0.0 : 1.0 / spanA;
    final invB = spanB < 1e-9 ? 0.0 : 1.0 / spanB;
    final out = List<Vector2>.generate(positions.length, (i) {
      final p = positions[i];
      final a = axis == PlanarAxis.xy
          ? p.x
          : (axis == PlanarAxis.xz ? p.x : p.y);
      final b = axis == PlanarAxis.xy
          ? p.y
          : (axis == PlanarAxis.xz ? p.z : p.z);
      var u = (a - minA) * invA;
      var v = (b - minB) * invB;
      if (padding > 0.0) {
        u = u * (1.0 - 2.0 * padding) + padding;
        v = v * (1.0 - 2.0 * padding) + padding;
      }
      return Vector2(u, v);
    });
    return out;
  }

  // ----- Cylindrical ----------------------------------------------------

  /// Generates cylindrical UVs around the Y axis. [u] is the longitude in
  /// [0, 1] (angle / 2π), [v] is the normalised height. When the mesh is
  /// not centred on the origin the caller should pass [centerY] so the v
  /// range maps cleanly to [0, 1].
  static List<Vector2> cylindricalUv(
    List<Vector3> positions, {
    double centerY = 0.0,
    double? heightHint,
  }) {
    if (positions.isEmpty) return const <Vector2>[];
    var minY = double.infinity;
    var maxY = double.negativeInfinity;
    for (final p in positions) {
      if (p.y < minY) minY = p.y;
      if (p.y > maxY) maxY = p.y;
    }
    final height = heightHint ?? (maxY - minY).abs();
    final invH = height < 1e-9 ? 0.0 : 1.0 / height;
    return List<Vector2>.generate(positions.length, (i) {
      final p = positions[i];
      final angle = math.atan2(p.z, p.x);
      // Map [-π, π] → [0, 1].
      final u = (angle * 0.5 + 0.5 * math.pi) / math.pi;
      final v = (p.y - minY) * invH;
      return Vector2(u.clamp(0.0, 1.0), v.clamp(0.0, 1.0));
    });
  }

  // ----- Spherical ------------------------------------------------------

  /// Generates spherical UVs from positions on a sphere centred at
  /// [center]. [u] = longitude, [v] = latitude.
  static List<Vector2> sphericalUv(
    List<Vector3> positions, {
    Vector3? center,
  }) {
    final c = center ?? Vector3.zero();
    return List<Vector2>.generate(positions.length, (i) {
      final d = positions[i] - c;
      final len = d.length;
      if (len < 1e-9) return Vector2(0.5, 0.5);
      final v = 0.5 - (math.asin((d.y / len).clamp(-1.0, 1.0)) / math.pi);
      var u = 0.5 + (math.atan2(d.z, d.x) / (2.0 * math.pi));
      if (u >= 1.0) u -= 1.0;
      if (u < 0.0) u += 1.0;
      return Vector2(u, v.clamp(0.0, 1.0));
    });
  }

  // ----- Ribbon (drawn guides) -----------------------------------------

  /// Generates UVs for a ribbon mesh built from [rows] x [cols] vertices.
  ///
  /// [u] follows the stroke's arc length (sample [i] of [cols] along the
  /// stroke), [v] follows the cross-ribbon offset (sample [j] of [rows]
  /// across the FOV-scaled width). Both are normalised to [0, 1].
  static List<Vector2> ribbonUv(int rows, int cols) {
    final out = <Vector2>[];
    final invC = cols <= 1 ? 0.0 : 1.0 / (cols - 1);
    final invR = rows <= 1 ? 0.0 : 1.0 / (rows - 1);
    for (var j = 0; j < rows; j++) {
      for (var i = 0; i < cols; i++) {
        out.add(Vector2(i * invC, j * invR));
      }
    }
    return out;
  }

  // ----- Loft -----------------------------------------------------------

  /// Generates UVs for a lofted surface skinned across [curveCount]
  /// curves, each sampled at [samplesPerCurve] vertices.
  ///
  /// [u] indexes the cross-section curves (0 at the first selected curve,
  /// 1 at the last), [v] indexes the position along each curve. This
  /// matches the Feather loft preview where the tension slider reshapes
  /// the u-direction interpolation.
  static List<Vector2> loftUv(int curveCount, int samplesPerCurve) {
    final out = <Vector2>[];
    final invC = curveCount <= 1 ? 0.0 : 1.0 / (curveCount - 1);
    final invS = samplesPerCurve <= 1 ? 0.0 : 1.0 / (samplesPerCurve - 1);
    for (var c = 0; c < curveCount; c++) {
      for (var s = 0; s < samplesPerCurve; s++) {
        out.add(Vector2(c * invC, s * invS));
      }
    }
    return out;
  }

  // ----- Grid lines -----------------------------------------------------

  /// Builds the iso-parametric grid line edges for a regular row x col
  /// vertex grid (as produced by [ribbonUv] / [loftUv]).
  ///
  /// [uLines] sets how many lines run along the u (column) direction,
  /// [vLines] how many run along the v (row) direction. The returned
  /// edges reference the same vertex indices the mesh already uses, so
  /// the renderer can paint them as a single line strip with no extra
  /// vertex data.
  static List<GridLineEdge> gridLineSegments(
    int rows,
    int cols, {
    int uLines = 8,
    int vLines = 8,
  }) {
    final out = <GridLineEdge>[];
    if (rows < 2 || cols < 2) return out;
    // Lines along u (constant v, step across columns).
    final vStep = (vLines <= 0) ? 1 : ((rows - 1) / vLines).floor().clamp(1, rows - 1);
    for (var j = 0; j < rows; j += vStep) {
      for (var i = 0; i < cols - 1; i++) {
        out.add(GridLineEdge(j * cols + i, j * cols + i + 1));
      }
    }
    // Lines along v (constant u, step down rows).
    final uStep = (uLines <= 0) ? 1 : ((cols - 1) / uLines).floor().clamp(1, cols - 1);
    for (var i = 0; i < cols; i += uStep) {
      for (var j = 0; j < rows - 1; j++) {
        out.add(GridLineEdge(j * cols + i, (j + 1) * cols + i));
      }
    }
    return out;
  }

  /// Builds grid edges for an indexed triangle mesh by walking the
  /// shared edges of every triangle. Used for primitive shapes that are
  /// not laid out on a regular grid.
  static List<GridLineEdge> gridLineSegmentsFromIndices(
    List<int> indices, {
    int stride = 1,
  }) {
    final seen = <int>{};
    final out = <GridLineEdge>[];
    for (var t = 0; t < indices.length; t += 3) {
      if (t ~/ 3 % (stride <= 0 ? 1 : stride) != 0) continue;
      final a = indices[t];
      final b = indices[t + 1];
      final c = indices[t + 2];
      _addEdge(seen, out, a, b);
      _addEdge(seen, out, b, c);
      _addEdge(seen, out, c, a);
    }
    return out;
  }

  static void _addEdge(Set<int> seen, List<GridLineEdge> out, int a, int b) {
    final key = a < b ? a * 1000003 + b : b * 1000003 + a;
    if (seen.contains(key)) return;
    seen.add(key);
    out.add(GridLineEdge(a, b));
  }
}
