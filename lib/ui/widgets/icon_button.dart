// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// icon_button.dart — Feather-style icon button.
//
// Matches the Feather 3D tool dock & top bar aesthetic:
//   * outline icons (1.75 px stroke),
//   * 44 × 44 tappable area,
//   * soft glow halo when active,
//   * subtle press scale (FeatherCurves.press, 80 ms).
//
// The outline icons are drawn with a [CustomPainter] so we control the
// stroke width precisely — Material's [Icon] widget renders glyphs from
// a font and the stroke is baked in. If you need a real outline look,
// pass an [iconBuilder]; otherwise the Material glyph is fine for the
// production build.

import 'package:flutter/material.dart';

import '../theme/feather_animations.dart';
import '../theme/feather_colors.dart';
import '../theme/feather_typography.dart';
import '../theme/glassmorphism.dart';
import 'glass_panel.dart';

class FeatherIconButton extends StatefulWidget {
  const FeatherIconButton({
    super.key,
    required this.icon,
    this.iconBuilder,
    this.label,
    this.tooltip,
    this.onTap,
    this.onLongPress,
    this.isSelected = false,
    this.isActive = false,
    this.size = 44.0,
    this.iconSize = 22.0,
    this.strokeWidth = 1.75,
    this.color,
    this.activeColor,
    this.glowColor,
    this.spec = GlassSpec.subtle,
  });

  /// Material glyph used as a fallback.
  final IconData icon;

  /// Optional custom-painter outline icon. When non-null it overrides
  /// [icon]. Use this for crisp 1.75-px stroke Feather-style icons.
  final Widget? iconBuilder;

  final String? label;
  final String? tooltip;
  final VoidCallback? onTap;
  final VoidCallback? onLongPress;

  /// Selected = part of a single-choice group (tool dock).
  final bool isSelected;

  /// Active = a toggleable state (mirror, pressure).
  final bool isActive;

  final double size;
  final double iconSize;
  final double strokeWidth;

  final Color? color;
  final Color? activeColor;
  final Color? glowColor;
  final GlassSpec spec;

  @override
  State<FeatherIconButton> createState() => _FeatherIconButtonState();
}

class _FeatherIconButtonState extends State<FeatherIconButton>
    with SingleTickerProviderStateMixin {
  late final AnimationController _press;
  late final Animation<double> _scale;

  @override
  void initState() {
    super.initState();
    _press = AnimationController(
      vsync: this,
      duration: FeatherDurations.instant,
    );
    _scale = Tween<double>(begin: 1.0, end: 0.92).animate(
      CurvedAnimation(parent: _press, curve: FeatherCurves.press),
    );
  }

  @override
  void dispose() {
    _press.dispose();
    super.dispose();
  }

  @override
  Widget build(BuildContext context) {
    final palette = _palette(context);
    final accent = widget.activeColor ?? palette.accent;
    final isSelected = widget.isSelected || widget.isActive;

    Widget core = GlassPanel(
      spec: widget.spec,
      width: widget.size,
      height: widget.size,
      padding: EdgeInsets.zero,
      alignment: Alignment.center,
      child: AnimatedContainer(
        duration: FeatherDurations.quick,
        curve: FeatherCurves.toggle,
        decoration: BoxDecoration(
          color: isSelected
              ? accent.withValues(alpha: 0.22)
              : Colors.transparent,
          borderRadius: BorderRadius.circular(widget.spec.radius),
          boxShadow: isSelected
              ? [
                  BoxShadow(
                    color:
                        (widget.glowColor ?? accent).withValues(alpha: 0.55),
                    blurRadius: 14,
                    spreadRadius: 0,
                  ),
                ]
              : const [],
        ),
        child: _Icon(
          icon: widget.icon,
          iconBuilder: widget.iconBuilder,
          size: widget.iconSize,
          strokeWidth: widget.strokeWidth,
          color: isSelected
              ? (widget.activeColor ?? palette.textPrimary)
              : (widget.color ?? palette.textSecondary),
        ),
      ),
    );

    core = AnimatedBuilder(
      animation: _scale,
      builder: (context, child) =>
          Transform.scale(scale: _scale.value, child: child),
      child: core,
    );

    core = GestureDetector(
      behavior: HitTestBehavior.opaque,
      onTapDown: (_) => _press.forward(),
      onTapUp: (_) {
        _press.reverse();
        widget.onTap?.call();
      },
      onTapCancel: () => _press.reverse(),
      onLongPress: widget.onLongPress,
      child: core,
    );

    if (widget.label != null) {
      core = Column(
        mainAxisSize: MainAxisSize.min,
        children: [
          core,
          const SizedBox(height: 4),
          Text(
            widget.label!,
            style: FeatherTypography.micro.copyWith(
              color: isSelected ? palette.textPrimary : palette.textTertiary,
            ),
          ),
        ],
      );
    }

    if (widget.tooltip != null) {
      core = Tooltip(
        message: widget.tooltip!,
        child: core,
      );
    }

    return core;
  }

  FeatherPalette _palette(BuildContext context) {
    final theme = Theme.of(context);
    return theme.brightness == Brightness.dark
        ? FeatherColors.dark
        : FeatherColors.light;
  }
}

class _Icon extends StatelessWidget {
  const _Icon({
    required this.icon,
    required this.iconBuilder,
    required this.size,
    required this.strokeWidth,
    required this.color,
  });

  final IconData icon;
  final Widget? iconBuilder;
  final double size;
  final double strokeWidth;
  final Color color;

  @override
  Widget build(BuildContext context) {
    if (iconBuilder != null) {
      return IconTheme(
        data: IconThemeData(color: color, size: size),
        child: iconBuilder!,
      );
    }
    return Icon(icon, size: size, color: color);
  }
}
