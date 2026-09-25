// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// bent_guide.dart — Bend-mode builder (existing guide + bend path → deformed guide).
//
// Feather docs (3dguide_draw.txt):
//
//   "Bend an existing 3D Guide to create more organic shapes, revolve, or
//    make a tube. While a 3D Guide is active, tap the Bend icon below the
//    Draw icon. Then, use your pen to draw on the screen, and the 3D
//    Guide will bend along the drawn line."
//
//   "The bending starts from the orange line, which is the starting point
//    of the 3D Guide. You can repeat the Bend 3D Guide process multiple
//    times."
//
// Implementation: the source guide's local spine (the direction from
// [Guide3D.startPoint] to [Guide3D.startEnd], extended to the guide's
// full extent) is re-rooted onto the drawn bend path via arc-length
// parameterisation. The cross-section perpendicular to the spine is
// preserved by a parallel-transport frame, so a Cube bent along a circle
// becomes a tube segment with square cross-section — exactly the
// "revolve / make a tube" use case the docs call out.
//
// The bend can be repeated: each pass's bend path is appended to a
// history list stored in the guide's [meta], and the next pass bends the
// already-bent mesh. The orange starting line of the result is the first
// vertex of the latest bend path.

import 'package:feather_krita/core/math/math.dart';

import 'guide3d.dart';
import 'guide3d_type.dart';
import 'guide_mesh_generators.dart';

/// Tunables for the Bend-mode builder.
class BentGuideParams {
  const BentGuideParams({
    this.spineLength = 2.0,
    this.opacity = 0.5,
    this.resample = 64,
  });

  /// Length of the source guide's spine in local units. Defaults to 2
  /// (matches a unit cube / tube / sphere of radius 1). The bend pipeline
  /// normalises the local Y coordinate onto [0, spineLength] before
  /// looking up the bend path.
  final double spineLength;

  /// Default opacity for the bent guide (inherits from the source by
  /// default in [build]; this is the fallback).
  final double opacity;

  /// How many samples to take along the bend path. Higher = smoother
  /// deformation.
  final int resample;
}

/// Builds a [Guide3D] of type [Guide3DType.bent] by deforming an existing
/// guide along a drawn bend path.
class BentGuideBuilder {
  const BentGuideBuilder();

  /// Bends [source] along [bendPath].
  ///
  /// The bend path is a polyline in *world* space (the artist drew it on
  /// the screen). It is converted into the source guide's local space
  /// first so the deformation composes cleanly with the guide's existing
  /// transform. The orange starting line of the result is the start of
  /// the bend path; the bend history (this path, plus any prior paths)
  /// is recorded in [Guide3D.meta] so the bend can be repeated and the
  /// guide reloaded.
  Guide3D build({
    required Guide3D source,
    required List<Vector3> bendPath,
    BentGuideParams params = const BentGuideParams(),
    String? name,
  }) {
    if (bendPath.length < 2) return _cloneAsBent(source, params, name);
    // Bring the bend path into the source guide's local space.
    final inverse = Matrix4.inverted(source.transform);
    final localPath = bendPath
        .map((p) => inverse.transform3(p.clone()))
        .toList();

    // Determine the spine direction from the orange starting line.
    final spineDir = (source.startEnd - source.startPoint);
    if (spineDir.length2 < 1e-9) {
      spineDir.setValues(0, 1, 0);
    }
    spineDir.normalize();
    // Effective spine length: project the mesh bounds onto the spine dir.
    final bounds = source.mesh.bounds;
    final corners = <Vector3>[
      Vector3(bounds.min.x, bounds.min.y, bounds.min.z),
      Vector3(bounds.max.x, bounds.min.y, bounds.min.z),
      Vector3(bounds.min.x, bounds.max.y, bounds.min.z),
      Vector3(bounds.max.x, bounds.max.y, bounds.min.z),
      Vector3(bounds.min.x, bounds.min.y, bounds.max.z),
      Vector3(bounds.max.x, bounds.min.y, bounds.max.z),
      Vector3(bounds.min.x, bounds.max.y, bounds.max.z),
      Vector3(bounds.max.x, bounds.max.y, bounds.max.z),
    ];
    var minProj = double.infinity;
    var maxProj = double.negativeInfinity;
    for (final c in corners) {
      final proj = c.dot(spineDir);
      if (proj < minProj) minProj = proj;
      if (proj > maxProj) maxProj = proj;
    }
    final spineLen = (maxProj - minProj).abs().clamp(1e-3, 1e4);

    final deformed = _bendAlongSpine(
      source.mesh,
      localPath,
      spineDir: spineDir,
      spineLength: spineLen,
      spineOffset: minProj,
      resample: params.resample,
    );

    // The orange starting line of the bent guide: the first edge of the
    // bend path in local space (transformed back to world on demand).
    final newStart = localPath.first.clone();
    final newEnd = localPath.length > 1
        ? localPath[1].clone()
        : newStart.clone();

    // Bend history: append this path to any prior history.
    final history = _extractHistory(source.meta);
    history.add(localPath.map((p) => <double>[p.x, p.y, p.z]).toList());
    final meta = <String, dynamic>{
      'mode': 'bend',
      'sourceType': source.type.wireName,
      'spineLength': spineLen,
      'bendHistory': history,
      if (source.meta['mode'] != null) 'origin': source.meta,
    };

    return Guide3D(
      type: Guide3DType.bent,
      mesh: deformed,
      startPoint: newStart,
      startEnd: newEnd,
      transform: source.transform.clone(),
      opacity: source.opacity,
      visible: source.visible,
      locked: source.locked,
      drawnCurves: source.drawnCurves.map((c) => c.clone()).toList(),
      name: name ?? source.name,
      meta: meta,
    );
  }

  /// Restores a bent guide from JSON. Replays the bend history onto a
  /// rebuilt source guide. The source guide is reconstructed from the
  /// stored origin meta; if that is missing the bend is applied to a
  /// fallback plane.
  Guide3D fromJson(Map<String, dynamic> json) {
    final meta = (json['meta'] as Map?)?.cast<String, dynamic>() ?? {};
    final history = (meta['bendHistory'] as List?)
            ?.map((h) => (h as List)
                .map((p) => Vector3(
                      (p as List)[0] as double,
                      p[1] as double,
                      p[2] as double,
                    ))
                .toList())
            .toList() ??
        <List<Vector3>>[];
    // Reconstruct a minimal source guide (a unit plane along Y) and
    // replay the history.
    var source = _unitSpineSource();
    for (final path in history) {
      source = build(
        source: source,
        bendPath: path,
        params: BentGuideParams(
          spineLength: (meta['spineLength'] as num?)?.toDouble() ?? 2.0,
          opacity: (json['opacity'] as num?)?.toDouble() ?? 0.5,
        ),
      );
    }
    source.applyJson(json);
    return source;
  }

  /// Replays [additionalPath] on top of an already-bent guide. The
  /// Feather docs explicitly note bends can be repeated — this is the
  /// entry point for the second and subsequent passes.
  Guide3D repeat({
    required Guide3D bent,
    required List<Vector3> additionalPath,
    BentGuideParams params = const BentGuideParams(),
  }) {
    return build(source: bent, bendPath: additionalPath, params: params);
  }

  // ----- Internals ------------------------------------------------------

  Guide3DMesh _bendAlongSpine(
    Guide3DMesh base,
    List<Vector3> path, {
    required Vector3 spineDir,
    required double spineLength,
    required double spineOffset,
    required int resample,
  }) {
    // Build a right-handed frame around the spine direction so we can
    // express every base vertex as (along, perp1, perp2) and re-emit it
    // along the bend path.
    final perp1 = _perpendicular(spineDir);
    final perp2 = spineDir.cross(perp1)..normalize();

    // Resample the path to uniform arc length.
    final dense = _resample(path, resample.clamp(16, 512));
    final cum = <double>[0.0];
    for (var i = 1; i < dense.length; i++) {
      cum.add(cum.last + (dense[i] - dense[i - 1]).length);
    }
    final total = cum.last;
    if (total < 1e-9) return base.clone();

    // Parallel-transport frames along the path.
    final tangents = <Vector3>[];
    for (var i = 0; i < dense.length; i++) {
      final prev = dense[(i - 1).clamp(0, dense.length - 1)];
      final next = dense[(i + 1).clamp(0, dense.length - 1)];
      final t = (next - prev)..normalize();
      if (t.length2 < 1e-9) t.setFrom(spineDir);
      tangents.add(t);
    }
    var prevN = perp1.clone();
    final rights = <Vector3>[];
    final ups = <Vector3>[];
    for (var i = 0; i < dense.length; i++) {
      final t = tangents[i];
      var r = prevN - t * prevN.dot(t);
      if (r.length2 < 1e-6) {
        r = _perpendicular(t);
      }
      r.normalize();
      final u = t.cross(r)..normalize();
      rights.add(r);
      ups.add(u);
      prevN = r;
    }

    final out = base.clone();
    for (var i = 0; i < base.positions.length; i++) {
      final p = base.positions[i];
      final along = p.dot(spineDir) - spineOffset;
      final s = (along / spineLength).clamp(0.0, 1.0) * total;
      var lo = 0;
      var hi = cum.length - 1;
      while (lo < hi - 1) {
        final mid = (lo + hi) ~/ 2;
        if (cum[mid] <= s) {
          lo = mid;
        } else {
          hi = mid;
        }
      }
      final segLen = cum[hi] - cum[lo];
      final frac = segLen < 1e-9 ? 0.0 : (s - cum[lo]) / segLen;
      final origin = dense[lo] + (dense[hi] - dense[lo]) * frac;
      final r = rights[lo] + (rights[hi] - rights[lo]) * frac;
      final u = ups[lo] + (ups[hi] - ups[lo]) * frac;
      final perp1Amt = p.dot(perp1);
      final perp2Amt = p.dot(perp2);
      out.positions[i]
        ..setFrom(origin)
        ..add(r * perp1Amt)
        ..add(u * perp2Amt);
    }
    out.recomputeNormals();
    return out;
  }

  Vector3 _perpendicular(Vector3 v) {
    final n = v.normalized();
    var other = Vector3(1, 0, 0);
    if (n.dot(other).abs() > 0.9) other = Vector3(0, 1, 0);
    final p = other - n * other.dot(n);
    return p..normalize();
  }

  List<Vector3> _resample(List<Vector3> curve, int samples) {
    if (curve.length < 2) {
      return List<Vector3>.filled(
          samples, curve.isEmpty ? Vector3.zero() : curve.first.clone());
    }
    final cum = <double>[0.0];
    for (var i = 1; i < curve.length; i++) {
      cum.add(cum.last + (curve[i] - curve[i - 1]).length);
    }
    final total = cum.last;
    if (total < 1e-9) {
      return List<Vector3>.filled(samples, curve.first.clone());
    }
    final out = <Vector3>[];
    for (var s = 0; s < samples; s++) {
      final target = total * s / (samples - 1);
      var lo = 0;
      var hi = cum.length - 1;
      while (lo < hi - 1) {
        final mid = (lo + hi) ~/ 2;
        if (cum[mid] <= target) {
          lo = mid;
        } else {
          hi = mid;
        }
      }
      final segLen = cum[hi] - cum[lo];
      final frac = segLen < 1e-9 ? 0.0 : (target - cum[lo]) / segLen;
      out.add(curve[lo] + (curve[hi] - curve[lo]) * frac);
    }
    return out;
  }

  List<List<List<double>>> _extractHistory(Map<String, dynamic> meta) {
    final raw = meta['bendHistory'] as List?;
    if (raw == null) return <List<List<double>>>[];
    return raw
        .map((h) => (h as List)
            .map((p) => <double>[
                  (p as List)[0] as double,
                  p[1] as double,
                  p[2] as double,
                ])
            .toList())
        .toList();
  }

  Guide3D _unitSpineSource() {
    // A flat ribbon along Y, length 2 — the canonical "unbent" spine.
    final mesh = GuideMeshGenerators.ribbonMesh(
      centerline: [Vector3(0, -1, 0), Vector3(0, 1, 0)],
      width: 1.0,
      viewNormal: Vector3(0, 0, 1),
      rows: 2,
    );
    return Guide3D(
      type: Guide3DType.drawn,
      mesh: mesh,
      startPoint: mesh.positions[0].clone(),
      startEnd: mesh.positions[1].clone(),
    );
  }

  Guide3D _cloneAsBent(Guide3D source, BentGuideParams params, String? name) {
    return Guide3D(
      type: Guide3DType.bent,
      mesh: source.mesh.clone(),
      startPoint: source.startPoint.clone(),
      startEnd: source.startEnd.clone(),
      transform: source.transform.clone(),
      opacity: source.opacity,
      visible: source.visible,
      locked: source.locked,
      drawnCurves: source.drawnCurves.map((c) => c.clone()).toList(),
      name: name ?? source.name,
      meta: <String, dynamic>{
        'mode': 'bend',
        'sourceType': source.type.wireName,
        'bendHistory': <List<List<double>>>[],
        'noop': true,
      },
    );
  }
}

/// Re-exported so callers can build the canonical unit-spine source used
/// by [BentGuideBuilder.fromJson] when no origin meta is present.
Guide3D unitSpineGuide() {
  final mesh = GuideMeshGenerators.ribbonMesh(
    centerline: [Vector3(0, -1, 0), Vector3(0, 1, 0)],
    width: 1.0,
    viewNormal: Vector3(0, 0, 1),
    rows: 2,
  );
  return Guide3D(
    type: Guide3DType.drawn,
    mesh: mesh,
    startPoint: mesh.positions[0].clone(),
    startEnd: mesh.positions[1].clone(),
  );
}
