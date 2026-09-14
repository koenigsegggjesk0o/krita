# Standalone build notes (reduced subtree)

This document explains how to build **only the brush-engine code in this
extraction** against system Qt + lager + Boost + Eigen, without the rest
of Krita.

> **Read this carefully.** As stated in README.md §6, the
> `brush-engine/core/brushengine/` interfaces (`KisPaintOp`,
> `KisPaintOpPreset`, …) are compiled *into* `kritaimage` upstream.
> They `#include` kritaimage-internal headers that are NOT in this
> extraction (e.g. `kis_paint_device.h`, `kis_painter.h`,
> `kis_properties_configuration.h`, `kis_distance_information.h`,
> `kis_lod_transform.h`, `kis_spacing_information.h`, `kis_dom_utils.h`,
> and the `tiles3/` tile engine). A fully standalone build therefore
> requires either (a) linking the real kritaimage, or (b) providing
> minimal stubs for those headers. Both options are documented below.

## Option A — build against a real kritaimage (simplest)

Keep this extraction as a *reference* and build the real Krita tree.

```bash
git clone --depth 1 https://github.com/KDE/krita.git
# (drop brush-engine/core/brushengine/ from this extraction over the
#  matching upstream dir if you have local edits)
cmake -S krita -B build -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTING=OFF
cmake --build build --target kritalibbrush kritaimage kritapaintop --parallel
```

The produced libs (`libkritalibbrush.so`, `libkritaimage.so`,
`libkritapaintop.so`) and the paintop plugin `.so`s in
`build/lib/plugins/kritapaintops/` are exactly what the brush engine
consists of at runtime.

## Option B — reduced subtree with stubs (advanced)

To compile this extraction *alone*, you must provide stub headers for
the kritaimage internals the `brushengine/` sources include. The minimal
stub set (each is a tiny forward-declaration header) is:

```
stubs/
  kis_paint_device.h          // forward-decl KisPaintDeviceSP; empty impl
  kis_painter.h               // forward-decl KisPainter
  kis_properties_configuration.h  // minimal QMap<QString,QString> wrapper
  kis_distance_information.h  // trivial struct { qreal distance; }
  kis_lod_transform.h         // no-op
  kis_spacing_information.h   // struct { QPointF; qreal; bool isotropic; }
  kis_dom_utils.h             // round()/toString() helpers
  kis_algebra_2d.h            // already in kritaglobal (included)
  kis_assert.h                // already in kritaglobal (included)
  kis_paintop_registry.h      // the singleton (real impl is in brushengine/)
  kis_shared_ptr.h            // already in kritaglobal (included)
```

A starting `CMakeLists.txt` for the reduced subtree:

```cmake
cmake_minimum_required(VERSION 3.16)
project(krita_brush_engine_extract LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt5 COMPONENTS Core Gui Widgets Svg Xml REQUIRED)
find_package(Eigen3 REQUIRED)
find_package(Boost REQUIRED)
find_package(lager REQUIRED)  # BSL-1.0, install from github.com/arximboldi/lager

# --- kritaglobal ---
add_library(kritaglobal STATIC
    dependencies/krita-libs/global/kis_debug.cpp
    dependencies/krita-libs/global/kis_algebra_2d.cpp
    # …add the .cpp files listed in libs/global/CMakeLists.txt…
)
target_include_directories(kritaglobal PUBLIC dependencies/krita-libs/global)
target_link_libraries(kritaglobal PUBLIC Qt5::Core Boost::boost lager::lager)

# --- kritaversion (header-only macros) ---
add_library(kritaversion INTERFACE)
target_include_directories(kritaversion INTERFACE dependencies/krita-libs/version)

# --- kritapigment (subset needed by brush) ---
add_library(kritapigment STATIC
    dependencies/krita-libs/pigment/KoColor.cpp
    dependencies/krita-libs/pigment/KoColorSpace.cpp
    # … see libs/pigment/CMakeLists.txt …
)
target_link_libraries(kritapigment PUBLIC kritaglobal Eigen3::Eigen)

# --- kritawidgetutils (KoResource, KoID) ---
add_library(kritawidgetutils STATIC
    dependencies/krita-libs/widgetutils/KoResource.cpp
    dependencies/krita-libs/widgetutils/KoID.cpp
)
target_link_libraries(kritawidgetutils PUBLIC kritaglobal Qt5::Widgets)

# --- kritaresources ---
add_library(kritaresources STATIC
    resources/KoResource.cpp
    resources/KisResourceStorage.cpp
    resources/KisResourceModel.cpp
    # … see libs/resources/CMakeLists.txt …
)
target_link_libraries(kritaresources PUBLIC kritaglobal kritawidgetutils Qt5::Sql)

# --- stub kritaimage internals (Option B only) ---
add_library(kritaimage_stubs INTERFACE)
target_include_directories(kritaimage_stubs INTERFACE stubs)

# --- kritalibbrush ---
add_library(kritalibbrush SHARED
    brush-engine/core/brush/kis_brush.cpp
    brush-engine/core/brush/kis_auto_brush.cpp
    brush-engine/core/brush/kis_gbr_brush.cpp
    brush-engine/core/brush/kis_abr_brush.cpp
    brush-engine/core/brush/kis_abr_brush_collection.cpp
    brush-engine/core/brush/kis_imagepipe_brush.cpp
    brush-engine/core/brush/kis_pipebrush_parasite.cpp
    brush-engine/core/brush/kis_png_brush.cpp
    brush-engine/core/brush/kis_svg_brush.cpp
    brush-engine/core/brush/kis_text_brush.cpp
    brush-engine/core/brush/kis_qimage_pyramid.cpp
    brush-engine/core/brush/kis_scaling_size_brush.cpp
    brush-engine/core/brush/kis_brush_registry.cpp
    brush-engine/core/brush/kis_predefined_brush_factory.cpp
    brush-engine/core/brush/kis_auto_brush_factory.cpp
    brush-engine/core/brush/kis_text_brush_factory.cpp
    brush-engine/core/brush/KisAbrStorage.cpp
    brush-engine/core/brush/KisColorfulBrush.cpp
    brush-engine/core/brush/KisBrushTypeMetaDataFixup.cpp
    brush-engine/core/brush/KisBrushModel.cpp
    brush-engine/core/brush/KisBrushServerProvider.cpp
    brush-engine/core/brush/kis_boundary.cc
)
target_link_libraries(kritalibbrush PUBLIC
    kritaimage_stubs kritaglobal kritapigment kritawidgetutils
    Qt5::Core Qt5::Gui Qt5::Svg Boost::boost lager::lager)
generate_export_header(kritalibbrush BASE_NAME kritabrush
    EXPORT_MACRO_NAME BRUSH_EXPORT)

# --- kritapaintop (libpaintop) ---
# (mirror the .cpp list from plugins/paintops/libpaintop/CMakeLists.txt)
# add_library(kritapaintop SHARED ...)

# --- paintop plugins (one .so each) ---
# foreach(engine IN ITEMS defaultpaintops spray ...) ...
```

## What still won't link standalone

Even with the stubs above, the following will NOT compile without real
kritaimage internals, because they touch `KisPaintDevice`,
`KisPainter::bltFixed`, or the tile engine:

- `brush-engine/core/brushengine/kis_paintop.cc` (the `paintDab` base
  implementation dispatches to `KisPainter`)
- `brush-engine/core/brushengine/kis_paintop_utils.cpp`
- every `paintops/<engine>/<engine>_paintop.cpp` that actually paints
  (i.e. all of them — that's their job)

So Option B yields a `kritalibbrush.so` (brush *tip* loading + the
KisAutoBrush dab generator) that is genuinely standalone, but the
*engine* layer that paints onto a canvas is not separable from
kritaimage. This is the fundamental architectural coupling documented in
README.md §6.

## Recommendation

For anything beyond reading the source, use **Option A** (full Krita
build) and treat this extraction as a curated reference + the brush
preset/tip assets, which are fully usable as-is in a real Krita install.
