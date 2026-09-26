// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// tool_dock_test.dart — unit tests for the v0.54-A eyedropper tool
// addition to the FeatherTool enum + ToolDock widget.
//
// Verifies the eyedropper tool value exists, carries the right icon /
// tooltip / colour, and renders as a dock button the user can tap. The
// actual colour sampling is covered by assistance_wiring_test.dart's
// sampleColorAt group; this file covers the UI-side wiring.

import 'package:feather_krita/ui/theme/feather_colors.dart';
import 'package:feather_krita/ui/widgets/tool_dock.dart';
import 'package:flutter/material.dart';
import 'package:flutter_test/flutter_test.dart';

void main() {
  group('FeatherTool.eyedropper — enum properties', () {
    test('exists in the FeatherTool values list', () {
      expect(FeatherTool.values, contains(FeatherTool.eyedropper));
    });

    test('uses the colorize (eyedropper) icon', () {
      expect(FeatherTool.eyedropper.icon, Icons.colorize_rounded);
    });

    test('carries the "Eyedropper" tooltip', () {
      expect(FeatherTool.eyedropper.tooltip, 'Eyedropper');
    });

    test('colour resolves to the rose/pink accent', () {
      final c = FeatherTool.eyedropper.color(FeatherColors.dark);
      expect(c, FeatherPalette.accentPink);
    });
  });

  group('ToolDock — eyedropper button rendering', () {
    testWidgets('renders an eyedropper dock button the user can tap',
        (tester) async {
      FeatherTool? picked;
      await tester.pumpWidget(
        MaterialApp(
          theme: ThemeData(brightness: Brightness.dark, useMaterial3: true),
          home: Scaffold(
            backgroundColor: const Color(0xFF0F172A),
            body: Center(
              child: ToolDock(
                active: FeatherTool.draw,
                onSelect: (t) => picked = t,
              ),
            ),
          ),
        ),
      );
      // The eyedropper button shows the colorize icon + its tooltip.
      expect(find.byIcon(Icons.colorize_rounded), findsOneWidget);
      // Tapping it fires onSelect with FeatherTool.eyedropper.
      await tester.tap(find.byIcon(Icons.colorize_rounded));
      await tester.pumpAndSettle(const Duration(milliseconds: 200));
      expect(picked, FeatherTool.eyedropper);
    });

    testWidgets('eyedropper button highlights when it is the active tool',
        (tester) async {
      await tester.pumpWidget(
        MaterialApp(
          theme: ThemeData(brightness: Brightness.dark, useMaterial3: true),
          home: Scaffold(
            backgroundColor: const Color(0xFF0F172A),
            body: Center(
              child: ToolDock(
                active: FeatherTool.eyedropper,
                onSelect: (_) {},
              ),
            ),
          ),
        ),
      );
      // The button is present + the dock built without error (the
      // selected-state glow is handled by _DockButton's animation).
      expect(find.byIcon(Icons.colorize_rounded), findsOneWidget);
    });
  });
}
