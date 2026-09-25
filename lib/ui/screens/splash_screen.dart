// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// splash_screen.dart — Boot / splash screen with REAL progress.
//
// Replaces the legacy main.dart BootScreen for the new UI package. The
// progress bar is driven by the [BootPhase] stream the parent passes
// in — each phase update is the literal truth (engine probe, resource
// import, preset scan), not a timer.

import 'package:flutter/material.dart';

import '../theme/feather_animations.dart';
import '../theme/feather_colors.dart';
import '../theme/feather_typography.dart';
import '../theme/glassmorphism.dart';
import '../widgets/glass_panel.dart';

class BootPhase {
  const BootPhase({
    required this.progress, // 0..1
    required this.label,
    this.detail,
    this.engineReal,
    this.warning,
  });

  final double progress;
  final String label;
  final String? detail;
  final bool? engineReal;
  final String? warning;
}

class SplashScreen extends StatelessWidget {
  const SplashScreen({
    super.key,
    required this.phase,
    this.appVersion = '0.50.0',
  });

  final BootPhase phase;
  final String appVersion;

  @override
  Widget build(BuildContext context) {
    final palette = _palette(context);
    return Scaffold(
      backgroundColor: palette.appBackground,
      body: Stack(
        children: [
          // Ambient orbs (subtle, deep-slate palette).
          Positioned(
            left: -80,
            top: -80,
            child: _Orb(
              color: FeatherPalette.accentBlue.withValues(alpha: 0.16),
              size: 260,
            ),
          ),
          Positioned(
            right: -100,
            bottom: -100,
            child: _Orb(
              color: FeatherPalette.accentPurple.withValues(alpha: 0.14),
              size: 320,
            ),
          ),
          Center(
            child: GlassPanel(
              spec: GlassSpec.strong,
              width: 360,
              padding: const EdgeInsets.fromLTRB(28, 28, 28, 22),
              child: Column(
                mainAxisSize: MainAxisSize.min,
                crossAxisAlignment: CrossAxisAlignment.start,
                children: [
                  _Logo(palette: palette),
                  const SizedBox(height: 22),
                  Text(
                    'Feather-Krita',
                    style: FeatherTypography.display
                        .copyWith(color: palette.textPrimary),
                  ),
                  const SizedBox(height: 4),
                  Text(
                    '3D Drawing · Real Krita Brush Engine',
                    style: FeatherTypography.body
                        .copyWith(color: palette.textTertiary),
                  ),
                  const SizedBox(height: 24),
                  _ProgressBar(phase: phase, palette: palette),
                  const SizedBox(height: 18),
                  _Badges(phase: phase, palette: palette),
                ],
              ),
            ),
          ),
          Positioned(
            bottom: 24,
            left: 0,
            right: 0,
            child: Center(
              child: Text(
                '$appVersion · GPL-2.0-or-later',
                style: FeatherTypography.caption
                    .copyWith(color: palette.textTertiary),
              ),
            ),
          ),
        ],
      ),
    );
  }

  FeatherPalette _palette(BuildContext context) =>
      Theme.of(context).brightness == Brightness.dark
          ? FeatherColors.dark
          : FeatherColors.light;
}

class _Orb extends StatelessWidget {
  const _Orb({required this.color, required this.size});
  final Color color;
  final double size;

  @override
  Widget build(BuildContext context) {
    return Container(
      width: size,
      height: size,
      decoration: BoxDecoration(shape: BoxShape.circle, color: color),
    );
  }
}

class _Logo extends StatelessWidget {
  const _Logo({required this.palette});
  final FeatherPalette palette;

  @override
  Widget build(BuildContext context) {
    return Container(
      width: 64,
      height: 64,
      decoration: BoxDecoration(
        gradient: LinearGradient(
          begin: Alignment.topLeft,
          end: Alignment.bottomRight,
          colors: [FeatherPalette.accentBlue, FeatherPalette.accentPurple],
        ),
        borderRadius: BorderRadius.circular(16),
        boxShadow: [
          BoxShadow(
            color: FeatherPalette.accentBlue.withValues(alpha: 0.45),
            blurRadius: 22,
            spreadRadius: 2,
          ),
        ],
      ),
      child: const Icon(Icons.brush, color: Colors.white, size: 32),
    );
  }
}

class _ProgressBar extends StatelessWidget {
  const _ProgressBar({required this.phase, required this.palette});

  final BootPhase phase;
  final FeatherPalette palette;

  @override
  Widget build(BuildContext context) {
    return Column(
      crossAxisAlignment: CrossAxisAlignment.stretch,
      children: [
        ClipRRect(
          borderRadius: BorderRadius.circular(4),
          child: LinearProgressIndicator(
            value: phase.progress.clamp(0.0, 1.0),
            minHeight: 4,
            backgroundColor: palette.textPrimary.withValues(alpha: 0.08),
            valueColor: AlwaysStoppedAnimation<Color>(palette.accent),
          ),
        ),
        const SizedBox(height: 10),
        Row(
          children: [
            Expanded(
              child: Text(
                phase.label,
                style: FeatherTypography.caption
                    .copyWith(color: palette.textSecondary),
              ),
            ),
            Text(
              '${(phase.progress * 100).round()}%',
              style: FeatherTypography.mono
                  .copyWith(color: palette.textSecondary),
            ),
          ],
        ),
        if (phase.detail != null) ...[
          const SizedBox(height: 4),
          Text(
            phase.detail!,
            style: FeatherTypography.micro
                .copyWith(color: palette.textTertiary),
          ),
        ],
      ],
    );
  }
}

class _Badges extends StatelessWidget {
  const _Badges({required this.phase, required this.palette});

  final BootPhase phase;
  final FeatherPalette palette;

  @override
  Widget build(BuildContext context) {
    return Wrap(
      spacing: 6,
      runSpacing: 6,
      children: [
        if (phase.engineReal != null)
          _Badge(
            palette: palette,
            ok: phase.engineReal!,
            okText: 'REAL KRITA ENGINE',
            failText: 'SYNTHETIC FALLBACK',
          ),
        if (phase.warning != null)
          _WarnBadge(palette: palette, text: phase.warning!),
      ],
    );
  }
}

class _Badge extends StatelessWidget {
  const _Badge({
    required this.palette,
    required this.ok,
    required this.okText,
    required this.failText,
  });

  final FeatherPalette palette;
  final bool ok;
  final String okText;
  final String failText;

  @override
  Widget build(BuildContext context) {
    final color = ok ? FeatherPalette.accentGreen : FeatherPalette.accentOrange;
    return Container(
      padding: const EdgeInsets.symmetric(horizontal: 10, vertical: 4),
      decoration: BoxDecoration(
        color: color.withValues(alpha: 0.12),
        borderRadius: BorderRadius.circular(999),
        border: Border.all(color: color.withValues(alpha: 0.55)),
      ),
      child: Row(
        mainAxisSize: MainAxisSize.min,
        children: [
          Icon(ok ? Icons.verified_rounded : Icons.layers_rounded,
              size: 12, color: color),
          const SizedBox(width: 6),
          Text(
            ok ? okText : failText,
            style: FeatherTypography.micro.copyWith(
              color: color,
              fontWeight: FontWeight.w700,
              letterSpacing: 0.6,
            ),
          ),
        ],
      ),
    );
  }
}

class _WarnBadge extends StatelessWidget {
  const _WarnBadge({required this.palette, required this.text});
  final FeatherPalette palette;
  final String text;

  @override
  Widget build(BuildContext context) {
    return Container(
      padding: const EdgeInsets.symmetric(horizontal: 10, vertical: 4),
      decoration: BoxDecoration(
        color: FeatherPalette.accentOrange.withValues(alpha: 0.12),
        borderRadius: BorderRadius.circular(999),
        border: Border.all(
            color: FeatherPalette.accentOrange.withValues(alpha: 0.55)),
      ),
      child: Row(
        mainAxisSize: MainAxisSize.min,
        children: [
          Icon(Icons.warning_amber_rounded,
              size: 12, color: FeatherPalette.accentOrange),
          const SizedBox(width: 6),
          Text(
            text,
            style: FeatherTypography.micro.copyWith(
              color: FeatherPalette.accentOrange,
              fontWeight: FontWeight.w600,
            ),
          ),
        ],
      ),
    );
  }
}

/// Helper — runs a tiny entrance animation when the splash mounts.
/// Kept tiny on purpose; the splash should not steal attention from the
/// real progress bar.
class SplashEntrance extends StatefulWidget {
  const SplashEntrance({super.key, required this.child});

  final Widget child;

  @override
  State<SplashEntrance> createState() => _SplashEntranceState();
}

class _SplashEntranceState extends State<SplashEntrance>
    with SingleTickerProviderStateMixin {
  late final AnimationController _c;

  @override
  void initState() {
    super.initState();
    _c = AnimationController(
      vsync: this,
      duration: FeatherDurations.page,
    )..forward();
  }

  @override
  void dispose() {
    _c.dispose();
    super.dispose();
  }

  @override
  Widget build(BuildContext context) {
    return FadeTransition(
      opacity: CurvedAnimation(parent: _c, curve: FeatherCurves.page),
      child: widget.child,
    );
  }
}
