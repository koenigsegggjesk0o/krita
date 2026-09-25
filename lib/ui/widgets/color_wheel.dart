// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// color_wheel.dart — HSV color wheel + saturation/value square.
//
// Matches Feather 3D's color panel:
//   * outer ring = hue (0–360°), drag to spin,
//   * inner square = saturation (X) × value (Y), drag to adjust,
//   * hex code below,
//   * eyedropper toggle.
//
// HSV conversion via Flutter's built-in [HSVColor] — no manual math
// errors possible. The wheel + square is drawn by one CustomPainter so
// the colors stay consistent across the two regions.

import 'dart:math' as math;

import 'package:flutter/material.dart';

import '../theme/feather_colors.dart';
import '../theme/feather_typography.dart';
import '../theme/glassmorphism.dart';
import 'glass_panel.dart';
import 'icon_button.dart';

class ColorWheel extends StatefulWidget {
  const ColorWheel({
    super.key,
    required this.color,
    required this.onChanged,
    this.onEyeDropper,
    this.eyeDropperActive = false,
    this.diameter = 240.0,
  });

  final Color color;
  final ValueChanged<Color> onChanged;
  final VoidCallback? onEyeDropper;
  final bool eyeDropperActive;
  final double diameter;

  @override
  State<ColorWheel> createState() => _ColorWheelState();
}

class _ColorWheelState extends State<ColorWheel> {
  _DragRegion? _region;

  HSVColor get _hsv => HSVColor.fromColor(widget.color);

  @override
  Widget build(BuildContext context) {
    final palette = _palette(context);
    final wheelSize = widget.diameter;
    // Inner SV square: inscribed in the hue ring's hole.
    final ringWidth = wheelSize * 0.14;
    final innerDiameter = wheelSize - ringWidth * 2 - 8;
    final squareSide = innerDiameter / math.sqrt2;

    return GlassPanel(
      spec: GlassSpec.strong,
      width: wheelSize + 32,
      padding: const EdgeInsets.fromLTRB(16, 16, 16, 12),
      child: Column(
        mainAxisSize: MainAxisSize.min,
        children: [
          Text(
            'COLOR',
            style: FeatherTypography.micro
                .copyWith(color: palette.textTertiary, letterSpacing: 1.2),
          ),
          const SizedBox(height: 10),
          SizedBox(
            width: wheelSize,
            height: wheelSize,
            child: GestureDetector(
              behavior: HitTestBehavior.opaque,
              onPanStart: (d) => _handle(d.localPosition, wheelSize,
                  ringWidth, squareSide, start: true),
              onPanUpdate: (d) => _handle(d.localPosition, wheelSize,
                  ringWidth, squareSide),
              onPanEnd: (_) => setState(() => _region = null),
              child: CustomPaint(
                size: Size.square(wheelSize),
                painter: _ColorWheelPainter(
                  hsv: _hsv,
                  region: _region,
                  accent: palette.accent,
                  ringWidth: ringWidth,
                  squareSide: squareSide,
                ),
              ),
            ),
          ),
          const SizedBox(height: 12),
          _HexRow(
            color: widget.color,
            palette: palette,
            onChanged: (c) => widget.onChanged(c),
          ),
          const SizedBox(height: 8),
          Row(
            mainAxisAlignment: MainAxisAlignment.spaceEvenly,
            children: [
              FeatherIconButton(
                icon: Icons.colorize_rounded,
                tooltip: 'Eyedropper',
                isActive: widget.eyeDropperActive,
                activeColor: palette.textPrimary,
                size: 40,
                iconSize: 18,
                onTap: widget.onEyeDropper,
              ),
              FeatherIconButton(
                icon: Icons.refresh_rounded,
                tooltip: 'Reset to white',
                size: 40,
                iconSize: 18,
                onTap: () => widget.onChanged(Colors.white),
              ),
            ],
          ),
        ],
      ),
    );
  }

  void _handle(
    Offset local,
    double wheelSize,
    double ringWidth,
    double squareSide, {
    bool start = false,
  }) {
    final center = Offset(wheelSize / 2, wheelSize / 2);
    final dx = local.dx - center.dx;
    final dy = local.dy - center.dy;
    final dist = math.sqrt(dx * dx + dy * dy);
    final outerR = wheelSize / 2;
    final innerR = outerR - ringWidth;

    HSVColor next;
    if (dist > innerR && dist <= outerR + 6) {
      // Hue ring.
      var angle = math.atan2(dy, dx);
      if (angle < 0) angle += 2 * math.pi;
      final hue = (angle * 180 / math.pi) % 360;
      next = _hsv.withHue(hue);
      if (start) setState(() => _region = _DragRegion.hue);
    } else {
      // SV square (centered).
      final half = squareSide / 2;
      final sx = (dx + half).clamp(0.0, squareSide) / squareSide;
      final sy = (dy + half).clamp(0.0, squareSide) / squareSide;
      // Top-left = S=0,V=1 (white). Bottom-right = S=1,V=0 (black).
      next = _hsv.withSaturation(sx).withValue(1 - sy);
      if (start) setState(() => _region = _DragRegion.square);
    }
    widget.onChanged(next.toColor());
  }

  FeatherPalette _palette(BuildContext context) =>
      Theme.of(context).brightness == Brightness.dark
          ? FeatherColors.dark
          : FeatherColors.light;
}

enum _DragRegion { hue, square }

class _ColorWheelPainter extends CustomPainter {
  _ColorWheelPainter({
    required this.hsv,
    required this.region,
    required this.accent,
    required this.ringWidth,
    required this.squareSide,
  });

  final HSVColor hsv;
  final _DragRegion? region;
  final Color accent;
  final double ringWidth;
  final double squareSide;

  @override
  void paint(Canvas canvas, Size size) {
    final center = Offset(size.width / 2, size.height / 2);
    final outerR = size.width / 2;
    final innerR = outerR - ringWidth;

    // Hue ring as 360 slices.
    const steps = 360;
    for (int i = 0; i < steps; i++) {
      final a0 = (i / steps) * 2 * math.pi - math.pi / 2;
      final a1 = ((i + 1) / steps) * 2 * math.pi - math.pi / 2;
      final path = Path()
        ..moveTo(center.dx + outerR * math.cos(a0),
            center.dy + outerR * math.sin(a0))
        ..arcTo(Rect.fromCircle(center: center, radius: outerR), a0,
            a1 - a0, false)
        ..lineTo(center.dx + innerR * math.cos(a1),
            center.dy + innerR * math.sin(a1))
        ..arcTo(Rect.fromCircle(center: center, radius: innerR), a1,
            a0 - a1, false)
        ..close();
      canvas.drawPath(
        path,
        Paint()..color = HSVColor.fromAHSV(1, (i / steps) * 360, 1, 1).toColor(),
      );
    }
    // Ring border.
    canvas.drawCircle(
      center,
      (outerR + innerR) / 2,
      Paint()
        ..style = PaintingStyle.stroke
        ..strokeWidth = ringWidth + 2
        ..color = const Color(0x10FFFFFF),
    );

    // Hue indicator.
    final hueAngle = (hsv.hue / 360) * 2 * math.pi - math.pi / 2;
    final huePos = Offset(
      center.dx + (outerR - ringWidth / 2) * math.cos(hueAngle),
      center.dy + (outerR - ringWidth / 2) * math.sin(hueAngle),
    );
    canvas.drawCircle(
        huePos, 9, Paint()..color = const Color(0xFFFFFFFF));
    canvas.drawCircle(huePos, 7,
        Paint()..color = HSVColor.fromAHSV(1, hsv.hue, 1, 1).toColor());

    // SV square — axis-aligned and inscribed.
    final rect = Rect.fromCenter(center: center, width: squareSide, height: squareSide);

    // Paint SV gradient using ImageFilter-less layering: vertical value,
    // horizontal saturation, then white base.
    canvas.save();
    canvas.clipRect(rect);
    canvas.drawRect(
      rect,
      Paint()..color = Colors.white,
    );

    // Saturation gradient (white → pure hue) horizontally.
    final satPaint = Paint()
      ..shader = LinearGradient(
        begin: Alignment.centerLeft,
        end: Alignment.centerRight,
        colors: [
          const Color(0xFFFFFFFF),
          HSVColor.fromAHSV(1, hsv.hue, 1, 1).toColor(),
        ],
      ).createShader(rect);
    canvas.drawRect(rect, satPaint);

    // Value gradient (transparent → black) vertically.
    final valPaint = Paint()
      ..shader = LinearGradient(
        begin: Alignment.topCenter,
        end: Alignment.bottomCenter,
        colors: [
          const Color(0x00FFFFFF),
          const Color(0xFF000000),
        ],
      ).createShader(rect);
    canvas.drawRect(rect, valPaint);
    canvas.restore();

    // Square border.
    canvas.drawRect(
      rect,
      Paint()
        ..style = PaintingStyle.stroke
        ..strokeWidth = 1
        ..color = const Color(0x33FFFFFF),
    );

    // SV indicator.
    final sx = rect.left + hsv.saturation * squareSide;
    final sy = rect.top + (1 - hsv.value) * squareSide;
    canvas.drawCircle(
      Offset(sx, sy),
      8,
      Paint()
        ..style = PaintingStyle.stroke
        ..strokeWidth = 2
        ..color = const Color(0xFFFFFFFF),
    );
    canvas.drawCircle(
      Offset(sx, sy),
      8,
      Paint()
        ..style = PaintingStyle.stroke
        ..strokeWidth = 1
        ..color = const Color(0x66000000),
    );
  }

  @override
  bool shouldRepaint(covariant _ColorWheelPainter old) =>
      old.hsv != hsv || old.region != region;
}

class _HexRow extends StatelessWidget {
  const _HexRow({
    required this.color,
    required this.palette,
    required this.onChanged,
  });

  final Color color;
  final FeatherPalette palette;
  final ValueChanged<Color> onChanged;

  @override
  Widget build(BuildContext context) {
    final hex = _toHex(color);
    return Row(
      children: [
        Container(
          width: 28,
          height: 28,
          decoration: BoxDecoration(
            color: color,
            borderRadius: BorderRadius.circular(8),
            border: Border.all(color: palette.panelBorder),
          ),
        ),
        const SizedBox(width: 10),
        Expanded(
          child: Text(
            '#$hex',
            style: FeatherTypography.monoLarge.copyWith(color: palette.textPrimary),
          ),
        ),
        const SizedBox(width: 6),
        Text(
          'tap to edit',
          style: FeatherTypography.micro.copyWith(color: palette.textTertiary),
        ),
      ],
    );
  }

  String _toHex(Color c) {
    final r = (c.r * 255).round().toRadixString(16).padLeft(2, '0').toUpperCase();
    final g = (c.g * 255).round().toRadixString(16).padLeft(2, '0').toUpperCase();
    final b = (c.b * 255).round().toRadixString(16).padLeft(2, '0').toUpperCase();
    return '$r$g$b';
  }
}
