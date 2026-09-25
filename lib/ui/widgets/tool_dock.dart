// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// tool_dock.dart — Left vertical tool dock (icon-only, glassmorphism).
//
// The Feather 3D tool dock is a narrow (~50 px) floating strip on the
// *right* of the canvas — but in our redesign we keep the brush panel on
// the left and the tool dock on the right per the design brief. The icons
// are outline glyphs (1.75 px stroke) and the active tool glows.
//
// Tools (from interfaceandgestures_interface.txt):
//   Draw / Draw Shape, Erase / Vacuum, Select / Deselect, Mirror,
//   Clipboard, Stage Panel — plus a top system-menu cluster.

import 'package:flutter/material.dart';

import '../theme/feather_animations.dart';
import '../theme/feather_colors.dart';
import 'icon_button.dart';

enum FeatherTool {
  home,
  systemMenu,
  draw,
  // Sub-mode of [draw] reached by tapping the Draw dock button again
  // (per Feather's interface docs: "Draw and Draw Shape — switches
  // with each tap"). The dock still shows a single "draw" button; the
  // active sub-mode is tracked by the host's `_tool` field and surfaced
  // to the dock via [_drawIsSelected].
  drawShape,
  erase,
  // Sub-modes of [erase] reached by tapping the Erase dock button
  // again (per Feather's interface docs: "Erase and Vacuum — switches
  // with each tap"). The dock still shows a single "erase" button; the
  // active sub-mode is tracked by the host's `_tool` field and surfaced
  // to the dock via [_eraseIsSelected].
  eraser,
  vacuum,
  select,
  // Sub-mode of [select] reached by tapping the Select dock button
  // again (per Feather's interface docs: "Select and Deselect"). The
  // dock still shows a single "select" button; the active sub-mode is
  // tracked by the host's `_tool` field and surfaced to the dock via
  // [_selectIsSelected].
  deselect,
  mirror,
  clipboard,
  stage,
  liquify,
  transform,
}

/// True when [active] should highlight the dock's single Draw button
/// (i.e. the user is in the draw or drawShape state).
bool _drawIsSelected(FeatherTool active) =>
    active == FeatherTool.draw || active == FeatherTool.drawShape;

/// True when [active] should highlight the dock's single Erase button
/// (i.e. the user is in the eraser, vacuum, or pending-erase state).
bool _eraseIsSelected(FeatherTool active) =>
    active == FeatherTool.erase ||
    active == FeatherTool.eraser ||
    active == FeatherTool.vacuum;

/// True when [active] should highlight the dock's single Select button
/// (i.e. the user is in the select or deselect state).
bool _selectIsSelected(FeatherTool active) =>
    active == FeatherTool.select || active == FeatherTool.deselect;

extension FeatherToolX on FeatherTool {
  IconData get icon {
    switch (this) {
      case FeatherTool.home:
        return Icons.home_outlined;
      case FeatherTool.systemMenu:
        return Icons.menu_rounded;
      case FeatherTool.draw:
        return Icons.edit_outlined;
      case FeatherTool.drawShape:
        return Icons.hexagon_outlined;
      case FeatherTool.erase:
      case FeatherTool.eraser:
        return Icons.auto_fix_high_outlined;
      case FeatherTool.vacuum:
        return Icons.delete_sweep_outlined;
      case FeatherTool.select:
        return Icons.highlight_alt_outlined;
      case FeatherTool.deselect:
        return Icons.highlight_remove_outlined;
      case FeatherTool.mirror:
        return Icons.flip_outlined;
      case FeatherTool.clipboard:
        return Icons.attach_file_outlined;
      case FeatherTool.stage:
        return Icons.layers_outlined;
      case FeatherTool.liquify:
        return Icons.waves_outlined;
      case FeatherTool.transform:
        return Icons.open_with_outlined;
    }
  }

  String get tooltip {
    switch (this) {
      case FeatherTool.home:
        return 'Home';
      case FeatherTool.systemMenu:
        return 'System menu';
      case FeatherTool.draw:
        return 'Draw (free)';
      case FeatherTool.drawShape:
        return 'Draw Shape';
      case FeatherTool.erase:
      case FeatherTool.eraser:
        return 'Erase (partial)';
      case FeatherTool.vacuum:
        return 'Vacuum (whole curves)';
      case FeatherTool.select:
        return 'Select';
      case FeatherTool.deselect:
        return 'Deselect';
      case FeatherTool.mirror:
        return 'Mirror';
      case FeatherTool.clipboard:
        return 'Clipboard';
      case FeatherTool.stage:
        return 'Stage panel';
      case FeatherTool.liquify:
        return 'Liquify';
      case FeatherTool.transform:
        return 'Transform';
    }
  }

  Color color(FeatherPalette palette) {
    switch (this) {
      case FeatherTool.draw:
      case FeatherTool.drawShape:
        return FeatherColors.toolDraw;
      case FeatherTool.erase:
      case FeatherTool.eraser:
      case FeatherTool.vacuum:
        return FeatherColors.toolErase;
      case FeatherTool.select:
      case FeatherTool.deselect:
        return FeatherColors.toolSelect;
      case FeatherTool.mirror:
        return FeatherColors.toolMirror;
      case FeatherTool.liquify:
        return FeatherColors.toolLiquify;
      case FeatherTool.stage:
        return FeatherColors.toolStage;
      default:
        return palette.accent;
    }
  }
}

class ToolDock extends StatelessWidget {
  const ToolDock({
    super.key,
    required this.active,
    required this.onSelect,
    this.extra = const [],
    this.onHome,
    this.onSystemMenu,
  });

  final FeatherTool active;
  final ValueChanged<FeatherTool> onSelect;
  final List<FeatherTool> extra;
  final VoidCallback? onHome;
  final VoidCallback? onSystemMenu;

  static const List<FeatherTool> _core = [
    FeatherTool.draw,
    FeatherTool.erase,
    FeatherTool.select,
    FeatherTool.mirror,
    FeatherTool.clipboard,
    FeatherTool.stage,
    FeatherTool.liquify,
    FeatherTool.transform,
  ];

  @override
  Widget build(BuildContext context) {
    final palette = _palette(context);
    final tools = [..._core, ...extra];

    return Container(
      width: 56,
      padding: const EdgeInsets.symmetric(vertical: 10, horizontal: 4),
      decoration: BoxDecoration(
        color: palette.panelFill,
        borderRadius: BorderRadius.circular(20),
        border: Border.all(color: palette.panelBorder, width: 1),
        boxShadow: [
          BoxShadow(
            color: palette.shadow,
            blurRadius: 18,
            offset: const Offset(2, 0),
          ),
        ],
      ),
      child: Column(
        mainAxisSize: MainAxisSize.min,
        children: [
          FeatherIconButton(
            icon: FeatherTool.home.icon,
            tooltip: FeatherTool.home.tooltip,
            size: 44,
            iconSize: 20,
            onTap: onHome,
          ),
          const SizedBox(height: 6),
          FeatherIconButton(
            icon: FeatherTool.systemMenu.icon,
            tooltip: FeatherTool.systemMenu.tooltip,
            size: 44,
            iconSize: 20,
            onTap: onSystemMenu,
          ),
          Padding(
            padding: const EdgeInsets.symmetric(vertical: 8, horizontal: 6),
            child: Container(
              height: 1,
              color: palette.divider,
            ),
          ),
          for (final t in tools) ...[
            _DockButton(
              tool: t,
              // The Draw / Erase / Select dock buttons stay highlighted
              // for both of their respective sub-modes (single button,
              // cycled state per Feather's interface docs).
              selected: t == FeatherTool.draw
                  ? _drawIsSelected(active)
                  : t == FeatherTool.erase
                      ? _eraseIsSelected(active)
                      : t == FeatherTool.select
                          ? _selectIsSelected(active)
                          : t == active,
              onSelect: () => onSelect(t),
            ),
            if (t != tools.last) const SizedBox(height: 6),
          ],
        ],
      ),
    );
  }

  FeatherPalette _palette(BuildContext context) =>
      Theme.of(context).brightness == Brightness.dark
          ? FeatherColors.dark
          : FeatherColors.light;
}

class _DockButton extends StatefulWidget {
  const _DockButton({
    required this.tool,
    required this.selected,
    required this.onSelect,
  });

  final FeatherTool tool;
  final bool selected;
  final VoidCallback onSelect;

  @override
  State<_DockButton> createState() => _DockButtonState();
}

class _DockButtonState extends State<_DockButton>
    with SingleTickerProviderStateMixin {
  late final AnimationController _glow;

  @override
  void initState() {
    super.initState();
    _glow = AnimationController(
      vsync: this,
      duration: FeatherDurations.medium,
    );
    if (widget.selected) _glow.value = 1;
  }

  @override
  void didUpdateWidget(_DockButton old) {
    super.didUpdateWidget(old);
    if (old.selected != widget.selected) {
      if (widget.selected) {
        _glow.forward();
      } else {
        _glow.reverse();
      }
    }
  }

  @override
  void dispose() {
    _glow.dispose();
    super.dispose();
  }

  @override
  Widget build(BuildContext context) {
    final palette = _palette(context);
    final accent = widget.tool.color(palette);
    return FeatherIconButton(
      icon: widget.tool.icon,
      tooltip: widget.tool.tooltip,
      isSelected: widget.selected,
      size: 44,
      iconSize: 20,
      color: widget.selected ? accent : palette.textSecondary,
      activeColor: accent,
      glowColor: accent,
      onTap: widget.onSelect,
    );
  }

  FeatherPalette _palette(BuildContext context) =>
      Theme.of(context).brightness == Brightness.dark
          ? FeatherColors.dark
          : FeatherColors.light;
}
