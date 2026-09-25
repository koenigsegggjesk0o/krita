// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// right_panel.dart — Right contextual panel.
//
// Tabs (per stagepanel.txt + transform):
//   * Layers  — the active group's child curves/resources list,
//   * Resources — imported images / objects available in the scene,
//   * Environment — render toggle, lighting direction, glow area,
//                   background (world) image, grid.
//
// Each tab is glassmorphism and the indicator slides with a spring curve.
// The panel itself is collapsible — tap the header chevron to collapse
// to a thin rail with just the tab icons.

import 'package:flutter/material.dart';

import '../theme/feather_animations.dart';
import '../theme/feather_colors.dart';
import '../theme/feather_typography.dart';
import '../theme/glassmorphism.dart';
import 'glass_panel.dart';
import 'icon_button.dart';

enum RightPanelTab { layers, resources, environment }

extension RightPanelTabX on RightPanelTab {
  String get label {
    switch (this) {
      case RightPanelTab.layers:
        return 'Layers';
      case RightPanelTab.resources:
        return 'Resources';
      case RightPanelTab.environment:
        return 'Environment';
    }
  }

  IconData get icon {
    switch (this) {
      case RightPanelTab.layers:
        return Icons.view_list_outlined;
      case RightPanelTab.resources:
        return Icons.collections_outlined;
      case RightPanelTab.environment:
        return Icons.wb_twilight_outlined;
    }
  }
}

class RightPanel extends StatefulWidget {
  const RightPanel({
    super.key,
    this.tab = RightPanelTab.layers,
    this.collapsed = false,
    this.onCollapsed,
    this.layers = const [],
    this.resources = const [],
    this.renderMode = false,
    this.lightDirection = 0.0,
    this.glowArea = 0.5,
    this.onRenderToggle,
    this.onLightDirection,
    this.onGlowArea,
    this.onSelectLayer,
    this.onToggleLayerVisibility,
    this.onSelectResource,
  });

  final RightPanelTab tab;
  final bool collapsed;
  final ValueChanged<bool>? onCollapsed;
  final List<LayerItem> layers;
  final List<ResourceItem> resources;
  final bool renderMode;
  final double lightDirection; // radians
  final double glowArea; // 0..1
  final VoidCallback? onRenderToggle;
  final ValueChanged<double>? onLightDirection;
  final ValueChanged<double>? onGlowArea;
  final ValueChanged<String>? onSelectLayer;
  final ValueChanged<String>? onToggleLayerVisibility;
  final ValueChanged<String>? onSelectResource;

  @override
  State<RightPanel> createState() => _RightPanelState();
}

class _RightPanelState extends State<RightPanel> {
  late RightPanelTab _tab = widget.tab;

  @override
  Widget build(BuildContext context) {
    final palette = _palette(context);
    if (widget.collapsed) {
      return _CollapsedRail(
        palette: palette,
        tab: _tab,
        onPick: (t) => setState(() => _tab = t),
        onExpand: () => widget.onCollapsed?.call(false),
      );
    }
    return GlassPanel(
      spec: GlassSpec.panel,
      width: 280,
      padding: const EdgeInsets.fromLTRB(0, 0, 0, 12),
      child: Column(
        mainAxisSize: MainAxisSize.min,
        crossAxisAlignment: CrossAxisAlignment.stretch,
        children: [
          _Header(
            palette: palette,
            onCollapse: () => widget.onCollapsed?.call(true),
          ),
          _TabBar(
            palette: palette,
            tab: _tab,
            onSelect: (t) => setState(() => _tab = t),
          ),
          const SizedBox(height: 8),
          AnimatedSwitcher(
            duration: FeatherDurations.medium,
            switchInCurve: FeatherCurves.slideIn,
            switchOutCurve: FeatherCurves.slideIn,
            child: _buildTab(palette),
          ),
        ],
      ),
    );
  }

  Widget _buildTab(FeatherPalette palette) {
    switch (_tab) {
      case RightPanelTab.layers:
        return _LayersTab(
          key: const ValueKey('layers'),
          palette: palette,
          items: widget.layers,
          onSelect: widget.onSelectLayer,
          onToggleVisibility: widget.onToggleLayerVisibility,
        );
      case RightPanelTab.resources:
        return _ResourcesTab(
          key: const ValueKey('resources'),
          palette: palette,
          items: widget.resources,
          onSelect: widget.onSelectResource,
        );
      case RightPanelTab.environment:
        return _EnvironmentTab(
          key: const ValueKey('environment'),
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
  const _Header({required this.palette, required this.onCollapse});

  final FeatherPalette palette;
  final VoidCallback onCollapse;

  @override
  Widget build(BuildContext context) {
    return Padding(
      padding: const EdgeInsets.fromLTRB(14, 12, 6, 6),
      child: Row(
        children: [
          Text(
            'STAGE',
            style: FeatherTypography.micro
                .copyWith(color: palette.textTertiary, letterSpacing: 1.2),
          ),
          const Spacer(),
          FeatherIconButton(
            icon: Icons.chevron_right_rounded,
            tooltip: 'Collapse',
            size: 32,
            iconSize: 18,
            onTap: onCollapse,
          ),
        ],
      ),
    );
  }
}

class _TabBar extends StatelessWidget {
  const _TabBar({
    required this.palette,
    required this.tab,
    required this.onSelect,
  });

  final FeatherPalette palette;
  final RightPanelTab tab;
  final ValueChanged<RightPanelTab> onSelect;

  @override
  Widget build(BuildContext context) {
    return Padding(
      padding: const EdgeInsets.symmetric(horizontal: 10),
      child: Row(
        children: [
          for (final t in RightPanelTab.values)
            Expanded(
              child: GestureDetector(
                onTap: () => onSelect(t),
                child: AnimatedContainer(
                  duration: FeatherDurations.quick,
                  curve: FeatherCurves.toggle,
                  margin: const EdgeInsets.symmetric(horizontal: 2),
                  padding: const EdgeInsets.symmetric(vertical: 8),
                  decoration: BoxDecoration(
                    color: t == tab
                        ? palette.accent.withValues(alpha: 0.22)
                        : Colors.transparent,
                    borderRadius: BorderRadius.circular(8),
                    border: Border.all(
                      color:
                          t == tab ? palette.accent : Colors.transparent,
                    ),
                  ),
                  child: Column(
                    children: [
                      Icon(t.icon,
                          size: 16,
                          color: t == tab
                              ? palette.accent
                              : palette.textTertiary),
                      const SizedBox(height: 2),
                      Text(
                        t.label,
                        style: FeatherTypography.micro.copyWith(
                          color: t == tab
                              ? palette.textPrimary
                              : palette.textTertiary,
                          letterSpacing: 0.4,
                        ),
                      ),
                    ],
                  ),
                ),
              ),
            ),
        ],
      ),
    );
  }
}

class LayerItem {
  const LayerItem({
    required this.id,
    required this.name,
    this.color = const Color(0xFF60A5FA),
    this.visible = true,
    this.selected = false,
    this.count = 0,
  });
  final String id;
  final String name;
  final Color color;
  final bool visible;
  final bool selected;
  final int count;
}

class ResourceItem {
  const ResourceItem({required this.id, required this.name, this.kind = 'Image'});
  final String id;
  final String name;
  final String kind;
}

class _LayersTab extends StatelessWidget {
  const _LayersTab({
    super.key,
    required this.palette,
    required this.items,
    this.onSelect,
    this.onToggleVisibility,
  });

  final FeatherPalette palette;
  final List<LayerItem> items;
  final ValueChanged<String>? onSelect;
  final ValueChanged<String>? onToggleVisibility;

  @override
  Widget build(BuildContext context) {
    if (items.isEmpty) {
      return _EmptyHint(palette: palette, text: 'No groups yet. Tap + to add.');
    }
    return ConstrainedBox(
      constraints: const BoxConstraints(maxHeight: 280),
      child: ListView.separated(
        shrinkWrap: true,
        padding: const EdgeInsets.symmetric(horizontal: 10),
        itemCount: items.length,
        separatorBuilder: (_, __) => const SizedBox(height: 4),
        itemBuilder: (_, i) => _LayerRow(
          item: items[i],
          palette: palette,
          onSelect: () => onSelect?.call(items[i].id),
          onToggleVis: () => onToggleVisibility?.call(items[i].id),
        ),
      ),
    );
  }
}

class _LayerRow extends StatelessWidget {
  const _LayerRow({
    required this.item,
    required this.palette,
    required this.onSelect,
    required this.onToggleVis,
  });

  final LayerItem item;
  final FeatherPalette palette;
  final VoidCallback onSelect;
  final VoidCallback onToggleVis;

  @override
  Widget build(BuildContext context) {
    return GestureDetector(
      onTap: onSelect,
      child: AnimatedContainer(
        duration: FeatherDurations.quick,
        padding: const EdgeInsets.symmetric(horizontal: 8, vertical: 8),
        decoration: BoxDecoration(
          color: item.selected
              ? palette.accent.withValues(alpha: 0.18)
              : palette.panelFill,
          borderRadius: BorderRadius.circular(8),
          border: Border.all(
            color: item.selected ? palette.accent : palette.panelBorder,
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
            const SizedBox(width: 8),
            Expanded(
              child: Column(
                crossAxisAlignment: CrossAxisAlignment.start,
                children: [
                  Text(
                    item.name,
                    style: FeatherTypography.caption.copyWith(
                      color: palette.textPrimary,
                      fontWeight: FontWeight.w600,
                    ),
                  ),
                  if (item.count > 0)
                    Text(
                      '${item.count} curves',
                      style: FeatherTypography.micro
                          .copyWith(color: palette.textTertiary),
                    ),
                ],
              ),
            ),
            GestureDetector(
              onTap: onToggleVis,
              child: Icon(
                item.visible
                    ? Icons.visibility_outlined
                    : Icons.visibility_off_outlined,
                size: 16,
                color: item.visible ? palette.textSecondary : palette.textTertiary,
              ),
            ),
          ],
        ),
      ),
    );
  }
}

class _ResourcesTab extends StatelessWidget {
  const _ResourcesTab({
    super.key,
    required this.palette,
    required this.items,
    this.onSelect,
  });

  final FeatherPalette palette;
  final List<ResourceItem> items;
  final ValueChanged<String>? onSelect;

  @override
  Widget build(BuildContext context) {
    if (items.isEmpty) {
      return _EmptyHint(palette: palette, text: 'No resources imported.');
    }
    return ConstrainedBox(
      constraints: const BoxConstraints(maxHeight: 280),
      child: ListView.separated(
        shrinkWrap: true,
        padding: const EdgeInsets.symmetric(horizontal: 10),
        itemCount: items.length,
        separatorBuilder: (_, __) => const SizedBox(height: 4),
        itemBuilder: (_, i) => _ResourceRow(
          item: items[i],
          palette: palette,
          onSelect: () => onSelect?.call(items[i].id),
        ),
      ),
    );
  }
}

class _ResourceRow extends StatelessWidget {
  const _ResourceRow({
    required this.item,
    required this.palette,
    required this.onSelect,
  });

  final ResourceItem item;
  final FeatherPalette palette;
  final VoidCallback onSelect;

  @override
  Widget build(BuildContext context) {
    return GestureDetector(
      onTap: onSelect,
      child: Container(
        padding: const EdgeInsets.symmetric(horizontal: 8, vertical: 8),
        decoration: BoxDecoration(
          color: palette.panelFill,
          borderRadius: BorderRadius.circular(8),
          border: Border.all(color: palette.panelBorder),
        ),
        child: Row(
          children: [
            Icon(Icons.image_outlined, size: 16, color: palette.textSecondary),
            const SizedBox(width: 8),
            Expanded(
              child: Column(
                crossAxisAlignment: CrossAxisAlignment.start,
                children: [
                  Text(
                    item.name,
                    style: FeatherTypography.caption.copyWith(
                      color: palette.textPrimary,
                      fontWeight: FontWeight.w600,
                    ),
                  ),
                  Text(
                    item.kind,
                    style: FeatherTypography.micro
                        .copyWith(color: palette.textTertiary),
                  ),
                ],
              ),
            ),
          ],
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
      padding: const EdgeInsets.symmetric(horizontal: 14),
      child: Column(
        crossAxisAlignment: CrossAxisAlignment.stretch,
        children: [
          _ToggleRow(
            label: 'Render Mode',
            icon: Icons.wb_incandescent_outlined,
            active: renderMode,
            palette: palette,
            onTap: onRenderToggle,
          ),
          const SizedBox(height: 10),
          _LabeledSlider(
            label: 'Light direction',
            value: (lightDirection / (2 * 3.14159265)).clamp(0.0, 1.0),
            palette: palette,
            formatter: (v) => '${(v * 360).round()}°',
            onChanged: (v) =>
                onLightDirection?.call(v * 2 * 3.14159265),
          ),
          _LabeledSlider(
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

class _ToggleRow extends StatelessWidget {
  const _ToggleRow({
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
            Icon(icon,
                size: 16,
                color: active ? palette.accent : palette.textSecondary),
            const SizedBox(width: 8),
            Expanded(
              child: Text(
                label,
                style: FeatherTypography.caption.copyWith(
                  color:
                      active ? palette.textPrimary : palette.textSecondary,
                  fontWeight: FontWeight.w600,
                ),
              ),
            ),
            Icon(
              active ? Icons.toggle_on_rounded : Icons.toggle_off_rounded,
              size: 20,
              color: active ? palette.accent : palette.textTertiary,
            ),
          ],
        ),
      ),
    );
  }
}

class _LabeledSlider extends StatelessWidget {
  const _LabeledSlider({
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
            width: 92,
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
            width: 44,
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

class _EmptyHint extends StatelessWidget {
  const _EmptyHint({required this.palette, required this.text});
  final FeatherPalette palette;
  final String text;

  @override
  Widget build(BuildContext context) {
    return Padding(
      padding: const EdgeInsets.all(16),
      child: Text(
        text,
        textAlign: TextAlign.center,
        style: FeatherTypography.caption.copyWith(color: palette.textTertiary),
      ),
    );
  }
}

class _CollapsedRail extends StatelessWidget {
  const _CollapsedRail({
    required this.palette,
    required this.tab,
    required this.onPick,
    required this.onExpand,
  });

  final FeatherPalette palette;
  final RightPanelTab tab;
  final ValueChanged<RightPanelTab> onPick;
  final VoidCallback onExpand;

  @override
  Widget build(BuildContext context) {
    return GlassPanel(
      spec: GlassSpec.subtle,
      width: 48,
      padding: const EdgeInsets.symmetric(vertical: 8),
      child: Column(
        mainAxisSize: MainAxisSize.min,
        children: [
          for (final t in RightPanelTab.values)
            Padding(
              padding: const EdgeInsets.symmetric(vertical: 4),
              child: FeatherIconButton(
                icon: t.icon,
                tooltip: t.label,
                isSelected: t == tab,
                size: 36,
                iconSize: 16,
                onTap: () {
                  onPick(t);
                  onExpand();
                },
              ),
            ),
        ],
      ),
    );
  }
}
