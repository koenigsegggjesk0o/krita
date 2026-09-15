/*
 * SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * krita_brush_wrapper.h — Wrapper untuk Krita brush engine (TIDAK mengubah kode Krita)
 *
 * File ini adalah BRIDGE CODE yang memanggil Krita brush engine asli
 * tanpa mengubah satupun baris kode Krita.
 *
 * Krita brush engine (libkritalibbrush) di-compile terpisah dari
 * krita-source/libs/brush/ (100% asli KDE, tidak diubah).
 * File ini hanya memanggil API publik Krita.
 */

#ifndef KRITA_BRUSH_WRAPPER_H
#define KRITA_BRUSH_WRAPPER_H

#include <QString>
#include <QImage>
#include <QPointF>
#include <QSharedPointer>

// Forward declarations dari Krita (tidak include header Krita di sini
// untuk avoid dependency di header file)
class KisBrush;
class KisBrushRegistry;
class KisPaintOpPreset;
class KisPaintInformation;
class KisResourcesInterface;
class KoColor;
class KoColorSpace;

namespace FeatherKrita {

/**
 * BrushDab — hasil dari Krita brush engine
 * 
 * Ini adalah output 2D dari brush engine Krita.
 * Bridge code akan memetakan ini ke permukaan 3D.
 */
struct BrushDab {
    QImage image;           // 2D dab image dari Krita brush engine
    QPointF position;       // Posisi 2D di UV space
    double scaleFactor;     // Scale dab
    double rotation;        // Rotasi dab (radians)
};

/**
 * BrushInput — input untuk Krita brush engine
 * 
 * Dikirim dari 3D canvas ke Krita brush engine.
 * Berisi informasi pointer (posisi, pressure, tilt, dll)
 */
struct BrushInput {
    QPointF position;       // Posisi 2D di UV space
    double pressure;        // 0.0 - 1.0
    double xTilt;           // -60.0 to 60.0
    double yTilt;           // -60.0 to 60.0
    double rotation;        // Stylus rotation
    double tangentialPressure; // Wheel pressure
    double perspective;     // Perspective value
    double time;            // Time in seconds
    double speed;           // Movement speed
    bool isHovering;        // Is hover (no paint)
};

/**
 * KritaBrushWrapper — Wrapper class untuk Krita brush engine
 *
 * Class ini:
 * 1. Load brush preset (.kpp file) dari Krita
 * 2. Terima input dari 3D canvas
 * 3. Panggil Krita brush engine untuk generate dab
 * 4. Return dab ke bridge code untuk di-map ke 3D
 *
 * PENTING: Class ini TIDAK mengubah kode Krita.
 * Class ini hanya memanggil API publik Krita:
 *   - KisBrush::generateDab()
 *   - KisPaintOpPreset::loadFromDevice()
 *   - KisBrushRegistry::instance()
 */
class KritaBrushWrapper {
public:
    KritaBrushWrapper();
    ~KritaBrushWrapper();

    // === Brush Preset Management ===
    
    /**
     * Load brush preset dari file .kpp
     * File .kpp = Krita Paint Op Preset (PNG + gzip XML)
     * 
     * @param presetPath Path ke file .kpp
     * @return true jika berhasil load
     */
    bool loadPreset(const QString& presetPath);
    
    /**
     * Get daftar brush preset yang tersedia
     * Scan folder untuk file .kpp
     * 
     * @param folderPath Folder berisi .kpp files
     * @return List of preset names
     */
    QStringList listPresets(const QString& folderPath);
    
    /**
     * Get brush preset thumbnail
     * 
     * @return QImage thumbnail dari preset
     */
    QImage getPresetThumbnail() const;

    // === Brush Engine ===
    
    /**
     * Generate dab dari input
     * 
     * Ini MEMANGGIL Krita brush engine asli (KisBrush::generateDab)
     * untuk menghasilkan 2D dab image.
     * 
     * @param input BrushInput dari 3D canvas
     * @return BrushDab hasil dari Krita engine
     */
    BrushDab generateDab(const BrushInput& input);
    
    /**
     * Set brush size
     * 
     * @param size Brush diameter in pixels
     */
    void setBrushSize(int size);
    
    /**
     * Set brush color
     * 
     * @param color QColor (will be converted to KoColor)
     */
    void setBrushColor(const QColor& color);
    
    /**
     * Set brush opacity
     * 
     * @param opacity 0.0 - 1.0
     */
    void setBrushOpacity(double opacity);
    
    /**
     * Set brush spacing
     * 
     * @param spacing 0.0 - 1.0
     */
    void setBrushSpacing(double spacing);

    // === State ===
    
    /**
     * Check if brush engine is ready
     */
    bool isReady() const;
    
    /**
     * Get current brush preset name
     */
    QString currentPresetName() const;
    
    /**
     * Reset brush state (untuk stroke baru)
     */
    void resetStroke();

private:
    // Pointer ke object Krita (tidak di-include di header)
    void* m_brush;              // KisBrushSP
    void* m_preset;             // KisPaintOpPresetSP
    void* m_resourcesInterface; // KisResourcesInterfaceSP
    void* m_colorSpace;         // const KoColorSpace*
    void* m_color;              // KoColor
    
    QString m_presetName;
    bool m_ready;
    
    // Brush state
    int m_size;
    double m_opacity;
    double m_spacing;
    
    // Internal helpers
    void initializeKrita();
    void cleanupKrita();
};

} // namespace FeatherKrita

#endif // KRITA_BRUSH_WRAPPER_H
