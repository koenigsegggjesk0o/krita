// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// liquify_brush.dart — Liquify brush types: Push, Pinch, Comb.
//
// Per docs/liquify_editandapply.txt:
//
//   - **Push**  — "Distorts naturally, as if pushing or pulling with a
//     finger. Useful in most situations."
//   - **Pinch** — "Distorts sharply and precisely, as if pinching.
//     Useful for extending or shrinking specific curves."
//   - **Comb**  — "Gently smooths and aligns as if combing. Ideal for
//     straightening wavy curves. Works best when used with a rubbing
//     motion."
//
// Each brush is a stateless function that takes the previous position of
// a stroke point, the brush centre, the drag delta, the settings, and
// returns the new position. The [LiquifyEngine] iterates over the
// points of every selected stroke and calls the active brush on each.
//
// Falloff is shared (see [liquifyFalloff] in liquify_settings.dart);
// the difference between the brushes is *what* they do to a point inside
// the brush radius:
//
//   - Push:  move the point along the drag delta (natural finger-push).
//   - Pinch: move the point toward (or away from) the brush centre
//     (shrink/extend). The drag delta's magnitude scales the pinch.
//   - Comb:  move the point toward the projected position on the line
//     through the brush centre in the drag direction (alignment).

import 'package:vector_math/vector_math_64.dart';

import 'package:feather_krita/engine/liquify/liquify_settings.dart';

/// The three Feather liquify brush types.
enum LiquifyBrushType {
  /// Natural push / pull along the drag direction.
  push,

  /// Sharp pinch toward (or extend away from) the brush centre.
  pinch,

  /// Smooth align toward the drag axis (combing).
  comb,
}

/// Abstract base class for liquify brushes.
abstract class LiquifyBrush {
  const LiquifyBrush(this.type);
  final LiquifyBrushType type;

  /// Returns the displacement to apply to a point at [point] given the
  /// brush centre [centre], the drag delta [drag], and the [settings].
  ///
  /// The returned vector is the *displacement* (not the new position):
  /// `newPosition = point + brush.displacement(...)`.
  Vector3 displacement({
    required Vector3 point,
    required Vector3 centre,
    required Vector3 drag,
    required LiquifySettings settings,
  });
}

/// Push brush: moves points along the drag delta, weighted by the
/// falloff so the centre moves the full amount and the edge moves
/// nothing. Per the doc: "as if pushing or pulling with a finger."
class PushBrush extends LiquifyBrush {
  const PushBrush() : super(LiquifyBrushType.push);

  @override
  Vector3 displacement({
    required Vector3 point,
    required Vector3 centre,
    required Vector3 drag,
    required LiquifySettings settings,
  }) {
    final d = (point - centre).length;
    final w = liquifyFalloff(settings, d) * settings.strength;
    return drag * w;
  }
}

/// Pinch brush: moves points toward (or away from) the brush centre
/// along the radial direction. Positive drag magnitude pinches inward
/// (shrink); negative drag magnitude expands outward (extend). Per the
/// doc: "Useful for extending or shrinking specific curves."
class PinchBrush extends LiquifyBrush {
  const PinchBrush() : super(LiquifyBrushType.pinch);

  @override
  Vector3 displacement({
    required Vector3 point,
    required Vector3 centre,
    required Vector3 drag,
    required LiquifySettings settings,
  }) {
    final delta = point - centre;
    final dist = delta.length;
    if (dist < 1e-6) return Vector3.zero();
    final w = liquifyFalloff(settings, dist) * settings.strength;
    // Drag magnitude drives the pinch distance.
    final pinchDist = drag.length * w;
    // Direction: toward the centre for positive pinch, away for
    // negative (we use the sign of the drag projected on the radial
    // direction so the user can push outward by dragging away from
    // the centre).
    final radial = delta / dist;
    final sign = drag.dot(radial) >= 0 ? -1.0 : 1.0;
    return radial * (sign * pinchDist);
  }
}

/// Comb brush: moves points toward their projection on the line through
/// the brush centre in the drag direction. Repeated drags along the
/// same direction gradually straighten wavy curves — the "combing"
/// motion the doc describes. Per the doc: "Works best when used with a
/// rubbing motion."
class CombBrush extends LiquifyBrush {
  const CombBrush() : super(LiquifyBrushType.comb);

  @override
  Vector3 displacement({
    required Vector3 point,
    required Vector3 centre,
    required Vector3 drag,
    required LiquifySettings settings,
  }) {
    final dragLen = drag.length;
    if (dragLen < 1e-6) return Vector3.zero();
    final dir = drag / dragLen;
    final delta = point - centre;
    final d = delta.length;
    final w = liquifyFalloff(settings, d) * settings.strength;
    // Project the point onto the line `centre + t * dir`.
    final t = delta.dot(dir);
    final projected = centre + dir * t;
    // Lerp the point toward its projection by `w * dragLen`.
    final toward = (projected - point) * w;
    // Cap the displacement so a single comb stroke can't yank a point
    // past its projection (which would invert the local ordering).
    final maxStep = (projected - point).length;
    if (toward.length > maxStep) {
      return (projected - point);
    }
    return toward;
  }
}

/// Factory: returns the singleton brush instance for [type].
const Map<LiquifyBrushType, LiquifyBrush> _kBrushes = {
  LiquifyBrushType.push: PushBrush(),
  LiquifyBrushType.pinch: PinchBrush(),
  LiquifyBrushType.comb: CombBrush(),
};

/// Returns the brush instance for [type].
LiquifyBrush liquifyBrushFor(LiquifyBrushType type) {
  return _kBrushes[type] ?? const PushBrush();
}
