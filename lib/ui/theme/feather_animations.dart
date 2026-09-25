// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// feather_animations.dart — Animation curves and durations.
//
// Feather 3D uses spring-y motion for popovers (elasticOut, ~300 ms) and
// gentler easeOutCubic for inline toggles (150–220 ms). Everything here
// is data — no widget code — so it's safe to import from anywhere.

import 'package:flutter/animation.dart';

class FeatherDurations {
  const FeatherDurations._();

  static const Duration instant = Duration(milliseconds: 80);
  static const Duration quick = Duration(milliseconds: 150);
  static const Duration medium = Duration(milliseconds: 220);
  static const Duration slow = Duration(milliseconds: 300);
  static const Duration page = Duration(milliseconds: 420);
}

class FeatherCurves {
  const FeatherCurves._();

  /// Tap feedback — buttons, icon buttons.
  static const Curve press = Curves.easeOutCubic;

  /// Inline toggles — pressure sensitivity, mirror, dark mode.
  static const Curve toggle = Curves.easeOutCubic;

  /// Sliders / joystick springs back.
  static const Curve returnSpring = Curves.easeOutBack;

  /// Popovers (color wheel, material picker, radial menu, numpad).
  /// Custom elastic-out for the springy "pop" Feather 3D uses.
  static const Curve popover = _ElasticOutCurve(0.6);

  /// Panel slide-ins.
  static const Curve slideIn = Curves.easeOutCubic;

  /// Page transitions.
  static const Curve page = Curves.easeOutCubic;

  /// Spring-based decay for the joystick handle when released.
  static const Curve joystickReturn = Curves.elasticOut;
}

/// Standard elastic-out — matches CSS's `cubic-bezier(0.16, 1, 0.3, 1)`
/// close enough for Feather's popovers, with a touch of overshoot.
class _ElasticOutCurve extends Curve {
  const _ElasticOutCurve(this.tension);

  final double tension;

  @override
  double transformInternal(double t) {
    // A simplified elastic-out: easeOutCubic with mild overshoot.
    final eased = 1 - (1 - t).pow3();
    final overshoot = (1 - t) * (1 - t) * 0.08 * tension;
    return (eased - overshoot).clamp(0.0, 1.05);
  }
}

extension on num {
  double pow3() => toDouble() * toDouble() * toDouble();
}

/// Pre-built tween helpers for the most common transitions.
class FeatherTweens {
  const FeatherTweens._();

  /// Popover scale: 0.88 → 1.0 with a hair of overshoot baked by the curve.
  static Tween<double> get popoverScale => Tween<double>(begin: 0.88, end: 1.0);

  /// Popover opacity: 0 → 1.
  static Tween<double> get popoverOpacity =>
      Tween<double>(begin: 0.0, end: 1.0);

  /// Panel slide from the side (positive offset = off-screen).
  static Tween<Offset> slideFromLeft(double distance) =>
      Tween<Offset>(
        begin: Offset(-distance, 0),
        end: Offset.zero,
      );

  static Tween<Offset> slideFromRight(double distance) =>
      Tween<Offset>(
        begin: Offset(distance, 0),
        end: Offset.zero,
      );

  static Tween<Offset> slideFromBottom(double distance) =>
      Tween<Offset>(
        begin: Offset(0, distance),
        end: Offset.zero,
      );
}
