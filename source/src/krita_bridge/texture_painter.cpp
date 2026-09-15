/*
 * SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * texture_painter.cpp — Implementasi texture painter
 */

#include "texture_painter.h"
#include <QPainter>
#include <QDebug>
#include <cmath>

namespace FeatherKrita {

TexturePainter::TexturePainter(int textureWidth, int textureHeight)
    : m_textureWidth(textureWidth)
    , m_textureHeight(textureHeight)
    , m_eraserMode(false)
{
    m_texture = QImage(m_textureWidth, m_textureHeight, QImage::Format_ARGB32_Premultiplied);
    initialize();
}

TexturePainter::~TexturePainter()
{
}

void TexturePainter::initialize(const QColor& backgroundColor)
{
    m_texture.fill(backgroundColor);
    m_textureBackup = m_texture.copy();
}

void TexturePainter::applyDab(const BrushDab& dab, const QPointF& uvPosition)
{
    if (dab.image.isNull()) {
        return;
    }
    
    // Convert UV ke pixel position
    QPointF pixelPos = uvToPixel(uvPosition);
    
    // Center dab di pixel position
    QPointF centeredPos(
        pixelPos.x() - dab.image.width() / 2.0,
        pixelPos.y() - dab.image.height() / 2.0
    );
    
    // Pilih composite mode berdasarkan eraser mode
    QPainter::CompositionMode mode = m_eraserMode 
        ? QPainter::CompositionMode_DestinationOut 
        : QPainter::CompositionMode_SourceOver;
    
    blendDabWrapped(dab.image, centeredPos, mode);
}

void TexturePainter::applyDabWithComposite(const BrushDab& dab, const QPointF& uvPosition,
                                            QPainter::CompositionMode compositeMode)
{
    if (dab.image.isNull()) {
        return;
    }
    
    QPointF pixelPos = uvToPixel(uvPosition);
    QPointF centeredPos(
        pixelPos.x() - dab.image.width() / 2.0,
        pixelPos.y() - dab.image.height() / 2.0
    );
    
    blendDabWrapped(dab.image, centeredPos, compositeMode);
}

void TexturePainter::blendDab(const QImage& dabImage, const QPointF& pixelPos,
                               QPainter::CompositionMode mode)
{
    QPainter painter(&m_texture);
    painter.setCompositionMode(mode);
    painter.drawImage(pixelPos, dabImage);
    painter.end();
}

void TexturePainter::blendDabWrapped(const QImage& dabImage, const QPointF& pixelPos,
                                      QPainter::CompositionMode mode)
{
    // Handle wrapping di tepi texture (UV boundary)
    int dabW = dabImage.width();
    int dabH = dabImage.height();
    int texW = m_textureWidth;
    int texH = m_textureHeight;
    
    int x = static_cast<int>(std::round(pixelPos.x()));
    int y = static_cast<int>(std::round(pixelPos.y()));
    
    // Paint dab normal
    blendDab(dabImage, pixelPos, mode);
    
    // Wrap horizontal (kalau dab melintasi tepi kiri/kanan)
    if (x < 0) {
        blendDab(dabImage, QPointF(x + texW, y), mode);
    } else if (x + dabW > texW) {
        blendDab(dabImage, QPointF(x - texW, y), mode);
    }
    
    // Wrap vertical (kalau dab melintasi tepi atas/bawah)
    if (y < 0) {
        blendDab(dabImage, QPointF(x, y + texH), mode);
    } else if (y + dabH > texH) {
        blendDab(dabImage, QPointF(x, y - texH), mode);
    }
    
    // Corner wrap
    if (x < 0 && y < 0) {
        blendDab(dabImage, QPointF(x + texW, y + texH), mode);
    } else if (x + dabW > texW && y + dabH > texH) {
        blendDab(dabImage, QPointF(x - texW, y - texH), mode);
    } else if (x < 0 && y + dabH > texH) {
        blendDab(dabImage, QPointF(x + texW, y - texH), mode);
    } else if (x + dabW > texW && y < 0) {
        blendDab(dabImage, QPointF(x - texW, y + texH), mode);
    }
}

QImage TexturePainter::getTexture() const
{
    return m_texture;
}

const unsigned char* TexturePainter::getTextureData() const
{
    return m_texture.constBits();
}

void TexturePainter::clear()
{
    m_texture.fill(Qt::transparent);
    m_textureBackup = m_texture.copy();
}

QPointF TexturePainter::uvToPixel(const QPointF& uv) const
{
    return QPointF(
        uv.x() * m_textureWidth,
        uv.y() * m_textureHeight
    );
}

QPointF TexturePainter::pixelToUV(const QPointF& pixel) const
{
    return QPointF(
        pixel.x() / m_textureWidth,
        pixel.y() / m_textureHeight
    );
}

void TexturePainter::setEraserMode(bool eraser)
{
    m_eraserMode = eraser;
}

} // namespace FeatherKrita
