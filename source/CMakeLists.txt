# SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
# SPDX-License-Identifier: GPL-2.0-or-later
#
# CMakeLists.txt — Build C++ GDExtension library for Feather-Krita app
#
# Output:
#   - Windows: feather_krita.dll
#   - Linux:   libfeather_krita.so
#   - Android: libfeather_krita.so (ARM64)
#
# Links against:
#   - Qt6 (Core, Gui, Widgets)
#   - Krita brush engine (libkritalibbrush) — 100% asli KDE, TIDAK DIUBAH
#   - godot-cpp (GDExtension headers)
#
# Usage:
#   mkdir build && cd build
#   cmake .. -DCMAKE_BUILD_TYPE=Release
#   cmake --build . --config Release
#
# Set these variables (via -D or environment):
#   - KRITA_SOURCE_DIR  : path ke krita-source/ (berisi libs/brush/)
#   - GODOT_CPP_DIR     : path ke godot-cpp/ (cloned dari github.com/godotengine/godot-cpp)
#   - Qt6_DIR           : path ke Qt6 installation (otomatis via qmake/cmake)
#
# Example:
#   cmake .. \
#     -DKRITA_SOURCE_DIR=/path/to/krita-source \
#     -DGODOT_CPP_DIR=/path/to/godot-cpp \
#     -DQt6_DIR=/path/to/Qt/6.7.0/gcc_64/lib/cmake/Qt6

cmake_minimum_required(VERSION 3.22)
project(feather_krita LANGUAGES CXX)

# ---------------------------------------------------------------------------
# C++ standard
# ---------------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# ---------------------------------------------------------------------------
# Build options
# ---------------------------------------------------------------------------
option(FEATHER_BUILD_TESTS    "Build test executables" OFF)
option(FEATHER_STATIC_KRITA   "Statically link Krita brush engine" OFF)
option(FEATHER_VERBOSE_MAKE   "Verbose makefile" OFF)

if(FEATHER_VERBOSE_MAKE)
    set(CMAKE_VERBOSE_MAKEFILE ON)
endif()

# Output dirs (so .dll/.so ends up easy to find)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Position-independent code (required for shared lib)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# ---------------------------------------------------------------------------
# Required external paths
# ---------------------------------------------------------------------------
if(NOT DEFINED KRITA_SOURCE_DIR)
    set(KRITA_SOURCE_DIR "$ENV{KRITA_SOURCE_DIR}" CACHE PATH "Path to krita-source/")
endif()
if(NOT DEFINED GODOT_CPP_DIR)
    set(GODOT_CPP_DIR "$ENV{GODOT_CPP_DIR}" CACHE PATH "Path to godot-cpp/")
endif()

if(NOT KRITA_SOURCE_DIR OR NOT EXISTS "${KRITA_SOURCE_DIR}/libs/brush")
    message(WARNING
        "KRITA_SOURCE_DIR not set or invalid (='${KRITA_SOURCE_DIR}').\n"
        "Expected to contain libs/brush/ (Krita brush engine).\n"
        "Set -DKRITA_SOURCE_DIR=/path/to/krita-source")
endif()

if(NOT GODOT_CPP_DIR OR NOT EXISTS "${GODOT_CPP_DIR}/gdextension")
    message(WARNING
        "GODOT_CPP_DIR not set or invalid (='${GODOT_CPP_DIR}').\n"
        "Expected to contain gdextension/ interface headers.\n"
        "Set -DGODOT_CPP_DIR=/path/to/godot-cpp")
endif()

# ---------------------------------------------------------------------------
# Qt6
# ---------------------------------------------------------------------------
find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets Network)
qt_standard_project_setup()

message(STATUS "Qt6 found: ${Qt6_DIR}")
message(STATUS "  Version: ${Qt6_VERSION}")

# Automatically handle moc/uuid/uic
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# ---------------------------------------------------------------------------
# godot-cpp
# ---------------------------------------------------------------------------
# godot-cpp is built as a separate static library (libgodot-cpp.a).
# Either provide a pre-built one via GODOT_CPP_BUILD_DIR, or add its
# sources as a subdirectory.

if(EXISTS "${GODOT_CPP_DIR}/CMakeLists.txt" AND NOT TARGET godot-cpp::godot-cpp)
    # Add godot-cpp as subdirectory (it builds libgodot-cpp)
    if(NOT EXISTS "${GODOT_CPP_DIR}/build")
        message(STATUS "Configuring godot-cpp as subdirectory...")
        # Use the following to pre-build godot-cpp:
        #   cd godot-cpp && scons platform=windows target=template_release
        # We use scons-built static lib if available; otherwise rely on
        # the user having run `scons` already.
    endif()
endif()

# Find the godot-cpp static library (built via scons)
set(GODOT_CPP_LIB_DIR "${GODOT_CPP_DIR}/bin" CACHE PATH "godot-cpp bin/ directory")
set(_GODOT_CPP_PLATFORM "")
if(WIN32)
    set(_GODOT_CPP_PLATFORM "windows")
elseif(ANDROID)
    set(_GODOT_CPP_PLATFORM "android")
elseif(APPLE)
    set(_GODOT_CPP_PLATFORM "macos")
else()
    set(_GODOT_CPP_PLATFORM "linux")
endif()

# Try to find the prebuilt godot-cpp static lib
find_library(GODOT_CPP_LIBRARY
    NAMES godot-cpp
    PATHS "${GODOT_CPP_LIB_DIR}"
    NO_DEFAULT_PATH
    DOC "godot-cpp static library (build via scons in godot-cpp/)")

if(GODOT_CPP_LIBRARY)
    message(STATUS "Found godot-cpp library: ${GODOT_CPP_LIBRARY}")
    add_library(godot_cpp STATIC IMPORTED)
    set_target_properties(godot_cpp PROPERTIES
        IMPORTED_LOCATION "${GODOT_CPP_LIBRARY}"
        INTERFACE_INCLUDE_DIRECTORIES "${GODOT_CPP_DIR}/gdextension"
                                       "${GODOT_CPP_DIR}/include")
else()
    message(WARNING
        "godot-cpp library not found in ${GODOT_CPP_LIB_DIR}.\n"
        "Build it first:\n"
        "  cd ${GODOT_CPP_DIR}\n"
        "  scons platform=${_GODOT_CPP_PLATFORM} target=template_release\n"
        "Continuing with header-only include path.")
    add_library(godot_cpp INTERFACE)
    target_include_directories(godot_cpp INTERFACE
        "${GODOT_CPP_DIR}/gdextension"
        "${GODOT_CPP_DIR}/include")
endif()

# ---------------------------------------------------------------------------
# Krita brush engine (libkritalibbrush) — 100% asli, TIDAK DIUBAH
# ---------------------------------------------------------------------------
# Krita brush engine source ada di:
#   ${KRITA_SOURCE_DIR}/libs/brush/
#   ${KRITA_SOURCE_DIR}/libs/image/brushengine/
#   ${KRITA_SOURCE_DIR}/libs/pigment/
#   ${KRITA_SOURCE_DIR}/libs/resources/
#
# Asumsi: krita-source/ sudah di-compile (mis. via KDE build system),
# dan libkritalibbrush.so/.dll sudah tersedia.
# Atau, jika KRITA_BUILD_DIR diset, kita link ke sana.

if(NOT DEFINED KRITA_BUILD_DIR)
    set(KRITA_BUILD_DIR "$ENV{KRITA_BUILD_DIR}" CACHE PATH "Krita build directory")
endif()

# Try to find prebuilt Krita brush library
find_library(KRITA_BRUSH_LIBRARY
    NAMES kritalibbrush kritalibbrush-16
    PATHS "${KRITA_BUILD_DIR}/lib"
          "${KRITA_BUILD_DIR}/bin"
          "/usr/lib"
          "/usr/lib/x86_64-linux-gnu"
    DOC "Krita brush engine library (libkritalibbrush.so/.dll)")

# Other Krita libs needed
find_library(KRITA_IMAGE_LIBRARY   NAMES kritaimage)
find_library(KRITA_PIGMENT_LIBRARY NAMES kritapigment)
find_library(KRITA_RESOURCES_LIBRARY NAMES kritaresources)

# Include directories for Krita headers
set(KRITA_INCLUDE_DIRS
    "${KRITA_SOURCE_DIR}/libs/brush"
    "${KRITA_SOURCE_DIR}/libs/brush/krita_paintop"
    "${KRITA_SOURCE_DIR}/libs/image"
    "${KRITA_SOURCE_DIR}/libs/image/brushengine"
    "${KRITA_SOURCE_DIR}/libs/pigment"
    "${KRITA_SOURCE_DIR}/libs/resources"
    "${KRITA_SOURCE_DIR}/libs/global"
    "${KRITA_SOURCE_DIR}/libs/widgetutils"
    "${KRITA_SOURCE_DIR}/libs/version"
    "${KRITA_SOURCE_DIR}"
    "${KRITA_BUILD_DIR}/libs/brush"      # generated kis_brush_export.h etc.
    "${KRITA_BUILD_DIR}/libs/image"
    "${KRITA_BUILD_DIR}/libs/pigment"
    "${KRITA_BUILD_DIR}"
)

# ---------------------------------------------------------------------------
# Source files
# ---------------------------------------------------------------------------
set(FEATHER_SOURCES
    src/guide_surface/guide_surface.cpp
    src/stroke_manager/stroke_manager.cpp
    src/export/export_lock.cpp
    src/krita_bridge/krita_brush_wrapper.cpp
    src/krita_bridge/texture_painter.cpp
    src/gdextension_entry.cpp
)

set(FEATHER_HEADERS
    src/guide_surface/guide_surface.h
    src/stroke_manager/stroke_manager.h
    src/export/export_lock.h
    src/krita_bridge/krita_brush_wrapper.h
    src/krita_bridge/texture_painter.h
)

# ---------------------------------------------------------------------------
# GDExtension shared library
# ---------------------------------------------------------------------------
# PENTING: GDExtension library HARUS punya nama yang sama dengan yang
# direferensi di .gdextension file (feather_krita.windows.template_release.dll
# atau libfeather_krita.linux.template_release.so).

if(WIN32)
    set(FEATHER_LIB_NAME "feather_krita")
elseif(ANDROID)
    set(FEATHER_LIB_NAME "feather_krita")
else()
    set(FEATHER_LIB_NAME "feather_krita")
endif()

add_library(${FEATHER_LIB_NAME} SHARED
    ${FEATHER_SOURCES}
    ${FEATHER_HEADERS}
)

target_include_directories(${FEATHER_LIB_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${KRITA_INCLUDE_DIRS}
    ${GODOT_CPP_DIR}/gdextension
    ${GODOT_CPP_DIR}/include
)

# Qt6 link
target_link_libraries(${FEATHER_LIB_NAME} PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    Qt6::Network
)

# godot-cpp link
target_link_libraries(${FEATHER_LIB_NAME} PRIVATE godot_cpp)

# Krita brush engine link (if found)
if(KRITA_BRUSH_LIBRARY)
    target_link_libraries(${FEATHER_LIB_NAME} PRIVATE ${KRITA_BRUSH_LIBRARY})
    message(STATUS "Linking Krita brush: ${KRITA_BRUSH_LIBRARY}")
else()
    message(WARNING
        "Krita brush library not found. Set KRITA_BUILD_DIR.\n"
        "Build will succeed but link errors may occur for Krita symbols.")
endif()

if(KRITA_IMAGE_LIBRARY)
    target_link_libraries(${FEATHER_LIB_NAME} PRIVATE ${KRITA_IMAGE_LIBRARY})
endif()
if(KRITA_PIGMENT_LIBRARY)
    target_link_libraries(${FEATHER_LIB_NAME} PRIVATE ${KRITA_PIGMENT_LIBRARY})
endif()
if(KRITA_RESOURCES_LIBRARY)
    target_link_libraries(${FEATHER_LIB_NAME} PRIVATE ${KRITA_RESOURCES_LIBRARY})
endif()

# ---------------------------------------------------------------------------
# Platform-specific settings
# ---------------------------------------------------------------------------
if(WIN32)
    target_compile_definitions(${FEATHER_LIB_NAME} PRIVATE
        UNICODE _UNICODE
        NOMINMAX
        WIN32_LEAN_AND_MEAN
    )
    # Windows: .dll needs to export the gdextension_initialize function
    set_target_properties(${FEATHER_LIB_NAME} PROPERTIES
        PREFIX ""
        OUTPUT_NAME "feather_krita"
        SUFFIX ".dll"
    )
elseif(ANDROID)
    target_compile_definitions(${FEATHER_LIB_NAME} PRIVATE
        __ANDROID__
    )
    set_target_properties(${FEATHER_LIB_NAME} PROPERTIES
        PREFIX "lib"
        OUTPUT_NAME "feather_krita"
        SUFFIX ".so"
    )
else()
    # Linux
    set_target_properties(${FEATHER_LIB_NAME} PROPERTIES
        PREFIX "lib"
        OUTPUT_NAME "feather_krita"
        SUFFIX ".so"
    )
endif()

# C++ defines
target_compile_definitions(${FEATHER_LIB_NAME} PRIVATE
    FEATHER_KRITA_BUILD
)

# Optimization flags
if(CMAKE_BUILD_TYPE STREQUAL "Release")
    target_compile_options(${FEATHER_LIB_NAME} PRIVATE
        -O2
        $<$<CXX_COMPILER_ID:GNU,Clang>:-ffast-math>
        $<$<CXX_COMPILER_ID:GNU,Clang>:-fvisibility=hidden>
        $<$<CXX_COMPILER_ID:GNU,Clang>:-fvisibility-inlines-hidden>
    )
endif()

# Warnings
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(${FEATHER_LIB_NAME} PRIVATE
        -Wall -Wextra -Wpedantic
        -Wno-unused-parameter
    )
elseif(MSVC)
    target_compile_options(${FEATHER_LIB_NAME} PRIVATE /W3 /permissive-)
endif()

# ---------------------------------------------------------------------------
# GDExtension metadata file
# ---------------------------------------------------------------------------
# Generate the .gdextension file that Godot reads to load the extension.
set(GDEXTENSION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/feather_krita.gdextension")

# Determine relative path from project.godot to .gdextension file
file(WRITE ${GDEXTENSION_FILE}
"[configuration]
entry_symbol = \"gdextension_initialize\"
compatibility_minimum = true

[libraries]
")

if(WIN32)
    file(APPEND ${GDEXTENSION_FILE}
"windows.debug.x86_64 = \"res://bin/feather_krita.windows.debug.x86_64.dll\"
windows.release.x86_64 = \"res://bin/feather_krita.windows.release.x86_64.dll\"
")
elseif(ANDROID)
    file(APPEND ${GDEXTENSION_FILE}
"android.debug.arm64 = \"res://bin/libfeather_krita.android.debug.arm64.so\"
android.release.arm64 = \"res://bin/libfeather_krita.android.release.arm64.so\"
")
else()
    file(APPEND ${GDEXTENSION_FILE}
"linux.debug.x86_64 = \"res://bin/libfeather_krita.linux.debug.x86_64.so\"
linux.release.x86_64 = \"res://bin/libfeather_krita.linux.release.x86_64.so\"
")
endif()

# ---------------------------------------------------------------------------
# Install rules
# ---------------------------------------------------------------------------
include(GNUInstallDirs)

install(TARGETS ${FEATHER_LIB_NAME}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(FILES ${GDEXTENSION_FILE}
    DESTINATION ${CMAKE_INSTALL_BINDIR}
)

# ---------------------------------------------------------------------------
# Tests
# ---------------------------------------------------------------------------
if(FEATHER_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

# ---------------------------------------------------------------------------
# Status summary
# ---------------------------------------------------------------------------
message(STATUS "")
message(STATUS "================ Feather-Krita Build Config ================")
message(STATUS "  Library name        : ${FEATHER_LIB_NAME}")
message(STATUS "  Build type          : ${CMAKE_BUILD_TYPE}")
message(STATUS "  Krita source dir    : ${KRITA_SOURCE_DIR}")
message(STATUS "  Krita build dir     : ${KRITA_BUILD_DIR}")
message(STATUS "  godot-cpp dir       : ${GODOT_CPP_DIR}")
message(STATUS "  Qt6 version         : ${Qt6_VERSION}")
message(STATUS "  Platform            : ${CMAKE_SYSTEM_NAME}")
if(ANDROID)
    message(STATUS "  Android ABI         : ${ANDROID_ABI}")
endif()
message(STATUS "============================================================")
message(STATUS "")
