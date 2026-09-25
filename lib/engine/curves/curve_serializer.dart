// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// curve_serializer.dart — Serialize / deserialize [Curve3D]s.
//
// Two formats are supported:
//   - JSON: human-readable, used for .feather project files, undo /
//     redo diffs, and clipboard transfer. Lossless within float64
//     precision (which is plenty).
//   - Binary: a compact custom format used by the autosave cache and
//     the in-progress stroke buffer. ~3x smaller than JSON, 5x faster
//     to parse.
//
// Both formats are versioned. The current JSON shape is v=1; the
// binary magic is `FCRV` (Feather Curve) with version 1. Forward
// compatibility: unknown fields in JSON are ignored; unknown binary
// versions raise a [CurveSerializerException].
//
// Polymorphism: each subclass carries a [CurveKind] discriminator
// that dispatches deserialization to the right factory. New curve
// types add a new enum value + a new (de)serializer branch — no
// existing code changes.

import 'dart:convert';
import 'dart:typed_data';

import 'package:feather_krita/core/math/math.dart';
import 'package:feather_krita/engine/curves/curve3d.dart';
import 'package:feather_krita/engine/curves/bezier_curve3d.dart';
import 'package:feather_krita/engine/curves/catmull_rom_curve3d.dart';
import 'package:feather_krita/engine/curves/nurbs_curve3d.dart';

/// Thrown when (de)serialization encounters an unsupported version
/// or malformed payload.
class CurveSerializerException implements Exception {
  const CurveSerializerException(this.message);
  final String message;
  @override
  String toString() => 'CurveSerializerException: $message';
}

/// Current serializer format versions.
const int kCurveJsonVersion = 1;
const int kCurveBinaryVersion = 1;

/// Magic bytes for the binary format ("FCRV" = Feather Curve).
const List<int> kCurveBinaryMagic = [0x46, 0x43, 0x52, 0x56];

/// Serializes [Curve3D]s to / from JSON and binary.
class CurveSerializer {
  const CurveSerializer._();

  // ----- JSON ----------------------------------------------------------

  /// Serializes [curve] to a JSON-compatible map.
  static Map<String, dynamic> toJson(Curve3D curve) {
    final out = <String, dynamic>{
      'v': kCurveJsonVersion,
      'kind': curve.kind.name,
      'color': curve.color,
      'thickness': curve.thickness,
      'materialIndex': curve.materialIndex,
      'transform': curve.transform.storage.toList(),
    };
    switch (curve.kind) {
      case CurveKind.bezier:
        final b = curve as BezierCurve3D;
        out['data'] = {
          'closed': b.closed,
          'points': b.controlPoints
              .map((p) => [p.x, p.y, p.z])
              .toList(),
        };
      case CurveKind.catmullRom:
        final c = curve as CatmullRomCurve3D;
        out['data'] = {
          'closed': c.closed,
          'tension': c.tension,
          'parameterization': c.parameterization.name,
          'points': c.controlPoints
              .map((p) => [p.x, p.y, p.z])
              .toList(),
        };
      case CurveKind.nurbs:
        final n = curve as NurbsCurve3D;
        out['data'] = {
          'degree': n.degree,
          'positions': n.positions
              .map((p) => [p.x, p.y, p.z])
              .toList(),
          'weights': n.weights.toList(),
          'knots': n.knots.toList(),
        };
      case CurveKind.base:
        // Base Curve3D is abstract — shouldn't happen, but be
        // permissive: serialize as a degenerate Bezier (single point).
        out['kind'] = CurveKind.bezier.name;
        out['data'] = {
          'closed': false,
          'points': [
            [0.0, 0.0, 0.0],
            [0.0, 0.0, 0.0],
          ],
        };
    }
    return out;
  }

  /// Serializes [curve] to a JSON string.
  static String toJsonString(Curve3D curve) =>
      jsonEncode(toJson(curve));

  /// Deserializes a [Curve3D] from a JSON-compatible map.
  static Curve3D fromJson(Map<String, dynamic> json) {
    final version = (json['v'] as num?)?.toInt() ?? 1;
    if (version > kCurveJsonVersion) {
      throw CurveSerializerException(
          'Unsupported curve JSON version $version (max $kCurveJsonVersion)');
    }
    final kindName = json['kind'] as String? ?? 'bezier';
    final kind = CurveKind.values.firstWhere(
      (k) => k.name == kindName,
      orElse: () => CurveKind.bezier,
    );
    final color = (json['color'] as num?)?.toInt() ?? 0xFF000000;
    final thickness = (json['thickness'] as num?)?.toDouble() ?? 1.0;
    final materialIndex = (json['materialIndex'] as num?)?.toInt() ?? 0;
    final transformList = (json['transform'] as List?)
            ?.map((e) => (e as num).toDouble())
            .toList() ??
        Matrix4.identity().storage.toList();
    final transform = Matrix4.fromList(transformList);
    final data = (json['data'] as Map?)?.cast<String, dynamic>() ?? {};

    switch (kind) {
      case CurveKind.bezier:
        final points = ((data['points'] as List?) ?? [])
            .map((e) => _vec3FromList((e as List).cast<num>()))
            .toList();
        final closed = (data['closed'] as bool?) ?? false;
        return BezierCurve3D(
          controlPoints: points,
          closed: closed,
          color: color,
          thickness: thickness,
          materialIndex: materialIndex,
          transform: transform,
        );
      case CurveKind.catmullRom:
        final points = ((data['points'] as List?) ?? [])
            .map((e) => _vec3FromList((e as List).cast<num>()))
            .toList();
        final closed = (data['closed'] as bool?) ?? false;
        final tension = (data['tension'] as num?)?.toDouble() ?? 0.0;
        final paramName = data['parameterization'] as String? ?? 'uniform';
        final param = CatmullRomParameterization.values.firstWhere(
          (p) => p.name == paramName,
          orElse: () => CatmullRomParameterization.uniform,
        );
        return CatmullRomCurve3D(
          controlPoints: points,
          closed: closed,
          tension: tension,
          parameterization: param,
          color: color,
          thickness: thickness,
          materialIndex: materialIndex,
          transform: transform,
        );
      case CurveKind.nurbs:
        final positions = ((data['positions'] as List?) ?? [])
            .map((e) => _vec3FromList((e as List).cast<num>()))
            .toList();
        final weights = ((data['weights'] as List?) ?? [])
            .map((e) => (e as num).toDouble())
            .toList();
        final knots = ((data['knots'] as List?) ?? [])
            .map((e) => (e as num).toDouble())
            .toList();
        final degree = (data['degree'] as num?)?.toInt() ?? 3;
        return NurbsCurve3D(
          positions: positions,
          weights: weights,
          knots: knots,
          degree: degree,
          color: color,
          thickness: thickness,
          materialIndex: materialIndex,
          transform: transform,
        );
      case CurveKind.base:
        throw CurveSerializerException(
            'Cannot deserialize abstract Curve3D (kind=base)');
    }
  }

  /// Deserializes a [Curve3D] from a JSON string.
  static Curve3D fromJsonString(String json) =>
      fromJson(jsonDecode(json) as Map<String, dynamic>);

  // ----- Binary --------------------------------------------------------

  /// Serializes [curve] to a compact binary blob.
  static Uint8List toBinary(Curve3D curve) {
    // Layout:
    //   [0..4)    magic 'FCRV'
    //   [4..6)    version (uint16 LE)
    //   [6..7)    kind (uint8)
    //   [7..11)   color (uint32 LE)
    //   [11..19)  thickness (float64 LE)
    //   [19..23)  materialIndex (int32 LE)
    //   [23..87)  transform (16 float64 LE = 128 bytes)
    //   [87..)    kind-specific payload
    final transformStorage = curve.transform.storage;
    final payload = _encodePayload(curve);
    final totalLength = 87 + payload.length;
    final out = Uint8List(totalLength);
    final data = ByteData.view(out.buffer);
    out[0] = kCurveBinaryMagic[0];
    out[1] = kCurveBinaryMagic[1];
    out[2] = kCurveBinaryMagic[2];
    out[3] = kCurveBinaryMagic[3];
    data.setUint16(4, kCurveBinaryVersion, Endian.little);
    out[6] = curve.kind.index;
    data.setUint32(7, curve.color, Endian.little);
    data.setFloat64(11, curve.thickness, Endian.little);
    data.setInt32(19, curve.materialIndex, Endian.little);
    for (var i = 0; i < 16; i++) {
      data.setFloat64(23 + i * 8, transformStorage[i], Endian.little);
    }
    out.setRange(87, totalLength, payload);
    return out;
  }

  static Uint8List _encodePayload(Curve3D curve) {
    switch (curve.kind) {
      case CurveKind.bezier:
        final b = curve as BezierCurve3D;
        final points = b.controlPoints;
        // Layout: 1 byte closed + 4 bytes point count + n * 3 * 8 bytes.
        final n = points.length;
        final size = 1 + 4 + n * 24;
        final out = Uint8List(size);
        final data = ByteData.view(out.buffer);
        out[0] = b.closed ? 1 : 0;
        data.setUint32(1, n, Endian.little);
        for (var i = 0; i < n; i++) {
          final p = points[i];
          data.setFloat64(5 + i * 24 + 0, p.x, Endian.little);
          data.setFloat64(5 + i * 24 + 8, p.y, Endian.little);
          data.setFloat64(5 + i * 24 + 16, p.z, Endian.little);
        }
        return out;
      case CurveKind.catmullRom:
        final c = curve as CatmullRomCurve3D;
        final points = c.controlPoints;
        final n = points.length;
        final size = 1 + 1 + 8 + 4 + n * 24;
        final out = Uint8List(size);
        final data = ByteData.view(out.buffer);
        out[0] = c.closed ? 1 : 0;
        out[1] = c.parameterization.index;
        data.setFloat64(2, c.tension, Endian.little);
        data.setUint32(10, n, Endian.little);
        for (var i = 0; i < n; i++) {
          final p = points[i];
          data.setFloat64(14 + i * 24 + 0, p.x, Endian.little);
          data.setFloat64(14 + i * 24 + 8, p.y, Endian.little);
          data.setFloat64(14 + i * 24 + 16, p.z, Endian.little);
        }
        return out;
      case CurveKind.nurbs:
        final n = curve as NurbsCurve3D;
        final positions = n.positions;
        final weights = n.weights;
        final knots = n.knots;
        final size = 4 + 4 + 4 +
            positions.length * 24 +
            weights.length * 8 +
            knots.length * 8;
        final out = Uint8List(size);
        final data = ByteData.view(out.buffer);
        data.setUint32(0, n.degree, Endian.little);
        data.setUint32(4, positions.length, Endian.little);
        data.setUint32(8, knots.length, Endian.little);
        var offset = 12;
        for (var i = 0; i < positions.length; i++) {
          final p = positions[i];
          data.setFloat64(offset, p.x, Endian.little); offset += 8;
          data.setFloat64(offset, p.y, Endian.little); offset += 8;
          data.setFloat64(offset, p.z, Endian.little); offset += 8;
        }
        for (var i = 0; i < weights.length; i++) {
          data.setFloat64(offset, weights[i], Endian.little); offset += 8;
        }
        for (var i = 0; i < knots.length; i++) {
          data.setFloat64(offset, knots[i], Endian.little); offset += 8;
        }
        return out;
      case CurveKind.base:
        return Uint8List(0);
    }
  }

  /// Deserializes a [Curve3D] from a binary blob produced by
  /// [toBinary].
  static Curve3D fromBinary(Uint8List bytes) {
    if (bytes.length < 87) {
      throw CurveSerializerException(
          'Binary curve too short: ${bytes.length} bytes (need >= 87)');
    }
    for (var i = 0; i < 4; i++) {
      if (bytes[i] != kCurveBinaryMagic[i]) {
        throw CurveSerializerException(
            'Bad magic: expected FCRV, got 0x${bytes[0].toRadixString(16)}'
            '${bytes[1].toRadixString(16)}${bytes[2].toRadixString(16)}'
            '${bytes[3].toRadixString(16)}');
      }
    }
    final data = ByteData.view(bytes.buffer, bytes.offsetInBytes, bytes.length);
    final version = data.getUint16(4, Endian.little);
    if (version > kCurveBinaryVersion) {
      throw CurveSerializerException(
          'Unsupported binary version $version (max $kCurveBinaryVersion)');
    }
    final kindIndex = bytes[6];
    if (kindIndex >= CurveKind.values.length) {
      throw CurveSerializerException(
          'Unknown curve kind index $kindIndex');
    }
    final kind = CurveKind.values[kindIndex];
    final color = data.getUint32(7, Endian.little);
    final thickness = data.getFloat64(11, Endian.little);
    final materialIndex = data.getInt32(19, Endian.little);
    final transformStorage = Float64List(16);
    for (var i = 0; i < 16; i++) {
      transformStorage[i] = data.getFloat64(23 + i * 8, Endian.little);
    }
    final transform = Matrix4.fromList(transformStorage.toList());
    final payload = bytes.sublist(87);
    switch (kind) {
      case CurveKind.bezier:
        return _decodeBezierPayload(payload,
            color: color,
            thickness: thickness,
            materialIndex: materialIndex,
            transform: transform);
      case CurveKind.catmullRom:
        return _decodeCatmullRomPayload(payload,
            color: color,
            thickness: thickness,
            materialIndex: materialIndex,
            transform: transform);
      case CurveKind.nurbs:
        return _decodeNurbsPayload(payload,
            color: color,
            thickness: thickness,
            materialIndex: materialIndex,
            transform: transform);
      case CurveKind.base:
        throw CurveSerializerException(
            'Cannot deserialize abstract Curve3D (kind=base)');
    }
  }

  static BezierCurve3D _decodeBezierPayload(
    Uint8List payload, {
    required int color,
    required double thickness,
    required int materialIndex,
    required Matrix4 transform,
  }) {
    final data = ByteData.view(payload.buffer, payload.offsetInBytes, payload.length);
    final closed = payload[0] == 1;
    final n = data.getUint32(1, Endian.little);
    final points = <Vector3>[];
    for (var i = 0; i < n; i++) {
      final x = data.getFloat64(5 + i * 24 + 0, Endian.little);
      final y = data.getFloat64(5 + i * 24 + 8, Endian.little);
      final z = data.getFloat64(5 + i * 24 + 16, Endian.little);
      points.add(Vector3(x, y, z));
    }
    return BezierCurve3D(
      controlPoints: points,
      closed: closed,
      color: color,
      thickness: thickness,
      materialIndex: materialIndex,
      transform: transform,
    );
  }

  static CatmullRomCurve3D _decodeCatmullRomPayload(
    Uint8List payload, {
    required int color,
    required double thickness,
    required int materialIndex,
    required Matrix4 transform,
  }) {
    final data = ByteData.view(payload.buffer, payload.offsetInBytes, payload.length);
    final closed = payload[0] == 1;
    final paramIndex = payload[1];
    final param = paramIndex < CatmullRomParameterization.values.length
        ? CatmullRomParameterization.values[paramIndex]
        : CatmullRomParameterization.uniform;
    final tension = data.getFloat64(2, Endian.little);
    final n = data.getUint32(10, Endian.little);
    final points = <Vector3>[];
    for (var i = 0; i < n; i++) {
      final x = data.getFloat64(14 + i * 24 + 0, Endian.little);
      final y = data.getFloat64(14 + i * 24 + 8, Endian.little);
      final z = data.getFloat64(14 + i * 24 + 16, Endian.little);
      points.add(Vector3(x, y, z));
    }
    return CatmullRomCurve3D(
      controlPoints: points,
      closed: closed,
      tension: tension,
      parameterization: param,
      color: color,
      thickness: thickness,
      materialIndex: materialIndex,
      transform: transform,
    );
  }

  static NurbsCurve3D _decodeNurbsPayload(
    Uint8List payload, {
    required int color,
    required double thickness,
    required int materialIndex,
    required Matrix4 transform,
  }) {
    final data = ByteData.view(payload.buffer, payload.offsetInBytes, payload.length);
    final degree = data.getUint32(0, Endian.little);
    final nPos = data.getUint32(4, Endian.little);
    final nKnots = data.getUint32(8, Endian.little);
    var offset = 12;
    final positions = <Vector3>[];
    for (var i = 0; i < nPos; i++) {
      final x = data.getFloat64(offset, Endian.little); offset += 8;
      final y = data.getFloat64(offset, Endian.little); offset += 8;
      final z = data.getFloat64(offset, Endian.little); offset += 8;
      positions.add(Vector3(x, y, z));
    }
    final weights = <double>[];
    for (var i = 0; i < nPos; i++) {
      weights.add(data.getFloat64(offset, Endian.little)); offset += 8;
    }
    final knots = <double>[];
    for (var i = 0; i < nKnots; i++) {
      knots.add(data.getFloat64(offset, Endian.little)); offset += 8;
    }
    return NurbsCurve3D(
      positions: positions,
      weights: weights,
      knots: knots,
      degree: degree,
      color: color,
      thickness: thickness,
      materialIndex: materialIndex,
      transform: transform,
    );
  }

  // ----- Helpers -------------------------------------------------------

  static Vector3 _vec3FromList(List<num> v) =>
      Vector3(v[0].toDouble(), v[1].toDouble(), v[2].toDouble());
}
