// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// assist_panel_test.dart — unit tests for the v0.54-A AssistPanel widget.
//
// Verifies the panel renders, the Stable Strokes toggle + intensity slider
// are present, and the per-control callbacks fire with the right payload.
// The panel is a pure widget (no host, no engine), so these are standard
// widget-test pumps. Mirror axes + Draw Shape mode sections are added in
// follow-up commits of v0.54-A; their tests will be added then.

import 'package:feather_krita/ui/widgets/assist_panel.dart';
import 'package:flutter/material.dart';
import 'package:flutter_test/flutter_test.dart';

void main() {
  // Helper: pump the AssistPanel inside a dark MaterialApp + Scaffold so the
  // panel's palette resolves to FeatherColors.dark and the BackdropFilter in
  // GlassPanel has a surface to blur.
  Future<void> pumpPanel(
    WidgetTester tester, {
    required AssistPanel panel,
  }) async {
    await tester.pumpWidget(
      MaterialApp(
        theme: ThemeData(brightness: Brightness.dark, useMaterial3: true),
        home: Scaffold(
          backgroundColor: const Color(0xFF0F172A),
          body: Center(child: panel),
        ),
      ),
    );
    await tester.pumpAndSettle(const Duration(milliseconds: 200));
  }

  group('AssistPanel — rendering', () {
    testWidgets('renders the Assist header + Stable Strokes section',
        (tester) async {
      await pumpPanel(tester, panel: const AssistPanel());

      expect(find.text('Assist'), findsOneWidget);
      // The section label is uppercased by _sectionLabel.
      expect(find.text('STABLE STROKES'), findsOneWidget);
      expect(find.text('Stabilize stroke'), findsOneWidget);
      expect(find.text('Causal Gaussian pre-filter'), findsOneWidget);
      // One Switch widget — the Stable Strokes toggle row.
      expect(find.byType(Switch), findsOneWidget);
      // One Slider widget — the intensity slider.
      expect(find.byType(Slider), findsOneWidget);
    });

    testWidgets('intensity slider is disabled when Stable Strokes is off',
        (tester) async {
      await pumpPanel(tester, panel: const AssistPanel());
      final slider = tester.widget<Slider>(find.byType(Slider));
      expect(slider.onChanged, isNull);
    });

    testWidgets('intensity slider is enabled when Stable Strokes is on',
        (tester) async {
      await pumpPanel(
        tester,
        panel: const AssistPanel(initialStableStrokes: true),
      );
      final slider = tester.widget<Slider>(find.byType(Slider));
      expect(slider.onChanged, isNotNull);
    });
  });

  group('AssistPanel — Stable Strokes callbacks', () {
    testWidgets('tapping the toggle fires onStableStrokesChanged true',
        (tester) async {
      bool? captured;
      await pumpPanel(
        tester,
        panel: AssistPanel(
          onStableStrokesChanged: (v) => captured = v,
        ),
      );
      // Tap the toggle row (the GestureDetector wrapping the row).
      await tester.tap(find.text('Stabilize stroke'));
      await tester.pumpAndSettle(const Duration(milliseconds: 200));
      expect(captured, isTrue);
    });

    testWidgets('dragging the intensity slider fires onStableStrokesIntensityChanged',
        (tester) async {
      double? captured;
      await pumpPanel(
        tester,
        panel: AssistPanel(
          initialStableStrokes: true,
          onStableStrokesIntensityChanged: (v) => captured = v,
        ),
      );
      // Drag the slider to the right (increase intensity).
      final center = tester.getCenter(find.byType(Slider));
      await tester.drag(find.byType(Slider), const Offset(40, 0));
      await tester.pumpAndSettle(const Duration(milliseconds: 200));
      // Slider issued at least one onChanged callback with a value > 0.5.
      expect(captured, isNotNull);
      expect(captured! > 0.5, isTrue);
      // Sanity: center is finite (suppress unused warning).
      expect(center, isNotNull);
    });

    testWidgets('tapping close fires onClose', (tester) async {
      bool closed = false;
      await pumpPanel(
        tester,
        panel: AssistPanel(onClose: () => closed = true),
      );
      await tester.tap(find.byIcon(Icons.close_rounded));
      await tester.pumpAndSettle(const Duration(milliseconds: 200));
      expect(closed, isTrue);
    });
  });
}
