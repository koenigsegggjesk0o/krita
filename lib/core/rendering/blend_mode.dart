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
// blend_mode.dart — Pixel blend modes for the software rasterizer.
//
// Provides [BlendMode3D] — an enum + table of per-pixel blend functions
// used by the rasterizer when compositing fragment output onto the
// framebuffer. Mirrors the Porter-Duff / W3C Compositing blend modes
// where it matters, plus a couple of feather-specific extras (additive
// glow, screen, multiply, overlay).
//
// All blend functions take the SOURCE color (the fragment shader output)
// and the DESTINATION color (what is currently in the framebuffer) plus
// a generic alpha factor, and return the new destination color.
//
// Colors are ARGB-packed ints (0xAARRGGBB) in 0–255 channels. The blend
// math is done in straight (non-premultiplied) alpha space; callers are
// responsible for premultiplication if they need it.

/// Blend modes supported by the Feather-Krita software rasterizer.
///
/// The mode controls how a fragment's color ([src]) is combined with the
/// framebuffer color ([dst]) at the same pixel. All math runs in straight
/// 8-bit RGBA space.
enum BlendMode3D {
  /// Replace destination with source. The fragment alpha is ignored.
  replace,

  /// Standard alpha compositing (Porter-Duff source-over).
  normal,

  /// Source + destination, clamped to 255. Used for glow/emissive passes.
  additive,

  /// Source * destination / 255. Darkens.
  multiply,

  /// 1 - (1 - src) * (1 - dst). Lightens, never exceeds 255.
  screen,

  /// Inverse multiply when dst < 128, inverse screen otherwise.
  overlay,

  /// Dest keeps color, source alpha accumulates (used by cutout masks).
  maskAlpha,
}

/// Pure-functions table of blend implementations.
///
/// Keeping the blend math in a separate class — rather than switch-ing on
/// the enum in the hot rasterizer loop — lets the rasterizer specialize
/// per-mode without re-checking the enum for every pixel.
class BlendFunctions {
  const BlendFunctions._();

  /// Unpacks an ARGB int into (r, g, b, a) doubles in [0, 255].
  static (double r, double g, double b, double a) unpack(int c) {
    return (
      ((c >> 16) & 0xff).toDouble(),
      ((c >> 8) & 0xff).toDouble(),
      (c & 0xff).toDouble(),
      ((c >> 24) & 0xff).toDouble(),
    );
  }

  /// Packs (r, g, b, a) doubles (each clamped to [0, 255]) into an ARGB int.
  static int pack(double r, double g, double b, double a) {
    return (_clamp(a).round() << 24) |
        (_clamp(r).round() << 16) |
        (_clamp(g).round() << 8) |
        _clamp(b).round();
  }

  static int _clampChannel(double v) {
    if (v < 0) return 0;
    if (v > 255) return 255;
    return v.round();
  }

  static double _clamp(double v) => v.clamp(0.0, 255.0);

  /// Blends [src] over [dst] using [mode]. The [fragmentAlpha] argument
  /// is the fragment shader's final alpha (0–255) after any dithering —
  /// the caller may already have folded this into [src]'s alpha, in
  /// which case pass 255 to disable the extra scaling.
  static int blend(
      BlendMode3D mode, int src, int dst, double fragmentAlpha) {
    final fa = (fragmentAlpha / 255.0).clamp(0.0, 1.0);
    switch (mode) {
      case BlendMode3D.replace:
        return src;
      case BlendMode3D.normal:
        return _normal(src, dst, fa);
      case BlendMode3D.additive:
        return _additive(src, dst, fa);
      case BlendMode3D.multiply:
        return _multiply(src, dst, fa);
      case BlendMode3D.screen:
        return _screen(src, dst, fa);
      case BlendMode3D.overlay:
        return _overlay(src, dst, fa);
      case BlendMode3D.maskAlpha:
        return _maskAlpha(src, dst, fa);
    }
  }

  static int _normal(int src, int dst, double fa) {
    final (sr, sg, sb, sa) = unpack(src);
    final (dr, dg, db, da) = unpack(dst);
    final a = sa * fa / 255.0;
    final inv = 1.0 - a;
    return pack(
      sr * a + dr * inv,
      sg * a + dg * inv,
      sb * a + db * inv,
      sa * fa + da * (1.0 - a),
    );
  }

  static int _additive(int src, int dst, double fa) {
    final (sr, sg, sb, sa) = unpack(src);
    final (dr, dg, db, da) = unpack(dst);
    final k = sa * fa / 255.0;
    return pack(
      dr + sr * k,
      dg + sg * k,
      db + sb * k,
      da + sa * fa,
    );
  }

  static int _multiply(int src, int dst, double fa) {
    final (sr, sg, sb, sa) = unpack(src);
    final (dr, dg, db, da) = unpack(dst);
    final orr = sr * dr / 255.0;
    final og = sg * dg / 255.0;
    final ob = sb * db / 255.0;
    return _lerpChannels(src, dst, orr, og, ob, sa * fa);
  }

  static int _screen(int src, int dst, double fa) {
    final (sr, sg, sb, sa) = unpack(src);
    final (dr, dg, db, da) = unpack(dst);
    final orr = 255.0 - (255.0 - sr) * (255.0 - dr) / 255.0;
    final og = 255.0 - (255.0 - sg) * (255.0 - dg) / 255.0;
    final ob = 255.0 - (255.0 - sb) * (255.0 - db) / 255.0;
    return _lerpChannels(src, dst, orr, og, ob, sa * fa);
  }

  static int _overlay(int src, int dst, double fa) {
    final (sr, sg, sb, sa) = unpack(src);
    final (dr, dg, db, _) = unpack(dst);
    final orr = _overlayChannel(sr, dr);
    final og = _overlayChannel(sg, dg);
    final ob = _overlayChannel(sb, db);
    return _lerpChannels(src, dst, orr, og, ob, sa * fa);
  }

  static double _overlayChannel(double s, double d) {
    if (d < 128.0) {
      return 2.0 * s * d / 255.0;
    }
    return 255.0 - 2.0 * (255.0 - s) * (255.0 - d) / 255.0;
  }

  static int _maskAlpha(int src, int dst, double fa) {
    final (_, _, _, sa) = unpack(src);
    final (dr, dg, db, da) = unpack(dst);
    final a = (sa * fa + da).clamp(0.0, 255.0);
    return pack(dr, dg, db, a);
  }

  /// Helper: composites (orr, og, ob) — the would-be output color of the
  /// blend — over the destination using the source alpha scaled by [fa].
  /// This is the standard "blend color, then alpha-composite" pattern
  /// used by multiply / screen / overlay.
  static int _lerpChannels(int src, int dst, double orr, double og,
      double ob, double alpha255) {
    final (dr, dg, db, da) = unpack(dst);
    final a = (alpha255 / 255.0).clamp(0.0, 1.0);
    final inv = 1.0 - a;
    return pack(
      orr * a + dr * inv,
      og * a + dg * inv,
      ob * a + db * inv,
      alpha255.clamp(0.0, 255.0) + da * inv,
    );
  }
}

/// A small dithering helper used by the rasterizer to reduce banding on
/// smooth-shaded gradients (e.g. shaded_shader's Lambert ramp). The matrix
/// is the classic 4x4 Bayer ordered-dither pattern, rescaled to a
/// configurable amplitude.
class ColorDither {
  static const List<List<int>> _bayer4x4 = [
    [0, 8, 2, 10],
    [12, 4, 14, 6],
    [3, 11, 1, 9],
    [15, 7, 13, 5],
  ];

  /// Returns a dithered ARGB color. [x], [y] are the pixel coordinates
  /// inside the framebuffer. [amplitude] is the maximum per-pixel
  /// perturbation in 0–255 units (typically 4–8).
  static int dither(int argb, int x, int y, {double amplitude = 6.0}) {
    final t = (_bayer4x4[y & 3][x & 3] - 7.5) * (amplitude / 8.0);
    final r = ((argb >> 16) & 0xff).toDouble() + t;
    final g = ((argb >> 8) & 0xff).toDouble() + t;
    final b = (argb & 0xff).toDouble() + t;
    final a = (argb >> 24) & 0xff;
    return (a << 24) |
        (r.clamp(0.0, 255.0).round() << 16) |
        (g.clamp(0.0, 255.0).round() << 8) |
        b.clamp(0.0, 255.0).round();
  }
}
