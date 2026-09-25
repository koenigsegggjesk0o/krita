// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// liquify_panel.dart — Liquify settings (Push / Pinch / Comb).
//
// Per liquify_editandapply.txt, the Liquify panel exposes:
//   * mode switcher — Push / Pinch / Comb,
//   * three circular sliders — Size, Range, Strength,
//   * bottom action row — Undo All / Compare (press-and-hold) / Apply.
//
// The panel slides in from the right when the Liquify tool is selected
// and the parent editor passes `visible: true`. Animation handled by the
// parent via AnimatedSlide/SizeTransition; this widget is the panel body.

import 'package:flutter/material.dart';

import '../theme/feather_animations.dart';
import '../theme/feather_colors.dart';
import '../theme/feather_typography.dart';
import '../theme/glassmorphism.dart';
import 'circular_slider.dart';
import 'glass_panel.dart';

enum LiquifyMode { push, pinch, comb }

extension LiquifyModeX on LiquifyMode {
  String get label {
    switch (this) {
      case LiquifyMode.push:
        return 'Push';
      case LiquifyMode.pinch:
        return 'Pinch';
      case LiquifyMode.comb:
        return 'Comb';
    }
  }

  IconData get icon {
    switch (this) {
      case LiquifyMode.push:
        return Icons.north_east_rounded;
      case LiquifyMode.pinch:
        return Icons.center_focus_strong_outlined;
      case LiquifyMode.comb:
        return Icons.waves_outlined;
    }
  }

  String get blurb {
    switch (this) {
      case LiquifyMode.push:
        return 'Distorts naturally, like pushing with a finger.';
      case LiquifyMode.pinch:
        return 'Sharp, precise distortion. Extend or shrink curves.';
      case LiquifyMode.comb:
        return 'Smooths and aligns. Best with a rubbing motion.';
    }
  }
}

class LiquifyPanel extends StatefulWidget {
  const LiquifyPanel({
    super.key,
    this.mode = LiquifyMode.push,
    this.size = 40,
    this.range = 60,
    this.strength = 0.5,
    this.onMode,
    this.onSize,
    this.onRange,
    this.onStrength,
    this.onUndoAll,
    this.onCompareStart,
    this.onCompareEnd,
    this.onApply,
  });

  final LiquifyMode mode;
  final double size;
  final double range;
  final double strength; // 0..1
  final ValueChanged<LiquifyMode>? onMode;
  final ValueChanged<double>? onSize;
  final ValueChanged<double>? onRange;
  final ValueChanged<double>? onStrength;
  final VoidCallback? onUndoAll;
  final VoidCallback? onCompareStart;
  final VoidCallback? onCompareEnd;
  final VoidCallback? onApply;

  @override
  State<LiquifyPanel> createState() => _LiquifyPanelState();
}

class _LiquifyPanelState extends State<LiquifyPanel> {
  @override
  Widget build(BuildContext context) {
    final palette = _palette(context);
    return GlassPanel(
      spec: GlassSpec.panel,
      width: 260,
      padding: const EdgeInsets.all(16),
      child: Column(
        mainAxisSize: MainAxisSize.min,
        crossAxisAlignment: CrossAxisAlignment.stretch,
        children: [
          Text(
            'LIQUIFY',
            style: FeatherTypography.micro
                .copyWith(color: palette.textTertiary, letterSpacing: 1.2),
          ),
          const SizedBox(height: 10),
          _ModeSwitcher(
            mode: widget.mode,
            palette: palette,
            onSelect: widget.onMode,
          ),
          const SizedBox(height: 8),
          Text(
            widget.mode.blurb,
            style: FeatherTypography.caption.copyWith(color: palette.textTertiary),
          ),
          const SizedBox(height: 12),
          Row(
            mainAxisAlignment: MainAxisAlignment.spaceAround,
            children: [
              _Slider(
                label: 'SIZE',
                value: widget.size,
                min: 1,
                max: 100,
                palette: palette,
                unit: '',
                onChanged: widget.onSize,
              ),
              _Slider(
                label: 'RANGE',
                value: widget.range,
                min: 1,
                max: 100,
                palette: palette,
                unit: '',
                onChanged: widget.onRange,
              ),
              _Slider(
                label: 'STR',
                value: widget.strength * 100,
                min: 0,
                max: 100,
                palette: palette,
                unit: '%',
                onChanged: (v) => widget.onStrength?.call(v / 100),
              ),
            ],
          ),
          const SizedBox(height: 14),
          _ActionBar(
            palette: palette,
            onUndoAll: widget.onUndoAll,
            onCompareStart: widget.onCompareStart,
            onCompareEnd: widget.onCompareEnd,
            onApply: widget.onApply,
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

class _ModeSwitcher extends StatelessWidget {
  const _ModeSwitcher({
    required this.mode,
    required this.palette,
    required this.onSelect,
  });

  final LiquifyMode mode;
  final FeatherPalette palette;
  final ValueChanged<LiquifyMode>? onSelect;

  @override
  Widget build(BuildContext context) {
    return Container(
      padding: const EdgeInsets.all(3),
      decoration: BoxDecoration(
        color: palette.panelFill,
        borderRadius: BorderRadius.circular(10),
        border: Border.all(color: palette.panelBorder),
      ),
      child: Row(
        children: [
          for (final m in LiquifyMode.values)
            Expanded(
              child: GestureDetector(
                onTap: () => onSelect?.call(m),
                child: AnimatedContainer(
                  duration: FeatherDurations.quick,
                  curve: FeatherCurves.toggle,
                  padding: const EdgeInsets.symmetric(vertical: 7),
                  decoration: BoxDecoration(
                    color: m == mode
                        ? FeatherColors.toolLiquify.withValues(alpha: 0.28)
                        : Colors.transparent,
                    borderRadius: BorderRadius.circular(8),
                  ),
                  child: Column(
                    children: [
                      Icon(m.icon,
                          size: 14,
                          color: m == mode
                              ? FeatherColors.toolLiquify
                              : palette.textTertiary),
                      const SizedBox(height: 2),
                      Text(
                        m.label,
                        style: FeatherTypography.micro.copyWith(
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
            ),
        ],
      ),
    );
  }
}

class _Slider extends StatelessWidget {
  const _Slider({
    required this.label,
    required this.value,
    required this.min,
    required this.max,
    required this.palette,
    required this.unit,
    required this.onChanged,
  });

  final String label;
  final double value;
  final double min;
  final double max;
  final FeatherPalette palette;
  final String unit;
  final ValueChanged<double>? onChanged;

  @override
  Widget build(BuildContext context) {
    return Column(
      mainAxisSize: MainAxisSize.min,
      children: [
        Text(
          label,
          style: FeatherTypography.micro
              .copyWith(color: palette.textTertiary, letterSpacing: 1.2),
        ),
        const SizedBox(height: 6),
        CircularSlider(
          value: value,
          min: min,
          max: max,
          diameter: 72,
          strokeWidth: 4,
          color: FeatherColors.toolLiquify,
          label: unit.isEmpty ? null : unit,
          valueFormatter: (v) => v.toStringAsFixed(0),
          onChanged: onChanged,
        ),
      ],
    );
  }
}

class _ActionBar extends StatelessWidget {
  const _ActionBar({
    required this.palette,
    required this.onUndoAll,
    required this.onCompareStart,
    required this.onCompareEnd,
    required this.onApply,
  });

  final FeatherPalette palette;
  final VoidCallback? onUndoAll;
  final VoidCallback? onCompareStart;
  final VoidCallback? onCompareEnd;
  final VoidCallback? onApply;

  @override
  Widget build(BuildContext context) {
    return Row(
      mainAxisAlignment: MainAxisAlignment.spaceBetween,
      children: [
        _ActionPill(
          palette: palette,
          icon: Icons.undo_rounded,
          label: 'Undo All',
          color: FeatherColors.toolErase,
          onTap: onUndoAll,
        ),
        GestureDetector(
          onTapDown: (_) => onCompareStart?.call(),
          onTapUp: (_) => onCompareEnd?.call(),
          onTapCancel: () => onCompareEnd?.call(),
          child: _ActionPill(
            palette: palette,
            icon: Icons.compare_arrows_rounded,
            label: 'Compare',
            color: palette.accent,
          ),
        ),
        _ActionPill(
          palette: palette,
          icon: Icons.check_rounded,
          label: 'Apply',
          color: FeatherColors.toolStage,
          onTap: onApply,
        ),
      ],
    );
  }
}

class _ActionPill extends StatelessWidget {
  const _ActionPill({
    required this.palette,
    required this.icon,
    required this.label,
    required this.color,
    this.onTap,
  });

  final FeatherPalette palette;
  final IconData icon;
  final String label;
  final Color color;
  final VoidCallback? onTap;

  @override
  Widget build(BuildContext context) {
    return GestureDetector(
      onTap: onTap,
      child: Container(
        padding: const EdgeInsets.symmetric(horizontal: 10, vertical: 8),
        decoration: BoxDecoration(
          color: color.withValues(alpha: 0.2),
          borderRadius: BorderRadius.circular(10),
          border: Border.all(color: color, width: 1.2),
        ),
        child: Column(
          mainAxisSize: MainAxisSize.min,
          children: [
            Icon(icon, color: color, size: 16),
            const SizedBox(height: 2),
            Text(
              label,
              style: FeatherTypography.micro.copyWith(
                color: color,
                fontWeight: FontWeight.w700,
              ),
            ),
          ],
        ),
      ),
    );
  }
}
