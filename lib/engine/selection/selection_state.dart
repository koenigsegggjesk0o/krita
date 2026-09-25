// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// selection_state.dart — Selection state: the active set, hover, and the
// "primary" stroke.
//
// The Feather selection system is built around three concepts:
//
//   - **Active set**   — the strokes currently selected (highlighted).
//     Transform / duplicate / liquify all operate on this set.
//   - **Hover**        — the stroke the pointer is currently over (but
//     not yet clicked). Rendered with a softer highlight so the user
//     knows what a tap would select.
//   - **Primary**      — the single "leader" of the active set, used as
//     the rotation / scale pivot for transforms that need one. By
//     convention the most-recently-added member of the set.
//
// This file defines the immutable state container
// ([SelectionState]) plus a small mutable model ([SelectionModel]) that
// the rest of the engine subscribes to via [ChangeNotifier].

import 'package:flutter/foundation.dart';

import 'package:feather_krita/models/stroke.dart';

/// An immutable snapshot of the selection.
@immutable
class SelectionState {
  const SelectionState({
    this.active = const <int>[],
    this.hover,
    this.primary,
  });

  /// IDs of all selected strokes. Order is insertion order; the last
  /// element is the most recently added.
  final List<int> active;

  /// ID of the hovered stroke, or `null`.
  final int? hover;

  /// ID of the primary (pivot) stroke, or `null` when the set is empty.
  final int? primary;

  /// True if nothing is selected.
  bool get isEmpty => active.isEmpty;

  /// True if at least one stroke is selected.
  bool get isNotEmpty => active.isNotEmpty;

  /// Number of selected strokes.
  int get count => active.length;

  /// True if [id] is in the active set.
  bool isActive(int id) => active.contains(id);

  /// True if [id] is hovered.
  bool isHover(int id) => hover == id;

  /// True if [id] is the primary.
  bool isPrimary(int id) => primary == id;

  /// Returns a copy with [active] replaced. [primary] is set to the last
  /// element of the new [active] (or `null` if empty).
  SelectionState withActive(List<int> next) {
    return SelectionState(
      active: List<int>.unmodifiable(next),
      hover: hover,
      primary: next.isEmpty ? null : next.last,
    );
  }

  /// Returns a copy with [hover] replaced.
  SelectionState withHover(int? next) =>
      SelectionState(active: active, hover: next, primary: primary);

  /// Returns a copy with [primary] set to [id]. [id] must already be in
  /// the active set.
  SelectionState withPrimary(int id) {
    if (!active.contains(id)) return this;
    return SelectionState(active: active, hover: hover, primary: id);
  }

  @override
  bool operator ==(Object other) {
    if (other is! SelectionState) return false;
    if (other.active.length != active.length) return false;
    for (var i = 0; i < active.length; i++) {
      if (other.active[i] != active[i]) return false;
    }
    return other.hover == hover && other.primary == primary;
  }

  @override
  int get hashCode => Object.hash(
        Object.hashAll(active),
        hover,
        primary,
      );

  @override
  String toString() =>
      'SelectionState(active=$active, hover=$hover, primary=$primary)';
}

/// Mutable, observable selection model.
///
/// The selection system ([SelectionSystem]) mutates this via the
/// `replace` / `add` / `remove` / `clear` / `setHover` / `setPrimary`
/// helpers and listeners (the canvas, the duplicate panel, the
/// transform toolbar) rebuild on every change.
class SelectionModel extends ChangeNotifier {
  SelectionModel([SelectionState? initial])
      : _state = initial ?? const SelectionState();

  SelectionState _state;

  /// The current immutable snapshot.
  SelectionState get state => _state;

  /// IDs of all selected strokes.
  List<int> get active => _state.active;

  /// The hovered stroke ID, or `null`.
  int? get hover => _state.hover;

  /// The primary (pivot) stroke ID, or `null`.
  int? get primary => _state.primary;

  /// True if nothing is selected.
  bool get isEmpty => _state.isEmpty;

  /// Replaces the entire state. Notifies listeners.
  void replace(SelectionState next) {
    if (next == _state) return;
    _state = next;
    notifyListeners();
  }

  /// Replaces the active set.
  void setActive(List<int> next) => replace(_state.withActive(next));

  /// Adds [id] to the active set (no-op if already present).
  void add(int id) {
    if (_state.active.contains(id)) return;
    final next = [..._state.active, id];
    replace(_state.withActive(next));
  }

  /// Removes [id] from the active set.
  void remove(int id) {
    if (!_state.active.contains(id)) return;
    final next = _state.active.where((e) => e != id).toList();
    replace(_state.withActive(next));
  }

  /// Clears the active set.
  void clear() => replace(const SelectionState());

  /// Toggles [id] in the active set.
  void toggle(int id) {
    if (_state.active.contains(id)) {
      remove(id);
    } else {
      add(id);
    }
  }

  /// Sets the hovered stroke.
  void setHover(int? id) {
    if (_state.hover == id) return;
    replace(_state.withHover(id));
  }

  /// Sets the primary stroke (must be in the active set).
  void setPrimary(int id) => replace(_state.withPrimary(id));

  /// Filters the active set to only contain IDs that exist in [strokes].
  /// Used after a delete / load to drop stale IDs.
  void reconcile(Iterable<Stroke> strokes) {
    final valid = strokes.map((s) => s.id).toSet();
    final next = _state.active.where((id) => valid.contains(id)).toList();
    if (next.length != _state.active.length) {
      replace(_state.withActive(next));
    }
  }
}
