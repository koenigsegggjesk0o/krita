// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// render_mode_toggle.dart — Feather 3D-style 3-state render mode toggle.
//
// Feather 3D exposes three render modes the artist can switch between
// from the top bar:
//   * Shaded    — every stroke's per-stroke material is respected
//                 (Shaded → Lambert + Phong, Glow → additive bloom,
//                 Cutout → background read, Shadeless → flat).
//   * Shadeless — every stroke is forced flat (no lighting, no shadows).
//                 Useful for silhouette / blocking review.
//   * Wireframe — strokes draw as polyline skeletons (no fill, no
//                 lighting). Useful for inspecting curve density +
//                 topology without the lit surface distracting.
//
// The engine's [CanvasScene.renderMode] is a boolean (shaded vs flat);
// this widget exposes the third (wireframe) by routing through a host-
// side overlay path (the host draws each stroke's screen points as a
// [CanvasOverlayPolyline] on top of the flat render — see the v55-B
// worklog entry for the honest scope note: a real wireframe material
// pass would require touching lib/core/rendering which is out of scope).
//
// Visual language: rose/pink gradient pill with 3 icon buttons, matches
// the existing GuidePanel + AssistPanel toggle family. Mounted by the
// host (lib/screens/main_screen.dart) as a Positioned overlay just below
// the top bar so it reads as the prominent Feather 3D-style top-bar
// cluster the brief asks for, without disturbing the existing single
// binary toggle in [TopBar] (which stays for legacy parity).

import 'package:flutter/material.dart';

import '../theme/feather_animations.dart';
import '../theme/feather_colors.dart';
import '../theme/feather_typography.dart';
import '../theme/glassmorphism.dart';
import 'glass_panel.dart';

/// The three Feather 3D render modes the artist can switch between.
enum RenderModeState {
  /// Per-stroke materials respected (Shaded / Glow / Cutout / Shadeless).
  shaded,

  /// Every stroke forced flat — silhouette / blocking review.
  shadeless,

  /// Strokes drawn as polyline skeletons — topology / density review.
  wireframe,
}

extension RenderModeStateX on RenderModeState {
  String get label {
    switch (this) {
      case RenderModeState.shaded:
        return 'Shaded';
      case RenderModeState.shadeless:
        return 'Shadeless';
      case RenderModeState.wireframe:
        return 'Wireframe';
    }
  }

  IconData get icon {
    switch (this) {
      case RenderModeState.shaded:
        // Lit cube — per-stroke materials + Lambert + Phong.
        return Icons.view_in_ar_rounded;
      case RenderModeState.shadeless:
        // Flat cube — silhouette / blocking.
        return Icons.crop_square_rounded;
      case RenderModeState.wireframe:
        // Wireframe cube — topology / density.
        return Icons.grid_on_rounded;
    }
  }

  String get tooltip {
    switch (this) {
      case RenderModeState.shaded:
        return 'Shaded — per-stroke materials + lighting';
      case RenderModeState.shadeless:
        return 'Shadeless — flat silhouettes (no lighting)';
      case RenderModeState.wireframe:
        return 'Wireframe — polyline skeleton overlay';
    }
  }
}

/// A prominent 3-button render-mode cluster (rose/pink gradient pill).
///
/// Stateless — the host owns the canonical [RenderModeState] and feeds
/// it back through [mode]. Tapping a button fires [onChanged] with the
/// new value; the host then projects the choice onto the engine's
/// [CanvasScene.renderMode] bool (+ a wireframe-overlay flag).
class RenderModeToggle extends StatelessWidget {
  const RenderModeToggle({
    super.key,
    required this.mode,
    this.onChanged,
  });

  final RenderModeState mode;
  final ValueChanged<RenderModeState>? onChanged;

  /// The rose/pink gradient used by the toggle's active button — matches
  /// the GuidePanel + AssistPanel family (NOT the default Material blue).
  static const List<Color> _roseGradient = <Color>[
    FeatherPalette.accentPink,
    FeatherPalette.accentPurple,
    FeatherPalette.accentOrange,
  ];

  @override
  Widget build(BuildContext context) {
    final palette = _palette(context);
    return GlassPanel(
      spec: GlassSpec.subtle,
      padding: const EdgeInsets.symmetric(horizontal: 4, vertical: 4),
      child: Row(
        mainAxisSize: MainAxisSize.min,
        children: [
          for (int i = 0; i < RenderModeState.values.length; i++) ...[
            _RenderModeButton(
              state: RenderModeState.values[i],
              active: RenderModeState.values[i] == mode,
              palette: palette,
              onTap: onChanged == null
                  ? null
                  : () => onChanged!(RenderModeState.values[i]),
            ),
            if (i < RenderModeState.values.length - 1)
              Padding(
                padding: const EdgeInsets.symmetric(horizontal: 2),
                child: Container(
                  width: 1,
                  height: 20,
                  color: palette.divider,
                ),
              ),
          ],
        ],
      ),
    );
  }

  FeatherPalette _palette(BuildContext context) =>
      Theme.of(context).brightness == Brightness.dark
          ? FeatherColors.dark
          : FeatherColors.light;
}

class _RenderModeButton extends StatelessWidget {
  const _RenderModeButton({
    required this.state,
    required this.active,
    required this.palette,
    required this.onTap,
  });

  final RenderModeState state;
  final bool active;
  final FeatherPalette palette;
  final VoidCallback? onTap;

  @override
  Widget build(BuildContext context) {
    return Tooltip(
      message: state.tooltip,
      child: GestureDetector(
        behavior: HitTestBehavior.opaque,
        onTap: onTap,
        child: AnimatedContainer(
          duration: FeatherDurations.quick,
          curve: FeatherCurves.toggle,
          padding: const EdgeInsets.symmetric(horizontal: 10, vertical: 8),
          decoration: BoxDecoration(
            gradient: active
                ? const LinearGradient(
                    begin: Alignment.topLeft,
                    end: Alignment.bottomRight,
                    colors: RenderModeToggle._roseGradient,
                  )
                : null,
            color: active ? null : Colors.transparent,
            borderRadius: BorderRadius.circular(8),
          ),
          child: Row(
            mainAxisSize: MainAxisSize.min,
            children: [
              Icon(
                state.icon,
                size: 16,
                color: active ? Colors.white : palette.textSecondary,
              ),
              const SizedBox(width: 6),
              Text(
                state.label,
                style: FeatherTypography.caption.copyWith(
                  color: active ? Colors.white : palette.textSecondary,
                  fontWeight: FontWeight.w600,
                ),
              ),
            ],
          ),
        ),
      ),
    );
  }
}
