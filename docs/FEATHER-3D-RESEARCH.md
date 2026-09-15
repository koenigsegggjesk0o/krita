# Feather 3D — Riset Mendalam Komprehensif

SPDX-FileCopyrightText: 2026 Research Document
SPDX-License-Identifier: GPL-2.0-or-later

## 1. OVERVIEW

| Aspek | Detail |
|-------|--------|
| **Nama App** | Feather: Draw in 3D |
| **Developer** | Sketchsoft Inc |
| **Platform** | iPad only (iOS) |
| **Harga** | $14.99 (sekali bayar, tanpa subscription) |
| **Ukuran App** | 51.1 MB |
| **Rating** | 4.1/5 (109 ratings), Editors' Choice |
| **Engine** | "Airbreath rendering engine" (custom, dibuat khusus untuk Feather) |
| **Website** | https://www.feather.art |
| **Dokumentasi** | https://support.feather.art |

---

## 2. KONSEP INTI — "3D Drawing" Bukan "3D Sculpting"

### Bedanya Feather 3D dengan ZBrush/Nomad Sculpt:

| Aspek | Feather 3D | ZBrush/Nomad (Sculpting tradisional) |
|-------|-----------|--------------------------------------|
| **Metode** | Gambar stroke di ruang 3D | Deform mesh (push/pull vertices) |
| **Hasil stroke** | 3D entity (punya posisi XYZ, tebal, warna) | Deformasi permukaan mesh |
| **Canvas** | Ruang 3D kosong / guide surface | Mesh 3D (sphere, cube, dll) |
| **Editing** | Move/rotate/scale stroke pakai joystick | Brush mengubah geometri mesh |
| **Liquify** | Push/pull stroke di 3D space | Push/pull vertices mesh |

### Cara Kerja "2D Brush jadi 3D":

1. User buka canvas 3D (ruang kosong)
2. User buat **3D Guide Surface** (bola, silinder, kerucut, cincin)
3. User gambar di permukaan guide tersebut
4. Stroke yang dibuat **bukan pixel 2D**, tapi **3D entity**:
   - Posisi XYZ di ruang 3D
   - Tebal (thickness)
   - Warna
   - Bentuk (bulat, pipih, dll)
5. Stroke bisa di-edit:
   - Move (geser di 3D)
   - Rotate (putar di 3D)
   - Scale (besar/kecil)
   - Liquify (ubah bentuk dengan push/pull)
6. Hasil akhir = kumpulan 3D strokes yang bisa dilihat dari semua sudut

### Two-Stage Workflow:
1. **Stage 1**: Buat 3D guide (sphere/cylinder/cone/ring atau custom curve)
2. **Stage 2**: Gambar di atas guide tersebut

---

## 3. UI / LAYOUT INTERFACE

### Struktur UI Feather 3D:

```
┌─────────────────────────────────────────────┐
│              3D Viewport (Canvas)            │
│                                              │
│    [3D Guide Surface]                        │
│    [3D Strokes]                              │
│    [Joystick Control]                        │
│                                              │
├──────────────────────────────────────────────┤
│           Bottom Context Menu                │
│  (Berubah sesuai tool yang dipilih)          │
├──────────────────────────────────────────────┤
│        Bottom Toolbar (Icons)                │
│  [Draw] [Erase] [Shapes] [Liquify] [Select]  │
│  [Light] [Export] [Resource] [Settings]      │
└─────────────────────────────────────────────┘
```

### Tab/Menu Utama:

| Tab | Fungsi |
|-----|--------|
| **Draw** | Gambar 3D guide atau curve. Bisa freehand, straight line, atau shape |
| **Erase** | Hapus dengan brush. Punya mode "Vacuum" (hapus area kosong) |
| **Shapes** | Gambar garis lurus, lingkaran, ellipse, kurva smooth |
| **Liquify** | Push/pull stroke di 3D space |
| **Select** | Pilih/deselect stroke untuk edit |
| **Light** | Setup lighting dan shadow (single tap) |
| **Export** | Export ke PNG, GIF, OBJ, GLTF, MP4 |
| **Resource** | Import 3D model (OBJ) atau .feather file |
| **Settings** | Konfigurasi app |

### Navigation:
- **Pinch**: Zoom in/out
- **Two-finger rotate**: Rotate 3D view
- **Two-finger pan**: Pan camera
- **Joystick**: Move/rotate/scale selected stroke

### Input:
- **Apple Pencil**: Primary drawing input (pressure-sensitive)
- **Touch**: Navigation (pinch, rotate, pan)
- **Finger-pen mode**: Draw dengan jari (tanpa stylus)

---

## 4. TOOLS & FEATURES LENGKAP

### Drawing Tools:

| Tool | Detail |
|------|--------|
| **3D Brushes** | Pressure-sensitive, dengan styles dan patterns. Brush mengikuti permukaan 3D guide |
| **Eraser** | Hapus stroke. Mode "Vacuum" untuk hapus area kosong |
| **Shapes** | Straight lines, circles, ellipses, smooth curves |
| **3D Guides** | Draw on spheres, cylinders, cones, rings. Custom curve guides juga tersedia |
| **Live Mirror** | Symmetrical drawing across multiple axes (termasuk Z-axis). Virtual mirror |
| **Geometric Primitives** | Sphere, cylinder, cone, ring sebagai canvas |

### Editing Tools:

| Tool | Detail |
|------|--------|
| **3D Liquify** | Push/pull stroke di 3D space. Ubah bentuk/proportion dengan drag |
| **Virtual Joystick** | Move, rotate, scale stroke. "Editing feels like gaming" |
| **Select/Deselect** | Pilih individual stroke untuk edit |
| **Clipboard** | Reference image di separate window |

### Visual Effects:

| Effect | Detail |
|--------|--------|
| **Lighting** | Real-time light dan shadow. Single tap setup |
| **Post-processing** | Grain, depth of field, glow, pixelation, toon shading |
| **Background** | Warna atau image di belakang 3D artwork |

### Import/Export:

| Format | Tipe |
|--------|------|
| **PNG** | Image export |
| **JPEG** | Image export |
| **GIF** | Animation export |
| **MP4** | Video export |
| **OBJ** | 3D model export |
| **GLTF** | 3D model export |
| **.feather** | Native file format (save/load project) |
| **Import OBJ** | Import 3D model dari luar |
| **Import Image** | Reference image |

### Other Features:

| Feature | Detail |
|---------|--------|
| **Fully Offline** | Tidak butuh internet |
| **AR View** | Lihat 3D creation di real-world (augmented reality) |
| **Folders** | Organisasi drawing ke folder |
| **Data Backup** | Backup file |
| **Share** | 3D viewer link untuk share artwork |
| **Gallery** | View dan share 3D artworks |
| **Finger-pen Mode** | Gambar tanpa stylus |

---

## 5. AIRBREATH RENDERING ENGINE

Engine custom yang dibuat khusus untuk Feather 3D:

| Aspek | Detail |
|-------|--------|
| **Real-time lighting** | Hitung dan tampilkan light + shadow real-time |
| **Optimized for iPad** | Smooth performance di Apple Silicon |
| **Pressure-sensitive** | Support Apple Pencil pressure |
| **Post-processing** | Grain, DOF, glow, pixelation, toon shading |
| **Custom stroke rendering** | 3D strokes dengan thickness dan color |

---

## 6. LAYERING SYSTEM

### Pendekatan Feather 3D (Bukan traditional layers):

Feather 3D **TIDAK** menggunakan sistem layer tradisional seperti Photoshop/Krita (tumpukan canvas 2D).

**Sebagai gantinya:**
- Setiap stroke adalah **individual 3D entity** di ruang 3D
- Tidak ada "layer panel" — strokes exist in 3D space
- User bisa **select individual stroke** untuk edit (move/rotate/scale/liquify)
- Strokes bisa di-group (mungkin pakai folder system)
- Urutan stroke menentukan depth (z-order di 3D space)

### Perbandingan Layer System:

| Aspek | Krita/Photoshop | Feather 3D |
|-------|-----------------|------------|
| **Layer** | Tumpukan canvas 2D | Individual 3D stroke |
| **Z-order** | Layer index (top to bottom) | Z-position di 3D space |
| **Editing** | Edit seluruh layer | Edit individual stroke |
| **Group** | Group layers | Folder system |
| **Visibility** | Show/hide layer | Select/deselect stroke |

---

## 7. WORKFLOW PENGGUNA

### Step-by-step workflow Feather 3D:

1. **Buka App** → New canvas (3D space kosong)
2. **Pilih Guide Surface** (opsional):
   - Sphere (bola)
   - Cylinder (silinder)
   - Cone (kerucut)
   - Ring (cincin)
   - Custom curve (gambar kurva sendiri)
3. **Gambar di Guide Surface**:
   - Pilih brush (dengan style dan pattern)
   - Gambar di permukaan guide
   - Stroke otomatis mengikuti kelengkungan 3D
4. **Navigate 3D Space**:
   - Pinch zoom
   - Two-finger rotate
   - Joystick untuk move/rotate/scale
5. **Edit Strokes**:
   - Select stroke
   - Move/rotate/scale dengan joystick
   - Liquify untuk push/pull
6. **Apply Effects**:
   - Lighting (single tap)
   - Post-processing (grain, DOF, glow, dll)
   - Background image/color
7. **Export**:
   - PNG/JPEG (image)
   - GIF/MP4 (animation)
   - OBJ/GLTF (3D model)
   - Share 3D viewer link

---

## 8. BUSINESS MODEL

| Aspek | Detail |
|-------|--------|
| **Harga** | $14.99 (sekali bayar) |
| **Subscription** | Tidak ada |
| **In-app purchase** | Tidak disebutkan |
| **Free trial** | Tidak disebutkan |
| **Offline** | Fully offline |

---

## 9. INTEGRASI DENGAN APP KITA (Krita Brush + Feather 3D Style)

### Arsitektur:

```
┌─────────────────────────────────────────────┐
│              Godot Engine (C++)              │
│  ┌─────────────────────────────────────────┐ │
│  │         3D Viewport (Canvas)            │ │
│  │  [Guide Surface: Sphere/Cylinder/Cone]  │ │
│  │  [3D Strokes]                           │ │
│  │  [Joystick Control]                     │ │
│  └─────────────────────────────────────────┘ │
│  ┌─────────────────────────────────────────┐ │
│  │         UI Layer (Godot Control)        │ │
│  │  [Draw] [Erase] [Shapes] [Liquify]      │ │
│  │  [Light] [Export] [Settings]            │ │
│  └─────────────────────────────────────────┘ │
│  ┌─────────────────────────────────────────┐ │
│  │    Bridge Code (C++ GDExtension)        │ │
│  │  - Input → Krita brush engine           │ │
│  │  - Krita dab → 3D texture mapping       │ │
│  │  - UV unwrap + texture paint            │ │
│  └─────────────────────────────────────────┘ │
│  ┌─────────────────────────────────────────┐ │
│  │   Krita Brush Engine (100% ASLI)        │ │
│  │  libkritalibbrush.so / .dll             │ │
│  │  (TIDAK DIUBAH, dari krita-source/)     │ │
│  └─────────────────────────────────────────┘ │
└─────────────────────────────────────────────┘
```

### Cara Kerja Brush di App Kita:

1. User pilih brush di UI
2. User sentuh 3D guide surface (sphere/cylinder/cone)
3. Bridge code dapat:
   - 3D position (raycast dari touch ke mesh)
   - UV coordinate dari raycast hit
   - Pressure (dari stylus/touch)
4. Bridge code kirim ke Krita brush engine:
   - Position (X,Y di UV space)
   - Pressure
   - Tilt
   - Time
5. Krita brush engine generate **dab** (QImage 2D) — **100% kode Krita asli, tidak diubah**
6. Bridge code terima dab QImage
7. Bridge code apply dab ke texture mesh di UV position
8. Godot update texture → render 3D mesh dengan brush stroke

### Yang TIDAK DIUBAH (dari Krita):
- `kis_brush.cpp` — base class brush
- `kis_auto_brush.cpp` — procedural brush
- `kis_gbr_brush.cpp` — GBR brush loader
- `kis_abr_brush.cpp` — ABR brush loader
- `kis_brush_registry.cpp` — brush factory
- `KisBrushModel.cpp` — brush state model
- Semua file di `krita-source/libs/brush/`
- Semua 15 paintop plugins di `krita-source/plugins/paintops/`

### Yang DIBUAT BARU (bridge code):
- `krita_bridge.cpp` — C++ GDExtension untuk Godot
- `krita_brush_wrapper.h` — Wrapper class untuk call Krita API
- `texture_painter.cpp` — UV mapping + texture paint
- `guide_surface.cpp` — 3D guide surface (sphere/cylinder/cone)
- `stroke_manager.cpp` — Manage 3D strokes
- `ui_layout.tscn` — Godot scene untuk UI Feather-style
- `joystick_control.cpp` — Virtual joystick untuk 3D manipulation

---

## 10. KOMPETITOR & MARKET ANALYSIS

| App | Platform | Harga | Mirip Feather? |
|-----|----------|-------|----------------|
| **Feather 3D** | iPad | $14.99 | (Ini referensi) |
| **Nomad Sculpt** | iPad + Android | $14.99 | Sculpting (beda konsep) |
| **Forger** | iPad | $4.99 | Sculpting (beda konsep) |
| **Sculptura** | iPad | $7.99 | Sculpting (beda konsep) |
| **Putty 3D** | iPad | $3.99 | Sculpting (beda konsep) |

**Keunggulan app kita:**
- ✅ Platform: **Windows + Android** (Feather cuma iPad)
- ✅ Brush engine: **Krita asli** (15 brush engine, ratusan preset)
- ✅ Harga: **Freemium** (gratis pakai, bayar untuk export)
- ✅ Konsep: **3D drawing** (sama seperti Feather, beda dari sculpting)
