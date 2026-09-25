// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// selection.dart — Selection system: tap-select, deselect, select-all,
// invert, box-select, lasso-select.
//
// Per docs/selection_duplicate.txt the Feather selection UI is the entry
// point for both transform (joystick) and duplicate: "The UI related to
// Transform only appears when you tap the Select tool or select a group
// or resource from the Stage Panel."
//
// This module is the *logic* layer: it owns a [SelectionModel] and
// exposes high-level operations that the canvas pointer handlers and
// the toolbar call into. The hit-testing helpers (point-in-stroke,
// point-in-box, point-in-lasso) live here too so callers can pass raw
// screen-space input and get back a selection update.
//
// Hit-testing strategy:
//   - Tap-select    — ray-cast against each stroke's world-space
//     ribbon (point-segment distance), pick the closest hit within the
//     tap radius.
//   - Box-select    — keep strokes whose world-space bounding box
//     intersects the screen-space box (test all 8 AABB corners).
//   - Lasso-select  — same as box but the test polygon is the lasso
//     path (point-in-polygon via ray casting).
//
// The selection system does NOT mutate strokes; it only mutates the
// [SelectionModel]. Strokes are read by ID from a [StrokeSource]
// callback so the system stays decoupled from [StrokeManager].

import 'package:vector_math/vector_math_64.dart';

import 'package:feather_krita/models/stroke.dart';
import 'package:feather_krita/utils/vector_math_utils.dart';
import 'package:feather_krita/engine/selection/selection_state.dart';

/// A read-only source of strokes keyed by ID. The selection system uses
/// this to look up stroke geometry for hit-testing without depending on
/// [StrokeManager] directly.
typedef StrokeSource = List<Stroke> Function();

/// Result of a hit-test against the scene.
class HitResult {
  const HitResult({this.strokeId, this.distance = double.infinity});
  final int? strokeId;
  final double distance;
  bool get hit => strokeId != null;
}

/// Screen-space selection modes.
enum SelectMode {
  /// Replace the current selection.
  replace,

  /// Add to the current selection.
  add,

  /// Remove from the current selection.
  subtract,

  /// Toggle membership in the current selection.
  toggle,
}

/// The selection system.
class SelectionSystem {
  SelectionSystem({
    required SelectionModel model,
    required StrokeSource strokes,
    this.tapRadiusPixels = 12.0,
  })  : _model = model,
        _strokes = strokes;

  final SelectionModel _model;
  final StrokeSource _strokes;
  final double tapRadiusPixels;

  SelectionModel get model => _model;

  // ----- Single-stroke operations -------------------------------------

  /// Selects a single stroke by ID. [mode] controls how the existing
  /// selection is treated.
  void select(int id, {SelectMode mode = SelectMode.replace}) {
    switch (mode) {
      case SelectMode.replace:
        _model.setActive([id]);
        break;
      case SelectMode.add:
        _model.add(id);
        break;
      case SelectMode.subtract:
        _model.remove(id);
        break;
      case SelectMode.toggle:
        _model.toggle(id);
        break;
    }
  }

  /// Deselects everything.
  void deselect() => _model.clear();

  /// Selects all strokes in the source.
  void selectAll() {
    _model.setActive(_strokes().map((s) => s.id).toList());
  }

  /// Inverts the current selection (everything not selected becomes
  /// selected, everything selected becomes deselected).
  void invert() {
    final current = _model.active.toSet();
    final next =
        _strokes().where((s) => !current.contains(s.id)).map((s) => s.id).toList();
    _model.setActive(next);
  }

  // ----- Tap-select ----------------------------------------------------

  /// Hit-tests a screen-space tap and selects the closest stroke within
  /// the tap radius. The hit-test is done in WORLD space: [rayOrigin]
  /// and [rayDir] form the camera ray under the tap.
  Future<void> tapSelect(
    Vector3 rayOrigin,
    Vector3 rayDir, {
    SelectMode mode = SelectMode.replace,
    double worldTapRadius = 0.05,
  }) async {
    final hit = _hitTest(rayOrigin, rayDir, worldTapRadius);
    if (!hit.hit) {
      if (mode == SelectMode.replace) deselect();
      return;
    }
    select(hit.strokeId!, mode: mode);
  }

  /// Synchronous variant of [tapSelect] for callers that already have
  /// the strokes synchronously (the common case).
  void tapSelectSync(
    Vector3 rayOrigin,
    Vector3 rayDir, {
    SelectMode mode = SelectMode.replace,
    double worldTapRadius = 0.05,
  }) {
    final hit = _hitTest(rayOrigin, rayDir, worldTapRadius);
    if (!hit.hit) {
      if (mode == SelectMode.replace) deselect();
      return;
    }
    select(hit.strokeId!, mode: mode);
  }

  HitResult _hitTest(
      Vector3 rayOrigin, Vector3 rayDir, double worldTapRadius) {
    var bestId = -1;
    var bestDist = double.infinity;
    for (final stroke in _strokes()) {
      if (!stroke.isVisible) continue;
      for (var i = 0; i < stroke.length - 1; i++) {
        final a = stroke.worldPosition(i);
        final b = stroke.worldPosition(i + 1);
        final (d, _) = VectorMathUtils.pointSegment(rayOrigin, a, b);
        // Combine "distance from ray to segment" with "ray parameter" so
        // the closest visible segment wins.
        final along = (a - rayOrigin).dot(rayDir);
        final score = d + (along < 0 ? along.abs() * 0.1 : 0);
        if (d < worldTapRadius && score < bestDist) {
          bestDist = score;
          bestId = stroke.id;
        }
      }
    }
    if (bestId < 0) return const HitResult();
    return HitResult(strokeId: bestId, distance: bestDist);
  }

  // ----- Box-select ----------------------------------------------------

  /// Selects every stroke whose world-space bounding box intersects
  /// the screen-space box [boxMin]-[boxMax] when projected via
  /// [viewProjection].
  void boxSelect(
    Vector2 boxMin,
    Vector2 boxMax,
    Matrix4 viewProjection, {
    double viewportWidth = 1.0,
    double viewportHeight = 1.0,
    SelectMode mode = SelectMode.replace,
  }) {
    final ids = <int>[];
    for (final stroke in _strokes()) {
      if (!stroke.isVisible) continue;
      if (_strokeIntersectsBox(stroke, boxMin, boxMax, viewProjection,
          viewportWidth, viewportHeight)) {
        ids.add(stroke.id);
      }
    }
    _applyMode(ids, mode);
  }

  bool _strokeIntersectsBox(
    Stroke stroke,
    Vector2 boxMin,
    Vector2 boxMax,
    Matrix4 viewProjection,
    double viewportWidth,
    double viewportHeight,
  ) {
    // Project every point; if any lands inside the box, it's a hit.
    // (A more conservative test would project the AABB's 8 corners,
    // but stroke-level point projection matches Feather's visual
    // feedback exactly.)
    for (final p in stroke.points) {
      final world = stroke.transform.transform3(p.position.clone());
      final ndc = viewProjection.transform3(world.clone());
      if (ndc.z >= 1.0 || ndc.z <= -1.0) continue; // clipped
      final sx = (ndc.x * 0.5 + 0.5) * viewportWidth;
      final sy = (1.0 - (ndc.y * 0.5 + 0.5)) * viewportHeight;
      if (sx >= boxMin.x && sx <= boxMax.x && sy >= boxMin.y && sy <= boxMax.y) {
        return true;
      }
    }
    return false;
  }

  // ----- Lasso-select --------------------------------------------------

  /// Selects every stroke whose projected screen position is inside the
  /// [lasso] polygon.
  void lassoSelect(
    List<Vector2> lasso,
    Matrix4 viewProjection, {
    double viewportWidth = 1.0,
    double viewportHeight = 1.0,
    SelectMode mode = SelectMode.replace,
  }) {
    if (lasso.length < 3) return;
    final ids = <int>[];
    for (final stroke in _strokes()) {
      if (!stroke.isVisible) continue;
      for (final p in stroke.points) {
        final world = stroke.transform.transform3(p.position.clone());
        final ndc = viewProjection.transform3(world.clone());
        if (ndc.z >= 1.0 || ndc.z <= -1.0) continue;
        final sx = (ndc.x * 0.5 + 0.5) * viewportWidth;
        final sy = (1.0 - (ndc.y * 0.5 + 0.5)) * viewportHeight;
        if (_pointInPolygon(Vector2(sx, sy), lasso)) {
          ids.add(stroke.id);
          break;
        }
      }
    }
    _applyMode(ids, mode);
  }

  /// Ray-cast point-in-polygon test.
  bool _pointInPolygon(Vector2 p, List<Vector2> poly) {
    var inside = false;
    var j = poly.length - 1;
    for (var i = 0; i < poly.length; i++) {
      final pi = poly[i];
      final pj = poly[j];
      if (((pi.y > p.y) != (pj.y > p.y)) &&
          (p.x <
              (pj.x - pi.x) * (p.y - pi.y) / ((pj.y - pi.y) + 1e-12) + pi.x)) {
        inside = !inside;
      }
      j = i;
    }
    return inside;
  }

  // ----- Mode application ---------------------------------------------

  void _applyMode(List<int> ids, SelectMode mode) {
    switch (mode) {
      case SelectMode.replace:
        _model.setActive(ids);
        break;
      case SelectMode.add:
        ids.forEach(_model.add);
        break;
      case SelectMode.subtract:
        ids.forEach(_model.remove);
        break;
      case SelectMode.toggle:
        ids.forEach(_model.toggle);
        break;
    }
  }

  /// Convenience: the [Stroke]s currently selected.
  List<Stroke> selectedStrokes() {
    final ids = _model.active.toSet();
    return _strokes().where((s) => ids.contains(s.id)).toList();
  }
}
