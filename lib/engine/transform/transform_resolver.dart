// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// transform_resolver.dart — Base interface for joystick transform resolvers.
//
// A "resolver" is the bridge between raw pointer input (screen-space deltas
// from a joystick handle drag) and a concrete 3D transform (translation,
// rotation, scale) that can be applied to a [Stroke] or any other scene
// object.
//
// The Feather transform tool has two distinct resolvers:
//   - [Joystick2dResolver] — view-based. "It transforms as it appears."
//   - [Joystick3dResolver] — world-axis-based. Red = X, green = Y, blue = Z.
//
// Both implement the same [TransformResolver] interface so the rest of the
// engine (selection system, gizmo renderer, undo journal) can treat them
// uniformly: feed them pointer deltas, get back a [TransformDelta], and
// apply it to the active selection. The actual *application* (matrix
// multiplication against a [Stroke]'s local transform) lives in
// `lib/models/stroke.dart` (`Stroke.applyTranslation`, `applyRotation`,
// `applyScale`).

import 'package:vector_math/vector_math_64.dart';

import 'package:feather_krita/engine/transform/transform_mode.dart';

/// A discrete piece of transform output produced by a resolver.
///
/// Each field is *optional* in the sense that a single drag usually only
/// drives one of them at a time — e.g. a 2D-joystick move drag populates
/// [translation] only, a rotate drag populates [rotation] only. The
/// resolver returns a fresh [TransformDelta] per pointer event so the
/// caller can either accumulate them or apply them incrementally.
class TransformDelta {
  const TransformDelta({
    this.translation,
    this.rotation,
    this.scale,
    this.pivot,
  });

  /// World-space translation to apply. `null` means "no translation this
  /// event".
  final Vector3? translation;

  /// World-space rotation (as a quaternion) to apply around [pivot].
  final Quaternion? rotation;

  /// Per-axis scale factors. Free scale → all components equal; width
  /// scale → only X differs; height scale → only Y differs; uniform
  /// (locked) scale → all components equal.
  final Vector3? scale;

  /// Pivot point for [rotation] and [scale]. For the 2D joystick this is
  /// the screen crosshair projected to the selection centre; for the 3D
  /// joystick it is the invisible centre of the selected object.
  final Vector3? pivot;

  /// Identity (no-op) delta.
  static const TransformDelta zero = TransformDelta();

  /// True if this delta carries no transform.
  bool get isZero =>
      translation == null && rotation == null && scale == null;

  /// Returns a new [TransformDelta] that is the composition of `this`
  /// followed by [other]. Pivots are taken from `this` when present.
  TransformDelta compose(TransformDelta other) {
    final t = translation == null
        ? other.translation
        : (other.translation == null
            ? translation
            : translation! + other.translation!);
    Quaternion? r;
    if (rotation != null && other.rotation != null) {
      r = other.rotation! * rotation!;
    } else {
      r = other.rotation ?? rotation;
    }
    Vector3? s;
    if (scale != null && other.scale != null) {
      s = Vector3(
        scale!.x * other.scale!.x,
        scale!.y * other.scale!.y,
        scale!.z * other.scale!.z,
      );
    } else {
      s = other.scale ?? scale;
    }
    final p = pivot ?? other.pivot;
    return TransformDelta(
      translation: t,
      rotation: r,
      scale: s,
      pivot: p,
    );
  }
}

/// View-frame passed to every resolver call.
///
/// The 2D joystick needs the camera's right / up / forward directions to
/// map screen-space drags into world-space translations and rotations. The
/// 3D joystick needs them only to decide which cones are usable in perfect
/// views (front / side / top — only two of the three cones are reachable).
class ViewFrame {
  const ViewFrame({
    required this.right,
    required this.up,
    required this.forward,
    required this.viewportWidth,
    required this.viewportHeight,
    this.crosshairWorld,
  });

  /// Camera right direction (normalised).
  final Vector3 right;

  /// Camera up direction (normalised).
  final Vector3 up;

  /// Camera forward / view normal (normalised, points into the scene).
  final Vector3 forward;

  /// Viewport size in pixels.
  final double viewportWidth;
  final double viewportHeight;

  /// World-space point under the screen crosshair (selection centre
  /// projected onto the screen). `null` means "use the selection centre".
  final Vector3? crosshairWorld;

  /// World units per screen pixel at the crosshair depth. Subclasses set
  /// this from the camera's fov + distance.
  double get worldPerPixel => 1.0;
}

/// Base interface for all joystick resolvers.
///
/// Concrete resolvers are stateless value transformers: given the active
/// mode + lock state + a ViewFrame + a normalised pointer delta in
/// [-1, 1]², they emit a [TransformDelta]. The caller is responsible for
/// accumulating deltas across pointer events and applying them.
abstract class TransformResolver {
  /// The kind of joystick this resolver serves.
  JoystickKind get kind;

  /// Resolves a single pointer delta into a [TransformDelta].
  ///
  /// [mode] is the active sub-mode (cast to the appropriate enum by the
  /// subclass). [lock] is only meaningful for the 2D joystick. [delta] is
  /// the normalised pointer movement in [-1, 1]² where (1, 0) is "full
  /// right" and (0, 1) is "full up". [frame] provides the camera basis.
  TransformDelta resolve({
    required Object mode,
    JoystickLock lock,
    required Vector2 delta,
    required ViewFrame frame,
  });
}
