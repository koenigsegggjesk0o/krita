// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// settings_model.dart — App-wide settings persistence model.
//
// Wraps every value the app persists through [SharedPreferences] in a
// single typed model so callers don't sprinkle string-key lookups across
// the codebase. The [SettingsRepository] is the only place that knows
// the SharedPreferences key names; everything else goes through this
// model.

/// Theme mode (matches MaterialApp's [ThemeMode]).
enum AppThemeMode {
  system,
  light,
  dark,
}

/// Units for distance / size displays.
enum DisplayUnit {
  /// World units (the internal 3D coordinate system).
  world,

  /// Meters (assumes 1 world unit == 1 meter).
  meters,

  /// Millimeters (assumes 1 world unit == 1 millimeter).
  millimeters,
}

/// App-wide settings model.
class SettingsModel {
  const SettingsModel({
    this.themeMode = AppThemeMode.system,
    this.displayUnit = DisplayUnit.world,
    this.firstRunAt,
    this.lastOpenedProjectPath,
    this.recentProjectPaths = const <String>[],
    this.autoSaveEnabled = false,
    this.autoSaveIntervalSeconds = 120,
    this.keepLastNAutoSaves = 5,
    this.maxUndoSteps = 100,
    this.showGridByDefault = true,
    this.showMirrorPlanesByDefault = true,
    this.defaultGuideSurface = 'Sphere',
    this.defaultBrushSize = 32.0,
    this.defaultBrushColor = 0xFF1A1A1A,
    this.preferredExportFormat = 'png',
    this.proStatus = false,
    this.crashReportingEnabled = false,
    this.telemetryEnabled = false,
    this.locale,
  });

  /// UI theme preference.
  final AppThemeMode themeMode;

  /// Display unit for distances / sizes.
  final DisplayUnit displayUnit;

  /// First-launch timestamp (UTC). `null` until first settings save.
  final DateTime? firstRunAt;

  /// Path of the project last opened, or `null` when the user has
  /// never opened a project.
  final String? lastOpenedProjectPath;

  /// Up to 10 most-recent project paths.
  final List<String> recentProjectPaths;

  /// Auto-save master switch.
  final bool autoSaveEnabled;

  /// Auto-save interval in seconds.
  final int autoSaveIntervalSeconds;

  /// How many rotating auto-save snapshots to keep on disk.
  final int keepLastNAutoSaves;

  /// Maximum undo steps retained.
  final int maxUndoSteps;

  /// Whether the 3D grid overlay is shown by default.
  final bool showGridByDefault;

  /// Whether the X/Y/Z mirror plane indicators are shown by default.
  final bool showMirrorPlanesByDefault;

  /// Display name of the default guide surface for new documents.
  final String defaultGuideSurface;

  /// Default brush radius in texture pixels.
  final double defaultBrushSize;

  /// Default brush ARGB color.
  final int defaultBrushColor;

  /// Preferred export format extension (without dot).
  final String preferredExportFormat;

  /// Whether the user has unlocked Pro features.
  final bool proStatus;

  /// Whether crash reporting is allowed.
  final bool crashReportingEnabled;

  /// Whether anonymous telemetry is allowed.
  final bool telemetryEnabled;

  /// Optional BCP-47 locale override (e.g. "en-US"). `null` = system.
  final String? locale;

  SettingsModel copyWith({
    AppThemeMode? themeMode,
    DisplayUnit? displayUnit,
    DateTime? firstRunAt,
    String? lastOpenedProjectPath,
    List<String>? recentProjectPaths,
    bool? autoSaveEnabled,
    int? autoSaveIntervalSeconds,
    int? keepLastNAutoSaves,
    int? maxUndoSteps,
    bool? showGridByDefault,
    bool? showMirrorPlanesByDefault,
    String? defaultGuideSurface,
    double? defaultBrushSize,
    int? defaultBrushColor,
    String? preferredExportFormat,
    bool? proStatus,
    bool? crashReportingEnabled,
    bool? telemetryEnabled,
    String? locale,
  }) =>
      SettingsModel(
        themeMode: themeMode ?? this.themeMode,
        displayUnit: displayUnit ?? this.displayUnit,
        firstRunAt: firstRunAt ?? this.firstRunAt,
        lastOpenedProjectPath:
            lastOpenedProjectPath ?? this.lastOpenedProjectPath,
        recentProjectPaths: recentProjectPaths ?? this.recentProjectPaths,
        autoSaveEnabled: autoSaveEnabled ?? this.autoSaveEnabled,
        autoSaveIntervalSeconds:
            autoSaveIntervalSeconds ?? this.autoSaveIntervalSeconds,
        keepLastNAutoSaves: keepLastNAutoSaves ?? this.keepLastNAutoSaves,
        maxUndoSteps: maxUndoSteps ?? this.maxUndoSteps,
        showGridByDefault: showGridByDefault ?? this.showGridByDefault,
        showMirrorPlanesByDefault:
            showMirrorPlanesByDefault ?? this.showMirrorPlanesByDefault,
        defaultGuideSurface: defaultGuideSurface ?? this.defaultGuideSurface,
        defaultBrushSize: defaultBrushSize ?? this.defaultBrushSize,
        defaultBrushColor: defaultBrushColor ?? this.defaultBrushColor,
        preferredExportFormat:
            preferredExportFormat ?? this.preferredExportFormat,
        proStatus: proStatus ?? this.proStatus,
        crashReportingEnabled:
            crashReportingEnabled ?? this.crashReportingEnabled,
        telemetryEnabled: telemetryEnabled ?? this.telemetryEnabled,
        locale: locale ?? this.locale,
      );

  /// Default settings.
  static const SettingsModel defaults = SettingsModel();

  @override
  bool operator ==(Object other) =>
      identical(this, other) ||
      other is SettingsModel &&
          other.themeMode == themeMode &&
          other.displayUnit == displayUnit &&
          other.firstRunAt == firstRunAt &&
          other.lastOpenedProjectPath == lastOpenedProjectPath &&
          _listEq(other.recentProjectPaths, recentProjectPaths) &&
          other.autoSaveEnabled == autoSaveEnabled &&
          other.autoSaveIntervalSeconds == autoSaveIntervalSeconds &&
          other.keepLastNAutoSaves == keepLastNAutoSaves &&
          other.maxUndoSteps == maxUndoSteps &&
          other.showGridByDefault == showGridByDefault &&
          other.showMirrorPlanesByDefault == showMirrorPlanesByDefault &&
          other.defaultGuideSurface == defaultGuideSurface &&
          other.defaultBrushSize == defaultBrushSize &&
          other.defaultBrushColor == defaultBrushColor &&
          other.preferredExportFormat == preferredExportFormat &&
          other.proStatus == proStatus &&
          other.crashReportingEnabled == crashReportingEnabled &&
          other.telemetryEnabled == telemetryEnabled &&
          other.locale == locale;

  @override
  int get hashCode => Object.hash(
        themeMode,
        displayUnit,
        firstRunAt,
        lastOpenedProjectPath,
        Object.hashAll(recentProjectPaths),
        autoSaveEnabled,
        autoSaveIntervalSeconds,
        keepLastNAutoSaves,
        maxUndoSteps,
        showGridByDefault,
        showMirrorPlanesByDefault,
        defaultGuideSurface,
        defaultBrushSize,
        defaultBrushColor,
        preferredExportFormat,
        proStatus,
        crashReportingEnabled,
        telemetryEnabled,
        locale,
      );
}

bool _listEq<T>(List<T> a, List<T> b) {
  if (a.length != b.length) return false;
  for (var i = 0; i < a.length; i++) {
    if (a[i] != b[i]) return false;
  }
  return true;
}
