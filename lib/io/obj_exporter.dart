// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// obj_exporter.dart — Wavefront OBJ tube-mesh exporter.
//
// Exports each visible [Stroke] as a tube mesh: a regular polygon of
// [tubeSegments] vertices is swept along the stroke's world-space
// polyline, generating triangle faces between adjacent rings. The
// result is a single OBJ file with one object (`o Stroke_<id>`) per
// stroke, no material library, no normals / UVs (positions + faces
// only). Compatible with Blender, Maya, MeshLab, three.js, etc.
//
// Tube framing follows the standard parallel-transport approach: for
// each polyline point we compute the unit tangent (central difference,
// with one-sided fallback at the ends), then build an orthonormal
// frame (tangent, normal, binormal). The normal at point 0 is an
// arbitrary vector perpendicular to the tangent; subsequent frames
// are propagated by parallel transport (rotation about the binormal)
// so the tube doesn't twist. The ring vertices at sample i are
//
//     v(i, k) = center(i) + radius * (cos(theta_k) * N_i + sin(theta_k) * B_i)
//
// where theta_k = 2*pi*k / tubeSegments.

import 'dart:math' as math;

import 'package:vector_math/vector_math_64.dart';

import 'package:feather_krita/models/stroke.dart';

/// Wavefront OBJ tube-mesh exporter.
class ObjExporter {
  ObjExporter({
    this.tubeSegments = 8,
    this.tubeRadius = 0.05,
  })  : assert(tubeSegments >= 3, 'tubeSegments must be >= 3'),
        assert(tubeRadius > 0, 'tubeRadius must be > 0');

  /// Number of vertices in each ring around the polyline.
  final int tubeSegments;

  /// Tube radius in world units.
  final double tubeRadius;

  /// Exports [strokes] as a single OBJ string.
  String export({required List<Stroke> strokes}) {
    final out = StringBuffer()
      ..writeln('# Exported from Feather-Krita')
      ..writeln('# Strokes: ${strokes.length}')
      ..writeln('# Tube segments: $tubeSegments, radius: $tubeRadius');

    // OBJ vertex indices are 1-based and global across the file.
    // `vertexBase` is the 0-based offset of the next vertex; we add 1
    // when emitting face indices to convert to OBJ's 1-based convention.
    var vertexBase = 0;

    for (final stroke in strokes) {
      if (!stroke.isVisible || stroke.points.length < 2) continue;
      out
        ..writeln()
        ..writeln('o Stroke_${stroke.id}')
        ..writeln('g Stroke_${stroke.id}');

      // World-space polyline.
      final points = <Vector3>[
        for (final p in stroke.points)
          stroke.transform.transform3(p.position.clone()),
      ];

      // Per-vertex orthonormal frame (parallel-transport).
      final tangents = _computeTangents(points);
      final normals = _propagateFrames(tangents);

      // --- Ring vertices: tubeSegments verts per polyline point. ---
      // Index layout (0-based): ring j vertex k -> vertexBase + j*tubeSegments + k.
      for (var j = 0; j < points.length; j++) {
        final c = points[j];
        final n = normals[j];
        final b = tangents[j].cross(n)..normalize();
        for (var k = 0; k < tubeSegments; k++) {
          final theta = 2.0 * math.pi * k / tubeSegments;
          final ct = math.cos(theta);
          final st = math.sin(theta);
          final v = c + (n * ct + b * st) * tubeRadius;
          out.writeln('v ${_fmt(v.x)} ${_fmt(v.y)} ${_fmt(v.z)}');
        }
      }

      // --- Side faces: two triangles per quad between ring j and j+1. ---
      for (var j = 0; j < points.length - 1; j++) {
        for (var k = 0; k < tubeSegments; k++) {
          final k2 = (k + 1) % tubeSegments;
          // Convert to 1-based OBJ indices.
          final a = vertexBase + j * tubeSegments + k + 1;
          final b = vertexBase + j * tubeSegments + k2 + 1;
          final c = vertexBase + (j + 1) * tubeSegments + k2 + 1;
          final d = vertexBase + (j + 1) * tubeSegments + k + 1;
          out
            ..writeln('f $a $b $c')
            ..writeln('f $a $c $d');
        }
      }

      // --- Start cap (closes ring 0 with a fan to a center vertex). ---
      // The center vertex is the first polyline point itself; it gets
      // emitted once after all ring vertices and indexed separately.
      final startCenterIdx =
          vertexBase + points.length * tubeSegments + 1; // 1-based
      final startC = points.first;
      out.writeln('v ${_fmt(startC.x)} ${_fmt(startC.y)} ${_fmt(startC.z)}');
      // Reverse winding so the cap normal faces -tangent (outward).
      for (var k = 0; k < tubeSegments; k++) {
        final k2 = (k + 1) % tubeSegments;
        final a = vertexBase + k + 1; // ring 0 vertex k (1-based)
        final b = vertexBase + k2 + 1; // ring 0 vertex k2
        out.writeln('f $startCenterIdx $b $a');
      }

      // --- End cap (closes the last ring with a fan to a center vertex). ---
      final endCenterIdx = startCenterIdx + 1; // 1-based
      final endC = points.last;
      out.writeln('v ${_fmt(endC.x)} ${_fmt(endC.y)} ${_fmt(endC.z)}');
      final lastRingBase =
          vertexBase + (points.length - 1) * tubeSegments; // 0-based
      for (var k = 0; k < tubeSegments; k++) {
        final k2 = (k + 1) % tubeSegments;
        final a = lastRingBase + k + 1; // 1-based
        final b = lastRingBase + k2 + 1;
        out.writeln('f $endCenterIdx $a $b');
      }

      // Advance the base past this stroke's vertices: rings + 2 caps.
      vertexBase += points.length * tubeSegments + 2;
    }

    return out.toString();
  }

  // ---- Geometry helpers -------------------------------------------------

  /// Computes unit tangents at each polyline point using central
  /// differences (one-sided at the endpoints). Falls back to +X when
  /// the local chord is degenerate.
  static List<Vector3> _computeTangents(List<Vector3> points) {
    final out = <Vector3>[];
    for (var i = 0; i < points.length; i++) {
      Vector3 t;
      if (i == 0) {
        t = points[1] - points[0];
      } else if (i == points.length - 1) {
        t = points[i] - points[i - 1];
      } else {
        t = points[i + 1] - points[i - 1];
      }
      if (t.length2 < 1e-20) t = Vector3(1, 0, 0);
      out.add(t..normalize());
    }
    return out;
  }

  /// Propagates a normal vector along the polyline by parallel
  /// transport. The initial normal is an arbitrary unit vector
  /// perpendicular to `tangents[0]`; at each subsequent point the
  /// normal is rotated by the same rotation that takes the previous
  /// tangent to the current tangent (rotation about their cross
  /// product). This keeps the tube from twisting.
  static List<Vector3> _propagateFrames(List<Vector3> tangents) {
    if (tangents.isEmpty) return const <Vector3>[];
    final normals = List<Vector3>.filled(tangents.length, Vector3.zero());

    // Initial normal: pick any vector not parallel to tangents[0],
    // subtract its tangent component, normalize.
    final t0 = tangents[0];
    final seed = t0.x.abs() < 0.9 ? Vector3(1, 0, 0) : Vector3(0, 1, 0);
    final n0 = seed - t0 * seed.dot(t0);
    if (n0.length2 < 1e-20) {
      normals[0] = Vector3(0, 1, 0);
    } else {
      normals[0] = n0..normalize();
    }

    for (var i = 1; i < tangents.length; i++) {
      final tPrev = tangents[i - 1];
      final tCur = tangents[i];
      final axis = tPrev.cross(tCur);
      final axisLen = axis.length;
      if (axisLen < 1e-12) {
        // Tangents are parallel (or anti-parallel); keep the normal.
        normals[i] = normals[i - 1].clone();
        continue;
      }
      axis.normalize();
      final angle = math.acos(tPrev.dot(tCur).clamp(-1.0, 1.0));
      normals[i] = _rotateAboutAxis(normals[i - 1], axis, angle);
    }
    return normals;
  }

  /// Rodrigues rotation of [v] about unit [axis] by [angle] radians.
  static Vector3 _rotateAboutAxis(Vector3 v, Vector3 axis, double angle) {
    final cosA = math.cos(angle);
    final sinA = math.sin(angle);
    final dot = axis.dot(v);
    final cross = axis.cross(v);
    return v * cosA + cross * sinA + axis * (dot * (1.0 - cosA));
  }

  static String _fmt(double v) {
    // Trim trailing zeros; keep 6 decimal places max.
    if (v.isNaN || v.isInfinite) return '0';
    final s = v.toStringAsFixed(6);
    return s.contains('.')
        ? s.replaceFirst(RegExp(r'0+$'), '').replaceFirst(RegExp(r'\.$'), '')
        : s;
  }
}
