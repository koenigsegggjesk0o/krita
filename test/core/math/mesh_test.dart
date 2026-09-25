// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// mesh_test.dart — unit tests for the triangle mesh container.

import 'package:feather_krita/core/math/aabb.dart';
import 'package:feather_krita/core/math/mat4.dart';
import 'package:feather_krita/core/math/mesh.dart';
import 'package:feather_krita/core/math/vec2.dart';
import 'package:feather_krita/core/math/vec3.dart';
import 'package:flutter_test/flutter_test.dart';

/// A small tetrahedron used across multiple tests.
Mesh _tet() => const Mesh(
      vertices: [
        Vec3(0, 0, 0),
        Vec3(1, 0, 0),
        Vec3(0, 1, 0),
        Vec3(0, 0, 1),
      ],
      indices: [
        0, 1, 2, // base
        0, 3, 1, // side 1
        1, 3, 2, // side 2
        2, 3, 0, // side 3
      ],
    );

void main() {
  group('Mesh basics', () {
    test('triangleCount derives from index length / 3', () {
      expect(_tet().triangleCount, 4);
    });

    test('validate reports no errors for a well-formed mesh', () {
      expect(_tet().validate(), isEmpty);
    });

    test('validate flags index out of range', () {
      final m = Mesh(
        vertices: [const Vec3(0, 0, 0)],
        indices: [0, 1, 2],
      );
      final errors = m.validate();
      expect(errors, isNotEmpty);
      expect(errors.first, contains('out of vertex range'));
    });

    test('validate flags index count not divisible by 3', () {
      final m = Mesh(vertices: [const Vec3(0, 0, 0)], indices: [0, 1]);
      expect(m.validate(), isNotEmpty);
    });
  });

  group('Mesh.computeNormals', () {
    test('produces unit-length normals parallel to each vertex', () {
      final m = _tet().computeNormals();
      expect(m.normals!.length, m.vertices.length);
      for (final n in m.normals!) {
        expect(n.length, closeTo(1.0, 1e-6));
      }
    });

    test('a flat plane yields the plane normal at every vertex', () {
      final m = const Mesh(
        vertices: [
          Vec3(0, 0, 0),
          Vec3(1, 0, 0),
          Vec3(0, 0, 1),
          Vec3(1, 0, 1),
        ],
        indices: [0, 2, 1, 1, 2, 3],
      ).computeNormals();
      for (final n in m.normals!) {
        expect(n.y.abs(), closeTo(1.0, 1e-6));
      }
    });

    test('computeNormals does not mutate the receiver', () {
      final m = _tet();
      expect(m.normals, isNull);
      final computed = m.computeNormals();
      expect(m.normals, isNull);
      expect(computed.normals, isNotNull);
    });
  });

  group('Mesh.computeBoundingBox', () {
    test('returns a tight AABB around the vertices', () {
      final aabb = _tet().computeBoundingBox();
      expect(aabb.min, const Vec3(0, 0, 0));
      expect(aabb.max, const Vec3(1, 1, 1));
    });

    test('contains the corners and rejects points outside', () {
      final aabb = _tet().computeBoundingBox();
      expect(aabb.contains(const Vec3(0.5, 0.5, 0.5)), isTrue);
      expect(aabb.contains(const Vec3(2, 0, 0)), isFalse);
    });
  });

  group('Mesh.transform', () {
    test('translates vertex positions by the matrix translation', () {
      final m = _tet();
      final moved = m.transform(Mat4.makeTranslation(10, 20, 30));
      expect(moved.vertices.first, const Vec3(10, 20, 30));
      expect(moved.vertices[1], const Vec3(11, 20, 30));
    });

    test('scale matrix scales positions', () {
      final m = _tet();
      final scaled = m.transform(Mat4.makeScale(2, 3, 4));
      expect(scaled.vertices[1], const Vec3(2, 0, 0));
      expect(scaled.vertices[2], const Vec3(0, 3, 0));
      expect(scaled.vertices[3], const Vec3(0, 0, 4));
    });

    test('normals are renormalised after transform', () {
      final m = _tet().computeNormals();
      final transformed = m.transform(Mat4.makeScale(2, 2, 2));
      for (final n in transformed.normals!) {
        expect(n.length, closeTo(1.0, 1e-6));
      }
    });
  });

  group('Mesh.merge', () {
    test('concatenates vertices with the correct index offset', () {
      final a = _tet();
      final b = const Mesh(
        vertices: [Vec3(10, 0, 0), Vec3(11, 0, 0), Vec3(10, 1, 0)],
        indices: [0, 1, 2],
      );
      final merged = a.merge(b);
      expect(merged.vertices.length, a.vertices.length + b.vertices.length);
      expect(merged.triangleCount, a.triangleCount + b.triangleCount);
      // The indices of b are shifted by a.vertexCount.
      expect(merged.indices[a.indices.length], a.vertices.length);
    });

    test('merge preserves attributes when both sides have them', () {
      final a = _tet().computeNormals();
      // Build a second mesh with normals too.
      final b = const Mesh(
        vertices: [Vec3(10, 0, 0), Vec3(11, 0, 0), Vec3(10, 1, 0)],
        indices: [0, 1, 2],
      ).computeNormals();
      final merged = a.merge(b);
      expect(merged.normals, isNotNull);
      expect(merged.normals!.length,
          a.vertices.length + b.vertices.length);
    });

    test('merge drops attributes when only one side has them', () {
      final a = _tet().computeNormals();
      final b = const Mesh(
        vertices: [Vec3(10, 0, 0), Vec3(11, 0, 0), Vec3(10, 1, 0)],
        indices: [0, 1, 2],
      ); // no normals
      final merged = a.merge(b);
      expect(merged.normals, isNull);
    });
  });

  group('Mesh.subdivide', () {
    test('a single triangle becomes 4 triangles', () {
      final m = const Mesh(
        vertices: [Vec3(0, 0, 0), Vec3(1, 0, 0), Vec3(0, 1, 0)],
        indices: [0, 1, 2],
      );
      final s = m.subdivide();
      expect(s.triangleCount, 4);
      // Vertex count grew by 3 edge midpoints.
      expect(s.vertices.length, m.vertices.length + 3);
    });

    test('subdivision preserves the surface area of a triangle', () {
      final m = const Mesh(
        vertices: [Vec3(0, 0, 0), Vec3(2, 0, 0), Vec3(0, 2, 0)],
        indices: [0, 1, 2],
      );
      final s = m.subdivide();
      double area(Mesh mesh) {
        var sum = 0.0;
        for (var i = 0; i < mesh.indices.length; i += 3) {
          final a = mesh.vertices[mesh.indices[i]];
          final b = mesh.vertices[mesh.indices[i + 1]];
          final c = mesh.vertices[mesh.indices[i + 2]];
          sum += (b - a).cross(c - a).length * 0.5;
        }
        return sum;
      }

      expect(area(s), closeTo(area(m), 1e-9));
    });

    test('subdivision validates to a well-formed mesh', () {
      expect(_tet().subdivide().validate(), isEmpty);
    });
  });
}
