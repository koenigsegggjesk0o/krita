// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// scene_node.dart — the Feather scene-graph node.
//
// A real scene graph node (not a flat list). Each node carries:
//
//   * A local [transform] (Matrix4) relative to its parent.
//   * A list of [children] — forming the graph.
//   * A [kind] tag describing what the node references (curve, guide,
//     group, resource, light, camera).
//   * Visibility / locked / opacity flags.
//   * A [material] describing how the referenced geometry renders.
//
// The node is intentionally data-only — the rendering pipeline walks the
// graph to build its draw list. Mutating helpers ([addChild], [removeChild],
// [setLocalTransform]) keep the parent / child links consistent and let
// the host invalidate caches.
//
// Depends on: lib/core/math/

import 'dart:ui' show Color;

import 'package:feather_krita/core/math/math.dart';

/// What a [SceneNode] references.
enum SceneNodeKind {
  /// Empty group / transform — owns children only.
  group,

  /// A drawn curve (references a Stroke by id).
  curve,

  /// A 3D guide surface (references a GuideSurface by id).
  guide,

  /// An imported resource (reference image / mesh).
  resource,

  /// A scene light.
  light,

  /// A saved camera bookmark.
  camera,
}

/// A lightweight material description.
class SceneMaterial {
  SceneMaterial({
    this.color = const Color(0xffeeeeee),
    this.metallic = 0.0,
    this.roughness = 0.6,
    this.emissive = const Color(0xff000000),
    this.opacity = 1.0,
    this.doubleSided = true,
  });

  Color color;
  double metallic;
  double roughness;
  Color emissive;
  double opacity;
  bool doubleSided;

  SceneMaterial copy() => SceneMaterial(
        color: color,
        metallic: metallic,
        roughness: roughness,
        emissive: emissive,
        opacity: opacity,
        doubleSided: doubleSided,
      );
}

/// One node in the scene graph.
class SceneNode {
  SceneNode({
    required this.id,
    this.name,
    this.kind = SceneNodeKind.group,
    this.refId,
    Matrix4? transform,
    this.visible = true,
    this.locked = false,
    this.opacity = 1.0,
    SceneMaterial? material,
    List<SceneNode>? children,
  })  : transform = transform ?? Matrix4.identity(),
        material = material ?? SceneMaterial(),
        _children = children ?? [];

  /// Stable unique id (UUID-ish).
  final String id;

  /// Human-readable label shown in the Stage Panel's Group Tab.
  String? name;

  final SceneNodeKind kind;

  /// Id of the referenced Stroke / Guide / Resource, when [kind] is not
  /// `group`.
  String? refId;

  /// Local transform relative to the parent.
  Matrix4 transform;

  bool visible;
  bool locked;
  double opacity;
  SceneMaterial material;

  SceneNode? parent;
  final List<SceneNode> _children;
  List<SceneNode> get children => List.unmodifiable(_children);

  // ----- Hierarchy mutations -------------------------------------------

  void addChild(SceneNode child) {
    if (child.parent != null) {
      child.parent!._children.remove(child);
    }
    child.parent = this;
    _children.add(child);
  }

  void removeChild(SceneNode child) {
    if (_children.remove(child)) {
      child.parent = null;
    }
  }

  /// Detach this node from its parent.
  void detach() {
    parent?.removeChild(this);
  }

  /// Replace the local transform (copies the matrix).
  void setLocalTransform(Matrix4 m) {
    transform = m.clone();
  }

  // ----- Traversal ------------------------------------------------------

  /// World transform = parent's world transform * local transform.
  Matrix4 worldTransform() {
    if (parent == null) return transform.clone();
    return parent!.worldTransform() * transform;
  }

  /// Depth-first iteration over this subtree (excluding self).
  Iterable<SceneNode> descendants() sync* {
    for (final c in _children) {
      yield c;
      yield* c.descendants();
    }
  }

  /// Find a descendant by id (depth-first), or `null`.
  SceneNode? findById(String id) {
    for (final c in _children) {
      if (c.id == id) return c;
      final hit = c.findById(id);
      if (hit != null) return hit;
    }
    return null;
  }

  /// All leaf curve nodes in this subtree (depth-first, visible-only by
  /// default).
  Iterable<SceneNode> curveLeaves({bool visibleOnly = true}) sync* {
    for (final c in _children) {
      if (visibleOnly && !c.visible) continue;
      if (c.kind == SceneNodeKind.curve) {
        yield c;
      }
      yield* c.curveLeaves(visibleOnly: visibleOnly);
    }
  }

  /// Effective opacity = parent.opacity * this.opacity.
  double effectiveOpacity() {
    final p = parent?.effectiveOpacity() ?? 1.0;
    return p * opacity;
  }

  /// Effective visibility — `false` if any ancestor is hidden.
  bool effectivelyVisible() {
    if (!visible) return false;
    return parent?.effectivelyVisible() ?? true;
  }

  // ----- Serialization --------------------------------------------------

  Map<String, dynamic> toJson() => {
        'id': id,
        'name': name,
        'kind': kind.name,
        'refId': refId,
        'transform': _matrixToList(transform),
        'visible': visible,
        'locked': locked,
        'opacity': opacity,
        'material': {
          'color': material.color.toARGB32(),
          'metallic': material.metallic,
          'roughness': material.roughness,
          'emissive': material.emissive.toARGB32(),
          'opacity': material.opacity,
          'doubleSided': material.doubleSided,
        },
        'children': _children.map((c) => c.toJson()).toList(),
      };

  factory SceneNode.fromJson(Map<String, dynamic> json) {
    final node = SceneNode(
      id: json['id'] as String,
      name: json['name'] as String?,
      kind: SceneNodeKind.values.firstWhere(
        (k) => k.name == (json['kind'] as String? ?? 'group'),
        orElse: () => SceneNodeKind.group,
      ),
      refId: json['refId'] as String?,
      transform: _listToMatrix(json['transform']),
      visible: json['visible'] as bool? ?? true,
      locked: json['locked'] as bool? ?? false,
      opacity: (json['opacity'] as num?)?.toDouble() ?? 1.0,
    );
    final m = json['material'] as Map<String, dynamic>?;
    if (m != null) {
      node.material = SceneMaterial(
        color: Color((m['color'] as int?) ?? 0xffeeeeee),
        metallic: (m['metallic'] as num?)?.toDouble() ?? 0.0,
        roughness: (m['roughness'] as num?)?.toDouble() ?? 0.6,
        emissive: Color((m['emissive'] as int?) ?? 0xff000000),
        opacity: (m['opacity'] as num?)?.toDouble() ?? 1.0,
        doubleSided: m['doubleSided'] as bool? ?? true,
      );
    }
    for (final childJson in (json['children'] as List? ?? [])) {
      node.addChild(SceneNode.fromJson(childJson as Map<String, dynamic>));
    }
    return node;
  }
}

List<double> _matrixToList(Matrix4 m) {
  return [
    m.entry(0, 0), m.entry(1, 0), m.entry(2, 0), m.entry(3, 0),
    m.entry(0, 1), m.entry(1, 1), m.entry(2, 1), m.entry(3, 1),
    m.entry(0, 2), m.entry(1, 2), m.entry(2, 2), m.entry(3, 2),
    m.entry(0, 3), m.entry(1, 3), m.entry(2, 3), m.entry(3, 3),
  ];
}

Matrix4 _listToMatrix(dynamic json) {
  if (json is! List || json.length != 16) return Matrix4.identity();
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
