// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// brush_preset.dart — Feather brush preset model.
//
// Docs (brushes_brushpreset.txt): "Save your brush size, color, and
// shape to create your own presets. Load them anytime to continue
// working quickly." The preset "saves the current brush type, color,
// size, and opacity."
//
// This is the FEATHER-side preset — a compact, user-authored snapshot of
// the brush panel's state (name, color, thickness, opacity, material
// type, pattern). It is distinct from the Krita .kpp loader
// (lib/models/brush_preset.dart), which imports Krita's native paintop
// preset XML. A Feather preset wraps a [BrushSettings] plus a
// [MaterialType] + [PatternSettings] so loading one tap restores the
// full draw state including the material response.

import 'package:feather_krita/engine/brush/brush_settings.dart';
import 'package:feather_krita/engine/material/material_type.dart';
import 'package:feather_krita/engine/material/pattern_generator.dart';

/// A user-authored Feather brush preset.
///
/// Stored per-note (docs: "Brush presets are saved per note") and
/// selected/deselected/deleted from the preset panel.
class FeatherBrushPreset {
  FeatherBrushPreset({
    required this.name,
    this.color = 0xFF000000,
    this.thickness = 8.0,
    this.opacity = 1.0,
    this.materialType = MaterialType.shadeless,
    this.pattern,
    BrushSettings? settings,
    this.favorite = false,
  }) : settings = settings ?? BrushSettings(size: thickness, opacity: opacity);

  /// Display name (user-editable).
  String name;

  /// Brush color as ARGB int (0xAARRGGBB).
  int color;

  /// Brush thickness in mm — the docs' "size" snapshot. Mirrored into
  /// [settings.size] so the renderer reads one source of truth.
  double thickness;

  /// Brush opacity (0..1) — the docs' "opacity" snapshot. Mirrored into
  /// [settings.opacity].
  double opacity;

  /// Material type this preset paints with.
  MaterialType materialType;

  /// Optional pattern (only honored for Shadeless/Shaded — see
  /// [materialTypeSupportsPattern]).
  PatternSettings? pattern;

  /// Full brush settings (size, opacity, flow, spacing, hardness,
  /// smoothing, pressure). [thickness]/[opacity] above are convenience
  /// mirrors kept in sync by [syncToSettings].
  BrushSettings settings;

  /// Whether the user starred this preset.
  bool favorite;

  /// Pushes [thickness] / [opacity] into [settings] so the renderer's
  /// single source of truth is current. Call after editing the snapshot
  /// fields.
  void syncToSettings() {
    settings = settings.copyWith(size: thickness, opacity: opacity);
  }

  /// Pulls [thickness] / [opacity] back from [settings] (e.g. after the
  /// user nudges the size slider while the preset is loaded).
  void syncFromSettings() {
    thickness = settings.size;
    opacity = settings.opacity;
  }

  /// Effective pattern, or null when the material type disallows one.
  PatternSettings? get effectivePattern =>
      materialTypeSupportsPattern(materialType) ? pattern : null;

  FeatherBrushPreset copy() => FeatherBrushPreset(
        name: name,
        color: color,
        thickness: thickness,
        opacity: opacity,
        materialType: materialType,
        pattern: pattern?.copyWith(),
        settings: settings.copyWith(),
        favorite: favorite,
      );

  Map<String, dynamic> toJson() => {
        'name': name,
        'color': color,
        'thickness': thickness,
        'opacity': opacity,
        'materialType': materialType.name,
        if (pattern != null) 'pattern': pattern!.toJson(),
        'settings': settings.toJson(),
        'favorite': favorite,
      };

  factory FeatherBrushPreset.fromJson(Map<String, dynamic> json) {
    final preset = FeatherBrushPreset(
      name: json['name'] as String? ?? 'Preset',
      color: (json['color'] as num?)?.toInt() ?? 0xFF000000,
      thickness: (json['thickness'] as num?)?.toDouble() ?? 8.0,
      opacity: (json['opacity'] as num?)?.toDouble() ?? 1.0,
      materialType:
          materialTypeFromName(json['materialType'] as String?),
      pattern: json['pattern'] is Map<String, dynamic>
          ? PatternSettings.fromJson(
              json['pattern'] as Map<String, dynamic>)
          : null,
      settings: json['settings'] is Map<String, dynamic>
          ? BrushSettings.fromJson(
              json['settings'] as Map<String, dynamic>)
          : null,
      favorite: json['favorite'] as bool? ?? false,
    );
    preset.syncToSettings();
    return preset;
  }

  @override
  String toString() =>
      'FeatherBrushPreset("$name", color=0x${color.toRadixString(16)}, '
      'th=$thickness, op=$opacity, mat=$materialType)';
}
