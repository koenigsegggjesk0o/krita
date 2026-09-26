// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// perf_polish_test.dart — v0.55-C unit tests for the paint-loop perf
// primitives extracted to lib/utils/paint_perf.dart.
//
// These tests verify the dab throttle + stroke soft-cap decision logic in
// isolation. The host (lib/screens/main_screen.dart) wires these pure
// functions into its _onStrokeUpdate / _onStrokeEnd / _undo / _redo paths;
// pumping the full MainScreen widget tree to test them end-to-end would
// require the entire editor + native krita_bridge setup, which is out of
// scope for a perf-polish pass.

import 'package:feather_krita/utils/paint_perf.dart';
import 'package:flutter_test/flutter_test.dart';

void main() {
  group('kDabThrottle', () {
    test('is exactly 16 ms (60 fps frame budget)', () {
      expect(kDabThrottle, const Duration(milliseconds: 16));
    });

    test('is non-zero', () {
      expect(kDabThrottle.inMicroseconds, greaterThan(0));
    });
  });

  group('kStrokeSoftCap', () {
    test('is exactly 1000 strokes', () {
      expect(kStrokeSoftCap, 1000);
    });

    test('is positive', () {
      expect(kStrokeSoftCap, greaterThan(0));
    });
  });

  group('shouldFireDab', () {
    final throttle = const Duration(milliseconds: 16);

    test('fires on the first dab of a stroke (null lastDabTime)', () {
      final now = DateTime.now();
      expect(shouldFireDab(null, now, throttle), isTrue);
    });

    test('does NOT fire within the throttle window', () {
      final last = DateTime(2026, 9, 29, 9, 0, 0, 000);
      final now = last.add(const Duration(milliseconds: 15));
      expect(shouldFireDab(last, now, throttle), isFalse);
    });

    test('fires exactly at the throttle window boundary', () {
      final last = DateTime(2026, 9, 29, 9, 0, 0, 000);
      final now = last.add(const Duration(milliseconds: 16));
      expect(shouldFireDab(last, now, throttle), isTrue);
    });

    test('fires past the throttle window', () {
      final last = DateTime(2026, 9, 29, 9, 0, 0, 000);
      final now = last.add(const Duration(milliseconds: 100));
      expect(shouldFireDab(last, now, throttle), isTrue);
    });

    test('respects a custom throttle window', () {
      final last = DateTime(2026, 9, 29, 9, 0, 0, 000);
      final now = last.add(const Duration(milliseconds: 50));
      // With a 100 ms throttle, 50 ms is too soon.
      expect(
        shouldFireDab(last, now, const Duration(milliseconds: 100)),
        isFalse,
      );
      // With a 25 ms throttle, 50 ms is past the window.
      expect(
        shouldFireDab(last, now, const Duration(milliseconds: 25)),
        isTrue,
      );
    });
  });

  group('shouldWarnStrokeCap', () {
    const cap = 1000;

    test('does NOT fire below the cap', () {
      expect(shouldWarnStrokeCap(999, cap, false), isFalse);
    });

    test('fires AT the cap (boundary)', () {
      expect(shouldWarnStrokeCap(1000, cap, false), isTrue);
    });

    test('fires ABOVE the cap', () {
      expect(shouldWarnStrokeCap(5000, cap, false), isTrue);
    });

    test('does NOT fire again once already warned (idempotent)', () {
      expect(shouldWarnStrokeCap(1000, cap, true), isFalse);
      expect(shouldWarnStrokeCap(5000, cap, true), isFalse);
    });

    test('respects a custom cap value', () {
      expect(shouldWarnStrokeCap(50, 50, false), isTrue);
      expect(shouldWarnStrokeCap(49, 50, false), isFalse);
    });
  });

  group('shouldRearmStrokeCap', () {
    const cap = 1000;

    test('re-arms when count drops below the cap after a warning', () {
      expect(shouldRearmStrokeCap(999, cap, true), isTrue);
      expect(shouldRearmStrokeCap(0, cap, true), isTrue);
    });

    test('does NOT re-arm when still at or above the cap', () {
      expect(shouldRearmStrokeCap(1000, cap, true), isFalse);
      expect(shouldRearmStrokeCap(5000, cap, true), isFalse);
    });

    test('does NOT re-arm when not already warned (no-op)', () {
      expect(shouldRearmStrokeCap(999, cap, false), isFalse);
      expect(shouldRearmStrokeCap(0, cap, false), isFalse);
    });

    test('respects a custom cap value', () {
      expect(shouldRearmStrokeCap(49, 50, true), isTrue);
      expect(shouldRearmStrokeCap(50, 50, true), isFalse);
    });
  });

  group('shouldWarnStrokeCap + shouldRearmStrokeCap (round-trip)', () {
    const cap = 1000;

    test('warn → re-arm → warn cycle (undo then redraw)', () {
      // Stroke count climbs from 999 → 1000: warn fires.
      var warned = false;
      expect(shouldWarnStrokeCap(1000, cap, warned), isTrue);
      warned = true;

      // User keeps drawing to 1500: warn does NOT re-fire (idempotent).
      expect(shouldWarnStrokeCap(1500, cap, warned), isFalse);

      // User undoes back to 999: re-arm fires.
      expect(shouldRearmStrokeCap(999, cap, warned), isTrue);
      warned = false;

      // User redoes back to 1000: warn re-fires (re-armed cycle).
      expect(shouldWarnStrokeCap(1000, cap, warned), isTrue);
    });

    test('does NOT re-arm when count stays at the cap (no false re-arm)', () {
      var warned = true;
      // Count stays at 1000: should NOT re-arm (no drop below cap).
      expect(shouldRearmStrokeCap(1000, cap, warned), isFalse);
      // Warn should also NOT re-fire (still warned).
      expect(shouldWarnStrokeCap(1000, cap, warned), isFalse);
    });
  });
}
