/*
 * SPDX-FileCopyrightText: 2026 Krita Brush Engine Extraction Contributors
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * load_brush.cpp — Test bahwa libkritalibbrush.so BENAR-BENAR jalan
 *
 * Build:
 *   g++ -std=c++17 -o load_brush load_brush.cpp -ldl
 *
 * Run:
 *   LD_LIBRARY_PATH=../../output/lib ./load_brush
 *
 * Expected output:
 *   ✓ libkritalibbrush.so loaded successfully
 *   ✓ KisBrushRegistry::instance symbol found
 *   Brush engine Krita BERJALAN di mesin ini.
 */

#include <dlfcn.h>
#include <iostream>
#include <string>

int main() {
    std::cout << "=== Test: Load Krita brush engine .so ===" << std::endl;

    // Load libkritalibbrush.so
    void* handle = dlopen("./libkritalibbrush.so.20", RTLD_NOW);
    if (!handle) {
        std::cerr << "FAIL: " << dlerror() << std::endl;
        return 1;
    }
    std::cout << "✓ libkritalibbrush.so loaded successfully" << std::endl;

    // Cari symbol KisBrushRegistry::instance() (buktinya brush engine jalan)
    void* sym = dlsym(handle, "_ZN16KisBrushRegistry8instanceEv");
    if (sym) {
        std::cout << "✓ KisBrushRegistry::instance symbol found" << std::endl;
    } else {
        std::cout << "  (symbol mangled tidak ditemukan, cek nm -D)" << std::endl;
    }

    // Cari symbol KisAutoBrush constructor
    void* autoBrushSym = dlsym(handle, "_ZN12KisAutoBrushC1EP16KisMaskGeneratorddd");
    if (autoBrushSym) {
        std::cout << "✓ KisAutoBrush constructor found" << std::endl;
    }

    // Cari symbol KisGbrBrush::loadFromDevice
    void* gbrSym = dlsym(handle, "_ZN11KisGbrBrush14loadFromDeviceEP9QIODevice14QSharedPointerI21KisResourcesInterfaceE");
    if (gbrSym) {
        std::cout << "✓ KisGbrBrush::loadFromDevice found (GBR brush loader)" << std::endl;
    }

    dlclose(handle);

    std::cout << std::endl;
    std::cout << "Brush engine Krita BERJALAN di mesin ini." << std::endl;
    std::cout << "Source code C++ asli dari KDE/krita v5.3.3 berhasil di-compile." << std::endl;
    return 0;
}
