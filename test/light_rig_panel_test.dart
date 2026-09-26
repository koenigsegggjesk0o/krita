// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// light_rig_panel_test.dart — widget tests for the v55-B LightRigPanel.
//
// Verifies the panel renders the sun dial + the four sliders (azimuth /
// elevation / intensity / ambient), the section labels are present, and
// each slider's callback fires with the right payload. The panel is a
// pure widget (no host, no engine), so these are standard widget-test
// pumps.

import 'package:feather_krita/ui/widgets/light_rig_panel.dart';
import 'package:flutter/material.dart';
import 'package:flutter_test/flutter_test.dart';
import 'dart:math' as math;

void main() {
  // Helper: pump the panel inside a dark MaterialApp + Scaffold so the
  // GlassPanel's BackdropFilter has a surface to blur against.
  Future<void> pumpPanel(
    WidgetTester tester, {
    double azimuth = 5 * math.pi / 4,
    double elevation = math.pi / 4,
    double intensity = 1.0,
    double ambient = 0.35,
    ValueChanged<double>? onAzimuth,
    ValueChanged<double>? onElevation,
    ValueChanged<double>? onIntensity,
    ValueChanged<double>? onAmbient,
    VoidCallback? onClose,
  }) async {
    await tester.pumpWidget(
      MaterialApp(
        theme: ThemeData(brightness: Brightness.dark, useMaterial3: true),
        home: Scaffold(
          backgroundColor: const Color(0xFF0F172A),
          body: Center(
            child: LightRigPanel(
              initialAzimuth: azimuth,
              initialElevation: elevation,
              initialIntensity: intensity,
              initialAmbient: ambient,
              onAzimuthChanged: onAzimuth,
              onElevationChanged: onElevation,
              onIntensityChanged: onIntensity,
              onAmbientChanged: onAmbient,
              onClose: onClose,
            ),
          ),
        ),
      ),
    );
    await tester.pumpAndSettle(const Duration(milliseconds: 200));
  }

  group('LightRigPanel — rendering', () {
    testWidgets('renders the Light header + sun dial + 4 sliders',
        (tester) async {
      await pumpPanel(tester);

      expect(find.text('Light'), findsOneWidget);
      // Section labels.
      expect(find.text('DIRECTION'), findsOneWidget);
      expect(find.text('BRIGHTNESS'), findsOneWidget);
      // Slider labels.
      expect(find.text('Azimuth'), findsOneWidget);
      expect(find.text('Elevation'), findsOneWidget);
      expect(find.text('Intensity'), findsOneWidget);
      expect(find.text('Ambient'), findsOneWidget);
      // 4 sliders in total.
      expect(find.byType(Slider), findsNWidgets(4));
      // Sun icon (rotating dial).
      expect(find.byIcon(Icons.wb_sunny_rounded), findsOneWidget);
    });

    testWidgets('azimuth readout shows the initial value in degrees',
        (tester) async {
      // azimuth = 5π/4 = 225°.
      await pumpPanel(tester, azimuth: 5 * math.pi / 4);
      expect(find.textContaining('Az 225°'), findsOneWidget);
    });

    testWidgets('elevation readout shows the initial value in degrees',
        (tester) async {
      // elevation = π/4 = 45°.
      await pumpPanel(tester, elevation: math.pi / 4);
      expect(find.textContaining('El 45°'), findsOneWidget);
    });
  });

  group('LightRigPanel — callbacks', () {
    testWidgets('dragging the Azimuth slider fires onAzimuthChanged',
        (tester) async {
      double? picked;
      await pumpPanel(
        tester,
        azimuth: math.pi,
        onAzimuth: (v) => picked = v,
      );

      // Slider value normalised to 0..1 (azimuth / 2π). π / 2π = 0.5.
      final slider = tester.widget<Slider>(find.byType(Slider).first);
      expect(slider.value, closeTo(0.5, 0.001));

      // Drag the slider to the right (increase azimuth).
      await tester.drag(find.byType(Slider).first, const Offset(60, 0));
      await tester.pumpAndSettle();

      expect(picked, isNotNull);
      expect(picked! >= math.pi, isTrue);
    });

    testWidgets('dragging the Intensity slider fires onIntensityChanged',
        (tester) async {
      double? picked;
      await pumpPanel(
        tester,
        intensity: 1.0,
        onIntensity: (v) => picked = v,
      );

      // Intensity slider is normalised to 0..1 (intensity / 2). 1.0 / 2.0
      // = 0.5. The sliders render in build order: azimuth, elevation,
      // intensity, ambient — so the Intensity slider is the 3rd.
      final sliders = tester.widgetList<Slider>(find.byType(Slider)).toList();
      expect(sliders.length, 4);
      expect(sliders[2].value, closeTo(0.5, 0.001));

      // Drag the 3rd slider to the right (increase intensity).
      await tester.drag(
        find.byType(Slider).at(2),
        const Offset(60, 0),
      );
      await tester.pumpAndSettle();

      expect(picked, isNotNull);
      expect(picked! > 1.0, isTrue);
    });

    testWidgets('dragging the Ambient slider fires onAmbientChanged',
        (tester) async {
      double? picked;
      await pumpPanel(
        tester,
        ambient: 0.35,
        onAmbient: (v) => picked = v,
      );

      // Ambient slider is normalised to 0..1. 4th slider in build order.
      final sliders = tester.widgetList<Slider>(find.byType(Slider)).toList();
      expect(sliders[3].value, closeTo(0.35, 0.001));

      // Drag the 4th slider to the right (increase ambient).
      await tester.drag(
        find.byType(Slider).at(3),
        const Offset(60, 0),
      );
      await tester.pumpAndSettle();

      expect(picked, isNotNull);
      expect(picked! > 0.35, isTrue);
    });

    testWidgets('tapping the close button fires onClose', (tester) async {
      bool closed = false;
      await pumpPanel(tester, onClose: () => closed = true);

      // Find the close icon (in the header).
      final closeIcon = find.byIcon(Icons.close_rounded);
      expect(closeIcon, findsOneWidget);
      await tester.tap(closeIcon);
      await tester.pumpAndSettle();

      expect(closed, isTrue);
    });
  });
}
