// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// material_picker_test.dart — widget tests for the v55-B MaterialPicker.
//
// Verifies the picker exposes all five Feather material chips (Shadeless,
// Shaded, Glow, Cutout, Metallic), the per-material pattern / glow sub-
// sections swap correctly, and the onMaterial callback fires with the
// right payload. The picker is a pure widget (no host, no engine), so
// these are standard widget-test pumps.

import 'package:feather_krita/ui/widgets/material_picker.dart';
import 'package:flutter/material.dart';
import 'package:flutter_test/flutter_test.dart';

void main() {
  // Helper: pump the MaterialPicker inside a dark MaterialApp + Scaffold so
  // the GlassPanel's BackdropFilter has a surface to blur against.
  Future<void> pumpPicker(
    WidgetTester tester, {
    FeatherMaterial material = FeatherMaterial.shadeless,
    FeatherPattern pattern = FeatherPattern.none,
    ValueChanged<FeatherMaterial>? onMaterial,
  }) async {
    await tester.pumpWidget(
      MaterialApp(
        theme: ThemeData(brightness: Brightness.dark, useMaterial3: true),
        home: Scaffold(
          backgroundColor: const Color(0xFF0F172A),
          body: Center(
            child: MaterialPicker(
              material: material,
              pattern: pattern,
              onMaterial: onMaterial,
            ),
          ),
        ),
      ),
    );
    await tester.pumpAndSettle(const Duration(milliseconds: 200));
  }

  group('FeatherMaterial enum — 5 materials', () {
    test('the enum exposes all five Feather materials', () {
      expect(FeatherMaterial.values, hasLength(5));
      expect(FeatherMaterial.values, contains(FeatherMaterial.metallic));
    });

    test('each material has a non-empty label', () {
      for (final m in FeatherMaterial.values) {
        expect(m.label.isNotEmpty, isTrue, reason: '$m has empty label');
      }
    });

    test('each material has an icon', () {
      for (final m in FeatherMaterial.values) {
        expect(m.icon, isNotNull, reason: '$m has no icon');
      }
    });

    test('only shadeless + shaded accept patterns', () {
      expect(FeatherMaterial.shadeless.supportsPattern, isTrue);
      expect(FeatherMaterial.shaded.supportsPattern, isTrue);
      expect(FeatherMaterial.glow.supportsPattern, isFalse);
      expect(FeatherMaterial.cutout.supportsPattern, isFalse);
      expect(FeatherMaterial.metallic.supportsPattern, isFalse);
    });

    test('metallic has the "Metallic" label', () {
      expect(FeatherMaterial.metallic.label, 'Metallic');
    });
  });

  group('MaterialPicker — rendering', () {
    testWidgets('renders the MATERIAL header + all 5 material chips',
        (tester) async {
      await pumpPicker(tester);

      expect(find.text('MATERIAL'), findsOneWidget);
      // The _MaterialGrid renders one chip per FeatherMaterial value.
      expect(find.text('Shadeless'), findsOneWidget);
      expect(find.text('Shaded'), findsOneWidget);
      expect(find.text('Glow'), findsOneWidget);
      expect(find.text('Cutout'), findsOneWidget);
      expect(find.text('Metallic'), findsOneWidget);
    });

    testWidgets('shadeless selection shows the PATTERN section',
        (tester) async {
      await pumpPicker(
        tester,
        material: FeatherMaterial.shadeless,
        pattern: FeatherPattern.dot,
      );

      expect(find.text('PATTERN'), findsOneWidget);
      expect(find.text('Dot'), findsOneWidget);
    });

    testWidgets('glow selection shows the Glow intensity slider',
        (tester) async {
      await pumpPicker(tester, material: FeatherMaterial.glow);

      expect(find.text('Glow intensity'), findsOneWidget);
      // No PATTERN section for glow (per the docs).
      expect(find.text('PATTERN'), findsNothing);
    });

    testWidgets(
        'metallic selection shows the "no pattern available" hint '
        '(metallic does not accept patterns)', (tester) async {
      await pumpPicker(tester, material: FeatherMaterial.metallic);

      expect(
        find.textContaining('No pattern available for Metallic'),
        findsOneWidget,
      );
      // No PATTERN section for metallic (supportsPattern == false).
      expect(find.text('PATTERN'), findsNothing);
      // No glow intensity slider for metallic either.
      expect(find.text('Glow intensity'), findsNothing);
    });
  });

  group('MaterialPicker — callbacks', () {
    testWidgets('tapping a chip fires onMaterial with that value',
        (tester) async {
      FeatherMaterial? picked;
      await pumpPicker(
        tester,
        material: FeatherMaterial.shadeless,
        onMaterial: (m) => picked = m,
      );

      // Tap the "Metallic" chip — the last in the 5-row grid.
      await tester.tap(find.text('Metallic'));
      await tester.pumpAndSettle();

      expect(picked, FeatherMaterial.metallic);
    });

    testWidgets('tapping the Glow chip fires onMaterial(glow)',
        (tester) async {
      FeatherMaterial? picked;
      await pumpPicker(
        tester,
        material: FeatherMaterial.shadeless,
        onMaterial: (m) => picked = m,
      );

      await tester.tap(find.text('Glow'));
      await tester.pumpAndSettle();

      expect(picked, FeatherMaterial.glow);
    });
  });
}
