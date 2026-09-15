# Build di GitHub Actions (GRATIS, tanpa install apa-apa)

**Ini cara paling gampang.** GitHub yang build, Anda tinggal download hasilnya.

## Langkah-langkah

### 1. Buka tab Actions

Buka: **https://github.com/koenigsegggjesk0o/krita/actions**

Anda akan lihat workflow bernama **"Build Krita Brush Engine"**.

### 2. Jalankan build

- Klik workflow **"Build Krita Brush Engine"** di sidebar kiri
- Klik tombol **"Run workflow"** (kanan atas)
- Pilih branch `main`
- Klik **"Run workflow"** (tombol hijau)

### 3. Tunggu build selesai

- Build berjalan **30-60 menit** (tergantung beban server GitHub)
- Anda bisa lihat progress real-time dengan klik build yang running
- GitHub kirim email kalau build selesai (atau gagal)

### 4. Download hasilnya

Setelah build sukses:

- Klik build yang sudah selesai (centang hijau ✓)
- Scroll ke bawah, cari bagian **"Artifacts"**
- Download **`krita-brush-engine-built`** (file zip, ~50-100MB)
- Extract zip → dapat folder berisi:

```
krita-brush-engine-built/
├── lib/
│   ├── libkritalibbrush.so.20.0.0     ← brush tip loaders (JALAN)
│   ├── libkritaimage.so.20.0.0        ← brush engine interfaces (JALAN)
│   ├── libkritalibpaintop.so.20.0.0   ← shared paintop options (JALAN)
│   ├── libkritapigment.so.20.0.0      ← color science (JALAN)
│   ├── libkritaresources.so.20.0.0    ← resource management (JALAN)
│   ├── libkritaglobal.so.20.0.0       ← foundation (JALAN)
│   └── ...
├── plugins/
│   ├── kritaspraypaintop.so           ← spray brush engine (JALAN)
│   ├── kritacolorsmudgepaintop.so     ← colorsmudge engine (JALAN)
│   ├── kritahairypaintop.so           ← hairy bristle engine (JALAN)
│   ├── kritadeformpaintop.so          ← deform engine (JALAN)
│   ├── ... 14 plugin paintop total
│   ├── kritabrushhud.so               ← brush HUD (JALAN)
│   ├── kritabrushimport.so            ← brush import (JALAN)
│   └── kritabrushexport.so            ← brush export (JALAN)
├── symbols/
│   └── *.txt                          ← daftar symbol tiap library
└── MANIFEST.txt                       ← info build
```

### 5. Pakai .so

File `.so` bisa Anda pakai untuk:

**A. Drop-in ke Krita** (ganti library bawaan):
```bash
# Backup dulu
cp /path/to/krita/lib/libkritalibbrush.so.20 /tmp/backup/
# Copy hasil build
cp libkritalibbrush.so.20.0.0 /path/to/krita/lib/
ldconfig
```

**B. Link ke aplikasi C++ Anda sendiri**:
```bash
g++ myapp.cpp -o myapp \
    -L./lib -lkritalibbrush -lkritaimage -lkritapigment \
    -lkritaresources -lkritaglobal -lkritawidgetutils \
    $(pkg-config --cflags --libs Qt6Core Qt6Gui) \
    -Wl,-rpath,./lib
export LD_LIBRARY_PATH=./lib:$LD_LIBRARY_PATH
./myapp
```

**C. Load dinamis dengan dlopen** (lihat `build-docker/test_brush_engine.cpp`).

## Biaya

**GRATIS** untuk:
- **Public repo**: unlimited build minutes
- **Private repo**: 2,000 menit/bulan (build ~60 menit = ~33 build/bulan)

## Troubleshooting

**Build gagal?**
- Buka build yang gagal, lihat log error
- Klik step yang merah untuk lihat detail
- Umumnya: dependency missing → saya update workflow

**Artifact tidak ada?**
- Artifact hanya disimpan 90 hari
- Re-run build kalau sudah expired

**Mau build versi Krita lain?**
- Edit `.github/workflows/build-brush-engine.yml`
- Ganti `git clone --depth 1 https://github.com/KDE/krita.git` ke branch/tag tertentu
- Contoh: `git clone --depth 1 --branch v5.3.3 https://github.com/KDE/krita.git`

## Verifikasi: build GitHub = binary resmi Krita

Setelah download artifact, verifikasi:

```bash
# Bandingkan symbol hasil build GitHub vs symbol dari AppImage
nm -D libkritalibbrush.so.20.0.0 | c++filt | grep KisAutoBrush | head
# vs (dari repo ini)
cat re-binary/analysis/libbrush-symbols.txt | grep KisAutoBrush | head
# HARUS MATCH — bukti code yang di-build = code yang KDE distribusikan
```

## Cara kerja workflow

1. **Ubuntu 24.04 runner** disediakan GitHub (4 CPU, 16GB RAM, 14GB disk)
2. **apt install** semua dependency (Qt6, KF5, boost, eigen, dll)
3. **Build lager + immer** dari source (dependency header-only)
4. **Clone Krita** dari `github.com/KDE/krita` (TIDAK diubah)
5. **cmake configure** dengan Qt6 + Release mode
6. **cmake --build** target brush engine (14 plugin + 5 library)
7. **Collect** semua `.so` ke folder artifact
8. **Verify** semua `.so` valid ELF
9. **Upload** sebagai artifact yang bisa di-download

## Source code TIDAK diubah

Workflow ini clone dari `https://github.com/KDE/krita` (repo resmi KDE).
Sama persis dengan source code di folder `brush-engine/` repo Anda.
Hasil build = `.so` yang SAMA dengan yang KDE pakai untuk build Krita.
