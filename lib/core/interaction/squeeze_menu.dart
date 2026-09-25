// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// squeeze_menu.dart — the Feather Squeeze Menu radial interface.
//
// Models the contextual radial palette documented in
// `interfaceandgestures_squeezemenu.txt`. The menu unfolds around the
// last interaction point (or the hover point on devices that support
// it) and offers:
//
//   * A DEFAULT set (always available): Undo, Redo, Find Group.
//   * A CONTEXTUAL set chosen by the active tool:
//       - Draw     → Add New Group, Quick Brush Panel, Recall Recent Guide.
//       - Select   → Select All, Stamp.
//   * The radial layout places items at evenly-spaced angles around the
//     unfold anchor; selection is by angular hit-test from the drag
//     direction (squeeze-drag to pick) or by tap.
//
// This file is pure data + geometry — the actual Flutter overlay widget
// consumes [SqueezeMenuModel] for layout and reports selection back via
// [selectAtAngle].
//
// Depends on: lib/core/math/, lib/core/interaction/input_state.dart

import 'dart:math' as math;

import 'package:feather_krita/core/interaction/input_state.dart';

/// One entry in the Squeeze Menu.
class SqueezeMenuItem {
  const SqueezeMenuItem({
    required this.id,
    required this.label,
    required this.kind,
    this.iconGlyph,
    this.destructive = false,
  });

  /// Stable identifier (used for analytics / key bindings).
  final SqueezeActionKind kind;
  final String id;
  final String label;
  final String? iconGlyph;
  final bool destructive;
}

/// Every action the Squeeze Menu can dispatch.
enum SqueezeActionKind {
  undo,
  redo,
  findGroup,
  addNewGroup,
  quickBrushPanel,
  recallRecentGuide,
  selectAll,
  stamp,
  toggleStableStroke,
  togglePressure,
  injector,
}

/// Which contextual set is shown.
enum SqueezeContext { draw, select, generic }

/// Layout + selection geometry for one menu.
class SqueezeMenuModel {
  SqueezeMenuModel({
    required this.anchorX,
    required this.anchorY,
    required this.context,
    this.radius = 110.0,
    this.innerRadius = 24.0,
  }) {
    _items = _build(context);
  }

  /// Screen-space unfold position (pixels).
  final double anchorX;
  final double anchorY;
  final SqueezeContext context;
  final double radius;
  final double innerRadius;

  late final List<SqueezeMenuItem> _items;
  List<SqueezeMenuItem> get items => _items;

  List<SqueezeMenuItem> _build(SqueezeContext ctx) {
    switch (ctx) {
      case SqueezeContext.draw:
        return const [
          SqueezeMenuItem(
              id: 'undo', label: 'Undo', kind: SqueezeActionKind.undo),
          SqueezeMenuItem(
              id: 'redo', label: 'Redo', kind: SqueezeActionKind.redo),
          SqueezeMenuItem(
              id: 'find',
              label: 'Find Group',
              kind: SqueezeActionKind.findGroup),
          SqueezeMenuItem(
              id: 'addgrp',
              label: 'Add Group',
              kind: SqueezeActionKind.addNewGroup),
          SqueezeMenuItem(
              id: 'brush',
              label: 'Brush Panel',
              kind: SqueezeActionKind.quickBrushPanel),
          SqueezeMenuItem(
              id: 'recall',
              label: 'Recall Guide',
              kind: SqueezeActionKind.recallRecentGuide),
        ];
      case SqueezeContext.select:
        return const [
          SqueezeMenuItem(
              id: 'undo', label: 'Undo', kind: SqueezeActionKind.undo),
          SqueezeMenuItem(
              id: 'redo', label: 'Redo', kind: SqueezeActionKind.redo),
          SqueezeMenuItem(
              id: 'find',
              label: 'Find Group',
              kind: SqueezeActionKind.findGroup),
          SqueezeMenuItem(
              id: 'selall',
              label: 'Select All',
              kind: SqueezeActionKind.selectAll),
          SqueezeMenuItem(
              id: 'stamp', label: 'Stamp', kind: SqueezeActionKind.stamp),
        ];
      case SqueezeContext.generic:
        return const [
          SqueezeMenuItem(
              id: 'undo', label: 'Undo', kind: SqueezeActionKind.undo),
          SqueezeMenuItem(
              id: 'redo', label: 'Redo', kind: SqueezeActionKind.redo),
          SqueezeMenuItem(
              id: 'find',
              label: 'Find Group',
              kind: SqueezeActionKind.findGroup),
        ];
    }
  }

  /// The angle (radians, 0 = right, CCW positive) at which [index] is
  /// laid out. Items are spread starting from the top (-π/2) and going
  /// clockwise so the first item sits directly above the anchor.
  double angleFor(int index) {
    final n = _items.length;
    if (n == 0) return 0;
    final step = 2 * math.pi / n;
    return -math.pi / 2 + index * step;
  }

  /// Pixel position of [index].
  ({double x, double y}) positionFor(int index) {
    final a = angleFor(index);
    return (
      x: anchorX + radius * math.cos(a),
      y: anchorY + radius * math.sin(a),
    );
  }

  /// Hit-test a drag direction (relative to the anchor) and return the
  /// nearest item, or `null` when the drag is shorter than [innerRadius]
  /// (a "cancel" gesture).
  SqueezeMenuItem? selectAtAngle(double dx, double dy) {
    final dist = math.sqrt(dx * dx + dy * dy);
    if (dist < innerRadius) return null;
    final a = math.atan2(dy, dx);
    var best = 0;
    var bestDelta = double.infinity;
    for (var i = 0; i < _items.length; i++) {
      var d = (a - angleFor(i)).abs();
      if (d > math.pi) d = 2 * math.pi - d;
      if (d < bestDelta) {
        bestDelta = d;
        best = i;
      }
    }
    return _items[best];
  }

  /// Pick the appropriate context from the active tool.
  static SqueezeContext contextForTool(FeatherTool tool) {
    switch (tool) {
      case FeatherTool.draw:
      case FeatherTool.drawShape:
        return SqueezeContext.draw;
      case FeatherTool.select:
      case FeatherTool.deselect:
        return SqueezeContext.select;
      case FeatherTool.erase:
      case FeatherTool.vacuum:
        return SqueezeContext.generic;
    }
  }
}

/// Controller for the open/closed lifecycle of the Squeeze Menu.
class SqueezeMenuController {
  SqueezeMenuController();

  SqueezeMenuModel? _model;
  SqueezeMenuModel? get model => _model;
  bool get isOpen => _model != null;

  /// Open at [x]/[y] for the given [context].
  void open(double x, double y, SqueezeContext context) {
    _model = SqueezeMenuModel(anchorX: x, anchorY: y, context: context);
  }

  /// Open at [x]/[y], inferring the context from [tool].
  void openForTool(double x, double y, FeatherTool tool) {
    open(x, y, SqueezeMenuModel.contextForTool(tool));
  }

  /// Close without firing an action.
  void close() {
    _model = null;
  }

  /// Hit-test a drag/tap position relative to the open menu's anchor and
  /// return the selected item kind (and close). Returns `null` if the
  /// gesture cancelled (short drag inside the inner radius).
  SqueezeActionKind? commit(double dx, double dy) {
    final m = _model;
    if (m == null) return null;
    final item = m.selectAtAngle(dx, dy);
    close();
    return item?.kind;
  }
}
