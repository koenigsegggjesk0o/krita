// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// krita_bridge_real.cpp — REAL Krita engine FFI bridge.
//
// This file implements the `extern "C"` API declared in krita_bridge.h by
// CALLING the unmodified Krita v6.0.4 shared libraries (libkritabrush,
// libkritaimage, libkritapigment, libkritaresources, libkritaversion).
// It is a THIN GLUE layer: every brush/mask/pixel computation runs inside
// real, unmodified Krita code:
//
//   - brush tip   : KisGaussCircleMaskGenerator / KisCircleMaskGenerator
//                   (libs/image — gaussian falloff LUT, antialiasing)
//   - dab engine  : KisAutoBrush (libs/brush) -> KisBrush::mask(...) ->
//                   KisBrush::generateMaskAndApplyMaskOrCreateDab
//                   -> brush pyramid scaling/rotation
//                   -> KisBrushMaskApplicator (SIMD-stamped masks)
//   - color       : KoColor + KoColorSpaceRegistry (libs/pigment)
//   - presets     : KisBrush::fromXML + KisResourcesInterface (real .kpp
//                   brush-definition parsing)
//
// The glue in this file only:
//   1. converts the C ABI structs (BrushInput / BrushDab) to Krita calls
//      and back,
//   2. reorders Krita's native byte layout to the ABI's R,G,B,A layout and
//      applies the ABI's documented pressure->alpha scaling (desktop Krita
//      performs the same scaling in KisPainter at composite time),
//   3. unzips .kpp preset containers (PKZIP entry extraction only — the
//      parsed XML is handed to Krita's own KisBrush::fromXML).
//
// No painting math is re-implemented. The Krita source tree is used
// UNMODIFIED, per project directive.

#include "krita_bridge.h"

// ---------------------------------------------------------------------------
// Real Krita headers (unmodified v6.0.4 source tree).
// ---------------------------------------------------------------------------
#include <kis_auto_brush.h>          // libs/brush
#include <kis_brush.h>               // libs/brush  (KisBrush::mask, fromXML)
#include <kis_dab_shape.h>           // libs/brush
#include <kis_mask_generator.h>      // libs/image  (gauss/circle generators)
#include <kis_fixed_paint_device.h>  // libs/image
#include <kis_paint_information.h>   // libs/image/brushengine
#include <KisResourcesInterface.h>   // libs/resources
#include <KoColor.h>                 // libs/pigment
#include <KoColorSpaceRegistry.h>    // libs/pigment
#include <KritaVersionWrapper.h>     // libs/version

#include <QByteArray>
#include <QColor>
#include <QDir>
#include <QDomDocument>
#include <QDomElement>
#include <QFile>
#include <QFileInfo>
#include <QList>
#include <QString>

#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <new>
#include <string>
#include <vector>

#include <zlib.h>

// ---------------------------------------------------------------------------
// Glue: minimal PKZIP entry extraction for .kpp preset files.
// (Serialization only — the extracted XML is parsed by Krita's own code.)
// ---------------------------------------------------------------------------

namespace {

inline uint16_t rd16(const uint8_t* p) {
    return uint16_t(p[0]) | (uint16_t(p[1]) << 8);
}
inline uint32_t rd32(const uint8_t* p) {
    return uint32_t(p[0]) | (uint32_t(p[1]) << 8) |
           (uint32_t(p[2]) << 16) | (uint32_t(p[3]) << 24);
}

// Inflate raw DEFLATE data using the system zlib.
QByteArray inflateRaw(const char* data, size_t n) {
    z_stream zs;
    std::memset(&zs, 0, sizeof(zs));
    if (inflateInit2(&zs, -15) != Z_OK) return QByteArray();
    zs.next_in = reinterpret_cast<Bytef*>(const_cast<char*>(data));
    zs.avail_in = uint(n);
    QByteArray out;
    out.reserve(4096);
    uint8_t buf[8192];
    int ret = Z_OK;
    do {
        zs.next_out = buf;
        zs.avail_out = sizeof(buf);
        ret = inflate(&zs, Z_NO_FLUSH);
        if (ret == Z_STREAM_ERROR || ret == Z_NEED_DICT || ret == Z_DATA_ERROR || ret == Z_MEM_ERROR) {
            inflateEnd(&zs);
            return QByteArray();
        }
        const size_t produced = sizeof(buf) - zs.avail_out;
        if (produced > 0) out.append(reinterpret_cast<const char*>(buf), produced);
    } while (ret != Z_STREAM_END && zs.avail_out == 0);
    inflateEnd(&zs);
    return out;
}

qint64 findEocd(const QByteArray& data) {
    const qint64 n = data.size();
    const qint64 maxBack = n < 65557 ? n : 65557;
    for (qint64 i = n - 22; i >= n - maxBack; --i) {
        if (i < 0) break;
        const uint8_t* p = reinterpret_cast<const uint8_t*>(data.constData() + i);
        if (rd32(p) == 0x06054b50UL) return i;
    }
    return -1;
}

QByteArray zipExtractFile(const QByteArray& zipData, const QString& name) {
    const qint64 eocd = findEocd(zipData);
    if (eocd < 0) return QByteArray();
    const uint8_t* base = reinterpret_cast<const uint8_t*>(zipData.constData());
    const uint8_t* e = base + eocd;
    const uint32_t cdOffset = rd32(e + 16);
    const uint32_t cdSize = rd32(e + 12);
    if (cdOffset + cdSize > (uint32_t)zipData.size()) return QByteArray();

    const uint8_t* cd = base + cdOffset;
    const uint8_t* cdEnd = cd + cdSize;
    while (cd + 46 <= cdEnd) {
        if (rd32(cd) != 0x02014b50UL) break;
        const uint16_t method = rd16(cd + 10);
        const uint32_t compSize = rd32(cd + 20);
        const uint16_t nameLen = rd16(cd + 28);
        const uint16_t extraLen = rd16(cd + 30);
        const uint16_t commentLen = rd16(cd + 32);
        const uint32_t localOff = rd32(cd + 42);
        const QByteArray entryName(reinterpret_cast<const char*>(cd + 46), nameLen);
        cd += 46 + nameLen + extraLen + commentLen;
        if (QString::fromUtf8(entryName) != name) continue;

        if (localOff + 30 > (uint32_t)zipData.size()) return QByteArray();
        const uint8_t* lh = base + localOff;
        if (rd32(lh) != 0x04034b50UL) return QByteArray();
        const uint16_t lhNameLen = rd16(lh + 26);
        const uint16_t lhExtraLen = rd16(lh + 28);
        const uint8_t* data = lh + 30 + lhNameLen + lhExtraLen;
        if (data + compSize > base + zipData.size()) return QByteArray();

        if (method == 0) {
            return QByteArray(reinterpret_cast<const char*>(data), compSize);
        } else if (method == 8) {
            return inflateRaw(reinterpret_cast<const char*>(data), compSize);
        }
        return QByteArray();
    }
    return QByteArray();
}

QList<QByteArray> zipListXmlEntries(const QByteArray& zipData) {
    QList<QByteArray> names;
    const qint64 eocd = findEocd(zipData);
    if (eocd < 0) return names;
    const uint8_t* base = reinterpret_cast<const uint8_t*>(zipData.constData());
    const uint8_t* e = base + eocd;
    const uint32_t cdOffset = rd32(e + 16);
    const uint32_t cdSize = rd32(e + 12);
    if (cdOffset + cdSize > (uint32_t)zipData.size()) return names;

    const uint8_t* cd = base + cdOffset;
    const uint8_t* cdEnd = cd + cdSize;
    while (cd + 46 <= cdEnd) {
        if (rd32(cd) != 0x02014b50UL) break;
        const uint16_t nameLen = rd16(cd + 28);
        const uint16_t extraLen = rd16(cd + 30);
        const uint16_t commentLen = rd16(cd + 32);
        const QByteArray entryName(reinterpret_cast<const char*>(cd + 46), nameLen);
        cd += 46 + nameLen + extraLen + commentLen;
        if (entryName.length() >= 4 &&
            entryName.endsWith(".xml") &&
            !entryName.contains('/')) {
            names.append(entryName);
        }
    }
    return names;
}

// Find the first descendant element with the given tag name (recursive).
QDomElement firstDescendant(const QDomElement& parent, const QString& tag) {
    if (parent.tagName() == tag) return parent;
    for (QDomElement child = parent.firstChildElement();
         !child.isNull();
         child = child.nextSiblingElement()) {
        const QDomElement hit = firstDescendant(child, tag);
        if (!hit.isNull()) return hit;
    }
    return QDomElement();
}

} // namespace

// ---------------------------------------------------------------------------
// Bridge context — owns real Krita objects.
// ---------------------------------------------------------------------------
struct KritaBrushContext {
    // ABI parameters (mirrors the fallback bridge defaults).
    double size = 16.0;
    double opacity = 1.0;
    double hardness = 0.85;
    double spacing = 0.15;
    double smudge = 0.0;
    uint32_t color = 0xFF000000; // ARGB
    bool eraser = false;

    // Real Krita engine state.
    const KoColorSpace* cs = nullptr;   // lazily created RGBA8
    int rIdx = 0;                       // probed channel byte indices
    int gIdx = 1;
    int bIdx = 2;
    int aIdx = 3;                       // -1 if the colorspace has no alpha
    KisBrushSP brush;                   // real KisAutoBrush / preset brush
    bool brushFromPreset = false;       // brush loaded via KisBrush::fromXML

    // Bookkeeping.
    QString presetPath;
    QString presetName;
    std::vector<std::string> availablePresets;
    std::string lastError;
    std::string nameBuffer;
    std::string versionBuffer;

    void setError(const std::string& m) { lastError = m; }
    void clearError() { lastError.clear(); }
};

// ---------------------------------------------------------------------------
// Engine setup (all construction goes through real Krita classes).
// ---------------------------------------------------------------------------

namespace {

// Rebuild the real auto brush from the current size/hardness parameters.
// This mirrors what Krita's own KisAutoBrushFactory::createBrush does for
// <brush type="auto_brush"> definitions: a circle generator for hard tips,
// a gaussian generator with (1 - hardness) fade for soft tips, spikes=2
// (Krita's own default for round tips).
void rebuildAutoBrush(KritaBrushContext* h) {
    const qreal fade = qBound<qreal>(0.0, 1.0 - h->hardness, 1.0);
    KisMaskGenerator* gen;
    if (h->hardness >= 0.999) {
        gen = new KisCircleMaskGenerator(h->size, 1.0, 0.0, 0.0, 2, true);
    } else {
        gen = new KisGaussCircleMaskGenerator(h->size, 1.0, fade, fade, 2, true);
    }
    // KisAutoBrush takes ownership of the mask generator.
    h->brush = KisBrushSP(new KisAutoBrush(gen, 0.0, 0.0, 1.0));
    h->brushFromPreset = false;
}

// Lazily initialize the real colorspace + brush. Returns false on failure
// (last_error is set).
bool ensureEngine(KritaBrushContext* h) {
    if (!h->cs) {
        h->cs = KoColorSpaceRegistry::instance()->rgb8();
        if (!h->cs) {
            h->setError("KoColorSpaceRegistry::rgb8() failed");
            return false;
        }
        // Probe the channel byte indices with fully TRANSPARENT primary
        // colors: exactly one byte is 255 per probe, which identifies R, G
        // and B regardless of the layout Krita uses (RGBA, BGRA, ...).
        struct Probe { const char* name; int* idx; };
        const QColor primaries[3] = {
            QColor(255, 0, 0, 0),
            QColor(0, 255, 0, 0),
            QColor(0, 0, 255, 0),
        };
        int* targets[3] = { &h->rIdx, &h->gIdx, &h->bIdx };
        for (int p = 0; p < 3; ++p) {
            KoColor probe(primaries[p], h->cs);
            const quint8* d = probe.data();
            for (int i = 0; i < h->cs->channelCount(); ++i) {
                if (d[i] == 255) { *targets[p] = i; break; }
            }
        }
        // Alpha: the remaining channel (channel indices sum to 0+1+2+3).
        if (h->cs->channelCount() == 4) {
            h->aIdx = 6 - (h->rIdx + h->gIdx + h->bIdx);
            if (h->aIdx < 0 || h->aIdx > 3 ||
                (h->aIdx == h->rIdx || h->aIdx == h->gIdx || h->aIdx == h->bIdx)) {
                h->aIdx = -1;
            }
        } else {
            h->aIdx = -1; // no alpha channel: alpha is implicitly 255
        }
    }
    if (!h->brush) rebuildAutoBrush(h);
    return true;
}

KoColor abiColorToKoColor(KritaBrushContext* h, bool forceBlack = false) {
    if (forceBlack) {
        return KoColor(QColor(0, 0, 0), h->cs);
    }
    const int r = (h->color >> 16) & 0xFF;
    const int g = (h->color >> 8) & 0xFF;
    const int b = h->color & 0xFF;
    return KoColor(QColor(r, g, b), h->cs);
}

} // namespace

// ---------------------------------------------------------------------------
// C API implementation.
// ---------------------------------------------------------------------------

extern "C" {

KritaBrushContext* krita_brush_init(void) {
    KritaBrushContext* ctx = new (std::nothrow) KritaBrushContext();
    return ctx;
}

void krita_brush_destroy(KritaBrushContext* handle) {
    if (!handle) return;
    delete handle;
}

int32_t krita_brush_load_preset(KritaBrushContext* handle, const char* path) {
    if (!handle || !path) return 1;
    handle->clearError();
    if (!ensureEngine(handle)) return 10;

    QFileInfo info(QString::fromUtf8(path));
    if (!info.exists() || !info.isFile()) {
        handle->setError("preset file does not exist: " + std::string(path));
        return 2;
    }
    handle->presetPath = info.filePath();
    handle->presetName = info.completeBaseName();

    QFile f(info.filePath());
    if (!f.open(QIODevice::ReadOnly)) {
        handle->setError("cannot open preset file");
        return 3;
    }
    const QByteArray container = f.readAll();
    f.close();

    // Extract the preset XML (a .kpp is a PKZIP archive whose XML entry is
    // named after the preset; a bare XML preset is also accepted).
    QByteArray xml;
    if (container.size() >= 4 && rd32(reinterpret_cast<const uint8_t*>(container.constData())) == 0x04034b50UL) {
        const QList<QByteArray> entries = zipListXmlEntries(container);
        for (const QByteArray& entryName : entries) {
            const QByteArray candidate = zipExtractFile(container, QString::fromUtf8(entryName));
            if (!candidate.isEmpty()) { xml = candidate; break; }
        }
    } else if (!container.isEmpty()) {
        xml = container;
    }
    if (xml.isEmpty()) {
        handle->setError("no preset XML found in container");
        return 4;
    }

    QDomDocument doc;
    if (!doc.setContent(xml)) {
        handle->setError("preset XML is not well-formed");
        return 5;
    }

    // REAL path: hand the <brush> definition to Krita's own loader.
    QDomElement root = doc.documentElement();
    QDomElement brushEl = firstDescendant(root, "brush");
    if (!brushEl.isNull()) {
        KisBrushSP realBrush =
            KisBrush::fromXML(brushEl, KisResourcesInterface::instance());
        if (realBrush && realBrush->valid()) {
            handle->brush = realBrush;
            handle->brushFromPreset = true;
        }
        // Real spacing attribute on the brush element.
        if (brushEl.hasAttribute("spacing")) {
            bool ok = false;
            const double s = brushEl.attribute("spacing").toDouble(&ok);
            if (ok && s > 0.0) handle->spacing = s;
        }
        // Real tip diameter from the MaskGenerator element.
        QDomElement mg = brushEl.firstChildElement("MaskGenerator");
        if (!mg.isNull()) {
            QString diamAttr = mg.hasAttribute("diameter")
                ? mg.attribute("diameter") : mg.attribute("radius");
            bool ok = false;
            const double d = diamAttr.toDouble(&ok);
            if (ok && d >= 1.0) handle->size = d;
        }
    }

    // Best-effort preset-level parameters (size/opacity/hardness live in the
    // paintop settings section, which belongs to kritaui; the brush tip and
    // spacing above are the real engine parts).
    QDomElement param = firstDescendant(root, "param");
    while (!param.isNull()) {
        const QString key = param.attribute("name");
        bool ok = false;
        const double v = param.attribute("value").toDouble(&ok);
        if (ok) {
            if (key == "opacity" || key == "brush_opacity") {
                if (v > 0.0) handle->opacity = v;
            } else if (key == "hardness" || key == "softness") {
                handle->hardness = (key == "softness") ? (1.0 - v) : v;
            }
        }
        param = param.nextSiblingElement("param");
    }

    return 0;
}

void krita_brush_set_size(KritaBrushContext* handle, double size) {
    if (!handle) return;
    const double s = size < 1.0 ? 1.0 : size;
    if (std::abs(s - handle->size) > 1e-9 || !handle->brush) {
        handle->size = s;
        // Only rebuild auto brushes; preset-loaded brushes are scaled via
        // KisDabShape in generate_dab (real Krita behavior).
        if (!handle->brushFromPreset) {
            if (ensureEngine(handle)) rebuildAutoBrush(handle);
        }
    } else {
        handle->size = s;
    }
}

void krita_brush_set_color(KritaBrushContext* handle, uint32_t argb) {
    if (!handle) return;
    handle->color = argb;
}

void krita_brush_set_opacity(KritaBrushContext* handle, double opacity) {
    if (!handle) return;
    handle->opacity = opacity < 0.0 ? 0.0 : (opacity > 1.0 ? 1.0 : opacity);
}

void krita_brush_set_spacing(KritaBrushContext* handle, double spacing) {
    if (!handle) return;
    handle->spacing = spacing < 0.0 ? 0.0 : (spacing > 5.0 ? 5.0 : spacing);
}

void krita_brush_set_smudge(KritaBrushContext* handle, double smudge) {
    if (!handle) return;
    handle->smudge = smudge < 0.0 ? 0.0 : (smudge > 1.0 ? 1.0 : smudge);
}

double krita_brush_get_size(KritaBrushContext* handle) {
    return handle ? handle->size : 0.0;
}

double krita_brush_get_opacity(KritaBrushContext* handle) {
    return handle ? handle->opacity : 0.0;
}

double krita_brush_get_spacing(KritaBrushContext* handle) {
    return handle ? handle->spacing : 0.0;
}

double krita_brush_get_hardness(KritaBrushContext* handle) {
    return handle ? handle->hardness : 0.0;
}

double krita_brush_get_smudge(KritaBrushContext* handle) {
    return handle ? handle->smudge : 0.0;
}

const char* krita_brush_get_preset_name(KritaBrushContext* handle) {
    if (!handle) return "";
    handle->nameBuffer = handle->presetName.toStdString();
    return handle->nameBuffer.c_str();
}

bool krita_brush_generate_dab(KritaBrushContext* handle,
                              const BrushInput* input,
                              BrushDab* out_dab) {
    if (!handle || !input || !out_dab) return false;
    handle->clearError();

    out_dab->width = 0;
    out_dab->height = 0;
    out_dab->stride = 0;
    out_dab->reserved = 0;
    out_dab->pixels = nullptr;

    if (!ensureEngine(handle)) return false;

    const double pressure = (input->pressure > 0.0 && input->pressure <= 1.0)
        ? input->pressure : 1.0;
    const bool eraser = (input->flags & kBrushInputEraser) || handle->eraser;

    // ABI pressure->size policy (identical to the fallback bridge's
    // documented behavior: minimum 20% of the configured diameter).
    const double sizeFactor = 0.2 + 0.8 * pressure;
    if (handle->size * sizeFactor < 1.0) return false;

    // Real dab scale. For auto brushes the generator diameter equals the
    // configured size, so the extra factor is 1; preset brushes scale
    // relative to their own effective size (real Krita semantics).
    qreal scale = sizeFactor;
    if (handle->brushFromPreset) {
        const qreal eff = qMax<qreal>(1.0, handle->brush->userEffectiveSize());
        scale = sizeFactor * (handle->size / eff);
    }

    // Real Krita dab generation:
    //   tip image -> pyramid scaling/rotation -> colorize through mask.
    const KisDabShape shape(scale, 1.0, 0.0);
    const KisPaintInformation info(QPointF(0.0, 0.0), pressure);
    const KoColor c = abiColorToKoColor(handle, eraser);

    KisFixedPaintDeviceSP dst = new KisFixedPaintDevice(handle->cs);
    handle->brush->mask(dst, c, shape, info,
                        input->x - std::floor(input->x),   // real subpixel AA
                        input->y - std::floor(input->y));
    if (!dst || dst->bounds().isEmpty() || !dst->data()) {
        handle->setError("KisBrush::mask produced no dab");
        return false;
    }

    const QRect bounds = dst->bounds();
    const int w = bounds.width();
    const int h = bounds.height();
    const int stride = w * 4;
    const size_t bytes = size_t(stride) * size_t(h);
    uint8_t* buf = static_cast<uint8_t*>(std::malloc(bytes));
    if (!buf) {
        handle->setError("out of memory");
        return false;
    }

    // Convert Krita's native RGBA8 layout to the ABI's straight-alpha
    // R,G,B,A layout and apply the ABI's pressure->alpha scaling.
    const quint8* src = dst->data();
    const int rIdx = handle->rIdx;
    const int gIdx = handle->gIdx;
    const int bIdx = handle->bIdx;
    const int aIdx = handle->aIdx;
    const quint32 pixelSize = handle->cs->pixelSize();
    for (int y = 0; y < h; ++y) {
        const quint8* row = src + size_t(y) * size_t(w) * pixelSize;
        uint8_t* out = buf + size_t(y) * size_t(stride);
        for (int x = 0; x < w; ++x) {
            const quint8* px = row + size_t(x) * pixelSize;
            out[0] = px[rIdx];
            out[1] = px[gIdx];
            out[2] = px[bIdx];
            const uint8_t alpha = (aIdx >= 0) ? px[aIdx] : 255;
            // ABI pressure scaling on the alpha channel.
            const int a = int(std::lround(double(alpha) * pressure));
            out[3] = uint8_t(a > 255 ? 255 : a);
            out += 4;
        }
    }

    out_dab->width = w;
    out_dab->height = h;
    out_dab->stride = stride;
    out_dab->reserved = 0;
    out_dab->pixels = buf;
    return true;
}

void krita_brush_release_dab(KritaBrushContext* handle, BrushDab* dab) {
    if (!handle || !dab) return;
    if (dab->pixels) {
        std::free(dab->pixels);
        dab->pixels = nullptr;
    }
    dab->width = 0;
    dab->height = 0;
    dab->stride = 0;
}

void krita_brush_cleanup(KritaBrushContext* handle) {
    if (!handle) return;
    // Real Krita keeps per-stroke state inside the paintop / stroke
    // strategy; the dab-level ABI is stateless per stroke.
}

const char* krita_brush_last_error(KritaBrushContext* handle) {
    if (!handle) return "";
    return handle->lastError.c_str();
}

const char* krita_brush_version(void) {
    // Real Krita version, fetched from the unmodified libkritaversion.
    static std::string v;
    if (v.empty()) {
        const QString s = KritaVersionWrapper::versionString(false);
        v = "FeatherBridge-Krita/2.0 (real engine " + s.toStdString() + ")";
    }
    return v.c_str();
}

int32_t krita_brush_preset_count(KritaBrushContext* handle) {
    if (!handle) return -1;
    if (handle->availablePresets.empty()) {
        QStringList dirs;
        dirs << "assets/brushes"
             << "brushes"
             << QDir::homePath() + "/.local/share/krita/brushes"
             << QDir::homePath() + "/AppData/Roaming/krita/brushes";
        QStringList files;
        for (const QString& d : dirs) {
            QDir dir(d);
            if (dir.exists()) {
                const QStringList found = dir.entryList(QStringList() << "*.kpp", QDir::Files);
                for (const QString& f : found) files << f;
            }
        }
        files.removeDuplicates();
        handle->availablePresets.reserve(files.size());
        for (const QString& f : files) {
            handle->availablePresets.push_back(f.toUtf8().constData());
        }
    }
    return int32_t(handle->availablePresets.size());
}

const char* krita_brush_preset_name(KritaBrushContext* handle, int32_t index) {
    if (!handle) return nullptr;
    if (index < 0 || index >= krita_brush_preset_count(handle)) return nullptr;
    handle->nameBuffer = handle->availablePresets[index];
    return handle->nameBuffer.c_str();
}

} // extern "C"
