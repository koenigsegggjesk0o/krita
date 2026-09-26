// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// tutorial_overlay.dart — Feather 3D-style first-run tutorial captions.
//
// Feather 3D shows small handwritten captions on first launch ("Tap to
// draw", "Pinch to orbit", etc). Pre-v55-B the host's [setCaption]
// helper existed but only fired on guide-mode entry — the first-run
// flow was missing. This file exposes:
//
//   * [kTutorialShownKey] — the SharedPreferences flag key the host
//     reads / writes to gate the sequence to once-per-install.
//   * [kTutorialHints] — the 4 hint strings, in display order.
//   * [tutorialHintAt] — a pure accessor that returns the hint at the
//     given index or `null` if out of bounds (used by the host's
//     sequence loop + unit-tested in isolation here).
//
// The host (lib/screens/main_screen.dart) owns the Timer + the
// SharedPreferences read / write; this file just holds the data + the
// pure accessor so the sequence logic is unit-testable without pumping
// the full MainScreen (which initialises the engine + FFI).

/// SharedPreferences key for the "tutorial already shown" flag.
///
/// The `v55b` suffix ties the flag to this task's sequence — if a
/// future task rewrites the hints, it can bump the suffix to force a
/// re-show for upgraders.
const String kTutorialShownKey = 'feather.tutorialShown.v55b';

/// The 4 first-run hints, in display order. Wording mirrors the brief's
/// examples ("Tap to draw", "Pinch to orbit") + Feather 3D's own
/// onboarding captions.
const List<String> kTutorialHints = <String>[
  'Tap to draw a 3D stroke',
  'Pinch to orbit the camera',
  'Pick a material for the next stroke',
  'Tap the sun to tune the light',
];

/// Returns the tutorial hint at [index], or `null` if out of bounds.
///
/// Pure function — the host calls this in its Timer.periodic loop to
/// advance through the sequence; the `null` return signals "sequence
/// done". Kept here (not inline in the host) so the bounds logic is
/// unit-testable in isolation without pumping MainScreen.
String? tutorialHintAt(int index) {
  if (index < 0 || index >= kTutorialHints.length) return null;
  return kTutorialHints[index];
}

/// Returns true if [index] is the last hint in the sequence (i.e. the
/// host should write the SharedPreferences flag + cancel the Timer
/// after showing this hint). Pure + unit-testable.
bool isLastTutorialHint(int index) =>
    index == kTutorialHints.length - 1;
