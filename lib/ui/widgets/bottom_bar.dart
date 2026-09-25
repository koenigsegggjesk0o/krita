// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// bottom_bar.dart — Bottom floating pill (mode switcher).
//
// Per Feather 3D's design research:
//   * a single floating pill anchored at the bottom-center,
//   * 3 primary modes — Draw / Edit / Lift — switch the contextual UI,
//   * the pill is glassmorphism + a smooth animated selection pill behind
//     the active label,
//   * contextual right-side actions: undo/redo, apply, settings.
//
// The pill's selection indicator animates between the labels using an
// AnimatedPositioned (spring curve) for the Feather 3D "slide and pop"
// feel.

import 'package:flutter/material.dart';

import '../theme/feather_animations.dart';
import '../theme/feather_colors.dart';
import '../theme/feather_typography.dart';
import '../theme/glassmorphism.dart';
import 'glass_panel.dart';

enum FeatherMode { draw, edit, lift }

extension FeatherModeX on FeatherMode {
  String get label {
    switch (this) {
      case FeatherMode.draw:
        return 'Draw';
      case FeatherMode.edit:
        return 'Edit';
      case FeatherMode.lift:
        return 'Lift';
    }
  }

  IconData get icon {
    switch (this) {
      case FeatherMode.draw:
        return Icons.draw_outlined;
      case FeatherMode.edit:
        return Icons.tune_rounded;
      case FeatherMode.lift:
        return Icons.arrow_upward_rounded;
    }
  }

  Color color(FeatherPalette palette) {
    switch (this) {
      case FeatherMode.draw:
        return FeatherColors.toolDraw;
      case FeatherMode.edit:
        return FeatherColors.toolSelect;
      case FeatherMode.lift:
        return FeatherColors.toolLight;
    }
  }
}

class BottomBar extends StatelessWidget {
  const BottomBar({
    super.key,
    required this.mode,
    required this.onMode,
    this.leftActions = const [],
    this.rightActions = const [],
  });

  final FeatherMode mode;
  final ValueChanged<FeatherMode> onMode;
  final List<BottomBarAction> leftActions;
  final List<BottomBarAction> rightActions;

  @override
  Widget build(BuildContext context) {
    final palette = _palette(context);
    return GlassPanel(
      spec: GlassSpec.subtle,
      padding: const EdgeInsets.symmetric(horizontal: 8, vertical: 6),
      child: Row(
        mainAxisSize: MainAxisSize.min,
        children: [
          for (final a in leftActions) ...[
            _SideButton(action: a, palette: palette),
            const SizedBox(width: 4),
          ],
          if (leftActions.isNotEmpty) _divider(palette),
          _ModeSwitcher(
            mode: mode,
            palette: palette,
            onSelect: onMode,
          ),
          if (rightActions.isNotEmpty) _divider(palette),
          for (final a in rightActions) ...[
            const SizedBox(width: 4),
            _SideButton(action: a, palette: palette),
          ],
        ],
      ),
    );
  }

  Widget _divider(FeatherPalette p) => Container(
        height: 24,
        width: 1,
        margin: const EdgeInsets.symmetric(horizontal: 4),
        color: p.divider,
      );

  FeatherPalette _palette(BuildContext context) =>
      Theme.of(context).brightness == Brightness.dark
          ? FeatherColors.dark
          : FeatherColors.light;
}

class BottomBarAction {
  const BottomBarAction({
    required this.icon,
    this.label,
    this.tooltip,
    this.onTap,
    this.destructive = false,
    this.active = false,
    this.color,
  });

  final IconData icon;
  final String? label;
  final String? tooltip;
  final VoidCallback? onTap;
  final bool destructive;
  final bool active;
  final Color? color;
}

class _SideButton extends StatelessWidget {
  const _SideButton({required this.action, required this.palette});

  final BottomBarAction action;
  final FeatherPalette palette;

  @override
  Widget build(BuildContext context) {
    final color = action.destructive
        ? FeatherColors.toolErase
        : (action.color ?? (action.active ? palette.accent : palette.textPrimary));
    return Tooltip(
      message: action.tooltip ?? '',
      child: GestureDetector(
        onTap: action.onTap,
        child: Container(
          height: 36,
          padding: const EdgeInsets.symmetric(horizontal: 10),
          decoration: BoxDecoration(
            color: action.active
                ? color.withValues(alpha: 0.18)
                : palette.panelFill,
            borderRadius: BorderRadius.circular(10),
            border: Border.all(
              color: action.active ? color : palette.panelBorder,
              width: action.active ? 1.4 : 1,
            ),
          ),
          child: Row(
            mainAxisSize: MainAxisSize.min,
            children: [
              Icon(action.icon, size: 16, color: color),
              if (action.label != null) ...[
                const SizedBox(width: 6),
                Text(
                  action.label!,
                  style: FeatherTypography.caption
                      .copyWith(color: color, fontWeight: FontWeight.w600),
                ),
              ],
            ],
          ),
        ),
      ),
    );
  }
}

class _ModeSwitcher extends StatelessWidget {
  const _ModeSwitcher({
    required this.mode,
    required this.palette,
    required this.onSelect,
  });

  final FeatherMode mode;
  final FeatherPalette palette;
  final ValueChanged<FeatherMode> onSelect;

  static const _modes = FeatherMode.values;

  @override
  Widget build(BuildContext context) {
    final idx = _modes.indexOf(mode);
    return LayoutBuilder(
      builder: (context, c) {
        // Each button is 76 px wide; the indicator slides between them.
        const w = 76.0;
        return SizedBox(
          width: w * _modes.length,
          height: 36,
          child: Stack(
            children: [
              AnimatedPositioned(
                duration: FeatherDurations.slow,
                curve: FeatherCurves.popover,
                left: idx * w,
                top: 0,
                child: Container(
                  width: w,
                  height: 36,
                  decoration: BoxDecoration(
                    color: mode.color(palette).withValues(alpha: 0.22),
                    borderRadius: BorderRadius.circular(10),
                    border: Border.all(color: mode.color(palette), width: 1.4),
                    boxShadow: [
                      BoxShadow(
                        color: mode.color(palette).withValues(alpha: 0.5),
                        blurRadius: 12,
                      ),
                    ],
                  ),
                ),
              ),
              Row(
                children: [
                  for (final m in _modes)
                    GestureDetector(
                      onTap: () => onSelect(m),
                      behavior: HitTestBehavior.opaque,
                      child: SizedBox(
                        width: w,
                        height: 36,
                        child: Row(
                          mainAxisAlignment: MainAxisAlignment.center,
                          children: [
                            Icon(m.icon,
                                size: 14,
                                color: m == mode
                                    ? mode.color(palette)
                                    : palette.textSecondary),
                            const SizedBox(width: 4),
                            Text(
                              m.label,
                              style: FeatherTypography.caption.copyWith(
                                color: m == mode
                                    ? palette.textPrimary
                                    : palette.textTertiary,
                                fontWeight: FontWeight.w700,
                              ),
                            ),
                          ],
                        ),
                      ),
                    ),
                ],
              ),
            ],
          ),
        );
      },
    );
  }
}
