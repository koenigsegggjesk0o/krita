// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// material_picker.dart — Material type + pattern picker.
//
// Feather 3D materials (from brushes_materials.txt + the Feather 3D UI):
//   * Shadeless — flat, no light response. Patterns allowed.
//   * Shaded    — light + shadows. Patterns allowed.
//   * Glow      — glow-area effect, no light/shadow, no patterns.
//   * Cutout    — responds to the world/background, no patterns.
//   * Metallic  — polished metal: Lambert + high-spec Phong + sky-color
//                 environment reflection tint. v0.56-C made this a REAL
//                 engine material (see lib/engine/material/
//                 metallic_material.dart) — the host maps
//                 [FeatherMaterial.metallic] → [CanvasMaterial.metallic]
//                 so strokes render through the chrome catch-light path.
//                 Patterns are NOT supported on Metallic (polished metal
//                 has no engraving in the Feather doc model).
//
// Patterns (Shadeless/Shaded only): Dot, Line, Cross, Terrazzo, Stippled Dot.
// Each pattern has a slide for intensity / angle / contrast.
//
// The picker is presented inside a GlassPanel popover (typically summoned
// from the brush panel's "Material" chip).

import 'package:flutter/material.dart';

import '../theme/feather_animations.dart';
import '../theme/feather_colors.dart';
import '../theme/feather_typography.dart';
import '../theme/glassmorphism.dart';
import 'glass_panel.dart';

enum FeatherMaterial { shadeless, shaded, glow, cutout, metallic }

extension FeatherMaterialX on FeatherMaterial {
  String get label {
    switch (this) {
      case FeatherMaterial.shadeless:
        return 'Shadeless';
      case FeatherMaterial.shaded:
        return 'Shaded';
      case FeatherMaterial.glow:
        return 'Glow';
      case FeatherMaterial.cutout:
        return 'Cutout';
      case FeatherMaterial.metallic:
        return 'Metallic';
    }
  }

  IconData get icon {
    switch (this) {
      case FeatherMaterial.shadeless:
        return Icons.circle_outlined;
      case FeatherMaterial.shaded:
        return Icons.contrast_rounded;
      case FeatherMaterial.glow:
        return Icons.light_mode_outlined;
      case FeatherMaterial.cutout:
        return Icons.content_cut_rounded;
      case FeatherMaterial.metallic:
        return Icons.auto_fix_high_rounded;
    }
  }

  bool get supportsPattern => this == FeatherMaterial.shadeless || this == FeatherMaterial.shaded;
}

enum FeatherPattern { none, dot, line, cross, terrazzo, stippled }

extension FeatherPatternX on FeatherPattern {
  String get label {
    switch (this) {
      case FeatherPattern.none:
        return 'None';
      case FeatherPattern.dot:
        return 'Dot';
      case FeatherPattern.line:
        return 'Line';
      case FeatherPattern.cross:
        return 'Cross';
      case FeatherPattern.terrazzo:
        return 'Terrazzo';
      case FeatherPattern.stippled:
        return 'Stippled';
    }
  }
}

class MaterialPicker extends StatefulWidget {
  const MaterialPicker({
    super.key,
    required this.material,
    required this.pattern,
    this.intensity = 0.5,
    this.angle = 0.0,
    this.contrast = 0.5,
    this.glowIntensity = 0.7,
    this.onMaterial,
    this.onPattern,
    this.onIntensity,
    this.onAngle,
    this.onContrast,
    this.onGlowIntensity,
  });

  final FeatherMaterial material;
  final FeatherPattern pattern;
  final double intensity;
  final double angle;
  final double contrast;
  final double glowIntensity;
  final ValueChanged<FeatherMaterial>? onMaterial;
  final ValueChanged<FeatherPattern>? onPattern;
  final ValueChanged<double>? onIntensity;
  final ValueChanged<double>? onAngle;
  final ValueChanged<double>? onContrast;
  final ValueChanged<double>? onGlowIntensity;

  @override
  State<MaterialPicker> createState() => _MaterialPickerState();
}

class _MaterialPickerState extends State<MaterialPicker> {
  @override
  Widget build(BuildContext context) {
    final palette = _palette(context);
    return GlassPanel(
      spec: GlassSpec.strong,
      width: 280,
      padding: const EdgeInsets.all(16),
      child: Column(
        crossAxisAlignment: CrossAxisAlignment.stretch,
        mainAxisSize: MainAxisSize.min,
        children: [
          Text(
            'MATERIAL',
            style: FeatherTypography.micro
                .copyWith(color: palette.textTertiary, letterSpacing: 1.2),
          ),
          const SizedBox(height: 10),
          _MaterialGrid(
            current: widget.material,
            palette: palette,
            onSelect: widget.onMaterial,
          ),
          const SizedBox(height: 12),
          if (widget.material == FeatherMaterial.glow) ...[
            _LabeledSlider(
              label: 'Glow intensity',
              value: widget.glowIntensity,
              palette: palette,
              formatter: (v) => '${(v * 100).round()}%',
              onChanged: widget.onGlowIntensity,
            ),
          ] else if (widget.material.supportsPattern) ...[
            Text(
              'PATTERN',
              style: FeatherTypography.micro.copyWith(
                  color: palette.textTertiary, letterSpacing: 1.2),
            ),
            const SizedBox(height: 8),
            _PatternRow(
              current: widget.pattern,
              palette: palette,
              onSelect: widget.onPattern,
            ),
            if (widget.pattern != FeatherPattern.none) ...[
              const SizedBox(height: 12),
              _LabeledSlider(
                label: 'Intensity',
                value: widget.intensity,
                palette: palette,
                formatter: (v) => '${(v * 100).round()}',
                onChanged: widget.onIntensity,
              ),
              _LabeledSlider(
                label: 'Angle',
                value: widget.angle,
                palette: palette,
                formatter: (v) => '${(v * 360).round()}°',
                onChanged: widget.onAngle,
              ),
              _LabeledSlider(
                label: 'Contrast',
                value: widget.contrast,
                palette: palette,
                formatter: (v) => '${(v * 100).round()}%',
                onChanged: widget.onContrast,
              ),
            ],
          ] else ...[
            Text(
              'No pattern available for ${widget.material.label}.',
              style: FeatherTypography.caption
                  .copyWith(color: palette.textTertiary),
            ),
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

class _MaterialGrid extends StatelessWidget {
  const _MaterialGrid({
    required this.current,
    required this.palette,
    required this.onSelect,
  });

  final FeatherMaterial current;
  final FeatherPalette palette;
  final ValueChanged<FeatherMaterial>? onSelect;

  @override
  Widget build(BuildContext context) {
    final items = FeatherMaterial.values;
    return GridView.count(
      crossAxisCount: 2,
      shrinkWrap: true,
      physics: const NeverScrollableScrollPhysics(),
      mainAxisSpacing: 8,
      crossAxisSpacing: 8,
      childAspectRatio: 2.6,
      children: [
        for (final m in items)
          _MaterialChip(
            material: m,
            selected: m == current,
            palette: palette,
            onTap: () => onSelect?.call(m),
          ),
      ],
    );
  }
}

class _MaterialChip extends StatelessWidget {
  const _MaterialChip({
    required this.material,
    required this.selected,
    required this.palette,
    required this.onTap,
  });

  final FeatherMaterial material;
  final bool selected;
  final FeatherPalette palette;
  final VoidCallback onTap;

  @override
  Widget build(BuildContext context) {
    return GestureDetector(
      onTap: onTap,
      child: AnimatedContainer(
        duration: FeatherDurations.quick,
        curve: FeatherCurves.toggle,
        decoration: BoxDecoration(
          color: selected
              ? palette.accent.withValues(alpha: 0.22)
              : palette.panelFill,
          borderRadius: BorderRadius.circular(10),
          border: Border.all(
            color: selected ? palette.accent : palette.panelBorder,
            width: selected ? 1.5 : 1,
          ),
        ),
        padding: const EdgeInsets.symmetric(horizontal: 8, vertical: 6),
        child: Row(
          children: [
            Icon(material.icon,
                size: 16,
                color: selected ? palette.accent : palette.textSecondary),
            const SizedBox(width: 6),
            Expanded(
              child: Text(
                material.label,
                style: FeatherTypography.caption.copyWith(
                  color:
                      selected ? palette.textPrimary : palette.textSecondary,
                  fontWeight: FontWeight.w600,
                ),
              ),
            ),
          ],
        ),
      ),
    );
  }
}

class _PatternRow extends StatelessWidget {
  const _PatternRow({
    required this.current,
    required this.palette,
    required this.onSelect,
  });

  final FeatherPattern current;
  final FeatherPalette palette;
  final ValueChanged<FeatherPattern>? onSelect;

  @override
  Widget build(BuildContext context) {
    return Wrap(
      spacing: 6,
      runSpacing: 6,
      children: [
        for (final p in FeatherPattern.values)
          GestureDetector(
            onTap: () => onSelect?.call(p),
            child: AnimatedContainer(
              duration: FeatherDurations.quick,
              curve: FeatherCurves.toggle,
              padding:
                  const EdgeInsets.symmetric(horizontal: 10, vertical: 6),
              decoration: BoxDecoration(
                color: p == current
                    ? palette.accent.withValues(alpha: 0.22)
                    : palette.panelFill,
                borderRadius: BorderRadius.circular(8),
                border: Border.all(
                  color: p == current ? palette.accent : palette.panelBorder,
                ),
              ),
              child: Text(
                p.label,
                style: FeatherTypography.caption.copyWith(
                  color: p == current
                      ? palette.textPrimary
                      : palette.textSecondary,
                  fontWeight: FontWeight.w600,
                ),
              ),
            ),
          ),
      ],
    );
  }
}

class _LabeledSlider extends StatelessWidget {
  const _LabeledSlider({
    required this.label,
    required this.value,
    required this.palette,
    required this.formatter,
    required this.onChanged,
  });

  final String label;
  final double value;
  final FeatherPalette palette;
  final String Function(double) formatter;
  final ValueChanged<double>? onChanged;

  @override
  Widget build(BuildContext context) {
    return Padding(
      padding: const EdgeInsets.only(top: 8),
      child: Row(
        children: [
          SizedBox(
            width: 76,
            child: Text(
              label,
              style: FeatherTypography.caption.copyWith(
                  color: palette.textSecondary, fontWeight: FontWeight.w500),
            ),
          ),
          Expanded(
            child: SliderTheme(
              data: SliderTheme.of(context).copyWith(
                trackHeight: 3,
                thumbShape:
                    const RoundSliderThumbShape(enabledThumbRadius: 7),
                overlayShape:
                    const RoundSliderOverlayShape(overlayRadius: 14),
                activeTrackColor: palette.accent,
                inactiveTrackColor:
                    palette.textPrimary.withValues(alpha: 0.1),
                thumbColor: palette.textPrimary,
              ),
              child: Slider(
                value: value.clamp(0.0, 1.0),
                onChanged: onChanged,
              ),
            ),
          ),
          SizedBox(
            width: 44,
            child: Text(
              formatter(value),
              textAlign: TextAlign.right,
              style: FeatherTypography.mono.copyWith(color: palette.textPrimary),
            ),
          ),
        ],
      ),
    );
  }
}
