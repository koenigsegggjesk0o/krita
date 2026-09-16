# Feather-Krita Work Log

---
Task ID: 3-a
Agent: general-purpose
Task: Write FFI bindings, 3D engine, models, and native C++ bridge

Work Log:
- lib/ffi/krita_bindings.dart — Dart FFI bindings to Krita C++ brush engine (BrushInput/BrushDab structs, native function signatures, KritaBrushEngine high-level wrapper, dynamic library loader for Windows/Linux/macOS/Android)
- lib/engine/texture_painter.dart — 2048x2048 offscreen RGBA8 texture with 14 blend modes (normal, multiply, screen, overlay, soft-light, hard-light, color dodge/burn, add, subtract, darken, lighten, erase, replace), UV↔pixel conversion, edge wrapping/clamping, bilinear sampling, undo/redo
- lib/engine/guide_surface.dart — 3D guide surfaces: sphere, cylinder, cone, torus/ring, plane, custom Catmull-Rom tube. Each generates positions/UVs/normals/indices. World-space raycast via local-space transform + Möller-Trumbore + barycentric UV interpolation. JSON serialization.
- lib/engine/stroke_manager.dart — ChangeNotifier-based stroke manager: add/select/delete strokes, joystick move/rotate/scale, 6 liquify modes (push/pull/twist/inflate/deflate/smooth), live X/Y/Z mirror with auto-regeneration, 50-level undo/redo via JSON snapshots, project save/load.
- lib/engine/camera_controller.dart — Orbit camera with damped yaw/pitch/distance/target, perspective projection, one-finger orbit, two-finger pinch+rotate+pan, mouse wheel zoom, frameBounds(), serialization.
- lib/models/stroke.dart — StrokePoint (Vector3 position, pressure, tilt, time) and Stroke (id, brushType, color, thickness, points, isVisible, transform, mirrorOfId) with applyTranslation/applyRotation/applyScale/bakeTransform/mirrored + JSON serde.
- lib/models/brush_preset.dart — BrushPreset model with BrushSettingValue (scalar/bool/curve/string/integer), .kpp loader (ZIP via `archive` package, XML via `xml` package), directory scanner, platform-specific default search paths.
- lib/models/export_format.dart — ExportFormat enum (PNG, JPEG, GIF, MP4, OBJ, GLTF, FeatherProject) with metadata table (label, extension, MIME, description, isProOnly, category) and lookup helpers.
- lib/utils/vector_math_utils.dart — Ray-triangle (Möller-Trumbore) + ray-mesh + ray-plane/sphere/AABB, barycentric 2D/3D, quaternion slerp/fromTo/lookRotation/toAxisAngle, Matrix4 compose/decompose/transformNormal/matrix3ToMatrix4, lookAt + perspective matrices, screen unprojection.
- native/krita_bridge/krita_bridge.h — C ABI header: BrushInput/BrushDab structs matching Dart FFI, opaque KritaBrushContext, all extern "C" function declarations.
- native/krita_bridge/krita_bridge.cpp — C++ implementation: allocates KisPaintDevice + KisPainter, loads KisPaintOpPreset from .kpp, calls KisPainter::paintAt() to generate dabs, converts KisPaintDevice to RGBA8 QImage → malloc'd buffer, brush server preset listing.
- native/cmake/CMakeLists.txt — CMake build config: Qt6 discovery, Krita source/build tree include paths, find_library for kritabrush/kritaimage/kritapigment/kritaui/etc., shared library target with platform-specific rpath, multi-config output directory flattening, install rules.

Stage Summary:
- All 12 files written with complete, functional implementations (no TODO/placeholder).
- Dart engine layer (FFI bindings + 4 engine modules + 3 models + 1 util) imports cleanly against `vector_math`, `flutter/foundation`, `archive`, `xml`, and the project's own modules.
- FFI structs (BrushInput, BrushDab) match the C header byte-for-byte; native handle is opaque `KritaBrushContext*`.
- 3D engine supports Feather-style workflow: draw onto parametric guide surfaces, manage strokes with live mirror + liquify, orbit camera with touch gestures.
- Native bridge calls Krita's public KisPaintDevice/KisPainter/KisPaintOpPreset API without modifying Krita source.
- Build config validates KRITA_SOURCE_DIR + KRITA_BUILD_DIR and links the appropriate Krita libraries for Windows/Linux/macOS.
- pubspec.yaml updated with `archive: ^3.4.0` and `xml: ^6.4.0` dependencies required by brush_preset.dart.
