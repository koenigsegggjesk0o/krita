// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// feather_typography.dart — Typography stack for Feather-Krita.
//
// Three families, each with a clear role:
//   * Inter — UI chrome (labels, buttons, panel headers).
//   * Caveat — handwritten captions used by the tutorial overlay.
//   * JetBrains Mono — numeric / hex / data readouts (sliders, hex code,
//                       brush size).
//
// Fonts are referenced by family name. If a font is not bundled, Flutter
// falls back gracefully to the platform default — the typography stays
// correct because we only depend on the TextStyle weights and sizes.

import 'package:flutter/material.dart';

class FeatherTypography {
  const FeatherTypography._();

  // Family names — declared in pubspec later; gracefully fall back if
  // the asset is missing.
  static const String familyUI = 'Inter';
  static const String familyCaption = 'Caveat';
  static const String familyMono = 'JetBrainsMono';

  // ---- UI --------------------------------------------------------------
  static const TextStyle display = TextStyle(
    fontFamily: familyUI,
    fontSize: 28,
    fontWeight: FontWeight.w700,
    height: 1.15,
    letterSpacing: -0.5,
  );

  static const TextStyle h1 = TextStyle(
    fontFamily: familyUI,
    fontSize: 22,
    fontWeight: FontWeight.w700,
    height: 1.2,
    letterSpacing: -0.3,
  );

  static const TextStyle h2 = TextStyle(
    fontFamily: familyUI,
    fontSize: 17,
    fontWeight: FontWeight.w600,
    height: 1.25,
  );

  static const TextStyle body = TextStyle(
    fontFamily: familyUI,
    fontSize: 14,
    fontWeight: FontWeight.w400,
    height: 1.4,
  );

  static const TextStyle bodyStrong = TextStyle(
    fontFamily: familyUI,
    fontSize: 14,
    fontWeight: FontWeight.w600,
    height: 1.4,
  );

  static const TextStyle caption = TextStyle(
    fontFamily: familyUI,
    fontSize: 11,
    fontWeight: FontWeight.w500,
    height: 1.3,
    letterSpacing: 0.1,
  );

  static const TextStyle micro = TextStyle(
    fontFamily: familyUI,
    fontSize: 9,
    fontWeight: FontWeight.w600,
    height: 1.2,
    letterSpacing: 0.6,
  );

  // ---- Handwritten tutorial caption -----------------------------------
  static const TextStyle tutorial = TextStyle(
    fontFamily: familyCaption,
    fontSize: 22,
    fontWeight: FontWeight.w500,
    height: 1.25,
  );

  // ---- Numeric / data --------------------------------------------------
  static const TextStyle mono = TextStyle(
    fontFamily: familyMono,
    fontSize: 12,
    fontWeight: FontWeight.w500,
    height: 1.3,
    letterSpacing: 0.2,
  );

  static const TextStyle monoLarge = TextStyle(
    fontFamily: familyMono,
    fontSize: 16,
    fontWeight: FontWeight.w600,
    height: 1.2,
    letterSpacing: 0.1,
  );

  // ---- Button label ----------------------------------------------------
  static const TextStyle buttonLabel = TextStyle(
    fontFamily: familyUI,
    fontSize: 13,
    fontWeight: FontWeight.w600,
    height: 1.2,
  );

  /// Helper — apply a foreground color while preserving the rest.
  static TextStyle withColor(TextStyle base, Color color) =>
      base.copyWith(color: color);
}
