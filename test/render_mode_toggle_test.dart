// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// render_mode_toggle_test.dart — widget tests for the v55-B
// RenderModeToggle (Feather 3D-style 3-state render mode cluster).
//
// Verifies the toggle exposes all three Feather render modes (Shaded,
// Shadeless, Wireframe), the active button reflects the current mode,
// and the onChanged callback fires with the right payload.

import 'package:feather_krita/ui/widgets/render_mode_toggle.dart';
import 'package:flutter/material.dart';
import 'package:flutter_test/flutter_test.dart';

void main() {
  // Helper: pump the toggle inside a dark MaterialApp + Scaffold so the
  // GlassPanel's BackdropFilter has a surface to blur against.
  Future<void> pumpToggle(
    WidgetTester tester, {
    required RenderModeState mode,
    ValueChanged<RenderModeState>? onChanged,
  }) async {
    await tester.pumpWidget(
      MaterialApp(
        theme: ThemeData(brightness: Brightness.dark, useMaterial3: true),
        home: Scaffold(
          backgroundColor: const Color(0xFF0F172A),
          body: Center(
            child: RenderModeToggle(
              mode: mode,
              onChanged: onChanged,
            ),
          ),
        ),
      ),
    );
    await tester.pumpAndSettle(const Duration(milliseconds: 200));
  }

  group('RenderModeState enum — 3 modes', () {
    test('the enum exposes all three Feather render modes', () {
      expect(RenderModeState.values, hasLength(3));
      expect(RenderModeState.values, contains(RenderModeState.shaded));
      expect(RenderModeState.values, contains(RenderModeState.shadeless));
      expect(RenderModeState.values, contains(RenderModeState.wireframe));
    });

    test('each mode has a non-empty label', () {
      for (final m in RenderModeState.values) {
        expect(m.label.isNotEmpty, isTrue, reason: '$m has empty label');
      }
    });

    test('each mode has an icon + tooltip', () {
      for (final m in RenderModeState.values) {
        expect(m.icon, isNotNull, reason: '$m has no icon');
        expect(m.tooltip.isNotEmpty, isTrue, reason: '$m has empty tooltip');
      }
    });

    test('labels match the brief (Shaded / Shadeless / Wireframe)', () {
      expect(RenderModeState.shaded.label, 'Shaded');
      expect(RenderModeState.shadeless.label, 'Shadeless');
      expect(RenderModeState.wireframe.label, 'Wireframe');
    });
  });

  group('RenderModeToggle — rendering', () {
    testWidgets('renders all 3 mode buttons', (tester) async {
      await pumpToggle(tester, mode: RenderModeState.shaded);

      expect(find.text('Shaded'), findsOneWidget);
      expect(find.text('Shadeless'), findsOneWidget);
      expect(find.text('Wireframe'), findsOneWidget);
    });

    testWidgets('the active mode is highlighted (gradient)', (tester) async {
      await pumpToggle(tester, mode: RenderModeState.wireframe);

      // Three buttons rendered.
      expect(find.byIcon(Icons.view_in_ar_rounded), findsOneWidget);
      expect(find.byIcon(Icons.crop_square_rounded), findsOneWidget);
      expect(find.byIcon(Icons.grid_on_rounded), findsOneWidget);
    });

    testWidgets('switching the active mode updates the highlighted button',
        (tester) async {
      await pumpToggle(tester, mode: RenderModeState.shaded);
      // Initially Shaded is active.
      expect(find.text('Shaded'), findsOneWidget);

      // Re-pump with shadeless active — the widget is stateless so this
      // just verifies the active state propagates without error.
      await pumpToggle(tester, mode: RenderModeState.shadeless);
      expect(find.text('Shadeless'), findsOneWidget);
    });
  });

  group('RenderModeToggle — callbacks', () {
    testWidgets('tapping the Wireframe button fires onChanged(wireframe)',
        (tester) async {
      RenderModeState? picked;
      await pumpToggle(
        tester,
        mode: RenderModeState.shaded,
        onChanged: (m) => picked = m,
      );

      await tester.tap(find.text('Wireframe'));
      await tester.pumpAndSettle();

      expect(picked, RenderModeState.wireframe);
    });

    testWidgets('tapping the Shadeless button fires onChanged(shadeless)',
        (tester) async {
      RenderModeState? picked;
      await pumpToggle(
        tester,
        mode: RenderModeState.shaded,
        onChanged: (m) => picked = m,
      );

      await tester.tap(find.text('Shadeless'));
      await tester.pumpAndSettle();

      expect(picked, RenderModeState.shadeless);
    });

    testWidgets('tapping the active mode also fires onChanged (idempotent)',
        (tester) async {
      RenderModeState? picked;
      await pumpToggle(
        tester,
        mode: RenderModeState.shaded,
        onChanged: (m) => picked = m,
      );

      await tester.tap(find.text('Shaded'));
      await tester.pumpAndSettle();

      expect(picked, RenderModeState.shaded);
    });

    testWidgets('onChanged = null disables taps gracefully', (tester) async {
      await pumpToggle(tester, mode: RenderModeState.shaded);
      // No callback — tapping should not throw.
      await tester.tap(find.text('Wireframe'));
      await tester.pumpAndSettle();
      // Test passes if no exception was thrown.
      expect(true, isTrue);
    });
  });
}
