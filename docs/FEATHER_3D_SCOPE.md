# Feather-3D Scope & Feasibility — "All Krita Brushes in 3D"

> **Status:** APPROVED DIRECTION (user directive, 2026-09-22) — scoping document,
> no implementation yet. Task origin: 5-loop-84, commissioned by the user:
> *"intinya brush nya semua engine nya dari krita … bisa jadi 3d karena feather nya
> … intinya bagian brush nya semua dari krita tapi sisanya feather 3d"*
>
> **One-line architecture mandate:**
> **Brush = 100% Krita (every engine family). Everything else = Feather (Dart).**

Every claim below cites PRIMARY evidence (file/line in the verified `krita-source/`
mirror == upstream KDE v6.0.4, or the local app/bridge tree). Trust policy §9 applies.

---

## 1. The Two-Party Architecture (contract)

| Responsibility | Owner | How |
|---|---|---|
| Brush stroke simulation (dabs, sensors, spacing, curves, particles, bristles, smudge sampling, warp) | **Krita engine** (native C++) | wrapper exposes a *stroke session* on a real `KisPaintDevice`, dispatched via `KisPaintOpRegistry` |
| 3D scene, meshes, surfaces, UV raycast, texture atlases, viewport rendering, camera, lights, undo, export, UI | **Feather-3D** (pure Dart) | extends the existing `lib/engine/` + `lib/io/` modules |
| The seam between them | **krita_bridge ABI v2** | texture bytes in → stroke events → dirty-rect bytes out |

The rule "Krita source is NEVER modified" is unchanged. All engine access stays
behind `native/krita_bridge/krita_bridge_real.cpp` (allowed surface).

---

## 2. Inventory: ALL brush engine families in Krita v6.0.4 (primary evidence)

Source of truth: `krita-source/plugins/paintops/` directory listing + registration
files + plugin constructors (verified via GitHub Contents API on the mirror `main`).
CMakeLists registers 16 subdirectories (15 plugin dirs + shared `libpaintop`).

| # | Family (`paintopid`) | Plugin dir | Character | Needs canvas pixels? |
|---|---|---|---|---|
| 1 | `paintbrush` (Pixel) | `defaultpaintops/brush` | the classic auto/pixel brush | no |
| 2 | `duplicate` (Clone) | `defaultpaintops/duplicate` | clone stamp from a source point | yes (source region) |
| 3 | `colorsmudge` (Color Smudge) | `colorsmudge` | smudge/mix by sampling the layer | **yes** |
| 4 | `curvebrush` (Curve) | `curvebrush` | draws line/curve geometry | no |
| 5 | `deform` (Deform/Warp) | `deform` | warps existing pixels (liquify-family) | **yes (wide region)** |
| 6 | `experiment` (Experimental) | `experiment` | recursive/experimental shapes | no |
| 7 | `filterop` (Filter) | `filterop` | paints through a filter (blur, etc.) | **yes** |
| 8 | `gridbrush` (Grid) | `gridbrush` | grid-shaped dabs | no |
| 9 | `hairy` (Hairy/Bristle) | `hairy` | bristle simulation | no |
| 10 | `hatching` (Hatching) | `hatching` | hatching/crosshatch lines | no |
| 11 | `mypaint` (MyPaint) | `mypaint` | embedded MyPaint engine (Krita 5+) | no |
| 12 | `particle` (Particle) | `particle` | particle brush | no |
| 13 | `roundmarker` (Round Marker) | `roundmarker` | hard round marker | no |
| 14 | `sketch` (Sketch) | `sketch` | sketchy/chained lines | no |
| 15 | `spray` (Spray) | `spray` | particle spray / spatter | no |
| 16 | `tangentnormal` (Tangent Normal) | `tangentnormal` | normal-map painting | no |

Notes:
- "Eraser" is a preset MODE of these engines (settings-level `Krita/erase` /
  `CompositeOp=erase`) — the ABI already surfaces it (`krita_brush_get_eraser`).
- Registration evidence (sample, verbatim from the mirror):
  `defaultpaintops/defaultpaintops_plugin.cc` —
  `r->add(new KisSimplePaintOpFactory<KisBrushOp, KisBrushOpSettings,
  KisBrushOpSettingsWidget>("paintbrush", ...))` and
  `...<KisDuplicateOp, ...>("duplicate", ...)`; each other dir ships
  `krita<name>paintop.json` (`X-KDE-ServiceTypes: ["Krita/Paintop"]`, version 28).

---

## 3. Current state vs. target (gap analysis)

### 3.1 What the bridge (v1, 37 exports) does today
`native/krita_bridge/krita_bridge.h` + `krita_bridge_real.cpp` (1,591 lines):
- Preset system is ALREADY engine-wide: `.kpp` scan/load, paintop family
  identity (`krita_brush_get_paintop_id` → "paintbrush"/"spray"/…), full
  settings-param map (`preset_param_*`, `set_param` live for consumed keys),
  sensor curves (`get_curve`/`set_curve`), eraser flag, version.
- **Dab rendering is auto-brush-only**: `krita_brush_generate_dab` goes through
  `KisAutoBrush` + `KisBrush::mask()` + mask generators
  (`krita_bridge_real.cpp` includes `kis_mask_generator.h`; builds
  `KisAutoBrush(gen, 0.0, 0.0, 1.0)`). For a spray/hairy/sketch preset the app
  today renders auto-brush-style dabs from the parsed tip (or the honest
  pure-Dart synthetic fallback) — the OTHER engines' real pipelines do not run.
- The host (Dart) composites dabs into the texture itself
  (`lib/engine/texture_painter.dart`, 14 blend modes; opacity is the master
  multiplier by ABI contract).

### 3.2 The gap, precisely
To honor "brush = 100% Krita, ALL engines", dab generation must go through
**Krita's own paintop pipeline** instead of the mask-only path. Krita's own
architecture for this is (all verified in the mirror source):

- `KisPaintOpRegistry::instance()` —
  `KisPaintOp* paintOp(const KisPaintOpPresetSP, KisPainter*, KisNodeSP, KisImageSP)`
  (`libs/image/brushengine/kis_paintop_registry.h:47,75,90`) — **the dispatcher
  Krita desktop itself uses**: give it a preset, it constructs the right engine
  (spray→`KisSprayOp`, hairy→hairy op, …).
- `KisPaintOp::paintAt(const KisPaintInformation&, KisDistanceInformation*)`
  public entry (`kis_paintop.h:45`) → virtual `paintAt(...) = 0` (L118) — every
  engine's stroke step.
- `KisPainter::paintPolyline/paintBezierCurve/paintPainterPath`
  (`libs/image/kis_painter.h:445,466,534`) and `setPaintOpPreset(...)` (L626) —
  full stroke drivers.
- `KisPaintDevice` (`kis_paint_device.h`): `readBytes(...)/writeBytes(...)`
  (L415–451) — **exactly the texture in/out API** needed for 3D texture
  painting round-trips; `convertToQImage` (L513) for full readback.
- Headless registration: paintop factories register via plain
  `KisPaintOpRegistry::add(...)` in plugin constructors — the bridge can
  instantiate/attach them by LINKING the already-built plugin libraries
  (krita-build compiles all of `plugins/paintops/**` today); no Qt plugin
  discovery (KSycoca/KService) is required. This mirrors how the existing
  bridge already runs `QCoreApplication` headless in CI smokes.

### 3.3 Why this also FIXES canvas-sampling engines
`colorsmudge`, `deform`, `filterop`, `duplicate` read DESTINATION pixels.
In the stroke-session model the destination IS the surface texture: Feather
uploads the current texture region into the `KisPaintDevice` at stroke start
(`writeBytes`), the engine samples/modifies it natively, Feather reads back
the dirty rect. This is the same "device = texture" model as Blender's
texture-paint mode and Substance's stroke reprojection (see §6 research).

---

## 4. Bridge ABI v2 — "stroke session" (design draft)

Additive exports (v1 stays for back-compat & the portable/fallback bridges;
new functions follow the existing campaign conventions — capability probes
return 0/NULL on non-real bridges):

```c
// Session lifecycle
KRITA_BRIDGE_API int32_t  krita_stroke_begin(KritaBrushContext* h,
                              int32_t tex_w, int32_t tex_h);
    // creates an RGBA8 KisPaintDevice (texture-sized) + KisPainter,
    // applies the loaded preset via the registry dispatcher
KRITA_BRIDGE_API int32_t  krita_stroke_upload(KritaBrushContext* h,
                              const uint8_t* rgba, int32_t x, int32_t y,
                              int32_t w, int32_t h);
    // writeBytes — sync current texture pixels INTO the device
KRITA_BRIDGE_API void     krita_stroke_move(KritaBrushContext* h,
                              double u, double v,          // UV → texel coords
                              double pressure, double tilt_x, double tilt_y,
                              double time_s);
    // one KisPaintInformation through paintAt (UV space = device space)
KRITA_BRIDGE_API int32_t  krita_stroke_dirty_rect(KritaBrushContext* h,
                              int32_t* x, int32_t* y, int32_t* w, int32_t* h);
    // union of dirty rects since last readback (KisPainter tracks dirty rects)
KRITA_BRIDGE_API int32_t  krita_stroke_readback(KritaBrushContext* h,
                              uint8_t* out_rgba, int32_t x, int32_t y,
                              int32_t w, int32_t h);
    // readBytes of the dirty region (or full device) back to Dart
KRITA_BRIDGE_API void     krita_stroke_end(KritaBrushContext* h);
KRITA_BRIDGE_API const char* krita_stroke_engine_id(KritaBrushContext* h);
    // the registry-dispatched paintop id actually running (honesty badge)
```

Design notes:
- **Preset-driven dispatch**: the registry picks the engine from the loaded
  preset's `paintopid` — the bridge never hardcodes engine lists.
- **Dirty-rect contract**: only changed texels cross the FFI boundary per
  frame (keeps 2k–4k textures interactive).
- **Session scope**: one session = one stroke on one texture (the surface's
  atlas region). `krita_brush_cleanup` semantics preserved.
- **Undo stays Feather-side** (stroke list → re-upload + replay), consistent
  with the existing 50-level stroke undo.
- **Registration**: link the paintop plugin libs and populate the registry by
  direct factory registration in the wrapper's init (evidence: plugin ctors
  only call `KisPaintOpRegistry::instance()->add(...)`), then one
  `registry->paintOp(preset, painter, node, image)` call per session.
- **Smoke gates** (krita-build, per the full-chain convention): (a) registry
  contains >= 16 families; (b) a SPRAY preset produces different pixels than
  an auto-brush preset through the same session ABI; (c) colorsmudge actually
  changes a pre-uploaded texture (proves device round-trip); (d) preset
  param/curve surface unchanged (no regression).

---

## 5. Feather-3D v2 (Dart side) — assets vs. work

### 5.1 Already built (reused as-is)
| Module | File(s) | Role |
|---|---|---|
| Surface raycast → UV | `lib/engine/guide_surface.dart` (1,014 L) | Möller–Trumbore + barycentric UV — the stroke→texture seam already exists |
| Texture compositor | `lib/engine/texture_painter.dart` (825 L) | 14 blend modes; becomes the FALLBACK/overlay path |
| Software 3D rasterizer | `lib/engine/scene_pipeline.dart` (905 L) | textured mesh render, stroke ribbons, MSAA-ish contour feathering |
| Camera / lights | `camera_controller.dart`, `light_rig.dart` (presets noon/golden/rim) | unchanged |
| Stroke system | `stroke_manager.dart` (796 L; liquify 6 modes, mirror), `stroke_replay.dart` | becomes session-event source + Feather-only effects |
| Exporters | `lib/io/`: gltf, gif, mp4 (pure-Dart H.264 + muxer), `.feather` project | extended, not replaced |
| FFI loader + fallbacks | `lib/ffi/krita_bindings.dart`, `synthetic_dab.dart` | extended with session bindings; honest provenance badge (v0.48) |

### 5.2 New work (Feather side)
1. **Session bindings + stroke adapter** (`lib/ffi/` + `state/editor_state.dart`):
   pointer → raycast hit → UV → `krita_stroke_move`; dirty-rect →
   `ui.Image`/`TextureImage` patch; engine compositing mode vs. legacy dab mode.
2. **Per-engine preset picker UX**: group by family (scan already returns
   family), engine badge from `krita_stroke_engine_id`, parameter inspector
   already shows the full settings map.
3. **Mesh import** — extend beyond parametric guide surfaces:
   GLTF/GLB loader (options in §6), `SurfaceMesh` (already generic:
   positions+uvs+indices) fed from imported meshes; `.feather` doc v3 block
   (additive, tolerant parse — same pattern as `guideShape` in loop-59).
4. **UV atlas + seam handling**: per-mesh atlas texture, UV-island
   **dilation/padding** after readback (standard technique — prevents seam
   bleeding; Substance/Blender do exactly this), optional tiling mode.
5. **Canvas-sampling UX** (Tier B/C engines): duplicate source-point gizmo,
   deform radius overlay, filter picker for `filterop`.
6. **Performance**: dirty-rect coalescing, texture upload on render tick,
   memory budget for 2k/4k atlases (see risks).

---

## 6. Tech-stack decisions (researched 2026-09-22)

| Question | Options found | Decision for Feather |
|---|---|---|
| 3D viewport renderer | (a) keep pure-Dart software rasterizer; (b) `flutter_scene` (Impeller/Flutter GPU scene graph — young, Impeller-only); (c) raw Flutter GPU API (very early); (d) `flutter_gl_ffi`/OpenGL texture blit | **(a) for v1** — deterministic, testable (bit-exact suite), already runs on all 3 platforms incl. the Android APK; re-evaluate (b)/(d) ONLY if mesh+texture scale hurts (flutter_scene is not yet stable enough to bet the paint loop on; per-stroke texture streaming is exactly the weak spot) |
| glTF import | (a) pub `gltf_loader` 0.4.0 (glTF 2.0 helper); (b) extend Feather's own pure-Dart parser (exporter already exists) | **(b) preferred** — keeps zero-dep philosophy + one code path for read/write; (a) as reference for spec edge cases |
| Seam quality | UV island dilation/edge padding (Substance-style), stretch-limit | Implement Feather-side post-readback pass (pure Dart, test-locked) |
| Color pipeline | RGBA8 device now; Krita supports RGBA16F/linear | Stay RGBA8 sRGB for v1 (matches current texture model); linear/16-bit = future toggle |
| Brush settings UI | existing settings-map inspector + curve editor (v0.47) | Extend: family-grouped picker + engine badge; no new framework |

### Industry-pattern validation (web research)
- Blender texture paint & Substance Painter both use the **same core loop**
  Feather already has: stroke in 3D view → raycast hit → UV → paint into 2D
  texture atlas → re-render (Substance additionally reprojects strokes on UV
  change; that is out of scope for v1).
- Substance's "UV Reprojection" and padding/dilation docs confirm the
  seam-handling requirements in §5.2(4).

---

## 7. Integration tiers (honest effort estimate)

| Tier | Engines | Why |
|---|---|---|
| **A — works day one via stroke session** | paintbrush, spray, hairy, curvebrush, sketch, experiment, particle, gridbrush, hatching, roundmarker, tangentnormal, mypaint | pure dab/stream engines; dispatch = preset; no extra host input |
| **B — needs extra UI/input** | duplicate (source-point gizmo), filterop (filter selection) | engine runs, host must supply semantics |
| **C — works only because texture is uploaded** | colorsmudge, deform | destination-sampling engines; require §4's upload path + wider dirty rects (deform region) |

Effort shape (not a deadline promise): bridge ABI + smoke ≈ the sensor-curve
campaign scale; Dart session adapter ≈ the live-param-editing campaign; mesh
import + atlas ≈ the largest single piece (new module, doc format v3 block);
dilation/padding and Tier-B/C UX ≈ small follow-ups. All phases ride the
established chain: wrapper change → `krita-build` 4 legs → `build-app` 5 legs →
emulator smoke → release.

---

## 8. Risks & mitigations

| Risk | Evidence / severity | Mitigation |
|---|---|---|
| Registry init headless (no KSycoca) | plugin ctors are plain `registry->add(...)` — LOW | link plugin libs; register in wrapper init; smoke gate (a) asserts >= 16 families |
| Big-texture readback stalls UI | 4k RGBA = 64 MB per full read — MED | dirty-rect-only transfers (§4); upload at most once per render tick |
| Stroke crosses UV island seam | classic texture-paint artifact — MED | dilation pass + (later) stroke splitting across islands |
| Engines with their own timers (particle/sketch randomness) | non-deterministic across replays — LOW | accept for live painting; undo = texture snapshot, not re-simulate (already Feather's model) |
| `KisPainter` composite-op differences vs. Dart painter | current Dart compositor owns opacity semantics — LOW | session mode lets Krita composite (authoritative); legacy dab mode unchanged |
| krita-build link surface grows (plugin libs) | wrapper CMake change on builder — LOW | allowed surface (wrapper + workflows), no Krita source edits |
| Memory ceiling on Android (192 MB APK already) | atlas sizes 1k→2k→4k — MED | cap atlas by device class; lazy surface textures; measure in emulator smoke |
| GPL compliance | bridge already GPL-2.0-or-later | unchanged; no Krita source modification |

---

## 9. Phased roadmap (proposal, one milestone per loop campaign)

- **F1 — ABI v2 + proof of engines** (bridge + smoke only): session exports,
  registry registration, smoke gates (a)–(d). Release marker: "engine-family
  parity proven" (spray ≠ pixel pixels, colorsmudge round-trip).
- **F2 — Dart session integration**: bindings, stroke adapter, dirty-rect
  texture patch, engine badge switch, tests; legacy dab path kept as fallback.
- **F3 — Picker/UX per family**: family-grouped preset picker, Tier-B inputs
  (duplicate gizmo, filter picker), deform overlay.
- **F4 — Mesh era**: GLTF import, generic-mesh painting (extend raycast),
  `.feather` v3 additive block, atlas manager.
- **F5 — Quality**: seam dilation, tiling option, perf pass, (optional) GPU
  renderer evaluation with real measurements.

Each phase ends GREEN through the full chain and ships a release, per the
standing convention. **Krita source remains untouched throughout.**

---

## 10. Sources (primary evidence)

- Mirror tree (verified byte-identical to upstream v6.0.4, audits 5-loop-75/81/82):
  `plugins/paintops/` listing + `CMakeLists.txt`; `defaultpaintops/
  defaultpaintops_plugin.cc` (registration pattern, "paintbrush"/"duplicate");
  `libs/image/brushengine/kis_paintop.h` (paintAt L45/L118);
  `kis_paintop_registry.h` (paintOp dispatcher L47/L75/L90);
  `libs/image/kis_painter.h` (stroke APIs L445/466/534, setPaintOpPreset L626);
  `libs/image/kis_paint_device.h` (readBytes/writeBytes L415–451).
- Local app: `native/krita_bridge/krita_bridge.h` (37-export ABI),
  `krita_bridge_real.cpp` (auto-brush dab path), `lib/engine/*` (Feather-3D
  modules), `lib/io/*` (exporters), `lib/ffi/krita_bindings.dart`.
- Web research (2026-09-22, via z-ai web_search): flutter.dev Flutter GPU /
  flutter_scene; pub.dev `gltf_loader`; Adobe Substance docs (UV reprojection,
  dilation/padding); Blender texture-paint references; community.kde.org
  Krita/BrushEngine.
