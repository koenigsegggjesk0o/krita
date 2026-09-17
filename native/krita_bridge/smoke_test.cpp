// Local smoke test for the krita_bridge .so (Linux)
// Validates: init, param setters, dab generation, error handling, ABI shape.
#include <cstdio>
#include <cstdint>
#include <cstring>

extern "C" {
#include "krita_bridge.h"
}

int main() {
    printf("version: %s\n", krita_brush_version());

    KritaBrushContext* ctx = krita_brush_init();
    if (!ctx) { printf("FAIL: init returned null\n"); return 1; }
    printf("init: OK\n");

    krita_brush_set_size(ctx, 64.0);
    krita_brush_set_opacity(ctx, 1.0);
    krita_brush_set_spacing(ctx, 0.1);
    krita_brush_set_smudge(ctx, 0.0);
    printf("params: OK\n");

    BrushInput input;
    memset(&input, 0, sizeof(input));
    input.x = 100.0;
    input.y = 100.0;
    input.pressure = 1.0;
    input.tilt_x = 0.0;
    input.tilt_y = 0.0;
    input.time = 0.0;
    input.velocity_x = 0.0f;
    input.velocity_y = 0.0f;
    input.flags = (int32_t)kBrushInputNone;
    input._padding = 0;

    BrushDab dab;
    memset(&dab, 0, sizeof(dab));
    bool ok = krita_brush_generate_dab(ctx, &input, &dab);
    if (!ok || !dab.pixels) {
        printf("FAIL: generate_dab ok=%d err=%s\n", (int)ok, krita_brush_last_error(ctx));
        krita_brush_destroy(ctx);
        return 1;
    }
    printf("generate_dab: ok w=%d h=%d stride=%d\n", dab.width, dab.height, dab.stride);
    if (dab.width != 64 || dab.height != 64) {
        printf("FAIL: unexpected dab size\n");
        krita_brush_release_dab(ctx, &dab);
        krita_brush_destroy(ctx);
        return 1;
    }
    const uint8_t* px = dab.pixels;
    const uint8_t* center = px + (((size_t)dab.height / 2) * dab.stride) + ((size_t)dab.width / 2) * 4;
    const uint8_t* corner = px;
    printf("center RGBA: %d,%d,%d,%d\n", center[0], center[1], center[2], center[3]);
    printf("corner RGBA: %d,%d,%d,%d\n", corner[0], corner[1], corner[2], corner[3]);
    const bool pixelsOk = center[3] > 200 && corner[3] < 60;
    krita_brush_release_dab(ctx, &dab);
    printf("release_dab: OK\n");

    // Low-pressure dab
    input.pressure = 0.25;
    ok = krita_brush_generate_dab(ctx, &input, &dab);
    printf("low-pressure dab: ok=%d w=%d\n", (int)ok, dab.width);
    if (ok) krita_brush_release_dab(ctx, &dab);

    // Preset API: bad path should fail gracefully, not crash
    int32_t rc = krita_brush_load_preset(ctx, "/nonexistent/file.kpp");
    printf("load_preset(badpath): rc=%d (expect nonzero)\n", rc);
    printf("last_error: %s\n", krita_brush_last_error(ctx));

    // Preset listing may be -1 (Krita unavailable) — must not crash
    int32_t n = krita_brush_preset_count(ctx);
    printf("preset_count: %d\n", n);

    krita_brush_cleanup(ctx);
    krita_brush_destroy(ctx);
    printf("destroy: OK\n");
    printf(pixelsOk ? "SMOKE TEST: PASS\n" : "SMOKE TEST: FAIL\n");
    return pixelsOk ? 0 : 1;
}
