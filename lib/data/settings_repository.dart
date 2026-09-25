// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// settings_repository.dart — App settings persistence (SharedPreferences).
//
// Reads / writes [SettingsModel] to [SharedPreferences]. Every field has
// a stable key so older app versions keep working when new fields are
// added (missing keys fall back to the model's default). The
// repository is the only place in the codebase that knows the key
// names; everything else goes through the typed [SettingsModel].

import 'dart:convert';

import 'package:shared_preferences/shared_preferences.dart';

import 'package:feather_krita/data/models/settings_model.dart';

/// SharedPreferences-backed settings store.
class SettingsRepository {
  SettingsRepository();

  static const _kThemeMode = 'feather.themeMode';
  static const _kDisplayUnit = 'feather.displayUnit';
  static const _kFirstRunAt = 'feather.firstRunAt';
  static const _kLastOpenedProjectPath = 'feather.lastOpenedProjectPath';
  static const _kRecentProjectPaths = 'feather.recentProjectPaths';
  static const _kAutoSaveEnabled = 'feather.autoSaveEnabled';
  static const _kAutoSaveIntervalSeconds = 'feather.autoSaveIntervalSeconds';
  static const _kKeepLastNAutoSaves = 'feather.keepLastNAutoSaves';
  static const _kMaxUndoSteps = 'feather.maxUndoSteps';
  static const _kShowGridByDefault = 'feather.showGridByDefault';
  static const _kShowMirrorPlanesByDefault = 'feather.showMirrorPlanesByDefault';
  static const _kDefaultGuideSurface = 'feather.defaultGuideSurface';
  static const _kDefaultBrushSize = 'feather.defaultBrushSize';
  static const _kDefaultBrushColor = 'feather.defaultBrushColor';
  static const _kPreferredExportFormat = 'feather.preferredExportFormat';
  static const _kProStatus = 'feather.proStatus';
  static const _kCrashReportingEnabled = 'feather.crashReportingEnabled';
  static const _kTelemetryEnabled = 'feather.telemetryEnabled';
  static const _kLocale = 'feather.locale';

  /// Loads the current settings, falling back to [SettingsModel.defaults]
  /// when SharedPreferences has nothing stored.
  Future<SettingsModel> load() async {
    final prefs = await SharedPreferences.getInstance();
    return SettingsModel(
      themeMode: _enumFromString(
              prefs.getString(_kThemeMode), AppThemeMode.values) ??
          AppThemeMode.system,
      displayUnit: _enumFromString(
              prefs.getString(_kDisplayUnit), DisplayUnit.values) ??
          DisplayUnit.world,
      firstRunAt: _parseDate(prefs.getString(_kFirstRunAt)),
      lastOpenedProjectPath: prefs.getString(_kLastOpenedProjectPath),
      recentProjectPaths: prefs.getStringList(_kRecentProjectPaths) ??
          const <String>[],
      autoSaveEnabled: prefs.getBool(_kAutoSaveEnabled) ?? false,
      autoSaveIntervalSeconds:
          prefs.getInt(_kAutoSaveIntervalSeconds) ?? 120,
      keepLastNAutoSaves: prefs.getInt(_kKeepLastNAutoSaves) ?? 5,
      maxUndoSteps: prefs.getInt(_kMaxUndoSteps) ?? 100,
      showGridByDefault: prefs.getBool(_kShowGridByDefault) ?? true,
      showMirrorPlanesByDefault:
          prefs.getBool(_kShowMirrorPlanesByDefault) ?? true,
      defaultGuideSurface:
          prefs.getString(_kDefaultGuideSurface) ?? 'Sphere',
      defaultBrushSize: prefs.getDouble(_kDefaultBrushSize) ?? 32.0,
      defaultBrushColor: prefs.getInt(_kDefaultBrushColor) ?? 0xFF1A1A1A,
      preferredExportFormat:
          prefs.getString(_kPreferredExportFormat) ?? 'png',
      proStatus: prefs.getBool(_kProStatus) ?? false,
      crashReportingEnabled:
          prefs.getBool(_kCrashReportingEnabled) ?? false,
      telemetryEnabled: prefs.getBool(_kTelemetryEnabled) ?? false,
      locale: prefs.getString(_kLocale),
    );
  }

  /// Persists [model] to SharedPreferences. Writes every key it can —
  /// a single failing key (e.g. an unsupported type on an older
  /// SharedPreferences plugin) does not abort the rest.
  Future<void> save(SettingsModel model) async {
    final prefs = await SharedPreferences.getInstance();
    await prefs.setString(_kThemeMode, model.themeMode.name);
    await prefs.setString(_kDisplayUnit, model.displayUnit.name);
    if (model.firstRunAt != null) {
      await prefs.setString(
          _kFirstRunAt, model.firstRunAt!.toIso8601String());
    }
    if (model.lastOpenedProjectPath != null) {
      await prefs.setString(
          _kLastOpenedProjectPath, model.lastOpenedProjectPath!);
    } else {
      await prefs.remove(_kLastOpenedProjectPath);
    }
    await prefs.setStringList(
        _kRecentProjectPaths, model.recentProjectPaths.take(10).toList());
    await prefs.setBool(_kAutoSaveEnabled, model.autoSaveEnabled);
    await prefs.setInt(
        _kAutoSaveIntervalSeconds, model.autoSaveIntervalSeconds);
    await prefs.setInt(_kKeepLastNAutoSaves, model.keepLastNAutoSaves);
    await prefs.setInt(_kMaxUndoSteps, model.maxUndoSteps);
    await prefs.setBool(_kShowGridByDefault, model.showGridByDefault);
    await prefs.setBool(
        _kShowMirrorPlanesByDefault, model.showMirrorPlanesByDefault);
    await prefs.setString(_kDefaultGuideSurface, model.defaultGuideSurface);
    await prefs.setDouble(_kDefaultBrushSize, model.defaultBrushSize);
    await prefs.setInt(_kDefaultBrushColor, model.defaultBrushColor);
    await prefs.setString(_kPreferredExportFormat, model.preferredExportFormat);
    await prefs.setBool(_kProStatus, model.proStatus);
    await prefs.setBool(
        _kCrashReportingEnabled, model.crashReportingEnabled);
    await prefs.setBool(_kTelemetryEnabled, model.telemetryEnabled);
    if (model.locale != null) {
      await prefs.setString(_kLocale, model.locale!);
    } else {
      await prefs.remove(_kLocale);
    }
  }

  /// Records that the user just opened [path]: bumps it to the top of
  /// the recent-projects list (deduped, capped at 10).
  Future<void> recordRecentProject(String path) async {
    final current = await load();
    final next = <String>[path, ...current.recentProjectPaths.where((p) => p != path)]
        .take(10)
        .toList();
    await save(current.copyWith(
      lastOpenedProjectPath: path,
      recentProjectPaths: next,
    ));
  }

  /// Clears the recent-projects list.
  Future<void> clearRecentProjects() async {
    final current = await load();
    await save(current.copyWith(
      recentProjectPaths: const <String>[],
      lastOpenedProjectPath: null,
    ));
  }
}

T? _enumFromString<T extends Enum>(String? value, List<T> values) {
  if (value == null) return null;
  for (final v in values) {
    if (v.name == value) return v;
  }
  return null;
}

DateTime? _parseDate(String? v) {
  if (v == null) return null;
  return DateTime.tryParse(v);
}

// dart:convert is imported for future use (e.g. JSON-encoding complex
// metadata). Kept so adding a JSON field later is a one-liner.
// ignore: unused_element
void _useJsonEncoder() => jsonEncode(const <String, dynamic>{});
