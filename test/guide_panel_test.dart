// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// guide_panel_test.dart — unit tests for the v0.53 GuidePanel widget.
//
// Verifies the panel renders, the four mode segments are present, and the
// per-control callbacks (mode change, primitive shape, snap toggle, ribbon
// toggle, close) fire with the right payload. The panel is a pure widget
// (no host, no engine), so these are standard widget-test pumps.

import 'package:feather_krita/engine/guide3d/guide3d_primitive.dart';
import 'package:feather_krita/engine/guide3d/guide3d_type.dart';
import 'package:feather_krita/ui/widgets/guide_panel.dart';
import 'package:flutter/material.dart';
import 'package:flutter_test/flutter_test.dart';

void main() {
  // Helper: pump the GuidePanel inside a dark MaterialApp + Scaffold so the
  // panel's palette resolves to FeatherColors.dark and the BackdropFilter in
  // GlassPanel has a surface to blur.
  Future<void> pumpPanel(
    WidgetTester tester, {
    required GuidePanel panel,
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
    // BackdropFilter defers one frame; pump so the glass layer settles.
    await tester.pumpAndSettle(const Duration(milliseconds: 200));
  }

  group('GuidePanel — rendering', () {
    testWidgets('renders the 3D Guide header and all four mode segments',
        (tester) async {
      await pumpPanel(tester, panel: const GuidePanel());

      expect(find.text('3D Guide'), findsOneWidget);
      // The SegmentedButton renders one Text widget per segment.
      expect(find.text('Draw'), findsOneWidget);
      expect(find.text('Loft'), findsOneWidget);
      expect(find.text('Bend'), findsOneWidget);
      expect(find.text('Prim'), findsOneWidget);
    });

    testWidgets('renders the snap + ribbon toggle rows', (tester) async {
      await pumpPanel(tester, panel: const GuidePanel());

      expect(find.text('Snap to guide'), findsOneWidget);
      expect(find.text('Show guide ribbon'), findsOneWidget);
      // Two Switch widgets — one per toggle row.
      expect(find.byType(Switch), findsNWidgets(2));
    });
  });

  group('GuidePanel — mode callback', () {
    testWidgets('tapping the Bend segment fires onModeChanged with bent',
        (tester) async {
      Guide3DType? captured;
      await pumpPanel(
        tester,
        panel: GuidePanel(
          onModeChanged: (m) => captured = m,
        ),
      );

      // The SegmentedButton's segment is tapped via its label Text.
      await tester.tap(find.text('Bend'));
      await tester.pumpAndSettle();

      expect(captured, Guide3DType.bent);
    });

    testWidgets('tapping the Loft segment fires onModeChanged with lofted',
        (tester) async {
      Guide3DType? captured;
      await pumpPanel(
        tester,
        panel: GuidePanel(
          onModeChanged: (m) => captured = m,
        ),
      );

      await tester.tap(find.text('Loft'));
      await tester.pumpAndSettle();

      expect(captured, Guide3DType.lofted);
    });
  });

  group('GuidePanel — primitive section', () {
    testWidgets(
        'selecting Primitives mode then tapping Sphere fires the shape callback',
        (tester) async {
      Guide3DPrimitive? captured;
      await pumpPanel(
        tester,
        panel: GuidePanel(
          initialMode: Guide3DType.primitive,
          onPrimitiveKindChanged: (k) => captured = k,
        ),
      );

      // The primitive chips are visible only in Primitives mode.
      expect(find.text('Sphere'), findsOneWidget);
      await tester.tap(find.text('Sphere'));
      await tester.pumpAndSettle();

      expect(captured, Guide3DPrimitive.sphere);
    });

    testWidgets('the size slider fires onPrimitiveSizeChanged on drag',
        (tester) async {
      double? captured;
      await pumpPanel(
        tester,
        panel: GuidePanel(
          initialMode: Guide3DType.primitive,
          onPrimitiveSizeChanged: (v) => captured = v,
        ),
      );

      // The size Slider lives in the primitive section.
      final sliderFinder = find.byType(Slider);
      expect(sliderFinder, findsWidgets);

      // Drag the slider rightward to bump the value off the initial 2.0.
      await tester.timedDrag(
        sliderFinder.first,
        const Offset(40, 0),
        const Duration(milliseconds: 100),
      );
      await tester.pumpAndSettle();

      expect(captured, isNotNull);
      expect(captured, greaterThan(2.0));
    });
  });

  group('GuidePanel — toggles + close', () {
    testWidgets('tapping the snap switch flips it and fires onSnapChanged',
        (tester) async {
      bool? captured;
      await pumpPanel(
        tester,
        panel: GuidePanel(
          onSnapChanged: (v) => captured = v,
        ),
      );

      // The snap switch is the first Switch in the panel.
      await tester.tap(find.byType(Switch).first);
      await tester.pumpAndSettle();

      // Initial snap value is false → the toggle should report true.
      expect(captured, isTrue);
    });

    testWidgets('tapping the ribbon switch fires onRibbonChanged false',
        (tester) async {
      bool? captured;
      await pumpPanel(
        tester,
        panel: GuidePanel(
          // Default initialRibbon is true, so toggling → false.
          onRibbonChanged: (v) => captured = v,
        ),
      );

      await tester.tap(find.byType(Switch).at(1));
      await tester.pumpAndSettle();

      expect(captured, isFalse);
    });

    testWidgets('the close button fires onClose', (tester) async {
      var closed = 0;
      await pumpPanel(
        tester,
        panel: GuidePanel(onClose: () => closed++),
      );

      // The close button is the round white-on-gradient icon at the header.
      await tester.tap(find.byIcon(Icons.close_rounded));
      await tester.pumpAndSettle();

      expect(closed, 1);
    });
  });
}
