// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// scene_state.dart — Pure Riverpod state for the scene graph.
//
// The scene holds the document-level data:
//   - curves: 3D strokes (the "curves" the artist painted).
//   - guides: parametric guide surfaces the artist draws onto.
//   - layers: user-facing grouping / visibility / opacity for strokes.
//   - resources: imported images / 3D models referenced by the scene.
//   - environment: ambient lighting + background tone.
//
// This is a pure value type — no engine singletons, no Flutter widgets.
// The legacy [EditorState] (lib/state/editor_state.dart) still owns the live
// engine instances; this file mirrors the scene-graph slice as a Riverpod
// [Notifier] state so chrome widgets can subscribe to just the data they
// need (e.g. the layer panel reads `layers` without re-running on every
// brush-color change).

import 'package:feather_krita/engine/guide_surface.dart';
import 'package:feather_krita/models/stroke.dart';

/// One layer in the scene.
///
/// Layers are an organizational tool: each stroke carries a `layerId` and
/// the layer list controls per-group visibility / opacity / lock. The
/// layer order is the insertion order of [SceneState.layers].
class SceneLayer {
  const SceneLayer({
    required this.id,
    required this.name,
    this.isVisible = true,
    this.isLocked = false,
    this.opacity = 1.0,
  });

  /// Document-unique layer ID (a monotonic counter assigned by the
  /// scene notifier when a layer is added).
  final int id;

  /// User-facing layer name.
  final String name;

  /// Whether strokes on this layer are rendered.
  final bool isVisible;

  /// Whether strokes on this layer are editable.
  final bool isLocked;

  /// Per-layer opacity multiplier in [0, 1].
  final double opacity;

  SceneLayer copyWith({
    String? name,
    bool? isVisible,
    bool? isLocked,
    double? opacity,
  }) =>
      SceneLayer(
        id: id,
        name: name ?? this.name,
        isVisible: isVisible ?? this.isVisible,
        isLocked: isLocked ?? this.isLocked,
        opacity: opacity ?? this.opacity,
      );

  @override
  bool operator ==(Object other) =>
      identical(this, other) ||
      other is SceneLayer &&
          other.id == id &&
          other.name == name &&
          other.isVisible == isVisible &&
          other.isLocked == isLocked &&
          other.opacity == opacity;

  @override
  int get hashCode => Object.hash(id, name, isVisible, isLocked, opacity);
}

/// Ambient environment settings for the scene.
class SceneEnvironment {
  const SceneEnvironment({
    this.ambientColor = 0xFFFFFFFF,
    this.ambientIntensity = 0.35,
    this.backgroundColor = 0xFF202428,
    this.fogStart = 50.0,
    this.fogEnd = 200.0,
  });

  final int ambientColor;
  final double ambientIntensity;
  final int backgroundColor;
  final double fogStart;
  final double fogEnd;

  SceneEnvironment copyWith({
    int? ambientColor,
    double? ambientIntensity,
    int? backgroundColor,
    double? fogStart,
    double? fogEnd,
  }) =>
      SceneEnvironment(
        ambientColor: ambientColor ?? this.ambientColor,
        ambientIntensity: ambientIntensity ?? this.ambientIntensity,
        backgroundColor: backgroundColor ?? this.backgroundColor,
        fogStart: fogStart ?? this.fogStart,
        fogEnd: fogEnd ?? this.fogEnd,
      );

  @override
  bool operator ==(Object other) =>
      identical(this, other) ||
      other is SceneEnvironment &&
          other.ambientColor == ambientColor &&
          other.ambientIntensity == ambientIntensity &&
          other.backgroundColor == backgroundColor &&
          other.fogStart == fogStart &&
          other.fogEnd == fogEnd;

  @override
  int get hashCode =>
      Object.hash(ambientColor, ambientIntensity, backgroundColor, fogStart, fogEnd);
}

/// A guide surface entry in the scene.
///
/// Wraps a [GuideSurface] with a per-scene id + display name + visibility
/// flag so the artist can keep multiple guides in the scene and toggle
/// each independently.
class SceneGuide {
  SceneGuide({
    required this.id,
    required this.surface,
    this.name,
    this.isVisible = true,
  });

  final int id;
  final GuideSurface surface;
  final String? name;
  final bool isVisible;

  String get displayName => name ?? guideSurfaceTypeName(surface.type);

  SceneGuide copyWith({String? name, bool? isVisible}) => SceneGuide(
        id: id,
        surface: surface,
        name: name ?? this.name,
        isVisible: isVisible ?? this.isVisible,
      );
}

/// Immutable scene-graph state.
class SceneState {
  const SceneState({
    this.curves = const <Stroke>[],
    this.guides = const <SceneGuide>[],
    this.layers = const <SceneLayer>[
      SceneLayer(id: 0, name: 'Layer 1'),
    ],
    this.activeLayerId = 0,
    this.nextLayerId = 1,
    this.nextGuideId = 1,
    this.environment = const SceneEnvironment(),
    this.activeGuideId,
  });

  /// All painted strokes in z-order (back-to-front).
  final List<Stroke> curves;

  /// All guide surfaces in the scene.
  final List<SceneGuide> guides;

  /// Layers in order.
  final List<SceneLayer> layers;

  /// The layer new strokes are added to.
  final int activeLayerId;

  /// Next available layer id (monotonic).
  final int nextLayerId;

  /// Next available guide id (monotonic).
  final int nextGuideId;

  /// Ambient environment.
  final SceneEnvironment environment;

  /// The guide surface new strokes snap onto, if any.
  final int? activeGuideId;

  /// The active guide surface object, if any.
  SceneGuide? get activeGuide =>
      guides.where((g) => g.id == activeGuideId).firstOrNull;

  /// The active layer, if any.
  SceneLayer? get activeLayer =>
      layers.where((l) => l.id == activeLayerId).firstOrNull;

  SceneState copyWith({
    List<Stroke>? curves,
    List<SceneGuide>? guides,
    List<SceneLayer>? layers,
    int? activeLayerId,
    int? nextLayerId,
    int? nextGuideId,
    SceneEnvironment? environment,
    int? activeGuideId,
  }) =>
      SceneState(
        curves: curves ?? this.curves,
        guides: guides ?? this.guides,
        layers: layers ?? this.layers,
        activeLayerId: activeLayerId ?? this.activeLayerId,
        nextLayerId: nextLayerId ?? this.nextLayerId,
        nextGuideId: nextGuideId ?? this.nextGuideId,
        environment: environment ?? this.environment,
        activeGuideId: activeGuideId ?? this.activeGuideId,
      );

  /// Snapshot of the scene as plain JSON-friendly data (used by the
  /// project repository for .feather serialization).
  Map<String, dynamic> toJson() => {
        'curves': curves.map((s) => s.toJson()).toList(),
        'guides': guides
            .map((g) => {
                  'id': g.id,
                  'name': g.name,
                  'isVisible': g.isVisible,
                  'type': guideSurfaceTypeName(g.surface.type),
                  'params': g.surface.shapeParams,
                })
            .toList(),
        'layers': layers
            .map((l) => {
                  'id': l.id,
                  'name': l.name,
                  'isVisible': l.isVisible,
                  'isLocked': l.isLocked,
                  'opacity': l.opacity,
                })
            .toList(),
        'activeLayerId': activeLayerId,
        'nextLayerId': nextLayerId,
        'nextGuideId': nextGuideId,
        'activeGuideId': activeGuideId,
        'environment': {
          'ambientColor': environment.ambientColor,
          'ambientIntensity': environment.ambientIntensity,
          'backgroundColor': environment.backgroundColor,
          'fogStart': environment.fogStart,
          'fogEnd': environment.fogEnd,
        },
      };

  @override
  bool operator ==(Object other) =>
      identical(this, other) ||
      other is SceneState &&
          other.activeLayerId == activeLayerId &&
          other.nextLayerId == nextLayerId &&
          other.nextGuideId == nextGuideId &&
          other.activeGuideId == activeGuideId &&
          other.environment == environment &&
          _listEq(other.guides, guides) &&
          _listEq(other.layers, layers) &&
          _listEq(other.curves, curves);

  @override
  int get hashCode => Object.hash(
        activeLayerId,
        nextLayerId,
        nextGuideId,
        activeGuideId,
        environment,
        Object.hashAll(guides),
        Object.hashAll(layers),
        Object.hashAll(curves),
      );
}

bool _listEq<T>(List<T> a, List<T> b) {
  if (a.length != b.length) return false;
  for (var i = 0; i < a.length; i++) {
    if (a[i] != b[i]) return false;
  }
  return true;
}
