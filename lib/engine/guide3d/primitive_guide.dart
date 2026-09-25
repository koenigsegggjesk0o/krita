// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// primitive_guide.dart — Primitives-mode builder (cube / pyramid / sphere / tube).
//
// Feather docs (3dguide_primitives.txt):
//
//   "Tap the shape icon, located to the right of the Loft icon. You can
//    choose from four basic options: Cube, Pyramid, Sphere, and Tube
//    shapes. Shapes are always created at the fixed coordinates (0, 0, 0)."
//
//   "Segment Slider — Use the slider on the left side of the screen to
//    adjust the number of segments for the shape. Slide up to increase
//    the segments and slide down to decrease them. By adjusting the
//    segments of each shape, you can create shapes like cylinders or
//    cones."
//
//   "Use Joystick to Move — Before finalizing the shape, use the joystick
//    to transform it. You can also adjust its scale as needed."
//
// Implementation: a thin facade over [GuideMeshGenerators] that packages
// the chosen primitive into a [Guide3D] of type [Guide3DType.primitive].
// The transform is left at identity (origin placement); the joystick
// moves mutate the transform after construction. Each primitive declares
// a canonical orange starting line so the Bend mode can anchor on it.

import 'package:feather_krita/core/math/math.dart';

import 'guide3d.dart';
import 'guide3d_primitive.dart';
import 'guide3d_type.dart';
import 'guide_mesh_generators.dart';

/// Tunables for the Primitives-mode builder.
class PrimitiveGuideParams {
  const PrimitiveGuideParams({
    this.size = 2.0,
    this.height = 2.0,
    this.radius = 1.0,
    this.segments,
    this.opacity = 0.5,
  });

  /// Edge length for [Guide3DPrimitive.cube].
  final double size;

  /// Height for [Guide3DPrimitive.pyramid] and [Guide3DPrimitive.tube].
  final double height;

  /// Radius for [Guide3DPrimitive.pyramid], [Guide3DPrimitive.sphere]
  /// and [Guide3DPrimitive.tube].
  final double radius;

  /// Segment count override. Falls back to the primitive's
  /// [Guide3DPrimitive.defaultSegments] when null.
  final int? segments;

  /// Default opacity for the new guide.
  final double opacity;
}

/// Builds a [Guide3D] of type [Guide3DType.primitive].
class PrimitiveGuideBuilder {
  const PrimitiveGuideBuilder();

  /// Builds a primitive guide of [kind] with [params].
  ///
  /// The guide is placed at the world origin (identity transform) per the
  /// Feather docs. The joystick moves are applied afterwards via the
  /// guide's [Guide3D.transform].
  Guide3D build({
    required Guide3DPrimitive kind,
    PrimitiveGuideParams params = const PrimitiveGuideParams(),
    String? name,
  }) {
    final segments = clampPrimitiveSegments(kind, params.segments);
    final mesh = _buildMesh(kind, params, segments);
    final start = _startEdgeFor(kind, mesh, segments);
    return Guide3D(
      type: Guide3DType.primitive,
      mesh: mesh,
      startPoint: start.first,
      startEnd: start.second,
      opacity: params.opacity,
      name: name,
      meta: <String, dynamic>{
        'mode': 'primitive',
        'primitive': kind.wireName,
        'size': params.size,
        'height': params.height,
        'radius': params.radius,
        'segments': segments,
      },
    );
  }

  /// Rebuilds a primitive guide with a new segment count. Used by the
  /// Segment slider's live-preview step before Done is tapped.
  Guide3D withSegments(Guide3D guide, int segments) {
    final kind = guide3DPrimitiveFromName(guide.meta['primitive']);
    final params = PrimitiveGuideParams(
      size: (guide.meta['size'] as num?)?.toDouble() ?? 2.0,
      height: (guide.meta['height'] as num?)?.toDouble() ?? 2.0,
      radius: (guide.meta['radius'] as num?)?.toDouble() ?? 1.0,
      segments: segments,
      opacity: guide.opacity,
    );
    final rebuilt = build(kind: kind, params: params, name: guide.name)
      ..transform = guide.transform.clone()
      ..visible = guide.visible
      ..locked = guide.locked
      ..drawnCurves
          .addAll(guide.drawnCurves.map((c) => c.clone()));
    rebuilt.markTransformDirty();
    return rebuilt;
  }

  /// Restores a primitive guide from JSON. Rebuilds the mesh from the
  /// stored primitive kind + segments.
  Guide3D fromJson(Map<String, dynamic> json) {
    final meta = (json['meta'] as Map?)?.cast<String, dynamic>() ?? {};
    final kind = guide3DPrimitiveFromName(meta['primitive'] as String?);
    final params = PrimitiveGuideParams(
      size: (meta['size'] as num?)?.toDouble() ?? 2.0,
      height: (meta['height'] as num?)?.toDouble() ?? 2.0,
      radius: (meta['radius'] as num?)?.toDouble() ?? 1.0,
      segments: (meta['segments'] as num?)?.toInt(),
      opacity: (json['opacity'] as num?)?.toDouble() ?? 0.5,
    );
    final guide = build(kind: kind, params: params, name: json['name'] as String?);
    guide.applyJson(json);
    return guide;
  }

  // ----- Per-primitive mesh + start edge -------------------------------

  Guide3DMesh _buildMesh(
    Guide3DPrimitive kind,
    PrimitiveGuideParams params,
    int segments,
  ) {
    switch (kind) {
      case Guide3DPrimitive.cube:
        return GuideMeshGenerators.cubeMesh(
            size: params.size, segments: segments);
      case Guide3DPrimitive.pyramid:
        return GuideMeshGenerators.pyramidMesh(
            baseRadius: params.radius,
            height: params.height,
            segments: segments);
      case Guide3DPrimitive.sphere:
        return GuideMeshGenerators.sphereMesh(
            radius: params.radius, segments: segments);
      case Guide3DPrimitive.tube:
        return GuideMeshGenerators.tubeMesh(
            radius: params.radius,
            height: params.height,
            segments: segments);
    }
  }

  /// Canonical orange starting line for each primitive.
  ///
  ///   - cube      : bottom-front edge (vertices 0 → 1 of the +Z face).
  ///   - pyramid   : apex → first base vertex (the silhouette edge).
  ///   - sphere    : top pole → first ring vertex (a meridian segment).
  ///   - tube      : first edge of the bottom ring (a base chord).
  ///
  /// These are picked so the artist can visually locate the bend anchor
  /// regardless of which primitive was inserted.
  ({Vector3 first, Vector3 second}) _startEdgeFor(
    Guide3DPrimitive kind,
    Guide3DMesh mesh,
    int segments,
  ) {
    if (mesh.positions.length < 2) {
      final p = mesh.positions.isEmpty ? Vector3.zero() : mesh.positions.first;
      return (first: p.clone(), second: p.clone());
    }
    switch (kind) {
      case Guide3DPrimitive.cube:
        // +Z face is built first; vertices 0 and 1 are its bottom edge.
        return (
          first: mesh.positions[0].clone(),
          second: mesh.positions[1].clone(),
        );
      case Guide3DPrimitive.pyramid:
        // Vertex 0 is the apex, vertex 1 is the first base vertex.
        return (
          first: mesh.positions[0].clone(),
          second: mesh.positions[1].clone(),
        );
      case Guide3DPrimitive.sphere:
        // Vertex 0 is the top pole, vertex 1 is the first ring vertex.
        return (
          first: mesh.positions[0].clone(),
          second: mesh.positions[1].clone(),
        );
      case Guide3DPrimitive.tube:
        // Vertices 0 and 1 are the bottom ring's first chord.
        return (
          first: mesh.positions[0].clone(),
          second: mesh.positions[1].clone(),
        );
    }
  }
}
