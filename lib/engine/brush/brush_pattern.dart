// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// brush_pattern.dart — brush-facing facade over the pattern generator.
//
// The procedural patterns themselves live in
// lib/engine/material/pattern_generator.dart. This module ties a pattern
// to a brush: it scales the pattern's UV to the brush size (so a 30 mm
// brush shows fewer, larger pattern cells than a 5 mm brush — the
// pattern "rides" the stroke at a consistent world scale), and it
// samples coverage along a stroke parameter [t] for ribbon rendering.
//
// The renderer calls [sampleStroke] per ribbon segment to decide how
// strongly the pattern modulates the segment's color.

import 'package:feather_krita/engine/brush/brush_settings.dart';
import 'package:feather_krita/engine/material/pattern_generator.dart';

/// Brush-level pattern sampler. Wraps a [PatternGenerator] and scales
/// its UV to the brush's world-space size.
class BrushPattern {
  BrushPattern({
    required this.settings,
    required this.brushSize,
    this.aspect = 1.0,
  }) : generator = PatternGenerator(settings);

  /// The pattern description (type, scale, angle, intensity, contrast).
  final PatternSettings settings;

  /// Brush size in mm — the world-space reference for UV tiling.
  final double brushSize;

  /// Aspect ratio of the ribbon's U vs V tiling (1 = square cells).
  final double aspect;

  final PatternGenerator generator;

  /// Samples the pattern at ribbon coordinates [uAlong] (distance along
  /// the stroke, in mm) and [vAcross] (signed offset across the ribbon,
  /// in mm). Returns coverage in [0, 1] AFTER intensity/contrast shaping.
  double sampleStroke(double uAlong, double vAcross) {
    // Convert mm → pattern UV by dividing by the brush size, so a cell
    // is roughly one brush-width wide regardless of the brush diameter.
    final u = uAlong / (brushSize * settings.scale * 0.125 + 1e-9);
    final v = vAcross / (brushSize * settings.scale * 0.125 * aspect + 1e-9);
    return generator.sample(u, v);
  }

  /// Convenience: samples the pattern at a normalized stroke parameter
  /// [t] (0..1 along the whole stroke) for a ribbon of [lengthMm]. The
  /// cross-ribbon coordinate is taken at the centerline (0).
  double sampleAtT(double t, double lengthMm) =>
      sampleStroke(t.clamp(0.0, 1.0) * lengthMm, 0.0);

  /// Rasterizes a preview tile (RGBA8) for the brush preset panel.
  /// Delegates to [PatternGenerator.generateTile] at a fixed 64 px.
  List<int> previewTile({int size = 64, int argb = 0xFFFFFFFF}) =>
      generator.generateTile(size, argb: argb);

  /// Samples the pattern across the ribbon's cross-section at a fixed
  /// arc-length [uAlong]. [vAcross] is the signed offset in mm from the
  /// centerline (−brushSize/2 .. +brushSize/2). Returns coverage 0..1.
  double sampleCrossSection(double uAlong, double vAcross) =>
      sampleStroke(uAlong, vAcross);

  /// Returns a copy of this pattern with the brush-relative scale
  /// rotated by [degrees] (delegates to [PatternSettings.copyWith] on
  /// the pattern's angle). The brush size is preserved.
  BrushPattern rotated(double degrees) => BrushPattern(
        settings: settings.copyWith(angle: settings.angle + degrees),
        brushSize: brushSize,
        aspect: aspect,
      );

  /// Returns a copy with a new brush size (e.g. when the brush size
  /// slider changes while a pattern is bound).
  BrushPattern withBrushSize(double mm) =>
      BrushPattern(settings: settings, brushSize: mm, aspect: aspect);

  /// Whether the pattern is effectively invisible (intensity == 0).
  bool get isInactive => settings.intensity <= 0.0;

  /// Returns a list of [samples] coverage values along the stroke from
  /// t=0..1 over [lengthMm]. Used by the renderer to pre-compute a
  /// coverage curve for a whole ribbon in one pass.
  List<double> coverageAlong(double lengthMm, int samples) {
    final out = List<double>.filled(samples, 0.0);
    for (var i = 0; i < samples; i++) {
      final t = samples <= 1 ? 0.0 : i / (samples - 1);
      out[i] = sampleAtT(t, lengthMm);
    }
    return out;
  }

  @override
  String toString() =>
      'BrushPattern(${settings.type}, brushSize=$brushSize, '
      'intensity=${settings.intensity})';
}

/// Builds a [BrushPattern] from a [BrushSettings] + optional
/// [PatternSettings], or returns null when no pattern is set.
BrushPattern? brushPatternFor({
  required BrushSettings brush,
  PatternSettings? pattern,
}) {
  if (pattern == null) return null;
  return BrushPattern(settings: pattern, brushSize: brush.size);
}
