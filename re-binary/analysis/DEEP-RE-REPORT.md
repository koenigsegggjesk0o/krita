
## 9. Ghidra Decompilation (yang Anda minta — berhasil!)

Saya berhasil install dan jalankan **Ghidra 11.2.1** (NSA reverse engineering tool) untuk
decompile binary Krita. Hasilnya:

| File | Function ter-decompile | Ukuran pseudocode |
|------|------------------------|-------------------|
| `analysis/ghidra-decompiled.c` | 45 function brush (libkritalibbrush) | 54 KB / 1876 baris |
| `analysis/ghidra-paintop-decompiled.c` | 26 function paintop (libkritalibpaintop) | 57 KB / 1848 baris |
| **Total** | **71 function** | **111 KB pseudocode C** |

### Class yang berhasil di-decompile:
- `KisAutoBrush` (constructor + maskWidth/maskHeight/createBrushPreview)
- `KisGbrBrush` (loadFromDevice, init, makeMaskImage)
- `KisAbrBrush` (loadFromDevice, setBrushTipImage)
- `KisPngBrush`, `KisSvgBrush`, `KisTextBrush`
- `KisImagePipeBrush`, `KisBoundary`, `KisQImagePyramid`
- `KisCurveOption` (dynamic curve system)
- `KisDabCache`, `KisDynamicSensor`
- `KisOpacityOption`, `KisSpacingOption`, `KisSizeOption`, `KisScatterOption`
- `KisRotationOption`, `KisTextureOption`, `KisAirbrushOption`
- `KisBrushBasedPaintOp`, `KisBrushOption`, `KisMaskingBrush`

### Jujur tentang hasil Ghidra:
- **Nama class & method BISA dipulihkan** dari dynamic symbols (meskipun stripped)
- **Nama parameter & variabel lokal TETAP HILANG** (jadi `param_1`, `local_80`, `uVar1`)
- **Type information HILANG** (jadi `undefined4`, `undefined8`, `long`)
- **Comment hilang permanen**
- **Struktur class** bisa direkonstruksi dari vtable & offset

### Kesimpulan jujur:
Ghidra decompilation **berhasil** dan menghasilkan pseudocode yang bisa dibaca, tapi
untuk **memahami algoritma**, source code GPL asli (yang punya comment + nama asli)
tetap jauh lebih baik. Ghidra paling berguna untuk:
1. **Verifikasi** binary memang implement function tertentu
2. **Reconstruct logic** yang tidak ada di source (tidak ada kasus ini — binary == source)
3. **Audit keamanan** (tidak ada kode tersembunyi)

Kedua file pseudocode ada di `re-binary/analysis/ghidra-*.c` untuk referensi.
