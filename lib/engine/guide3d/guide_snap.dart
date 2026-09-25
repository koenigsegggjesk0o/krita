// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// guide_snap.dart — Snapping logic for 3D Guides.
//
// When the artist drags a control point (or the pen hovers near a guide)
// the editor wants to snap the cursor onto the nearest guide surface so
// strokes land cleanly on the translucent ribbon. [Guide3DSnap] does
// exactly that: given a world-space query point and the active guide
// set, it returns the closest surface hit plus the guide id that owns
// it.
//
// Two snap modes are supported:
//
//   - [snapPoint]      : nearest-surface projection of an arbitrary
//                        world point. Used by the cursor / hover affordance.
//   - [snapRay]        : ray-based pick (the screen-to-world ray). Used
//                        by pen-down — picks the *nearest front-facing*
//                        hit so a curve on the far side of a guide is
//                        not selected through the guide body.
//
// The "both sides drawable" contract is enforced here: when a ray hits a
// back-facing triangle first, the front-facing hit behind it is shadowed
// and [SnapResult.occludedByGuide] is set so the caller can de-select.

import 'package:feather_krita/core/math/math.dart';

import 'guide3d.dart';

/// Result of a snap query.
class SnapResult {
  const SnapResult({
    required this.point,
    required this.normal,
    required this.uv,
    required this.guideId,
    required this.distance,
    this.occludedByGuide = false,
  });

  /// World-space snapped point.
  final Vector3 point;

  /// Surface normal at the snap point.
  final Vector3 normal;

  /// UV on the owning guide's surface.
  final Vector2 uv;

  /// Id of the guide that was snapped to (index into the list passed to
  /// the snap call).
  final int guideId;

  /// Distance from the query point / ray origin to the snap point.
  final double distance;

  /// True when the snap landed on a back-facing triangle that occludes a
  /// front-facing curve behind it — the caller should treat the snap as
  /// "blocked by the guide" and not select any curve.
  final bool occludedByGuide;
}

/// Snapping helper for 3D Guides.
class Guide3DSnap {
  Guide3DSnap({this.maxDistance = 0.25, this.rayMaxDistance = 1e4});

  /// Maximum world-space distance for [snapPoint] queries.
  final double maxDistance;

  /// Maximum ray distance for [snapRay] queries.
  final double rayMaxDistance;

  /// Snaps [world] onto the nearest guide surface in [guides].
  ///
  /// Returns `null` when no guide is within [maxDistance]. The snap is
  /// computed by ray-casting from [world] along the surface normal of
  /// the nearest triangle's centroid — a cheap projection that works
  /// for the common case of a cursor hovering near a guide.
  SnapResult? snapPoint(List<Guide3D> guides, Vector3 world) {
    SnapResult? best;
    for (var i = 0; i < guides.length; i++) {
      final guide = guides[i];
      if (!guide.visible || guide.locked) continue;
      final hit = _nearestSurfacePoint(guide, world);
      if (hit == null) continue;
      if (hit.distance > maxDistance) continue;
      if (best == null || hit.distance < best.distance) {
        best = SnapResult(
          point: hit.point,
          normal: hit.normal,
          uv: hit.uv,
          guideId: i,
          distance: hit.distance,
          occludedByGuide: false,
        );
      }
    }
    return best;
  }

  /// Snaps a screen-to-world [ray] onto the nearest front-facing guide
  /// surface. Returns `null` when the ray misses every guide.
  ///
  /// Implements the both-sides-drawable contract: the *nearest* hit
  /// wins, and if that hit is back-facing, [SnapResult.occludedByGuide]
  /// is set so the caller knows a far-side curve is shadowed.
  SnapResult? snapRay(List<Guide3D> guides, Ray ray) {
    SnapResult? best;
    for (var i = 0; i < guides.length; i++) {
      final guide = guides[i];
      final hit = guide.raycast(ray, maxDistance: rayMaxDistance);
      if (hit == null) continue;
      if (best != null && hit.distance >= best.distance) continue;
      best = SnapResult(
        point: hit.point,
        normal: hit.normal,
        uv: hit.uv,
        guideId: i,
        distance: hit.distance,
        occludedByGuide: hit.backfaceHit,
      );
    }
    return best;
  }

  /// Snaps a ray but skips back-facing hits entirely — used when the
  /// artist is selecting an existing curve and a guide body should not
  /// block the pick (the curve sits on the near side of the guide).
  SnapResult? snapRayFrontOnly(List<Guide3D> guides, Ray ray) {
    SnapResult? best;
    for (var i = 0; i < guides.length; i++) {
      final guide = guides[i];
      final hit = guide.raycast(ray, maxDistance: rayMaxDistance);
      if (hit == null) continue;
      if (hit.backfaceHit) continue;
      if (best != null && hit.distance >= best.distance) continue;
      best = SnapResult(
        point: hit.point,
        normal: hit.normal,
        uv: hit.uv,
        guideId: i,
        distance: hit.distance,
        occludedByGuide: false,
      );
    }
    return best;
  }

  /// Snaps a world point onto a single [guide] only. Convenience for
  /// the per-guide snap used by the stroke recorder when the artist is
  /// drawing onto an already-selected guide.
  SnapResult? snapPointOnGuide(Guide3D guide, Vector3 world) {
    if (!guide.visible || guide.locked) return null;
    final hit = _nearestSurfacePoint(guide, world);
    if (hit == null) return null;
    if (hit.distance > maxDistance) return null;
    return SnapResult(
      point: hit.point,
      normal: hit.normal,
      uv: hit.uv,
      guideId: -1,
      distance: hit.distance,
      occludedByGuide: false,
    );
  }

  // ----- Internals ------------------------------------------------------

  /// Finds the nearest point on [guide]'s surface to [world] by scanning
  /// every triangle's closest point. O(triangles) — fine for guide
  /// meshes (typically < 2k triangles); for very dense meshes a BVH
  /// would be preferable.
  _SurfaceNear? _nearestSurfacePoint(Guide3D guide, Vector3 world) {
    if (guide.mesh.indices.length < 3) return null;
    final inverse = Matrix4.inverted(guide.transform);
    final local = inverse.transform3(world.clone());
    var bestDist2 = double.infinity;
    Vector3? bestPoint;
    Vector3? bestNormal;
    Vector2? bestUv;
    for (var i = 0; i < guide.mesh.indices.length; i += 3) {
      final i0 = guide.mesh.indices[i];
      final i1 = guide.mesh.indices[i + 1];
      final i2 = guide.mesh.indices[i + 2];
      final v0 = guide.mesh.positions[i0];
      final v1 = guide.mesh.positions[i1];
      final v2 = guide.mesh.positions[i2];
      final closest = _closestPointOnTriangle(local, v0, v1, v2);
      final d2 = (closest - local).length2;
      if (d2 < bestDist2) {
        bestDist2 = d2;
        bestPoint = closest.clone();
        // Barycentric for UV / normal interpolation.
        final bary = VectorMathUtils.barycentric3D(closest, v0, v1, v2);
        if (bary != null) {
          final uv0 = guide.mesh.uvs[i0];
          final uv1 = guide.mesh.uvs[i1];
          final uv2 = guide.mesh.uvs[i2];
          bestUv = Vector2(
            uv0.x * bary.x + uv1.x * bary.y + uv2.x * bary.z,
            uv0.y * bary.x + uv1.y * bary.y + uv2.y * bary.z,
          );
          final n0 = guide.mesh.normals[i0];
          final n1 = guide.mesh.normals[i1];
          final n2 = guide.mesh.normals[i2];
          bestNormal = (n0 * bary.x + n1 * bary.y + n2 * bary.z)..normalize();
        } else {
          bestUv = guide.mesh.uvs[i0].clone();
          bestNormal = guide.mesh.normals[i0].clone();
        }
      }
    }
    if (bestPoint == null || bestNormal == null || bestUv == null) return null;
    final worldPoint = guide.transform.transform3(bestPoint);
    final worldNormal =
        VectorMathUtils.transformNormal(guide.transform, bestNormal).normalized();
    final dist = (worldPoint - world).length;
    return _SurfaceNear(
      point: worldPoint,
      normal: worldNormal,
      uv: bestUv,
      distance: dist,
    );
  }

  /// Closest point on triangle [a]-[b]-[c] to [p]. Uses the classic
  /// Ericson "Real-Time Collision Detection" algorithm.
  static Vector3 _closestPointOnTriangle(
      Vector3 p, Vector3 a, Vector3 b, Vector3 c) {
    final ab = b - a;
    final ac = c - a;
    final ap = p - a;
    final d1 = ab.dot(ap);
    final d2 = ac.dot(ap);
    if (d1 <= 0 && d2 <= 0) return a.clone();

    final bp = p - b;
    final d3 = ab.dot(bp);
    final d4 = ac.dot(bp);
    if (d3 >= 0 && d4 <= d3) return b.clone();

    final vc = d1 * d4 - d3 * d2;
    if (vc <= 0 && d1 >= 0 && d3 <= 0) {
      final v = d1 / (d1 - d3);
      return a + ab * v;
    }

    final cp = p - c;
    final d5 = ab.dot(cp);
    final d6 = ac.dot(cp);
    if (d6 >= 0 && d5 <= d6) return c.clone();

    final vb = d5 * d2 - d1 * d6;
    if (vb <= 0 && d2 >= 0 && d6 <= 0) {
      final w = d2 / (d2 - d6);
      return a + ac * w;
    }

    final va = d3 * d6 - d5 * d4;
    if (va <= 0 && (d4 - d3) >= 0 && (d5 - d6) >= 0) {
      final w = (d4 - d3) / ((d4 - d3) + (d5 - d6));
      return b + (c - b) * w;
    }

    final denom = 1.0 / (va + vb + vc);
    final v = vb * denom;
    final w = vc * denom;
    return a + ab * v + ac * w;
  }
}

/// Internal record for the nearest-surface-point search.
class _SurfaceNear {
  const _SurfaceNear({
    required this.point,
    required this.normal,
    required this.uv,
    required this.distance,
  });
  final Vector3 point;
  final Vector3 normal;
  final Vector2 uv;
  final double distance;
}
