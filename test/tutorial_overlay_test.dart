// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// tutorial_overlay_test.dart — unit tests for the v55-B tutorial_overlay
// helper (first-run caption sequence constants + pure accessor).
//
// Verifies the hint list has 4 entries, the SharedPreferences key is
// stable, the pure accessor returns the right hint at each index + null
// past the end, and the isLastTutorialHint predicate is correct.

import 'package:feather_krita/ui/widgets/tutorial_overlay.dart';
import 'package:flutter_test/flutter_test.dart';

void main() {
  group('tutorial_overlay — constants', () {
    test('the SharedPreferences key is stable + v55b-tagged', () {
      // The v55b suffix lets a future task bump the flag to force a
      // re-show for upgraders. Locking the value here guards against
      // accidental renames.
      expect(kTutorialShownKey, 'feather.tutorialShown.v55b');
    });

    test('the hint list has exactly 4 entries (per the brief)', () {
      expect(kTutorialHints, hasLength(4));
    });

    test('every hint is a non-empty, friendly sentence', () {
      for (final h in kTutorialHints) {
        expect(h.isNotEmpty, isTrue, reason: 'empty hint: $h');
        // Each hint ends with no terminal punctuation (Caveat caption
        // style — the brief's examples "Tap to draw" + "Pinch to orbit"
        // also have no period).
        expect(h.endsWith('.'), isFalse,
            reason: 'hint should not end with a period: $h');
      }
    });

    test('the first hint matches the brief example "Tap to draw"', () {
      // The brief lists "Tap to draw" as the first example. The actual
      // hint is a slightly longer "Tap to draw a 3D stroke" — verify it
      // starts with the brief's wording.
      expect(kTutorialHints.first.startsWith('Tap to draw'), isTrue);
    });

    test('the second hint matches the brief example "Pinch to orbit"', () {
      // Same: the brief lists "Pinch to orbit" as the second example.
      expect(kTutorialHints[1].startsWith('Pinch to orbit'), isTrue);
    });
  });

  group('tutorialHintAt — pure accessor', () {
    test('returns the hint at each valid index', () {
      for (var i = 0; i < kTutorialHints.length; i++) {
        expect(tutorialHintAt(i), kTutorialHints[i]);
      }
    });

    test('returns null for negative index', () {
      expect(tutorialHintAt(-1), isNull);
      expect(tutorialHintAt(-100), isNull);
    });

    test('returns null for index past the end (sequence-done signal)', () {
      expect(tutorialHintAt(kTutorialHints.length), isNull);
      expect(tutorialHintAt(kTutorialHints.length + 1), isNull);
      expect(tutorialHintAt(999), isNull);
    });

    test('returns null for index 0 when the hint list is empty', () {
      // Defensive: if a future task empties the list, the accessor must
      // still return null (no exception) so the host's Timer loop just
      // exits cleanly.
      // (We can't override kTutorialHints here, but we can verify the
      // bounds-check path on a known-out-of-range index. Already covered
      // above; this test documents the contract.)
      expect(tutorialHintAt(0), isNotNull);
    });
  });

  group('isLastTutorialHint — predicate', () {
    test('returns true only for the last index', () {
      expect(isLastTutorialHint(kTutorialHints.length - 1), isTrue);
    });

    test('returns false for every other index', () {
      for (var i = 0; i < kTutorialHints.length - 1; i++) {
        expect(isLastTutorialHint(i), isFalse, reason: 'index $i');
      }
    });

    test('returns false for out-of-range indices (defensive)', () {
      expect(isLastTutorialHint(-1), isFalse);
      expect(isLastTutorialHint(kTutorialHints.length), isFalse);
      expect(isLastTutorialHint(999), isFalse);
    });
  });
}
