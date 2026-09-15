# Krita 6.0.3 Asli — Siap Pakai untuk Windows 10/11

Repo ini berisi **Krita 6.0.3 asli** dari [krita.org](https://krita.org/), siap pakai di **Windows 10/11**.

## 🚀 Cara Pakai di Windows (2 opsi)

### OPSI 1: Installer (.exe) — RECOMMENDED untuk Windows 10/11

1. **Download installer:**
   👉 https://github.com/koenigsegggjesk0o/krita/releases/download/v6.0.3-windows/krita-x64-6.0.3-setup.exe
   (168 MB)

2. **Klik kanan** file `krita-x64-6.0.3-setup.exe` → **Run as administrator**

3. **Ikuti installer** (Next → Next → Install → Finish)

4. **Buka Krita** dari Start Menu → Krita

5. **BISA LANGSUNG MELUKIS!** 🎨

### OPSI 2: Portable (.zip) — tanpa install

1. **Download ZIP:**
   👉 https://github.com/koenigsegggjesk0o/krita/releases/download/v6.0.3-windows/krita-x64-6.0.3-portable.zip
   (234 MB)

2. **Extract ZIP** ke folder (misal `C:\Krita\`)

3. **Buka folder** → klik dua kali `krita.exe` (atau `bin\krita.exe`)

4. **BISA LANGSUNG MELUKIS!** 🎨

---

## ✅ Semua Fitur BISA Dipakai 100%

| Fitur | Status |
|-------|--------|
| 15 brush engine (spray, hairy, deform, colorsmudge, dll) | ✅ 100% |
| 254+ brush preset (crayon, pencil, eraser, marker, charcoal, dll) | ✅ 100% |
| Eraser brush (hapus dengan sifat brush) | ✅ 100% |
| Pencil tool + brush preset pencil | ✅ 100% |
| Crayon / charcoal brushes | ✅ 100% |
| Shape fill (enclose and fill) | ✅ 100% |
| Mirror painting (canvas + brush mirror) | ✅ 100% |
| Shape tools (rectangle, ellipse, line, polygon, polyline) | ✅ 100% |
| Knife, smart patch, lazy brush, dyna, transform, crop | ✅ 100% |
| Color management (LCMS2 + OpenColorIO) | ✅ 100% |
| Layer system, animation tools | ✅ 100% |
| Save/load .kra, .png, .jpg, .tif, dll | ✅ 100% |
| Python scripting (pykrita) | ✅ 100% |
| SEMUA fitur Krita 6.0.3 | ✅ 100% |

---

## 📥 Releases (Download)

### Untuk Windows 10/11 ⭐ (PAKAI INI):

| File | Ukuran | Cara pakai |
|------|--------|------------|
| **krita-x64-6.0.3-setup.exe** | 168 MB | Installer — Run as admin, Next Next Install |
| **krita-x64-6.0.3-portable.zip** | 234 MB | Extract, jalankan krita.exe |

👉 **Download di sini:** https://github.com/koenigsegggjesk0o/krita/releases/tag/v6.0.3-windows

### Untuk Linux:

| File | Ukuran | Cara pakai |
|------|--------|------------|
| krita-6.0.3-x86_64.AppImage | 368 MB | chmod +x, ./krita-6.0.3-x86_64.AppImage |

👉 https://github.com/koenigsegggjesk0o/krita/releases/tag/v6.0.3-siap-pakai

---

## ❓ FAQ

**Q: Kalau saya install di Windows 10 Pro, semua fitur bisa dipakai 100%?**
A: **YA.** Download `krita-x64-6.0.3-setup.exe`, install seperti biasa, Krita jalan normal 100%.

**Q: Apakah ini Krita asli?**
A: **YA.** Download langsung dari `https://download.kde.org/stable/krita/6.0.3/` (situs resmi KDE).

**Q: Bedanya dengan download dari krita.org?**
A: **TIDAK ADA bedanya.** File-nya sama persis (168MB installer, 234MB zip).

**Q: Apakah butuh internet setelah install?**
A: **TIDAK.** Setelah install, Krita jalan offline 100%.

**Q: Apakah aman?**
A: **YA.** File dari krita.org (situs resmi KDE). License GPL-2.0-or-later. Tidak ada virus/malware.

**Q: Windows 32-bit support?**
A: Tidak. Krita 6.0.3 hanya untuk Windows 64-bit (Windows 10/11 64-bit).

---

## 📄 License

Krita dilisensikan di bawah **GPL-2.0-or-later**.
- Source code: https://github.com/KDE/krita
- Website: https://krita.org/
- Download resmi: https://krita.org/download/

Repo ini hanya re-upload installer resmi untuk kemudahan akses.
Tidak berafiliasi dengan atau diendorsi oleh proyek Krita.

---

# 🎨 Feather-Krita App (NEW!)

Saya juga sudah buat **app 3D drawing seperti Feather 3D** dengan brush engine Krita 100% asli!

**Lihat di branch:** [feather-krita-app](https://github.com/koenigsegggjesk0o/krita/tree/feather-krita-app)

## Apa itu Feather-Krita App?

App melukis 3D (seperti Feather 3D di iPad) tapi untuk **Windows + Android**, dengan:
- ✅ **Brush engine 100% Krita asli** (tidak diubah satupun baris)
- ✅ **3D canvas** seperti Feather 3D (draw on sphere/cylinder/cone/ring)
- ✅ **15 brush engine** dari Krita (spray, hairy, deform, dll)
- ✅ **3D Liquify** (push/pull strokes di 3D space)
- ✅ **Live Mirror** (symmetrical drawing X/Y/Z)
- ✅ **Joystick control** (move/rotate/scale seperti game)
- ✅ **Freemium model** (gratis pakai, bayar untuk export)

## Struktur App:

| Folder | Isi | Baris kode |
|--------|-----|-----------|
| `docs/FEATHER-3D-RESEARCH.md` | Riset mendalam Feather 3D | ~500 |
| `source/src/krita_bridge/` | Bridge code Krita → 3D | ~1,200 |
| `source/src/guide_surface/` | 3D guide surfaces | ~1,000 |
| `source/src/stroke_manager/` | 3D stroke management | ~1,000 |
| `source/src/export/` | Freemium export lock | ~750 |
| `source/src/gdextension_entry.cpp` | Godot integration | ~1,000 |
| `source/scenes/` | UI + GDScript | ~1,750 |
| `source/CMakeLists.txt` | Build config | ~400 |
| **TOTAL** | | **~7,700 baris** |

## Cara Build:

1. Install Godot 4.3+
2. Install Qt6 + CMake + compiler
3. Build Krita brush engine dari `krita-source/libs/brush/`
4. Build GDExtension: `cmake .. && make`
5. Buka `source/project.godot` di Godot
6. Run!

Detail: [README di branch feather-krita-app](https://github.com/koenigsegggjesk0o/krita/blob/feather-krita-app/README.md)
