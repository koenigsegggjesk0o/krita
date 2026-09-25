// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// app_theme.dart — Theme data for the Feather-Krita UI package.
//
// This is the *new* ui/ package theme; it composes the values from
// feather_colors, feather_typography and glassmorphism into a ThemeData
// that downstream screens can pass to MaterialApp. The legacy
// lib/theme/app_theme.dart still exists for the old editor — this file
// is the canonical source for anything under lib/ui/.

import 'package:flutter/material.dart';

import 'feather_colors.dart';
import 'feather_typography.dart';

class AppTheme {
  const AppTheme._();

  /// Default dark theme — what the app boots into.
  static ThemeData get darkTheme => _build(FeatherColors.dark);

  /// Light theme — opt-in via Settings.
  static ThemeData get lightTheme => _build(FeatherColors.light);

  static ThemeData _build(FeatherPalette p) {
    final isDark = p.brightness == Brightness.dark;
    final scheme = isDark
        ? ColorScheme.dark(
            primary: p.accent,
            secondary: FeatherPalette.accentPurple,
            surface: p.canvasBackground,
            onSurface: p.textPrimary,
            error: FeatherPalette.accentPink,
          )
        : ColorScheme.light(
            primary: p.accent,
            secondary: FeatherPalette.accentPurple,
            surface: p.canvasBackground,
            onSurface: p.textPrimary,
            error: FeatherPalette.accentPink,
          );

    return ThemeData(
      useMaterial3: true,
      brightness: p.brightness,
      scaffoldBackgroundColor: p.appBackground,
      canvasColor: p.appBackground,
      colorScheme: scheme,
      fontFamily: FeatherTypography.familyUI,
      splashFactory: NoSplash.splashFactory,
      visualDensity: VisualDensity.standard,
      textTheme: TextTheme(
        displayLarge: FeatherTypography.display.copyWith(color: p.textPrimary),
        headlineLarge: FeatherTypography.h1.copyWith(color: p.textPrimary),
        headlineMedium: FeatherTypography.h2.copyWith(color: p.textPrimary),
        bodyLarge: FeatherTypography.body.copyWith(color: p.textPrimary),
        bodyMedium: FeatherTypography.body.copyWith(color: p.textSecondary),
        bodySmall: FeatherTypography.caption.copyWith(color: p.textTertiary),
        labelLarge:
            FeatherTypography.buttonLabel.copyWith(color: p.textPrimary),
        labelSmall: FeatherTypography.micro.copyWith(color: p.textTertiary),
      ),
      iconTheme: IconThemeData(color: p.textPrimary, size: 22),
      dividerTheme: DividerThemeData(
        color: p.divider,
        thickness: 1,
        space: 1,
      ),
      appBarTheme: AppBarTheme(
        backgroundColor: Colors.transparent,
        elevation: 0,
        centerTitle: true,
        foregroundColor: p.textPrimary,
        titleTextStyle:
            FeatherTypography.h2.copyWith(color: p.textPrimary),
      ),
      tooltipTheme: TooltipThemeData(
        decoration: BoxDecoration(
          color: p.panelFillStrong,
          borderRadius: BorderRadius.circular(8),
          border: Border.all(color: p.panelBorder),
        ),
        textStyle: FeatherTypography.caption.copyWith(color: p.textPrimary),
        waitDuration: const Duration(milliseconds: 500),
      ),
      snackBarTheme: SnackBarThemeData(
        backgroundColor: p.panelFillStrong,
        contentTextStyle:
            FeatherTypography.body.copyWith(color: p.textPrimary),
        behavior: SnackBarBehavior.floating,
        shape: RoundedRectangleBorder(
          borderRadius: BorderRadius.circular(12),
        ),
      ),
    );
  }
}
