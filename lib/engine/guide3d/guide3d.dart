
// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// guide3d.dart — Base class for every Feather-style 3D Guide.
//
// A 3D Guide is a translucent surface the artist draws curves ON. Four
// creation modes (Draw, Loft, Primitives, Bend) all reduce to the same
// runtime representation: a [Guide3DMesh] in local space, placed by a
// [Matrix4] transform, with an opacity, a list of curves the artist has
// painted on it, and a trackable orange starting line.
//
// This file defines that common representation and the two operations
// every guide must support:
//
//   - [raycast]           : convert a world-space screen ray into a hit on
//                           the surface (used to place pen strokes).
//   - [projectToSurface]  : convenience that wraps [raycast] and returns
//                           the (uv, point, normal) triple a stroke
//                           recorder needs.
//
// The per-mode builders (drawn_guide / lofted_guide / primitive_guide /
// bent_guide) are responsible for assembling the [Guide3DMesh]; this
// class is agnostic to how the mesh was produced.
//
// Both-sides-drawable contract: a guide is a translucent surface, but a
// curve painted on the *far* side of the guide cannot be selected (the
// guide occludes it). [raycast] honours this by returning the *nearest*
// hit only — when the artist taps a far-side curve, the guide itself is
// hit first and the curve is shadowed. The renderer flags the occluded
// side via [backfaceHit] so the snap layer can de-select accordingly.

import 'package:feather_krita/core/math/math.dart';

import 'guide3d_type.dart';
import 'guide_mesh_generators.dart';

/// One curve the artist has painted onto a guide.
///
/// Stored in the guide's *local* space so the curve follows the guide
/// when it is transformed. Each point carries the UV at which it was
/// painted so the stroke can be re-projected onto a re-tessellated
/// surface without re-raycasting.
class GuideDrawnCurve {
  GuideDrawnCurve({
    required this.points,
    required this.uvs,
    this.color = 0xFFFFFFFF,
  }) : assert(points.length == uvs.length);

  /// Local-space positions of the curve samples.
  final List<Vector3> points;

  /// UV coordinates on the guide surface, one per [points] entry.
  final List<Vector2> uvs;

  /// ARGB color the curve was painted with.
  final int color;

  GuideDrawnCurve clone() => GuideDrawnCurve(
        points: points.map((p) => p.clone()).toList(),
        uvs: uvs.map((u) => u.clone()).toList(),
        color: color,
      );

  Map<String, dynamic> toJson() => {
        'points': points
            .map((p) => <double>[p.x, p.y, p.z])
            .toList(),
        'uvs': uvs.map((u) => <double>[u.x, u.y]).toList(),
        'color': color,
      };

  factory GuideDrawnCurve.fromJson(Map<String, dynamic> json) {
    final pts = (json['points'] as List)
        .map((e) => Vector3(
              (e as List)[0] as double,
              e[1] as double,
              e[2] as double,
            ))
        .toList();
    final uvs = (json['uvs'] as List)
        .map((e) => Vector2((e as List)[0] as double, e[1] as double))
        .toList();
    return GuideDrawnCurve(
      points: pts,
      uvs: uvs,
      color: (json['color'] as num?)?.toInt() ?? 0xFFFFFFFF,
    );
  }
}

/// Result of a [Guide3D.raycast] / [Guide3D.projectToSurface] call.
class Guide3DHit {
  const Guide3DHit({
    required this.uv,
    required this.point,
    required this.normal,
    required this.distance,
    required this.triangleIndex,
    required this.backfaceHit,
  });

  /// UV on the guide surface in [0, 1]².
  final Vector2 uv;

  /// World-space hit point.
  final Vector3 point;

  /// Interpolated surface normal in world space.
  final Vector3 normal;

  /// Distance from the ray origin to the hit point.
  final double distance;

  /// Triangle index that was hit.
  final int triangleIndex;

  /// True when the hit landed on a back-facing triangle — the renderer
  /// uses this to mark the occluded side of the guide.
  final bool backfaceHit;
}

/// A 3D Guide surface.
///
/// Subclassing is by composition, not inheritance: the per-mode builders
/// return a configured [Guide3D] with [type] set appropriately. This
/// keeps the four creation modes uniform at runtime while still letting
/// each builder attach mode-specific metadata via [meta].
class Guide3D {
  Guide3D({
    required this.type,
    required this.mesh,
    required this.startPoint,
    required this.startEnd,
    Matrix4? transform,
    this.opacity = 0.5,
    this.visible = true,
    this.locked = false,
    List<GuideDrawnCurve>? drawnCurves,
    Map<String, dynamic>? meta,
    this.name,
  })  : transform = transform ?? Matrix4.identity(),
        drawnCurves = drawnCurves ?? [],
        meta = meta ?? <String, dynamic>{};

  /// Creation mode this guide originated from.
  final Guide3DType type;

  /// Tessellated surface mesh in local space.
  final Guide3DMesh mesh;

  /// World-space transform placing the mesh in the scene.
  Matrix4 transform;

  /// Translucency in [0, 1]; 0.5 matches Feather's default.
  double opacity;

  /// Whether the guide is rendered and ray-pickable.
  bool visible;

  /// When true the guide is shown but cannot be selected or have its
  /// opacity changed — matches Feather's lock affordance.
  bool locked;

  /// Curves the artist has painted on this guide, in local space.
  final List<GuideDrawnCurve> drawnCurves;

  /// Mode-specific metadata (e.g. primitive kind, bend history, loft
  /// tension). Stored verbatim in JSON.
  final Map<String, dynamic> meta;

  /// Optional human-readable name (shown in the Resources tab).
  String? name;

  /// First endpoint of the orange starting line, in *local* space.
  /// Transform by [transform] for world space.
  final Vector3 startPoint;

  /// Second endpoint of the orange starting line.
  final Vector3 startEnd;

  // ----- Cached inverse -------------------------------------------------

  Matrix4 _inverse = Matrix4.identity();
  bool _inverseDirty = true;

  void _ensureInverse() {
    if (_inverseDirty) {
      _inverse = Matrix4.inverted(transform);
      _inverseDirty = false;
    }
  }

  /// Marks the cached inverse dirty after [transform] is mutated.
  void markTransformDirty() => _inverseDirty = true;

  /// World-space first endpoint of the orange starting line.
  Vector3 get startLineWorldStart => transform.transform3(Vector3(startPoint.x, startPoint.y, startPoint.z));

  /// World-space second endpoint of the orange starting line.
  Vector3 get startLineWorldEnd => transform.transform3(Vector3(startEnd.x, startEnd.y, startEnd.z));

  /// Local-space centre of the surface mesh.
  Vector3 get localCenter => mesh.bounds.center;

  /// World-space centre of the surface mesh.
  Vector3 get worldCenter => transform.transform3(Vector3(localCenter.x, localCenter.y, localCenter.z));

  // ----- Picking --------------------------------------------------------

  /// Casts [ray] (world space) against this guide and returns the
  /// nearest hit, or `null` if no triangle is intersected.
  ///
  /// Honours [visible] and [locked]. The ray is transformed into the
  /// mesh's local space and tested with Möller-Trumbore; the closest
  /// hit is converted back to world space and its UV / normal are
  /// barycentrically interpolated. The [Guide3DHit.backfaceHit] flag is
  /// set when the hit triangle's normal faces away from the ray — the
  /// renderer uses this to mark the occluded side of the guide.
  Guide3DHit? raycast(Ray ray, {double maxDistance = double.infinity}) {
    if (!visible || locked) return null;
    _ensureInverse();

    final Vector3 localOrigin = _inverse.transform3(Vector3.copy(ray.origin as dynamic));
    final Vector3 localDir = _inverse.transform3(Vector3.copy(ray.direction as dynamic));
    final Vector3 localDirN = localDir.normalized();

    var bestT = maxDistance;
    var bestTri = -1;
    var bestBary = Vector3.zero();
    var bestBackface = false;
    final eps = 1e-7;
    for (var i = 0; i < mesh.indices.length; i += 3) {
      final i0 = mesh.indices[i];
      final i1 = mesh.indices[i + 1];
      final i2 = mesh.indices[i + 2];
      final hit = VectorMathUtils.rayTriangle(
        localOrigin,
        localDirN,
        mesh.positions[i0],
        mesh.positions[i1],
        mesh.positions[i2],
        epsilon: eps,
      );
      if (hit == null) continue;
      if (hit.t < eps || hit.t >= bestT) continue;
      // Backface: triangle normal vs ray direction in local space.
      final e1 = mesh.positions[i1] - mesh.positions[i0];
      final e2 = mesh.positions[i2] - mesh.positions[i0];
      final n = e1.cross(e2);
      bestBackface = n.dot(localDirN) > 0.0;
      bestT = hit.t;
      bestTri = i ~/ 3;
      bestBary = hit.barycentric;
    }
    if (bestTri < 0) return null;

    final i0 = mesh.indices[bestTri * 3];
    final i1 = mesh.indices[bestTri * 3 + 1];
    final i2 = mesh.indices[bestTri * 3 + 2];
    final b = bestBary;
    final uv0 = mesh.uvs[i0];
    final uv1 = mesh.uvs[i1];
    final uv2 = mesh.uvs[i2];
    final uv = Vector2(
      uv0.x * b.x + uv1.x * b.y + uv2.x * b.z,
      uv0.y * b.x + uv1.y * b.y + uv2.y * b.z,
    );
    final localHit = localOrigin + localDirN * bestT;
    final n0 = mesh.normals[i0];
    final n1 = mesh.normals[i1];
    final n2 = mesh.normals[i2];
    final localNormal = (n0 * b.x + n1 * b.y + n2 * b.z)..normalize();
    final worldPoint = transform.transform3(Vector3(localHit.x, localHit.y, localHit.z));
    final worldNormal = VectorMathUtils.transformNormal(transform, localNormal)
        .normalized();
    return Guide3DHit(
      uv: uv,
      point: worldPoint,
      normal: worldNormal,
      distance: bestT,
      triangleIndex: bestTri,
      backfaceHit: bestBackface,
    );
  }

  /// Convenience wrapper around [raycast] that returns the (uv, point,
  /// normal) triple a stroke recorder needs. Returns `null` when the
  /// ray misses the guide.
  ({Vector2 uv, Vector3 point, Vector3 normal})? projectToSurface(Ray ray) {
    final hit = raycast(ray);
    if (hit == null) return null;
    return (uv: hit.uv, point: hit.point, normal: hit.normal);
  }

  /// Adds a [GuideDrawnCurve] already expressed in *local* space. The
  /// caller (stroke manager) is responsible for converting a world-space
  /// stroke into local space via the inverse transform.
  void addDrawnCurve(GuideDrawnCurve curve) {
    drawnCurves.add(curve);
  }

  /// Removes every drawn curve whose local-space points all sit outside
  /// [maxDistance] from [worldPoint]. Used by the eraser.
  void pruneCurvesNear(Vector3 worldPoint, double maxDistance) {
    _ensureInverse();
    final local = _inverse.transform3(Vector3(worldPoint.x, worldPoint.y, worldPoint.z));
    drawnCurves.removeWhere((c) {
      for (final p in c.points) {
        if ((p - local).length <= maxDistance) return false;
      }
      return true;
    });
  }

  // ----- Serialization --------------------------------------------------

  Map<String, dynamic> toJson() => {
        'type': type.wireName,
        'name': name,
        'opacity': opacity,
        'visible': visible,
        'locked': locked,
        'transform': transform.storage.toList(),
        'startPoint': [startPoint.x, startPoint.y, startPoint.z],
        'startEnd': [startEnd.x, startEnd.y, startEnd.z],
        'drawnCurves': drawnCurves.map((c) => c.toJson()).toList(),
        if (meta.isNotEmpty) 'meta': Map<String, dynamic>.of(meta),
        // Mesh is rebuilt by the per-mode builder on load — only the
        // construction parameters (in [meta]) are persisted.
      };

  /// Restores the transform / opacity / curves onto a guide that was
  /// already rebuilt by the per-mode builder. Called from the builders'
  /// fromJson implementations.
  void applyJson(Map<String, dynamic> json) {
    final tList = (json['transform'] as List).cast<num>();
    transform = Matrix4.fromList(tList.map((e) => e.toDouble()).toList());
    markTransformDirty();
    opacity = (json['opacity'] as num?)?.toDouble() ?? 0.5;
    visible = json['visible'] as bool? ?? true;
    locked = json['locked'] as bool? ?? false;
    name = json['name'] as String?;
    final curves = json['drawnCurves'] as List? ?? const [];
    drawnCurves
      ..clear()
      ..addAll(curves
          .map((e) => GuideDrawnCurve.fromJson(e as Map<String, dynamic>)));
    final m = json['meta'];
    if (m is Map) {
      meta
        ..clear()
        ..addAll(m.cast<String, dynamic>());
    }
  }
}
