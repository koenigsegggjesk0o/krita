// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// smoke_test_real.cpp — CI smoke test for the REAL Krita bridge.
//
// Links against libkrita_bridge_real.so + the real libkrita* libraries and
// verifies end-to-end dab generation through unmodified Krita v6.0.4 code:
//   - engine init (KoColorSpaceRegistry + KisAutoBrush construction)
//   - hard / soft dab generation (KisBrush::mask path)
//   - exact color passthrough (straight-alpha RGBA output)
//   - pressure->alpha and pressure->size ABI scaling
//   - eraser mode (black-alpha mask output)

#include "krita_bridge.h"

#include <cstdio>
#include <cstdlib>
#include <cstring>

static int g_failures = 0;

#define CHECK(cond, msg)                                                  \
    do {                                                                  \
        if (cond) {                                                       \
            std::printf("  ok: %s\n", msg);                               \
        } else {                                                          \
            std::printf("  FAIL: %s (line %d)\n", msg, __LINE__);         \
            ++g_failures;                                                 \
        }                                                                 \
    } while (0)

int main() {
    std::printf("version: %s\n", krita_brush_version());
    KritaBrushContext* b = krita_brush_init();
    CHECK(b != nullptr, "init returns handle");

    const double size0 = krita_brush_get_size(b);
    CHECK(size0 > 0.0, "default size positive");

    // --- Hard round brush, full pressure: opaque exact-color center.
    krita_brush_set_size(b, 32.0);
    krita_brush_set_color(b, 0xFF2040A0u); // A=FF R=0x20 G=0x40 B=0xA0
    BrushInput in;
    std::memset(&in, 0, sizeof(in));
    in.pressure = 1.0;
    BrushDab dab;
    std::memset(&dab, 0, sizeof(dab));
    const bool ok = krita_brush_generate_dab(b, &in, &dab);
    CHECK(ok, "hard dab generated");
    if (ok) {
        CHECK(dab.width >= 28 && dab.width <= 34, "hard dab diameter ~= size");
        CHECK(dab.stride == dab.width * 4, "stride == width*4");
        const uint8_t* px = dab.pixels + (dab.height / 2) * dab.stride + (dab.width / 2) * 4;
        std::printf("  center RGBA: %u %u %u %u\n", px[0], px[1], px[2], px[3]);
        CHECK(px[0] == 0x20 && px[1] == 0x40 && px[2] == 0xA0, "center color exact (straight RGBA)");
        CHECK(px[3] >= 250, "center opaque at full pressure");
        // Corner of the bounding box must be transparent for a round tip.
        const uint8_t* corner = dab.pixels;
        CHECK(corner[3] == 0, "bounding-box corner transparent");
        krita_brush_release_dab(b, &dab);
    }

    // --- Soft brush (default hardness 0.85 -> gaussian fade 0.15):
    // falloff present, edge weaker than center.
    krita_brush_set_size(b, 64.0);
    BrushInput in2;
    std::memset(&in2, 0, sizeof(in2));
    in2.pressure = 1.0;
    BrushDab soft;
    std::memset(&soft, 0, sizeof(soft));
    if (krita_brush_generate_dab(b, &in2, &soft)) {
        CHECK(soft.width >= 60, "soft dab sized from size=64");
        const uint8_t* c = soft.pixels + (soft.height / 2) * soft.stride + (soft.width / 2) * 4;
        const uint8_t* edge = soft.pixels + (soft.height / 2) * soft.stride + 4; // near left edge
        std::printf("  soft center alpha=%u edge alpha=%u\n", c[3], edge[3]);
        CHECK(c[3] >= 200, "soft center still strong");
        CHECK(edge[3] < c[3], "soft edge weaker than center (falloff)");
        krita_brush_release_dab(b, &soft);
    } else {
        CHECK(false, "soft dab generated");
    }

    // --- Pressure->alpha scaling: half pressure gives weaker center.
    BrushInput in3;
    std::memset(&in3, 0, sizeof(in3));
    in3.pressure = 0.5;
    BrushDab half;
    std::memset(&half, 0, sizeof(half));
    if (krita_brush_generate_dab(b, &in3, &half)) {
        const uint8_t* c = half.pixels + (half.height / 2) * half.stride + (half.width / 2) * 4;
        std::printf("  half-pressure center alpha=%u\n", c[3]);
        CHECK(c[3] < 255, "half pressure scales alpha down");
        CHECK(half.width < soft.width, "half pressure shrinks dab");
        krita_brush_release_dab(b, &half);
    } else {
        CHECK(false, "half-pressure dab generated");
    }

    // --- Eraser mode: black-alpha mask output.
    BrushInput in4;
    std::memset(&in4, 0, sizeof(in4));
    in4.pressure = 1.0;
    in4.flags = kBrushInputEraser;
    BrushDab erase;
    std::memset(&erase, 0, sizeof(erase));
    if (krita_brush_generate_dab(b, &in4, &erase)) {
        const uint8_t* c = erase.pixels + (erase.height / 2) * erase.stride + (erase.width / 2) * 4;
        std::printf("  eraser center RGBA: %u %u %u %u\n", c[0], c[1], c[2], c[3]);
        CHECK(c[0] == 0 && c[1] == 0 && c[2] == 0, "eraser dab is black mask");
        CHECK(c[3] >= 250, "eraser center opaque mask");
        krita_brush_release_dab(b, &erase);
    } else {
        CHECK(false, "eraser dab generated");
    }

    krita_brush_destroy(b);

    if (g_failures == 0) {
        std::printf("SMOKE OK — real Krita bridge end-to-end\n");
        return 0;
    }
    std::printf("SMOKE FAILED — %d failure(s)\n", g_failures);
    return 1;
}
