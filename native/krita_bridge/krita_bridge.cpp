// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// krita_bridge.cpp — C++ implementation of the FFI bridge.
//
// Implements the `extern "C"` API declared in krita_bridge.h on top of
// Krita's public C++ API. The bridge:
//   - Owns a KisPaintDevice used as the dab surface.
//   - Loads KisPaintOpPresets from .kpp files.
//   - Calls KisPainter::paintAt() to generate dabs.
//   - Converts the dirty rect of the paint device to a contiguous RGBA8
//     buffer that the Dart side copies out.
//
// IMPORTANT: this file MUST NOT modify any Krita source code. It only
// calls public Krita / Qt API.

#include "krita_bridge.h"

#include <cstring>
#include <cstdlib>
#include <new>
#include <string>
#include <vector>

#include <QImage>
#include <QDir>
#include <QFileInfo>
#include <QStandardPaths>
#include <QColor>
#include <QTransform>

// Krita headers — these are resolved at build time via the CMakeLists
// include paths. The paths below match the layout of
// `krita-source/libs/`.
#include <KoColorSpace.h>
#include <KoColorSpaceRegistry.h>
#include <KoColor.h>

#include <kis_paint_device.h>
#include <kis_painter.h>
#include <kis_image.h>
#include <kis_paintop_preset.h>
#include <kis_paintop_registry.h>
#include <kis_paintop.h>
#include <kis_paint_information.h>
#include <kis_brush.h>
#include <kis_brush_server.h>
#include <kis_brush_resource_server.h>
#include <kis_resources_interface.h>
#include <kis_display_color_converter.h>

// ---------------------------------------------------------------------------
// Internal context.
// ---------------------------------------------------------------------------

namespace {

/// Convert an ARGB packed value to a KoColor using the device's color
/// space. Krita expects colors in the working color space, which for our
/// brush device is RGBA8.
KoColor argbToKoColor(uint32_t argb, const KoColorSpace* cs) {
    // KoColor can be constructed from a QColor.
    QColor c = QColor::fromRgba(argb);
    KoColor color(c, cs);
    return color;
}

} // namespace

struct KritaBrushContext {
    /// Working color space (RGBA8 unpremultiplied).
    const KoColorSpace* colorSpace = nullptr;

    /// Offscreen paint device dabs are stamped into. We resize it as
    /// needed to fit each dab.
    KisPaintDeviceSP device = nullptr;

    /// The currently loaded paintop preset. May be null.
    KisPaintOpPresetSP preset = nullptr;

    /// Painter bound to [device]. Lazily created.
    QScopedPointer<KisPainter> painter;

    /// Cached brush settings.
    double size = 16.0;
    double opacity = 1.0;
    double spacing = 0.15;
    double smudge = 0.0;
    uint32_t color = 0xFF000000;

    /// True after at least one dab has been generated without an
    /// intervening cleanup() call. Used to set "isStartOfStroke" on the
    /// first dab.
    bool strokeStarted = false;

    /// Buffer for the last error message.
    std::string lastError;

    /// Buffer for preset names returned via krita_brush_preset_name.
    std::string presetNameBuffer;

    /// Cached list of preset names available from the brush server.
    std::vector<std::string> availablePresets;

    void setError(const std::string& msg) {
        lastError = msg;
    }

    void clearError() {
        lastError.clear();
    }
};

// ---------------------------------------------------------------------------
// Helpers.
// ---------------------------------------------------------------------------

namespace {

/// Computes the radius of the currently loaded preset in pixels. Falls
/// back to the context's [size] if the preset is unavailable.
double effectiveRadius(KritaBrushContext* ctx) {
    if (!ctx->preset) return ctx->size * 0.5;
    KisPaintOpSettingsSP settings = ctx->preset->settings();
    if (!settings) return ctx->size * 0.5;
    QVariant v = settings->getProperty("size");
    bool ok = false;
    double s = v.toDouble(&ok);
    return ok ? s * 0.5 : ctx->size * 0.5;
}

/// Reads back the dirty rect of [ctx->device] as an RGBA8 buffer. The
/// returned buffer is allocated with malloc() and the caller owns it.
/// Returns an empty BrushDab if the device has no extent.
BrushDab readDeviceToDab(KritaBrushContext* ctx) {
    BrushDab out = {};
    out.width = 0;
    out.height = 0;
    out.stride = 0;
    out.reserved = 0;
    out.pixels = nullptr;

    if (!ctx->device) return out;
    QRect extent = ctx->device->extent();
    if (extent.isEmpty()) return out;

    // Convert to a QImage in RGBA8 format. Krita's
    // convertToQImage gives us a 32-bit ARGB image.
    const QImage img = ctx->device->convertToQImage(
        ctx->colorSpace, extent.x(), extent.y(),
        extent.width(), extent.height(),
        KoColorConversionProfile::Intent::IntentPerceptual);
    if (img.isNull()) {
        ctx->setError("convertToQImage returned a null image");
        return out;
    }

    const QImage normalized = img.convertToFormat(QImage::Format_RGBA8888);
    const int w = normalized.width();
    const int h = normalized.height();
    const int stride = w * 4;
    const size_t bytes = static_cast<size_t>(stride) * static_cast<size_t>(h);

    uint8_t* buf = static_cast<uint8_t*>(std::malloc(bytes));
    if (!buf) {
        ctx->setError("out of memory allocating dab buffer");
        return out;
    }
    std::memcpy(buf, normalized.constBits(), bytes);

    out.width = w;
    out.height = h;
    out.stride = stride;
    out.reserved = 0;
    out.pixels = buf;
    return out;
}

} // namespace

// ---------------------------------------------------------------------------
// C API implementation.
// ---------------------------------------------------------------------------

extern "C" {

KritaBrushContext* krita_brush_init(void) {
    KritaBrushContext* ctx = new (std::nothrow) KritaBrushContext();
    if (!ctx) return nullptr;

    ctx->colorSpace = KoColorSpaceRegistry::instance()->rgb8(
        KoColorSpaceRegistry::instance()->rgb8Profile("sRGB-elle-V2-srgbtrc.icc"));
    if (!ctx->colorSpace) {
        // Fall back to the default RGB8 space.
        ctx->colorSpace = KoColorSpaceRegistry::instance()->colorSpace(
            "RGBA", "", KoColorSpaceRegistry::instance()->rgb8Profile(""));
    }
    if (!ctx->colorSpace) {
        delete ctx;
        return nullptr;
    }

    ctx->device = new KisPaintDevice(ctx->colorSpace);
    if (!ctx->device) {
        delete ctx;
        return nullptr;
    }

    ctx->painter.reset(new KisPainter(ctx->device));
    if (!ctx->painter) {
        delete ctx;
        return nullptr;
    }

    // Apply default color.
    KoColor c = argbToKoColor(ctx->color, ctx->colorSpace);
    ctx->painter->setPaintColor(c);

    return ctx;
}

void krita_brush_destroy(KritaBrushContext* handle) {
    if (!handle) return;
    // KisPaintDevice and KisPaintOpPreset are reference-counted via
    // KoSP shared pointers, so they will be released automatically when
    // the QSharedPointer / KisSharedPointer destructors run.
    delete handle;
}

int32_t krita_brush_load_preset(KritaBrushContext* handle, const char* path) {
    if (!handle || !path) return 1;
    handle->clearError();

    QFileInfo info(QString::fromUtf8(path));
    if (!info.exists() || !info.isFile()) {
        handle->setError("preset file does not exist: " + std::string(path));
        return 2;
    }

    // KisPaintOpPreset can be loaded directly from a .kpp file via its
    // resource loader. We construct a fresh preset, set its file name,
    // and ask it to load.
    KisPaintOpPresetSP preset = new KisPaintOpPreset(info.filePath());
    if (!preset->load()) {
        handle->setError("failed to load preset: " + std::string(path));
        return 3;
    }

    handle->preset = preset;
    if (handle->painter) {
        handle->painter->setPaintOpPreset(preset);
    }

    // Apply the preset's default size / opacity so subsequent generate_dab
    // calls have a sensible state.
    if (KisPaintOpSettingsSP s = preset->settings()) {
        bool ok = false;
        const double sz = s->getProperty("size").toDouble(&ok);
        if (ok) handle->size = sz;
        const double op = s->getProperty("opacity").toDouble(&ok);
        if (ok) handle->opacity = op;
    }
    return 0;
}

void krita_brush_set_size(KritaBrushContext* handle, double size) {
    if (!handle) return;
    handle->size = size < 1.0 ? 1.0 : size;
    if (handle->preset && handle->preset->settings()) {
        // Push the new size into the preset so the paintop picks it up
        // on the next dab.
        KisPaintOpSettingsSP s = handle->preset->settings();
        s->setProperty("size", size);
    }
}

void krita_brush_set_color(KritaBrushContext* handle, uint32_t argb) {
    if (!handle) return;
    handle->color = argb;
    if (handle->painter && handle->colorSpace) {
        KoColor c = argbToKoColor(argb, handle->colorSpace);
        handle->painter->setPaintColor(c);
    }
}

void krita_brush_set_opacity(KritaBrushContext* handle, double opacity) {
    if (!handle) return;
    handle->opacity = opacity < 0.0 ? 0.0 : (opacity > 1.0 ? 1.0 : opacity);
    if (handle->preset && handle->preset->settings()) {
        handle->preset->settings()->setProperty("opacity", handle->opacity);
    }
}

void krita_brush_set_spacing(KritaBrushContext* handle, double spacing) {
    if (!handle) return;
    handle->spacing = spacing < 0.0 ? 0.0 : (spacing > 5.0 ? 5.0 : spacing);
    if (handle->preset && handle->preset->settings()) {
        handle->preset->settings()->setProperty("spacing", handle->spacing);
    }
}

void krita_brush_set_smudge(KritaBrushContext* handle, double smudge) {
    if (!handle) return;
    handle->smudge = smudge < 0.0 ? 0.0 : (smudge > 1.0 ? 1.0 : smudge);
    if (handle->preset && handle->preset->settings()) {
        handle->preset->settings()->setProperty("smudge_mode", handle->smudge);
    }
}

bool krita_brush_generate_dab(KritaBrushContext* handle,
                              const BrushInput* input,
                              BrushDab* out_dab) {
    if (!handle || !input || !out_dab) return false;
    handle->clearError();

    // Zero out the output first.
    out_dab->width = 0;
    out_dab->height = 0;
    out_dab->stride = 0;
    out_dab->reserved = 0;
    out_dab->pixels = nullptr;

    if (!handle->painter || !handle->device || !handle->colorSpace) {
        handle->setError("brush context not initialized");
        return false;
    }

    // Reset the device extent so the new dab is the only thing painted.
    handle->device->clear();

    // Build a KisPaintInformation from the input struct.
    QPointF pos(input->x, input->y);
    KisPaintInformation info(pos, input->pressure,
                             QVector2D(input->tilt_x, input->tilt_y),
                             input->tilt_x, input->tilt_y,
                             input->time,
                             QPointF(input->velocity_x, input->velocity_y));
    info.setIsStartOfStroke(!handle->strokeStarted);
    handle->strokeStarted = true;

    // Make sure the painter has the preset bound. If no preset is loaded,
    // we fall back to a simple circle brush by setting a default preset.
    if (handle->preset) {
        handle->painter->setPaintOpPreset(handle->preset);
    } else {
        handle->setError("no preset loaded; call krita_brush_load_preset first");
        return false;
    }

    // Translate so the dab is centered on the device's origin.
    const double radius = effectiveRadius(handle);
    handle->painter->translate(QTransform::fromTranslate(radius, radius));

    // Generate the dab.
    handle->painter->paintAt(info);

    // Reset the painter transform so subsequent dabs use absolute coords.
    handle->painter->translate(QTransform::fromTranslate(-radius, -radius));

    // Read back the dirty pixels.
    BrushDab dab = readDeviceToDab(handle);
    *out_dab = dab;
    return dab.pixels != nullptr;
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
    handle->strokeStarted = false;
    if (handle->device) {
        handle->device->clear();
    }
}

const char* krita_brush_last_error(KritaBrushContext* handle) {
    if (!handle) return "";
    return handle->lastError.c_str();
}

const char* krita_brush_version(void) {
    // Krita exposes its version via KAboutData at runtime; we hard-code a
    // compile-time fallback that's overridden when Krita's headers ship
    // KRITA_VERSION_STRING.
#ifdef KRITA_VERSION_STRING
    return KRITA_VERSION_STRING;
#else
    return "5.x (bridge-compiled)";
#endif
}

int32_t krita_brush_preset_count(KritaBrushContext* handle) {
    if (!handle) return -1;
    if (handle->availablePresets.empty()) {
        KisBrushServer* server = KisBrushServer::instance();
        if (!server) return -1;
        KisBrushResourceServer* res = server->brushServer();
        if (!res) return -1;
        QList<KisBrushSP> brushes = res->resources();
        handle->availablePresets.reserve(brushes.size());
        for (const KisBrushSP& b : brushes) {
            if (b) handle->availablePresets.push_back(b->name().toUtf8().constData());
        }
    }
    return static_cast<int32_t>(handle->availablePresets.size());
}

const char* krita_brush_preset_name(KritaBrushContext* handle, int32_t index) {
    if (!handle) return nullptr;
    if (index < 0 || index >= krita_brush_preset_count(handle)) return nullptr;
    handle->presetNameBuffer = handle->availablePresets[index];
    return handle->presetNameBuffer.c_str();
}

} // extern "C"
