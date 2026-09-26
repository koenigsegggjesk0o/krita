// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// eraser_engine.dart — erase + vacuum for 3D strokes.
//
// Docs (brushes_drawanderase.txt):
//   - Erase: "Tap or drag with your pen to erase parts of the curve."
//     The eraser "removes points from the center of the curve" — i.e.
//     stroke SAMPLE POINTS within the eraser radius are deleted, and
//     the remaining samples re-chain into one or more shorter strokes.
//   - Vacuum: "Tap or drag with your pen to erase all curves it
//     touches." Any stroke with a sample inside the vacuum radius is
//     removed ENTIRELY (not split).
//   - Isolate by 3D Guide: "Cover the curves you don't want to erase
//     with a 3D Guide. The eraser will not erase curves within the
//     guide." A stroke fully inside a guide's bounds is protected.
//
// The engine operates in WORLD space (strokes carry a local→world
// [Matrix4] transform; [Stroke.worldPosition] applies it). It returns
// [EraseResult]s so the editor can push the change onto the undo stack.

import 'package:vector_math/vector_math_64.dart';

import 'package:feather_krita/models/stroke.dart';

/// Eraser behavior.
enum EraserMode {
  /// Delete sample points within the radius; re-chain survivors.
  point,

  /// Delete an entire stroke if any sample touches the radius.
  vacuum,
}

/// The outcome of an erase pass on one stroke.
class EraseResult {
  const EraseResult({this.strokes = const <Stroke>[], this.modified = false});

  /// The strokes that survive the erase (0, 1, or many — point-erase can
  /// split one stroke into several). Empty = stroke fully erased.
  final List<Stroke> strokes;

  /// Whether the original stroke was changed at all.
  final bool modified;
}

/// Outcome of an in-place erase pass over a list of strokes (see
/// [EraserEngine.erasePointsInPlace]). [mutated] holds the surviving
/// strokes that lost sample points (the caller should re-sync their
/// Stroke3D / curve-cache); [dropped] holds the strokes that fell
/// below [EraserEngine.minSurvivingPoints] and should be removed
/// entirely from the document (with their Stroke3D / material records
/// cleaned up).
class InPlaceEraseResult {
  const InPlaceEraseResult({
    this.mutated = const <Stroke>[],
    this.dropped = const <Stroke>[],
  });

  /// Surviving strokes that lost at least one sample point but still
  /// have >= [EraserEngine.minSurvivingPoints] samples left.
  final List<Stroke> mutated;

  /// Strokes that dropped below [EraserEngine.minSurvivingPoints] after
  /// the erase — the caller removes them from the document.
  final List<Stroke> dropped;

  /// True when at least one stroke was mutated or dropped.
  bool get anyChange => mutated.isNotEmpty || dropped.isNotEmpty;
}

/// Erase + vacuum engine.
class EraserEngine {
  EraserEngine({this.minSurvivingPoints = 2});

  /// A re-chained stroke must have at least this many samples to
  /// survive (a lone sample carries no direction and renders nothing).
  final int minSurvivingPoints;

  /// Erases [stroke] at [worldPos] with [radius] (world units).
  PointEraseResult erasePoint(
          Stroke stroke, Vector3 worldPos, double radius) =>
      _erasePoint(stroke, worldPos, radius * radius);

  /// Vacuum-erases [stroke]: if any sample is within [radius] of
  /// [worldPos], the whole stroke is removed (returns empty result).
  EraseResult eraseVacuum(Stroke stroke, Vector3 worldPos, double radius) {
    final r2 = radius * radius;
    for (var i = 0; i < stroke.length; i++) {
      if (stroke.worldPosition(i).distanceToSquared(worldPos) <= r2) {
        return const EraseResult(strokes: [], modified: true);
      }
    }
    return EraseResult(strokes: [stroke], modified: false);
  }

  /// Dispatch on [mode]. Convenience wrapper.
  EraseResult erase(Stroke stroke, Vector3 worldPos, double radius,
      [EraserMode mode = EraserMode.point]) {
    if (mode == EraserMode.vacuum) {
      return eraseVacuum(stroke, worldPos, radius);
    }
    return erasePoint(stroke, worldPos, radius);
  }

  /// Erases sample points within [radius] of [worldPos] from each stroke
  /// in [strokes] IN PLACE — i.e. by mutating each stroke's `points`
  /// list directly (no re-chaining, no stroke splitting). This is the
  /// behaviour the Feather editor host expects: strokes keep their
  /// identity so the host's Stroke3D / material / selection records
  /// stay attached, and a stroke that drops below
  /// [minSurvivingPoints] is reported back in [InPlaceEraseResult.dropped]
  /// so the host can remove it cleanly.
  ///
  /// Strokes for which [skip] returns `true` are passed through
  /// unchanged (used by the host to enforce the 3D Guide isolation
  /// contract — strokes fully inside a guide's bounds are protected).
  ///
  /// Returns the list of strokes that lost points
  /// ([InPlaceEraseResult.mutated], surviving) plus the list that
  /// dropped below [minSurvivingPoints] ([InPlaceEraseResult.dropped],
  /// to be removed). The caller is responsible for syncing the
  /// Stroke3D of each mutated stroke and removing each dropped stroke
  /// from the document + its auxiliary records.
  InPlaceEraseResult erasePointsInPlace(
    List<Stroke> strokes,
    Vector3 worldPos,
    double radius, {
    bool Function(Stroke stroke)? skip,
  }) {
    final r2 = radius * radius;
    final mutated = <Stroke>[];
    final dropped = <Stroke>[];
    for (final stroke in strokes) {
      if (skip != null && skip(stroke)) continue;
      final before = stroke.points.length;
      stroke.points.removeWhere((p) {
        final wp = stroke.transform.transform3(p.position.clone());
        return (wp - worldPos).length2 <= r2;
      });
      if (stroke.points.length != before) {
        if (stroke.points.length < minSurvivingPoints) {
          dropped.add(stroke);
        } else {
          mutated.add(stroke);
        }
      }
    }
    return InPlaceEraseResult(mutated: mutated, dropped: dropped);
  }

  /// Isolate-by-guide: returns true when [stroke]'s world bounds lie
  /// ENTIRELY inside [guideBounds] (the docs' protection contract — a
  /// guide "covers" the curves it should shield). Such a stroke is
  /// skipped by [eraseWithGuideProtection].
  bool isProtectedByGuide(Stroke stroke, Aabb3 guideBounds) {
    if (stroke.isEmpty) return false;
    final bounds = stroke.worldBounds();
    return guideBounds.containsAabb3(bounds);
  }

  /// Erases with guide protection: if [stroke] is fully inside
  /// [guideBounds], the stroke is returned unchanged.
  EraseResult eraseWithGuideProtection(
    Stroke stroke,
    Vector3 worldPos,
    double radius,
    EraserMode mode,
    Aabb3? guideBounds,
  ) {
    if (guideBounds != null && isProtectedByGuide(stroke, guideBounds)) {
      return EraseResult(strokes: [stroke], modified: false);
    }
    return erase(stroke, worldPos, radius, mode);
  }

  PointEraseResult _erasePoint(
      Stroke stroke, Vector3 worldPos, double r2) {
    // Mark each sample erased/kept, then re-chain the kept runs.
    final runs = <List<StrokePoint>>[];
    var current = <StrokePoint>[];
    var anyErased = false;
    for (var i = 0; i < stroke.length; i++) {
      final wp = stroke.worldPosition(i);
      final erased = wp.distanceToSquared(worldPos) <= r2;
      if (erased) {
        anyErased = true;
        if (current.length >= minSurvivingPoints) runs.add(current);
        current = <StrokePoint>[];
      } else {
        current.add(stroke.points[i]);
      }
    }
    if (current.length >= minSurvivingPoints) runs.add(current);
    if (!anyErased) {
      return PointEraseResult(strokes: [stroke], modified: false);
    }
    final out = <Stroke>[];
    for (final run in runs) {
      out.add(Stroke(
        brushType: stroke.brushType,
        color: stroke.color,
        thickness: stroke.thickness,
        points: run,
        transform: stroke.transform.clone(),
        mirrorOfId: stroke.mirrorOfId,
      ));
    }
    return PointEraseResult(strokes: out, modified: true);
  }
}

/// Point-erase result (mirrors [EraseResult] — kept as its own type so
/// the editor can distinguish point vs vacuum outcomes if needed).
typedef PointEraseResult = EraseResult;
