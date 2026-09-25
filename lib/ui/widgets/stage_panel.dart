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
    this.lightDirection = 0.0,
    this.glowArea = 0.5,
    this.onAddGroup,
    this.onDeleteGroup,
    this.onRenameGroup,
    this.onSelectGroup,
    this.onImportResource,
    this.onDeleteResource,
    this.onRenderToggle,
    this.onLightDirection,
    this.onGlowArea,
    this.onClose,
  });

  final StageTab initialTab;
  final List<LayerItem> groups;
  final List<ResourceItem> resources;
  final String? activeGroupId;
  final bool renderMode;
  final double lightDirection;
  final double glowArea;
  final VoidCallback? onAddGroup;
  final ValueChanged<String>? onDeleteGroup;
  final void Function(String id, String name)? onRenameGroup;
  final ValueChanged<String>? onSelectGroup;
  final VoidCallback? onImportResource;
  final ValueChanged<String>? onDeleteResource;
  final VoidCallback? onRenderToggle;
  final ValueChanged<double>? onLightDirection;
  final ValueChanged<double>? onGlowArea;
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
          lightDirection: widget.lightDirection,
          glowArea: widget.glowArea,
          onRenderToggle: widget.onRenderToggle,
          onLightDirection: widget.onLightDirection,
          onGlowArea: widget.onGlowArea,
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

class _ResourceTab extends StatelessWidget {
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
                  onTap: onImport,
                ),
              ],
            ),
          ),
          Flexible(
            child: resources.isEmpty
                ? Padding(
                    padding: const EdgeInsets.all(20),
                    child: Text(
                      'Import images or 3D resources for your scene.',
                      style: FeatherTypography.caption
                          .copyWith(color: palette.textTertiary),
                    ),
                  )
                : ListView.separated(
                    shrinkWrap: true,
                    padding: const EdgeInsets.symmetric(horizontal: 12),
                    itemCount: resources.length,
                    separatorBuilder: (_, __) => const SizedBox(height: 4),
                    itemBuilder: (_, i) {
                      final r = resources[i];
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
                            Icon(Icons.image_outlined,
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
                                    r.kind,
                                    style: FeatherTypography.micro
                                        .copyWith(color: palette.textTertiary),
                                  ),
                                ],
                              ),
                            ),
                            GestureDetector(
                              onTap: () => onDelete?.call(r.id),
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
}

class _EnvironmentTab extends StatelessWidget {
  const _EnvironmentTab({
    super.key,
    required this.palette,
    required this.renderMode,
    required this.lightDirection,
    required this.glowArea,
    this.onRenderToggle,
    this.onLightDirection,
    this.onGlowArea,
  });

  final FeatherPalette palette;
  final bool renderMode;
  final double lightDirection;
  final double glowArea;
  final VoidCallback? onRenderToggle;
  final ValueChanged<double>? onLightDirection;
  final ValueChanged<double>? onGlowArea;

  @override
  Widget build(BuildContext context) {
    return Padding(
      padding: const EdgeInsets.all(14),
      child: Column(
        crossAxisAlignment: CrossAxisAlignment.stretch,
        children: [
          _Row(
            label: 'Render Mode',
            icon: Icons.wb_incandescent_outlined,
            active: renderMode,
            palette: palette,
            onTap: onRenderToggle,
          ),
          const SizedBox(height: 14),
          _Slider(
            label: 'Light direction',
            value: (lightDirection / (2 * 3.14159265)).clamp(0.0, 1.0),
            palette: palette,
            formatter: (v) => '${(v * 360).round()}°',
            onChanged: (v) => onLightDirection?.call(v * 2 * 3.14159265),
          ),
          const SizedBox(height: 8),
          _Slider(
            label: 'Glow area',
            value: glowArea,
            palette: palette,
            formatter: (v) => '${(v * 100).round()}%',
            onChanged: onGlowArea,
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
