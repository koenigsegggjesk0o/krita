// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// brush_picker.dart — Brush preset picker (circular thumbnails).
//
// Feather 3D shows brush presets as a horizontal strip of circular
// thumbnails with a selection ring on the active one. We render each
// preset's stroke preview via a CustomPainter (no asset needed).
//
// The picker is presented inside a GlassPanel popover.

import 'dart:math' as math;

import 'package:flutter/material.dart';

import '../theme/feather_animations.dart';
import '../theme/feather_colors.dart';
import '../theme/feather_typography.dart';
import '../theme/glassmorphism.dart';
import 'glass_panel.dart';

/// A brush preset's user-facing identity.
class BrushPreset {
  const BrushPreset({
    required this.id,
    required this.name,
    required this.previewColor,
    this.strokeWidth = 4,
    this.tapered = true,
    this.dashed = false,
    this.filePath,
  });

  final String id;
  final String name;
  final Color previewColor;
  final double strokeWidth;
  final bool tapered;
  final bool dashed;

  /// Absolute path to the source .kpp file, when this preset was scanned
  /// from disk at boot. Null for the bundled visual-only catalog entries
  /// (no real .kpp behind them). When non-null AND the real native Krita
  /// backend is live, the editor host feeds this path to
  /// [KritaBrushController.loadPreset] so the native bridge unpacks the
  /// .kpp container and parses the real paintop-settings. When null (or
  /// on the fallback engine) the host falls back to the visual-hints
  /// path (colour + strokeWidth only).
  final String? filePath;
}

class BrushPicker extends StatefulWidget {
  const BrushPicker({
    super.key,
    required this.presets,
    required this.selectedId,
    this.onSelected,
    this.onSave,
  });

  final List<BrushPreset> presets;
  final String? selectedId;
  final ValueChanged<BrushPreset>? onSelected;
  final VoidCallback? onSave;

  @override
  State<BrushPicker> createState() => _BrushPickerState();
}

class _BrushPickerState extends State<BrushPicker> {
  @override
  Widget build(BuildContext context) {
    final palette = _palette(context);
    return GlassPanel(
      spec: GlassSpec.strong,
      width: 360,
      padding: const EdgeInsets.all(16),
      child: Column(
        crossAxisAlignment: CrossAxisAlignment.stretch,
        mainAxisSize: MainAxisSize.min,
        children: [
          Row(
            children: [
              Text(
                'BRUSH PRESET',
                style: FeatherTypography.micro.copyWith(
                    color: palette.textTertiary, letterSpacing: 1.2),
              ),
              const Spacer(),
              GestureDetector(
                onTap: widget.onSave,
                child: Container(
                  padding:
                      const EdgeInsets.symmetric(horizontal: 8, vertical: 4),
                  decoration: BoxDecoration(
                    color: palette.accent.withValues(alpha: 0.22),
                    borderRadius: BorderRadius.circular(8),
                    border: Border.all(color: palette.accent),
                  ),
                  child: Row(
                    mainAxisSize: MainAxisSize.min,
                    children: [
                      Icon(Icons.bookmark_add_outlined,
                          size: 14, color: palette.accent),
                      const SizedBox(width: 4),
                      Text(
                        'SAVE',
                        style: FeatherTypography.micro
                            .copyWith(color: palette.accent),
                      ),
                    ],
                  ),
                ),
              ),
            ],
          ),
          const SizedBox(height: 12),
          SizedBox(
            height: 88,
            child: ListView.separated(
              scrollDirection: Axis.horizontal,
              itemCount: widget.presets.length,
              separatorBuilder: (_, __) => const SizedBox(width: 10),
              itemBuilder: (_, i) {
                final p = widget.presets[i];
                return _BrushThumb(
                  preset: p,
                  selected: p.id == widget.selectedId,
                  palette: palette,
                  onTap: () => widget.onSelected?.call(p),
                );
              },
            ),
          ),
          const SizedBox(height: 12),
          Text(
            widget.presets
                    .firstWhere(
                      (p) => p.id == widget.selectedId,
                      orElse: () => widget.presets.first,
                    )
                    .name,
            textAlign: TextAlign.center,
            style: FeatherTypography.bodyStrong.copyWith(
              color: palette.textPrimary,
            ),
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

class _BrushThumb extends StatelessWidget {
  const _BrushThumb({
    required this.preset,
    required this.selected,
    required this.palette,
    required this.onTap,
  });

  final BrushPreset preset;
  final bool selected;
  final FeatherPalette palette;
  final VoidCallback onTap;

  @override
  Widget build(BuildContext context) {
    return GestureDetector(
      onTap: onTap,
      child: AnimatedScale(
        scale: selected ? 1.08 : 1.0,
        duration: FeatherDurations.quick,
        curve: FeatherCurves.popover,
        child: Container(
          width: 64,
          height: 64,
          decoration: BoxDecoration(
            shape: BoxShape.circle,
            color: palette.panelFillStrong,
            border: Border.all(
              color: selected ? palette.accent : palette.panelBorder,
              width: selected ? 2.5 : 1,
            ),
            boxShadow: selected
                ? [
                    BoxShadow(
                      color: palette.accent.withValues(alpha: 0.5),
                      blurRadius: 12,
                    ),
                  ]
                : [],
          ),
          child: CustomPaint(
            painter: _BrushPreviewPainter(preset: preset),
          ),
        ),
      ),
    );
  }
}

class _BrushPreviewPainter extends CustomPainter {
  _BrushPreviewPainter({required this.preset});

  final BrushPreset preset;

  @override
  void paint(Canvas canvas, Size size) {
    final center = Offset(size.width / 2, size.height / 2);
    final path = Path();
    final w = preset.strokeWidth;
    final stroke = Paint()
      ..color = preset.previewColor
      ..style = PaintingStyle.stroke
      ..strokeWidth = w
      ..strokeCap = StrokeCap.round
      ..strokeJoin = StrokeJoin.round;

    final points = <Offset>[];
    for (int i = 0; i <= 24; i++) {
      final t = i / 24;
      final x = size.width * 0.18 + t * size.width * 0.64;
      final y =
          center.dy + math.sin(t * math.pi * 2.2) * size.height * 0.18;
      points.add(Offset(x, y));
    }
    // Build a path so we can dash if needed.
    path.moveTo(points.first.dx, points.first.dy);
    for (final p in points.skip(1)) {
      path.lineTo(p.dx, p.dy);
    }
    if (preset.dashed) {
      // Simple dashed effect — paint as series of segments.
      for (int i = 0; i < points.length - 1; i += 2) {
        canvas.drawLine(points[i], points[i + 1], stroke);
      }
    } else if (preset.tapered) {
      // Tapered — draw with two passes of decreasing width.
      canvas.drawPath(path, stroke);
      final tapered = Paint()
        ..color = preset.previewColor
        ..style = PaintingStyle.stroke
        ..strokeWidth = w * 0.5
        ..strokeCap = StrokeCap.round;
      canvas.drawPath(path, tapered);
    } else {
      canvas.drawPath(path, stroke);
    }
  }

  @override
  bool shouldRepaint(covariant _BrushPreviewPainter old) =>
      old.preset.id != preset.id;
}
