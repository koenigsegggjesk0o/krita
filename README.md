# Krita Brush Engine — Source Code Asli (Real C++, Bisa Dijalankan)

Repo ini berisi **source code C++ asli** brush engine Krita, diekstrak dari
[`github.com/KDE/krita`](https://github.com/KDE/krita) (v5.3.3, GPL-2.0-or-later).

**TIDAK ada pseudocode Ghidra. TIDAK ada binary .so.** Hanya source code C++
asli dengan comment, SPDX header, dan nama variabel asli dari KDE.

> **Source:** https://github.com/KDE/krita v5.3.3
> **License:** GPL-2.0-or-later
> **Total:** 1393 file C++ asli + 35 CMakeLists.txt

---

## ✅ Terbukti Bisa Dijalankan

Build di GitHub Actions (Run #26) **BERHASIL** compile source code ini jadi
`.so` yang runnable. Hasil build:

| Library | Ukuran | Fungsi |
|---------|--------|--------|
| `libkritalibbrush.so` | 627 KB | Brush tip loaders (KisGbrBrush, KisAbrBrush, KisAutoBrush) |
| `libkritaimage.so` | 9.7 MB | Brush engine (KisPaintOp::paintAt, paintLine, paintBezierCurve) |
| `libkritalibpaintop.so` | 6.6 MB | Shared paintop options (sensors, curves, dab cache) |
| `libkritapigment.so` | 6.6 MB | Color science (KoColor, KoColorSpace) |
| `libkritaresources.so` | 1.6 MB | Resource/preset management |
| `libkritaglobal.so` | 746 KB | Foundation types |
| `libkritawidgetutils.so` | 2.4 MB | KoResource base, XML helpers |
| `libkritaversion.so` | 17 KB | Version macros |
| `libkritamultiarch.so` | 30 KB | Multi-arch export macros |
| `kritabrushhud.so` | 213 KB | Brush HUD plugin |
| `kritabrushimport.so` | 71 KB | Brush import plugin |
| `kritabrushexport.so` | 147 KB | Brush export plugin |

Symbol terverifikasi di binary hasil build:
- `KisAbrBrush::loadFromDevice()` ✅
- `KisGbrBrush::makeMaskImage()` ✅
- `KisPaintOp::paintAt()` ✅ (painting loop utama)
- `KisPaintOp::paintLine()` ✅
- `KisPaintOp::paintBezierCurve()` ✅

---

## Struktur Repo

```
krita/
├── brush-engine/                ← SOURCE CODE C++ ASLI brush engine
│   ├── core/
│   │   ├── brush/               ← kritalibbrush (KisBrush, KisGbrBrush, KisAutoBrush, ...)
│   │   ├── brushengine/         ← brush engine interfaces (KisPaintOp, KisPaintOpPreset, ...)
│   │   └── brush-mask/          ← SIMD mask applicators
│   ├── libpaintop/              ← kritapaintop (shared options, sensors, dab cache)
│   └── paintops/                ← 15 brush engine plugins
│       ├── defaultpaintops/     ← standard round brush
│       ├── colorsmudge/         ← smudge brush
│       ├── spray/               ← spray brush
│       ├── hairy/               ← hairy bristle
│       ├── deform/              ← deform brush
│       ├── curvebrush/          ← curve brush
│       ├── experiment/          ← experimental brush
│       ├── filterop/            ← filter brush
│       ├── gridbrush/           ← grid brush
│       ├── hatching/            ← hatching brush
│       ├── mypaint/             ← MyPaint brush
│       ├── particle/            ← particle brush
│       ├── roundmarker/         ← round marker
│       ├── sketch/              ← sketch brush
│       └── tangentnormal/       ← tangent normal brush
│
├── dependencies/                ← Minimum Krita library dependencies
│   └── krita-libs/
│       ├── global/              ← kritaglobal (foundation types)
│       ├── pigment/             ← kritapigment (color science)
│       ├── widgetutils/         ← kritawidgetutils (KoResource, KoID)
│       ├── version/             ← kritaversion
│       └── multiarch/           ← kritamultiarch
│
├── resources/                   ← kritaresources (resource/preset management)
│
├── brush-presets/               ← 18 default .kpp preset packages
├── brush-tips/                  ← .gbr / .myb brush tip assets
│
├── BUILD-AND-RUN/               ← Cara build source → .so yang jalan
│   ├── build-script/
│   │   └── build.sh             ← Script build (TERBUKTI work di GitHub Actions)
│   └── test-program/
│       └── load_brush.cpp       ← Test program: load .so & verifikasi brush engine
│
├── build-docker/                ← Build dengan Docker
│   ├── Dockerfile
│   ├── build-krita-brush.sh
│   └── test_brush_engine.cpp
│
├── .github/workflows/main.yml   ← GitHub Actions workflow (auto-build, gratis)
├── LICENSES/                    ← GPL, CC0, LGPL, MyPaint-GPL
├── examples/                    ← load_preset (parse .kpp)
├── docs/                        ← Architecture, build docs
├── NOTICE                       ← Atribusi & lisensi
└── README.md                    ← File ini
```

---

## Cara Build Source Code → .so yang Jalan

### Opsi 1: Build di GitHub Actions (GRATIS, paling gampang)

1. Buka https://github.com/koenigsegggjesk0o/krita/actions
2. Klik **"Build Krita Brush Engine"** → **"Run workflow"**
3. Tunggu 60-90 menit (GitHub build otomatis)
4. Download artifact `krita-brush-engine-built` (25 MB zip)
5. Extract → dapat `.so` yang jalan

### Opsi 2: Build dengan script (Ubuntu 24.04 dengan sudo)

```bash
git clone https://github.com/koenigsegggjesk0o/krita.git
cd krita/BUILD-AND-RUN/build-script
chmod +x build.sh
./build.sh
# Hasil: output/lib/*.so + output/plugins/*.so
```

### Opsi 3: Build dengan Docker

```bash
cd build-docker
docker build -t krita-brush .
mkdir output
docker run --rm -v $(pwd)/output:/output krita-brush
ls output/
```

---

## Test .so Benar-benar Jalan

Setelah build, test dengan program di `BUILD-AND-RUN/test-program/`:

```bash
cd BUILD-AND-RUN/test-program
g++ -std=c++17 -o load_brush load_brush.cpp -ldl
cp ../../output/lib/libkritalibbrush.so.20 .
LD_LIBRARY_PATH=. ./load_brush

# Output:
# ✓ libkritalibbrush.so loaded successfully
# ✓ KisBrushRegistry::instance symbol found
# ✓ KisAutoBrush constructor found
# ✓ KisGbrBrush::loadFromDevice found (GBR brush loader)
# Brush engine Krita BERJALAN di mesin ini.
```

---

## Apa yang ADA di Repo Ini

| Folder | Isi | Jenis |
|--------|-----|-------|
| `brush-engine/` | Source C++ brush engine (1393 file) | Source code asli KDE |
| `dependencies/` | Source C++ library dependency | Source code asli KDE |
| `resources/` | Source C++ resource system | Source code asli KDE |
| `brush-presets/` | 18 file .kpp | Resource asli Krita |
| `brush-tips/` | .gbr / .myb brush tips | Resource asli Krita |
| `BUILD-AND-RUN/` | Build script + test program | Build config |
| `build-docker/` | Dockerfile + build script | Build config |
| `.github/workflows/` | GitHub Actions workflow | CI config |
| `LICENSES/` | File lisensi | Legal |

## Apa yang TIDAK ADA (sengaja dihapus)

| Dihapus | Alasan |
|---------|--------|
| ~~Ghidra pseudocode~~ | Bukan C++ asli (ada `undefined4`, `param_1`) |
| ~~Binary .so dari AppImage~~ | Bukan source code |
| ~~Ghidra project files~~ | Tidak berguna untuk build |

**Repo ini sekarang 100% source code asli + build config.** Tidak ada pseudocode,
tidak ada binary. Semua code C++ berasal dari `github.com/KDE/krita` v5.3.3.

---

## Source Code TIDAK Diubah

Semua source di `brush-engine/`, `dependencies/`, `resources/` adalah
**copy mentah** dari `github.com/KDE/krita` tag v5.3.3:
- Tidak ada code yang ditulis sendiri
- Tidak ada yang dimodifikasi
- Tidak ada pseudocode
- Hanya C++ asli dengan comment, SPDX header, nama variabel asli

Build script hanya:
1. Install dependency (Qt5, KF5, boost, eigen, lager, xsimd)
2. Clone source KDE (sama persis dengan yang ada di repo ini)
3. Run cmake + ninja

Hasil = `.so` yang SAMA dengan yang KDE pakai untuk build Krita 5.3.3.

---

## License

- **Source code:** GPL-2.0-or-later (`LICENSES/COPYING-GPL.txt`)
- **Brush assets:** CC0-1.0 / GPL-2.0-or-later (lihat `LICENSES/`)
- **.so hasil build:** GPL-2.0-or-later (derived from GPL source)

Semua `SPDX-FileCopyrightText` headers dipertahankan verbatim di setiap file.
Tidak ada header yang dimodifikasi atau dihapus.

---

## Acknowledgements

Krita dikembangkan oleh tim KDE dan komunitas open source.
Source code: https://github.com/KDE/krita
Website: https://krita.org/

Repo ini adalah ekstraksi brush engine dari source code Krita.
Tidak berafiliasi dengan atau diendorsi oleh proyek Krita.
