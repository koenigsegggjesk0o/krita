// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// assist_panel.dart — Feather assistance subsystems switcher panel.
//
// The engine ships four assistance modules (lib/engine/assistance/ +
// lib/engine/color/color_sampler.dart) that were scaffolded with tests
// but never called at runtime (AUDIT_FINAL.md §3). The v0.54-A wiring
// task connects them to the live paint path; this panel is the
// user-facing switcher for their per-assist toggles:
//
//   * Stable Strokes  — causal Gaussian pre-filter toggle + intensity.
//   * Mirror          — X / Y / Z axis segmented buttons (v0.54-A §3).
//   * Draw Shape       — Auto / Line / Circle mode segmented buttons
//                        (v0.54-A §4).
//
// Visual language matches [GuidePanel]: Material 3 + dark theme, rose/pink
// gradient header (accentPink → accentPurple → accentOrange), glass body.
// The panel is a self-contained StatefulWidget — the host only forwards a
// handful of callbacks + the current state mirrors.
//
// Each section is built by its own `_build*Section` helper so the panel
// grows additively as subsystems are wired ( Stable Strokes ships first;
// Mirror + Shape sections are added in the follow-up commits of the same
// v0.54-A task).

import 'package:flutter/material.dart';

import '../../engine/assistance/mirror_assist.dart' show MirrorAxis;
import '../theme/feather_colors.dart';
import '../theme/feather_typography.dart';
import '../theme/glassmorphism.dart';
import 'glass_panel.dart';

/// Self-contained Feather assistance switcher panel.
///
/// See the file doc for the visual + behavioural contract. The panel is
/// stateful: it owns the live values of every slider / toggle / segmented
/// button so the UI stays responsive between callback dispatches. The
/// host (main_screen.dart) is notified of every change through the
/// `on*Changed` callbacks.
class AssistPanel extends StatefulWidget {
  const AssistPanel({
    super.key,
    // ----- Stable Strokes -----
    this.initialStableStrokes = false,
    this.initialStableStrokesIntensity = 0.5,
    this.onStableStrokesChanged,
    this.onStableStrokesIntensityChanged,
    // ----- Mirror (v0.54-A §3) -----
    this.initialMirrorAxes = const <MirrorAxis>{},
    this.onMirrorAxisToggled,
    // ----- Close -----
    this.onClose,
  });

  // ----- Initial values (one-shot, read in initState) -------------------
  final bool initialStableStrokes;
  final double initialStableStrokesIntensity;
  final Set<MirrorAxis> initialMirrorAxes;

  // ----- Change callbacks ----------------------------------------------
  final ValueChanged<bool>? onStableStrokesChanged;
  final ValueChanged<double>? onStableStrokesIntensityChanged;

  /// Fired when the user taps one of the X / Y / Z mirror-axis chips.
  /// The host toggles the axis in its [MirrorAssist.activeAxes] set.
  final ValueChanged<MirrorAxis>? onMirrorAxisToggled;

  /// Fired when the user taps the panel's close button.
  final VoidCallback? onClose;

  @override
  State<AssistPanel> createState() => _AssistPanelState();
}

class _AssistPanelState extends State<AssistPanel> {
  late bool _stableStrokes;
  late double _stableStrokesIntensity;
  late Set<MirrorAxis> _mirrorAxes;

  @override
  void initState() {
    super.initState();
    _stableStrokes = widget.initialStableStrokes;
    _stableStrokesIntensity = widget.initialStableStrokesIntensity;
    _mirrorAxes = Set<MirrorAxis>.of(widget.initialMirrorAxes);
  }

  /// The rose/pink gradient used by the panel header — matches the
  /// erase / mirror / bend tool family in [FeatherColors] (NOT the
  /// default Material blue/indigo).
  static const List<Color> _roseGradient = <Color>[
    FeatherPalette.accentPink,
    FeatherPalette.accentPurple,
    FeatherPalette.accentOrange,
  ];

  static const double _panelWidth = 272.0;

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
                _sectionLabel('Stable Strokes', palette),
                const SizedBox(height: 8),
                _buildStableStrokesSection(palette),
                const Divider(height: 28),
                _sectionLabel('Mirror', palette),
                const SizedBox(height: 8),
                _buildMirrorSection(palette),
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
        borderRadius: const BorderRadius.vertical(top: Radius.circular(17)),
        gradient: const LinearGradient(
          begin: Alignment.centerLeft,
          end: Alignment.centerRight,
          colors: _roseGradient,
        ),
      ),
      child: Row(
        children: [
          const Icon(Icons.auto_awesome_outlined,
              color: Colors.white, size: 18),
          const SizedBox(width: 8),
          Text(
            'Assist',
            style: FeatherTypography.h2.copyWith(color: Colors.white),
          ),
          const Spacer(),
          _CloseButton(onTap: widget.onClose),
        ],
      ),
    );
  }

  // ----- Stable Strokes section -----------------------------------------

  Widget _buildStableStrokesSection(FeatherPalette palette) {
    return Column(
      crossAxisAlignment: CrossAxisAlignment.stretch,
      children: [
        _buildToggleRow(
          palette: palette,
          icon: Icons.waves_outlined,
          label: 'Stabilize stroke',
          subtitle: 'Causal Gaussian pre-filter',
          value: _stableStrokes,
          onChanged: (v) {
            setState(() => _stableStrokes = v);
            widget.onStableStrokesChanged?.call(v);
          },
        ),
        const SizedBox(height: 12),
        Opacity(
          opacity: _stableStrokes ? 1.0 : 0.4,
          child: Column(
            crossAxisAlignment: CrossAxisAlignment.stretch,
            children: [
              Row(
                children: [
                  Text(
                    'Intensity',
                    style: FeatherTypography.caption
                        .copyWith(color: palette.textSecondary),
                  ),
                  const Spacer(),
                  Text(
                    '${(_stableStrokesIntensity * 100).round()}%',
                    style: FeatherTypography.mono
                        .copyWith(color: palette.textPrimary),
                  ),
                ],
              ),
              Slider(
                value: _stableStrokesIntensity,
                min: 0.0,
                max: 1.0,
                divisions: 20,
                activeColor: FeatherPalette.accentPink,
                onChanged: _stableStrokes
                    ? (v) {
                        setState(() => _stableStrokesIntensity = v);
                        widget.onStableStrokesIntensityChanged?.call(v);
                      }
                    : null,
              ),
            ],
          ),
        ),
      ],
    );
  }

  // ----- Mirror section (v0.54-A §3) -----------------------------------

  Widget _buildMirrorSection(FeatherPalette palette) {
    // Per-axis colour swatches (per mirror_assist.dart docs: X=red, Y=green,
    // Z=blue). Each chip toggles its axis in the host's MirrorAssist set.
    const axisColours = <MirrorAxis, Color>{
      MirrorAxis.x: Color(0xFFFF3B30),
      MirrorAxis.y: Color(0xFF34C759),
      MirrorAxis.z: Color(0xFF0A84FF),
    };
    return Column(
      crossAxisAlignment: CrossAxisAlignment.stretch,
      children: [
        Text(
          'Active axes produce 2\u2070 copies of each committed stroke (X = red, Y = green, Z = blue).',
          style: FeatherTypography.micro
              .copyWith(color: palette.textTertiary),
        ),
        const SizedBox(height: 10),
        Row(
          children: [
            for (final axis in const [MirrorAxis.x, MirrorAxis.y, MirrorAxis.z])
              Expanded(
                child: Padding(
                  padding: EdgeInsets.only(
                      right: axis == MirrorAxis.z ? 0 : 8),
                  child: _MirrorAxisChip(
                    axis: axis,
                    colour: axisColours[axis]!,
                    active: _mirrorAxes.contains(axis),
                    palette: palette,
                    onTap: () {
                      setState(() {
                        if (_mirrorAxes.contains(axis)) {
                          _mirrorAxes.remove(axis);
                        } else {
                          _mirrorAxes.add(axis);
                        }
                      });
                      widget.onMirrorAxisToggled?.call(axis);
                    },
                  ),
                ),
              ),
          ],
        ),
        const SizedBox(height: 8),
        Text(
          _mirrorAxes.isEmpty
              ? 'Mirror off'
              : 'Mirror on \u2014 ${1 << _mirrorAxes.length} copies per stroke',
          style: FeatherTypography.caption.copyWith(
            color: _mirrorAxes.isEmpty ? palette.textTertiary : FeatherPalette.accentPink,
            fontWeight: FontWeight.w600,
          ),
        ),
      ],
    );
  }

  // ----- Shared helpers -------------------------------------------------

  Widget _sectionLabel(String text, FeatherPalette palette) {
    return Text(
      text.toUpperCase(),
      style: FeatherTypography.micro.copyWith(
        color: palette.textTertiary,
        letterSpacing: 1.2,
      ),
    );
  }

  Widget _buildToggleRow({
    required FeatherPalette palette,
    required IconData icon,
    required String label,
    String? subtitle,
    required bool value,
    required ValueChanged<bool> onChanged,
  }) {
    return GestureDetector(
      behavior: HitTestBehavior.opaque,
      onTap: () => onChanged(!value),
      child: Container(
        padding: const EdgeInsets.symmetric(horizontal: 10, vertical: 10),
        decoration: BoxDecoration(
          color: value
              ? FeatherPalette.accentPink.withValues(alpha: 0.16)
              : palette.panelFill,
          borderRadius: BorderRadius.circular(10),
          border: Border.all(
            color: value ? FeatherPalette.accentPink : palette.panelBorder,
            width: value ? 1.4 : 1,
          ),
        ),
        child: Row(
          children: [
            Icon(icon,
                size: 18,
                color: value ? FeatherPalette.accentPink : palette.textSecondary),
            const SizedBox(width: 10),
            Expanded(
              child: Column(
                crossAxisAlignment: CrossAxisAlignment.start,
                children: [
                  Text(
                    label,
                    style: FeatherTypography.caption.copyWith(
                      color: value ? palette.textPrimary : palette.textSecondary,
                      fontWeight: FontWeight.w600,
                    ),
                  ),
                  if (subtitle != null)
                    Text(
                      subtitle,
                      style: FeatherTypography.micro
                          .copyWith(color: palette.textTertiary),
                    ),
                ],
              ),
            ),
            Switch.adaptive(
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
                  return FeatherPalette.accentPink.withValues(alpha: 0.6);
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
      ),
    );
  }

  FeatherPalette _palette(BuildContext context) =>
      Theme.of(context).brightness == Brightness.dark
          ? FeatherColors.dark
          : FeatherColors.light;
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

/// A single X / Y / Z mirror-axis toggle chip. Renders the axis letter
/// on a coloured swatch (red / green / blue per mirror_assist.dart docs).
/// Tapping toggles the axis in the host's [MirrorAssist.activeAxes] set.
class _MirrorAxisChip extends StatelessWidget {
  const _MirrorAxisChip({
    required this.axis,
    required this.colour,
    required this.active,
    required this.palette,
    required this.onTap,
  });

  final MirrorAxis axis;
  final Color colour;
  final bool active;
  final FeatherPalette palette;
  final VoidCallback onTap;

  @override
  Widget build(BuildContext context) {
    return GestureDetector(
      behavior: HitTestBehavior.opaque,
      onTap: onTap,
      child: Tooltip(
        message: 'Mirror ${axis.name.toUpperCase()} axis',
        child: Container(
          height: 44,
          decoration: BoxDecoration(
            color: active ? colour.withValues(alpha: 0.22) : palette.panelFill,
            borderRadius: BorderRadius.circular(10),
            border: Border.all(
              color: active ? colour : palette.panelBorder,
              width: active ? 1.6 : 1,
            ),
          ),
          child: Column(
            mainAxisAlignment: MainAxisAlignment.center,
            children: [
              Text(
                axis.name.toUpperCase(),
                style: FeatherTypography.bodyStrong.copyWith(
                  color: active ? colour : palette.textSecondary,
                  fontSize: 15,
                  fontWeight: FontWeight.w700,
                ),
              ),
            ],
          ),
        ),
      ),
    );
  }
}
