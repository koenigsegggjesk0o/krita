// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// duplicate.dart — Duplicate engine: copy, symmetric-by-view, symmetric-by-mirror.
//
// Per docs/selection_duplicate.txt:
//
//   "Duplicate is essential for repetitive patterns, structural shapes,
//    or just faster sketching."
//
//   - **Duplicate**           — "Select the curve you want to duplicate
//     and tap the leftmost icon. The duplicated curves are in the same
//     position as the original, so be careful not to confuse them."
//   - **Symmetrically by View** — "duplicate symmetrically based on the
//     view direction. If the sketch is skewed to the right, it will be
//     duplicated to the left, and vice versa."
//   - **Symmetrically by Mirror** — "can only be used when the mirror is
//     on. It duplicates symmetrically based on the currently active
//     mirror axis. If multiple axes are active, multiple curves will be
//     duplicated at once."
//
// The mirror plane for "by view" is the camera's right axis: a point at
// +right gets mirrored to -right, leaving the up and forward components
// unchanged. (Equivalently: the mirror plane's normal is `frame.right`.)
// For "by mirror" the active [MirrorConfig] axes drive the reflection
// directly — one copy per non-identity sign vector.

import 'package:vector_math/vector_math_64.dart';

import 'package:feather_krita/models/stroke.dart';

/// Result of a duplicate operation: the freshly-created strokes + a
/// short message for the toast (per the doc: "A message will appear with
/// the number of duplicated curves").
class DuplicateResult {
  DuplicateResult(this.strokes, this.message);
  final List<Stroke> strokes;
  final String message;
  int get count => strokes.length;
}

/// Configuration of the live mirror (re-declared here to avoid a
/// dependency on `lib/engine/stroke_manager.dart`, which would create a
/// cycle when that module imports this one).
class MirrorAxes {
  const MirrorAxes({
    this.x = false,
    this.y = false,
    this.z = false,
    this.origin,
  });
  final bool x;
  final bool y;
  final bool z;
  final Vector3? origin;

  bool get anyEnabled => x || y || z;
  Vector3 get originPoint => origin ?? Vector3.zero();

  /// Returns the list of sign vectors to mirror across (excluding the
  /// identity). For one enabled axis this is one mirror; for two axes
  /// it's three (the two singles plus the combined); for three it's
  /// seven (all non-identity combinations).
  List<Vector3> signVectors() {
    final out = <Vector3>[];
    final xs = x ? [1.0, -1.0] : [1.0];
    final ys = y ? [1.0, -1.0] : [1.0];
    final zs = z ? [1.0, -1.0] : [1.0];
    for (final sx in xs) {
      for (final sy in ys) {
        for (final sz in zs) {
          if (sx == 1.0 && sy == 1.0 && sz == 1.0) continue;
          out.add(Vector3(sx, sy, sz));
        }
      }
    }
    return out;
  }
}

/// The duplicate engine.
class DuplicateEngine {
  DuplicateEngine({this.nextId = 1});

  /// The next stroke ID to assign. Bumped after every duplicate.
  int nextId;

  /// Plain duplicate: each input stroke produces one copy at the same
  /// position. Per the doc: "duplicated curves are in the same position
  /// as the original."
  DuplicateResult duplicate(List<Stroke> strokes) {
    final out = <Stroke>[];
    for (final s in strokes) {
      out.add(_copy(s));
    }
    return DuplicateResult(out,
        '${out.length} curve${out.length == 1 ? '' : 's'} duplicated');
  }

  /// Symmetric-by-view duplicate: each stroke produces one mirrored
  /// copy across the plane whose normal is [viewRight] (the camera's
  /// right direction). The mirror origin defaults to the scene origin
  /// (which is also the screen crosshair target in Feather).
  DuplicateResult duplicateByView(
    List<Stroke> strokes,
    Vector3 viewRight, {
    Vector3? origin,
  }) {
    final o = origin ?? Vector3.zero();
    final out = <Stroke>[];
    for (final s in strokes) {
      out.add(_mirrorAcrossPlane(s, viewRight, o));
    }
    return DuplicateResult(out,
        '${out.length} curve${out.length == 1 ? '' : 's'} duplicated symmetrically by view');
  }

  /// Symmetric-by-mirror duplicate: each stroke produces one mirrored
  /// copy per active mirror axis combination. Per the doc: "If multiple
  /// axes are active, multiple curves will be duplicated at once."
  DuplicateResult duplicateByMirror(
    List<Stroke> strokes,
    MirrorAxes axes,
  ) {
    if (!axes.anyEnabled) {
      return DuplicateResult(const [], 'Mirror is off — nothing duplicated');
    }
    final signs = axes.signVectors();
    final out = <Stroke>[];
    for (final s in strokes) {
      for (final sign in signs) {
        out.add(_mirrorBySign(s, axes.originPoint, sign));
      }
    }
    return DuplicateResult(out,
        '${out.length} curve${out.length == 1 ? '' : 's'} duplicated symmetrically by mirror');
  }

  // ----- Internal helpers ---------------------------------------------

  Stroke _copy(Stroke s) {
    final copy = s.copy()..id = nextId++;
    return copy;
  }

  /// Mirrors [s] across the plane through [origin] with normal [normal].
  /// The reflection matrix is `T(origin) * R * T(-origin)` where R is
  /// the Householder reflection `I - 2 n n^T`.
  Stroke _mirrorAcrossPlane(Stroke s, Vector3 normal, Vector3 origin) {
    final n = normal.normalized();
    // Build the 3x3 reflection matrix.
    final r = Matrix3(
      1 - 2 * n.x * n.x, -2 * n.x * n.y, -2 * n.x * n.z,
      -2 * n.y * n.x, 1 - 2 * n.y * n.y, -2 * n.y * n.z,
      -2 * n.z * n.x, -2 * n.z * n.y, 1 - 2 * n.z * n.z,
    );
    final m = _composeReflect(r, origin);
    final copy = s.copy()
      ..id = nextId++
      ..transform = m * s.transform;
    return copy;
  }

  /// Mirrors [s] by a sign vector (component-wise flip) around [origin].
  /// Equivalent to a scale of (sx, sy, sz) about [origin].
  Stroke _mirrorBySign(Stroke s, Vector3 origin, Vector3 sign) {
    final toOrigin = Matrix4.identity()..setTranslation(-origin);
    final scaleMat = Matrix4.identity()
      ..scaleByVector3(Vector3(sign.x, sign.y, sign.z));
    final fromOrigin = Matrix4.identity()..setTranslation(origin);
    final m = fromOrigin * scaleMat * toOrigin;
    final copy = s.copy()
      ..id = nextId++
      ..transform = m * s.transform;
    return copy;
  }

  Matrix4 _composeReflect(Matrix3 r, Vector3 origin) {
    final toOrigin = Matrix4.identity()..setTranslation(-origin);
    final rot = Matrix4.identity()..setRotation(r);
    final fromOrigin = Matrix4.identity()..setTranslation(origin);
    return fromOrigin * rot * toOrigin;
  }
}
