// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// selection_state.dart — Pure Riverpod state for the scene selection.
//
// Holds the set of currently-selected curves / guides plus a primary
// selection (the one transform operations anchor to) and a hover id (the
// object under the pointer, for highlight feedback).
//
// Selection is stored as ordered integer ids so the state stays a small
// value type (no engine singletons, no Stroke references). The lookup
// against the actual scene data is done by the consumer via
// [SceneState.curves.firstWhere].
//
// Pure value type — emits through a Riverpod [Notifier].

/// Pure selection state.
class SelectionState {
  const SelectionState({
    this.curveIds = const <int>[],
    this.guideIds = const <int>[],
    this.primaryCurveId,
    this.primaryGuideId,
    this.hoverCurveId,
    this.hoverGuideId,
  });

  /// Ordered list of selected stroke ids (back-to-front order, matches
  /// the scene's [SceneState.curves]).
  final List<int> curveIds;

  /// Ordered list of selected guide ids.
  final List<int> guideIds;

  /// The primary selected stroke — the anchor for transforms. `null`
  /// when no stroke is selected.
  final int? primaryCurveId;

  /// The primary selected guide.
  final int? primaryGuideId;

  /// The stroke currently under the pointer (hover highlight only —
  /// not part of the selection set).
  final int? hoverCurveId;

  /// The guide currently under the pointer.
  final int? hoverGuideId;

  /// Total number of selected objects (curves + guides).
  int get length => curveIds.length + guideIds.length;

  /// True when nothing is selected.
  bool get isEmpty => length == 0;

  /// True when at least one object is selected.
  bool get isNotEmpty => !isEmpty;

  /// True when more than one object is selected.
  bool get isMultiple => length > 1;

  /// True when [id] is in the selection set.
  bool isCurveSelected(int id) => curveIds.contains(id);
  bool isGuideSelected(int id) => guideIds.contains(id);

  /// Returns a new state with [id] added to the curve selection (and
  /// made primary when [makePrimary] is true).
  SelectionState addCurve(int id, {bool makePrimary = true}) {
    if (curveIds.contains(id)) {
      return makePrimary ? copyWith(primaryCurveId: id) : this;
    }
    final next = [...curveIds, id];
    return copyWith(
      curveIds: next,
      primaryCurveId: makePrimary ? id : primaryCurveId,
    );
  }

  /// Returns a new state with [id] removed from the curve selection.
  /// When the primary was [id], the new primary becomes the last
  /// remaining selection (or `null`).
  SelectionState removeCurve(int id) {
    final next = curveIds.where((c) => c != id).toList();
    final newPrimary = primaryCurveId == id
        ? (next.isEmpty ? null : next.last)
        : primaryCurveId;
    return copyWith(curveIds: next, primaryCurveId: newPrimary);
  }

  /// Returns a new state with [id] toggled in the curve selection.
  SelectionState toggleCurve(int id) =>
      curveIds.contains(id) ? removeCurve(id) : addCurve(id);

  /// Returns a new state with the curve selection replaced by [ids].
  SelectionState selectCurves(List<int> ids) => copyWith(
        curveIds: List<int>.unmodifiable(ids),
        primaryCurveId: ids.isEmpty ? null : ids.last,
      );

  /// Returns a new state with the guide selection replaced by [ids].
  SelectionState selectGuides(List<int> ids) => copyWith(
        guideIds: List<int>.unmodifiable(ids),
        primaryGuideId: ids.isEmpty ? null : ids.last,
      );

  /// Clears the selection.
  SelectionState clear() => const SelectionState();

  /// Sets the hover without touching the selection set.
  SelectionState withHover({int? curveId, int? guideId}) => copyWith(
        hoverCurveId: curveId,
        hoverGuideId: guideId,
      );

  SelectionState copyWith({
    List<int>? curveIds,
    List<int>? guideIds,
    int? primaryCurveId,
    int? primaryGuideId,
    int? hoverCurveId,
    int? hoverGuideId,
  }) =>
      SelectionState(
        curveIds: curveIds ?? this.curveIds,
        guideIds: guideIds ?? this.guideIds,
        primaryCurveId: primaryCurveId ?? this.primaryCurveId,
        primaryGuideId: primaryGuideId ?? this.primaryGuideId,
        hoverCurveId: hoverCurveId ?? this.hoverCurveId,
        hoverGuideId: hoverGuideId ?? this.hoverGuideId,
      );

  @override
  bool operator ==(Object other) =>
      identical(this, other) ||
      other is SelectionState &&
          _listEq(other.curveIds, curveIds) &&
          _listEq(other.guideIds, guideIds) &&
          other.primaryCurveId == primaryCurveId &&
          other.primaryGuideId == primaryGuideId &&
          other.hoverCurveId == hoverCurveId &&
          other.hoverGuideId == hoverGuideId;

  @override
  int get hashCode => Object.hash(
        Object.hashAll(curveIds),
        Object.hashAll(guideIds),
        primaryCurveId,
        primaryGuideId,
        hoverCurveId,
        hoverGuideId,
      );
}

bool _listEq<T>(List<T> a, List<T> b) {
  if (a.length != b.length) return false;
  for (var i = 0; i < a.length; i++) {
    if (a[i] != b[i]) return false;
  }
  return true;
}
