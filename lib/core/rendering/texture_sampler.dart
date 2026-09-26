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
// texture_sampler.dart — Texture sampling for the software rasterizer.
//
// Provides:
//   - [TextureWrap]   — repeat / clamp / mirror
//   - [TextureFilter] — nearest / bilinear / trilinear
//   - [Texture]       — wraps a packed ARGB8888 mipmap chain
//   - [TextureSampler] — stateless sampling function
//
// All sampling math runs in pure Dart. The texture data is stored as a
// list of `Uint32List` mip levels (one per LOD); the sampler selects the
// appropriate level(s) based on the requested gradient.
//
// Performance: the sampler is hot-path code — every fragment of a textured
// triangle calls into it. The implementation avoids allocations in the
// bilinear path (uses fixed-point index math) and skips mip filtering
// entirely when the texture has only one level.

import 'dart:math' as math;
import 'dart:typed_data';

/// Texture coordinate wrap modes.
enum TextureWrap {
  /// Tile the texture — UV outside [0,1] wraps around.
  repeat,

  /// Clamp UV to [0,1] — border pixels stretch.
  clamp,

  /// Mirror at every integer boundary.
  mirror,
}

/// Minification / magnification filters.
enum TextureFilter {
  /// Pick the nearest texel. Fast, hard-edged.
  nearest,

  /// 2x2 linear blend. Smooth, slightly slower.
  bilinear,

  /// Bilinear on each of the two nearest mip levels, then linear blend
  /// between them. Requires a mipmap chain.
  trilinear,
}

/// An immutable texture image with optional mipmap chain.
class Texture {
  Texture(this.levels, {this.wrapU = TextureWrap.clamp, this.wrapV = TextureWrap.clamp})
      : assert(levels.isNotEmpty),
        _width = levels.first.width,
        _height = levels.first.height {
    // Verify mip chain: each level should be half the previous.
    var w = _width;
    var h = _height;
    for (var i = 0; i < levels.length; i++) {
      final lv = levels[i];
      if (lv.width != w || lv.height != h) {
        throw ArgumentError(
            'Mip level $i has wrong size: expected ${w}x$h}, got ${lv.width}x${lv.height}');
      }
      w = math.max(1, w ~/ 2);
      h = math.max(1, h ~/ 2);
    }
  }

  /// Mip levels — level 0 is the highest resolution. Each subsequent
  /// level is half the previous (rounded down, minimum 1x1).
  final List<MipLevel> levels;

  /// Wrap mode for U axis.
  final TextureWrap wrapU;

  /// Wrap mode for V axis.
  final TextureWrap wrapV;

  final int _width;
  final int _height;

  /// Width of mip level 0.
  int get width => _width;

  /// Height of mip level 0.
  int get height => _height;

  /// Number of mip levels.
  int get mipCount => levels.length;

  /// Builds a single-mip texture from an ARGB8888 packed [Uint32List].
  factory Texture.fromArgb(Uint32List pixels, int width, int height,
      {TextureWrap wrapU = TextureWrap.clamp,
      TextureWrap wrapV = TextureWrap.clamp}) {
    return Texture(
      [MipLevel(pixels, width, height)],
      wrapU: wrapU,
      wrapV: wrapV,
    );
  }

  /// Builds a mipmap chain from a base image. Generates all levels down
  /// to 1x1 with a 2x2 box filter. The base image is consumed as level 0
  /// (no copy).
  factory Texture.withMipmaps(Uint32List base, int width, int height,
      {TextureWrap wrapU = TextureWrap.clamp,
      TextureWrap wrapV = TextureWrap.clamp}) {
    final levels = <MipLevel>[MipLevel(base, width, height)];
    var w = width;
    var h = height;
    while (w > 1 || h > 1) {
      final nw = math.max(1, w ~/ 2);
      final nh = math.max(1, h ~/ 2);
      levels.add(_downsample(levels.last, nw, nh));
      w = nw;
      h = nh;
    }
    return Texture(levels, wrapU: wrapU, wrapV: wrapV);
  }

  static MipLevel _downsample(MipLevel src, int nw, int nh) {
    final out = Uint32List(nw * nh);
    for (var y = 0; y < nh; y++) {
      for (var x = 0; x < nw; x++) {
        final sx = x * 2;
        final sy = y * 2;
        final a = src.getClamped(sx, sy);
        final b = src.getClamped(sx + 1, sy);
        final c = src.getClamped(sx, sy + 1);
        final d = src.getClamped(sx + 1, sy + 1);
        out[y * nw + x] = _avg4(a, b, c, d);
      }
    }
    return MipLevel(out, nw, nh);
  }

  static int _avg4(int a, int b, int c, int d) {
    final ar = (a >> 16) & 0xff;
    final ag = (a >> 8) & 0xff;
    final ab = a & 0xff;
    final aa = (a >> 24) & 0xff;
    final br = (b >> 16) & 0xff;
    final bg = (b >> 8) & 0xff;
    final bb = b & 0xff;
    final ba = (b >> 24) & 0xff;
    final cr = (c >> 16) & 0xff;
    final cg = (c >> 8) & 0xff;
    final cb = c & 0xff;
    final ca = (c >> 24) & 0xff;
    final dr = (d >> 16) & 0xff;
    final dg = (d >> 8) & 0xff;
    final db = d & 0xff;
    final da = (d >> 24) & 0xff;
    return ((aa + ba + ca + da) ~/ 4) << 24 |
        ((ar + br + cr + dr) ~/ 4) << 16 |
        ((ag + bg + cg + dg) ~/ 4) << 8 |
        (ab + bb + cb + db) ~/ 4;
  }
}

/// A single mip level — a packed ARGB8888 pixel array plus dimensions.
class MipLevel {
  MipLevel(this.pixels, this.width, this.height)
      : assert(pixels.length == width * height);

  /// ARGB8888 packed pixels.
  final Uint32List pixels;

  /// Width in texels.
  final int width;

  /// Height in texels.
  final int height;

  /// Returns the texel at ([x], [y]) with no bounds checking.
  int get(int x, int y) => pixels[y * width + x];

  /// Returns the texel at ([x], [y]) clamped to the edges.
  int getClamped(int x, int y) {
    if (x < 0) x = 0;
    if (y < 0) y = 0;
    if (x >= width) x = width - 1;
    if (y >= height) y = height - 1;
    return pixels[y * width + x];
  }
}

/// Stateless texture sampler. Holds no per-call state — safe to share
/// across rasterizers and threads.
class TextureSampler {
  const TextureSampler();

  /// Samples [texture] at ([u], [v]) using [filter]. Returns ARGB int.
  ///
  /// For [TextureFilter.trilinear], [lodBias] selects the mip level: 0 =
  /// base, 1 = first mip, etc. The rasterizer computes the actual LOD
  /// from screen-space UV derivatives; here we accept a precomputed
  /// value so the sampler stays pure.
  int sample(
    Texture texture,
    double u,
    double v, {
    TextureFilter filter = TextureFilter.bilinear,
    double lod = 0.0,
  }) {
    switch (filter) {
      case TextureFilter.nearest:
        return _sampleNearest(texture, u, v, _selectLevel(texture, lod));
      case TextureFilter.bilinear:
        return _sampleBilinear(texture, u, v, _selectLevel(texture, lod));
      case TextureFilter.trilinear:
        if (texture.mipCount == 1) {
          return _sampleBilinear(texture, u, v, texture.levels.first);
        }
        final lo = lod.floor().clamp(0, texture.mipCount - 1);
        final hi = (lo + 1).clamp(0, texture.mipCount - 1);
        if (lo == hi) {
          return _sampleBilinear(
              texture, u, v, texture.levels[lo]);
        }
        final a =
            _sampleBilinear(texture, u, v, texture.levels[lo]);
        final b =
            _sampleBilinear(texture, u, v, texture.levels[hi]);
        final t = lod - lo;
        return _lerpColor(a, b, t);
    }
  }

  MipLevel _selectLevel(Texture tex, double lod) {
    final li = lod.round().clamp(0, tex.mipCount - 1);
    return tex.levels[li];
  }

  int _sampleNearest(Texture tex, double u, double v, MipLevel level) {
    final uu = _wrap(tex.wrapU, u);
    final vv = _wrap(tex.wrapV, v);
    final x = (uu * level.width).floor() % level.width;
    final y = (vv * level.height).floor() % level.height;
    return level.get(x, y);
  }

  int _sampleBilinear(Texture tex, double u, double v, MipLevel level) {
    final uu = _wrap(tex.wrapU, u);
    final vv = _wrap(tex.wrapV, v);
    final fx = uu * level.width - 0.5;
    final fy = vv * level.height - 0.5;
    final x0 = fx.floor();
    final y0 = fy.floor();
    final tx = fx - x0;
    final ty = fy - y0;
    final x0i = _wrapIndex(tex.wrapU, x0, level.width);
    final x1i = _wrapIndex(tex.wrapU, x0 + 1, level.width);
    final y0i = _wrapIndex(tex.wrapV, y0, level.height);
    final y1i = _wrapIndex(tex.wrapV, y0 + 1, level.height);
    final c00 = level.get(x0i, y0i);
    final c10 = level.get(x1i, y0i);
    final c01 = level.get(x0i, y1i);
    final c11 = level.get(x1i, y1i);
    final top = _lerpColor(c00, c10, tx);
    final bot = _lerpColor(c01, c11, tx);
    return _lerpColor(top, bot, ty);
  }

  double _wrap(TextureWrap mode, double v) {
    switch (mode) {
      case TextureWrap.clamp:
        return v.clamp(0.0, 1.0);
      case TextureWrap.repeat:
        final f = v - v.floor();
        return f == 0.0 && v < 0.0 ? 1.0 : f;
      case TextureWrap.mirror:
        final n = v.floor();
        final f = v - n;
        return (n & 1) == 0 ? f : 1.0 - f;
    }
  }

  int _wrapIndex(TextureWrap mode, int i, int size) {
    if (size == 0) return 0;
    switch (mode) {
      case TextureWrap.clamp:
        return i.clamp(0, size - 1);
      case TextureWrap.repeat:
        var r = i % size;
        if (r < 0) r += size;
        return r;
      case TextureWrap.mirror:
        final period = size * 2;
        var r = i % period;
        if (r < 0) r += period;
        return r < size ? r : period - 1 - r;
    }
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
    final r = (ar + (br - ar) * t).round();
    final g = (ag + (bg - ag) * t).round();
    final bl = (ab + (bb - ab) * t).round();
    final al = (aa + (ba - aa) * t).round();
    return (al << 24) | (r << 16) | (g << 8) | bl;
  }
}
