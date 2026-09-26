// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// gltf_tube_export_test.dart — tests for the glTF exporter's optional
// tube-mesh mode (`emitTubes: true`), which wires
// `lib/engine/curves/curve_renderer.dart` (gap #6 in AUDIT_FINAL.md)
// into the export pipeline.
//
// Coverage:
//   * emitTubes on + matching Stroke3D  -> TRIANGLES (mode 4) + indices.
//   * emitTubes on + NO matching Stroke3D -> falls back to LINE_STRIP (3).
//   * emitTubes off (default)            -> LINE_STRIP (3), unchanged.
//   * The tube mesh's POSITION bounds are wider than the bare line (the
//     tube radius expands the AABB off the stroke's centreline).
//   * export() round-trips a tube-mode document to valid UTF-8 JSON.

import 'dart:convert';

import 'package:feather_krita/engine/curves/stroke3d.dart';
import 'package:feather_krita/io/gltf_exporter.dart';
import 'package:feather_krita/models/stroke.dart';
import 'package:flutter_test/flutter_test.dart';
import 'package:vector_math/vector_math_64.dart';

/// A 5-point stroke along the +X axis (local space). The matching
/// [Stroke3D] uses the same positions as samples so `toBezier()` fits a
/// real cubic with non-zero length.
Stroke _lineStroke(int id) => Stroke(
      id: id,
      name: 'Line $id',
      points: [
        for (var i = 0; i < 5; i++)
          StrokePoint(position: Vector3(i.toDouble(), 0, 0)),
      ],
    );

/// The matching capture: same positions as [_lineStroke], identity
/// transform, default thickness (4.0 → tube radius 2.0).
Stroke3D _lineStroke3D() => Stroke3D(
      samples: [
        for (var i = 0; i < 5; i++)
          StrokeSample3D(position: Vector3(i.toDouble(), 0, 0)),
      ],
      thickness: 4.0,
    );

void main() {
  group('GltfExporter.exportJson — emitTubes: true', () {
    test('a stroke with a matching Stroke3D becomes TRIANGLES (mode 4)', () {
      final json = GltfExporter().exportJson(
        strokes: [_lineStroke(1)],
        strokeCurves: {1: _lineStroke3D()},
        guides: [],
        emitTubes: true,
      );
      final meshes = json['meshes'] as List;
      expect(meshes.length, 1);
      final prim = (meshes[0] as Map)['primitives'] as List;
      expect(prim.length, 1);
      expect(prim[0]['mode'], 4); // TRIANGLES
      expect(prim[0].containsKey('indices'), isTrue,
          reason: 'a tube mesh must have an index buffer');
    });

    test('the tube mesh has many more vertices than the 5-point line', () {
      final json = GltfExporter().exportJson(
        strokes: [_lineStroke(1)],
        strokeCurves: {1: _lineStroke3D()},
        guides: [],
        emitTubes: true,
      );
      final accessors = json['accessors'] as List;
      // First accessor is the tube POSITION (the index accessor is SCALAR).
      final pos = accessors[0] as Map;
      expect(pos['type'], 'VEC3');
      // 5-point line = 5 verts; a tube with 8 radial × 33 length + 2 cap
      // centres = 266 verts. Assert it's well above the line count and
      // below the UNSIGNED_SHORT cap (65536).
      final count = pos['count'] as int;
      expect(count, greaterThan(50));
      expect(count, lessThan(65536));
    });

    test('the tube POSITION bounds extend off the centreline (radius)', () {
      // The bare 5-point line spans x in [0,4], y=z=0. The tube (radius 2)
      // must expand y and z to roughly [-2, 2] and extend x slightly past
      // [0,4] due to the end caps.
      final tubeJson = GltfExporter().exportJson(
        strokes: [_lineStroke(1)],
        strokeCurves: {1: _lineStroke3D()},
        guides: [],
        emitTubes: true,
      );
      final tubePos = (tubeJson['accessors'] as List)[0] as Map;
      final tubeMin = (tubePos['min'] as List).cast<double>();
      final tubeMax = (tubePos['max'] as List).cast<double>();
      // y and z must be non-zero (the tube has volume).
      expect(tubeMax[1].abs() + tubeMin[1].abs(), greaterThan(1.0),
          reason: 'tube should expand off the y=0 centreline');
      expect(tubeMax[2].abs() + tubeMin[2].abs(), greaterThan(1.0),
          reason: 'tube should expand off the z=0 centreline');

      // Compare against the bare-line bounds: x span should be >= the
      // line's [0,4] (caps extend it), and y/z should be strictly wider.
      final lineJson = GltfExporter().exportJson(
        strokes: [_lineStroke(1)],
        strokeCurves: {1: _lineStroke3D()},
        guides: [],
        emitTubes: false,
      );
      final linePos = (lineJson['accessors'] as List)[0] as Map;
      final lineMin = (linePos['min'] as List).cast<double>();
      final lineMax = (linePos['max'] as List).cast<double>();
      expect(lineMin[1], 0.0);
      expect(lineMax[1], 0.0);
      expect(tubeMax[1] - tubeMin[1], greaterThan(lineMax[1] - lineMin[1]));
    });

    test('a stroke WITHOUT a matching Stroke3D falls back to LINE_STRIP', () {
      final json = GltfExporter().exportJson(
        strokes: [_lineStroke(1)],
        strokeCurves: {}, // no entry for stroke id 1
        guides: [],
        emitTubes: true,
      );
      final meshes = json['meshes'] as List;
      expect(meshes.length, 1);
      final prim = (meshes[0] as Map)['primitives'] as List;
      expect(prim[0]['mode'], 3); // LINE_STRIP fallback
      expect(prim[0].containsKey('indices'), isFalse,
          reason: 'LINE_STRIP has no index buffer');
    });

    test('emitTubes defaults to false (LINE_STRIP preserved)', () {
      final json = GltfExporter().exportJson(
        strokes: [_lineStroke(1)],
        strokeCurves: {1: _lineStroke3D()},
        guides: [],
        // emitTubes omitted -> default false
      );
      final prim = (json['meshes'] as List)[0]['primitives'] as List;
      expect(prim[0]['mode'], 3); // LINE_STRIP
    });

    test('hidden strokes are still skipped in tube mode', () {
      final s = _lineStroke(1)..isVisible = false;
      final json = GltfExporter().exportJson(
        strokes: [s],
        strokeCurves: {1: _lineStroke3D()},
        guides: [],
        emitTubes: true,
      );
      expect(json['meshes'], isEmpty);
      expect(json['nodes'], isEmpty);
    });

    test('a degenerate (single-point) Stroke3D falls back to LINE_STRIP', () {
      // A single coincident sample -> toBezier produces a zero-length
      // curve -> _buildStrokeTube returns null -> LINE_STRIP fallback.
      final degenerate = Stroke3D(
        samples: [StrokeSample3D(position: Vector3(5, 5, 5))],
      );
      final json = GltfExporter().exportJson(
        strokes: [_lineStroke(1)],
        strokeCurves: {1: degenerate},
        guides: [],
        emitTubes: true,
      );
      final prim = (json['meshes'] as List)[0]['primitives'] as List;
      expect(prim[0]['mode'], 3,
          reason: 'degenerate fit must fall back to LINE_STRIP');
    });
  });

  group('GltfExporter.export — emitTubes byte output', () {
    test('export(emitTubes: true) returns valid UTF-8 JSON with TRIANGLES', () {
      final bytes = GltfExporter().export(
        strokes: [_lineStroke(1)],
        strokeCurves: {1: _lineStroke3D()},
        guides: [],
        emitTubes: true,
      );
      expect(bytes, isNotEmpty);
      final json = jsonDecode(utf8.decode(bytes)) as Map<String, dynamic>;
      expect((json['asset'] as Map)['version'], '2.0');
      final prim = (json['meshes'] as List)[0]['primitives'] as List;
      expect(prim[0]['mode'], 4); // TRIANGLES
    });
  });
}
