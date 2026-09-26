// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// gltf_exporter_test.dart — unit tests for the glTF 2.0 scene exporter.
//
// Strokes become LINE_STRIP primitives; guides become TRIANGLES. Tests
// cover asset metadata, stroke → mesh mapping, hidden-stroke skipping,
// base64 buffer embedding, accessor min/max, and the guide-mesh path
// (using a minimal hand-built Guide3D + Guide3DMesh).

import 'dart:convert';

import 'package:feather_krita/engine/guide3d/guide3d.dart';
import 'package:feather_krita/engine/guide3d/guide3d_type.dart';
import 'package:feather_krita/engine/guide3d/guide_mesh_generators.dart';
import 'package:feather_krita/io/gltf_exporter.dart';
import 'package:feather_krita/models/stroke.dart';
import 'package:flutter_test/flutter_test.dart';
import 'package:vector_math/vector_math_64.dart';

Stroke _stroke(int id, List<Vector3> pts,
        {bool visible = true, String? name}) =>
    Stroke(
      id: id,
      isVisible: visible,
      name: name,
      points: pts.map((p) => StrokePoint(position: p)).toList(),
    );

Guide3D _triangleGuide() => Guide3D(
      type: Guide3DType.primitive,
      mesh: Guide3DMesh(
        positions: [
          Vector3(0, 0, 0),
          Vector3(1, 0, 0),
          Vector3(0, 1, 0),
        ],
        uvs: [
          Vector2(0, 0),
          Vector2(1, 0),
          Vector2(0, 1),
        ],
        indices: [0, 1, 2],
      ),
      startPoint: Vector3.zero(),
      startEnd: Vector3(1, 0, 0),
      name: 'Tri',
    );

void main() {
  group('GltfExporter.exportJson — asset metadata', () {
    test('asset.version is "2.0" with the Feather generator string', () {
      final json = GltfExporter().exportJson(
        strokes: [],
        strokeCurves: {},
        guides: [],
      );
      expect(json['asset'], isNotNull);
      expect((json['asset'] as Map)['version'], '2.0');
      expect((json['asset'] as Map)['generator'], 'Feather-Krita glTF Exporter');
    });

    test('empty scene still has a single scene + empty nodes list', () {
      final json = GltfExporter().exportJson(
        strokes: [],
        strokeCurves: {},
        guides: [],
      );
      expect(json['scene'], 0);
      expect((json['scenes'] as List).length, 1);
      expect(json['nodes'], isEmpty);
      expect(json['meshes'], isEmpty);
    });
  });

  group('GltfExporter.exportJson — strokes as LINE_STRIP', () {
    test('a visible stroke produces one mesh with mode = 3', () {
      final json = GltfExporter().exportJson(
        strokes: [_stroke(1, [Vector3.zero(), Vector3(1, 0, 0)])],
        strokeCurves: {},
        guides: [],
      );
      final meshes = json['meshes'] as List;
      expect(meshes.length, 1);
      final prim = (meshes[0] as Map)['primitives'] as List;
      expect(prim.length, 1);
      expect(prim[0]['mode'], 3); // LINE_STRIP
      // The node has a name.
      final nodes = json['nodes'] as List;
      expect(nodes.length, 1);
      expect((nodes[0] as Map).containsKey('name'), isTrue);
    });

    test('hidden strokes are skipped', () {
      final json = GltfExporter().exportJson(
        strokes: [
          _stroke(1, [Vector3.zero(), Vector3(1, 0, 0)], visible: false),
        ],
        strokeCurves: {},
        guides: [],
      );
      expect(json['meshes'], isEmpty);
      expect(json['nodes'], isEmpty);
    });

    test('strokes with < 2 points are skipped', () {
      final json = GltfExporter().exportJson(
        strokes: [
          _stroke(1, [Vector3.zero()]), // single point
        ],
        strokeCurves: {},
        guides: [],
      );
      expect(json['meshes'], isEmpty);
    });

    test('POSITION accessor reports min and max bounds', () {
      final json = GltfExporter().exportJson(
        strokes: [
          _stroke(1, [Vector3(-1, -2, -3), Vector3(1, 2, 3)]),
        ],
        strokeCurves: {},
        guides: [],
      );
      final accessors = json['accessors'] as List;
      expect(accessors.length, greaterThanOrEqualTo(1));
      final pos = accessors[0] as Map;
      expect(pos['type'], 'VEC3');
      expect(pos['componentType'], 5126); // FLOAT
      expect(pos['min'], [-1.0, -2.0, -3.0]);
      expect(pos['max'], [1.0, 2.0, 3.0]);
    });
  });

  group('GltfExporter.exportJson — guides as TRIANGLES', () {
    test('a visible guide produces one mesh with mode = 4', () {
      final json = GltfExporter().exportJson(
        strokes: [],
        strokeCurves: {},
        guides: [_triangleGuide()],
      );
      final meshes = json['meshes'] as List;
      expect(meshes.length, 1);
      final prim = (meshes[0] as Map)['primitives'] as List;
      expect(prim[0]['mode'], 4); // TRIANGLES
      expect(prim[0].containsKey('indices'), isTrue);
    });

    test('hidden guides are skipped', () {
      final g = _triangleGuide()..visible = false;
      final json = GltfExporter().exportJson(
        strokes: [],
        strokeCurves: {},
        guides: [g],
      );
      expect(json['meshes'], isEmpty);
    });
  });

  group('GltfExporter.exportJson — buffer embedding', () {
    test('the buffer URI is a base64 data: URI whose decode length matches byteLength', () {
      final json = GltfExporter().exportJson(
        strokes: [_stroke(1, [Vector3.zero(), Vector3(1, 0, 0)])],
        strokeCurves: {},
        guides: [],
      );
      final buffers = json['buffers'] as List;
      expect(buffers.length, 1);
      final buf = buffers[0] as Map;
      final uri = buf['uri'] as String;
      expect(uri, startsWith('data:application/octet-stream;base64,'));
      final b64 = uri.substring('data:application/octet-stream;base64,'.length);
      final decoded = base64.decode(b64);
      expect(decoded.length, buf['byteLength']);
    });
  });

  group('GltfExporter.export — byte output', () {
    test('export returns a UTF-8 JSON buffer', () {
      final bytes = GltfExporter().export(
        strokes: [_stroke(1, [Vector3.zero(), Vector3(1, 0, 0)])],
        strokeCurves: {},
        guides: [],
      );
      expect(bytes, isNotEmpty);
      // Round-trip the bytes back to a JSON map.
      final json = jsonDecode(utf8.decode(bytes)) as Map<String, dynamic>;
      expect((json['asset'] as Map)['version'], '2.0');
    });
  });
}
