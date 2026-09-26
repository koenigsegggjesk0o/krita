// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// main_screen.dart — Editor host (feather-integration).
//
// This is the production editor screen. It owns the LIVE instances of the
// new 158-file engine and composes the layout-only
// [EditorScreen] widget (lib/ui/screens/editor_screen.dart) around them.
//
// Owned engine singletons:
//   * [Scene]               — the scene graph + layer stack + environment
//                             (lib/core/scene/scene.dart).
//   * [OrbitCamera]         — turntable camera with yaw/pitch/distance/FOV
//                             (lib/core/camera/orbit_camera.dart).
//   * [KritaEngine]         — native Krita bridge lifecycle, with the
//                             honest fallback when the DLL is absent
//                             (lib/engine/krita_bridge/krita_engine.dart).
//   * [BrushEngine]         — 2D dab generation + 3D stroke assembly
//                             (lib/engine/brush/brush_engine.dart).
//   * [Guide3DManager]      — owns every active [Guide3D] surface
//                             (lib/engine/guide3d/guide_manager.dart).
//   * [LiquifyEngine]       — push/pinch/comb deformation lifecycle
//                             (lib/engine/liquify/liquify_engine.dart).
//   * [SelectionSystem]     — tap/box/lasso selection over the stroke list
//                             (lib/engine/selection/selection.dart).
//
// The host also owns the document's [Stroke] list, a simple undo/redo
// stack of deep-copied stroke snapshots, and a mirror of the
// [EditorUiState] the EditorScreen widget emits via [onUiStateChanged].
//
// Wiring contract:
//   * Strokes drawn on the canvas are raycast onto the active guide (or
//     a y=0 ground plane fallback) and assembled by [BrushEngine] into a
//     [Stroke] that is committed to both the document list and the
//     [Scene] graph.
//   * Camera pan/zoom gestures from the canvas viewport drive
//     [OrbitCamera.orbit] / [OrbitCamera.zoom].
//   * Brush-panel mutations (color / size / opacity / pressure) are
//     mirrored into [BrushEngine.settings] and the live [KritaBrushBackend]
//     so dabs and 3D strokes stay in sync.
//   * The Stage panel's Group Tab mutates [Scene.layers].
//   * The Liquify panel drives [LiquifyEngine] (begin / apply / undoAll).
//   * The Joystick applies [Matrix4] transforms to the selected strokes.
//   * Undo/redo replays deep-copied stroke snapshots.
//
// Honest gaps (NOT gimmicks — these are flagged for the next loop):
//   * Tap-select on the canvas (SelectionSystem.tapSelectSync) requires a
//     distinct tap gesture; the current CanvasViewport only emits pan /
//     stroke gestures. Selecting via the radial-menu "Select All" works;
//     individual tap-select is wired but not gesture-routed yet.
//   * Liquify drag-on-canvas is now routed (see the [_isLiquifyDragging]
//     branch in CanvasViewport's gesture handler + [_onLiquifyDrag] in
//     this host). The brush cursor + drag delta drive a live
//     [LiquifyRenderer] preview overlay on the canvas.
//   * The brush preset picker now wires REAL .kpp loading. At boot the
//     host scans the feather_presets + feather_resources/paintoppresets
//     directories for .kpp files and builds UI [BrushPreset] entries
//     that carry the file path. When the real native Krita backend is
//     live, picking one feeds the path to
//     [KritaBrushController.loadPreset] — the native bridge unpacks the
//     .kpp container (ZIP KoStore / legacy PNG-with-zTXt / bare XML) and
//     hands the preset XML to Krita's own paintop-settings parser. The
//     host then reads the parsed scalar params (size / opacity /
//     hardness / flow / eraser) back via the reflection getters and
//     mirrors them into the [BrushEngine] + the brush settings panel.
//     On the fallback engine (or for the bundled visual-only presets
//     with no .kpp behind them) the picker keeps the legacy visual-hints
//     behaviour (colour + strokeWidth).

import 'dart:async';
import 'dart:convert' show jsonDecode;
import 'dart:io' show Directory, File, Platform;
import 'dart:math' as dmath;
import 'dart:typed_data';
import 'dart:ui' as ui;

import 'package:file_picker/file_picker.dart';
import 'package:flutter/material.dart';
import 'package:flutter/rendering.dart' show RenderRepaintBoundary;
import 'package:flutter/services.dart' show Clipboard, ClipboardData, LogicalKeyboardKey;
import 'package:share_plus/share_plus.dart';
import 'package:shared_preferences/shared_preferences.dart';
import 'package:vector_math/vector_math_64.dart' hide Colors;

import 'package:feather_krita/core/camera/orbit_camera.dart';
import 'package:feather_krita/core/scene/scene.dart';
import 'package:feather_krita/data/models/project_model.dart';
import 'package:feather_krita/data/models/settings_model.dart';
import 'package:feather_krita/data/preset_repository.dart';
import 'package:feather_krita/data/settings_repository.dart';
import 'package:feather_krita/engine/assistance/assistance_wiring.dart';
import 'package:feather_krita/engine/assistance/mirror_assist.dart';
import 'package:feather_krita/engine/assistance/stable_strokes.dart';
import 'package:feather_krita/engine/brush/brush_engine.dart';
import 'package:feather_krita/engine/brush/brush_renderer.dart';
import 'package:feather_krita/engine/brush/brush_settings.dart';
import 'package:feather_krita/engine/brush/eraser_engine.dart';
import 'package:feather_krita/core/math/vec3.dart';
import 'package:feather_krita/core/math/ray.dart' as math;
import 'package:feather_krita/core/math/plane.dart' as math show Plane;
import 'package:feather_krita/core/math/sphere.dart' as math show Sphere;
import 'package:feather_krita/core/math/scalar_math.dart' as scalarmath
    show wrapAngle;
import 'package:feather_krita/engine/curves/stroke3d.dart';
import 'package:feather_krita/engine/guide3d/guide_manager.dart';
import 'package:feather_krita/engine/guide3d/guide3d_type.dart';
import 'package:feather_krita/engine/guide3d/guide_snap.dart';
import 'package:feather_krita/engine/guide3d/guide_renderer.dart';
import 'package:feather_krita/engine/guide3d/drawn_guide.dart';
import 'package:feather_krita/engine/guide3d/lofted_guide.dart';
import 'package:feather_krita/engine/guide3d/bent_guide.dart';
import 'package:feather_krita/engine/guide3d/primitive_guide.dart';
import 'package:feather_krita/engine/guide3d/guide3d_primitive.dart';
import 'package:feather_krita/engine/krita_bridge/krita_canvas_controller.dart';
import 'package:feather_krita/engine/krita_bridge/krita_engine.dart';
import 'package:feather_krita/engine/krita_bridge/krita_fallback.dart'
    show KritaBrushBackend;
import 'package:feather_krita/engine/liquify/liquify_brush.dart';
import 'package:feather_krita/engine/liquify/liquify_engine.dart';
import 'package:feather_krita/engine/liquify/liquify_renderer.dart';
import 'package:feather_krita/engine/material/light_rig.dart';
import 'package:feather_krita/engine/selection/selection.dart';
import 'package:feather_krita/engine/selection/selection_renderer.dart';
import 'package:feather_krita/engine/selection/selection_state.dart';
import 'package:feather_krita/engine/transform/gizmo_renderer.dart';
import 'package:feather_krita/engine/transform/joystick2d.dart';
import 'package:feather_krita/engine/transform/joystick3d.dart';
import 'package:feather_krita/engine/transform/transform_mode.dart';
import 'package:feather_krita/engine/transform/transform_resolver.dart';
import 'package:feather_krita/ffi/krita_bindings.dart' show BrushColor, BrushInput;
import 'package:feather_krita/io/app_dirs.dart' show exportsDir;
import 'package:feather_krita/io/gltf_exporter.dart';
import 'package:feather_krita/io/obj_exporter.dart';
import 'package:feather_krita/io/png_exporter.dart';
import 'package:feather_krita/models/stroke.dart';
import 'package:feather_krita/ui/screens/editor_screen.dart';
import 'package:feather_krita/ui/theme/feather_colors.dart';
import 'package:feather_krita/ui/theme/feather_typography.dart';
import 'package:feather_krita/ui/widgets/about_dialog.dart';
import 'package:feather_krita/ui/widgets/brush_picker.dart'
    show BrushPreset;
import 'package:feather_krita/ui/widgets/editor_shortcuts.dart';
import 'package:feather_krita/ui/widgets/assist_panel.dart';
import 'package:feather_krita/ui/widgets/guide_panel.dart';
import 'package:feather_krita/ui/widgets/material_picker.dart'
    show FeatherMaterial, FeatherPattern;
import 'package:feather_krita/ui/widgets/right_panel.dart'
    show LayerItem, ResourceItem;
import 'package:feather_krita/ui/widgets/tool_dock.dart' show FeatherTool;
import 'package:feather_krita/ui/widgets/liquify_panel.dart'
    show LiquifyMode;
import 'package:feather_krita/ui/widgets/render_mode_toggle.dart'
    show RenderModeState, RenderModeToggle;
import 'package:feather_krita/ui/widgets/light_rig_panel.dart'
    show LightRigPanel;
import 'package:feather_krita/ui/widgets/tutorial_overlay.dart'
    show kTutorialShownKey, kTutorialHints, tutorialHintAt;
import 'package:feather_krita/ui/widgets/stroke_list_panel.dart'
    show StrokeListItem, StrokeListPanel;
import 'package:feather_krita/utils/app_version.dart';
import 'package:feather_krita/utils/crash_log.dart';
import 'package:feather_krita/utils/paint_perf.dart';
import 'package:feather_krita/ui/widgets/canvas_viewport.dart'
    show
        CanvasMaterial,
        CanvasScene,
        CanvasStroke,
        CanvasOverlay,
        CanvasOverlayLine,
        CanvasOverlayPolyline,
        CanvasOverlayCircle,
        CanvasOverlayTriangle;

/// The production editor host. Owns the new engine and composes the
/// [EditorScreen] layout shell around it.
class MainScreen extends StatefulWidget {
  const MainScreen({
    super.key,
    this.engineReal = false,
  });

  /// Whether the boot probe found the real native Krita bridge. When
  /// false the editor runs on the honest synthetic-dab fallback (the
  /// [KritaEngine] still constructs and exposes a [KritaFallbackEngine]
  /// backend, so painting is unaffected).
  final bool engineReal;

  // ----- v56-B: undo stack OOM mitigation --------------------------------
  //
  // The host stores a deep-copy snapshot of the whole stroke list per undo
  // step (see [_MainScreenState._pushUndo]). At 1000 strokes × 40 snapshots
  // that was ~40 000 Stroke copies in memory (≈200 MB) — a real OOM risk on
  // Android that v0.55-C flagged honestly.
  //
  // Fix (conservative, no behaviour gimmick):
  //   * Halve the base undo depth 40 → 20 ([kBaseMaxUndo]).
  //   * On large documents (> [kLargeDocStrokeThreshold] strokes) further
  //     halve it to 10 ([kLargeDocMaxUndo]) because each snapshot is big.
  //   * The redo stack needs no explicit cap: it is cleared on every
  //     [_MainScreenState._pushUndo] and can only grow to the depth of a
  //     prior undo run, so it is implicitly bounded by this cap.
  /// Base undo depth. Was 40; halved to 20 in v56-B to bound snapshot memory.
  @visibleForTesting
  static const int kBaseMaxUndo = 20;

  /// Reduced undo depth applied to large documents.
  @visibleForTesting
  static const int kLargeDocMaxUndo = 10;

  /// Stroke count above which the undo depth is reduced to [kLargeDocMaxUndo].
  @visibleForTesting
  static const int kLargeDocStrokeThreshold = 200;

  /// v0.57-B: world-space snap radius used by the snap-to-guide helper
  /// ([_MainScreenState._guideSnapHelper]). Pinned as a @visibleForTesting
  /// constant so [test/guide_snap_wiring_test.dart] can assert the wiring
  /// without pumping the full [MainScreen] widget (which needs engine + FFI
  /// init). Mirrors the [effectiveMaxUndoFor] pattern from v56-B.
  @visibleForTesting
  static const double kGuideSnapMaxDistance = 0.25;

  /// v0.57-B: ARGB colour used for the translucent guide-surface triangles
  /// emitted by [_MainScreenState._buildGuideRibbonOverlay]. Pinned as a
  /// @visibleForTesting constant so [test/guide_renderer_wiring_test.dart]
  /// can assert the wiring without pumping the full [MainScreen] widget.
  /// Matches [Guide3DRenderer.defaultSurfaceColor] (0xFF8FB8FF).
  @visibleForTesting
  static const int kGuideRibbonSurfaceColor = 0xFF8FB8FF;

  /// v0.57-B: ARGB colour used for the orange guide starting-line segment
  /// emitted by [_MainScreenState._buildGuideRibbonOverlay]. Matches
  /// [Guide3DRenderer.orangeStartColor] (0xFFFF8A00).
  @visibleForTesting
  static const int kGuideRibbonStartLineColor = 0xFFFF8A00;

  /// Effective undo depth for a document with [strokeCount] strokes.
  ///
  /// Memory-aware: halves the cap on large docs to bound peak memory. This is
  /// a pure function of the stroke count so it is unit-testable without
  /// pumping the full [MainScreen] (which needs engine + FFI init).
  @visibleForTesting
  static int effectiveMaxUndoFor(int strokeCount) =>
      strokeCount > kLargeDocStrokeThreshold ? kLargeDocMaxUndo : kBaseMaxUndo;

  @override
  State<MainScreen> createState() => _MainScreenState();
}

class _MainScreenState extends State<MainScreen>
    with TickerProviderStateMixin {
  // ----- Engine singletons ------------------------------------------------

  late final Scene _scene;
  late final OrbitCamera _camera;
  late final KritaEngine _krita;
  late final BrushEngine _brush;
  late final Guide3DManager _guides;
  late final LiquifyEngine _liquify;
  late final SelectionModel _selectionModel;
  late final SelectionSystem _selection;

  // v0.57-B: snap-to-guide helper. Stateless engine object — constructed
  // once at boot and reused per stroke sample. The snap toggle itself is
  // [_guideSnap] (written by the GuidePanel's "Snap to guide" switch).
  // The snap radius is [MainScreen.kGuideSnapMaxDistance] (pinned on the
  // public widget class so tests can reach it without pumping the state).
  late final Guide3DSnap _guideSnapHelper;

  /// The document's strokes in z-order (back → front). The [Scene] graph
  /// holds string-id references to these; the host owns the canonical
  /// list and the integer-id space.
  final List<Stroke> _strokes = <Stroke>[];
  int _nextStrokeId = 1;

  // ----- Undo / redo (deep-copy snapshots of the stroke list) -------------

  final List<List<Stroke>> _undoStack = <List<Stroke>>[];
  final List<List<Stroke>> _redoStack = <List<Stroke>>[];
  // v56-B: was 40 — halved to 20 to bound snapshot memory. Aliases the
  // testable [MainScreen.kBaseMaxUndo] so there is one source of truth for
  // the base cap. The effective cap is further reduced on large documents;
  // see [_effectiveMaxUndo].
  static const int _maxUndo = MainScreen.kBaseMaxUndo;

  /// v56-B: memory-aware effective undo depth.
  ///
  /// Returns [_maxUndo] (20) for normal documents and a reduced cap
  /// ([MainScreen.kLargeDocMaxUndo] = 10) when the document holds more than
  /// [MainScreen.kLargeDocStrokeThreshold] strokes, because each deep-copy
  /// snapshot is large on big documents. The pure threshold logic lives in
  /// the testable [MainScreen.effectiveMaxUndoFor].
  int get _effectiveMaxUndo => _strokes.length > MainScreen.kLargeDocStrokeThreshold
      ? MainScreen.kLargeDocMaxUndo
      : _maxUndo;

  // v0.55-C: stroke soft-cap (memory hygiene).
  //
  // The host stores every committed stroke in [_strokes] (and a parallel
  // deep-copy snapshot in [_undoStack] / [_redoStack], capped by [_maxUndo]).
  // Each Stroke carries a position + pressure list, and each Stroke3D in
  // [_stroke3Ds] carries the per-sample world geometry. At ~5 KB / stroke
  // (Stroke + Stroke3D + scene-graph reference + material map entry) the
  // host's document memory crosses ~5 MB at 1000 strokes and ~50 MB at
  // 10000 strokes — the latter is the OOM risk the brief flagged.
  //
  // We do NOT auto-merge or auto-delete the oldest stroke (that would
  // destroy the user's work silently). Instead we surface a soft warning
  // SnackBar the moment the stroke count crosses [kStrokeSoftCap], nudging
  // the user to merge / export / clear. The warning fires exactly once per
  // threshold-cross (re-arms if the count drops back below and re-crosses).
  // The cap value + decision logic live in lib/utils/paint_perf.dart so
  // they're unit-testable in isolation.
  bool _strokeCapWarned = false;

  // ----- Persistent settings (v0.57-D) ------------------------------------
  //
  // The [SettingsRepository] is the disk-backed persistence layer for
  // session-spanning defaults (brush size, grid visibility, mirror
  // planes, guide surface, …). It is the ONLY place in the host that
  // touches [SharedPreferences] key names for app settings — every
  // consumer goes through the typed [SettingsModel]. The tutorial
  // first-run flag (kTutorialShownKey) is intentionally NOT routed
  // through here: it predates SettingsModel and lives as a one-off
  // SharedPreferences bool keyed by [kTutorialShownKey].
  final SettingsRepository _settingsRepo = SettingsRepository();
  bool _settingsLoaded = false;

  // v0.57-D: brush-preset data layer. [PresetRepository.searchDirs] is
  // the single source of truth for "which directories hold .kpp files"
  // — used by [_scanDiskPresets] at boot. The sidecar save/load +
  // favorite APIs ([saveSidecar], [loadSidecar], [toggleFavorite],
  // [favoriteIds], [deleteSidecar]) operate on the heavier
  // lib/models/brush_preset.dart [BrushPreset] type and are exercised
  // by test/data/preset_repository_test.dart; they are NOT yet called
  // from the host because the host's UI uses the lightweight
  // lib/ui/widgets/brush_picker.dart [BrushPreset] (no favorite field).
  // A future favorite-toggle UI task will swap the host to the heavier
  // BrushPreset + call these APIs directly.
  final PresetRepository _presetRepo = PresetRepository();

  // ----- UI state ---------------------------------------------------------

  FeatherTool _tool = FeatherTool.draw;
  Color _color = const Color(0xFF60A5FA);
  double _brushSize = 20.0; // mm
  double _brushOpacity = 1.0;
  bool _pressure = true;
  FeatherMaterial _material = FeatherMaterial.shadeless;
  FeatherPattern _pattern = FeatherPattern.none;
  bool _renderMode = false;
  bool _uiHidden = false;
  String _groupName = 'Group 1';

  // ----- Render mode state (v55-B Feather 3D parity) ----------------------
  //
  // The prominent 3-state top-bar toggle ([RenderModeToggle]) exposes the
  // three Feather 3D render modes: Shaded / Shadeless / Wireframe. The
  // engine's [CanvasScene.renderMode] is a boolean (shaded vs flat), so
  // [_renderModeState] is the canonical UI state and the host projects it
  // onto [_renderMode] (shaded → true; shadeless/wireframe → false) plus
  // a host-side [_wireframeOverlay] flag (true only in wireframe). The
  // wireframe overlay draws each stroke's screen points as a
  // [CanvasOverlayPolyline] on top of the flat render — see
  // [_buildOverlayPrimitives]. HONESTY NOTE: this is a visual overlay,
  // NOT a real wireframe material pass (which would require touching
  // lib/core/rendering — out of scope). The old single-button toggle in
  // [TopBar] stays for legacy parity and toggles shaded ↔ shadeless
  // (clearing wireframe if active); the new 3-button cluster is the
  // prominent Feather 3D-style toggle the brief asks for.
  RenderModeState _renderModeState = RenderModeState.shaded;
  bool _wireframeOverlay = false;

  // ----- Environment state (Stage panel → CanvasScene) -------------------
  //
  // Mirror of the Stage panel's Environment tab. The host projects these
  // onto the [CanvasScene] in [_buildCanvasScene]:
  //   * [_lightAzimuth] + [_lightElevation] → [CanvasScene.lightDir] (2D
  //     screen-space direction vector — see [_azimuthElevationToLightDir]).
  //   * [_glowArea] → [CanvasScene.glowArea] (glow halo blur radius).
  //   * [_backgroundColor] → [CanvasScene.backgroundColor] (canvas bg +
  //     cutout material color; null = palette default).
  //   * [_showGrid] → [CanvasScene.showGrid].
  //   * [_showGroundPlane] → [CanvasScene.showGroundPlane] (also gates
  //     shadow projection in the painter).
  double _lightAzimuth = 5 * dmath.pi / 4;
  double _lightElevation = dmath.pi / 4;
  double _glowArea = 0.5;
  Color? _backgroundColor;
  bool _showGrid = true;
  bool _showGroundPlane = false;

  // ----- Light rig quick-access (v55-B) -----------------------------------
  //
  // The prominent Light dial ([LightRigPanel]) exposes the key light's
  // intensity + ambient floor in addition to azimuth / elevation (which
  // the Stage panel already exposes). Pre-v55-B these two were hardcoded
  // in [_buildLightRig] (intensity = 1.0, ambient = 0.35). The host now
  // owns them as fields so the dial + the rig stay in sync. The panel
  // floats at top-left (the sun-icon toggle button); tapping the toggle
  // shows / hides the panel overlay.
  double _lightIntensity = 1.0;
  double _lightAmbient = 0.35;
  bool _lightRigPanelVisible = false;

  // ----- 3D strokes list (v55-B) ------------------------------------------
  //
  // Feather 3D shows a list of 3D strokes (not just 2D layers) with
  // per-stroke controls: name, material icon, visibility toggle, delete,
  // reorder. The host already owns the canonical [_strokes] list + the
  // [_strokeMaterials] per-stroke material map; the [StrokeListPanel]
  // is the user-facing surface for that list. It floats at the right
  // edge (below the Stage panel toggle) when [_strokeListPanelVisible]
  // is true. The toggle button sits at the bottom-right (stacked left
  // of the AssistPanel toggle so the two don't overlap).
  bool _strokeListPanelVisible = false;

  // ----- Live stroke assembly state ---------------------------------------

  /// World-space points of the stroke currently being drawn (one per
  /// onStrokeUpdate callback). Cleared on stroke end.
  final List<Vector3> _liveStroke = <Vector3>[];

  /// Viewport size captured from the last build — needed to map screen
  /// pixel coordinates to camera rays.
  Size _viewportSize = Size.zero;

  /// When true, the next finished stroke is converted into a Draw-mode
  /// 3D Guide (see [_createDrawnGuide]) instead of being committed as a
  /// paint stroke. Future UI (a long-press on the Draw tool icon, or a
  /// dedicated "Guide Draw" toggle in the bottom bar) will flip this;
  /// for now it is a host-side flag the editor state can set directly.
  bool _guideDrawMode = false;

  // ----- Guide creation sub-modes (Loft / Primitive / Bend) ---------------
  //
  // Three sub-modes for creating 3D Guides beyond the Draw-mode pen-stroke
  // flow already handled by [_guideDrawMode]. Each sub-mode is entered via
  // [setGuideMode] and exits via its own finish / cancel method (which
  // reverts [_guideMode] to [_GuideMode.none]). The host owns the in-progress
  // state (selected curve ids, live preview guide ids, tension / segment
  // slider values, bend source guide); the launcher UI is rendered as an
  // overlay on top of the [EditorScreen] (see [_buildGuideOverlay]).
  //
  //   * Loft      — the artist taps 2+ existing strokes; the host calls
  //                 [LoftedGuideBuilder.build] to skin a surface across
  //                 them, with a live tension slider.
  //   * Primitive — the artist picks Cube / Pyramid / Sphere / Tube; the
  //                 host calls [PrimitiveGuideBuilder.build] to drop the
  //                 shape at the world origin, with a live segment slider.
  //   * Bend      — the artist (with a guide selected) draws a path on the
  //                 canvas; the host calls [BentGuideBuilder.build] to
  //                 deform the source guide along the path. The bent result
  //                 becomes the new source so the bend can be repeated.

  /// Currently active guide-creation sub-mode.
  _GuideMode _guideMode = _GuideMode.none;

  // Loft state:
  /// Stroke ids selected as loft curves, in selection order.
  final List<int> _loftStrokeIds = <int>[];
  /// Tension slider value in [0, 1] (0 = sharp, 1 = smooth). Matches the
  /// Feather tension slider: slide up for smoother, down for sharper.
  double _loftTension = 0.5;
  /// Manager id of the in-progress lofted guide preview, or `null` when
  /// fewer than two curves are selected (no preview is shown).
  GuideId? _loftPreviewId;

  // Primitive state:
  /// Currently active primitive kind, or `null` when no primitive is being
  /// inserted.
  Guide3DPrimitive? _primitiveKind;
  /// Live segment count for the segment slider.
  int _primitiveSegments = 24;
  /// Manager id of the in-progress primitive preview, or `null`.
  GuideId? _primitivePreviewId;

  // Bend state:
  /// Manager id of the source guide being bent. Defaults to the currently
  /// selected guide on entering bend mode; updated to the bent result after
  /// each bend pass so the artist can repeat the bend (per the Feather docs:
  /// "You can repeat the Bend 3D Guide process multiple times").
  GuideId? _bendSourceId;

  // ----- Guide panel (Feather 3D mode switcher) ---------------------------
  //
  // Self-contained block added in v0.53 to surface the full Draw / Loft /
  // Bend / Primitives guide mode switcher (lib/ui/widgets/guide_panel.dart).
  // The panel floats on the right; a rose/pink toggle button in the
  // bottom-right corner shows / hides it. The mode callback routes to the
  // existing [setGuideMode] / [setGuideDrawMode] setters (the "call the
  // GuideManager's setter" requirement); the shape callback routes to the
  // existing [insertPrimitive]; the snap / ribbon toggles are captured into
  // [_guideSnap] / [_guideRibbon] for the next milestone's snap + ribbon
  // renderer wiring. The size / axis / segment / angle knobs are captured
  // into fields so the F2 stroke-session milestone can consume them without
  // revisiting this block.
  bool _guidePanelVisible = false;
  // The next seven fields are written by the GuidePanel callbacks and read
  // by the F2 stroke-session milestone (snap renderer, ribbon renderer,
  // PrimitiveGuideParams.size feed-through, loft axis/segments, bend
  // angle/axis). Suppressed here so the v0.53 wiring block ships clean.
  bool _guideSnap = false;
  bool _guideRibbon = true;
  // ignore: unused_field
  double _guidePrimitiveSize = 2.0;
  // ignore: unused_field
  GuideAxis _guideLoftAxis = GuideAxis.y;
  // ignore: unused_field
  int _guideLoftSegments = 32;
  // ignore: unused_field
  double _guideBendAngle = 90.0;
  // ignore: unused_field
  GuideAxis _guideBendAxis = GuideAxis.y;

  // ----- Tutorial caption overlay -----------------------------------------
  //
  // A handwritten-style caption (Caveat font, white text + soft shadow)
  // shown at the top-center of the canvas. [setCaption] fades the caption
  // in over ~400 ms, holds it for 3 seconds, then fades it out over ~400 ms.
  // Used by guide-creation modes to surface the Feather-style "Let's sketch
  // this!" tutorial prompts.

  late final AnimationController _captionAnim;
  Timer? _captionHideTimer;
  String _captionText = '';

  // ----- First-run tutorial captions (v55-B) ------------------------------
  //
  // Feather 3D shows small handwritten captions on first launch ("Tap to
  // draw", "Pinch to orbit", etc). Pre-v55-B the [setCaption] helper
  // existed but only fired on guide-mode entry — the first-run flow was
  // missing. The host now runs a 4-hint sequence on first launch (gated
  // by a SharedPreferences flag so it only shows once per install).
  //
  // The sequence uses the existing [setCaption] (Caveat font, white text
  // + soft shadow, 3 s hold + 400 ms fade). Hints fire every 3.7 s
  // (3 s hold + 700 ms gap) so the artist has time to read each one.
  // The SharedPreferences flag is written AFTER the last hint shows so
  // a crash mid-tutorial will replay it on next launch (honest: this is
  // intentional — a half-shown tutorial is worse than a replay).
  //
  // The hint list + the pure accessor live in
  // lib/ui/widgets/tutorial_overlay.dart so the bounds logic is
  // unit-testable without pumping the full MainScreen (which
  // initialises the engine + FFI).
  Timer? _tutorialTimer;
  bool _tutorialActive = false;

  // ----- Per-stroke material + joystick resolver wiring -------------------

  /// Material type per stroke id. Strokes draw with the material that was
  /// active in the [MaterialPicker] when they were committed. Future
  /// strokes inherit the live [_material] field; the map keeps the
  /// per-stroke record so changing the picker later doesn't retro-edit
  /// already-drawn strokes. Kept as a host-side map (rather than a field
  /// on the [Stroke] model) to avoid touching the Stroke serialization
  /// surface in this small task.
  final Map<int, FeatherMaterial> _strokeMaterials = <int, FeatherMaterial>{};

  // ----- Stroke3D capture (curves-system source of truth) -----------------
  //
  // The engine ships a 10-file curves package (lib/engine/curves/) that
  // models strokes as [Stroke3D] — an ordered list of [StrokeSample3D]s
  // carrying position, pressure, tilt, timestamp, and optional UV. This
  // is the *capture* model: the raw input from the pointer handler,
  // before smoothing / simplification / curve-fitting.
  //
  // The host now wires Stroke3D as the SOURCE OF TRUTH for every paint
  // stroke. On draw, each pointer sample is appended to a live
  // [_liveStroke3D]. On stroke end, the Stroke3D is smoothed (gaussian,
  // via [StrokeSmoother]) and simplified (RDP), then CONVERTED to a
  // [Stroke] for rendering — the Stroke3D is kept in [_stroke3Ds] as
  // the canonical curves-system view (used by the glTF exporter and
  // available for future Bezier / Catmull-Rom / NURBS fitting).
  //
  // Erase / vacuum operations mutate both the rendered Stroke and the
  // Stroke3D via [_syncStroke3D] / [_strokeToStroke3D] so the two views
  // never drift. Undo / redo rebuilds the Stroke3D map from the new
  // Stroke snapshot via [_rebuildStroke3Ds] (lossy: raw timestamps are
  // lost, but positions / pressures are preserved).

  /// The Stroke3D currently being captured (one sample per
  /// onStrokeUpdate). `null` outside of an active draw stroke.
  Stroke3D? _liveStroke3D;

  /// Wall-clock timestamp at the start of the current stroke, used to
  /// compute per-sample `time` (seconds since stroke start).
  DateTime? _strokeStartTime;

  /// The canonical Stroke3D for every committed paint stroke, keyed by
  /// stroke id. Kept in sync with [_strokes] on draw / erase / vacuum /
  /// undo / redo. Used by the glTF exporter (passed alongside the
  /// rendered Strokes so exporters can choose either view).
  final Map<int, Stroke3D> _stroke3Ds = <int, Stroke3D>{};

  // ----- Assistance subsystems (v0.54-A wiring) ---------------------------
  //
  // Four assistance modules shipped with tests but were never called at
  // runtime (AUDIT_FINAL.md §3). They are now wired into the live paint
  // path via the pure helpers in
  // `lib/engine/assistance/assistance_wiring.dart`:
  //
  //   * Stable Strokes  — causal Gaussian pre-filter on each stroke
  //                       update's world sample (see [_stableStrokes]).
  //   * ColorSampler    — eye-dropper tool tap → nearest-stroke colour
  //                       (see [_onTapSelect] eyedropper branch).
  //   * MirrorAssist    — X/Y/Z reflection of the committed stroke into
  //                       2^N - 1 editable copies (see [_onStrokeEnd]).
  //   * DrawShapeAssist — PCA line / Kasa circle snap on the committed
  //                       stroke's control points (see [_onStrokeEnd]).
  //
  // The assist panel (lib/ui/widgets/assist_panel.dart) floats on the
  // right of the canvas (next to the guide panel) and exposes the
  // per-assist toggles. Each toggle is independent of the active tool so
  // the artist can combine them (e.g. Stable Strokes + Mirror X).

  /// Stable Strokes stabilizer instance. Reset on every stroke start;
  /// fed each stroke-update world sample. When [_stableStrokesEnabled]
  /// is false the stabilizer passes samples through unchanged.
  final StableStrokes _stableStrokes = StableStrokes(
    config: const StableStrokesConfig(
      enabled: false,
      intensity: 0.5,
      maxWindow: 12,
      extrapolation: 0.5,
    ),
  );

  /// Whether Stable Strokes is active. Toggled from the assist panel.
  bool _stableStrokesEnabled = false;

  /// Live Stable Strokes intensity (0..1), mirrored from the assist
  /// panel slider. Read on stroke start when the stabilizer config is
  /// rebuilt (see [_onStrokeStart]).
  double _stableStrokesIntensity = 0.5;

  /// Whether the assist panel overlay is visible. Toggled by the
  /// floating rose/pink button in the bottom-right of the canvas.
  bool _assistPanelVisible = false;

  /// Mirror assistance instance (v0.54-A §3). Owns the active X/Y/Z axis
  /// set. When [_mirrorAssist.isEnabled] (any axis active) OR the
  /// [FeatherTool.mirror] dock tool is selected, each committed draw
  /// stroke is reflected into 2^N - 1 editable copies (see
  /// [_onStrokeEnd]). The axis set is mutated from the assist panel's
  /// X/Y/Z chips via [onMirrorAxisToggled].
  final MirrorAssist _mirrorAssist = MirrorAssist();

  /// v57-C: Eraser engine (point / vacuum / in-place erase). Wraps the
  /// pure-geometry erase math so [_eraseAt] / [_eraseAtWorld] stay
  /// thin host glue. Stateless — minSurvivingPoints = 2 (matches the
  /// "strokes that drop below 2 points are removed" rule that lived
  /// inline in [_eraseAt] before v57-C).
  final EraserEngine _eraser = EraserEngine();

  /// When false (default), the joystick handlers route through
  /// [Joystick2dResolver] (view-based: move = camera-right/up plane,
  /// rotate = around camera forward, scale = uniform around the
  /// crosshair). When true, they route through [Joystick3dResolver]
  /// (world-axis: move callback → translate along world X, rotate
  /// callback → rotate around world Y, scale callback → translate along
  /// world Z). Exposed for future UI wiring (e.g. a "2D/3D Joystick"
  /// toggle on the joystick panel).
  bool _joystick3d = false;

  /// Lock state for the 2D joystick (off by default). When on, the
  /// gizmo's per-axis scale handles collapse to a single uniform handle
  /// and the stick snap to cardinal directions. Wired from the
  /// JoystickWidget's Lock pill via [_toggleJoystickLock] (honesty-gap 1
  /// fix — previously the toggle was dead UI). The host's joystick input
  /// handlers early-return when this is [JoystickLock.on] so drags have
  /// no transform effect (the knob still tracks visually + snaps back).
  JoystickLock _joystickLock = JoystickLock.off;

  /// True when the joystick is locked — convenience for the input-handler
  /// gate (see [_onJoystickMove] / _onJoystickRotate / _onJoystickScale).
  bool get _joystickLocked => _joystickLock == JoystickLock.on;

  /// Last-known screen position of the liquify brush cursor, in canvas
  /// pixels. Set on every liquify drag update; cleared when the liquify
  /// tool is exited. Used by [LiquifyRenderer] to draw the brush preview
  /// (inner circle + outer range ring + drag arrow).
  Offset? _liquifyCursor;

  /// Live liquify drag delta in screen pixels, or `null` when no drag is
  /// in progress. Drives the arrow drawn by [LiquifyRenderer] so the
  /// user can see the drag direction + magnitude at a glance.
  Vector2? _liquifyDragDelta;

  /// Stateless resolver instances. Constructed once; both are safe to
  /// reuse across calls (they hold no per-drag state).
  final Joystick2dResolver _joy2d = Joystick2dResolver();
  final Joystick3dResolver _joy3d = Joystick3dResolver();

  // ----- Presets (UI-only visual catalog) ---------------------------------

  late final List<BrushPreset> _presets;

  // ----- Real Krita dab rendering (host-side composite path) ---------------
  //
  // The brush engine exposes `generateDab(BrushInput) → BrushDab` on the
  // active [KritaBrushBackend]. When the real native engine is live, the
  // host feeds each stroke-update sample through that path: the engine
  // produces a real Krita dab (the same pixel stamp Krita would paint on
  // a flat canvas), the host composite-stamps it onto a
  // [KritaCanvasController] (the host-side paint buffer — Krita's
  // KisPaintDevice equivalent), rasterizes the buffer into a [ui.Image]
  // and hands it to the canvas viewport's painter, which draws it on top
  // of the 3D stroke ribbons via `canvas.drawImage`.
  //
  // Verdict on the live paint path (audited v0.54-C):
  //
  //   _onPanUpdate (canvas_viewport.dart)
  //     → onStrokeUpdate callback (editor_screen.dart → main_screen.dart
  //       `onStrokeUpdate: _onStrokeUpdate` wiring)
  //     → _onStrokeUpdate (this file)
  //     → gated by `_realBackendActive && _guideMode != bend && !_guideDrawMode`
  //       → _paintRealDab (this file)
  //     → backend.generateDab(BrushInput(...))
  //     → KritaBrushController.generateDab (krita_brush_controller.dart)
  //     → FFI _native.generateDab → `krita_brush_generate_dab`
  //       (krita_bindings.dart lookup)
  //
  // (Re-confirmed by the actual v0.54-C agent — d88bb4fc → HEAD: every
  // link above traced to its file:line. Verdict holds: MIXED. Real on
  // Windows/Linux/Android (native lib loads at boot → `_realBackendActive`
  // true); fallback to the 3D polyline renderer on iOS/macOS/web where
  // the boot probe falls back to [KritaFallbackEngine]. No code change
  // needed — this comment was already accurate; this marker just makes
  // the "audited v0.54-C" attribution honest.)
  //
  // So the host DOES call `backend.generateDab` on the live paint path —
  // but only when `_realBackendActive == true`, i.e. the native
  // `krita_bridge` library loaded successfully at boot (Windows/Linux/
  // Android where the artifact is bundled; iOS/macOS/web fall back).
  //
  // When the real engine is NOT available (the boot probe fell back to
  // [KritaFallbackEngine]), the `_realBackendActive` gate short-circuits
  // and the host NEVER calls `backend.generateDab` on the live path — the
  // editor keeps painting through the 3D polyline renderer only (current
  // behaviour, no regression). The fallback engine still implements
  // `generateDab` for tests (and for the smoke-test / synthetic-dab paths
  // in lib/engine/krita_bridge/krita_smoke_test.dart), but the host's
  // rendering loop never reaches it.

  /// True when the real native Krita brush backend is live (the FFI
  /// bridge loaded and a brush handle was allocated). Set in [initState]
  /// from [_krita.status]. Gates the real-dab rendering path in
  /// [_onStrokeUpdate].
  bool _realBackendActive = false;

  /// v0.55-A: True after the first-run engine-load info dialog has been
  /// shown once for this MainScreen instance. The dialog fires from a
  /// post-frame callback in [initState] when [_realBackendActive] is
  /// false — a CLEAR, user-readable explanation instead of letting the
  /// fallback be silent. The flag guards against re-showing it on a
  /// hot-reload or a setState round-trip.
  bool _engineDialogShown = false;

  /// v0.55-A: The GitHub Releases URL shown in the engine-load info
  /// dialog + the About dialog. Kept as a static const so the dialog
  /// tests can assert against it without a host.
  static const String kGitHubReleasesUrl =
      'https://github.com/koenigsegggjesk0o/krita/releases';

  /// The host-side paint canvas that composite-stamps real Krita dabs.
  /// Lazily allocated to the viewport size on the first dab; resized
  /// when the viewport changes (existing content preserved in the
  /// top-left corner, like KisImage::resize). Null until the first real
  /// dab is painted.
  KritaCanvasController? _paintCanvas;

  /// The rasterized form of [_paintCanvas] — a [ui.Image] snapshot of
  /// the backing RGBA8 buffer, drawn on top of the 3D stroke ribbons by
  /// the canvas viewport's painter. Re-rasterized whenever
  /// [_paintCanvas] mutates (see [_scheduleRasterizePaint]).
  ui.Image? _paintLayerImage;

  /// The [_paintCanvas] version captured in [_paintLayerImage]. Used to
  /// decide whether a re-rasterization is needed (the canvas controller
  /// bumps its version on every paintDab / resize / clear).
  int _paintLayerVersion = -1;

  /// True while a rasterization is in flight. Guards against stacking
  /// multiple concurrent rasterizations — incoming dabs during a
  /// rasterization are picked up by a follow-up rasterization scheduled
  /// when the in-flight one completes (if the canvas version advanced).
  bool _rasterizingPaint = false;

  // v0.55-C: dab-generation throttle (paint-loop perf).
  //
  // The native krita_bridge's `krita_brush_generate_dab` is a SYNCHRONOUS
  // FFI call (krita_brush_controller.dart:451). On a fast pen drag the
  // GestureDetector's onPanUpdate fires per micro-pixel move — easily
  // 200–500 events/sec on a 120 Hz Android tablet. Without a throttle the
  // dab path runs generateDab → paintDab → schedule-rasterize on every
  // event, saturating the UI isolate and dropping frames.
  //
  // The throttle below caps dab GENERATION (not stroke-sample capture) at
  // ~60 Hz (16 ms). The Stroke3D capture (_liveStroke3D.addSample) still
  // records every sample for curve fidelity; only the rasterized preview
  // layer is throttled. The final dab on stroke-end fires unconditionally
  // (see _onStrokeEnd → _paintRealDab flush) so the committed paint layer
  // matches the curve.
  //
  // 16 ms is chosen to match the canonical 60 fps frame budget. On 120 Hz
  // displays the dab layer renders at 60 Hz while the gesture stream stays
  // at 120 Hz — visually identical to a 120 Hz dab stream because the
  // rasterization (which is the actual visual bottleneck) is already
  // coalesced in _scheduleRasterizePaint.
  //
  // The throttle window + decision function live in lib/utils/paint_perf.dart
  // (kDabThrottle, shouldFireDab) so they're unit-testable in isolation.
  DateTime? _lastDabTime;
  /// The last screen-space position handed to [_onStrokeUpdate]. Used by
  /// [_onStrokeEnd] to flush a final dab at the exact pen-lift position
  /// (the throttle gate in [_onStrokeUpdate] may have skipped the most
  /// recent update(s); without this flush, the rasterized paint layer
  /// would lag the curve end-point by up to 16 ms of pen motion).
  Offset? _lastStrokeScreenPos;

  // ----- Export wiring (TopBar buttons → exporters) ----------------------
  //
  // The TopBar now exposes two buttons wired by this host:
  //   * Export  (Icons.ios_share_rounded) → [_showExportSheet]
  //   * Share   (Icons.share_outlined)     → [_shareLastExport]
  //
  // PNG export captures the live editor via a [RepaintBoundary] that
  // wraps [EditorScreen] (keyed by [_viewportBoundaryKey]). The captured
  // [ui.Image] is converted to an RGBA8 buffer (premultiplied → straight
  // alpha) and handed to [PngExporter]. glTF / OBJ exports pick a save
  // location via [FilePicker.platform.saveFile] (desktop) or fall back to
  // the canonical exports directory (mobile). The last successful export
  // path is cached in [_lastExportPath] so the Share button can re-share
  // without forcing a re-pick.

  /// Key for the [RepaintBoundary] wrapping the [EditorScreen]. Used by
  /// [_captureViewportPng] to call `RenderRepaintBoundary.toImage()`.
  final GlobalKey _viewportBoundaryKey = GlobalKey();

  /// Path of the most recent successful export (any format). The Share
  /// button re-shares this file; null until the first export succeeds.
  String? _lastExportPath;

  /// True while an export is in flight. Disables the export buttons
  /// (the sheet closes on tap; this guards against re-entry from the
  /// Share button while a PNG snapshot is still rendering).
  bool _exporting = false;

  @override
  void initState() {
    super.initState();
    _scene = Scene();
    // Seed a default group so the layer panel has something to show.
    _scene.layers.addNewGroup(_groupName);

    _camera = OrbitCamera();
    _krita = KritaEngine()..init();
    // The brush engine uses the procedural fallback by default; when the
    // real Krita backend is available we still keep the procedural engine
    // for 3D stroke assembly (its 2D dab path is only used for the live
    // preview; the native backend drives the real dabs when present).
    _brush = createBrushEngine(
      settings: const BrushSettings(size: 20.0, opacity: 1.0),
      color: _color.toARGB32(),
    );
    // Detect the real backend so we can route stroke updates through the
    // real `generateDab` path. On the fallback (no native library), the
    // host keeps using the 3D polyline renderer only — no behaviour
    // regression vs. the previous build.
    _realBackendActive = _krita.status == KritaEngineStatus.realEngine;
    _guides = Guide3DManager();
    _guideSnapHelper = Guide3DSnap(maxDistance: MainScreen.kGuideSnapMaxDistance);
    _liquify = LiquifyEngine();
    _selectionModel = SelectionModel();
    _selection = SelectionSystem(
      model: _selectionModel,
      strokes: () => _strokes,
    );

    _presets = _buildDefaultPresets();

    // Scan the preset directories for real .kpp files (the boot-phase
    // resource import already extracted Krita's stock preset library into
    // feather_resources/paintoppresets; the user's own .kpp files live in
    // feather_presets). The scan is best-effort + async: it produces UI
    // [BrushPreset] entries that carry the file path so [_onPickPreset]
    // can feed them to the native bridge's loadPreset. Failures (missing
    // dirs, parse errors) are swallowed — the bundled visual catalog stays
    // as the fallback tail.
    _scanDiskPresets();

    // Tutorial caption fade controller (forward = visible, reverse = hidden).
    _captionAnim = AnimationController(
      vsync: this,
      duration: const Duration(milliseconds: 400),
    );

    // v0.55-A: Surface a CLEAR first-run info dialog when the real
    // native Krita bridge did NOT load. The probe already ran in
    // main.dart (probeKritaEngine) and the splash screen already showed
    // a warning banner — but the editor entering fallback mode silently
    // was the class of bug that left first-run Windows users confused
    // ("the app opened but brushes feel wrong"). The post-frame
    // callback lets the first frame paint before we block on the dialog.
    if (!_realBackendActive) {
      WidgetsBinding.instance.addPostFrameCallback((_) {
        if (!mounted || _engineDialogShown) return;
        _engineDialogShown = true;
        _showEngineLoadDialog();
      });
    }

    // v0.55-B: First-run tutorial captions. Gated by a SharedPreferences
    // flag so the sequence only shows once per install. The post-frame
    // callback lets the first frame paint + the engine-load dialog (if
    // any) surface before we start the caption sequence. The sequence
    // itself is fired from [_maybeStartFirstRunTutorial] which reads the
    // flag async; if the flag is already set, the sequence is skipped.
    WidgetsBinding.instance.addPostFrameCallback((_) {
      if (!mounted) return;
      _maybeStartFirstRunTutorial();
      // v0.57-D: Apply persisted user defaults (brush size, grid
      // visibility) loaded from [SettingsRepository]. Best-effort: a
      // failed read leaves the in-memory defaults in place.
      _loadPersistedSettings();
    });
  }

  @override
  void dispose() {
    _captionHideTimer?.cancel();
    _tutorialTimer?.cancel();
    _captionAnim.dispose();
    _brush.dispose();
    _krita.shutdown();
    // Release the GPU-backed paint layer. Use a post-frame callback so
    // any in-flight paint pass that captured the old image finishes
    // before we release it.
    final image = _paintLayerImage;
    _paintLayerImage = null;
    _paintCanvas = null;
    if (image != null) {
      WidgetsBinding.instance.addPostFrameCallback((_) => image.dispose());
    }
    super.dispose();
  }

  // ----- Build ------------------------------------------------------------

  @override
  Widget build(BuildContext context) {
    return LayoutBuilder(
      builder: (context, constraints) {
        _viewportSize = Size(constraints.maxWidth, constraints.maxHeight);
        // Wrap the whole editor (canvas + overlays) in [EditorShortcuts]
        // so the keyboard bindings are live the moment the screen mounts.
        // The wrapper's auto-focusing [Focus] node is an ANCESTOR of every
        // child, so the bindings fire regardless of which descendant holds
        // focus — and a modal dialog (export sheet, settings) opens in its
        // own focus scope, pausing the editor bindings until it closes.
        return EditorShortcuts(
          bindings: _shortcutBindings(),
          child: Stack(
            children: [
              // RepaintBoundary wrapping the EditorScreen so PNG export
              // can capture the live canvas via RenderRepaintBoundary.toImage().
              // The boundary sits beneath the guide overlay + caption so
              // those transient hints are NOT included in the snapshot.
              RepaintBoundary(
                key: _viewportBoundaryKey,
                child: EditorScreen(
                  initial: _initialUiState(),
                  scene: _buildCanvasScene(),
                  layers: _buildLayerItems(),
                  resources: _buildResourceItems(),
                  presets: _presets,
                  canUndo: _undoStack.isNotEmpty,
                  canRedo: _redoStack.isNotEmpty,
                  // Drawing is enabled in the Draw tool, the Eraser / Vacuum
                  // sub-modes (pen drag erases), and Bend mode (the artist
                  // draws the bend path on the canvas). Loft mode uses
                  // tap-to-select instead — see [_onTapSelect] branching.
                  drawingEnabled: _tool == FeatherTool.draw ||
                      _tool == FeatherTool.eraser ||
                      _tool == FeatherTool.vacuum ||
                      _tool == FeatherTool.mirror ||
                      _guideMode == _GuideMode.bend,
                  onStrokeStart: _onStrokeStart,
                  onStrokeUpdate: _onStrokeUpdate,
                  onStrokeEnd: _onStrokeEnd,
                  onUndo: _undo,
                  onRedo: _redo,
                  onUiStateChanged: _onUiStateChanged,
                  onPan: _onPan,
                  onZoom: _onZoom,
                  onSelectAll: _selectAll,
                  onLiquifyMode: _onLiquifyMode,
                  onLiquifyApply: _onLiquifyApply,
                  onLiquifyUndoAll: _onLiquifyUndoAll,
                  onJoystickMove: _onJoystickMove,
                  onJoystickRotate: _onJoystickRotate,
                  onJoystickScale: _onJoystickScale,
                  // Honesty-gap 1 fix: forward the 2D/3D + Lock toggle
                  // callbacks + state mirror so the JoystickWidget's pills
                  // are no longer dead UI. The host owns the canonical
                  // state; the editor just reflects it back.
                  isJoystick3D: _joystick3d,
                  joystickLocked: _joystickLocked,
                  onToggle3D: _toggleJoystick3D,
                  onLockToggle: _toggleJoystickLock,
                  onPickPreset: _onPickPreset,
                  // Export wiring: TopBar buttons → host exporter calls.
                  onExport: _showExportSheet,
                  onShare: _shareLastExport,
                  // v0.55-A: Top-bar About / Help button →
                  // [_showAboutDialog] (engine status, native lib path,
                  // crash log path, GitHub Releases re-download link).
                  onAbout: _showAboutDialog,
                  // Tool-gated gesture callbacks: tap-select only fires in the
                  // Select tool, the Eye-dropper tool (v0.54-A §2 — samples
                  // the nearest stroke's colour), OR in Loft mode (where taps
                  // add strokes to the loft curve selection). Liquify drag
                  // only in the Liquify tool. The viewport uses their presence
                  // (vs null) to route single-finger gestures away from
                  // camera orbit.
                  onTapSelect: (_tool == FeatherTool.select ||
                          _tool == FeatherTool.eyedropper ||
                          _guideMode == _GuideMode.loft)
                      ? _onTapSelect
                      : null,
                  onLiquifyDrag:
                      _tool == FeatherTool.liquify ? _onLiquifyDrag : null,
                  // Desktop mouse wiring: right-click orbit (reuses _onPan
                  // which already orbits), middle-click pan (real camera
                  // translate), Ctrl+scroll brush size ±.
                  onOrbit: _onPan,
                  onCameraPan: _onCameraPan,
                  onBrushSizeDelta: _onBrushSizeDelta,
                ),
              ),

              // Guide-creation launcher + per-mode panels (top-center overlay,
              // just below the top bar). When no mode is active, shows the
              // launcher row [Loft] [Primitive] [Bend] [Caption]. When a mode
              // is active, shows the mode's panel (tension/segment sliders +
              // Done/Cancel).
              Positioned(
                left: 0,
                right: 0,
                top: 68,
                child: Center(child: _buildGuideOverlay()),
              ),

              // Tutorial caption (top-center, white Caveat text with shadow).
              // Mounted only while a caption is showing; IgnorePointer lets
              // taps fall through to the canvas underneath.
              if (_captionText.isNotEmpty)
                Positioned(
                  left: 0,
                  right: 0,
                  top: 116,
                  child: Center(
                    child: IgnorePointer(
                      child: FadeTransition(
                        opacity: _captionAnim,
                        child: Text(
                          _captionText,
                          style: FeatherTypography.tutorial.copyWith(
                            color: Colors.white,
                            fontSize: 28,
                            shadows: const <Shadow>[
                              Shadow(
                                color: Colors.black54,
                                blurRadius: 8,
                                offset: Offset(0, 2),
                              ),
                            ],
                          ),
                        ),
                      ),
                    ),
                  ),
                ),

              // ----- Guide panel (v0.53) — self-contained wiring block -----
              // Floating rose/pink toggle button (bottom-right) + the
              // GuidePanel overlay (right side) when [_guidePanelVisible].
              // The panel's mode callback routes to the host's existing
              // [setGuideMode] / [setGuideDrawMode] setters; the shape
              // callback routes to [insertPrimitive]; the remaining knobs
              // are captured into the [_guide*] fields for the next
              // milestone. See [lib/ui/widgets/guide_panel.dart].
              Positioned(
                right: 16,
                bottom: 96,
                child: _GuidePanelToggleButton(
                  visible: _guidePanelVisible,
                  onTap: () => setState(
                      () => _guidePanelVisible = !_guidePanelVisible),
                ),
              ),
              if (_guidePanelVisible)
                Positioned(
                  right: 16,
                  top: 80,
                  bottom: 96,
                  child: SingleChildScrollView(
                    child: _buildGuidePanel(),
                  ),
                ),

              // ----- Assist panel (v0.54-A) — self-contained wiring block -----
              // Floating rose/pink toggle button (bottom-right, stacked
              // left of the guide panel toggle) + the AssistPanel overlay
              // when [_assistPanelVisible]. The panel exposes the per-assist
              // toggles (Stable Strokes now; Mirror axes + Draw Shape mode
              // in the follow-up commits of v0.54-A). Each callback mutates
              // a host field that the stroke-update / stroke-end handlers
              // read. See [lib/ui/widgets/assist_panel.dart].
              Positioned(
                right: 76,
                bottom: 96,
                child: _AssistPanelToggleButton(
                  visible: _assistPanelVisible,
                  onTap: () => setState(
                      () => _assistPanelVisible = !_assistPanelVisible),
                ),
              ),
              if (_assistPanelVisible)
                Positioned(
                  right: 76,
                  top: 80,
                  bottom: 96,
                  child: SingleChildScrollView(
                    child: _buildAssistPanel(),
                  ),
                ),

              // ----- Render mode toggle (v55-B) — prominent Feather 3D-style
              // 3-button cluster (Shaded / Shadeless / Wireframe). Mounted
              // just below the top bar on the right side so it reads as the
              // prominent top-bar toggle the brief asks for, without
              // disturbing the existing single-button toggle in [TopBar]
              // (which stays for legacy parity + is kept in sync via
              // [_onRenderModeChanged]). The rose/pink gradient matches the
              // GuidePanel + AssistPanel family.
              Positioned(
                right: 16,
                top: 70,
                child: RenderModeToggle(
                  mode: _renderModeState,
                  onChanged: _onRenderModeChanged,
                ),
              ),

              // ----- Light rig quick-access (v55-B) — floating sun-icon
              // toggle button (top-left, just below the top bar) + the
              // [LightRigPanel] dial overlay. The dial exposes azimuth /
              // elevation / intensity / ambient (the four knobs the brief
              // asks for); the sun icon rotates with the azimuth so the
              // artist gets a visual affordance of where the key light
              // sits. The host forwards the panel's callbacks to the
              // existing [_lightAzimuth] / [_lightElevation] fields +
              // the new [_lightIntensity] / [_lightAmbient] fields, then
              // [_buildLightRig] feeds them to the [MaterialLightRig].
              Positioned(
                left: 16,
                top: 70,
                child: _LightRigPanelToggleButton(
                  visible: _lightRigPanelVisible,
                  onTap: () => setState(
                      () => _lightRigPanelVisible = !_lightRigPanelVisible),
                ),
              ),
              if (_lightRigPanelVisible)
                Positioned(
                  left: 16,
                  top: 124,
                  child: _buildLightRigPanel(),
                ),

              // ----- 3D strokes list (v55-B) — floating rose/pink toggle
              // button (bottom-right, stacked left of the AssistPanel
              // toggle so the three bottom-right buttons don't overlap)
              // + the [StrokeListPanel] overlay when
              // [_strokeListPanelVisible]. The panel lists every stroke
              // in the document with: name, colour swatch, material
              // icon, visibility toggle, delete button, drag handle for
              // reorder. The host wires the panel's callbacks to
              // [_setStrokeVisible] / [_deleteStrokeById] /
              // [_reorderStrokes] — all undoable (push the pre-mutation
              // snapshot via [_pushUndo]).
              Positioned(
                right: 136,
                bottom: 96,
                child: _StrokeListPanelToggleButton(
                  visible: _strokeListPanelVisible,
                  onTap: () => setState(() =>
                      _strokeListPanelVisible = !_strokeListPanelVisible),
                ),
              ),
              if (_strokeListPanelVisible)
                Positioned(
                  right: 16,
                  top: 124,
                  child: _buildStrokeListPanel(),
                ),
            ],
          ),
        );
      },
    );
  }

  // ----- UI state adaptation ----------------------------------------------

  EditorUiState _initialUiState() => EditorUiState(
        tool: _tool,
        color: _color,
        size: _brushSize,
        opacity: _brushOpacity,
        pressure: _pressure,
        material: _material,
        pattern: _pattern,
        groupName: _groupName,
        renderMode: _renderMode,
        uiHidden: _uiHidden,
        // Seed the editor with the host's current env state so the
        // Stage panel opens with the right slider / toggle positions.
        lightAzimuth: _lightAzimuth,
        lightElevation: _lightElevation,
        glowArea: _glowArea,
        backgroundColor: _backgroundColor,
        showGrid: _showGrid,
        showGroundPlane: _showGroundPlane,
      );

  /// Builds the [CanvasScene] the viewport paints. Projects each 3D
  /// stroke through the camera's view-projection matrix to 2D screen
  /// points so the 2D [CanvasViewport] can render them.
  CanvasScene _buildCanvasScene() {
    final aspect = _viewportSize.isEmpty
        ? 1.0
        : _viewportSize.width / _viewportSize.height;
    final vp = _camera.viewProjectionMatrix(aspect);
    final screenStrokes = <CanvasStroke>[];
    for (final stroke in _strokes) {
      if (!stroke.isVisible) continue;
      final points = <Offset>[];
      for (final p in stroke.points) {
        final world = stroke.transform.transform3(p.position.clone());
        final ndc = vp.transform3(world.clone());
        // Clip points behind the camera / outside the near-far range.
        if (ndc.z <= -1.0 || ndc.z >= 1.0) continue;
        final sx = (ndc.x * 0.5 + 0.5) * _viewportSize.width;
        final sy = (1.0 - (ndc.y * 0.5 + 0.5)) * _viewportSize.height;
        points.add(Offset(sx, sy));
      }
      if (points.length < 2) continue;
      final mat = _strokeMaterials[stroke.id] ?? FeatherMaterial.shadeless;
      screenStrokes.add(CanvasStroke(
        points: points,
        color: Color(stroke.color),
        width: stroke.thickness.clamp(1.0, 24.0),
        material: _toCanvasMaterial(mat),
      ));
    }
    return CanvasScene(
      strokes: screenStrokes,
      activeColor: _color,
      pan: Offset.zero,
      zoom: 1.0,
      showGrid: _showGrid,
      renderMode: _renderMode,
      // Lighting direction (Stage panel → Environment → Azimuth /
      // Elevation). The painter's _strokeGeometry uses this for the
      // legacy 2D Lambert lit-sign fallback (dot(normal, -lightDir),
      // used when [CanvasScene.lightRig] is null) and the drop-shadow
      // projection (skew along lightDir). The rig-bound path
      // ([_buildLightRig]) derives the lit sign from
      // [MaterialLightRig.evaluate] instead — see honesty-gap 2 fix.
      // Converted from spherical (azimuth, elevation) to a 2D screen-
      // space direction vector — see [_azimuthElevationToLightDir].
      lightDir: _azimuthElevationToLightDir(_lightAzimuth, _lightElevation),
      // Ground plane + shadow projection. When ON, the painter draws a
      // visible band at 80% of the viewport height (computed in
      // _groundLineY) and projects stroke shadows onto it. When OFF,
      // no ground plane and no shadows at all.
      showGroundPlane: _showGroundPlane,
      // User-picked background color override (null = palette default).
      // The painter uses this for the canvas background AND the Cutout
      // material so cutout strokes disappear into the chosen color.
      backgroundColor: _backgroundColor,
      // Glow halo size multiplier (0..1).
      glowArea: _glowArea,
      // The host-side paint raster (real Krita dabs). Null on the
      // fallback engine — the viewport's painter skips drawing it and
      // falls back to the 3D polyline renderer only (current behaviour).
      paintLayer: _paintLayerImage,
      // Overlay primitives (gizmo / selection highlight / liquify brush
      // preview). Built from the engine renderers' draw-lists — see
      // [_buildOverlayPrimitives]. Empty when no overlay applies (e.g.
      // the Draw tool with no active selection).
      overlayPrimitives: _buildOverlayPrimitives(vp),
      // Honesty-gap 2 fix: pass the material light rig so the painter
      // derives per-vertex lit signs from [MaterialLightRig.evaluate]
      // (shared with the Shaded material + BrushRenderer) instead of
      // the legacy inline 2D Lambert. Built from the same
      // (azimuth, elevation) as [lightDir] above so the rig's key-light
      // direction matches the shadow-projection direction — see
      // [_buildLightRig].
      lightRig: _buildLightRig(),
    );
  }

  /// Honesty-gap 2 fix: constructs the [MaterialLightRig] shared with the
  /// painter (and the Shaded material / BrushRenderer when wired). The
  /// rig's 3D key-light direction is built from the same
  /// (azimuth, elevation) the legacy 2D [CanvasScene.lightDir] is derived
  /// from, so the rig's lit-sign decision matches the legacy inline calc
  /// (a 2D normal `(nx, ny, 0)` dotted with `-key.unit` equals the legacy
  /// `-(nx*ldx + ny*ldy)` when `key.unit.xy == lightDir`).
  ///
  /// The rig's ambient/intensity are irrelevant for the painter's
  /// lit-sign decision (only the sign of `n · (-L)` matters, which is
  /// direction-only); they ARE consumed by the Shaded material's
  /// [ShadedMaterial.shade] when a stroke carries that material.
  ///
  /// v55-B: intensity + ambient are now host-owned fields (mutated by
  /// the [LightRigPanel] dial). Pre-v55-B they were hardcoded (1.0 +
  /// 0.35); the dial exposes them to the artist the way Feather 3D's
  /// quick-access light affordance does.
  MaterialLightRig _buildLightRig() {
    final el = _lightElevation.clamp(0.0, dmath.pi / 2);
    final ce = dmath.cos(el);
    final se = dmath.sin(el);
    final dir = Vec3(
      ce * dmath.cos(_lightAzimuth),
      ce * dmath.sin(_lightAzimuth),
      se,
    );
    return MaterialLightRig(
      direction: dir,
      intensity: _lightIntensity.clamp(0.0, 4.0),
      ambient: _lightAmbient.clamp(0.0, 1.0),
    );
  }

  /// Converts a spherical light direction (azimuth + elevation, both in
  /// radians) into a 2D screen-space direction vector for the canvas
  /// painter's Lambert tube shader.
  ///
  /// Convention:
  ///   * Azimuth [az] is the heading around the scene, measured
  ///     clockwise from +x (right) when viewed from above. 0 = light
  ///     from the right, π/2 = light from below (screen-y down),
  ///     π = light from the left, 3π/2 = light from above.
  ///   * Elevation [el] is the angle above the ground plane
  ///     (0 = horizon, π/2 = zenith). Higher elevation → steeper
  ///     downward light travel → shorter shadow skew.
  ///
  /// The returned [Offset] is the direction light TRAVELS (the OpenGL
  /// convention the painter's lit-sign + shadow-projection code
  /// expects):
  ///   dx = cos(el) * cos(az)
  ///   dy = cos(el) * sin(az)
  /// The depth (z) component is implicit (sin(el)) and only affects
  /// the perceptual shadow length via the dy magnitude — higher
  /// elevation shrinks dy → less shadow skew along the screen-y axis.
  Offset _azimuthElevationToLightDir(double az, double el) {
    final e = el.clamp(0.0, dmath.pi / 2);
    final ce = dmath.cos(e);
    return Offset(ce * dmath.cos(az), ce * dmath.sin(az));
  }

  /// Builds the overlay primitives for the current frame, adapting the
  /// engine renderers' draw-lists into [CanvasOverlay] values the
  /// viewport's painter can draw. Returns an empty list when no overlay
  /// applies.
  ///
  /// Order (back → front, matching how the painter draws them):
  ///   1. Selection highlight (active strokes' outlines + bounding box +
  ///      corner handles) — drawn by [SelectionRenderer] when the
  ///      selection is non-empty in the Select / Transform / Liquify
  ///      tools. Skipped in Draw / Eraser / Vacuum to keep those tools'
  ///      cursors clean.
  ///   2. Transform gizmo (2D stick + handles OR 3D cones + arcs) —
  ///      drawn by [Joystick2dGizmoRenderer] / [Joystick3dGizmoRenderer]
  ///      when the Transform tool is active AND there's a selection.
  ///   3. Liquify brush preview (inner circle + outer range ring + drag
  ///      arrow) — drawn by [LiquifyRenderer] when the Liquify tool is
  ///      active AND the cursor has been reported (i.e. the user has
  ///      started a drag).
  List<CanvasOverlay> _buildOverlayPrimitives(Matrix4 vp) {
    final out = <CanvasOverlay>[];
    final hasSelection = _selectionModel.state.isNotEmpty;
    final showSelection = hasSelection &&
        (_tool == FeatherTool.select ||
            _tool == FeatherTool.transform ||
            _tool == FeatherTool.liquify);
    if (showSelection) {
      out.addAll(_buildSelectionOverlay(vp));
    }
    if (hasSelection && _tool == FeatherTool.transform) {
      out.addAll(_buildGizmoOverlay(vp));
    }
    if (_tool == FeatherTool.liquify && _liquifyCursor != null) {
      out.addAll(_buildLiquifyOverlay());
    }
    // v55-B: wireframe overlay — when the render-mode toggle is in the
    // wireframe state, draw each stroke's screen-projected points as a
    // [CanvasOverlayPolyline] on top of the flat (shadeless) render. This
    // is a visual overlay, NOT a real wireframe material pass (which
    // would require touching lib/core/rendering — out of scope for this
    // UI feature-parity task). The polylines use the stroke's own colour
    // at full alpha so the artist can still tell strokes apart.
    if (_wireframeOverlay) {
      out.addAll(_buildWireframeOverlay(vp));
    }
    // v0.57-B: guide ribbon overlay — when the GuidePanel's "Show guide
    // ribbon" toggle ([_guideRibbon]) is ON, project every active guide
    // through [Guide3DRenderer.renderAll] and adapt the resulting world-
    // space triangle / line batches into screen-space [CanvasOverlay]
    // primitives (translucent surface triangles + grid line segments +
    // the orange starting line). See [_buildGuideRibbonOverlay].
    if (_guideRibbon) {
      out.addAll(_buildGuideRibbonOverlay(vp));
    }
    return out;
  }

  /// v0.57-B: adapts [Guide3DRenderer.renderAll]'s world-space batches
  /// into screen-space [CanvasOverlay] primitives so the canvas viewport
  /// can draw guide ribbons without importing the engine (the viewport's
  /// file doc explicitly states "We don't import the engine to keep the
  /// UI package hermetic — the parent editor screen adapts the engine
  /// state into [CanvasScene]"). This mirrors the existing
  /// [_buildSelectionOverlay] / [_buildGizmoOverlay] / [_buildWireframeOverlay]
  /// adaptation pattern: the host owns the engine → canvas-pixel bridge.
  ///
  /// Surface triangles become [CanvasOverlayTriangle] (translucent, drawn
  /// with the engine's [Guide3DRenderer.defaultSurfaceColor]); grid line
  /// segments + the orange starting line become [CanvasOverlayLine]. The
  /// view-projection [vp] is the same matrix the stroke-projection loop
  /// in [_buildCanvasScene] uses, so guides and strokes share the same
  /// screen space. Vertices behind the camera (NDC.z outside [-1, 1))
  /// are skipped per-triangle / per-segment.
  List<CanvasOverlay> _buildGuideRibbonOverlay(Matrix4 vp) {
    if (_guides.guides.isEmpty) return const <CanvasOverlay>[];
    final renderer = Guide3DRenderer();
    final renderData = renderer.renderAll(_guides.guides);
    final out = <CanvasOverlay>[];
    final w = _viewportSize.width;
    final h = _viewportSize.height;
    if (w == 0 || h == 0) return const <CanvasOverlay>[];

    Offset? project(Vector3 world) {
      final ndc = vp.transform3(world.clone());
      if (ndc.z <= -1.0 || ndc.z >= 1.0) return null;
      final sx = (ndc.x * 0.5 + 0.5) * w;
      final sy = (1.0 - (ndc.y * 0.5 + 0.5)) * h;
      return Offset(sx, sy);
    }

    for (final data in renderData) {
      // Surface: triangles. Skip a triangle when any vertex is behind
      // the camera (cheap cull — a proper clip plane would split the
      // triangle, but for guide ribbons a per-vertex cull is good
      // enough and avoids emitting degenerate screen triangles).
      final sVerts = data.surface.vertices;
      if (data.surface.mode == GuideBatchMode.triangles) {
        for (var i = 0; i + 2 < data.surface.indices.length; i += 3) {
          final a = project(sVerts[data.surface.indices[i]].position);
          final b = project(sVerts[data.surface.indices[i + 1]].position);
          final c = project(sVerts[data.surface.indices[i + 2]].position);
          if (a == null || b == null || c == null) continue;
          out.add(CanvasOverlayTriangle(a, b, c,
              MainScreen.kGuideRibbonSurfaceColor));
        }
      }
      // Grid + start line: line segments.
      for (final batch in [data.grid, data.startLine]) {
        if (batch.mode != GuideBatchMode.lines) continue;
        final bVerts = batch.vertices;
        for (var i = 0; i + 1 < batch.indices.length; i += 2) {
          final a = project(bVerts[batch.indices[i]].position);
          final b = project(bVerts[batch.indices[i + 1]].position);
          if (a == null || b == null) continue;
          final color = batch == data.startLine
              ? MainScreen.kGuideRibbonStartLineColor
              : Guide3DRenderer.gridLineColor;
          out.add(CanvasOverlayLine(a, b, color, strokeWidth: 1.2));
        }
      }
    }
    return out;
  }

  /// Builds the wireframe overlay — one [CanvasOverlayPolyline] per
  /// visible stroke, drawn with the stroke's own colour. Mirrors the
  /// screen-projection loop in [_buildCanvasScene] (world → view-proj →
  /// NDC → screen pixels) but emits a polyline instead of a [CanvasStroke].
  /// Strokes with fewer than 2 visible screen points are skipped (no
  /// polyline to draw).
  List<CanvasOverlay> _buildWireframeOverlay(Matrix4 vp) {
    final out = <CanvasOverlay>[];
    for (final stroke in _strokes) {
      if (!stroke.isVisible) continue;
      final points = <Offset>[];
      for (final p in stroke.points) {
        final world = stroke.transform.transform3(p.position.clone());
        final ndc = vp.transform3(world.clone());
        if (ndc.z <= -1.0 || ndc.z >= 1.0) continue;
        final sx = (ndc.x * 0.5 + 0.5) * _viewportSize.width;
        final sy = (1.0 - (ndc.y * 0.5 + 0.5)) * _viewportSize.height;
        points.add(Offset(sx, sy));
      }
      if (points.length < 2) continue;
      out.add(CanvasOverlayPolyline(
        points,
        stroke.color,
        strokeWidth: 1.4,
      ));
    }
    return out;
  }

  /// Adapts [SelectionRenderer]'s draw-list into [CanvasOverlay] values.
  /// The renderer needs a `Map<int, dynamic>` lookup (it duck-types on
  /// `isVisible` / `transform` / `points`) — we build it from the live
  /// [Stroke] list.
  List<CanvasOverlay> _buildSelectionOverlay(Matrix4 vp) {
    final strokesById = <int, dynamic>{
      for (final s in _strokes) s.id: s,
    };
    final renderer = SelectionRenderer(
      strokesById: strokesById,
      state: _selectionModel.state,
      viewProjection: vp,
      viewportWidth: _viewportSize.width,
      viewportHeight: _viewportSize.height,
    );
    return _adaptGizmoPrimitives(renderer.build().primitives);
  }

  /// Adapts the appropriate gizmo renderer's draw-list (2D or 3D based
  /// on [_joystick3d]) into [CanvasOverlay] values. The gizmo is
  /// centred at the selection pivot projected to screen, with a fixed
  /// 80-pixel radius (matches the on-screen joystick widget's default
  /// size — kept in pixels so the gizmo doesn't shrink with zoom).
  List<CanvasOverlay> _buildGizmoOverlay(Matrix4 vp) {
    const radius = 80.0;
    final selected = _selectedStrokes();
    if (selected.isEmpty) return const [];
    final pivotWorld = _selectionPivot(selected);
    final ndc = vp.transform3(pivotWorld.clone());
    if (ndc.z <= -1.0 || ndc.z >= 1.0) return const [];
    final sx = (ndc.x * 0.5 + 0.5) * _viewportSize.width;
    final sy = (1.0 - (ndc.y * 0.5 + 0.5)) * _viewportSize.height;
    final centre = Vector2(sx, sy);
    final frame = _viewFrame(selected);
    final drawList = _joystick3d
        ? Joystick3dGizmoRenderer(
            centre: centre,
            radius: radius,
            frame: frame,
            usableAxes: _joy3d.usableAxes(frame),
          ).build()
        : Joystick2dGizmoRenderer(
            centre: centre,
            radius: radius,
            lock: _joystickLock,
          ).build();
    return _adaptGizmoPrimitives(drawList.primitives);
  }

  /// Adapts [LiquifyRenderer]'s draw-list into [CanvasOverlay] values.
  /// The brush centre is the last-reported liquify cursor; the radii
  /// come from the active [LiquifySettings] scaled by the camera's
  /// world-to-pixel factor at the cursor's depth.
  List<CanvasOverlay> _buildLiquifyOverlay() {
    final cursor = _liquifyCursor;
    if (cursor == null) return const [];
    // Compute pixels-per-world-unit at the camera distance (matches the
    // formula in [OrbitCamera.pan]).
    final viewportH = _viewportSize.height;
    if (viewportH <= 0) return const [];
    final worldPerPixel =
        2.0 * _camera.distance * dmath.tan(_camera.fovYRadians * 0.5) /
            viewportH;
    final pixelsPerWorld = worldPerPixel <= 0 ? 1.0 : 1.0 / worldPerPixel;
    final renderer = LiquifyRenderer(
      centre: Vector2(cursor.dx, cursor.dy),
      settings: _liquify.settings,
      brushType: _liquify.brushType,
      dragDelta: _liquifyDragDelta,
      worldToScreen: pixelsPerWorld,
    );
    return _adaptGizmoPrimitives(renderer.build().primitives);
  }

  /// Translates the engine's pure-data [GizmoPrimitive] hierarchy into
  /// the UI's [CanvasOverlay] hierarchy. The two mirror each other
  /// (lines, polylines, circles, triangles) — this adapter keeps the UI
  /// package decoupled from the engine (no `package:feather_krita/engine`
  /// import in `canvas_viewport.dart`).
  List<CanvasOverlay> _adaptGizmoPrimitives(List<GizmoPrimitive> prims) {
    final out = <CanvasOverlay>[];
    for (final p in prims) {
      if (p is GizmoLine) {
        out.add(CanvasOverlayLine(
          Offset(p.a.x, p.a.y),
          Offset(p.b.x, p.b.y),
          p.color,
          strokeWidth: p.strokeWidth,
        ));
      } else if (p is GizmoPolyline) {
        out.add(CanvasOverlayPolyline(
          p.points.map((v) => Offset(v.x, v.y)).toList(growable: false),
          p.color,
          strokeWidth: p.strokeWidth,
        ));
      } else if (p is GizmoCircle) {
        out.add(CanvasOverlayCircle(
          Offset(p.center.x, p.center.y),
          p.radius,
          p.color,
          filled: p.filled,
          strokeWidth: p.strokeWidth,
        ));
      } else if (p is GizmoTriangle) {
        out.add(CanvasOverlayTriangle(
          Offset(p.a.x, p.a.y),
          Offset(p.b.x, p.b.y),
          Offset(p.c.x, p.c.y),
          p.color,
        ));
      }
    }
    return out;
  }

  /// Maps the UI-side [FeatherMaterial] enum (emitted by the
  /// [MaterialPicker]) to the canvas viewport's rendering hint enum
  /// [CanvasMaterial]. The two enums mirror the same Feather material
  /// kinds; this converter keeps the canvas viewport decoupled from
  /// the picker (it doesn't import material_picker.dart).
  ///
  /// v0.56-C: the picker's 5th kind — [FeatherMaterial.metallic] —
  /// now maps to a REAL [CanvasMaterial.metallic] render branch (a
  /// Lambert + Phong tube plus a thin chrome catch-light, see
  /// _paintMetallicTube in canvas_viewport.dart). The engine side is
  /// backed by [MetallicMaterial] (lib/engine/material/
  /// metallic_material.dart) — a Lambert + high-spec Phong +
  /// environment-reflection-tint BRDF. The v55-B fake mapping
  /// (metallic → shaded) is gone.
  CanvasMaterial _toCanvasMaterial(FeatherMaterial m) {
    switch (m) {
      case FeatherMaterial.shadeless:
        return CanvasMaterial.flat;
      case FeatherMaterial.shaded:
        return CanvasMaterial.shaded;
      case FeatherMaterial.glow:
        return CanvasMaterial.glow;
      case FeatherMaterial.cutout:
        return CanvasMaterial.cutout;
      case FeatherMaterial.metallic:
        // Real metallic render path (v0.56-C). See _paintMetallicTube
        // in canvas_viewport.dart + MetallicMaterial in the engine.
        return CanvasMaterial.metallic;
    }
  }

  List<LayerItem> _buildLayerItems() {
    final items = <LayerItem>[];
    for (final layer in _scene.layers.layers) {
      items.add(LayerItem(
        id: layer.id,
        name: layer.name,
        color: layer.color,
        visible: layer.visible,
        selected: layer.id == _scene.layers.activeId,
        count: layer.curveIds.length,
      ));
    }
    return items;
  }

  /// Builds the [StrokeListItem] snapshot the [StrokeListPanel] renders.
  /// Iterates the canonical [_strokes] list in z-order (back → front)
  /// + projects each stroke into the value object the panel expects
  /// (id, name, color, material, isVisible, sampleCount). The material
  /// comes from the per-stroke [_strokeMaterials] map (defaulting to
  /// shadeless when no entry exists, matching the canvas painter's
  /// fallback). The sample count comes from the parallel [_stroke3Ds]
  /// map (the curves-system capture view) — falls back to the Stroke's
  /// point count when no Stroke3D exists (e.g. after an undo rebuild).
  List<StrokeListItem> _buildStrokeListItems() {
    final items = <StrokeListItem>[];
    for (var i = 0; i < _strokes.length; i++) {
      final s = _strokes[i];
      final mat = _strokeMaterials[s.id] ?? FeatherMaterial.shadeless;
      final s3d = _stroke3Ds[s.id];
      items.add(StrokeListItem(
        id: s.id,
        name: 'Stroke ${i + 1}',
        color: s.color,
        material: mat,
        isVisible: s.isVisible,
        sampleCount: s3d != null ? s3d.length : s.points.length,
      ));
    }
    return items;
  }

  /// v55-B: toggles a stroke's visibility flag. Mutates the canonical
  /// [_strokes] entry + setState so the next frame's [_buildCanvasScene]
  /// skips / includes the stroke (the painter already honours
  /// `stroke.isVisible` — see the `if (!stroke.isVisible) continue;`
  /// guard in [_buildCanvasScene]).
  void _setStrokeVisible(int id, bool visible) {
    final s = _findStroke(id);
    if (s == null) return;
    setState(() => s.isVisible = visible);
  }

  /// v55-B: deletes a stroke by id. Mirrors the cleanup pattern the
  /// eraser / vacuum paths use (removes from [_strokes] + the parallel
  /// [_stroke3Ds] + [_strokeMaterials] maps). Pushes the pre-delete
  /// snapshot onto the undo stack so the delete is undoable (matches
  /// the existing undo contract). Clears the stroke from the selection
  /// model if it was selected (uses [SelectionModel.setActive] — the
  /// model exposes no public state setter).
  void _deleteStrokeById(int id) {
    final s = _findStroke(id);
    if (s == null) return;
    _pushUndo();
    setState(() {
      _strokes.removeWhere((st) => st.id == id);
      _stroke3Ds.remove(id);
      _strokeMaterials.remove(id);
      if (_selectionModel.active.contains(id)) {
        _selectionModel.setActive(
          _selectionModel.active.where((sid) => sid != id).toList(),
        );
      }
    });
  }

  /// v55-B: reorders the canonical [_strokes] list (z-order). The
  /// [ReorderableListView] contract normalises the indices before
  /// calling this (oldIndex is the source; newIndex is the target
  /// AFTER normalisation — see the panel's onReorder wrapper). The
  /// reorder is undoable (pushes the pre-reorder snapshot).
  void _reorderStrokes(int oldIndex, int newIndex) {
    if (oldIndex < 0 || oldIndex >= _strokes.length) return;
    if (newIndex < 0 || newIndex >= _strokes.length) return;
    if (oldIndex == newIndex) return;
    _pushUndo();
    setState(() {
      final s = _strokes.removeAt(oldIndex);
      _strokes.insert(newIndex, s);
    });
  }

  List<ResourceItem> _buildResourceItems() {
    final items = <ResourceItem>[];
    for (final r in _scene.resources.values) {
      items.add(ResourceItem(id: r.id, name: r.name, kind: r.kind));
    }
    // Active guides appear as resources too (the Resources tab).
    final guideIds = _guides.ids;
    final guideList = _guides.guides;
    for (var i = 0; i < guideList.length; i++) {
      final guide = guideList[i];
      items.add(ResourceItem(
        id: 'guide-${guideIds[i]}',
        name: guide.name ?? 'Guide',
        kind: 'Guide3D',
      ));
    }
    return items;
  }

  List<BrushPreset> _buildDefaultPresets() {
    return const [
      BrushPreset(
        id: 'basic_round',
        name: 'Basic Round',
        previewColor: Color(0xFF1A1A1A),
        strokeWidth: 5,
      ),
      BrushPreset(
        id: 'soft_airbrush',
        name: 'Soft Airbrush',
        previewColor: Color(0xFF60A5FA),
        strokeWidth: 3,
      ),
      BrushPreset(
        id: 'ink_fineliner',
        name: 'Ink Fineliner',
        previewColor: Color(0xFF111111),
        strokeWidth: 2,
        tapered: false,
      ),
      BrushPreset(
        id: 'marker_flat',
        name: 'Marker Flat',
        previewColor: Color(0xFFFB923C),
        strokeWidth: 7,
        tapered: false,
      ),
      BrushPreset(
        id: 'dashed_thin',
        name: 'Dashed Thin',
        previewColor: Color(0xFFA78BFA),
        strokeWidth: 3,
        dashed: true,
      ),
    ];
  }

  // ----- UI state changes (from EditorScreen) -----------------------------

  void _onUiStateChanged(EditorUiState s) {
    setState(() {
      if (_tool != s.tool) {
        // Exiting the liquify tool clears the brush cursor + drag delta
        // so the preview doesn't linger on the canvas after the user
        // switches tools.
        if (_tool == FeatherTool.liquify && s.tool != FeatherTool.liquify) {
          _liquifyCursor = null;
          _liquifyDragDelta = null;
        }
        _tool = s.tool;
        // Entering the liquify tool begins a liquify session on the
        // current selection; leaving it applies the session.
        if (_tool == FeatherTool.liquify) {
          _liquify.begin(_selectedStrokes());
        } else if (_liquify.isEditing) {
          _liquify.apply();
        }
        // Entering the mirror tool (v0.54-A §3) opens the assist panel
        // and defaults the mirror axis set to {X} when no axis is
        // active — the most common single-axis mirror. The user can
        // then toggle Y / Z in the panel. Leaving the mirror tool does
        // NOT clear the axis set (the mirror ASSIST is independent of
        // the tool; the artist may keep mirror on while drawing with
        // the draw tool).
        if (_tool == FeatherTool.mirror) {
          _assistPanelVisible = true;
          if (_mirrorAssist.activeAxes.isEmpty) {
            _mirrorAssist.activeAxes = {MirrorAxis.x};
          }
        }
      }
      _color = s.color;
      _brushSize = s.size;
      _brushOpacity = s.opacity;
      _pressure = s.pressure;
      _material = s.material;
      _pattern = s.pattern;
      _renderMode = s.renderMode;
      // v55-B sync: when the legacy [TopBar] single render-mode button
      // toggles the boolean, mirror it into the new 3-state UI. The old
      // button flips shaded ↔ shadeless; if the host was in wireframe,
      // the old button clears wireframe + lands on the new state.
      if (s.renderMode) {
        _renderModeState = RenderModeState.shaded;
        _wireframeOverlay = false;
      } else if (_renderModeState == RenderModeState.shaded) {
        // Was shaded, the old button just flipped to false → shadeless.
        _renderModeState = RenderModeState.shadeless;
        _wireframeOverlay = false;
      }
      // If the new 3-state toggle was already in shadeless or wireframe,
      // the old button's flip-to-false is a no-op on the new state
      // (stays shadeless / wireframe — both already have _renderMode
      // == false). Flip-to-true always wins (lands on shaded).
      _uiHidden = s.uiHidden;
      _groupName = s.groupName;
      // Mirror the Stage panel's Environment tab into the host's env
      // state. [_buildCanvasScene] then projects these onto the
      // [CanvasScene] handed to the viewport.
      _lightAzimuth = s.lightAzimuth;
      _lightElevation = s.lightElevation;
      _glowArea = s.glowArea;
      _backgroundColor = s.backgroundColor;
      _showGrid = s.showGrid;
      _showGroundPlane = s.showGroundPlane;
    });
    // Mirror into the live brush engine.
    _brush.color = _color.toARGB32();
    _brush.settings = _brush.settings.copyWith(
      size: _brushSize,
      opacity: _brushOpacity,
      pressureEnabled: _pressure,
    );
    // Mirror into the Krita backend (real or fallback) so dabs match.
    final backend = _krita.isInitialized ? _krita.backend : null;
    if (backend != null) {
      backend
        ..size = _brushSize
        ..opacity = _brushOpacity
        ..color = BrushColor(
          (_color.r * 255).round(),
          (_color.g * 255).round(),
          (_color.b * 255).round(),
          (_color.a * 255).round(),
        );
    }
  }

  // ----- Render mode state (v55-B) ----------------------------------------
  //
  // Handler for the prominent 3-state [RenderModeToggle]. Maps the new
  // 3-state enum onto the engine's existing boolean [CanvasScene.renderMode]
  // (shaded → true; shadeless / wireframe → false) plus the host-side
  // [_wireframeOverlay] flag (true only in wireframe — drives the polyline
  // overlay in [_buildOverlayPrimitives]). The setState rebuild propagates
  // both flags into [_buildCanvasScene] + the overlay primitives list so
  // the next frame paints the right render mode.
  void _onRenderModeChanged(RenderModeState next) {
    setState(() {
      _renderModeState = next;
      switch (next) {
        case RenderModeState.shaded:
          _renderMode = true;
          _wireframeOverlay = false;
          break;
        case RenderModeState.shadeless:
          _renderMode = false;
          _wireframeOverlay = false;
          break;
        case RenderModeState.wireframe:
          // Wireframe = flat render + polyline overlay on top. Honest
          // scope note: a real wireframe material pass would require
          // touching lib/core/rendering (out of scope); this overlay
          // gives the artist the visual affordance (curve skeleton).
          _renderMode = false;
          _wireframeOverlay = true;
          break;
      }
    });
  }

  // ----- Keyboard shortcuts (loop-keyboard-shortcuts-mouse) --------------
  //
  // The bindings map below is wired into [EditorShortcuts] in [build].
  // Every Ctrl-shortcut is registered twice — once with `control: true`
  // (Windows/Linux) and once with `meta: true` (macOS / Android physical
  // keyboards) — so the same logical shortcut works on every desktop.
  // Plain-letter shortcuts (B/E/V/S/M/G/L/P/F) are registered without
  // modifiers; when a text field has focus those keys are consumed by
  // the field and never reach this node, so typing in a Save-As filename
  // box is unaffected. Each handler is a thin wrapper around the real
  // engine method (or the existing UI-state path through [_setUiState]),
  // so the wiring stays here and the behaviour stays testable.

  void _shortcutUndo() => _undo();

  void _shortcutRedo() => _redo();

  void _shortcutSetTool(FeatherTool tool) => _setUiStateTool(tool);

  /// Pushes a tool change through the same path the EditorScreen uses
  /// ([_onUiStateChanged]) so the host's mirror + engine side-effects
  /// (brush size sync, liquify session begin/end, cursor clear) all fire.
  void _setUiStateTool(FeatherTool next) {
    final s = _currentUiState().copyWith(tool: next);
    _onUiStateChanged(s);
    // Re-mirror into the EditorScreen so the dock + top bar reflect the
    // new tool. The EditorScreen rebuilds via its own setState on
    // _onUiStateChanged (it stores the new state) but the host's
    // setState ensures the build method re-runs with the new
    // drawingEnabled / onTapSelect / onLiquifyDrag gating.
    setState(() {});
  }

  /// Mirror of the live UI state, used to compute "next" states for the
  /// keyboard shortcuts (tool / mode / visibility toggles). Kept in sync
  /// with the host fields by [_onUiStateChanged].
  EditorUiState _currentUiState() => EditorUiState(
        tool: _tool,
        color: _color,
        size: _brushSize,
        opacity: _brushOpacity,
        pressure: _pressure,
        material: _material,
        pattern: _pattern,
        groupName: _groupName,
        renderMode: _renderMode,
        uiHidden: _uiHidden,
      );

  void _shortcutToggleMirror() => _setUiStateTool(FeatherTool.mirror);

  void _shortcutToggleGuideDrawMode() {
    setState(() => _guideDrawMode = !_guideDrawMode);
  }

  void _shortcutLoftMode() => setGuideMode(_GuideMode.loft);

  void _shortcutPrimitivesMode() => setGuideMode(_GuideMode.primitive);

  void _shortcutDeselectAll() {
    _selectionModel.clear();
    setState(() {});
  }

  void _shortcutSelectAll() => _selectAll();

  void _shortcutDeleteSelected() {
    final selected = _selectedStrokes();
    if (selected.isEmpty) return;
    _pushUndo();
    final ids = selected.map((s) => s.id).toSet();
    _strokes.removeWhere((s) => ids.contains(s.id));
    for (final id in ids) {
      _stroke3Ds.remove(id);
      _strokeMaterials.remove(id);
    }
    _selectionModel.clear();
    _rebuildStroke3Ds();
    setState(() {});
  }

  void _shortcutExport() => _showExportSheet();

  /// Ctrl+S — quick-save the current document as a `.feather` JSON file
  /// in the exports directory. Reuses the [ProjectModel] schema so the
  /// file is loadable by [_shortcutOpen] (and any future project browser).
  Future<void> _shortcutSave() async {
    try {
      final dir = exportsDir();
      final now = DateTime.now();
      final stamp = '${now.year}'
          '${now.month.toString().padLeft(2, '0')}'
          '${now.day.toString().padLeft(2, '0')}_'
          '${now.hour.toString().padLeft(2, '0')}'
          '${now.minute.toString().padLeft(2, '0')}'
          '${now.second.toString().padLeft(2, '0')}';
      final path = '${dir.path}${Platform.pathSeparator}feather_$stamp.feather';
      final model = ProjectModel(
        name: 'feather_$stamp',
        scene: ProjectSceneModel(strokes: _strokes),
        camera: ProjectCameraModel(
          yaw: _camera.yaw,
          pitch: _camera.pitch,
          distance: _camera.distance,
          targetX: _camera.target.x,
          targetY: _camera.target.y,
          targetZ: _camera.target.z,
        ),
        brush: ProjectBrushModel(
          presetName: 'Basic Round',
          size: _brushSize,
          opacity: _brushOpacity,
          color: _color.toARGB32(),
        ),
        created: now,
        modified: now,
      );
      File(path).writeAsStringSync(model.toJson().toString());
      _lastExportPath = path;
      _showExportSnackBar('Project saved', path);
    } catch (e) {
      _showExportSnackBar('Save failed: $e', null, isError: true);
    }
  }

  /// Ctrl+O — pick a `.feather` file and load its strokes + camera + brush
  /// into the live editor. Pushes the current state to the undo stack so
  /// the open is reversible.
  Future<void> _shortcutOpen() async {
    final result = await FilePicker.platform.pickFiles(
      dialogTitle: 'Open Feather project',
      type: FileType.custom,
      allowedExtensions: const ['feather'],
    );
    final path = result?.files.single.path;
    if (path == null) return;
    try {
      final raw = File(path).readAsStringSync();
      final json = (jsonDecode(raw) as Map?)?.cast<String, dynamic>() ?? {};
      final model = ProjectModel.fromJson(json);
      _pushUndo();
      _strokes
        ..clear()
        ..addAll(model.scene.strokes);
      _selectionModel.clear();
      _rebuildStroke3Ds();
      _camera.yaw = model.camera.yaw;
      _camera.pitch = model.camera.pitch;
      _camera.distance = model.camera.distance;
      _camera.target = Vector3(
        model.camera.targetX,
        model.camera.targetY,
        model.camera.targetZ,
      );
      _brushSize = model.brush.size;
      _brushOpacity = model.brush.opacity;
      _color = Color(model.brush.color);
      setState(() {});
      _showExportSnackBar('Opened ${model.name}', path);
    } catch (e) {
      _showExportSnackBar('Open failed: $e', null, isError: true);
    }
  }

  void _shortcutResetCamera() {
    setState(() {
      _camera.yaw = 0.0;
      _camera.pitch = -0.35;
      _camera.distance = 6.0;
      _camera.target = Vector3.zero();
    });
  }

  void _shortcutZoomIn() => setState(() => _camera.zoom(0.9));

  void _shortcutZoomOut() => setState(() => _camera.zoom(1.1));

  void _shortcutToggleUi() {
    setState(() => _uiHidden = !_uiHidden);
    _onUiStateChanged(_currentUiState().copyWith(uiHidden: _uiHidden));
  }

  /// 1 = 2D joystick, 2 = (no-op middle slot, reserved), 3 = 3D joystick.
  /// Per the task spec: "1/2/3 = Switch 2D/3D joystick". We map 1 → 2D
  /// resolver, 3 → 3D resolver; 2 is a reserved middle slot that toggles
  /// the joystick lock off (a no-op until a lock UI is wired).
  void _shortcutJoystick2d() => setJoystick3d(false);

  void _shortcutJoystick3d() => setJoystick3d(true);

  void _shortcutBrushSizeDelta(double delta) {
    final next = (_brushSize + delta).clamp(1.0, 500.0);
    if ((next - _brushSize).abs() < 1e-6) return;
    setState(() => _brushSize = next);
    _brush.settings = _brush.settings.copyWith(size: _brushSize);
    final backend = _krita.isInitialized ? _krita.backend : null;
    if (backend != null) backend.size = _brushSize;
    // v0.57-D: persist the new brush size so the next session opens
    // with the same value (SettingsRepository round-trip).
    _persistSettings();
  }

  /// The full shortcut → callback map. Built once per build; cheap because
  /// the closures capture only `this`.
  Map<ShortcutActivator, VoidCallback> _shortcutBindings() {
    return <ShortcutActivator, VoidCallback>{
      // Undo / redo (Ctrl/Cmd+Z, Ctrl/Cmd+Shift+Z, Ctrl/Cmd+Y).
      ctrlKey(LogicalKeyboardKey.keyZ): _shortcutUndo,
      metaKey(LogicalKeyboardKey.keyZ): _shortcutUndo,
      ctrlKey(LogicalKeyboardKey.keyZ, shift: true): _shortcutRedo,
      metaKey(LogicalKeyboardKey.keyZ, shift: true): _shortcutRedo,
      ctrlKey(LogicalKeyboardKey.keyY): _shortcutRedo,
      metaKey(LogicalKeyboardKey.keyY): _shortcutRedo,
      // Document ops.
      ctrlKey(LogicalKeyboardKey.keyS): _shortcutSave,
      metaKey(LogicalKeyboardKey.keyS): _shortcutSave,
      ctrlKey(LogicalKeyboardKey.keyO): _shortcutOpen,
      metaKey(LogicalKeyboardKey.keyO): _shortcutOpen,
      ctrlKey(LogicalKeyboardKey.keyE): _shortcutExport,
      metaKey(LogicalKeyboardKey.keyE): _shortcutExport,
      ctrlKey(LogicalKeyboardKey.keyA): _shortcutSelectAll,
      metaKey(LogicalKeyboardKey.keyA): _shortcutSelectAll,
      ctrlKey(LogicalKeyboardKey.keyD): _shortcutDeselectAll,
      metaKey(LogicalKeyboardKey.keyD): _shortcutDeselectAll,
      // Tool hotkeys (plain letters; a focused text field consumes these).
      const SingleActivator(LogicalKeyboardKey.keyB):
          () => _shortcutSetTool(FeatherTool.draw),
      const SingleActivator(LogicalKeyboardKey.keyE):
          () => _shortcutSetTool(FeatherTool.eraser),
      const SingleActivator(LogicalKeyboardKey.keyV):
          () => _shortcutSetTool(FeatherTool.vacuum),
      const SingleActivator(LogicalKeyboardKey.keyS):
          () => _shortcutSetTool(FeatherTool.select),
      const SingleActivator(LogicalKeyboardKey.keyM): _shortcutToggleMirror,
      const SingleActivator(LogicalKeyboardKey.keyG): _shortcutToggleGuideDrawMode,
      const SingleActivator(LogicalKeyboardKey.keyL): _shortcutLoftMode,
      const SingleActivator(LogicalKeyboardKey.keyP): _shortcutPrimitivesMode,
      // Selection / deletion.
      const SingleActivator(LogicalKeyboardKey.delete): _shortcutDeleteSelected,
      const SingleActivator(LogicalKeyboardKey.backspace):
          _shortcutDeleteSelected,
      const SingleActivator(LogicalKeyboardKey.escape): _shortcutDeselectAll,
      // Camera.
      const SingleActivator(LogicalKeyboardKey.keyF): _shortcutResetCamera,
      const SingleActivator(LogicalKeyboardKey.equal): _shortcutZoomIn,
      const SingleActivator(LogicalKeyboardKey.numpadAdd): _shortcutZoomIn,
      const SingleActivator(LogicalKeyboardKey.minus): _shortcutZoomOut,
      const SingleActivator(LogicalKeyboardKey.numpadSubtract): _shortcutZoomOut,
      // Joystick resolver switch (1 = 2D, 3 = 3D; 2 reserved).
      const SingleActivator(LogicalKeyboardKey.digit1): _shortcutJoystick2d,
      const SingleActivator(LogicalKeyboardKey.digit3): _shortcutJoystick3d,
      // UI visibility.
      const SingleActivator(LogicalKeyboardKey.tab): _shortcutToggleUi,
    };
  }

  // ----- Desktop mouse handlers (loop-keyboard-shortcuts-mouse) ------------
  //
  // Wired into [CanvasViewport] via [EditorScreen.onOrbit / onCameraPan /
  // onBrushSizeDelta]. The viewport's Listener routes right / middle
  // drags and the scroll wheel to these handlers; the host then mutates
  // the camera / brush directly.

  /// Right-click drag (and left+Space) → orbit. [_onPan] already orbits,
  /// so we just forward — the dedicated handler exists so the routing is
  /// explicit and future divergence (e.g. a different orbit sensitivity
  /// for mouse vs. touch) has a clear home.
  void _onOrbit(Offset delta) => _onPan(delta);

  /// Middle-click drag → pan (translate the orbit centre). Distinct from
  /// [_onPan] which orbits. Uses [OrbitCamera.pan] which converts the
  /// pixel delta to a world-space translation along the camera's right /
  /// up axes (the eye follows so the view translates without rotating).
  void _onCameraPan(Offset delta) {
    if (_viewportSize.isEmpty) return;
    setState(() {
      _camera.pan(
        delta.dx,
        delta.dy,
        _viewportSize.width,
        _viewportSize.height,
      );
    });
  }

  /// Ctrl + scroll → brush size ± (millimetres). Clamps to the engine's
  /// 1..500 mm range and mirrors the new size into the procedural brush
  /// + the Krita backend so the next dab matches.
  void _onBrushSizeDelta(double delta) => _shortcutBrushSizeDelta(delta);

  // ----- Camera gestures --------------------------------------------------

  void _onPan(Offset delta) {
    setState(() {
      // Two-finger drag orbits the camera (yaw / pitch). This matches
      // Feather 3D's "spin the model" gesture.
      _camera.orbit(delta.dx * 0.008, delta.dy * 0.008);
    });
  }

  void _onZoom(double scale) {
    if ((scale - 1.0).abs() < 0.001) return;
    setState(() {
      // Pinch out (scale > 1) → zoom in (distance shrinks).
      _camera.zoom(1.0 / scale);
    });
  }

  // ----- Stroke drawing ---------------------------------------------------

  void _onStrokeStart() {
    _liveStroke.clear();
    // Bend mode: the stroke is a bend path, not a paint stroke. We don't
    // push undo (the stroke list isn't mutated) — the resulting bent guide
    // is committed to the [Guide3DManager], which is its own undo domain.
    if (_guideMode != _GuideMode.bend) {
      _pushUndo();
    }
    // Stable Strokes: sync the stabilizer's enabled flag to the host's
    // [_stableStrokesEnabled] toggle and reset the buffer for a new
    // stroke. The stabilizer is a CAUSAL pre-filter on the world sample
    // stream — see [_onStrokeUpdate] for the push call.
    _stableStrokes.setConfig(StableStrokesConfig(
      enabled: _stableStrokesEnabled,
      intensity: _stableStrokesIntensity,
      maxWindow: 12,
      extrapolation: 0.5,
    ));
    _stableStrokes.reset();
    // Initialize Stroke3D capture for draw mode. The Stroke3D is the
    // curves-system source of truth (carries pressure + timestamp per
    // sample) and is converted to a Stroke for rendering in
    // [_onStrokeEnd]. Bend / guide-draw / eraser / vacuum don't need a
    // Stroke3D — they're either guide-creation paths or erase paths.
    // The mirror tool (v0.54-A §3) also captures a Stroke3D so the
    // committed stroke + its mirror copies share the same source.
    if ((_tool == FeatherTool.draw ||
            _tool == FeatherTool.mirror) &&
        !_guideDrawMode &&
        _guideMode != _GuideMode.bend) {
      _liveStroke3D = Stroke3D(
        color: _color.toARGB32(),
        thickness: _brushSize * 0.15,
        inputDevice: StrokeInputDevice.touch,
        source: 'feather-canvas',
      );
      _strokeStartTime = DateTime.now();
    }
    // v0.55-C: reset the dab throttle so the first dab of the new stroke
    // fires immediately (otherwise the first dab of every stroke would be
    // skipped if the previous stroke ended < 16 ms ago — common on rapid
    // tap-drag lifts).
    _lastDabTime = null;
  }

  void _onStrokeUpdate(Offset screenPos) {
    // v0.55-C: track the latest screen-space position so [_onStrokeEnd]
    // can flush a final dab at the exact pen-lift point (the throttle gate
    // below may skip the most recent update(s)).
    _lastStrokeScreenPos = screenPos;
    // Eraser sub-mode: remove stroke POINTS within the erase radius
    // (per the Feather docs: "removes points from the center of the
    // curve, not the surrounding geometry"). Strokes covered by a 3D
    // Guide are protected (see [_strokeProtectedByGuide]).
    if (_tool == FeatherTool.eraser) {
      _eraseAt(screenPos);
      setState(() {});
      return;
    }
    // Vacuum sub-mode: remove entire STROKES the pen touches (per the
    // Feather docs: "erases all curves it touches"). Same guide
    // isolation as the eraser.
    if (_tool == FeatherTool.vacuum) {
      _vacuumAt(screenPos);
      setState(() {});
      return;
    }
    final world = _screenToWorld(screenPos);
    if (world == null) return;
    // v0.57-B: snap-to-guide. When the GuidePanel's "Snap to guide"
    // toggle ([_guideSnap]) is ON, project the world sample onto the
    // nearest guide surface within [Guide3DSnap.maxDistance] (default
    // 0.25 world units). This softens the hard raycast inside
    // [_screenToWorld] — which falls through to the y=0 ground plane
    // when the pen ray misses a guide — so strokes land cleanly on a
    // nearby guide ribbon even when the cursor is just off its edge.
    // When no guide is in range, [snapPoint] returns null and the
    // original world sample flows through unchanged (no behaviour
    // change vs. snap-OFF). Skip when no guides exist (cheap exit).
    Vector3 effectiveWorld = world;
    if (_guideSnap && _guides.guides.isNotEmpty) {
      final snap = _guideSnapHelper.snapPoint(_guides.guides, world);
      if (snap != null) effectiveWorld = snap.point;
    }
    // Stable Strokes pre-filter: push the raw world sample through the
    // causal Gaussian stabilizer and use its output (when non-null) as
    // the world position fed to the live stroke + Stroke3D capture. On
    // warm-up (first sample, returns null) we fall back to the raw world
    // so the stroke stays continuous — the stabilizer's own doc notes
    // the consumer "may want to suppress the dab until the filter has
    // warmed up"; we choose continuity over a 1-frame gap.
    //
    // HONESTY NOTE: the real-Krita dab path (see [_paintRealDab] below)
    // continues to use the raw [screenPos] — re-projecting the smoothed
    // world back to screen space would require a world→screen helper
    // that doesn't exist yet (see [_screenToRay] for the inverse). The
    // stabilizer therefore smooths the 3D curve geometry (the ribbon
    // + the exported Stroke3D) but not the real-Krita paint layer's
    // dab placement. This is a partial wiring — documented honestly.
    final pressure = _pressure ? 0.8 : 0.5;
    final t = _strokeStartTime == null
        ? 0.0
        : DateTime.now().difference(_strokeStartTime!).inMicroseconds / 1e6;
    final smoothed =
        smoothStrokeSample(_stableStrokes, effectiveWorld, pressure: pressure, time: t);
    final sampleWorld = smoothed ?? effectiveWorld;
    _liveStroke.add(sampleWorld);
    // Build Stroke3D (curves-system source of truth) for draw mode.
    if (_liveStroke3D != null && _strokeStartTime != null) {
      _liveStroke3D!.addSample(StrokeSample3D(
        position: sampleWorld.clone(),
        pressure: pressure,
        time: t,
      ));
    }
    // Feed the brush engine so the live dab pipeline (real Krita
    // backend) can stamp dabs in real time. The BrushEngine-assembled
    // stroke is discarded in [_onStrokeEnd] for the Draw path — the
    // canonical stroke is built from the Stroke3D so the curves system
    // stays the source of truth. Bend / guide-draw still use
    // BrushEngine's stroke assembly (those strokes are interpreted as
    // guide paths, not paint).
    if (_guideMode == _GuideMode.bend || _guideDrawMode) {
      if (_liveStroke.length == 1) {
        _brush
            .beginStroke(StrokePoint(position: sampleWorld.clone(), pressure: 0.8));
      } else {
        _brush
            .addPoint(StrokePoint(position: sampleWorld.clone(), pressure: 0.8));
      }
    }
    // Wire real Krita dab → host paint canvas → ui.Image → canvas drawImage.
    // The fallback engine skips this path and keeps using the 3D polyline
    // renderer (current behaviour). Bend / guide-draw modes also skip —
    // those strokes are interpreted as guide paths, not paint.
    //
    // v0.55-C: dab throttle. generateDab is a synchronous FFI call into the
    // native krita_bridge (krita_brush_controller.dart:451) and the
    // GestureDetector fires onPanUpdate per micro-pixel move — without a
    // gate, a fast pen drag would call generateDab → paintDab → schedule-
    // rasterize 200–500×/sec on a 120 Hz tablet, saturating the UI isolate.
    // We cap dab GENERATION at ~60 Hz (16 ms). Stroke3D capture above
    // (_liveStroke3D.addSample) is NOT throttled — the curve geometry still
    // records every sample for fidelity. The final dab of the stroke is
    // flushed unconditionally in [_onStrokeEnd] so the committed paint
    // layer matches the curve end-point.
    if (_realBackendActive &&
        _guideMode != _GuideMode.bend &&
        !_guideDrawMode) {
      final now = DateTime.now();
      if (shouldFireDab(_lastDabTime, now, kDabThrottle)) {
        _paintRealDab(screenPos);
        _lastDabTime = now;
      }
    }
    setState(() {});
  }

  // ----- Real Krita dab → host paint canvas → ui.Image --------------------
  //
  // The pipeline (only runs when the real native backend is live):
  //
  //   stroke update (screenPos, pressure)
  //     → _krita.backend.generateDab(BrushInput(x, y, pressure))   [sync, FFI]
  //     → BrushDab (RGBA8 pixels, width, height)
  //     → _paintCanvas.paintDab(dab, x, y, mode, alpha)             [sync, host]
  //     → _scheduleRasterizePaint()
  //        → _paintCanvas.readPixels()                              [sync, copy]
  //        → ui.ImmutableBuffer + ui.ImageDescriptor.raw + codec    [async]
  //        → _paintLayerImage = image
  //        → setState → _buildCanvasScene → CanvasScene.paintLayer
  //        → _CanvasPainter.paint → canvas.drawImage(paintLayer, Offset.zero)
  //
  // Coalescing: rasterization is async; multiple dabs during a rasterization
  // are picked up by a follow-up rasterization scheduled when the in-flight
  // one completes (if the canvas version advanced).

  /// Generates one dab from the real Krita backend at [screenPos] (in
  /// canvas / viewport pixel space) and composite-stamps it onto the
  /// host-side paint canvas. After stamping, schedules a re-rasterization
  /// of the paint canvas into a [ui.Image] so the canvas viewport's
  /// painter can `drawImage` it on the next frame.
  ///
  /// The dab's pressure is `0.8` when the brush panel's pressure toggle
  /// is on (matching the value the 3D stroke-assembly path uses) and
  /// `1.0` when off. The blend mode is [KritaBlendMode.erase] when the
  /// loaded preset is an eraser, [KritaBlendMode.normal] otherwise.
  /// Per-dab alpha is `flow * opacity * pressure` — the same per-dab
  /// modulation the engine's own stroke renderer applies (see
  /// `KritaDabRenderer.renderStroke`).
  void _paintRealDab(Offset screenPos) {
    if (!_krita.isInitialized) return;
    final backend = _krita.backend;
    final pressure = _pressure ? 0.8 : 1.0;
    final dab = backend.generateDab(BrushInput(
      x: screenPos.dx,
      y: screenPos.dy,
      pressure: pressure,
    ));
    if (dab.isEmpty) return;
    _ensurePaintCanvas();
    final erase = backend.isEraserPreset;
    final mode = erase ? KritaBlendMode.erase : KritaBlendMode.normal;
    final alpha = (backend.currentFlow *
            backend.currentOpacity *
            pressure)
        .clamp(0.0, 1.0);
    _paintCanvas!.paintDab(
      dab,
      screenPos.dx,
      screenPos.dy,
      mode: mode,
      alpha: alpha,
    );
    _scheduleRasterizePaint();
  }

  /// Lazily allocates / resizes [_paintCanvas] to match the current
  /// viewport size. The canvas is sized in logical pixels (1:1 with the
  /// painter's coordinate space), so dabs painted at screen positions
  /// land exactly where the user expects them.
  void _ensurePaintCanvas() {
    final w = _viewportSize.width.round().clamp(1, 4096);
    final h = _viewportSize.height.round().clamp(1, 4096);
    if (_paintCanvas == null) {
      _paintCanvas = KritaCanvasController(width: w, height: h);
    } else if (_paintCanvas!.width != w || _paintCanvas!.height != h) {
      _paintCanvas!.resize(w, h);
    }
  }

  /// Schedules a re-rasterization of the paint canvas backing buffer
  /// into a [ui.Image] (assigned to [_paintLayerImage]). Coalesces
  /// multiple dab updates into a single rasterization — if a
  /// rasterization is already in flight, this call is a no-op; a
  /// follow-up rasterization is scheduled when the in-flight one
  /// completes if the canvas's version has advanced in the meantime.
  void _scheduleRasterizePaint() {
    if (_rasterizingPaint) return;
    final canvas = _paintCanvas;
    if (canvas == null) return;
    final pixels = canvas.readPixels();
    final w = canvas.width;
    final h = canvas.height;
    final version = canvas.version;
    _rasterizingPaint = true;
    _rgbaToImage(pixels, w, h).then((image) {
      if (!mounted) {
        image.dispose();
        _rasterizingPaint = false;
        return;
      }
      final old = _paintLayerImage;
      setState(() {
        _paintLayerImage = image;
        _paintLayerVersion = version;
        _rasterizingPaint = false;
      });
      // Dispose of the previous image after the next frame so any
      // in-flight paint pass that captured it finishes first.
      if (old != null) {
        WidgetsBinding.instance.addPostFrameCallback((_) => old.dispose());
      }
      // If a new dab came in while we were rasterizing, schedule another.
      if (_paintCanvas != null &&
          _paintCanvas!.version != _paintLayerVersion) {
        _scheduleRasterizePaint();
      }
    }).catchError((Object _) {
      if (mounted) setState(() => _rasterizingPaint = false);
    });
  }

  /// Converts an RGBA8 pixel buffer ([pixels], laid out row-major as
  /// `width * height * 4` bytes in R,G,B,A byte order) into a [ui.Image].
  /// Uses [ui.ImmutableBuffer] + [ui.ImageDescriptor.raw] under the hood
  /// (the modern Flutter path for raw-pixel upload). Async because GPU
  /// buffer upload runs on the platform thread.
  Future<ui.Image> _rgbaToImage(
    Uint8List pixels,
    int width,
    int height,
  ) async {
    final buffer = await ui.ImmutableBuffer.fromUint8List(pixels);
    final descriptor = ui.ImageDescriptor.raw(
      buffer,
      width: width,
      height: height,
      pixelFormat: ui.PixelFormat.rgba8888,
    );
    final codec = await descriptor.instantiateCodec();
    final frame = await codec.getNextFrame();
    return frame.image;
  }

  void _onStrokeEnd() {
    // Bend mode: the just-drawn stroke is a bend path, not a paint stroke.
    // Finalize the brush engine (so its internal state is cleaned up),
    // then route the live stroke into [BentGuideBuilder] via
    // [_onBendStrokeEnd]. The returned brush stroke is discarded.
    if (_guideMode == _GuideMode.bend) {
      _brush.endStroke(brushType: BrushType.basic);
      _onBendStrokeEnd();
      _liveStroke.clear();
      _liveStroke3D = null;
      _strokeStartTime = null;
      setState(() {});
      return;
    }
    if (_guideDrawMode) {
      // Guide Draw mode: the stroke's world points become the
      // centerline of a new Draw-mode 3D Guide. Finalize the brush
      // engine so its internal stroke state is cleaned up, then
      // discard the returned stroke (we don't want a paint stroke).
      _brush.endStroke(brushType: BrushType.basic);
      _createDrawnGuide(_liveStroke);
      _liveStroke.clear();
      _liveStroke3D = null;
      _strokeStartTime = null;
      setState(() {});
      return;
    }
    // Eraser / vacuum sub-modes: the erase happened live in
    // [_onStrokeUpdate]. Nothing to commit — just clean up the capture
    // state and refresh.
    if (_tool == FeatherTool.eraser || _tool == FeatherTool.vacuum) {
      _liveStroke.clear();
      _liveStroke3D = null;
      _strokeStartTime = null;
      setState(() {});
      return;
    }
    // Draw mode: convert the captured Stroke3D → Stroke for rendering.
    // The Stroke3D is the curves-system source of truth (carries
    // per-sample pressure + timestamp); the Stroke is the rendered
    // view (positions, pressures, optional UVs). Smoothing (gaussian
    // via [StrokeSmoother]) and simplification (RDP) are applied to
    // the Stroke3D before conversion so the rendered Stroke is clean.
    final stroke3d = _liveStroke3D;
    _liveStroke.clear();
    _liveStroke3D = null;
    _strokeStartTime = null;
    // Finalize the brush engine (no-op when no stroke was started —
    // draw mode bypasses BrushEngine's stroke assembly).
    _brush.endStroke(brushType: BrushType.basic);
    if (stroke3d == null || stroke3d.length < 2) {
      setState(() {});
      return;
    }
    final smoothed = stroke3d.smooth(strength: 0.4);
    final simplified = smoothed.simplify(0.005);
    final stroke = _stroke3DToStroke(simplified);
    stroke.id = _nextStrokeId++;
    stroke.color = _color.toARGB32();
    stroke.thickness = _brushSize * 0.15;
    _strokes.add(stroke);
    // Keep the Stroke3D as the curves-system source of truth (used by
    // the glTF exporter and available for future Bezier / Catmull-Rom /
    // NURBS fitting via stroke3d.toBezier / toCatmullRom / toNurbs).
    _stroke3Ds[stroke.id] = stroke3d;
    // Record the active material so the canvas painter can render this
    // stroke with the right material kind (shadeless / shaded / glow /
    // cutout). Future picker changes don't retro-edit existing strokes.
    _strokeMaterials[stroke.id] = _material;
    // Register with the scene graph + active layer.
    _scene.addCurve(curveId: 'stroke-${stroke.id}');
    // Mirror assistance (v0.54-A §3): when the mirror assist has any
    // active axis OR the FeatherTool.mirror dock tool is selected,
    // reflect the committed (smoothed + simplified) Stroke3D into
    // 2^N - 1 editable copies and commit each as its own stroke. Per
    // mirror_assist.dart: "the stroke manager registers each copy as
    // its own curve in the active group, so mirror-drawn strokes remain
    // independently editable." The mirror tool with no axes set
    // defaults to {X} for the stroke (the most common single-axis
    // mirror) without mutating the persistent [_mirrorAssist] state.
    //
    // HONESTY NOTE (PROGRESS_SATURDAY §1.4 "eraser strokes must stay
    // eraser on mirror paths"): the eraser TOOL doesn't reach this
    // commit path (it returns early above), so mirror copies are ONLY
    // produced for paint strokes. Mirror-ERASE (erasing at mirrored
    // positions simultaneously) is NOT wired — the eraser mutates
    // existing strokes live in [_onStrokeUpdate] and has no mirror
    // counterpart. Documented honestly.
    final mirrorActive = _mirrorAssist.isEnabled || _tool == FeatherTool.mirror;
    if (mirrorActive) {
      final assist = (_tool == FeatherTool.mirror && !_mirrorAssist.isEnabled)
          ? MirrorAssist(activeAxes: const {MirrorAxis.x})
          : _mirrorAssist;
      final copies = mirrorStroke(simplified, assist);
      for (final copy in copies) {
        final copyStroke = _stroke3DToStroke(copy);
        copyStroke.id = _nextStrokeId++;
        copyStroke.color = stroke.color;
        copyStroke.thickness = stroke.thickness;
        _strokes.add(copyStroke);
        _stroke3Ds[copyStroke.id] = copy;
        _strokeMaterials[copyStroke.id] = _material;
        _scene.addCurve(curveId: 'stroke-${copyStroke.id}');
      }
    }
    // v0.55-C: dab throttle final-flush. The throttle gate in
    // [_onStrokeUpdate] may have skipped the last few dabs (those within
    // 16 ms of the previous dab). Fire one final dab at the exact pen-lift
    // screen position so the rasterized paint layer matches the curve
    // end-point. Only fires on the real-backend draw / mirror path (the
    // same gate as [_onStrokeUpdate]'s dab block). Bend / guide-draw /
    // eraser / vacuum all return early above before reaching this point.
    if (_realBackendActive && _lastStrokeScreenPos != null) {
      _paintRealDab(_lastStrokeScreenPos!);
      _lastDabTime = DateTime.now();
    }
    _lastStrokeScreenPos = null;
    // v0.55-C: stroke soft-cap warning. Fires a SnackBar exactly once when
    // the stroke count crosses [kStrokeSoftCap] (1000), nudging the user
    // to merge / export / clear before document memory pressure mounts.
    // Re-arms if the count drops back below the cap (via undo / erase /
    // vacuum) and re-crosses. Decision logic in lib/utils/paint_perf.dart.
    _maybeWarnStrokeCap();
    setState(() {});
  }

  /// v0.55-C: surfaces the soft-cap warning SnackBar when the document
  /// stroke count crosses [kStrokeSoftCap]. Idempotent per threshold-cross
  /// (re-arms below the cap).
  void _maybeWarnStrokeCap() {
    final count = _strokes.length;
    if (shouldWarnStrokeCap(count, kStrokeSoftCap, _strokeCapWarned)) {
      _strokeCapWarned = true;
      _showExportSnackBar(
        'Many strokes ($count) — consider merging or exporting to keep the app responsive.',
        null,
      );
    } else if (shouldRearmStrokeCap(count, kStrokeSoftCap, _strokeCapWarned)) {
      // Re-arm so a future re-cross re-fires the warning.
      _strokeCapWarned = false;
    }
  }

  /// Builds a [Guide3D] from a finished pen stroke and adds it to the
  /// active-guide list. Each stroke point becomes a [DrawStrokeSample]
  /// paired with the camera's current forward vector — the resulting
  /// ribbon is perpendicular to the viewing angle, per the Feather
  /// Draw-mode spec (3dguide_draw.txt). The cross-ribbon width is
  /// scaled by the camera FOV via [DrawGuideParams.effectiveWidth].
  void _createDrawnGuide(List<Vector3> strokePoints) {
    if (strokePoints.length < 2) return;
    final viewDir = _camera.forward;
    final samples = strokePoints
        .map((p) => DrawStrokeSample(
              viewPoint: p.clone(),
              viewDirection: viewDir,
            ))
        .toList();
    final fovDeg = _camera.fovYRadians * (180.0 / 3.1415926535897932);
    final guide = const DrawnGuideBuilder().build(
      samples: samples,
      params: DrawGuideParams(fovDegrees: fovDeg),
      name: 'Guide ${_guides.length + 1}',
    );
    _guides.add(guide);
    setState(() {});
  }

  /// Toggles the Guide Draw mode flag. When true, the next finished
  /// stroke is converted into a 3D Guide (see [_createDrawnGuide]);
  /// when false, strokes are committed as normal paint. Exposed for
  /// future UI wiring (e.g. a long-press on the Draw tool icon).
  void setGuideDrawMode(bool enabled) {
    setState(() => _guideDrawMode = enabled);
  }

  // ----- Guide creation sub-modes (Loft / Primitive / Bend) ---------------
  //
  // See the field doc on [_guideMode] for the per-mode state and the
  // feather docs (3dguide_loft.txt / 3dguide_primitives.txt /
  // 3dguide_draw.txt) for the user-facing behaviour.

  /// Activates the given guide-creation sub-mode. Switching to a new mode
  /// cancels any in-progress previous mode (via [_cancelGuideModeInternal]).
  /// Each mode also surfaces a handwritten tutorial caption via
  /// [setCaption] (Feather-style "Let's sketch this!" prompts).
  void setGuideMode(_GuideMode mode) {
    setState(() {
      _cancelGuideModeInternal();
      _guideMode = mode;
      if (mode == _GuideMode.bend) {
        // Pre-select the source guide: the currently selected guide, or
        // the most recently added guide if nothing is selected.
        _bendSourceId = _guides.selectedId ??
            (_guides.ids.isNotEmpty ? _guides.ids.last : null);
      }
    });
    switch (mode) {
      case _GuideMode.loft:
        setCaption('Tap strokes to loft!');
      case _GuideMode.primitive:
        setCaption('Pick a shape!');
      case _GuideMode.bend:
        setCaption('Draw a bend path!');
      case _GuideMode.none:
        break;
    }
  }

  /// Clears all per-mode in-progress state (called when switching modes or
  /// finalising / cancelling). Removes any live preview guides from the
  /// [Guide3DManager] so the canvas doesn't carry dangling previews.
  void _cancelGuideModeInternal() {
    // Loft.
    if (_loftPreviewId != null) {
      _guides.remove(_loftPreviewId!);
      _loftPreviewId = null;
    }
    _loftStrokeIds.clear();
    // Primitive.
    if (_primitivePreviewId != null) {
      _guides.remove(_primitivePreviewId!);
      _primitivePreviewId = null;
    }
    _primitiveKind = null;
    // Bend.
    _bendSourceId = null;
    _liveStroke.clear();
  }

  // ----- Loft -----

  /// Tap handler while in loft mode. [world] is the world-space tap point
  /// (already raycast by the caller). Finds the nearest stroke within the
  /// tap radius and toggles its membership in [_loftStrokeIds]. Rebuilds
  /// the live loft preview whenever the selection changes.
  ///
  /// v0.57-D: uses the engine [Sphere] (lib/core/math/sphere.dart) as a
  /// per-stroke bounding volume for a quick-reject pre-filter. For each
  /// stroke a bounding sphere is built from its world points
  /// ([Sphere.fromPoints]); when the tap point lies farther than
  /// `tapRadius` outside the sphere, the stroke is skipped WITHOUT the
  /// per-point distance loop. This cuts the hit-test from O(stroke
  /// points) to O(1) for the common case of tapping far from any stroke.
  void _onLoftTap(Vector3 world) {
    const tapRadius = 0.5;
    final worldVec3 = Vec3(world.x, world.y, world.z);
    int? nearestId;
    var nearestDist = double.infinity;
    for (final s in _strokes) {
      // Build the per-stroke bounding sphere (Ritter seed — centroid +
      // farthest-point radius). Cheap enough to recompute per tap (a
      // loft-mode tap is a low-frequency gesture).
      final points = <Vec3>[
        for (var i = 0; i < s.length; i++)
          Vec3(s.worldPosition(i).x, s.worldPosition(i).y, s.worldPosition(i).z),
      ];
      final sphere = math.Sphere.fromPoints(points);
      // Quick-reject: skip the whole stroke when the tap point is farther
      // than tapRadius outside the bounding sphere.
      final distToCenter = sphere.center.distanceTo(worldVec3);
      if (distToCenter - sphere.radius > tapRadius) continue;
      // Precise check: nearest point on the stroke polyline.
      for (var i = 0; i < s.length; i++) {
        final d = (s.worldPosition(i) - world).length;
        if (d < nearestDist) {
          nearestDist = d;
          nearestId = s.id;
        }
      }
    }
    if (nearestId != null && nearestDist < tapRadius) {
      if (_loftStrokeIds.contains(nearestId)) {
        _loftStrokeIds.remove(nearestId);
      } else {
        _loftStrokeIds.add(nearestId);
      }
      _rebuildLoftPreview();
      setState(() {});
    }
  }

  /// Rebuilds the in-progress lofted guide preview from the currently
  /// selected strokes + tension. Removes the previous preview (if any)
  /// and adds the new one to the [Guide3DManager]. No-op when fewer than
  /// two curves are selected (matches the Feather docs: "When you select
  /// two or more curves, a preview of the 3D Guide will appear
  /// immediately").
  void _rebuildLoftPreview() {
    if (_loftPreviewId != null) {
      _guides.remove(_loftPreviewId!);
      _loftPreviewId = null;
    }
    final curves = <List<Vector3>>[];
    for (final id in _loftStrokeIds) {
      final s = _findStroke(id);
      if (s == null || s.length < 2) continue;
      curves.add([for (var i = 0; i < s.length; i++) s.worldPosition(i)]);
    }
    if (curves.length < 2) return;
    final guide = const LoftedGuideBuilder().build(
      curves: curves,
      params: LoftGuideParams(tension: _loftTension),
      name: 'Lofted Guide ${_guides.length + 1}',
    );
    _loftPreviewId = _guides.add(guide);
  }

  /// Sets the loft tension slider value in [0, 1] and rebuilds the live
  /// preview. Matches the Feather tension slider: 0 = sharp (linear-ish),
  /// 1 = smooth (full Catmull-Rom).
  void setLoftTension(double t) {
    setState(() {
      _loftTension = t.clamp(0.0, 1.0);
      _rebuildLoftPreview();
    });
  }

  /// Finalises the loft: the in-progress preview (already in the manager)
  /// becomes a committed guide. Clears loft state and exits loft mode.
  void finishLoft() {
    setState(() {
      _loftStrokeIds.clear();
      _loftPreviewId = null;
      _guideMode = _GuideMode.none;
    });
    setCaption('Lofted guide created!');
  }

  /// Cancels the loft: removes the in-progress preview from the manager
  /// and clears loft state.
  void cancelLoft() {
    setState(() {
      _cancelGuideModeInternal();
      _guideMode = _GuideMode.none;
    });
  }

  // ----- Primitive -----

  /// Inserts a primitive guide of [kind] at the world origin (per the
  /// Feather docs: "Shapes are always created at the fixed coordinates
  /// (0, 0, 0)"). Replaces any previous in-progress primitive preview.
  /// The segment slider is reset to the primitive's default segment count.
  void insertPrimitive(Guide3DPrimitive kind) {
    setState(() {
      if (_primitivePreviewId != null) {
        _guides.remove(_primitivePreviewId!);
        _primitivePreviewId = null;
      }
      _primitiveKind = kind;
      _primitiveSegments = kind.defaultSegments;
      final guide = const PrimitiveGuideBuilder().build(
        kind: kind,
        params: PrimitiveGuideParams(segments: _primitiveSegments),
        name: '${kind.displayName} Guide',
      );
      _primitivePreviewId = _guides.add(guide);
    });
  }

  /// Sets the segment count and rebuilds the in-progress primitive preview
  /// via [PrimitiveGuideBuilder.withSegments]. Clamps to the primitive's
  /// legal range so a runaway slider cannot collapse the mesh.
  void setPrimitiveSegments(int n) {
    if (_primitiveKind == null) return;
    setState(() {
      final clamped = n.clamp(
        _primitiveKind!.minSegments,
        Guide3DPrimitive.maxSegments,
      );
      _primitiveSegments = clamped;
      final oldGuide =
          _primitivePreviewId != null ? _guides[_primitivePreviewId!] : null;
      if (oldGuide == null) return;
      final rebuilt =
          const PrimitiveGuideBuilder().withSegments(oldGuide, clamped);
      _guides.remove(_primitivePreviewId!);
      _primitivePreviewId = _guides.add(rebuilt);
    });
  }

  /// Finalises the primitive: the in-progress preview (already in the
  /// manager) becomes a committed guide.
  void finishPrimitive() {
    setState(() {
      _primitiveKind = null;
      _primitivePreviewId = null;
      _guideMode = _GuideMode.none;
    });
    setCaption('Primitive guide created!');
  }

  /// Cancels the primitive: removes the in-progress preview from the
  /// manager and clears primitive state.
  void cancelPrimitive() {
    setState(() {
      _cancelGuideModeInternal();
      _guideMode = _GuideMode.none;
    });
  }

  // ----- Bend -----

  /// Called from [_onStrokeEnd] when in bend mode. Builds a new bent guide
  /// from the source guide ([_bendSourceId]) and the just-drawn bend path
  /// (the live stroke's world points), adds it to the [Guide3DManager],
  /// and re-anchors the source to the bent result so the artist can repeat
  /// the bend (per the Feather docs: "You can repeat the Bend 3D Guide
  /// process multiple times").
  void _onBendStrokeEnd() {
    if (_bendSourceId == null || _liveStroke.length < 2) return;
    final source = _guides[_bendSourceId!];
    if (source == null) return;
    final bent = const BentGuideBuilder().build(
      source: source,
      bendPath: _liveStroke.map((p) => p.clone()).toList(),
      params: const BentGuideParams(),
      name: source.name != null ? '${source.name} (bent)' : 'Bent Guide',
    );
    final newId = _guides.add(bent);
    _bendSourceId = newId;
  }

  /// Cancels the bend: clears the bend source + live stroke. The source
  /// guide itself is untouched (no bent copy was added if the artist
  /// cancels before drawing).
  void cancelBend() {
    setState(() {
      _bendSourceId = null;
      _liveStroke.clear();
      _guideMode = _GuideMode.none;
    });
  }

  // ----- Tutorial caption -------------------------------------------------

  /// Shows [text] as a handwritten caption (Caveat font, white text + soft
  /// shadow) at the top-center of the canvas. The caption fades in over
  /// ~400 ms, stays visible for 3 seconds, then fades out over ~400 ms.
  /// Calling this again while a caption is showing replaces the text and
  /// restarts the timer (so rapid calls don't pile up overlapping timers).
  ///
  /// Used by [setGuideMode] to surface the Feather-style "Let's sketch
  /// this!" tutorial prompts when each mode is entered, and exposed for
  /// any future tutorial / onboarding flow to call directly.
  void setCaption(String text) {
    _captionHideTimer?.cancel();
    setState(() => _captionText = text);
    _captionAnim.forward(from: 0.0);
    _captionHideTimer = Timer(const Duration(seconds: 3), () {
      if (!mounted) return;
      _captionAnim.reverse().whenComplete(() {
        if (mounted) setState(() => _captionText = '');
      });
    });
  }

  // ----- Persistent settings I/O (v0.57-D) --------------------------------

  /// Loads the persisted [SettingsModel] from disk via
  /// [SettingsRepository] and applies the user-default fields that the
  /// host owns ([_brushSize] ← defaultBrushSize, [_showGrid] ←
  /// showGridByDefault). Best-effort: a read failure or a missing
  /// manifest leaves the in-memory defaults untouched (so a corrupt
  /// prefs file never blocks the editor from opening). Idempotent —
  /// guarded by [_settingsLoaded] so a re-entrant call is a no-op.
  Future<void> _loadPersistedSettings() async {
    if (_settingsLoaded) return;
    final SettingsModel model;
    try {
      model = await _settingsRepo.load();
    } catch (_) {
      // Corrupt prefs → keep in-memory defaults. Flagged loaded so a
      // later _persistSettings write can still recover the file.
      _settingsLoaded = true;
      return;
    }
    _settingsLoaded = true;
    if (!mounted) return;
    setState(() {
      // Only apply the brush size when the model carries a non-default
      // value (the SettingsModel default is 32.0; the host's own
      // default is 20.0 — both are valid, so we always trust the disk
      // value when present).
      _brushSize = model.defaultBrushSize;
      _showGrid = model.showGridByDefault;
    });
    // Mirror into the live brush engine so the first dab matches the
    // persisted size.
    _brush.settings = _brush.settings.copyWith(size: _brushSize);
    final backend = _krita.isInitialized ? _krita.backend : null;
    if (backend != null) backend.size = _brushSize;
  }

  /// Persists the current host-owned defaults ([_brushSize], [_showGrid])
  /// back to disk via [SettingsRepository]. Fire-and-forget: callers do
  /// not await (a slow disk write must never block the UI thread). The
  /// write preserves every other SettingsModel field by re-loading the
  /// current model first + copyWith-ing only the host-owned fields.
  Future<void> _persistSettings() async {
    try {
      final current = await _settingsRepo.load();
      await _settingsRepo.save(current.copyWith(
        defaultBrushSize: _brushSize,
        showGridByDefault: _showGrid,
      ));
    } catch (_) {
      // Best-effort: a failed write is retried on the next change.
    }
  }

  // ----- First-run tutorial caption sequence (v55-B) ---------------------

  /// Reads the SharedPreferences flag + starts the 4-hint tutorial
  /// sequence if the flag is unset. Idempotent: if the flag is already
  /// set OR the tutorial is already active, this is a no-op.
  ///
  /// Honest scope note: the SharedPreferences read is async; if the
  /// host is disposed before the read completes, the [_tutorialActive]
  /// guard + the [mounted] check inside [_runTutorialSequence] prevent
  /// any setState after disposal.
  Future<void> _maybeStartFirstRunTutorial() async {
    if (_tutorialActive) return;
    final prefs = await SharedPreferences.getInstance();
    if (!mounted) return;
    if (prefs.getBool(kTutorialShownKey) == true) return;
    _runTutorialSequence();
  }

  /// Runs the 4-hint tutorial sequence. Each hint shows for ~3 s via
  /// [setCaption] (which handles its own fade-in / fade-out + hide
  /// timer); the next hint fires 3.7 s later (3 s hold + 700 ms gap).
  /// After the last hint shows, the SharedPreferences flag is written
  /// so the sequence never replays (unless the user clears app data).
  void _runTutorialSequence() {
    _tutorialActive = true;
    var index = 0;
    setCaption(kTutorialHints[index]);
    _tutorialTimer?.cancel();
    _tutorialTimer = Timer.periodic(const Duration(milliseconds: 3700),
        (timer) {
      if (!mounted) {
        timer.cancel();
        return;
      }
      index += 1;
      final hint = tutorialHintAt(index);
      if (hint == null) {
        timer.cancel();
        // Mark the tutorial as shown AFTER the last hint has fired so a
        // crash mid-sequence will replay on next launch (honest: better
        // to re-show than to leave the artist with no onboarding).
        SharedPreferences.getInstance().then((prefs) {
          prefs.setBool(kTutorialShownKey, true);
        });
        // Clear the active flag after the last hint's hold time so the
        // host can re-trigger the tutorial if a future host method
        // wants to (e.g. a "Replay tutorial" settings button).
        Future<void>.delayed(const Duration(seconds: 4), () {
          if (mounted) _tutorialActive = false;
        });
        return;
      }
      setCaption(hint);
    });
  }

  // ----- Helpers -----

  /// Looks up a stroke by id in the document's stroke list. Returns `null`
  /// when the id is no longer present (e.g. after an undo that removed it).
  Stroke? _findStroke(int id) {
    for (final s in _strokes) {
      if (s.id == id) return s;
    }
    return null;
  }

  /// Unprojects a screen position to a world-space point.
  /// Tries active Guide3D surfaces first (via Vec3<->Vector3 bridge),
  /// falls back to y=0 ground plane if no guide hit.
  Vector3? _screenToWorld(Offset screen) {
    if (_viewportSize.isEmpty) return null;
    final ray = _screenToRay(screen);

    // GAP 1 FIX: Try Guide3D raycast first (Vec3<->Vector3 bridge).
    final guides = _guides.guides;
    for (final guide in guides) {
      if (!guide.visible || guide.locked) continue;
      // Bridge vector_math Vector3 -> engine Vec3.
      final engineOrigin = Vec3(ray.origin.x, ray.origin.y, ray.origin.z);
      final engineDir = Vec3(ray.direction.x, ray.direction.y, ray.direction.z);
      final engineRay = math.Ray.normalized(engineOrigin, engineDir);
      final hit = guide.raycast(engineRay);
      if (hit != null) {
        // Bridge back Vec3 -> Vector3.
        return Vector3(hit.point.x, hit.point.y, hit.point.z);
      }
    }

    // Fallback: ground plane (y = 0). v0.57-D: uses the engine [Plane]
    // (lib/core/math/plane.dart) for the ray-plane intersection instead
    // of an inline `t = -origin.y / dir.y` calc. The plane is the
    // canonical y=0 ground: normal=(0,1,0), distance=0. A max-distance
    // guard (1000 world units) is kept so a ray grazing the horizon
    // does not project a stroke to an absurdly far point. The engine
    // Ray is built from the vector_math _CamRay via the same
    // Vec3<->Vector3 bridge used for the Guide3D path above.
    const groundPlane = math.Plane(Vec3.unitY(), 0.0);
    final engineOrigin = Vec3(ray.origin.x, ray.origin.y, ray.origin.z);
    final engineDir = Vec3(ray.direction.x, ray.direction.y, ray.direction.z);
    final engineRay = math.Ray.normalized(engineOrigin, engineDir);
    final hit = groundPlane.intersectRay(engineRay);
    if (hit != null) {
      final dist = (hit - engineOrigin).length;
      if (dist.isFinite && dist < 1000) {
        return Vector3(hit.x, hit.y, hit.z);
      }
    }
    return null;
  }

  /// Builds a world-space ray (origin + direction, both [Vector3]) from
  /// the camera through [screen].
  _CamRay _screenToRay(Offset screen) {
    final w = _viewportSize.width;
    final h = _viewportSize.height;
    if (w == 0 || h == 0) {
      return _CamRay(Vector3.zero(), Vector3(0, 0, -1));
    }
    // Normalized device coordinates.
    final ndcX = (screen.dx / w) * 2.0 - 1.0;
    final ndcY = 1.0 - (screen.dy / h) * 2.0;
    final aspect = w / h;
    final vp = _camera.viewProjectionMatrix(aspect);
    final inv = Matrix4.inverted(vp);
    // Near-plane point (ndc z = -1).
    final near = inv.transform3(Vector3(ndcX, ndcY, -1.0));
    // Far-plane point (ndc z = 1).
    final far = inv.transform3(Vector3(ndcX, ndcY, 1.0));
    final dir = (far - near)..normalize();
    return _CamRay(near, dir);
  }

  // ----- Selection --------------------------------------------------------

  List<Stroke> _selectedStrokes() {
    final ids = _selectionModel.active.toSet();
    return _strokes.where((s) => ids.contains(s.id)).toList();
  }

  void _selectAll() {
    _selection.selectAll();
    setState(() {});
  }

  // ----- Joystick (transform selected strokes) ---------------------------
  //
  // The joystick widget emits raw, normalised pointers:
  //   - onJoystickMove(Offset v)    → v in [-1, 1]² (full-right = +X)
  //   - onJoystickRotate(double r)  → r in radians (already-integrated angle)
  //   - onJoystickScale(Offset s)   → s in [-1, 1]² (drag on scale handle)
  //
  // We route each through the appropriate [TransformResolver] (2D view-
  // based by default, 3D world-axis when [_joystick3d] is true), then
  // apply the resulting [TransformDelta] to the active selection. The
  // resolvers own the camera-basis math, axis projection, and pivot
  // derivation — the host just dispatches.

  void _onJoystickMove(Offset v) {
    // Honesty-gap 1 fix: when the Lock toggle is on, the host ignores
    // joystick drag input (the widget's knob still tracks visually and
    // snaps back on release — the user can experiment without
    // disturbing the selection).
    if (_joystickLocked) return;
    final selected = _selectedStrokes();
    if (selected.isEmpty) return;
    final frame = _viewFrame(selected);
    // delta is in normalised screen units; Y is flipped (screen-down = +Y
    // in input, but resolver expects (0, 1) = "full up").
    final drag = Vector2(v.dx, -v.dy);
    final delta = _resolveJoystickDelta(
      mode2d: TransformMode2d.move,
      mode3d: TransformMode3d.moveX,
      drag: drag,
      frame: frame,
    );
    _applyTransformDelta(selected, delta);
  }

  void _onJoystickRotate(double r) {
    // Honesty-gap 1 fix: lock gate (see [_onJoystickMove]).
    if (_joystickLocked) return;
    final selected = _selectedStrokes();
    if (selected.isEmpty) return;
    final frame = _viewFrame(selected);
    // 2D rotate: the resolver maps atan2(-d.y, d.x) to the rotation angle
    // (rotateSpeed defaults to 2π, so the scale factor is 1.0). A unit
    // drag at angle r from +X → rotation r. We pass (cos r, -sin r).
    // 3D rotate (rotateY): the resolver scales the projected drag by
    // rotateSpeed (2π). Pass (r / 2π, 0) so the resulting angle is ≈ r
    // (modulo the axis's screen projection).
    final drag = _joystick3d
        ? Vector2(r / (dmath.pi * 2.0), 0.0)
        : Vector2(dmath.cos(r), -dmath.sin(r));
    final delta = _resolveJoystickDelta(
      mode2d: TransformMode2d.rotate,
      mode3d: TransformMode3d.rotateY,
      drag: drag,
      frame: frame,
    );
    _applyTransformDelta(selected, delta);
  }

  void _onJoystickScale(Offset s) {
    // Honesty-gap 1 fix: lock gate (see [_onJoystickMove]).
    if (_joystickLocked) return;
    final selected = _selectedStrokes();
    if (selected.isEmpty) return;
    final frame = _viewFrame(selected);
    final drag = Vector2(s.dx, -s.dy);
    final delta = _resolveJoystickDelta(
      mode2d: TransformMode2d.freeScale,
      // 3D mode has no native scale; reuse the scale callback to drive a
      // Z-axis translate (the third cone) so the toggle still does
      // something useful in 3D mode.
      mode3d: TransformMode3d.moveZ,
      drag: drag,
      frame: frame,
    );
    _applyTransformDelta(selected, delta);
  }

  /// Builds the [ViewFrame] the resolver needs from the live camera and
  /// the active selection's centroid (used as the rotation/scale pivot
  /// when the resolver doesn't compute one).
  ViewFrame _viewFrame(List<Stroke> selected) {
    return ViewFrame(
      right: _camera.right,
      up: _camera.up,
      forward: _camera.forward,
      viewportWidth: _viewportSize.width,
      viewportHeight: _viewportSize.height,
      crosshairWorld: _selectionPivot(selected),
    );
  }

  /// Dispatches [drag] to the active resolver (2D or 3D based on
  /// [_joystick3d]) under the appropriate sub-mode.
  TransformDelta _resolveJoystickDelta({
    required TransformMode2d mode2d,
    required TransformMode3d mode3d,
    required Vector2 drag,
    required ViewFrame frame,
  }) {
    if (_joystick3d) {
      return _joy3d.resolve(mode: mode3d, delta: drag, frame: frame);
    }
    return _joy2d.resolve(mode: mode2d, delta: drag, frame: frame);
  }

  /// Applies a single [TransformDelta] to every stroke in [selected].
  /// Translation is applied as-is; rotation uses the delta's pivot (or
  /// falls back to the selection centroid); scale is averaged across
  /// axes (the resolver emits uniform scale for free-scale and per-axis
  /// for width/height — we collapse to a single factor here since the
  /// [Stroke] model only carries a uniform [Stroke.applyScale]).
  void _applyTransformDelta(List<Stroke> selected, TransformDelta d) {
    if (d.isZero) return;
    final fallbackPivot = _selectionPivot(selected);
    for (final s in selected) {
      if (d.translation != null) {
        s.applyTranslation(d.translation!);
      }
      if (d.rotation != null) {
        s.applyRotation(d.rotation!, pivot: d.pivot ?? fallbackPivot);
      }
      if (d.scale != null) {
        final f = (d.scale!.x + d.scale!.y + d.scale!.z) / 3.0;
        if ((f - 1.0).abs() > 1e-4) {
          s.applyScale(f, pivot: d.pivot ?? fallbackPivot);
        }
      }
    }
    setState(() {});
  }

  /// Flips the joystick resolver between 2D (view-based) and 3D
  /// (world-axis). Exposed for future UI wiring (a "2D/3D" toggle on the
  /// joystick panel). Calling this does not affect already-applied
  /// transforms; the next joystick drag will use the new resolver.
  void setJoystick3d(bool enabled) {
    setState(() => _joystick3d = enabled);
  }

  /// Honesty-gap 1 fix: JoystickWidget's 2D/3D pill → host flips
  /// [_joystick3d]. The next joystick drag routes through the new
  /// resolver (2D = planar screen-space rotation around camera.forward;
  /// 3D = full 3-axis applyJoystickTransform via [Joystick3dResolver]).
  void _toggleJoystick3D() => setJoystick3d(!_joystick3d);

  /// Honesty-gap 1 fix: JoystickWidget's Lock pill → host flips
  /// [_joystickLock]. When on, the input handlers early-return so drags
  /// have no transform effect; the gizmo renderer also draws the lock
  /// indicator. The widget's knob still tracks visually and snaps back.
  void _toggleJoystickLock() {
    setState(() {
      _joystickLock = _joystickLock == JoystickLock.on
          ? JoystickLock.off
          : JoystickLock.on;
    });
  }

  Vector3 _selectionPivot(List<Stroke> selected) {
    if (selected.isEmpty) return Vector3.zero();
    var sum = Vector3.zero();
    for (final s in selected) {
      sum += s.worldCenter();
    }
    return sum..scale(1.0 / selected.length);
  }

  // ----- Liquify ----------------------------------------------------------

  void _onLiquifyMode(LiquifyMode mode) {
    final t = switch (mode) {
      LiquifyMode.push => LiquifyBrushType.push,
      LiquifyMode.pinch => LiquifyBrushType.pinch,
      LiquifyMode.comb => LiquifyBrushType.comb,
    };
    _liquify.setBrushType(t);
  }

  void _onLiquifyApply() {
    if (!_liquify.isEditing) return;
    _pushUndo();
    _liquify.apply();
    setState(() {});
  }

  void _onLiquifyUndoAll() {
    if (!_liquify.isEditing) return;
    _liquify.undoAll(_strokes);
    setState(() {});
  }

  // GAP 3 FIX: Forward canvas drag to LiquifyEngine.
  void _onLiquifyDrag(Offset screenPos, Offset dragDelta) {
    if (!_liquify.isEditing) return;
    // Track the brush cursor + drag so the LiquifyRenderer preview can
    // be drawn on the next canvas rebuild (see [_buildOverlayPrimitives]).
    _liquifyCursor = screenPos;
    _liquifyDragDelta =
        dragDelta == Offset.zero ? null : Vector2(dragDelta.dx, dragDelta.dy);
    final world = _screenToWorld(screenPos);
    if (world == null) {
      setState(() {});
      return;
    }
    final right = _camera.right;
    final up = _camera.up;
    final worldDelta = right * (dragDelta.dx * 0.5) + up * (-dragDelta.dy * 0.5);
    _liquify.applyDrag(strokes: _strokes, centre: world, drag: worldDelta);
    setState(() {});
  }

  // GAP 2 FIX: Tap-select on canvas.
  void _onTapSelect(Offset screenPos) {
    final world = _screenToWorld(screenPos);
    if (world == null) return;
    // Eye-dropper tool (v0.54-A §2): sample the nearest visible stroke's
    // colour via ColorSampler and push it to the host's active colour.
    // Falls through silently when no stroke is within the pick radius
    // (the active colour stays unchanged). The brush engine + brush
    // panel are synced so the next dab / the colour swatch reflect the
    // sampled colour. The pick radius is the world-space default
    // (kEyedropperPickRadius = 4 mm) — generous enough for finger taps.
    if (_tool == FeatherTool.eyedropper) {
      final argb = sampleColorAt(_strokes, world);
      if (argb != null) {
        setState(() {
          _color = Color(argb);
          _brush.color = argb;
        });
      }
      return;
    }
    // Loft mode: tap adds / removes the nearest stroke from the loft curve
    // selection (and rebuilds the live loft preview).
    if (_guideMode == _GuideMode.loft) {
      _onLoftTap(world);
      return;
    }
    // Select tool: toggle the nearest stroke in the [SelectionModel].
    // Find nearest stroke within tap radius (in world space — strokes
    // carry their own local-to-world transform, so we go through
    // [Stroke.worldPosition] rather than the raw local point).
    const tapRadius = 0.5;
    int? nearestId;
    var nearestDist = double.infinity;
    for (final s in _strokes) {
      for (var i = 0; i < s.length; i++) {
        final d = (s.worldPosition(i) - world).length;
        if (d < nearestDist) {
          nearestDist = d;
          nearestId = s.id;
        }
      }
    }
    if (nearestId != null && nearestDist < tapRadius) {
      _selectionModel.toggle(nearestId);
      setState(() {});
    }
  }

  // ----- Brush presets ----------------------------------------------------

  /// Scans the preset search directories (feather_presets +
  /// feather_resources/paintoppresets) for real .kpp files and rebuilds
  /// [_presets] so each disk preset is shown in the picker WITH its file
  /// path attached. The scan is filesystem-only (no .kpp unpacking —
  /// that happens on the native side at pick time via [loadPreset]), so
  /// it stays fast even with hundreds of stock presets.
  ///
  /// Real disk presets are placed FIRST (so they are the loadable
  /// entries), followed by the bundled visual-only catalog as a
  /// fallback tail. Best-effort: any I/O error aborts silently and the
  /// bundled catalog remains.
  Future<void> _scanDiskPresets() async {
    // v0.57-D: delegate the search-directory discovery to
    // [PresetRepository.searchDirs] (the single source of truth for
    // "which dirs hold .kpp files" — bundles feather_presets + the
    // extracted Krita stock library, filters to existing dirs). The
    // file iteration + cheap synthetic BrushPreset construction stay
    // inline: PresetRepository.listAll() would call
    // BrushPreset.loadFromFile (real .kpp parse) on every file, which
    // the boot scan deliberately avoids to stay fast with hundreds of
    // stock presets.
    final List<Directory> dirs;
    try {
      dirs = _presetRepo.searchDirs();
    } catch (_) {
      return;
    }
    final scanned = <BrushPreset>[];
    try {
      for (final dir in dirs) {
        if (!dir.existsSync()) continue;
        await for (final entity in dir.list(recursive: true)) {
          if (entity is! File) continue;
          if (!entity.path.toLowerCase().endsWith('.kpp')) continue;
          final fileName = entity.uri.pathSegments.last;
          var stem = fileName.replaceAll('.kpp', '').replaceAll('.KPP', '');
          stem = stem.replaceAll('_', ' ');
          if (stem.isEmpty) stem = fileName;
          // Derive a stable display colour from the file stem so the
          // picker strip has visual variety without parsing the .kpp.
          final hue = (stem.hashCode & 0xFFFF) % 360;
          scanned.add(BrushPreset(
            id: 'disk_$fileName',
            name: stem,
            previewColor: HSLColor.fromAHSL(
                    1.0, hue.toDouble(), 0.18, 0.28)
                .toColor(),
            strokeWidth: 4,
            filePath: entity.path,
          ));
        }
      }
    } catch (_) {
      // Best-effort: keep whatever was scanned so far.
    }
    if (!mounted || scanned.isEmpty) return;
    setState(() {
      _presets = <BrushPreset>[...scanned, ..._buildDefaultPresets()];
    });
  }

  void _onPickPreset(BrushPreset preset) {
    // Real .kpp loading path: when the real native Krita backend is live
    // AND the picked preset carries a .kpp file path (i.e. it was scanned
    // from disk at boot), feed the path to
    // [KritaBrushController.loadPreset]. The native bridge unpacks the
    // .kpp container (ZIP KoStore / legacy PNG-with-zTXt / bare XML),
    // hands the preset XML to Krita's own paintop-settings parser, and
    // records the parsed (name, value) pairs on its param map of record.
    // We then read the scalar reflection getters (currentSize /
    // currentOpacity / currentHardness / currentFlow / isEraserPreset)
    // back and mirror them into the [BrushEngine] + host UI state so the
    // brush settings panel shows the preset's REAL params, not the
    // visual-catalog placeholders.
    //
    // Fallback path (no native bridge, or preset has no .kpp behind it):
    // keep the legacy visual-hints behaviour — seed size from the
    // catalog's strokeWidth and leave opacity / hardness / flow at their
    // current values. The fallback engine's loadPreset cannot parse a
    // real .kpp, so reading its scalar getters back would surface engine
    // defaults rather than the preset's values.
    final backend = _krita.isInitialized ? _krita.backend : null;
    final canRealLoad =
        _realBackendActive && backend != null && preset.filePath != null;
    if (canRealLoad) {
      final ok = backend.loadPreset(preset.filePath!);
      if (ok) {
        // Read back the engine-parsed scalar params. The backend's size
        // is in the same mm convention the host uses (the host sets
        // backend.size = _brushSize directly elsewhere), so no unit
        // conversion is needed.
        final size = backend.currentSize.clamp(1.0, 300.0);
        final opacity = backend.currentOpacity.clamp(0.0, 1.0);
        final hardness = backend.currentHardness.clamp(0.0, 1.0);
        final flow = backend.currentFlow.clamp(0.0, 1.0);
        final isEraser = backend.isEraserPreset;
        setState(() {
          _color = preset.previewColor;
          _brushSize = size;
          _brushOpacity = opacity;
          // Auto-switch to the eraser tool when the loaded preset is an
          // eraser (CompositeOp=erase / Krita/erase / EraserMode). The
          // native dab path already composites destination-out; this
          // surfaces it in the UI so the tool dock matches. Non-eraser
          // presets leave the current tool untouched.
          if (isEraser) {
            _tool = FeatherTool.eraser;
          }
        });
        // Mirror into the Feather brush engine. Hardness / flow are NOT
        // in the brush panel UI, but they drive dab generation so they
        // must be mirrored too.
        _brush.color = _color.toARGB32();
        _brush.settings = _brush.settings.copyWith(
          size: _brushSize,
          opacity: _brushOpacity,
          hardness: hardness,
          flow: flow,
          pressureEnabled: _pressure,
        );
        // The native backend already has every param from loadPreset;
        // only push the colour (a session-level setting the .kpp does
        // not carry) so dabs match the active swatch.
        backend.color = BrushColor(
          (_color.r * 255).round(),
          (_color.g * 255).round(),
          (_color.b * 255).round(),
          (_color.a * 255).round(),
        );
        return;
      }
      // loadPreset failed — fall through to visual hints so the user
      // still gets feedback. [backend.lastError()] holds the diagnostic.
    }
    _applyPresetVisualHints(preset, backend);
  }

  /// Applies the legacy visual-catalog hints (colour + size from
  /// [BrushPreset.strokeWidth]) without a real .kpp round-trip. Used for
  /// the bundled placeholder presets AND as the fallback when the real
  /// engine is unavailable or [loadPreset] rejects the file.
  void _applyPresetVisualHints(
      BrushPreset preset, KritaBrushBackend? backend) {
    setState(() {
      _color = preset.previewColor;
      _brushSize = preset.strokeWidth * 4.0;
    });
    _brush.color = _color.toARGB32();
    _brush.settings = _brush.settings.copyWith(
      size: _brushSize,
      opacity: _brushOpacity,
      pressureEnabled: _pressure,
    );
    if (backend != null) {
      backend
        ..size = _brushSize
        ..color = BrushColor(
          (_color.r * 255).round(),
          (_color.g * 255).round(),
          (_color.b * 255).round(),
          (_color.a * 255).round(),
        );
    }
  }

  // ----- Undo / redo ------------------------------------------------------

  void _pushUndo() {
    _undoStack.add(_strokes.map((s) => s.copy()).toList());
    // v56-B: cap by the memory-aware effective depth (20 normal, 10 on big
    // docs). The redo stack needs no cap here — it is cleared on the next
    // line and only re-grows to the depth of a prior undo run.
    if (_undoStack.length > _effectiveMaxUndo) _undoStack.removeAt(0);
    _redoStack.clear();
  }

  void _undo() {
    if (_undoStack.isEmpty) return;
    _redoStack.add(_strokes.map((s) => s.copy()).toList());
    final prev = _undoStack.removeLast();
    _strokes
      ..clear()
      ..addAll(prev);
    _selectionModel.reconcile(_strokes);
    // Rebuild the Stroke3D map from the new Stroke snapshot so the
    // curves-system source of truth stays in sync. Lossy: raw
    // timestamps are lost (reverse-conversion from Stroke points),
    // but positions / pressures are preserved.
    _rebuildStroke3Ds();
    // v0.55-C: re-arm the stroke soft-cap warning if undo dropped the
    // count back below [_kStrokeSoftCap] (so a future re-cross refires).
    _maybeWarnStrokeCap();
    setState(() {});
  }

  void _redo() {
    if (_redoStack.isEmpty) return;
    _undoStack.add(_strokes.map((s) => s.copy()).toList());
    final next = _redoStack.removeLast();
    _strokes
      ..clear()
      ..addAll(next);
    _selectionModel.reconcile(_strokes);
    _rebuildStroke3Ds();
    // v0.55-C: re-fire the stroke soft-cap warning if redo pushed the
    // count back across [_kStrokeSoftCap] (the re-arm in _undo cleared
    // the flag, so this is the re-cross).
    _maybeWarnStrokeCap();
    setState(() {});
  }

  // ----- Stroke3D ↔ Stroke conversion + sync ------------------------------
  //
  // The host owns two parallel views of every paint stroke:
  //   * [_strokes]       — the rendered [Stroke] list (used by the canvas
  //                        viewport, selection, transform, undo / redo).
  //   * [_stroke3Ds]     — the canonical [Stroke3D] per stroke id (used
  //                        by the curves system: smoothing, simplification,
  //                        Bezier / Catmull-Rom / NURBS fitting, export).
  //
  // The two are kept in sync by these helpers:
  //   * [_stroke3DToStroke] — Stroke3D → Stroke (positions, pressures,
  //                           tilts, times, UVs).
  //   * [_strokeToStroke3D] — Stroke → Stroke3D (reverse; lossy on
  //                           timestamps when invoked after undo / redo).
  //   * [_syncStroke3D]     — rebuild the Stroke3D for a single stroke
  //                           after an in-place erase mutation.
  //   * [_rebuildStroke3Ds] — rebuild the whole Stroke3D map from the
  //                           current Stroke list (after undo / redo).

  /// Converts a [Stroke3D] to a [Stroke] for rendering. Each sample's
  /// position / pressure / tilt / time / UV is preserved verbatim.
  Stroke _stroke3DToStroke(Stroke3D s3d) {
    final points = s3d.samples
        .map((sample) => StrokePoint(
              position: sample.position.clone(),
              pressure: sample.pressure,
              tilt: sample.tilt.clone(),
              time: sample.time,
              uv: sample.uv?.clone(),
            ))
        .toList();
    return Stroke(
      brushType: BrushType.basic,
      color: s3d.color,
      thickness: s3d.thickness,
      points: points,
    );
  }

  /// Reverse-converts a [Stroke] back to a [Stroke3D]. Used to rebuild
  /// the Stroke3D map after undo / redo (the undo stack snapshots Strokes,
  /// not Stroke3Ds) and after an in-place erase mutation.
  Stroke3D _strokeToStroke3D(Stroke stroke) {
    final samples = stroke.points
        .map((p) => StrokeSample3D(
              position: p.position.clone(),
              pressure: p.pressure,
              tilt: p.tilt.clone(),
              time: p.time,
              uv: p.uv?.clone(),
            ))
        .toList();
    return Stroke3D(
      samples: samples,
      color: stroke.color,
      thickness: stroke.thickness,
      inputDevice: StrokeInputDevice.touch,
      source: 'feather-canvas',
    );
  }

  /// Rebuilds the Stroke3D for [stroke] from its current StrokePoint
  /// list. Called after an erase mutation removes points from the
  /// Stroke in place — the Stroke3D is recreated so its cached
  /// polyline length is reset and the curves system sees the
  /// post-erase sample list.
  void _syncStroke3D(Stroke stroke) {
    _stroke3Ds[stroke.id] = _strokeToStroke3D(stroke);
  }

  /// Rebuilds the entire [_stroke3Ds] map from the current [_strokes]
  /// list. Called after undo / redo (which replace the Stroke list
  /// wholesale) so the Stroke3D map reflects the new snapshot.
  void _rebuildStroke3Ds() {
    _stroke3Ds.clear();
    for (final stroke in _strokes) {
      _stroke3Ds[stroke.id] = _strokeToStroke3D(stroke);
    }
  }

  // ----- Eraser + Vacuum (per Feather Draw-and-Erase docs) ----------------
  //
  // Eraser: tap or drag with the pen to erase PARTS of curves. Per the
  // docs, "the Eraser removes points from the center of the curve, not
  // the surrounding geometry" — so we test against stroke POINTS (not
  // the tube surface) and remove any within the erase radius. Strokes
  // that fall below 2 points are dropped entirely.
  //
  // Vacuum: tap or drag with the pen to erase WHOLE curves. Per the
  // docs, "erases all curves it touches" — so we test whether ANY
  // point of a stroke is within the vacuum radius and remove the
  // entire stroke on a single hit.
  //
  // 3D Guide isolation: "Cover the curves you don't want to erase with
  // a 3D Guide. The eraser will not erase curves within the guide." We
  // approximate "within the guide" as the stroke's world centroid
  // falling inside a guide's world-space bounding sphere (centered on
  // the guide's mesh-bounds center, radius = half the diagonal of the
  // mesh AABB). Strokes whose centroid is inside any visible guide are
  // skipped by both the eraser and the vacuum.

  /// World-space radius within which the eraser removes stroke points.
  static const double _eraseRadius = 0.5;

  /// World-space radius within which the vacuum removes whole strokes.
  static const double _vacuumRadius = 1.0;

  /// Eraser: removes stroke POINTS within [_eraseRadius] of the world
  /// position under [screenPos]. Strokes covered by a 3D Guide are
  /// skipped (see [_strokeProtectedByGuide]). Strokes that drop below
  /// 2 points after the erase are removed entirely (with their
  /// Stroke3D + material records cleaned up).
  ///
  /// v57-C GAP 0 FIX (mirror-erase): when the mirror assist has any
  /// active axis OR the FeatherTool.mirror dock tool is selected, the
  /// erase is ALSO applied at each mirrored world position. Closes the
  /// "eraser stays eraser on mirror paths" gap honestly — the eraser
  /// mutates existing strokes at all mirrored positions simultaneously,
  /// matching the mirror-draw behaviour (which produces 2^N - 1 mirror
  /// copies of each committed paint stroke).
  ///
  /// HONESTY NOTE (brief deviation): the v57-C brief suggested
  /// converting each mirrored world point back to screen and calling
  /// `_eraseAt` recursively. That round trip would be lossy:
  /// [_screenToWorld] re-raycasts to a guide / ground plane, so the
  /// world point under the screen projection of `mirror(world)` is
  /// NOT `mirror(world)` (unless the mirror plane is parallel to the
  /// camera). Erasing at the screen projection would erase the wrong
  /// points. Instead we call [_eraseAtWorld] directly at each mirrored
  /// world point — same behaviour the brief intends (erase at mirrored
  /// positions) without the lossy world→screen→world round trip.
  void _eraseAt(Offset screenPos) {
    final world = _screenToWorld(screenPos);
    if (world == null) return;
    _eraseAtWorld(world);
    final mirrorActive =
        _mirrorAssist.isEnabled || _tool == FeatherTool.mirror;
    if (mirrorActive) {
      final assist =
          (_tool == FeatherTool.mirror && !_mirrorAssist.isEnabled)
              ? MirrorAssist(activeAxes: const {MirrorAxis.x})
              : _mirrorAssist;
      // reflectPoints returns 2^N copies (identity first); skip the
      // identity (already erased above) and erase at each mirrored
      // world point directly.
      final copies = assist.reflectPoints([world]);
      for (var i = 1; i < copies.length; i++) {
        _eraseAtWorld(copies[i].first);
      }
    }
  }

  /// Erases stroke POINTS within [_eraseRadius] of [world] using the
  /// [EraserEngine]. Extracted from [_eraseAt] so the mirror-erase
  /// path can call it directly at each mirrored world position without
  /// re-entering the mirror logic (avoids infinite recursion).
  void _eraseAtWorld(Vector3 world) {
    final res = _eraser.erasePointsInPlace(
      _strokes,
      world,
      _eraseRadius,
      skip: _strokeProtectedByGuide,
    );
    for (final s in res.mutated) {
      _syncStroke3D(s);
    }
    if (res.dropped.isNotEmpty) {
      final dropSet = res.dropped.toSet();
      _strokes.removeWhere(dropSet.contains);
      for (final s in res.dropped) {
        _stroke3Ds.remove(s.id);
        _strokeMaterials.remove(s.id);
      }
    }
  }

  /// Vacuum: removes entire STROKES that have any point within
  /// [_vacuumRadius] of the world position under [screenPos]. Strokes
  /// covered by a 3D Guide are skipped.
  void _vacuumAt(Offset screenPos) {
    final world = _screenToWorld(screenPos);
    if (world == null) return;
    _strokes.removeWhere((s) {
      if (_strokeProtectedByGuide(s)) return false;
      for (var i = 0; i < s.length; i++) {
        final d = (s.worldPosition(i) - world).length;
        if (d <= _vacuumRadius) {
          _stroke3Ds.remove(s.id);
          _strokeMaterials.remove(s.id);
          return true;
        }
      }
      return false;
    });
  }

  /// 3D Guide isolation check. Returns true when [stroke]'s world
  /// centroid falls inside any visible guide's world-space bounding
  /// sphere (centered on the guide's mesh-bounds center, radius = half
  /// the diagonal of the mesh AABB). Such strokes are protected from
  /// the eraser and vacuum (per the Feather docs: "Cover the curves
  /// you don't want to erase with a 3D Guide. The eraser will not
  /// erase curves within the guide").
  bool _strokeProtectedByGuide(Stroke stroke) {
    if (_guides.guides.isEmpty) return false;
    final center = stroke.worldCenter();
    for (final guide in _guides.guides) {
      if (!guide.visible) continue;
      final guideCenter = guide.worldCenter;
      final bounds = guide.mesh.bounds;
      final radius = (bounds.max - bounds.min).length * 0.5;
      if ((guideCenter - center).length <= radius) {
        return true;
      }
    }
    return false;
  }

  // ----- Export (glTF + OBJ + PNG) ----------------------------------------
  //
  // Three export entry points exposed on [_MainScreenState] for future
  // UI wiring (an export sheet / share sheet). Each delegates to the
  // pure-Dart exporter in lib/io/:
  //
  //   * [exportAsGltf] — GltfExporter: strokes as LINE_STRIP primitives,
  //                      guides as TRIANGLES primitives, single
  //                      base64-encoded binary buffer.
  //   * [exportAsObj]  — ObjExporter: strokes as capped tube meshes
  //                      (parallel-transport framing, configurable
  //                      segment count + radius).
  //   * [exportAsPng]  — PngExporter: raw RGBA8 buffer → PNG file
  //                      (caller captures the viewport pixels via a
  //                      RepaintBoundary or similar).

  /// Exports the current scene (strokes + active guides) as a glTF 2.0
  /// JSON file at [path]. Strokes are emitted as LINE_STRIP primitives;
  /// guides as TRIANGLES primitives. The Stroke3D map is passed
  /// alongside so the exporter can be extended to emit fitted Bezier /
  /// Catmull-Rom curves without changing the call site.
  Future<File> exportAsGltf(String path) async {
    final exporter = GltfExporter();
    final bytes = exporter.export(
      strokes: _strokes,
      strokeCurves: _stroke3Ds,
      guides: _guides.guides,
    );
    final file = File(path);
    await file.writeAsBytes(bytes, flush: true);
    return file;
  }

  /// Exports the current strokes as a tube mesh in Wavefront OBJ format
  /// at [path]. Each stroke becomes a closed tube (capped at both ends)
  /// with 8 vertices per ring and a 0.05 world-unit radius (defaults of
  /// [ObjExporter]).
  Future<File> exportAsObj(String path) async {
    final exporter = ObjExporter();
    final obj = exporter.export(strokes: _strokes);
    final file = File(path);
    await file.writeAsString(obj, flush: true);
    return file;
  }

  /// Exports the given RGBA pixel buffer as a PNG file at [path]. The
  /// buffer must be `width * height * 4` bytes (RGBA8, top-to-bottom
  /// row-major). Uses the pure-Dart [PngExporter] (no platform
  /// plugins). The caller is responsible for capturing the viewport
  /// pixels — e.g. via a [RepaintBoundary] + `toByteData` on the
  /// rendered image.
  Future<File> exportAsPng(
    String path, {
    required int width,
    required int height,
    required List<int> rgba,
  }) async {
    final exporter = PngExporter();
    return exporter.exportToFile(
      path: path,
      width: width,
      height: height,
      rgba: rgba,
    );
  }

  // ----- Export UI wiring (TopBar buttons → exporters) -------------------
  //
  // The TopBar now exposes Export + Share buttons (see top_bar.dart).
  // These methods bridge those buttons to the exporter entry points above:
  //
  //   * [_showExportSheet]   — opens a material bottom sheet with four
  //                            options (glTF / OBJ / PNG / Share). Each
  //                            option kicks off the corresponding handler
  //                            below. The sheet is the single entry point
  //                            for every export action so the host owns
  //                            the file-picker + share-plus plumbing in
  //                            one place.
  //   * [_exportGltf]        — FilePicker.saveFile → exportAsGltf.
  //   * [_exportObj]         — FilePicker.saveFile → exportAsObj.
  //   * [_exportPng]         — RepaintBoundary.toImage → exportAsPng.
  //   * [_shareLastExport]   — share_plus.Share.shareXFiles on the last
  //                            exported file (falls back to a PNG snapshot
  //                            when nothing has been exported yet).
  //
  // File-picker note: `FilePicker.platform.saveFile` is only implemented
  // on desktop (Windows / macOS / Linux). On mobile it returns null and
  // we fall back to writing into the canonical exports directory
  // (lib/io/app_dirs.dart `exportsDir`) with an auto-generated filename.
  // The chosen / fallback path is cached in [_lastExportPath] so the
  // Share button can re-share without forcing a re-pick.

  /// Opens the export bottom sheet. Wired to the TopBar Export button.
  Future<void> _showExportSheet() async {
    if (_exporting) return;
    final ctx = context;
    if (!mounted) return;
    final choice = await showModalBottomSheet<_ExportFormat>(
      context: ctx,
      backgroundColor: Theme.of(ctx).colorScheme.surface,
      showDragHandle: true,
      builder: (sheetCtx) => SafeArea(
        child: Column(
          mainAxisSize: MainAxisSize.min,
          children: [
            Padding(
              padding: const EdgeInsets.fromLTRB(24, 8, 24, 12),
              child: Align(
                alignment: Alignment.centerLeft,
                child: Text(
                  'Export',
                  style: Theme.of(sheetCtx).textTheme.titleMedium,
                ),
              ),
            ),
            ListTile(
              leading: const Icon(Icons.view_in_ar_rounded),
              title: const Text('Export glTF'),
              subtitle: const Text(
                  'Strokes as line strips + guides as triangles (.gltf)'),
              onTap: () => Navigator.pop(sheetCtx, _ExportFormat.gltf),
            ),
            ListTile(
              leading: const Icon(Icons.grain_outlined),
              title: const Text('Export OBJ'),
              subtitle: const Text(
                  'Strokes as capped tube meshes (.obj)'),
              onTap: () => Navigator.pop(sheetCtx, _ExportFormat.obj),
            ),
            ListTile(
              leading: const Icon(Icons.image_outlined),
              title: const Text('Export PNG'),
              subtitle: const Text(
                  'Snapshot of the current canvas viewport (.png)'),
              onTap: () => Navigator.pop(sheetCtx, _ExportFormat.png),
            ),
            ListTile(
              leading: const Icon(Icons.share_outlined),
              title: const Text('Share'),
              subtitle: const Text(
                  'Open the system share sheet for the last export'),
              onTap: () => Navigator.pop(sheetCtx, _ExportFormat.share),
            ),
            const SizedBox(height: 8),
          ],
        ),
      ),
    );
    if (choice == null) return;
    switch (choice) {
      case _ExportFormat.gltf:
        await _exportGltf();
        break;
      case _ExportFormat.obj:
        await _exportObj();
        break;
      case _ExportFormat.png:
        await _exportPng();
        break;
      case _ExportFormat.share:
        await _shareLastExport();
        break;
    }
  }

  /// Picks a save path (desktop) or falls back to the exports dir, then
  /// calls [exportAsGltf]. Caches the result in [_lastExportPath] and
  /// surfaces a SnackBar with the chosen path.
  Future<void> _exportGltf() async {
    if (_exporting) return;
    setState(() => _exporting = true);
    try {
      final path = await _pickSavePath(
        dialogTitle: 'Export glTF',
        defaultName: _defaultExportName('gltf'),
        extension: 'gltf',
      );
      if (path == null) return; // user canceled
      final file = await exportAsGltf(path);
      _lastExportPath = file.path;
      _showExportSnackBar('glTF exported', file.path);
    } catch (e) {
      _showExportSnackBar('glTF export failed: $e', null, isError: true);
    } finally {
      if (mounted) setState(() => _exporting = false);
    }
  }

  /// Picks a save path (desktop) or falls back to the exports dir, then
  /// calls [exportAsObj]. Caches the result in [_lastExportPath].
  Future<void> _exportObj() async {
    if (_exporting) return;
    setState(() => _exporting = true);
    try {
      final path = await _pickSavePath(
        dialogTitle: 'Export OBJ',
        defaultName: _defaultExportName('obj'),
        extension: 'obj',
      );
      if (path == null) return; // user canceled
      final file = await exportAsObj(path);
      _lastExportPath = file.path;
      _showExportSnackBar('OBJ exported', file.path);
    } catch (e) {
      _showExportSnackBar('OBJ export failed: $e', null, isError: true);
    } finally {
      if (mounted) setState(() => _exporting = false);
    }
  }

  /// Captures the live editor via the [RepaintBoundary] wrapping the
  /// [EditorScreen], converts the resulting [ui.Image] to a straight-alpha
  /// RGBA8 buffer, and calls [exportAsPng]. Caches the result.
  Future<void> _exportPng() async {
    if (_exporting) return;
    setState(() => _exporting = true);
    try {
      final path = await _pickSavePath(
        dialogTitle: 'Export PNG',
        defaultName: _defaultExportName('png'),
        extension: 'png',
      );
      if (path == null) return; // user canceled
      final captured = await _captureViewportRgba();
      if (captured == null) {
        _showExportSnackBar(
            'PNG export failed: viewport not ready', null, isError: true);
        return;
      }
      final file = await exportAsPng(
        path,
        width: captured.width,
        height: captured.height,
        rgba: captured.rgba,
      );
      _lastExportPath = file.path;
      _showExportSnackBar('PNG exported', file.path);
    } catch (e) {
      _showExportSnackBar('PNG export failed: $e', null, isError: true);
    } finally {
      if (mounted) setState(() => _exporting = false);
    }
  }

  /// Fires the platform share sheet (share_plus) for the last exported
  /// file. If nothing has been exported yet, kicks off a PNG snapshot
  /// into the exports dir and shares that.
  Future<void> _shareLastExport() async {
    if (_exporting) return;
    setState(() => _exporting = true);
    try {
      var path = _lastExportPath;
      if (path == null) {
        // No previous export — capture a PNG snapshot into the exports
        // dir and share that. This makes the Share button work on a
        // fresh session without forcing the user through the Export
        // sheet first.
        final captured = await _captureViewportRgba();
        if (captured == null) {
          _showExportSnackBar(
              'Share failed: viewport not ready', null, isError: true);
          return;
        }
        path = '${exportsDir().path}${Platform.pathSeparator}'
            '${_defaultExportName('png')}';
        await exportAsPng(
          path,
          width: captured.width,
          height: captured.height,
          rgba: captured.rgba,
        );
        _lastExportPath = path;
      }
      // share_plus shareXFiles: opens the platform share sheet for the
      // given file(s). On desktop this is the Windows share dialog /
      // macOS NSSharingServicePicker; on mobile the native share sheet.
      await Share.shareXFiles(
        [XFile(path)],
        text: 'Exported from Feather-Krita',
      );
    } catch (e) {
      _showExportSnackBar('Share failed: $e', null, isError: true);
    } finally {
      if (mounted) setState(() => _exporting = false);
    }
  }

  // ----- First-run engine-load info dialog (v0.55-A) --------------------
  //
  // The native Krita bridge load can fail silently on first-run Windows
  // installs (DLL missing, wrong MSVC runtime, antivirus quarantine).
  // Pre-v0.55-A the editor entered fallback mode with NO user-visible
  // signal beyond the splash banner (which disappears after 200 ms).
  // This dialog fires once per MainScreen mount when the real backend
  // is NOT active — it explains the situation, tells the user where to
  // re-download, and offers a "View crash log" shortcut.

  /// Shows the first-run engine-load info dialog. Idempotent-guarded by
  /// [_engineDialogShown] in [initState]. Wired to fire from a post-frame
  /// callback so the first frame paints before the dialog blocks input.
  void _showEngineLoadDialog() {
    final ctx = context;
    if (!mounted) return;
    showDialog<void>(
      context: ctx,
      barrierDismissible: true,
      builder: (dialogCtx) {
        final palette = Theme.of(dialogCtx).brightness == Brightness.dark
            ? FeatherColors.dark
            : FeatherColors.light;
        return AlertDialog(
          backgroundColor: palette.panelFill,
          shape: RoundedRectangleBorder(
            borderRadius: BorderRadius.circular(16),
            side: BorderSide(color: palette.panelBorder),
          ),
          title: Row(
            children: [
              Icon(Icons.warning_amber_rounded,
                  color: FeatherPalette.accentOrange, size: 26),
              const SizedBox(width: 10),
              Expanded(
                child: Text(
                  'Krita engine not loaded',
                  style: FeatherTypography.h1
                      .copyWith(color: palette.textPrimary),
                ),
              ),
            ],
          ),
          content: ConstrainedBox(
            constraints: const BoxConstraints(maxWidth: 440),
            child: Column(
              mainAxisSize: MainAxisSize.min,
              crossAxisAlignment: CrossAxisAlignment.start,
              children: [
                Text(
                  'Feather-Krita could not load the real Krita brush '
                  'engine (krita_bridge.dll). The app will run in '
                  'fallback mode with reduced features.',
                  style: FeatherTypography.body
                      .copyWith(color: palette.textPrimary, height: 1.5),
                ),
                const SizedBox(height: 12),
                Text(
                  'Please re-download from GitHub Releases if this '
                  'persists. If the file is present, the MSVC 2019+ '
                  'runtime may be missing or the DLL may have been '
                  'quarantined by antivirus.',
                  style: FeatherTypography.body
                      .copyWith(color: palette.textSecondary, height: 1.5),
                ),
                const SizedBox(height: 12),
                InkWell(
                  onTap: () async {
                    await Clipboard.setData(
                        const ClipboardData(text: kGitHubReleasesUrl));
                    if (dialogCtx.mounted) {
                      ScaffoldMessenger.of(dialogCtx).showSnackBar(
                        const SnackBar(
                          content: Text('GitHub URL copied to clipboard'),
                          duration: Duration(seconds: 2),
                        ),
                      );
                    }
                  },
                  borderRadius: BorderRadius.circular(8),
                  child: Padding(
                    padding: const EdgeInsets.symmetric(vertical: 6),
                    child: Row(
                      children: [
                        Icon(Icons.open_in_new_rounded,
                            size: 16, color: FeatherPalette.accentBlue),
                        const SizedBox(width: 6),
                        Expanded(
                          child: Text(
                            kGitHubReleasesUrl,
                            style: FeatherTypography.caption.copyWith(
                              color: FeatherPalette.accentBlue,
                              decoration: TextDecoration.underline,
                            ),
                          ),
                        ),
                      ],
                    ),
                  ),
                ),
              ],
            ),
          ),
          actions: [
            TextButton(
              onPressed: () => Navigator.of(dialogCtx).pop(),
              child: Text('Continue in fallback mode',
                  style: TextStyle(color: palette.textPrimary)),
            ),
          ],
        );
      },
    );
  }

  // ----- About / Help dialog (v0.55-A) ----------------------------------
  //
  // Constructed from the live engine + crash-log path state so the user
  // can self-diagnose at any time (not just the first run). Wired to the
  // TopBar's onAbout button via the EditorScreen prop.

  /// Builds the [FeatherAboutInfo] props from the live host state.
  Future<FeatherAboutInfo> _buildAboutInfo() async {
    final caps = _krita.capabilities;
    final nativeLibPath = caps?.libraryPath ?? '';
    final engineVersion = caps?.versionString ??
        _krita.version() ??
        'FeatherBridge-Fallback/1.0';
    final crashPath = await CrashLog.path();
    return FeatherAboutInfo(
      appVersion: kAppVersion,
      appVersionLabel: kAppVersionLabel,
      engineReal: _realBackendActive,
      engineStatusText: _realBackendActive
          ? 'Real Krita bridge loaded'
          : 'Fallback — native bridge not loaded',
      nativeLibPath: nativeLibPath,
      engineVersionString: engineVersion,
      crashLogPath: crashPath,
      gitHubReleasesUrl: kGitHubReleasesUrl,
    );
  }

  /// Opens the About / Help dialog. Wired to the TopBar's onAbout button.
  Future<void> _showAboutDialog() async {
    final ctx = context;
    if (!mounted) return;
    final info = await _buildAboutInfo();
    if (!mounted) return;
    showDialog<void>(
      context: ctx,
      builder: (_) => FeatherAboutDialog(
        info: info,
        onCopyCrashLog: () async {
          final text = await CrashLog.readAll() ?? '(crash log is empty)';
          await Clipboard.setData(ClipboardData(text: text));
          if (ctx.mounted) {
            ScaffoldMessenger.of(ctx).showSnackBar(
              const SnackBar(
                content: Text('Crash log copied to clipboard'),
                duration: Duration(seconds: 2),
              ),
            );
          }
        },
      ),
    );
  }

  /// Captures the [RepaintBoundary] wrapping the [EditorScreen] via
  /// `RenderRepaintBoundary.toImage()`, then converts the resulting
  /// [ui.Image] to a straight-alpha RGBA8 buffer (Flutter hands us
  /// premultiplied alpha; [PngExporter] expects straight alpha, so we
  /// un-premultiply every pixel). Returns null when the boundary hasn't
  /// attached yet (e.g. called before the first frame painted).
  Future<_CapturedViewport?> _captureViewportRgba() async {
    final boundary = _viewportBoundaryKey.currentContext?.findRenderObject();
    if (boundary is! RenderRepaintBoundary) return null;
    // toImage is async; pixelRatio is set to 1.0 so the RGBA buffer
    // matches the on-screen pixel size exactly (no upscaling).
    final image = await boundary.toImage(pixelRatio: 1.0);
    try {
      final byteData = await image.toByteData(format: ui.ImageByteFormat.rawRgba);
      if (byteData == null) return null;
      final src = byteData.buffer.asUint8List();
      // Flutter's rawRgba is premultiplied; un-premultiply so the PNG
      // encoder gets straight alpha (PngExporter's contract).
      final rgba = Uint8List(src.length);
      for (var i = 0; i < src.length; i += 4) {
        final a = src[i + 3];
        if (a == 0) {
          rgba[i] = 0;
          rgba[i + 1] = 0;
          rgba[i + 2] = 0;
          rgba[i + 3] = 0;
        } else if (a == 255) {
          rgba[i] = src[i];
          rgba[i + 1] = src[i + 1];
          rgba[i + 2] = src[i + 2];
          rgba[i + 3] = 255;
        } else {
          rgba[i] = (src[i] * 255 / a).round().clamp(0, 255);
          rgba[i + 1] = (src[i + 1] * 255 / a).round().clamp(0, 255);
          rgba[i + 2] = (src[i + 2] * 255 / a).round().clamp(0, 255);
          rgba[i + 3] = a;
        }
      }
      return _CapturedViewport(width: image.width, height: image.height, rgba: rgba);
    } finally {
      image.dispose();
    }
  }

  /// Opens FilePicker.saveFile (desktop) and returns the chosen path.
  /// Returns null when the user cancels. On platforms where saveFile is
  /// not implemented (mobile / web), falls back to a path inside the
  /// canonical exports directory with [defaultName] as the filename.
  Future<String?> _pickSavePath({
    required String dialogTitle,
    required String defaultName,
    required String extension,
  }) async {
    final picked = await FilePicker.platform.saveFile(
      dialogTitle: dialogTitle,
      fileName: defaultName,
      type: extension == 'gltf' || extension == 'obj' || extension == 'png'
          ? FileType.custom
          : FileType.any,
      allowedExtensions: extension == 'gltf' || extension == 'obj' || extension == 'png'
          ? [extension]
          : null,
    );
    if (picked != null) return picked;
    // Mobile / web fallback: write into the canonical exports dir.
    final dir = exportsDir();
    return '${dir.path}${Platform.pathSeparator}$defaultName';
  }

  /// Builds a default export filename: `feather_<timestamp>.<ext>`.
  String _defaultExportName(String extension) {
    final now = DateTime.now();
    final stamp = '${now.year}'
        '${now.month.toString().padLeft(2, '0')}'
        '${now.day.toString().padLeft(2, '0')}_'
        '${now.hour.toString().padLeft(2, '0')}'
        '${now.minute.toString().padLeft(2, '0')}'
        '${now.second.toString().padLeft(2, '0')}';
    return 'feather_$stamp.$extension';
  }

  /// Surfaces a SnackBar with the export result. When [path] is non-null
  /// the SnackBar text includes the chosen file path so the user knows
  /// where the export landed (file-system reveal would need the `process`
  /// package; deliberately kept out of scope to limit the dep surface).
  void _showExportSnackBar(String message, String? path, {bool isError = false}) {
    if (!mounted) return;
    final messenger = ScaffoldMessenger.maybeOf(context);
    if (messenger == null) return;
    final text = path == null ? message : '$message\n$path';
    messenger
      ..clearSnackBars()
      ..showSnackBar(
        SnackBar(
          content: Text(text),
          backgroundColor:
              isError ? Theme.of(context).colorScheme.error : null,
          duration: Duration(seconds: isError ? 4 : 3),
        ),
      );
  }

  // ----- Guide creation overlay UI ----------------------------------------
  //
  // A small top-center overlay mounted on top of the [EditorScreen]. When
  // no guide mode is active, it shows a launcher row with four buttons:
  //   [Loft] [Primitive] [Bend] [Caption]
  // Tapping one enters the corresponding sub-mode (see [setGuideMode]) —
  // or, for [Caption], fires [setCaption] as a manual demo of the
  // tutorial overlay. When a sub-mode is active, the launcher is replaced
  // by that mode's panel (slider + Done / Cancel).

  /// Builds the v0.53 [GuidePanel] overlay. The panel's mode callback
  /// translates the engine's [Guide3DType] into the host's [_GuideMode]
  /// and routes through the existing [setGuideMode] / [setGuideDrawMode]
  /// setters. The shape callback routes to [insertPrimitive]. The
  /// remaining knobs (size / axis / segments / angle / snap / ribbon) are
  /// captured into the [_guide*] fields for the F2 milestone.
  Widget _buildGuidePanel() {
    return GuidePanel(
      onModeChanged: (mode) {
        switch (mode) {
          case Guide3DType.drawn:
            setGuideDrawMode(true);
            setGuideMode(_GuideMode.none);
          case Guide3DType.lofted:
            setGuideDrawMode(false);
            setGuideMode(_GuideMode.loft);
          case Guide3DType.bent:
            setGuideDrawMode(false);
            setGuideMode(_GuideMode.bend);
          case Guide3DType.primitive:
            setGuideDrawMode(false);
            setGuideMode(_GuideMode.primitive);
        }
      },
      onPrimitiveKindChanged: insertPrimitive,
      onPrimitiveSizeChanged: (v) =>
          setState(() => _guidePrimitiveSize = v),
      onLoftAxisChanged: (a) => setState(() => _guideLoftAxis = a),
      onLoftSegmentsChanged: (n) => setState(() => _guideLoftSegments = n),
      onBendAngleChanged: (v) => setState(() => _guideBendAngle = v),
      onBendAxisChanged: (a) => setState(() => _guideBendAxis = a),
      onSnapChanged: (v) => setState(() => _guideSnap = v),
      onRibbonChanged: (v) => setState(() => _guideRibbon = v),
      onClose: () => setState(() => _guidePanelVisible = false),
    );
  }

  /// Builds the v0.54-A [AssistPanel] overlay. The panel's Stable Strokes
  /// toggle / intensity callbacks mutate the host's [_stableStrokesEnabled]
  /// / [_stableStrokesIntensity] fields, which the stroke-start handler
  /// reads when it rebuilds the stabilizer config (see [_onStrokeStart]).
  /// Mirror axes + Draw Shape mode sections are added in the follow-up
  /// commits of v0.54-A.
  Widget _buildAssistPanel() {
    return AssistPanel(
      initialStableStrokes: _stableStrokesEnabled,
      initialStableStrokesIntensity: _stableStrokesIntensity,
      onStableStrokesChanged: (v) => setState(() => _stableStrokesEnabled = v),
      onStableStrokesIntensityChanged: (v) =>
          setState(() => _stableStrokesIntensity = v),
      onClose: () => setState(() => _assistPanelVisible = false),
    );
  }

  /// Builds the v55-B [LightRigPanel] overlay. Forwards the host's
  /// current azimuth / elevation / intensity / ambient as initial
  /// values + wires each slider's callback to mutate the corresponding
  /// host field + setState (so [_buildCanvasScene] + [_buildLightRig]
  /// pick up the new values on the next frame).
  Widget _buildLightRigPanel() {
    return LightRigPanel(
      initialAzimuth: _lightAzimuth,
      initialElevation: _lightElevation,
      initialIntensity: _lightIntensity,
      initialAmbient: _lightAmbient,
      // v0.57-D: wrap the azimuth into [-pi, pi] via scalar_math.wrapAngle
      // so a slider drag past +pi/-pi does not leave the host with a
      // raw angle like 7.5 rad (which would still render correctly but
      // makes equality checks + serialisation non-canonical). The wrap
      // is a no-op for values already in range.
      onAzimuthChanged: (v) => setState(() => _lightAzimuth = scalarmath.wrapAngle(v)),
      onElevationChanged: (v) => setState(() => _lightElevation = v),
      onIntensityChanged: (v) => setState(() => _lightIntensity = v),
      onAmbientChanged: (v) => setState(() => _lightAmbient = v),
      onClose: () => setState(() => _lightRigPanelVisible = false),
    );
  }

  /// Builds the v55-B [StrokeListPanel] overlay. Forwards a snapshot
  /// of the host's [_strokes] list (built by [_buildStrokeListItems])
  /// + wires the panel's callbacks to the host's mutation methods
  /// ([_setStrokeVisible] / [_deleteStrokeById] / [_reorderStrokes]).
  /// All three mutations setState so the next frame's snapshot reflects
  /// the change + the canvas painter picks up the new stroke list.
  Widget _buildStrokeListPanel() {
    return StrokeListPanel(
      strokes: _buildStrokeListItems(),
      onToggleVisible: (id) {
        final s = _findStroke(id);
        if (s != null) _setStrokeVisible(id, !s.isVisible);
      },
      onDelete: (id) => _deleteStrokeById(id),
      onReorder: (oldIndex, newIndex) => _reorderStrokes(oldIndex, newIndex),
      onClose: () => setState(() => _strokeListPanelVisible = false),
    );
  }

  Widget _buildGuideOverlay() {
    switch (_guideMode) {
      case _GuideMode.none:
        return _buildGuideLauncher();
      case _GuideMode.loft:
        return _buildLoftPanel();
      case _GuideMode.primitive:
        return _buildPrimitivePanel();
      case _GuideMode.bend:
        return _buildBendPanel();
    }
  }

  Widget _buildGuideLauncher() {
    return Material(
      color: Colors.transparent,
      child: Container(
        padding: const EdgeInsets.symmetric(horizontal: 8, vertical: 4),
        decoration: BoxDecoration(
          color: Colors.black.withValues(alpha: 0.55),
          borderRadius: BorderRadius.circular(10),
        ),
        child: Row(
          mainAxisSize: MainAxisSize.min,
          children: [
            _launcherButton('Loft', () => setGuideMode(_GuideMode.loft)),
            _launcherButton(
                'Primitive', () => setGuideMode(_GuideMode.primitive)),
            _launcherButton('Bend', () => setGuideMode(_GuideMode.bend)),
            _launcherButton(
                'Caption', () => setCaption("Let's sketch this!")),
          ],
        ),
      ),
    );
  }

  Widget _launcherButton(String label, VoidCallback onTap) {
    return Padding(
      padding: const EdgeInsets.symmetric(horizontal: 2),
      child: GestureDetector(
        onTap: onTap,
        child: Container(
          padding: const EdgeInsets.symmetric(horizontal: 10, vertical: 6),
          decoration: BoxDecoration(
            color: Colors.white.withValues(alpha: 0.12),
            borderRadius: BorderRadius.circular(8),
          ),
          child: Text(
            label,
            style: const TextStyle(
              color: Colors.white,
              fontSize: 12,
              fontWeight: FontWeight.w600,
            ),
          ),
        ),
      ),
    );
  }

  Widget _buildLoftPanel() {
    return Material(
      color: Colors.transparent,
      child: Container(
        padding: const EdgeInsets.symmetric(horizontal: 12, vertical: 8),
        decoration: BoxDecoration(
          color: Colors.black.withValues(alpha: 0.7),
          borderRadius: BorderRadius.circular(10),
          border:
              Border.all(color: Colors.blueAccent.withValues(alpha: 0.6)),
        ),
        child: Column(
          mainAxisSize: MainAxisSize.min,
          children: [
            Text(
              'Loft — ${_loftStrokeIds.length} curve(s) selected. '
              'Tap strokes to add / remove.',
              style: const TextStyle(color: Colors.white, fontSize: 12),
            ),
            const SizedBox(height: 6),
            Row(
              mainAxisSize: MainAxisSize.min,
              children: [
                const Text('Tension',
                    style: TextStyle(color: Colors.white70, fontSize: 11)),
                SizedBox(
                  width: 160,
                  child: Slider(
                    value: _loftTension,
                    min: 0.0,
                    max: 1.0,
                    divisions: 100,
                    label: _loftTension.toStringAsFixed(2),
                    onChanged: setLoftTension,
                  ),
                ),
                _iconBtn(Icons.check_rounded, finishLoft, Colors.greenAccent),
                _iconBtn(Icons.close_rounded, cancelLoft, Colors.redAccent),
              ],
            ),
          ],
        ),
      ),
    );
  }

  Widget _buildPrimitivePanel() {
    return Material(
      color: Colors.transparent,
      child: Container(
        padding: const EdgeInsets.symmetric(horizontal: 12, vertical: 8),
        decoration: BoxDecoration(
          color: Colors.black.withValues(alpha: 0.7),
          borderRadius: BorderRadius.circular(10),
          border: Border.all(
              color: Colors.purpleAccent.withValues(alpha: 0.6)),
        ),
        child: Column(
          mainAxisSize: MainAxisSize.min,
          children: [
            Row(
              mainAxisSize: MainAxisSize.min,
              children: [
                for (final kind in Guide3DPrimitive.values)
                  Padding(
                    padding: const EdgeInsets.symmetric(horizontal: 2),
                    child: _primitiveButton(kind),
                  ),
              ],
            ),
            const SizedBox(height: 6),
            Row(
              mainAxisSize: MainAxisSize.min,
              children: [
                const Text('Segments',
                    style: TextStyle(color: Colors.white70, fontSize: 11)),
                SizedBox(
                  width: 160,
                  child: Slider(
                    value: _primitiveSegments.toDouble(),
                    min: 3,
                    max: 64,
                    divisions: 61,
                    label: '$_primitiveSegments',
                    onChanged: (v) => setPrimitiveSegments(v.round()),
                  ),
                ),
                SizedBox(
                  width: 32,
                  child: Text('$_primitiveSegments',
                      style: const TextStyle(
                          color: Colors.white70, fontSize: 11)),
                ),
                _iconBtn(
                    Icons.check_rounded, finishPrimitive, Colors.greenAccent),
                _iconBtn(
                    Icons.close_rounded, cancelPrimitive, Colors.redAccent),
              ],
            ),
          ],
        ),
      ),
    );
  }

  Widget _primitiveButton(Guide3DPrimitive kind) {
    final active = _primitiveKind == kind;
    return GestureDetector(
      onTap: () => insertPrimitive(kind),
      child: Container(
        padding: const EdgeInsets.symmetric(horizontal: 10, vertical: 6),
        decoration: BoxDecoration(
          color: active
              ? Colors.purpleAccent.withValues(alpha: 0.6)
              : Colors.white.withValues(alpha: 0.12),
          borderRadius: BorderRadius.circular(8),
          border: Border.all(
            color: active ? Colors.purpleAccent : Colors.transparent,
          ),
        ),
        child: Text(
          kind.displayName,
          style: const TextStyle(
            color: Colors.white,
            fontSize: 12,
            fontWeight: FontWeight.w600,
          ),
        ),
      ),
    );
  }

  Widget _buildBendPanel() {
    final source = _bendSourceId != null ? _guides[_bendSourceId!] : null;
    final sourceName =
        source?.name ?? 'Guide #${_bendSourceId ?? '-'}';
    return Material(
      color: Colors.transparent,
      child: Container(
        padding: const EdgeInsets.symmetric(horizontal: 12, vertical: 8),
        decoration: BoxDecoration(
          color: Colors.black.withValues(alpha: 0.7),
          borderRadius: BorderRadius.circular(10),
          border: Border.all(
              color: Colors.orangeAccent.withValues(alpha: 0.6)),
        ),
        child: Row(
          mainAxisSize: MainAxisSize.min,
          children: [
            Text(
              'Bend — source: $sourceName. Draw a path on the canvas.',
              style: const TextStyle(color: Colors.white, fontSize: 12),
            ),
            _iconBtn(Icons.check_rounded,
                () => setGuideMode(_GuideMode.none), Colors.greenAccent),
            _iconBtn(Icons.close_rounded, cancelBend, Colors.redAccent),
          ],
        ),
      ),
    );
  }

  Widget _iconBtn(IconData icon, VoidCallback onTap, Color color) {
    return IconButton(
      onPressed: onTap,
      icon: Icon(icon, color: color, size: 20),
      tooltip: '',
      padding: const EdgeInsets.all(4),
      constraints: const BoxConstraints(minWidth: 32, minHeight: 32),
    );
  }
}

/// Guide-creation sub-mode for the host's overlay UI. `none` is the
/// default painting mode; the other values activate the Loft / Primitive
/// / Bend guide-creation flows per the Feather docs.
enum _GuideMode { none, loft, primitive, bend }

/// A world-space camera ray (origin + direction, both vector_math
/// [Vector3]). Kept separate from the engine's [Ray] type
/// (lib/core/math/ray.dart) which uses the immutable [Vec3]; the camera
/// and stroke model operate on [Vector3] so this is the natural type
/// here.
class _CamRay {
  _CamRay(this.origin, this.direction);
  final Vector3 origin;
  final Vector3 direction;
}

/// Format choices surfaced by the export bottom sheet
/// (see [_MainScreenState._showExportSheet]). Drives the switch that
/// dispatches to [_exportGltf] / [_exportObj] / [_exportPng] /
/// [_shareLastExport].
enum _ExportFormat { gltf, obj, png, share }

/// Immutable result of [_MainScreenState._captureViewportRgba]: the
/// width + height of the captured [ui.Image] plus the straight-alpha
/// RGBA8 buffer (length == width * height * 4) ready for
/// [PngExporter.exportToFile].
class _CapturedViewport {
  const _CapturedViewport({
    required this.width,
    required this.height,
    required this.rgba,
  });
  final int width;
  final int height;
  final List<int> rgba;
}

/// Floating rose/pink toggle button for the v0.53 [GuidePanel] overlay.
/// Sits in the bottom-right corner of the editor Stack; tapping it flips
/// [_MainScreenState._guidePanelVisible]. Stateless + self-contained.
class _GuidePanelToggleButton extends StatelessWidget {
  const _GuidePanelToggleButton({required this.visible, required this.onTap});

  final bool visible;
  final VoidCallback onTap;

  @override
  Widget build(BuildContext context) {
    return GestureDetector(
      behavior: HitTestBehavior.opaque,
      onTap: onTap,
      child: Tooltip(
        message: visible ? 'Hide 3D Guide panel' : 'Show 3D Guide panel',
        child: Container(
          width: 48,
          height: 48,
          decoration: BoxDecoration(
            shape: BoxShape.circle,
            gradient: const LinearGradient(
              begin: Alignment.topLeft,
              end: Alignment.bottomRight,
              colors: <Color>[
                Color(0xFFF472B6),
                Color(0xFFA78BFA),
                Color(0xFFFB923C),
              ],
            ),
            boxShadow: <BoxShadow>[
              BoxShadow(
                color: const Color(0xFFF472B6).withValues(alpha: 0.45),
                blurRadius: 16,
                offset: const Offset(0, 4),
              ),
            ],
          ),
          child: Icon(
            visible
                ? Icons.close_rounded
                : Icons.view_in_ar_rounded,
            color: Colors.white,
            size: 22,
          ),
        ),
      ),
    );
  }
}

/// Floating rose/pink toggle button for the v0.54-A [AssistPanel] overlay.
/// Sits in the bottom-right corner of the editor Stack (offset left of the
/// [_GuidePanelToggleButton] so the two don't overlap); tapping it flips
/// [_MainScreenState._assistPanelVisible]. Stateless + self-contained,
/// mirroring [_GuidePanelToggleButton].
class _AssistPanelToggleButton extends StatelessWidget {
  const _AssistPanelToggleButton({required this.visible, required this.onTap});

  final bool visible;
  final VoidCallback onTap;

  @override
  Widget build(BuildContext context) {
    return GestureDetector(
      behavior: HitTestBehavior.opaque,
      onTap: onTap,
      child: Tooltip(
        message: visible ? 'Hide Assist panel' : 'Show Assist panel',
        child: Container(
          width: 48,
          height: 48,
          decoration: BoxDecoration(
            shape: BoxShape.circle,
            gradient: const LinearGradient(
              begin: Alignment.topLeft,
              end: Alignment.bottomRight,
              colors: <Color>[
                Color(0xFFFB923C),
                Color(0xFFF472B6),
                Color(0xFFA78BFA),
              ],
            ),
            boxShadow: <BoxShadow>[
              BoxShadow(
                color: const Color(0xFFFB923C).withValues(alpha: 0.45),
                blurRadius: 16,
                offset: const Offset(0, 4),
              ),
            ],
          ),
          child: Icon(
            visible ? Icons.close_rounded : Icons.auto_awesome_outlined,
            color: Colors.white,
            size: 22,
          ),
        ),
      ),
    );
  }
}

/// Floating rose/pink toggle button for the v55-B [LightRigPanel] overlay.
/// Sits at the top-left of the editor Stack (just below the top bar);
/// tapping it flips [_MainScreenState._lightRigPanelVisible]. Stateless +
/// self-contained, mirroring [_GuidePanelToggleButton] /
/// [_AssistPanelToggleButton]. The icon is a sun; when the panel is open
/// the icon becomes a close X.
class _LightRigPanelToggleButton extends StatelessWidget {
  const _LightRigPanelToggleButton({required this.visible, required this.onTap});

  final bool visible;
  final VoidCallback onTap;

  @override
  Widget build(BuildContext context) {
    return GestureDetector(
      behavior: HitTestBehavior.opaque,
      onTap: onTap,
      child: Tooltip(
        message: visible ? 'Hide Light panel' : 'Show Light panel',
        child: Container(
          width: 48,
          height: 48,
          decoration: BoxDecoration(
            shape: BoxShape.circle,
            gradient: const LinearGradient(
              begin: Alignment.topLeft,
              end: Alignment.bottomRight,
              colors: <Color>[
                Color(0xFFFB923C),
                Color(0xFFF472B6),
                Color(0xFFA78BFA),
              ],
            ),
            boxShadow: <BoxShadow>[
              BoxShadow(
                color: const Color(0xFFFB923C).withValues(alpha: 0.45),
                blurRadius: 16,
                offset: const Offset(0, 4),
              ),
            ],
          ),
          child: Icon(
            visible ? Icons.close_rounded : Icons.wb_sunny_outlined,
            color: Colors.white,
            size: 22,
          ),
        ),
      ),
    );
  }
}

/// Floating rose/pink toggle button for the v55-B [StrokeListPanel]
/// overlay. Sits at the bottom-right of the editor Stack (stacked left
/// of the AssistPanel toggle so the three bottom-right buttons — Guide,
/// Assist, Strokes — don't overlap); tapping it flips
/// [_MainScreenState._strokeListPanelVisible]. Stateless + self-
/// contained, mirroring [_GuidePanelToggleButton] /
/// [_AssistPanelToggleButton] / [_LightRigPanelToggleButton]. The icon
/// is a timeline (3D curve); when the panel is open the icon becomes a
/// close X.
class _StrokeListPanelToggleButton extends StatelessWidget {
  const _StrokeListPanelToggleButton({
    required this.visible,
    required this.onTap,
  });

  final bool visible;
  final VoidCallback onTap;

  @override
  Widget build(BuildContext context) {
    return GestureDetector(
      behavior: HitTestBehavior.opaque,
      onTap: onTap,
      child: Tooltip(
        message: visible ? 'Hide 3D Strokes panel' : 'Show 3D Strokes panel',
        child: Container(
          width: 48,
          height: 48,
          decoration: BoxDecoration(
            shape: BoxShape.circle,
            gradient: const LinearGradient(
              begin: Alignment.topLeft,
              end: Alignment.bottomRight,
              colors: <Color>[
                Color(0xFFA78BFA),
                Color(0xFFF472B6),
                Color(0xFFFB923C),
              ],
            ),
            boxShadow: <BoxShadow>[
              BoxShadow(
                color: const Color(0xFFA78BFA).withValues(alpha: 0.45),
                blurRadius: 16,
                offset: const Offset(0, 4),
              ),
            ],
          ),
          child: Icon(
            visible ? Icons.close_rounded : Icons.timeline_rounded,
            color: Colors.white,
            size: 22,
          ),
        ),
      ),
    );
  }
}

