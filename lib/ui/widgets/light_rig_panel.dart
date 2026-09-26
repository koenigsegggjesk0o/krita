// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// light_rig_panel.dart — Feather 3D-style light rig quick-access dial.
//
// Feather 3D exposes the scene's key light as a small floating dial the
// artist can summon without opening the full Environment tab. The dial
// has four knobs:
//   * Azimuth    — heading around the scene (0..360°).
//   * Elevation  — angle above the ground plane (0..90°).
//   * Intensity  — key-light brightness multiplier (0..2).
//   * Ambient    — shadow-side floor (0..1).
//
// A sun icon at the top of the panel rotates with the azimuth so the
// artist gets a visual affordance of where the key light sits in the
// scene. The panel is a self-contained StatefulWidget (state mirrors
// the host's canonical values); the host is notified of every change
// through the `on*Changed` callbacks.
//
// Visual language: rose/pink gradient header (matches GuidePanel +
// AssistPanel + RenderModeToggle). Mounted by the host
// (lib/screens/main_screen.dart) as a Positioned overlay + a floating
// sun toggle button.

import 'dart:math' as math;

import 'package:flutter/material.dart';

import '../theme/feather_colors.dart';
import '../theme/feather_typography.dart';
import '../theme/glassmorphism.dart';
import 'glass_panel.dart';

/// Compact Feather 3D-style light rig dial. Self-contained stateful
/// widget; the host forwards the canonical values + receives change
/// callbacks.
class LightRigPanel extends StatefulWidget {
  const LightRigPanel({
    super.key,
    this.initialAzimuth = 5 * math.pi / 4,
    this.initialElevation = math.pi / 4,
    this.initialIntensity = 1.0,
    this.initialAmbient = 0.35,
    this.onAzimuthChanged,
    this.onElevationChanged,
    this.onIntensityChanged,
    this.onAmbientChanged,
    this.onClose,
  });

  /// Initial azimuth in radians (0..2π). 0 = light from the right,
  /// π/2 = from below, π = from the left, 3π/2 = from above.
  final double initialAzimuth;

  /// Initial elevation in radians (0..π/2). 0 = horizon, π/2 = zenith.
  final double initialElevation;

  /// Initial key-light intensity (0..2).
  final double initialIntensity;

  /// Initial ambient floor (0..1).
  final double initialAmbient;

  final ValueChanged<double>? onAzimuthChanged;
  final ValueChanged<double>? onElevationChanged;
  final ValueChanged<double>? onIntensityChanged;
  final ValueChanged<double>? onAmbientChanged;
  final VoidCallback? onClose;

  @override
  State<LightRigPanel> createState() => _LightRigPanelState();
}

class _LightRigPanelState extends State<LightRigPanel> {
  late double _azimuth;
  late double _elevation;
  late double _intensity;
  late double _ambient;

  @override
  void initState() {
    super.initState();
    _azimuth = widget.initialAzimuth;
    _elevation = widget.initialElevation;
    _intensity = widget.initialIntensity;
    _ambient = widget.initialAmbient;
  }

  /// The rose/pink gradient used by the panel header — matches the
  /// GuidePanel + AssistPanel family.
  static const List<Color> _roseGradient = <Color>[
    FeatherPalette.accentPink,
    FeatherPalette.accentPurple,
    FeatherPalette.accentOrange,
  ];

  static const double _panelWidth = 264.0;

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
                _buildSunDial(palette),
                const SizedBox(height: 14),
                _sectionLabel('Direction', palette),
                const SizedBox(height: 8),
                _LabeledSlider(
                  label: 'Azimuth',
                  value: (_azimuth / (2 * math.pi)).clamp(0.0, 1.0),
                  palette: palette,
                  formatter: (v) => '${(v * 360).round()}°',
                  onChanged: (v) {
                    final rad = v * 2 * math.pi;
                    setState(() => _azimuth = rad);
                    widget.onAzimuthChanged?.call(rad);
                  },
                ),
                _LabeledSlider(
                  label: 'Elevation',
                  value: (_elevation / (math.pi / 2)).clamp(0.0, 1.0),
                  palette: palette,
                  formatter: (v) => '${(v * 90).round()}°',
                  onChanged: (v) {
                    final rad = v * math.pi / 2;
                    setState(() => _elevation = rad);
                    widget.onElevationChanged?.call(rad);
                  },
                ),
                const Divider(height: 24),
                _sectionLabel('Brightness', palette),
                const SizedBox(height: 8),
                _LabeledSlider(
                  label: 'Intensity',
                  value: (_intensity / 2.0).clamp(0.0, 1.0),
                  palette: palette,
                  formatter: (v) => (v * 2.0).toStringAsFixed(2),
                  onChanged: (v) {
                    final val = v * 2.0;
                    setState(() => _intensity = val);
                    widget.onIntensityChanged?.call(val);
                  },
                ),
                _LabeledSlider(
                  label: 'Ambient',
                  value: _ambient.clamp(0.0, 1.0),
                  palette: palette,
                  formatter: (v) => (v).toStringAsFixed(2),
                  onChanged: (v) {
                    setState(() => _ambient = v);
                    widget.onAmbientChanged?.call(v);
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
        borderRadius: const BorderRadius.vertical(top: Radius.circular(17)),
        gradient: const LinearGradient(
          begin: Alignment.centerLeft,
          end: Alignment.centerRight,
          colors: _roseGradient,
        ),
      ),
      child: Row(
        children: [
          const Icon(Icons.wb_sunny_outlined, color: Colors.white, size: 18),
          const SizedBox(width: 8),
          Text(
            'Light',
            style: FeatherTypography.h2.copyWith(color: Colors.white),
          ),
          const Spacer(),
          _CloseButton(onTap: widget.onClose),
        ],
      ),
    );
  }

  // ----- Sun dial -------------------------------------------------------

  /// A sun icon that rotates with the azimuth, surrounded by a faint
  /// elevation ring. Visual affordance for where the key light sits.
  Widget _buildSunDial(FeatherPalette palette) {
    // Convert azimuth to degrees for the rotation. 0° = light from the
    // right; the sun icon rotates clockwise as azimuth increases.
    final azimuthDeg = (_azimuth * 180.0 / math.pi) % 360.0;
    return SizedBox(
      height: 84,
      child: Stack(
        alignment: Alignment.center,
        children: [
          // Faint elevation ring — the sun sits on this circle; its
          // vertical offset encodes elevation (0 = horizon = edge of
          // circle, π/2 = zenith = top of circle).
          Container(
            width: 72,
            height: 72,
            decoration: BoxDecoration(
              shape: BoxShape.circle,
              border: Border.all(
                color: palette.textTertiary.withValues(alpha: 0.25),
                width: 1,
              ),
            ),
          ),
          // The sun icon, rotated by azimuth + lifted by elevation.
          Transform.translate(
            offset: Offset(
              28 * math.cos(_azimuth),
              -28 * math.sin(_elevation),
            ),
            child: Transform.rotate(
              angle: _azimuth,
              child: Container(
                width: 28,
                height: 28,
                decoration: BoxDecoration(
                  shape: BoxShape.circle,
                  gradient: const LinearGradient(
                    begin: Alignment.topLeft,
                    end: Alignment.bottomRight,
                    colors: _roseGradient,
                  ),
                  boxShadow: [
                    BoxShadow(
                      color: FeatherPalette.accentOrange.withValues(alpha: 0.5),
                      blurRadius: 12,
                      spreadRadius: 1,
                    ),
                  ],
                ),
                child: const Icon(Icons.wb_sunny_rounded,
                    color: Colors.white, size: 16),
              ),
            ),
          ),
          // Azimuth readout below the dial.
          Positioned(
            bottom: 0,
            child: Text(
              'Az ${azimuthDeg.round()}° · El ${(_elevation * 180.0 / math.pi).round()}°',
              style: FeatherTypography.mono.copyWith(
                color: palette.textTertiary,
                fontSize: 10,
              ),
            ),
          ),
        ],
      ),
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

  FeatherPalette _palette(BuildContext context) =>
      Theme.of(context).brightness == Brightness.dark
          ? FeatherColors.dark
          : FeatherColors.light;
}

class _LabeledSlider extends StatelessWidget {
  const _LabeledSlider({
    required this.label,
    required this.value,
    required this.palette,
    required this.formatter,
    required this.onChanged,
  });

  final String label;
  final double value;
  final FeatherPalette palette;
  final String Function(double) formatter;
  final ValueChanged<double>? onChanged;

  @override
  Widget build(BuildContext context) {
    return Padding(
      padding: const EdgeInsets.only(top: 6, bottom: 2),
      child: Row(
        children: [
          SizedBox(
            width: 70,
            child: Text(
              label,
              style: FeatherTypography.caption.copyWith(
                color: palette.textSecondary,
                fontWeight: FontWeight.w500,
              ),
            ),
          ),
          Expanded(
            child: SliderTheme(
              data: SliderTheme.of(context).copyWith(
                trackHeight: 3,
                thumbShape:
                    const RoundSliderThumbShape(enabledThumbRadius: 7),
                overlayShape:
                    const RoundSliderOverlayShape(overlayRadius: 14),
                activeTrackColor: FeatherPalette.accentPink,
                inactiveTrackColor:
                    palette.textPrimary.withValues(alpha: 0.1),
                thumbColor: palette.textPrimary,
              ),
              child: Slider(
                value: value.clamp(0.0, 1.0),
                onChanged: onChanged,
              ),
            ),
          ),
          SizedBox(
            width: 44,
            child: Text(
              formatter(value),
              textAlign: TextAlign.right,
              style: FeatherTypography.mono
                  .copyWith(color: palette.textPrimary, fontSize: 11),
            ),
          ),
        ],
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
