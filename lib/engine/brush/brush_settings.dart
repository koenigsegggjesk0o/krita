// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// brush_settings.dart — tunable parameters of a Feather brush.
//
// Docs (brushes_interface.txt):
//   - Brush Size Slider: 1mm..300mm.
//   - Opacity Slider: 0%..100%.
//   - Pressure Sensitivity: toggle; when on, size/opacity/color respond
//     to pen pressure.
//   - Spacing / hardness / flow come from the Krita paintop model (the
//     .kpp loader surfaces them) — this file is the Feather-side mirror.
//
// Pure data: no Flutter, no FFI. Serializes to JSON for the .feather
// project format and the brush preset system.

/// Which brush property a pressure signal modulates.
enum PressureTarget {
  /// Pressure modulates brush size.
  size,

  /// Pressure modulates opacity.
  opacity,

  /// Pressure modulates color value (darker on light press).
  color,
}

/// Tunable brush parameters. All scalars are in user-facing units (mm
/// for [size], 0..1 fractions for the rest) — the renderer converts to
/// scene units at draw time.
class BrushSettings {
  const BrushSettings({
    this.size = 8.0,
    this.opacity = 1.0,
    this.flow = 1.0,
    this.spacing = 0.15,
    this.hardness = 0.8,
    this.smoothing = 0.3,
    this.pressureEnabled = true,
    this.pressureTarget = PressureTarget.size,
    this.minSize = 1.0,
  });

  /// Brush size in mm (docs: 1..300).
  final double size;

  /// Opacity in [0, 1] (docs: 0%..100%).
  final double opacity;

  /// Per-dab application rate in [0, 1] (Krita FlowValue).
  final double flow;

  /// Dab spacing as a fraction of brush radius in [0, 5].
  final double spacing;

  /// Edge hardness in [0, 1] (1 = crisp, 0 = soft). Only applies to
  /// auto-brush paintops (see BrushPreset.supportsHardness).
  final double hardness;

  /// Stroke smoothing in [0, 1] (0 = raw input, 1 = heavy stabilization).
  final double smoothing;

  /// Whether pressure sensitivity is enabled (docs' toggle).
  final bool pressureEnabled;

  /// Which property pressure modulates when [pressureEnabled].
  final PressureTarget pressureTarget;

  /// Minimum size (mm) a pressure-modulated brush drops to at zero
  /// pressure. Clamps the size ramp so a light touch never vanishes.
  final double minSize;

  /// Minimum brush size in mm (docs' slider floor).
  static const double kMinSizeMm = 1.0;

  /// Maximum brush size in mm (docs' slider ceiling).
  static const double kMaxSizeMm = 300.0;

  BrushSettings copyWith({
    double? size,
    double? opacity,
    double? flow,
    double? spacing,
    double? hardness,
    double? smoothing,
    bool? pressureEnabled,
    PressureTarget? pressureTarget,
    double? minSize,
  }) =>
      BrushSettings(
        size: size ?? this.size,
        opacity: opacity ?? this.opacity,
        flow: flow ?? this.flow,
        spacing: spacing ?? this.spacing,
        hardness: hardness ?? this.hardness,
        smoothing: smoothing ?? this.smoothing,
        pressureEnabled: pressureEnabled ?? this.pressureEnabled,
        pressureTarget: pressureTarget ?? this.pressureTarget,
        minSize: minSize ?? this.minSize,
      );

  /// Resolves the effective size at a given [pressure] (0..1), applying
  /// the pressure ramp when enabled.
  double sizeAtPressure(double pressure) {
    if (!pressureEnabled || pressureTarget != PressureTarget.size) {
      return size;
    }
    final p = pressure.clamp(0.0, 1.0);
    return minSize + (size - minSize) * p;
  }

  /// Resolves the effective opacity at a given [pressure] (0..1).
  double opacityAtPressure(double pressure) {
    if (!pressureEnabled || pressureTarget != PressureTarget.opacity) {
      return opacity;
    }
    return opacity * pressure.clamp(0.0, 1.0);
  }

  Map<String, dynamic> toJson() => {
        'size': size,
        'opacity': opacity,
        'flow': flow,
        'spacing': spacing,
        'hardness': hardness,
        'smoothing': smoothing,
        'pressureEnabled': pressureEnabled,
        'pressureTarget': pressureTarget.name,
        'minSize': minSize,
      };

  factory BrushSettings.fromJson(Map<String, dynamic> json) => BrushSettings(
        size: (json['size'] as num?)?.toDouble() ?? 8.0,
        opacity: (json['opacity'] as num?)?.toDouble() ?? 1.0,
        flow: (json['flow'] as num?)?.toDouble() ?? 1.0,
        spacing: (json['spacing'] as num?)?.toDouble() ?? 0.15,
        hardness: (json['hardness'] as num?)?.toDouble() ?? 0.8,
        smoothing: (json['smoothing'] as num?)?.toDouble() ?? 0.3,
        pressureEnabled: json['pressureEnabled'] as bool? ?? true,
        pressureTarget: PressureTarget.values.firstWhere(
          (t) => t.name == (json['pressureTarget'] as String? ?? 'size'),
          orElse: () => PressureTarget.size,
        ),
        minSize: (json['minSize'] as num?)?.toDouble() ?? 1.0,
      );

  @override
  String toString() =>
      'BrushSettings(size=$size, opacity=$opacity, flow=$flow, '
      'spacing=$spacing, hardness=$hardness, smoothing=$smoothing, '
      'pressure=$pressureEnabled/$pressureTarget)';
}
