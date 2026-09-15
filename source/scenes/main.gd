# SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
# SPDX-License-Identifier: GPL-2.0-or-later
#
# scenes/main.gd — Main UI script untuk Feather-Krita app
#
# Handles:
#   - Tool selection (Draw, Erase, Shapes, Liquify, Select, Light, Export, Settings)
#   - Brush size/opacity/color controls
#   - 3D drawing (raycast → UV → Krita brush dab → texture paint)
#   - Joystick control for 3D manipulation (Move/Rotate/Scale/Liquify)
#   - Mirror toggle (X/Y/Z)
#   - Export with lock check (freemium model)
#   - Camera navigation (pinch zoom, two-finger rotate/pan)

extends Node3D

# ===========================================================================
# Tool enum (must match StrokeType / context menu)
# ===========================================================================
enum Tool {
        DRAW,
        ERASE,
        SHAPES,
        LQUIFY,  # Liquify
        SELECT,
        LIGHT,
        EXPORT,
        SETTINGS,
}

# Liquify modes (must match C++ LiquifyMode enum)
enum LiquifyMode {
        PUSH = 0,
        PULL = 1,
        SMOOTH = 2,
        INFLATE = 3,
        DEFLATE = 4,
        TWIRL = 5,
}

# Joystick modes
enum JoystickMode {
        MOVE = 0,
        ROTATE = 1,
        SCALE = 2,
        LIQUIFY = 3,
}

# Mirror axis flags (matches C++ MirrorAxis)
const MIRROR_NONE := 0
const MIRROR_X    := 1
const MIRROR_Y    := 2
const MIRROR_Z    := 4
const MIRROR_ALL  := 7

# Export format constants (matches C++ ExportFormat)
const FMT_PNG     := 0
const FMT_JPEG    := 1
const FMT_GIF     := 2
const FMT_MP4     := 3
const FMT_OBJ     := 4
const FMT_GLTF    := 5
const FMT_FEATHER := 6
const FMT_STL     := 7

# ===========================================================================
# Node references
# ===========================================================================
@onready var camera: Camera3D = $Camera3D
@onready var guide_mesh: MeshInstance3D = $GuideSurfaceMesh
@onready var strokes_root: Node3D = $StrokesRoot
@onready var light_helper: MeshInstance3D = $LightHelper
@onready var directional_light: DirectionalLight3D = $DirectionalLight3D

@onready var status_label: Label = $UI/TopBar/StatusLabel
@onready var mirror_x_ind: Label = $UI/TopBar/MirrorXIndicator
@onready var mirror_y_ind: Label = $UI/TopBar/MirrorYIndicator
@onready var mirror_z_ind: Label = $UI/TopBar/MirrorZIndicator
@onready var pro_badge: Label = $UI/TopBar/ProBadge

@onready var context_label: Label = $UI/ContextMenu/ContextLabel
@onready var context_container: HBoxContainer = $UI/ContextMenu/ContextContainer
@onready var brush_size_slider: HSlider = $UI/ContextMenu/ContextContainer/BrushSizeSlider
@onready var brush_opacity_slider: HSlider = $UI/ContextMenu/ContextContainer/BrushOpacitySlider
@onready var color_picker_button: ColorPickerButton = $UI/ContextMenu/ContextContainer/ColorPickerButton
@onready var brush_preset_button: Button = $UI/ContextMenu/ContextContainer/BrushPresetButton

@onready var draw_button: Button = $UI/BottomToolbar/ToolbarContainer/DrawButton
@onready var erase_button: Button = $UI/BottomToolbar/ToolbarContainer/EraseButton
@onready var shapes_button: Button = $UI/BottomToolbar/ToolbarContainer/ShapesButton
@onready var liquify_button: Button = $UI/BottomToolbar/ToolbarContainer/LiquifyButton
@onready var select_button: Button = $UI/BottomToolbar/ToolbarContainer/SelectButton
@onready var light_button: Button = $UI/BottomToolbar/ToolbarContainer/LightButton
@onready var export_button: Button = $UI/BottomToolbar/ToolbarContainer/ExportButton
@onready var settings_button: Button = $UI/BottomToolbar/ToolbarContainer/SettingsButton

@onready var joystick: Control = $UI/Joystick
@onready var joystick_bg: ColorRect = $UI/Joystick/JoystickBG
@onready var joystick_knob: ColorRect = $UI/Joystick/JoystickKnob
@onready var joystick_mode_label: Label = $UI/Joystick/JoystickModeLabel
@onready var joystick_mode_buttons: VBoxContainer = $UI/Joystick/JoystickModeButtons
@onready var mode_move_btn: Button = $UI/Joystick/JoystickModeButtons/ModeMove
@onready var mode_rotate_btn: Button = $UI/Joystick/JoystickModeButtons/ModeRotate
@onready var mode_scale_btn: Button = $UI/Joystick/JoystickModeButtons/ModeScale
@onready var mode_liquify_btn: Button = $UI/Joystick/JoystickModeButtons/ModeLiquify

@onready var shape_selector: Window = $UI/ShapeSelector
@onready var export_dialog: Window = $UI/ExportDialog
@onready var license_dialog: Window = $UI/LicenseDialog
@onready var lock_notice: Label = $UI/ExportDialog/ExportVBox/LockNotice
@onready var unlock_button: Button = $UI/ExportDialog/ExportVBox/UnlockButton
@onready var license_key_edit: LineEdit = $UI/LicenseDialog/LicenseVBox/LicenseKeyEdit
@onready var license_status: Label = $UI/LicenseDialog/LicenseVBox/StatusLabel2

@onready var color_palette: Panel = $UI/ColorPalette
@onready var toast: Label = $UI/Toast

# ===========================================================================
# State
# ===========================================================================
var current_tool: int = Tool.DRAW
var current_liquify_mode: int = LiquifyMode.PUSH
var current_joystick_mode: int = JoystickMode.MOVE
var mirror_axes: int = MIRROR_NONE

# Brush state
var brush_color: Color = Color.BLACK
var brush_size: float = 50.0
var brush_opacity: float = 1.0
var brush_preset_path: String = "res://brushes/basic.kpp"

# Drawing state
var is_drawing: bool = false
var current_stroke_points: PackedVector3Array = PackedVector3Array()
var current_stroke_pressures: PackedFloat32Array = PackedFloat32Array()

# Camera navigation state
var is_orbiting: bool = false
var is_panning: bool = false
var orbit_prev_pos: Vector2 = Vector2.ZERO
var pan_prev_pos: Vector2 = Vector2.ZERO
var camera_distance: float = 4.0
var camera_yaw: float = 0.0
var camera_pitch: float = 0.4
var camera_target: Vector3 = Vector3.ZERO

# Joystick state
var joystick_active: bool = false
var joystick_center: Vector2 = Vector2.ZERO
var joystick_value: Vector2 = Vector2.ZERO

# Stroke selection
var selected_stroke_id: String = ""

# Selected export format (for export dialog)
var selected_export_format: int = FMT_PNG

# GDExtension objects
var guide_surface: Resource = null  # FeatherGuideSurface
var stroke_manager: RefCounted = null  # FeatherStrokeManager
var export_lock: RefCounted = null  # FeatherExportLock
var krita_brush: RefCounted = null  # FeatherKritaBrush
var texture_painter: RefCounted = null  # FeatherTexturePainter

# ===========================================================================
# Lifecycle
# ===========================================================================
func _ready() -> void:
        _connect_internal_signals()
        _init_gdextension()
        _init_camera()
        _init_joystick()
        _set_tool(Tool.DRAW)
        _update_pro_badge()
        _update_mirror_indicators()
        
        # Load brush presets list (if any folder is configured)
        # krita_brush.list_presets("res://brushes/")
        
        _show_toast("Welcome to Feather Krita", 2.0)


func _connect_internal_signals() -> void:
        # Joystick mode buttons
        mode_move_btn.pressed.connect(func(): _set_joystick_mode(JoystickMode.MOVE))
        mode_rotate_btn.pressed.connect(func(): _set_joystick_mode(JoystickMode.ROTATE))
        mode_scale_btn.pressed.connect(func(): _set_joystick_mode(JoystickMode.SCALE))
        mode_liquify_btn.pressed.connect(func(): _set_joystick_mode(JoystickMode.LIQUIFY))
        
        # Color palette swatches
        for color_rect in color_palette.get_node("ColorGrid").get_children():
                if color_rect is ColorRect:
                        color_rect.gui_input.connect(_on_color_swatch_input.bind(color_rect))


func _init_gdextension() -> void:
        # Load GDExtension classes (registered by C++ entry point)
        var FeatherGuideSurface = load("res://feather_krita.gdextension")
        if FeatherGuideSurface == null:
                push_warning("FeatherGuideSurface GDExtension class not found. Build the C++ library first.")
        
        # Instantiate the GDExtension objects.
        # These use ClassDB-registered classes, so we instantiate via `new()`.
        guide_surface = _gdnew("FeatherGuideSurface")
        stroke_manager = _gdnew("FeatherStrokeManager")
        export_lock = _gdnew("FeatherExportLock")
        krita_brush = _gdnew("FeatherKritaBrush")
        texture_painter = _gdnew("FeatherTexturePainter")
        
        if guide_surface and export_lock:
                export_lock.load_stored_license()
                # Connect to license_changed signal
                if export_lock.has_signal("license_changed"):
                        export_lock.license_changed.connect(_on_license_changed)
                
                # Create default guide surface (sphere)
                guide_surface.create_sphere(1.0, 48, 32)
                _apply_guide_surface_to_mesh()
        else:
                push_error("Failed to instantiate GDExtension classes. Check build.")


func _gdnew(class_name_str: String) -> Variant:
        # Instantiate a ClassDB-registered class by name.
        if not ClassDB.class_exists(class_name_str):
                push_error("ClassDB has no class: " + class_name_str)
                return null
        return ClassDB.instantiate(class_name_str)


func _init_camera() -> void:
        _update_camera_transform()


func _init_joystick() -> void:
        joystick_center = joystick.global_position + joystick.size * 0.5
        joystick.gui_input.connect(_on_joystick_gui_input)


# ===========================================================================
# Input handling
# ===========================================================================
func _unhandled_input(event: InputEvent) -> void:
        # === Keyboard shortcuts ===
        if event is InputEventKey and event.pressed:
                var key_event: InputEventKey = event
                if key_event.is_command_or_control_pressed():
                        if key_event.keycode == KEY_Z:
                                if key_event.shift_pressed:
                                        _redo()
                                else:
                                        _undo()
                                get_viewport().set_input_as_handled()
                                return
        
        # === Mouse / pen input ===
        if event is InputEventMouseButton:
                _handle_mouse_button(event)
        elif event is InputEventMouseMotion:
                _handle_mouse_motion(event)
        
        # === Touch input ===
        if event is InputEventScreenTouch:
                _handle_screen_touch(event)
        elif event is InputEventScreenDrag:
                _handle_screen_drag(event)


func _handle_mouse_button(event: InputEventMouseButton) -> void:
        # Camera navigation
        if event.button_index == MOUSE_BUTTON_MIDDLE or \
           (event.button_index == MOUSE_BUTTON_RIGHT and event.alt_pressed):
                if event.pressed:
                        is_orbiting = true
                        orbit_prev_pos = event.position
                else:
                        is_orbiting = false
                return
        
        if event.button_index == MOUSE_BUTTON_RIGHT and event.shift_pressed:
                if event.pressed:
                        is_panning = true
                        pan_prev_pos = event.position
                else:
                        is_panning = false
                return
        
        # Zoom
        if event.button_index == MOUSE_BUTTON_WHEEL_UP:
                camera_distance = max(0.5, camera_distance * 0.9)
                _update_camera_transform()
                return
        if event.button_index == MOUSE_BUTTON_WHEEL_DOWN:
                camera_distance = min(100.0, camera_distance * 1.1)
                _update_camera_transform()
                return
        
        # Drawing
        if event.button_index == MOUSE_BUTTON_LEFT:
                if event.pressed:
                        _start_drawing(event.position, 1.0)
                else:
                        _end_drawing()


func _handle_mouse_motion(event: InputEventMouseMotion) -> void:
        if is_orbiting:
                var delta: Vector2 = event.position - orbit_prev_pos
                camera_yaw -= delta.x * 0.01
                camera_pitch = clamp(camera_pitch + delta.y * 0.01, -1.5, 1.5)
                orbit_prev_pos = event.position
                _update_camera_transform()
                return
        
        if is_panning:
                var delta: Vector2 = event.position - pan_prev_pos
                var right := camera.global_transform.basis.x
                var up := camera.global_transform.basis.y
                camera_target -= right * delta.x * 0.005 * camera_distance
                camera_target += up * delta.y * 0.005 * camera_distance
                pan_prev_pos = event.position
                _update_camera_transform()
                return
        
        if is_drawing:
                var pressure: float = 1.0
                if event is InputEventMouseMotion:
                        # Godot 4 doesn't expose pen pressure via InputEventMouseMotion directly;
                        # use InputEventPanGesture or tablet driver for real pressure.
                        pressure = 1.0
                _continue_drawing(event.position, pressure)


func _handle_screen_touch(event: InputEventScreenTouch) -> void:
        # Single-finger draw; two-finger gestures handled in drag
        if event.index == 0 and not is_orbiting and not is_panning:
                if event.pressed:
                        _start_drawing(event.position, 1.0)
                else:
                        _end_drawing()


func _handle_screen_drag(event: InputEventScreenDrag) -> void:
        # Two-finger gestures: rotate / pinch zoom
        if event.index == 1:
                # Activate orbit on second finger
                is_orbiting = true
                orbit_prev_pos = event.position
        elif event.index == 0 and is_drawing:
                _continue_drawing(event.position, 1.0)


# ===========================================================================
# Drawing
# ===========================================================================
func _start_drawing(screen_pos: Vector2, pressure: float) -> void:
        if current_tool == Tool.DRAW or current_tool == Tool.ERASE:
                var hit := _raycast_guide(screen_pos)
                if hit.get("hit", false):
                        is_drawing = true
                        current_stroke_points = PackedVector3Array()
                        current_stroke_pressures = PackedFloat32Array()
                        current_stroke_points.append(hit.point)
                        current_stroke_pressures.append(pressure)
                        
                        if krita_brush and krita_brush.is_ready():
                                texture_painter.set_eraser_mode(current_tool == Tool.ERASE)
                                var dab := krita_brush.generate_dab(Vector2(hit.uv.x * 2048.0, hit.uv.y * 2048.0), pressure)
                                texture_painter.apply_dab(dab, Vector2(hit.uv.x, hit.uv.y))
                                _update_guide_texture()
        
        elif current_tool == Tool.SELECT:
                # Pick stroke at screen_pos
                var world_pos := _screen_to_world(screen_pos)
                var id: String = stroke_manager.pick_stroke(world_pos, 0.5)
                if id != "":
                        selected_stroke_id = id
                        _show_toast("Stroke selected", 1.0)
        
        elif current_tool == Tool.LQUIFY:
                # Liquify at cursor
                var world_pos := _screen_to_world(screen_pos)
                var params := {
                        "center": world_pos,
                        "radius": 0.5,
                        "strength": 0.2,
                        "direction": camera.global_transform.basis.z
                }
                if selected_stroke_id != "":
                        stroke_manager.liquify(selected_stroke_id, current_liquify_mode,
                                world_pos, 0.5, 0.2, camera.global_transform.basis.z)
                else:
                        stroke_manager.liquify_selected(current_liquify_mode,
                                world_pos, 0.5, 0.2, camera.global_transform.basis.z)


func _continue_drawing(screen_pos: Vector2, pressure: float) -> void:
        if not is_drawing:
                return
        var hit := _raycast_guide(screen_pos)
        if hit.get("hit", false):
                var p: Vector3 = hit.point
                # Skip if too close to last point (avoid duplicate)
                if current_stroke_points.size() > 0:
                        var last: Vector3 = current_stroke_points[current_stroke_points.size() - 1]
                        if (p - last).length_squared() < 0.0001:
                                return
                current_stroke_points.append(p)
                current_stroke_pressures.append(pressure)
                
                # Paint dab
                if krita_brush and krita_brush.is_ready():
                        var dab := krita_brush.generate_dab(Vector2(hit.uv.x * 2048.0, hit.uv.y * 2048.0), pressure)
                        texture_painter.apply_dab(dab, Vector2(hit.uv.x, hit.uv.y))
                        _update_guide_texture()


func _end_drawing() -> void:
        if not is_drawing:
                return
        is_drawing = false
        
        if current_stroke_points.size() >= 2:
                # Commit stroke to StrokeManager
                if stroke_manager:
                        var id: String = stroke_manager.add_freehand(
                                current_stroke_points, brush_color, brush_size * 0.001, brush_preset_path
                        )
                        if mirror_axes != MIRROR_NONE:
                                # Update live mirror if enabled
                                stroke_manager.enable_live_mirror(mirror_axes)
                        _render_stroke(id)
                        _show_toast("Stroke added (" + str(current_stroke_points.size()) + " pts)", 1.5)
        
        current_stroke_points = PackedVector3Array()
        current_stroke_pressures = PackedFloat32Array()


# ===========================================================================
# Raycast / world conversion
# ===========================================================================
func _raycast_guide(screen_pos: Vector2) -> Dictionary:
        if guide_surface == null:
                return {"hit": false}
        var origin: Vector3 = camera.project_ray_origin(screen_pos)
        var dir: Vector3 = camera.project_ray_normal(screen_pos)
        var ray_dict := {
                "origin": origin,
                "direction": dir,
        }
        return guide_surface.raycast(ray_dict)


func _screen_to_world(screen_pos: Vector2) -> Vector3:
        var origin: Vector3 = camera.project_ray_origin(screen_pos)
        var dir: Vector3 = camera.project_ray_normal(screen_pos)
        # Project to a point at fixed distance (for picking/liquify)
        return origin + dir * camera_distance


# ===========================================================================
# Camera
# ===========================================================================
func _update_camera_transform() -> void:
        var x := sin(camera_yaw) * cos(camera_pitch) * camera_distance
        var y := sin(camera_pitch) * camera_distance
        var z := cos(camera_yaw) * cos(camera_pitch) * camera_distance
        camera.global_transform.origin = camera_target + Vector3(x, y, z)
        camera.look_at(camera_target, Vector3.UP)


# ===========================================================================
# Tool selection
# ===========================================================================
func _set_tool(tool: int) -> void:
        current_tool = tool
        # Reset button states
        for btn in [draw_button, erase_button, shapes_button, liquify_button,
                                select_button, light_button, export_button, settings_button]:
                btn.button_pressed = false
        
        var tool_name: String = ""
        match tool:
                Tool.DRAW:
                        draw_button.button_pressed = true
                        tool_name = "Draw"
                        context_label.text = "Tool: Draw"
                Tool.ERASE:
                        erase_button.button_pressed = true
                        tool_name = "Erase"
                        context_label.text = "Tool: Erase"
                        if texture_painter:
                                texture_painter.set_eraser_mode(true)
                Tool.SHAPES:
                        shapes_button.button_pressed = true
                        tool_name = "Shapes"
                        context_label.text = "Tool: Shapes — pick a guide surface"
                        shape_selector.popup_centered()
                Tool.LQUIFY:
                        liquify_button.button_pressed = true
                        tool_name = "Liquify"
                        context_label.text = "Tool: Liquify — drag to push/pull"
                Tool.SELECT:
                        select_button.button_pressed = true
                        tool_name = "Select"
                        context_label.text = "Tool: Select — tap a stroke"
                Tool.LIGHT:
                        light_button.button_pressed = true
                        tool_name = "Light"
                        context_label.text = "Tool: Light"
                        _cycle_light_preset()
                Tool.EXPORT:
                        export_button.button_pressed = true
                        tool_name = "Export"
                        context_label.text = "Tool: Export"
                        _open_export_dialog()
                        return  # Don't change drawing state
                Tool.SETTINGS:
                        settings_button.button_pressed = true
                        tool_name = "Settings"
                        context_label.text = "Tool: Settings"
        
        if tool != Tool.ERASE and texture_painter:
                texture_painter.set_eraser_mode(false)
        
        status_label.text = "Feather Krita — " + tool_name


# ===========================================================================
# Joystick
# ===========================================================================
func _set_joystick_mode(mode: int) -> void:
        current_joystick_mode = mode
        for btn in [mode_move_btn, mode_rotate_btn, mode_scale_btn, mode_liquify_btn]:
                btn.button_pressed = false
        match mode:
                JoystickMode.MOVE:
                        mode_move_btn.button_pressed = true
                        joystick_mode_label.text = "Move"
                JoystickMode.ROTATE:
                        mode_rotate_btn.button_pressed = true
                        joystick_mode_label.text = "Rotate"
                JoystickMode.SCALE:
                        mode_scale_btn.button_pressed = true
                        joystick_mode_label.text = "Scale"
                JoystickMode.LIQUIFY:
                        mode_liquify_btn.button_pressed = true
                        joystick_mode_label.text = "Liquify"


func _on_joystick_gui_input(event: InputEvent) -> void:
        if event is InputEventMouseButton:
                if event.button_index == MOUSE_BUTTON_LEFT:
                        if event.pressed:
                                joystick_active = true
                                joystick_center = joystick.global_position + joystick.size * 0.5
                                _update_joystick_knob(event.position)
                        else:
                                joystick_active = false
                                joystick_value = Vector2.ZERO
                                joystick_knob.position = joystick.size * 0.5 - joystick_knob.size * 0.5
        elif event is InputEventMouseMotion and joystick_active:
                _update_joystick_knob(event.position)


func _update_joystick_knob(local_pos: Vector2) -> void:
        var center := joystick.size * 0.5
        var delta := local_pos - center
        var radius: float = min(joystick.size.x, joystick.size.y) * 0.4
        if delta.length() > radius:
                delta = delta.normalized() * radius
        joystick_value = delta / radius
        joystick_knob.position = center + delta - joystick_knob.size * 0.5


func _process(_delta: float) -> void:
        if joystick_active and joystick_value.length() > 0.05:
                _apply_joystick(joystick_value, _delta)
        
        # Update toast alpha
        if toast.visible:
                var mod := toast.modulate
                mod.a -= _delta * 0.5
                if mod.a <= 0.0:
                        toast.visible = false
                else:
                        toast.modulate = mod


func _apply_joystick(value: Vector2, delta: float) -> void:
        if selected_stroke_id == "" and current_joystick_mode != JoystickMode.LIQUIFY:
                return
        
        var speed := 2.0 * delta
        match current_joystick_mode:
                JoystickMode.MOVE:
                        var right := camera.global_transform.basis.x
                        var up := camera.global_transform.basis.y
                        var delta_v := (right * value.x + up * -value.y) * speed
                        stroke_manager.move_stroke(selected_stroke_id, delta_v)
                JoystickMode.ROTATE:
                        var rot := Vector3(value.y * 90.0, value.x * 90.0, 0.0) * delta
                        stroke_manager.rotate_stroke(selected_stroke_id, rot)
                JoystickMode.SCALE:
                        var s := 1.0 + value.y * speed * 0.5
                        stroke_manager.scale_stroke_uniform(selected_stroke_id, s)
                JoystickMode.LIQUIFY:
                        # Liquify push in joystick direction
                        var right := camera.global_transform.basis.x
                        var up := camera.global_transform.basis.y
                        var dir := (right * value.x + up * -value.y).normalized()
                        var center := _screen_to_world(get_viewport().get_visible_rect().size * 0.5)
                        if selected_stroke_id != "":
                                stroke_manager.liquify(selected_stroke_id, LiquifyMode.PUSH,
                                        center, 1.0, speed, dir)
                        else:
                                stroke_manager.liquify_selected(LiquifyMode.PUSH,
                                        center, 1.0, speed, dir)


# ===========================================================================
# Guide surface rendering
# ===========================================================================
func _apply_guide_surface_to_mesh() -> void:
        if guide_surface == null or guide_mesh == null:
                return
        
        var vertices := guide_surface.get_vertices() as PackedVector3Array
        var normals := guide_surface.get_normals() as PackedVector3Array
        var uvs := guide_surface.get_uvs() as PackedVector2Array
        var indices := guide_surface.get_indices() as PackedInt32Array
        
        if vertices.size() == 0:
                guide_mesh.mesh = null
                return
        
        var arr := []
        arr.resize(Mesh.ARRAY_MAX)
        arr[Mesh.ARRAY_VERTEX] = vertices
        arr[Mesh.ARRAY_NORMAL] = normals
        arr[Mesh.ARRAY_TEX_UV] = uvs
        arr[Mesh.ARRAY_INDEX] = indices
        
        var arr_mesh := ArrayMesh.new()
        arr_mesh.add_surface_from_arrays(Mesh.PRIMITIVE_TRIANGLES, arr)
        guide_mesh.mesh = arr_mesh
        guide_mesh.material_override = null
        
        # Create new material if none
        if guide_mesh.material_override == null:
                var mat := StandardMaterial3D.new()
                mat.albedo_color = Color(0.6, 0.7, 0.9, 0.4)
                mat.transparency = BaseMaterial3D.TRANSPARENCY_ALPHA
                mat.roughness = 0.8
                guide_mesh.material_override = mat
        
        # Initialize texture painter if not yet
        if texture_painter:
                texture_painter.initialize(Color(0, 0, 0, 0))
                _update_guide_texture()


func _update_guide_texture() -> void:
        if texture_painter == null or guide_mesh == null:
                return
        var data := texture_painter.get_texture_data() as PackedByteArray
        if data.size() == 0:
                return
        var img := Image.create_from_data(
                texture_painter.get_width(),
                texture_painter.get_height(),
                false, Image.FORMAT_RGBA8, data
        )
        var tex := ImageTexture.create_from_image(img)
        var mat := guide_mesh.material_override as StandardMaterial3D
        if mat:
                mat.albedo_texture = tex


# ===========================================================================
# Stroke rendering
# ===========================================================================
func _render_stroke(stroke_id: String) -> void:
        if stroke_manager == null or strokes_root == null:
                return
        var s := stroke_manager.get_stroke(stroke_id)
        if s.is_empty():
                return
        
        var positions: PackedVector3Array = s.get("positions", PackedVector3Array())
        if positions.size() < 2:
                return
        
        # Build a tube mesh from positions (simple version: line as immediate)
        var mi := MeshInstance3D.new()
        mi.name = "Stroke_" + stroke_id
        
        # Simple tube via CSGPolygon3D path
        var path := Path3D.new()
        var curve := Curve3D.new()
        for p in positions:
                curve.add_point(p)
        path.curve = curve
        
        var poly := CSGPolygon3D.new()
        poly.mode = CSGPolygon3D.MODE_PATH
        poly.path_node = path.get_path()
        poly.polygon = PackedVector2Array([
                Vector2(-0.05, -0.05),
                Vector2(0.05, -0.05),
                Vector2(0.05, 0.05),
                Vector2(-0.05, 0.05),
        ])
        poly.smooth_faces = true
        
        var mat := StandardMaterial3D.new()
        mat.albedo_color = s.get("color", Color.BLACK)
        mat.roughness = 0.5
        poly.material = mat
        
        mi.add_child(path)
        mi.add_child(poly)
        strokes_root.add_child(mi)


# ===========================================================================
# Mirror
# ===========================================================================
func _toggle_mirror_axis(axis: int) -> void:
        if mirror_axes & axis:
                mirror_axes &= ~axis
        else:
                mirror_axes |= axis
        _update_mirror_indicators()
        if stroke_manager:
                if mirror_axes == MIRROR_NONE:
                        stroke_manager.disable_live_mirror()
                else:
                        stroke_manager.enable_live_mirror(mirror_axes)


func _update_mirror_indicators() -> void:
        mirror_x_ind.modulate = Color(0.3, 0.9, 0.4, 1) if (mirror_axes & MIRROR_X) else Color(0.5, 0.5, 0.5, 1)
        mirror_y_ind.modulate = Color(0.3, 0.9, 0.4, 1) if (mirror_axes & MIRROR_Y) else Color(0.5, 0.5, 0.5, 1)
        mirror_z_ind.modulate = Color(0.3, 0.9, 0.4, 1) if (mirror_axes & MIRROR_Z) else Color(0.5, 0.5, 0.5, 1)


# ===========================================================================
# Light presets
# ===========================================================================
func _cycle_light_preset() -> void:
        # Single tap light setup (Feather-style)
        # Cycle through a few lighting moods
        var presets := [
                {"color": Color(1, 0.96, 0.88, 1), "energy": 1.2, "yaw": 30.0, "pitch": 45.0},
                {"color": Color(0.8, 0.85, 1.0, 1), "energy": 1.5, "yaw": -45.0, "pitch": 60.0},
                {"color": Color(1.0, 0.7, 0.5, 1), "energy": 1.0, "yaw": 60.0, "pitch": 25.0},
                {"color": Color(1.0, 1.0, 1.0, 1), "energy": 0.8, "yaw": 0.0, "pitch": 90.0},
        ]
        var idx := int(directional_light.light_energy * 10) % presets.size()
        var p: Dictionary = presets[idx]
        directional_light.light_color = p.color
        directional_light.light_energy = p.energy
        directional_light.rotation_degrees = Vector3(p.pitch, p.yaw, 0)
        light_helper.global_position = directional_light.global_position
        _show_toast("Light: preset " + str(idx + 1) + "/" + str(presets.size()), 1.5)


# ===========================================================================
# Export
# ===========================================================================
func _open_export_dialog() -> void:
        if export_lock == null:
                _show_toast("Export system not initialized", 2.0)
                return
        _refresh_export_dialog()
        export_dialog.popup_centered()


func _refresh_export_dialog() -> void:
        var is_pro: bool = export_lock.is_pro_version()
        lock_notice.visible = not is_pro
        unlock_button.visible = not is_pro
        pro_badge.text = "PRO" if is_pro else "FREE"
        pro_badge.modulate = Color(0.3, 0.9, 0.4, 1) if is_pro else Color(0.8, 0.6, 0.2, 1)


func _do_export(format: int) -> void:
        if export_lock == null:
                _show_toast("Export system not initialized", 2.0)
                return
        if not export_lock.can_export(format):
                _show_toast("Export locked — upgrade to Pro", 2.0)
                _open_license_dialog()
                return
        
        var fmt_name: String = export_lock.format_name(format)
        var ext: String = export_lock.format_extension(format)
        var path: String = "user://export_" + str(Time.get_ticks_msec()) + "." + ext
        
        # Format-specific export logic
        match format:
                FMT_PNG, FMT_JPEG:
                        _export_image(path, format)
                FMT_OBJ, FMT_GLTF, FMT_STL:
                        _export_3d(path, format)
                FMT_GIF, FMT_MP4:
                        _show_toast("Animation export coming soon", 2.0)
                        return
                FMT_FEATHER:
                        _export_feather(path)
        
        _show_toast("Exported: " + fmt_name + " → " + path, 3.0)


func _export_image(path: String, format: int) -> void:
        # Capture viewport as image
        var img := get_viewport().get_texture().get_image()
        if format == FMT_JPEG:
                img.save_jpg(path)
        else:
                img.save_png(path)


func _export_3d(path: String, format: int) -> void:
        # Export strokes as OBJ/GLTF using MeshInstance3D
        # (Simplified: merge all stroke meshes)
        var merged := ArrayMesh.new()
        for child in strokes_root.get_children():
                if child is MeshInstance3D:
                        var mesh := (child as MeshInstance3D).mesh
                        if mesh:
                                # Bake transform into mesh
                                var t := (child as MeshInstance3D).global_transform
                                var xform := Transform3D(t.basis, t.origin)
                                for s in range(mesh.get_surface_count()):
                                        var arrays := mesh.surface_get_arrays(s)
                                        var verts := arrays[Mesh.ARRAY_VERTEX] as PackedVector3Array
                                        for i in range(verts.size()):
                                                verts[i] = xform * verts[i]
                                        arrays[Mesh.ARRAY_VERTEX] = verts
                                        merged.add_surface_from_arrays(Mesh.PRIMITIVE_TRIANGLES, arrays)
        
        ResourceSaver.save(merged, path)


func _export_feather(path: String) -> void:
        # Save project file: serialized stroke list + scene state
        var data := {
                "version": "0.1.0",
                "strokes": stroke_manager.get_all_stroke_ids(),
                "camera": {
                        "distance": camera_distance,
                        "yaw": camera_yaw,
                        "pitch": camera_pitch,
                        "target": [camera_target.x, camera_target.y, camera_target.z],
                },
        }
        var f := FileAccess.open(path, FileAccess.WRITE)
        f.store_string(JSON.stringify(data, "\t"))
        f.close()


func _open_license_dialog() -> void:
        license_dialog.popup_centered()
        license_status.text = ""
        if export_lock.is_trial_active():
                license_status.text = "Trial active — " + str(export_lock.trial_days_remaining()) + " days left"
        elif export_lock.is_pro_version():
                license_status.text = "Pro version active"


# ===========================================================================
# UI signal handlers
# ===========================================================================
func _on_draw_button_pressed() -> void: _set_tool(Tool.DRAW)
func _on_erase_button_pressed() -> void: _set_tool(Tool.ERASE)
func _on_shapes_button_pressed() -> void: _set_tool(Tool.SHAPES)
func _on_liquify_button_pressed() -> void: _set_tool(Tool.LQUIFY)
func _on_select_button_pressed() -> void: _set_tool(Tool.SELECT)
func _on_light_button_pressed() -> void: _set_tool(Tool.LIGHT)
func _on_export_button_pressed() -> void: _set_tool(Tool.EXPORT)
func _on_settings_button_pressed() -> void: _set_tool(Tool.SETTINGS)


func _on_brush_size_changed(value: float) -> void:
        brush_size = value
        if krita_brush:
                krita_brush.set_brush_size(int(value))


func _on_brush_opacity_changed(value: float) -> void:
        brush_opacity = value
        if krita_brush:
                krita_brush.set_brush_opacity(value)


func _on_color_changed(color: Color) -> void:
        brush_color = color
        if krita_brush:
                krita_brush.set_brush_color(color)


func _on_color_swatch_input(event: InputEvent, color_rect: ColorRect) -> void:
        if event is InputEventMouseButton and event.pressed:
                color_picker_button.color = color_rect.color
                brush_color = color_rect.color
                if krita_brush:
                        krita_brush.set_brush_color(color_rect.color)
                color_palette.visible = false


func _on_brush_preset_button_pressed() -> void:
        _show_toast("Brush preset selector coming soon", 1.5)


# === Shape selector ===
func _on_shape_sphere_pressed() -> void:
        guide_surface.create_sphere(1.0, 48, 32)
        _apply_guide_surface_to_mesh()
        shape_selector.visible = false
        _show_toast("Guide: Sphere", 1.5)

func _on_shape_cylinder_pressed() -> void:
        guide_surface.create_cylinder(1.0, 2.0, 48)
        _apply_guide_surface_to_mesh()
        shape_selector.visible = false
        _show_toast("Guide: Cylinder", 1.5)

func _on_shape_cone_pressed() -> void:
        guide_surface.create_cone(1.0, 2.0, 48)
        _apply_guide_surface_to_mesh()
        shape_selector.visible = false
        _show_toast("Guide: Cone", 1.5)

func _on_shape_ring_pressed() -> void:
        guide_surface.create_ring(1.5, 0.25, 64, 24)
        _apply_guide_surface_to_mesh()
        shape_selector.visible = false
        _show_toast("Guide: Ring", 1.5)

func _on_shape_none_pressed() -> void:
        guide_mesh.mesh = null
        shape_selector.visible = false
        _show_toast("Guide removed", 1.5)


# === Export dialog ===
func _on_format_png_pressed() -> void:     selected_export_format = FMT_PNG
func _on_format_jpeg_pressed() -> void:    selected_export_format = FMT_JPEG
func _on_format_obj_pressed() -> void:     selected_export_format = FMT_OBJ
func _on_format_gltf_pressed() -> void:    selected_export_format = FMT_GLTF
func _on_format_mp4_pressed() -> void:     selected_export_format = FMT_MP4
func _on_format_gif_pressed() -> void:     selected_export_format = FMT_GIF
func _on_format_feather_pressed() -> void: selected_export_format = FMT_FEATHER

func _on_export_now_pressed() -> void:
        _do_export(selected_export_format)
        export_dialog.visible = false

func _on_export_cancel_pressed() -> void:
        export_dialog.visible = false

func _on_export_unlock_pressed() -> void:
        _open_license_dialog()


# === License dialog ===
func _on_license_activate_pressed() -> void:
        var key: String = license_key_edit.text.strip_edges()
        if key == "":
                license_status.text = "Please enter a license key"
                return
        if not export_lock.is_valid_key_format(key):
                license_status.text = "Invalid key format (need XXXX-XXXX-XXXX-XXXX)"
                return
        if export_lock.unlock_export(key):
                license_status.text = "✓ Pro activated!"
                license_dialog.visible = false
                _show_toast("Pro unlocked — export enabled", 2.0)
        else:
                license_status.text = "✗ Invalid license key"


func _on_license_close_pressed() -> void:
        license_dialog.visible = false


func _on_license_trial_pressed() -> void:
        if export_lock.start_trial(7):
                license_status.text = "✓ Trial started — 7 days"
                _show_toast("7-day trial started", 2.0)
                _refresh_export_dialog()
        else:
                license_status.text = "✗ Trial unavailable (already used or Pro active)"


func _on_license_store_pressed() -> void:
        # Trigger platform store purchase
        # In production: call Google Play Billing or Windows Store API
        _show_toast("Opening store...", 1.5)
        # Simulated store callback:
        # export_lock.unlock_from_store(2 /* GooglePlay */, "GPA.XXXX-XXXX-XXXX", "user@email.com")
        license_status.text = "Store purchase requires platform integration"


# === License change callback ===
func _on_license_changed(is_pro: bool) -> void:
        pro_badge.text = "PRO" if is_pro else "FREE"
        pro_badge.modulate = Color(0.3, 0.9, 0.4, 1) if is_pro else Color(0.8, 0.6, 0.2, 1)
        _refresh_export_dialog()


func _update_pro_badge() -> void:
        if export_lock == null:
                pro_badge.text = "FREE"
                pro_badge.modulate = Color(0.8, 0.6, 0.2, 1)
                return
        var is_pro: bool = export_lock.is_pro_version()
        pro_badge.text = "PRO" if is_pro else "FREE"
        pro_badge.modulate = Color(0.3, 0.9, 0.4, 1) if is_pro else Color(0.8, 0.6, 0.2, 1)


# ===========================================================================
# Undo / Redo
# ===========================================================================
func _undo() -> void:
        if stroke_manager:
                stroke_manager.undo()
                _show_toast("Undo", 0.5)

func _redo() -> void:
        if stroke_manager:
                stroke_manager.redo()
                _show_toast("Redo", 0.5)


# ===========================================================================
# Toast
# ===========================================================================
func _show_toast(msg: String, duration: float = 2.0) -> void:
        toast.text = msg
        toast.visible = true
        toast.modulate = Color(1, 1, 1, 1)
        # Schedule fade-out via _process
