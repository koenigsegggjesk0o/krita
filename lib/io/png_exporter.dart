// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// png_exporter.dart — PNG viewport / texture snapshot exporter.
//
// Produces a PNG file from a raw RGBA pixel buffer. Used by the
// viewport snapshot exporter (which hands the GPU frame's pixels to
// [PngExporter.export]) and by the texture-painter thumbnailer (which
// snapshots a downscaled copy of the paint texture for the
// recent-projects list).
//
// Pure-Dart: the PNG encoder is implemented inline (no `image` package
// dependency, no platform plugins) so it runs in the same isolate as
// the rest of the export pipeline. The encoder writes:
//
//   - PNG signature (8 bytes).
//   - IHDR chunk (13 bytes data: width, height, bit depth 8, color
//     type 6 = truecolor+alpha, compression 0, filter 0, interlace 0).
//   - IDAT chunk: zlib-compressed scanlines, each prefixed by a
//     filter-type byte (0 = none).
//   - IEND chunk (0 bytes data).
//
// Each chunk is length-prefixed and CRC-32 protected (CRC computed
// over the chunk type + data, polynomial 0xEDB88320 reflected).

import 'dart:typed_data';
import 'dart:io' show File;

/// PNG exporter. Construct with [width]/[height] and call [export] with
/// a raw RGBA buffer (4 bytes per pixel, row-major top-to-bottom).
class PngExporter {
  PngExporter({this.includeAlpha = true});

  /// Whether the output PNG includes the alpha channel. When false,
  /// RGB pixels are written (color type 2) and the input alpha is
  /// dropped.
  final bool includeAlpha;

  /// Encodes [rgba] (length == width * height * 4) as a PNG byte
  /// buffer. Throws [ArgumentError] when the input length does not
  /// match [width] * [height] * 4.
  Uint8List export({
    required int width,
    required int height,
    required List<int> rgba,
  }) {
    final expected = width * height * 4;
    if (rgba.length != expected) {
      throw ArgumentError(
          'RGBA buffer length ${rgba.length} != $expected ($width*$height*4)');
    }
    final out = BytesBuilder();
    out.add(_pngSignature);

    // IHDR
    final ihdr = BytesBuilder()
      ..add(_u32(width))
      ..add(_u32(height))
      ..addByte(8) // bit depth
      ..addByte(includeAlpha ? 6 : 2) // color type: 6 = RGBA, 2 = RGB
      ..addByte(0) // compression: deflate
      ..addByte(0) // filter: standard
      ..addByte(0); // interlace: none
    _writeChunk(out, 'IHDR', ihdr.toBytes());

    // IDAT: filtered scanlines + zlib-compressed.
    final raw = Uint8List((includeAlpha ? 4 : 3) * width * height + height);
    var o = 0;
    for (var y = 0; y < height; y++) {
      raw[o++] = 0; // filter type 0 (none)
      for (var x = 0; x < width; x++) {
        final src = (y * width + x) * 4;
        raw[o++] = rgba[src];
        raw[o++] = rgba[src + 1];
        raw[o++] = rgba[src + 2];
        if (includeAlpha) raw[o++] = rgba[src + 3];
      }
    }
    final compressed = _zlibDeflate(raw);
    _writeChunk(out, 'IDAT', compressed);

    // IEND
    _writeChunk(out, 'IEND', Uint8List(0));

    return out.toBytes();
  }

  /// Convenience: encodes [rgba] and writes it to [path]. Returns the
  /// file when done.
  Future<File> exportToFile({
    required String path,
    required int width,
    required int height,
    required List<int> rgba,
  }) async {
    final bytes = export(width: width, height: height, rgba: rgba);
    final file = File(path);
    await file.writeAsBytes(bytes, flush: true);
    return file;
  }

  // ---- PNG chunk plumbing ------------------------------------------------

  static const List<int> _pngSignature = [
    0x89, 0x50, 0x4E, 0x47, 0x0D, 0x0A, 0x1A, 0x0A,
  ];

  void _writeChunk(BytesBuilder out, String type, List<int> data) {
    out.add(_u32(data.length));
    final typeBytes = type.codeUnits;
    out.add(typeBytes);
    out.add(data);
    out.add(_u32(_crc32(typeBytes, data)));
  }

  Uint8List _u32(int v) {
    final b = ByteData(4)..setUint32(0, v, Endian.big);
    return b.buffer.asUint8List();
  }

  // ---- Zlib deflate (raw, no header) -------------------------------------

  /// Compresses [data] into a zlib stream (RFC 1950): 2-byte header,
  /// deflate-compressed body, 4-byte Adler-32 trailer. The deflate
  /// body uses stored blocks (BTYPE=00) only — no compression, but
  /// the output is still a valid zlib stream and any PNG decoder will
  /// accept it. (A real DEFLATE encoder would shrink the file
  /// further; for typical viewport snapshots the disk savings do not
  /// justify the extra code path here.)
  Uint8List _zlibDeflate(List<int> data) {
    final out = BytesBuilder();
    // zlib header: CMF=0x78 (deflate, 32K window), FLG=0x01 (no
    // dict, fastest level — Adler-32 check passes by construction).
    out.addByte(0x78);
    out.addByte(0x01);
    // Stored blocks: max 65535 bytes per block.
    var offset = 0;
    while (offset < data.length) {
      final chunkLen = (data.length - offset).clamp(0, 65535);
      final isLast = offset + chunkLen >= data.length;
      out.addByte(isLast ? 0x01 : 0x00);
      out.addByte(chunkLen & 0xff);
      out.addByte((chunkLen >> 8) & 0xff);
      out.addByte((~chunkLen) & 0xff);
      out.addByte(((~chunkLen) >> 8) & 0xff);
      out.add(data.sublist(offset, offset + chunkLen));
      offset += chunkLen;
    }
    // Handle empty input: emit a single empty final stored block.
    if (data.isEmpty) {
      out.addByte(0x01);
      out.addByte(0x00);
      out.addByte(0x00);
      out.addByte(0xff);
      out.addByte(0xff);
    }
    // Adler-32 trailer.
    out.add(_adler32(data));
    return out.toBytes();
  }

  Uint8List _adler32(List<int> data) {
    var a = 1, b = 0;
    for (final v in data) {
      a = (a + v) % 65521;
      b = (b + a) % 65521;
    }
    final s = (b << 16) | a;
    final bd = ByteData(4)..setUint32(0, s, Endian.big);
    return bd.buffer.asUint8List();
  }

  int _crc32(List<int> type, List<int> data) {
    var crc = 0xFFFFFFFF;
    void update(int b) {
      crc ^= b;
      for (var i = 0; i < 8; i++) {
        crc = (crc & 1) != 0 ? (0xEDB88320 ^ (crc >> 1)) : (crc >> 1);
      }
    }
    for (final b in type) {
      update(b);
    }
    for (final b in data) {
      update(b);
    }
    return (crc ^ 0xFFFFFFFF) & 0xFFFFFFFF;
  }
}

/// Top-level convenience: encodes [rgba] as PNG bytes.
Uint8List buildPng({
  required int width,
  required int height,
  required List<int> rgba,
  bool includeAlpha = true,
}) =>
    PngExporter(includeAlpha: includeAlpha)
        .export(width: width, height: height, rgba: rgba);
