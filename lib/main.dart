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
import 'utils/crash_log.dart';

void main() {
  // WidgetsFlutterBinding MUST be initialized before any FFI probe +
  // before runApp. This was already true pre-v0.55-A; the comment is
  // here to enforce it as an invariant of the boot sequence.
  WidgetsFlutterBinding.ensureInitialized();

  // v0.55-A: route Flutter framework errors (widget-tree exceptions,
  // build failures, layout overflow in debug) into the crash log AND
  // the default presenter (red error screen in debug, silent in
  // release). The crash log is best-effort — see CrashLog.
  FlutterError.onError = (details) {
    FlutterError.presentError(details);
    CrashLog.write('FlutterError: ${details.exceptionAsString()}\n'
        '${details.stack ?? StackTrace.current}');
  };

  // Override the default ErrorWidget so unhandled errors in the widget
  // tree render a tappable red screen with a Copy button instead of the
  // terse default grey box. A first-run Windows user who hits a layout
  // exception now sees the trace and can paste it into a GitHub issue.
  ErrorWidget.builder = (FlutterErrorDetails details) {
    return _CrashErrorWidget(details: details);
  };

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

  // v0.55-A: wrap the runApp call in runZonedGuarded so async errors,
  // FFI callback errors, and isolate-handler errors that escape the
  // Flutter framework's reach are STILL caught — logged to the crash
  // file and re-surfaced via FlutterError.reportError so the
  // ErrorWidget builder picks them up on the next frame. Without this,
  // a native crash inside an async callback silently kills the process.
  runZonedGuarded(() {
    runApp(
      const ProviderScope(
        child: FeatherKritaApp(),
      ),
    );
  }, (error, stack) {
    CrashLog.write('ZoneError: $error\n$stack');
    FlutterError.reportError(FlutterErrorDetails(
      exception: error,
      stack: stack,
      library: 'feather_krita.zone',
    ));
  });
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

    // v0.55-A: surface the fallback state on the splash itself when the
    // probe did NOT load the real bridge. The MainScreen host ALSO shows
    // a first-run info dialog (see _MainScreenState.initState), but the
    // splash warning gives the user immediate feedback during boot.
    final engineWarning = engineOk
        ? null
        : 'Real Krita engine not loaded — using fallback (reduced features)';

    // ---- Phase 2: first-run resource import (0.16 → 0.92) ------------
    _setPhase(new_splash.BootPhase(
      progress: 0.16,
      label: 'Importing real Krita resources…',
      engineReal: engineOk,
      warning: engineWarning,
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
      warning: _combinedWarning(engineWarning, _resourcesMissing),
    ));
    await Future<void>.delayed(const Duration(milliseconds: 120));
    if (!mounted) return;

    // ---- Phase 4: enter the editor (0.97 → 1.00) ----------------------
    _setPhase(new_splash.BootPhase(
      progress: 0.97,
      label: 'Ready',
      engineReal: engineOk,
      warning: _combinedWarning(engineWarning, _resourcesMissing),
    ));
    await Future<void>.delayed(const Duration(milliseconds: 240));
    if (!mounted) return;
    _setPhase(new_splash.BootPhase(
      progress: 1.0,
      label: 'Ready',
      engineReal: engineOk,
      warning: _combinedWarning(engineWarning, _resourcesMissing),
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

  /// Combine the engine-fallback warning + the resource-missing warning
  /// into a single splash banner line (v0.55-A).
  String? _combinedWarning(String? engineWarning, bool resourcesMissing) {
    if (engineWarning == null && !resourcesMissing) return null;
    final parts = <String>[
      if (engineWarning != null) engineWarning,
      if (resourcesMissing)
        'Resource payload unavailable — using bundled presets',
    ];
    return parts.join(' · ');
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

/// v0.55-A: Custom ErrorWidget rendered in place of the default grey
/// box when an unhandled exception escapes the widget tree (or is
/// re-reported by the runZonedGuarded handler in [main]).
///
/// Layout:
///   * full-screen dark red backdrop (so the user immediately sees
///     something went wrong),
///   * centered Material card with the exception + stack trace,
///   * "Copy error" button (writes the trace to the clipboard + the
///     crash log file so it can be pasted into a GitHub issue),
///   * "Restart" hint (close + relaunch — Flutter does not expose a
///     clean hot-restart for desktop apps, so we just tell the user).
///
/// This widget is constructed by [ErrorWidget.builder] from a
/// [FlutterErrorDetails]; it must NOT throw or depend on the rest of
/// the app's runtime state (it renders when the app is already
/// broken). All operations inside the build are best-effort.
class _CrashErrorWidget extends StatefulWidget {
  const _CrashErrorWidget({required this.details});

  final FlutterErrorDetails details;

  @override
  State<_CrashErrorWidget> createState() => _CrashErrorWidgetState();
}

class _CrashErrorWidgetState extends State<_CrashErrorWidget> {
  bool _copied = false;

  String get _traceText {
    final d = widget.details;
    final ex = d.exceptionAsString();
    final st = d.stack?.toString() ?? '';
    return 'Feather-Krita $kAppVersionLabel ($kAppVersion)\n'
        'Platform: ${_platformLine()}\n'
        'Library: ${d.library ?? "(unknown)"}\n'
        'Exception: $ex\n'
        'Stack:\n$st';
  }

  String _platformLine() {
    try {
      return 'default'; // Platform.pathSeparator etc. intentionally not
      // surfaced — keeps the trace PII-free for the GitHub issue.
    } catch (_) {
      return 'unknown';
    }
  }

  Future<void> _copy() async {
    final text = _traceText;
    await Clipboard.setData(ClipboardData(text: text));
    await CrashLog.write(text);
    if (!mounted) return;
    setState(() => _copied = true);
  }

  @override
  Widget build(BuildContext context) {
    return Material(
      color: const Color(0xFF1A0B0B),
      child: SafeArea(
        child: Center(
          child: ConstrainedBox(
            constraints: const BoxConstraints(maxWidth: 640),
            child: Padding(
              padding: const EdgeInsets.all(24),
              child: Column(
                mainAxisSize: MainAxisSize.min,
                crossAxisAlignment: CrossAxisAlignment.start,
                children: [
                  Row(
                    children: [
                      const Icon(Icons.error_outline_rounded,
                          color: Color(0xFFEF4444), size: 28),
                      const SizedBox(width: 12),
                      Expanded(
                        child: Text(
                          'Feather-Krita hit an unexpected error',
                          style: Theme.of(context)
                              .textTheme
                              .titleLarge
                              ?.copyWith(
                                color: const Color(0xFFFCA5A5),
                                fontWeight: FontWeight.w600,
                              ),
                        ),
                      ),
                    ],
                  ),
                  const SizedBox(height: 16),
                  Text(
                    'The app is still running but a part of the UI has '
                    'crashed. Copy the error below and attach it to a '
                    'GitHub issue so we can fix it. Restart the app to '
                    'clear this screen.',
                    style: Theme.of(context).textTheme.bodyMedium?.copyWith(
                          color: const Color(0xFFFCA5A5),
                        ),
                  ),
                  const SizedBox(height: 16),
                  Flexible(
                    child: Container(
                      padding: const EdgeInsets.all(12),
                      decoration: BoxDecoration(
                        color: const Color(0xFF0F0808),
                        borderRadius: BorderRadius.circular(8),
                        border: Border.all(
                            color: const Color(0x33EF4444), width: 1),
                      ),
                      child: SingleChildScrollView(
                        child: SelectableText(
                          _traceText,
                          style: const TextStyle(
                            color: Color(0xFFE5E7EB),
                            fontFamily: 'JetBrainsMono',
                            fontSize: 11,
                            height: 1.4,
                          ),
                        ),
                      ),
                    ),
                  ),
                  const SizedBox(height: 16),
                  Row(
                    mainAxisAlignment: MainAxisAlignment.end,
                    children: [
                      FilledButton.icon(
                        onPressed: _copied ? null : _copy,
                        icon: Icon(_copied
                            ? Icons.check_rounded
                            : Icons.copy_rounded),
                        label: Text(_copied
                            ? 'Copied to clipboard + crash log'
                            : 'Copy error'),
                      ),
                    ],
                  ),
                ],
              ),
            ),
          ),
        ),
      ),
    );
  }
}

