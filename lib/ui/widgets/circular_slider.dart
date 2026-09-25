// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// circular_slider.dart — Radial slider used by brush size, opacity,
// liquify strength and other Feather-style numeric controls.
//
// The Feather 3D UI uses *circular* sliders for everything brush-related
// (size, opacity, range, strength). Linear sliders only appear in the
// expanded numpad view. This widget is the canonical radial control:
//
//   * a 360° track with a filled arc from -90° (top) clockwise,
//   * a draggable knob,
//   * an optional value label in the center,
//   * an optional unit suffix,
//   * tap-to-jump + drag with momentum.
//
// Pure-CustomPainter drawing — no SVG assets required.

import 'dart:math' as math;

import 'package:flutter/material.dart';

import '../theme/feather_animations.dart';
import '../theme/feather_colors.dart';
import '../theme/feather_typography.dart';

class CircularSlider extends StatefulWidget {
  const CircularSlider({
    super.key,
    required this.value,
    required this.min,
    required this.max,
    this.defaultValue,
    this.step,
    this.onChanged,
    this.onChangeStart,
    this.onChangeEnd,
    this.label,
    this.valueFormatter,
    this.diameter = 96.0,
    this.strokeWidth = 6.0,
    this.color,
    this.trackColor,
    this.knobRadius = 8.0,
    this.snapToDefault = false,
  });

  final double value;
  final double min;
  final double max;
  final double? defaultValue;
  final double? step;
  final ValueChanged<double>? onChanged;
  final VoidCallback? onChangeStart;
  final VoidCallback? onChangeEnd;
  final String? label;
  final String Function(double)? valueFormatter;
  final double diameter;
  final double strokeWidth;
  final Color? color;
  final Color? trackColor;
  final double knobRadius;
  final bool snapToDefault;

  @override
  State<CircularSlider> createState() => _CircularSliderState();
}

class _CircularSliderState extends State<CircularSlider> {
  bool _dragging = false;

  double get _span => (widget.max - widget.min).abs();

  double _valueFromAngle(double radians) {
    final norm = ((radians / (2 * math.pi)) % 1.0 + 1.0) % 1.0;
    var v = widget.min + norm * _span;
    if (widget.step != null && widget.step! > 0) {
      v = (v / widget.step!).round() * widget.step!;
    }
    return v.clamp(widget.min, widget.max);
  }

  double get _norm {
    if (_span == 0) return 0;
    return ((widget.value - widget.min) / _span).clamp(0.0, 1.0);
  }

  void _handlePanStart(Offset local, Size size) {
    widget.onChangeStart?.call();
    setState(() => _dragging = true);
    _emit(local, size);
  }

  void _handlePanUpdate(Offset local, Size size) => _emit(local, size);

  void _handlePanEnd(_) {
    setState(() => _dragging = false);
    widget.onChangeEnd?.call();
  }

  void _emit(Offset local, Size size) {
    final center = Offset(size.width / 2, size.height / 2);
    final dx = local.dx - center.dx;
    final dy = local.dy - center.dy;
    // Start at top (-90°) and go clockwise.
    var angle = math.atan2(dy, dx) + math.pi / 2;
    if (angle < 0) angle += 2 * math.pi;
    var v = _valueFromAngle(angle);
    if (widget.snapToDefault &&
        widget.defaultValue != null &&
        (v - widget.defaultValue!).abs() < _span * 0.04) {
      v = widget.defaultValue!;
    }
    widget.onChanged?.call(v);
  }

  @override
  Widget build(BuildContext context) {
    final palette = _palette(context);
    final accent = widget.color ?? palette.accent;

    return GestureDetector(
      behavior: HitTestBehavior.opaque,
      onPanStart: (d) => _handlePanStart(d.localPosition, context.size!),
      onPanUpdate: (d) => _handlePanUpdate(d.localPosition, context.size!),
      onPanEnd: _handlePanEnd,
      child: AnimatedScale(
        scale: _dragging ? 1.05 : 1.0,
        duration: FeatherDurations.instant,
        curve: FeatherCurves.press,
        child: SizedBox(
          width: widget.diameter,
          height: widget.diameter,
          child: CustomPaint(
            painter: _CircularSliderPainter(
              progress: _norm,
              color: accent,
              trackColor:
                  widget.trackColor ?? palette.textPrimary.withValues(alpha: 0.1),
              strokeWidth: widget.strokeWidth,
              knobRadius: widget.knobRadius,
              knobColor: palette.textPrimary,
              glow: _dragging,
            ),
            child: Center(
              child: Column(
                mainAxisSize: MainAxisSize.min,
                children: [
                  Text(
                    widget.valueFormatter != null
                        ? widget.valueFormatter!(widget.value)
                        : widget.value.toStringAsFixed(0),
                    style: FeatherTypography.monoLarge.copyWith(
                      color: palette.textPrimary,
                    ),
                  ),
                  if (widget.label != null) ...[
                    const SizedBox(height: 2),
                    Text(
                      widget.label!,
                      style: FeatherTypography.micro.copyWith(
                        color: palette.textTertiary,
                      ),
                    ),
                  ],
                ],
              ),
            ),
          ),
        ),
      ),
    );
  }

  FeatherPalette _palette(BuildContext context) =>
      Theme.of(context).brightness == Brightness.dark
          ? FeatherColors.dark
          : FeatherColors.light;
}

class _CircularSliderPainter extends CustomPainter {
  _CircularSliderPainter({
    required this.progress,
    required this.color,
    required this.trackColor,
    required this.strokeWidth,
    required this.knobRadius,
    required this.knobColor,
    required this.glow,
  });

  final double progress;
  final Color color;
  final Color trackColor;
  final double strokeWidth;
  final double knobRadius;
  final Color knobColor;
  final bool glow;

  @override
  void paint(Canvas canvas, Size size) {
    final center = Offset(size.width / 2, size.height / 2);
    final radius = (size.shortestSide / 2) - strokeWidth - 4;

    // Track.
    canvas.drawCircle(
      center,
      radius,
      Paint()
        ..style = PaintingStyle.stroke
        ..strokeWidth = strokeWidth
        ..color = trackColor,
    );

    // Progress arc — from -90° clockwise.
    final rect = Rect.fromCircle(center: center, radius: radius);
    if (progress > 0) {
      final paint = Paint()
        ..style = PaintingStyle.stroke
        ..strokeWidth = strokeWidth
        ..strokeCap = StrokeCap.round
        ..color = color
        ..maskFilter = glow
            ? const MaskFilter.blur(BlurStyle.normal, 6)
            : const MaskFilter.blur(BlurStyle.normal, 0);
      canvas.drawArc(
        rect,
        -math.pi / 2,
        2 * math.pi * progress,
        false,
        paint,
      );
    }

    // Knob.
    final knobAngle = -math.pi / 2 + 2 * math.pi * progress;
    final knob = Offset(
      center.dx + radius * math.cos(knobAngle),
      center.dy + radius * math.sin(knobAngle),
    );
    canvas.drawCircle(
      knob,
      knobRadius + (glow ? 2 : 0),
      Paint()..color = knobColor,
    );
    if (glow) {
      canvas.drawCircle(
        knob,
        knobRadius + 6,
        Paint()
          ..color = color.withValues(alpha: 0.35)
          ..maskFilter = const MaskFilter.blur(BlurStyle.normal, 6),
      );
    }
  }

  @override
  bool shouldRepaint(covariant _CircularSliderPainter old) =>
      old.progress != progress ||
      old.color != color ||
      old.glow != glow ||
      old.knobColor != knobColor;
}
