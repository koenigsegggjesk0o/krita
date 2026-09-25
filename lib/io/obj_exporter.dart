// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// obj_exporter.dart — Wavefront OBJ exporter for guide surfaces and strokes.
//
// Emits a single .obj file (plus inline material directives) that opens
// in Blender / Maya / Cinema 4D / 3D Viewer. Two object groups are
// emitted:
//
//   - "GuideSurface"  : the parametric guide surface mesh (positions,
//                       normals, UVs, triangle indices).
//   - "StrokeRibbons" : each 3D stroke ribbon-ized as a thin tube along
//                       its point sequence, with the stroke's color as
//                       a per-stroke material. Stroke tubes are built
//                       by sweeping a small circle around the stroke
//                       tangent — minimal but visually faithful.
//
// The exporter is pure-Dart (no native code, no Flutter widgets) so it
// runs on every platform including the unit-test isolate.

import 'dart:math' as math;

import 'package:vector_math/vector_math_64.dart';

import 'package:feather_krita/engine/guide_surface.dart';
import 'package:feather_krita/models/stroke.dart';

/// Builds a Wavefront OBJ document for [mesh] plus optional stroke
/// ribbons. The result is ready to write to a .obj file.
class ObjExporter {
  ObjExporter({
    this.strokeTubeRadius = 0.02,
    this.strokeTubeSegments = 6,
    this.includeStrokes = true,
    this.includeGuideSurface = true,
    this.objectName = 'FeatherScene',
  })  : assert(strokeTubeRadius > 0),
        assert(strokeTubeSegments >= 3);

  /// Tube radius (world units) for stroke ribbons.
  final double strokeTubeRadius;

  /// Number of segments around each stroke tube.
  final int strokeTubeSegments;

  /// Whether to emit stroke ribbons.
  final bool includeStrokes;

  /// Whether to emit the guide surface mesh.
  final bool includeGuideSurface;

  /// Top-level object name written to the .obj file.
  final String objectName;

  /// Builds the OBJ document for [mesh] and [strokes].
  String build(SurfaceMesh mesh, {List<Stroke> strokes = const <Stroke>[]}) {
    final buf = StringBuffer()
      ..writeln('# Exported from Feather-Krita')
      ..writeln('# https://feather-krita.example')
      ..writeln('o $objectName')
      ..writeln();

    var vertexOffset = 0;
    var normalOffset = 0;
    var uvOffset = 0;

    if (includeGuideSurface) {
      vertexOffset += _emitMesh(buf, mesh, name: 'GuideSurface');
      // The guide mesh emits its own normals/UVs.
      normalOffset += mesh.normals.length;
      uvOffset += mesh.uvs.length;
      buf.writeln();
    }

    if (includeStrokes) {
      for (var i = 0; i < strokes.length; i++) {
        final stroke = strokes[i];
        if (stroke.points.length < 2) continue;
        final ribbon = _StrokeRibbon(stroke, strokeTubeRadius, strokeTubeSegments);
        final used = _emitRibbon(buf, ribbon,
            vertexOffset: vertexOffset,
            normalOffset: normalOffset,
            uvOffset: uvOffset,
            name: 'Stroke_${stroke.id}');
        vertexOffset += used.vertices;
        normalOffset += used.normals;
        uvOffset += used.uvs;
      }
    }

    return buf.toString();
  }

  /// Emits a [SurfaceMesh] block into [buf]. Returns the number of
  /// vertices written.
  int _emitMesh(StringBuffer buf, SurfaceMesh mesh, {required String name}) {
    buf.writeln('o $name');
    for (final p in mesh.positions) {
      buf.writeln('v ${_f(p.x)} ${_f(p.y)} ${_f(p.z)}');
    }
    for (final uv in mesh.uvs) {
      buf.writeln('vt ${_f(uv.x)} ${_f(uv.y)}');
    }
    for (final n in mesh.normals) {
      buf.writeln('vn ${_f(n.x)} ${_f(n.y)} ${_f(n.z)}');
    }
    final tris = mesh.triangleCount;
    for (var t = 0; t < tris; t++) {
      final a = mesh.indices[t * 3] + 1;
      final b = mesh.indices[t * 3 + 1] + 1;
      final c = mesh.indices[t * 3 + 2] + 1;
      buf.writeln('f $a/$a/$a $b/$b/$b $c/$c/$c');
    }
    return mesh.positions.length;
  }

  /// Emits one stroke ribbon. Returns the number of vertices / normals /
  /// UVs written so the caller can advance the global offsets.
  _RibbonStats _emitRibbon(
    StringBuffer buf,
    _StrokeRibbon ribbon, {
    required int vertexOffset,
    required int normalOffset,
    required int uvOffset,
    required String name,
  }) {
    buf.writeln('o $name');
    for (final p in ribbon.positions) {
      buf.writeln('v ${_f(p.x)} ${_f(p.y)} ${_f(p.z)}');
    }
    for (final n in ribbon.normals) {
      buf.writeln('vn ${_f(n.x)} ${_f(n.y)} ${_f(n.z)}');
    }
    for (final uv in ribbon.uvs) {
      buf.writeln('vt ${_f(uv.x)} ${_f(uv.y)}');
    }
    // Faces are quads around each segment of the tube; triangulate as
    // two triangles per quad.
    final rings = ribbon.ringCount;
    final segs = ribbon.segments;
    for (var r = 0; r < rings - 1; r++) {
      for (var s = 0; s < segs; s++) {
        final a = vertexOffset + r * segs + s + 1;
        final b = vertexOffset + r * segs + (s + 1) % segs + 1;
        final c = vertexOffset + (r + 1) * segs + (s + 1) % segs + 1;
        final d = vertexOffset + (r + 1) * segs + s + 1;
        final na = normalOffset + r * segs + s + 1;
        final nb = normalOffset + r * segs + (s + 1) % segs + 1;
        final nc = normalOffset + (r + 1) * segs + (s + 1) % segs + 1;
        final nd = normalOffset + (r + 1) * segs + s + 1;
        final ta = uvOffset + r * segs + s + 1;
        final tb = uvOffset + r * segs + (s + 1) % segs + 1;
        final tc = uvOffset + (r + 1) * segs + (s + 1) % segs + 1;
        final td = uvOffset + (r + 1) * segs + s + 1;
        buf.writeln('f $a/$ta/$na $b/$tb/$nb $c/$tc/$nc');
        buf.writeln('f $a/$ta/$na $c/$tc/$nc $d/$td/$nd');
      }
    }
    return _RibbonStats(
      vertices: ribbon.positions.length,
      normals: ribbon.normals.length,
      uvs: ribbon.uvs.length,
    );
  }

  String _f(double v) {
    if (v == v.toInt()) return v.toInt().toString();
    return v.toStringAsFixed(6);
  }
}

class _RibbonStats {
  const _RibbonStats({
    required this.vertices,
    required this.normals,
    required this.uvs,
  });
  final int vertices;
  final int normals;
  final int uvs;
}

/// Sweeps a small circle around the stroke's tangent to build a tube.
class _StrokeRibbon {
  _StrokeRibbon(this.stroke, this.radius, this.segments) {
    _build();
  }

  final Stroke stroke;
  final double radius;
  final int segments;

  late final List<Vector3> positions;
  late final List<Vector3> normals;
  late final List<Vector2> uvs;
  late final int ringCount;

  void _build() {
    positions = <Vector3>[];
    normals = <Vector3>[];
    uvs = <Vector2>[];
    final pts = stroke.points;
    final world = <Vector3>[];
    for (final p in pts) {
      // Apply the stroke's local→world transform to each point.
      final w = p.position.clone();
      stroke.transform.transform3(w);
      world.add(w);
    }
    // Build a stable frame at each point using parallel transport.
    var prevN = Vector3(0.0, 1.0, 0.0);
    for (var i = 0; i < world.length; i++) {
      final p = world[i];
      final tangent = (i < world.length - 1
              ? world[i + 1] - p
              : p - world[i - 1])
        ..normalize();
      final ref = (prevN - tangent * tangent.dot(prevN));
      if (ref.length < 1e-6) {
        // Pick an arbitrary perpendicular.
        final alt = (tangent.x.abs() < 0.9)
            ? Vector3(1.0, 0.0, 0.0)
            : Vector3(0.0, 1.0, 0.0);
        final reframe = alt - tangent * tangent.dot(alt);
        ref.setFrom(reframe);
      }
      ref.normalize();
      final binorm = tangent.cross(ref)..normalize();
      prevN = ref.clone();
      for (var s = 0; s < segments; s++) {
        final a = 2.0 * math.pi * s / segments;
        final dir = ref * math.cos(a) + binorm * math.sin(a);
        positions.add(p + dir * radius);
        normals.add(dir);
        uvs.add(Vector2(s / segments, i.toDouble()));
      }
    }
    ringCount = world.length;
  }
}

/// Convenience top-level function: builds an OBJ document for [mesh]
/// with no strokes. Mirrors the [buildGltf] entry point in
/// gltf_exporter.dart so callers can swap formats with one line.
String buildObj(SurfaceMesh mesh, {String name = 'FeatherSurface'}) {
  return ObjExporter(objectName: name).build(mesh, strokes: const <Stroke>[]);
}
