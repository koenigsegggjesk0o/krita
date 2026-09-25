// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// stage_panel.dart — Stage panel popover (Group / Resource / Environment).
//
// Distinct from [RightPanel] (the always-visible right contextual panel):
// the Stage panel is summoned from the Tool Dock and is a larger modal
// popover that lets the user *manage* groups (rename, reorder, delete,
// add), import resources (file picker, drag-drop), and tune environment
// options at a higher fidelity than the inline Environment tab.
//
// Per stagepanel.txt:
//   * Group Tab    — create/delete/manage groups,
//   * Resource Tab — import resources,
//   * Environment Tab — render toggle, lighting, glow area, world/bg.

import 'dart:math' as math;

import 'package:flutter/material.dart';

import '../theme/feather_animations.dart';
import '../theme/feather_colors.dart';
import '../theme/feather_typography.dart';
import '../theme/glassmorphism.dart';
import 'glass_panel.dart';
import 'icon_button.dart';
import 'right_panel.dart' show LayerItem, ResourceItem;

class StagePanel extends StatefulWidget {
  const StagePanel({
    super.key,
    this.initialTab = StageTab.group,
    this.groups = const [],
    this.resources = const [],
    this.activeGroupId,
    this.renderMode = false,
    // Lighting direction is split into azimuth (heading around the
    // scene, 0..2π) and elevation (angle above the ground plane,
    // 0..π/2). The host converts the pair into a screen-space
    // direction vector for the Lambert tube shader — see
    // [_MainScreenState._azimuthElevationToLightDir] in
    // lib/screens/main_screen.dart.
    this.lightAzimuth = 5 * math.pi / 4,
    this.lightElevation = math.pi / 4,
    this.glowArea = 0.5,
    this.backgroundColor,
    this.showGrid = true,
    this.showGroundPlane = false,
    this.onAddGroup,
    this.onDeleteGroup,
    this.onRenameGroup,
    this.onSelectGroup,
    this.onImportResource,
    this.onDeleteResource,
    this.onRenderToggle,
    this.onLightAzimuth,
    this.onLightElevation,
    this.onGlowArea,
    this.onPickBackgroundColor,
    this.onClearBackgroundColor,
    this.onToggleGrid,
    this.onToggleGroundPlane,
    this.onClose,
  });

  final StageTab initialTab;
  final List<LayerItem> groups;
  final List<ResourceItem> resources;
  final String? activeGroupId;
  final bool renderMode;
  final double lightAzimuth;
  final double lightElevation;
  final double glowArea;
  final Color? backgroundColor;
  final bool showGrid;
  final bool showGroundPlane;
  final VoidCallback? onAddGroup;
  final ValueChanged<String>? onDeleteGroup;
  final void Function(String id, String name)? onRenameGroup;
  final ValueChanged<String>? onSelectGroup;
  final VoidCallback? onImportResource;
  final ValueChanged<String>? onDeleteResource;
  final VoidCallback? onRenderToggle;
  final ValueChanged<double>? onLightAzimuth;
  final ValueChanged<double>? onLightElevation;
  final ValueChanged<double>? onGlowArea;
  final VoidCallback? onPickBackgroundColor;
  final VoidCallback? onClearBackgroundColor;
  final ValueChanged<bool>? onToggleGrid;
  final ValueChanged<bool>? onToggleGroundPlane;
  final VoidCallback? onClose;

  @override
  State<StagePanel> createState() => _StagePanelState();
}

enum StageTab { group, resource, environment }

class _StagePanelState extends State<StagePanel> {
  late StageTab _tab = widget.initialTab;

  @override
  Widget build(BuildContext context) {
    final palette = _palette(context);
    return GlassPanel(
      spec: GlassSpec.strong,
      width: 420,
      padding: const EdgeInsets.all(0),
      child: Column(
        mainAxisSize: MainAxisSize.min,
        crossAxisAlignment: CrossAxisAlignment.stretch,
        children: [
          _Header(palette: palette, onClose: widget.onClose),
          _SegmentedTabs(
            palette: palette,
            tab: _tab,
            onSelect: (t) => setState(() => _tab = t),
          ),
          Flexible(
            child: AnimatedSwitcher(
              duration: FeatherDurations.medium,
              switchInCurve: FeatherCurves.slideIn,
              child: _buildTab(palette),
            ),
          ),
        ],
      ),
    );
  }

  Widget _buildTab(FeatherPalette palette) {
    switch (_tab) {
      case StageTab.group:
        return _GroupTab(
          key: const ValueKey('group'),
          palette: palette,
          groups: widget.groups,
          activeId: widget.activeGroupId,
          onAdd: widget.onAddGroup,
          onDelete: widget.onDeleteGroup,
          onRename: widget.onRenameGroup,
          onSelect: widget.onSelectGroup,
        );
      case StageTab.resource:
        return _ResourceTab(
          key: const ValueKey('resource'),
          palette: palette,
          resources: widget.resources,
          onImport: widget.onImportResource,
          onDelete: widget.onDeleteResource,
        );
      case StageTab.environment:
        return _EnvironmentTab(
          key: const ValueKey('env'),
          palette: palette,
          renderMode: widget.renderMode,
          lightAzimuth: widget.lightAzimuth,
          lightElevation: widget.lightElevation,
          glowArea: widget.glowArea,
          backgroundColor: widget.backgroundColor,
          showGrid: widget.showGrid,
          showGroundPlane: widget.showGroundPlane,
          onRenderToggle: widget.onRenderToggle,
          onLightAzimuth: widget.onLightAzimuth,
          onLightElevation: widget.onLightElevation,
          onGlowArea: widget.onGlowArea,
          onPickBackgroundColor: widget.onPickBackgroundColor,
          onClearBackgroundColor: widget.onClearBackgroundColor,
          onToggleGrid: widget.onToggleGrid,
          onToggleGroundPlane: widget.onToggleGroundPlane,
        );
    }
  }

  FeatherPalette _palette(BuildContext context) =>
      Theme.of(context).brightness == Brightness.dark
          ? FeatherColors.dark
          : FeatherColors.light;
}

class _Header extends StatelessWidget {
  const _Header({required this.palette, required this.onClose});
  final FeatherPalette palette;
  final VoidCallback? onClose;

  @override
  Widget build(BuildContext context) {
    return Padding(
      padding: const EdgeInsets.fromLTRB(16, 14, 6, 8),
      child: Row(
        children: [
          Text(
            'STAGE PANEL',
            style: FeatherTypography.h2.copyWith(color: palette.textPrimary),
          ),
          const Spacer(),
          FeatherIconButton(
            icon: Icons.close_rounded,
            tooltip: 'Close',
            size: 36,
            iconSize: 18,
            onTap: onClose,
          ),
        ],
      ),
    );
  }
}

class _SegmentedTabs extends StatelessWidget {
  const _SegmentedTabs({
    required this.palette,
    required this.tab,
    required this.onSelect,
  });

  final FeatherPalette palette;
  final StageTab tab;
  final ValueChanged<StageTab> onSelect;

  @override
  Widget build(BuildContext context) {
    return Padding(
      padding: const EdgeInsets.symmetric(horizontal: 12),
      child: Container(
        padding: const EdgeInsets.all(3),
        decoration: BoxDecoration(
          color: palette.panelFill,
          borderRadius: BorderRadius.circular(10),
          border: Border.all(color: palette.panelBorder),
        ),
        child: Row(
          children: [
            for (final t in StageTab.values)
              Expanded(
                child: GestureDetector(
                  onTap: () => onSelect(t),
                  child: AnimatedContainer(
                    duration: FeatherDurations.quick,
                    curve: FeatherCurves.toggle,
                    padding: const EdgeInsets.symmetric(vertical: 7),
                    decoration: BoxDecoration(
                      color: t == tab
                          ? palette.accent.withValues(alpha: 0.28)
                          : Colors.transparent,
                      borderRadius: BorderRadius.circular(8),
                    ),
                    child: Text(
                      t.name[0].toUpperCase() + t.name.substring(1),
                      textAlign: TextAlign.center,
                      style: FeatherTypography.caption.copyWith(
                        color: t == tab
                            ? palette.textPrimary
                            : palette.textTertiary,
                        fontWeight: FontWeight.w700,
                      ),
                    ),
                  ),
                ),
              ),
          ],
        ),
      ),
    );
  }
}

class _GroupTab extends StatelessWidget {
  const _GroupTab({
    super.key,
    required this.palette,
    required this.groups,
    required this.activeId,
    this.onAdd,
    this.onDelete,
    this.onRename,
    this.onSelect,
  });

  final FeatherPalette palette;
  final List<LayerItem> groups;
  final String? activeId;
  final VoidCallback? onAdd;
  final ValueChanged<String>? onDelete;
  final void Function(String id, String name)? onRename;
  final ValueChanged<String>? onSelect;

  @override
  Widget build(BuildContext context) {
    return ConstrainedBox(
      constraints: const BoxConstraints(maxHeight: 320),
      child: Column(
        mainAxisSize: MainAxisSize.min,
        children: [
          Padding(
            padding: const EdgeInsets.fromLTRB(14, 12, 14, 6),
            child: Row(
              children: [
                Text(
                  'GROUPS',
                  style: FeatherTypography.micro.copyWith(
                      color: palette.textTertiary, letterSpacing: 1.2),
                ),
                const Spacer(),
                FeatherIconButton(
                  icon: Icons.add_rounded,
                  tooltip: 'Add group',
                  size: 32,
                  iconSize: 16,
                  activeColor: palette.accent,
                  onTap: onAdd,
                ),
              ],
            ),
          ),
          Flexible(
            child: groups.isEmpty
                ? Padding(
                    padding: const EdgeInsets.all(20),
                    child: Text(
                      'No groups. Tap + to add one.',
                      style: FeatherTypography.caption
                          .copyWith(color: palette.textTertiary),
                    ),
                  )
                : ListView.separated(
                    shrinkWrap: true,
                    padding: const EdgeInsets.symmetric(horizontal: 12),
                    itemCount: groups.length,
                    separatorBuilder: (_, __) => const SizedBox(height: 4),
                    itemBuilder: (_, i) {
                      final g = groups[i];
                      return _GroupRow(
                        item: g,
                        active: g.id == activeId,
                        palette: palette,
                        onSelect: () => onSelect?.call(g.id),
                        onDelete: () => onDelete?.call(g.id),
                        onRename: (name) => onRename?.call(g.id, name),
                      );
                    },
                  ),
          ),
        ],
      ),
    );
  }
}

class _GroupRow extends StatelessWidget {
  const _GroupRow({
    required this.item,
    required this.active,
    required this.palette,
    required this.onSelect,
    required this.onDelete,
    required this.onRename,
  });

  final LayerItem item;
  final bool active;
  final FeatherPalette palette;
  final VoidCallback onSelect;
  final VoidCallback onDelete;
  final ValueChanged<String> onRename;

  @override
  Widget build(BuildContext context) {
    return GestureDetector(
      onTap: onSelect,
      child: Container(
        padding: const EdgeInsets.symmetric(horizontal: 10, vertical: 8),
        decoration: BoxDecoration(
          color: active
              ? palette.accent.withValues(alpha: 0.18)
              : palette.panelFill,
          borderRadius: BorderRadius.circular(10),
          border: Border.all(
            color: active ? palette.accent : palette.panelBorder,
          ),
        ),
        child: Row(
          children: [
            Container(
              width: 12,
              height: 12,
              decoration: BoxDecoration(
                color: item.color,
                shape: BoxShape.circle,
              ),
            ),
            const SizedBox(width: 10),
            Expanded(
              child: Text(
                item.name,
                style: FeatherTypography.bodyStrong.copyWith(
                  color: active ? palette.textPrimary : palette.textSecondary,
                ),
              ),
            ),
            Text(
              '${item.count}',
              style: FeatherTypography.mono
                  .copyWith(color: palette.textTertiary),
            ),
            const SizedBox(width: 6),
            GestureDetector(
              onTap: () => _showRename(context),
              child: Icon(Icons.edit_outlined,
                  size: 14, color: palette.textTertiary),
            ),
            const SizedBox(width: 6),
            GestureDetector(
              onTap: onDelete,
              child: Icon(Icons.delete_outline_rounded,
                  size: 14, color: FeatherColors.toolErase.withValues(alpha: 0.8)),
            ),
          ],
        ),
      ),
    );
  }

  void _showRename(BuildContext context) {
    final ctrl = TextEditingController(text: item.name);
    showDialog<void>(
      context: context,
      builder: (_) => AlertDialog(
        title: const Text('Rename group'),
        content: TextField(controller: ctrl, autofocus: true),
        actions: [
          TextButton(
            onPressed: () => Navigator.pop(context),
            child: const Text('Cancel'),
          ),
          TextButton(
            onPressed: () {
              onRename(ctrl.text);
              Navigator.pop(context);
            },
            child: const Text('Save'),
          ),
        ],
      ),
    );
  }
}

/// Resource-kind filter chips (per stagepanel.txt Resource Tab:
/// imported images, 3D models, saved guides).
enum ResourceKindFilter { all, image, model, guide }

extension ResourceKindFilterX on ResourceKindFilter {
  String get label {
    switch (this) {
      case ResourceKindFilter.all:
        return 'All';
      case ResourceKindFilter.image:
        return 'Images';
      case ResourceKindFilter.model:
        return '3D Models';
      case ResourceKindFilter.guide:
        return 'Saved Guides';
    }
  }
}

class _ResourceTab extends StatefulWidget {
  const _ResourceTab({
    super.key,
    required this.palette,
    required this.resources,
    this.onImport,
    this.onDelete,
  });

  final FeatherPalette palette;
  final List<ResourceItem> resources;
  final VoidCallback? onImport;
  final ValueChanged<String>? onDelete;

  @override
  State<_ResourceTab> createState() => _ResourceTabState();
}

class _ResourceTabState extends State<_ResourceTab> {
  ResourceKindFilter _filter = ResourceKindFilter.all;

  FeatherPalette get palette => widget.palette;

  /// Map a ResourceItem's free-form `kind` string onto one of the
  /// three documented resource categories. Anything we can't classify
  /// lands in `image` (the historical default).
  ResourceKindFilter _classify(ResourceItem r) {
    final k = r.kind.toLowerCase();
    if (k.contains('model') || k.contains('obj') || k.contains('gltf')) {
      return ResourceKindFilter.model;
    }
    if (k.contains('guide') || k.contains('saved')) {
      return ResourceKindFilter.guide;
    }
    return ResourceKindFilter.image;
  }

  IconData _iconFor(ResourceKindFilter k) {
    switch (k) {
      case ResourceKindFilter.model:
        return Icons.view_in_ar_outlined;
      case ResourceKindFilter.guide:
        return Icons.route_outlined;
      case ResourceKindFilter.image:
        return Icons.image_outlined;
      case ResourceKindFilter.all:
        return Icons.apps_outlined;
    }
  }

  @override
  Widget build(BuildContext context) {
    final visible = _filter == ResourceKindFilter.all
        ? widget.resources
        : widget.resources.where(_classifyFilter).toList();
    return ConstrainedBox(
      constraints: const BoxConstraints(maxHeight: 360),
      child: Column(
        mainAxisSize: MainAxisSize.min,
        children: [
          Padding(
            padding: const EdgeInsets.fromLTRB(14, 12, 14, 6),
            child: Row(
              children: [
                Text(
                  'RESOURCES',
                  style: FeatherTypography.micro.copyWith(
                      color: palette.textTertiary, letterSpacing: 1.2),
                ),
                const Spacer(),
                FeatherIconButton(
                  icon: Icons.upload_file_outlined,
                  tooltip: 'Import',
                  size: 32,
                  iconSize: 16,
                  activeColor: palette.accent,
                  onTap: widget.onImport,
                ),
              ],
            ),
          ),
          // Kind filter chips.
          Padding(
            padding: const EdgeInsets.symmetric(horizontal: 10),
            child: Wrap(
              spacing: 4,
              runSpacing: 4,
              children: [
                for (final f in ResourceKindFilter.values)
                  _FilterChip(
                    label: f.label,
                    active: _filter == f,
                    palette: palette,
                    onTap: () => setState(() => _filter = f),
                  ),
              ],
            ),
          ),
          const SizedBox(height: 6),
          Flexible(
            child: visible.isEmpty
                ? Padding(
                    padding: const EdgeInsets.all(20),
                    child: Text(
                      widget.resources.isEmpty
                          ? 'Import images, 3D models or saved guides.'
                          : 'No resources in this category.',
                      style: FeatherTypography.caption
                          .copyWith(color: palette.textTertiary),
                    ),
                  )
                : ListView.separated(
                    shrinkWrap: true,
                    padding: const EdgeInsets.symmetric(horizontal: 12),
                    itemCount: visible.length,
                    separatorBuilder: (_, __) => const SizedBox(height: 4),
                    itemBuilder: (_, i) {
                      final r = visible[i];
                      final k = _classify(r);
                      return Container(
                        padding: const EdgeInsets.symmetric(
                            horizontal: 10, vertical: 8),
                        decoration: BoxDecoration(
                          color: palette.panelFill,
                          borderRadius: BorderRadius.circular(10),
                          border: Border.all(color: palette.panelBorder),
                        ),
                        child: Row(
                          children: [
                            Icon(_iconFor(k),
                                size: 16, color: palette.textSecondary),
                            const SizedBox(width: 10),
                            Expanded(
                              child: Column(
                                crossAxisAlignment: CrossAxisAlignment.start,
                                children: [
                                  Text(
                                    r.name,
                                    style: FeatherTypography.bodyStrong
                                        .copyWith(color: palette.textPrimary),
                                  ),
                                  Text(
                                    k.label,
                                    style: FeatherTypography.micro
                                        .copyWith(color: palette.textTertiary),
                                  ),
                                ],
                              ),
                            ),
                            GestureDetector(
                              onTap: () => widget.onDelete?.call(r.id),
                              child: Icon(Icons.delete_outline_rounded,
                                  size: 14,
                                  color: FeatherColors.toolErase
                                      .withValues(alpha: 0.8)),
                            ),
                          ],
                        ),
                      );
                    },
                  ),
          ),
        ],
      ),
    );
  }

  bool _classifyFilter(ResourceItem r) => _classify(r) == _filter;
}

class _FilterChip extends StatelessWidget {
  const _FilterChip({
    required this.label,
    required this.active,
    required this.palette,
    required this.onTap,
  });

  final String label;
  final bool active;
  final FeatherPalette palette;
  final VoidCallback onTap;

  @override
  Widget build(BuildContext context) {
    return GestureDetector(
      behavior: HitTestBehavior.opaque,
      onTap: onTap,
      child: AnimatedContainer(
        duration: FeatherDurations.quick,
        curve: FeatherCurves.toggle,
        padding: const EdgeInsets.symmetric(horizontal: 10, vertical: 5),
        decoration: BoxDecoration(
          color: active
              ? palette.accent.withValues(alpha: 0.22)
              : palette.panelFill,
          borderRadius: BorderRadius.circular(8),
          border: Border.all(
            color: active ? palette.accent : palette.panelBorder,
            width: active ? 1.2 : 1,
          ),
        ),
        child: Text(
          label,
          style: FeatherTypography.micro.copyWith(
            color: active ? palette.textPrimary : palette.textTertiary,
            fontWeight: FontWeight.w700,
            letterSpacing: 0.4,
          ),
        ),
      ),
    );
  }
}

class _EnvironmentTab extends StatelessWidget {
  const _EnvironmentTab({
    super.key,
    required this.palette,
    required this.renderMode,
    required this.lightAzimuth,
    required this.lightElevation,
    required this.glowArea,
    this.backgroundColor,
    this.showGrid = true,
    this.showGroundPlane = false,
    this.onRenderToggle,
    this.onLightAzimuth,
    this.onLightElevation,
    this.onGlowArea,
    this.onPickBackgroundColor,
    this.onClearBackgroundColor,
    this.onToggleGrid,
    this.onToggleGroundPlane,
  });

  final FeatherPalette palette;
  final bool renderMode;
  final double lightAzimuth;
  final double lightElevation;
  final double glowArea;
  final Color? backgroundColor;
  final bool showGrid;
  final bool showGroundPlane;
  final VoidCallback? onRenderToggle;
  final ValueChanged<double>? onLightAzimuth;
  final ValueChanged<double>? onLightElevation;
  final ValueChanged<double>? onGlowArea;
  final VoidCallback? onPickBackgroundColor;
  final VoidCallback? onClearBackgroundColor;
  final ValueChanged<bool>? onToggleGrid;
  final ValueChanged<bool>? onToggleGroundPlane;

  @override
  Widget build(BuildContext context) {
    return Padding(
      padding: const EdgeInsets.all(14),
      child: Column(
        crossAxisAlignment: CrossAxisAlignment.stretch,
        children: [
          // Lighting section (per stagepanel.txt Environment Tab:
          // lighting direction).
          _SectionLabel(palette: palette, text: 'LIGHTING'),
          const SizedBox(height: 6),
          _Row(
            label: 'Render Mode',
            icon: Icons.wb_incandescent_outlined,
            active: renderMode,
            palette: palette,
            onTap: onRenderToggle,
          ),
          const SizedBox(height: 8),
          // Azimuth: heading around the scene (0..360°). 0° = light from
          // the right, 90° = light from below, 180° = light from the
          // left, 270° = light from above — matching the screen-space
          // direction vector the painter uses.
          _Slider(
            label: 'Azimuth',
            value: (lightAzimuth / (2 * math.pi)).clamp(0.0, 1.0),
            palette: palette,
            formatter: (v) => '${(v * 360).round()}°',
            onChanged: (v) => onLightAzimuth?.call(v * 2 * math.pi),
          ),
          const SizedBox(height: 8),
          // Elevation: angle above the ground plane (0..90°). 0° = light
          // travels horizontally (long shadows, strong side-lit feel);
          // 90° = light comes from straight above (no shadow skew, top
          // lighting). The host projects this to the y-component of the
          // screen-space direction vector.
          _Slider(
            label: 'Elevation',
            value: (lightElevation / (math.pi / 2)).clamp(0.0, 1.0),
            palette: palette,
            formatter: (v) => '${(v * 90).round()}°',
            onChanged: (v) => onLightElevation?.call(v * math.pi / 2),
          ),
          const SizedBox(height: 8),
          _Slider(
            label: 'Glow area',
            value: glowArea,
            palette: palette,
            formatter: (v) => '${(v * 100).round()}%',
            onChanged: onGlowArea,
          ),
          const SizedBox(height: 18),
          // Background section (per stagepanel.txt Environment Tab:
          // background). Now a color picker — when the user picks a
          // color, the canvas background + cutout material use it; when
          // null, the palette default is used.
          _SectionLabel(palette: palette, text: 'BACKGROUND'),
          const SizedBox(height: 6),
          _BackgroundColorRow(
            palette: palette,
            color: backgroundColor,
            onTap: onPickBackgroundColor,
            onClear: onClearBackgroundColor,
          ),
          const SizedBox(height: 18),
          // World settings (per stagepanel.txt Environment Tab:
          // world settings).
          _SectionLabel(palette: palette, text: 'WORLD'),
          const SizedBox(height: 6),
          _Row(
            label: 'Grid',
            icon: Icons.grid_on_outlined,
            active: showGrid,
            palette: palette,
            onTap: () => onToggleGrid?.call(!showGrid),
          ),
          const SizedBox(height: 8),
          _Row(
            label: 'Ground plane',
            icon: Icons.horizontal_distribute_outlined,
            active: showGroundPlane,
            palette: palette,
            onTap: () => onToggleGroundPlane?.call(!showGroundPlane),
          ),
        ],
      ),
    );
  }
}

class _SectionLabel extends StatelessWidget {
  const _SectionLabel({required this.palette, required this.text});
  final FeatherPalette palette;
  final String text;

  @override
  Widget build(BuildContext context) {
    return Text(
      text,
      style: FeatherTypography.micro
          .copyWith(color: palette.textTertiary, letterSpacing: 1.4),
    );
  }
}

class _BackgroundColorRow extends StatelessWidget {
  const _BackgroundColorRow({
    required this.palette,
    required this.color,
    this.onTap,
    this.onClear,
  });

  final FeatherPalette palette;
  final Color? color;
  final VoidCallback? onTap;
  final VoidCallback? onClear;

  @override
  Widget build(BuildContext context) {
    final hasColor = color != null;
    return Container(
      padding: const EdgeInsets.symmetric(horizontal: 10, vertical: 10),
      decoration: BoxDecoration(
        color: palette.panelFill,
        borderRadius: BorderRadius.circular(10),
        border: Border.all(color: palette.panelBorder),
      ),
      child: Row(
        children: [
          // Color swatch (checkerboard placeholder when no override set
          // so it reads as "transparent / palette default").
          Container(
            width: 28,
            height: 28,
            decoration: BoxDecoration(
              color: hasColor ? color : palette.canvasBackground,
              borderRadius: BorderRadius.circular(6),
              border: Border.all(color: palette.panelBorder),
            ),
          ),
          const SizedBox(width: 10),
          Expanded(
            child: GestureDetector(
              behavior: HitTestBehavior.opaque,
              onTap: onTap,
              child: Row(
                children: [
                  Icon(Icons.palette_outlined,
                      size: 16, color: palette.textSecondary),
                  const SizedBox(width: 8),
                  Expanded(
                    child: Text(
                      hasColor
                          ? 'Background color set'
                          : 'Pick background color',
                      style: FeatherTypography.bodyStrong.copyWith(
                        color: hasColor
                            ? palette.textPrimary
                            : palette.textTertiary,
                      ),
                    ),
                  ),
                  Icon(Icons.chevron_right_rounded,
                      size: 18, color: palette.textTertiary),
                ],
              ),
            ),
          ),
          if (hasColor)
            GestureDetector(
              behavior: HitTestBehavior.opaque,
              onTap: onClear,
              child: Padding(
                padding: const EdgeInsets.only(left: 6),
                child: Icon(Icons.close_rounded,
                    size: 16, color: palette.textTertiary),
              ),
            ),
        ],
      ),
    );
  }
}

class _Row extends StatelessWidget {
  const _Row({
    required this.label,
    required this.icon,
    required this.active,
    required this.palette,
    required this.onTap,
  });

  final String label;
  final IconData icon;
  final bool active;
  final FeatherPalette palette;
  final VoidCallback? onTap;

  @override
  Widget build(BuildContext context) {
    return GestureDetector(
      behavior: HitTestBehavior.opaque,
      onTap: onTap,
      child: Container(
        padding: const EdgeInsets.symmetric(horizontal: 10, vertical: 10),
        decoration: BoxDecoration(
          color: active
              ? palette.accent.withValues(alpha: 0.18)
              : palette.panelFill,
          borderRadius: BorderRadius.circular(10),
          border: Border.all(
            color: active ? palette.accent : palette.panelBorder,
          ),
        ),
        child: Row(
          children: [
            Icon(icon,
                size: 16,
                color: active ? palette.accent : palette.textSecondary),
            const SizedBox(width: 10),
            Expanded(
              child: Text(
                label,
                style: FeatherTypography.bodyStrong.copyWith(
                  color:
                      active ? palette.textPrimary : palette.textSecondary,
                ),
              ),
            ),
            Icon(
              active ? Icons.toggle_on_rounded : Icons.toggle_off_rounded,
              size: 22,
              color: active ? palette.accent : palette.textTertiary,
            ),
          ],
        ),
      ),
    );
  }
}

class _Slider extends StatelessWidget {
  const _Slider({
    required this.label,
    required this.value,
    required this.palette,
    required this.formatter,
    required this.onChanged,
  });

  final String label;
  final double value;
  final FeatherPalette palette;
  final String Function(double) formatter;
  final ValueChanged<double>? onChanged;

  @override
  Widget build(BuildContext context) {
    return Padding(
      padding: const EdgeInsets.only(top: 8),
      child: Row(
        children: [
          SizedBox(
            width: 110,
            child: Text(
              label,
              style: FeatherTypography.caption.copyWith(
                color: palette.textSecondary,
                fontWeight: FontWeight.w500,
              ),
            ),
          ),
          Expanded(
            child: SliderTheme(
              data: SliderTheme.of(context).copyWith(
                trackHeight: 3,
                thumbShape:
                    const RoundSliderThumbShape(enabledThumbRadius: 7),
                activeTrackColor: palette.accent,
                inactiveTrackColor:
                    palette.textPrimary.withValues(alpha: 0.1),
                thumbColor: palette.textPrimary,
              ),
              child: Slider(
                value: value.clamp(0.0, 1.0),
                onChanged: onChanged,
              ),
            ),
          ),
          SizedBox(
            width: 50,
            child: Text(
              formatter(value),
              textAlign: TextAlign.right,
              style:
                  FeatherTypography.mono.copyWith(color: palette.textPrimary),
            ),
          ),
        ],
      ),
    );
  }
}
