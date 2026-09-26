// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// stroke_list_panel.dart — Feather 3D-style 3D strokes panel.
//
// Feather 3D shows a list of 3D strokes (not just 2D layers) with per-
// stroke controls: name, material icon, visibility toggle, delete, and
// a drag handle to reorder. This panel is the user-facing surface for
// that list — a self-contained StatefulWidget that takes a snapshot of
// the document's strokes (via [StrokeListItem] value objects) + emits
// callbacks the host wires to its canonical [_strokes] list.
//
// Visual language: rose/pink gradient header (matches GuidePanel +
// AssistPanel + RenderModeToggle + LightRigPanel family). Reorder
// uses Flutter's [ReorderableListView] so the drag handle + reorder
// animation come for free.

import 'package:flutter/material.dart';

import '../theme/feather_colors.dart';
import '../theme/feather_typography.dart';
import '../theme/glassmorphism.dart';
import 'glass_panel.dart';
import 'material_picker.dart' show FeatherMaterial, FeatherMaterialX;

/// One row in the 3D strokes list. Plain value object the host builds
/// from its canonical [_strokes] list + the per-stroke material map.
class StrokeListItem {
  const StrokeListItem({
    required this.id,
    required this.name,
    required this.color,
    required this.material,
    required this.isVisible,
    required this.sampleCount,
  });

  /// Stroke id (matches the host's [_strokes] id space).
  final int id;

  /// Display name ("Stroke 1", "Stroke 2", ...).
  final String name;

  /// Stroke color (ARGB int → Color for the swatch).
  final int color;

  /// Per-stroke material (drives the icon + label).
  final FeatherMaterial material;

  /// Whether the stroke is currently visible.
  final bool isVisible;

  /// Number of captured samples (Stroke3D length) — shown as the
  /// secondary label.
  final int sampleCount;
}

/// Feather 3D-style 3D strokes panel. Self-contained stateful widget;
/// the host forwards a snapshot of its [_strokes] list + receives
/// callbacks for visibility / delete / reorder.
class StrokeListPanel extends StatefulWidget {
  const StrokeListPanel({
    super.key,
    this.strokes = const <StrokeListItem>[],
    this.onToggleVisible,
    this.onDelete,
    this.onReorder,
    this.onClose,
  });

  final List<StrokeListItem> strokes;
  final ValueChanged<int>? onToggleVisible;
  final ValueChanged<int>? onDelete;
  final void Function(int oldIndex, int newIndex)? onReorder;
  final VoidCallback? onClose;

  @override
  State<StrokeListPanel> createState() => _StrokeListPanelState();
}

class _StrokeListPanelState extends State<StrokeListPanel> {
  /// The rose/pink gradient used by the panel header — matches the
  /// GuidePanel + AssistPanel family.
  static const List<Color> _roseGradient = <Color>[
    FeatherPalette.accentPink,
    FeatherPalette.accentPurple,
    FeatherPalette.accentOrange,
  ];

  static const double _panelWidth = 288.0;

  @override
  Widget build(BuildContext context) {
    final palette = _palette(context);
    return GlassPanel(
      spec: GlassSpec.panel,
      width: _panelWidth,
      padding: EdgeInsets.zero,
      child: Column(
        mainAxisSize: MainAxisSize.min,
        crossAxisAlignment: CrossAxisAlignment.stretch,
        children: [
          _buildHeader(palette),
          if (widget.strokes.isEmpty)
            Padding(
              padding: const EdgeInsets.fromLTRB(16, 18, 16, 18),
              child: Text(
                'No 3D strokes yet. Tap the canvas to draw one.',
                style: FeatherTypography.caption
                    .copyWith(color: palette.textTertiary),
              ),
            )
          else
            ConstrainedBox(
              constraints: const BoxConstraints(maxHeight: 360),
              child: ReorderableListView.builder(
                buildDefaultDragHandles: false,
                padding: const EdgeInsets.fromLTRB(8, 8, 8, 8),
                itemCount: widget.strokes.length,
                onReorder: (oldIndex, newIndex) {
                  // ReorderableListView's contract: if oldIndex < newIndex,
                  // the target is newIndex - 1 (the list grows by one
                  // above the old slot before the move lands). Normalise
                  // so the host sees a straight swap.
                  final norm = newIndex > oldIndex ? newIndex - 1 : newIndex;
                  widget.onReorder?.call(oldIndex, norm);
                },
                proxyDecorator: (child, _, __) => child,
                itemBuilder: (context, i) {
                  final s = widget.strokes[i];
                  return _StrokeRow(
                    key: ValueKey<int>(s.id),
                    item: s,
                    palette: palette,
                    onToggleVisible: () =>
                        widget.onToggleVisible?.call(s.id),
                    onDelete: () => widget.onDelete?.call(s.id),
                  );
                },
              ),
            ),
        ],
      ),
    );
  }

  // ----- Header ---------------------------------------------------------

  Widget _buildHeader(FeatherPalette palette) {
    return Container(
      padding: const EdgeInsets.symmetric(horizontal: 14, vertical: 12),
      decoration: BoxDecoration(
        borderRadius: const BorderRadius.vertical(top: Radius.circular(17)),
        gradient: const LinearGradient(
          begin: Alignment.centerLeft,
          end: Alignment.centerRight,
          colors: _roseGradient,
        ),
      ),
      child: Row(
        children: [
          const Icon(Icons.timeline_rounded, color: Colors.white, size: 18),
          const SizedBox(width: 8),
          Text(
            '3D Strokes',
            style: FeatherTypography.h2.copyWith(color: Colors.white),
          ),
          const Spacer(),
          _CloseButton(onTap: widget.onClose),
        ],
      ),
    );
  }

  FeatherPalette _palette(BuildContext context) =>
      Theme.of(context).brightness == Brightness.dark
          ? FeatherColors.dark
          : FeatherColors.light;
}

class _StrokeRow extends StatelessWidget {
  const _StrokeRow({
    super.key,
    required this.item,
    required this.palette,
    required this.onToggleVisible,
    required this.onDelete,
  });

  final StrokeListItem item;
  final FeatherPalette palette;
  final VoidCallback onToggleVisible;
  final VoidCallback onDelete;

  @override
  Widget build(BuildContext context) {
    return Padding(
      padding: const EdgeInsets.symmetric(vertical: 3),
      child: Container(
        decoration: BoxDecoration(
          color: palette.panelFill,
          borderRadius: BorderRadius.circular(10),
          border: Border.all(color: palette.panelBorder),
        ),
        padding: const EdgeInsets.symmetric(horizontal: 8, vertical: 6),
        child: Row(
          children: [
            // Drag handle (ReorderableListView's drag trigger).
            ReorderableDragStartListener(
              index: 0,
              child: Padding(
                padding: const EdgeInsets.only(right: 6),
                child: Icon(
                  Icons.drag_indicator_rounded,
                  size: 18,
                  color: palette.textTertiary,
                ),
              ),
            ),
            // Color swatch (the stroke's own colour).
            Container(
              width: 14,
              height: 14,
              decoration: BoxDecoration(
                color: Color(item.color),
                shape: BoxShape.circle,
                border: Border.all(
                  color: palette.textPrimary.withValues(alpha: 0.18),
                  width: 0.6,
                ),
              ),
            ),
            const SizedBox(width: 8),
            // Material icon (per-stroke material).
            Icon(
              item.material.icon,
              size: 16,
              color: palette.textSecondary,
            ),
            const SizedBox(width: 8),
            // Name + sample count.
            Expanded(
              child: Column(
                crossAxisAlignment: CrossAxisAlignment.start,
                mainAxisSize: MainAxisSize.min,
                children: [
                  Text(
                    item.name,
                    style: FeatherTypography.caption.copyWith(
                      color: palette.textPrimary,
                      fontWeight: FontWeight.w600,
                    ),
                  ),
                  Text(
                    '${item.material.label} · ${item.sampleCount} pts',
                    style: FeatherTypography.micro
                        .copyWith(color: palette.textTertiary),
                  ),
                ],
              ),
            ),
            // Visibility toggle.
            GestureDetector(
              behavior: HitTestBehavior.opaque,
              onTap: onToggleVisible,
              child: Padding(
                padding: const EdgeInsets.symmetric(horizontal: 4),
                child: Icon(
                  item.isVisible
                      ? Icons.visibility_outlined
                      : Icons.visibility_off_outlined,
                  size: 16,
                  color: item.isVisible
                      ? palette.textSecondary
                      : palette.textTertiary,
                ),
              ),
            ),
            // Delete button.
            GestureDetector(
              behavior: HitTestBehavior.opaque,
              onTap: onDelete,
              child: Padding(
                padding: const EdgeInsets.only(left: 4),
                child: Icon(
                  Icons.delete_outline_rounded,
                  size: 16,
                  color: FeatherColors.toolErase.withValues(alpha: 0.8),
                ),
              ),
            ),
          ],
        ),
      ),
    );
  }
}

/// A small circular close button used in the panel header.
class _CloseButton extends StatelessWidget {
  const _CloseButton({this.onTap});

  final VoidCallback? onTap;

  @override
  Widget build(BuildContext context) {
    return GestureDetector(
      onTap: onTap,
      child: Container(
        width: 26,
        height: 26,
        decoration: BoxDecoration(
          color: Colors.white.withValues(alpha: 0.18),
          shape: BoxShape.circle,
        ),
        child: const Icon(Icons.close_rounded,
            color: Colors.white, size: 16),
      ),
    );
  }
}
