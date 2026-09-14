# Reverse Engineering Report — Krita Binary Brush Engine

SPDX-FileCopyrightText: 2026 Krita Brush Engine Extraction Contributors
SPDX-License-Identifier: GPL-2.0-or-later

## Sumber binary

- **File**: krita-5.3.3-x86_64.AppImage (364 MB)
- **URL**: https://download.kde.org/stable/krita/5.3.3/krita-5.3.3-x86_64.AppImage
- **Tanggal build**: 2025-07-26 (dari timestamp file)
- **Format**: ELF 64-bit x86-64, static-pie, **stripped** (debug symbols dihapus)
- **BuildID**: a87aaf5da1bf2ef30becedd1aa58e217818fb503
- **Path build internal**: /builds/graphics/krita/ (KDE Invent CI)

## Tools yang dipakai

- `extract` (AppImage --appimage-extract) untuk ekstrak squashfs
- `nm -D` + `c++filt` untuk dynamic symbols & demangling
- `objdump -d` untuk disassembly
- `readelf` untuk ELF header analysis
- `strings` untuk string extraction
- `unzip` untuk ekstrak .bundle (resource bundles = ZIP)

## Binary brush engine yang diekstrak

### Core libraries (lib/)

| Library | Ukuran | Fungsi |
|---------|--------|--------|
| `libkritalibbrush.so.20.0.0` | 524K | Brush tip loading (KisBrush, KisGbrBrush, KisAutoBrush, dll) |
| `libkritalibpaintop.so.20.0.0` | 4.0M | Shared paintop option/sensor/widget lib |
| `libkritaimage.so.20.0.0` | 7.3M | Image engine (embeds brushengine/ interfaces) |
| `libkritapigment.so.20.0.0` | 5.5M | Color science (KoColor, KoColorSpace) |
| `libkritaresources.so.20.0.0` | 1.4M | Resource management (KoResource, storage, models) |

### Paintop plugins (kritaplugins/) — 15 brush engine

| Plugin | Ukuran | Factory Class (Qt metadata) | Engine ID |
|--------|--------|------------------------------|-----------|
| `colorsmudge` | 1.7M | `ColorSmudgePaintOpPluginFactory` | `kritacolorsmudgepaintop` |
| `curve` | 596K | `YCurvePaintOpPluginFactory` | `kritacurvepaintop` |
| `defaults` | 1.1M | `DefaultPaintOpsPluginFactory` | `kritadefaultpaintop` |
| `deform` | 804K | `ZDeformPaintOpPluginFactory` | `kritadeformpaintop` |
| `experiment` | 472K | `ExperimentPaintOpPluginFactory` | `kritaexperimentpaintop` |
| `grid` | 624K | `XGridPaintOpPluginFactory` | `kritagridpaintop` |
| `hairy` | 784K | `YHairyPaintOpPluginFactory` | `kritahairypaintop` |
| `hatching` | 1.0M | `HatchingPaintOpPluginFactory` | `kritahatchingpaintop` |
| `my` | 3.6M | `CvMyPaintOpPluginFactory` | `kritamypaintop` |
| `particle` | 524K | `ParticlePaintOpPluginFactory` | `kritaparticlepaintop` |
| `roundmarker` | 500K | `RoundMarkerPaintOpPluginFactory` | `kritaroundmarkerpaintop` |
| `sketch` | 848K | `ZSketchPaintOpPluginFactory` | `kritasketchpaintop` |
| `spray` | 1.5M | `YSprayPaintOpPluginFactory` | `kritaspraypaintop` |
| `tangentnormal` | 708K | `TangentNormalPaintOpPluginFactory` | `kritatangentnormalpaintop` |

### Brush tip import/export plugins
### Brush tip import/export plugins

| Plugin | Fungsi |
|--------|--------|
| `kritabrushimport.so` | Import brush dari berbagai format |
| `kritabrushexport.so` | Export brush ke berbagai format |
| `kritabrushhud.so` | Brush HUD (Heads-Up Display) |

## Symbol yang ter-ekstrak dari binary

Meskipun binary di-strip, **dynamic symbols (exported C++ symbols) tetap ada**.
Total symbol brush ter-ekstrak dari libkritalibbrush.so: **369**

### Class brush di binary (match dengan source code)

- `KisAbrBrush`
- `KisAbrBrushCollection`
- `KisAbrStorage`
- `KisAutoBrush`
- `KisAutoBrushFactory`
- `KisBoundary`
- `KisBrushFactory`
- `KisBrushModel`
- `KisBrushRegistry`
- `KisBrushServerProvider`
- `KisColorfulBrush`
- `KisGbrBrush`
- `KisImagePipeBrush`
- `KisOptimizedBrushOutline`
- `KisPipeBrushParasite`
- `KisPngBrush`
- `KisPredefinedBrushFactory`
- `KisQImagePyramid`
- `KisScalingSizeBrush`
- `KisSvgBrush`
- `KisTextBrush`
- `KisTextBrushFactory`

### Method kritis ter-ekstrak (buktikan algoritma brush engine ada di binary)

- `KisAutoBrush::generateMaskAndApplyMaskOrCreateDab()` — function inti dab generator
- `KisAutoBrush::maskHeight()` / `maskWidth()` / `maskGenerator()` — dimensi mask
- `KisAutoBrush::createBrushPreview()` — preview generator
- `KisGbrBrush::makeMaskImage()` / `KisImagePipeBrush::makeMaskImage()` — mask image builder
- `KisAbrBrush::loadFromDevice()` — ABR (Photoshop) brush loader
- `KisGbrBrush::loadFromDevice()` / `initFromPaintDev()` — GBR (GIMP) brush loader
- `KisBoundary::generateBoundary()` — brush outline boundary

### Dependency mask generator (terungkap dari symbol 'U' = undefined/imported)

- `KisCircleMaskGenerator`
- `KisCurveCircleMaskGenerator`
- `KisCurveRectangleMaskGenerator`
- `KisGaussCircleMaskGenerator`
- `KisGaussRectangleMaskGenerator`
- `KisMaskGenerator`
- `KisRectangleMaskGenerator`

## Resource brush yang diekstrak dari AppImage

### Preset .kpp siap pakai (dari paintoppresets/)

Total: **16 preset**

- a)_Eraser_Circle.kpp
- b)_Basic-5_Size_default.kpp
- j)_WaterC_Basic_Lines-Dry.kpp
- j)_WaterC_Basic_Lines-Wet-Pattern.kpp
- j)_WaterC_Basic_Lines-Wet.kpp
- j)_WaterC_Basic_Round-Fringe_02.kpp
- j)_WaterC_Basic_Round-Grain.kpp
- j)_WaterC_Basic_Round-Grunge.kpp
- j)_WaterC_Flat_Big-Grain_Tilt.kpp
- j)_WaterC_Flat_Decay_Tilt.kpp
- j)_WaterC_Special_Blobs.kpp
- j)_WaterC_Special_Splats.kpp
- j)_WaterC_Spread-Pattern.kpp
- j)_WaterC_Spread.kpp
- j)_WaterC_Spread_WideArea.kpp
- j)_WaterC_Water-Pattern.kpp

### Brush tips dari 4 resource bundle

| Format | Jumlah | Sumber |
|--------|--------|--------|
| `.gbr` (GIMP brush) | 98 | Krita_3/4_Default_Resources.bundle |
| `.gih` (GIMP image pipe) | 102 | Krita_3/4_Default_Resources.bundle |
| `.png` (raster brush) | 52 | Krita_3/4_Default_Resources.bundle, RGBA_brushes.bundle |
| `.svg` (vector brush) | 3 | Krita_3_Default_Resources.bundle |

Total brush tip dari binary: **255 file**

## Bukti binary = source code Krita

1. **String path build di binary**: `/builds/graphics/krita/libs/brush/kis_brush.cpp`
   → ini path CI KDE Invent (invent.kde.org/graphics/krita) = sumber source code resmi Krita

2. **19 dari 22 class brush di binary MATCH dengan class di source code**
   (3 sisanya hanya beda regex parsing, bukan beda class)

3. **Binary me-link library yang persis sama dengan dependency source code**:
   libkritaimage.so, libkritapigment.so, libkritaresources.so, libkritaglobal.so

## Keterbatasan RE (jujur)

- Binary **stripped**: debug symbols hilang, nama variabel lokal tidak bisa dipulihkan
- **Decompiler C++** (Ghidra/IDA) bisa menghasilkan pseudocode tapi jauh lebih sulit dibaca
  daripada source code asli yang sudah ada komentar & struktur
- Source code asli (GPL) tetap sumber terbaik untuk **memahami algoritma**
- Binary unggul untuk: **ekstrak resource siap pakai** (preset .kpp + 255 brush tip)
  yang di-bundle untuk end-user, tidak semuanya ada di repo source
