// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// krita_param_map.dart — Parameter map management for the Krita brush
// engine.
//
// The Krita paintop settings surface is a flat list of `(name, value)`
// pairs serialized as `<param name="...">value</param>` entries inside
// the preset XML. The C ABI exposes that list through
// `krita_brush_preset_param_count` / `_name` / `_value` (document order,
// insertion-ordered) and `krita_brush_set_param` records edits with
// REPLACE-OR-APPEND semantics (verified in CI): setting a name that
// already exists overwrites its value in place; setting a new name
// appends at the end of the map.
//
// [KritaParamMap] mirrors that exact semantics on the Dart side by
// extending [MapBase] over an internal [LinkedHashMap] — which natively
// does replace-or-append (`map[k] = v` on an existing key keeps the
// original position; on a new key it appends). Because [KritaParamMap]
// IS-A `Map<String, String>`, existing call sites that expect a plain
// Map keep working unchanged; new code gets the rich [set] method that
// also reports whether the call appended a new entry.
//
// Known parameter names (the ones the C wrapper actually consumes for
// live brush effects) are catalogued in [KritaKnownParams] with their
// value ranges / units, so the UI can render typed editors instead of
// raw strings.

import 'dart:collection';

/// Category of a known Krita paintop-settings parameter. Used by the UI
/// to pick the right editor widget (slider / toggle / dropdown).
enum KritaParamCategory {
  /// Continuous scalar in a fixed range (slider).
  scalar,

  /// Boolean toggle ("true" / "1" maps to on).
  toggle,

  /// Enumerated string from a closed set (e.g. CompositeOp).
  enumeration,

  /// Free-form XML payload (e.g. brush_definition, sensor curves).
  blob,

  /// Unknown / uncurated — render as a raw text field.
  raw,
}

/// Metadata for one known Krita paintop-settings parameter.
class KritaParamSpec {
  /// Constructs a parameter spec.
  const KritaParamSpec({
    required this.name,
    required this.category,
    this.aliases = const <String>[],
    this.min,
    this.max,
    this.unit,
    this.description = '',
  });

  /// Canonical parameter name (the one Krita writes into preset XML).
  final String name;

  /// Aliases the C wrapper also accepts (see krita_bridge.h
  /// `krita_brush_set_param` doc for the full alias list).
  final List<String> aliases;

  /// Editor category for the UI.
  final KritaParamCategory category;

  /// Inclusive minimum value (scalars only).
  final double? min;

  /// Inclusive maximum value (scalars only).
  final double? max;

  /// Human-readable unit label (e.g. "%", "px", "").
  final String? unit;

  /// Short description for tooltips.
  final String description;

  /// All names that refer to this param (canonical + aliases).
  List<String> get allNames => <String>[name, ...aliases];
}

/// Catalogue of the Krita paintop-settings parameters the C wrapper
/// consumes for live brush effects (krita_bridge.h `set_param` doc).
abstract final class KritaKnownParams {
  /// Master brush opacity as a percentage 0..100 (Krita's paintop
  /// settings name). Aliases `OpacityValue` / `opacity` /
  /// `brush_opacity` carry the same value as a 0..1 fraction.
  static const opacity = KritaParamSpec(
    name: 'Krita/opacity',
    aliases: ['OpacityValue', 'opacity', 'brush_opacity'],
    category: KritaParamCategory.scalar,
    min: 0.0,
    max: 100.0,
    unit: '%',
    description: 'Master brush opacity.',
  );

  /// Per-dab flow (build-up rate), 0..1 fraction. Alias `flow`.
  static const flow = KritaParamSpec(
    name: 'FlowValue',
    aliases: ['flow'],
    category: KritaParamCategory.scalar,
    min: 0.0,
    max: 1.0,
    unit: '',
    description: 'Per-dab flow (paint build-up rate).',
  );

  /// Brush hardness, 0..1 (0 = soft gaussian, 1 = hard disk).
  static const hardness = KritaParamSpec(
    name: 'hardness',
    category: KritaParamCategory.scalar,
    min: 0.0,
    max: 1.0,
    unit: '',
    description: 'Brush edge hardness (rebuilds the auto-brush tip).',
  );

  /// Softness = 1 - hardness (complement). Alias `softness`.
  static const softness = KritaParamSpec(
    name: 'SoftnessValue',
    aliases: ['softness'],
    category: KritaParamCategory.scalar,
    min: 0.0,
    max: 1.0,
    unit: '',
    description: 'Tip softness (complement of hardness).',
  );

  /// Dab spacing as a fraction of brush diameter, in (0, 5].
  static const spacing = KritaParamSpec(
    name: 'brush_spacing',
    category: KritaParamCategory.scalar,
    min: 0.0,
    max: 5.0,
    unit: 'x dia',
    description: 'Dab spacing as a fraction of brush diameter.',
  );

  /// Smudge rate, 0..1. Aliases `smudge_rate` / `smudge`.
  static const smudge = KritaParamSpec(
    name: 'SmudgeRateValue',
    aliases: ['smudge_rate', 'smudge'],
    category: KritaParamCategory.scalar,
    min: 0.0,
    max: 1.0,
    unit: '',
    description: 'Smudge rate (how much existing color is picked up).',
  );

  /// Eraser flag. Aliases `Krita/erase` / `EraserMode` / `eraser` carry
  /// "true" / "1"; `CompositeOp` carries the value "erase".
  static const eraser = KritaParamSpec(
    name: 'CompositeOp',
    aliases: ['Krita/erase', 'EraserMode', 'eraser'],
    category: KritaParamCategory.enumeration,
    description: 'Eraser mode (destination-out compositing).',
  );

  /// The full parameter catalogue, in stable display order.
  static const List<KritaParamSpec> all = <KritaParamSpec>[
    opacity,
    flow,
    hardness,
    softness,
    spacing,
    smudge,
    eraser,
  ];

  /// Looks up a param spec by any of its names (canonical or alias).
  /// Returns `null` for uncurated names (the caller should still store
  /// them in the map — unknown names are valid preset entries, the
  /// engine just does not consume them for live brush effects).
  static KritaParamSpec? lookup(String name) {
    for (final spec in all) {
      if (spec.allNames.contains(name)) return spec;
    }
    return null;
  }
}

/// An insertion-ordered map of Krita paintop-settings parameters with
/// REPLACE-OR-APPEND semantics matching the C ABI's
/// `krita_brush_set_param` (verified in CI):
///   * [set] on an EXISTING key overwrites the value in place (keeps
///     the original document-order position);
///   * [set] on a NEW key appends at the end of the map.
///
/// This is the canonical Dart-side view of the loaded preset's option
/// surface. The high-level [KritaBrushController] mirrors the engine's
/// native map into a [KritaParamMap] after every `loadPreset` /
/// `setParam` / `setCurve` call so the UI inspector reads a single
/// consistent snapshot.
///
/// Extends [MapBase] so [KritaParamMap] IS-A `Map<String, String>` —
/// existing call sites that expect a plain Map keep working unchanged,
/// while new code gets the rich [set] method and the [names] /
/// [values] / [entries] iterables (insertion-ordered by the underlying
/// [LinkedHashMap], which natively does replace-or-append).
class KritaParamMap extends MapBase<String, String> {
  /// Constructs an empty param map.
  KritaParamMap();

  /// Constructs a param map pre-populated with [entries], preserving
  /// insertion order. Duplicate keys keep the LAST value (same as a
  /// sequence of [set] calls).
  KritaParamMap.fromEntries(Iterable<MapEntry<String, String>> entries) {
    for (final e in entries) {
      set(e.key, e.value);
    }
  }

  final LinkedHashMap<String, String> _entries = LinkedHashMap<String, String>();

  /// The parameter names in insertion order (canonical names first,
  /// appended names last — exactly the document order the C ABI reports
  /// through `krita_brush_preset_param_name`).
  Iterable<String> get names => _entries.keys;

  /// All `(name, value)` entries in insertion order.
  Iterable<MapEntry<String, String>> get entryList =>
      _entries.entries.map((e) => MapEntry<String, String>(e.key, e.value));

  /// REPLACE-OR-APPEND [name] with [value]. If [name] already exists its
  /// value is overwritten in place (the original document-order position
  /// is preserved); otherwise the pair is appended at the end.
  ///
  /// Returns `true` when the call appended a new entry, `false` when it
  /// replaced an existing one. Mirrors the C ABI's set_param semantics
  /// exactly so the Dart-side mirror and the engine-side map of record
  /// stay in lockstep.
  bool set(String name, String value) {
    if (name.isEmpty) {
      throw ArgumentError.value(name, 'name', 'must not be empty');
    }
    final existed = _entries.containsKey(name);
    _entries[name] = value;
    return !existed;
  }

  /// Returns true when the map contains [name].
  bool contains(String name) => _entries.containsKey(name);

  /// Returns a plain `Map<String, String>` snapshot (insertion-ordered).
  /// Useful when a caller needs a plain Map (e.g. JSON serialization).
  Map<String, String> toMap() => Map<String, String>.from(_entries);

  /// Returns a copy of this map. Edits to the copy do not affect this
  /// map and vice versa.
  KritaParamMap copy() {
    final out = KritaParamMap();
    out._entries.addAll(_entries);
    return out;
  }

  // ----- MapBase primitive operations (delegated to the LinkedHashMap,
  // which natively does replace-or-append). --------------------------

  @override
  String? operator [](Object? key) => _entries[key];

  /// REPLACE-OR-APPEND: setting an existing key keeps its position;
  /// setting a new key appends. Same semantics as [set] but without
  /// the "did it append?" return value (required by the [Map]
  /// contract).
  @override
  void operator []=(String key, String value) {
    if (key.isEmpty) {
      throw ArgumentError.value(key, 'key', 'must not be empty');
    }
    _entries[key] = value;
  }

  @override
  Iterable<String> get keys => _entries.keys;

  @override
  void clear() => _entries.clear();

  @override
  String? remove(Object? key) => _entries.remove(key);

  @override
  int get length => _entries.length;

  @override
  String toString() => 'KritaParamMap(${toMap()})';
}
