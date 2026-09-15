# Krita Brush Engine — Source Code ASLI LENGKAP (Real C++, Reverse Engineered dari Krita v6.0.4)

Repo ini berisi **SEMUA source code C++ asli** yang berkaitan dengan brush, gambar,
dan engine dari [Krita](https://krita.org/) v6.0.4 — diambil langsung dari
[`github.com/KDE/krita`](https://github.com/KDE/krita) tag v6.0.4 (GPL-2.0-or-later).

**TIDAK ada pseudocode Ghidra.** **TIDAK ada binary .so.** **TIDAK ada code buatan
sendiri.** Hanya C++ asli dari KDE dengan comment, SPDX header, nama variabel asli.

> **Source:** https://github.com/KDE/krita tag v6.0.4
> **License:** GPL-2.0-or-later
> **Total:** 4586 file C/C++ asli + 48 cmake macro files
> **Ukuran:** 167 MB

---

## Apa yang Diambil (SEMUA yang berkaitan brush/gambar/engine)

### `krita-source/libs/` — Library Inti

| Folder | File C++ | Fungsi |
|--------|----------|--------|
| `brush/` | 63 | **kritalibbrush** — brush tip loaders (KisBrush, KisGbrBrush, KisAbrBrush, KisAutoBrush, KisPngBrush, KisSvgBrush, KisTextBrush, KisImagePipeBrush) |
| `image/` | 1038 | **kritaimage** — image engine + brushengine interfaces (KisPaintOp, KisPaintOpPreset, KisPaintOpRegistry, KisPaintInformation, KisPainter, KisPaintDevice) + brush mask applicators (SIMD) |
| `global/` | 142 | **kritaglobal** — foundation types (KisDebug, KisLager, KisAlgebra2D, KisSharedPtr) |
| `pigment/` | 220 | **kritapigment** — color science (KoColor, KoColorSpace, KoColorProfile, composite ops) |
| `widgetutils/` | 187 | **kritawidgetutils** — KoResource base, KoID, XML helpers |
| `resources/` | 136 | **kritaresources** — resource storage (folder/bundle/memory/SQL), resource models, tag system |
| `version/` | - | **kritaversion** — version macros |
| `multiarch/` | - | **kritamultiarch** — multi-arch export macros |
| `command/` | 27 | **kritacommand** — undo/redo (KisUndoCommand, dipakai brush stroke) |
| `koplugin/` | 12 | **kritakoplugin** — plugin framework (dipakai paintop plugins) |
| `flake/` | 513 | **kritaflake** — vector flake framework (dipakai drawing tools) |
| `basicflakes/` | 5 | **basicflakes** — basic drawing tools (KoPencilTool, KoCreatePathTool) |
| `widgets/` | 145 | **kritawidgets** — UI widgets (brush option widgets) |
| `ui/` | 1033 | **kritaui** — Krita UI (brush docker, option widget, palette) |

### `krita-source/plugins/` — Brush Engine & Drawing Tools

| Folder | Fungsi |
|--------|--------|
| `paintops/` | **15 brush engine plugins** (lihat detail di bawah) |
| `paintops/libpaintop/` | Shared paintop library (sensors, curves, dab cache, texture option) |
| `tools/basictools/` | Basic drawing tools (freehand brush, line, rectangle, ellipse, etc.) |
| `tools/tool_dyna/` | Dynamic brush tool |
| `tools/tool_lazybrush/` | Lazy brush tool (coloring assistant) |
| `tools/tool_knife/` | Knife tool |
| `tools/tool_polygon/` | Polygon tool |
| `tools/tool_polyline/` | Polyline tool |
| `tools/tool_enclose_and_fill/` | Enclose & fill tool |
| `tools/tool_smart_patch/` | Smart patch tool |
| `tools/tool_transform2/` | Transform tool |
| `tools/selectiontools/` | Selection tools |
| `tools/defaulttool/` | Default tool |
| `tools/tool_crop/` | Crop tool |
| `tools/svgtexttool/` | SVG text tool |
| `tools/karbonplugins/` | Karbon plugins |
| `dockers/brushhud/` | Brush HUD docker |
| `dockers/advancedcolorselector/` | Advanced color selector |
| `dockers/artisticcolorselector/` | Artistic color selector |
| `dockers/smallcolorselector/` | Small color selector |
| `dockers/specificcolorselector/` | Specific color selector |
| `dockers/widegamutcolorselector/` | Wide gamut color selector |

### 15 Brush Engine Plugins (`krita-source/plugins/paintops/`)

| Engine | Folder | Fungsi |
|--------|--------|--------|
| Default brush | `defaultpaintops/` | Standard round brush (KisBrushOp) |
| Color smudge | `colorsmudge/` | Smudge brush (smears color) |
| Curve brush | `curvebrush/` | Interpolated curve brush |
| Deform | `deform/` | Pixel deformation (grow/shrink/swirl) |
| Experiment | `experiment/` | Recursive stroke |
| Filter | `filterop/` | Filter along stroke |
| Grid | `gridbrush/` | Grid snapping brush |
| Hairy | `hairy/` | Multi-bristle hairy brush |
| Hatching | `hatching/` | Hatching/cross-hatching |
| MyPaint | `mypaint/` | libmypaint wrapper |
| Particle | `particle/` | Particle simulation brush |
| Round marker | `roundmarker/` | Hard round marker |
| Sketch | `sketch/` | Sketchy vibrating lines |
| Spray | `spray/` | Scatter spray brush |
| Tangent normal | `tangentnormal/` | Tangent-space normal map painting |

### Folder Pendukung

| Folder | Isi |
|--------|-----|
| `krita-source/LICENSES/` | 21 file lisensi SPDX (GPL-2.0, LGPL, CC0, BSL, dll) |
| `krita-source/cmake/` | 48 cmake macro files (build system Krita) |
| `krita-source/3rdparty/` | Vendored 3rdparty deps |
| `krita-source/CMakeLists.txt` | Top-level CMake config |
| `krita-source/COPYING` | GPL license text |
| `krita-source/KRITA-README.md` | README asli Krita |

---

## Source Code TIDAK Diubah

**SEMUA code di `krita-source/` adalah copy mentah dari `github.com/KDE/krita` tag v6.0.4.**

- ❌ Tidak ada code yang ditulis sendiri
- ❌ Tidak ada yang dimodifikasi
- ❌ Tidak ada pseudocode Ghidra
- ❌ Tidak ada binary .so
- ✅ Hanya C++ asli dengan comment, SPDX header, nama variabel asli

Anda bisa verifikasi dengan:
```bash
cd krita-source
git clone --depth 1 --branch v6.0.4 https://github.com/KDE/krita.git /tmp/krita-upstream
diff -rq libs/brush /tmp/krita-upstream/libs/brush  # harusnya identical
```

---

## Cara Build Source Code → .so yang Jalan

### Opsi 1: Build di GitHub Actions (GRATIS, paling gampang)

1. Buka https://github.com/koenigsegggjesk0o/krita/actions
2. Klik **"Build Krita Brush Engine"** → **"Run workflow"**
3. Tunggu 60-90 menit (GitHub build otomatis)
4. Download artifact `krita-brush-engine-built` (zip)
5. Extract → dapat `.so` yang jalan

### Opsi 2: Build dengan Docker

```bash
cd build-docker
docker build -t krita-brush .
mkdir output
docker run --rm -v $(pwd)/output:/output krita-brush
ls output/  # → lihat .so yang jalan
```

### Opsi 3: Build langsung di Ubuntu 24.04

```bash
cd build-docker
chmod +x build-krita-brush.sh
./build-krita-brush.sh  # butuh sudo, ~60-90 menit
ls output/  # → lihat .so yang jalan
```

---

## Hasil Build yang Terbukti Jalan

Build di GitHub Actions (Run #26 dengan Krita v5.3.3) BERHASIL menghasilkan:

| Library | Ukuran | Status |
|---------|--------|--------|
| `libkritalibbrush.so.20.0.0` | 627 KB | ✅ Brush tip loaders |
| `libkritaimage.so.20.0.0` | 9.7 MB | ✅ Brush engine (KisPaintOp::paintAt, paintLine, paintBezierCurve) |
| `libkritalibpaintop.so.20.0.0` | 6.6 MB | ✅ Shared paintop options |
| `libkritapigment.so.20.0.0` | 6.6 MB | ✅ Color science |
| `libkritaresources.so.20.0.0` | 1.6 MB | ✅ Resource management |
| `libkritaglobal.so.20.0.0` | 746 KB | ✅ Foundation |
| `libkritawidgetutils.so.20.0.0` | 2.4 MB | ✅ Widget utils |
| `libkritaversion.so.20.0.0` | 17 KB | ✅ Version |
| `libkritamultiarch.so.20.0.0` | 30 KB | ✅ Multi-arch |
| `kritabrushhud.so` | 213 KB | ✅ Brush HUD plugin |
| `kritabrushimport.so` | 71 KB | ✅ Brush import plugin |
| `kritabrushexport.so` | 147 KB | ✅ Brush export plugin |

Symbol terverifikasi:
- `KisAbrBrush::loadFromDevice()` ✅
- `KisGbrBrush::makeMaskImage()` ✅
- `KisPaintOp::paintAt()` ✅ (painting loop utama)
- `KisPaintOp::paintLine()` ✅
- `KisPaintOp::paintBezierCurve()` ✅

---

## License

- **Source code:** GPL-2.0-or-later (`krita-source/LICENSES/GPL-2.0-or-later.txt`)
- **Brush assets:** CC0-1.0 / GPL-2.0-or-later
- **.so hasil build:** GPL-2.0-or-later (derived from GPL source)

Semua `SPDX-FileCopyrightText` headers dipertahankan verbatim di setiap file.

---

## Acknowledgements

Krita dikembangkan oleh tim KDE dan komunitas open source.
- Source code: https://github.com/KDE/krita
- Website: https://krita.org/

Repo ini adalah ekstraksi SEMUA bagian brush/gambar/engine dari source code Krita v6.0.4.
Tidak berafiliasi dengan atau diendorsi oleh proyek Krita.
