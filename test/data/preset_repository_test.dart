// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// preset_repository_test.dart — unit tests for the v0.57-D wiring of
// PresetRepository to the runtime. Verifies searchDirs() returns the
// existing preset directories, the sidecar JSON round-trips every
// PresetModel field, toggleFavorite + favoriteIds persist the favorite
// flag across calls, and deleteSidecar removes the sidecar.

import 'dart:io';

import 'package:feather_krita/data/models/preset_model.dart';
import 'package:feather_krita/data/preset_repository.dart';
import 'package:feather_krita/models/brush_preset.dart' show BrushPreset;
import 'package:flutter_test/flutter_test.dart';

void main() {
  late PresetRepository repo;

  setUp(() {
    repo = PresetRepository();
  });

  group('PresetRepository — searchDirs', () {
    test('searchDirs() returns at least the user presets dir (always exists)',
        () {
      // presetsDir() creates itself on access, so searchDirs() always
      // includes at least one entry. The Krita stock library dir is
      // only included when the first-run extraction completed.
      final dirs = repo.searchDirs();
      expect(dirs, isNotEmpty);
      // Every returned dir exists on disk (the repo filters).
      for (final d in dirs) {
        expect(d.existsSync(), isTrue,
            reason: 'searchDirs returned a non-existent dir: ${d.path}');
      }
    });

    test('searchDirs() entries are unique (no duplicate paths)', () {
      final dirs = repo.searchDirs();
      final paths = dirs.map((d) => d.absolute.path).toSet();
      expect(paths.length, dirs.length,
          reason: 'searchDirs returned duplicate paths');
    });
  });

  group('PresetRepository — sidecar round-trip', () {
    test('saveSidecar → loadSidecar preserves every PresetModel field',
        () async {
      final model = PresetModel(
        id: 'test_preset_001',
        name: 'Test Preset',
        paintopId: 'basic_tip',
        settings: const <String, String>{'size': '10', 'opacity': '0.8'},
        description: 'A test preset',
        category: 'Ink',
        isFavorite: true,
        thumbnailPath: '/tmp/thumb.png',
        sourcePath: '/tmp/source.kpp',
      );
      await repo.saveSidecar(model);
      final reloaded = await repo.loadSidecar(model.id);
      expect(reloaded, isNotNull);
      expect(reloaded!.id, 'test_preset_001');
      expect(reloaded.name, 'Test Preset');
      expect(reloaded.paintopId, 'basic_tip');
      expect(reloaded.settings, <String, String>{'size': '10', 'opacity': '0.8'});
      expect(reloaded.description, 'A test preset');
      expect(reloaded.category, 'Ink');
      expect(reloaded.isFavorite, isTrue);
      expect(reloaded.thumbnailPath, '/tmp/thumb.png');
      expect(reloaded.sourcePath, '/tmp/source.kpp');
      // Cleanup so the sidecar doesn't leak into other tests.
      await repo.deleteSidecar(model.id);
    });

    test('loadSidecar returns null when no sidecar exists', () async {
      final reloaded = await repo.loadSidecar('nonexistent_preset_xyz');
      expect(reloaded, isNull);
    });
  });

  group('PresetRepository — favorites', () {
    test('toggleFavorite flips false → true + favoriteIds lists it', () async {
      final preset = BrushPreset(
        id: 'fav_test_preset',
        name: 'Fav Test',
        paintopId: 'airbrush',
      );
      // Initially no sidecar → toggleFavorite creates one with isFavorite=true.
      final afterFirst = await repo.toggleFavorite(preset);
      expect(afterFirst.isFavorite, isTrue);
      final favIds = await repo.favoriteIds();
      expect(favIds, contains('fav_test_preset'));
      // Toggle again → isFavorite flips to false, id drops off the list.
      final afterSecond = await repo.toggleFavorite(preset);
      expect(afterSecond.isFavorite, isFalse);
      final favIdsAfter = await repo.favoriteIds();
      expect(favIdsAfter, isNot(contains('fav_test_preset')));
      // Cleanup.
      await repo.deleteSidecar(preset.id);
    });

    test('favoriteIds skips malformed sidecar JSON without crashing',
        () async {
      // Write a malformed sidecar directly to the presets dir.
      final dir = repo.searchDirs().first;
      final junk = File('${dir.path}${Platform.pathSeparator}junk.preset.json');
      await junk.writeAsString('{ this is not valid json');
      // favoriteIds should swallow the parse error + return whatever
      // well-formed sidecars exist (possibly empty).
      final favIds = await repo.favoriteIds();
      expect(favIds, isA<List<String>>());
      // Cleanup.
      await junk.delete();
    });
  });

  group('PresetRepository — deleteSidecar', () {
    test('deleteSidecar returns false for a missing sidecar', () async {
      final removed = await repo.deleteSidecar('never_existed');
      expect(removed, isFalse);
    });

    test('deleteSidecar returns true + removes an existing sidecar', () async {
      const model = PresetModel(
        id: 'del_test_preset',
        name: 'Del Test',
        paintopId: 'pencil',
      );
      await repo.saveSidecar(model);
      expect((await repo.loadSidecar(model.id))?.id, 'del_test_preset');
      final removed = await repo.deleteSidecar(model.id);
      expect(removed, isTrue);
      expect(await repo.loadSidecar(model.id), isNull);
    });
  });
}
