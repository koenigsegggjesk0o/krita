// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// Export-only: software rasterizer for high-quality PNG export.
// Not used in real-time (too slow for 30 fps); the canvas viewport's
// CustomPainter handles live rendering. This subsystem is reserved
// for the export pipeline (rasterize scene -> framebuffer -> PNG)
// and for future headless render paths.
//
// render_stats.dart — Frame-level performance counters for the renderer.
//
// Tracks per-frame:
//   - Frames per second (smoothed over the last N frames)
//   - Frame time (min / avg / max over the last N frames)
//   - Draw calls (rasterTriangle invocations)
//   - Triangle count (sum of triangles per draw call)
//   - Fragment count (pixels shaded, post-depth-test)
//   - Rasterization time (microseconds in the raster loop)
//
// The renderer resets the per-frame counters at the start of each
// frame, increments them during the frame, then commits them to the
// rolling-history window at the end. The UI reads the public getters
// (fps, avgFrameTimeMs, etc.) for the on-screen HUD.

import 'dart:typed_data';

/// Per-frame render statistics with rolling-window smoothing.
class RenderStats {
  RenderStats({this.windowSize = 60});

  /// Number of frames to smooth over. Default 60 (~1 second at 60fps).
  final int windowSize;

  /// Rolling frame-time history in microseconds.
  final Float64List _frameTimes = Float64List(60);

  int _frameTimeHead = 0;
  int _frameTimeCount = 0;

  // Per-frame counters (reset at the start of each frame).
  int _drawCalls = 0;
  int _triangleCount = 0;
  int _fragmentCount = 0;
  int _vertexCount = 0;
  int _pixelOverdraw = 0;
  int _depthTestFails = 0;
  int _backfaceCulls = 0;
  int _clipOperations = 0;
  int _msaaSamples = 0;
  int _textureSamples = 0;
  int _blendOperations = 0;
  Stopwatch? _frameStopwatch;
  Stopwatch? _rasterStopwatch;

  /// Marks the start of a new frame. Resets all per-frame counters.
  void beginFrame() {
    _drawCalls = 0;
    _triangleCount = 0;
    _fragmentCount = 0;
    _vertexCount = 0;
    _pixelOverdraw = 0;
    _depthTestFails = 0;
    _backfaceCulls = 0;
    _clipOperations = 0;
    _msaaSamples = 0;
    _textureSamples = 0;
    _blendOperations = 0;
    _frameStopwatch = Stopwatch()..start();
  }

  /// Marks the start of the rasterization pass. The renderer calls
  /// this immediately before invoking the rasterizer.
  void beginRaster() {
    _rasterStopwatch = Stopwatch()..start();
  }

  /// Marks the end of the rasterization pass.
  void endRaster() {
    _rasterStopwatch?.stop();
  }

  /// Marks the end of the frame. Commits the frame time to the rolling
  /// history.
  void endFrame() {
    _frameStopwatch?.stop();
    final us = _frameStopwatch?.elapsedMicroseconds ?? 0;
    _frameTimes[_frameTimeHead] = us.toDouble();
    _frameTimeHead = (_frameTimeHead + 1) % windowSize;
    if (_frameTimeCount < windowSize) _frameTimeCount++;
  }

  /// Records a draw call. [triangles] is the number of triangles in
  /// the call.
  void recordDrawCall({required int triangles, required int vertices}) {
    _drawCalls++;
    _triangleCount += triangles;
    _vertexCount += vertices;
  }

  /// Records a backface cull (triangle skipped).
  void recordBackfaceCull() => _backfaceCulls++;

  /// Records a near-plane clip operation (triangle split).
  void recordClipOperation() => _clipOperations++;

  /// Records a fragment that survived the depth test (i.e. was shaded
  /// and blended).
  void recordFragment() {
    _fragmentCount++;
    _pixelOverdraw++;
  }

  /// Records a fragment that failed the depth test.
  void recordDepthTestFail() => _depthTestFails++;

  /// Records N MSAA samples evaluated.
  void recordMsaaSamples(int n) => _msaaSamples += n;

  /// Records a texture sample.
  void recordTextureSample() => _textureSamples++;

  /// Records a blend operation.
  void recordBlend() => _blendOperations++;

  // ---- Public read-only getters ---------------------------------------

  /// Smoothed frames per second over the last [windowSize] frames.
  double get fps {
    if (_frameTimeCount == 0) return 0;
    var sum = 0.0;
    for (var i = 0; i < _frameTimeCount; i++) {
      sum += _frameTimes[i];
    }
    final avgUs = sum / _frameTimeCount;
    return avgUs > 0 ? 1000000.0 / avgUs : 0;
  }

  /// Average frame time in milliseconds (smoothed).
  double get avgFrameTimeMs {
    if (_frameTimeCount == 0) return 0;
    var sum = 0.0;
    for (var i = 0; i < _frameTimeCount; i++) {
      sum += _frameTimes[i];
    }
    return sum / _frameTimeCount / 1000.0;
  }

  /// Min frame time in milliseconds over the rolling window.
  double get minFrameTimeMs {
    if (_frameTimeCount == 0) return 0;
    var min = double.infinity;
    for (var i = 0; i < _frameTimeCount; i++) {
      if (_frameTimes[i] < min) min = _frameTimes[i];
    }
    return min / 1000.0;
  }

  /// Max frame time in milliseconds over the rolling window.
  double get maxFrameTimeMs {
    if (_frameTimeCount == 0) return 0;
    var max = 0.0;
    for (var i = 0; i < _frameTimeCount; i++) {
      if (_frameTimes[i] > max) max = _frameTimes[i];
    }
    return max / 1000.0;
  }

  /// Rasterization time for the current/last frame in milliseconds.
  double get rasterTimeMs =>
      (_rasterStopwatch?.elapsedMicroseconds ?? 0) / 1000.0;

  /// Total frame time for the current/last frame in milliseconds.
  double get lastFrameTimeMs =>
      (_frameStopwatch?.elapsedMicroseconds ?? 0) / 1000.0;

  /// Draw calls in the current/last frame.
  int get drawCalls => _drawCalls;

  /// Triangles submitted in the current/last frame.
  int get triangleCount => _triangleCount;

  /// Vertices transformed in the current/last frame.
  int get vertexCount => _vertexCount;

  /// Fragments shaded in the current/last frame.
  int get fragmentCount => _fragmentCount;

  /// Pixel overdraw count (sum of fragmentCount + depthTestFails).
  int get pixelOverdraw => _pixelOverdraw + _depthTestFails;

  /// Depth test failures in the current/last frame.
  int get depthTestFails => _depthTestFails;

  /// Backface culls in the current/last frame.
  int get backfaceCulls => _backfaceCulls;

  /// Clip operations in the current/last frame.
  int get clipOperations => _clipOperations;

  /// MSAA samples evaluated in the current/last frame.
  int get msaaSamples => _msaaSamples;

  /// Texture samples in the current/last frame.
  int get textureSamples => _textureSamples;

  /// Blend operations in the current/last frame.
  int get blendOperations => _blendOperations;

  /// Average overdraw per pixel: fragments / (framebuffer pixel count).
  /// The renderer supplies the pixel count.
  double overdrawFactor(int pixelCount) =>
      pixelCount > 0 ? _fragmentCount / pixelCount : 0;

  /// Returns a single-line summary suitable for an on-screen HUD.
  String hudLine({int? pixelCount}) {
    final ps = pixelCount != null ? ' px=${fragmentCount}' : '';
    return 'fps=${fps.toStringAsFixed(0)} '
        'frame=${avgFrameTimeMs.toStringAsFixed(1)}ms '
        'raster=${rasterTimeMs.toStringAsFixed(1)}ms '
        'dc=$drawCalls tri=$triangleCount frag=$fragmentCount$ps '
        'over=${overdrawFactor(pixelCount ?? 0).toStringAsFixed(2)}';
  }

  /// Returns a multi-line breakdown suitable for a debug overlay.
  String debugDump() {
    final b = StringBuffer()
      ..writeln('=== Render Stats ===')
      ..writeln('FPS (smoothed):  ${fps.toStringAsFixed(1)}')
      ..writeln('Frame time avg:  ${avgFrameTimeMs.toStringAsFixed(2)}ms')
      ..writeln('Frame time min:  ${minFrameTimeMs.toStringAsFixed(2)}ms')
      ..writeln('Frame time max:  ${maxFrameTimeMs.toStringAsFixed(2)}ms')
      ..writeln('Raster time:     ${rasterTimeMs.toStringAsFixed(2)}ms')
      ..writeln('Draw calls:      $drawCalls')
      ..writeln('Triangles:       $triangleCount')
      ..writeln('Vertices:        $vertexCount')
      ..writeln('Fragments:       $fragmentCount')
      ..writeln('Depth fails:     $depthTestFails')
      ..writeln('Backface culls:  $backfaceCulls')
      ..writeln('Clip ops:        $clipOperations')
      ..writeln('MSAA samples:    $msaaSamples')
      ..writeln('Texture samples: $textureSamples')
      ..writeln('Blend ops:       $blendOperations');
    return b.toString();
  }
}
