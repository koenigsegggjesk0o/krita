// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// brush_state.dart — Pure Riverpod state for the active brush.
//
// Holds the live brush configuration the editor applies to new strokes:
//   - preset: which Krita brush preset is loaded (paintop id + settings).
//   - color: ARGB packed primary color.
//   - size: brush radius in texture pixels (at pressure = 1.0).
//   - opacity: per-dab application rate in [0, 1].
//   - spacing: distance between dabs as a fraction of the radius.
//   - material: which surface property the stroke drives (albedo / bump /
//     metallic / roughness / emissive).
//   - pattern: optional tiled image used to modulate the dab.
//
// Pure value type — emits through a Riverpod [Notifier]. The legacy
// [EditorState] keeps the parallel live configuration it pushes into the
// native brush engine; this file is the data slice the chrome widgets
// (color picker, brush slider, material dropdown) subscribe to.

import 'package:feather_krita/models/brush_preset.dart' show BrushPreset;

/// Which surface property the brush paints into. Matches the channels
/// the texture painter exposes (see TexturePainter.fill / paintDab).
enum BrushMaterial {
  /// Base color / albedo channel (default).
  albedo,

  /// Bump / normal channel.
  bump,

  /// Metallic channel (PBR).
  metallic,

  /// Roughness channel (PBR).
  roughness,

  /// Emissive channel.
  emissive,
}

/// Optional tiled pattern used to modulate the dab's alpha or color.
class BrushPattern {
  const BrushPattern({
    required this.id,
    required this.name,
    this.opacity = 1.0,
    this.scale = 1.0,
    this.rotation = 0.0,
  });

  /// Stable identifier (file name without extension).
  final String id;

  /// Display name.
  final String name;

  /// Pattern alpha multiplier in [0, 1].
  final double opacity;

  /// Pattern UV scale (> 1 = tile tighter).
  final double scale;

  /// Pattern rotation in radians.
  final double rotation;

  BrushPattern copyWith({
    double? opacity,
    double? scale,
    double? rotation,
  }) =>
      BrushPattern(
        id: id,
        name: name,
        opacity: opacity ?? this.opacity,
        scale: scale ?? this.scale,
        rotation: rotation ?? this.rotation,
      );

  @override
  bool operator ==(Object other) =>
      identical(this, other) ||
      other is BrushPattern &&
          other.id == id &&
          other.opacity == opacity &&
          other.scale == scale &&
          other.rotation == rotation;

  @override
  int get hashCode => Object.hash(id, opacity, scale, rotation);
}

/// Immutable brush state.
class BrushState {
  const BrushState({
    this.preset,
    this.presetName = 'Basic Round',
    this.color = 0xFF1A1A1A,
    this.size = 32.0,
    this.opacity = 1.0,
    this.spacing = 0.10,
    this.hardness = 0.8,
    this.flow = 1.0,
    this.material = BrushMaterial.albedo,
    this.pattern,
  });

  /// Loaded Krita preset, when one is loaded.
  final BrushPreset? preset;

  /// Display name of the loaded preset (or a fallback).
  final String presetName;

  /// ARGB packed primary color (0xAARRGGBB).
  final int color;

  /// Brush radius in texture pixels at full pressure.
  final double size;

  /// Per-dab opacity in [0, 1].
  final double opacity;

  /// Inter-dab spacing as a fraction of the radius.
  final double spacing;

  /// Brush edge hardness in [0, 1] (0 = softest, 1 = hardest).
  final double hardness;

  /// Per-dab flow in [0, 1] (Krita "FlowValue" sensor base).
  final double flow;

  /// Target surface material channel.
  final BrushMaterial material;

  /// Optional tiled pattern (null = solid color).
  final BrushPattern? pattern;

  BrushState copyWith({
    BrushPreset? preset,
    String? presetName,
    int? color,
    double? size,
    double? opacity,
    double? spacing,
    double? hardness,
    double? flow,
    BrushMaterial? material,
    BrushPattern? pattern,
    bool clearPattern = false,
  }) =>
      BrushState(
        preset: preset ?? this.preset,
        presetName: presetName ?? this.presetName,
        color: color ?? this.color,
        size: size ?? this.size,
        opacity: opacity ?? this.opacity,
        spacing: spacing ?? this.spacing,
        hardness: hardness ?? this.hardness,
        flow: flow ?? this.flow,
        material: material ?? this.material,
        pattern: clearPattern ? null : (pattern ?? this.pattern),
      );

  /// Extracts the RGB channels as a 3-tuple.
  ({int r, int g, int b}) get rgb => (
        r: (color >> 16) & 0xff,
        g: (color >> 8) & 0xff,
        b: color & 0xff,
      );

  /// Alpha channel in [0, 255].
  int get alpha => (color >> 24) & 0xff;

  Map<String, dynamic> toJson() => {
        'presetName': presetName,
        'color': color,
        'size': size,
        'opacity': opacity,
        'spacing': spacing,
        'hardness': hardness,
        'flow': flow,
        'material': material.name,
        if (pattern != null) 'pattern': pattern!.id,
      };

  @override
  bool operator ==(Object other) =>
      identical(this, other) ||
      other is BrushState &&
          other.presetName == presetName &&
          other.color == color &&
          other.size == size &&
          other.opacity == opacity &&
          other.spacing == spacing &&
          other.hardness == hardness &&
          other.flow == flow &&
          other.material == material &&
          other.pattern == pattern;

  @override
  int get hashCode => Object.hash(
        presetName,
        color,
        size,
        opacity,
        spacing,
        hardness,
        flow,
        material,
        pattern,
      );
}
