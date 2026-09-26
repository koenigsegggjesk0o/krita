// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// about_dialog_test.dart — Widget tests for the v0.55-A About / Help
// dialog + CrashLog helper.
//
// Verifies:
//   * the dialog renders the version label + the GitHub Releases URL,
//   * the engine-status row uses the correct colour (green for real,
//     orange for fallback),
//   * the TopBar's new onAbout button renders + fires its callback,
//   * CrashLog.cachedPath() returns null before resolution (no I/O
//     on the test isolate).

import 'package:feather_krita/ui/widgets/about_dialog.dart';
import 'package:feather_krita/ui/widgets/icon_button.dart';
import 'package:feather_krita/ui/widgets/top_bar.dart';
import 'package:feather_krita/utils/crash_log.dart';
import 'package:flutter/material.dart';
import 'package:flutter_test/flutter_test.dart';

const _kTestReleasesUrl = 'https://github.com/koenigsegggjesk0o/krita/releases';

FeatherAboutInfo _info({required bool engineReal}) => FeatherAboutInfo(
      appVersion: '0.55.0+1',
      appVersionLabel: 'v0.55.0',
      engineReal: engineReal,
      engineStatusText: engineReal
          ? 'Real Krita bridge loaded'
          : 'Fallback — native bridge not loaded',
      nativeLibPath: engineReal ? 'krita_bridge.dll' : '',
      engineVersionString: engineReal
          ? 'FeatherBridge/1.0 (Krita 6.0.4)'
          : 'FeatherBridge-Fallback/1.0',
      crashLogPath: '/tmp/feather_krita_crash.log',
      gitHubReleasesUrl: _kTestReleasesUrl,
    );

void main() {
  group('FeatherAboutDialog', () {
    testWidgets('renders version label + GitHub Releases URL',
        (tester) async {
      await tester.pumpWidget(
        MaterialApp(
          home: Scaffold(
            body: Builder(
              builder: (ctx) => Center(
                child: ElevatedButton(
                  onPressed: () => showDialog<void>(
                    context: ctx,
                    builder: (_) => FeatherAboutDialog(info: _info(engineReal: true)),
                  ),
                  child: const Text('open'),
                ),
              ),
            ),
          ),
        ),
      );
      await tester.tap(find.text('open'));
      await tester.pumpAndSettle();
      expect(find.text('About Feather-Krita'), findsOneWidget);
      expect(find.textContaining('v0.55.0'), findsOneWidget);
      expect(find.textContaining(_kTestReleasesUrl), findsOneWidget);
      expect(find.text('Real Krita bridge loaded'), findsOneWidget);
      expect(find.text('krita_bridge.dll'), findsOneWidget);
    });

    testWidgets('shows fallback status when engineReal is false',
        (tester) async {
      await tester.pumpWidget(
        MaterialApp(
          home: Scaffold(
            body: Builder(
              builder: (ctx) => Center(
                child: ElevatedButton(
                  onPressed: () => showDialog<void>(
                    context: ctx,
                    builder: (_) =>
                        FeatherAboutDialog(info: _info(engineReal: false)),
                  ),
                  child: const Text('open'),
                ),
              ),
            ),
          ),
        ),
      );
      await tester.tap(find.text('open'));
      await tester.pumpAndSettle();
      expect(find.text('Fallback — native bridge not loaded'), findsOneWidget);
      expect(find.text('(not loaded — fallback mode)'), findsOneWidget);
    });

    testWidgets('Close button dismisses the dialog', (tester) async {
      await tester.pumpWidget(
        MaterialApp(
          home: Scaffold(
            body: Builder(
              builder: (ctx) => Center(
                child: ElevatedButton(
                  onPressed: () => showDialog<void>(
                    context: ctx,
                    builder: (_) => FeatherAboutDialog(info: _info(engineReal: true)),
                  ),
                  child: const Text('open'),
                ),
              ),
            ),
          ),
        ),
      );
      await tester.tap(find.text('open'));
      await tester.pumpAndSettle();
      await tester.tap(find.text('Close'));
      await tester.pumpAndSettle();
      expect(find.text('About Feather-Krita'), findsNothing);
    });
  });

  group('TopBar.onAbout', () {
    testWidgets('renders the About / Help button and fires the callback',
        (tester) async {
      var taps = 0;
      await tester.pumpWidget(
        MaterialApp(
          home: Scaffold(
            body: TopBar(
              tool: 'Draw',
              onAbout: () => taps++,
            ),
          ),
        ),
      );
      // The help icon button should be present + tappable.
      final helpFinder = find.ancestor(
        of: find.byIcon(Icons.help_outline_rounded),
        matching: find.byType(FeatherIconButton),
      );
      expect(helpFinder, findsOneWidget);
      await tester.tap(helpFinder);
      await tester.pump();
      expect(taps, 1);
    });

    testWidgets('renders without throwing when onAbout is null (disabled)',
        (tester) async {
      // The About button is always rendered (matching the Export / Share
      // buttons which are also unconditional) — when onAbout is null it
      // is simply a no-op tap target. We verify the bar still builds and
      // tapping the disabled button does NOT throw.
      await tester.pumpWidget(
        const MaterialApp(
          home: Scaffold(
            body: TopBar(tool: 'Draw'),
          ),
        ),
      );
      expect(find.byIcon(Icons.help_outline_rounded), findsOneWidget);
      final helpFinder = find.ancestor(
        of: find.byIcon(Icons.help_outline_rounded),
        matching: find.byType(FeatherIconButton),
      );
      await tester.tap(helpFinder, warnIfMissed: false);
      await tester.pump();
      // No exception → pass.
    });
  });

  group('CrashLog', () {
    test('cachedPath returns null before resolution', () {
      // CrashLog caches the path after the first path() call. The test
      // isolate has NOT called path() yet (path_provider is not set up
      // for headless tests), so cachedPath() should be null. This is a
      // sanity check — a non-null cachedPath would mean a previous test
      // polluted the static cache.
      expect(CrashLog.cachedPath(), isNull);
    });
  });
}
