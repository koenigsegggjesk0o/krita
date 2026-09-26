// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// about_dialog.dart — Self-contained About / Help dialog for first-run
// self-diagnosis on Windows installs (v0.55-A).
//
// The dialog surfaces the four pieces of information a Windows user
// needs to file a useful bug report when the first install misbehaves:
//   * the app version (`v0.55.0`),
//   * the engine status (Real native Krita bridge vs. Fallback),
//   * the native library path that loaded (or '' on fallback),
//   * the crash-log file path + a button to view it,
//   * a link to GitHub Releases for re-download.
//
// The dialog is a pure function of its props (no host state, no FFI
// calls inside the build), so it is widget-testable in isolation.

import 'package:flutter/material.dart';
import 'package:flutter/services.dart';

import '../theme/feather_colors.dart';
import '../theme/feather_typography.dart';

/// The props for [FeatherAboutDialog]. Kept as a plain class so the
/// host can construct it from its engine + path state without holding
/// a BuildContext.
class FeatherAboutInfo {
  const FeatherAboutInfo({
    required this.appVersion,
    required this.appVersionLabel,
    required this.engineReal,
    required this.engineStatusText,
    required this.nativeLibPath,
    required this.engineVersionString,
    required this.crashLogPath,
    required this.gitHubReleasesUrl,
  });

  /// Full `major.minor.patch+build` version (matches pubspec).
  final String appVersion;

  /// Short `vX.Y.Z` label.
  final String appVersionLabel;

  /// True when the real native Krita bridge loaded.
  final bool engineReal;

  /// One-line human-readable engine status (e.g. "Real Krita bridge
  /// loaded" or "Fallback — native bridge not loaded").
  final String engineStatusText;

  /// The native library path/name that loaded, or '' on the fallback.
  final String nativeLibPath;

  /// The bridge self-identification string (e.g.
  /// "FeatherBridge/1.0 (Krita 6.0.4)" or
  /// "FeatherBridge-Fallback/1.0").
  final String engineVersionString;

  /// The crash-log file path (best-effort, may be null).
  final String? crashLogPath;

  /// GitHub Releases URL for re-download.
  final String gitHubReleasesUrl;
}

/// Shows the Feather-Krita About / Help dialog.
///
/// Call from the host:
/// ```dart
/// showDialog<void>(
///   context: context,
///   builder: (_) => FeatherAboutDialog(info: info),
/// );
/// ```
class FeatherAboutDialog extends StatelessWidget {
  const FeatherAboutDialog({
    super.key,
    required this.info,
    this.onCopyCrashLog,
    this.onViewCrashLog,
  });

  final FeatherAboutInfo info;

  /// Optional: copy the crash log to the clipboard. Wired by the host
  /// (which owns the [CrashLog.readAll] call).
  final VoidCallback? onCopyCrashLog;

  /// Optional: open the crash log file in the platform viewer.
  final VoidCallback? onViewCrashLog;

  @override
  Widget build(BuildContext context) {
    final palette = Theme.of(context).brightness == Brightness.dark
        ? FeatherColors.dark
        : FeatherColors.light;
    return AlertDialog(
      backgroundColor: palette.panelFill,
      shape: RoundedRectangleBorder(
        borderRadius: BorderRadius.circular(16),
        side: BorderSide(color: palette.panelBorder),
      ),
      title: Row(
        children: [
          Icon(Icons.info_outline_rounded,
              color: FeatherPalette.accentBlue, size: 22),
          const SizedBox(width: 10),
          Text(
            'About Feather-Krita',
            style: FeatherTypography.h1
                .copyWith(color: palette.textPrimary),
          ),
        ],
      ),
      content: ConstrainedBox(
        constraints: const BoxConstraints(maxWidth: 460),
        child: SingleChildScrollView(
          child: Column(
            mainAxisSize: MainAxisSize.min,
            crossAxisAlignment: CrossAxisAlignment.start,
            children: [
              _Row(
                label: 'Version',
                value: '${info.appVersionLabel}  (${info.appVersion})',
                palette: palette,
              ),
              const SizedBox(height: 10),
              _Row(
                label: 'Engine',
                value: info.engineStatusText,
                valueColor: info.engineReal
                    ? FeatherPalette.accentGreen
                    : FeatherPalette.accentOrange,
                palette: palette,
              ),
              const SizedBox(height: 10),
              _Row(
                label: 'Bridge',
                value: info.engineVersionString.isEmpty
                    ? '(unknown)'
                    : info.engineVersionString,
                palette: palette,
              ),
              const SizedBox(height: 10),
              _Row(
                label: 'Native lib',
                value: info.nativeLibPath.isEmpty
                    ? '(not loaded — fallback mode)'
                    : info.nativeLibPath,
                palette: palette,
              ),
              const SizedBox(height: 10),
              _Row(
                label: 'Crash log',
                value: info.crashLogPath ?? '(not yet resolved)',
                palette: palette,
              ),
              const SizedBox(height: 16),
              Text(
                'If the app behaves unexpectedly, copy the crash log '
                'and attach it to a GitHub issue. Re-download the '
                'latest build from GitHub Releases if the engine is '
                'in fallback mode after a fresh install.',
                style: FeatherTypography.caption
                    .copyWith(color: palette.textTertiary, height: 1.4),
              ),
              const SizedBox(height: 10),
              InkWell(
                onTap: () async {
                  await Clipboard.setData(
                      ClipboardData(text: info.gitHubReleasesUrl));
                  if (context.mounted) {
                    ScaffoldMessenger.of(context).showSnackBar(
                      const SnackBar(
                        content: Text('GitHub URL copied to clipboard'),
                        duration: Duration(seconds: 2),
                      ),
                    );
                  }
                },
                borderRadius: BorderRadius.circular(8),
                child: Padding(
                  padding: const EdgeInsets.symmetric(vertical: 6),
                  child: Row(
                    children: [
                      Icon(Icons.open_in_new_rounded,
                          size: 16, color: FeatherPalette.accentBlue),
                      const SizedBox(width: 6),
                      Expanded(
                        child: Text(
                          info.gitHubReleasesUrl,
                          style: FeatherTypography.caption.copyWith(
                            color: FeatherPalette.accentBlue,
                            decoration: TextDecoration.underline,
                          ),
                        ),
                      ),
                    ],
                  ),
                ),
              ),
            ],
          ),
        ),
      ),
      actions: [
        if (onCopyCrashLog != null)
          TextButton.icon(
            onPressed: onCopyCrashLog,
            icon: Icon(Icons.copy_rounded, size: 18, color: palette.textSecondary),
            label: Text('Copy crash log',
                style: TextStyle(color: palette.textSecondary)),
          ),
        TextButton(
          onPressed: () => Navigator.of(context).pop(),
          child: Text('Close',
              style: TextStyle(color: palette.textPrimary)),
        ),
      ],
    );
  }
}

class _Row extends StatelessWidget {
  const _Row({
    required this.label,
    required this.value,
    required this.palette,
    this.valueColor,
  });

  final String label;
  final String value;
  final FeatherPalette palette;
  final Color? valueColor;

  @override
  Widget build(BuildContext context) {
    return Row(
      crossAxisAlignment: CrossAxisAlignment.start,
      children: [
        SizedBox(
          width: 84,
          child: Text(
            label,
            style: FeatherTypography.caption
                .copyWith(color: palette.textTertiary),
          ),
        ),
        Expanded(
          child: SelectableText(
            value,
            style: FeatherTypography.caption.copyWith(
              color: valueColor ?? palette.textPrimary,
              fontWeight: FontWeight.w600,
            ),
          ),
        ),
      ],
    );
  }
}
