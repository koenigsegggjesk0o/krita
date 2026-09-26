// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// crash_log.dart — Best-effort append-only crash log writer.
//
// Writes a single `feather_krita_crash.log` into the user's documents
// directory (resolved via path_provider) so a Windows user can attach
// it to a GitHub issue when the first-run install misbehaves.
//
// Design rules (HARD):
//   * EVERY operation is best-effort. A path_provider failure, a disk
//     write failure, or even a null documents directory must NEVER crash
//     the app — a crash logger that crashes is worse than no logger.
//   * The path is cached after the first successful resolution so the
//     About dialog can show it synchronously after boot.
//   * Each record is wrapped in a banner with a UTC timestamp so the
//     GitHub issue triage can correlate crashes with the boot probe +
//     resource-extract phases.
//
// This file is platform-agnostic: it works the same on Windows, Android,
// Linux, macOS. iOS is intentionally NOT special-cased (the brief says
// do not touch iOS — this file just uses path_provider's portable API).
//
// NOT a crash reporter: there is no network upload. The user opts in by
// opening the About dialog, viewing the log, and pasting it into a
// GitHub issue. This is a deliberate choice — the app is offline-first.

import 'dart:async';
import 'dart:io' show File, FileMode, Platform;

import 'package:path_provider/path_provider.dart';

/// Best-effort append-only crash log writer.
///
/// See the file header for the design rules.
class CrashLog {
  static const String _fileName = 'feather_krita_crash.log';

  /// Cached path after the first successful resolution. Null when the
  /// documents directory has not been resolved yet (or when it failed).
  static String? _cachedPath;

  /// Whether [path] has been resolved at least once (success or fail).
  /// Used by the About dialog to distinguish "still resolving" from
  /// "resolved but path_provider failed".
  static bool _resolved = false;

  /// Resolves the crash log file path. Returns null when the documents
  /// directory cannot be resolved (path_provider missing on the
  /// platform, or the platform raises). Cached after the first call.
  static Future<String?> path() async {
    if (_resolved) return _cachedPath;
    try {
      final dir = await getApplicationDocumentsDirectory();
      final file = File('${dir.path}${Platform.pathSeparator}$_fileName');
      _cachedPath = file.path;
      _resolved = true;
      return _cachedPath;
    } catch (_) {
      _resolved = true;
      return null;
    }
  }

  /// The cached path (synchronous), or null when not yet resolved or
  /// when the resolution failed. Used by the About dialog.
  static String? cachedPath() => _cachedPath;

  /// Appends a crash record with a UTC-timestamped banner. Best effort:
  /// any I/O failure is swallowed.
  static Future<void> write(String record) async {
    final p = await path();
    if (p == null) return;
    try {
      final file = File(p);
      final sink = file.openWrite(mode: FileMode.append);
      final now = DateTime.now().toUtc().toIso8601String();
      sink
        ..writeln(
            '========================================================')
        ..writeln('FEATHER-KRITA CRASH — $now')
        ..writeln(
            '========================================================')
        ..writeln(record)
        ..writeln();
      await sink.flush();
      await sink.close();
    } catch (_) {
      // Swallow — never crash from the crash logger.
    }
  }

  /// Reads the entire crash log (for the About dialog's "view log"
  /// button), or null when the file does not exist or cannot be read.
  static Future<String?> readAll() async {
    final p = await path();
    if (p == null) return null;
    try {
      final file = File(p);
      if (!file.existsSync()) return null;
      return file.readAsString();
    } catch (_) {
      return null;
    }
  }
}
