// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// stroke_list_panel_test.dart — widget tests for the v55-B StrokeListPanel.
//
// Verifies the panel renders the 3D Strokes header + one row per stroke,
// the empty-state hint shows when the stroke list is empty, the per-row
// controls (visibility toggle, delete, drag handle) are present, and the
// per-row callbacks fire with the right stroke id. Reorder is exercised
// via the panel's onReorder callback (the actual drag is hard to drive
// in a widget test; the callback contract is what we verify).

import 'package:feather_krita/ui/widgets/material_picker.dart'
    show FeatherMaterial;
import 'package:feather_krita/ui/widgets/stroke_list_panel.dart';
import 'package:flutter/material.dart';
import 'package:flutter_test/flutter_test.dart';

void main() {
  // Helper: pump the panel inside a dark MaterialApp + Scaffold so the
  // GlassPanel's BackdropFilter has a surface to blur against.
  Future<void> pumpPanel(
    WidgetTester tester, {
    List<StrokeListItem> strokes = const <StrokeListItem>[],
    ValueChanged<int>? onToggleVisible,
    ValueChanged<int>? onDelete,
    void Function(int oldIndex, int newIndex)? onReorder,
    VoidCallback? onClose,
  }) async {
    await tester.pumpWidget(
      MaterialApp(
        theme: ThemeData(brightness: Brightness.dark, useMaterial3: true),
        home: Scaffold(
          backgroundColor: const Color(0xFF0F172A),
          body: Center(
            child: StrokeListPanel(
              strokes: strokes,
              onToggleVisible: onToggleVisible,
              onDelete: onDelete,
              onReorder: onReorder,
              onClose: onClose,
            ),
          ),
        ),
      ),
    );
    await tester.pumpAndSettle(const Duration(milliseconds: 200));
  }

  // Build a fake stroke list for the tests.
  List<StrokeListItem> fakeStrokes() => const <StrokeListItem>[
        StrokeListItem(
          id: 1,
          name: 'Stroke 1',
          color: 0xFF60A5FA,
          material: FeatherMaterial.shaded,
          isVisible: true,
          sampleCount: 42,
        ),
        StrokeListItem(
          id: 2,
          name: 'Stroke 2',
          color: 0xFFF472B6,
          material: FeatherMaterial.glow,
          isVisible: false,
          sampleCount: 17,
        ),
        StrokeListItem(
          id: 3,
          name: 'Stroke 3',
          color: 0xFFFB923C,
          material: FeatherMaterial.metallic,
          isVisible: true,
          sampleCount: 8,
        ),
      ];

  group('StrokeListPanel — rendering', () {
    testWidgets('renders the 3D Strokes header', (tester) async {
      await pumpPanel(tester);

      expect(find.text('3D Strokes'), findsOneWidget);
    });

    testWidgets('shows the empty-state hint when the stroke list is empty',
        (tester) async {
      await pumpPanel(tester, strokes: const <StrokeListItem>[]);

      expect(
        find.text('No 3D strokes yet. Tap the canvas to draw one.'),
        findsOneWidget,
      );
    });

    testWidgets('renders one row per stroke (3 strokes → 3 names)',
        (tester) async {
      await pumpPanel(tester, strokes: fakeStrokes());

      expect(find.text('Stroke 1'), findsOneWidget);
      expect(find.text('Stroke 2'), findsOneWidget);
      expect(find.text('Stroke 3'), findsOneWidget);
    });

    testWidgets('each row shows the material label + sample count',
        (tester) async {
      await pumpPanel(tester, strokes: fakeStrokes());

      // Stroke 1 is Shaded with 42 pts.
      expect(find.text('Shaded · 42 pts'), findsOneWidget);
      // Stroke 2 is Glow with 17 pts.
      expect(find.text('Glow · 17 pts'), findsOneWidget);
      // Stroke 3 is Metallic with 8 pts.
      expect(find.text('Metallic · 8 pts'), findsOneWidget);
    });

    testWidgets('each row has a drag handle, visibility icon, delete icon',
        (tester) async {
      await pumpPanel(tester, strokes: fakeStrokes());

      // 3 drag handles (one per row).
      expect(find.byIcon(Icons.drag_indicator_rounded), findsNWidgets(3));
      // 3 visibility icons — 2 visible (visibility_outlined) + 1 hidden
      // (visibility_off_outlined).
      expect(find.byIcon(Icons.visibility_outlined), findsNWidgets(2));
      expect(find.byIcon(Icons.visibility_off_outlined), findsNWidgets(1));
      // 3 delete icons.
      expect(find.byIcon(Icons.delete_outline_rounded), findsNWidgets(3));
    });
  });

  group('StrokeListPanel — callbacks', () {
    testWidgets('tapping a row\'s visibility icon fires onToggleVisible(id)',
        (tester) async {
      int? tappedId;
      await pumpPanel(
        tester,
        strokes: fakeStrokes(),
        onToggleVisible: (id) => tappedId = id,
      );

      // Tap the visibility icon on the first row.
      await tester.tap(find.byIcon(Icons.visibility_outlined).first);
      await tester.pumpAndSettle();

      expect(tappedId, 1);
    });

    testWidgets('tapping a row\'s delete icon fires onDelete(id)',
        (tester) async {
      int? deletedId;
      await pumpPanel(
        tester,
        strokes: fakeStrokes(),
        onDelete: (id) => deletedId = id,
      );

      // Tap the delete icon on the second row.
      await tester.tap(find.byIcon(Icons.delete_outline_rounded).at(1));
      await tester.pumpAndSettle();

      expect(deletedId, 2);
    });

    testWidgets('tapping the close button fires onClose', (tester) async {
      bool closed = false;
      await pumpPanel(
        tester,
        strokes: fakeStrokes(),
        onClose: () => closed = true,
      );

      // Find the close icon in the header (only one close icon there;
      // the row delete icons are delete_outline_rounded, not close_rounded).
      final closeIcon = find.byIcon(Icons.close_rounded);
      expect(closeIcon, findsOneWidget);
      await tester.tap(closeIcon);
      await tester.pumpAndSettle();

      expect(closed, isTrue);
    });

    testWidgets(
        'onReorder is wired (ReorderableListView reports the move)',
        (tester) async {
      int? receivedOld;
      int? receivedNew;
      await pumpPanel(
        tester,
        strokes: fakeStrokes(),
        onReorder: (oldIndex, newIndex) {
          receivedOld = oldIndex;
          receivedNew = newIndex;
        },
      );

      // Drive a reorder by long-pressing the first row's drag handle
      // + dragging it down. ReorderableListView reports oldIndex=0,
      // newIndex=1; the panel's wrapper normalises to oldIndex=0,
      // newIndex=0 (1 - 1) — which the host treats as a no-op. To get
      // a non-trivial move, drag the first row past the second: the
      // drag offset needs to clear the second row's height (~44 px).
      final dragHandle = find.byIcon(Icons.drag_indicator_rounded).first;
      await tester.longPress(dragHandle);
      await tester.pumpAndSettle(const Duration(milliseconds: 600));
      // Drag the first handle down past the second row.
      await tester.drag(dragHandle, const Offset(0, 80));
      await tester.pumpAndSettle();

      // The panel's wrapper normalises newIndex > oldIndex to newIndex - 1.
      // For oldIndex=0 dragged to position 2, the raw callback reports
      // newIndex=2; normalised to 1. The host's _reorderStrokes treats
      // (0, 1) as a real move.
      expect(receivedOld, isNotNull);
      expect(receivedNew, isNotNull);
      // Old index should be 0 (the first row was dragged).
      expect(receivedOld, 0);
    });
  });
}
