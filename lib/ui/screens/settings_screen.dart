// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// settings_screen.dart — Settings screen.
//
// Per interfaceandgestures_interface.txt "Options" + home_settings.txt:
//   * Preference  — dark mode, hide UI, activate rendering when
//                   opening notes,
//   * Cursor      — show touch cursor, show pen cursor, color, hover,
//   * Language    — picker (restarts app on change),
//   * Data mgmt   — export / import notes,
//   * Help        — support, tutorial, bug report,
//   * About       — version, socials, restore examples,
//   * Profile     — sign in/out, profile picture, nickname.
//
// The screen is a scrollable list of glassmorphism "cards" — each card
// is a logical group. Tapping a setting triggers the matching callback
// so the parent decides how to persist it (SharedPreferences in the
// production build).

import 'package:flutter/material.dart';

import '../theme/feather_animations.dart';
import '../theme/feather_colors.dart';
import '../theme/feather_typography.dart';
import '../theme/glassmorphism.dart';
import '../widgets/glass_panel.dart';
import '../widgets/icon_button.dart';

class SettingsState {
  const SettingsState({
    this.darkMode = true,
    this.hideUi = false,
    this.activateRenderOnOpen = false,
    this.showTouchCursor = false,
    this.showPenCursor = true,
    this.cursorColor = const Color(0xFF60A5FA),
    this.enableHover = false,
    this.language = 'English',
    this.profileNickname = 'Artist',
    this.signedIn = false,
  });

  final bool darkMode;
  final bool hideUi;
  final bool activateRenderOnOpen;
  final bool showTouchCursor;
  final bool showPenCursor;
  final Color cursorColor;
  final bool enableHover;
  final String language;
  final String profileNickname;
  final bool signedIn;

  SettingsState copyWith({
    bool? darkMode,
    bool? hideUi,
    bool? activateRenderOnOpen,
    bool? showTouchCursor,
    bool? showPenCursor,
    Color? cursorColor,
    bool? enableHover,
    String? language,
    String? profileNickname,
    bool? signedIn,
  }) =>
      SettingsState(
        darkMode: darkMode ?? this.darkMode,
        hideUi: hideUi ?? this.hideUi,
        activateRenderOnOpen: activateRenderOnOpen ?? this.activateRenderOnOpen,
        showTouchCursor: showTouchCursor ?? this.showTouchCursor,
        showPenCursor: showPenCursor ?? this.showPenCursor,
        cursorColor: cursorColor ?? this.cursorColor,
        enableHover: enableHover ?? this.enableHover,
        language: language ?? this.language,
        profileNickname: profileNickname ?? this.profileNickname,
        signedIn: signedIn ?? this.signedIn,
      );
}

class SettingsScreen extends StatefulWidget {
  const SettingsScreen({
    super.key,
    required this.state,
    this.appVersion = '0.50.0',
    this.onChange,
    this.onBack,
    this.onExportAll,
    this.onImport,
    this.onSupportCenter,
    this.onBasicTutorial,
    this.onBugReport,
    this.onRestoreExamples,
    this.onSignIn,
    this.onSignOut,
    this.onEditProfile,
  });

  final SettingsState state;
  final String appVersion;
  final ValueChanged<SettingsState>? onChange;
  final VoidCallback? onBack;
  final VoidCallback? onExportAll;
  final VoidCallback? onImport;
  final VoidCallback? onSupportCenter;
  final VoidCallback? onBasicTutorial;
  final VoidCallback? onBugReport;
  final VoidCallback? onRestoreExamples;
  final VoidCallback? onSignIn;
  final VoidCallback? onSignOut;
  final VoidCallback? onEditProfile;

  @override
  State<SettingsScreen> createState() => _SettingsScreenState();
}

class _SettingsScreenState extends State<SettingsScreen> {
  late SettingsState _s = widget.state;

  void _update(SettingsState next) {
    setState(() => _s = next);
    widget.onChange?.call(next);
  }

  @override
  Widget build(BuildContext context) {
    final palette = _palette(context);
    return Scaffold(
      backgroundColor: palette.appBackground,
      body: SafeArea(
        child: Stack(
          children: [
            ListView(
              padding: const EdgeInsets.fromLTRB(24, 16, 24, 80),
              children: [
                _Header(
                  palette: palette,
                  onBack: widget.onBack,
                ),
                const SizedBox(height: 12),
                _ProfileCard(
                  palette: palette,
                  state: _s,
                  onEdit: widget.onEditProfile,
                  onSignIn: widget.onSignIn,
                  onSignOut: widget.onSignOut,
                ),
                const SizedBox(height: 14),
                _SectionTitle(palette: palette, text: 'PREFERENCE'),
                _Card(palette: palette, children: [
                  _SwitchRow(
                    palette: palette,
                    icon: Icons.dark_mode_outlined,
                    label: 'Dark mode',
                    active: _s.darkMode,
                    onChanged: (v) => _update(_s.copyWith(darkMode: v)),
                  ),
                  _Divider(palette: palette),
                  _SwitchRow(
                    palette: palette,
                    icon: Icons.visibility_off_outlined,
                    label: 'Hide UI',
                    active: _s.hideUi,
                    onChanged: (v) => _update(_s.copyWith(hideUi: v)),
                  ),
                  _Divider(palette: palette),
                  _SwitchRow(
                    palette: palette,
                    icon: Icons.wb_incandescent_outlined,
                    label: 'Activate rendering when opening notes',
                    active: _s.activateRenderOnOpen,
                    onChanged: (v) => _update(
                        _s.copyWith(activateRenderOnOpen: v)),
                  ),
                ]),
                const SizedBox(height: 14),
                _SectionTitle(palette: palette, text: 'CURSOR'),
                _Card(palette: palette, children: [
                  _SwitchRow(
                    palette: palette,
                    icon: Icons.touch_app_outlined,
                    label: 'Show touch cursor',
                    active: _s.showTouchCursor,
                    onChanged: (v) =>
                        _update(_s.copyWith(showTouchCursor: v)),
                  ),
                  _Divider(palette: palette),
                  _SwitchRow(
                    palette: palette,
                    icon: Icons.edit_outlined,
                    label: 'Show pen cursor',
                    active: _s.showPenCursor,
                    onChanged: (v) =>
                        _update(_s.copyWith(showPenCursor: v)),
                  ),
                  _Divider(palette: palette),
                  _SwitchRow(
                    palette: palette,
                    icon: Icons.sensors_outlined,
                    label: 'Hover (M2+)',
                    active: _s.enableHover,
                    onChanged: (v) =>
                        _update(_s.copyWith(enableHover: v)),
                  ),
                  _Divider(palette: palette),
                  _ColorRow(
                    palette: palette,
                    label: 'Cursor color',
                    color: _s.cursorColor,
                    onPick: (c) => _update(_s.copyWith(cursorColor: c)),
                  ),
                ]),
                const SizedBox(height: 14),
                _SectionTitle(palette: palette, text: 'LANGUAGE'),
                _Card(palette: palette, children: [
                  _LanguageRow(
                    palette: palette,
                    current: _s.language,
                    onPick: (l) => _update(_s.copyWith(language: l)),
                  ),
                ]),
                const SizedBox(height: 14),
                _SectionTitle(palette: palette, text: 'DATA MANAGEMENT'),
                _Card(palette: palette, children: [
                  _ActionRow(
                    palette: palette,
                    icon: Icons.upload_outlined,
                    label: 'Export all notes',
                    onTap: widget.onExportAll,
                  ),
                  _Divider(palette: palette),
                  _ActionRow(
                    palette: palette,
                    icon: Icons.download_outlined,
                    label: 'Import notes',
                    onTap: widget.onImport,
                  ),
                ]),
                const SizedBox(height: 14),
                _SectionTitle(palette: palette, text: 'HELP'),
                _Card(palette: palette, children: [
                  _ActionRow(
                    palette: palette,
                    icon: Icons.help_outline,
                    label: 'Support center',
                    onTap: widget.onSupportCenter,
                  ),
                  _Divider(palette: palette),
                  _ActionRow(
                    palette: palette,
                    icon: Icons.play_circle_outline,
                    label: 'Basic tutorial',
                    onTap: widget.onBasicTutorial,
                  ),
                  _Divider(palette: palette),
                  _ActionRow(
                    palette: palette,
                    icon: Icons.bug_report_outlined,
                    label: 'Bug report',
                    onTap: widget.onBugReport,
                  ),
                ]),
                const SizedBox(height: 14),
                _SectionTitle(palette: palette, text: 'ABOUT'),
                _Card(palette: palette, children: [
                  _InfoRow(
                    palette: palette,
                    label: 'Version',
                    value: widget.appVersion,
                  ),
                  _Divider(palette: palette),
                  _ActionRow(
                    palette: palette,
                    icon: Icons.restore_outlined,
                    label: 'Restore example notes',
                    onTap: widget.onRestoreExamples,
                  ),
                ]),
                const SizedBox(height: 24),
                Text(
                  'GPL-2.0-or-later · Feather-Krita Contributors',
                  textAlign: TextAlign.center,
                  style: FeatherTypography.micro
                      .copyWith(color: palette.textTertiary),
                ),
              ],
            ),
          ],
        ),
      ),
    );
  }

  FeatherPalette _palette(BuildContext context) =>
      Theme.of(context).brightness == Brightness.dark
          ? FeatherColors.dark
          : FeatherColors.light;
}

class _Header extends StatelessWidget {
  const _Header({required this.palette, required this.onBack});
  final FeatherPalette palette;
  final VoidCallback? onBack;

  @override
  Widget build(BuildContext context) {
    return Row(
      children: [
        FeatherIconButton(
          icon: Icons.arrow_back_rounded,
          tooltip: 'Back',
          size: 40,
          iconSize: 20,
          onTap: onBack,
        ),
        const SizedBox(width: 10),
        Text(
          'Settings',
          style: FeatherTypography.h1.copyWith(color: palette.textPrimary),
        ),
      ],
    );
  }
}

class _ProfileCard extends StatelessWidget {
  const _ProfileCard({
    required this.palette,
    required this.state,
    this.onEdit,
    this.onSignIn,
    this.onSignOut,
  });

  final FeatherPalette palette;
  final SettingsState state;
  final VoidCallback? onEdit;
  final VoidCallback? onSignIn;
  final VoidCallback? onSignOut;

  @override
  Widget build(BuildContext context) {
    return GlassPanel(
      spec: GlassSpec.panel,
      padding: const EdgeInsets.all(14),
      child: Row(
        children: [
          CircleAvatar(
            radius: 22,
            backgroundColor: FeatherPalette.accentBlue,
            child: const Icon(Icons.person, color: Colors.white, size: 22),
          ),
          const SizedBox(width: 12),
          Expanded(
            child: Column(
              crossAxisAlignment: CrossAxisAlignment.start,
              children: [
                Text(
                  state.profileNickname,
                  style: FeatherTypography.h2
                      .copyWith(color: palette.textPrimary),
                ),
                Text(
                  state.signedIn ? 'Signed in' : 'Not signed in',
                  style: FeatherTypography.caption
                      .copyWith(color: palette.textTertiary),
                ),
              ],
            ),
          ),
          FeatherIconButton(
            icon: Icons.edit_outlined,
            tooltip: 'Edit profile',
            size: 36,
            iconSize: 16,
            onTap: onEdit,
          ),
          const SizedBox(width: 6),
          FeatherIconButton(
            icon: state.signedIn
                ? Icons.logout_rounded
                : Icons.login_rounded,
            tooltip: state.signedIn ? 'Sign out' : 'Sign in',
            size: 36,
            iconSize: 16,
            color: state.signedIn ? FeatherColors.toolErase : palette.accent,
            onTap: state.signedIn ? onSignOut : onSignIn,
          ),
        ],
      ),
    );
  }
}

class _SectionTitle extends StatelessWidget {
  const _SectionTitle({required this.palette, required this.text});
  final FeatherPalette palette;
  final String text;

  @override
  Widget build(BuildContext context) {
    return Padding(
      padding: const EdgeInsets.only(left: 4, bottom: 6),
      child: Text(
        text,
        style: FeatherTypography.micro
            .copyWith(color: palette.textTertiary, letterSpacing: 1.4),
      ),
    );
  }
}

class _Card extends StatelessWidget {
  const _Card({required this.palette, required this.children});
  final FeatherPalette palette;
  final List<Widget> children;

  @override
  Widget build(BuildContext context) {
    return GlassPanel(
      spec: GlassSpec.subtle,
      padding: const EdgeInsets.symmetric(horizontal: 12, vertical: 4),
      child: Column(children: children),
    );
  }
}

class _Divider extends StatelessWidget {
  const _Divider({required this.palette});
  final FeatherPalette palette;

  @override
  Widget build(BuildContext context) =>
      Divider(height: 1, color: palette.divider);
}

class _SwitchRow extends StatelessWidget {
  const _SwitchRow({
    required this.palette,
    required this.icon,
    required this.label,
    required this.active,
    required this.onChanged,
  });

  final FeatherPalette palette;
  final IconData icon;
  final String label;
  final bool active;
  final ValueChanged<bool> onChanged;

  @override
  Widget build(BuildContext context) {
    return Padding(
      padding: const EdgeInsets.symmetric(vertical: 8),
      child: Row(
        children: [
          Icon(icon, size: 18, color: palette.textSecondary),
          const SizedBox(width: 10),
          Expanded(
            child: Text(
              label,
              style: FeatherTypography.body
                  .copyWith(color: palette.textPrimary),
            ),
          ),
          Switch.adaptive(
            value: active,
            activeTrackColor: palette.accent,
            onChanged: onChanged,
          ),
        ],
      ),
    );
  }
}

class _ColorRow extends StatelessWidget {
  const _ColorRow({
    required this.palette,
    required this.label,
    required this.color,
    required this.onPick,
  });

  final FeatherPalette palette;
  final String label;
  final Color color;
  final ValueChanged<Color> onPick;

  static const _swatches = [
    Color(0xFF60A5FA),
    Color(0xFFA78BFA),
    Color(0xFFF472B6),
    Color(0xFFFB923C),
    Color(0xFF34D399),
    Color(0xFFFACC15),
    Color(0xFFFFFFFF),
  ];

  @override
  Widget build(BuildContext context) {
    return Padding(
      padding: const EdgeInsets.symmetric(vertical: 8),
      child: Row(
        children: [
          Icon(Icons.palette_outlined, size: 18, color: palette.textSecondary),
          const SizedBox(width: 10),
          Expanded(
            child: Text(
              label,
              style: FeatherTypography.body
                  .copyWith(color: palette.textPrimary),
            ),
          ),
          for (final c in _swatches) ...[
            const SizedBox(width: 6),
            GestureDetector(
              onTap: () => onPick(c),
              child: AnimatedContainer(
                duration: FeatherDurations.quick,
                curve: FeatherCurves.toggle,
                width: c == color ? 22 : 18,
                height: c == color ? 22 : 18,
                decoration: BoxDecoration(
                  shape: BoxShape.circle,
                  color: c,
                  border: Border.all(
                    color: c == color
                        ? palette.textPrimary
                        : palette.panelBorder,
                    width: c == color ? 2 : 1,
                  ),
                ),
              ),
            ),
          ],
        ],
      ),
    );
  }
}

class _LanguageRow extends StatelessWidget {
  const _LanguageRow({
    required this.palette,
    required this.current,
    required this.onPick,
  });

  final FeatherPalette palette;
  final String current;
  final ValueChanged<String> onPick;

  static const _languages = [
    'English',
    'Bahasa Indonesia',
    '日本語',
    '한국어',
    '中文 (简体)',
    'Español',
    'Français',
    'Deutsch',
  ];

  @override
  Widget build(BuildContext context) {
    return Padding(
      padding: const EdgeInsets.symmetric(vertical: 8),
      child: Row(
        children: [
          Icon(Icons.translate_rounded, size: 18, color: palette.textSecondary),
          const SizedBox(width: 10),
          Expanded(
            child: Text(
              'Language',
              style: FeatherTypography.body
                  .copyWith(color: palette.textPrimary),
            ),
          ),
          Container(
            padding:
                const EdgeInsets.symmetric(horizontal: 10, vertical: 6),
            decoration: BoxDecoration(
              color: palette.panelFill,
              borderRadius: BorderRadius.circular(8),
              border: Border.all(color: palette.panelBorder),
            ),
            child: PopupMenuButton<String>(
              tooltip: 'Pick language',
              color: palette.panelFillStrong,
              child: Row(
                mainAxisSize: MainAxisSize.min,
                children: [
                  Text(
                    current,
                    style: FeatherTypography.caption.copyWith(
                      color: palette.textPrimary,
                      fontWeight: FontWeight.w600,
                    ),
                  ),
                  const SizedBox(width: 4),
                  Icon(Icons.arrow_drop_down_rounded,
                      size: 16, color: palette.textTertiary),
                ],
              ),
              itemBuilder: (_) => [
                for (final l in _languages)
                  PopupMenuItem(
                    value: l,
                    child: Text(l,
                        style: FeatherTypography.body
                            .copyWith(color: palette.textPrimary)),
                  ),
              ],
              onSelected: onPick,
            ),
          ),
        ],
      ),
    );
  }
}

class _ActionRow extends StatelessWidget {
  const _ActionRow({
    required this.palette,
    required this.icon,
    required this.label,
    required this.onTap,
  });

  final FeatherPalette palette;
  final IconData icon;
  final String label;
  final VoidCallback? onTap;

  @override
  Widget build(BuildContext context) {
    return InkWell(
      onTap: onTap,
      child: Padding(
        padding: const EdgeInsets.symmetric(vertical: 10),
        child: Row(
          children: [
            Icon(icon, size: 18, color: palette.textSecondary),
            const SizedBox(width: 10),
            Expanded(
              child: Text(
                label,
                style: FeatherTypography.body
                    .copyWith(color: palette.textPrimary),
              ),
            ),
            Icon(Icons.chevron_right_rounded,
                size: 18, color: palette.textTertiary),
          ],
        ),
      ),
    );
  }
}

class _InfoRow extends StatelessWidget {
  const _InfoRow({
    required this.palette,
    required this.label,
    required this.value,
  });

  final FeatherPalette palette;
  final String label;
  final String value;

  @override
  Widget build(BuildContext context) {
    return Padding(
      padding: const EdgeInsets.symmetric(vertical: 10),
      child: Row(
        children: [
          Icon(Icons.info_outline, size: 18, color: palette.textSecondary),
          const SizedBox(width: 10),
          Expanded(
            child: Text(
              label,
              style: FeatherTypography.body
                  .copyWith(color: palette.textPrimary),
            ),
          ),
          Text(
            value,
            style: FeatherTypography.mono
                .copyWith(color: palette.textTertiary),
          ),
        ],
      ),
    );
  }
}
