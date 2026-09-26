// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// top_bar.dart — Top floating bar (contextual actions).
//
// Feather 3D's top bar is contextual: it shows different actions when
// you're drawing vs. selecting vs. liquifying. The bar floats above
// the canvas (glassmorphism) and is anchored to the top-center.
//
// Layout (left→right):
//   * undo / redo (history),
//   * contextual cluster (draw tools, selection tools, liquify ops),
//   * view cluster (zoom, render toggle, hide UI),
//   * right-side: status (group name + active color).

import 'package:flutter/material.dart';

import '../theme/feather_colors.dart';
import '../theme/feather_typography.dart';
import '../theme/glassmorphism.dart';
import 'glass_panel.dart';
import 'icon_button.dart';

class TopBar extends StatelessWidget {
  const TopBar({
    super.key,
    required this.tool,
    this.groupName = 'Group 1',
    this.color = const Color(0xFF60A5FA),
    this.canUndo = false,
    this.canRedo = false,
    this.renderMode = false,
    this.onUndo,
    this.onRedo,
    this.onRenderToggle,
    this.onHideUI,
    this.onZoomIn,
    this.onZoomOut,
    this.onFit,
    this.onExport,
    this.onShare,
    // v0.55-A: About / Help button — opens the FeatherAboutDialog so a
    // first-run Windows user can self-diagnose engine status + native
    // lib path + crash log path + GitHub Releases URL.
    this.onAbout,
    this.contextualActions = const [],
  });

  final String tool;
  final String groupName;
  final Color color;
  final bool canUndo;
  final bool canRedo;
  final bool renderMode;
  final VoidCallback? onUndo;
  final VoidCallback? onRedo;
  final VoidCallback? onRenderToggle;
  final VoidCallback? onHideUI;
  final VoidCallback? onZoomIn;
  final VoidCallback? onZoomOut;
  final VoidCallback? onFit;

  /// Export menu — opens a sheet / popover with glTF / OBJ / PNG options
  /// (host owns the actual exporter calls + file picker wiring). Wired by
  /// [MainScreen._showExportSheet].
  final VoidCallback? onExport;

  /// Share — fires the platform share sheet for the last exported file
  /// (or, if none exists, kicks off a PNG snapshot then shares it). Wired
  /// by [MainScreen._shareLastExport].
  final VoidCallback? onShare;

  /// v0.55-A: About / Help — opens the FeatherAboutDialog (version,
  /// engine status, native lib path, crash log path, GitHub Releases
  /// link). Wired by [MainScreen._showAboutDialog].
  final VoidCallback? onAbout;
  final List<TopBarAction> contextualActions;

  @override
  Widget build(BuildContext context) {
    final palette = _palette(context);
    return GlassPanel(
      spec: GlassSpec.subtle,
      padding: const EdgeInsets.symmetric(horizontal: 8, vertical: 6),
      child: Row(
        mainAxisSize: MainAxisSize.min,
        children: [
          // History cluster.
          FeatherIconButton(
            icon: Icons.undo_rounded,
            tooltip: 'Undo',
            size: 38,
            iconSize: 18,
            color: canUndo ? palette.textPrimary : palette.textTertiary,
            onTap: canUndo ? onUndo : null,
          ),
          const SizedBox(width: 4),
          FeatherIconButton(
            icon: Icons.redo_rounded,
            tooltip: 'Redo',
            size: 38,
            iconSize: 18,
            color: canRedo ? palette.textPrimary : palette.textTertiary,
            onTap: canRedo ? onRedo : null,
          ),
          _divider(palette),
          // Contextual cluster.
          for (final a in contextualActions) ...[
            _ContextualButton(action: a, palette: palette),
            const SizedBox(width: 4),
          ],
          if (contextualActions.isNotEmpty) _divider(palette),
          // View cluster.
          FeatherIconButton(
            icon: Icons.zoom_out_rounded,
            tooltip: 'Zoom out',
            size: 38,
            iconSize: 18,
            onTap: onZoomOut,
          ),
          const SizedBox(width: 4),
          FeatherIconButton(
            icon: Icons.zoom_in_rounded,
            tooltip: 'Zoom in',
            size: 38,
            iconSize: 18,
            onTap: onZoomIn,
          ),
          const SizedBox(width: 4),
          FeatherIconButton(
            icon: Icons.fit_screen_outlined,
            tooltip: 'Fit',
            size: 38,
            iconSize: 18,
            onTap: onFit,
          ),
          _divider(palette),
          FeatherIconButton(
            icon: Icons.wb_incandescent_outlined,
            tooltip: 'Render mode',
            isActive: renderMode,
            activeColor: FeatherColors.toolLight,
            size: 38,
            iconSize: 18,
            onTap: onRenderToggle,
          ),
          const SizedBox(width: 4),
          FeatherIconButton(
            icon: Icons.visibility_off_outlined,
            tooltip: 'Hide UI',
            size: 38,
            iconSize: 18,
            onTap: onHideUI,
          ),
          _divider(palette),
          // Export cluster — glTF / OBJ / PNG via the host's export sheet.
          FeatherIconButton(
            icon: Icons.ios_share_rounded,
            tooltip: 'Export',
            size: 38,
            iconSize: 18,
            onTap: onExport,
          ),
          const SizedBox(width: 4),
          FeatherIconButton(
            icon: Icons.share_outlined,
            tooltip: 'Share',
            size: 38,
            iconSize: 18,
            onTap: onShare,
          ),
          const SizedBox(width: 4),
          // v0.55-A: About / Help button — self-diagnosis for first-run
          // Windows installs (engine status, native lib path, crash log,
          // GitHub Releases re-download link).
          FeatherIconButton(
            icon: Icons.help_outline_rounded,
            tooltip: 'About / Help',
            size: 38,
            iconSize: 18,
            onTap: onAbout,
          ),
          _divider(palette),
          // Status chip.
          _StatusChip(
            tool: tool,
            groupName: groupName,
            color: color,
            palette: palette,
          ),
        ],
      ),
    );
  }

  Widget _divider(FeatherPalette palette) => Container(
        height: 24,
        width: 1,
        margin: const EdgeInsets.symmetric(horizontal: 4),
        color: palette.divider,
      );

  FeatherPalette _palette(BuildContext context) =>
      Theme.of(context).brightness == Brightness.dark
          ? FeatherColors.dark
          : FeatherColors.light;
}

class TopBarAction {
  const TopBarAction({
    required this.icon,
    required this.label,
    this.active = false,
    this.onTap,
  });
  final IconData icon;
  final String label;
  final bool active;
  final VoidCallback? onTap;
}

class _ContextualButton extends StatelessWidget {
  const _ContextualButton({required this.action, required this.palette});

  final TopBarAction action;
  final FeatherPalette palette;

  @override
  Widget build(BuildContext context) {
    return FeatherIconButton(
      icon: action.icon,
      tooltip: action.label,
      isActive: action.active,
      size: 38,
      iconSize: 18,
      onTap: action.onTap,
    );
  }
}

class _StatusChip extends StatelessWidget {
  const _StatusChip({
    required this.tool,
    required this.groupName,
    required this.color,
    required this.palette,
  });

  final String tool;
  final String groupName;
  final Color color;
  final FeatherPalette palette;

  @override
  Widget build(BuildContext context) {
    return Container(
      padding: const EdgeInsets.symmetric(horizontal: 8, vertical: 6),
      decoration: BoxDecoration(
        color: palette.panelFill,
        borderRadius: BorderRadius.circular(8),
        border: Border.all(color: palette.panelBorder),
      ),
      child: Row(
        children: [
          Container(
            width: 10,
            height: 10,
            decoration: BoxDecoration(
              color: color,
              shape: BoxShape.circle,
              boxShadow: [
                BoxShadow(color: color.withValues(alpha: 0.6), blurRadius: 6),
              ],
            ),
          ),
          const SizedBox(width: 6),
          Text(
            tool,
            style: FeatherTypography.micro
                .copyWith(color: palette.textTertiary, letterSpacing: 0.8),
          ),
          const SizedBox(width: 8),
          Container(width: 1, height: 12, color: palette.divider),
          const SizedBox(width: 8),
          Text(
            groupName,
            style: FeatherTypography.caption
                .copyWith(color: palette.textPrimary, fontWeight: FontWeight.w600),
          ),
        ],
      ),
    );
  }
}
