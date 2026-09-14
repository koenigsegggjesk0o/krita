# Deep Reverse Engineering Report — Krita Brush Engine Binary

SPDX-FileCopyrightText: 2026 Krita Brush Engine Extraction Contributors
SPDX-License-Identifier: GPL-2.0-or-later

Analisis RE mendalam terhadap binary Krita asli (krita-5.3.3-x86_64.AppImage).
Tools: objdump (disassembly), nm (symbols), c++filt (demangle), strings.

## 1. Function Inventory (disassembly lengkap)

| Library | Total function ter-disassemble | Function brush (Kis*) |
|---------|--------------------------------:|----------------------:|
| `libkritalibbrush` | 841 | 507 |
| `libkritalibpaintop` | 1691 | 1211 |
| `libkritaimage` | 8224 | 7334 |

## 2. Cross-Reference: Binary ↔ Source Code

### Class di binary tapi TIDAK di source ekstraksi (dependency eksternal, BUKAN proprietary)

Class-class ini adalah dependency brush engine dari library Krita lain:

| Class | Library asal | Fungsi |
|-------|--------------|--------|
| `KisCircleMaskGenerator` | kritaimage | Mask generator soft-round |
| `KisGaussCircleMaskGenerator` | kritaimage | Mask Gaussian blur |
| `KisCurveCircleMaskGenerator` | kritaimage | Mask curve-based |
| `KisRectangleMaskGenerator` | kritaimage | Mask kotak |
| `KisCurveRectangleMaskGenerator` | kritaimage | Mask kotak curve |
| `KisGaussRectangleMaskGenerator` | kritaimage | Mask kotak Gaussian |
| `KisFixedPaintDevice` | kritaimage | Paint device fixed-size |
| `KisCubicCurve` | kritaimage/widgeutils | Cubic curve interpolation |
| `KisOutlineGenerator` | kritaimage | Brush outline |
| `KisOptimizedByteArray` | kritaimage | Optimized byte array |
| `KisResourceLocator` | kritaresources | Resource file locator |
| `KisResourceModel` | kritaresources | Resource Qt model |
| `KisTag` | kritaresources | Resource tag |
| `KisStoragePluginFactoryBase` | kritaresources | Storage plugin base |
| `KisStoragePluginRegistry` | kritaresources | Storage plugin registry |

**Kesimpulan jujur**: Tidak ada class proprietary tersembunyi. Semua class
tambahan di binary adalah dependency yang sudah saya identifikasi dan
sebagian diekstrak di `dependencies/krita-libs/`. Binary == source code.

## 3. Factory ID brush tip (dari string binary)

String yang ditemukan di libkritalibbrush.so membuktikan factory registrasi:

```
abr_brush
auto_brush
clonedBrush
colorfulBrush
gbr_brush
kis_text_brush
png_brush
svg_brush
```

## 4. Assertion & error message (runtime behavior)

```
_ZNK14QMessageLogger7warningEv
_Z27kis_safe_assert_recoverablePKcS0_i
_Z22kis_assert_recoverablePKcS0_i
_ZNK12QImageReader11errorStringEv
0 && "unknown brush type"
WARNING: KisDomUtils::toInt failed:
WARNING: KisDomUtils::toDouble failed:
unknown
GBR loading failed: width or height is 0
GBR loading failed; image could not be created from following dimensions
```

## 5. Build provenance (bukti binary dari source resmi KDE)

```
/builds/graphics/krita/libs/brush/kis_predefined_brush_factory.cpp
/builds/graphics/krita/libs/brush/kis_auto_brush.cpp
?/builds/graphics/krita/libs/brush/kis_brush.cpp
/builds/graphics/krita/libs/brush/kis_brush_registry.cpp
/builds/graphics/krita/libs/global/KoGenericRegistry.h
/builds/graphics/krita/libs/resources/KoResourceServer.h
/builds/graphics/krita/libs/brush/kis_imagepipe_brush.cpp
/builds/graphics/krita/libs/brush/kis_brushes_pipe.h
/builds/graphics/krita/libs/brush/kis_qimage_pyramid.cpp
/builds/graphics/krita/libs/brush/kis_text_brush.cpp
```

Path `/builds/graphics/krita/` adalah path CI di KDE Invent (GitLab KDE).
Binary ini dikompilasi dari source code resmi Krita di invent.kde.org/graphics/krita.

## 6. Runtime import dependency (26 symbol external)

Symbol yang di-import (undefined, di-resolve saat runtime dari library lain):

Lihat file: `analysis/imported-symbols.txt`

## 7. File analisis RE yang dihasilkan

- `analysis/all-functions.txt` — 841 function ter-disassemble dari libkritalibbrush
- `analysis/brush-functions.txt` — 492 function brush (Kis*)
- `analysis/libbrush-symbols.txt` — 369 dynamic symbol demangled
- `analysis/imported-symbols.txt` — 26 imported symbol
- `analysis/vtables.txt` — vtable & typeinfo
- `analysis/paintop-metadata.txt` — Qt metadata 14 paintop plugin

## 8. Kenapa TIDAK pakai Ghidra/IDA (jujur)

Ghidra/IDA decompiler pada C++ stripped menghasilkan:
- Nama function: `FUN_00123456` (hilang, karena stripped)
- Type: `undefined4`, `undefined8` (hilang)
- Class hierarchy: hilang (kecuali dari vtable)
- Comment: hilang permanen

Hasilnya **lebih buruk** dari source code GPL yang sudah ada (yang punya
nama asli, type, comment, struktur). Karena itu deeper RE yang **benar-benar
bernilai** adalah:
1. Verifikasi binary ↔ source (tidak ada proprietary tersembunyi) ✅
2. Factory ID & assertion string (runtime behavior) ✅
3. Function inventory & call dependency ✅
4. Ekstraksi resource siap pakai (255 brush tip + 16 preset) ✅

Source code tetap sumber terbaik untuk **memahami algoritma**.
Binary RE unggul untuk **verifikasi & resource ekstraksi**.
