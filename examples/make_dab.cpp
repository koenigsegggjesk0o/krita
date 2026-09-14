/*
 * SPDX-FileCopyrightText: 2026 Krita Brush Engine Extraction Contributors
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * make_dab.cpp — render a single procedural brush dab to a PNG.
 *
 * Demonstrates the core brush-tip API:
 *   KisBrushRegistry -> KisAutoBrushFactory -> KisAutoBrush
 *   KisBrush::generateDab(KisDabShape, KisPaintInformation, ...) -> QImage
 *
 * IMPORTANT: this example must be compiled inside a Krita build tree.
 * The brush engine interfaces (KisPaintInformation, KisDabShape) live in
 * kritaimage, and KisAutoBrush lives in kritalibbrush. See ../README.md
 * §6 for why the brushengine/ sources cannot form a fully standalone .so.
 *
 * Pseudo-code sketch (the real call signatures depend on the exact
 * Krita version; refer to libs/brush/kis_auto_brush.h and
 * libs/brush/kis_brush.h in the matching source tree):
 *
 *   #include <brushengine/kis_paint_information.h>
 *   #include <kis_auto_brush.h>
 *   #include <kis_brush_registry.h>
 *   #include <KoColor.h>
 *   #include <KoColorSpaceRegistry.h>
 *
 *   // 1. Build a procedural soft round brush via the factory.
 *   auto *factory = qobject_cast<KisAutoBrushFactory*>(
 *       KisBrushRegistry::instance()->get("auto"));
 *   KisBrushSP brush = factory->createBrush(xmlSettings);  // or build
 *                                                            // KisAutoBrush
 *                                                            // directly
 *
 *   // 2. A fake paint tick at the centre of the image, full pressure.
 *   KisPaintInformation info(QPointF(0.5, 0.5), 1.0 /*pressure*/);
 *
 *   // 3. Render the dab.
 *   KisDabShape shape(1.0 /*scaleX*/, 1.0 /*scaleY*/, 0.0 /*rotation*/);
 *   QImage dab = brush->generateDab(shape, info,
 *                                   256 /*subPixelX*/, 256 /*subPixelY*/,
 *                                   KoColorSpaceRegistry::instance()->rgb8(),
 *                                   KoColor(QColor(Qt::black),
 *                                           KoColorSpaceRegistry::instance()->rgb8()));
 *
 *   // 4. Save.
 *   dab.save("dab.png");
 *
 * The full typed call (with the current upstream signature) is shown in
 * the function body below. Because signatures drift between Krita
 * versions, prefer copying the exact prototype from the source tree you
 * are building against.
 */

// This file is a documentation sketch. To turn it into a real program:
//  1. Build Krita from source (see ../README.md §5.1).
//  2. Fill in the real includes above.
//  3. Compile with the flags in examples/README.md.
//
// The function below mirrors the upstream generateDab usage from
// libs/brush/kis_brush.h:

#if 0  // enable when building inside a Krita tree

#include <QImage>
#include <QPointF>
#include <QColor>

#include <KoColor.h>
#include <KoColorSpaceRegistry.h>

#include <brushengine/kis_paint_information.h>
#include <kis_auto_brush.h>
#include <kis_brush.h>
#include <kis_dab_shape.h>

void renderSampleDab() {
    // Procedural soft-round auto brush.
    KisAutoBrush *brush = new KisAutoBrush(KisBrushShape::CIRCLE, 0.5, 0.0);
    brush->setSpacing(0.05);
    brush->setDimension(256);  // base size

    // Centre of a 512x512 canvas, full pressure, no tilt.
    KisPaintInformation info(QPointF(256.0, 256.0), 1.0);

    KisDabShape shape(1.0, 1.0, 0.0);
    const KoColorSpace *cs = KoColorSpaceRegistry::instance()->rgb8();
    KoColor color(QColor(64, 96, 200), cs);

    QImage dab = brush->generateDab(shape, info, 0, 0, cs, color);
    dab.save("dab.png");

    qDebug("wrote dab.png  %dx%d", dab.width(), dab.height());
    delete brush;
}

int main() {
    renderSampleDab();
    return 0;
}

#endif

#include <cstdio>
int main() {
    std::printf(
        "make_dab.cpp is a documentation sketch.\n"
        "See the commented #if 0 block for the real KisAutoBrush::generateDab\n"
        "usage. Enable it inside a Krita build tree as described in\n"
        "examples/README.md.\n");
    return 0;
}
