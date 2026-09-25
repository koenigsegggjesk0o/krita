// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// mirror_assist.dart — the Mirror drawing assistance.
//
// Implements the symmetric-drawing tool documented in
// `assistance/mirror.txt`. The user toggles one or more of the three
// world axes (X = red, Y = green, Z = blue); every stroke is then
// duplicated across each active mirror plane. With N active axes the
// assist produces 2^N copies of the input (the identity plus every
// combination of axis reflections).
//
// The assist is pure data + geometry: given a list of stroke points it
// returns the list of reflected copies (each a fresh point list). The
// stroke manager registers each copy as its own curve in the active
// group, so mirror-drawn strokes remain independently editable.
//
// Depends on: lib/core/math/

import 'package:feather_krita/core/math/math.dart';

/// The three mirror axes Feather exposes (colour-coded in the UI).
enum MirrorAxis {
  /// X axis — red. Reflects across the YZ plane (negate X).
  x,

  /// Y axis — green. Reflects across the XZ plane (negate Y).
  y,

  /// Z axis — blue. Reflects across the XY plane (negate Z).
  z,
}

extension MirrorAxisX on MirrorAxis {
  /// The ARGB tint used by the Tool Menu swatch (per the docs: red /
  /// green / blue).
  int get argb => switch (this) {
        MirrorAxis.x => 0xffff3b30,
        MirrorAxis.y => 0xff34c759,
        MirrorAxis.z => 0xff0a84ff,
      };

  /// The reflection matrix that flips this axis.
  Matrix4 get reflection {
    final m = Matrix4.identity();
    switch (this) {
      case MirrorAxis.x:
        m[0] = -1.0; // column-major index 0 = entry(0,0)
        break;
      case MirrorAxis.y:
        m[5] = -1.0; // index 5 = entry(1,1)
        break;
      case MirrorAxis.z:
        m[10] = -1.0; // index 10 = entry(2,2)
        break;
    }
    return m;
  }
}

/// Configuration for the mirror assistance.
class MirrorAssist {
  MirrorAssist({
    this.activeAxes = const {},
    this.origin,
  });

  /// The set of active mirror axes. Empty = mirror off.
  Set<MirrorAxis> activeAxes;

  /// Optional world-space origin for the mirror planes (default = world
  /// origin).
  Vector3? origin;

  bool get isEnabled => activeAxes.isNotEmpty;

  /// Toggle an axis. Returns the new active set.
  Set<MirrorAxis> toggle(MirrorAxis axis) {
    if (activeAxes.contains(axis)) {
      activeAxes = activeAxes.where((a) => a != axis).toSet();
    } else {
      activeAxes = {...activeAxes, axis};
    }
    return activeAxes;
  }

  void enable(MirrorAxis axis) => activeAxes = {...activeAxes, axis};
  void disable(MirrorAxis axis) =>
      activeAxes = activeAxes.where((a) => a != axis).toSet();
  void clear() => activeAxes = {};

  /// The list of 2^N reflection transforms to apply (including the
  /// identity). Each is a world-space matrix the stroke manager uses to
  /// transform the source points into a mirrored copy.
  List<Matrix4> reflectionTransforms() {
    final axes = activeAxes.toList()..sort((a, b) => a.index.compareTo(b.index));
    final count = 1 << axes.length; // 2^N
    final result = <Matrix4>[];
    for (var mask = 0; mask < count; mask++) {
      var m = Matrix4.identity();
      for (var i = 0; i < axes.length; i++) {
        if ((mask & (1 << i)) != 0) {
          m = axes[i].reflection * m;
        }
      }
      // Translate to origin, reflect, translate back.
      if (origin != null) {
        final o = origin!;
        final toOrigin = Matrix4.identity()..setTranslation(-o);
        final fromOrigin = Matrix4.identity()..setTranslation(o);
        m = fromOrigin * m * toOrigin;
      }
      result.add(m);
    }
    return result;
  }

  /// Apply the mirror to a list of world-space points. Returns one list
  /// per reflection transform (the first is always the unchanged input
  /// when the identity is in the set — i.e. when at least one axis is
  /// active the source itself is returned as the first copy).
  List<List<Vector3>> reflectPoints(List<Vector3> points) {
    if (!isEnabled) return [points];
    final transforms = reflectionTransforms();
    return transforms
        .map((m) => points.map((p) => m.transformed3(p.clone())).toList())
        .toList();
  }

  /// Apply the mirror to a single point — returns the list of mirrored
  /// positions (length = 2^N, including the input position).
  List<Vector3> reflectPoint(Vector3 p) {
    if (!isEnabled) return [p.clone()];
    final transforms = reflectionTransforms();
    return transforms.map((m) => m.transformed3(p.clone())).toList();
  }

  /// Theoretical copy count for the current axis set (2^N).
  int get copyCount => 1 << activeAxes.length;

  Map<String, dynamic> toJson() => {
        'axes': activeAxes.map((a) => a.name).toList(),
        'origin': origin == null
            ? null
            : [origin!.x, origin!.y, origin!.z],
      };

  factory MirrorAssist.fromJson(Map<String, dynamic> json) {
    final axes = <MirrorAxis>{};
    for (final name in (json['axes'] as List? ?? [])) {
      final a = MirrorAxis.values.firstWhere(
        (m) => m.name == name,
        orElse: () => MirrorAxis.x,
      );
      axes.add(a);
    }
    final o = json['origin'] as List?;
    return MirrorAssist(
      activeAxes: axes,
      origin: o == null
          ? null
          : Vector3(
              (o[0] as num).toDouble(),
              (o[1] as num).toDouble(),
              (o[2] as num).toDouble(),
            ),
    );
  }

  MirrorAssist copy() => MirrorAssist.fromJson(toJson());
}

/// A small live-streaming mirror used while drawing — reflects incoming
/// stroke samples in real time so the artist sees all copies grow
/// simultaneously.
class LiveMirror {
  LiveMirror(this.assist);

  final MirrorAssist assist;
  late final List<Matrix4> _transforms = assist.reflectionTransforms();

  /// Refresh the cached transforms (call when the axis set changes).
  void refresh() {
    _transforms
      ..clear()
      ..addAll(assist.reflectionTransforms());
  }

  /// Reflect a fresh stroke sample into every mirror copy. The returned
  /// list has one entry per copy (including the source), in the same
  /// order as [MirrorAssist.reflectionTransforms].
  List<Vector3> sample(Vector3 point) {
    return _transforms
        .map((m) => m.transformed3(point.clone()))
        .toList(growable: false);
  }

  /// Number of live copies.
  int get copyCount => _transforms.length;
}
