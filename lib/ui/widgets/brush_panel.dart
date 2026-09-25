// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// brush_panel.dart — Brush settings sidebar (left of the canvas).
//
// Mirrors Feather 3D's left sidebar exactly:
//   1. Brush Type (icon + label)        — tap/drag to change brush,
//   2. Active Color                     — opens ColorWheel popover,
//   3. Brush Size circular slider       — 1–300 mm,
//   4. Opacity circular slider          — 0–100 %,
//   5. Pressure sensitivity toggle,
//   6. Injector toggle,
//   7. Brush Preset quick-open button,
//   8. Material quick-open button,
//   9. Pattern quick-open button,
//  10. History cluster (Undo / Redo)   — per brushes_interface.txt
//                                      History Panel.
//
// All circular sliders are 96 px diameter to fit the narrow ~140 px wide
// sidebar without crowding. Tap a slider to expand the inline numpad.

import 'package:flutter/material.dart';

import '../theme/feather_animations.dart';
import '../theme/feather_colors.dart';
import '../theme/feather_typography.dart';
import '../theme/glassmorphism.dart';
import 'circular_slider.dart';
import 'glass_panel.dart';
import 'icon_button.dart';
import 'material_picker.dart';

class BrushPanel extends StatefulWidget {
  const BrushPanel({
    super.key,
    required this.brushType,
    required this.color,
    required this.size,
    required this.opacity,
    this.pressure = true,
    this.injector = false,
    this.material = FeatherMaterial.shaded,
    this.pattern = FeatherPattern.none,
    this.canUndo = false,
    this.canRedo = false,
    this.onBrushType,
    this.onColor,
    this.onSize,
    this.onOpacity,
    this.onPressure,
    this.onInjector,
    this.onOpenPresets,
    this.onOpenColor,
    this.onOpenMaterial,
    this.onOpenPattern,
    this.onUndo,
    this.onRedo,
  });

  final IconData brushType;
  final Color color;
  final double size; // mm, 1..300
  final double opacity; // 0..1
  final bool pressure;
  final bool injector;
  final FeatherMaterial material;
  final FeatherPattern pattern;
  final bool canUndo;
  final bool canRedo;

  final VoidCallback? onBrushType;
  final ValueChanged<Color>? onColor;
  final ValueChanged<double>? onSize;
  final ValueChanged<double>? onOpacity;
  final ValueChanged<bool>? onPressure;
  final ValueChanged<bool>? onInjector;
  final VoidCallback? onOpenPresets;
  final VoidCallback? onOpenColor;
  final VoidCallback? onOpenMaterial;
  final VoidCallback? onOpenPattern;
  final VoidCallback? onUndo;
  final VoidCallback? onRedo;

  @override
  State<BrushPanel> createState() => _BrushPanelState();
}

class _BrushPanelState extends State<BrushPanel> {
  bool _numpadSize = false;
  bool _numpadOpacity = false;

  @override
  Widget build(BuildContext context) {
    final palette = _palette(context);
    return GlassPanel(
      spec: GlassSpec.panel,
      width: 140,
      padding: const EdgeInsets.symmetric(horizontal: 14, vertical: 16),
      child: Column(
        mainAxisSize: MainAxisSize.min,
        crossAxisAlignment: CrossAxisAlignment.stretch,
        children: [
          // 1. Brush Type.
          _BrushTypeButton(
            icon: widget.brushType,
            color: widget.color,
            palette: palette,
            onTap: widget.onBrushType,
          ),
          const SizedBox(height: 12),
          // 2. Active color swatch.
          _ActiveColorButton(
            color: widget.color,
            palette: palette,
            onTap: widget.onOpenColor,
          ),
          const SizedBox(height: 16),
          // 3. Size slider.
          _SliderBlock(
            label: 'SIZE',
            value: widget.size,
            min: 1,
            max: 300,
            palette: palette,
            unit: 'mm',
            expanded: _numpadSize,
            onToggleExpand: () =>
                setState(() => _numpadSize = !_numpadSize),
            onChanged: widget.onSize,
          ),
          const SizedBox(height: 12),
          // 4. Opacity slider.
          _SliderBlock(
            label: 'OPACITY',
            value: widget.opacity * 100,
            min: 0,
            max: 100,
            palette: palette,
            unit: '%',
            expanded: _numpadOpacity,
            onToggleExpand: () =>
                setState(() => _numpadOpacity = !_numpadOpacity),
            onChanged: (v) => widget.onOpacity?.call(v / 100),
          ),
          const SizedBox(height: 12),
          // 5. Pressure toggle.
          _ToggleRow(
            icon: Icons.touch_app_outlined,
            label: 'Pressure',
            active: widget.pressure,
            palette: palette,
            onTap: () => widget.onPressure?.call(!widget.pressure),
          ),
          const SizedBox(height: 8),
          // 6. Injector toggle.
          _ToggleRow(
            icon: Icons.colorize_outlined,
            label: 'Injector',
            active: widget.injector,
            palette: palette,
            onTap: () => widget.onInjector?.call(!widget.injector),
          ),
          const Divider(),
          // 7. Presets quick-open + material + pattern.
          Row(
            mainAxisAlignment: MainAxisAlignment.spaceBetween,
            children: [
              FeatherIconButton(
                icon: Icons.bookmark_border_rounded,
                tooltip: 'Brush presets',
                size: 40,
                iconSize: 18,
                onTap: widget.onOpenPresets,
              ),
              FeatherIconButton(
                icon: Icons.layers_outlined,
                tooltip: 'Material',
                size: 40,
                iconSize: 18,
                onTap: widget.onOpenMaterial,
              ),
              FeatherIconButton(
                icon: Icons.pattern_outlined,
                tooltip: 'Pattern',
                size: 40,
                iconSize: 18,
                onTap: widget.onOpenPattern,
              ),
            ],
          ),
          const Divider(),
          // 8. History cluster (Undo / Redo) — per brushes_interface.txt
          // History Panel.
          _HistoryRow(
            canUndo: widget.canUndo,
            canRedo: widget.canRedo,
            palette: palette,
            onUndo: widget.onUndo,
            onRedo: widget.onRedo,
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

class _BrushTypeButton extends StatelessWidget {
  const _BrushTypeButton({
    required this.icon,
    required this.color,
    required this.palette,
    required this.onTap,
  });

  final IconData icon;
  final Color color;
  final FeatherPalette palette;
  final VoidCallback? onTap;

  @override
  Widget build(BuildContext context) {
    return GestureDetector(
      onTap: onTap,
      child: Container(
        height: 56,
        decoration: BoxDecoration(
          color: palette.panelFill,
          borderRadius: BorderRadius.circular(12),
          border: Border.all(color: palette.panelBorder),
        ),
        child: Row(
          children: [
            const SizedBox(width: 10),
            Container(
              width: 32,
              height: 32,
              decoration: BoxDecoration(
                color: color,
                shape: BoxShape.circle,
                border: Border.all(
                  color: palette.textPrimary.withValues(alpha: 0.4),
                ),
              ),
              child: Icon(icon, color: palette.textInverse, size: 16),
            ),
            const Spacer(),
            Icon(Icons.unfold_more_rounded,
                size: 14, color: palette.textTertiary),
            const SizedBox(width: 10),
          ],
        ),
      ),
    );
  }
}

class _ActiveColorButton extends StatelessWidget {
  const _ActiveColorButton({
    required this.color,
    required this.palette,
    required this.onTap,
  });

  final Color color;
  final FeatherPalette palette;
  final VoidCallback? onTap;

  @override
  Widget build(BuildContext context) {
    return GestureDetector(
      onTap: onTap,
      child: Container(
        height: 32,
        decoration: BoxDecoration(
          color: color,
          borderRadius: BorderRadius.circular(8),
          border: Border.all(
            color: palette.textPrimary.withValues(alpha: 0.4),
          ),
        ),
      ),
    );
  }
}

class _SliderBlock extends StatelessWidget {
  const _SliderBlock({
    required this.label,
    required this.value,
    required this.min,
    required this.max,
    required this.palette,
    required this.unit,
    required this.expanded,
    required this.onToggleExpand,
    required this.onChanged,
  });

  final String label;
  final double value;
  final double min;
  final double max;
  final FeatherPalette palette;
  final String unit;
  final bool expanded;
  final VoidCallback onToggleExpand;
  final ValueChanged<double>? onChanged;

  @override
  Widget build(BuildContext context) {
    return Column(
      children: [
        Row(
          mainAxisAlignment: MainAxisAlignment.center,
          children: [
            Text(
              label,
              style: FeatherTypography.micro.copyWith(
                color: palette.textTertiary,
                letterSpacing: 1.2,
              ),
            ),
          ],
        ),
        const SizedBox(height: 6),
        GestureDetector(
          onTap: onToggleExpand,
          child: CircularSlider(
            value: value,
            min: min,
            max: max,
            diameter: 96,
            strokeWidth: 5,
            label: unit,
            valueFormatter: (v) => v.toStringAsFixed(0),
            onChanged: onChanged,
          ),
        ),
        AnimatedCrossFade(
          duration: FeatherDurations.medium,
          crossFadeState: expanded
              ? CrossFadeState.showSecond
              : CrossFadeState.showFirst,
          firstChild: const SizedBox(height: 0, width: double.infinity),
          secondChild: Padding(
            padding: const EdgeInsets.only(top: 8),
            child: _Numpad(
              palette: palette,
              value: value,
              unit: unit,
              onChanged: onChanged,
            ),
          ),
        ),
      ],
    );
  }
}

class _Numpad extends StatelessWidget {
  const _Numpad({
    required this.palette,
    required this.value,
    required this.unit,
    required this.onChanged,
  });

  final FeatherPalette palette;
  final double value;
  final String unit;
  final ValueChanged<double>? onChanged;

  @override
  Widget build(BuildContext context) {
    return Wrap(
      alignment: WrapAlignment.center,
      spacing: 4,
      runSpacing: 4,
      children: [
        for (final v in const [1, 5, 10, 25, 50, 100, 200, 300])
          GestureDetector(
            onTap: () => onChanged?.call(v.toDouble()),
            child: Container(
              width: 28,
              height: 22,
              alignment: Alignment.center,
              decoration: BoxDecoration(
                color: (value - v).abs() < 0.01
                    ? palette.accent.withValues(alpha: 0.25)
                    : palette.panelFill,
                borderRadius: BorderRadius.circular(6),
                border: Border.all(color: palette.panelBorder),
              ),
              child: Text(
                '$v',
                style: FeatherTypography.micro.copyWith(
                  color: palette.textSecondary,
                ),
              ),
            ),
          ),
      ],
    );
  }
}

class _ToggleRow extends StatelessWidget {
  const _ToggleRow({
    required this.icon,
    required this.label,
    required this.active,
    required this.palette,
    required this.onTap,
  });

  final IconData icon;
  final String label;
  final bool active;
  final FeatherPalette palette;
  final VoidCallback onTap;

  @override
  Widget build(BuildContext context) {
    return GestureDetector(
      behavior: HitTestBehavior.opaque,
      onTap: onTap,
      child: Container(
        height: 36,
        padding: const EdgeInsets.symmetric(horizontal: 8),
        decoration: BoxDecoration(
          color: active
              ? palette.accent.withValues(alpha: 0.18)
              : palette.panelFill,
          borderRadius: BorderRadius.circular(10),
          border: Border.all(
            color: active ? palette.accent : palette.panelBorder,
            width: active ? 1.4 : 1,
          ),
        ),
        child: Row(
          children: [
            Icon(icon,
                size: 16,
                color: active ? palette.accent : palette.textSecondary),
            const SizedBox(width: 8),
            Expanded(
              child: Text(
                label,
                style: FeatherTypography.caption.copyWith(
                  color: active ? palette.textPrimary : palette.textSecondary,
                  fontWeight: FontWeight.w600,
                ),
              ),
            ),
            Icon(
              active ? Icons.toggle_on_rounded : Icons.toggle_off_rounded,
              size: 20,
              color: active ? palette.accent : palette.textTertiary,
            ),
          ],
        ),
      ),
    );
  }
}

/// History cluster (Undo / Redo) — mirrors Feather's left-sidebar
/// History Panel (per brushes_interface.txt §13). Two side-by-side
/// icon buttons; disabled when the host reports `canUndo` / `canRedo`
/// as false.
class _HistoryRow extends StatelessWidget {
  const _HistoryRow({
    required this.canUndo,
    required this.canRedo,
    required this.palette,
    this.onUndo,
    this.onRedo,
  });

  final bool canUndo;
  final bool canRedo;
  final FeatherPalette palette;
  final VoidCallback? onUndo;
  final VoidCallback? onRedo;

  @override
  Widget build(BuildContext context) {
    return Padding(
      padding: const EdgeInsets.only(top: 4),
      child: Row(
        mainAxisAlignment: MainAxisAlignment.spaceBetween,
        children: [
          _HistoryButton(
            icon: Icons.undo_rounded,
            label: 'Undo',
            enabled: canUndo,
            palette: palette,
            onTap: onUndo,
          ),
          _HistoryButton(
            icon: Icons.redo_rounded,
            label: 'Redo',
            enabled: canRedo,
            palette: palette,
            onTap: onRedo,
          ),
        ],
      ),
    );
  }
}

class _HistoryButton extends StatelessWidget {
  const _HistoryButton({
    required this.icon,
    required this.label,
    required this.enabled,
    required this.palette,
    this.onTap,
  });

  final IconData icon;
  final String label;
  final bool enabled;
  final FeatherPalette palette;
  final VoidCallback? onTap;

  @override
  Widget build(BuildContext context) {
    final color = enabled ? palette.accent : palette.textTertiary;
    return Tooltip(
      message: label,
      child: GestureDetector(
        behavior: HitTestBehavior.opaque,
        onTap: enabled ? onTap : null,
        child: Container(
          width: 56,
          height: 36,
          decoration: BoxDecoration(
            color: enabled
                ? palette.accent.withValues(alpha: 0.16)
                : palette.panelFill,
            borderRadius: BorderRadius.circular(10),
            border: Border.all(
              color: enabled ? palette.accent : palette.panelBorder,
              width: enabled ? 1.2 : 1,
            ),
          ),
          child: Row(
            mainAxisAlignment: MainAxisAlignment.center,
            children: [
              Icon(icon, size: 16, color: color),
              const SizedBox(width: 4),
              Text(
                label,
                style: FeatherTypography.micro.copyWith(
                  color: color,
                  fontWeight: FontWeight.w700,
                  letterSpacing: 0.4,
                ),
              ),
            ],
          ),
        ),
      ),
    );
  }
}
