// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// feather_project.dart — The .feather project document format.
//
// A project file is a single JSON document with:
//   version        — format version (1 = loop-11 writer, 2 = adds per-point
//                    UVs + nextId + mirror snapshot)
//   fileName       — display name (with .feather extension)
//   texture        — {width, height} of the paint texture
//   guideSurface   — display name of the guide surface type
//   brush          — {preset, size, opacity, color, mirrorX/Y/Z}
//   strokes        — the full stroke-manager snapshot (strokes, selectedIds,
//                    nextId, mirror) — a superset of StrokeManager's own
//                    serialization, so [FeatherProjectDocument.applyTo] can
//                    rebuild stroke history through StrokeManager.fromJsonString.
//   camera         — OPTIONAL loop-18 extension: {yaw, pitch, distance,
//                    target:[x,y,z]} orbit-camera pose (radians / world
//                    units). Written by fromEditor since loop-18; older app
//                    builds ignore the unknown key, and documents without it
//                    leave the camera untouched on load. Format version stays
//                    2 because the extension is purely additive.
//   light          — OPTIONAL loop-53 extension: {azimuthDeg, elevationDeg,
//                    intensity} — the scene key-light rig's sun sky position
//                    (degrees, azimuth normalized to [0, 360), elevation in
//                    [-30, +88]) plus the diffuse intensity in [0, 1]. Same
//                    additive contract as the camera block: older builds
//                    ignore the unknown key; documents without it leave the
//                    live rig untouched on load (the rig defaults reproduce
//                    the legacy fixed key light exactly). The rig's setters
//                    clamp elevation and intensity, so out-of-range values
//                    in a hand-edited file can never degenerate the light.
//                    Az/el (not the derived direction vector) is what gets
//                    stored, so the JSON round-trip involves no trig.
//   guideShape     — OPTIONAL loop-59 extension: {"params": {...}} — the
//                    canonical construction dimensions of the guide
//                    surface (radius/segments/rings for a sphere, etc.),
//                    captured from the live surface by fromEditor. Same
//                    additive contract as camera/light: older builds
//                    ignore the unknown key; documents without it rebuild
//                    the type's editor-standard shape exactly as before.
//                    On load the params are applied ONLY when the stored
//                    guideSurface name resolves to a known type — an
//                    unknown name (parity fallback) deliberately skips
//                    them so the rebuilt default matches what the open
//                    dialog's parity strip promises. forTypeWithParams
//                    floors/caps tesselation counts and mirrors the
//                    factories' .abs() on dimensions, so hand-edited
//                    values can never degenerate the mesh.
//
// Parsing is tolerant: missing optional fields fall back to sane defaults
// so v1 documents keep loading after the v2 writer ships.

import 'dart:convert';
import 'dart:typed_data' show ByteData, Endian, Uint8List;

import 'package:vector_math/vector_math_64.dart' show Vector3;

import 'package:feather_krita/data/models/project_model.dart'
    show ProjectModel;
import 'package:feather_krita/engine/guide_surface.dart';
import 'package:feather_krita/engine/stroke_replay.dart';
import 'package:feather_krita/models/stroke.dart';
import 'package:feather_krita/state/editor_state.dart';

/// Current on-disk format version written by
/// [FeatherProjectDocument.fromEditor].
const int kFeatherProjectVersion = 2;

/// A parsed .feather project document.
class FeatherProjectDocument {
  FeatherProjectDocument({
    this.version = kFeatherProjectVersion,
    this.fileName = 'Untitled.feather',
    this.textureWidth = 2048,
    this.textureHeight = 2048,
    this.guideSurfaceName = 'Sphere',
    this.brushPreset = 'Basic Round',
    this.brushSize = 32.0,
    this.brushOpacity = 1.0,
    this.brushColor = 0xFF1A1A1A,
    this.mirrorX = false,
    this.mirrorY = false,
    this.mirrorZ = false,
    this.cameraYaw,
    this.cameraPitch,
    this.cameraDistance,
    this.cameraTargetX,
    this.cameraTargetY,
    this.cameraTargetZ,
    this.lightAzimuthDeg,
    this.lightElevationDeg,
    this.lightIntensity,
    this.guideShapeParams,
    List<Stroke>? strokes,
  }) : strokes = strokes ?? <Stroke>[];

  final int version;
  final String fileName;
  final int textureWidth;
  final int textureHeight;
  final String guideSurfaceName;
  final String brushPreset;
  final double brushSize;
  final double brushOpacity;
  final int brushColor;
  final bool mirrorX;
  final bool mirrorY;
  final bool mirrorZ;

  /// Optional orbit-camera pose (loop-18). Null when the document has no
  /// camera block; [applyTo] leaves the live camera untouched then.
  final double? cameraYaw;
  final double? cameraPitch;
  final double? cameraDistance;
  final double? cameraTargetX;
  final double? cameraTargetY;
  final double? cameraTargetZ;

  /// True when all six camera fields were present in the document.
  bool get hasCamera =>
      cameraYaw != null &&
      cameraPitch != null &&
      cameraDistance != null &&
      cameraTargetX != null &&
      cameraTargetY != null &&
      cameraTargetZ != null;

  /// Optional key-light rig sky position (loop-53). Null when the document
  /// has no light block; [applyTo] leaves the live rig untouched then.
  final double? lightAzimuthDeg;
  final double? lightElevationDeg;
  final double? lightIntensity;

  /// True when all three light fields were present in the document.
  bool get hasLight =>
      lightAzimuthDeg != null &&
      lightElevationDeg != null &&
      lightIntensity != null;

  /// Optional guide-surface shape dimensions (loop-59). Null when the
  /// document has no guideShape block; [applyTo] then rebuilds the
  /// surface exactly as it did before this extension existed.
  final Map<String, num>? guideShapeParams;

  /// True when a non-empty guideShape params map was present.
  bool get hasGuideShape =>
      guideShapeParams != null && guideShapeParams!.isNotEmpty;

  final List<Stroke> strokes;

  /// The guide surface type for [guideSurfaceName].
  GuideSurfaceType get surfaceType =>
      guideSurfaceTypeFromName(guideSurfaceName);

  /// Parses a project document from [json]. Throws [FormatException] when
  /// the payload is not a JSON object or comes from an incompatible future
  /// version.
  factory FeatherProjectDocument.parse(String json) {
    final dynamic decoded = jsonDecode(json);
    if (decoded is! Map<String, dynamic>) {
      throw const FormatException('Project root must be a JSON object.');
    }
    final version = (decoded['version'] as num?)?.toInt() ?? 1;
    if (version > kFeatherProjectVersion) {
      throw FormatException(
        'Project was saved by a newer app version '
        '(format v$version, supported v$kFeatherProjectVersion).',
      );
    }
    final texture = decoded['texture'];
    final brush = decoded['brush'];
    final cam = decoded['camera'];
    final light = decoded['light'];
    final shape = decoded['guideShape'];
    final strokesJson = decoded['strokes'];
    final strokeList = strokesJson is Map<String, dynamic>
        ? (strokesJson['strokes'] as List? ?? [])
        : (strokesJson is List ? strokesJson : []);
    return FeatherProjectDocument(
      version: version,
      fileName: decoded['fileName'] as String? ?? 'Untitled.feather',
      textureWidth: texture is Map<String, dynamic>
          ? (texture['width'] as num?)?.toInt() ?? 2048
          : 2048,
      textureHeight: texture is Map<String, dynamic>
          ? (texture['height'] as num?)?.toInt() ?? 2048
          : 2048,
      guideSurfaceName: decoded['guideSurface'] as String? ?? 'Sphere',
      brushPreset: brush is Map<String, dynamic>
          ? brush['preset'] as String? ?? 'Basic Round'
          : 'Basic Round',
      brushSize: brush is Map<String, dynamic>
          ? (brush['size'] as num?)?.toDouble() ?? 32.0
          : 32.0,
      brushOpacity: brush is Map<String, dynamic>
          ? (brush['opacity'] as num?)?.toDouble() ?? 1.0
          : 1.0,
      brushColor: brush is Map<String, dynamic>
          ? (brush['color'] as num?)?.toInt() ?? 0xFF1A1A1A
          : 0xFF1A1A1A,
      mirrorX: brush is Map<String, dynamic>
          ? brush['mirrorX'] as bool? ?? false
          : false,
      mirrorY: brush is Map<String, dynamic>
          ? brush['mirrorY'] as bool? ?? false
          : false,
      mirrorZ: brush is Map<String, dynamic>
          ? brush['mirrorZ'] as bool? ?? false
          : false,
      cameraYaw:
          cam is Map<String, dynamic> ? (cam['yaw'] as num?)?.toDouble() : null,
      cameraPitch: cam is Map<String, dynamic>
          ? (cam['pitch'] as num?)?.toDouble()
          : null,
      cameraDistance: cam is Map<String, dynamic>
          ? (cam['distance'] as num?)?.toDouble()
          : null,
      cameraTargetX: cam is Map<String, dynamic>
          ? _camTargetAt(cam, 0)
          : null,
      cameraTargetY: cam is Map<String, dynamic>
          ? _camTargetAt(cam, 1)
          : null,
      cameraTargetZ: cam is Map<String, dynamic>
          ? _camTargetAt(cam, 2)
          : null,
      lightAzimuthDeg: light is Map<String, dynamic>
          ? (light['azimuthDeg'] as num?)?.toDouble()
          : null,
      lightElevationDeg: light is Map<String, dynamic>
          ? (light['elevationDeg'] as num?)?.toDouble()
          : null,
      lightIntensity: light is Map<String, dynamic>
          ? (light['intensity'] as num?)?.toDouble()
          : null,
      guideShapeParams: _shapeParamsFromJson(
        shape is Map<String, dynamic> ? shape['params'] : null,
      ),
      strokes: strokeList
          .whereType<Map<String, dynamic>>()
          .map(Stroke.fromJson)
          .toList(),
    );
  }

  /// Captures the current editor state as a project document.
  factory FeatherProjectDocument.fromEditor(EditorState state) {
    return FeatherProjectDocument(
      fileName: state.fileName,
      textureWidth: state.texture.width,
      textureHeight: state.texture.height,
      guideSurfaceName: guideSurfaceTypeName(state.guideSurface.type),
      brushPreset: state.brushPresetName,
      brushSize: state.brushSize,
      brushOpacity: state.brushOpacity,
      brushColor: state.brushColor,
      mirrorX: state.mirrorX,
      mirrorY: state.mirrorY,
      mirrorZ: state.mirrorZ,
      cameraYaw: state.camera.yaw,
      cameraPitch: state.camera.pitch,
      cameraDistance: state.camera.distance,
      cameraTargetX: state.camera.target.x,
      cameraTargetY: state.camera.target.y,
      cameraTargetZ: state.camera.target.z,
      lightAzimuthDeg: state.lightRig.sunAzimuthDeg,
      lightElevationDeg: state.lightRig.sunElevationDeg,
      lightIntensity: state.lightRig.intensity,
      guideShapeParams: state.guideSurface.shapeParams.isEmpty
          ? null
          : Map<String, num>.of(state.guideSurface.shapeParams),
      strokes: state.strokes.strokes.map((s) => s.copy()).toList(),
    );
  }

  /// Serializes to the on-disk JSON representation.
  String toJsonString() {
    String esc(String s) => s.replaceAll('"', r'\"');
    final nextId = strokes.fold<int>(1, (m, s) => m > s.id ? m : s.id + 1);
    final strokesJson = jsonEncode(strokes.map((s) => s.toJson()).toList());
    final cameraJson = hasCamera
        ? '"camera":{"yaw":${_num(cameraYaw!)},'
            '"pitch":${_num(cameraPitch!)},'
            '"distance":${_num(cameraDistance!)},'
            '"target":[${_num(cameraTargetX!)},'
            '${_num(cameraTargetY!)},${_num(cameraTargetZ!)}]},'
        : '';
    final lightJson = hasLight
        ? '"light":{"azimuthDeg":${_num(lightAzimuthDeg!)},'
            '"elevationDeg":${_num(lightElevationDeg!)},'
            '"intensity":${_num(lightIntensity!)}},'
        : '';
    final shapeJson = hasGuideShape
        ? '"guideShape":{"params":${jsonEncode(guideShapeParams!)}},'
        : '';
    return '{"version":$version,'
        '"fileName":"${esc(fileName)}",'
        '"texture":{"width":$textureWidth,"height":$textureHeight},'
        '"guideSurface":"${esc(guideSurfaceName)}",'
        '$shapeJson'
        '"brush":{"preset":"${esc(brushPreset)}",'
        '"size":${brushSize.toStringAsFixed(2)},'
        '"opacity":${brushOpacity.toStringAsFixed(3)},'
        '"color":$brushColor,'
        '"mirrorX":$mirrorX,"mirrorY":$mirrorY,"mirrorZ":$mirrorZ},'
        '$cameraJson'
        '$lightJson'
        '"strokes":{"strokes":$strokesJson,'
        '"selectedIds":[],"nextId":$nextId}}';
  }

  /// Compact JSON number: drops the trailing .0 on integral doubles so
  /// the emitted camera block matches the hand-rolled style above.
  static String _num(double v) {
    final i = v.roundToDouble();
    return v == i ? i.toInt().toString() : v.toStringAsFixed(6);
  }

  /// Restores this document into [state].
  ///
  /// Stroke data is applied through a StrokeManager snapshot so the load
  /// itself is undoable and mirror copies / ids are restored exactly as
  /// saved. Brush parameters and the guide surface are applied first so
  /// the restored document renders with the saved settings.
  void applyTo(EditorState state) {
    state
      ..fileName = fileName
      ..setBrushSize(brushSize)
      ..setBrushOpacity(brushOpacity)
      ..setBrushColor(brushColor)
      ..setBrushPresetName(brushPreset);
    // Restore the guide surface (loop-59). When the document carries the
    // additive guideShape block AND the stored surface name resolves to a
    // known type, rebuild through the stored dimensions. An unknown name
    // (parity fallback) or a missing block keeps the exact pre-loop-59
    // behavior — the type's editor-standard shape — matching what the
    // open dialog's parity strip discloses.
    final knownSurfaceName = guideSurfaceTypeFromNameStrict(guideSurfaceName);
    state.setGuideSurface(
      hasGuideShape && knownSurfaceName != null
          ? GuideSurface.forTypeWithParams(surfaceType, guideShapeParams!)
          : GuideSurface.forType(surfaceType),
    );

    final nextId = strokes.fold<int>(1, (m, s) => m > s.id ? m : s.id + 1);
    final snapshot = jsonEncode({
      'strokes': strokes.map((s) => s.toJson()).toList(),
      'selectedIds': <int>[],
      'nextId': nextId,
      'mirror': {
        'x': mirrorX,
        'y': mirrorY,
        'z': mirrorZ,
        'origin': [0.0, 0.0, 0.0],
      },
    });
    // One unified journal entry covers the whole load: the pre-load
    // strokes JSON AND the pre-load pixels, so undoing a project open
    // restores both (loop-20; before, undo only restored the strokes).
    state.captureUndo(withTexture: true);
    state.strokes.fromJsonString(snapshot, recordUndo: false);

    // Restore the canvas pixels: the .feather format stores no bitmap, so
    // the texture is re-rendered from the strokes (loop-14). Before this,
    // opening a project left the PREVIOUS document's pixels on screen.
    // The whole replay shares ONE texture-internal transaction so replay
    // dabs push nothing per dab; the transaction snapshot is dropped
    // right after (clearHistory) because the journal owns history now.
    final tex = state.texture;
    tex.beginStrokeUndo();
    tex.clear(pushUndo: false);
    replayStrokesIntoTexture(
      strokes: state.strokes.strokes,
      texture: tex,
      dabFor: state.replayDab,
      brushSizePx: brushSize,
      brushOpacity: brushOpacity,
      sourceTextureSize: textureWidth,
      // Honor each stroke's recorded thickness (source == target size).
      sizePxForStroke: (stroke, defaultPx) =>
          stroke.thickness > 0 ? stroke.thickness : defaultPx,
    );
    tex.endStrokeUndo();
    // The journal entry above owns the pre-load state; drop the texture-
    // internal transaction snapshot so it cannot leak 16 MB per open.
    tex.clearHistory();

    state
      ..mirrorX = mirrorX
      ..mirrorY = mirrorY
      ..mirrorZ = mirrorZ;

    // Restore the saved orbit-camera pose (loop-18). snapTo jumps both
    // the damped and target values so opening a project doesn't play a
    // fly-in animation. Documents saved before loop-18 have no camera
    // block and leave the live camera as-is.
    if (hasCamera) {
      state.camera.snapTo(
        target: Vector3(cameraTargetX!, cameraTargetY!, cameraTargetZ!),
        yaw: cameraYaw!,
        pitch: cameraPitch!,
        distance: cameraDistance!,
      );
    }

    // Restore the saved key-light rig (loop-53). setSunAzimuthElevationDeg
    // clamps the elevation into the valid arc and setIntensity clamps to
    // [0, 1], so out-of-range or corrupt values can never degenerate the
    // light direction. Documents saved before loop-53 have no light block
    // and leave the live rig untouched (its defaults reproduce the legacy
    // fixed key light exactly).
    if (hasLight) {
      state.lightRig
        ..setSunAzimuthElevationDeg(lightAzimuthDeg!, lightElevationDeg!)
        ..setIntensity(lightIntensity!);
    }

    state.notify();
  }
}

/// Reads [index] of the camera target array in a parsed camera block,
/// returning null when the array is missing or too short.
double? _camTargetAt(Map<String, dynamic> cam, int index) {
  final t = cam['target'];
  if (t is! List || t.length <= index) return null;
  final v = t[index];
  return v is num ? v.toDouble() : null;
}

/// Extracts the guideShape params map (loop-59) from a parsed document:
/// only finite numbers survive, and an absent/invalid/empty map yields
/// null so the load behaves exactly like a pre-loop-59 document.
Map<String, num>? _shapeParamsFromJson(dynamic raw) {
  if (raw is! Map) return null;
  final out = <String, num>{};
  raw.forEach((k, v) {
    if (v is num && v.isFinite) out[k.toString()] = v;
  });
  return out.isEmpty ? null : out;
}

// =========================================================================
// Binary .featherb serialization (feather-state-data-export).
//
// The legacy [FeatherProjectDocument] above is a JSON writer/reader. For
// large projects (many strokes, big thumbnails) the JSON round-trip is
// slow and verbose; this binary container is the compact alternative.
//
// Layout (all integers little-endian, all lengths are uint32):
//
//   offset 0   : magic  'FEB1'             (4 bytes)
//   offset 4   : format version            (uint32, currently 1)
//   offset 8   : JSON header length        (uint32)
//   offset 12  : JSON header bytes         (UTF-8, [ProjectModel.toJson] minus
//                                            thumbnail bytes)
//   offset N   : thumbnail length          (uint32)
//   offset N+4 : thumbnail bytes           (raw PNG, optional)
//
// The header JSON carries the same logical content as the JSON format
// (scene + camera + brush + metadata); the thumbnail is the only field
// that gets its own length-prefixed bin chunk so the JSON stays small.
//
// The container is forward-compatible: newer writers may append extra
// chunks after the thumbnail; older readers stop at the first chunk
// they don't recognize. [isBinary] is the entry-point sniff used by
// [ProjectRepository.load] to pick the right reader.
// =========================================================================

/// 4-byte magic identifying a binary .featherb file.
const List<int> kFeatherBinaryMagic = [0x46, 0x45, 0x42, 0x31]; // 'FEB1'

/// Binary .featherb serialization. See file header above for the layout.
class FeatherProjectBinary {
  FeatherProjectBinary._();

  /// Returns true when [bytes] starts with the binary magic.
  static bool isBinary(List<int> bytes) {
    if (bytes.length < 4) return false;
    return bytes[0] == 0x46 &&
        bytes[1] == 0x45 &&
        bytes[2] == 0x42 &&
        bytes[3] == 0x31;
  }

  /// Serializes [model] to a binary .featherb byte buffer.
  static Uint8List toBytes(ProjectModel model) {
    final headerJson = _jsonEncode(model);
    final headerBytes = utf8.encode(headerJson);
    final thumbBytes = model.thumbnail ?? const <int>[];
    final totalLength = 4 + 4 + 4 + headerBytes.length + 4 + thumbBytes.length;
    final out = Uint8List(totalLength);
    final bd = ByteData.sublistView(out);
    var o = 0;
    out[o++] = 0x46; // 'F'
    out[o++] = 0x45; // 'E'
    out[o++] = 0x42; // 'B'
    out[o++] = 0x31; // '1'
    bd.setUint32(o, 1, Endian.little); // format version
    o += 4;
    bd.setUint32(o, headerBytes.length, Endian.little);
    o += 4;
    out.setRange(o, o + headerBytes.length, headerBytes);
    o += headerBytes.length;
    bd.setUint32(o, thumbBytes.length, Endian.little);
    o += 4;
    out.setRange(o, o + thumbBytes.length, thumbBytes);
    return out;
  }

  /// Parses a binary .featherb buffer back to a [ProjectModel].
  /// Throws [FormatException] when [bytes] does not start with the
  /// magic or comes from an incompatible future version.
  static ProjectModel fromBytes(List<int> bytes) {
    if (!isBinary(bytes)) {
      throw const FormatException(
          'Not a Feather binary project (magic FEB1 missing).');
    }
    final data = bytes is Uint8List ? bytes : Uint8List.fromList(bytes);
    final bd = ByteData.sublistView(data);
    var o = 4;
    final version = bd.getUint32(o, Endian.little);
    o += 4;
    if (version > 1) {
      throw FormatException(
        'Project saved by a newer app version '
        '(binary v$version, supported v1).',
      );
    }
    final headerLen = bd.getUint32(o, Endian.little);
    o += 4;
    final headerBytes = data.sublist(o, o + headerLen);
    o += headerLen;
    final headerJson = utf8.decode(headerBytes, allowMalformed: true);
    final decoded = jsonDecode(headerJson);
    if (decoded is! Map<String, dynamic>) {
      throw const FormatException(
          'Binary project header is not a JSON object.');
    }
    final model = ProjectModel.fromJson(decoded);
    if (o + 4 > data.length) return model;
    final thumbLen = bd.getUint32(o, Endian.little);
    o += 4;
    if (thumbLen > 0 && o + thumbLen <= data.length) {
      final thumb = Uint8List.fromList(data.sublist(o, o + thumbLen));
      return model.copyWith(thumbnail: thumb);
    }
    return model;
  }

  /// Reads ONLY the thumbnail chunk from [bytes] without parsing the
  /// header JSON. Returns `null` when there is no thumbnail.
  static Uint8List? readThumbnail(List<int> bytes) {
    if (!isBinary(bytes)) return null;
    final data = bytes is Uint8List ? bytes : Uint8List.fromList(bytes);
    final bd = ByteData.sublistView(data);
    var o = 4;
    final version = bd.getUint32(o, Endian.little);
    o += 4;
    if (version > 1) return null;
    final headerLen = bd.getUint32(o, Endian.little);
    o += 4;
    o += headerLen;
    if (o + 4 > data.length) return null;
    final thumbLen = bd.getUint32(o, Endian.little);
    o += 4;
    if (thumbLen == 0 || o + thumbLen > data.length) return null;
    return Uint8List.fromList(data.sublist(o, o + thumbLen));
  }

  /// Pretty-prints the model's JSON header (used by [toBytes]).
  static String _jsonEncode(ProjectModel model) =>
      const JsonEncoder.withIndent('  ').convert(model.toJson());
}
