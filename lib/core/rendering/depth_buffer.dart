// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// Export-only: software rasterizer for high-quality PNG export.
// Not used in real-time (too slow for 30 fps); the canvas viewport's
// CustomPainter handles live rendering. This subsystem is reserved
// for the export pipeline (rasterize scene -> framebuffer -> PNG)
// and for future headless render paths.
//
// depth_buffer.dart — Z-buffer for the software rasterizer.
//
// Stores one float per pixel of the framebuffer. The depth value uses the
// same convention as the OpenGL clip-space Z after perspective divide:
//   - 0.0  = near plane (closest to camera)
//   - 1.0  = far plane  (farthest from camera)
//
// The buffer is cleared to 1.0 (far) so anything in front of the far
// plane passes the depth test by default. The depth-comparison function
// defaults to [DepthCompare.lessEqual] which matches the standard OpenGL
// "depth test enabled + glDepthFunc(LEQUAL)" setup that Feather-Krita's
// 3D preview expects.
//
// Performance note: the buffer is a plain `Float64List` rather than a
// `List<double>` — the typed array is ~3x faster on the JIT and unlocks
// SIMD-friendly access patterns for the rasterizer's tight inner loop.

import 'dart:typed_data';

/// Depth-comparison functions understood by [DepthBuffer].
enum DepthCompare {
  /// Always passes (depth test effectively disabled).
  always,

  /// Never passes — every fragment is discarded.
  never,

  /// Pass when fragment depth < stored depth (strict).
  less,

  /// Pass when fragment depth <= stored depth. The default — gives the
  /// "first surface wins" behaviour expected of opaque scenes rendered
  /// front-to-back, and is forgiving of exact-tie rasterization drift.
  lessEqual,

  /// Pass when fragment depth == stored depth. Used for stencil-like
  /// decal passes.
  equal,

  /// Pass when fragment depth >= stored depth.
  greaterEqual,

  /// Pass when fragment depth > stored depth (strict).
  greater,

  /// Pass when fragment depth != stored depth.
  notEqual,
}

/// A 2D depth buffer.
///
/// The buffer is owned by the [FrameBuffer] and exposed so the rasterizer
/// can poke pixels directly without going through the framebuffer's color
/// write path. Mutating the buffer outside a frame is safe; the next
/// [clear] will reset it.
class DepthBuffer {
  DepthBuffer(this.width, this.height)
      : _depth = Float64List(width * height),
        _stencil = Uint8List(width * height) {
    clear();
  }

  /// Framebuffer width in pixels.
  final int width;

  /// Framebuffer height in pixels.
  final int height;

  /// The raw depth values. Indexed as `y * width + x`. Exposed for the
  /// rasterizer's inner loop — callers MUST NOT rely on any particular
  /// value beyond the contract that 0 = near, 1 = far, and 1 is the
  /// cleared state.
  final Float64List _depth;

  /// Optional 8-bit stencil buffer, sharing the same pixel grid. Used by
  /// the cutout shader and the MSAA resolve pass. Currently never read
  /// by the rasterizer hot path; included so future features (decals,
  /// outlined silhouettes) don't need a buffer-format bump.
  final Uint8List _stencil;

  /// Active depth-comparison function. Defaults to [DepthCompare.lessEqual].
  DepthCompare compareFunc = DepthCompare.lessEqual;

  /// Whether to write the fragment depth into the buffer after a passing
  /// depth test. Disable for transparency passes that should not occlude
  /// each other.
  bool depthWrite = true;

  /// Bias added to the fragment depth BEFORE the comparison. Positive
  /// values push fragments further away (helpful for decals / shadow
  /// maps). Negative values pull them closer.
  double depthBias = 0.0;

  /// Slope-scaled bias (PolygonOffset-style). Added per-fragment based
  /// on the abs screen-space depth gradient. Default 0 disables.
  double slopeScaledBias = 0.0;

  /// Returns the depth value stored at ([x], [y]). Out-of-range reads
  /// return 1.0 (far) so accidental over-scan never reads as "near".
  double read(int x, int y) {
    if (x < 0 || y < 0 || x >= width || y >= height) return 1.0;
    return _depth[y * width + x];
  }

  /// Reads the stencil byte at ([x], [y]). Returns 0 on out-of-range.
  int readStencil(int x, int y) {
    if (x < 0 || y < 0 || x >= width || y >= height) return 0;
    return _stencil[y * width + x];
  }

  /// Writes [value] into the stencil byte at ([x], [y]). No-op on
  /// out-of-range.
  void writeStencil(int x, int y, int value) {
    if (x < 0 || y < 0 || x >= width || y >= height) return;
    _stencil[y * width + x] = value & 0xff;
  }

  /// Runs the depth test for ([x], [y]) with the fragment's depth
  /// [fragmentDepth] (NDC Z in [0, 1]). Returns true if the fragment
  /// passes the test.
  ///
  /// Side effects: if [depthWrite] is enabled AND the test passes, the
  /// stored depth is updated to the fragment's depth. The caller does
  /// not need to call [write] separately.
  bool test(int x, int y, double fragmentDepth,
      {double slope = 0.0}) {
    if (x < 0 || y < 0 || x >= width || y >= height) return false;
    final idx = y * width + x;
    final stored = _depth[idx];
    final d = fragmentDepth + depthBias + slope * slopeScaledBias;
    final passed = _applyCompare(d, stored);
    if (passed && depthWrite) {
      _depth[idx] = d;
    }
    return passed;
  }

  /// Same as [test] but does not write the new depth even if the test
  /// passes — used for read-only depth sampling (shadow / ambient-occlusion
  /// probes, MSAA coverage checks).
  bool testReadOnly(int x, int y, double fragmentDepth) {
    if (x < 0 || y < 0 || x >= width || y >= height) return false;
    final stored = _depth[y * width + x];
    return _applyCompare(fragmentDepth + depthBias, stored);
  }

  /// Unconditionally writes [fragmentDepth] at ([x], [y]). Bypasses the
  /// test. Used by the MSAA resolve pass.
  void write(int x, int y, double fragmentDepth) {
    if (x < 0 || y < 0 || x >= width || y >= height) return;
    _depth[y * width + x] = fragmentDepth;
  }

  /// Resets every depth sample to [value] (default 1.0 = far plane) and
  /// the stencil to 0. Cheap: a single [Float64List.fillRange] call.
  void clear({double value = 1.0}) {
    _depth.fillRange(0, _depth.length, value);
    _stencil.fillRange(0, _stencil.length, 0);
  }

  /// Clears a sub-rect of the buffer. Used for scissor-restricted clears.
  void clearRect(int x0, int y0, int x1, int y1, {double value = 1.0}) {
    final xa = x0.clamp(0, width);
    final xb = x1.clamp(0, width);
    final ya = y0.clamp(0, height);
    final yb = y1.clamp(0, height);
    for (var y = ya; y < yb; y++) {
      final row = y * width;
      _depth.fillRange(row + xa, row + xb, value);
      _stencil.fillRange(row + xa, row + xb, 0);
    }
  }

  /// Returns a snapshot copy of the depth buffer. The snapshot is a
  /// [Float64List] of length `width * height`. Used by debug overlays
  /// and the depth-of-field post pass.
  Float64List readback() => Float64List.fromList(_depth);

  /// Exposes the internal storage so the rasterizer can read/write
  /// multiple pixels without the per-call bounds check overhead. Use
  /// only inside the rasterizer hot loop.
  Float64List rawStorage() => _depth;

  /// Exposes the internal stencil storage for the same reason.
  Uint8List rawStencilStorage() => _stencil;

  bool _applyCompare(double a, double b) {
    switch (compareFunc) {
      case DepthCompare.always:
        return true;
      case DepthCompare.never:
        return false;
      case DepthCompare.less:
        return a < b;
      case DepthCompare.lessEqual:
        return a <= b;
      case DepthCompare.equal:
        return (a - b).abs() < 1e-6;
      case DepthCompare.greaterEqual:
        return a >= b;
      case DepthCompare.greater:
        return a > b;
      case DepthCompare.notEqual:
        return (a - b).abs() >= 1e-6;
    }
  }
}
