// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// liquify_engine.dart — Liquify deformation engine.
//
// Per docs/liquify_editandapply.txt, the Liquify tool's lifecycle is:
//
//   1. **Edit**  — press and drag the pen on a selected curve. Each
//      drag applies a brush (Push / Pinch / Comb) to the points within
//      the brush radius. The original curve is preserved so the user
//      can compare.
//   2. **Compare** — tap-and-hold "Compare" to view the curve before
//      liquify. Releasing returns to the edited version.
//   3. **Undo All** — revert to the pre-liquify state.
//   4. **Apply**   — commit the edits (the pre-liquify snapshot is
//      discarded and the edited curve becomes the new baseline).
//
// The engine is therefore a small state machine:
//
//        ┌────────┐  begin()  ┌─────────┐  apply()  ┌────────┐
//        │ idle   │ ─────────>│ editing │ ────────>│ idle   │
//        └────────┯            ┯─────────┘          └────────┘
//                 │ undoAll()  │
//                 └<───────────┘
//
// `begin()` snapshots every selected stroke into a pre-edit cache.
// `applyDrag()` mutates the live strokes. `compare()` returns the
// pre-edit cache (the caller swaps the renderer's source). `apply()`
// discards the cache. `undoAll()` restores the cache into the live
// strokes.

import 'package:flutter/foundation.dart';

import 'package:vector_math/vector_math_64.dart';

import 'package:feather_krita/models/stroke.dart';
import 'package:feather_krita/engine/liquify/liquify_brush.dart';
import 'package:feather_krita/engine/liquify/liquify_settings.dart';

/// Phase of the liquify lifecycle.
enum LiquifyPhase {
  /// No edit in progress; the live strokes are the baseline.
  idle,

  /// An edit is in progress; the pre-edit snapshot is held for compare
  /// / undo-all.
  editing,
}

/// The liquify engine.
///
/// Owns the pre-edit snapshot and the active brush / settings. The
/// caller (canvas pointer handler) drives it via [begin], [applyDrag],
/// [compare], [undoAll], and [apply].
class LiquifyEngine extends ChangeNotifier {
  LiquifyEngine({
    LiquifyBrushType brushType = LiquifyBrushType.push,
    LiquifySettings? settings,
  })  : _brushType = brushType,
        _settings = settings ?? const LiquifySettings();

  LiquifyBrushType _brushType;
  LiquifySettings _settings;
  LiquifyPhase _phase = LiquifyPhase.idle;

  /// Pre-edit snapshot: stroke ID → deep copy of the stroke at the
  /// moment [begin] was called. Empty in [LiquifyPhase.idle].
  final Map<int, Stroke> _snapshot = <int, Stroke>{};

  /// IDs of the strokes currently being edited.
  final Set<int> _activeIds = <int>{};

  LiquifyBrushType get brushType => _brushType;
  LiquifySettings get settings => _settings;
  LiquifyPhase get phase => _phase;
  bool get isEditing => _phase == LiquifyPhase.editing;

  void setBrushType(LiquifyBrushType t) {
    if (_brushType == t) return;
    _brushType = t;
    notifyListeners();
  }

  void setSettings(LiquifySettings s) {
    if (_settings == s) return;
    _settings = s;
    notifyListeners();
  }

  // ----- Lifecycle ----------------------------------------------------

  /// Begins a liquify edit session on [strokes]. Snapshots each stroke
  /// so [compare] and [undoAll] can restore it. Idempotent — calling
  /// [begin] again while editing is a no-op (the existing snapshot is
  /// kept).
  void begin(Iterable<Stroke> strokes) {
    if (_phase == LiquifyPhase.editing) return;
    _snapshot.clear();
    _activeIds.clear();
    for (final s in strokes) {
      _snapshot[s.id] = s.copy();
      _activeIds.add(s.id);
    }
    _phase = LiquifyPhase.editing;
    notifyListeners();
  }

  /// Applies one brush drag to the active strokes. [centre] is the
  /// brush centre in world space, [drag] is the world-space drag
  /// delta. The active brush + settings drive the per-point
  /// displacement.
  ///
  /// Returns the number of points that were moved.
  int applyDrag({
    required List<Stroke> strokes,
    required Vector3 centre,
    required Vector3 drag,
  }) {
    if (_phase != LiquifyPhase.editing) return 0;
    final brush = liquifyBrushFor(_brushType);
    var touched = 0;
    for (final stroke in strokes) {
      if (!_activeIds.contains(stroke.id)) continue;
      // Bake the transform so we operate on canonical world-space
      // positions, then mutate the points in place. The transform
      // becomes identity, which is fine — apply() will commit the
      // new positions.
      stroke.bakeTransform();
      for (final p in stroke.points) {
        final w = brush.displacement(
          point: p.position,
          centre: centre,
          drag: drag,
          settings: _settings,
        );
        if (w.length2 < 1e-10) continue;
        p.position = p.position + w;
        touched++;
      }
    }
    if (touched > 0) notifyListeners();
    return touched;
  }

  /// Returns the pre-edit snapshot for [strokeId], or `null` if there
  /// is no edit in progress or the stroke wasn't part of the edit.
  ///
  /// The renderer swaps to this version while "Compare" is held.
  Stroke? compare(int strokeId) => _snapshot[strokeId];

  /// Returns all pre-edit snapshots (for the renderer to swap in bulk
  /// during a compare hold).
  Map<int, Stroke> compareAll() => Map<int, Stroke>.unmodifiable(_snapshot);

  /// Reverts every active stroke to its pre-edit state and ends the
  /// session.
  void undoAll(List<Stroke> strokes) {
    if (_phase != LiquifyPhase.editing) return;
    final byId = {for (final s in strokes) s.id: s};
    for (final id in _activeIds) {
      final snap = _snapshot[id];
      final live = byId[id];
      if (snap == null || live == null) continue;
      _restore(live, snap);
    }
    _endSession();
  }

  /// Commits the edits: discards the snapshot and returns to idle.
  /// The live strokes' current state becomes the new baseline.
  void apply() {
    if (_phase != LiquifyPhase.editing) return;
    _endSession();
  }

  void _endSession() {
    _snapshot.clear();
    _activeIds.clear();
    _phase = LiquifyPhase.idle;
    notifyListeners();
  }

  void _restore(Stroke live, Stroke snap) {
    live
      ..brushType = snap.brushType
      ..color = snap.color
      ..thickness = snap.thickness
      ..isVisible = snap.isVisible
      ..name = snap.name
      ..mirrorOfId = snap.mirrorOfId
      ..transform = snap.transform.clone();
    live.points.clear();
    for (final p in snap.points) {
      live.points.add(p.copy());
    }
  }
}
