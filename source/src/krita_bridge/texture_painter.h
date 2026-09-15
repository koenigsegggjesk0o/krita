/*
 * SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * texture_painter.h — Map 2D brush dab ke permukaan mesh 3D
 *
 * Ini adalah BRIDGE CODE yang menghubungkan:
 * - Output 2D dari Krita brush engine (BrushDab)
 * - Ke texture mesh 3D di Godot
 *
 * Cara kerja:
 * 1. User sentuh permukaan 3D mesh
 * 2. Raycast → dapat UV coordinate
 * 3. Krita brush engine generate 2D dab
 * 4. Texture painter apply dab ke texture di UV position
 * 5. Godot render mesh dengan texture updated
 */

#ifndef TEXTURE_PAINTER_H
#define TEXTURE_PAINTER_H

#include <QImage>
#include <QPointF>
#include <QPainter>
#include "krita_brush_wrapper.h"

namespace FeatherKrita {

/**
 * TexturePainter — Apply 2D dab ke texture mesh 3D
 *
 * Class ini manage texture image yang di-paint oleh Krita brush.
 * Texture ini kemudian di-apply ke mesh 3D di Godot.
 */
class TexturePainter {
public:
    TexturePainter(int textureWidth = 2048, int textureHeight = 2048);
    ~TexturePainter();
    
    /**
     * Initialize texture dengan background color
     */
    void initialize(const QColor& backgroundColor = Qt::transparent);
    
    /**
     * Apply dab ke texture
     * 
     * @param dab BrushDab dari Krita brush engine
     * @param uvPosition UV coordinate (0.0 - 1.0)
     */
    void applyDab(const BrushDab& dab, const QPointF& uvPosition);
    
    /**
     * Apply dab dengan composite mode
     * 
     * @param dab BrushDab dari Krita
     * @param uvPosition UV coordinate
     * @param compositeMode QPainter::CompositionMode
     */
    void applyDabWithComposite(const BrushDab& dab, const QPointF& uvPosition, 
                                QPainter::CompositionMode compositeMode);
    
    /**
     * Get current texture image
     */
    QImage getTexture() const;
    
    /**
     * Get texture as RGBA data (untuk Godot)
     */
    const unsigned char* getTextureData() const;
    
    /**
     * Get texture size
     */
    int width() const { return m_textureWidth; }
    int height() const { return m_textureHeight; }
    
    /**
     * Clear texture (reset to background)
     */
    void clear();
    
    /**
     * Convert UV (0-1) to pixel position
     */
    QPointF uvToPixel(const QPointF& uv) const;
    
    /**
     * Convert pixel to UV (0-1)
     */
    QPointF pixelToUV(const QPointF& pixel) const;
    
    /**
     * Set eraser mode
     */
    void setEraserMode(bool eraser);
    bool isEraserMode() const { return m_eraserMode; }
    
private:
    QImage m_texture;           // Main painting texture
    QImage m_textureBackup;     // Backup untuk undo
    int m_textureWidth;
    int m_textureHeight;
    bool m_eraserMode;
    
    /**
     * Blend dab image ke texture di posisi tertentu
     */
    void blendDab(const QImage& dabImage, const QPointF& pixelPos, 
                  QPainter::CompositionMode mode);
    
    /**
     * Handle texture wrapping (untuk UV yang melintasi boundary)
     */
    void blendDabWrapped(const QImage& dabImage, const QPointF& pixelPos,
                         QPainter::CompositionMode mode);
};

} // namespace FeatherKrita

#endif // TEXTURE_PAINTER_H
