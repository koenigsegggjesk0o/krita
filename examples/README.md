# Examples

Three demonstrations of the brush engine.

## 1. `load_preset_standalone.c` — read a `.kpp` preset (zero-dependency C)

A pure-C parser for the Krita `.kpp` preset container format. A `.kpp` is
a **standard PNG file** whose text chunks carry the brush settings:

- `tEXt` chunk, key = `"version"`, value = `"2.2"` or `"5.0"`
- `zTXt` chunk, key = `"preset"`, value = zlib-compressed XML settings

The PNG pixel data doubles as the thumbnail. This parser walks the PNG
chunks directly (no Qt, no kritaimage) and extracts both the thumbnail
PNG and the decompressed settings XML.

Build (needs only a C compiler + zlib):
```bash
cc -std=c99 -O2 -Wall load_preset_standalone.c -o load_preset_standalone -lz
./load_preset_standalone ../brush-presets/colorsmudge.kpp
# -> writes ./thumbnail.png and ./settings.xml
```

This example has been **compiled and tested** in this environment and
successfully parses every `.kpp` in `brush-presets/`.

## 2. `load_preset.cpp` — same as above, Qt version (standalone)

The Qt equivalent of #1, using `QImageReader::text("version")` /
`QImageReader::text("preset")` (which is how upstream Krita reads it).
Standalone — needs only Qt + zlib, not kritaimage.

Build:
```bash
g++ -std=c++17 -fPIC load_preset.cpp -o load_preset \
    $(pkg-config --cflags --libs Qt5Core Qt5Gui) -lz
./load_preset ../brush-presets/curvebrush.kpp
```

## 3. `make_dab.cpp` — render a procedural dab (requires Krita build)

Demonstrates constructing a `KisAutoBrush` and rendering a single dab
`QImage`. Must be compiled inside a Krita build tree because it links
`kritalibbrush` + `kritapaintop` + `kritaimage`.

Build (inside a Krita build):
```bash
g++ -std=c++17 -fPIC \
    -I/path/to/krita/libs/brush -I/path/to/krita/libs/image \
    -I/path/to/krita/libs/global -I/path/to/krita/libs/pigment \
    -I/path/to/krita/build/libs/brush -I/path/to/krita/build/libs/image \
    make_dab.cpp -o make_dab \
    -L/path/to/krita/build/lib -lkritalibbrush -lkritaimage -lkritapigment \
    $(pkg-config --cflags --libs Qt5Core Qt5Gui Qt5Widgets) \
    -Wl,-rpath,/path/to/krita/build/lib
./make_dab  # writes dab.png
```

See the comments at the top of each file for the exact API calls used.
