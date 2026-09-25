// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// krita_canvas_controller.dart — Canvas control surface.
//
// Owns an RGBA8 backing buffer that hosts paint dabs from the brush
// controller. The buffer is the paint target for the editor's texture
// painter (Feather-Krita paints onto a 2D texture, not a 1D stroke
// list — the canvas controller is the 2D equivalent of Krita's
// KisPaintDevice for the host-side composite path).
//
// Responsibilities:
//   * Create / resize the buffer (preserving existing content where
//     possible, like KisImage::resize).
//   * Composite-stamp a [BrushDab] at a pixel position with a blend
//     mode (the same blend math the engine would run inside
//     KisPainter, but on the host side so we can paint even when the
//     native stroke-session ABI is not available).
//   * Read pixels back out (for texture upload to the GPU, for export,
//     for undo snapshots).
//   * Clear / fill the buffer.
//
// The controller is NOT a substitute for the native stroke-session ABI
// (`krita_stroke_*`): that path runs a REAL stroke through Krita's
// KisPaintOpRegistry. This controller is the host-side composite path
// used by the v1 dab-compositing engine (still the default for texture
// painting) and the fallback engine.

import 'dart:math' as math;
import 'dart:typed_data';

import 'krita_bindings.dart';

/// Blend modes supported by [KritaCanvasController.paintDab]. These
/// mirror the compositing ops Krita exposes in KisPainter (normal /
/// erase / multiply / screen / overlay / etc.) so the editor's blend
/// dropdown maps 1:1 to the engine's own vocabulary.
enum KritaBlendMode {
  /// Source-over alpha blend (the default).
  normal,

  /// Destination-out (subtract alpha — used by eraser presets).
  erase,

  /// Multiply source with destination (darkens).
  multiply,

  /// Screen blend (lightens).
  screen,

  /// Overlay — multiply if base dark, screen if base light.
  overlay,
}

/// A 2D RGBA8 paint canvas. Owns its pixel buffer and composite-stamps
/// brush dabs onto it. The buffer is laid out row-major as
/// `width * height * 4` bytes in R,G,B,A byte order (straight alpha —
/// premultiplication is applied only during the composite math).
class KritaCanvasController {
  /// Constructs a canvas of [width] x [height] pixels, fully
  /// transparent. [width] and [height] must be positive.
  KritaCanvasController({required int width, required int height})
      : assert(width > 0),
        assert(height > 0),
        _width = width,
        _height = height,
        _pixels = Uint8List(width * height * 4);

  int _width;
  int _height;
  Uint8List _pixels;

  /// The canvas width in pixels.
  int get width => _width;

  /// The canvas height in pixels.
  int get height => _height;

  /// The backing RGBA8 buffer (read/write). Callers that mutate this
  /// directly should call [markDirty] so render consumers can rasterize
  /// the new state.
  Uint8List get pixels => _pixels;

  /// Mutation serial — bumped on every [paintDab] / [clear] / [fill] /
  /// [resize] call so render consumers can rasterize exactly once per
  /// content change.
  int get version => _version;
  int _version = 0;

  /// True when at least one pixel is non-transparent (alpha > 0).
  bool get hasContent {
    for (var i = 3; i < _pixels.length; i += 4) {
      if (_pixels[i] != 0) return true;
    }
    return false;
  }

  /// Resizes the canvas to [width] x [height] pixels. Existing content
  /// is preserved in the top-left corner (clipped to the new bounds);
  /// newly exposed pixels are fully transparent. Like
  /// KisImage::resize with a top-left anchor. [width] and [height]
  /// must be positive.
  void resize(int width, int height) {
    if (width <= 0 || height <= 0) {
      throw ArgumentError('width and height must be positive');
    }
    if (width == _width && height == _height) return;
    final next = Uint8List(width * height * 4);
    final copyW = math.min(width, _width);
    final copyH = math.min(height, _height);
    for (var y = 0; y < copyH; y++) {
      final srcOff = y * _width * 4;
      final dstOff = y * width * 4;
      next.setRange(dstOff, dstOff + copyW * 4, _pixels, srcOff);
    }
    _width = width;
    _height = height;
    _pixels = next;
    _version++;
  }

  /// Composite-stamps [dab] onto the canvas at pixel position
  /// ([centerX], [centerY]) — the dab's CENTER is placed there. Pixels
  /// outside the canvas are clipped. The dab's alpha is scaled by
  /// [alpha] (0..1) before compositing, so callers can apply
  /// per-stroke opacity without rebuilding the dab. [mode] selects the
  /// blend op (see [KritaBlendMode]).
  void paintDab(BrushDab dab, double centerX, double centerY,
      {KritaBlendMode mode = KritaBlendMode.normal, double alpha = 1.0}) {
    if (dab.isEmpty) return;
    final a = alpha.clamp(0.0, 1.0);
    if (a <= 0.0) return;
    final halfW = dab.width / 2.0;
    final halfH = dab.height / 2.0;
    final minX = (centerX - halfW).round();
    final minY = (centerY - halfH).round();
    final dabStride = dab.stride == 0 ? dab.width * 4 : dab.stride;
    for (var y = 0; y < dab.height; y++) {
      final dstY = minY + y;
      if (dstY < 0 || dstY >= _height) continue;
      for (var x = 0; x < dab.width; x++) {
        final dstX = minX + x;
        if (dstX < 0 || dstX >= _width) continue;
        final srcOff = y * dabStride + x * 4;
        final dstOff = (dstY * _width + dstX) * 4;
        _compositePixel(dstOff, dab.pixels, srcOff, mode, a);
      }
    }
    _version++;
  }

  void _compositePixel(int dstOff, Uint8List src, int srcOff,
      KritaBlendMode mode, double alphaScale) {
    final sr = src[srcOff];
    final sg = src[srcOff + 1];
    final sb = src[srcOff + 2];
    final sa = (src[srcOff + 3] * alphaScale).round().clamp(0, 255);
    if (sa == 0) return;
    final dr = _pixels[dstOff];
    final dg = _pixels[dstOff + 1];
    final db = _pixels[dstOff + 2];
    final da = _pixels[dstOff + 3];
    switch (mode) {
      case KritaBlendMode.normal:
        final outA = sa + (da * (255 - sa) ~/ 255);
        if (outA == 0) {
          _pixels[dstOff] = 0;
          _pixels[dstOff + 1] = 0;
          _pixels[dstOff + 2] = 0;
          _pixels[dstOff + 3] = 0;
          return;
        }
        _pixels[dstOff] = ((sr * sa + dr * da * (255 - sa) ~/ 255) ~/ outA)
            .clamp(0, 255);
        _pixels[dstOff + 1] = ((sg * sa + dg * da * (255 - sa) ~/ 255) ~/ outA)
            .clamp(0, 255);
        _pixels[dstOff + 2] = ((sb * sa + db * da * (255 - sa) ~/ 255) ~/ outA)
            .clamp(0, 255);
        _pixels[dstOff + 3] = outA;
      case KritaBlendMode.erase:
        final outA = (da * (255 - sa) ~/ 255).clamp(0, 255);
        _pixels[dstOff + 3] = outA;
        // Color channels are unchanged (the erased pixel keeps its hue
        // until fully transparent — matches Krita's destination-out).
      case KritaBlendMode.multiply:
        final outA = sa + (da * (255 - sa) ~/ 255);
        if (outA == 0) return;
        final mr = (sr * dr ~/ 255).clamp(0, 255);
        final mg = (sg * dg ~/ 255).clamp(0, 255);
        final mb = (sb * db ~/ 255).clamp(0, 255);
        _pixels[dstOff] = ((mr * sa + dr * da * (255 - sa) ~/ 255) ~/ outA)
            .clamp(0, 255);
        _pixels[dstOff + 1] = ((mg * sa + dg * da * (255 - sa) ~/ 255) ~/ outA)
            .clamp(0, 255);
        _pixels[dstOff + 2] = ((mb * sa + db * da * (255 - sa) ~/ 255) ~/ outA)
            .clamp(0, 255);
        _pixels[dstOff + 3] = outA;
      case KritaBlendMode.screen:
        final outA = sa + (da * (255 - sa) ~/ 255);
        if (outA == 0) return;
        final sr2 = (255 - ((255 - sr) * (255 - dr) ~/ 255)).clamp(0, 255);
        final sg2 = (255 - ((255 - sg) * (255 - dg) ~/ 255)).clamp(0, 255);
        final sb2 = (255 - ((255 - sb) * (255 - db) ~/ 255)).clamp(0, 255);
        _pixels[dstOff] = ((sr2 * sa + dr * da * (255 - sa) ~/ 255) ~/ outA)
            .clamp(0, 255);
        _pixels[dstOff + 1] = ((sg2 * sa + dg * da * (255 - sa) ~/ 255) ~/ outA)
            .clamp(0, 255);
        _pixels[dstOff + 2] = ((sb2 * sa + db * da * (255 - sa) ~/ 255) ~/ outA)
            .clamp(0, 255);
        _pixels[dstOff + 3] = outA;
      case KritaBlendMode.overlay:
        final outA = sa + (da * (255 - sa) ~/ 255);
        if (outA == 0) return;
        int blend(int c, int b) {
          if (b < 128) return (2 * b * c ~/ 255).clamp(0, 255);
          return (255 - 2 * (255 - b) * (255 - c) ~/ 255).clamp(0, 255);
        }
        final or2 = blend(sr, dr);
        final og2 = blend(sg, dg);
        final ob2 = blend(sb, db);
        _pixels[dstOff] = ((or2 * sa + dr * da * (255 - sa) ~/ 255) ~/ outA)
            .clamp(0, 255);
        _pixels[dstOff + 1] = ((og2 * sa + dg * da * (255 - sa) ~/ 255) ~/ outA)
            .clamp(0, 255);
        _pixels[dstOff + 2] = ((ob2 * sa + db * da * (255 - sa) ~/ 255) ~/ outA)
            .clamp(0, 255);
        _pixels[dstOff + 3] = outA;
    }
  }

  /// Reads a copy of the full RGBA8 buffer. The returned list is
  /// independent of the canvas's internal storage (edits to it do not
  /// affect the canvas).
  Uint8List readPixels() => Uint8List.fromList(_pixels);

  /// Reads a sub-rectangle of the canvas. [x], [y], [w], [h] are in
  /// canvas pixel space; the rectangle is clipped to the canvas bounds.
  /// Returns an empty [Uint8List] when the clipped rectangle is empty.
  Uint8List readRect(int x, int y, int w, int h) {
    final cx = x.clamp(0, _width);
    final cy = y.clamp(0, _height);
    final cw = (w - (cx - x)).clamp(0, _width - cx);
    final ch = (h - (cy - y)).clamp(0, _height - cy);
    if (cw <= 0 || ch <= 0) return Uint8List(0);
    final out = Uint8List(cw * ch * 4);
    for (var row = 0; row < ch; row++) {
      final srcOff = ((cy + row) * _width + cx) * 4;
      final dstOff = row * cw * 4;
      out.setRange(dstOff, dstOff + cw * 4, _pixels, srcOff);
    }
    return out;
  }

  /// Writes [rgba] into a sub-rectangle at ([x], [y]) of size [w] x [h].
  /// [rgba] must hold at least `w * h * 4` bytes in R,G,B,A order. The
  /// rectangle is clipped to the canvas bounds.
  void writeRect(Uint8List rgba, int x, int y, int w, int h) {
    if (rgba.length < w * h * 4) {
      throw ArgumentError('rgba too short: need ${w * h * 4}, '
          'got ${rgba.length}');
    }
    final cx = x.clamp(0, _width);
    final cy = y.clamp(0, _height);
    final cw = (w - (cx - x)).clamp(0, _width - cx);
    final ch = (h - (cy - y)).clamp(0, _height - cy);
    if (cw <= 0 || ch <= 0) return;
    for (var row = 0; row < ch; row++) {
      final dstOff = ((cy + row) * _width + cx) * 4;
      final srcOff = row * w * 4;
      _pixels.setRange(dstOff, dstOff + cw * 4, rgba, srcOff);
    }
    _version++;
  }

  /// Clears the canvas to fully transparent.
  void clear() {
    _pixels.fillRange(0, _pixels.length, 0);
    _version++;
  }

  /// Fills the canvas with a solid [color].
  void fill(BrushColor color) {
    for (var i = 0; i < _pixels.length; i += 4) {
      _pixels[i] = color.r;
      _pixels[i + 1] = color.g;
      _pixels[i + 2] = color.b;
      _pixels[i + 3] = color.a;
    }
    _version++;
  }

  /// Notifies render consumers that the buffer mutated out-of-band
  /// (direct [pixels] writes). Only needed when callers skip the
  /// [paintDab] / [clear] / [fill] / [writeRect] / [resize] methods.
  void markDirty() => _version++;
}
