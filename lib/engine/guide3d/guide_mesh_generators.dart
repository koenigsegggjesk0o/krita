// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// guide_mesh_generators.dart — Triangle mesh generation for every 3D Guide
// topology.
//
// Every guide surface — drawn ribbon, lofted skin, primitive shape or bent
// deformation — ultimately boils down to a [Guide3DMesh] of positions,
// normals, UVs and triangle indices. This module owns the procedural
// geometry so the per-mode builders (drawn_guide / lofted_guide / …) can
// stay focused on the *interaction* contract and delegate the tessellation
// here.
//
// Generators:
//
//   - [cubeMesh]        : per-face subdivided box.
//   - [pyramidMesh]     : n-sided base pyramid (n = segments).
//   - [sphereMesh]      : UV sphere with shared segment/ring count.
//   - [tubeMesh]        : open cylinder shell along Y.
//   - [ribbonMesh]      : strip mesh for drawn guides (rows x cols grid
//                         built from a centerline + cross vector).
//   - [loftMesh]        : skinned surface across n cross-section curves
//                         using Catmull-Rom interpolation with a tension
//                         parameter.
//   - [bendMesh]        : re-roots an existing mesh along a bend path via
//                         arc-length parameterisation — the core of
//                         Feather's Bend mode.
//
// All generators produce CCW-wound triangles in local space; the caller is
// responsible for placing the mesh via [Guide3D.transform].

import 'dart:math' as math;

import 'package:feather_krita/core/math/math.dart';

import 'guide3d_primitive.dart';

/// Triangle mesh payload shared by every [Guide3D].
///
/// Mirrors the smaller [SurfaceMesh] from the legacy guide_surface module
/// but lives in the guide3d package so the two systems can evolve
/// independently. Fields are mutable so the bend pipeline can rewrite
/// positions in place; callers that need a snapshot should [clone].
class Guide3DMesh {
  Guide3DMesh({
    required this.positions,
    required this.uvs,
    required this.indices,
    List<Vector3>? normals,
  }) : normals = normals ?? _computeNormals(positions, indices);

  /// Vertex positions in local space.
  final List<Vector3> positions;

  /// Per-vertex UV coordinates in [0, 1]².
  final List<Vector2> uvs;

  /// Triangle indices (length == triangleCount * 3), CCW winding.
  final List<int> indices;

  /// Per-vertex normals. Lazily computed when not supplied.
  final List<Vector3> normals;

  int get vertexCount => positions.length;
  int get triangleCount => indices.length ~/ 3;

  /// World-space axis-aligned bounding box of all vertices (ignores the
  /// guide's transform — callers should pre-transform if they need a
  /// world-space box).
  Aabb3 get bounds {
    final out = Aabb3();
    for (final p in positions) {
      out.hullPoint(p);
    }
    return out;
  }

  /// Deep copy so a bend deformation can mutate without disturbing the
  /// source guide.
  Guide3DMesh clone() => Guide3DMesh(
        positions: positions.map((p) => p.clone()).toList(),
        uvs: uvs.map((u) => u.clone()).toList(),
        indices: List<int>.of(indices),
        normals: normals.map((n) => n.clone()).toList(),
      );

  /// Recomputes normals from the current positions / indices. Call after
  /// a bend deformation rewrites positions.
  void recomputeNormals() {
    final fresh = _computeNormals(positions, indices);
    normals.clear();
    normals.addAll(fresh);
  }

  static List<Vector3> _computeNormals(
      List<Vector3> positions, List<int> indices) {
    final out = List<Vector3>.generate(
        positions.length, (_) => Vector3.zero());
    final ab = Vector3.zero();
    final ac = Vector3.zero();
    final n = Vector3.zero();
    for (var i = 0; i + 2 < indices.length; i += 3) {
      final i0 = indices[i];
      final i1 = indices[i + 1];
      final i2 = indices[i + 2];
      final v0 = positions[i0];
      final v1 = positions[i1];
      final v2 = positions[i2];
      ab
        ..setFrom(v1)
        ..sub(v0);
      ac
        ..setFrom(v2)
        ..sub(v0);
      n
        ..setFrom(ab)
        ..crossInto(ac, n)
        ..normalize();
      out[i0].add(n);
      out[i1].add(n);
      out[i2].add(n);
    }
    for (final v in out) {
      if (v.length2 > 1e-12) v.normalize();
    }
    return out;
  }
}

/// Procedural mesh generators for 3D Guides.
class GuideMeshGenerators {
  const GuideMeshGenerators._();

  // ----- Cube -----------------------------------------------------------

  /// Builds a cube of [size] centred on the origin with each face
  /// subdivided into [segments]² quads. UVs are planar per face so strokes
  /// paint without distortion across face boundaries.
  static Guide3DMesh cubeMesh({
    double size = 2.0,
    int segments = 1,
  }) {
    size = size.abs();
    segments = segments.clamp(1, Guide3DPrimitive.maxSegments);
    final half = size * 0.5;
    final step = size / segments;
    final positions = <Vector3>[];
    final uvs = <Vector2>[];
    final indices = <int>[];

    void addFace(Vector3 u, Vector3 v, Vector3 normal) {
      final base = positions.length;
      for (var j = 0; j <= segments; j++) {
        for (var i = 0; i <= segments; i++) {
          positions.add(u * (i * step - half) +
              v * (j * step - half) +
              normal.scaled(half));
          uvs.add(Vector2(i / segments, j / segments));
        }
      }
      final cols = segments + 1;
      for (var j = 0; j < segments; j++) {
        for (var i = 0; i < segments; i++) {
          final a = base + j * cols + i;
          final b = a + 1;
          final c = a + cols;
          final d = c + 1;
          indices.addAll([a, c, b, b, c, d]);
        }
      }
    }

    final x = Vector3(1, 0, 0);
    final y = Vector3(0, 1, 0);
    final z = Vector3(0, 0, 1);
    addFace(x, y, z); // +Z face
    addFace(x, y, -z); // -Z face (winding flipped by negation)
    addFace(z, y, -x); // -X face
    addFace(z, y, x); // +X face
    addFace(x, z, y); // +Y face (top)
    addFace(x, z, -y); // -Y face (bottom)

    return Guide3DMesh(positions: positions, uvs: uvs, indices: indices);
  }

  // ----- Pyramid --------------------------------------------------------

  /// Builds a pyramid with an n-sided base (n = [segments], default 4 for
  /// a true square-base pyramid). Apex at +Y, base at -Y.
  static Guide3DMesh pyramidMesh({
    double baseRadius = 1.0,
    double height = 2.0,
    int segments = 4,
  }) {
    baseRadius = baseRadius.abs();
    height = height.abs();
    segments = segments.clamp(3, Guide3DPrimitive.maxSegments);
    final halfH = height * 0.5;
    final positions = <Vector3>[];
    final uvs = <Vector2>[];
    final indices = <int>[];

    final apex = 0;
    positions.add(Vector3(0, halfH, 0));
    uvs.add(Vector2(0.5, 0.0));

    final baseStart = positions.length;
    for (var s = 0; s <= segments; s++) {
      final theta = 2 * math.pi * s / segments;
      final x = baseRadius * math.cos(theta);
      final z = baseRadius * math.sin(theta);
      positions.add(Vector3(x, -halfH, z));
      uvs.add(Vector2(s / segments, 1.0));
    }
    for (var s = 0; s < segments; s++) {
      final a = baseStart + s;
      final b = baseStart + s + 1;
      indices.addAll([apex, a, b]);
    }

    final baseCenter = positions.length;
    positions.add(Vector3(0, -halfH, 0));
    uvs.add(Vector2(0.5, 0.5));
    for (var s = 0; s < segments; s++) {
      final a = baseStart + s;
      final b = baseStart + s + 1;
      indices.addAll([b, baseCenter, a]);
    }
    return Guide3DMesh(positions: positions, uvs: uvs, indices: indices);
  }

  // ----- Sphere ---------------------------------------------------------

  /// Builds a UV sphere of [radius] centred on the origin. [segments]
  /// controls longitude, [rings] controls latitude (defaults to
  /// segments / 2 so the slider reads as a single density knob).
  static Guide3DMesh sphereMesh({
    double radius = 1.0,
    int segments = 24,
    int? rings,
  }) {
    radius = radius.abs();
    segments = segments.clamp(3, Guide3DPrimitive.maxSegments);
    final ringCount = (rings ?? (segments ~/ 2)).clamp(2, Guide3DPrimitive.maxSegments);
    final positions = <Vector3>[];
    final uvs = <Vector2>[];
    final indices = <int>[];

    positions.add(Vector3(0, radius, 0));
    uvs.add(Vector2(0.5, 0.0));
    for (var r = 1; r < ringCount; r++) {
      final phi = math.pi * r / ringCount;
      final sinPhi = math.sin(phi);
      final cosPhi = math.cos(phi);
      final v = r / ringCount;
      for (var s = 0; s <= segments; s++) {
        final theta = 2 * math.pi * s / segments;
        final sinT = math.sin(theta);
        final cosT = math.cos(theta);
        positions.add(Vector3(
            radius * sinPhi * cosT, radius * cosPhi, radius * sinPhi * sinT));
        uvs.add(Vector2(s / segments, v));
      }
    }
    final bottomPole = positions.length;
    positions.add(Vector3(0, -radius, 0));
    uvs.add(Vector2(0.5, 1.0));

    for (var s = 0; s < segments; s++) {
      indices.addAll([0, 1 + s, 1 + s + 1]);
    }
    for (var r = 0; r < ringCount - 2; r++) {
      for (var s = 0; s < segments; s++) {
        final a = 1 + r * (segments + 1) + s;
        final b = a + 1;
        final c = a + (segments + 1);
        final d = c + 1;
        indices.addAll([a, c, b, b, c, d]);
      }
    }
    final lastRingStart = 1 + (ringCount - 2) * (segments + 1);
    for (var s = 0; s < segments; s++) {
      final a = lastRingStart + s;
      final b = a + 1;
      indices.addAll([a, bottomPole, b]);
    }
    return Guide3DMesh(positions: positions, uvs: uvs, indices: indices);
  }

  // ----- Tube -----------------------------------------------------------

  /// Builds an open cylinder shell of [radius] and [height] along the Y
  /// axis. The Segment slider reshapes [segments] — low counts read as a
  /// faceted prism, high counts as a smooth tube. End caps are omitted so
  /// the artist can draw through the openings (matching Feather's Tube).
  static Guide3DMesh tubeMesh({
    double radius = 1.0,
    double height = 2.0,
    int segments = 24,
  }) {
    radius = radius.abs();
    height = height.abs();
    segments = segments.clamp(3, Guide3DPrimitive.maxSegments);
    final halfH = height * 0.5;
    final positions = <Vector3>[];
    final uvs = <Vector2>[];
    final indices = <int>[];

    for (var r = 0; r < 2; r++) {
      final y = r == 0 ? -halfH : halfH;
      final v = r == 0 ? 0.0 : 1.0;
      for (var s = 0; s <= segments; s++) {
        final theta = 2 * math.pi * s / segments;
        positions
            .add(Vector3(radius * math.cos(theta), y, radius * math.sin(theta)));
        uvs.add(Vector2(s / segments, v));
      }
    }
    for (var s = 0; s < segments; s++) {
      final a = s;
      final b = s + 1;
      final c = s + (segments + 1);
      final d = c + 1;
      indices.addAll([a, c, b, b, c, d]);
    }
    return Guide3DMesh(positions: positions, uvs: uvs, indices: indices);
  }

  // ----- Ribbon (drawn guides) -----------------------------------------

  /// Builds a ribbon mesh for the Draw mode.
  ///
  /// [centerline] is the pen stroke sampled in world space. [width] is the
  /// FOV-scaled cross-ribbon extent. The ribbon is laid out as a [cols] x
  /// [rows] grid; [cols] defaults to the centerline length and [rows] to
  /// 2 (a single quad strip) but can be raised for sub-ribbon tessellation.
  /// The returned mesh's first edge (vertices 0 → 1) is the orange start
  /// line — Feather draws the start of every drawn guide as an orange
  /// segment so the artist can locate the bend anchor.
  static Guide3DMesh ribbonMesh({
    required List<Vector3> centerline,
    required double width,
    required Vector3 viewNormal,
    int rows = 2,
    int? cols,
  }) {
    final n = centerline.length;
    if (n < 2) {
      return Guide3DMesh(
          positions: [Vector3.zero()], uvs: [Vector2.zero()], indices: []);
    }
    rows = rows.clamp(2, 64);
    final colCount = cols ?? n;
    final positions = <Vector3>[];
    final uvs = <Vector2>[];

    // Cross vector = viewNormal × tangent, normalised and scaled to width/2.
    final tangent = Vector3.zero();
    final cross = Vector3.zero();
    final up = viewNormal.normalized();
    for (var j = 0; j < rows; j++) {
      final v = j / (rows - 1);
      final offset = (v - 0.5) * width;
      for (var i = 0; i < colCount; i++) {
        final tI = (i / (colCount - 1)) * (n - 1);
        final i0 = tI.floor().clamp(0, n - 2);
        final i1 = (i0 + 1).clamp(0, n - 1);
        final frac = tI - i0;
        final p = centerline[i0] + (centerline[i1] - centerline[i0]) * frac;
        tangent
          ..setFrom(centerline[i1])
          ..sub(centerline[i0])
          ..normalize();
        cross
          ..setFrom(up)
          ..crossInto(tangent, cross)
          ..normalize();
        positions.add(p + cross * offset);
        uvs.add(Vector2(i / (colCount - 1), v));
      }
    }
    final indices = <int>[];
    for (var j = 0; j < rows - 1; j++) {
      for (var i = 0; i < colCount - 1; i++) {
        final a = j * colCount + i;
        final b = a + 1;
        final c = a + colCount;
        final d = c + 1;
        indices.addAll([a, c, b, b, c, d]);
      }
    }
    return Guide3DMesh(positions: positions, uvs: uvs, indices: indices);
  }

  // ----- Loft -----------------------------------------------------------

  /// Builds a lofted surface skinning across [curves].
  ///
  /// Each entry in [curves] is a polyline of Vector3 points (the
  /// already-drawn curves the artist selected in sequence). The surface
  /// is sampled at [samplesPerCurve] vertices along each curve and
  /// [curveCount] = curves.length across. [tension] in [0, 1] controls
  /// the Catmull-Rom interpolation — 0 = sharp (linear), 1 = smooth
  /// (full Catmull-Rom). Mid-values blend the two.
  ///
  /// Curves with differing vertex counts are resampled to a common length
  /// so the skin can stitch cleanly.
  static Guide3DMesh loftMesh({
    required List<List<Vector3>> curves,
    int samplesPerCurve = 32,
    double tension = 0.5,
  }) {
    if (curves.length < 2) {
      return Guide3DMesh(
          positions: [Vector3.zero()], uvs: [Vector2.zero()], indices: []);
    }
    final curveCount = curves.length;
    samplesPerCurve = samplesPerCurve.clamp(2, 256);
    final tensionC = tension.clamp(0.0, 1.0);

    // Resample every curve to samplesPerCurve points (uniform arc length).
    final resampled = List<List<Vector3>>.generate(curveCount, (c) {
      return _resampleCurve(curves[c], samplesPerCurve);
    });

    final positions = <Vector3>[];
    final uvs = <Vector2>[];
    for (var c = 0; c < curveCount; c++) {
      for (var s = 0; s < samplesPerCurve; s++) {
        positions.add(resampled[c][s].clone());
        uvs.add(Vector2(c / (curveCount - 1), s / (samplesPerCurve - 1)));
      }
    }
    final indices = <int>[];
    for (var c = 0; c < curveCount - 1; c++) {
      for (var s = 0; s < samplesPerCurve - 1; s++) {
        final a = c * samplesPerCurve + s;
        final b = a + 1;
        final cc = a + samplesPerCurve;
        final d = cc + 1;
        indices.addAll([a, cc, b, b, cc, d]);
      }
    }

    // If tension requests smoothing, blend each interior cross-section row
    // toward its Catmull-Rom neighbours so the surface rounds across the
    // selected curves instead of kinking at every join.
    if (tensionC > 1e-3 && curveCount >= 3) {
      _smoothLoftCrossSections(positions, samplesPerCurve, curveCount, tensionC);
    }
    return Guide3DMesh(positions: positions, uvs: uvs, indices: indices);
  }

  static List<Vector3> _resampleCurve(List<Vector3> curve, int samples) {
    if (curve.length < 2) {
      return List<Vector3>.filled(samples, curve.isEmpty ? Vector3.zero() : curve.first.clone());
    }
    // Cumulative arc length.
    final cum = <double>[0.0];
    for (var i = 1; i < curve.length; i++) {
      cum.add(cum.last + (curve[i] - curve[i - 1]).length);
    }
    final total = cum.last;
    if (total < 1e-9) {
      return List<Vector3>.filled(samples, curve.first.clone());
    }
    final out = <Vector3>[];
    for (var s = 0; s < samples; s++) {
      final target = total * s / (samples - 1);
      // Binary search for the segment.
      var lo = 0;
      var hi = cum.length - 1;
      while (lo < hi - 1) {
        final mid = (lo + hi) ~/ 2;
        if (cum[mid] <= target) {
          lo = mid;
        } else {
          hi = mid;
        }
      }
      final segLen = cum[hi] - cum[lo];
      final frac = segLen < 1e-9 ? 0.0 : (target - cum[lo]) / segLen;
      out.add(curve[lo] + (curve[hi] - curve[lo]) * frac);
    }
    return out;
  }

  static void _smoothLoftCrossSections(
    List<Vector3> positions,
    int samplesPerCurve,
    int curveCount,
    double tension,
  ) {
    // Catmull-Rom blend across the curve index for each interior sample.
    final tmp = Vector3.zero();
    for (var c = 1; c < curveCount - 1; c++) {
      final p0 = (c - 2).clamp(0, curveCount - 1);
      final p3 = (c + 2).clamp(0, curveCount - 1);
      for (var s = 0; s < samplesPerCurve; s++) {
        final v0 = positions[p0 * samplesPerCurve + s];
        final v1 = positions[(c - 1) * samplesPerCurve + s];
        final v2 = positions[(c + 1) * samplesPerCurve + s];
        final v3 = positions[p3 * samplesPerCurve + s];
        // Centripetal Catmull-Rom at t=0.5 blends the four neighbours.
        tmp
          ..setFrom(v1)
          ..add(v2)
          ..scale(0.5);
        final cr = Vector3.zero()
          ..setFrom(tmp)
          ..add((v0 * -0.0625 + v1 * 0.5625 + v2 * 0.5625 + v3 * -0.0625) -
              tmp);
        final blended = v1 + (v2 - v1) * 0.5;
        final target = blended + (cr - blended) * tension;
        final idx = c * samplesPerCurve + s;
        positions[idx]
          ..setFrom(positions[idx] * (1.0 - tension * 0.5))
          ..add(target * (tension * 0.5));
      }
    }
  }

  // ----- Bend -----------------------------------------------------------

  /// Deforms [base] along [bendPath] and returns a new mesh.
  ///
  /// The bend pipeline is the heart of Feather's Bend mode:
  ///   1. The base guide's local +Y axis is treated as the "spine" —
  ///      parametrised by arc length s in [0, L].
  ///   2. The bend path is resampled to the same arc-length domain.
  ///   3. Each vertex (x, y, z) of the base mesh is re-located to
  ///      bendPath(s_y) + frameRight * x + frameUp * z, where frameRight
  ///      and frameUp come from the path's parallel-transport frame at s.
  ///
  /// This preserves the cross-section of the guide while bending the
  /// spine, exactly as Feather does when the artist draws a new bend
  /// stroke. The orange starting line of the new guide sits at s = 0
  /// (bendPath.first).
  static Guide3DMesh bendMesh({
    required Guide3DMesh base,
    required List<Vector3> bendPath,
    double spineLength = 2.0,
  }) {
    if (bendPath.length < 2) return base.clone();
    final samples = base.positions.length;
    // Resample bend path to a dense uniform-arc-length curve.
    final dense = _resampleCurve(bendPath, samples.clamp(32, 512));
    final cum = <double>[0.0];
    for (var i = 1; i < dense.length; i++) {
      cum.add(cum.last + (dense[i] - dense[i - 1]).length);
    }
    final totalLen = cum.last;
    if (totalLen < 1e-9) return base.clone();

    // Build parallel-transport frames along the path.
    final tangents = <Vector3>[];
    for (var i = 0; i < dense.length; i++) {
      final prev = dense[(i - 1).clamp(0, dense.length - 1)];
      final next = dense[(i + 1).clamp(0, dense.length - 1)];
      final t = (next - prev)..normalize();
      if (t.length2 < 1e-9) t.setValues(0, 1, 0);
      tangents.add(t);
    }
    var prevN = Vector3(1, 0, 0);
    if (prevN.dot(tangents[0]).abs() > 0.99) prevN = Vector3(0, 0, 1);
    final rights = <Vector3>[];
    final ups = <Vector3>[];
    for (var i = 0; i < dense.length; i++) {
      final t = tangents[i];
      var r = prevN - t * prevN.dot(t);
      if (r.length2 < 1e-6) {
        r = Vector3(0, 0, 1) - t * Vector3(0, 0, 1).dot(t);
      }
      r.normalize();
      final u = t.cross(r)..normalize();
      rights.add(r);
      ups.add(u);
      prevN = r;
    }

    final out = base.clone();
    for (var i = 0; i < out.positions.length; i++) {
      final p = base.positions[i];
      // Spine parameter: normalise local Y onto the path's arc length.
      final s = (p.y / spineLength + 0.5).clamp(0.0, 1.0) * totalLen;
      // Find path sample for s.
      var lo = 0;
      var hi = cum.length - 1;
      while (lo < hi - 1) {
        final mid = (lo + hi) ~/ 2;
        if (cum[mid] <= s) {
          lo = mid;
        } else {
          hi = mid;
        }
      }
      final segLen = cum[hi] - cum[lo];
      final frac = segLen < 1e-9 ? 0.0 : (s - cum[lo]) / segLen;
      final origin = dense[lo] + (dense[hi] - dense[lo]) * frac;
      final r = rights[lo] + (rights[hi] - rights[lo]) * frac;
      final u = ups[lo] + (ups[hi] - ups[lo]) * frac;
      out.positions[i]
        ..setFrom(origin)
        ..add(r * p.x)
        ..add(u * p.z);
    }
    out.recomputeNormals();
    return out;
  }
}
