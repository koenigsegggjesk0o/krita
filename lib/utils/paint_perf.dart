// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// paint_perf.dart — v0.55-C paint-loop perf primitives.
//
// Extracted from lib/screens/main_screen.dart so the dab throttle + stroke
// soft-cap decision logic is unit-testable in isolation (MainScreen is a
// 4000-line StatefulWidget that can't be pumped without the full editor
// tree; these primitives are pure functions over scalars).
//
// Scope: paint-loop perf ONLY. No engine internals, no rendering, no iOS.

/// Dab-generation throttle window. The native krita_bridge's
/// `krita_brush_generate_dab` is a synchronous FFI call (see
/// lib/engine/krita_bridge/krita_brush_controller.dart); on a fast pen
/// drag the GestureDetector fires onPanUpdate per micro-pixel move
/// (200–500 events/sec on a 120 Hz tablet), so without a gate the dab
/// path saturates the UI isolate and drops frames.
///
/// 16 ms = the canonical 60 fps frame budget. The Stroke3D capture (the
/// curve source of truth) is NOT throttled — only the rasterized preview
/// dab layer is.
const Duration kDabThrottle = Duration(milliseconds: 16);

/// Document stroke soft-cap. The host stores every committed stroke in a
/// List<Stroke> + a parallel Map<Stroke3D> + scene-graph references + a
/// material map entry. At ~5 KB / stroke, 10000 strokes is ~50 MB of
/// document state — the OOM risk the v0.55-C brief flagged.
///
/// We do NOT auto-merge or auto-delete the oldest stroke (silent data
/// loss). Instead the host surfaces a soft warning SnackBar the moment
/// the count crosses this cap.
const int kStrokeSoftCap = 1000;

/// Pure throttle decision for the dab-generation gate.
///
/// Returns `true` if a dab should fire NOW given the [lastDabTime] (null =
/// first dab of a stroke), the current [now], and the [throttle] window.
/// Used by `_MainScreenState._onStrokeUpdate` to cap dab generation at
/// ~60 Hz without dropping the Stroke3D capture (which records every
/// sample for curve fidelity).
bool shouldFireDab(DateTime? lastDabTime, DateTime now, Duration throttle) {
  if (lastDabTime == null) return true; // First dab of the stroke.
  return now.difference(lastDabTime) >= throttle;
}

/// Pure stroke-cap warning decision.
///
/// Returns `true` if the soft-cap warning SnackBar should fire NOW given
/// the current document stroke [count], the [cap], and whether the
/// warning has [alreadyWarned] for the current threshold-cross. The host
/// re-arms [alreadyWarned] when the count drops back below the cap (via
/// undo / redo / erase / vacuum), so a future re-cross re-fires the
/// warning.
bool shouldWarnStrokeCap(int count, int cap, bool alreadyWarned) {
  if (alreadyWarned) return false; // One warning per threshold-cross.
  return count >= cap;
}

/// Pure stroke-cap re-arm decision.
///
/// Returns `true` if the warning flag should be CLEARED now (re-armed)
/// because the [count] dropped back below the [cap]. The host calls this
/// from `_maybeWarnStrokeCap` on every stroke list mutation (commit /
/// undo / redo) so a future re-cross re-fires the warning.
bool shouldRearmStrokeCap(int count, int cap, bool alreadyWarned) {
  if (!alreadyWarned) return false; // Already disarmed.
  return count < cap;
}
