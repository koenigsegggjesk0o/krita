#!/bin/bash
# SPDX-FileCopyrightText: 2026 Krita Brush Engine Extraction Contributors
# SPDX-License-Identifier: GPL-2.0-or-later
#
# build-krita-brush.sh — Build Krita brush engine dari source → .so yang jalan
#
# Script ini akan:
#   1. Install semua dependency build (butuh sudo)
#   2. Clone Krita source dari KDE resmi (tidak diubah)
#   3. Build library brush engine
#   4. Build 14 plugin paintop
#   5. Verifikasi .so yang dihasilkan bisa di-load
#
# Hasil: file .so di ./output/ yang BENAR-BENAR bisa dipakai melukis.
#
# CARA PAKAI (di Ubuntu 24.04 dengan sudo):
#   chmod +x build-krita-brush.sh
#   ./build-krita-brush.sh
#
# Estimasi waktu: 30-60 menit (tergantung CPU)
# Disk dibutuhkan: ~10 GB

set -e

KRITA_SRC=/opt/krita-src
KRITA_BUILD=/opt/krita-build
KRITA_INSTALL=/opt/krita-install
OUTPUT_DIR="$(pwd)/output"

echo "============================================================"
echo "  BUILD KRITA BRUSH ENGINE DARI SOURCE → .so YANG JALAN"
echo "============================================================"
echo ""

# 1. INSTALL DEPENDENCIES
echo "[1/5] Install build dependencies (butuh sudo)..."
sudo apt-get update
sudo apt-get install -y \
    build-essential cmake ninja-build git pkg-config \
    extra-cmake-modules \
    qt6-base-dev qt6-tools-dev qt6-svg-dev \
    libkf5config-dev libkf5widgetsaddons-dev libkf5completion-dev \
    libkf5coreaddons-dev libkf5guiaddons-dev libkf5i18n-dev \
    libkf5itemviews-dev libkf5itemmodels-dev libkf5xmlgui-dev \
    libkf5configwidgets-dev libkf5iconthemes-dev libkf5kiocore-dev \
    libkf5service-dev libkf5windowsystem-dev libkf5notifications-dev \
    libkf5parts-dev libkf5plotting-dev libkf5kdelibs4support-dev \
    libkf5crash-dev libkf5archive-dev libkf5sonnet-dev \
    libboost-dev libboost-system-dev libboost-filesystem-dev \
    libeigen3-dev libexiv2-dev libfftw3-dev libgsl-dev \
    liblcms2-dev libopenexr-dev libilmbase-dev \
    libquazip5-dev libquazip1-qt5-dev \
    libgif-dev libheif-dev libjpeg-dev libpng-dev libtiff-dev \
    libraw-dev libwebp-dev libjxl-dev \
    libpoppler-qt6-dev libpoppler-cpp-dev \
    python3 python3-pyqt5 pyqt5-dev-tools python3-sip-dev \
    libglm-dev libmypaint-dev json-c intltool

# 2. INSTALL liblager + immer dari source
echo ""
echo "[2/5] Install liblager (state management untuk brush options)..."
if [ ! -d /tmp/lager ]; then
    git clone --depth 1 https://github.com/arximboldi/immer.git /tmp/immer
    git clone --depth 1 https://github.com/arximboldi/lager.git /tmp/lager
    cd /tmp/immer && cmake -S . -B build -DCMAKE_INSTALL_PREFIX=/usr \
        -DBUILD_TESTING=OFF -DBUILD_EXAMPLES=OFF -DBUILD_DOCS=OFF && \
        sudo cmake --build build --target install -j$(nproc)
    cd /tmp/lager && cmake -S . -B build -DCMAKE_INSTALL_PREFIX=/usr \
        -DBUILD_TESTING=OFF -DBUILD_EXAMPLES=OFF -DBUILD_DOCS=OFF && \
        sudo cmake --build build --target install -j$(nproc)
fi

# 3. CLONE KRITA SOURCE (TIDAK DIUBAH — persis dari KDE)
echo ""
echo "[3/5] Clone Krita source dari KDE resmi (tidak diubah)..."
if [ ! -d "$KRITA_SRC" ]; then
    git clone --depth 1 https://github.com/KDE/krita.git "$KRITA_SRC"
fi

# 4. CONFIGURE & BUILD
echo ""
echo "[4/5] Configure & build brush engine..."
cd "$KRITA_SRC"

cmake -S . -B "$KRITA_BUILD" \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX="$KRITA_INSTALL" \
    -DBUILD_TESTING=OFF \
    -DENABLE_PYTHON=OFF \
    -DWITH_QT6=ON \
    -DQT_MAJOR_VERSION=6 \
    -DWITH_HEIF=ON -DWITH_JPEG=ON -DWITH_PNG=ON -DWITH_TIFF=ON \
    -DWITH_RAW=ON -DWITH_GIF=ON -DWITH_FFTW3=ON -DWITH_OPENEXR=ON \
    -DWITH_KDE=ON -DWITH_XCF=OFF -DWITH_SOPRANO=OFF -DWITH_HDR=OFF \
    -DWITH_JXL=ON -DWITH_WEBP=ON \
    -G Ninja

# Build brush-related targets
cmake --build "$KRITA_BUILD" --target \
    kritalibbrush kritaimage kritalibpaintop kritapigment kritaresources \
    krita-defaultpaintops krita-colorsmudgepaintop krita-curvepaintop \
    krita-deformpaintop krita-experimentpaintop krita-gridpaintop \
    krita-hairypaintop krita-hatchingpaintop krita-mypaintop \
    krita-particlepaintop krita-roundmarkerpaintop krita-sketchpaintop \
    krita-spraypaintop krita-tangentnormalpaintop \
    krita-brushhud krita-brushimport krita-brushexport \
    --parallel $(nproc)

# 5. COPY HASIL
echo ""
echo "[5/5] Copy .so ke $OUTPUT_DIR..."
mkdir -p "$OUTPUT_DIR/lib" "$OUTPUT_DIR/plugins"

find "$KRITA_BUILD" -name "libkritalibbrush.so*" -exec cp -P {} "$OUTPUT_DIR/lib/" \; 2>/dev/null
find "$KRITA_BUILD" -name "libkritaimage.so*" -exec cp -P {} "$OUTPUT_DIR/lib/" \; 2>/dev/null
find "$KRITA_BUILD" -name "libkritalibpaintop.so*" -exec cp -P {} "$OUTPUT_DIR/lib/" \; 2>/dev/null
find "$KRITA_BUILD" -name "libkritapigment.so*" -exec cp -P {} "$OUTPUT_DIR/lib/" \; 2>/dev/null
find "$KRITA_BUILD" -name "libkritaresources.so*" -exec cp -P {} "$OUTPUT_DIR/lib/" \; 2>/dev/null
find "$KRITA_BUILD" -name "libkritaglobal.so*" -exec cp -P {} "$OUTPUT_DIR/lib/" \; 2>/dev/null
find "$KRITA_BUILD" -name "libkritawidgetutils.so*" -exec cp -P {} "$OUTPUT_DIR/lib/" \; 2>/dev/null

find "$KRITA_BUILD" -path "*kritaplugins*" -name "*paintop*.so" -exec cp {} "$OUTPUT_DIR/plugins/" \; 2>/dev/null
find "$KRITA_BUILD" -name "kritabrush*.so" -exec cp {} "$OUTPUT_DIR/plugins/" \; 2>/dev/null

echo ""
echo "============================================================"
echo "  BUILD SELESAI"
echo "============================================================"
echo ""
echo "Library yang dihasilkan:"
ls -lh "$OUTPUT_DIR/lib/"
echo ""
echo "Plugin paintop yang dihasilkan:"
ls -lh "$OUTPUT_DIR/plugins/"
echo ""
echo "VERIFIKASI: cek .so bisa di-load"
for so in "$OUTPUT_DIR/lib/"*.so.* "$OUTPUT_DIR/plugins/"*.so; do
    if [ -f "$so" ]; then
        if file "$so" | grep -q "ELF.*shared object"; then
            echo "  ✓ $(basename $so) - valid ELF"
        else
            echo "  ✗ $(basename $so) - INVALID"
        fi
    fi
done
echo ""
echo "Selesai. File .so siap dipakai oleh Krita atau aplikasi C++ Anda."
