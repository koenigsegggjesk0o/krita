// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// scene_layer.dart — an artist-facing layer / group.
//
// The Stage Panel's Group Tab (see `stagepanel.txt`) manages GROUPS —
// named collections of curves the artist can show / hide / lock /
// restyle together. A [SceneLayer] is that concept: it owns a set of
// curve ids (references into the [Scene] graph) plus per-group flags.
//
// Layers are NOT the scene graph themselves — they are an artist-facing
// view over it. A curve belongs to exactly one layer; layers can be
// reordered and nested one level deep (parent group) for organization.
//
// Depends on: lib/core/math/

import 'dart:ui' show Color;

/// One group / layer in the Stage Panel's Group Tab.
class SceneLayer {
  SceneLayer({
    required this.id,
    required this.name,
    List<String>? curveIds,
    this.visible = true,
    this.locked = false,
    this.opacity = 1.0,
    this.color = const Color(0xffffffff),
    this.parentId,
    this.expanded = false,
  }) : _curveIds = curveIds ?? [];

  /// Stable unique id.
  final String id;

  /// Display name (editable in the Group Tab).
  String name;

  /// Curve (Stroke) ids owned by this group, in z-order (back → front).
  final List<String> _curveIds;
  List<String> get curveIds => List.unmodifiable(_curveIds);

  bool visible;
  bool locked;

  /// 0–1 group opacity multiplier.
  double opacity;

  /// Tint used for the layer's swatch in the Group Tab.
  Color color;

  /// Optional parent group id (one-level nesting).
  String? parentId;

  /// Whether nested children are expanded in the Group Tab.
  bool expanded;

  // ----- Curve membership ----------------------------------------------

  /// Append a curve id at the top of the stack (front-most).
  void addCurve(String curveId, {bool toFront = true}) {
    if (_curveIds.contains(curveId)) return;
    if (toFront) {
      _curveIds.add(curveId);
    } else {
      _curveIds.insert(0, curveId);
    }
  }

  /// Remove a curve id; returns `true` if it was present.
  bool removeCurve(String curveId) => _curveIds.remove(curveId);

  /// Move [curveId] to [newIndex] within the stack.
  void reorderCurve(String curveId, int newIndex) {
    final i = _curveIds.indexOf(curveId);
    if (i < 0) return;
    _curveIds.removeAt(i);
    final clamped = newIndex.clamp(0, _curveIds.length);
    _curveIds.insert(clamped, curveId);
  }

  /// Reorder this layer's curves so [curveId] sits at the front.
  void bringToFront(String curveId) {
    final i = _curveIds.indexOf(curveId);
    if (i >= 0 && i != _curveIds.length - 1) {
      _curveIds.removeAt(i);
      _curveIds.add(curveId);
    }
  }

  /// Send [curveId] to the back.
  void sendToBack(String curveId) {
    final i = _curveIds.indexOf(curveId);
    if (i > 0) {
      _curveIds.removeAt(i);
      _curveIds.insert(0, curveId);
    }
  }

  // ----- Serialization --------------------------------------------------

  Map<String, dynamic> toJson() => {
        'id': id,
        'name': name,
        'curveIds': List<String>.from(_curveIds),
        'visible': visible,
        'locked': locked,
        'opacity': opacity,
        'color': color.toARGB32(),
        'parentId': parentId,
        'expanded': expanded,
      };

  factory SceneLayer.fromJson(Map<String, dynamic> json) {
    return SceneLayer(
      id: json['id'] as String,
      name: json['name'] as String? ?? 'Group',
      curveIds: (json['curveIds'] as List? ?? [])
          .map((e) => e as String)
          .toList(growable: true),
      visible: json['visible'] as bool? ?? true,
      locked: json['locked'] as bool? ?? false,
      opacity: (json['opacity'] as num?)?.toDouble() ?? 1.0,
      color: Color((json['color'] as int?) ?? 0xffffffff),
      parentId: json['parentId'] as String?,
      expanded: json['expanded'] as bool? ?? false,
    );
  }

  SceneLayer copy() => SceneLayer.fromJson(toJson());
}

/// The ordered stack of layers shown in the Group Tab.
class LayerStack {
  LayerStack({List<SceneLayer>? layers}) : _layers = layers ?? [];

  final List<SceneLayer> _layers;
  List<SceneLayer> get layers => List.unmodifiable(_layers);

  /// The id of the currently-active group (new curves are added here).
  String? activeId;

  SceneLayer? get active => activeId == null
      ? null
      : _layers.where((l) => l.id == activeId).firstOrNull;

  void add(SceneLayer layer, {bool makeActive = true}) {
    _layers.add(layer);
    if (makeActive) activeId = layer.id;
  }

  bool remove(String id) {
    final before = _layers.length;
    _layers.removeWhere((l) => l.id == id);
    final removed = _layers.length < before;
    if (removed && activeId == id) {
      activeId = _layers.isEmpty ? null : _layers.last.id;
    }
    return removed;
  }

  SceneLayer? findById(String id) =>
      _layers.where((l) => l.id == id).firstOrNull;

  /// Reorder a layer to [newIndex].
  void reorder(String id, int newIndex) {
    final i = _layers.indexWhere((l) => l.id == id);
    if (i < 0) return;
    final layer = _layers.removeAt(i);
    final clamped = newIndex.clamp(0, _layers.length);
    _layers.insert(clamped, layer);
  }

  /// Create a new empty group and make it active — the "Add New Group"
  /// action from the Squeeze Menu.
  SceneLayer addNewGroup(String name, {String Function()? idGen}) {
    final layer = SceneLayer(id: idGen?.call() ?? 'grp_${_layers.length}', name: name);
    add(layer);
    return layer;
  }

  Map<String, dynamic> toJson() => {
        'layers': _layers.map((l) => l.toJson()).toList(),
        'activeId': activeId,
      };

  factory LayerStack.fromJson(Map<String, dynamic> json) {
    final stack = LayerStack(
      layers: (json['layers'] as List? ?? [])
          .map((e) => SceneLayer.fromJson(e as Map<String, dynamic>))
          .toList(growable: true),
    );
    stack.activeId = json['activeId'] as String?;
    return stack;
  }
}
