/*
 * SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * krita_brush_wrapper.cpp — Implementasi wrapper untuk Krita brush engine
 *
 * PENTING: File ini TIDAK mengubah kode Krita.
 * File ini memanggil API publik Krita brush engine:
 *   - KisBrushRegistry::instance()
 *   - KisPaintOpPreset::loadFromDevice()
 *   - KisBrush::generateDab()
 *
 * Krita brush engine source code ada di krita-source/libs/brush/
 * dan TIDAK DIUBAH sama sekali.
 */

#include "krita_brush_wrapper.h"

// Include header Krita (dari krita-source/libs/brush/)
// Header ini 100% asli KDE, tidak diubah
#include <kis_brush.h>
#include <kis_brush_registry.h>
#include <kis_auto_brush.h>
#include <kis_gbr_brush.h>
#include <kis_abr_brush.h>
#include <kis_png_brush.h>
#include <kis_svg_brush.h>
#include <KisBrushServerProvider.h>
#include <KisBrushModel.h>

// Include header brush engine (dari krita-source/libs/image/brushengine/)
#include <brushengine/kis_paintop_preset.h>
#include <brushengine/kis_paintop.h>
#include <brushengine/kis_paint_information.h>

// Include header resource (dari krita-source/libs/resources/)
#include <KisResourcesInterface.h>
#include <KisGlobalResourcesInterface.h>

// Include header pigment (dari krita-source/libs/pigment/)
#include <KoColor.h>
#include <KoColorSpace.h>
#include <KoColorSpaceRegistry.h>

#include <QFile>
#include <QFileInfo>
#include <QDir>
#include <QImageReader>
#include <QDomDocument>
#include <QDebug>

namespace FeatherKrita {

KritaBrushWrapper::KritaBrushWrapper()
    : m_brush(nullptr)
    , m_preset(nullptr)
    , m_resourcesInterface(nullptr)
    , m_colorSpace(nullptr)
    , m_color(nullptr)
    , m_ready(false)
    , m_size(50)
    , m_opacity(1.0)
    , m_spacing(0.1)
{
    initializeKrita();
}

KritaBrushWrapper::~KritaBrushWrapper()
{
    cleanupKrita();
}

void KritaBrushWrapper::initializeKrita()
{
    // Initialize Krita brush registry
    // Ini memanggil KisBrushServerProvider::instance() dari Krita asli
    KisBrushServerProvider::instance();
    
    // Get color space dari Krita pigment (100% asli)
    m_colorSpace = KoColorSpaceRegistry::instance()->rgb8();
    
    // Create resources interface (dari Krita asli)
    m_resourcesInterface = new KisGlobalResourcesInterface();
    
    // Create default color (black)
    KoColor* color = new KoColor(QColor(0, 0, 0), static_cast<const KoColorSpace*>(m_colorSpace));
    m_color = color;
    
    m_ready = true;
    qDebug() << "KritaBrushWrapper: Initialized. Color space:" 
             << static_cast<const KoColorSpace*>(m_colorSpace)->name();
}

void KritaBrushWrapper::cleanupKrita()
{
    // Cleanup (Krita objects akan di-delete oleh QSharedPointer)
    if (m_resourcesInterface) {
        delete static_cast<KisGlobalResourcesInterface*>(m_resourcesInterface);
        m_resourcesInterface = nullptr;
    }
    if (m_color) {
        delete static_cast<KoColor*>(m_color);
        m_color = nullptr;
    }
    m_ready = false;
}

bool KritaBrushWrapper::loadPreset(const QString& presetPath)
{
    if (!m_ready) {
        qWarning() << "KritaBrushWrapper: Not ready";
        return false;
    }
    
    QFile file(presetPath);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "KritaBrushWrapper: Cannot open preset:" << presetPath;
        return false;
    }
    
    // Panggil KisPaintOpPreset::loadFromDevice() — API asli Krita
    // Format .kpp = PNG thumbnail + gzip XML settings
    KisPaintOpPreset* preset = new KisPaintOpPreset();
    KisResourcesInterfaceSP resourcesInterface = 
        static_cast<KisGlobalResourcesInterface*>(m_resourcesInterface);
    
    if (!preset->loadFromDevice(&file, resourcesInterface)) {
        qWarning() << "KritaBrushWrapper: Failed to load preset:" << presetPath;
        delete preset;
        return false;
    }
    
    m_preset = preset;
    m_presetName = QFileInfo(presetPath).baseName();
    
    qDebug() << "KritaBrushWrapper: Loaded preset:" << m_presetName;
    return true;
}

QStringList KritaBrushWrapper::listPresets(const QString& folderPath)
{
    QStringList presets;
    QDir dir(folderPath);
    
    QStringList filters;
    filters << "*.kpp";
    QFileInfoList files = dir.entryInfoList(filters, QDir::Files);
    
    for (const QFileInfo& file : files) {
        presets << file.baseName();
    }
    
    return presets;
}

QImage KritaBrushWrapper::getPresetThumbnail() const
{
    if (!m_preset) {
        return QImage();
    }
    
    // Panggil API asli Krita untuk get thumbnail
    KisPaintOpPreset* preset = static_cast<KisPaintOpPreset*>(m_preset);
    return preset->image();
}

BrushDab KritaBrushWrapper::generateDab(const BrushInput& input)
{
    BrushDab dab;
    
    if (!m_ready || !m_preset) {
        qWarning() << "KritaBrushWrapper: Not ready or no preset loaded";
        return dab;
    }
    
    // Buat KisPaintInformation dari input kita
    // Ini adalah data class dari Krita (tidak diubah)
    KisPaintInformation paintInfo(
        input.position,        // posisi 2D di UV space
        input.pressure,        // pressure 0-1
        input.xTilt,           // X tilt
        input.yTilt,           // Y tilt
        input.rotation,        // stylus rotation
        input.tangentialPressure, // wheel pressure
        input.perspective,     // perspective
        input.time,            // time
        input.speed,           // speed
        input.isHovering       // hovering mode
    );
    
    // Get brush dari preset
    KisPaintOpPreset* preset = static_cast<KisPaintOpPreset*>(m_preset);
    
    // Panggil KisBrush::generateDab() — API ASLI KRITA
    // Ini akan menghasilkan QImage dab menggunakan:
    //   - Brush mask (KisAutoBrush / KisGbrBrush / dll)
    //   - Brush settings (size, opacity, spacing, dll)
    //   - Paint information (pressure, tilt, dll)
    //
    // KODE INI TIDAK MENGUBAH APAPUN DI KRITA.
    // Kita hanya memanggil function yang sudah ada.
    
    KisBrushSP brush = preset->brush();
    if (!brush) {
        qWarning() << "KritaBrushWrapper: No brush in preset";
        return dab;
    }
    
    // Set brush size (via API asli Krita)
    brush->setUserEffectiveSize(static_cast<qreal>(m_size));
    
    // Generate dab menggunakan API asli Krita
    // KisBrush::generateDab() adalah virtual function yang
    // di-override oleh KisAutoBrush, KisGbrBrush, KisAbrBrush, dll
    KoColor* color = static_cast<KoColor*>(m_color);
    const KoColorSpace* cs = static_cast<const KoColorSpace*>(m_colorSpace);
    
    // KisDabShape: scale, rotation, dst (dari Krita asli)
    KisDabShape shape(m_scaleFactor, 1.0, input.rotation);
    
    // Panggil generateDab — INI FUNGSI ASLI KRITA, TIDAK DIUBAH
    QImage dabImage = brush->generateDab(
        shape,              // scale + rotation
        paintInfo,          // paint information (pressure, tilt, dll)
        0,                  // subPixelX
        0,                  // subPixelY
        cs,                 // color space
        *color              // brush color
    );
    
    // Return dab ke bridge code
    dab.image = dabImage;
    dab.position = input.position;
    dab.scaleFactor = m_scaleFactor;
    dab.rotation = input.rotation;
    
    return dab;
}

void KritaBrushWrapper::setBrushSize(int size)
{
    m_size = size;
    // Brush size akan di-apply saat generateDab dipanggil
}

void KritaBrushWrapper::setBrushColor(const QColor& color)
{
    if (!m_color || !m_colorSpace) return;
    
    KoColor* koColor = static_cast<KoColor*>(m_color);
    const KoColorSpace* cs = static_cast<const KoColorSpace*>(m_colorSpace);
    
    // Panggil API asli Krita untuk set color
    *koColor = KoColor(color, cs);
}

void KritaBrushWrapper::setBrushOpacity(double opacity)
{
    m_opacity = opacity;
    // Opacity di-apply via preset settings (tidak mengubah kode Krita)
}

void KritaBrushWrapper::setBrushSpacing(double spacing)
{
    m_spacing = spacing;
    // Spacing di-apply via preset settings
}

bool KritaBrushWrapper::isReady() const
{
    return m_ready && (m_preset != nullptr);
}

QString KritaBrushWrapper::currentPresetName() const
{
    return m_presetName;
}

void KritaBrushWrapper::resetStroke()
{
    // Reset brush state untuk stroke baru
    // Panggil API asli Krita jika diperlukan
    if (m_brush) {
        KisBrushSP brush = *static_cast<QSharedPointer<KisBrush>*>(m_brush);
        if (brush) {
            brush->notifyBrushIsGoingToBeClonedForStroke();
        }
    }
}

} // namespace FeatherKrita
