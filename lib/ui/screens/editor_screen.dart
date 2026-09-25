// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// editor_screen.dart — Main editor.
//
// Composes the canvas viewport with every panel:
//   * background: [CanvasViewport] filling the screen,
//   * left rail:  [BrushPanel] + history cluster,
//   * right rail: [RightPanel] (collapsible),
//   * top:        [TopBar] (contextual),
//   * bottom:     [BottomBar] (mode switcher),
//   * tool dock:  [ToolDock] floating right of center,
//   * popovers:   [ColorWheel], [MaterialPicker], [BrushPicker],
//                 [StagePanel], [LiquifyPanel] — only when their tools
//                 are active,
//   * gizmo:      [JoystickWidget] when the Select tool is active,
//   * overlay:    [RadialMenu] (squeeze), tutorial caption.
//
// The screen is purely a layout / state-machine shell — it owns the UI
// state (active tool, mode, popovers) and emits callbacks; the parent
// wires them to the real engine via Riverpod.

import 'package:flutter/material.dart';

import '../theme/feather_animations.dart';
import '../theme/feather_colors.dart';
import '../widgets/bottom_bar.dart';
import '../widgets/brush_panel.dart';
import '../widgets/brush_picker.dart';
import '../widgets/canvas_viewport.dart';
import '../widgets/color_wheel.dart';
import '../widgets/icon_button.dart';
import '../widgets/joystick_widget.dart';
import '../widgets/liquify_panel.dart';
import '../widgets/material_picker.dart';
import '../widgets/radial_menu.dart';
import '../widgets/right_panel.dart';
import '../widgets/stage_panel.dart';
import '../widgets/tool_dock.dart';
import '../widgets/top_bar.dart';

class EditorUiState {
  const EditorUiState({
    this.tool = FeatherTool.draw,
    this.mode = FeatherMode.draw,
    this.color = const Color(0xFF60A5FA),
    this.size = 20,
    this.opacity = 1.0,
    this.pressure = true,
    this.injector = false,
    this.material = FeatherMaterial.shaded,
    this.pattern = FeatherPattern.none,
    this.groupName = 'Group 1',
    this.renderMode = false,
    this.rightCollapsed = false,
    this.uiHidden = false,
  });

  final FeatherTool tool;
  final FeatherMode mode;
  final Color color;
  final double size;
  final double opacity;
  final bool pressure;
  final bool injector;
  final FeatherMaterial material;
  final FeatherPattern pattern;
  final String groupName;
  final bool renderMode;
  final bool rightCollapsed;
  final bool uiHidden;

  EditorUiState copyWith({
    FeatherTool? tool,
    FeatherMode? mode,
    Color? color,
    double? size,
    double? opacity,
    bool? pressure,
    bool? injector,
    FeatherMaterial? material,
    FeatherPattern? pattern,
    String? groupName,
    bool? renderMode,
    bool? rightCollapsed,
    bool? uiHidden,
  }) =>
      EditorUiState(
        tool: tool ?? this.tool,
        mode: mode ?? this.mode,
        color: color ?? this.color,
        size: size ?? this.size,
        opacity: opacity ?? this.opacity,
        pressure: pressure ?? this.pressure,
        injector: injector ?? this.injector,
        material: material ?? this.material,
        pattern: pattern ?? this.pattern,
        groupName: groupName ?? this.groupName,
        renderMode: renderMode ?? this.renderMode,
        rightCollapsed: rightCollapsed ?? this.rightCollapsed,
        uiHidden: uiHidden ?? this.uiHidden,
      );
}

class EditorScreen extends StatefulWidget {
  const EditorScreen({
    super.key,
    this.initial = const EditorUiState(),
    this.scene = const CanvasScene(),
    this.layers = const [],
    this.resources = const [],
    this.presets = const [],
    this.onStrokeStart,
    this.onStrokeUpdate,
    this.onStrokeEnd,
    this.onUndo,
    this.onRedo,
    this.canUndo = false,
    this.canRedo = false,
    // --- Host wiring (feather-integration) -------------------------------
    // The EditorScreen is a layout shell; these callbacks let the parent
    // host (lib/screens/main_screen.dart) observe UI-state mutations and
    // canvas gestures so it can drive the real engine (camera, brush,
    // selection, liquify, …). drawingEnabled gates single-finger drawing
    // so the host can switch to orbit/select/liquify modes.
    this.drawingEnabled = true,
    this.onUiStateChanged,
    this.onPan,
    this.onZoom,
    this.onSelectAll,
    this.onLiquifyMode,
    this.onLiquifyApply,
    this.onLiquifyUndoAll,
    this.onJoystickMove,
    this.onJoystickRotate,
    this.onJoystickScale,
    this.onPickPreset,
    // --- Gesture wiring (feather-integration gap 2 + 3) ------------------
    // onTapSelect: fired when the canvas receives a tap (single-finger,
    // no drag) while the Select tool is active. The host routes the
    // screen position through [SelectionSystem.tapSelectSync].
    // onLiquifyDrag: fired on every pan-update while the Liquify tool is
    // active. The host forwards (screenPos, dragDelta) into
    // [LiquifyEngine.applyDrag].
    this.onTapSelect,
    this.onLiquifyDrag,
  });

  final EditorUiState initial;
  final CanvasScene scene;
  final List<LayerItem> layers;
  final List<ResourceItem> resources;
  final List<BrushPreset> presets;

  final VoidCallback? onStrokeStart;
  final ValueChanged<Offset>? onStrokeUpdate;
  final VoidCallback? onStrokeEnd;
  final VoidCallback? onUndo;
  final VoidCallback? onRedo;
  final bool canUndo;
  final bool canRedo;

  /// When false, single-finger canvas drags are NOT treated as strokes
  /// (the host uses them for orbit / select / liquify instead).
  final bool drawingEnabled;

  /// Fired whenever the internal [EditorUiState] mutates (tool / mode /
  /// color / size / opacity / material / pattern / visibility flags).
  /// The host mirrors the values into the real engine.
  final ValueChanged<EditorUiState>? onUiStateChanged;

  /// Canvas pan gesture (two-finger drag), in logical pixels.
  final ValueChanged<Offset>? onPan;

  /// Canvas zoom gesture (pinch), as a multiplicative scale factor.
  final ValueChanged<double>? onZoom;

  /// Radial menu → Select All action.
  final VoidCallback? onSelectAll;

  /// Liquify panel → mode switch (Push / Pinch / Comb). The host maps the
  /// UI enum to [LiquifyBrushType] and pushes it to the [LiquifyEngine].
  final ValueChanged<LiquifyMode>? onLiquifyMode;

  /// Liquify panel → Apply button.
  final VoidCallback? onLiquifyApply;

  /// Liquify panel → Undo All button.
  final VoidCallback? onLiquifyUndoAll;

  /// Joystick → move delta (normalized -1..1 in each axis).
  final ValueChanged<Offset>? onJoystickMove;

  /// Joystick → rotate delta (radians).
  final ValueChanged<double>? onJoystickRotate;

  /// Joystick → scale delta (width, height) — fractional.
  final ValueChanged<Offset>? onJoystickScale;

  /// Brush picker → preset selected. The host loads it into the engine.
  final ValueChanged<BrushPreset>? onPickPreset;

  /// Canvas tap (select tool). The host runs [SelectionSystem.tapSelectSync]
  /// against the screen position to pick the nearest stroke within the
  /// tap radius.
  final ValueChanged<Offset>? onTapSelect;

  /// Canvas drag (liquify tool). Fired on every pan-update with the live
  /// screen position + the screen-space drag delta since the previous
  /// update. The host bridges them into world space and forwards them to
  /// [LiquifyEngine.applyDrag].
  final void Function(Offset screenPos, Offset dragDelta)? onLiquifyDrag;

  @override
  State<EditorScreen> createState() => _EditorScreenState();
}

class _EditorScreenState extends State<EditorScreen> {
  late EditorUiState _s = widget.initial;
  CanvasScene get _scene => widget.scene;

  // Popover visibility.
  bool _showColor = false;
  bool _showMaterial = false;
  bool _showPresets = false;
  bool _showStage = false;
  bool _showLiquify = false;
  bool _showRadial = false;
  Offset? _radialAnchor;

  // Live preview stroke while drawing.
  CanvasStroke? _preview;
  List<Offset> _live = const [];

  // Mirror of the Liquify panel's mode (so the panel stays in sync when
  // the host reports a mode change — kept local for now; the host also
  // receives the change via [widget.onLiquifyMode]).
  LiquifyMode _liquifyMode = LiquifyMode.push;

  void _set(EditorUiState next) {
    setState(() => _s = next);
    widget.onUiStateChanged?.call(next);
  }

  @override
  Widget build(BuildContext context) {
    final palette = _palette(context);
    if (_s.uiHidden) {
      return Scaffold(
        backgroundColor: palette.appBackground,
        body: Stack(
          children: [
            _canvasLayer(),
            Positioned(
              left: 16,
              bottom: 16,
              child: FeatherIconButton(
                icon: Icons.visibility_outlined,
                tooltip: 'Show UI',
                size: 44,
                iconSize: 20,
                onTap: () => _set(_s.copyWith(uiHidden: false)),
              ),
            ),
          ],
        ),
      );
    }
    return Scaffold(
      backgroundColor: palette.appBackground,
      body: Stack(
        children: [
          // 1. Canvas (bottom layer).
          _canvasLayer(),

          // 2. Left rail — brush panel.
          Positioned(
            left: 16,
            top: 0,
            bottom: 0,
            child: Center(
              child: BrushPanel(
                brushType: _iconFor(_s.tool),
                color: _s.color,
                size: _s.size,
                opacity: _s.opacity,
                pressure: _s.pressure,
                injector: _s.injector,
                material: _s.material,
                pattern: _s.pattern,
                onBrushType: () => setState(() => _showPresets = true),
                onColor: (c) => _set(_s.copyWith(color: c)),
                onSize: (v) => _set(_s.copyWith(size: v)),
                onOpacity: (v) => _set(_s.copyWith(opacity: v)),
                onPressure: (v) => _set(_s.copyWith(pressure: v)),
                onInjector: (v) => _set(_s.copyWith(injector: v)),
                onOpenPresets: () => setState(() => _showPresets = true),
                onOpenColor: () =>
                    setState(() => _showColor = !_showColor),
                onOpenMaterial: () =>
                    setState(() => _showMaterial = !_showMaterial),
              ),
            ),
          ),

          // 3. Tool dock — right of brush panel.
          Positioned(
            left: 168,
            top: 0,
            bottom: 0,
            child: Center(
              child: ToolDock(
                active: _s.tool,
                onSelect: (t) => _set(_s.copyWith(tool: t)),
                onHome: () => Navigator.of(context).maybePop(),
                onSystemMenu: () => setState(() => _showStage = true),
              ),
            ),
          ),

          // 4. Right panel — contextual.
          Positioned(
            right: 16,
            top: 76,
            bottom: 96,
            child: Align(
              alignment: Alignment.centerRight,
              child: RightPanel(
                collapsed: _s.rightCollapsed,
                onCollapsed: (c) => _set(_s.copyWith(rightCollapsed: c)),
                layers: widget.layers,
                resources: widget.resources,
                renderMode: _s.renderMode,
                onRenderToggle: () =>
                    _set(_s.copyWith(renderMode: !_s.renderMode)),
              ),
            ),
          ),

          // 5. Top bar.
          Positioned(
            left: 0,
            right: 0,
            top: 16,
            child: Center(
              child: TopBar(
                tool: _s.tool.name[0].toUpperCase() +
                    _s.tool.name.substring(1),
                groupName: _s.groupName,
                color: _s.color,
                canUndo: widget.canUndo,
                canRedo: widget.canRedo,
                renderMode: _s.renderMode,
                onUndo: widget.onUndo,
                onRedo: widget.onRedo,
                onRenderToggle: () =>
                    _set(_s.copyWith(renderMode: !_s.renderMode)),
                onHideUI: () => _set(_s.copyWith(uiHidden: true)),
                contextualActions: _contextualActions(),
              ),
            ),
          ),

          // 6. Bottom bar — mode switcher.
          Positioned(
            left: 0,
            right: 0,
            bottom: 16,
            child: Center(
              child: BottomBar(
                mode: _s.mode,
                onMode: (m) => _set(_s.copyWith(mode: m)),
                leftActions: _bottomLeftActions(),
                rightActions: _bottomRightActions(),
              ),
            ),
          ),

          // 7. Liquify panel — only when liquify tool.
          if (_s.tool == FeatherTool.liquify || _showLiquify)
            Positioned(
              right: 304,
              bottom: 96,
              child: LiquifyPanel(
                mode: _liquifyMode,
                onMode: (m) {
                  setState(() => _liquifyMode = m);
                  widget.onLiquifyMode?.call(m);
                },
                onApply: () {
                  setState(() => _showLiquify = false);
                  widget.onLiquifyApply?.call();
                },
                onUndoAll: widget.onLiquifyUndoAll,
              ),
            ),

          // 8. Joystick — when select tool is active.
          if (_s.tool == FeatherTool.select)
            Positioned(
              left: 0,
              right: 0,
              bottom: 96,
              child: Center(
                child: JoystickWidget(
                  onMove: widget.onJoystickMove,
                  onRotate: widget.onJoystickRotate,
                  onScale: widget.onJoystickScale,
                ),
              ),
            ),

          // 9. Popovers.
          if (_showColor)
            _Popover(
              anchor: const Offset(160, 0),
              alignment: _PopoverAlignment.left,
              child: ColorWheel(
                color: _s.color,
                onChanged: (c) => _set(_s.copyWith(color: c)),
                onEyeDropper: () {},
              ),
              onClose: () => setState(() => _showColor = false),
            ),
          if (_showMaterial)
            _Popover(
              anchor: const Offset(160, 0),
              alignment: _PopoverAlignment.left,
              child: MaterialPicker(
                material: _s.material,
                pattern: _s.pattern,
                onMaterial: (m) => _set(_s.copyWith(material: m)),
                onPattern: (p) => _set(_s.copyWith(pattern: p)),
              ),
              onClose: () => setState(() => _showMaterial = false),
            ),
          if (_showPresets)
            _Popover(
              anchor: const Offset(160, 200),
              alignment: _PopoverAlignment.left,
              child: BrushPicker(
                presets: widget.presets,
                selectedId: widget.presets.isNotEmpty
                    ? widget.presets.first.id
                    : null,
                onSelected: widget.onPickPreset,
              ),
              onClose: () => setState(() => _showPresets = false),
            ),
          if (_showStage)
            _Popover(
              anchor: const Offset(0, 0),
              alignment: _PopoverAlignment.center,
              child: StagePanel(
                groups: widget.layers,
                resources: widget.resources,
                onClose: () => setState(() => _showStage = false),
              ),
              onClose: () => setState(() => _showStage = false),
            ),

          // 10. Radial menu (squeeze).
          if (_showRadial && _radialAnchor != null)
            Positioned.fill(
              child: RadialMenu(
                items: [
                  RadialMenuItem(
                    icon: Icons.undo_rounded,
                    label: 'Undo',
                    onTap: widget.onUndo,
                  ),
                  RadialMenuItem(
                    icon: Icons.redo_rounded,
                    label: 'Redo',
                    onTap: widget.onRedo,
                  ),
                  RadialMenuItem(
                    icon: Icons.search_rounded,
                    label: 'Find Group',
                  ),
                  if (_s.tool == FeatherTool.draw)
                    RadialMenuItem(
                      icon: Icons.add_rounded,
                      label: 'New Group',
                    ),
                  if (_s.tool == FeatherTool.select)
                    RadialMenuItem(
                      icon: Icons.select_all_rounded,
                      label: 'Select All',
                      onTap: widget.onSelectAll,
                    ),
                ],
                onDismiss: () =>
                    setState(() => _showRadial = false),
              ),
            ),

          // 11. Tutorial caption (visible only when toggled by parent).
          // Kept here as the canonical place to mount the handwritten
          // overlay when the tutorial mode is on.
        ],
      ),
    );
  }

  Widget _canvasLayer() {
    // When drawing is disabled (the host is in select / liquify / orbit
    // mode), pass `onStrokeStart: null` so the viewport treats single-
    // finger drags as pan — which the host routes to camera orbit.
    final start = widget.drawingEnabled ? widget.onStrokeStart : null;
    return Positioned.fill(
      child: CanvasViewport(
        scene: CanvasScene(
          strokes: _scene.strokes,
          previewStroke: _preview,
          activeColor: _s.color,
          pan: _scene.pan,
          zoom: _scene.zoom,
          showGrid: _scene.showGrid,
          renderMode: _s.renderMode,
          selectionBounds: _scene.selectionBounds,
        ),
        onStrokeStart: start == null
            ? null
            : () {
                _live = const [];
                setState(() => _preview = null);
                widget.onStrokeStart?.call();
              },
        onStrokeUpdate: (p) {
          _live = [..._live, p];
          setState(() => _preview = CanvasStroke(
                points: _live,
                color: _s.color,
                width: _s.size * 0.1,
              ));
          widget.onStrokeUpdate?.call(p);
        },
        onStrokeEnd: () {
          setState(() {
            _preview = null;
            _live = const [];
          });
          widget.onStrokeEnd?.call();
        },
        onPan: widget.onPan,
        onZoom: widget.onZoom,
        onTapSelect: widget.onTapSelect,
        onLiquifyDrag: widget.onLiquifyDrag,
      ),
    );
  }

  List<TopBarAction> _contextualActions() {
    switch (_s.tool) {
      case FeatherTool.draw:
        return [
          TopBarAction(
              icon: Icons.edit_outlined,
              label: 'Free',
              active: true,
              onTap: () {}),
          TopBarAction(
              icon: Icons.show_chart_rounded, label: 'Straight', onTap: () {}),
          TopBarAction(
              icon: Icons.hexagon_outlined, label: 'Shape', onTap: () {}),
        ];
      case FeatherTool.erase:
        return [
          TopBarAction(
              icon: Icons.auto_fix_high_outlined,
              label: 'Partial',
              active: true,
              onTap: () {}),
          TopBarAction(
              icon: Icons.delete_sweep_outlined, label: 'Whole', onTap: () {}),
        ];
      case FeatherTool.select:
        return [
          TopBarAction(
              icon: Icons.copy_all_rounded, label: 'Duplicate', onTap: () {}),
          TopBarAction(
              icon: Icons.content_copy_rounded, label: 'Stamp', onTap: () {}),
        ];
      default:
        return [];
    }
  }

  List<BottomBarAction> _bottomLeftActions() {
    return [
      BottomBarAction(
        icon: Icons.add_rounded,
        tooltip: 'Squeeze menu',
        onTap: () => setState(() {
          _showRadial = true;
          _radialAnchor = Offset.zero;
        }),
      ),
    ];
  }

  List<BottomBarAction> _bottomRightActions() {
    return [
      BottomBarAction(
        icon: Icons.layers_outlined,
        tooltip: 'Stage panel',
        onTap: () => setState(() => _showStage = true),
      ),
      BottomBarAction(
        icon: Icons.more_horiz_rounded,
        tooltip: 'More',
      ),
    ];
  }

  FeatherPalette _palette(BuildContext context) =>
      Theme.of(context).brightness == Brightness.dark
          ? FeatherColors.dark
          : FeatherColors.light;
}

/// Wraps a popover with a tap-to-close scrim and a small entrance
/// animation. Real blur comes from the GlassPanel inside each popover.
enum _PopoverAlignment { left, center }

class _Popover extends StatefulWidget {
  const _Popover({
    required this.anchor,
    required this.alignment,
    required this.child,
    required this.onClose,
  });

  final Offset anchor;
  final _PopoverAlignment alignment;
  final Widget child;
  final VoidCallback onClose;

  @override
  State<_Popover> createState() => _PopoverState();
}

class _PopoverState extends State<_Popover>
    with SingleTickerProviderStateMixin {
  late final AnimationController _c;
  late final Animation<double> _scale;
  late final Animation<double> _opacity;

  @override
  void initState() {
    super.initState();
    _c = AnimationController(
      vsync: this,
      duration: FeatherDurations.slow,
    );
    _scale = FeatherTweens.popoverScale.animate(
      CurvedAnimation(parent: _c, curve: FeatherCurves.popover),
    );
    _opacity = FeatherTweens.popoverOpacity.animate(
      CurvedAnimation(parent: _c, curve: FeatherCurves.toggle),
    );
    _c.forward();
  }

  @override
  void dispose() {
    _c.dispose();
    super.dispose();
  }

  @override
  Widget build(BuildContext context) {
    Alignment align;
    switch (widget.alignment) {
      case _PopoverAlignment.left:
        align = Alignment.centerLeft;
        break;
      case _PopoverAlignment.center:
        align = Alignment.center;
        break;
    }
    return Positioned.fill(
      child: GestureDetector(
        behavior: HitTestBehavior.translucent,
        onTap: widget.onClose,
        child: Container(
          color: Colors.black.withValues(alpha: 0.18),
          alignment: align,
          padding: const EdgeInsets.symmetric(horizontal: 200, vertical: 60),
          child: GestureDetector(
            onTap: () {}, // swallow taps inside the popover
            child: AnimatedBuilder(
              animation: _c,
              builder: (context, child) => Opacity(
                opacity: _opacity.value,
                child: Transform.scale(
                  scale: _scale.value,
                  alignment: Alignment.center,
                  child: child,
                ),
              ),
              child: Material(
                type: MaterialType.transparency,
                child: widget.child,
              ),
            ),
          ),
        ),
      ),
    );
  }
}

/// Tiny helper to fetch a tool's icon without re-declaring the
/// extension from tool_dock.dart (which is already imported).
IconData _iconFor(FeatherTool t) => t.icon;
