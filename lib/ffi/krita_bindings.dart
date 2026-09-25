// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// krita_bindings.dart — Backward-compatibility re-export shim.
//
// The FFI bindings and the high-level brush engine used to live in this
// file. They have been split into a low-level bindings layer
// (`lib/engine/krita_bridge/krita_bindings.dart`) and a high-level
// controller layer (`lib/engine/krita_bridge/krita_brush_controller.dart`)
// plus a full engine module (preset loader, canvas controller, dab
// renderer, fallback engine, engine lifecycle, smoke test) under
// `lib/engine/krita_bridge/`.
//
// This file is kept ONLY to preserve the historical import path
// `package:feather_krita/ffi/krita_bindings.dart` so existing call
// sites keep compiling without a flag-day rewrite. New code should
// import from `package:feather_krita/engine/krita_bridge/...` directly.
//
// Everything that was publicly exported from the old file — the FFI
// struct mirrors (BrushInputNative, BrushDabNative), the Dart value
// types (BrushInput, BrushDab, BrushColor, BrushInputFlags,
// KritaPresetInfo), the CurveEditStatus enum, the high-level
// KritaBrushEngine class (now a typedef for KritaBrushController) — is
// re-exported here unchanged.

export '../engine/krita_bridge/krita_bindings.dart';
export '../engine/krita_bridge/krita_brush_controller.dart' show KritaBrushEngine;
