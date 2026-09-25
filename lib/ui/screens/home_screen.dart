// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// home_screen.dart — Project list (create / organize / open).
//
// Mirrors Feather 3D's home screen: a grid of project thumbnails with a
// prominent "New" tile, plus a small settings gear in the top-right and
// the user's profile chip in the top-left. Search filters by name.
//
// Per the design research, every thumbnail is a glass panel with a tiny
// preview of the scene's strokes. Long-press a tile to enter select
// mode for bulk organize (rename / duplicate / export / delete).

import 'package:flutter/material.dart';

import '../theme/feather_colors.dart';
import '../theme/feather_typography.dart';
import '../theme/glassmorphism.dart';
import '../widgets/glass_panel.dart';
import '../widgets/icon_button.dart';

class ProjectMeta {
  const ProjectMeta({
    required this.id,
    required this.name,
    required this.updated,
    this.thumbnailColors = const [Color(0xFF60A5FA), Color(0xFFA78BFA)],
    this.favorite = false,
  });

  final String id;
  final String name;
  final DateTime updated;
  final List<Color> thumbnailColors;
  final bool favorite;
}

class HomeScreen extends StatefulWidget {
  const HomeScreen({
    super.key,
    required this.projects,
    this.onOpen,
    this.onCreate,
    this.onSettings,
    this.onRename,
    this.onDelete,
    this.onDuplicate,
    this.onExport,
  });

  final List<ProjectMeta> projects;
  final ValueChanged<ProjectMeta>? onOpen;
  final VoidCallback? onCreate;
  final VoidCallback? onSettings;
  final void Function(ProjectMeta, String)? onRename;
  final ValueChanged<ProjectMeta>? onDelete;
  final ValueChanged<ProjectMeta>? onDuplicate;
  final ValueChanged<ProjectMeta>? onExport;

  @override
  State<HomeScreen> createState() => _HomeScreenState();
}

class _HomeScreenState extends State<HomeScreen> {
  final TextEditingController _search = TextEditingController();
  final Set<String> _selected = {};
  bool _selectMode = false;
  String _query = '';

  @override
  void dispose() {
    _search.dispose();
    super.dispose();
  }

  List<ProjectMeta> get _filtered {
    if (_query.isEmpty) return widget.projects;
    final q = _query.toLowerCase();
    return widget.projects
        .where((p) => p.name.toLowerCase().contains(q))
        .toList();
  }

  void _toggleSelect(String id) {
    setState(() {
      if (_selected.contains(id)) {
        _selected.remove(id);
      } else {
        _selected.add(id);
      }
      if (_selected.isEmpty) _selectMode = false;
    });
  }

  @override
  Widget build(BuildContext context) {
    final palette = _palette(context);
    return Scaffold(
      backgroundColor: palette.appBackground,
      body: Stack(
        children: [
          // Ambient orb backdrop.
          Positioned(
            right: -120,
            top: -80,
            child: Container(
              width: 320,
              height: 320,
              decoration: BoxDecoration(
                shape: BoxShape.circle,
                color: FeatherPalette.accentPurple.withValues(alpha: 0.12),
              ),
            ),
          ),
          SafeArea(
            child: Padding(
              padding: const EdgeInsets.fromLTRB(24, 16, 24, 24),
              child: Column(
                crossAxisAlignment: CrossAxisAlignment.stretch,
                children: [
                  _TopBar(
                    palette: palette,
                    onSettings: widget.onSettings,
                    selectMode: _selectMode,
                    selectedCount: _selected.length,
                    onExitSelect: () => setState(() {
                      _selectMode = false;
                      _selected.clear();
                    }),
                    onDuplicate: () => _bulkAction(widget.onDuplicate),
                    onExport: () => _bulkAction(widget.onExport),
                    onDelete: () => _bulkAction(widget.onDelete),
                  ),
                  const SizedBox(height: 16),
                  Text(
                    'Projects',
                    style: FeatherTypography.display
                        .copyWith(color: palette.textPrimary),
                  ),
                  const SizedBox(height: 4),
                  Text(
                    'Tap a thumbnail to open, long-press to organize.',
                    style: FeatherTypography.body
                        .copyWith(color: palette.textTertiary),
                  ),
                  const SizedBox(height: 16),
                  _SearchField(
                    controller: _search,
                    palette: palette,
                    onChanged: (v) => setState(() => _query = v),
                  ),
                  const SizedBox(height: 18),
                  Expanded(
                    child: _ProjectGrid(
                      projects: _filtered,
                      selected: _selected,
                      palette: palette,
                      onCreate: widget.onCreate,
                      onOpen: (p) {
                        if (_selectMode) {
                          _toggleSelect(p.id);
                        } else {
                          widget.onOpen?.call(p);
                        }
                      },
                      onLongPress: (p) {
                        setState(() {
                          _selectMode = true;
                          _selected.add(p.id);
                        });
                      },
                      onToggleSelect: _toggleSelect,
                    ),
                  ),
                ],
              ),
            ),
          ),
        ],
      ),
    );
  }

  void _bulkAction(ValueChanged<ProjectMeta>? cb) {
    if (cb == null) return;
    for (final p in widget.projects) {
      if (_selected.contains(p.id)) cb(p);
    }
    setState(() {
      _selected.clear();
      _selectMode = false;
    });
  }

  FeatherPalette _palette(BuildContext context) =>
      Theme.of(context).brightness == Brightness.dark
          ? FeatherColors.dark
          : FeatherColors.light;
}

class _TopBar extends StatelessWidget {
  const _TopBar({
    required this.palette,
    required this.onSettings,
    required this.selectMode,
    required this.selectedCount,
    required this.onExitSelect,
    required this.onDuplicate,
    required this.onExport,
    required this.onDelete,
  });

  final FeatherPalette palette;
  final VoidCallback? onSettings;
  final bool selectMode;
  final int selectedCount;
  final VoidCallback onExitSelect;
  final VoidCallback onDuplicate;
  final VoidCallback onExport;
  final VoidCallback onDelete;

  @override
  Widget build(BuildContext context) {
    if (selectMode) {
      return Row(
        children: [
          FeatherIconButton(
            icon: Icons.close_rounded,
            size: 40,
            iconSize: 18,
            onTap: onExitSelect,
          ),
          const SizedBox(width: 12),
          Text(
            '$selectedCount selected',
            style: FeatherTypography.h2.copyWith(color: palette.textPrimary),
          ),
          const Spacer(),
          FeatherIconButton(
            icon: Icons.copy_all_rounded,
            tooltip: 'Duplicate',
            size: 40,
            iconSize: 18,
            onTap: onDuplicate,
          ),
          const SizedBox(width: 6),
          FeatherIconButton(
            icon: Icons.ios_share_outlined,
            tooltip: 'Export',
            size: 40,
            iconSize: 18,
            onTap: onExport,
          ),
          const SizedBox(width: 6),
          FeatherIconButton(
            icon: Icons.delete_outline_rounded,
            tooltip: 'Delete',
            size: 40,
            iconSize: 18,
            color: FeatherColors.toolErase,
            activeColor: FeatherColors.toolErase,
            onTap: onDelete,
          ),
        ],
      );
    }
    return Row(
      children: [
        Container(
          padding: const EdgeInsets.symmetric(horizontal: 10, vertical: 6),
          decoration: BoxDecoration(
            color: palette.panelFill,
            borderRadius: BorderRadius.circular(999),
            border: Border.all(color: palette.panelBorder),
          ),
          child: Row(
            children: [
              CircleAvatar(
                radius: 12,
                backgroundColor: FeatherPalette.accentBlue,
                child: const Icon(Icons.person, size: 14, color: Colors.white),
              ),
              const SizedBox(width: 8),
              Text(
                'Artist',
                style: FeatherTypography.caption
                    .copyWith(color: palette.textPrimary, fontWeight: FontWeight.w600),
              ),
            ],
          ),
        ),
        const Spacer(),
        FeatherIconButton(
          icon: Icons.settings_outlined,
          tooltip: 'Settings',
          size: 40,
          iconSize: 20,
          onTap: onSettings,
        ),
      ],
    );
  }
}

class _SearchField extends StatelessWidget {
  const _SearchField({
    required this.controller,
    required this.palette,
    required this.onChanged,
  });

  final TextEditingController controller;
  final FeatherPalette palette;
  final ValueChanged<String> onChanged;

  @override
  Widget build(BuildContext context) {
    return Container(
      height: 40,
      padding: const EdgeInsets.symmetric(horizontal: 12),
      decoration: BoxDecoration(
        color: palette.panelFill,
        borderRadius: BorderRadius.circular(12),
        border: Border.all(color: palette.panelBorder),
      ),
      child: Row(
        children: [
          Icon(Icons.search_rounded, size: 18, color: palette.textTertiary),
          const SizedBox(width: 8),
          Expanded(
            child: TextField(
              controller: controller,
              onChanged: onChanged,
              style: FeatherTypography.body.copyWith(color: palette.textPrimary),
              decoration: InputDecoration(
                isCollapsed: true,
                border: InputBorder.none,
                hintText: 'Search projects',
                hintStyle: FeatherTypography.body
                    .copyWith(color: palette.textTertiary),
              ),
            ),
          ),
        ],
      ),
    );
  }
}

class _ProjectGrid extends StatelessWidget {
  const _ProjectGrid({
    required this.projects,
    required this.selected,
    required this.palette,
    required this.onCreate,
    required this.onOpen,
    required this.onLongPress,
    required this.onToggleSelect,
  });

  final List<ProjectMeta> projects;
  final Set<String> selected;
  final FeatherPalette palette;
  final VoidCallback? onCreate;
  final ValueChanged<ProjectMeta> onOpen;
  final ValueChanged<ProjectMeta> onLongPress;
  final ValueChanged<String> onToggleSelect;

  @override
  Widget build(BuildContext context) {
    return GridView.builder(
      padding: const EdgeInsets.only(bottom: 24),
      gridDelegate: const SliverGridDelegateWithMaxCrossAxisExtent(
        maxCrossAxisExtent: 220,
        mainAxisSpacing: 14,
        crossAxisSpacing: 14,
        childAspectRatio: 0.86,
      ),
      itemCount: projects.length + 1,
      itemBuilder: (_, i) {
        if (i == 0) {
          return _NewTile(palette: palette, onTap: onCreate);
        }
        final p = projects[i - 1];
        return _ProjectTile(
          project: p,
          selected: selected.contains(p.id),
          palette: palette,
          onTap: () => onOpen(p),
          onLongPress: () => onLongPress(p),
          onToggleSelect: () => onToggleSelect(p.id),
        );
      },
    );
  }
}

class _NewTile extends StatelessWidget {
  const _NewTile({required this.palette, required this.onTap});
  final FeatherPalette palette;
  final VoidCallback? onTap;

  @override
  Widget build(BuildContext context) {
    return GestureDetector(
      onTap: onTap,
      child: GlassPanel(
        spec: GlassSpec.panel,
        padding: EdgeInsets.zero,
        child: Container(
          decoration: BoxDecoration(
            borderRadius: BorderRadius.circular(18),
            border: Border.all(
              color: palette.accent.withValues(alpha: 0.5),
              width: 1.5,
            ),
          ),
          child: Column(
            mainAxisAlignment: MainAxisAlignment.center,
            children: [
              Container(
                width: 44,
                height: 44,
                decoration: BoxDecoration(
                  shape: BoxShape.circle,
                  color: palette.accent.withValues(alpha: 0.2),
                  border: Border.all(color: palette.accent, width: 1.4),
                ),
                child: Icon(Icons.add_rounded, color: palette.accent, size: 26),
              ),
              const SizedBox(height: 8),
              Text(
                'New Project',
                style: FeatherTypography.bodyStrong
                    .copyWith(color: palette.textPrimary),
              ),
            ],
          ),
        ),
      ),
    );
  }
}

class _ProjectTile extends StatelessWidget {
  const _ProjectTile({
    required this.project,
    required this.selected,
    required this.palette,
    required this.onTap,
    required this.onLongPress,
    required this.onToggleSelect,
  });

  final ProjectMeta project;
  final bool selected;
  final FeatherPalette palette;
  final VoidCallback onTap;
  final VoidCallback onLongPress;
  final VoidCallback onToggleSelect;

  @override
  Widget build(BuildContext context) {
    return GestureDetector(
      onTap: onTap,
      onLongPress: onLongPress,
      child: GlassPanel(
        spec: GlassSpec.panel,
        padding: EdgeInsets.zero,
        child: Stack(
          children: [
            Column(
              crossAxisAlignment: CrossAxisAlignment.stretch,
              children: [
                Expanded(
                  child: Container(
                    margin: const EdgeInsets.all(6),
                    decoration: BoxDecoration(
                      borderRadius: BorderRadius.circular(12),
                      gradient: LinearGradient(
                        begin: Alignment.topLeft,
                        end: Alignment.bottomRight,
                        colors: project.thumbnailColors,
                      ),
                    ),
                    child: CustomPaint(painter: _ThumbPreviewPainter()),
                  ),
                ),
                Padding(
                  padding: const EdgeInsets.fromLTRB(10, 0, 10, 10),
                  child: Row(
                    children: [
                      Expanded(
                        child: Column(
                          crossAxisAlignment: CrossAxisAlignment.start,
                          children: [
                            Text(
                              project.name,
                              maxLines: 1,
                              overflow: TextOverflow.ellipsis,
                              style: FeatherTypography.bodyStrong
                                  .copyWith(color: palette.textPrimary),
                            ),
                            Text(
                              _formatDate(project.updated),
                              style: FeatherTypography.micro
                                  .copyWith(color: palette.textTertiary),
                            ),
                          ],
                        ),
                      ),
                      if (project.favorite)
                        Icon(Icons.star_rounded,
                            size: 14, color: FeatherPalette.accentYellow),
                    ],
                  ),
                ),
              ],
            ),
            if (selected)
              Positioned(
                top: 8,
                right: 8,
                child: GestureDetector(
                  onTap: onToggleSelect,
                  child: Container(
                    width: 22,
                    height: 22,
                    decoration: BoxDecoration(
                      shape: BoxShape.circle,
                      color: palette.accent,
                      border:
                          Border.all(color: palette.textPrimary, width: 1.5),
                    ),
                    child: const Icon(Icons.check_rounded,
                        size: 14, color: Colors.white),
                  ),
                ),
              ),
          ],
        ),
      ),
    );
  }

  String _formatDate(DateTime d) {
    final now = DateTime.now();
    final diff = now.difference(d);
    if (diff.inHours < 1) return 'just now';
    if (diff.inDays < 1) return '${diff.inHours}h ago';
    if (diff.inDays < 7) return '${diff.inDays}d ago';
    return '${d.year}-${d.month.toString().padLeft(2, '0')}-${d.day.toString().padLeft(2, '0')}';
  }
}

class _ThumbPreviewPainter extends CustomPainter {
  @override
  void paint(Canvas canvas, Size size) {
    // A few decorative strokes so the thumbnail isn't a flat color.
    final rng = _Rng(7);
    for (int i = 0; i < 5; i++) {
      final paint = Paint()
        ..color = Colors.white.withValues(alpha: 0.4 + rng.nextDouble() * 0.3)
        ..style = PaintingStyle.stroke
        ..strokeWidth = 1.5 + rng.nextDouble() * 2
        ..strokeCap = StrokeCap.round;
      final path = Path();
      var x = rng.nextDouble() * size.width * 0.2;
      var y = rng.nextDouble() * size.height;
      path.moveTo(x, y);
      for (int j = 0; j < 4; j++) {
        x += rng.nextDouble() * size.width * 0.25;
        y += (rng.nextDouble() - 0.5) * size.height * 0.3;
        path.lineTo(x, y);
      }
      canvas.drawPath(path, paint);
    }
  }

  @override
  bool shouldRepaint(covariant CustomPainter oldDelegate) => false;
}

class _Rng {
  _Rng(this.seed);
  final int seed;
  int _state = 0;

  double nextDouble() {
    // Tiny LCG for deterministic thumbnails — not crypto.
    _state = (1103515245 * (seed + _state + 1) + 12345) & 0x7fffffff;
    return (_state % 10000) / 10000;
  }
}
