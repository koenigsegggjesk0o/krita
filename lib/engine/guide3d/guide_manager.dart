// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// guide_manager.dart — Owner of every [Guide3D] in a scene.
//
// Feather keeps many guides alive at once: the artist draws one, bends it,
// lofts another, drops a primitive — all of them sit in the scene and the
// Resources tab. The [Guide3DManager] is the single source of truth for
// that collection. It owns:
//
//   - the ordered list of active guides (rendered + pickable);
//   - the currently selected guide (the target of Bend / opacity / lock);
//   - the saved-guide library (the Resources tab's persistent entries);
//   - a monotonic id counter so every guide has a stable handle.
//
// The manager is intentionally framework-agnostic — no Riverpod, no
// ChangeNotifier. Callers (the editor state, the resources panel) wrap it
// in whatever reactive container they prefer and rebuild when the manager
// mutates. Mutation methods return the affected guide id so the wrapper
// can dispatch a targeted notification.

import 'package:feather_krita/core/math/math.dart';

import 'guide3d.dart';
import 'guide3d_type.dart';
import 'bent_guide.dart';
import 'drawn_guide.dart';
import 'lofted_guide.dart';
import 'primitive_guide.dart';

/// Stable handle for a guide in the manager.
typedef GuideId = int;

/// A saved-guide entry in the Resources tab. Stores the serialised guide
/// plus a display name and a thumbnail color (for the panel swatch).
class SavedGuide {
  SavedGuide({
    required this.id,
    required this.name,
    required this.json,
    this.thumbnailColor = 0xFF6FA8FF,
  });

  final int id;
  String name;
  Map<String, dynamic> json;
  int thumbnailColor;

  Map<String, dynamic> toJson() => {
        'id': id,
        'name': name,
        'thumbnailColor': thumbnailColor,
        'guide': json,
      };

  factory SavedGuide.fromJson(Map<String, dynamic> json) => SavedGuide(
        id: (json['id'] as num).toInt(),
        name: json['name'] as String? ?? 'Guide',
        json:
            (json['guide'] as Map?)?.cast<String, dynamic>() ?? const {},
        thumbnailColor: (json['thumbnailColor'] as num?)?.toInt() ?? 0xFF6FA8FF,
      );
}

/// Owns the live guides + the saved-guide library for a scene.
class Guide3DManager {
  Guide3DManager();

  final Map<GuideId, Guide3D> _guides = <GuideId, Guide3D>{};
  final List<GuideId> _order = <GuideId>[];
  final Map<int, SavedGuide> _saved = <int, SavedGuide>{};
  GuideId? _selected;
  int _nextId = 1;
  int _nextSavedId = 1;

  // ----- Active guides --------------------------------------------------

  /// Ordered list of active guide ids (rendering order).
  List<GuideId> get ids => List<GuideId>.unmodifiable(_order);

  /// All active guides in rendering order.
  List<Guide3D> get guides =>
      [for (final id in _order) _guides[id]!];

  /// Number of active guides.
  int get length => _guides.length;

  /// Currently selected guide id (target of Bend / opacity / lock), or
  /// `null` when nothing is selected.
  GuideId? get selectedId => _selected;

  /// The currently selected guide, or `null`.
  Guide3D? get selected => _selected == null ? null : _guides[_selected];

  /// Looks up a guide by id.
  Guide3D? operator [](GuideId id) => _guides[id];

  /// Adds [guide] to the scene and selects it. Returns the new id.
  GuideId add(Guide3D guide) {
    final id = _nextId++;
    _guides[id] = guide;
    _order.add(id);
    _selected = id;
    return id;
  }

  /// Removes a guide by id. Clears the selection if it pointed here.
  /// Returns true when a guide was removed.
  bool remove(GuideId id) {
    if (!_guides.containsKey(id)) return false;
    _guides.remove(id);
    _order.remove(id);
    if (_selected == id) {
      _selected = _order.isEmpty ? null : _order.last;
    }
    return true;
  }

  /// Selects a guide by id. Returns true when the id was active.
  bool select(GuideId id) {
    if (!_guides.containsKey(id)) return false;
    _selected = id;
    return true;
  }

  /// Clears the selection (no guide is active). The guides themselves
  /// stay in the scene.
  void clearSelection() => _selected = null;

  /// Moves a guide earlier in the rendering order (drawn underneath).
  void sendBackward(GuideId id) {
    final i = _order.indexOf(id);
    if (i <= 0) return;
    final tmp = _order[i - 1];
    _order[i - 1] = id;
    _order[i] = tmp;
  }

  /// Moves a guide later in the rendering order (drawn on top).
  void bringForward(GuideId id) {
    final i = _order.indexOf(id);
    if (i < 0 || i >= _order.length - 1) return;
    final tmp = _order[i + 1];
    _order[i + 1] = id;
    _order[i] = tmp;
  }

  /// Sets the opacity of the selected guide (or [id] when supplied).
  /// Clamped to [0, 1]. No-op when the guide is locked.
  void setOpacity(double opacity, [GuideId? id]) {
    final target = _guides[id ?? _selected];
    if (target == null || target.locked) return;
    target.opacity = opacity.clamp(0.0, 1.0);
  }

  /// Toggles the lock on the selected guide (or [id]).
  void toggleLock([GuideId? id]) {
    final target = _guides[id ?? _selected];
    if (target == null) return;
    target.locked = !target.locked;
  }

  /// Toggles visibility on the selected guide (or [id]).
  void toggleVisible([GuideId? id]) {
    final target = _guides[id ?? _selected];
    if (target == null) return;
    target.visible = !target.visible;
  }

  /// Sets the world-space transform of the selected guide (joystick move
  /// for primitives, transform for any guide). Marks the inverse dirty.
  void setTransform(Matrix4 transform, [GuideId? id]) {
    final target = _guides[id ?? _selected];
    if (target == null) return;
    target
      ..transform = transform
      ..markTransformDirty();
  }

  /// Removes every active guide.
  void clear() {
    _guides.clear();
    _order.clear();
    _selected = null;
  }

  // ----- Saved-guide library (Resources tab) ---------------------------

  /// All saved guides in insertion order.
  List<SavedGuide> get saved => [for (final s in _saved.values) s];

  /// Saves [guide] to the library with a display [name]. Returns the
  /// saved-entry id (distinct from the live [GuideId]).
  int saveGuide(Guide3D guide, String name) {
    final id = _nextSavedId++;
    _saved[id] = SavedGuide(
      id: id,
      name: name,
      json: guide.toJson(),
    );
    return id;
  }

  /// Loads a saved guide back into the active scene. Returns the new
  /// active [GuideId], or `null` when the saved entry was not found.
  GuideId? loadSaved(int savedId) {
    final entry = _saved[savedId];
    if (entry == null) return null;
    final guide = _fromJson(entry.json);
    if (guide == null) return null;
    return add(guide);
  }

  /// Renames a saved guide.
  bool renameSaved(int savedId, String name) {
    final entry = _saved[savedId];
    if (entry == null) return false;
    entry.name = name;
    return true;
  }

  /// Deletes a saved guide from the library.
  bool deleteSaved(int savedId) => _saved.remove(savedId) != null;

  // ----- Serialization --------------------------------------------------

  Map<String, dynamic> toJson() => {
        'nextId': _nextId,
        'nextSavedId': _nextSavedId,
        'guides': [for (final id in _order) _guides[id]!.toJson()..['id'] = id],
        'selected': _selected,
        'saved': [for (final s in _saved.values) s.toJson()],
      };

  /// Restores the manager from JSON. Guide meshes are rebuilt via the
  /// per-mode builders based on the stored `type` field.
  void loadFromJson(Map<String, dynamic> json) {
    clear();
    _saved.clear();
    _nextId = (json['nextId'] as num?)?.toInt() ?? 1;
    _nextSavedId = (json['nextSavedId'] as num?)?.toInt() ?? 1;
    final guides = json['guides'] as List? ?? const [];
    for (final raw in guides) {
      final map = raw as Map<String, dynamic>;
      final id = (map['id'] as num).toInt();
      final guide = _fromJson(map);
      if (guide == null) continue;
      _guides[id] = guide;
      _order.add(id);
    }
    _selected = (json['selected'] as num?)?.toInt();
    final saved = json['saved'] as List? ?? const [];
    for (final raw in saved) {
      final entry = SavedGuide.fromJson(raw as Map<String, dynamic>);
      _saved[entry.id] = entry;
    }
  }

  Guide3D? _fromJson(Map<String, dynamic> json) {
    final type = guide3DTypeFromName(json['type'] as String?);
    switch (type) {
      case Guide3DType.drawn:
        return const DrawnGuideBuilder().fromJson(json);
      case Guide3DType.lofted:
        return const LoftedGuideBuilder().fromJson(json);
      case Guide3DType.primitive:
        return const PrimitiveGuideBuilder().fromJson(json);
      case Guide3DType.bent:
        return const BentGuideBuilder().fromJson(json);
    }
  }
}
