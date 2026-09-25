// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// feather_colors.dart — Color palette for the Feather-Krita UI.
//
// Mirrors the Feather 3D design research: a deep slate dark mode default
// (#0F172A app background, #1E293B canvas), glassmorphism panel tints,
// and the accent colors used by each tool (Draw, Erase, Select, Mirror,
// Liquify, Stage, Light). Light-mode variants are provided so the
// Settings screen can flip the whole UI without per-widget work.

import 'package:flutter/material.dart';

/// Immutable palette container — pick `FeatherColors.dark` or
/// `FeatherColors.light` from anywhere in the app.
class FeatherPalette {
  const FeatherPalette({
    required this.brightness,
    required this.appBackground,
    required this.canvasBackground,
    required this.panelFill,
    required this.panelFillStrong,
    required this.panelBorder,
    required this.glassTint,
    required this.textPrimary,
    required this.textSecondary,
    required this.textTertiary,
    required this.textInverse,
    required this.divider,
    required this.shadow,
    required this.accent,
    required this.accentSoft,
  });

  final Brightness brightness;

  // Surface stack.
  final Color appBackground;   // window background
  final Color canvasBackground; // 3D viewport / paper canvas
  final Color panelFill;        // glass panel base tint
  final Color panelFillStrong;  // stronger panel (popover body)
  final Color panelBorder;      // 1px hairline on glass
  final Color glassTint;        // white tint overlay for frosted layer

  // Text.
  final Color textPrimary;
  final Color textSecondary;
  final Color textTertiary;
  final Color textInverse;
  final Color divider;
  final Color shadow;

  // Brand accent.
  final Color accent;
  final Color accentSoft;

  static const Color accentBlue = Color(0xFF60A5FA);
  static const Color accentPurple = Color(0xFFA78BFA);
  static const Color accentPink = Color(0xFFF472B6);
  static const Color accentOrange = Color(0xFFFB923C);
  static const Color accentGreen = Color(0xFF34D399);
  static const Color accentTeal = Color(0xFF22D3EE);
  static const Color accentYellow = Color(0xFFFACC15);

  /// Default dark palette — what the app boots into.
  static const FeatherPalette dark = FeatherPalette(
    brightness: Brightness.dark,
    appBackground: Color(0xFF0F172A), // slate-900
    canvasBackground: Color(0xFF1E293B), // slate-800
    panelFill: Color(0x14FFFFFF), // 8% white
    panelFillStrong: Color(0x29FFFFFF), // 16% white
    panelBorder: Color(0x1AFFFFFF), // 10% white
    glassTint: Color(0x0AFFFFFF),
    textPrimary: Color(0xFFF8FAFC),
    textSecondary: Color(0xFFCBD5E1),
    textTertiary: Color(0x99CBD5E1),
    textInverse: Color(0xFF0F172A),
    divider: Color(0x22FFFFFF),
    shadow: Color(0x66000000),
    accent: accentBlue,
    accentSoft: Color(0x3360A5FA),
  );

  /// Light palette — opt-in from Settings.
  static const FeatherPalette light = FeatherPalette(
    brightness: Brightness.light,
    appBackground: Color(0xFFF1F5F9), // slate-100
    canvasBackground: Color(0xFFFFFFFF),
    panelFill: Color(0x66FFFFFF),
    panelFillStrong: Color(0xB3FFFFFF),
    panelBorder: Color(0x33FFFFFF),
    glassTint: Color(0xFFFFFFFF),
    textPrimary: Color(0xFF0F172A),
    textSecondary: Color(0xFF334155),
    textTertiary: Color(0x99334857),
    textInverse: Color(0xFFF8FAFC),
    divider: Color(0x22000000),
    shadow: Color(0x33000000),
    accent: Color(0xFF2563EB),
    accentSoft: Color(0x332563EB),
  );

  FeatherPalette copyWith({Color? accent, Color? accentSoft}) =>
      FeatherPalette(
        brightness: brightness,
        appBackground: appBackground,
        canvasBackground: canvasBackground,
        panelFill: panelFill,
        panelFillStrong: panelFillStrong,
        panelBorder: panelBorder,
        glassTint: glassTint,
        textPrimary: textPrimary,
        textSecondary: textSecondary,
        textTertiary: textTertiary,
        textInverse: textInverse,
        divider: divider,
        shadow: shadow,
        accent: accent ?? this.accent,
        accentSoft: accentSoft ?? this.accentSoft,
      );

  static const FeatherPalette fallback = dark;
}

/// Backwards-compatible accessor used throughout the UI package.
class FeatherColors {
  const FeatherColors._();

  static const FeatherPalette dark = FeatherPalette.dark;
  static const FeatherPalette light = FeatherPalette.light;

  // Convenience — used by tool badges, joystick gizmos, etc.
  static const Color toolDraw = FeatherPalette.accentBlue;
  static const Color toolErase = FeatherPalette.accentPink;
  static const Color toolSelect = FeatherPalette.accentTeal;
  static const Color toolMirror = FeatherPalette.accentPurple;
  static const Color toolLiquify = FeatherPalette.accentOrange;
  static const Color toolStage = FeatherPalette.accentGreen;
  static const Color toolLight = FeatherPalette.accentYellow;
}
