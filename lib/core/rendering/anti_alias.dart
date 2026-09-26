// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// SCAFFOLD — software rasterizer for a FUTURE high-quality headless
// render path. NOT YET WIRED to any live path (AUDIT_FINAL gap #4):
// the canvas viewport's CustomPainter handles live faux-3D tube
// rendering, and PNG export uses RepaintBoundary pixel capture. Wiring
// this would regress WYSIWYG unless the canvas also moves to mesh
// rendering. Kept as a forward-looking scaffold for a future
// WebGL/Impeller/headless-export backend; see worklog v54-B.
//
// anti_alias.dart — MSAA coverage + FXAA post-process.
//
// Two complementary anti-aliasing strategies:
//
//   1. MSAA (Multi-Sample Anti-Aliasing). The rasterizer evaluates
//      N samples per pixel (2, 4, or 8) and blends the surviving
//      samples into a single coverage-weighted color. This is the
//      highest-quality AA — it eliminates triangle-edge jaggies
//      without blurring the interior. The MSAA pattern is configured
//      via [Rasterizer.samplePattern]; this file just provides the
//      patterns and a coverage-resolve helper for the custom paths
//      that need it (FXAA input, screen-space shadows).
//
//   2. FXAA (Fast Approximate anti-aliasing). A screen-space post-
//      process that detects edge pixels by their luminance gradient
//      and blends them with their neighbors. Cheaper than MSAA (no
//      per-sample cost in the rasterizer) but slightly blurrier. Used
//      as a fallback when MSAA's per-pixel cost would drop the frame
//      rate below 30fps.
//
// FXAA's quality is "console quality" — good enough for the Feather-
// Krita 3D preview at 1080p. The implementation follows the FXAA 3.11
// reference (NVIDIA whitepaper) at the "low" preset, which is the
// sweet spot for mobile GPUs.

import 'dart:math' as math;
import 'dart:typed_data';

import 'framebuffer.dart';
import 'rasterizer.dart';

/// MSAA helpers. The actual MSAA sampling happens inside the rasterizer
/// (it uses [SamplePattern.x4] etc.); this class provides the resolve
/// step for off-screen MSAA targets.
class MsaaResolver {
  const MsaaResolver();

  /// Resolves an N-sample color buffer into a 1-sample framebuffer.
  ///
  /// [msaaColors] is a flat array of `width * height * sampleCount`
  /// ARGB ints, indexed as `(y * width + x) * sampleCount + s`.
  /// [msaaDepth] is the matching depth array.
  ///
  /// The resolve uses a box filter (average of surviving samples) —
  /// the standard for MSAA. Per-pixel depth is taken from the
  /// minimum surviving sample.
  void resolve(
    Uint32List msaaColors,
    Float64List msaaDepth,
    int width,
    int height,
    int sampleCount,
    FrameBuffer out,
  ) {
    final colorOut = out.rawColorStorage();
    final depthOut = out.depth.rawStorage();
    final n = width * height;
    for (var i = 0; i < n; i++) {
      var r = 0, g = 0, b = 0, a = 0;
      var count = 0;
      var minD = double.infinity;
      for (var s = 0; s < sampleCount; s++) {
        final idx = i * sampleCount + s;
        final c = msaaColors[idx];
        final d = msaaDepth[idx];
        if (d >= 1.0) continue; // far = no coverage
        r += (c >> 16) & 0xff;
        g += (c >> 8) & 0xff;
        b += c & 0xff;
        a += (c >> 24) & 0xff;
        count++;
        if (d < minD) minD = d;
      }
      if (count == 0) {
        colorOut[i] = 0;
        depthOut[i] = 1.0;
      } else {
        colorOut[i] = ((a ~/ count) << 24) |
            ((r ~/ count) << 16) |
            ((g ~/ count) << 8) |
            (b ~/ count);
        depthOut[i] = minD;
      }
    }
  }
}

/// FXAA 3.11 — "low" preset, ported to Dart.
///
/// Usage:
///   final fxaa = Fxaa();
///   await fxaa.apply(framebuffer);
///
/// The filter runs in place on the framebuffer's color buffer. It
/// reads each pixel + its 4-neighbourhood, detects edges by luminance
/// gradient, and blends along the edge direction.
class Fxaa {
  Fxaa({
    this.subpixelAA = 0.75,
    this.edgeThreshold = 0.166,
    this.edgeThresholdMin = 0.0833,
  });

  /// Sub-pixel blend amount (0 = off, 1 = full). 0.75 is the FXAA
  /// default.
  final double subpixelAA;

  /// Edge-detection threshold (luminance gradient above this is an
  /// edge). 0.166 is the "low" preset.
  final double edgeThreshold;

  /// Minimum luminance threshold — below this, no edge detection
  /// runs (avoids noise in dark areas).
  final double edgeThresholdMin;

  /// Runs FXAA on [fb]'s color buffer in place.
  void apply(FrameBuffer fb) {
    final w = fb.width;
    final h = fb.height;
    final src = fb.rawColorStorage();
    // Copy the source — we read from src, write to fb's storage.
    final copy = Uint32List.fromList(src);
    for (var y = 1; y < h - 1; y++) {
      for (var x = 1; x < w - 1; x++) {
        final idx = y * w + x;
        final c = copy[idx];
        final lumaCenter = _luma(c);
        final lumaDown = _luma(copy[(y + 1) * w + x]);
        final lumaUp = _luma(copy[(y - 1) * w + x]);
        final lumaLeft = _luma(copy[y * w + (x - 1)]);
        final lumaRight = _luma(copy[y * w + (x + 1)]);
        final lumaMin = math.min(
            lumaCenter,
            math.min(lumaDown,
                math.min(lumaUp, math.min(lumaLeft, lumaRight))));
        final lumaMax = math.max(
            lumaCenter,
            math.max(lumaDown,
                math.max(lumaUp, math.max(lumaLeft, lumaRight))));
        final range = lumaMax - lumaMin;
        if (range < math.max(edgeThresholdMin,
            lumaMax * edgeThreshold)) {
          continue;
        }
        // Sub-pixel blend: average the 4 neighbours.
        final lumaAvg = (lumaDown + lumaUp + lumaLeft + lumaRight) * 0.25;
        final subpixelOffset =
            ((lumaAvg - lumaCenter) / range).clamp(-1.0, 1.0).abs();
        final blend = subpixelOffset * subpixelAA;
        // Horizontal / vertical edge detection.
        final lumaDL = _luma(copy[(y + 1) * w + (x - 1)]);
        final lumaDR = _luma(copy[(y + 1) * w + (x + 1)]);
        final lumaUL = _luma(copy[(y - 1) * w + (x - 1)]);
        final lumaUR = _luma(copy[(y - 1) * w + (x + 1)]);
        final lumaD = (lumaDL + lumaDR) * 0.5;
        final lumaU = (lumaUL + lumaUR) * 0.5;
        final lumaL = (lumaDL + lumaUL) * 0.5;
        final lumaR = (lumaDR + lumaUR) * 0.5;
        final rangeH = (lumaL - lumaCenter).abs() +
            (lumaR - lumaCenter).abs() +
            (lumaL - lumaR).abs();
        final rangeV = (lumaD - lumaCenter).abs() +
            (lumaU - lumaCenter).abs() +
            (lumaD - lumaU).abs();
        final horiz = rangeH > rangeV;
        // Sample along the edge — single neighbor in the strongest
        // gradient direction.
        final sampleIdx = horiz
            ? (lumaL < lumaR ? y * w + (x - 1) : y * w + (x + 1))
            : (lumaU < lumaD ? (y - 1) * w + x : (y + 1) * w + x);
        final edgeColor = copy[sampleIdx];
        src[idx] = _lerpColor(c, edgeColor, blend * 0.5);
      }
    }
  }

  /// Rec. 709 luminance.
  static double _luma(int argb) {
    final r = (argb >> 16) & 0xff;
    final g = (argb >> 8) & 0xff;
    final b = argb & 0xff;
    return (0.2126 * r + 0.7152 * g + 0.0722 * b) / 255.0;
  }

  static int _lerpColor(int a, int b, double t) {
    if (t <= 0.0) return a;
    if (t >= 1.0) return b;
    final ar = (a >> 16) & 0xff;
    final ag = (a >> 8) & 0xff;
    final ab = a & 0xff;
    final aa = (a >> 24) & 0xff;
    final br = (b >> 16) & 0xff;
    final bg = (b >> 8) & 0xff;
    final bb = b & 0xff;
    final ba = (b >> 24) & 0xff;
    return ((aa + (ba - aa) * t).round() << 24) |
        ((ar + (br - ar) * t).round() << 16) |
        ((ag + (bg - ag) * t).round() << 8) |
        (ab + (bb - ab) * t).round();
  }
}
