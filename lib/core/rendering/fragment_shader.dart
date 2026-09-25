// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// fragment_shader.dart — Fragment shading interface and shared types.
//
// Defines:
//   - [Vertex]          — a single mesh vertex (position, normal, UV,
//                         color) — the CPU-side format
//   - [VertexAttributes]— per-vertex attribute payload that the vertex
//                         processor hands to the rasterizer (already
//                         transformed to clip / world / screen space)
//   - [Fragment]        — the per-pixel interpolated data handed to a
//                         [FragmentShader] inside the rasterizer loop
//   - [FragmentShader]  — the interface implemented by every shader
//                         (shadeless, shaded, glow, cutout, custom)
//   - [Material]        — material parameters shared across triangles
//                         (shader + base color + textures + flags)
//
// All shaders are pure functions of the fragment state — they must not
// touch global mutable state, must not allocate per-pixel, and must be
// deterministic. The rasterizer calls [FragmentShader.shade] exactly
// once per surviving fragment.

import 'package:vector_math/vector_math_64.dart';

import 'blend_mode.dart';
import 'lighting.dart';
import 'texture_sampler.dart';

/// CPU-side mesh vertex. Uses double precision because the rasterizer
/// path needs the headroom for large scene scales (10^3 m scenes with
/// sub-millimetre stroke detail).
class Vertex {
  Vertex({
    required this.position,
    Vector3? normal,
    Vector2? uv,
    this.color = 0xffffffff,
  })  : normal = normal ?? Vector3(0, 0, 1),
        uv = uv ?? Vector2.zero();

  /// Object-space position (before model transform).
  final Vector3 position;

  /// Object-space unit normal.
  final Vector3 normal;

  /// Texture coordinates (0..1, wrapping / clamping handled by sampler).
  final Vector2 uv;

  /// Per-vertex ARGB color. Multiplied with the material's base color
  /// in the fragment shader.
  final int color;

  Vertex copyWith({
    Vector3? position,
    Vector3? normal,
    Vector2? uv,
    int? color,
  }) =>
      Vertex(
        position: position ?? this.position,
        normal: normal ?? this.normal,
        uv: uv ?? this.uv,
        color: color ?? this.color,
      );
}

/// Per-vertex transformed attributes. The vertex processor produces
/// one of these per input vertex; the rasterizer interpolates them
/// across each triangle to produce [Fragment]s.
class VertexAttributes {
  VertexAttributes({
    required this.clipPosition,
    required this.worldPosition,
    required this.worldNormal,
    required this.uv,
    required this.color,
    required this.w,
  });

  /// Clip-space position (before perspective divide). XYZW in homogeneous
  /// clip space.
  final Vector4 clipPosition;

  /// World-space position (model-transformed, before view-projection).
  final Vector3 worldPosition;

  /// World-space unit normal (inverse-transpose transformed).
  final Vector3 worldNormal;

  /// Texture coordinates (copied straight from the source vertex).
  final Vector2 uv;

  /// Per-vertex color (copied straight from the source vertex).
  final int color;

  /// The clip-space W component. Stored separately because the
  /// rasterizer needs it for perspective-correct interpolation.
  final double w;
}

/// A fully-interpolated fragment, ready for shading.
///
/// Fields are mutable to support a per-rasterizer scratch instance that
/// gets reused across pixels (no per-pixel allocation). The rasterizer
/// mutates the fields in place before each `shade` call; shaders must
/// not retain references to a [Fragment] past their `shade` call.
class Fragment {
  Fragment();

  /// Screen X in framebuffer pixels (sub-pixel accurate when MSAA is on).
  double x = 0;

  /// Screen Y in framebuffer pixels.
  double y = 0;

  /// NDC depth in [0, 1] (0 = near, 1 = far).
  double depth = 0;

  /// World-space position. Perspective-correct interpolated. The
  /// underlying [Vector3] instance is reused across pixels by the
  /// rasterizer — shaders that need to retain the value must clone.
  final Vector3 worldPosition = Vector3.zero();

  /// World-space unit normal. Perspective-correct interpolated, then
  /// re-normalized.
  final Vector3 worldNormal = Vector3.zero();

  /// Texture coordinates. Reused across pixels.
  final Vector2 uv = Vector2.zero();

  /// Interpolated per-vertex color (ARGB int).
  int vertexColor = 0xffffffff;

  /// Unit vector from this fragment toward the camera position. Reused
  /// across pixels.
  final Vector3 viewDir = Vector3.zero();

  /// Derivatives (dFdx, dFdy) of the UV — used for mip selection. May
  /// be (0,0) if derivatives aren't available.
  final Vector2 dUVdx = Vector2.zero();
  final Vector2 dUVdy = Vector2.zero();
}

/// Material parameters shared across triangles that use this material.
class Material {
  const Material({
    required this.shader,
    this.baseColor = 0xffffffff,
    this.opacity = 1.0,
    this.blendMode = BlendMode3D.normal,
    this.texture,
    this.textureFilter = TextureFilter.bilinear,
    this.doubleSided = false,
    this.depthWrite = true,
    this.depthTest = true,
    this.wireframe = false,
    this.specularStrength = 0.5,
    this.emissiveColor = 0x00000000,
    this.emissiveIntensity = 1.0,
    this.id = 0,
  });

  /// The fragment shader that runs for every fragment of this material.
  final FragmentShader shader;

  /// Base RGBA color (multiplied with vertex color and texture).
  final int baseColor;

  /// Overall opacity in [0, 1]. < 1 puts the material into the
  /// transparent render queue.
  final double opacity;

  /// Blend mode for compositing fragments onto the framebuffer.
  final BlendMode3D blendMode;

  /// Optional diffuse texture.
  final Texture? texture;

  /// Texture filtering mode.
  final TextureFilter textureFilter;

  /// If true, triangles are rendered regardless of winding (no
  /// backface cull).
  final bool doubleSided;

  /// If false, fragments do not write depth (transparency).
  final bool depthWrite;

  /// If false, fragments skip the depth test (overlay).
  final bool depthTest;

  /// If true, render only the triangle edges (debug).
  final bool wireframe;

  /// Specular highlight strength for the shaded shader (0..1).
  final double specularStrength;

  /// Emissive color (ARGB) for the glow shader.
  final int emissiveColor;

  /// Emissive intensity multiplier.
  final double emissiveIntensity;

  /// Per-material numeric id, used by the picking pass.
  final int id;

  /// True when this material needs transparency sorting (back-to-front).
  bool get isTransparent => opacity < 1.0 || blendMode != BlendMode3D.normal;

  Material copyWith({
    FragmentShader? shader,
    int? baseColor,
    double? opacity,
    BlendMode3D? blendMode,
    Texture? texture,
    TextureFilter? textureFilter,
    bool? doubleSided,
    bool? depthWrite,
    bool? depthTest,
    bool? wireframe,
    double? specularStrength,
    int? emissiveColor,
    double? emissiveIntensity,
    int? id,
  }) =>
      Material(
        shader: shader ?? this.shader,
        baseColor: baseColor ?? this.baseColor,
        opacity: opacity ?? this.opacity,
        blendMode: blendMode ?? this.blendMode,
        texture: texture ?? this.texture,
        textureFilter: textureFilter ?? this.textureFilter,
        doubleSided: doubleSided ?? this.doubleSided,
        depthWrite: depthWrite ?? this.depthWrite,
        depthTest: depthTest ?? this.depthTest,
        wireframe: wireframe ?? this.wireframe,
        specularStrength: specularStrength ?? this.specularStrength,
        emissiveColor: emissiveColor ?? this.emissiveColor,
        emissiveIntensity: emissiveIntensity ?? this.emissiveIntensity,
        id: id ?? this.id,
      );
}

/// The fragment shader interface. Every shader implements [shade] —
/// the rasterizer calls it once per surviving fragment.
///
/// Contract:
///   - Must be deterministic and side-effect free.
///   - May read from [Material] fields cached on [context].
///   - Must NOT retain references to [fragment] past the call.
///   - Returns the fragment color as an ARGB int (alpha is the
///     fragment's coverage-weighted alpha; the rasterizer handles
///     final blending via [Material.blendMode]).
abstract interface class FragmentShader {
  /// Returns the fragment's color for [fragment], given the [material]
  /// and [context] (lights, camera, sampler, etc.).
  int shade(Fragment fragment, Material material, ShaderContext context);
}

/// The shared per-frame state passed into every shader call. Built once
/// per frame by the renderer, then handed to the rasterizer.
class ShaderContext {
  ShaderContext({
    required this.cameraPosition,
    required this.lights,
    required this.sampler,
    required this.viewportWidth,
    required this.viewportHeight,
    this.exposure = 1.0,
    this.timeSeconds = 0.0,
  });

  /// World-space camera position (for viewDir and specular).
  final Vector3 cameraPosition;

  /// The light rig. May be null for shadeless materials.
  final LightRig? lights;

  /// The texture sampler — shared across all materials in the frame.
  final TextureSampler sampler;

  /// Framebuffer width in pixels — shaders that need screen-space
  /// coordinates (e.g. the cutout's vertical gradient) read this.
  final double viewportWidth;

  /// Framebuffer height in pixels.
  final double viewportHeight;

  /// Tonemap exposure.
  final double exposure;

  /// Elapsed time in seconds — for animated shaders (e.g. flowing
  /// emissive noise on the glow shader).
  final double timeSeconds;
}
