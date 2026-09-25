// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// glass_panel.dart — Reusable glassmorphism panel widget.
//
// One widget to rule them all: any floating surface in Feather-Krita
// (top bar, bottom pill, brush panel, stage panel, color popover, …)
// is a GlassPanel with a different [GlassSpec] and a child. It composes:
//   * an outer shadow painted by [GlassShadowPainter],
//   * a [BackdropFilter] providing the real blur,
//   * the [GlassPainter] painting tint + hairline border,
//   * an optional padding so callers don't repeat it everywhere.
//
// This widget must be placed over content that can actually be blurred —
// over a flat-color area the blur is a no-op (Flutter limitation).

import 'package:flutter/material.dart';
import 'package:flutter/widgets.dart';

import '../theme/glassmorphism.dart';

class GlassPanel extends StatelessWidget {
  const GlassPanel({
    super.key,
    required this.child,
    this.spec = GlassSpec.panel,
    this.padding = const EdgeInsets.all(12),
    this.margin,
    this.alignment,
    this.width,
    this.height,
    this.shape = BoxShape.rectangle,
  });

  final Widget child;
  final GlassSpec spec;
  final EdgeInsets padding;
  final EdgeInsets? margin;
  final AlignmentGeometry? alignment;
  final double? width;
  final double? height;
  final BoxShape shape;

  @override
  Widget build(BuildContext context) {
    final useCircle = shape == BoxShape.circle;
    final decor = BoxDecoration(
      color: spec.tint,
      borderRadius: useCircle ? null : spec.borderRadius,
      shape: shape,
      border: Border.all(color: spec.border, width: spec.borderWidth),
      boxShadow: [
        BoxShadow(
          color: spec.shadow,
          blurRadius: spec.shadowBlur,
          offset: spec.shadowOffset,
        ),
      ],
    );

    return Container(
      width: width,
      height: height,
      margin: margin,
      decoration: decor,
      child: CustomPaint(
        painter: _GlassBlurWrapper(spec: spec, circular: useCircle),
        child: ClipRRect(
          borderRadius:
              useCircle ? BorderRadius.circular(9999) : spec.borderRadius,
          child: BackdropFilter(
            filter: spec.imageFilter,
            child: Container(
              alignment: alignment,
              padding: padding,
              decoration: BoxDecoration(
                color: spec.tint,
                borderRadius:
                    useCircle ? null : spec.borderRadius,
                shape: shape,
              ),
              child: child,
            ),
          ),
        ),
      ),
    );
  }
}

/// Tiny shim — we keep the hairline highlight on top of the BackdropFilter
/// so it survives the clip. This is purely visual polish.
class _GlassBlurWrapper extends CustomPainter {
  _GlassBlurWrapper({required this.spec, required this.circular});

  final GlassSpec spec;
  final bool circular;

  @override
  void paint(Canvas canvas, Size size) {
    final rect = Offset.zero & size;
    final rrect = circular
        ? RRect.fromRectAndRadius(rect, Radius.circular(rect.width / 2))
        : RRect.fromRectAndRadius(rect, Radius.circular(spec.radius));
    canvas.drawRRect(
      rrect,
      Paint()
        ..shader = LinearGradient(
          begin: Alignment.topCenter,
          end: Alignment.bottomCenter,
          colors: [
            const Color(0x22FFFFFF),
            const Color(0x00FFFFFF),
          ],
        ).createShader(rect),
    );
  }

  @override
  bool shouldRepaint(covariant _GlassBlurWrapper old) =>
      old.spec != spec || old.circular != circular;
}
