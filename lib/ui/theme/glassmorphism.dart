// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// glassmorphism.dart — Helper for true glassmorphism panels.
//
// The "glass" effect in Feather 3D is a frosted layer:
//   1. a real BackdropFilter with an ImageFilter.blur (sigma 16–24),
//   2. a translucent white tint (8–20% opacity) painted *over* the blur,
//   3. a 1px hairline border at ~10% white,
//   4. a soft outer shadow.
//
// This file gives you [GlassSpec] (a value object describing the look) and
// [GlassPainter] (a CustomPainter that paints the tint + border + shadow)
// so [GlassPanel] in widgets/glass_panel.dart can compose a real
// BackdropFilter without duplicating this paint logic.

import 'dart:ui' as ui;

import 'package:flutter/material.dart';

/// All knobs that describe one glass layer.
class GlassSpec {
  const GlassSpec({
    this.blurSigma = 20.0,
    this.tint = const Color(0x14FFFFFF),
    this.border = const Color(0x1AFFFFFF),
    this.borderWidth = 1.0,
    this.shadow = const Color(0x66000000),
    this.shadowBlur = 24.0,
    this.shadowOffset = const Offset(0, 8),
    this.radius = 18.0,
  });

  /// Subtle — for floating chips and pills (bottom bar, top bar pills).
  static const GlassSpec subtle = GlassSpec(
    blurSigma: 16.0,
    tint: Color(0x0DFFFFFF),
    radius: 14.0,
    shadowBlur: 16.0,
  );

  /// Default — for the main panels (brush, stage, right panel).
  static const GlassSpec panel = GlassSpec();

  /// Strong — for popovers that need to feel "lifted" (color wheel,
  /// material picker, numpad).
  static const GlassSpec strong = GlassSpec(
    blurSigma: 24.0,
    tint: Color(0x29FFFFFF),
    radius: 22.0,
    shadowBlur: 32.0,
  );

  final double blurSigma;
  final Color tint;
  final Color border;
  final double borderWidth;
  final Color shadow;
  final double shadowBlur;
  final Offset shadowOffset;
  final double radius;

  ui.ImageFilter get imageFilter =>
      ui.ImageFilter.blur(sigmaX: blurSigma, sigmaY: blurSigma);

  BorderRadius get borderRadius => BorderRadius.circular(radius);

  GlassSpec copyWith({
    double? blurSigma,
    Color? tint,
    Color? border,
    double? borderWidth,
    Color? shadow,
    double? shadowBlur,
    Offset? shadowOffset,
    double? radius,
  }) =>
      GlassSpec(
        blurSigma: blurSigma ?? this.blurSigma,
        tint: tint ?? this.tint,
        border: border ?? this.border,
        borderWidth: borderWidth ?? this.borderWidth,
        shadow: shadow ?? this.shadow,
        shadowBlur: shadowBlur ?? this.shadowBlur,
        shadowOffset: shadowOffset ?? this.shadowOffset,
        radius: radius ?? this.radius,
      );
}

/// Paints the tint + hairline border + outer shadow. The blur itself
/// is provided by BackdropFilter, not by this painter.
class GlassPainter extends CustomPainter {
  const GlassPainter(this.spec);

  final GlassSpec spec;

  @override
  void paint(Canvas canvas, Size size) {
    final rect = Offset.zero & size;
    final rrect = RRect.fromRectAndRadius(
      rect.deflate(spec.borderWidth / 2),
      Radius.circular(spec.radius),
    );

    // Tint.
    final tintPaint = Paint()..color = spec.tint;
    canvas.drawRRect(rrect, tintPaint);

    // Inner highlight — top 1px lighter band, gives the "glass edge".
    final highlight = Paint()
      ..shader = ui.Gradient.linear(
        Offset(rect.left, rect.top),
        Offset(rect.left, rect.bottom),
        [const Color(0x22FFFFFF), const Color(0x00FFFFFF)],
      );
    canvas.drawRRect(rrect, highlight);

    // Hairline border.
    canvas.drawRRect(
      rrect,
      Paint()
        ..style = PaintingStyle.stroke
        ..strokeWidth = spec.borderWidth
        ..color = spec.border,
    );
  }

  @override
  bool shouldRepaint(covariant GlassPainter old) => old.spec != spec;
}

/// Drop shadow painted *behind* the glass layer — kept separate so the
/// BackdropFilter doesn't capture it.
class GlassShadowPainter extends CustomPainter {
  const GlassShadowPainter(this.spec);

  final GlassSpec spec;

  @override
  void paint(Canvas canvas, Size size) {
    final rect = Offset.zero & size;
    final rrect = RRect.fromRectAndRadius(rect, Radius.circular(spec.radius));
    canvas.drawRRect(
      rrect.shift(spec.shadowOffset),
      Paint()
        ..color = spec.shadow
        ..maskFilter = MaskFilter.blur(BlurStyle.normal, spec.shadowBlur),
    );
  }

  @override
  bool shouldRepaint(covariant GlassShadowPainter old) =>
      old.spec != spec;
}
