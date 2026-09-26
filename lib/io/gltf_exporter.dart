// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// gltf_exporter.dart — glTF 2.0 scene exporter.
//
// Exports the document's strokes + active 3D Guides as a self-contained
// glTF 2.0 JSON file (`.gltf`) with all geometry packed into a single
// base64-encoded binary buffer embedded as a `data:` URI.
//
// Mapping:
//   * Each visible [Stroke] becomes a glTF mesh. By default the primitive
//     `mode` is `3` (LINE_STRIP) and the vertices are the stroke's
//     world-space points. When [exportJson]'s `emitTubes` flag is set
//     (and a matching [Stroke3D] is supplied in `strokeCurves`), the
//     stroke is instead fitted to a Bezier via `Stroke3D.toBezier()`,
//     tessellated into a capped tube mesh by
//     `CurveRenderer.buildTubeMesh`, and emitted as `mode` `4`
//     (TRIANGLES) — so glTF viewers show solid lit tubes instead of
//     thin lines. Strokes without a matching [Stroke3D] always fall
//     back to LINE_STRIP.
//   * Each visible [Guide3D] becomes a glTF mesh with one primitive whose
//     `mode` is `4` (TRIANGLES). Vertices + indices come straight from
//     `guide.mesh`, transformed into world space by `guide.transform`.
//   * All vertex positions use componentType 5126 (FLOAT) and a VEC3
//     accessor with min/max populated (required by the glTF spec for
//     POSITION accessors).
//   * All indices use componentType 5123 (UNSIGNED_SHORT) — sufficient
//     for any mesh with < 65536 vertices, which covers every guide the
//     engine currently generates and every tube mesh the default
//     `tubeRadialSegments` / `tubeLengthSegments` produce.

import 'dart:convert' show base64Encode, jsonEncode, utf8;
import 'dart:typed_data' show ByteData, Endian, Uint8List;

import 'package:feather_krita/engine/curves/curve_renderer.dart';
import 'package:feather_krita/engine/curves/stroke3d.dart';
import 'package:feather_krita/engine/guide3d/guide3d.dart';
import 'package:feather_krita/models/stroke.dart';

/// glTF 2.0 scene exporter.
class GltfExporter {
  /// Builds the glTF JSON document for the given scene.
  ///
  /// [strokes] are emitted as LINE_STRIP primitives (one mesh each) by
  /// default. [strokeCurves] is the optional map from stroke id → the
  /// [Stroke3D] that captured the stroke; when [emitTubes] is true, each
  /// stroke that has an entry here is fitted to a Bezier and tessellated
  /// into a capped tube mesh (TRIANGLES) via [CurveRenderer.buildTubeMesh].
  /// Strokes without a matching entry always fall back to LINE_STRIP.
  /// [guides] are emitted as TRIANGLES primitives (one mesh each).
  ///
  /// [tubeRadialSegments] / [tubeLengthSegments] control the tube mesh
  /// density when [emitTubes] is on (defaults 8 / 32). The resulting
  /// vertex count must stay below 65536 (UNSIGNED_SHORT index range);
  /// any stroke whose tube would exceed that silently falls back to
  /// LINE_STRIP so export never fails on a long stroke.
  Map<String, dynamic> exportJson({
    required List<Stroke> strokes,
    required Map<int, Stroke3D> strokeCurves,
    required List<Guide3D> guides,
    bool emitTubes = false,
    int tubeRadialSegments = 8,
    int tubeLengthSegments = 32,
  }) {
    final buffer = <int>[];
    final accessors = <Map<String, dynamic>>[];
    final bufferViews = <Map<String, dynamic>>[];
    final meshes = <Map<String, dynamic>>[];
    final nodes = <Map<String, dynamic>>[];
    final sceneNodes = <int>[];

    int addBufferView(List<int> bytes, [int? target]) {
      final bv = <String, dynamic>{
        'buffer': 0,
        'byteOffset': buffer.length,
        'byteLength': bytes.length,
      };
      if (target != null) bv['target'] = target;
      bufferViews.add(bv);
      buffer.addAll(bytes);
      return bufferViews.length - 1;
    }

    int addPositionAccessor(int bvIdx, List<double> positions) {
      var minX = double.infinity, minY = double.infinity, minZ = double.infinity;
      var maxX = -double.infinity, maxY = -double.infinity, maxZ = -double.infinity;
      for (var i = 0; i < positions.length; i += 3) {
        final x = positions[i], y = positions[i + 1], z = positions[i + 2];
        if (x < minX) minX = x;
        if (x > maxX) maxX = x;
        if (y < minY) minY = y;
        if (y > maxY) maxY = y;
        if (z < minZ) minZ = z;
        if (z > maxZ) maxZ = z;
      }
      accessors.add({
        'bufferView': bvIdx,
        'componentType': 5126, // FLOAT
        'count': positions.length ~/ 3,
        'type': 'VEC3',
        'max': [maxX, maxY, maxZ],
        'min': [minX, minY, minZ],
      });
      return accessors.length - 1;
    }

    int addUShortIndexAccessor(int bvIdx, int count) {
      accessors.add({
        'bufferView': bvIdx,
        'componentType': 5123, // UNSIGNED_SHORT
        'count': count,
        'type': 'SCALAR',
      });
      return accessors.length - 1;
    }

    // Strokes -> LINE_STRIP primitives (default), or TRIANGLES tube
    // meshes when [emitTubes] is on and a matching [Stroke3D] is present.
    for (final stroke in strokes) {
      if (!stroke.isVisible || stroke.points.length < 2) continue;

      // Try the tube-mesh path first when requested.
      if (emitTubes) {
        final stroke3d = strokeCurves[stroke.id];
        if (stroke3d != null && stroke3d.samples.isNotEmpty) {
          final tube = _buildStrokeTube(
            stroke3d,
            radialSegments: tubeRadialSegments,
            lengthSegments: tubeLengthSegments,
          );
          // UNSIGNED_SHORT indices cap the mesh at 65536 verts. A tube
          // that exceeds that (very long stroke + high density) falls
          // back to LINE_STRIP so export never fails.
          if (tube != null &&
              tube.positions.length ~/ 3 < 65536 &&
              tube.indices.isNotEmpty) {
            final posBytes = _floatsToBytes(tube.positions);
            final idxBytes = _intsToUShortBytes(tube.indices);
            final posBv = addBufferView(posBytes, 34962); // ARRAY_BUFFER
            final idxBv =
                addBufferView(idxBytes, 34963); // ELEMENT_ARRAY_BUFFER
            final posAcc = addPositionAccessor(posBv, tube.positions);
            final idxAcc =
                addUShortIndexAccessor(idxBv, tube.indices.length);
            meshes.add({
              'primitives': [
                {
                  'attributes': {'POSITION': posAcc},
                  'indices': idxAcc,
                  'mode': 4, // TRIANGLES
                }
              ],
            });
            nodes.add({
              'mesh': meshes.length - 1,
              'name': stroke.name ?? 'Stroke ${stroke.id}',
            });
            sceneNodes.add(nodes.length - 1);
            continue;
          }
        }
      }

      // Default: LINE_STRIP from the stroke's own world-space points.
      final positions = <double>[];
      for (final p in stroke.points) {
        final wp = stroke.transform.transform3(p.position.clone());
        positions.add(wp.x);
        positions.add(wp.y);
        positions.add(wp.z);
      }
      final bytes = _floatsToBytes(positions);
      final bvIdx = addBufferView(bytes, 34962); // ARRAY_BUFFER
      final accIdx = addPositionAccessor(bvIdx, positions);
      meshes.add({
        'primitives': [
          {
            'attributes': {'POSITION': accIdx},
            'mode': 3, // LINE_STRIP
          }
        ],
      });
      nodes.add({
        'mesh': meshes.length - 1,
        'name': stroke.name ?? 'Stroke ${stroke.id}',
      });
      sceneNodes.add(nodes.length - 1);
    }

    // Guides -> TRIANGLES primitives.
    for (var i = 0; i < guides.length; i++) {
      final guide = guides[i];
      if (!guide.visible) continue;
      final positions = <double>[];
      for (final p in guide.mesh.positions) {
        final wp = guide.transform.transform3(p.clone());
        positions.add(wp.x);
        positions.add(wp.y);
        positions.add(wp.z);
      }
      if (positions.isEmpty) continue;
      final posBytes = _floatsToBytes(positions);
      final idxBytes = _intsToUShortBytes(guide.mesh.indices);
      final posBv = addBufferView(posBytes, 34962); // ARRAY_BUFFER
      final idxBv = addBufferView(idxBytes, 34963); // ELEMENT_ARRAY_BUFFER
      final posAcc = addPositionAccessor(posBv, positions);
      final idxAcc = addUShortIndexAccessor(idxBv, guide.mesh.indices.length);
      meshes.add({
        'primitives': [
          {
            'attributes': {'POSITION': posAcc},
            'indices': idxAcc,
            'mode': 4, // TRIANGLES
          }
        ],
      });
      nodes.add({
        'mesh': meshes.length - 1,
        'name': guide.name ?? 'Guide $i',
      });
      sceneNodes.add(nodes.length - 1);
    }

    return {
      'asset': {
        'version': '2.0',
        'generator': 'Feather-Krita glTF Exporter',
      },
      'scene': 0,
      'scenes': [
        {'nodes': sceneNodes}
      ],
      'nodes': nodes,
      'meshes': meshes,
      'accessors': accessors,
      'bufferViews': bufferViews,
      'buffers': [
        {
          'uri': 'data:application/octet-stream;base64,'
              '${base64Encode(Uint8List.fromList(buffer))}',
          'byteLength': buffer.length,
        }
      ],
    };
  }

  /// Encodes the scene as a glTF 2.0 JSON UTF-8 byte buffer.
  ///
  /// The result can be written directly to a `.gltf` file. Forwards the
  /// [emitTubes] / [tubeRadialSegments] / [tubeLengthSegments] options to
  /// [exportJson].
  Uint8List export({
    required List<Stroke> strokes,
    required Map<int, Stroke3D> strokeCurves,
    required List<Guide3D> guides,
    bool emitTubes = false,
    int tubeRadialSegments = 8,
    int tubeLengthSegments = 32,
  }) {
    final json = exportJson(
      strokes: strokes,
      strokeCurves: strokeCurves,
      guides: guides,
      emitTubes: emitTubes,
      tubeRadialSegments: tubeRadialSegments,
      tubeLengthSegments: tubeLengthSegments,
    );
    return Uint8List.fromList(utf8.encode(jsonEncode(json)));
  }

  // ---- Stroke → tube mesh -----------------------------------------------

  /// Fits [stroke3d] to a Bezier and tessellates a capped tube mesh via
  /// [CurveRenderer.buildTubeMesh]. Returns `null` when the fit produces
  /// a degenerate curve (e.g. all samples coincident) — the caller falls
  /// back to LINE_STRIP in that case.
  ///
  /// The Bezier's transform (set by `Stroke3D.toBezier` to the stroke's
  /// transform) is applied by `Curve3D.sampleAt`, so the returned
  /// positions are in the same world space as the LINE_STRIP path's
  /// `stroke.transform * point`.
  static CurveMesh? _buildStrokeTube(
    Stroke3D stroke3d, {
    int radialSegments = 8,
    int lengthSegments = 32,
  }) {
    try {
      final bezier = stroke3d.toBezier();
      // Reject degenerate fits (zero-length curve) — buildTubeMesh would
      // emit a collapsed mesh that's useless to a glTF viewer.
      if (bezier.length() < 1e-6) return null;
      return CurveRenderer.buildTubeMesh(
        bezier,
        radialSegments: radialSegments,
        lengthSegments: lengthSegments,
        capEnds: true,
      );
    } catch (_) {
      // toBezier can throw on pathological inputs (e.g. NaN samples);
      // never let export crash on a bad stroke.
      return null;
    }
  }

  // ---- Binary helpers ---------------------------------------------------

  static Uint8List _floatsToBytes(List<double> floats) {
    final bd = ByteData(floats.length * 4);
    for (var i = 0; i < floats.length; i++) {
      bd.setFloat32(i * 4, floats[i], Endian.little);
    }
    return bd.buffer.asUint8List();
  }

  static Uint8List _intsToUShortBytes(List<int> ints) {
    final bd = ByteData(ints.length * 2);
    for (var i = 0; i < ints.length; i++) {
      bd.setUint16(i * 2, ints[i].clamp(0, 65535), Endian.little);
    }
    return bd.buffer.asUint8List();
  }
}
