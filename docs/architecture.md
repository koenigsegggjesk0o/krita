# Brush Engine Architecture

This document summarises how the extracted brush-engine pieces fit
together, derived from the upstream `brushengine.h` doc comment and the
source in this package.

## The painting pipeline (stroke → dab → pixels)

```
 pointer events
       │
       ▼
 ┌───────────────────────┐
 │ KisPaintInformation   │  pos, pressure, xTilt, yTilt, rotation,
 │  (per tick)           │  tangentialPressure, perspective, time,
 │                       │  speed, isHovering  + random sources
 └───────────┬───────────┘
             │  KisPaintOpRegistry picks the factory for the active preset
             ▼
 ┌───────────────────────┐    creates per stroke
 │ KisPaintOpFactory     │ ───────────────────► ┌────────────────────┐
 └───────────┬───────────┘                      │ KisPaintOp         │
             │                                  │  (paintDab/Line)   │
             │                                  └─────────┬──────────┘
             │                                            │ reads
             ▼                                            ▼
 ┌───────────────────────┐    clones      ┌────────────────────────────┐
 │ KisPaintOpPreset      │ ─────────────► │ KisPaintOpSettings         │
 │  (.kpp KoResource)    │                │  (KisPropertiesConfiguration)│
 │  PNG thumb + gz XML   │                │  -> KisCurveOption x N     │
 └───────────┬───────────┘                │  -> Kis*OptionData x N     │
             │ loaded by                   │     (lager cursors)        │
             ▼                            └─────────┬──────────────────┘
 ┌───────────────────────┐                          │ feeds
 │ KisResourcesInterface │◄─────────────────────────┘
 │  (folder/bundle/sql)  │
 └───────────┬───────────┘
             │ resolves
             ▼
 ┌───────────────────────┐
 │ KisBrush (tip)        │  KisAutoBrush / KisGbrBrush / KisPngBrush /
 │  generateDab()        │  KisSvgBrush / KisAbrBrush / KisImagePipeBrush /
 │                       │  KisTextBrush / KisColorfulBrush
 └───────────┬───────────┘
             │ QImage dab (with mask)
             ▼
        KisPainter::bltFixed  →  KisPaintDevice  →  tile engine  →  canvas
```

## Key data flow facts

1. **A new `KisPaintOp` is created for every stroke** (per the upstream
   doc comment in `brush-engine/core/brushengine/brushengine.h`). The
   stroke owns the paintop; the paintop owns a cloned
   `KisPaintOpSettings` + a `KisDabCache`.

2. **`KisPaintInformation` is the only input the engine sees per tick.**
   It carries pointer state + per-stroke and per-tick randomness
   (`KisPerStrokeRandomSource`, `KisStrokeRandomSource`) so brushes can
   be deterministic given a seed.

3. **Settings are reactive.** Each option (Opacity, Size, Spacing,
   Scatter, Texture, Airbrush, …) is a
   `Kis*OptionData` (value) + `Kis*OptionModel` (Qt-facing) pair backed
   by a `lager::cursor`. This is why liblager is a hard, non-removable
   dependency — the entire option layer is built on it.

4. **Brush tip vs. brush engine are separate.** `KisBrush` (the *tip*)
   lives in `kritalibbrush`. `KisPaintOp` (the *engine*) lives in
   `kritaimage`'s `brushengine/` subdir. A paintop uses a tip to produce
   dabs; the same tip can be shared across engines (e.g. the round
   default engine and the colorsmudge engine both consume `KisBrush`).

5. **The `.kpp` file** (`brush-engine/core/brushengine/kis_paintop_preset.cpp`):
   - It is a **standard PNG file**. The thumbnail is the PNG pixel data.
   - The settings XML is stored *inside* the PNG as PNG text chunks:
     a `tEXt` chunk keyed `"version"` (value `"2.2"` or `"5.0"`) and a
     `zTXt` chunk keyed `"preset"` whose value is a zlib-compressed XML
     document of the brush settings (`<Preset name="…" paintopid="…">…`).
   - read: `QImageReader::text("version")` + `QImageReader::text("preset")`,
     then `QDomDocument::setContent(preset)`.
   - Confirmed by binary inspection: every `.kpp` begins with PNG
     signature `89 50 4E 47 …` and the settings live in `zTXt`/`tEXt`
     chunks. See `examples/load_preset_standalone.c` for a working
     zero-dependency C parser that extracts both the thumbnail PNG and
     the settings XML from any `.kpp`.

## Brush tip formats supported (kritalibbrush)

| Format | Extension | Loader class |
|--------|-----------|--------------|
| GIMP brush | `.gbr` | `KisGbrBrush` |
| Photoshop brush tool preset | `.abr` | `KisAbrBrush` / `KisAbrBrushCollection` (+ `KisAbrStorage`) |
| GIMP image pipe | `.gih`-style | `KisImagePipeBrush` + `kis_pipebrush_parasite` |
| PNG raster | `.png` | `KisPngBrush` |
| SVG vector | `.svg` | `KisSvgBrush` (needs Qt Svg) |
| Text glyph | — | `KisTextBrush` |
| Procedural soft round | — | `KisAutoBrush` (mask via `kis_brush_mask_*_applicator`) |
| Color-retaining | — | `KisColorfulBrush` (mix-in) |

## The 15 brush engines (paintops) and what they do

| Engine | Produces | Distinctive option |
|--------|----------|--------------------|
| defaultpaintops (KisBrushOp) | standard pixel stamp along the stroke | dab-rendering queue/executor for cached dabs |
| colorsmudge | smudge — samples canvas color and smears it | smudge rate, smear vs. dabs-per-radius |
| curvebrush | interpolated curve through points | curve density, line width |
| deform | pixel deformation at the cursor | deform mode (grow/shrink/swirl/etc.) |
| experiment | recursive stroke | connected-line density |
| filterop | runs a filter along the stroke | filter id selection |
| gridbrush | snaps dabs to a grid | grid spacing |
| hairy | multi-bristle strands | bristle count, ink depletion |
| hatching | parallel hatch lines | angle, spacing, cross-hatch |
| mypaint | libmypaint engine | wraps `libmypaint` (optional dep) |
| particle | particle simulation | particle count, gravity, iterations |
| roundmarker | hard round marker | opaque flat stamp |
| sketch | sketchy/vibrating lines | line density, randomness |
| spray | scatter particles | particle count, distribution, metaball option |
| tangentnormal | tangent-space normal map | XYZ normal channels |

## Where each piece lives in this extraction

| Concept | Path |
|---------|------|
| `KisPaintInformation`, `KisPaintOp`, `KisPaintOpPreset`, `KisPaintOpRegistry`, `KisPaintOpSettings`, `KisPaintOpFactory` | `brush-engine/core/brushengine/` |
| `KisBrush`, `KisAutoBrush`, `KisGbrBrush`, `KisAbrBrush`, `KisImagePipeBrush`, `KisPngBrush`, `KisSvgBrush`, `KisTextBrush`, `KisColorfulBrush`, `KisBrushRegistry`, `KisBrushFactory`, `KisQImagePyramid`, `KisBrushModel` (lager) | `brush-engine/core/brush/` |
| Brush mask SIMD/scalar applicators | `brush-engine/core/brush-mask/` |
| `KisDynamicSensor*`, `KisCurveOption*`, `Kis*OptionData`, `Kis*OptionModel`, `kis_brush_option_widget`, `kis_dab_cache`, `KisTextureMaskInfo`, `kis_brush_based_paintop` | `brush-engine/libpaintop/` |
| The 15 concrete engines | `brush-engine/paintops/<engine>/` |
| `KoResource`, `KisResourceStorage` (+ folder/bundle/memory backends), `KisResourceModel`, `KisTagModel`, `KisResourceLocator`, `KisResourceLoaderRegistry`, `KisResourcesInterface` | `resources/` |
| `.kpp` preset packages | `brush-presets/` |
| `.gbr` / `.myb` / `_prev.png` brush tips | `brush-tips/` |
| `kritaglobal`, `kritaversion`, `kritapigment`, `kritawidgetutils`, `kritamultiarch` | `dependencies/krita-libs/` |

## Why kritaimage is the hard coupling

`KisPaintOp::paintDab()` must write into a `KisPaintDevice`, which is the
kritaimage raster buffer (tile-managed by `tiles3/`). The brush engine
*generates* dabs (`KisBrush::generateDab` in kritalibbrush — separable)
but *applies* them through `KisPainter` (kritaimage — not separable).
This is why `brush-engine/core/brushengine/` source is provided for
reading and integration, but its compilation requires kritaimage. See
`docs/standalone-build.md` and `README.md` §6.
