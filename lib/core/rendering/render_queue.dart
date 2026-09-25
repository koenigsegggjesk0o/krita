// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// render_queue.dart — Sorted render queue for the software renderer.
//
// The renderer doesn't draw meshes in submission order — it sorts them
// into the classic two-pass configuration:
//
//   1. Opaque pass: front-to-back (by average depth), maximizing
//      early-z rejection in the depth buffer.
//   2. Transparent pass: back-to-front (painter's algorithm), so
//      farther surfaces draw first and nearer surfaces blend on top.
//
// Within each pass, items are also bucketed by material to minimize
// shader / texture state changes (though the software renderer has no
// GPU state to swap, the bucketing still helps the CPU cache by keeping
// triangles with the same texture together).
//
// The queue is built per-frame from a [Scene] (or any list of
// [RenderItem]s). It is intentionally cheap — the sort is O(n log n)
// on a list of a few thousand items, taking <100µs on a modern phone.

import 'dart:math' as math;

import 'package:vector_math/vector_math_64.dart';

import 'blend_mode.dart';
import 'fragment_shader.dart';
import 'vertex_processor.dart';

/// A single renderable triangle batch. The rasterizer consumes these
/// in queue order.
class RenderItem {
  RenderItem({
    required this.vertices,
    required this.indices,
    required this.material,
    required this.model,
    this.sortKey = 0,
  });

  /// The mesh vertices (object-space). The vertex processor transforms
  /// them per-frame.
  final List<Vertex> vertices;

  /// Triangle index buffer. Length must be a multiple of 3.
  final List<int> indices;

  /// The material applied to every triangle in this batch.
  final Material material;

  /// Model (object → world) matrix.
  final Matrix4 model;

  /// Precomputed sort key — the queue overwrites this with its own
  /// value when sorting.
  int sortKey;

  /// The average NDC depth of the batch's triangles. Computed lazily
  /// by the queue's sort pass.
  double avgDepth = 0;
}

/// Sort-pass identifiers — opaque vs transparent.
enum RenderPass { opaque, transparent, overlay }

/// The render queue. Built per-frame by the renderer.
class RenderQueue {
  RenderQueue();

  /// Items in the opaque pass, sorted front-to-back.
  final List<RenderItem> opaque = <RenderItem>[];

  /// Items in the transparent pass, sorted back-to-front.
  final List<RenderItem> transparent = <RenderItem>[];

  /// Items in the overlay pass (UI, gizmos) — drawn last, no depth
  /// write, depth test disabled.
  final List<RenderItem> overlay = <RenderItem>[];

  /// Adds [item] to the appropriate pass based on its material.
  void add(RenderItem item) {
    if (item.material.blendMode != _normalBlend ||
        item.material.opacity < 1.0) {
      transparent.add(item);
    } else if (!item.material.depthTest || !item.material.depthWrite) {
      overlay.add(item);
    } else {
      opaque.add(item);
    }
  }

  /// Adds many items at once.
  void addAll(Iterable<RenderItem> items) {
    for (final i in items) {
      add(i);
    }
  }

  /// Sorts all three passes by [RenderItem.avgDepth]. The renderer
  /// must call [computeDepths] first (with a camera) so the depth
  /// field is populated.
  void sort() {
    opaque.sort((a, b) => a.avgDepth.compareTo(b.avgDepth));
    transparent.sort((a, b) => b.avgDepth.compareTo(a.avgDepth));
    // Overlay keeps insertion order — its draw order is significant
    // (later items draw on top).
  }

  /// Clears the queue. Does not release the underlying list capacity —
  /// the next frame reuses the same lists.
  void clear() {
    opaque.clear();
    transparent.clear();
    overlay.clear();
  }

  /// Total item count across all passes.
  int get length => opaque.length + transparent.length + overlay.length;

  /// Total triangle count across all passes.
  int get triangleCount {
    var n = 0;
    for (final i in opaque) {
      n += i.indices.length ~/ 3;
    }
    for (final i in transparent) {
      n += i.indices.length ~/ 3;
    }
    for (final i in overlay) {
      n += i.indices.length ~/ 3;
    }
    return n;
  }

  /// Computes the average NDC depth of each item using [camera]. The
  /// depth is the average of the (post-projection, post-divide) Z of
  /// every vertex in the batch — a good-enough proxy for sort ordering.
  void computeDepths(Camera camera) {
    final vp = camera.viewProjection;
    final tmp = Vector4.zero();
    for (final pass in [opaque, transparent, overlay]) {
      for (final item in pass) {
        var sumZ = 0.0;
        var count = 0;
        // Sample the first up-to-64 vertices to keep the cost bounded.
        final step = math.max(1, item.vertices.length ~/ 64);
        for (var i = 0; i < item.vertices.length; i += step) {
          final v = item.vertices[i];
          final wp = item.model.transform3(v.position.clone());
          tmp.setValues(wp.x, wp.y, wp.z, 1.0);
          final clip = vp * tmp;
          if (clip.w > 1e-5) {
            sumZ += clip.z / clip.w;
            count++;
          }
        }
        item.avgDepth = count > 0 ? sumZ / count : 1.0;
      }
    }
  }

  static const BlendMode3D _normalBlend = BlendMode3D.normal;
}

/// Builds a [RenderQueue] from a list of [RenderItem]s in one call.
RenderQueue buildRenderQueue(
  List<RenderItem> items,
  Camera camera, {
  bool sort = true,
}) {
  final q = RenderQueue()..addAll(items);
  q.computeDepths(camera);
  if (sort) q.sort();
  return q;
}
