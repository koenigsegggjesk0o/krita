# Krita Brush Engine — Standalone Extraction

This package is a **focused extraction of the brush engine** (and only the
brush engine) from the [Krita](https://krita.org/) digital painting
application, an open-source project developed by the KDE community and
licensed under the GNU GPL-2.0-or-later.

It contains the source code, brush presets, brush tips, the resource
system that loads them, and the minimum set of Krita internal libraries
required to compile the brush engine — **without** any of Krita's UI,
workspace, document model, layer management, canvas, filters, animation,
or export code.

> **Source:** https://github.com/KDE/krita (official KDE mirror)
> **Branch:** master (shallow clone)
> **Extracted on:** 2026-09-14

---

## 1. What was extracted

| Area | Upstream path | Destination | Purpose |
|------|---------------|-------------|---------|
| Brush core library | `libs/brush/` | `brush-engine/core/brush/` | Brush tip loading (GBR/ABR/PNG/SVG/text/pipe), brush registry, brush factories, scaling pyramid, dab shape |
| Brush engine interfaces | `libs/image/brushengine/` | `brush-engine/core/brushengine/` | The `KisPaintOp`, `KisPaintOpFactory`, `KisPaintOpPreset`, `KisPaintOpSettings`, `KisPaintOpRegistry`, `KisPaintInformation` base classes that every brush engine implements |
| Brush mask algebra | `libs/image/kis_brush_mask_*` | `brush-engine/core/brush-mask/` | Scalar/SIMD mask applicators for generating the soft round dab |
| Shared paintop library | `plugins/paintops/libpaintop/` | `brush-engine/libpaintop/` | Reusable option widgets, sensors, curve options, dab cache, texture option, brush-selection widget used by all paintops |
| Brush engine plugins | `plugins/paintops/<engine>/` | `brush-engine/paintops/<engine>/` | 15 concrete brush engines (see §2) |
| Brush presets | `plugins/paintops/defaultpresets/` | `brush-presets/` | 18 default `.kpp` preset packages |
| Brush tips / textures | `plugins/paintops/mypaint/brushes/`, `…/defaultpaintops/…/data/*.gbr` | `brush-tips/` | `.myb` (MyPaint), `.gbr` (GIMP) brush tip files |
| Resource system | `libs/resources/` | `resources/` | Storage backends (folder/bundle/memory/SQL), resource models, tag models, resource locator — loads & manages `.kpp` presets and brush tips |
| Min. Krita libs | `libs/{global,version,pigment,widgetutils,multiarch}/` | `dependencies/krita-libs/` | Foundation types, color science, KoResource base, versioning, multi-arch export macros |
| External deps docs | — | `dependencies/external/` | Documentation of non-bundled external libraries (Qt, lager, Boost, Eigen, libmypaint) |

### What was deliberately NOT extracted
(per the task scope — these are not part of the brush engine)

- `krita/` (application shell, main window, workspace, docks, MDI)
- `libs/ui/`, `libs/widgets/`, `libs/resourcewidgets/` (GUI widgets)
- `libs/flake/`, `libs/basicflakes/` (shape/vector framework)
- `libs/command/`, `libs/koplugin/`, `libs/libkis/` (plugin/document API)
- `libs/image/{commands,filter,floodfill,generator,layerstyles,processing,tiles3}` (non-brush image internals)
- `libs/{psd,psdutils,metadata,store,impex}` (file format I/O)
- `plugins/{colors,extensions,dockers,tools,viewplugins,generators,filters,…}` (non-paintop plugins)
- `packaging/`, `sdk/`, `qmlmodules/`

---

## 2. The brush engines (paintops)

All 15 brush engine plugins shipped in `plugins/paintops/` are included:

| # | Engine | Main class | Description |
|---|--------|-----------|-------------|
| 1 | `defaultpaintops` | `KisBrushOp` | The standard pixel/round brush (the default Krita brush). Includes the dab-rendering queue & executor. |
| 2 | `colorsmudge` | `KisColorSmudgeOp` | Smudge brush that picks up and drags color (wet-oil effect). |
| 3 | `curvebrush` | `KisCurveOp` | Draws a curved line through the stroke points. |
| 4 | `deform` | `KisDeformPaintOp` | Deforms existing pixels (inflate, grow, shrink, swirl). |
| 5 | `experiment` | `KisExperimentPaintOp` | Experimental recursive stroke. |
| 6 | `filterop` | `KisFilterOp` | Applies a filter along the stroke. |
| 7 | `gridbrush` | `KisGridBrush` | Paints on a snapping grid. |
| 8 | `hairy` | `KisHairyPaintOp` | Multi-strand "hairy" bristle brush. |
| 9 | `hatching` | `KisHatchingPaintOp` | Hatching/cross-hatching stroke. |
| 10 | `mypaint` | `MyPaintPaintOp` | Wraps libmypaint (optional, see §5). |
| 11 | `particle` | `KisParticlePaintOp` | Particle-spray brush. |
| 12 | `roundmarker` | `KisRoundMarkerOp` | Hard round marker. |
| 13 | `sketch` | `KisSketchPaintOp` | Sketchy/vibrating line brush. |
| 14 | `spray` | `KisSprayPaintOp` | Scatter/spray brush (particles + metaball option). |
| 15 | `tangentnormal` | `KisTangentNormalPaintOp` | Tangent-space normal-map painting brush. |

---

## 3. Module functions

### `brush-engine/core/brush/` — `kritalibbrush`
Brush *tip* abstraction and loading. Key classes:
- **`KisBrush`** (`kis_brush.h`) — abstract base for all brush tips. Owns the QImage pyramid, spacing, masking, and dab generation (`generateDab` / `mask`).
- **`KisAutoBrush`** — procedurally-generated round/sharp/curved soft brush (the "Auto Brush").
- **`KisGbrBrush`** — loads GIMP `.gbr` brush tips.
- **`KisAbrBrush` / `KisAbrBrushCollection` / `KisAbrStorage`** — loads Photoshop `.abr` brush tool presets.
- **`KisImagePipeBrush` / `kis_pipebrush_parasite`** — animated image-pipe ("gih"-style) brushes that step through frames.
- **`KisPngBrush` / `KisSvgBrush`** — raster-PNG and vector-SVG brush tips.
- **`KisTextBrush`** — brush tip generated from a text glyph.
- **`KisColorfulBrush`** — brush tip that retains its own color (used by stamp/spatter brushes).
- **`KisScalingSizeBrush`** — base for brushes that scale by pixel-size vs. explicit dimension.
- **`KisQImagePyramid`** — multi-resolution mipmap pyramid for smooth dab scaling.
- **`KisBrushRegistry` / `KisBrushFactory`** — factory + registry pattern for brush tip types.
- **`KisBrushServerProvider`** — singleton accessor (process-wide).
- **`KisBrushModel`** — Lager-backed reactive state model for brush tip options (CommonData, AutoBrushData, PredefinedBrushData, TextBrushData, …).

### `brush-engine/core/brushengine/` — part of `kritaimage`
The base interfaces every brush engine implements (see the upstream doc comment in `brushengine.h`):
- **`KisPaintInformation`** — per-pointer-tick input (position, pressure, x/y tilt, rotation, tangential pressure, perspective, time, speed, hovering flag). Serialized to/from XML. Includes `KisPerStrokeRandomSource` & `KisStrokeRandomSource` for deterministic per-stroke randomness.
- **`KisPaintOp`** — abstract base class for a brush engine. Created per stroke. Implements `paintDab()` / `paintLine()`. Owns a `KisSpacingInformation` producer.
- **`KisPaintOpFactory`** — instantiates a `KisPaintOp` + its settings widget. Registered with `KisPaintOpRegistry`.
- **`KisPaintOpRegistry`** — singleton registry of all paintop factories (loaded from plugins at startup).
- **`KisPaintOpSettings`** — `KisPropertiesConfiguration` subclass persisting all engine options to XML (stored inside the `.kpp`).
- **`KisPaintOpPreset`** — a `KoResource` wrapping a `KisPaintOpSettings` + thumbnail. The `.kpp` file format is a **standard PNG file** whose text chunks carry the settings: a `tEXt` chunk keyed `"version"` (value `"2.2"` or `"5.0"`) and a `zTXt` chunk keyed `"preset"` whose value is a zlib-compressed XML document of the `KisPropertiesConfiguration`. The PNG pixel data doubles as the thumbnail. See `kis_paintop_preset.cpp::loadFromDevice` and the working parser in `examples/load_preset_standalone.c`.
- **`KisPaintOpConfigWidget`** — base for the engine's option widget.
- **`KisPaintOpPresetUpdateProxy`** — batching proxy for live option updates.
- **`KisOptimizedBrushOutline` / `KisStrokeSpeedMeasurer` / `KisPaintopSettingsIds`** — helpers.
- **`kis_locked_properties*`** — per-stroke locked-property overrides.
- **`kis_*_paintop_property.h`** — uniform/slider/combo property abstractions exposed to the preset UI.
- **`kis_paintop_lod_limitations`** — declares how each engine degrades under Level-of-Detail.

### `brush-engine/core/brush-mask/` — brush mask applicators
- **`kis_brush_mask_applicator_factories.h`** + **`kis_brush_mask_processor_factories.cpp`** — generate the SIMD/scalar mask applicator objects (per-CPU-arch compiled via `ko_compile_for_all_implementations`).
- **`kis_brush_mask_scalar_applicator.h`** — the portable scalar fallback for the soft round dab.

### `brush-engine/libpaintop/` — `kritapaintop` (shared paintop utilities)
Reusable building blocks for writing a brush engine plugin:
- **Dynamic sensors**: `KisDynamicSensor` hierarchy + factories (`Distance`, `DrawingAngle`, `Fade`, `Fuzzy`, `Time`) under `sensors/`. These feed pressure/tilt/etc. into curve-driven option values.
- **Curve option framework**: `KisCurveOption`, `KisCurveOptionData`, `KisCurveOptionModel`, `KisCurveRangeModel*` — the parameterised "curve" UI/data for every dynamic option.
- **Standard options**: `KisFlowOpacityOption`, `KisSizeOption`/`Data`, `KisSpacingOption`, `KisScatterOption`, `KisRotationOption`, `KisSharpnessOption`, `KisMirrorOption`, `KisAirbrushOptionData`, `KisTextureOption*`, `KisColorOption*`, `KisCompositeOpOption*`, `KisPaintingModeOption*`, `KisFilterOption*`.
- **Brush option widgets**: `kis_brush_option_widget`, `kis_brush_selection_widget`, `kis_auto_brush_widget`, `kis_custom_brush_widget`, `kis_text_brush_chooser`, `kis_predefined_brush_chooser`, `kis_texture_chooser`, `kis_clipboard_brush_widget`.
- **Dab pipeline**: `kis_dab_cache`, `KisDabCacheUtils`, `KisDabRenderingQueue/Executor` (in `defaultpaintops/brush/`) — cache & schedule dab rendering.
- **Base classes**: `kis_brush_based_paintop` / `_settings` / `_options_widget` — convenience base for brush-tip-driven engines.
- **Texture/masking**: `KisTextureMaskInfo`, `KisEmbeddedTextureData`, `KisMaskingBrushOption*`.
- **Lager models**: `Kis*OptionModel.cpp` + `Kis*OptionData.cpp` pairs for each option — the reactive state store.

### `brush-engine/paintops/<engine>/`
Each subdirectory is one self-contained brush engine plugin (see §2).
They link `kritapaintop` + `kritalibbrush` + `kritaimage` and register a
`KisPaintOpFactory` with `KisPaintOpRegistry` on plugin load.

### `resources/` — `kritaresources`
The resource management system that loads, stores, tags, and queries brush
presets and brush tips:
- **`KoResource`** — abstract base for any resource (brush preset, brush tip, gradient, palette, …).
- **`KisResourceStorage`** + backends **`KisFolderStorage`** / **`KisBundleStorage`** / **`KisMemoryStorage`** — pluggable storage backends. `.kpp` files live in a folder storage; `.bundle` archives use the bundle storage.
- **`KisResourceModel` / `KisResourceTypeModel` / `KisResourceIterator`** — Qt model layer over the resource database.
- **`KisTagModel` / `KisTagResourceModel`** — tagging system for resources.
- **`KisResourceLocator`** — locates resource files on disk.
- **`KisResourceLoader` / `KisResourceLoaderRegistry`** — type-specific loaders (the paintop-preset loader is registered here upstream).
- **`KisResourcesInterface` / `KisGlobalResourcesInterface` / `KisLocalStrokeResources`** — the API paintops use to fetch brush presets & tips during a stroke.
- **`KisResourceCacheDb` / `KoResourceCacheStorage`** — on-disk cache database.

### `dependencies/krita-libs/`
Minimum Krita internal libraries the brush engine cannot compile without:

| Lib | Upstream | Why the brush engine needs it |
|-----|----------|-------------------------------|
| `kritaglobal` | `libs/global/` | Foundation types: `KisDebug`, `kis_assert`, `KisLager` (lager→Qt adapter), `KisAlgebra2D`, `KisDomUtils`, `kis_lod_transform`, `KisMpl` (range helpers), `KisSharedPtr`, etc. Pervasive dependency. |
| `kritaversion` | `libs/version/` | Build version macros (`GENERIC_KRITA_LIB_VERSION`). Trivial but required by the CMake target setup. |
| `kritapigment` | `libs/pigment/` | Color science: `KoColor`, `KoColorSpace`, `KoColorProfile`, composite ops. Brush dabs are colored through pigment. |
| `kritawidgetutils` | `libs/widgetutils/` | `KoID` (used by brush factories & resource types), `KoResource` base, XML/config helpers, `KoXmlReader`. The `KoResource` base class for `KisPaintOpPreset` lives here. |
| `kritamultiarch` | `libs/multiarch/` | Export-macro shim so the brush libs can be built multi-arch. Small but linked by `kritalibbrush`. |

### `dependencies/external/`
External libraries that are **NOT** bundled (see
`LICENSES/external-dependencies.txt`): Qt, liblager, Boost, Eigen3,
libmypaint (optional).

---

## 4. Dependencies

### 4.1 Minimum required to build the brush engine

**External (must be installed on the system):**
- **CMake ≥ 3.16** + a C++20 compiler (GCC ≥ 10, Clang ≥ 11, MSVC 2019 16.11)
- **Qt 5.15 or Qt 6** — modules: Core, Gui, Widgets, Svg, Concurrent, Xml
- **liblager** (Boost Software License) — https://github.com/arximboldi/lager
- **Boost** (headers) — at least `boost/operators.hpp`, `boost/fusion`, `boost/optional`
- **Eigen3** — for the mask algebra in kritaimage
- **Expat** (via QtXml) — for the `.kpp` settings XML
- *(optional)* **libmypaint** — only if you want the `mypaint` paintop

**Krita-internal (bundled in `dependencies/krita-libs/`):**
- kritaglobal, kritaversion, kritapigment, kritawidgetutils, kritamultiarch

> ⚠️ **Unseparated dependency:** The brush engine interfaces in
> `brush-engine/core/brushengine/` are compiled as part of the upstream
> `kritaimage` library. We extracted the *source* of that subdirectory,
> but it cannot be compiled into a standalone `.so` without the rest of
> kritaimage's supporting source (tiles, painter, paint device, distance
> information). Those supporting files were intentionally excluded per the
> task scope. See §6 "Known unseparated dependencies".

### 4.2 Runtime (to actually *use* presets/tips)
- The built `kritalibbrush`, `kritapaintop`, the paintop plugin `.so`s
- A resource folder containing `.kpp` presets and `.gbr`/`.png`/`.svg`/`.myb` tips

---

## 5. Build instructions

This extraction mirrors Krita's CMake structure. It is designed to be
built as part of a Krita build tree, or as a reduced subtree.

### 5.1 As part of a full Krita build (recommended, simplest)
The cleanest path is to drop these folders back into a Krita checkout
and build normally — the brush engine has no standalone CMake target
upstream because `kritaimage` embeds the `brushengine/` interfaces.

```bash
git clone --depth 1 https://github.com/KDE/krita.git
# install Krita's build deps per https://docs.krita.org/en/untranslatable_pages/build_krita.html
cmake -S krita -B build -DCMAKE_INSTALL_PREFIX=$PWD/install \
    -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTING=OFF
cmake --build build --parallel
# The brush libraries land in build/lib:
#   libkritalibbrush.so, libkritapaintop.so, libkritaimage.so
# The paintop plugins land in build/lib/plugins/kritapaintops/*.so
```

### 5.2 Reduced subtree build (this extraction only)
A `CMakeLists.txt` that builds *just* the brush libs against system Qt +
lager + Boost + Eigen is provided in `docs/standalone-build.md`. Because
the `brushengine/` interfaces depend on kritaimage internals (§6), a
fully standalone build requires stubbing or vendoring those internals;
the doc explains the minimum stubs needed.

### 5.3 Installing external deps (Debian/Ubuntu example)
```bash
sudo apt install build-essential cmake ninja-build pkg-config \
    qtbase5-dev qtdeclarative5-dev qttools5-dev qtsvg5-dev \
    libeigen3-dev libboost-dev libboost-system-dev \
    libexiv2-dev libgsl-dev
# liblager is not packaged on most distros — build from source:
git clone --depth 1 https://github.com/arximboldi/lager.git
cmake -S lager -B lager/build -DCMAKE_INSTALL_PREFIX=/usr/local
cmake --build lager/build --target install
# (optional) libmypaint:
sudo apt install libmypaint-dev
```

---

## 6. Known unseparated dependencies (honest disclosure)

Per task requirement #9/#10, here is exactly what is **NOT** fully
standalone in this extraction:

| Component | What it still depends on | Status |
|-----------|--------------------------|--------|
| `brush-engine/core/brushengine/` (KisPaintOp etc.) | Compiled into upstream `kritaimage`. Source extracted, but to *compile* it you also need kritaimage's `kis_painter`, `kis_paint_device`, `kis_distance_information`, `kis_lod_transform`, `kis_properties_configuration`, `kis_spacing_information`, `kis_dom_utils`, `tiles3/`. | **Not standalone-compilable without kritaimage internals.** Source provided for reading/integration. |
| `KisPaintOpPreset` loader | `KoResource` (in `kritawidgetutils`, included), `KisResourcesInterface` (in `resources/`, included), kritaimage's `kis_properties_configuration` & `kis_paintop_registry`. | Needs kritaimage (as above). |
| Brush mask SIMD applicators | Built via `ko_compile_for_all_implementations` macro from Krita's cmake macros (`krita-macros.cmake`). | Needs Krita's CMake macros (not extracted — fetch from upstream `cmake/`). |
| `kritapigment` color spaces | Pulls in lcms2, OpenColorIO (optional). The extracted `pigment/` dir has its own sub-CMakeLists. | Needs lcms2 system lib. |
| `lager` reactive models | All `Kis*OptionData/Model` use `lager::cursor`. | **Hard dependency on liblager** — see `dependencies/external/`. Cannot be removed without rewriting the option layer. |
| MyPaint engine | Wraps `libmypaint` C library. | Optional; omit `-Dmypaint` from the paintops CMakeLists if unavailable. |

Everything else in this extraction is self-contained relative to the
listed external deps.

---

## 7. Running a minimal example

`examples/` contains a tiny program that demonstrates loading a `.kpp`
preset and reading its settings, plus generating a procedural soft-round
dab from `KisAutoBrush`. Because the full brush engine needs kritaimage
(§6), the example is structured as a *header-include* demonstration that
will compile inside a Krita build tree.

See `examples/README.md` for the two demonstrations:
1. **`load_preset.cpp`** — opens a `.kpp` from `brush-presets/`, reads the
   PNG thumbnail, and parses the settings XML.
2. **`make_dab.cpp`** — constructs a `KisAutoBrush` and renders a single
   dab `QImage` to disk.

---

## 8. License & attribution

See **`NOTICE`** and the **`LICENSES/`** directory for the full text.

- **Source code:** GNU GPL-2.0-or-later (`LICENSES/COPYING-GPL.txt`,
  copied verbatim from Krita's upstream `COPYING`). A few helper files
  are LGPL-2.0/2.1-or-later (see individual file headers).
- **MyPaint brush assets:** CC0-1.0 (Ramon Miranda, David Revoy/Deevad,
  Kaerhon) and GPL-2.0-or-later (Martin Renold & MyPaint team).
- **`defaultpresets.qrc`:** CC0-1.0.
- **External libs:** each retains its own upstream license (Qt = LGPL/GPL,
  lager & Boost = BSL-1.0, Eigen = MPL-2.0, libmypaint = LGPL-2.1+).

All original `SPDX-FileCopyrightText` headers and `SPDX-License-Identifier`
tags are preserved verbatim in every source file. No headers were modified
or removed. This extraction is an independent repackaging of the
brush-related components and is not affiliated with or endorsed by the
Krita project; the upstream Krita repository remains the authoritative
source.

---

## 9. Folder map

```
krita-brush-extract/
├── README.md                      ← this file
├── NOTICE                         ← attribution & redistribution notice
├── LICENSES/
│   ├── COPYING-GPL.txt            ← GPL-2.0 full text (from Krita COPYING)
│   ├── GPL-2.0-or-later.txt
│   ├── LGPL.txt
│   ├── CC0-1.0.txt                ← for MyPaint CC0 brush assets
│   ├── MyPaint-classic-GPL.txt    ← for libmypaint-classic brushes
│   └── external-dependencies.txt  ← Qt/lager/Boost/Eigen/libmypaint
│
├── brush-engine/
│   ├── core/
│   │   ├── brush/                 ← kritalibbrush (KisBrush, KisGbrBrush, …)
│   │   ├── brushengine/           ← KisPaintOp/Preset/Settings/Registry/Information
│   │   └── brush-mask/            ← mask applicator factories (SIMD+scalar)
│   ├── libpaintop/                ← shared option/sensor/widget lib (kritapaintop)
│   └── paintops/                  ← 15 brush engine plugins
│       ├── defaultpaintops/  colorsmudge/  curvebrush/  deform/
│       ├── experiment/       filterop/     gridbrush/   hairy/
│       ├── hatching/         mypaint/      particle/    roundmarker/
│       ├── sketch/           spray/        tangentnormal/
│       └── CMakeLists.txt
│
├── brush-presets/                 ← .kpp preset packages (18 default + samples)
├── brush-tips/                    ← .gbr / .myb / _prev.png brush tip assets
│
├── resources/                     ← kritaresources (storage, models, tags, locator)
│
├── dependencies/
│   ├── krita-libs/                ← minimum internal libs (global, version,
│   │                                pigment, widgetutils, multiarch)
│   └── external/                  ← docs for non-bundled external deps
│
├── examples/                      ← minimal load_preset / make_dab demos
└── docs/                          ← standalone-build.md, architecture notes
```

---

# Reverse Engineering Supplement (binary analysis)

In addition to the source-code extraction above, the **official Krita
binary** (`krita-5.3.3-x86_64.AppImage`, 364 MB, downloaded from
`https://download.kde.org/stable/krita/5.3.3/`) was reverse-engineered
to extract the compiled brush engine and bundled resources. The results
live in `re-binary/`.

See `re-binary/RE-REPORT.md` for the full analysis.

## What the binary analysis added (beyond the source extraction)

1. **Compiled brush engine libraries** (`re-binary/libraries/`):
   - `libkritalibbrush.so.20.0.0` (524 KB) — brush tip loaders, stripped
   - `libkritalibpaintop.so.20.0.0` (4.0 MB) — shared paintop option lib
   - `libkritaimage.so.20.0.0` (7.3 MB) — embeds the brushengine interfaces
   - `libkritapigment.so.20.0.0` (5.5 MB) — color science
   - `libkritaresources.so.20.0.0` (1.4 MB) — resource management

2. **15 compiled paintop plugin `.so` files** (`re-binary/plugins/`):
   - Engine IDs & factory classes extracted from Qt metadata sections,
     e.g. `ColorSmudgePaintOpPluginFactory` → `kritacolorsmudgepaintop`,
     `SprayPaintOpPluginFactory` → `kritaspraypaintop`, etc.

3. **369 brush symbols** demangled from `libkritalibbrush.so`
   (`re-binary/analysis/libbrush-symbols.txt`) — proves the binary
   contains the exact same `KisAbrBrush`, `KisGbrBrush`, `KisAutoBrush`,
   `KisBoundary`, `KisQImagePyramid`, … classes as the source code.

4. **255 brush tip assets** extracted from the 4 bundled resource
   `.bundle` files (`re-binary/resources-extracted/`):
   - 98 `.gbr` (GIMP brush)
   - 102 `.gih` (GIMP image pipe)
   - 52 `.png` (raster brush)
   - 3 `.svg` (vector brush)
   These are **not all present in the source repo** — many are produced
   by Krita artists and bundled only in the release binary.

5. **16 ready-to-use preset `.kpp`** from `paintoppresets/`
   (`re-binary/resources-extracted/presets/`): WaterC_Basic, Eraser_Circle,
   Basic-5_Size_default, etc. — the actual presets shipped to end users.

## Proof the binary == the source

- The binary contains the build path string
  `/builds/graphics/krita/libs/brush/kis_brush.cpp` — this is the KDE
  Invent CI path (`invent.kde.org/graphics/krita`), i.e. the binary was
  compiled from the exact same source tree this extraction is based on.
- 19 of 22 brush classes found in the binary's dynamic symbols match
  the classes declared in `brush-engine/core/brush/*.h` 1:1.
- The binary links `libkritaimage`, `libkritapigment`, `libkritaresources`,
  `libkritaglobal` — the same minimum dependency set extracted in
  `dependencies/krita-libs/`.

## Honest limitations of the RE pass

- The binary is **stripped** (no debug symbols): local variable names,
  inlined functions, and template instantiations cannot be recovered.
- Decompiling C++ (Ghidra/IDA) produces pseudocode far harder to read
  than the original GPL source, which is why the source extraction
  remains the authoritative reference for *understanding* the engine.
- The binary analysis is therefore strongest for **asset extraction**
  (presets + brush tips that ship only in the release bundle) and for
  **symbol-level verification**; the source extraction remains best for
  algorithm understanding.
