// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// Export-only: software rasterizer for high-quality PNG export.
// Not used in real-time (too slow for 30 fps); the canvas viewport's
// CustomPainter handles live rendering. This subsystem is reserved
// for the export pipeline (rasterize scene -> framebuffer -> PNG)
// and for future headless render paths.
//
// renderer.dart — Main render pipeline for the Feather-Krita software
// rasterizer.
//
// The pipeline, per frame:
//
//   1. Build a [RenderQueue] from the scene's meshes + camera. The
//      queue sorts opaque front-to-back and transparent back-to-front.
//   2. Clear the [FrameBuffer] (color + depth).
//   3. For each [RenderItem] in opaque pass:
//      a. Vertex-process the item's vertices (object → screen).
//      b. For each triangle: clip against the near plane, then
//         rasterize into the framebuffer.
//   4. For each [RenderItem] in transparent pass: same as opaque, but
//      the material's blendMode handles alpha compositing.
//   5. For each [RenderItem] in overlay pass: same, but depth test
//      disabled and depth write off.
//   6. Optional FXAA pass on the framebuffer's color buffer.
//   7. Resolve the framebuffer to a [ui.Image] for compositing.
//
// The renderer is configured once at construction time (viewport size,
// MSAA sample count, FXAA on/off) and reused across frames. The
// [ShaderContext] is rebuilt per-frame to pick up the latest camera
// position and light rig.

import 'package:vector_math/vector_math_64.dart';

import 'anti_alias.dart';
import 'fragment_shader.dart';
import 'framebuffer.dart';
import 'lighting.dart';
import 'rasterizer.dart';
import 'render_queue.dart';
import 'render_stats.dart';
import 'texture_sampler.dart';
import 'vertex_processor.dart';

/// Configuration for a [Renderer] instance.
class RenderConfig {
  const RenderConfig({
    this.msaa = MsaaMode.x1,
    this.enableFxaa = false,
    this.clearColor = 0xff101418,
    this.maxMeshes = 4096,
  });

  /// MSAA mode. Default is 1x (off) — FXAA covers AA on low-end devices.
  final MsaaMode msaa;

  /// Whether to apply FXAA after rasterization. Default off (MSAA is
  /// usually enough and FXAA blurs slightly).
  final bool enableFxaa;

  /// Default framebuffer clear color.
  final int clearColor;

  /// Maximum number of meshes the queue will accept before rejecting.
  final int maxMeshes;
}

/// MSAA sample counts. Maps to the predefined [SamplePattern]s.
enum MsaaMode { x1, x2, x4, x8 }

/// The main software renderer.
class Renderer {
  Renderer({
    required this.config,
    required this.stats,
  })  : _vertexProcessor = VertexProcessor(),
        _clipper = NearPlaneClipper(),
        _sampler = const TextureSampler() {
    _applyMsaa();
  }

  final RenderConfig config;
  final RenderStats stats;
  final VertexProcessor _vertexProcessor;
  final NearPlaneClipper _clipper;
  final TextureSampler _sampler;

  late Rasterizer _rasterizer;
  FrameBuffer? _framebuffer;
  Fxaa? _fxaa;

  /// The current framebuffer. Lazily resized to match the most recent
  /// [render] call's viewport.
  FrameBuffer get framebuffer {
    final fb = _framebuffer;
    if (fb == null) {
      throw StateError('Renderer.render must be called first.');
    }
    return fb;
  }

  void _applyMsaa() {
    final pattern = switch (config.msaa) {
      MsaaMode.x1 => SamplePattern.x1,
      MsaaMode.x2 => SamplePattern.x2,
      MsaaMode.x4 => SamplePattern.x4,
      MsaaMode.x8 => SamplePattern.x8,
    };
    _rasterizer = Rasterizer(samplePattern: pattern);
    if (config.enableFxaa) {
      _fxaa = Fxaa();
    }
  }

  /// Renders [queue] from [camera] with [lights]. Returns the
  /// framebuffer (already rasterized + post-processed). The caller is
  /// responsible for calling [FrameBuffer.resolve] to get a
  /// [ui.Image] for compositing.
  ///
  /// [viewportWidth] / [viewportHeight] are the framebuffer dimensions
  /// in pixels. The renderer lazily resizes its framebuffer to match.
  FrameBuffer render(
    RenderQueue queue,
    Camera camera,
    LightRig? lights,
    int viewportWidth,
    int viewportHeight, {
    Vector3? overrideCameraPosition,
  }) {
    // Resize framebuffer if needed.
    final fb = _framebuffer;
    if (fb == null ||
        fb.width != viewportWidth ||
        fb.height != viewportHeight) {
      _framebuffer = FrameBuffer(
        width: viewportWidth,
        height: viewportHeight,
        clearColor: config.clearColor,
      );
    }
    final target = _framebuffer!;
    stats.beginFrame();
    target.clear(color: config.clearColor);

    // Build the shader context.
    final camPos = overrideCameraPosition ?? camera.position;
    final context = ShaderContext(
      cameraPosition: camPos,
      lights: lights,
      sampler: _sampler,
      viewportWidth: viewportWidth.toDouble(),
      viewportHeight: viewportHeight.toDouble(),
    );

    stats.beginRaster();
    _drawPass(queue.opaque, camera, context, target, RenderPass.opaque);
    _drawPass(queue.transparent, camera, context, target,
        RenderPass.transparent);
    _drawPass(queue.overlay, camera, context, target, RenderPass.overlay);
    stats.endRaster();

    // FXAA post-process.
    if (_fxaa != null) {
      _fxaa!.apply(target);
    }

    stats.endFrame();
    return target;
  }

  void _drawPass(
    List<RenderItem> items,
    Camera camera,
    ShaderContext context,
    FrameBuffer fb,
    RenderPass pass,
  ) {
    for (final item in items) {
      // Vertex-process the batch.
      final processed =
          _vertexProcessor.process(item.vertices, item.model, camera);
      stats.recordDrawCall(
          triangles: item.indices.length ~/ 3,
          vertices: item.vertices.length);

      // Iterate triangles.
      for (var i = 0; i < item.indices.length; i += 3) {
        final ia = item.indices[i];
        final ib = item.indices[i + 1];
        final ic = item.indices[i + 2];
        if (ia < 0 ||
            ib < 0 ||
            ic < 0 ||
            ia >= processed.length ||
            ib >= processed.length ||
            ic >= processed.length) {
          continue;
        }
        final a = processed[ia];
        final b = processed[ib];
        final c = processed[ic];

        // Near-plane clip if any vertex is behind the camera.
        if (a.culled || b.culled || c.culled) {
          stats.recordClipOperation();
          final polygon = _clipper.clip(a, b, c);
          if (polygon.length < 3) {
            stats.recordBackfaceCull();
            continue;
          }
          // Fan-triangulate the clipped polygon (0, 1, 2; 0, 2, 3; ...).
          for (var t = 1; t < polygon.length - 1; t++) {
            _rasterizeOne(polygon[0], polygon[t], polygon[t + 1],
                item.material, context, fb, pass);
          }
        } else {
          _rasterizeOne(a, b, c, item.material, context, fb, pass);
        }
      }
    }
  }

  void _rasterizeOne(
    ProcessedVertex a,
    ProcessedVertex b,
    ProcessedVertex c,
    Material material,
    ShaderContext context,
    FrameBuffer fb,
    RenderPass pass,
  ) {
    // For the overlay pass, force depth test off + depth write off.
    Material m = material;
    if (pass == RenderPass.overlay &&
        (material.depthTest || material.depthWrite)) {
      m = material.copyWith(depthTest: false, depthWrite: false);
    }
    // For the transparent pass, force depth-write off (painter's
    // algorithm — the depth test still runs so fragments behind the
    // opaque layer are rejected).
    if (pass == RenderPass.transparent && material.depthWrite) {
      m = material.copyWith(depthWrite: false);
    }
    _rasterizer.rasterTriangle(a, b, c, m, context, fb);
    stats.recordFragment();
  }
}
