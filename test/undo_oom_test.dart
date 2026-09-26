// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// undo_oom_test.dart — unit tests for the v56-B undo stack OOM mitigation.
//
// The host (lib/screens/main_screen.dart) stores a deep-copy snapshot of the
// whole stroke list per undo step. At 1000 strokes × 40 snapshots that was
// ~40 000 Stroke copies (≈200 MB) — a real Android OOM risk that v0.55-C
// flagged honestly. v56-B halves the base undo depth (40 → 20) and further
// reduces it to 10 on large documents (> 200 strokes).
//
// The cap logic lives in the pure, @visibleForTesting static
// [MainScreen.effectiveMaxUndoFor] so it can be unit-tested without pumping
// the full [MainScreen] widget (which needs engine + FFI init). These tests
// pin the two thresholds the brief requires:
//   * effective cap is 20 when stroke count <= 200
//   * effective cap is 10 when stroke count > 200

import 'package:feather_krita/screens/main_screen.dart';
import 'package:flutter_test/flutter_test.dart';

void main() {
  group('MainScreen.effectiveMaxUndoFor (v56-B undo OOM cap)', () {
    test('returns 20 (base cap) when stroke count <= 200', () {
      // Empty document.
      expect(MainScreen.effectiveMaxUndoFor(0), 20);
      // Small document.
      expect(MainScreen.effectiveMaxUndoFor(1), 20);
      // Exactly at the threshold — NOT reduced (the check is strict > 200).
      expect(MainScreen.effectiveMaxUndoFor(200), 20);
      // A typical medium document.
      expect(MainScreen.effectiveMaxUndoFor(150), 20);
    });

    test('returns 10 (reduced cap) when stroke count > 200', () {
      // One stroke past the threshold.
      expect(MainScreen.effectiveMaxUndoFor(201), 10);
      // The brief's worst-case scenario: 1000 strokes.
      expect(MainScreen.effectiveMaxUndoFor(1000), 10);
      // A very large document.
      expect(MainScreen.effectiveMaxUndoFor(10000), 10);
    });

    test('exposes the thresholds as @visibleForTesting constants', () {
      // Pin the constants so a future refactor can't silently drift them.
      expect(MainScreen.kBaseMaxUndo, 20);
      expect(MainScreen.kLargeDocMaxUndo, 10);
      expect(MainScreen.kLargeDocStrokeThreshold, 200);
      // The base cap must be strictly greater than the large-doc cap,
      // otherwise the dynamic reduction would be a no-op / regression.
      expect(MainScreen.kBaseMaxUndo, greaterThan(MainScreen.kLargeDocMaxUndo));
    });
  });
}
