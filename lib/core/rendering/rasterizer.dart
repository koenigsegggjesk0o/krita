// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// Export-only: software rasterizer for high-quality PNG export.
// Not used in real-time (too slow for 30 fps); the canvas viewport's
// CustomPainter handles live rendering. This subsystem is reserved
// for the export pipeline (rasterize scene -> framebuffer -> PNG)
// and for future headless render paths.
//
// rasterizer.dart — Triangle rasterization with z-buffer + perspective-
// correct interpolation.
//
// The rasterizer is the hot loop of the software renderer. For every
// triangle it:
//
//   1. Computes the screen-space bounding box.
//   2. For every pixel in the bbox, evaluates the barycentric weights
//      of the triangle's three vertices.
//   3. Inside pixels get their depth interpolated linearly in screen
//      space (NDC Z is already post-perspective-divide — linear
//      interpolation is correct here, no perspective correction
//      needed).
//   4. Depth-test against the framebuffer's z-buffer. If the test
//      fails, the pixel is skipped.
//   5. Interpolate the per-vertex attributes (world position, normal,
//      UV, color) PERSPECTIVE-CORRECTLY — i.e. interpolate `attr / w`
//      in screen space, then multiply by the interpolated `1/w`.
//   6. Build a [Fragment], invoke [Material.shader.shade], then blend
//      the result onto the framebuffer via [Material.blendMode].
//
// The implementation uses an edge-function formulation (signed-area
// times two) so the inner loop is pure multiplies + adds with no
// divisions per pixel. The barycentric weights are recovered from the
// edge functions in O(1).

import 'dart:math' as math;

import 'package:vector_math/vector_math_64.dart';

import 'blend_mode.dart';
import 'depth_buffer.dart';
import 'fragment_shader.dart';
import 'framebuffer.dart';
import 'vertex_processor.dart';

/// A sample-pattern descriptor for MSAA. The default is a single sample
/// at the pixel center (no MSAA). 4x and 8x patterns are predefined.
class SamplePattern {
  const SamplePattern(this.samples);

  /// Sample positions in pixel units (origin = pixel center = (0.5, 0.5)).
  /// Length must be a power of two in [1, 16].
  final List<math.Point<double>> samples;

  /// 1x sampling (single sample at the pixel center).
  static const SamplePattern x1 = SamplePattern([
    math.Point(0.5, 0.5),
  ]);

  /// 2x MSAA — two diagonal samples.
  static const SamplePattern x2 = SamplePattern([
    math.Point(0.25, 0.25),
    math.Point(0.75, 0.75),
  ]);

  /// 4x MSAA — rotated grid.
  static const SamplePattern x4 = SamplePattern([
    math.Point(0.375, 0.125),
    math.Point(0.875, 0.375),
    math.Point(0.125, 0.625),
    math.Point(0.625, 0.875),
  ]);

  /// 8x MSAA — RotoDisc pattern (Direct3D 8x).
  static const SamplePattern x8 = SamplePattern([
    math.Point(0.5625, 0.3125),
    math.Point(0.4375, 0.6875),
    math.Point(0.8125, 0.5625),
    math.Point(0.1875, 0.4375),
    math.Point(0.3125, 0.8125),
    math.Point(0.6875, 0.1875),
    math.Point(0.9375, 0.9375),
    math.Point(0.0625, 0.0625),
  ]);

  /// Number of samples.
  int get count => samples.length;
}

/// The software rasterizer.
class Rasterizer {
  Rasterizer({
    this.samplePattern = SamplePattern.x1,
    this.depthBiasSlopeScale = 0.0,
    this.conservativeRaster = false,
  });

  /// MSAA sample pattern. Default is 1x (single sample at pixel center).
  final SamplePattern samplePattern;

  /// Slope-scaled depth bias — passed through to the depth buffer.
  final double depthBiasSlopeScale;

  /// If true, use conservative rasterization (any pixel whose center
  /// is inside the conservative bounding box of the triangle is
  /// rasterized, even if the barycentric weights would reject it).
  /// Useful for the picking pass and the cutout plate.
  final bool conservativeRaster;

  /// Scratch buffer for the fragment shader's `Fragment` — reused across
  /// pixels to avoid per-pixel allocation. NOT thread-safe; the
  /// rasterizer must not run concurrently on the same instance.
  final FragmentScratch _scratch = FragmentScratch();

  /// Rasterizes a single triangle into [fb]. [a], [b], [c] are the
  /// processed vertices. [material] is the surface description.
  /// [context] provides the lights / camera / sampler the shader needs.
  void rasterTriangle(
    ProcessedVertex a,
    ProcessedVertex b,
    ProcessedVertex c,
    Material material,
    ShaderContext context,
    FrameBuffer fb,
  ) {
    // Skip degenerate triangles.
    final area = _edgeFunctionV(a, b, c);
    if (area.abs() < 1e-9) return;
    final ccw = area < 0; // Negative because screen Y is flipped.
    // Backface cull (unless material is double-sided).
    if (!material.doubleSided && ccw) return;

    // Compute the screen-space bounding box, clamped to the framebuffer.
    final minX = (math.min(a.screen.x, math.min(b.screen.x, c.screen.x))
            .floor())
        .clamp(0, fb.width - 1);
    final maxX = (math.max(a.screen.x, math.max(b.screen.x, c.screen.x))
            .ceil())
        .clamp(0, fb.width - 1);
    final minY = (math.min(a.screen.y, math.min(b.screen.y, c.screen.y))
            .floor())
        .clamp(0, fb.height - 1);
    final maxY = (math.max(a.screen.y, math.max(b.screen.y, c.screen.y))
            .ceil())
        .clamp(0, fb.height - 1);
    if (maxX < minX || maxY < minY) return;

    // Reciprocals for barycentric recovery.
    final invArea = 1.0 / area;

    final depthStorage = fb.depth.rawStorage();
    final colorStorage = fb.rawColorStorage();
    final fbW = fb.width;

    final camPos = context.cameraPosition;

    // Screen-space depth gradient estimate — used for slope-scaled
    // depth bias (PolygonOffset-style). Computed as the max abs delta
    // between any two vertices' screen-Z. Cheap and good enough for
    // the bias term.
    final z0 = a.screen.z;
    final z1 = b.screen.z;
    final z2 = c.screen.z;
    final slope = (z0 - z1).abs() > (z1 - z2).abs()
        ? (z0 - z1).abs()
        : (z1 - z2).abs();

    // Per-pixel loop.
    for (var y = minY; y <= maxY; y++) {
      for (var x = minX; x <= maxX; x++) {
        // MSAA: track coverage and the surviving sample with the
        // MIN depth (front-most). For 1x, this is just one sample.
        double minDepth = double.infinity;
        int coverage = 0;
        double bx = 0, by = 0, bz = 0;
        for (var s = 0; s < samplePattern.count; s++) {
          final sp = samplePattern.samples[s];
          final px = x + sp.x;
          final py = y + sp.y;
          final w0 = _edgeFunction(b, c, px, py) * invArea;
          final w1 = _edgeFunction(c, a, px, py) * invArea;
          final w2 = _edgeFunction(a, b, px, py) * invArea;
          final inside = w0 >= 0 && w1 >= 0 && w2 >= 0;
          if (!inside && !conservativeRaster) continue;
          coverage++;
          // Interpolate depth (linear in screen space — correct
          // because NDC Z is post-divide).
          final d = a.screen.z * w0 +
              b.screen.z * w1 +
              c.screen.z * w2;
          if (d < minDepth) {
            minDepth = d;
            bx = w0;
            by = w1;
            bz = w2;
          }
        }
        if (coverage == 0) continue;

        // Depth test (with slope bias).
        final idx = y * fbW + x;
        final storedDepth = depthStorage[idx];
        final fragmentDepth = minDepth;
        if (material.depthTest &&
            !_depthTest(
                fragmentDepth, storedDepth, fb.depth, slope)) {
          continue;
        }
        // Depth-write (if enabled). We write the front-most sample's
        // depth.
        if (material.depthWrite && fb.depth.depthWrite) {
          depthStorage[idx] = fragmentDepth;
        }

        // Perspective-correct interpolation: interpolate `attr/w` and
        // multiply by interpolated `1/w`.
        final w0p = bx * a.invW;
        final w1p = by * b.invW;
        final w2p = bz * c.invW;
        final invWInterp = w0p + w1p + w2p;
        if (invWInterp == 0) continue;
        final invInvW = 1.0 / invWInterp;

        final wpX = (a.worldPosition.x * w0p +
                b.worldPosition.x * w1p +
                c.worldPosition.x * w2p) *
            invInvW;
        final wpY = (a.worldPosition.y * w0p +
                b.worldPosition.y * w1p +
                c.worldPosition.y * w2p) *
            invInvW;
        final wpZ = (a.worldPosition.z * w0p +
                b.worldPosition.z * w1p +
                c.worldPosition.z * w2p) *
            invInvW;
        final nX = (a.worldNormal.x * w0p +
                b.worldNormal.x * w1p +
                c.worldNormal.x * w2p) *
            invInvW;
        final nY = (a.worldNormal.y * w0p +
                b.worldNormal.y * w1p +
                c.worldNormal.y * w2p) *
            invInvW;
        final nZ = (a.worldNormal.z * w0p +
                b.worldNormal.z * w1p +
                c.worldNormal.z * w2p) *
            invInvW;
        final u = (a.uv.x * w0p + b.uv.x * w1p + c.uv.x * w2p) * invInvW;
        final v = (a.uv.y * w0p + b.uv.y * w1p + c.uv.y * w2p) * invInvW;
        final vertexColor = _lerpColor(a.color, b.color, c.color, bx, by, bz);

        // View direction.
        _scratch.viewDir
          ..setValues(camPos.x - wpX, camPos.y - wpY, camPos.z - wpZ)
          ..normalize();
        // Renormalize the interpolated normal.
        final nLen = math.sqrt(nX * nX + nY * nY + nZ * nZ);
        final nn = _scratch.worldNormal;
        if (nLen > 1e-9) {
          nn.setValues(nX / nLen, nY / nLen, nZ / nLen);
        } else {
          nn.setValues(0, 0, 1);
        }

        // Build the fragment (reused scratch — mutate in place).
        final frag = _scratch.fragment
          ..x = x.toDouble()
          ..y = y.toDouble()
          ..depth = fragmentDepth
          ..worldPosition.setValues(wpX, wpY, wpZ)
          ..worldNormal.setFrom(nn)
          ..uv.setValues(u, v)
          ..vertexColor = vertexColor
          ..viewDir.setFrom(_scratch.viewDir);

        // Shade.
        final src = material.shader.shade(frag, material, context);

        // Blend.
        final dst = colorStorage[idx];
        final coverageAlpha =
            coverage.toDouble() / samplePattern.count;
        final fa = ((src >> 24) & 0xff) * coverageAlpha *
            material.opacity;
        final blended = BlendFunctions.blend(
            material.blendMode, src, dst, fa);
        colorStorage[idx] = blended;
      }
    }
  }

  /// Edge function: signed area * 2 of the triangle (a, b, p).
  static double _edgeFunction(
      ProcessedVertex a, ProcessedVertex b, double px, double py) {
    return (b.screen.x - a.screen.x) * (py - a.screen.y) -
        (b.screen.y - a.screen.y) * (px - a.screen.x);
  }

  static double _edgeFunctionV(
      ProcessedVertex a, ProcessedVertex b, ProcessedVertex c) {
    return (b.screen.x - a.screen.x) * (c.screen.y - a.screen.y) -
        (b.screen.y - a.screen.y) * (c.screen.x - a.screen.x);
  }

  static bool _depthTest(double fragment, double stored,
      DepthBuffer buf, double slope) {
    // Inline the comparison for speed — saves the switch dispatch in
    // the hot loop. We only support the most common modes here; the
    // general case falls back to the buffer's method.
    switch (buf.compareFunc) {
      case DepthCompare.always:
        return true;
      case DepthCompare.never:
        return false;
      case DepthCompare.less:
        return fragment + buf.depthBias + slope * buf.slopeScaledBias <
            stored;
      case DepthCompare.lessEqual:
        return fragment + buf.depthBias + slope * buf.slopeScaledBias <=
            stored;
      case DepthCompare.equal:
        return (fragment - stored).abs() < 1e-6;
      case DepthCompare.greaterEqual:
        return fragment >= stored;
      case DepthCompare.greater:
        return fragment > stored;
      case DepthCompare.notEqual:
        return (fragment - stored).abs() >= 1e-6;
    }
  }

  static int _lerpColor(int a, int b, int c, double ba, double bb,
      double bc) {
    final ar = (a >> 16) & 0xff;
    final ag = (a >> 8) & 0xff;
    final ab = a & 0xff;
    final aa = (a >> 24) & 0xff;
    final br = (b >> 16) & 0xff;
    final bg = (b >> 8) & 0xff;
    final bb2 = b & 0xff;
    final ba2 = (b >> 24) & 0xff;
    final cr = (c >> 16) & 0xff;
    final cg = (c >> 8) & 0xff;
    final cb = c & 0xff;
    final ca = (c >> 24) & 0xff;
    final r = (ar * ba + br * bb + cr * bc).round().clamp(0, 255);
    final g = (ag * ba + bg * bb + cg * bc).round().clamp(0, 255);
    final bl = (ab * ba + bb2 * bb + cb * bc).round().clamp(0, 255);
    final al = (aa * ba + ba2 * bb + ca * bc).round().clamp(0, 255);
    return (al << 24) | (r << 16) | (g << 8) | bl;
  }
}

/// Scratch fragment storage. The rasterizer reuses one [Fragment] per
/// rasterizer instance to avoid per-pixel allocation. Not thread-safe.
class FragmentScratch {
  final Fragment fragment = Fragment();

  /// Reusable scratch vector for the (camera - worldPosition) view
  /// direction computation — avoids allocating a Vector3 per pixel.
  final Vector3 viewDir = Vector3.zero();

  /// Reusable scratch vector for the interpolated (un-normalized)
  /// normal — avoids allocating a Vector3 per pixel.
  final Vector3 worldNormal = Vector3.zero();
}
