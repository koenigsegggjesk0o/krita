// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// main.dart — Entry point aplikasi Feather-Krita (feather-integration).
//
// Boot sequence (REAL, not a timer):
//   1. Probe the native Krita bridge (hang-proof isolate + timeout).
//   2. First-run import of the full real Krita resource library
//      (assets/krita-data.zip → feather_resources/, 1:1, unmodified).
//   3. Enter the new editor host (lib/screens/main_screen.dart) which
//      composes the new 158-file engine with the new UI package
//      (lib/ui/screens/editor_screen.dart + lib/ui/widgets/*).
//
// 100% on the bar means the phases actually completed — nothing heavy
// runs invisibly after the loader. The boot screen is the new
// [SplashScreen] from lib/ui/screens/splash_screen.dart; the editor is
// the new [MainScreen] host (no longer the legacy v0.50 MainScreen).

import 'dart:async';

import 'package:flutter/material.dart';
import 'package:flutter/services.dart';

import 'package:flutter_riverpod/flutter_riverpod.dart';

import 'io/engine_probe.dart';
import 'io/krita_launcher.dart';
import 'io/krita_resources.dart';
import 'screens/main_screen.dart';
import 'ui/screens/splash_screen.dart' as new_splash;
import 'ui/theme/app_theme.dart' as new_theme;
import 'utils/app_version.dart';

void main() {
  WidgetsFlutterBinding.ensureInitialized();

  SystemChrome.setSystemUIOverlayStyle(
    const SystemUiOverlayStyle(
      statusBarColor: Colors.transparent,
      statusBarIconBrightness: Brightness.light,
      systemNavigationBarColor: Colors.transparent,
      systemNavigationBarIconBrightness: Brightness.light,
    ),
  );

  SystemChrome.setPreferredOrientations([
    DeviceOrientation.landscapeLeft,
    DeviceOrientation.landscapeRight,
    DeviceOrientation.portraitUp,
  ]);

  runApp(
    const ProviderScope(
      child: FeatherKritaApp(),
    ),
  );
}

/// Root app. Uses the new UI package theme (Feather glassmorphism palette).
class FeatherKritaApp extends StatelessWidget {
  const FeatherKritaApp({super.key});

  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      title: 'Feather-Krita',
      debugShowCheckedModeBanner: false,
      theme: new_theme.AppTheme.darkTheme,
      home: const _BootShell(),
    );
  }
}

/// Real boot shell: drives the new [SplashScreen] with the actual boot
/// phases (engine probe → resource import → preset scan → enter editor).
class _BootShell extends StatefulWidget {
  const _BootShell();

  @override
  State<_BootShell> createState() => _BootShellState();
}

class _BootShellState extends State<_BootShell> {
  new_splash.BootPhase _phase = const new_splash.BootPhase(
    progress: 0.0,
    label: 'Starting…',
  );
  bool _resourcesMissing = false;

  @override
  void initState() {
    super.initState();
    scheduleMicrotask(_boot);
  }

  Future<void> _boot() async {
    // ---- Phase 1: native Krita engine probe (0.00 → 0.16) ------------
    if (!mounted) return;
    _setPhase(const new_splash.BootPhase(
      progress: 0.0,
      label: 'Loading Krita brush engine…',
    ));
    final engineOk = await probeKritaEngine();
    if (!mounted) return;
    _setPhase(new_splash.BootPhase(
      progress: 0.16,
      label: 'Loading Krita brush engine…',
      engineReal: engineOk,
    ));

    // ---- Phase 2: first-run resource import (0.16 → 0.92) ------------
    _setPhase(new_splash.BootPhase(
      progress: 0.16,
      label: 'Importing real Krita resources…',
      engineReal: engineOk,
    ));
    final summary = await KritaResources.ensureImported(
      onProgress: (done, total) {
        if (!mounted) return;
        _setPhase(new_splash.BootPhase(
          progress: 0.16 + 0.76 * (total > 0 ? done / total : 1.0),
          label: 'Importing real Krita resources…',
          detail: '$done / $total files',
          engineReal: engineOk,
        ));
      },
    );
    if (!mounted) return;
    if (!summary.ok) {
      _resourcesMissing = true;
    }

    // ---- Phase 3: preset scan hand-off (0.92 → 0.97) ------------------
    _setPhase(new_splash.BootPhase(
      progress: 0.92,
      label: 'Scanning brush presets…',
      engineReal: engineOk,
      warning: _resourcesMissing
          ? 'Resource payload unavailable — using bundled presets'
          : null,
    ));
    await Future<void>.delayed(const Duration(milliseconds: 120));
    if (!mounted) return;

    // ---- Phase 4: enter the editor (0.97 → 1.00) ----------------------
    _setPhase(new_splash.BootPhase(
      progress: 0.97,
      label: 'Ready',
      engineReal: engineOk,
      warning: _resourcesMissing
          ? 'Resource payload unavailable — using bundled presets'
          : null,
    ));
    await Future<void>.delayed(const Duration(milliseconds: 240));
    if (!mounted) return;
    _setPhase(new_splash.BootPhase(
      progress: 1.0,
      label: 'Ready',
      engineReal: engineOk,
      warning: _resourcesMissing
          ? 'Resource payload unavailable — using bundled presets'
          : null,
    ));
    await Future<void>.delayed(const Duration(milliseconds: 200));
    if (!mounted) return;

    Navigator.of(context).pushReplacement(
      PageRouteBuilder(
        pageBuilder: (context, animation, secondaryAnimation) => MainScreen(
          engineReal: engineOk,
        ),
        transitionsBuilder: (context, animation, secondaryAnimation, child) {
          return FadeTransition(opacity: animation, child: child);
        },
        transitionDuration: const Duration(milliseconds: 400),
      ),
    );
  }

  void _setPhase(new_splash.BootPhase phase) {
    if (!mounted) return;
    setState(() => _phase = phase);
  }

  @override
  Widget build(BuildContext context) {
    // Append the bundled-Krita badge when present.
    return Stack(
      children: [
        new_splash.SplashScreen(
          phase: _phase,
          appVersion: kAppVersionLabel,
        ),
        if (KritaLauncher.isBundled)
          Positioned(
            bottom: 44,
            left: 0,
            right: 0,
            child: Center(
              child: Text(
                KritaLauncher.badgeLabel,
                style: const TextStyle(
                  color: Color(0xA634C759),
                  fontSize: 10,
                  fontWeight: FontWeight.w700,
                  letterSpacing: 0.5,
                ),
              ),
            ),
          ),
      ],
    );
  }
}
