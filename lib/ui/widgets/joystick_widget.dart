// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// joystick_widget.dart — 2D / 3D joystick gizmo.
//
// Per transform_2djoystick.txt + transform_3djoystick.txt:
//   * center "stick" — drag to move,
//   * scale handles — top (height) + left (width) + corner (free),
//   * rotate handle — right, spin to rotate,
//   * lock toggle — restricts to 4-direction / 15°-increments,
//   * 2D ⇄ 3D switch.
//
// In 3D mode the joystick shows three axes (X red, Y green, Z blue) as
// small arrows around the stick; the stick still does free-trackball.
//
// The widget emits raw transforms via the callbacks — the parent editor
// decides how to apply them to the scene.

import 'dart:math' as math;

import 'package:flutter/material.dart';

import '../theme/feather_animations.dart';
import '../theme/feather_colors.dart';
import '../theme/feather_typography.dart';

enum JoystickMode { move, rotate, scale }

class JoystickWidget extends StatefulWidget {
  const JoystickWidget({
    super.key,
    this.mode = JoystickMode.move,
    this.locked = false,
    this.is3D = false,
    this.size = 220.0,
    this.onMove,
    this.onRotate,
    this.onScale,
    this.onLockToggle,
    this.onToggle3D,
  });

  final JoystickMode mode;
  final bool locked;
  final bool is3D;
  final double size;
  final ValueChanged<Offset>? onMove;
  final ValueChanged<double>? onRotate; // radians
  final ValueChanged<Offset>? onScale; // (w, h) deltas
  final VoidCallback? onLockToggle;
  final VoidCallback? onToggle3D;

  @override
  State<JoystickWidget> createState() => _JoystickWidgetState();
}

class _JoystickWidgetState extends State<JoystickWidget>
    with SingleTickerProviderStateMixin {
  Offset _stick = Offset.zero;

  late final AnimationController _return;
  late Animation<Offset> _returnAnim;
  Offset _returnFrom = Offset.zero;

  @override
  void initState() {
    super.initState();
    _return = AnimationController(
      vsync: this,
      duration: FeatherDurations.slow,
    );
    _returnAnim = AlwaysStoppedAnimation<Offset>(Offset.zero);
    _return.addListener(() {
      setState(() => _stick = _returnAnim.value);
    });
  }

  @override
  void dispose() {
    _return.dispose();
    super.dispose();
  }

  void _snapBack() {
    _returnFrom = _stick;
    _returnAnim = Tween<Offset>(begin: _returnFrom, end: Offset.zero).animate(
      CurvedAnimation(parent: _return, curve: FeatherCurves.joystickReturn),
    )..addStatusListener((s) {
        if (s == AnimationStatus.completed) _stick = Offset.zero;
      });
    _return.forward(from: 0);
  }

  @override
  Widget build(BuildContext context) {
    final palette = _palette(context);
    final half = widget.size / 2;
    return SizedBox(
      width: widget.size + 60,
      height: widget.size + 60,
      child: Stack(
        alignment: Alignment.center,
        children: [
          // Background ring + axis arrows (3D mode).
          CustomPaint(
            size: Size.square(widget.size + 40),
            painter: _JoystickPainter(
              mode: widget.mode,
              locked: widget.locked,
              is3D: widget.is3D,
              stick: _stick,
              radius: half,
              palette: palette,
            ),
          ),
          // Stick — drag handle.
          Positioned(
            left: half + 20 + _stick.dx - 18,
            top: half + 20 + _stick.dy - 18,
            child: GestureDetector(
              onPanUpdate: (d) {
                var next = _stick + d.delta;
                final mag = next.distance;
                final maxR = half - 16;
                if (mag > maxR) {
                  next = next * (maxR / mag);
                }
                if (widget.locked) {
                  // Snap to 4 directions.
                  if (next.dx.abs() > next.dy.abs()) {
                    next = Offset(next.dx, 0);
                  } else {
                    next = Offset(0, next.dy);
                  }
                }
                setState(() => _stick = next);
                widget.onMove?.call(next / maxR);
              },
              onPanEnd: (_) => _snapBack(),
              child: Container(
                width: 36,
                height: 36,
                decoration: BoxDecoration(
                  shape: BoxShape.circle,
                  color: palette.textPrimary,
                  boxShadow: [
                    BoxShadow(
                      color: palette.shadow,
                      blurRadius: 8,
                      offset: const Offset(0, 2),
                    ),
                  ],
                ),
                child: Center(
                  child: Container(
                    width: 8,
                    height: 8,
                    decoration: BoxDecoration(
                      shape: BoxShape.circle,
                      color: palette.accent,
                    ),
                  ),
                ),
              ),
            ),
          ),
          // Scale handle (top).
          if (widget.mode == JoystickMode.scale)
            Positioned(
              top: 4,
              child: _Handle(
                icon: Icons.arrow_drop_down_rounded,
                palette: palette,
                onPan: (d) {
                  widget.onScale?.call(Offset(d.dx * 0.01, -d.dy * 0.01));
                },
              ),
            ),
          // Scale handle (left).
          if (widget.mode == JoystickMode.scale)
            Positioned(
              left: 4,
              child: _Handle(
                icon: Icons.arrow_right_rounded,
                palette: palette,
                onPan: (d) {
                  widget.onScale?.call(Offset(-d.dx * 0.01, d.dy * 0.01));
                },
              ),
            ),
          // Rotate handle (right).
          if (widget.mode == JoystickMode.rotate)
            Positioned(
              right: 4,
              child: _Handle(
                icon: Icons.refresh_rounded,
                palette: palette,
                onPan: (d) {
                  widget.onRotate?.call(d.dx * 0.01);
                },
              ),
            ),
          // Mode + lock + 2D/3D controls.
          Positioned(
            bottom: 0,
            child: Row(
              mainAxisSize: MainAxisSize.min,
              children: [
                _Pill(
                  palette: palette,
                  active: widget.mode == JoystickMode.move,
                  icon: Icons.open_with_rounded,
                  label: 'Move',
                  onTap: () => setState(() {}),
                ),
                const SizedBox(width: 4),
                _Pill(
                  palette: palette,
                  active: widget.mode == JoystickMode.rotate,
                  icon: Icons.refresh_rounded,
                  label: 'Rotate',
                  onTap: () => setState(() {}),
                ),
                const SizedBox(width: 4),
                _Pill(
                  palette: palette,
                  active: widget.mode == JoystickMode.scale,
                  icon: Icons.aspect_ratio_rounded,
                  label: 'Scale',
                  onTap: () => setState(() {}),
                ),
                const SizedBox(width: 8),
                _Pill(
                  palette: palette,
                  active: widget.locked,
                  icon: Icons.lock_outline,
                  label: 'Lock',
                  onTap: widget.onLockToggle,
                ),
                const SizedBox(width: 4),
                _Pill(
                  palette: palette,
                  active: widget.is3D,
                  icon: Icons.threed_rotation_outlined,
                  label: widget.is3D ? '3D' : '2D',
                  onTap: widget.onToggle3D,
                ),
              ],
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

class _Handle extends StatelessWidget {
  const _Handle({
    required this.icon,
    required this.palette,
    required this.onPan,
  });

  final IconData icon;
  final FeatherPalette palette;
  final void Function(Offset delta) onPan;

  @override
  Widget build(BuildContext context) {
    return GestureDetector(
      onPanUpdate: (d) => onPan(d.delta),
      child: Container(
        width: 28,
        height: 28,
        decoration: BoxDecoration(
          shape: BoxShape.circle,
          color: palette.accent,
          boxShadow: [
            BoxShadow(
              color: palette.accent.withValues(alpha: 0.6),
              blurRadius: 8,
            ),
          ],
        ),
        child: Icon(icon, color: palette.textInverse, size: 18),
      ),
    );
  }
}

class _Pill extends StatelessWidget {
  const _Pill({
    required this.palette,
    required this.active,
    required this.icon,
    required this.label,
    required this.onTap,
  });

  final FeatherPalette palette;
  final bool active;
  final IconData icon;
  final String label;
  final VoidCallback? onTap;

  @override
  Widget build(BuildContext context) {
    return GestureDetector(
      onTap: onTap,
      child: AnimatedContainer(
        duration: FeatherDurations.quick,
        curve: FeatherCurves.toggle,
        padding: const EdgeInsets.symmetric(horizontal: 8, vertical: 5),
        decoration: BoxDecoration(
          color: active
              ? palette.accent.withValues(alpha: 0.22)
              : palette.panelFill,
          borderRadius: BorderRadius.circular(8),
          border: Border.all(
            color: active ? palette.accent : palette.panelBorder,
          ),
        ),
        child: Row(
          mainAxisSize: MainAxisSize.min,
          children: [
            Icon(icon,
                size: 12,
                color: active ? palette.accent : palette.textSecondary),
            const SizedBox(width: 4),
            Text(
              label,
              style: FeatherTypography.micro.copyWith(
                color:
                    active ? palette.textPrimary : palette.textTertiary,
                fontWeight: FontWeight.w700,
              ),
            ),
          ],
        ),
      ),
    );
  }
}

class _JoystickPainter extends CustomPainter {
  _JoystickPainter({
    required this.mode,
    required this.locked,
    required this.is3D,
    required this.stick,
    required this.radius,
    required this.palette,
  });

  final JoystickMode mode;
  final bool locked;
  final bool is3D;
  final Offset stick;
  final double radius;
  final FeatherPalette palette;

  @override
  void paint(Canvas canvas, Size size) {
    final center = Offset(size.width / 2, size.height / 2);

    // Outer track.
    canvas.drawCircle(
      center,
      radius,
      Paint()
        ..style = PaintingStyle.stroke
        ..strokeWidth = 1.5
        ..color = palette.textPrimary.withValues(alpha: 0.18),
    );

    // Inner crosshair guides (4 directions).
    final guide = Paint()
      ..style = PaintingStyle.stroke
      ..strokeWidth = 0.5
      ..color = palette.textPrimary.withValues(alpha: 0.1);
    canvas.drawLine(
        Offset(center.dx - radius, center.dy),
        Offset(center.dx + radius, center.dy),
        guide);
    canvas.drawLine(
        Offset(center.dx, center.dy - radius),
        Offset(center.dx, center.dy + radius),
        guide);

    // 3D axis arrows.
    if (is3D) {
      _axisArrow(canvas, center, 0, radius - 6, FeatherColors.toolDraw, 'X');
      _axisArrow(canvas, center, math.pi / 2, radius - 6,
          FeatherColors.toolSelect, 'Y');
      _axisArrow(
          canvas, center, -math.pi / 4, radius - 6, FeatherColors.toolStage, 'Z');
    }

    // Lock indicator.
    if (locked) {
      canvas.drawCircle(
        center,
        radius - 4,
        Paint()
          ..style = PaintingStyle.stroke
          ..strokeWidth = 1
          ..color = palette.accent.withValues(alpha: 0.4),
      );
    }

    // Stick trail (line from center to stick).
    if (stick != Offset.zero) {
      canvas.drawLine(
        center,
        center + stick,
        Paint()
          ..color = palette.accent
          ..strokeWidth = 2
          ..strokeCap = StrokeCap.round,
      );
    }
  }

  void _axisArrow(Canvas canvas, Offset center, double angle, double r,
      Color color, String label) {
    final tip = Offset(
      center.dx + r * math.cos(angle),
      center.dy + r * math.sin(angle),
    );
    canvas.drawLine(
      center,
      tip,
      Paint()
        ..color = color
        ..strokeWidth = 1.5
        ..strokeCap = StrokeCap.round,
    );
    canvas.drawCircle(tip, 4, Paint()..color = color);
  }

  @override
  bool shouldRepaint(covariant _JoystickPainter old) =>
      old.stick != stick ||
      old.mode != mode ||
      old.locked != locked ||
      old.is3D != is3D;
}
