// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// math.dart — union barrel for the core math layer.
//
// The core math layer ships TWO complementary families of types and this
// barrel re-exports BOTH so every consumer can write a single import:
//
//   1. The project's own IMMUTABLE math types (Vec2 / Vec3 / Vec4 / Mat3 /
//      Mat4 / Quaternion / Aabb / Plane / Sphere / Ray / Triangle / Mesh /
//      Transform) — see vec2.dart … transform.dart. These are the
//      preferred engine types for new code: pure, immutable, no external
//      dependency.
//   2. The `vector_math` package types (Vector2 / Vector3 / Vector4 /
//      Matrix3 / Matrix4 / Aabb3) — used by the camera / interaction /
//      scene modules and by the legacy geometric helpers in
//      `lib/utils/vector_math_utils.dart` (ray casts, lookAt / perspective
//      matrix builders, barycentric coordinates, quaternion slerp).
//
// Names that exist in BOTH families (`Quaternion`, `Ray`, `Plane`,
// `Sphere`) are exported ONLY from the project's own modules — the
// `vector_math` re-export uses a restrictive `show` list to avoid the
// collision. Consumers who explicitly need the `vector_math` flavor of
// those types can import `package:vector_math/vector_math_64.dart`
// directly.
//
// The barrel also exposes the project's scalar helpers (`clampDouble`,
// `lerp`, `smoothstep`, `wrapAngle`, `bernstein`, …) and a few `k`-prefixed
// constant aliases (`kPi`, `kDegToRad`, …) for terse camera code.

// ----- The project's own immutable math types --------------------------
export 'vec2.dart';
export 'vec3.dart';
export 'vec4.dart';
export 'mat3.dart';
export 'mat4.dart';
export 'quaternion.dart';
export 'aabb.dart';
export 'plane.dart';
export 'sphere.dart';
export 'ray.dart';
export 'triangle.dart';
export 'mesh.dart';
export 'transform.dart';

// ----- Scalar helpers + math constants ---------------------------------
export 'math_utils.dart';
export 'scalar_math.dart';

// ----- vector_math bridge (restricted to non-colliding names) ----------
export 'package:vector_math/vector_math_64.dart'
    show Aabb3, Matrix3, Matrix4, Vector2, Vector3, Vector4;

// ----- Legacy geometric helpers (ray casts, lookAt, perspective, …) ----
export 'package:feather_krita/utils/vector_math_utils.dart'
    show RayTriangleHit, VectorMathUtils, Vector3Utils, Matrix4Utils;

/// Pi (terse alias of [PI] for camera / gesture code).
const double kPi = 3.1415926535897932;

/// Two times Pi.
const double kTwoPi = kPi * 2.0;

/// Half Pi (a quarter turn).
const double kHalfPi = kPi * 0.5;

/// Multiply a degree value by this to convert to radians.
const double kDegToRad = kPi / 180.0;

/// Multiply a radian value by this to convert to degrees.
const double kRadToDeg = 180.0 / kPi;
