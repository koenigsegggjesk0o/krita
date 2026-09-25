// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// stage_panel.dart — the Stage Panel model.
//
// The Stage Panel (see `stagepanel.txt`) is the document-management
// overlay with three tabs:
//
//   * GROUP TAB      — manage groups (create / delete / reorder, set the
//                      active group, show / hide / lock per group). Backed
//                      by the scene's [LayerStack].
//   * RESOURCE TAB   — import and manage reference resources (images,
//                      meshes). Backed by the scene's resource registry.
//   * ENVIRONMENT TAB — control the scene environment (background colour
//                      / image, lighting, world / grid / DOF settings).
//                      Backed by the scene's [Environment].
//
// This file is the UI-state layer: which tab is open, which group /
// resource is selected in the panel, search filter, etc. It wraps a
// [Scene] so the panel widget can stay declarative.
//
// Depends on: lib/core/math/, lib/core/scene/*

import 'dart:ui' show Color;

import 'package:flutter/foundation.dart';

import 'package:feather_krita/core/scene/environment.dart';
import 'package:feather_krita/core/scene/scene.dart';
import 'package:feather_krita/core/scene/scene_layer.dart';

/// The three Stage Panel tabs.
enum StageTab { group, resource, environment }

extension StageTabX on StageTab {
  String get label => switch (this) {
        StageTab.group => 'Group',
        StageTab.resource => 'Resource',
        StageTab.environment => 'Environment',
      };
}

/// The Stage Panel controller.
class StagePanelModel extends ChangeNotifier {
  StagePanelModel({
    required this.scene,
    StageTab initialTab = StageTab.group,
  }) : _tab = initialTab;

  final Scene scene;

  StageTab _tab;
  StageTab get activeTab => _tab;
  set activeTab(StageTab tab) {
    if (_tab == tab) return;
    _tab = tab;
    notifyListeners();
  }

  // ----- Group-tab state -----------------------------------------------

  /// The currently-inspected group in the Group Tab (defaults to active).
  String? get selectedGroupId =>
      _selectedGroupId ?? scene.layers.activeId;
  String? _selectedGroupId;
  set selectedGroupId(String? id) {
    _selectedGroupId = id;
    notifyListeners();
  }

  /// Search filter applied to the Group Tab list.
  String groupFilter = '';

  List<SceneLayer> get filteredLayers {
    final q = groupFilter.trim().toLowerCase();
    if (q.isEmpty) return scene.layers.layers;
    return scene.layers.layers
        .where((l) => l.name.toLowerCase().contains(q))
        .toList(growable: false);
  }

  /// Add a new group and make it the active + selected one.
  SceneLayer addGroup(String name) {
    final g = scene.layers.addNewGroup(name);
    _selectedGroupId = g.id;
    notifyListeners();
    return g;
  }

  /// Delete a group. Its curves are reparented to the active group (or
  /// the first remaining group) so nothing is orphaned.
  bool deleteGroup(String id) {
    final layer = scene.layers.findById(id);
    if (layer == null) return false;
    final target = scene.layers.layers
        .where((l) => l.id != id)
        .firstOrNull;
    if (target != null) {
      for (final c in layer.curveIds) {
        target.addCurve(c);
      }
    }
    final removed = scene.layers.remove(id);
    if (_selectedGroupId == id) _selectedGroupId = scene.layers.activeId;
    if (removed) notifyListeners();
    return removed;
  }

  void setGroupVisibility(String id, bool visible) {
    final l = scene.layers.findById(id);
    if (l == null) return;
    l.visible = visible;
    notifyListeners();
  }

  void setGroupLocked(String id, bool locked) {
    final l = scene.layers.findById(id);
    if (l == null) return;
    l.locked = locked;
    notifyListeners();
  }

  void setActiveGroup(String id) {
    scene.layers.activeId = id;
    _selectedGroupId = id;
    notifyListeners();
  }

  void reorderGroup(String id, int newIndex) {
    scene.layers.reorder(id, newIndex);
    notifyListeners();
  }

  // ----- Resource-tab state --------------------------------------------

  String? selectedResourceId;
  String resourceFilter = '';

  List<ResourceEntry> get filteredResources {
    final q = resourceFilter.trim().toLowerCase();
    final all = scene.resources.values.toList();
    if (q.isEmpty) return all;
    return all.where((r) => r.name.toLowerCase().contains(q)).toList();
  }

  void selectResource(String? id) {
    selectedResourceId = id;
    notifyListeners();
  }

  void setResourceVisibility(String id, bool visible) {
    final r = scene.resources[id];
    if (r == null) return;
    r.visible = visible;
    notifyListeners();
  }

  // ----- Environment-tab state -----------------------------------------

  void setBackgroundColor(int argb) {
    scene.environment.backgroundColor = Color(argb);
    notifyListeners();
  }

  void setBackgroundImage(String? assetPath, {double? opacity}) {
    scene.environment.backgroundImage.assetPath = assetPath;
    if (opacity != null) scene.environment.backgroundImage.opacity = opacity;
    notifyListeners();
  }

  void setLightAzimuth(double degrees) {
    scene.environment.light.azimuthDegrees = degrees;
    notifyListeners();
  }

  void setLightElevation(double degrees) {
    scene.environment.light.elevationDegrees = degrees;
    notifyListeners();
  }

  void setLightIntensity(double intensity) {
    scene.environment.light.intensity = intensity;
    notifyListeners();
  }

  void toggleGrid() {
    scene.environment.world.showGroundGrid =
        !scene.environment.world.showGroundGrid;
    notifyListeners();
  }

  void setGridSpacing(double spacing) {
    scene.environment.world.gridSpacing = spacing;
    notifyListeners();
  }

  void toggleDepthOfField() {
    scene.environment.world.depthOfField =
        !scene.environment.world.depthOfField;
    notifyListeners();
  }

  void setDofFocusDistance(double distance) {
    scene.environment.world.dofFocusDistance = distance;
    notifyListeners();
  }

  Environment get environment => scene.environment;
  LayerStack get layerStack => scene.layers;

  @override
  String toString() =>
      'StagePanelModel(tab: $activeTab, groups: ${scene.layers.layers.length}, '
      'resources: ${scene.resources.length})';
}
