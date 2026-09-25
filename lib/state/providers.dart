// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// providers.dart — Riverpod providers for the Feather-Krita state layer.
//
// Exposes one [NotifierProvider] per pure-state slice defined in this
// directory:
//   - editorUiProvider      — active tool, mode, UI visibility (EditorUiState)
//   - sceneProvider         — curves, guides, layers, environment (SceneState)
//   - brushProvider         — preset, color, size, material (BrushState)
//   - selectionProvider     — selected curve / guide ids (SelectionState)
//   - transformProvider     — mode, axis, lock, joystick type (TransformState)
//   - cameraProvider        — yaw / pitch / distance / FOV (CameraState)
//   - historyProvider       — undo/redo command stack (HistoryState)
//   - projectProvider       — project name, paths, auto-save (ProjectState)
//
// Notifiers expose mutation helpers (setActiveTool, addCurve, undo, etc.)
// that internally [state = state.copyWith(...)] so Riverpod diffing works.
//
// The legacy [EditorState] [ChangeNotifier] still owns the live engine
// instances; this Riverpod layer is the chrome-facing mirror of the
// user-mutable slice. Bridges (legacy EditorState <-> Riverpod notifiers)
// will be added in a later loop.

import 'package:flutter_riverpod/flutter_riverpod.dart';
import 'package:vector_math/vector_math_64.dart' show Vector3;

import 'package:feather_krita/engine/guide_surface.dart';
import 'package:feather_krita/engine/stroke_manager.dart' show TransformMode;
import 'package:feather_krita/models/brush_preset.dart' show BrushPreset;
import 'package:feather_krita/models/stroke.dart' show Stroke;
import 'package:feather_krita/state/brush_state.dart';
import 'package:feather_krita/state/camera_state.dart';
import 'package:feather_krita/state/editor_state.dart'
    show EditorMode, EditorUiState, Tool;
import 'package:feather_krita/state/history_state.dart';
import 'package:feather_krita/state/project_state.dart';
import 'package:feather_krita/state/scene_state.dart';
import 'package:feather_krita/state/selection_state.dart';
import 'package:feather_krita/state/transform_state.dart';

// ----- Editor UI --------------------------------------------------------

class EditorUiNotifier extends Notifier<EditorUiState> {
  @override
  EditorUiState build() => const EditorUiState();

  void setActiveTool(Tool tool) =>
      state = state.copyWith(activeTool: tool);
  void setActiveGuide(String name) =>
      state = state.copyWith(activeGuide: name);
  void setMode(EditorMode mode) => state = state.copyWith(mode: mode);
  void toggleGrid() => state = state.copyWith(showGrid: !state.showGrid);
  void toggleMirrorPlanes() =>
      state = state.copyWith(showMirrorPlanes: !state.showMirrorPlanes);
  void toggleToolbar() =>
      state = state.copyWith(showToolbar: !state.showToolbar);
  void toggleStatusBar() =>
      state = state.copyWith(showStatusBar: !state.showStatusBar);
  void toggleGuideList() =>
      state = state.copyWith(showGuideList: !state.showGuideList);
  void togglePresetBrowser() =>
      state = state.copyWith(showPresetBrowser: !state.showPresetBrowser);
  void closeAllPanels() => state = state.copyWith(
        showGuideList: false,
        showPresetBrowser: false,
      );
}

final editorUiProvider =
    NotifierProvider<EditorUiNotifier, EditorUiState>(EditorUiNotifier.new);

// ----- Scene ------------------------------------------------------------

class SceneNotifier extends Notifier<SceneState> {
  @override
  SceneState build() => const SceneState();

  void replaceCurves(List<Stroke> curves) =>
      state = state.copyWith(curves: List<Stroke>.unmodifiable(curves));
  void addCurve(Stroke stroke) =>
      state = state.copyWith(curves: [...state.curves, stroke]);
  void removeCurve(int id) =>
      state = state.copyWith(curves: state.curves.where((s) => s.id != id).toList());

  void addGuide(GuideSurface surface, {String? name}) {
    final guide = SceneGuide(
      id: state.nextGuideId,
      surface: surface,
      name: name,
    );
    state = state.copyWith(
      guides: [...state.guides, guide],
      nextGuideId: state.nextGuideId + 1,
      activeGuideId: guide.id,
    );
  }

  void removeGuide(int id) {
    final next = state.guides.where((g) => g.id != id).toList();
    state = state.copyWith(
      guides: next,
      activeGuideId: state.activeGuideId == id ? null : state.activeGuideId,
    );
  }

  void setActiveGuide(int? id) =>
      state = state.copyWith(activeGuideId: id);

  void addLayer(String name) {
    final layer = SceneLayer(id: state.nextLayerId, name: name);
    state = state.copyWith(
      layers: [...state.layers, layer],
      nextLayerId: state.nextLayerId + 1,
      activeLayerId: layer.id,
    );
  }

  void updateLayer(int id, SceneLayer Function(SceneLayer) update) {
    final next = state.layers.map((l) => l.id == id ? update(l) : l).toList();
    state = state.copyWith(layers: next);
  }

  void removeLayer(int id) {
    if (state.layers.length <= 1) return;
    final next = state.layers.where((l) => l.id != id).toList();
    state = state.copyWith(
      layers: next,
      activeLayerId:
          state.activeLayerId == id ? next.first.id : state.activeLayerId,
    );
  }

  void setActiveLayer(int id) =>
      state = state.copyWith(activeLayerId: id);

  void setEnvironment(SceneEnvironment env) =>
      state = state.copyWith(environment: env);
}

final sceneProvider =
    NotifierProvider<SceneNotifier, SceneState>(SceneNotifier.new);

// ----- Brush ------------------------------------------------------------

class BrushNotifier extends Notifier<BrushState> {
  @override
  BrushState build() => const BrushState();

  void setPreset(BrushPreset preset) => state = state.copyWith(
        preset: preset,
        presetName: preset.name,
        flow: preset.flowValue,
      );
  void setColor(int argb) => state = state.copyWith(color: argb);
  void setSize(double px) => state = state.copyWith(size: px);
  void setOpacity(double o) => state = state.copyWith(opacity: o);
  void setSpacing(double s) => state = state.copyWith(spacing: s);
  void setHardness(double h) => state = state.copyWith(hardness: h);
  void setFlow(double f) => state = state.copyWith(flow: f);
  void setMaterial(BrushMaterial m) => state = state.copyWith(material: m);
  void setPattern(BrushPattern? p) =>
      state = p == null ? state.copyWith(clearPattern: true) : state.copyWith(pattern: p);
}

final brushProvider =
    NotifierProvider<BrushNotifier, BrushState>(BrushNotifier.new);

// ----- Selection --------------------------------------------------------

class SelectionNotifier extends Notifier<SelectionState> {
  @override
  SelectionState build() => const SelectionState();

  void addCurve(int id) => state = state.addCurve(id);
  void removeCurve(int id) => state = state.removeCurve(id);
  void toggleCurve(int id) => state = state.toggleCurve(id);
  void selectCurves(List<int> ids) => state = state.selectCurves(ids);
  void selectGuides(List<int> ids) => state = state.selectGuides(ids);
  void clear() => state = state.clear();
  void setHover({int? curveId, int? guideId}) =>
      state = state.withHover(curveId: curveId, guideId: guideId);
}

final selectionProvider =
    NotifierProvider<SelectionNotifier, SelectionState>(SelectionNotifier.new);

// ----- Transform --------------------------------------------------------

class TransformNotifier extends Notifier<TransformState> {
  @override
  TransformState build() => const TransformState();

  void setMode(TransformMode mode) => state = state.copyWith(mode: mode);
  void setAxis(TransformAxis axis) => state = state.copyWith(axis: axis);
  void toggleLockX() => state = state.copyWith(lockX: !state.lockX);
  void toggleLockY() => state = state.copyWith(lockY: !state.lockY);
  void toggleLockZ() => state = state.copyWith(lockZ: !state.lockZ);
  void toggleUniformScale() =>
      state = state.copyWith(uniformScale: !state.uniformScale);
  void setJoystickType(JoystickType t) =>
      state = state.copyWith(joystickType: t);
  void toggleSnapToGrid() =>
      state = state.copyWith(snapToGrid: !state.snapToGrid);
  void setGridSize(double s) => state = state.copyWith(gridSize: s);
  void setSnapAngle(double deg) =>
      state = state.copyWith(snapAngleDeg: deg);
}

final transformProvider =
    NotifierProvider<TransformNotifier, TransformState>(TransformNotifier.new);

// ----- Camera -----------------------------------------------------------

class CameraNotifier extends Notifier<CameraState> {
  @override
  CameraState build() => const CameraState();

  void setYaw(double yaw) => state = state.copyWith(yaw: yaw);
  void setPitch(double pitch) =>
      state = state.copyWith(pitch: pitch.clamp(state.minPitch, state.maxPitch));
  void setDistance(double d) => state =
      state.copyWith(distance: d.clamp(state.minDistance, state.maxDistance));
  void setTarget(Vector3 target) => state = state.copyWith(target: target);
  void orbit(double dYaw, double dPitch) => state = state.copyWith(
        yaw: state.yaw + dYaw,
        pitch: (state.pitch + dPitch)
            .clamp(state.minPitch, state.maxPitch),
      );
  void zoom(double factor) => state = state.copyWith(
        distance:
            (state.distance * factor).clamp(state.minDistance, state.maxDistance),
      );
  void setProjection(CameraProjection p) =>
      state = state.copyWith(projection: p);
  void setFov(double radians) =>
      state = state.copyWith(fovYRadians: radians);
  void reset() => state = state.reset();
}

final cameraProvider =
    NotifierProvider<CameraNotifier, CameraState>(CameraNotifier.new);

// ----- History ----------------------------------------------------------

class HistoryNotifier extends Notifier<HistoryState> {
  @override
  HistoryState build() => const HistoryState();

  void push(HistoryCommand command, {String? branchLabel}) =>
      state = state.push(command, branchLabel: branchLabel);
  void undo() => state = state.undo();
  void redo() => state = state.redo();
  void jumpTo(int index) => state = state.jumpTo(index);
  void clear() => state = state.clear();
}

final historyProvider =
    NotifierProvider<HistoryNotifier, HistoryState>(HistoryNotifier.new);

// ----- Project ----------------------------------------------------------

class ProjectNotifier extends Notifier<ProjectState> {
  @override
  ProjectState build() => const ProjectState();

  void setName(String name) => state = state.copyWith(name: name);
  void setPath(String path) => state = state.copyWith(filePath: path);
  void markDirty() => state = state.markDirty();
  void markSaved(String path, {List<int>? thumbnail}) =>
      state = state.markSaved(path, thumbnail: thumbnail);
  void markAutoSaved() => state = state.markAutoSaved();
  void setAutoSave(AutoSaveConfig cfg) =>
      state = state.copyWith(autoSave: cfg);
  void setThumbnail(List<int> bytes) =>
      state = state.copyWith(thumbnailBytes: bytes);
}

final projectProvider =
    NotifierProvider<ProjectNotifier, ProjectState>(ProjectNotifier.new);
