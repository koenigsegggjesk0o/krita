// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// guide_panel.dart — Feather 3D Guide mode switcher panel.
//
// Feather 3D's signature feature is the four guide-creation modes: Draw,
// Loft, Bend and Primitives. The engine layer (lib/engine/guide3d/) ships
// full builders for every mode (DrawnGuideBuilder, LoftedGuideBuilder,
// BentGuideBuilder, PrimitiveGuideBuilder), but the user-facing UI to
// switch between them was missing — only a minimal in-canvas launcher
// row lived in main_screen.dart.
//
// This widget is the full panel. It is a self-contained StatefulWidget
// (the host only needs to forward a handful of callbacks) that surfaces:
//
//   * Mode picker        — segmented button (Draw / Loft / Bend / Primitives).
//   * Primitives section — shape chooser (cube / pyramid / sphere / tube)
//                          + size slider.
//   * Loft section       — axis chooser (X / Y / Z) + segment count slider.
//   * Bend section       — axis chooser + bend angle slider.
//   * Snap-to-guide      — toggle.
//   * Show guide ribbon  — toggle.
//
// Visual language matches the rest of the Feather-Krita chrome:
//   * Material 3 + dark theme (this widget reads `Theme.of(context)` and
//     falls back to the dark [FeatherColors] palette).
//   * NO indigo / blue primary — the header uses the rose/pink gradient
//     [accentPink → accentPurple → accentOrange] so the panel reads as
//     part of the Feather tool family (erase/mirror/bend) rather than
//     the default Material blue.
//   * Glassmorphism body via [GlassPanel] + [GlassSpec.panel].
//
// The panel is intentionally framework-agnostic — no Riverpod, no
// ChangeNotifier. The host (main_screen.dart) owns the canonical state
// and routes every callback to its existing `setGuideMode` /
// `insertPrimitive` / `setLoftTension` / `setPrimitiveSegments` setters
// plus two new boolean fields for snap + ribbon visibility.

import 'package:flutter/material.dart';

import '../../engine/guide3d/guide3d_primitive.dart';
import '../../engine/guide3d/guide3d_type.dart';
import '../theme/feather_colors.dart';
import '../theme/feather_typography.dart';
import '../theme/glassmorphism.dart';
import 'glass_panel.dart';

/// The world-space axis a Loft or Bend operation runs along.
///
/// Surfaced by [GuidePanel.onLoftAxisChanged] /
/// [GuidePanel.onBendAxisChanged]. The engine builders don't yet consume
/// an explicit axis (they infer it from the bend path / curve ordering),
/// but exposing it here future-proofs the panel for the F2 milestone
/// (Dart session bindings) without requiring a host refactor today.
enum GuideAxis { x, y, z }

/// Self-contained Feather 3D Guide mode switcher panel.
///
/// See the file doc for the visual + behavioural contract. The panel is
/// stateful: it owns the live values of every slider / toggle so the UI
/// stays responsive between callback dispatches. The host is notified of
/// every change through the `on*Changed` callbacks.
class GuidePanel extends StatefulWidget {
  const GuidePanel({
    super.key,
    this.initialMode = Guide3DType.drawn,
    this.initialPrimitiveKind = Guide3DPrimitive.cube,
    this.initialPrimitiveSize = 2.0,
    this.initialLoftAxis = GuideAxis.y,
    this.initialLoftSegments = 32,
    this.initialBendAngle = 90.0,
    this.initialBendAxis = GuideAxis.y,
    this.initialSnap = false,
    this.initialRibbon = true,
    this.onModeChanged,
    this.onPrimitiveKindChanged,
    this.onPrimitiveSizeChanged,
    this.onLoftAxisChanged,
    this.onLoftSegmentsChanged,
    this.onBendAngleChanged,
    this.onBendAxisChanged,
    this.onSnapChanged,
    this.onRibbonChanged,
    this.onClose,
  });

  // ----- Initial values (one-shot, read in initState) -------------------
  final Guide3DType initialMode;
  final Guide3DPrimitive initialPrimitiveKind;
  final double initialPrimitiveSize;
  final GuideAxis initialLoftAxis;
  final int initialLoftSegments;
  final double initialBendAngle;
  final GuideAxis initialBendAxis;
  final bool initialSnap;
  final bool initialRibbon;

  // ----- Change callbacks ----------------------------------------------
  final ValueChanged<Guide3DType>? onModeChanged;
  final ValueChanged<Guide3DPrimitive>? onPrimitiveKindChanged;
  final ValueChanged<double>? onPrimitiveSizeChanged;
  final ValueChanged<GuideAxis>? onLoftAxisChanged;
  final ValueChanged<int>? onLoftSegmentsChanged;
  final ValueChanged<double>? onBendAngleChanged;
  final ValueChanged<GuideAxis>? onBendAxisChanged;
  final ValueChanged<bool>? onSnapChanged;
  final ValueChanged<bool>? onRibbonChanged;

  /// Fired when the user taps the panel's close button. The host typically
  /// hides the panel in response.
  final VoidCallback? onClose;

  @override
  State<GuidePanel> createState() => _GuidePanelState();
}

class _GuidePanelState extends State<GuidePanel> {
  late Guide3DType _mode;
  late Guide3DPrimitive _primitiveKind;
  late double _primitiveSize;
  late GuideAxis _loftAxis;
  late int _loftSegments;
  late double _bendAngle;
  late GuideAxis _bendAxis;
  late bool _snap;
  late bool _ribbon;

  @override
  void initState() {
    super.initState();
    _mode = widget.initialMode;
    _primitiveKind = widget.initialPrimitiveKind;
    _primitiveSize = widget.initialPrimitiveSize;
    _loftAxis = widget.initialLoftAxis;
    _loftSegments = widget.initialLoftSegments;
    _bendAngle = widget.initialBendAngle;
    _bendAxis = widget.initialBendAxis;
    _snap = widget.initialSnap;
    _ribbon = widget.initialRibbon;
  }

  // ----- Helpers --------------------------------------------------------

  FeatherPalette _palette(BuildContext context) =>
      Theme.of(context).brightness == Brightness.dark
          ? FeatherColors.dark
          : FeatherColors.light;

  /// The rose/pink gradient used for the header bar and the active
  /// segmented-button segment. Deliberately NOT Material blue/indigo —
  /// matches the erase/mirror/bend tool family in [FeatherColors].
  static const List<Color> _roseGradient = <Color>[
    FeatherPalette.accentPink,
    FeatherPalette.accentPurple,
    FeatherPalette.accentOrange,
  ];

  static const double _panelWidth = 304.0;

  // ----- Build ----------------------------------------------------------

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
          Padding(
            padding: const EdgeInsets.fromLTRB(16, 14, 16, 16),
            child: Column(
              crossAxisAlignment: CrossAxisAlignment.stretch,
              children: [
                _sectionLabel('Mode', palette),
                const SizedBox(height: 8),
                _buildModePicker(palette),
                const SizedBox(height: 16),
                _buildModeSection(palette),
                const Divider(height: 28),
                _buildToggleRow(
                  palette: palette,
                  icon: Icons.center_focus_strong_outlined,
                  label: 'Snap to guide',
                  value: _snap,
                  onChanged: (v) {
                    setState(() => _snap = v);
                    widget.onSnapChanged?.call(v);
                  },
                ),
                const SizedBox(height: 10),
                _buildToggleRow(
                  palette: palette,
                  icon: Icons.timeline_outlined,
                  label: 'Show guide ribbon',
                  value: _ribbon,
                  onChanged: (v) {
                    setState(() => _ribbon = v);
                    widget.onRibbonChanged?.call(v);
                  },
                ),
              ],
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
        borderRadius: const BorderRadius.vertical(
          top: Radius.circular(17),
        ),
        gradient: const LinearGradient(
          begin: Alignment.centerLeft,
          end: Alignment.centerRight,
          colors: _roseGradient,
        ),
      ),
      child: Row(
        children: [
          const Icon(Icons.view_in_ar_rounded, color: Colors.white, size: 18),
          const SizedBox(width: 8),
          Text(
            '3D Guide',
            style: FeatherTypography.h2.copyWith(color: Colors.white),
          ),
          const Spacer(),
          _CloseButton(onTap: widget.onClose),
        ],
      ),
    );
  }

  // ----- Mode picker ----------------------------------------------------

  Widget _buildModePicker(FeatherPalette palette) {
    // Order matches the Feather 3D UI: Draw / Loft / Bend / Primitives.
    final segments = <ButtonSegment<Guide3DType>>[
      ButtonSegment(
        value: Guide3DType.drawn,
        label: const Text('Draw', style: TextStyle(fontSize: 11)),
      ),
      ButtonSegment(
        value: Guide3DType.lofted,
        label: const Text('Loft', style: TextStyle(fontSize: 11)),
      ),
      ButtonSegment(
        value: Guide3DType.bent,
        label: const Text('Bend', style: TextStyle(fontSize: 11)),
      ),
      ButtonSegment(
        value: Guide3DType.primitive,
        label: const Text('Prim', style: TextStyle(fontSize: 11)),
      ),
    ];
    return SegmentedButton<Guide3DType>(
      style: SegmentedButton.styleFrom(
        backgroundColor: palette.panelFill,
        foregroundColor: palette.textSecondary,
        selectedBackgroundColor: const Color(0x55F472B6),
        selectedForegroundColor: Colors.white,
      ),
      segments: segments,
      showSelectedIcon: false,
      selected: <Guide3DType>{_mode},
      onSelectionChanged: (selection) {
        final next = selection.first;
        setState(() => _mode = next);
        widget.onModeChanged?.call(next);
      },
    );
  }

  // ----- Per-mode sections ---------------------------------------------

  Widget _buildModeSection(FeatherPalette palette) {
    switch (_mode) {
      case Guide3DType.drawn:
        return _hint(palette, Guide3DType.drawn.helpText);
      case Guide3DType.lofted:
        return _buildLoftSection(palette);
      case Guide3DType.bent:
        return _buildBendSection(palette);
      case Guide3DType.primitive:
        return _buildPrimitiveSection(palette);
    }
  }

  Widget _buildPrimitiveSection(FeatherPalette palette) {
    return Column(
      crossAxisAlignment: CrossAxisAlignment.stretch,
      children: [
        _sectionLabel('Shape', palette),
        const SizedBox(height: 8),
        Wrap(
          spacing: 6,
          runSpacing: 6,
          children: [
            for (final kind in Guide3DPrimitive.values)
              _PrimitiveChip(
                kind: kind,
                selected: _primitiveKind == kind,
                palette: palette,
                onTap: () {
                  setState(() => _primitiveKind = kind);
                  widget.onPrimitiveKindChanged?.call(kind);
                },
              ),
          ],
        ),
        const SizedBox(height: 14),
        _sectionLabel('Size', palette),
        Row(
          children: [
            Expanded(
              child: Slider(
                value: _primitiveSize,
                min: 0.5,
                max: 6.0,
                divisions: 55,
                label: _primitiveSize.toStringAsFixed(1),
                activeColor: FeatherPalette.accentPink,
                onChanged: (v) {
                  setState(() => _primitiveSize = v);
                  widget.onPrimitiveSizeChanged?.call(v);
                },
              ),
            ),
            SizedBox(
              width: 38,
              child: Text(
                _primitiveSize.toStringAsFixed(1),
                textAlign: TextAlign.right,
                style: FeatherTypography.caption
                    .copyWith(color: palette.textSecondary),
              ),
            ),
          ],
        ),
      ],
    );
  }

  Widget _buildLoftSection(FeatherPalette palette) {
    return Column(
      crossAxisAlignment: CrossAxisAlignment.stretch,
      children: [
        _sectionLabel('Loft axis', palette),
        const SizedBox(height: 8),
        _AxisPicker(
          value: _loftAxis,
          palette: palette,
          onChanged: (axis) {
            setState(() => _loftAxis = axis);
            widget.onLoftAxisChanged?.call(axis);
          },
        ),
        const SizedBox(height: 14),
        _sectionLabel('Segments', palette),
        Row(
          children: [
            Expanded(
              child: Slider(
                value: _loftSegments.toDouble(),
                min: 4,
                max: 128,
                divisions: 124,
                label: '$_loftSegments',
                activeColor: FeatherPalette.accentPurple,
                onChanged: (v) {
                  final n = v.round();
                  setState(() => _loftSegments = n);
                  widget.onLoftSegmentsChanged?.call(n);
                },
              ),
            ),
            SizedBox(
              width: 38,
              child: Text(
                '$_loftSegments',
                textAlign: TextAlign.right,
                style: FeatherTypography.caption
                    .copyWith(color: palette.textSecondary),
              ),
            ),
          ],
        ),
      ],
    );
  }

  Widget _buildBendSection(FeatherPalette palette) {
    return Column(
      crossAxisAlignment: CrossAxisAlignment.stretch,
      children: [
        _sectionLabel('Bend axis', palette),
        const SizedBox(height: 8),
        _AxisPicker(
          value: _bendAxis,
          palette: palette,
          onChanged: (axis) {
            setState(() => _bendAxis = axis);
            widget.onBendAxisChanged?.call(axis);
          },
        ),
        const SizedBox(height: 14),
        _sectionLabel('Angle', palette),
        Row(
          children: [
            Expanded(
              child: Slider(
                value: _bendAngle,
                min: -180.0,
                max: 180.0,
                divisions: 360,
                label: '${_bendAngle.round()}°',
                activeColor: FeatherPalette.accentOrange,
                onChanged: (v) {
                  setState(() => _bendAngle = v);
                  widget.onBendAngleChanged?.call(v);
                },
              ),
            ),
            SizedBox(
              width: 42,
              child: Text(
                '${_bendAngle.round()}°',
                textAlign: TextAlign.right,
                style: FeatherTypography.caption
                    .copyWith(color: palette.textSecondary),
              ),
            ),
          ],
        ),
      ],
    );
  }

  // ----- Shared bits ----------------------------------------------------

  Widget _sectionLabel(String text, FeatherPalette palette) {
    return Text(
      text.toUpperCase(),
      style: FeatherTypography.micro.copyWith(color: palette.textTertiary),
    );
  }

  Widget _hint(FeatherPalette palette, String text) {
    return Container(
      padding: const EdgeInsets.all(10),
      decoration: BoxDecoration(
        color: palette.panelFill,
        borderRadius: BorderRadius.circular(8),
        border: Border.all(color: palette.panelBorder),
      ),
      child: Row(
        crossAxisAlignment: CrossAxisAlignment.start,
        children: [
          Icon(Icons.info_outline, size: 14, color: palette.textTertiary),
          const SizedBox(width: 6),
          Expanded(
            child: Text(
              text,
              style: FeatherTypography.caption
                  .copyWith(color: palette.textSecondary),
            ),
          ),
        ],
      ),
    );
  }

  Widget _buildToggleRow({
    required FeatherPalette palette,
    required IconData icon,
    required String label,
    required bool value,
    required ValueChanged<bool> onChanged,
  }) {
    return Container(
      padding: const EdgeInsets.symmetric(horizontal: 10, vertical: 4),
      decoration: BoxDecoration(
        color: palette.panelFill,
        borderRadius: BorderRadius.circular(8),
        border: Border.all(color: palette.panelBorder),
      ),
      child: Row(
        children: [
          Icon(icon, size: 16, color: palette.textSecondary),
          const SizedBox(width: 8),
          Expanded(
            child: Text(
              label,
              style: FeatherTypography.body
                  .copyWith(color: palette.textPrimary),
            ),
          ),
          Switch(
            value: value,
            onChanged: onChanged,
            // Avoid the deprecated activeColor / activeTrackColor; the
            // thumbColor + trackColor overrides below give the rose/pink
            // look without tripping the analyze baseline.
            thumbColor: WidgetStateProperty.resolveWith<Color>((states) {
              if (states.contains(WidgetState.selected)) {
                return Colors.white;
              }
              return palette.textTertiary;
            }),
            trackColor: WidgetStateProperty.resolveWith<Color>((states) {
              if (states.contains(WidgetState.selected)) {
                return const Color(0x99F472B6);
              }
              return palette.divider;
            }),
            trackOutlineColor: WidgetStateProperty.resolveWith<Color>((states) {
              if (states.contains(WidgetState.selected)) {
                return const Color(0x00FFFFFF);
              }
              return palette.panelBorder;
            }),
          ),
        ],
      ),
    );
  }
}

// ----- Close button ------------------------------------------------------

class _CloseButton extends StatelessWidget {
  const _CloseButton({this.onTap});

  final VoidCallback? onTap;

  @override
  Widget build(BuildContext context) {
    return GestureDetector(
      behavior: HitTestBehavior.opaque,
      onTap: onTap,
      child: Container(
        width: 26,
        height: 26,
        decoration: BoxDecoration(
          color: Colors.white.withValues(alpha: 0.18),
          shape: BoxShape.circle,
        ),
        child: const Icon(Icons.close_rounded, size: 16, color: Colors.white),
      ),
    );
  }
}

// ----- Primitive chip ----------------------------------------------------

class _PrimitiveChip extends StatelessWidget {
  const _PrimitiveChip({
    required this.kind,
    required this.selected,
    required this.palette,
    required this.onTap,
  });

  final Guide3DPrimitive kind;
  final bool selected;
  final FeatherPalette palette;
  final VoidCallback onTap;

  @override
  Widget build(BuildContext context) {
    return GestureDetector(
      behavior: HitTestBehavior.opaque,
      onTap: onTap,
      child: Container(
        padding: const EdgeInsets.symmetric(horizontal: 10, vertical: 6),
        decoration: BoxDecoration(
          color: selected
              ? const Color(0x55F472B6)
              : palette.panelFill,
          borderRadius: BorderRadius.circular(8),
          border: Border.all(
            color: selected ? FeatherPalette.accentPink : palette.panelBorder,
          ),
        ),
        child: Text(
          kind.displayName,
          style: FeatherTypography.caption.copyWith(
            color: selected ? Colors.white : palette.textPrimary,
            fontWeight: selected ? FontWeight.w700 : FontWeight.w500,
          ),
        ),
      ),
    );
  }
}

// ----- Axis picker -------------------------------------------------------

class _AxisPicker extends StatelessWidget {
  const _AxisPicker({
    required this.value,
    required this.palette,
    required this.onChanged,
  });

  final GuideAxis value;
  final FeatherPalette palette;
  final ValueChanged<GuideAxis> onChanged;

  @override
  Widget build(BuildContext context) {
    return SegmentedButton<GuideAxis>(
      style: SegmentedButton.styleFrom(
        backgroundColor: palette.panelFill,
        foregroundColor: palette.textSecondary,
        selectedBackgroundColor: const Color(0x55A78BFA),
        selectedForegroundColor: Colors.white,
      ),
      segments: const <ButtonSegment<GuideAxis>>[
        ButtonSegment(value: GuideAxis.x, label: Text('X', style: TextStyle(fontSize: 11))),
        ButtonSegment(value: GuideAxis.y, label: Text('Y', style: TextStyle(fontSize: 11))),
        ButtonSegment(value: GuideAxis.z, label: Text('Z', style: TextStyle(fontSize: 11))),
      ],
      showSelectedIcon: false,
      selected: <GuideAxis>{value},
      onSelectionChanged: (s) => onChanged(s.first),
    );
  }
}
