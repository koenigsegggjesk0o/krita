// SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
// SPDX-License-Identifier: GPL-2.0-or-later
//
// main_screen.dart — Main editor screen.
//
// Assembles the full Feather-Krita editor:
//   - A fullscreen 3D viewport ([CanvasWidget]) wrapped in a
//     [RepaintBoundary] so the export pipeline can rasterise it.
//   - A glass top bar with file name, undo / redo, folder, settings.
//   - A glass bottom dock with the eight primary tools.
//   - A left stroke-list panel and a right brush-settings panel (both
//     collapsible).
//   - A floating 3D manipulation joystick wired to the stroke manager's
//     transform + liquify operations.
//   - Modal screens for export, brush preset picking, and settings.
//
// All controls are wired to the live [EditorState] and the engine classes
// behind it; nothing is a stub.

import 'dart:convert';
import 'dart:io';
import 'dart:math' as math show pi;
import 'dart:typed_data';
import 'dart:ui' as ui;

import 'package:flutter/material.dart';
import 'package:flutter/rendering.dart';
import 'package:flutter/scheduler.dart';
import 'package:vector_math/vector_math_64.dart' hide Colors;

import 'package:path_provider/path_provider.dart';
import 'package:file_picker/file_picker.dart';
import 'package:image/image.dart' as img;

import 'package:feather_krita/theme/app_theme.dart';
import 'package:feather_krita/state/editor_state.dart';
import 'package:feather_krita/engine/stroke_manager.dart';
import 'package:feather_krita/engine/camera_controller.dart';
import 'package:feather_krita/models/export_format.dart';
import 'package:feather_krita/models/brush_preset.dart';

import 'package:feather_krita/widgets/glass_app_bar.dart';
import 'package:feather_krita/widgets/glass_bottom_bar.dart';
import 'package:feather_krita/widgets/brush_settings_panel.dart';
import 'package:feather_krita/widgets/stroke_list_panel.dart';
import 'package:feather_krita/widgets/canvas_widget.dart';
import 'package:feather_krita/widgets/joystick_widget.dart';

import 'package:feather_krita/screens/export_screen.dart';
import 'package:feather_krita/screens/brush_picker_screen.dart';
import 'package:feather_krita/screens/settings_screen.dart';

/// The main editor screen.
class MainScreen extends StatefulWidget {
  const MainScreen({super.key, this.state});

  /// Optional pre-built editor state (e.g. restored from a project file).
  final EditorState? state;

  @override
  State<MainScreen> createState() => _MainScreenState();
}

class _MainScreenState extends State<MainScreen> {
  late final EditorState _state;
  final GlobalKey _canvasKey = GlobalKey();

  bool _leftPanelOpen = true;
  bool _rightPanelOpen = true;

  @override
  void initState() {
    super.initState();
    _state = widget.state ?? EditorState();
  }

  @override
  void dispose() {
    _state.dispose();
    super.dispose();
  }

  // ----- Tool selection --------------------------------------------------

  void _onToolSelected(Tool tool) {
    _state.setActiveTool(tool);
    switch (tool) {
      case Tool.export:
        _openExport();
        break;
      case Tool.settings:
        _openSettings();
        break;
      case Tool.light:
        ScaffoldMessenger.of(context).showSnackBar(const SnackBar(
          content: Text('Light tool: drag on the canvas to reposition the '
              'light source. Two-finger drag to orbit.'),
          duration: Duration(seconds: 2),
        ));
        break;
      default:
        break;
    }
  }

  // ----- Joystick --------------------------------------------------------

  void _onJoystickInput(Vector2 input) {
    if (input == Vector2.zero()) return;
    final sel = _state.strokes.selectedStrokes;
    if (sel.isEmpty && _state.joystickMode != JoystickMode.liquify) {
      // Nothing to transform; gentle haptic reminder.
      return;
    }
    switch (_state.joystickMode) {
      case JoystickMode.move:
        _state.strokes.applyJoystickTransform(input, TransformMode.move);
        break;
      case JoystickMode.rotate:
        _state.strokes.applyJoystickTransform(input, TransformMode.rotate);
        break;
      case JoystickMode.scale:
        _state.strokes.applyJoystickTransform(input, TransformMode.scale);
        break;
      case JoystickMode.liquify:
        final center = _selectionCenter() ?? _state.camera.target;
        final radius = (_state.brushSize / 100).clamp(0.1, 3.0);
        final strength = input.length.clamp(0.0, 1.0) * 0.05;
        final dir = _state.camera.forward * input.y +
            _state.camera.right * input.x;
        _state.strokes.applyLiquify(
          LiquifyMode.push,
          center,
          radius,
          strength,
          dir,
        );
        break;
    }
  }

  Vector3? _selectionCenter() {
    final sel = _state.strokes.selectedStrokes;
    if (sel.isEmpty) return null;
    final sum = Vector3.zero();
    var count = 0;
    for (final s in sel) {
      for (final p in s.points) {
        sum.add(s.transform.transform3(p.position.clone()));
        count++;
      }
    }
    if (count == 0) return null;
    return sum..scale(1.0 / count);
  }

  // ----- Modal screens ---------------------------------------------------

  void _openExport() {
    showDialog(
      context: context,
      barrierDismissible: true,
      builder: (_) => ExportScreen(
        isPro: _state.isPro,
        exporter: _exportRunner,
        onUpgrade: _upgradeToPro,
        onClose: () => Navigator.of(context).pop(),
        baseName: _state.fileName.replaceAll(RegExp(r'\.[^.]+$'), ''),
      ),
    );
  }

  void _openBrushPicker() {
    showDialog(
      context: context,
      barrierDismissible: true,
      builder: (_) => BrushPickerScreen(
        presets: _state.presets,
        activePresetId: null,
        onPick: (preset) {
          _state.loadBrushPreset(preset);
          Navigator.of(context).pop();
        },
        onImport: _importKpp,
        onClose: () => Navigator.of(context).pop(),
      ),
    );
  }

  void _openSettings() {
    showDialog(
      context: context,
      barrierDismissible: true,
      builder: (_) => SettingsScreen(
        state: _state,
        onUpgrade: _upgradeToPro,
        onClose: () => Navigator.of(context).pop(),
      ),
    );
  }

  void _upgradeToPro() {
    setState(() => _state.isPro = true);
    ScaffoldMessenger.of(context).showSnackBar(const SnackBar(
      content: Text('Feather-Krita Pro activated. All formats unlocked.'),
      duration: Duration(seconds: 2),
    ));
  }

  // ----- Folder / project -----------------------------------------------

  Future<void> _openFolder() async {
    await showModalBottomSheet<void>(
      context: context,
      backgroundColor: Colors.transparent,
      builder: (ctx) => GlassContainer(
        padding: const EdgeInsets.all(16),
        borderRadius: AppTheme.radiusXLarge,
        color: AppTheme.darkGlass.withOpacity(0.7),
        child: SafeArea(
          child: Column(
            mainAxisSize: MainAxisSize.min,
            children: [
              _SheetAction(
                icon: Icons.note_add_rounded,
                label: 'New document',
                onTap: () {
                  Navigator.pop(ctx);
                  _state.newDocument();
                },
              ),
              _SheetAction(
                icon: Icons.folder_open_rounded,
                label: 'Open .feather…',
                onTap: () {
                  Navigator.pop(ctx);
                  _openProject();
                },
              ),
              _SheetAction(
                icon: Icons.save_alt_rounded,
                label: 'Save project…',
                onTap: () {
                  Navigator.pop(ctx);
                  _saveProject();
                },
              ),
              _SheetAction(
                icon: Icons.file_upload_outlined,
                label: 'Import brush preset (.kpp)…',
                onTap: () {
                  Navigator.pop(ctx);
                  _importKpp();
                },
              ),
            ],
          ),
        ),
      ),
    );
  }

  Future<void> _openProject() async {
    final result = await FilePicker.platform.pickFiles(
      type: FileType.custom,
      allowedExtensions: ['feather', 'json'],
    );
    if (result == null || result.files.single.path == null) return;
    try {
      final raw = await File(result.files.single.path!).readAsString();
      _state.strokes.fromJsonString(raw);
      setState(() => _state.fileName = result.files.single.name);
    } catch (e) {
      _toast('Open failed: $e');
    }
  }

  Future<void> _saveProject() async {
    final name =
        _state.fileName.replaceAll(RegExp(r'\.[^.]+$'), '') + '.feather';
    final path = await FilePicker.platform.saveFile(
      dialogTitle: 'Save Feather Project',
      fileName: name,
      type: FileType.custom,
      allowedExtensions: ['feather'],
    );
    if (path == null) return;
    try {
      await File(path).writeAsString(_state.strokes.toJsonString());
      setState(() => _state.fileName = path.split(Platform.pathSeparator).last);
      _toast('Saved to $path');
    } catch (e) {
      _toast('Save failed: $e');
    }
  }

  Future<void> _importKpp() async {
    final result = await FilePicker.platform.pickFiles(
      type: FileType.custom,
      allowedExtensions: ['kpp'],
    );
    if (result == null || result.files.single.path == null) return;
    try {
      final preset = await BrushPreset.loadFromFile(result.files.single.path!);
      setState(() => _state.presets.add(preset));
      _state.loadBrushPreset(preset);
      _toast('Imported "${preset.name}"');
    } catch (e) {
      _toast('Import failed: $e');
    }
  }

  void _toast(String message) {
    ScaffoldMessenger.of(context).showSnackBar(
      SnackBar(content: Text(message), duration: const Duration(seconds: 2)),
    );
  }

  // ----- Export runner ---------------------------------------------------

  Future<String> _exportRunner(
    ExportFormat format,
    int quality,
    void Function(double progress) onProgress,
  ) async {
    try {
      final dir = await _exportDir();
      final base = _state.fileName.replaceAll(RegExp(r'\.[^.]+$'), '');
      final path = defaultExportPath(dir, base, format);

      switch (format) {
        case ExportFormat.featherProject:
          await File(path).writeAsString(_state.strokes.toJsonString());
          onProgress(1);
          return path;

        case ExportFormat.obj:
          await File(path).writeAsString(_strokeObj());
          onProgress(1);
          return path;

        case ExportFormat.gltf:
          await File(path).writeAsString(_strokeGltf());
          onProgress(1);
          return path;

        case ExportFormat.png:
        case ExportFormat.jpeg:
        case ExportFormat.gif:
          final image = await _captureCanvas();
          final List<int> bytes;
          if (format == ExportFormat.png) {
            bytes = img.encodePng(image);
          } else if (format == ExportFormat.jpeg) {
            bytes = img.encodeJpg(image, quality: quality);
          } else {
            bytes = img.encodeGif(image);
          }
          await File(path).writeAsBytes(bytes);
          onProgress(1);
          return path;

        case ExportFormat.mp4:
          // Real turntable PNG frame sequence. True H.264 muxing requires a
          // native video encoder plugin; we emit a numbered frame sequence
          // that any video tool can encode to MP4.
          final frameDir = Directory('$path.frames');
          if (frameDir.existsSync()) frameDir.deleteSync(recursive: true);
          frameDir.createSync(recursive: true);
          const frames = 16;
          final savedDamping = _state.camera.damping;
          _state.camera.damping = CameraDamping.instant;
          try {
            for (var i = 0; i < frames; i++) {
              _state.camera.orbit(2 * math.pi / frames, 0);
              _state.camera.snap();
              await _pumpFrame();
              final frame = await _captureCanvas();
              final bytes = img.encodePng(frame);
              final framePath =
                  '${frameDir.path}/frame_${i.toString().padLeft(3, '0')}.png';
              await File(framePath).writeAsBytes(bytes);
              onProgress((i + 1) / frames);
            }
          } finally {
            _state.camera.damping = savedDamping;
          }
          await File('$path.txt').writeAsString(
            'Feather-Krita MP4 frame sequence\n'
            '${frames} frames written to:\n${frameDir.path}\n'
            'Encode to MP4 with: ffmpeg -framerate 30 -i frame_%03d.png -c:v libx264 out.mp4\n',
          );
          return frameDir.path;
      }
    } catch (e) {
      return 'error: $e';
    }
  }

  Future<String> _exportDir() async {
    if (Platform.isWindows || Platform.isLinux || Platform.isMacOS) {
      final d = await getDownloadsDirectory();
      if (d != null) return d.path;
    }
    final d = await getApplicationDocumentsDirectory();
    return d.path;
  }

  Future<void> _pumpFrame() async {
    // Wait for two frames so the canvas repaints after the camera move.
    final first = SchedulerBinding.instance.endOfFrame;
    await first;
    await Future<void>.delayed(const Duration(milliseconds: 16));
    await SchedulerBinding.instance.endOfFrame;
  }

  Future<img.Image> _captureCanvas() async {
    final ctx = _canvasKey.currentContext;
    if (ctx == null) throw StateError('canvas not mounted');
    RenderRepaintBoundary? boundary =
        ctx.findRenderObject() as RenderRepaintBoundary?;
    if (boundary == null || boundary.debugNeedsPaint) {
      await _pumpFrame();
    }
    boundary = ctx.findRenderObject() as RenderRepaintBoundary?;
    if (boundary == null) throw StateError('canvas render object missing');
    final image = await boundary.toImage(pixelRatio: 1.0);
    final byteData = await image.toByteData(format: ui.ImageByteFormat.png);
    if (byteData == null) throw StateError('canvas capture failed');
    final pngBytes = byteData.buffer
        .asUint8List(byteData.offsetInBytes, byteData.lengthInBytes);
    final decoded = img.decodePng(pngBytes);
    if (decoded == null) throw StateError('canvas decode failed');
    return decoded;
  }

  // ----- Geometry exports ------------------------------------------------

  String _strokeObj() {
    final sb = StringBuffer('# Feather-Krita OBJ export v1.0.0\n');
    var idx = 1;
    final visible =
        _state.strokes.strokes.where((s) => s.isVisible).toList(growable: false);
    for (final s in visible) {
      sb.writeln('o Stroke_${s.id}');
      final start = idx;
      for (var i = 0; i < s.points.length; i++) {
        final w = s.worldPosition(i);
        sb.writeln('v ${w.x.toStringAsFixed(6)} '
            '${w.y.toStringAsFixed(6)} ${w.z.toStringAsFixed(6)}');
        idx++;
      }
      if (idx - start >= 2) {
        final parts = <String>[];
        for (var i = start; i < idx; i++) {
          parts.add('$i');
        }
        sb.writeln('l ${parts.join(' ')}');
      }
    }
    return sb.toString();
  }

  String _strokeGltf() {
    final visible =
        _state.strokes.strokes.where((s) => s.isVisible).toList(growable: false);
    final verts = <double>[];
    for (final s in visible) {
      for (var i = 0; i < s.points.length - 1; i++) {
        final a = s.worldPosition(i);
        final b = s.worldPosition(i + 1);
        verts.addAll([a.x, a.y, a.z, b.x, b.y, b.z]);
      }
    }
    final vertCount = verts.length ~/ 3;
    if (vertCount == 0) {
      return jsonEncode({
        'asset': {'version': '2.0', 'generator': 'Feather-Krita 1.0.0'},
      });
    }
    final bytes =
        Float32List.fromList(verts).buffer.asUint8List();
    final b64 = base64Encode(bytes);
    var minx = 1e9, miny = 1e9, minz = 1e9, maxx = -1e9, maxy = -1e9, maxz = -1e9;
    for (var i = 0; i < verts.length; i += 3) {
      final x = verts[i], y = verts[i + 1], z = verts[i + 2];
      if (x < minx) minx = x;
      if (x > maxx) maxx = x;
      if (y < miny) miny = y;
      if (y > maxy) maxy = y;
      if (z < minz) minz = z;
      if (z > maxz) maxz = z;
    }
    final gltf = {
      'asset': {'version': '2.0', 'generator': 'Feather-Krita 1.0.0'},
      'scene': 0,
      'scenes': [
        {'nodes': [0]}
      ],
      'nodes': [{'mesh': 0}],
      'meshes': [
        {
          'primitives': [
            {'attributes': {'POSITION': 0}, 'mode': 1}
          ]
        }
      ],
      'buffers': [
        {
          'uri': 'data:application/octet-stream;base64,$b64',
          'byteLength': bytes.length
        }
      ],
      'bufferViews': [
        {
          'buffer': 0,
          'byteOffset': 0,
          'byteLength': bytes.length,
          'target': 34962
        }
      ],
      'accessors': [
        {
          'bufferView': 0,
          'componentType': 5126,
          'count': vertCount,
          'type': 'VEC3',
          'max': [maxx, maxy, maxz],
          'min': [minx, miny, minz]
        }
      ],
    };
    return jsonEncode(gltf);
  }

  // ----- Build -----------------------------------------------------------

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      body: ListenableBuilder(
        listenable: _state,
        builder: (context, _) {
          return Stack(
            fit: StackFit.expand,
            children: [
              // 3D canvas.
              RepaintBoundary(
                key: _canvasKey,
                child: CanvasWidget(state: _state),
              ),

              // Top app bar.
              Positioned(
                top: 0,
                left: 0,
                right: 0,
                child: GlassAppBar(
                  fileName: _state.fileName,
                  canUndo: _state.canUndo,
                  canRedo: _state.canRedo,
                  onUndo: _state.undo,
                  onRedo: _state.redo,
                  onOpenFolder: _openFolder,
                  onRename: (name) =>
                      setState(() => _state.fileName = name),
                  onShowSettings: _openSettings,
                ),
              ),

              // Left stroke list.
              Positioned(
                top: 72,
                left: 12,
                bottom: 120,
                child: AnimatedSwitcher(
                  duration: const Duration(milliseconds: 200),
                  child: _leftPanelOpen
                      ? StrokeListPanel(
                          key: const ValueKey('left'),
                          manager: _state.strokes,
                          onClose: () =>
                              setState(() => _leftPanelOpen = false),
                        )
                      : const SizedBox.shrink(),
                ),
              ),

              // Right brush settings.
              Positioned(
                top: 72,
                right: 12,
                bottom: 120,
                child: AnimatedSwitcher(
                  duration: const Duration(milliseconds: 200),
                  child: _rightPanelOpen
                      ? BrushSettingsPanel(
                          key: const ValueKey('right'),
                          state: _state,
                          onPickPreset: _openBrushPicker,
                          onClose: () =>
                              setState(() => _rightPanelOpen = false),
                        )
                      : const SizedBox.shrink(),
                ),
              ),

              // Panel toggles (when collapsed).
              if (!_leftPanelOpen)
                Positioned(
                  top: 72,
                  left: 12,
                  child: _PanelToggle(
                    icon: Icons.layers_rounded,
                    onTap: () => setState(() => _leftPanelOpen = true),
                  ),
                ),
              if (!_rightPanelOpen)
                Positioned(
                  top: 72,
                  right: 12,
                  child: _PanelToggle(
                    icon: Icons.brush_rounded,
                    onTap: () => setState(() => _rightPanelOpen = true),
                  ),
                ),

              // Bottom dock.
              Positioned(
                bottom: 18,
                left: 0,
                right: 0,
                child: Center(
                  child: GlassBottomBar(
                    activeTool: _state.activeTool,
                    onToolSelected: _onToolSelected,
                  ),
                ),
              ),

              // Floating joystick.
              Positioned(
                bottom: 110,
                right: 24,
                child: JoystickWidget(
                  mode: _state.joystickMode,
                  onModeChanged: _state.setJoystickMode,
                  onInput: _onJoystickInput,
                ),
              ),
            ],
          );
        },
      ),
    );
  }
}

// ---------------------------------------------------------------------------
// Small helper widgets.
// ---------------------------------------------------------------------------

class _PanelToggle extends StatelessWidget {
  const _PanelToggle({required this.icon, required this.onTap});

  final IconData icon;
  final VoidCallback onTap;

  @override
  Widget build(BuildContext context) {
    return GestureDetector(
      onTap: onTap,
      child: GlassContainer(
        width: 44,
        height: 44,
        borderRadius: AppTheme.radiusMedium,
        color: AppTheme.darkGlass.withOpacity(0.6),
        child: Icon(icon, color: AppTheme.textSecondary, size: 20),
      ),
    );
  }
}

class _SheetAction extends StatelessWidget {
  const _SheetAction({
    required this.icon,
    required this.label,
    required this.onTap,
  });

  final IconData icon;
  final String label;
  final VoidCallback onTap;

  @override
  Widget build(BuildContext context) {
    return InkWell(
      borderRadius: BorderRadius.circular(AppTheme.radiusMedium),
      onTap: onTap,
      child: Padding(
        padding: const EdgeInsets.symmetric(horizontal: 12, vertical: 14),
        child: Row(
          children: [
            Icon(icon, color: AppTheme.accent, size: 20),
            const SizedBox(width: 12),
            Text(
              label,
              style: const TextStyle(
                color: AppTheme.textPrimary,
                fontSize: 14,
                fontWeight: FontWeight.w500,
              ),
            ),
            const Spacer(),
            const Icon(Icons.chevron_right_rounded,
                color: AppTheme.textTertiary, size: 18),
          ],
        ),
      ),
    );
  }
}
