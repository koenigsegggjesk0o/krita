# Feather Krita — 3D Drawing App

SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
SPDX-License-Identifier: GPL-2.0-or-later

> **3D drawing app** combining the **Krita brush engine** (100% unmodified KDE code) with **Godot 4** as the 3D rendering and UI layer.
>
> Inspired by [Feather: Draw in 3D](https://www.feather.art) for iPad, this project targets **Windows + Android** with a **freemium model** (free to use, pay to export).

[![License: GPL-2.0-or-later](https://img.shields.io/badge/license-GPL--2.0--or--later-blue.svg)](https://spdx.org/licenses/GPL-2.0-or-later.html)

---

## Table of Contents

- [Overview](#overview)
- [Architecture](#architecture)
- [How Krita Brush Engine is Integrated WITHOUT Modification](#how-krita-brush-engine-is-integrated-without-modification)
- [Features](#features)
- [Project Structure](#project-structure)
- [Build Instructions](#build-instructions)
- [Usage](#usage)
- [License](#license)

---

## Overview

Feather Krita is a 3D **drawing** app (not sculpting). Every brush stroke is an independent **3D entity** with:

- XYZ position in 3D space
- Thickness (per-point, modulated by pressure)
- Color (RGBA)
- Rotation & scale (transform)

The user paints onto a **3D guide surface** (sphere, cylinder, cone, ring). The 2D brush stroke is mapped to the surface via UV coordinates. Strokes can then be selected, moved, rotated, scaled, mirrored, and liquified in 3D — exactly like Feather 3D.

### Key differentiators vs. Feather 3D

| Aspect | Feather 3D | Feather Krita |
|--------|-----------|---------------|
| Platform | iPad only | **Windows + Android** |
| Brush engine | Custom "Airbreath" | **Krita brush engine (15 engines, hundreds of presets)** |
| Pricing | $14.99 one-time | **Freemium** (free to use, pay to export) |
| Concept | 3D drawing | **3D drawing (same concept)** |

---

## Architecture

```
┌─────────────────────────────────────────────────────────┐
│                    Godot Engine 4 (C++)                  │
│  ┌────────────────────────────────────────────────────┐  │
│  │            3D Viewport (Forward+ renderer)         │  │
│  │   • Guide Surface mesh (sphere/cylinder/cone/ring) │  │
│  │   • Stroke meshes (tube/ribbon per stroke)         │  │
│  │   • Lighting + shadows (real-time)                 │  │
│  └────────────────────────────────────────────────────┘  │
│  ┌────────────────────────────────────────────────────┐  │
│  │           UI Layer (Godot Control + TSCN)          │  │
│  │   • Bottom toolbar (Draw/Erase/Shapes/Liquify/...)  │  │
│  │   • Joystick (Move/Rotate/Scale/Liquify modes)     │  │
│  │   • Color picker + brush size/opacity sliders      │  │
│  │   • Export dialog (with freemium lock)             │  │
│  └────────────────────────────────────────────────────┘  │
│  ┌────────────────────────────────────────────────────┐  │
│  │       Bridge Code (C++ GDExtension, OUR code)      │  │
│  │   • FeatherGuideSurface   → guide surface + raycast│  │
│  │   • FeatherStrokeManager  → 3D stroke CRUD + edit  │  │
│  │   • FeatherExportLock     → freemium license logic │  │
│  │   • FeatherKritaBrush     → calls Krita brush API  │  │
│  │   • FeatherTexturePainter → dab → UV → texture map │  │
│  └────────────────────────────────────────────────────┘  │
│  ┌────────────────────────────────────────────────────┐  │
│  │   Krita Brush Engine (100% UNMODIFIED KDE code)    │  │
│  │   • libkritalibbrush.so / .dll (from krita-source) │  │
│  │   • libs/brush/, libs/image/brushengine/,          │  │
│  │     libs/pigment/, libs/resources/                 │  │
│  └────────────────────────────────────────────────────┘  │
└─────────────────────────────────────────────────────────┘
```

### Data flow (per dab)

```
1. User touches screen
        ↓
2. Godot raycasts from camera through screen point
        ↓
3. FeatherGuideSurface.raycast() → RaycastHit { point, uv, normal }
        ↓
4. FeatherKritaBrush.generate_dab(uv, pressure, tilt, ...)
        ↓
5. KritaBrushWrapper calls KisBrush::generateDab()  ← KRITA, UNMODIFIED
        ↓
6. Returns QImage dab (2D)
        ↓
7. FeatherTexturePainter.apply_dab(dab, uv)
        ↓
8. QImage texture updated at UV position
        ↓
9. Godot re-uploads texture to GPU → mesh re-rendered
        ↓
10. User sees 3D stroke painted on guide surface
```

---

## How Krita Brush Engine is Integrated WITHOUT Modification

This is the most important architectural principle of the project. We **never** modify a single line of Krita source code.

### What stays untouched

Every file under `krita-source/libs/` is the **original KDE code**, including:

| Path | Description |
|------|-------------|
| `krita-source/libs/brush/` | `kis_brush.cpp`, `kis_auto_brush.cpp`, `kis_gbr_brush.cpp`, `kis_abr_brush.cpp`, `kis_brush_registry.cpp`, `KisBrushModel.cpp` |
| `krita-source/libs/image/brushengine/` | `kis_paintop_preset.cpp`, `kis_paintop.cpp`, `kis_paint_information.h` |
| `krita-source/libs/pigment/` | `KoColor.cpp`, `KoColorSpace.cpp`, color profiles |
| `krita-source/libs/resources/` | `KisResourcesInterface.cpp`, resource loaders |
| `krita-source/plugins/paintops/` | All 15 paintop plugins (airbrush, particle, hairy, etc.) |

### What we add (bridge code only)

| File | Role |
|------|------|
| `source/src/krita_bridge/krita_brush_wrapper.{h,cpp}` | Wraps `KisBrush` and `KisPaintOpPreset` API. Calls `KisBrush::generateDab()` directly — no Krita code touched. |
| `source/src/krita_bridge/texture_painter.{h,cpp}` | Applies the QImage dab to a texture at UV coordinates. Pure Qt code, no Krita dependency. |
| `source/src/guide_surface/guide_surface.{h,cpp}` | Procedural mesh generation + Möller–Trumbore raycast. Pure Qt code. |
| `source/src/stroke_manager/stroke_manager.{h,cpp}` | 3D stroke CRUD + transforms + liquify + mirror. Pure Qt code. |
| `source/src/export/export_lock.{h,cpp}` | Freemium license policy (offline SHA-256 key verification + store purchase hooks). Pure Qt code. |
| `source/src/gdextension_entry.cpp` | Registers the above as Godot GDExtension types via `godot-cpp`. Pure binding code. |

### How the bridge calls Krita

`KritaBrushWrapper::generateDab()` does this (excerpt):

```cpp
// Build KisPaintInformation (Krita's data class — unchanged)
KisPaintInformation paintInfo(input.position, input.pressure,
                              input.xTilt, input.yTilt, ...);

// Get brush from preset (Krita's class — unchanged)
KisPaintOpPreset* preset = static_cast<KisPaintOpPreset*>(m_preset);
KisBrushSP brush = preset->brush();

// CALL KRITA'S ORIGINAL FUNCTION — UNMODIFIED
QImage dabImage = brush->generateDab(
    shape,              // scale + rotation
    paintInfo,          // paint info
    0, 0,               // subpixel
    cs,                 // color space
    *color              // brush color
);
```

We only **call** Krita's public API. The brush engine runs **exactly as in Krita itself**.

### License implications

Because we link against Krita (which is GPL-2.0-or-later), our bridge code and the entire app are licensed under **GPL-2.0-or-later** to comply with GPL's viral clause.

---

## Features

### Drawing tools
- **3D Brushes** — pressure-sensitive Krita brushes (15 engines, hundreds of presets)
- **Eraser** — destination-out composite
- **Shapes** — straight lines, circles, ellipses, smooth curves
- **3D Guide Surfaces** — sphere, cylinder, cone, ring (custom UV mapping)

### Editing tools (Feather-style)
- **3D Liquify** — push / pull / smooth / inflate / deflate / twirl
- **Virtual Joystick** — Move / Rotate / Scale / Liquify modes ("editing feels like gaming")
- **Select / Deselect** — pick individual strokes for editing
- **Live Mirror** — symmetrical drawing across X / Y / Z axes

### Visual
- Real-time lighting & shadows
- PBR materials
- Transparent guide surfaces

### I/O
- **Import**: Krita brush presets (`.kpp`), OBJ models
- **Export** (freemium):
  - **Free**: `.feather` project file (save/load)
  - **Pro**: PNG, JPEG, OBJ, glTF, GIF, MP4

### Freemium license
- Offline license key verification (SHA-256 hash check)
- Store purchase hooks (Google Play Billing, Windows Store)
- Optional 7-day free trial

---

## Project Structure

```
feather-krita-app/
├── docs/
│   └── FEATHER-3D-RESEARCH.md         # Deep research on Feather 3D
├── research/                          # Market research artifacts
├── source/
│   ├── CMakeLists.txt                 # Build system for GDExtension lib
│   ├── feather_krita.gdextension      # Auto-generated by CMake
│   ├── project.godot                  # Godot 4 project config
│   ├── scenes/
│   │   ├── main.tscn                  # Main scene (3D viewport + UI)
│   │   └── main.gd                    # UI / drawing logic (GDScript)
│   ├── resources/                     # Brushes, presets, themes
│   └── src/
│       ├── gdextension_entry.cpp      # GDExtension entry + class registration
│       ├── krita_bridge/              # Bridge to Krita brush engine
│       │   ├── krita_brush_wrapper.{h,cpp}
│       │   └── texture_painter.{h,cpp}
│       ├── guide_surface/             # 3D guide surfaces
│       │   └── guide_surface.{h,cpp}
│       ├── stroke_manager/            # 3D stroke management
│       │   └── stroke_manager.{h,cpp}
│       ├── export/                    # Freemium export lock
│       │   └── export_lock.{h,cpp}
│       └── ui/                        # (Future: C++ UI helpers)
└── README.md                          # This file
```

---

## Build Instructions

### Prerequisites

1. **Qt 6.5+** (Core, Gui, Widgets, Network modules)
2. **CMake 3.22+**
3. **C++20 compiler** (GCC 11+, Clang 14+, MSVC 2022+)
4. **Godot 4.3+** (for running the app)
5. **godot-cpp** (cloned from https://github.com/godotengine/godot-cpp)
6. **Krita source** — for headers + the compiled `libkritalibbrush.so/.dll`

### Step 1: Build Krita brush engine

Follow KDE's build instructions to produce `libkritalibbrush.so` (Linux/Android) or `kritalibbrush.dll` (Windows):

```bash
git clone https://invent.kde.org/graphics/krita.git krita-source
mkdir krita-build && cd krita-build
cmake ../krita-source -DCMAKE_BUILD_TYPE=Release \
    -DBUILD_TESTING=OFF \
    -DKRITA_BROKEN_TESTS=OFF
cmake --build . --target kritalibbrush -j$(nproc)
```

After build, `libkritalibbrush.so` lives in `krita-build/lib/` (Linux) or `krita-build/bin/` (Windows).

### Step 2: Build godot-cpp

```bash
git clone https://github.com/godotengine/godot-cpp.git
cd godot-cpp
# Linux:
scons platform=linux target=template_release generate_bindings=yes
# Windows:
scons platform=windows target=template_release generate_bindings=yes
# Android (cross-compile):
scons platform=android target=template_release arch=arm64 generate_bindings=yes
```

### Step 3: Build Feather Krita GDExtension

```bash
cd /home/z/feather-krita-app/source
mkdir build && cd build
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DKRITA_SOURCE_DIR=/path/to/krita-source \
    -DKRITA_BUILD_DIR=/path/to/krita-build \
    -DGODOT_CPP_DIR=/path/to/godot-cpp \
    -DQt6_DIR=/path/to/Qt/6.7.0/gcc_64/lib/cmake/Qt6
cmake --build . --config Release -j$(nproc)
```

Output (in `build/bin/`):
- Linux: `libfeather_krita.linux.release.x86_64.so`
- Windows: `feather_krita.windows.release.x86_64.dll`
- Android: `libfeather_krita.android.release.arm64.so`

### Step 4: Run the app

```bash
# Copy the .so/.dll into the source/bin/ folder (where project.godot expects it)
mkdir -p /home/z/feather-krita-app/source/bin
cp build/bin/libfeather_krita.*.so /home/z/feather-krita-app/source/bin/

# Open the project in Godot 4 editor
godot4 --editor --path /home/z/feather-krita-app/source/
```

Or run from CLI:

```bash
godot4 --path /home/z/feather-krita-app/source/
```

---

## Usage

### Basic workflow

1. **Launch app** — a default sphere guide surface appears.
2. **Draw** — click/drag on the guide surface. Brush dabs are painted via Krita engine onto the surface texture.
3. **Switch guide** — toolbar → **Shapes** → pick Sphere / Cylinder / Cone / Ring.
4. **Erase** — toolbar → **Erase** → paint to erase.
5. **Select & edit** — toolbar → **Select** → tap a stroke → use joystick (bottom-left) to **Move** / **Rotate** / **Scale**.
6. **Liquify** — toolbar → **Liquify** → drag to push/pull strokes in 3D. Switch joystick mode to **Liquify** for joystick-driven deform.
7. **Mirror** — enable X/Y/Z mirror (top indicators turn green). New strokes auto-mirror.
8. **Light** — toolbar → **Light** cycles through lighting presets.
9. **Export** — toolbar → **Export** → pick format. Free users can only save `.feather` project files. Pro users unlock PNG/JPEG/OBJ/glTF/MP4/GIF.

### Unlocking Pro

Toolbar → **Export** → **Unlock Export** → enter license key (`XXXX-XXXX-XXXX-XXXX`) or start 7-day trial.

License keys are verified offline via SHA-256 hash prefix check. For production, integrate Google Play Billing or Windows Store for purchase verification.

### Keyboard shortcuts

| Key | Action |
|-----|--------|
| `B` | Draw |
| `E` | Erase |
| `S` | Shapes |
| `L` | Liquify |
| `V` | Select |
| `H` | Light |
| `X` | Export |
| `,` | Settings |
| `Ctrl+Z` | Undo |
| `Ctrl+Shift+Z` | Redo |
| `Shift+X` | Toggle X mirror |
| `Shift+Y` | Toggle Y mirror |
| `Shift+Z` | Toggle Z mirror |
| `Alt + LMB drag` | Orbit camera |
| `Shift + RMB drag` | Pan camera |
| `Mouse wheel` | Zoom |

### Touch gestures

| Gesture | Action |
|---------|--------|
| Single finger | Draw |
| Two-finger drag | Orbit |
| Pinch | Zoom |
| Two-finger pan | Pan |

---

## License

**GPL-2.0-or-later**

```
SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
SPDX-License-Identifier: GPL-2.0-or-later
```

This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program; if not, write to the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.

### Why GPL?

We link against Krita's brush engine (`libkritalibbrush`), which is licensed under GPL-2.0-or-later. To comply with GPL's copyleft clause, the entire Feather Krita app must also be GPL-2.0-or-later.

### Third-party components

| Component | License |
|-----------|---------|
| Krita brush engine | GPL-2.0-or-later (KDE) |
| Godot Engine | MIT |
| godot-cpp | MIT |
| Qt6 | GPL-3.0 / LGPL-3.0 / Commercial |

---

## Acknowledgements

- [KDE Krita team](https://krita.org) — for the brush engine and paintop plugins
- [Godot Engine contributors](https://godotengine.org) — for the best 3D open-source engine
- [Sketchsoft Inc](https://www.feather.art) — for the original Feather 3D inspiration
