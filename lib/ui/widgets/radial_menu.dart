// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// radial_menu.dart — Squeeze / radial contextual menu.
//
// Per interfaceandgestures_squeezemenu.txt: when the user squeezes
// Apple Pencil (or long-presses on touch), a radial menu unfolds at
// the last interaction point. The default menu has Undo / Redo / Find
// Group; the contextual menu adds tool-specific items (Add Group,
// Quick Brush, Recall Guide, Select All, Stamp).
//
// The radial menu is rendered as:
//   * a center "grip" button,
//   * N items placed around a circle, each a circular icon button,
//   * the items pop in with [FeatherCurves.popover] (elasticOut),
//   * tap-outside-to-cancel handled by the parent overlay.

import 'dart:math' as math;

import 'package:flutter/material.dart';

import '../theme/feather_animations.dart';
import '../theme/feather_colors.dart';
import '../theme/feather_typography.dart';

class RadialMenuItem {
  const RadialMenuItem({
    required this.icon,
    required this.label,
    this.onTap,
    this.color,
    this.destructive = false,
  });

  final IconData icon;
  final String label;
  final VoidCallback? onTap;
  final Color? color;
  final bool destructive;
}

class RadialMenu extends StatefulWidget {
  const RadialMenu({
    super.key,
    required this.items,
    this.center,
    this.radius = 96.0,
    this.itemSize = 48.0,
    this.onDismiss,
  });

  final List<RadialMenuItem> items;
  final RadialMenuItem? center;
  final double radius;
  final double itemSize;
  final VoidCallback? onDismiss;

  @override
  State<RadialMenu> createState() => _RadialMenuState();
}

class _RadialMenuState extends State<RadialMenu>
    with SingleTickerProviderStateMixin {
  late final AnimationController _ctrl;
  late final Animation<double> _scale;
  late final Animation<double> _opacity;

  @override
  void initState() {
    super.initState();
    _ctrl = AnimationController(
      vsync: this,
      duration: FeatherDurations.slow,
    );
    _scale = Tween<double>(begin: 0.4, end: 1.0).animate(
      CurvedAnimation(parent: _ctrl, curve: FeatherCurves.popover),
    );
    _opacity = Tween<double>(begin: 0.0, end: 1.0).animate(
      CurvedAnimation(parent: _ctrl, curve: FeatherCurves.toggle),
    );
    _ctrl.forward();
  }

  @override
  void dispose() {
    _ctrl.dispose();
    super.dispose();
  }

  void _dismiss() {
    _ctrl.reverse().then((_) => widget.onDismiss?.call());
  }

  @override
  Widget build(BuildContext context) {
    final palette = _palette(context);
    final n = widget.items.length;
    return GestureDetector(
      behavior: HitTestBehavior.translucent,
      onTap: _dismiss,
      child: Stack(
        alignment: Alignment.center,
        children: [
          // Dim scrim.
          Positioned.fill(
            child: Container(color: palette.shadow.withValues(alpha: 0.18)),
          ),
          // Radial layout.
          AnimatedBuilder(
            animation: _scale,
            builder: (context, _) {
              return Opacity(
                opacity: _opacity.value,
                child: Transform.scale(
                  scale: _scale.value,
                  child: SizedBox(
                    width: widget.radius * 2 + widget.itemSize,
                    height: widget.radius * 2 + widget.itemSize,
                    child: Stack(
                      alignment: Alignment.center,
                      children: [
                        // Center button.
                        if (widget.center != null)
                          _RadialItem(
                            item: widget.center!,
                            size: widget.itemSize + 6,
                            palette: palette,
                            onTap: () {
                              widget.center?.onTap?.call();
                              _dismiss();
                            },
                          ),
                        // Radial items.
                        for (int i = 0; i < n; i++)
                          _PositionedRadialItem(
                            index: i,
                            total: n,
                            radius: widget.radius,
                            child: _RadialItem(
                              item: widget.items[i],
                              size: widget.itemSize,
                              palette: palette,
                              onTap: () {
                                widget.items[i].onTap?.call();
                                _dismiss();
                              },
                            ),
                          ),
                      ],
                    ),
                  ),
                ),
              );
            },
          ),
        ],
      ),
    );
  }

  FeatherPalette _palette(BuildContext context) =>
      Theme.of(context).brightness == Brightness.dark
          ? FeatherColors.dark
          : FeatherColors.light;
}

class _PositionedRadialItem extends StatelessWidget {
  const _PositionedRadialItem({
    required this.index,
    required this.total,
    required this.radius,
    required this.child,
  });

  final int index;
  final int total;
  final double radius;
  final Widget child;

  @override
  Widget build(BuildContext context) {
    // Start at -90° (top) and go clockwise.
    final angle = -math.pi / 2 + (index / total) * 2 * math.pi;
    final dx = radius * math.cos(angle);
    final dy = radius * math.sin(angle);
    return Transform.translate(
      offset: Offset(dx, dy),
      child: child,
    );
  }
}

class _RadialItem extends StatelessWidget {
  const _RadialItem({
    required this.item,
    required this.size,
    required this.palette,
    required this.onTap,
  });

  final RadialMenuItem item;
  final double size;
  final FeatherPalette palette;
  final VoidCallback onTap;

  @override
  Widget build(BuildContext context) {
    final color = item.destructive
        ? FeatherColors.toolErase
        : (item.color ?? palette.accent);
    return GestureDetector(
      onTap: onTap,
      child: Column(
        mainAxisSize: MainAxisSize.min,
        children: [
          Container(
            width: size,
            height: size,
            decoration: BoxDecoration(
              shape: BoxShape.circle,
              color: palette.panelFillStrong,
              border: Border.all(color: color, width: 1.5),
              boxShadow: [
                BoxShadow(
                  color: color.withValues(alpha: 0.5),
                  blurRadius: 12,
                ),
              ],
            ),
            child: Icon(item.icon, color: color, size: size * 0.42),
          ),
          const SizedBox(height: 4),
          Container(
            padding: const EdgeInsets.symmetric(horizontal: 6, vertical: 2),
            decoration: BoxDecoration(
              color: palette.panelFillStrong,
              borderRadius: BorderRadius.circular(6),
              border: Border.all(color: palette.panelBorder),
            ),
            child: Text(
              item.label,
              style: FeatherTypography.micro.copyWith(
                color: palette.textPrimary,
                fontWeight: FontWeight.w700,
              ),
            ),
          ),
        ],
      ),
    );
  }
}
