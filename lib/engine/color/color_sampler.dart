// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// color_sampler.dart — the eyedropper.
//
// Docs (brushes_color.txt):
//   - "Tap the Eyedropper on the right side of the Color Panel. When the
//     Eyedropper turns black, it's ready to sample colors."
//   - "You can sample new colors from the drawn curves or reference
//     images in the clipboard. The curve you want to sample from must
//     be in the currently Active Group."
//
// This module samples an ARGB color from two sources:
//   1. Drawn curves — finds the nearest stroke sample within a max
//      pick radius and returns that stroke's color. (Feather curves
//      carry one color each; the docs' eyedropper reads the curve
//      color under the pen.)
//   2. Reference images — bilinearly samples an RGBA8 buffer at a UV
//      coordinate, for imported blueprint/three-view images.
//
// Pure Dart; operates on the existing [Stroke] model (vector_math) and
// raw pixel buffers so it runs in unit tests without a canvas.

import 'dart:typed_data';

import 'package:vector_math/vector_math_64.dart';

import 'package:feather_krita/models/stroke.dart';

/// Eyedropper — samples ARGB colors from curves or reference images.
class ColorSampler {
  ColorSampler({this.defaultPickRadius = 4.0});

  /// Default world-space pick radius (mm) for curve sampling when the
  /// caller doesn't specify one.
  final double defaultPickRadius;

  /// Samples the color of the nearest curve sample to [worldPos] in
  /// [strokes]. Returns null when no sample is within [maxDistance]
  /// (defaults to [defaultPickRadius]).
  int? sampleFromCurves(
    List<Stroke> strokes,
    Vector3 worldPos, {
    double? maxDistance,
  }) {
    final maxDist = maxDistance ?? defaultPickRadius;
    var bestDist2 = maxDist * maxDist;
    int? bestColor;
    for (final stroke in strokes) {
      if (!stroke.isVisible) continue;
      for (var i = 0; i < stroke.length; i++) {
        final d2 = stroke.worldPosition(i).distanceToSquared(worldPos);
        if (d2 <= bestDist2) {
          bestDist2 = d2;
          bestColor = stroke.color;
        }
      }
    }
    return bestColor;
  }

  /// Single-curve convenience: returns [stroke]'s color when any sample
  /// is within [maxDistance], else null.
  int? sampleFromStroke(
    Stroke stroke,
    Vector3 worldPos, {
    double? maxDistance,
  }) {
    if (!stroke.isVisible) return null;
    final maxDist = maxDistance ?? defaultPickRadius;
    final r2 = maxDist * maxDist;
    for (var i = 0; i < stroke.length; i++) {
      if (stroke.worldPosition(i).distanceToSquared(worldPos) <= r2) {
        return stroke.color;
      }
    }
    return null;
  }

  /// Bilinearly samples an RGBA8 reference image at UV [u], [v] (both in
  /// [0, 1], clamped). [rgba] is row-major, top-left origin, stride =
  /// width*4. Returns a packed ARGB int (alpha promoted to opaque when
  /// the buffer is RGB-only — pass [rgbOnly] true for 3-channel data).
  int sampleFromImage(
    Uint8List rgba,
    int width,
    int height,
    double u,
    double v, {
    bool rgbOnly = false,
  }) {
    if (width <= 0 || height <= 0 || rgba.isEmpty) return 0xFF000000;
    final channels = rgbOnly ? 3 : 4;
    final stride = width * channels;
    // Clamp UV to [0, 1] and map to pixel centers.
    final fu = u.clamp(0.0, 1.0) * (width - 1);
    final fv = v.clamp(0.0, 1.0) * (height - 1);
    final x0 = fu.floor();
    final y0 = fv.floor();
    final x1 = (x0 + 1).clamp(0, width - 1);
    final y1 = (y0 + 1).clamp(0, height - 1);
    final tx = fu - x0;
    final ty = fv - y0;

    int px(int x, int y) {
      final i = y * stride + x * channels;
      return i;
    }

    final i00 = px(x0, y0);
    final i10 = px(x1, y0);
    final i01 = px(x0, y1);
    final i11 = px(x1, y1);
    int ch(int i, int c) => i + c < rgba.length ? rgba[i + c] : 0;

    double bil(int c) {
      final a = ch(i00, c) * (1 - tx) + ch(i10, c) * tx;
      final b = ch(i01, c) * (1 - tx) + ch(i11, c) * tx;
      return a * (1 - ty) + b * ty;
    }

    final r = bil(0).round().clamp(0, 255);
    final g = bil(1).round().clamp(0, 255);
    final b = bil(2).round().clamp(0, 255);
    final a = rgbOnly ? 255 : bil(3).round().clamp(0, 255);
    return (a << 24) | (r << 16) | (g << 8) | b;
  }

  /// Nearest-neighbor pixel sample of an RGBA8 buffer at integer (x, y).
  int samplePixel(Uint8List rgba, int width, int x, int y,
      {bool rgbOnly = false}) {
    final channels = rgbOnly ? 3 : 4;
    final i = y * width * channels + x * channels;
    if (i + 2 >= rgba.length) return 0xFF000000;
    final r = rgba[i];
    final g = rgba[i + 1];
    final b = rgba[i + 2];
    final a = rgbOnly ? 255 : (i + 3 < rgba.length ? rgba[i + 3] : 255);
    return (a << 24) | (r << 16) | (g << 8) | b;
  }
}
