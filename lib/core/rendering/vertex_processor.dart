// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// Export-only: software rasterizer for high-quality PNG export.
// Not used in real-time (too slow for 30 fps); the canvas viewport's
// CustomPainter handles live rendering. This subsystem is reserved
// for the export pipeline (rasterize scene -> framebuffer -> PNG)
// and for future headless render paths.
//
// vertex_processor.dart — Vertex transform pipeline.
//
// Stages each input [Vertex] through:
//   1. Model transform (object → world).
//   2. View-projection transform (world → clip).
//   3. Viewport transform (clip → screen pixels, with perspective divide).
//   4. Clip-space culling (near-plane and trivial reject).
//
// The output is a [ProcessedVertex] — the screen-space position + the
// per-vertex attributes the rasterizer needs for perspective-correct
// interpolation.
//
// The processor also computes per-vertex inverse-W so the rasterizer
// can interpolate attributes as `attr / w` and recover the
// perspective-correct value via a single multiply at the fragment.

import 'dart:math' as math;

import 'package:vector_math/vector_math_64.dart';

import 'fragment_shader.dart';

/// A vertex after the transform pipeline.
class ProcessedVertex {
  ProcessedVertex({
    required this.screen,
    required this.ndc,
    required this.worldPosition,
    required this.worldNormal,
    required this.uv,
    required this.color,
    required this.w,
    required this.invW,
    required this.culled,
  });

  /// Screen-space (pixel) coordinates. XY = pixel, Z = NDC depth in
  /// [0, 1] (0 = near, 1 = far). W is unused (always 1 after divide).
  final Vector4 screen;

  /// NDC coordinates (XYZ in [-1, 1] cubed, W = 1). Used by the clip
  /// stage.
  final Vector4 ndc;

  /// World-space position. Pre-perspective-divide (i.e. the actual
  /// world-space point that maps to this vertex).
  final Vector3 worldPosition;

  /// World-space unit normal.
  final Vector3 worldNormal;

  /// Texture coordinates (copied from the input vertex).
  final Vector2 uv;

  /// Per-vertex ARGB color (copied from the input vertex).
  final int color;

  /// Original clip-space W. The rasterizer interpolates `1/W` and
  /// uses it for perspective-correct attribute interpolation.
  final double w;

  /// Precomputed 1/W for the rasterizer hot loop.
  final double invW;

  /// True if this vertex was culled (behind near plane or outside the
  /// clip volume). Triangles with any culled vertex go through the
  /// clip polygon stage.
  final bool culled;
}

/// A camera description. Built once per frame by the renderer and
/// handed to the vertex processor.
class Camera {
  Camera({
    required this.position,
    required this.target,
    required this.up,
    required this.fovYRadians,
    required this.aspect,
    required this.near,
    required this.far,
    required this.viewportWidth,
    required this.viewportHeight,
  }) {
    _rebuildMatrices();
  }

  /// World-space camera position.
  final Vector3 position;

  /// World-space point the camera is looking at.
  final Vector3 target;

  /// World-space up direction (usually (0, 1, 0)).
  final Vector3 up;

  /// Vertical field of view in radians.
  final double fovYRadians;

  /// Aspect ratio (width / height).
  final double aspect;

  /// Near clip plane distance.
  final double near;

  /// Far clip plane distance.
  final double far;

  /// Viewport width in pixels.
  final double viewportWidth;

  /// Viewport height in pixels.
  final double viewportHeight;

  late final Matrix4 view;
  late final Matrix4 projection;
  late final Matrix4 viewProjection;
  late final Matrix4 inverseViewProjection;

  void _rebuildMatrices() {
    final f = (target - position)..normalize();
    final s = f.cross(up.normalized())..normalize();
    final u = s.cross(f);
    view = Matrix4(
      s.x, u.x, -f.x, 0,
      s.y, u.y, -f.y, 0,
      s.z, u.z, -f.z, 0,
      -s.dot(position), -u.dot(position), f.dot(position), 1,
    );
    final tanHalf = math.tan(fovYRadians * 0.5);
    final nf = 1.0 / (near - far);
    projection = Matrix4(
      1.0 / (aspect * tanHalf), 0, 0, 0,
      0, 1.0 / tanHalf, 0, 0,
      0, 0, (far + near) * nf, -1,
      0, 0, 2 * far * near * nf, 0,
    );
    viewProjection = projection * view;
    inverseViewProjection = Matrix4.inverted(viewProjection);
  }

  /// Builds a ray from a screen pixel — used by the picking pass.
  Ray screenToRay(double sx, double sy) {
    final ndcX = 2.0 * sx / viewportWidth - 1.0;
    final ndcY = 1.0 - 2.0 * sy / viewportHeight;
    final near = inverseViewProjection.transform3(Vector3(ndcX, ndcY, -1.0));
    final far = inverseViewProjection.transform3(Vector3(ndcX, ndcY, 1.0));
    final dir = (far - near)..normalize();
    return Ray.originDirection(near, dir);
  }
}

/// A single triangle after vertex processing. References three
/// [ProcessedVertex]es by index. Material is shared with the source
/// mesh triangle.
class ProcessedTriangle {
  ProcessedTriangle(this.a, this.b, this.c, this.material);

  final int a;
  final int b;
  final int c;
  final Material material;
}

/// The vertex processor.
class VertexProcessor {
  VertexProcessor();

  /// Transforms [vertices] through [model] → [camera] → screen. Returns
  /// the list of processed vertices (same length as the input). Indices
  /// are not remapped — the caller passes the mesh's index buffer
  /// unchanged to the rasterizer.
  List<ProcessedVertex> process(
    List<Vertex> vertices,
    Matrix4 model,
    Camera camera,
  ) {
    final out = List<ProcessedVertex>.filled(vertices.length,
        _placeholder, growable: false);
    final normalMatrix = _normalMatrix(model);
    final vp = camera.viewProjection;
    final halfW = camera.viewportWidth * 0.5;
    final halfH = camera.viewportHeight * 0.5;
    for (var i = 0; i < vertices.length; i++) {
      final v = vertices[i];
      // World position.
      final wp = model.transform3(v.position.clone());
      // World normal (inverse-transpose).
      final wn = normalMatrix.transformed(v.normal)..normalize();
      // Clip space.
      final clip = vp * Vector4(wp.x, wp.y, wp.z, 1.0);
      final w = clip.w;
      final culled = w <= 0.0;
      final invW = culled ? 0.0 : 1.0 / w;
      // NDC (perspective divide).
      final ndc = culled
          ? Vector4(0, 0, 0, 0)
          : Vector4(clip.x * invW, clip.y * invW,
              clip.z * invW, 1.0);
      // Screen space.
      final screen = culled
          ? Vector4(0, 0, 0, 0)
          : Vector4(
              halfW + ndc.x * halfW,
              halfH - ndc.y * halfH,
              ndc.z * 0.5 + 0.5,
              1.0,
            );
      out[i] = ProcessedVertex(
        screen: screen,
        ndc: ndc,
        worldPosition: wp,
        worldNormal: wn,
        uv: v.uv,
        color: v.color,
        w: w,
        invW: invW,
        culled: culled,
      );
    }
    return out;
  }

  /// Builds the inverse-transpose of the rotation/scale portion of
  /// [model]. Used to transform normals safely under non-uniform
  /// scaling.
  Matrix3 _normalMatrix(Matrix4 model) {
    final inv = Matrix4.inverted(model);
    return Matrix3(
      inv.entry(0, 0), inv.entry(1, 0), inv.entry(2, 0),
      inv.entry(0, 1), inv.entry(1, 1), inv.entry(2, 1),
      inv.entry(0, 2), inv.entry(1, 2), inv.entry(2, 2),
    );
  }

  static final ProcessedVertex _placeholder = ProcessedVertex(
    screen: Vector4.zero(),
    ndc: Vector4.zero(),
    worldPosition: Vector3.zero(),
    worldNormal: Vector3(0, 0, 1),
    uv: Vector2.zero(),
    color: 0xffffffff,
    w: 1.0,
    invW: 1.0,
    culled: true,
  );
}

/// Sutherland-Hodgman near-plane clipper. Produces a polygon (0..N
/// vertices) from a triangle that may straddle the near plane.
///
/// The rasterizer calls this only when at least one of the triangle's
/// vertices has been culled by the near plane.
class NearPlaneClipper {
  NearPlaneClipper();

  /// Clips a triangle (a, b, c) against the near plane (w = epsilon).
  /// Returns a list of 0, 3, or 4 [ProcessedVertex]es forming one or
  /// two triangles covering the visible portion.
  ///
  /// The returned vertices share storage with the input where possible
  /// (no copy-on-clip for the surviving side); intersection vertices
  /// are freshly allocated.
  List<ProcessedVertex> clip(
      ProcessedVertex a, ProcessedVertex b, ProcessedVertex c,
      {double epsilon = 1e-5}) {
    final input = [a, b, c];
    final output = <ProcessedVertex>[];
    for (var i = 0; i < 3; i++) {
      final cur = input[i];
      final next = input[(i + 1) % 3];
      final curIn = cur.w > epsilon;
      final nextIn = next.w > epsilon;
      if (curIn) {
        output.add(cur);
        if (!nextIn) {
          output.add(_intersect(cur, next, epsilon));
        }
      } else if (nextIn) {
        output.add(_intersect(cur, next, epsilon));
      }
    }
    return output;
  }

  /// Linear-interpolates two vertices by parameter [t] (0 = a, 1 = b)
  /// in clip-space-W space, then recomputes screen-space coords.
  ProcessedVertex _intersect(
      ProcessedVertex a, ProcessedVertex b, double epsilon) {
    // Solve for the W = epsilon crossing point.
    final t = (epsilon - a.w) / (b.w - a.w);
    return _lerp(a, b, t);
  }

  ProcessedVertex _lerp(ProcessedVertex a, ProcessedVertex b, double t) {
    final screen = _lerpV4(a.screen, b.screen, t);
    final ndc = _lerpV4(a.ndc, b.ndc, t);
    final wp = _lerpV3(a.worldPosition, b.worldPosition, t);
    final wn = _lerpV3(a.worldNormal, b.worldNormal, t)..normalize();
    final uv = _lerpV2(a.uv, b.uv, t);
    final w = a.w + (b.w - a.w) * t;
    final invW = w != 0 ? 1.0 / w : 0.0;
    return ProcessedVertex(
      screen: screen,
      ndc: ndc,
      worldPosition: wp,
      worldNormal: wn,
      uv: uv,
      color: _lerpColor(a.color, b.color, t),
      w: w,
      invW: invW,
      culled: false,
    );
  }

  static Vector4 _lerpV4(Vector4 a, Vector4 b, double t) =>
      Vector4(a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t,
          a.z + (b.z - a.z) * t, a.w + (b.w - a.w) * t);

  static Vector3 _lerpV3(Vector3 a, Vector3 b, double t) =>
      Vector3(a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t,
          a.z + (b.z - a.z) * t);

  static Vector2 _lerpV2(Vector2 a, Vector2 b, double t) =>
      Vector2(a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t);

  static int _lerpColor(int a, int b, double t) {
    final ar = (a >> 16) & 0xff;
    final ag = (a >> 8) & 0xff;
    final ab = a & 0xff;
    final aa = (a >> 24) & 0xff;
    final br = (b >> 16) & 0xff;
    final bg = (b >> 8) & 0xff;
    final bb = b & 0xff;
    final ba = (b >> 24) & 0xff;
    return ((aa + (ba - aa) * t).round() << 24) |
        ((ar + (br - ar) * t).round() << 16) |
        ((ag + (bg - ag) * t).round() << 8) |
        (ab + (bb - ab) * t).round();
  }
}
