// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// resource_repository_test.dart — unit tests for the v0.57-D wiring of
// ResourceRepository to the runtime. Verifies the importFile → list →
// byId → byKind → remove round-trip (content-addressed dedup), the
// rename path, and the indexExtractedPresets manifest builder used by
// KritaResources.ensureImported after the first-run extraction.

import 'dart:io';

import 'package:feather_krita/data/models/resource_model.dart';
import 'package:feather_krita/data/resource_repository.dart';
import 'package:flutter_test/flutter_test.dart';

void main() {
  late ResourceRepository repo;
  late Directory tempDir;

  setUp(() async {
    repo = ResourceRepository();
    // Isolate each test under a fresh system-temp dir so the manifest +
    // imported-bytes files don't collide with other tests or the user's
    // real feather_resources tree. We point FEATHER_DATA_DIR at the
    // temp dir BEFORE constructing ResourceRepository — but
    // Platform.environment is immutable at runtime, so instead we
    // clean the repo's real resourcesDir (under system temp) in setUp.
    tempDir = await Directory.systemTemp.createTemp('feather_res_repo_test_');
    // Wipe the manifest so each test starts clean.
    final manifest = File(
        '${repo.resourcesDir.path}${Platform.pathSeparator}index.json');
    if (manifest.existsSync()) await manifest.delete();
  });

  tearDown(() async {
    // Best-effort cleanup of any imported files this test created.
    try {
      if (tempDir.existsSync()) await tempDir.delete(recursive: true);
    } catch (_) {}
  });

  group('ResourceRepository — importFile round-trip', () {
    test('importFile registers a file + list/byId/byKind see it', () async {
      // Create a temp source file to import.
      final source = File('${tempDir.path}${Platform.pathSeparator}tip.png');
      await source.writeAsBytes(<int>[1, 2, 3, 4, 5]);

      final model = await repo.importFile(source,
          kind: ResourceKind.image, displayName: 'My Tip');
      expect(model.id, isNotEmpty);
      expect(model.kind, ResourceKind.image);
      expect(model.name, 'My Tip');
      expect(model.mimeType, 'image/png');
      expect(model.sizeBytes, 5);

      // list() includes it.
      final all = await repo.list();
      expect(all.any((r) => r.id == model.id), isTrue);

      // byId() finds it.
      final byId = await repo.byId(model.id);
      expect(byId, isNotNull);
      expect(byId!.name, 'My Tip');

      // byKind() filters by kind.
      final images = await repo.byKind(ResourceKind.image);
      expect(images, hasLength(1));
      final models = await repo.byKind(ResourceKind.model);
      expect(models, isEmpty);
    });

    test('re-importing the same file is a no-op (content-addressed dedup)',
        () async {
      final source = File('${tempDir.path}${Platform.pathSeparator}dup.png');
      await source.writeAsBytes(<int>[9, 9, 9]);

      final first = await repo.importFile(source, kind: ResourceKind.image);
      final second = await repo.importFile(source, kind: ResourceKind.image);

      // Same id returned both times; only one manifest entry exists.
      expect(second.id, first.id);
      final all = await repo.list();
      expect(all.where((r) => r.id == first.id), hasLength(1));

      // Cleanup.
      await repo.remove(first.id);
    });

    test('remove deletes the manifest entry + stored file', () async {
      final source = File('${tempDir.path}${Platform.pathSeparator}gone.obj');
      await source.writeAsBytes(<int>[0, 0, 1]);

      final model = await repo.importFile(source, kind: ResourceKind.model);
      expect(await repo.byId(model.id), isNotNull);

      final removed = await repo.remove(model.id);
      expect(removed, isTrue);
      expect(await repo.byId(model.id), isNull);
      // The stored copy is gone too.
      final stored = File(model.path);
      expect(stored.existsSync(), isFalse);
    });
  });

  group('ResourceRepository — rename', () {
    test('rename changes the display name only (path unchanged)', () async {
      final source = File('${tempDir.path}${Platform.pathSeparator}r.png');
      await source.writeAsBytes(<int>[7]);

      final model = await repo.importFile(source, kind: ResourceKind.image);
      final renamed = await repo.rename(model.id, 'New Name');
      expect(renamed, isNotNull);
      expect(renamed!.name, 'New Name');
      expect(renamed.path, model.path);
      expect(renamed.id, model.id);

      // Cleanup.
      await repo.remove(model.id);
    });
  });

  group('ResourceRepository — indexExtractedPresets', () {
    test('indexes .kpp files under paintoppresets/ as brushPreset', () async {
      // Simulate the Krita extraction by writing two fake .kpp files
      // under <resourcesDir>/paintoppresets/.
      final presetsDir = Directory(
          '${repo.resourcesDir.path}${Platform.pathSeparator}paintoppresets');
      presetsDir.createSync(recursive: true);
      final kpp1 =
          File('${presetsDir.path}${Platform.pathSeparator}basic_tip.kpp');
      await kpp1.writeAsBytes(<int>[1, 2, 3]);
      final kpp2 =
          File('${presetsDir.path}${Platform.pathSeparator}airbrush.kpp');
      await kpp2.writeAsBytes(<int>[4, 5, 6, 7]);
      // A non-.kpp file that should be skipped.
      final txt =
          File('${presetsDir.path}${Platform.pathSeparator}readme.txt');
      await txt.writeAsBytes(<int>[0]);

      final added = await repo.indexExtractedPresets();
      expect(added, 2);

      final brushPresets = await repo.byKind(ResourceKind.brushPreset);
      expect(brushPresets, hasLength(2));
      final names = brushPresets.map((r) => r.name).toSet();
      expect(names, containsAll(<String>['basic_tip', 'airbrush']));
      for (final r in brushPresets) {
        expect(r.mimeType, 'application/x-krita-brushpreset');
      }

      // Cleanup the indexed entries + the simulated extraction dir.
      for (final r in brushPresets) {
        await repo.remove(r.id);
      }
      await presetsDir.delete(recursive: true);
    });

    test('indexExtractedPresets is idempotent (re-index adds 0)', () async {
      final presetsDir = Directory(
          '${repo.resourcesDir.path}${Platform.pathSeparator}paintoppresets');
      presetsDir.createSync(recursive: true);
      final kpp =
          File('${presetsDir.path}${Platform.pathSeparator}once.kpp');
      await kpp.writeAsBytes(<int>[42]);

      final first = await repo.indexExtractedPresets();
      expect(first, 1);
      // Re-indexing the same file content → dedup → 0 newly added.
      final second = await repo.indexExtractedPresets();
      expect(second, 0);

      final brushPresets = await repo.byKind(ResourceKind.brushPreset);
      expect(brushPresets, hasLength(1));

      // Cleanup.
      for (final r in brushPresets) {
        await repo.remove(r.id);
      }
      await presetsDir.delete(recursive: true);
    });

    test('indexExtractedPresets returns 0 when paintoppresets/ is missing',
        () async {
      // No paintoppresets dir → 0 indexed, no crash.
      final added = await repo.indexExtractedPresets();
      expect(added, 0);
    });
  });
}
