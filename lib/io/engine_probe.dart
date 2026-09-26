// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// engine_probe.dart — Hang-proof pre-flight check for the native Krita
// bridge.
//
// [EditorState] constructs the [KritaBrushEngine] synchronously on the UI
// isolate. If `DynamicLibrary.open` blocks on the user's machine (a DLL
// whose load path deadlocks — the class of bug that froze the old build
// at a fake "100%" screen), the whole app freezes with no way out.
//
// This probe opens the same candidate libraries inside a THROWAWAY
// isolate with a hard timeout:
//   - probe finishes true  → the library loads cleanly; the editor may
//     construct the real engine on the UI isolate.
//   - probe finishes false → the library is absent; the editor runs on
//     the honest synthetic-dab fallback (no behavior change vs. today).
//   - probe TIMES OUT      → the library load is wedged on this machine;
//     the editor skips the native engine for this session (the app stays
//     usable; next launch retries).
//
// The candidate lists mirror `krita_bindings.dart` exactly — the probe
// must accept exactly what the real loader would accept.
//
// Refactor (feather-state-data-export): the probe now returns a
// [KritaProbeOutcome] that carries the candidate path that loaded (when
// any) plus a short outcome reason, so the boot screen can surface WHY
// the engine was skipped instead of just a boolean. The legacy
// [probeKritaEngine] entry-point stays a boolean for backwards
// compatibility; the new [probeKritaEngineDetailed] is the richer form.

import 'dart:async';
import 'dart:ffi' show DynamicLibrary;
import 'dart:io' show OSError, Platform;
import 'dart:isolate';

/// Why the probe returned its outcome.
enum KritaProbeReason {
  /// The library loaded cleanly on the probe isolate.
  loaded,

  /// No candidate library was found on this platform.
  notFound,

  /// A candidate was found but `DynamicLibrary.open` raised an error.
  openFailed,

  /// The probe isolate did not finish within the timeout. The library
  /// load is wedged on this machine — the editor should skip the
  /// native engine for this session.
  timedOut,

  /// Mobile platforms (Android / iOS) bundle the library inside the
  /// app package, so the probe short-circuits to `true` without
  /// touching the filesystem.
  bundledMobile,
}

/// Outcome of a [probeKritaEngineDetailed] run.
class KritaProbeOutcome {
  const KritaProbeOutcome({
    required this.success,
    required this.reason,
    this.candidatePath,
    this.error,
  });

  /// True when the editor may construct the real [KritaBrushEngine]
  /// on the UI isolate.
  final bool success;

  /// Why the probe returned this outcome.
  final KritaProbeReason reason;

  /// The candidate path that loaded (when [reason] == loaded).
  final String? candidatePath;

  /// The error message when [reason] == openFailed.
  final String? error;

  /// Legacy boolean view: `true` when [success].
  bool toBool() => success;
}

/// Probes whether the native Krita bridge loads cleanly on this machine.
///
/// Android and iOS bundle the library inside the package and the OS
/// loader resolves it without the filesystem candidates, so the probe
/// short-circuits to `true` there (a missing bundled library still fails
/// fast inside [EditorState]'s own try/catch — the hang class this probe
/// guards against does not apply to bundled libraries).
Future<bool> probeKritaEngine(
    {Duration timeout = const Duration(seconds: 12)}) async {
  return (await probeKritaEngineDetailed(timeout: timeout)).toBool();
}

/// Richer variant of [probeKritaEngine]: returns a [KritaProbeOutcome]
/// carrying the reason and the candidate path that loaded.
Future<KritaProbeOutcome> probeKritaEngineDetailed({
  Duration timeout = const Duration(seconds: 12),
}) async {
  if (Platform.isAndroid || Platform.isIOS) {
    return const KritaProbeOutcome(
      success: true,
      reason: KritaProbeReason.bundledMobile,
    );
  }
  if (!Platform.isWindows && !Platform.isLinux && !Platform.isMacOS) {
    return const KritaProbeOutcome(
      success: false,
      reason: KritaProbeReason.notFound,
    );
  }
  try {
    return await Isolate.run(_tryOpenDetailed).timeout(
      timeout,
      onTimeout: () => const KritaProbeOutcome(
        success: false,
        reason: KritaProbeReason.timedOut,
      ),
    );
  } on TimeoutException {
    return const KritaProbeOutcome(
      success: false,
      reason: KritaProbeReason.timedOut,
    );
  } catch (e) {
    return KritaProbeOutcome(
      success: false,
      reason: KritaProbeReason.openFailed,
      error: e.toString(),
    );
  }
}

KritaProbeOutcome _tryOpenDetailed() {
  for (final candidate in _candidates()) {
    try {
      DynamicLibrary.open(candidate);
      return KritaProbeOutcome(
        success: true,
        reason: KritaProbeReason.loaded,
        candidatePath: candidate,
      );
    } on ArgumentError {
      continue;
    } on OSError {
      continue;
    }
  }
  return const KritaProbeOutcome(
    success: false,
    reason: KritaProbeReason.notFound,
  );
}

List<String> _candidates() {
  // Install-dir candidate, resolved relative to the running executable's
  // directory via [Platform.resolvedExecutable]. On Windows a Start Menu
  // shortcut sets the process CWD to system32 (or the user's home), so the
  // CWD-relative candidates below all fail even when `krita_bridge.dll`
  // sits right next to `feather_krita.exe`. The resolvedExecutable path
  // does NOT depend on CWD, so it loads reliably from any launch context
  // (Start Menu, file explorer double-click, scheduled task, etc.). It is
  // listed FIRST so it wins when both an install-dir copy and a stray
  // CWD-relative copy exist; the legacy CWD-relative candidates below stay
  // as fallbacks for dev runs where CWD == the repo root.
  //
  // Implementation note: `package:path` is intentionally NOT a dependency
  // (pubspec.yaml declares only `path_provider`), so we derive the
  // executable's parent directory with a simple last-separator split that
  // handles both `/` (POSIX) and `\` (Windows) — `Platform.resolvedExecutable`
  // is normalised to the platform's native separator already.
  final exePath = Platform.resolvedExecutable;
  final lastSep = exePath.lastIndexOf(RegExp(r'[/\\]'));
  final exeDir = lastSep >= 0 ? exePath.substring(0, lastSep) : exePath;
  final sep = Platform.isWindows ? r'\' : '/';
  if (Platform.isWindows) {
    return <String>[
      '$exeDir${sep}krita_bridge.dll',
      'krita_bridge.dll',
      'lib\\krita_bridge.dll',
      '.\\krita_bridge.dll',
    ];
  }
  if (Platform.isLinux) {
    return <String>[
      '$exeDir${sep}libkrita_bridge.so',
      'lib/libkrita_bridge.so',
      'libkrita_bridge.so',
      './libkrita_bridge.so',
      'assets/native/linux/libkrita_bridge.so',
      'assets/native/libkrita_bridge.so',
      '../assets/native/linux/libkrita_bridge.so',
    ];
  }
  if (Platform.isMacOS) {
    return <String>[
      '$exeDir${sep}libkrita_bridge.dylib',
      'libkrita_bridge.dylib',
    ];
  }
  return const <String>[];
}
