// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// settings_repository_test.dart — unit tests for the v0.57-D wiring of
// SettingsRepository to the runtime. Verifies the load/save round-trip
// preserves every field (no key drift), the defaults match
// SettingsModel.defaults when the prefs are empty, and the
// recordRecentProject helper dedupes + caps the recent-projects list.

import 'package:feather_krita/data/models/settings_model.dart';
import 'package:feather_krita/data/settings_repository.dart';
import 'package:flutter_test/flutter_test.dart';
import 'package:shared_preferences/shared_preferences.dart';

void main() {
  setUp(() {
    // SharedPreferences plugin requires a mock initial-values map in
    // unit tests; an empty map simulates a fresh install.
    SharedPreferences.setMockInitialValues(<String, Object>{});
  });

  group('SettingsRepository — load defaults', () {
    test('load() on empty prefs returns the SettingsModel defaults',
        () async {
      final repo = SettingsRepository();
      final model = await repo.load();
      // Defaults pinned by SettingsModel: brush size 32, grid on,
      // mirror planes on, undo 100, autosave off.
      expect(model.defaultBrushSize, 32.0);
      expect(model.showGridByDefault, isTrue);
      expect(model.showMirrorPlanesByDefault, isTrue);
      expect(model.maxUndoSteps, 100);
      expect(model.autoSaveEnabled, isFalse);
      expect(model.recentProjectPaths, isEmpty);
      expect(model.themeMode, AppThemeMode.system);
    });
  });

  group('SettingsRepository — save/load round-trip', () {
    test('every field survives a save → load round-trip', () async {
      final repo = SettingsRepository();
      final original = SettingsModel(
        themeMode: AppThemeMode.dark,
        displayUnit: DisplayUnit.millimeters,
        autoSaveEnabled: true,
        autoSaveIntervalSeconds: 60,
        keepLastNAutoSaves: 3,
        maxUndoSteps: 50,
        showGridByDefault: false,
        showMirrorPlanesByDefault: false,
        defaultGuideSurface: 'Cylinder',
        defaultBrushSize: 17.5,
        defaultBrushColor: 0xABCDEF01,
        preferredExportFormat: 'webp',
        proStatus: true,
        crashReportingEnabled: true,
        telemetryEnabled: true,
        locale: 'fr-CA',
        recentProjectPaths: const <String>['/a', '/b', '/c'],
        lastOpenedProjectPath: '/b',
      );
      await repo.save(original);
      final reloaded = await repo.load();
      expect(reloaded.themeMode, AppThemeMode.dark);
      expect(reloaded.displayUnit, DisplayUnit.millimeters);
      expect(reloaded.autoSaveEnabled, isTrue);
      expect(reloaded.autoSaveIntervalSeconds, 60);
      expect(reloaded.keepLastNAutoSaves, 3);
      expect(reloaded.maxUndoSteps, 50);
      expect(reloaded.showGridByDefault, isFalse);
      expect(reloaded.showMirrorPlanesByDefault, isFalse);
      expect(reloaded.defaultGuideSurface, 'Cylinder');
      expect(reloaded.defaultBrushSize, 17.5);
      expect(reloaded.defaultBrushColor, 0xABCDEF01);
      expect(reloaded.preferredExportFormat, 'webp');
      expect(reloaded.proStatus, isTrue);
      expect(reloaded.crashReportingEnabled, isTrue);
      expect(reloaded.telemetryEnabled, isTrue);
      expect(reloaded.locale, 'fr-CA');
      expect(reloaded.recentProjectPaths, <String>['/a', '/b', '/c']);
      expect(reloaded.lastOpenedProjectPath, '/b');
    });

    test('partial save via copyWith preserves the other fields', () async {
      // Mirrors the MainScreen._persistSettings pattern: re-load, then
      // copyWith only the host-owned fields (brush size + grid).
      final repo = SettingsRepository();
      await repo.save(const SettingsModel(
        defaultBrushSize: 40.0,
        defaultBrushColor: 0xFF112233,
        preferredExportFormat: 'png',
      ));
      final current = await repo.load();
      await repo.save(current.copyWith(
        defaultBrushSize: 25.0,
        showGridByDefault: false,
      ));
      final reloaded = await repo.load();
      // The two mutated fields took the new values…
      expect(reloaded.defaultBrushSize, 25.0);
      expect(reloaded.showGridByDefault, isFalse);
      // …and the untouched fields are preserved.
      expect(reloaded.defaultBrushColor, 0xFF112233);
      expect(reloaded.preferredExportFormat, 'png');
    });
  });

  group('SettingsRepository — recent projects', () {
    test('recordRecentProject bumps the path to the top + dedupes',
        () async {
      final repo = SettingsRepository();
      await repo.save(const SettingsModel(
        recentProjectPaths: <String>['/old', '/x', '/y'],
        lastOpenedProjectPath: '/old',
      ));
      await repo.recordRecentProject('/x');
      final reloaded = await repo.load();
      // /x is now first; the duplicate is gone; the rest follow.
      expect(reloaded.recentProjectPaths.first, '/x');
      expect(reloaded.recentProjectPaths, hasLength(3));
      expect(reloaded.recentProjectPaths.toSet().length, 3);
      expect(reloaded.lastOpenedProjectPath, '/x');
    });

    test('recordRecentProject caps the recent list at 10', () async {
      final repo = SettingsRepository();
      final initial = <String>[for (var i = 0; i < 10; i++) '/p$i'];
      await repo.save(SettingsModel(
        recentProjectPaths: initial,
      ));
      await repo.recordRecentProject('/new');
      final reloaded = await repo.load();
      expect(reloaded.recentProjectPaths.first, '/new');
      // Still capped at 10 (the oldest /p9 was dropped).
      expect(reloaded.recentProjectPaths, hasLength(10));
      expect(reloaded.recentProjectPaths.contains('/p9'), isFalse);
      expect(reloaded.recentProjectPaths.contains('/p0'), isTrue);
    });
  });
}
