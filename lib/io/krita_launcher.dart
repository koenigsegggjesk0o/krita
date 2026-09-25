// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// krita_launcher.dart — Locates and launches the FULL official Krita
// application bundled with the Feather package.
//
// The Windows package ships the complete, UNMODIFIED official Krita
// 6.0.4 portable install (downloaded from krita.org at build time,
// SHA256-pinned by CI) inside `krita/` next to the Feather executable.
// This helper finds it and launches the real krita.exe — the 100%
// original application, every file exactly as KDE ships it. Nothing is
// patched, wrapped or modified.
//
// Linux / macOS: when a system Krita install is detected in a
// well-known location (/usr/bin/krita, /Applications/krita.app, ...) it
// is launched instead. No files are modified in either case.

import 'dart:io';

/// Outcome of one [KritaLauncher.probe] run.
class KritaProbeResult {
  const KritaProbeResult({
    required this.path,
    required this.bundled,
    required this.launchable,
  });

  /// Absolute path of the discovered Krita executable, or `null` when
  /// no candidate was found.
  final String? path;

  /// True when [path] points at the bundled official Krita install
  /// (the Windows `krita/bin/krita.exe`). False for system installs.
  final bool bundled;

  /// True when the file at [path] exists and is executable.
  final bool launchable;

  /// A short honest label for badges (mirrors the legacy
  /// [KritaLauncher.badgeLabel] when bundled, otherwise describes the
  /// system install).
  String get label => bundled
      ? 'FULL KRITA 6.0.4 BUNDLED'
      : (path != null ? 'SYSTEM KRITA' : 'NO KRITA');
}

/// Locates and launches the bundled full official Krita application.
class KritaLauncher {
  /// Absolute path of the bundled `krita.exe` (Windows), when the full
  /// official install is present next to the Feather executable.
  static String? bundledKritaExePath() => _bundledKritaExePath();

  static String? _bundledKritaExePath() {
    if (Platform.isWindows) {
      final exe = Platform.resolvedExecutable;
      final dir = File(exe).parent.path;
      final sep = Platform.pathSeparator;
      final base = dir.endsWith(sep) ? dir : '$dir$sep';
      final candidate = File('${base}krita${sep}bin${sep}krita.exe');
      return candidate.existsSync() ? candidate.path : null;
    }
    if (Platform.isLinux) {
      // Portable AppImage / opt install shipped alongside Feather.
      final exe = Platform.resolvedExecutable;
      final dir = File(exe).parent.path;
      final sep = Platform.pathSeparator;
      final base = dir.endsWith(sep) ? dir : '$dir$sep';
      for (final rel in const <String>[
        'krita/bin/krita',
        'krita/krita',
        'krita.AppImage',
      ]) {
        final candidate = File('$base$rel');
        if (candidate.existsSync()) return candidate.path;
      }
    }
    if (Platform.isMacOS) {
      final exe = Platform.resolvedExecutable;
      final dir = File(exe).parent.path;
      final sep = Platform.pathSeparator;
      final base = dir.endsWith(sep) ? dir : '$dir$sep';
      final candidate =
          Directory('${base}krita.app${sep}Contents${sep}MacOS');
      if (candidate.existsSync()) {
        final inner = Directory('${candidate.path}');
        for (final f in inner.listSync()) {
          if (f is File && f.path.endsWith('krita')) return f.path;
        }
      }
    }
    return null;
  }

  /// Locates a system-installed Krita executable (non-bundled).
  /// Returns `null` when no system Krita is detected.
  static String? systemKritaExePath() {
    if (Platform.isWindows) return null;
    if (Platform.isLinux) {
      for (final p in const <String>['/usr/bin/krita', '/usr/local/bin/krita']) {
        if (File(p).existsSync()) return p;
      }
    }
    if (Platform.isMacOS) {
      const p = '/Applications/krita.app/Contents/MacOS/krita';
      if (File(p).existsSync()) return p;
    }
    return null;
  }

  /// Whether the full official Krita application is bundled here.
  static bool get isBundled => bundledKritaExePath() != null;

  /// A short honest label describing the bundled install (for badges).
  static String get badgeLabel => 'FULL KRITA 6.0.4 BUNDLED';

  /// Probes the host for a Krita install (bundled first, system
  /// second). Returns a [KritaProbeResult] describing what was found.
  static KritaProbeResult probe() {
    final bundledPath = bundledKritaExePath();
    if (bundledPath != null) {
      return KritaProbeResult(
        path: bundledPath,
        bundled: true,
        launchable: File(bundledPath).existsSync(),
      );
    }
    final systemPath = systemKritaExePath();
    if (systemPath != null) {
      return KritaProbeResult(
        path: systemPath,
        bundled: false,
        launchable: File(systemPath).existsSync(),
      );
    }
    return const KritaProbeResult(path: null, bundled: false, launchable: false);
  }

  /// Launches the real krita executable (detached — Feather stays usable).
  ///
  /// Returns false when no Krita install is present or the OS refused
  /// to start the process.
  static Future<bool> launch() async {
    final exe = bundledKritaExePath() ?? systemKritaExePath();
    if (exe == null) return false;
    try {
      await Process.start(exe, const <String>[],
          mode: ProcessStartMode.detached);
      return true;
    } catch (_) {
      return false;
    }
  }
}
