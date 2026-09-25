// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// mesh.dart — Triangle mesh container and geometry processing.
//
// A [Mesh] holds vertex positions, triangle indices, and optional
// per-vertex normals, UVs, and tangents. The lists themselves are
// final references but their contents may be rebuilt; the geometry
// helpers (computeNormals, computeTangents, transform, merge,
// subdivide) return new [Mesh] instances, leaving the receiver
// unchanged so meshes behave as immutable values at the API level.
//
// All algorithms are pure Dart and run synchronously.

import 'aabb.dart';
import 'math_utils.dart';
import 'mat3.dart';
import 'mat4.dart';
import 'vec2.dart';
import 'vec3.dart';

/// A triangle mesh.
class Mesh {
  /// Vertex positions.
  final List<Vec3> vertices;

  /// Triangle indices (flat, three per triangle, CCW winding).
  final List<int> indices;

  /// Optional per-vertex normals, parallel to [vertices].
  final List<Vec3>? normals;

  /// Optional per-vertex UV coordinates, parallel to [vertices].
  final List<Vec2>? uvs;

  /// Optional per-vertex tangent vectors, parallel to [vertices].
  final List<Vec3>? tangents;

  /// Constructs a mesh. The lists are stored by reference; pass copies
  /// if you need defensive isolation.
  const Mesh({
    required this.vertices,
    required this.indices,
    this.normals,
    this.uvs,
    this.tangents,
  });

  /// Number of triangles.
  int get triangleCount => indices.length ~/ 3;

  /// Returns a copy with one or more attribute arrays replaced.
  Mesh copyWith({
    List<Vec3>? vertices,
    List<int>? indices,
    List<Vec3>? normals,
    List<Vec2>? uvs,
    List<Vec3>? tangents,
  }) =>
      Mesh(
        vertices: vertices ?? this.vertices,
        indices: indices ?? this.indices,
        normals: normals ?? this.normals,
        uvs: uvs ?? this.uvs,
        tangents: tangents ?? this.tangents,
      );

  /// Recomputes per-vertex normals as the area-weighted average of
  /// adjacent face normals. Returns a new mesh with [normals] set.
  Mesh computeNormals() {
    final accum = List<Vec3>.filled(vertices.length, const Vec3.zero());
    for (var i = 0; i + 2 < indices.length; i += 3) {
      final ia = indices[i];
      final ib = indices[i + 1];
      final ic = indices[i + 2];
      final a = vertices[ia];
      final b = vertices[ib];
      final c = vertices[ic];
      final n = (b - a).cross(c - a);
      accum[ia] = accum[ia] + n;
      accum[ib] = accum[ib] + n;
      accum[ic] = accum[ic] + n;
    }
    final out = List<Vec3>.generate(
        vertices.length, (i) => accum[i].normalized());
    return copyWith(normals: out);
  }

  /// Recomputes per-vertex tangents using Lengyel's tangent-space
  /// derivation. Requires [uvs]; if absent, returns this mesh
  /// unchanged. Tangents are Gram-Schmidt orthogonalized against
  /// [normals] (falling back to +Y when normals are absent).
  Mesh computeTangents() {
    final uv = uvs;
    if (uv == null) return this;
    final accum = List<Vec3>.filled(vertices.length, const Vec3.zero());
    for (var i = 0; i + 2 < indices.length; i += 3) {
      final i0 = indices[i];
      final i1 = indices[i + 1];
      final i2 = indices[i + 2];
      final v0 = vertices[i0];
      final v1 = vertices[i1];
      final v2 = vertices[i2];
      final uv0 = uv[i0];
      final uv1 = uv[i1];
      final uv2 = uv[i2];
      final edge1 = v1 - v0;
      final edge2 = v2 - v0;
      final duv1 = uv1 - uv0;
      final duv2 = uv2 - uv0;
      final det = duv1.x * duv2.y - duv1.y * duv2.x;
      final r = det.abs() < epsilon ? 0.0 : 1.0 / det;
      final t = (edge1 * duv2.y - edge2 * duv1.y) * r;
      accum[i0] = accum[i0] + t;
      accum[i1] = accum[i1] + t;
      accum[i2] = accum[i2] + t;
    }
    final nrm = normals;
    final out = List<Vec3>.generate(vertices.length, (i) {
      var t = accum[i];
      final n = nrm != null ? nrm[i] : const Vec3.unitY();
      // Gram-Schmidt: t = normalize(t - n * dot(n, t))
      t = t - n * n.dot(t);
      final l = t.length;
      return l < epsilon ? const Vec3.unitX() : t / l;
    });
    return copyWith(tangents: out);
  }

  /// Axis-aligned bounding box of the vertices.
  Aabb computeBoundingBox() => Aabb.fromPoints(vertices);

  /// Transforms vertex positions by [m] (as points) and, when
  /// present, normals and tangents by the inverse-transpose of the
  /// upper-left 3x3 (re-normalized). Returns a new mesh.
  Mesh transform(Mat4 m) {
    final newVerts =
        List<Vec3>.generate(vertices.length, (i) => m.transformPoint(vertices[i]));
    List<Vec3>? newNorms;
    List<Vec3>? newTans;
    final normalMat = Mat3.normalFromMat4(m);
    if (normals != null) {
      newNorms = List<Vec3>.generate(
          normals!.length, (i) => normalMat.transform(normals![i]).normalized());
    }
    if (tangents != null) {
      newTans = List<Vec3>.generate(
          tangents!.length, (i) => normalMat.transform(tangents![i]).normalized());
    }
    return Mesh(
      vertices: newVerts,
      indices: indices,
      normals: newNorms,
      uvs: uvs,
      tangents: newTans,
    );
  }

  /// Concatenates [other] onto a copy of this mesh. Index offsets are
  /// applied to [other]'s indices. Attribute arrays are merged only
  /// when both meshes provide them; otherwise the merged attribute is
  /// `null`.
  Mesh merge(Mesh other) {
    final offset = vertices.length;
    final newVerts = [...vertices, ...other.vertices];
    final newIdx = [...indices, ...other.indices.map((i) => i + offset)];
    final newNorms = (normals == null || other.normals == null)
        ? null
        : [...normals!, ...other.normals!];
    final newUvs = (uvs == null || other.uvs == null)
        ? null
        : [...uvs!, ...other.uvs!];
    final newTans = (tangents == null || other.tangents == null)
        ? null
        : [...tangents!, ...other.tangents!];
    return Mesh(
      vertices: newVerts,
      indices: newIdx,
      normals: newNorms,
      uvs: newUvs,
      tangents: newTans,
    );
  }

  /// Performs one level of midpoint subdivision: each triangle is
  /// split into four by connecting edge midpoints. Existing vertices
  /// are reused; midpoints are deduplicated via a cache keyed on the
  /// sorted endpoint pair. Returns a new mesh with the same
  /// attributes (normals/uvs/tangents are NOT extended for new
  /// midpoint vertices — re-run [computeNormals] afterwards).
  Mesh subdivide() {
    final newVerts = [...vertices];
    final newIdx = <int>[];
    final midpoints = <(int, int), int>{};

    int midpoint(int a, int b) {
      final key = a < b ? (a, b) : (b, a);
      final existing = midpoints[key];
      if (existing != null) return existing;
      final idx = newVerts.length;
      newVerts.add((vertices[a] + vertices[b]) * 0.5);
      midpoints[key] = idx;
      return idx;
    }

    for (var i = 0; i + 2 < indices.length; i += 3) {
      final a = indices[i];
      final b = indices[i + 1];
      final c = indices[i + 2];
      final ab = midpoint(a, b);
      final bc = midpoint(b, c);
      final ca = midpoint(c, a);
      newIdx
        ..addAll([a, ab, ca])
        ..addAll([b, bc, ab])
        ..addAll([c, ca, bc])
        ..addAll([ab, bc, ca]);
    }
    return Mesh(
      vertices: newVerts,
      indices: newIdx,
      normals: normals,
      uvs: uvs,
      tangents: tangents,
    );
  }

  /// Returns a list of human-readable validation errors. An empty
  /// list means the mesh is well-formed: non-empty vertices, index
  /// count divisible by 3, all indices in range, and all attribute
  /// arrays (when present) parallel to [vertices].
  List<String> validate() {
    final errors = <String>[];
    if (vertices.isEmpty) {
      errors.add('Mesh has no vertices.');
    }
    if (indices.length % 3 != 0) {
      errors.add('Index count ${indices.length} is not a multiple of 3.');
    }
    for (var i = 0; i < indices.length; i++) {
      final idx = indices[i];
      if (idx < 0 || idx >= vertices.length) {
        errors.add('Index at $i ($idx) out of vertex range '
            '[0, ${vertices.length}).');
        break;
      }
    }
    if (normals != null && normals!.length != vertices.length) {
      errors.add('Normals count ${normals!.length} != vertex count '
          '${vertices.length}.');
    }
    if (uvs != null && uvs!.length != vertices.length) {
      errors.add('UV count ${uvs!.length} != vertex count '
          '${vertices.length}.');
    }
    if (tangents != null && tangents!.length != vertices.length) {
      errors.add('Tangent count ${tangents!.length} != vertex count '
          '${vertices.length}.');
    }
    return errors;
  }
}
