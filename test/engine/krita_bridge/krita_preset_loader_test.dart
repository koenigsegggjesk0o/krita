// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// krita_preset_loader_test.dart — v0.57-C tests for KritaPresetLoader.
//
// Validates the PURE-DART surface that the host
// (lib/screens/main_screen.dart `_scanDiskPresets`) wires:
//   * [KritaPresetLoader.looksLikeKppPath] — .kpp extension filter.
//   * [KritaPresetLoader.parsePresetNameFromPath] — file-stem display name.
//
// The engine-roundtrip methods (loadPreset / scanDirectory / applyParamMap)
// require a real [KritaBrushController] (FFI native library) and are NOT
// unit-testable in isolation — they are exercised end-to-end by the
// native CI smoke (native/krita_bridge/smoke_test_real.cpp) and the
// engine's own `krita_brush_load_preset` tests. The data classes
// ([KritaLoadedPreset], [KritaPresetScanResult]) are constructible
// directly and are tested below for toString + field round-trips.

import 'dart:io' show Platform;

import 'package:feather_krita/engine/krita_bridge/krita_bindings.dart'
    show KritaPresetInfo;
import 'package:feather_krita/engine/krita_bridge/krita_param_map.dart';
import 'package:feather_krita/engine/krita_bridge/krita_preset_loader.dart';
import 'package:flutter_test/flutter_test.dart';

void main() {
  group('KritaPresetLoader.looksLikeKppPath', () {
    final loader = const KritaPresetLoader();

    test('accepts lowercase .kpp', () {
      expect(loader.looksLikeKppPath('brushes/round_brush.kpp'), isTrue);
    });

    test('accepts uppercase .KPP (case-insensitive)', () {
      expect(loader.looksLikeKppPath('brushes/ROUND_BRUSH.KPP'), isTrue);
    });

    test('accepts mixed-case .KpP', () {
      expect(loader.looksLikeKppPath('a/b/c.MyBrUsH.KpP'), isTrue);
    });

    test('rejects non-.kpp extensions', () {
      expect(loader.looksLikeKppPath('a/b.kpp.png'), isFalse); // .png suffix
      expect(loader.looksLikeKppPath('a/b.kpp'), isTrue); // .kpp suffix
      expect(loader.looksLikeKppPath('a/b.png'), isFalse);
      expect(loader.looksLikeKppPath('a/b.kpp.txt'), isFalse);
      expect(loader.looksLikeKppPath('a/b.kpp.bak'), isFalse);
    });

    test('rejects paths with no extension', () {
      expect(loader.looksLikeKppPath('brushes/round_brush'), isFalse);
    });

    test('rejects empty path', () {
      expect(loader.looksLikeKppPath(''), isFalse);
    });

    test('host contract: same predicate the boot scan uses', () {
      // Pins the contract that lib/screens/main_screen.dart
      // `_scanDiskPresets` depends on for filtering disk entities.
      for (final p in [
        'feather_presets/basic.kpp',
        'feather_resources/paintoppresets/eraser.KPP',
      ]) {
        expect(loader.looksLikeKppPath(p), isTrue);
      }
      for (final p in [
        'feather_presets/README.md',
        'feather_presets/.DS_Store',
        'feather_presets/thumb.png',
      ]) {
        expect(loader.looksLikeKppPath(p), isFalse);
      }
    });
  });

  group('KritaPresetLoader.parsePresetNameFromPath', () {
    final loader = const KritaPresetLoader();

    test('parses simple .kpp filename', () {
      expect(loader.parsePresetNameFromPath('round_brush.kpp'), 'Round brush');
    });

    test('parses .KPP uppercase extension', () {
      expect(loader.parsePresetNameFromPath('ROUND_BRUSH.KPP'), 'ROUND BRUSH');
    });

    test('strips directory path (POSIX separator)', () {
      expect(loader.parsePresetNameFromPath('a/b/round_brush.kpp'),
          'Round brush');
    });

    test('strips directory path (platform separator)', () {
      final p = ['feather_presets', 'sub_dir', 'round_brush.kpp']
          .join(Platform.pathSeparator);
      expect(loader.parsePresetNameFromPath(p), 'Round brush');
    });

    test('strips nested directory path with mixed separators', () {
      // parsePresetNameFromPath splits on both Platform.pathSeparator and
      // '/'. Verify a path with both yields just the file stem.
      final p = 'a/b${Platform.pathSeparator}c/round_brush.kpp';
      expect(loader.parsePresetNameFromPath(p), 'Round brush');
    });

    test('title-cases the first letter (UI display convention)', () {
      expect(loader.parsePresetNameFromPath('basic_5_size.kpp'),
          'Basic 5 size');
      expect(loader.parsePresetNameFromPath('small_detail.kpp'),
          'Small detail');
    });

    test('preserves existing capitalization in the rest of the stem', () {
      // Only the first letter is uppercased — internal capitalization is
      // preserved (e.g. camelCase preset names).
      expect(loader.parsePresetNameFromPath('myBrushXYZ.kpp'), 'MyBrushXYZ');
    });

    test('returns "Untitled" for empty stem (after .kpp strip)', () {
      // ".kpp" alone — stem is empty after stripping the extension.
      expect(loader.parsePresetNameFromPath('.kpp'), 'Untitled');
    });

    test('returns "Untitled" for path ending in separator + .kpp', () {
      expect(loader.parsePresetNameFromPath('a/b/.kpp'), 'Untitled');
    });

    test('handles filename with no underscores (single word)', () {
      expect(loader.parsePresetNameFromPath('paint.kpp'), 'Paint');
    });

    test('host contract: same display-name logic the boot scan uses', () {
      // Pins the contract that lib/screens/main_screen.dart
      // `_scanDiskPresets` depends on for the BrushPreset.name field.
      // The previous inline logic was:
      //   stem = fileName.replaceAll('.kpp', '').replaceAll('.KPP', '');
      //   stem = stem.replaceAll('_', ' ');
      //   if (stem.isEmpty) stem = fileName;
      // The wired helper produces the SAME name for non-empty stems
      // (modulo the title-case first letter, which is an intentional UI
      // improvement) and 'Untitled' instead of the raw filename for
      // empty stems.
      expect(loader.parsePresetNameFromPath('b)_Basic-5_Size_default.kpp'),
          'B) Basic-5 Size default');
    });
  });

  group('KritaPresetLoader data classes', () {
    test('KritaLoadedPreset holds all parsed fields + toString', () {
      final params = KritaParamMap()
        ..set('Krita/opacity', '100')
        ..set('SizeValue', '40');
      final p = KritaLoadedPreset(
        path: '/presets/basic.kpp',
        name: 'Basic',
        paintopId: 'paintbrush',
        params: params,
        isEraser: false,
      );
      expect(p.path, '/presets/basic.kpp');
      expect(p.name, 'Basic');
      expect(p.paintopId, 'paintbrush');
      expect(p.params.length, 2);
      expect(p.params['Krita/opacity'], '100');
      expect(p.isEraser, isFalse);
      expect(p.toString(), contains('Basic'));
      expect(p.toString(), contains('paintop=paintbrush'));
      expect(p.toString(), contains('eraser=false'));
    });

    test('KritaLoadedPreset eraser flag serializes in toString', () {
      final p = KritaLoadedPreset(
        path: '/presets/eraser.kpp',
        name: 'Eraser',
        paintopId: 'paintbrush',
        params: KritaParamMap(),
        isEraser: true,
      );
      expect(p.toString(), contains('eraser=true'));
    });

    test('KritaPresetScanResult ok getter + toString', () {
      // Success case: engineCode >= 0.
      final ok = KritaPresetScanResult(
        directory: '/presets',
        presets: const [
          KritaPresetInfo(name: 'Basic', family: 'paintbrush', path: '/p/1.kpp'),
          KritaPresetInfo(name: 'Eraser', family: 'paintbrush', path: '/p/2.kpp'),
        ],
        engineCode: 2,
      );
      expect(ok.ok, isTrue);
      expect(ok.presets.length, 2);
      expect(ok.toString(), contains('2 presets'));
      expect(ok.toString(), contains('code=2'));
    });

    test('KritaPresetScanResult.ok is false for negative engine codes', () {
      // -1 = bad arguments, -2 = missing directory (per file doc).
      for (final code in [-1, -2, -99]) {
        final r = KritaPresetScanResult(
          directory: '/nowhere',
          presets: const <KritaPresetInfo>[],
          engineCode: code,
        );
        expect(r.ok, isFalse, reason: 'code $code should be !ok');
        expect(r.presets, isEmpty);
      }
    });

    test('KritaPresetScanResult.ok is true for code 0 (empty dir)', () {
      final r = KritaPresetScanResult(
        directory: '/empty',
        presets: const <KritaPresetInfo>[],
        engineCode: 0,
      );
      expect(r.ok, isTrue);
      expect(r.presets, isEmpty);
    });
  });

  group('KritaPresetLoader const-constructible + stateless', () {
    test('const constructor compiles + instances are identical', () {
      // The host (main_screen.dart) holds KritaPresetLoader as a
      // `static const` field — verify the const contract holds.
      const a = KritaPresetLoader();
      const b = KritaPresetLoader();
      expect(identical(a, b), isTrue);
    });

    test('pure-Dart helpers do not mutate loader state', () {
      const loader = KritaPresetLoader();
      // Call each helper multiple times — results must be deterministic
      // and not affect subsequent calls (stateless contract).
      expect(loader.looksLikeKppPath('a.kpp'), isTrue);
      expect(loader.looksLikeKppPath('a.png'), isFalse);
      expect(loader.looksLikeKppPath('a.kpp'), isTrue);
      expect(loader.parsePresetNameFromPath('a_b.kpp'), 'A b');
      expect(loader.parsePresetNameFromPath('c_d.kpp'), 'C d');
      expect(loader.parsePresetNameFromPath('a_b.kpp'), 'A b');
    });
  });

  group('Host wiring contract (main_screen._scanDiskPresets)', () {
    // These tests pin the contract the host depends on. The host's
    // _scanDiskPresets uses BOTH helpers in sequence:
    //   if (!_presetLoader.looksLikeKppPath(entity.path)) continue;
    //   final stem = _presetLoader.parsePresetNameFromPath(entity.path);

    final loader = const KritaPresetLoader();

    test('a .kpp file passes both checks + yields a friendly name', () {
      const path = 'feather_presets/basic_5_size.kpp';
      expect(loader.looksLikeKppPath(path), isTrue);
      expect(loader.parsePresetNameFromPath(path), 'Basic 5 size');
    });

    test('a non-.kpp file is filtered out before name parsing', () {
      // The host's `continue` means parsePresetNameFromPath is never
      // called on a non-.kpp path — but verify the helpers compose
      // safely anyway (parsePresetNameFromPath would still strip the
      // extension if called on a .png path, returning the stem).
      const path = 'feather_presets/README.md';
      expect(loader.looksLikeKppPath(path), isFalse);
      // Not called by the host, but verify no crash:
      expect(loader.parsePresetNameFromPath(path), 'README.md');
    });

    test('uppercase .KPP path passes both checks', () {
      const path = 'feather_presets/ROUND_BRUSH.KPP';
      expect(loader.looksLikeKppPath(path), isTrue);
      expect(loader.parsePresetNameFromPath(path), 'ROUND BRUSH');
    });
  });
}
