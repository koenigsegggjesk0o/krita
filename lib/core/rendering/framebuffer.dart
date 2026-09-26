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
// framebuffer.dart — Render target for the software rasterizer.
//
// Owns a packed ARGB8888 color buffer ([Uint32List]) plus an associated
// [DepthBuffer]. Provides:
//   - clear(color)
//   - putPixel / blendPixel / getPixel
//   - scissor-rect clipping
//   - resolve() — convert the color buffer to a Flutter [ui.Image] for
//     compositing onto a Canvas
//
// The resolve path is the bridge between the pure-Dart rasterizer and
// Flutter's `Canvas.drawImage`. It builds a `ui.Image` once per frame
// (the canonical "present" step) and hands it back to the caller.
//
// All colors are 0xAARRGGBB ints in straight (non-premultiplied) alpha
// space — premultiplication happens at the [Image] conversion step so
// the rasterizer's blend math stays simple.

import 'dart:typed_data';
import 'dart:ui' as ui;

import 'depth_buffer.dart';

/// A rectangular render target.
class FrameBuffer {
  FrameBuffer({
    required this.width,
    required this.height,
    this.clearColor = 0x00000000,
  })  : _color = Uint32List(width * height),
        depth = DepthBuffer(width, height) {
    clear();
  }

  /// Width in pixels.
  final int width;

  /// Height in pixels.
  final int height;

  /// Default clear color (ARGB int). Used by [clear] when no argument
  /// is supplied.
  final int clearColor;

  /// Packed color buffer (ARGB). Indexed as `y * width + x`.
  final Uint32List _color;

  /// The associated depth buffer. Lifetime is tied to this framebuffer.
  final DepthBuffer depth;

  /// Active scissor rectangle. Pixels outside this rect are not
  /// written by [putPixel] / [blendPixel]. Default covers the whole
  /// framebuffer.
  ui.Rect scissor = ui.Rect.zero;

  /// Sets the scissor to ([x], [y], [w], [h]) in framebuffer space
  /// (origin top-left). Clamped to the framebuffer bounds.
  void setScissor(int x, int y, int w, int h) {
    final xa = x.clamp(0, width);
    final ya = y.clamp(0, height);
    final xb = (x + w).clamp(0, width);
    final yb = (y + h).clamp(0, height);
    scissor = ui.Rect.fromLTRB(xa.toDouble(), ya.toDouble(),
        xb.toDouble(), yb.toDouble());
  }

  /// Resets the scissor to the whole framebuffer.
  void resetScissor() {
    scissor = ui.Rect.fromLTWH(
        0, 0, width.toDouble(), height.toDouble());
  }

  /// True when ([x], [y]) is inside the framebuffer bounds AND the
  /// active scissor rect. The single inline bounds check used by the
  /// rasterizer's hot loop.
  bool inBounds(int x, int y) {
    if (x < 0 || y < 0 || x >= width || y >= height) return false;
    if (scissor != ui.Rect.zero) {
      final left = scissor.left.toInt();
      final top = scissor.top.toInt();
      final right = scissor.right.toInt();
      final bottom = scissor.bottom.toInt();
      if (x < left || x >= right || y < top || y >= bottom) return false;
    }
    return true;
  }

  /// Clears the color buffer to [color] (default [clearColor]) and the
  /// depth buffer to far (1.0). Cheap: two fill-range calls.
  void clear({int? color}) {
    final c = color ?? clearColor;
    _color.fillRange(0, _color.length, c);
    depth.clear();
    if (scissor != ui.Rect.zero) resetScissor();
  }

  /// Clears the color buffer only — depth is untouched. Used by the
  /// transparent pass which needs to keep the opaque depth around.
  void clearColorOnly({int? color}) {
    _color.fillRange(0, _color.length, color ?? clearColor);
  }

  /// Writes [argb] directly into the color buffer. Bypasses depth and
  /// scissor — only call from code that has already validated the
  /// pixel is in-bounds.
  void putPixelRaw(int x, int y, int argb) {
    _color[y * width + x] = argb;
  }

  /// Bounds-checked, scissor-checked pixel write.
  void putPixel(int x, int y, int argb) {
    if (!inBounds(x, y)) return;
    _color[y * width + x] = argb;
  }

  /// Reads the color at ([x], [y]). Returns 0 (transparent) on
  /// out-of-range.
  int getPixel(int x, int y) {
    if (x < 0 || y < 0 || x >= width || y >= height) return 0;
    return _color[y * width + x];
  }

  /// Returns the raw color storage so the rasterizer can avoid the
  /// per-pixel method dispatch. Use only inside the rasterizer hot loop.
  Uint32List rawColorStorage() => _color;

  /// Resolves the color buffer into a Flutter [ui.Image] for compositing
  /// onto a Canvas. The returned image is owned by the caller and MUST
  /// be disposed when no longer needed (typically after
  /// `canvas.drawImage`).
  ///
  /// Internally: delegates to [ui.decodeImageFromPixels] (which runs on
  /// the platform's image-decoding isolate). The future completes once
  /// the decode callback fires — typically within a frame or two.
  Future<ui.Image> resolve() {
    final completer = <ui.Image>[];
    final bytes = _color.buffer.asUint8List();
    ui.decodeImageFromPixels(bytes, width, height,
        ui.PixelFormat.bgra8888, (img) => completer.add(img));
    // decodeImageFromPixels fires its callback synchronously on most
    // platforms (or on the next microtask); we poll the completer until
    // it has an image, yielding between checks so the event loop can
    // dispatch the callback.
    return _awaitImage(completer);
  }

  Future<ui.Image> _awaitImage(List<ui.Image> completer) async {
    while (completer.isEmpty) {
      await Future<void>.delayed(Duration.zero);
    }
    return completer.first;
  }

  /// Synchronous variant of [resolve] that uses the simpler
  /// [ui.decodeImageFromPixels] API. Useful for tests and the FXAA pass
  /// where a future is awkward. The callback fires on the UI isolate.
  void resolveImage(void Function(ui.Image) onImage) {
    final bytes = _color.buffer.asUint8List();
    ui.decodeImageFromPixels(
        bytes, width, height, ui.PixelFormat.bgra8888, onImage);
  }

  /// Copies a sub-rect of the framebuffer into [dst] starting at
  /// ([dstX], [dstY]). Used by the MSAA resolve pass to fetch the
  /// fragment under a sample. Returns the number of pixels copied.
  int blitTo(Uint32List dst, int dstX, int dstY, int dstWidth,
      int srcX, int srcY, int srcW, int srcH) {
    var copied = 0;
    for (var y = 0; y < srcH; y++) {
      final sy = srcY + y;
      final dy = dstY + y;
      if (sy < 0 || sy >= height || dy < 0) continue;
      for (var x = 0; x < srcW; x++) {
        final sx = srcX + x;
        final dx = dstX + x;
        if (sx < 0 || sx >= width || dx < 0) continue;
        dst[dy * dstWidth + dx] = _color[sy * width + sx];
        copied++;
      }
    }
    return copied;
  }
}
