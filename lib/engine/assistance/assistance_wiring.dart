// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// assistance_wiring.dart — pure helpers that bridge the four assistance
// subsystems (Stable Strokes, ColorSampler, MirrorAssist, DrawShapeAssist)
// to the live paint path owned by `lib/screens/main_screen.dart`.
//
// Each helper is a small, side-effect-free function over the existing
// engine types so it can be unit-tested in isolation
// (see `test/engine/assistance/assistance_wiring_test.dart`). The host
// calls these from its stroke-update / stroke-end / tap handlers; the
// helpers never touch Flutter widgets or host-owned mutable state.
//
// Scope: this file is the ONLY new code under `lib/engine/assistance/`
// for the v0.54-A wiring task. The four existing modules
// (stable_strokes.dart, mirror_assist.dart, draw_shape_assist.dart) and
// `lib/engine/color/color_sampler.dart` stay untouched — this file just
// composes their public APIs.

import 'dart:math' as math;

import 'package:vector_math/vector_math_64.dart';

import 'package:feather_krita/engine/assistance/draw_shape_assist.dart';
import 'package:feather_krita/engine/assistance/mirror_assist.dart';
import 'package:feather_krita/engine/assistance/stable_strokes.dart';
import 'package:feather_krita/engine/color/color_sampler.dart';
import 'package:feather_krita/engine/curves/stroke3d.dart';
import 'package:feather_krita/models/stroke.dart';

// ----- Stable Strokes ------------------------------------------------------

/// Pushes a raw world-space sample through the stabilizer and returns the
/// smoothed world position (or `null` when the filter is still warming
/// up — see [StableStrokes.push]).
///
/// The host calls this on every `onStrokeUpdate` BEFORE appending to the
/// live stroke / Stroke3D capture so the committed curve geometry is the
/// stabilizer's output, not the raw pen path. When the stabilizer is
/// disabled (or returns `null` during warm-up) the host should fall back
/// to the raw [world] so the stroke stays continuous.
Vector3? smoothStrokeSample(
  StableStrokes stabilizer,
  Vector3 world, {
  double pressure = 0.5,
  double time = 0.0,
}) {
  final out = stabilizer.push(StableSample(
    position: world.clone(),
    pressure: pressure,
    time: time,
  ));
  return out?.position.clone();
}

// ----- ColorSampler --------------------------------------------------------

/// Default world-space pick radius (mm) for the eye-dropper tap. Matches
/// the [ColorSampler.defaultPickRadius] but exposed here so the host and
/// the tests agree on a single constant.
const double kEyedropperPickRadius = 4.0;

/// Samples the ARGB colour of the nearest visible stroke sample to
/// [world] in [strokes]. Returns `null` when no stroke is within the
/// pick radius (the host keeps the active colour unchanged in that case).
///
/// Thin wrapper around [ColorSampler.sampleFromCurves] so the host's
/// eye-dropper tap handler is a one-liner and the wiring is testable
/// without a live canvas.
int? sampleColorAt(
  List<Stroke> strokes,
  Vector3 world, {
  double radius = kEyedropperPickRadius,
  ColorSampler? sampler,
}) {
  final s = sampler ?? ColorSampler();
  return s.sampleFromCurves(strokes, world, maxDistance: radius);
}

// ----- MirrorAssist --------------------------------------------------------

/// Builds the mirrored copies of [source] (excluding the identity copy,
/// which is the source itself) using [assist.reflectionTransforms].
///
/// Each returned [Stroke3D] is a fresh instance carrying the reflected
/// sample positions, with the same colour / thickness / material / input
/// device / source as [source]. Pressure / tilt / time / uv are copied
/// per-sample so the mirrored strokes render with the same brush
/// dynamics as the original. The transform is reset to identity (the
/// reflected positions are baked into the samples so the stroke is
/// independently editable — per the mirror_assist.dart doc: "the stroke
/// manager registers each copy as its own curve in the active group").
///
/// Returns an empty list when mirror is disabled (no active axes).
List<Stroke3D> mirrorStroke(Stroke3D source, MirrorAssist assist) {
  if (!assist.isEnabled) return const <Stroke3D>[];
  final transforms = assist.reflectionTransforms();
  if (transforms.length <= 1) return const <Stroke3D>[];
  final out = <Stroke3D>[];
  // Skip the first transform (identity = the source itself).
  for (var i = 1; i < transforms.length; i++) {
    final m = transforms[i];
    final reflectedSamples = <StrokeSample3D>[];
    for (final sample in source.samples) {
      final p = m.transformed3(sample.position.clone());
      reflectedSamples.add(StrokeSample3D(
        position: p,
        pressure: sample.pressure,
        tilt: sample.tilt.clone(),
        time: sample.time,
        uv: sample.uv?.clone(),
        velocity: sample.velocity,
      ));
    }
    out.add(Stroke3D(
      samples: reflectedSamples,
      color: source.color,
      thickness: source.thickness,
      materialIndex: source.materialIndex,
      inputDevice: source.inputDevice,
      source: '${source.source}#mirror-$i',
      closed: source.closed,
    ));
  }
  return out;
}

// ----- DrawShapeAssist -----------------------------------------------------

/// The shape-correction mode the host applies on stroke commit when the
/// Draw Shape tool is active. `auto` lets [DrawShapeAssist.snap]
/// auto-detect (line / circle / arc / freehand); `line` and `circle`
/// force the corresponding primitive regardless of the auto-detect
/// residual.
enum DrawShapeMode { auto, line, circle }

/// Snaps [source]'s world-space sample positions to the nearest perfect
/// primitive (line / circle / arc) per [mode], returning a NEW [Stroke3D]
/// whose samples carry the snapped positions. Pressure / tilt / time /
/// uv are preserved from the nearest original sample (clamped index) so
/// the snapped stroke keeps the original brush dynamics.
///
/// Per PROGRESS_SATURDAY §1.5: only the 3D control points snap — the
/// painted texture (the real-Krita dab layer) stays freehand. This
/// helper only touches the Stroke3D geometry; the host's dab pipeline
/// is unaffected.
///
/// When [mode] is `auto` and [DrawShapeAssist.snap] falls back to
/// freehand, the returned stroke is the smoothed freehand (not the raw
/// input). When [mode] is `line` / `circle` the snap is forced even if
/// the auto-detect residual would have rejected it.
Stroke3D snapStroke(
  Stroke3D source,
  DrawShapeAssist assist,
  DrawShapeMode mode,
) {
  if (source.samples.length < 2) return source;
  final positions = <Vector3>[];
  for (final s in source.samples) {
    positions.add(source.transform.transformed3(s.position.clone()));
  }

  final DrawShapeResult result;
  switch (mode) {
    case DrawShapeMode.auto:
      result = assist.snap(positions);
      break;
    case DrawShapeMode.line:
      result = _forceLine(positions);
      break;
    case DrawShapeMode.circle:
      result = _forceCircle(positions, assist.config.tessellationSegments);
      break;
  }

  final snapped = result.points;
  if (snapped.length < 2) return source;

  // Map snapped positions back to StrokeSample3D, preserving pressure /
  // tilt / time / uv from the nearest original sample (by clamped index).
  final newSamples = <StrokeSample3D>[];
  final span = (snapped.length - 1).clamp(1, 1 << 30);
  for (var i = 0; i < snapped.length; i++) {
    final srcIdx =
        (i * (source.samples.length - 1) ~/ span).clamp(0, source.samples.length - 1);
    final template = source.samples[srcIdx];
    newSamples.add(StrokeSample3D(
      position: snapped[i].clone(),
      pressure: template.pressure,
      tilt: template.tilt.clone(),
      time: template.time,
      uv: template.uv?.clone(),
      velocity: template.velocity,
    ));
  }

  return Stroke3D(
    samples: newSamples,
    color: source.color,
    thickness: source.thickness,
    materialIndex: source.materialIndex,
    inputDevice: source.inputDevice,
    source: '${source.source}#shape-${mode.name}',
    closed: mode == DrawShapeMode.circle ? true : source.closed,
  );
}

DrawShapeResult _forceLine(List<Vector3> pts) {
  return DrawShapeResult(
    kind: DrawShapeKind.line,
    points: <Vector3>[pts.first.clone(), pts.last.clone()],
    residual: 0.0,
  );
}

DrawShapeResult _forceCircle(List<Vector3> pts, int segments) {
  // Centroid + average radius (Kasa-style algebraic fit — good enough
  // for the forced-circle mode; the auto-detect path uses the full
  // least-squares fit in DrawShapeAssist._tryCircle).
  final center = Vector3.zero();
  for (final p in pts) {
    center.add(p);
  }
  center.scale(1.0 / pts.length);
  var radius = 0.0;
  for (final p in pts) {
    radius += (p - center).length;
  }
  radius /= pts.length;
  if (radius < 1e-6) radius = 1e-6;

  // Build a local basis in the plane of best fit. The Feather canvas
  // default ground plane is y=0 (XZ), so most drawn circles live in XZ.
  // We pick the plane normal from the first non-degenerate edge crossed
  // with the world up (Y); if the edge is parallel to Y we fall back to
  // a Z-normal (XY plane).
  var normal = Vector3(0, 1, 0);
  if (pts.length > 1) {
    final e = pts[1] - pts[0];
    if (e.length > 1e-6) {
      final n = e.cross(Vector3(0, 1, 0));
      normal = n.length < 1e-6 ? Vector3(0, 0, 1) : n.normalized();
    }
  }
  final u = normal.cross(Vector3(0, 1, 0));
  final uu = u.length < 1e-6 ? Vector3(1, 0, 0) : u.normalized();
  final v = normal.cross(uu)..normalize();

  final out = <Vector3>[];
  final seg = segments.clamp(8, 256);
  for (var i = 0; i < seg; i++) {
    final a = 2 * math.pi * i / seg;
    out.add(center + uu * (radius * math.cos(a)) + v * (radius * math.sin(a)));
  }
  out.add(out.first.clone());
  return DrawShapeResult(
    kind: DrawShapeKind.circle,
    points: out,
    center: center,
    radius: radius,
    residual: 0.0,
  );
}
