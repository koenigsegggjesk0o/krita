# External dependencies (not bundled)

This extraction deliberately does NOT vendor the following external
libraries. They must be installed on the build host. See
`../../LICENSES/external-dependencies.txt` for license details.

| Dependency | License | Required by | Optional? |
|-----------|---------|-------------|-----------|
| Qt 5.15 / 6 (Core, Gui, Widgets, Svg, Xml, Concurrent) | LGPL-3.0 / GPL-3.0 / Commercial | everything | No |
| liblager | BSL-1.0 | brush option data/model layer (all `Kis*OptionData/Model`) | No |
| Boost (headers) | BSL-1.0 | brush core, paint_information, brush model | No |
| Eigen3 | MPL-2.0 | kritaimage mask algebra | No |
| libmypaint | LGPL-2.1-or-later | `paintops/mypaint` engine only | Yes |
| Expat / QtXml | (via Qt) | `.kpp` settings XML | No |
| zlib | zlib License | gzip layer of `.kpp` | No |

## Why they are not vendored

1. **Qt, Boost, Eigen** are large, system-provided, and every reasonable
   build host already has them. Vendoring would bloat this extraction
   and create maintenance burden.

2. **liblager** is a small header-mostly library under the permissive
   Boost Software License. Krita's own build fetches it via the separate
   `krita-deps-management` repo. We follow the same approach: install
   lager system-wide (`cmake --build lager --target install`).

3. **libmypaint** is optional and only affects the `mypaint` paintop.
   If absent, the build simply skips that one subdirectory (see
   `brush-engine/paintops/CMakeLists.txt`: `if(LibMyPaint_FOUND)`).

## Install commands (Debian/Ubuntu)

```bash
sudo apt install -y \
    build-essential cmake ninja-build pkg-config git \
    qtbase5-dev qtdeclarative5-dev qttools5-dev qtsvg5-dev \
    libeigen3-dev libboost-dev libboost-system-dev \
    libgsl-dev zlib1g-dev libexpat1-dev

# liblager (not packaged on Debian — build from source)
git clone --depth 1 https://github.com/arximboldi/lager.git /tmp/lager
cmake -S /tmp/lager -B /tmp/lager/build -DCMAKE_INSTALL_PREFIX=/usr/local
sudo cmake --build /tmp/lager/build --target install -j

# libmypaint (optional, for paintops/mypaint)
sudo apt install -y libmypaint-dev || \
  (git clone --depth 1 https://github.com/mypaint/libmypaint.git /tmp/lmp && \
   cd /tmp/lmp && ./autogen.sh && ./configure --prefix=/usr/local && \
   make -j && sudo make install)
```

## Install commands (macOS / Homebrew)

```bash
brew install qt@5 boost eigen cmake ninja pkg-config zlib
brew install lager  # if available; otherwise build from source as above
brew install libmypaint  # optional
```
