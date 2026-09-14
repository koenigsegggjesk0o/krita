/*
 * SPDX-FileCopyrightText: 2026 Krita Brush Engine Extraction Contributors
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * load_preset.cpp — Qt-based reader for the Krita .kpp preset format.
 *
 * ACTUAL .kpp FORMAT (verified from upstream kis_paintop_preset.cpp
 * and binary inspection):
 *
 *   A .kpp is a standard PNG file. The brush settings and a version tag
 *   are stored INSIDE the PNG as text chunks, read via QImageReader::text():
 *
 *     QImageReader reader(dev, "PNG");
 *     QString version = reader.text("version");   // "2.2" or "5.0"
 *     QString preset  = reader.text("preset");     // XML settings
 *
 *   The PNG pixel data is the preset thumbnail. This is exactly how
 *   KisPaintOpPreset::loadFromDevice() works upstream.
 *
 * This is the Qt counterpart of load_preset_standalone.c. It needs only
 * Qt5Core + Qt5Gui (no kritaimage), and is a faithful minimal mirror of
 * the upstream loader's container-parsing half.
 *
 * Build (standalone, Qt + zlib only — note: Qt bundles zlib so -lz is
 * usually not needed):
 *   g++ -std=c++17 -fPIC load_preset.cpp -o load_preset \
 *       $(pkg-config --cflags --libs Qt5Core Qt5Gui)
 *
 * Usage:
 *   ./load_preset <path/to/preset.kpp>
 *   -> writes ./thumbnail.png and ./settings.xml
 */

#include <QImage>
#include <QImageReader>
#include <QFile>
#include <QIODevice>
#include <QByteArray>
#include <QDebug>

int main(int argc, char **argv) {
    if (argc < 2) {
        qWarning() << "usage:" << argv[0] << "<preset.kpp>";
        return 1;
    }

    QFile f(argv[1]);
    if (!f.open(QIODevice::ReadOnly)) {
        qWarning() << "cannot open" << argv[1];
        return 2;
    }

    // QImageReader reads text chunks via .text("key") AFTER read().
    QImageReader reader(&f, "PNG");
    QImage img;
    if (!reader.read(&img)) {
        qWarning() << "PNG read failed:" << reader.errorString();
        return 3;
    }

    QString version = reader.text("version");
    QString preset  = reader.text("preset");

    qDebug() << "version chunk:" << version;
    qDebug() << "preset chunk size:" << preset.size() << "chars";

    if (!img.save("thumbnail.png")) {
        qWarning() << "could not write thumbnail.png";
    } else {
        qDebug() << "thumbnail:" << img.width() << "x" << img.height()
                 << "-> thumbnail.png";
    }

    if (preset.isEmpty()) {
        qWarning() << "no 'preset' text chunk — this .kpp has no settings";
        return 0;
    }

    QFile out("settings.xml");
    out.open(QIODevice::WriteOnly | QIODevice::Truncate);
    out.write(preset.toUtf8());
    out.close();
    qDebug() << "settings XML:" << preset.toUtf8().size() << "bytes -> settings.xml";

    // peek at the engine id
    int idx = preset.indexOf("paintopid");
    if (idx >= 0) {
        qDebug().noquote() << "engine hint:"
                           << preset.mid(idx, 80).replace("\n", " ");
    }
    return 0;
}
