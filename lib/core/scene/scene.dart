// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// scene.dart — the Feather scene root.
//
// Ties together the four pillars of a Feather document:
//
//   1. The SCENE GRAPH ([root]) — a real node hierarchy (transforms,
//      children, materials) for spatial composition.
//   2. The LAYER STACK ([layers]) — the artist-facing Group Tab view
//      over the curves (named, lockable, reorderable groups).
//   3. The GUIDES / RESOURCES registries — 3D guide surfaces and
//      imported reference assets keyed by id.
//   4. The ENVIRONMENT ([environment]) — background, lighting, world.
//
// Plus the document-level selection set and the active-group pointer.
//
// The scene is serializable (save / load, undo / redo) and exposes
// high-level mutators that keep the graph and the layer stack in sync:
// adding a curve creates its graph node AND registers it with a layer.
//
// Depends on: lib/core/math/, lib/core/scene/*

import 'package:feather_krita/core/math/math.dart';
import 'package:feather_krita/core/scene/environment.dart';
import 'package:feather_krita/core/scene/scene_layer.dart';
import 'package:feather_krita/core/scene/scene_node.dart';

/// A 3D guide surface registered with the scene.
class GuideEntry {
  GuideEntry({
    required this.id,
    required this.kind,
    this.visible = true,
    this.data,
  });

  final String id;
  final String kind;
  bool visible;
  Map<String, dynamic>? data;
}

/// An imported reference resource (image, mesh, material).
class ResourceEntry {
  ResourceEntry({
    required this.id,
    required this.kind,
    required this.name,
    this.assetPath,
    this.transform,
    this.visible = true,
  });

  final String id;
  final String kind;
  String name;
  String? assetPath;
  Matrix4? transform;
  bool visible;
}

/// The scene root.
class Scene {
  Scene({
    SceneNode? root,
    LayerStack? layers,
    Environment? environment,
  })  : _root = root ?? SceneNode(id: 'root', name: 'Scene'),
        layers = layers ?? LayerStack(),
        environment = environment ?? Environment();

  final SceneNode _root;
  SceneNode get root => _root;

  final LayerStack layers;
  Environment environment;

  final Map<String, GuideEntry> _guides = {};
  Map<String, GuideEntry> get guides => Map.unmodifiable(_guides);

  final Map<String, ResourceEntry> _resources = {};
  Map<String, ResourceEntry> get resources => Map.unmodifiable(_resources);

  /// Selected curve ids (the selection tool's set).
  final Set<String> _selection = {};
  Set<String> get selection => Set.unmodifiable(_selection);

  /// All curve node ids in the graph (depth-first, visible only).
  Iterable<String> get allCurveIds =>
      _root.curveLeaves().map((n) => n.id);

  // ----- Curve management ----------------------------------------------

  /// Add a curve to the scene graph AND register it with [layerId] (or
  /// the active layer when `null`). The new node is parented under the
  /// root group by default; pass [parentId] to nest it elsewhere.
  SceneNode addCurve({
    required String curveId,
    String? refId,
    String? layerId,
    String? parentId,
    Matrix4? transform,
    SceneMaterial? material,
  }) {
    final node = SceneNode(
      id: curveId,
      kind: SceneNodeKind.curve,
      refId: refId ?? curveId,
      transform: transform,
      material: material,
    );
    final parent = (parentId == null ? _root : _root.findById(parentId)) ?? _root;
    parent.addChild(node);

    final layer = layerId == null
        ? layers.active
        : layers.findById(layerId);
    if (layer != null) {
      layer.addCurve(curveId);
    } else if (layers.layers.isEmpty) {
      // Auto-create a default group so the curve always has a home.
      final g = layers.addNewGroup('Group 1');
      g.addCurve(curveId);
    }
    return node;
  }

  /// Remove a curve from the graph, its layer, and the selection.
  bool removeCurve(String curveId) {
    final node = _root.findById(curveId);
    if (node == null) return false;
    node.detach();
    for (final l in layers.layers) {
      l.removeCurve(curveId);
    }
    _selection.remove(curveId);
    return true;
  }

  // ----- Selection ------------------------------------------------------

  void select(String curveId) => _selection.add(curveId);
  void deselect(String curveId) => _selection.remove(curveId);
  void toggleSelect(String curveId) {
    if (_selection.contains(curveId)) {
      _selection.remove(curveId);
    } else {
      _selection.add(curveId);
    }
  }

  void clearSelection() => _selection.clear();

  /// "Select all currently visible groups" (Squeeze Menu action).
  void selectAllVisible() {
    _selection.clear();
    for (final n in _root.curveLeaves(visibleOnly: true)) {
      _selection.add(n.id);
    }
  }

  // ----- Guides ---------------------------------------------------------

  GuideEntry addGuide(GuideEntry guide) {
    _guides[guide.id] = guide;
    return guide;
  }

  bool removeGuide(String id) => _guides.remove(id) != null;

  // ----- Resources ------------------------------------------------------

  ResourceEntry addResource(ResourceEntry resource) {
    _resources[resource.id] = resource;
    return resource;
  }

  bool removeResource(String id) => _resources.remove(id) != null;

  // ----- Scene-graph queries -------------------------------------------

  /// Compute the axis-aligned bounding box of all visible curves' world
  /// transforms (cheap; uses node transforms only, not geometry).
  Aabb3? bounds() {
    Aabb3? box;
    for (final n in _root.curveLeaves()) {
      if (!n.effectivelyVisible()) continue;
      final p = n.worldTransform().getTranslation();
      if (box == null) {
        box = Aabb3.minMax(p, p);
      } else {
        box.hullPoint(p);
      }
    }
    return box;
  }

  /// Find the layer that owns [curveId], or `null`.
  SceneLayer? layerFor(String curveId) {
    for (final l in layers.layers) {
      if (l.curveIds.contains(curveId)) return l;
    }
    return null;
  }

  // ----- Serialization --------------------------------------------------

  Map<String, dynamic> toJson() => {
        'root': _root.toJson(),
        'layers': layers.toJson(),
        'environment': environment.toJson(),
        'guides': _guides.map((k, v) => MapEntry(k, {
              'id': v.id,
              'kind': v.kind,
              'visible': v.visible,
              'data': v.data,
            })),
        'resources': _resources.map((k, v) => MapEntry(k, {
              'id': v.id,
              'kind': v.kind,
              'name': v.name,
              'assetPath': v.assetPath,
              'transform': v.transform == null
                  ? null
                  : _matrixToList(v.transform!),
              'visible': v.visible,
            })),
        'selection': _selection.toList(),
      };

  factory Scene.fromJson(Map<String, dynamic> json) {
    final scene = Scene(
      root: SceneNode.fromJson(json['root'] as Map<String, dynamic>),
      layers: LayerStack.fromJson(json['layers'] as Map<String, dynamic>),
      environment:
          Environment.fromJson(json['environment'] as Map<String, dynamic>),
    );
    final guides = json['guides'] as Map<String, dynamic>? ?? {};
    guides.forEach((k, v) {
      final m = v as Map<String, dynamic>;
      scene._guides[k] = GuideEntry(
        id: m['id'] as String,
        kind: m['kind'] as String,
        visible: m['visible'] as bool? ?? true,
        data: m['data'] as Map<String, dynamic>?,
      );
    });
    final resources = json['resources'] as Map<String, dynamic>? ?? {};
    resources.forEach((k, v) {
      final m = v as Map<String, dynamic>;
      scene._resources[k] = ResourceEntry(
        id: m['id'] as String,
        kind: m['kind'] as String,
        name: m['name'] as String? ?? 'Resource',
        assetPath: m['assetPath'] as String?,
        transform: _listToMatrix(m['transform']),
        visible: m['visible'] as bool? ?? true,
      );
    });
    scene._selection.addAll(
      ((json['selection'] as List?) ?? []).cast<String>(),
    );
    return scene;
  }

  Scene copy() => Scene.fromJson(toJson());
}

List<double> _matrixToList(Matrix4 m) {
  return [
    m.entry(0, 0), m.entry(1, 0), m.entry(2, 0), m.entry(3, 0),
    m.entry(0, 1), m.entry(1, 1), m.entry(2, 1), m.entry(3, 1),
    m.entry(0, 2), m.entry(1, 2), m.entry(2, 2), m.entry(3, 2),
    m.entry(0, 3), m.entry(1, 3), m.entry(2, 3), m.entry(3, 3),
  ];
}

Matrix4? _listToMatrix(dynamic json) {
  if (json is! List || json.length != 16) return null;
  final v = json.cast<num>().map((e) => e.toDouble()).toList();
  final m = Matrix4.zero();
  m.setValues(
    v[0], v[1], v[2], v[3],
    v[4], v[5], v[6], v[7],
    v[8], v[9], v[10], v[11],
    v[12], v[13], v[14], v[15],
  );
  return m;
}
