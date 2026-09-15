/*
 * SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * gdextension_entry.cpp — Godot GDExtension entry point
 *
 * File ini mendaftarkan semua C++ class Feather-Krita sebagai Godot types
 * menggunakan godot-cpp GDExtension API.
 *
 * Class yang diregister:
 *   - FeatherGuideSurface    (Resource)
 *   - FeatherStrokeManager   (RefCounted)
 *   - FeatherExportLock      (RefCounted, with signals)
 *   - FeatherKritaBrush      (RefCounted)
 *   - FeatherTexturePainter  (RefCounted)
 *
 * Class-class ini adalah WRAPPER di sekitar implementasi Qt-based
 * (GuideSurface, StrokeManager, ExportLock, KritaBrushWrapper, TexturePainter).
 * Mereka meng-convert antara Godot types (String, Vector3, PackedVector3Array,
 * PackedByteArray, Dictionary) dan Qt types (QString, QVector3D, QVector, QImage).
 *
 * Cara pakai di Godot (GDScript):
 *
 *   var surface = FeatherGuideSurface.new()
 *   surface.create_sphere(1.0, 48, 32)
 *   var ray = {"origin": Vector3(0,0,5), "direction": Vector3(0,0,-1)}
 *   var hit = surface.raycast(ray)
 *   if hit.hit:
 *       print("UV: ", hit.uv)
 *
 *   var stroke_mgr = FeatherStrokeManager.new()
 *   var stroke_id = stroke_mgr.add_freehand(points, Color.BLACK)
 *
 *   var export_lock = FeatherExportLock.new()
 *   export_lock.load_stored_license()
 *   if export_lock.can_export(0):  # 0 = PNG
 *       export_image()
 *   else:
 *       show_upgrade_dialog()
 */

#include <godot_cpp/godot.hpp>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/core/defs.hpp>
#include <godot_cpp/core/memory.hpp>
#include <godot_cpp/variant/string.hpp>
#include <godot_cpp/variant/string_name.hpp>
#include <godot_cpp/variant/vector3.hpp>
#include <godot_cpp/variant/vector2.hpp>
#include <godot_cpp/variant/color.hpp>
#include <godot_cpp/variant/dictionary.hpp>
#include <godot_cpp/variant/array.hpp>
#include <godot_cpp/variant/packed_vector3_array.hpp>
#include <godot_cpp/variant/packed_vector2_array.hpp>
#include <godot_cpp/variant/packed_int32_array.hpp>
#include <godot_cpp/variant/packed_byte_array.hpp>
#include <godot_cpp/variant/packed_float32_array.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/classes/ref_counted.hpp>
#include <godot_cpp/classes/resource.hpp>
#include <godot_cpp/classes/object.hpp>

#include <QImage>
#include <QColor>
#include <QVector>
#include <QVector3D>
#include <QVector2D>
#include <QString>
#include <QByteArray>

#include "guide_surface/guide_surface.h"
#include "stroke_manager/stroke_manager.h"
#include "export/export_lock.h"
#include "krita_bridge/krita_brush_wrapper.h"
#include "krita_bridge/texture_painter.h"

using namespace godot;

namespace {

// ===========================================================================
// Type conversion helpers: Qt ↔ Godot
// ===========================================================================

inline QString    gd2qt_string(const String& s)        { return QString::fromUtf8(s.utf8().get_data()); }
inline String     qt2gd_string(const QString& s)       { return String(s.toUtf8().constData()); }

inline QVector3D  gd2qt_vec3(const Vector3& v)         { return QVector3D(v.x, v.y, v.z); }
inline Vector3    qt2gd_vec3(const QVector3D& v)       { return Vector3(v.x(), v.y(), v.z()); }

inline QVector2D  gd2qt_vec2(const Vector2& v)         { return QVector2D(v.x, v.y); }
inline Vector2    qt2gd_vec2(const QVector2D& v)       { return Vector2(v.x(), v.y()); }

inline QColor     gd2qt_color(const Color& c)          { return QColor::fromRgbF(c.r, c.g, c.b, c.a); }
inline Color      qt2gd_color(const QColor& c)         { return Color(c.redF(), c.greenF(), c.blueF(), c.alphaF()); }

} // anonymous namespace

namespace feather {

// ===========================================================================
// FeatherGuideSurface — Godot wrapper untuk FeatherKrita::GuideSurface
// ===========================================================================

class FeatherGuideSurface : public Resource {
    GDCLASS(FeatherGuideSurface, Resource)

private:
    FeatherKrita::GuideSurface m_surface;

public:
    FeatherGuideSurface() {}
    ~FeatherGuideSurface() {}

    // === Mesh Generators ===

    bool create_sphere(double radius = 1.0, int segments_h = 48, int segments_v = 32) {
        return m_surface.createSphere(float(radius), segments_h, segments_v);
    }

    bool create_cylinder(double radius = 1.0, double height = 2.0, int segments = 48) {
        return m_surface.createCylinder(float(radius), float(height), segments);
    }

    bool create_cone(double base_radius = 1.0, double height = 2.0, int segments = 48) {
        return m_surface.createCone(float(base_radius), float(height), segments);
    }

    bool create_ring(double major_radius = 1.5, double minor_radius = 0.25,
                     int major_seg = 64, int minor_seg = 24) {
        return m_surface.createRing(float(major_radius), float(minor_radius),
                                     major_seg, minor_seg);
    }

    bool create_plane(double width = 2.0, double height = 2.0,
                      int seg_x = 16, int seg_y = 16) {
        return m_surface.createPlane(float(width), float(height), seg_x, seg_y);
    }

    // === Raycast ===

    Dictionary raycast(const Dictionary& ray_dict) const {
        Dictionary result;
        if (!ray_dict.has("origin") || !ray_dict.has("direction")) {
            result["hit"] = false;
            return result;
        }

        FeatherKrita::Ray ray;
        ray.origin    = gd2qt_vec3(ray_dict["origin"]);
        ray.direction = gd2qt_vec3(ray_dict["direction"]).normalized();

        FeatherKrita::RaycastHit hit = m_surface.raycast(ray);
        result["hit"]         = hit.hit;
        result["point"]       = qt2gd_vec3(hit.point);
        result["normal"]      = qt2gd_vec3(hit.normal);
        result["uv"]          = qt2gd_vec2(hit.uv);
        result["distance"]    = double(hit.distance);
        result["triangle_idx"]= hit.triangleIdx;
        return result;
    }

    Vector2 get_uv_from_point(const Vector3& point) const {
        QVector2D uv = m_surface.getUVFromPoint(gd2qt_vec3(point));
        return qt2gd_vec2(uv);
    }

    // === Transform ===

    void set_transform(const Vector3& position,
                       const Vector3& rotation = Vector3(),
                       const Vector3& scale = Vector3(1, 1, 1)) {
        m_surface.setTransform(gd2qt_vec3(position),
                               gd2qt_vec3(rotation),
                               gd2qt_vec3(scale));
    }

    Vector3 get_position() const { return qt2gd_vec3(m_surface.position()); }
    Vector3 get_rotation() const { return qt2gd_vec3(m_surface.rotation()); }
    Vector3 get_scale()    const { return qt2gd_vec3(m_surface.scale()); }

    // === Mesh data accessors (for Godot rendering) ===

    PackedVector3Array get_vertices() const {
        PackedVector3Array arr;
        const auto& verts = m_surface.vertices();
        arr.resize(verts.size());
        for (int i = 0; i < verts.size(); ++i) {
            arr.set(i, qt2gd_vec3(verts[i].position));
        }
        return arr;
    }

    PackedVector3Array get_normals() const {
        PackedVector3Array arr;
        const auto& verts = m_surface.vertices();
        arr.resize(verts.size());
        for (int i = 0; i < verts.size(); ++i) {
            arr.set(i, qt2gd_vec3(verts[i].normal));
        }
        return arr;
    }

    PackedVector2Array get_uvs() const {
        PackedVector2Array arr;
        const auto& verts = m_surface.vertices();
        arr.resize(verts.size());
        for (int i = 0; i < verts.size(); ++i) {
            arr.set(i, qt2gd_vec2(verts[i].uv));
        }
        return arr;
    }

    PackedInt32Array get_indices() const {
        PackedInt32Array arr;
        const auto& idx = m_surface.indices();
        arr.resize(idx.size());
        for (int i = 0; i < idx.size(); ++i) {
            arr.set(i, int(idx[i]));
        }
        return arr;
    }

    String get_type_name() const { return qt2gd_string(m_surface.typeName()); }
    int get_type() const { return int(m_surface.type()); }
    int get_vertex_count() const { return m_surface.vertexCount(); }
    int get_triangle_count() const { return m_surface.triangleCount(); }

protected:
    static void _bind_methods() {
        // Mesh generators
        ClassDB::bind_method(D_METHOD("create_sphere", "radius", "segments_h", "segments_v"),
                             &FeatherGuideSurface::create_sphere,
                             DEFVAL(1.0), DEFVAL(48), DEFVAL(32));
        ClassDB::bind_method(D_METHOD("create_cylinder", "radius", "height", "segments"),
                             &FeatherGuideSurface::create_cylinder,
                             DEFVAL(1.0), DEFVAL(2.0), DEFVAL(48));
        ClassDB::bind_method(D_METHOD("create_cone", "base_radius", "height", "segments"),
                             &FeatherGuideSurface::create_cone,
                             DEFVAL(1.0), DEFVAL(2.0), DEFVAL(48));
        ClassDB::bind_method(D_METHOD("create_ring", "major_radius", "minor_radius",
                                      "major_seg", "minor_seg"),
                             &FeatherGuideSurface::create_ring,
                             DEFVAL(1.5), DEFVAL(0.25), DEFVAL(64), DEFVAL(24));
        ClassDB::bind_method(D_METHOD("create_plane", "width", "height", "seg_x", "seg_y"),
                             &FeatherGuideSurface::create_plane,
                             DEFVAL(2.0), DEFVAL(2.0), DEFVAL(16), DEFVAL(16));

        // Raycast
        ClassDB::bind_method(D_METHOD("raycast", "ray"),
                             &FeatherGuideSurface::raycast);
        ClassDB::bind_method(D_METHOD("get_uv_from_point", "point"),
                             &FeatherGuideSurface::get_uv_from_point);

        // Transform
        ClassDB::bind_method(D_METHOD("set_transform", "position", "rotation", "scale"),
                             &FeatherGuideSurface::set_transform,
                             DEFVAL(Vector3()), DEFVAL(Vector3(1, 1, 1)));
        ClassDB::bind_method(D_METHOD("get_position"), &FeatherGuideSurface::get_position);
        ClassDB::bind_method(D_METHOD("get_rotation"), &FeatherGuideSurface::get_rotation);
        ClassDB::bind_method(D_METHOD("get_scale"),    &FeatherGuideSurface::get_scale);

        // Mesh data
        ClassDB::bind_method(D_METHOD("get_vertices"), &FeatherGuideSurface::get_vertices);
        ClassDB::bind_method(D_METHOD("get_normals"),  &FeatherGuideSurface::get_normals);
        ClassDB::bind_method(D_METHOD("get_uvs"),      &FeatherGuideSurface::get_uvs);
        ClassDB::bind_method(D_METHOD("get_indices"),  &FeatherGuideSurface::get_indices);

        ClassDB::bind_method(D_METHOD("get_type_name"),     &FeatherGuideSurface::get_type_name);
        ClassDB::bind_method(D_METHOD("get_type"),          &FeatherGuideSurface::get_type);
        ClassDB::bind_method(D_METHOD("get_vertex_count"),  &FeatherGuideSurface::get_vertex_count);
        ClassDB::bind_method(D_METHOD("get_triangle_count"),&FeatherGuideSurface::get_triangle_count);

        ADD_PROPERTY(PropertyInfo(Variant::VECTOR3, "position"), "set_transform", "get_position");
        ADD_PROPERTY(PropertyInfo(Variant::VECTOR3, "rotation"), "set_transform", "get_rotation");
        ADD_PROPERTY(PropertyInfo(Variant::VECTOR3, "scale"),    "set_transform", "get_scale");
    }
};

// ===========================================================================
// FeatherStrokeManager — Godot wrapper untuk FeatherKrita::StrokeManager
// ===========================================================================

class FeatherStrokeManager : public RefCounted {
    GDCLASS(FeatherStrokeManager, RefCounted)

private:
    FeatherKrita::StrokeManager m_mgr;

    // Convert PackedVector3Array + PackedFloat32Array → QVector<StrokePoint>
    QVector<FeatherKrita::StrokePoint> array_to_points(
        const PackedVector3Array& positions,
        const PackedFloat32Array& pressures,
        const PackedFloat32Array& thicknesses) const
    {
        QVector<FeatherKrita::StrokePoint> points;
        int n = positions.size();
        points.resize(n);
        for (int i = 0; i < n; ++i) {
            points[i].position  = gd2qt_vec3(positions[i]);
            points[i].pressure  = (i < pressures.size()) ? float(pressures[i]) : 1.0f;
            points[i].thickness = (i < thicknesses.size()) ? float(thicknesses[i]) : 1.0f;
            points[i].tilt      = 0.0f;
        }
        return points;
    }

public:
    FeatherStrokeManager() {}
    ~FeatherStrokeManager() {}

    // === Stroke Management ===

    String add_freehand(const PackedVector3Array& positions,
                        const Color& color = Color(0, 0, 0),
                        double thickness = 0.05,
                        const String& brush_preset = String()) {
        PackedFloat32Array pressures;
        PackedFloat32Array thicknesses;
        pressures.resize(positions.size());
        thicknesses.resize(positions.size());
        for (int i = 0; i < positions.size(); ++i) {
            pressures.set(i, 1.0);
            thicknesses.set(i, 1.0);
        }
        auto pts = array_to_points(positions, pressures, thicknesses);
        QString id = m_mgr.addStroke(FeatherKrita::StrokeType::Freehand, pts,
                                      gd2qt_color(color), float(thickness),
                                      gd2qt_string(brush_preset));
        return qt2gd_string(id);
    }

    String add_line(const Vector3& start, const Vector3& end,
                    const Color& color = Color(0, 0, 0),
                    double thickness = 0.05,
                    const String& brush_preset = String()) {
        QVector<FeatherKrita::StrokePoint> pts(2);
        pts[0].position = gd2qt_vec3(start);
        pts[1].position = gd2qt_vec3(end);
        pts[0].pressure = pts[1].pressure = 1.0f;
        pts[0].thickness = pts[1].thickness = 1.0f;
        QString id = m_mgr.addStroke(FeatherKrita::StrokeType::Line, pts,
                                      gd2qt_color(color), float(thickness),
                                      gd2qt_string(brush_preset));
        return qt2gd_string(id);
    }

    bool remove_stroke(const String& id) {
        return m_mgr.removeStroke(gd2qt_string(id));
    }

    void clear() { m_mgr.clear(); }
    int count() const { return m_mgr.count(); }

    // === Selection ===

    bool select_stroke(const String& id, bool exclusive = true) {
        return m_mgr.selectStroke(gd2qt_string(id), exclusive);
    }

    String pick_stroke(const Vector3& point, double max_distance = 0.5) {
        return qt2gd_string(m_mgr.pickStroke(gd2qt_vec3(point), float(max_distance)));
    }

    void deselect_all() { m_mgr.deselectAll(); }
    String get_selected_id() const { return qt2gd_string(m_mgr.selectedStrokeId()); }

    // === Transform ===

    bool move_stroke(const String& id, const Vector3& delta) {
        return m_mgr.moveStroke(gd2qt_string(id), gd2qt_vec3(delta));
    }

    bool rotate_stroke(const String& id, const Vector3& euler_delta) {
        return m_mgr.rotateStroke(gd2qt_string(id), gd2qt_vec3(euler_delta));
    }

    bool scale_stroke_uniform(const String& id, double scale_factor) {
        return m_mgr.scaleStroke(gd2qt_string(id), float(scale_factor));
    }

    bool scale_stroke(const String& id, const Vector3& scale_delta) {
        return m_mgr.scaleStroke(gd2qt_string(id), gd2qt_vec3(scale_delta));
    }

    bool set_stroke_transform(const String& id,
                              const Vector3& position,
                              const Vector3& rotation,
                              const Vector3& scale) {
        return m_mgr.setStrokeTransform(gd2qt_string(id),
                                        gd2qt_vec3(position),
                                        gd2qt_vec3(rotation),
                                        gd2qt_vec3(scale));
    }

    // === Liquify ===

    // mode: 0=Push, 1=Pull, 2=Smooth, 3=Inflate, 4=Deflate, 5=Twirl
    bool liquify(const String& id, int mode, const Vector3& center,
                 double radius, double strength, const Vector3& direction) {
        FeatherKrita::LiquifyParams params;
        params.mode      = static_cast<FeatherKrita::LiquifyMode>(mode);
        params.center    = gd2qt_vec3(center);
        params.radius    = float(radius);
        params.strength  = float(strength);
        params.direction = gd2qt_vec3(direction).normalized();
        return m_mgr.liquify(gd2qt_string(id), params);
    }

    int liquify_selected(int mode, const Vector3& center,
                         double radius, double strength,
                         const Vector3& direction) {
        FeatherKrita::LiquifyParams params;
        params.mode      = static_cast<FeatherKrita::LiquifyMode>(mode);
        params.center    = gd2qt_vec3(center);
        params.radius    = float(radius);
        params.strength  = float(strength);
        params.direction = gd2qt_vec3(direction).normalized();
        return m_mgr.liquifySelected(params);
    }

    bool smooth_stroke(const String& id, int iterations = 1) {
        return m_mgr.smoothStroke(gd2qt_string(id), iterations);
    }

    // === Mirror ===

    // axis_flags: 1=X, 2=Y, 4=Z, 7=All
    String mirror_stroke(const String& id, int axis_flags,
                         const Vector3& plane_origin = Vector3()) {
        return qt2gd_string(m_mgr.mirrorStroke(
            gd2qt_string(id),
            static_cast<FeatherKrita::MirrorAxes>(axis_flags),
            gd2qt_vec3(plane_origin)));
    }

    void enable_live_mirror(int axis_flags) {
        m_mgr.enableLiveMirror(static_cast<FeatherKrita::MirrorAxes>(axis_flags));
    }

    void disable_live_mirror() {
        m_mgr.disableLiveMirror();
    }

    int get_live_mirror_axes() const {
        return int(m_mgr.liveMirrorAxes());
    }

    // === Undo / Redo ===

    void push_undo() { m_mgr.pushUndo(); }
    bool undo()      { return m_mgr.undo(); }
    bool redo()      { return m_mgr.redo(); }

    // === Stroke data accessors (for rendering) ===

    Dictionary get_stroke(const String& id) const {
        Dictionary d;
        const FeatherKrita::Stroke* s = m_mgr.getStroke(gd2qt_string(id));
        if (!s) return d;

        d["id"]         = qt2gd_string(s->id);
        d["type"]       = int(s->type);
        d["position"]   = qt2gd_vec3(s->position);
        d["rotation"]   = qt2gd_vec3(s->rotation);
        d["scale"]      = qt2gd_vec3(s->scale);
        d["color"]      = qt2gd_color(s->color);
        d["thickness"]  = double(s->thickness);
        d["opacity"]    = double(s->opacity);
        d["brush_preset"] = qt2gd_string(s->brushPreset);
        d["visible"]    = s->visible;
        d["locked"]     = s->locked;
        d["selected"]   = s->selected;

        PackedVector3Array positions;
        PackedFloat32Array pressures;
        PackedFloat32Array thicknesses;
        positions.resize(s->points.size());
        pressures.resize(s->points.size());
        thicknesses.resize(s->points.size());
        for (int i = 0; i < s->points.size(); ++i) {
            positions.set(i,  qt2gd_vec3(s->points[i].position));
            pressures.set(i,  double(s->points[i].pressure));
            thicknesses.set(i, double(s->points[i].thickness));
        }
        d["positions"]  = positions;
        d["pressures"]  = pressures;
        d["thicknesses"]= thicknesses;
        return d;
    }

    Array get_all_stroke_ids() const {
        Array arr;
        for (const auto& s : m_mgr.strokes()) {
            arr.append(qt2gd_string(s.id));
        }
        return arr;
    }

    Vector3 get_scene_bounds_min() const {
        QVector3D mn, mx;
        m_mgr.getSceneBounds(mn, mx);
        return qt2gd_vec3(mn);
    }

    Vector3 get_scene_bounds_max() const {
        QVector3D mn, mx;
        m_mgr.getSceneBounds(mn, mx);
        return qt2gd_vec3(mx);
    }

protected:
    static void _bind_methods() {
        // Stroke management
        ClassDB::bind_method(D_METHOD("add_freehand", "positions", "color",
                                      "thickness", "brush_preset"),
                             &FeatherStrokeManager::add_freehand,
                             DEFVAL(Color(0, 0, 0)), DEFVAL(0.05), DEFVAL(String()));
        ClassDB::bind_method(D_METHOD("add_line", "start", "end", "color",
                                      "thickness", "brush_preset"),
                             &FeatherStrokeManager::add_line,
                             DEFVAL(Color(0, 0, 0)), DEFVAL(0.05), DEFVAL(String()));
        ClassDB::bind_method(D_METHOD("remove_stroke", "id"),
                             &FeatherStrokeManager::remove_stroke);
        ClassDB::bind_method(D_METHOD("clear"), &FeatherStrokeManager::clear);
        ClassDB::bind_method(D_METHOD("count"), &FeatherStrokeManager::count);

        // Selection
        ClassDB::bind_method(D_METHOD("select_stroke", "id", "exclusive"),
                             &FeatherStrokeManager::select_stroke, DEFVAL(true));
        ClassDB::bind_method(D_METHOD("pick_stroke", "point", "max_distance"),
                             &FeatherStrokeManager::pick_stroke, DEFVAL(0.5));
        ClassDB::bind_method(D_METHOD("deselect_all"),
                             &FeatherStrokeManager::deselect_all);
        ClassDB::bind_method(D_METHOD("get_selected_id"),
                             &FeatherStrokeManager::get_selected_id);

        // Transform
        ClassDB::bind_method(D_METHOD("move_stroke", "id", "delta"),
                             &FeatherStrokeManager::move_stroke);
        ClassDB::bind_method(D_METHOD("rotate_stroke", "id", "euler_delta"),
                             &FeatherStrokeManager::rotate_stroke);
        ClassDB::bind_method(D_METHOD("scale_stroke_uniform", "id", "scale_factor"),
                             &FeatherStrokeManager::scale_stroke_uniform);
        ClassDB::bind_method(D_METHOD("scale_stroke", "id", "scale_delta"),
                             &FeatherStrokeManager::scale_stroke);
        ClassDB::bind_method(D_METHOD("set_stroke_transform", "id",
                                      "position", "rotation", "scale"),
                             &FeatherStrokeManager::set_stroke_transform);

        // Liquify
        ClassDB::bind_method(D_METHOD("liquify", "id", "mode", "center",
                                      "radius", "strength", "direction"),
                             &FeatherStrokeManager::liquify);
        ClassDB::bind_method(D_METHOD("liquify_selected", "mode", "center",
                                      "radius", "strength", "direction"),
                             &FeatherStrokeManager::liquify_selected);
        ClassDB::bind_method(D_METHOD("smooth_stroke", "id", "iterations"),
                             &FeatherStrokeManager::smooth_stroke, DEFVAL(1));

        // Mirror
        ClassDB::bind_method(D_METHOD("mirror_stroke", "id", "axis_flags",
                                      "plane_origin"),
                             &FeatherStrokeManager::mirror_stroke,
                             DEFVAL(Vector3()));
        ClassDB::bind_method(D_METHOD("enable_live_mirror", "axis_flags"),
                             &FeatherStrokeManager::enable_live_mirror);
        ClassDB::bind_method(D_METHOD("disable_live_mirror"),
                             &FeatherStrokeManager::disable_live_mirror);
        ClassDB::bind_method(D_METHOD("get_live_mirror_axes"),
                             &FeatherStrokeManager::get_live_mirror_axes);

        // Undo/Redo
        ClassDB::bind_method(D_METHOD("push_undo"), &FeatherStrokeManager::push_undo);
        ClassDB::bind_method(D_METHOD("undo"),      &FeatherStrokeManager::undo);
        ClassDB::bind_method(D_METHOD("redo"),      &FeatherStrokeManager::redo);

        // Accessors
        ClassDB::bind_method(D_METHOD("get_stroke", "id"),
                             &FeatherStrokeManager::get_stroke);
        ClassDB::bind_method(D_METHOD("get_all_stroke_ids"),
                             &FeatherStrokeManager::get_all_stroke_ids);
        ClassDB::bind_method(D_METHOD("get_scene_bounds_min"),
                             &FeatherStrokeManager::get_scene_bounds_min);
        ClassDB::bind_method(D_METHOD("get_scene_bounds_max"),
                             &FeatherStrokeManager::get_scene_bounds_max);
    }
};

// ===========================================================================
// FeatherExportLock — Godot wrapper untuk FeatherKrita::ExportLock
// ===========================================================================

class FeatherExportLock : public RefCounted {
    GDCLASS(FeatherExportLock, RefCounted)

private:
    FeatherKrita::ExportLock m_lock;

public:
    FeatherExportLock() {}
    ~FeatherExportLock() {}

    // Format constants exposed to GDScript:
    //   0=PNG, 1=JPEG, 2=GIF, 3=MP4, 4=OBJ, 5=GLTF, 6=Feather, 7=STL

    bool can_export(int format) const {
        return m_lock.canExport(static_cast<FeatherKrita::ExportFormat>(format));
    }

    static bool format_requires_pro(int format) {
        return FeatherKrita::ExportLock::formatRequiresPro(
            static_cast<FeatherKrita::ExportFormat>(format));
    }

    static String format_name(int format) {
        return qt2gd_string(FeatherKrita::ExportLock::formatName(
            static_cast<FeatherKrita::ExportFormat>(format)));
    }

    static String format_extension(int format) {
        return qt2gd_string(FeatherKrita::ExportLock::formatExtension(
            static_cast<FeatherKrita::ExportFormat>(format)));
    }

    bool unlock_export(const String& license_key) {
        return m_lock.unlockExport(gd2qt_string(license_key));
    }

    // source: 1=LicenseKey, 2=GooglePlay, 3=WindowsStore, 4=AppStore
    bool unlock_from_store(int source, const String& order_id,
                           const String& email = String()) {
        return m_lock.unlockFromStore(
            static_cast<FeatherKrita::LicenseSource>(source),
            gd2qt_string(order_id),
            gd2qt_string(email));
    }

    void lock_export() { m_lock.lockExport(); }

    bool is_pro_version() const { return m_lock.isProVersion(); }

    bool load_stored_license(const String& data_dir = String()) {
        return m_lock.loadStoredLicense(gd2qt_string(data_dir));
    }

    bool save_license(const String& data_dir = String()) const {
        return m_lock.saveLicense(gd2qt_string(data_dir));
    }

    bool clear_stored_license(const String& data_dir = String()) {
        return m_lock.clearStoredLicense(gd2qt_string(data_dir));
    }

    void set_data_directory(const String& dir) {
        m_lock.setDataDirectory(gd2qt_string(dir));
    }

    String get_data_directory() const {
        return qt2gd_string(m_lock.dataDirectory());
    }

    // Trial
    bool start_trial(int days = 7) { return m_lock.startTrial(days); }
    bool is_trial_active() const { return m_lock.isTrialActive(); }
    int  trial_days_remaining() const { return m_lock.trialDaysRemaining(); }

    // Validation
    static bool is_valid_key_format(const String& key) {
        return FeatherKrita::ExportLock::isValidKeyFormat(gd2qt_string(key));
    }

    static String generate_license_key() {
        return qt2gd_string(FeatherKrita::ExportLock::generateLicenseKey());
    }

    Dictionary get_license_info() const {
        Dictionary d;
        auto info = m_lock.licenseInfo();
        d["valid"]         = info.valid;
        d["source"]        = int(info.source);
        d["key"]           = qt2gd_string(info.key);
        d["order_id"]      = qt2gd_string(info.orderId);
        d["email"]         = qt2gd_string(info.customerEmail);
        d["product"]       = qt2gd_string(info.productName);
        d["purchase_date"] = info.purchaseDate.toString(Qt::ISODate).toUtf8().constData();
        d["expiry_date"]   = info.expiryDate.toString(Qt::ISODate).toUtf8().constData();
        return d;
    }

protected:
    static void _bind_methods() {
        ClassDB::bind_method(D_METHOD("can_export", "format"),
                             &FeatherExportLock::can_export);

        ClassDB::bind_static_method("FeatherExportLock",
                                    D_METHOD("format_requires_pro", "format"),
                                    &FeatherExportLock::format_requires_pro);
        ClassDB::bind_static_method("FeatherExportLock",
                                    D_METHOD("format_name", "format"),
                                    &FeatherExportLock::format_name);
        ClassDB::bind_static_method("FeatherExportLock",
                                    D_METHOD("format_extension", "format"),
                                    &FeatherExportLock::format_extension);

        ClassDB::bind_method(D_METHOD("unlock_export", "license_key"),
                             &FeatherExportLock::unlock_export);
        ClassDB::bind_method(D_METHOD("unlock_from_store", "source",
                                      "order_id", "email"),
                             &FeatherExportLock::unlock_from_store,
                             DEFVAL(String()));
        ClassDB::bind_method(D_METHOD("lock_export"),
                             &FeatherExportLock::lock_export);

        ClassDB::bind_method(D_METHOD("is_pro_version"),
                             &FeatherExportLock::is_pro_version);

        ClassDB::bind_method(D_METHOD("load_stored_license", "data_dir"),
                             &FeatherExportLock::load_stored_license,
                             DEFVAL(String()));
        ClassDB::bind_method(D_METHOD("save_license", "data_dir"),
                             &FeatherExportLock::save_license,
                             DEFVAL(String()));
        ClassDB::bind_method(D_METHOD("clear_stored_license", "data_dir"),
                             &FeatherExportLock::clear_stored_license,
                             DEFVAL(String()));

        ClassDB::bind_method(D_METHOD("set_data_directory", "dir"),
                             &FeatherExportLock::set_data_directory);
        ClassDB::bind_method(D_METHOD("get_data_directory"),
                             &FeatherExportLock::get_data_directory);

        ClassDB::bind_method(D_METHOD("start_trial", "days"),
                             &FeatherExportLock::start_trial, DEFVAL(7));
        ClassDB::bind_method(D_METHOD("is_trial_active"),
                             &FeatherExportLock::is_trial_active);
        ClassDB::bind_method(D_METHOD("trial_days_remaining"),
                             &FeatherExportLock::trial_days_remaining);

        ClassDB::bind_static_method("FeatherExportLock",
                                    D_METHOD("is_valid_key_format", "key"),
                                    &FeatherExportLock::is_valid_key_format);
        ClassDB::bind_static_method("FeatherExportLock",
                                    D_METHOD("generate_license_key"),
                                    &FeatherExportLock::generate_license_key);

        ClassDB::bind_method(D_METHOD("get_license_info"),
                             &FeatherExportLock::get_license_info);

        ADD_SIGNAL(MethodInfo("license_changed",
                              PropertyInfo(Variant::BOOL, "is_pro")));
        ADD_SIGNAL(MethodInfo("trial_status_changed",
                              PropertyInfo(Variant::BOOL, "active"),
                              PropertyInfo(Variant::INT, "days_remaining")));

        // Format constants (mirror of ExportFormat enum).
        // GDScript can access these as FeatherExportLock.FORMAT_PNG, etc.
        BIND_CONSTANT(FMT_PNG);
        BIND_CONSTANT(FMT_JPEG);
        BIND_CONSTANT(FMT_GIF);
        BIND_CONSTANT(FMT_MP4);
        BIND_CONSTANT(FMT_OBJ);
        BIND_CONSTANT(FMT_GLTF);
        BIND_CONSTANT(FMT_FEATHER);
        BIND_CONSTANT(FMT_STL);

        // License source constants
        BIND_CONSTANT(LICENSE_NONE);
        BIND_CONSTANT(LICENSE_KEY);
        BIND_CONSTANT(LICENSE_GOOGLE_PLAY);
        BIND_CONSTANT(LICENSE_WINDOWS_STORE);
        BIND_CONSTANT(LICENSE_APP_STORE);
    }

private:
    // Expose format/source constants to GDScript via BIND_CONSTANT above.
    static constexpr int FMT_PNG     = int(FeatherKrita::ExportFormat::PNG);
    static constexpr int FMT_JPEG    = int(FeatherKrita::ExportFormat::JPEG);
    static constexpr int FMT_GIF     = int(FeatherKrita::ExportFormat::GIF);
    static constexpr int FMT_MP4     = int(FeatherKrita::ExportFormat::MP4);
    static constexpr int FMT_OBJ     = int(FeatherKrita::ExportFormat::OBJ);
    static constexpr int FMT_GLTF    = int(FeatherKrita::ExportFormat::GLTF);
    static constexpr int FMT_FEATHER = int(FeatherKrita::ExportFormat::Feather);
    static constexpr int FMT_STL     = int(FeatherKrita::ExportFormat::STL);

    static constexpr int LICENSE_NONE          = int(FeatherKrita::LicenseSource::None);
    static constexpr int LICENSE_KEY           = int(FeatherKrita::LicenseSource::LicenseKey);
    static constexpr int LICENSE_GOOGLE_PLAY   = int(FeatherKrita::LicenseSource::GooglePlay);
    static constexpr int LICENSE_WINDOWS_STORE = int(FeatherKrita::LicenseSource::WindowsStore);
    static constexpr int LICENSE_APP_STORE     = int(FeatherKrita::LicenseSource::AppStore);
};

// ===========================================================================
// FeatherKritaBrush — Godot wrapper untuk FeatherKrita::KritaBrushWrapper
// ===========================================================================

class FeatherKritaBrush : public RefCounted {
    GDCLASS(FeatherKritaBrush, RefCounted)

private:
    FeatherKrita::KritaBrushWrapper m_brush;

public:
    FeatherKritaBrush() {}
    ~FeatherKritaBrush() {}

    bool load_preset(const String& path) {
        return m_brush.loadPreset(gd2qt_string(path));
    }

    PackedStringArray list_presets(const String& folder) {
        PackedStringArray arr;
        for (const auto& s : m_brush.listPresets(gd2qt_string(folder))) {
            arr.append(qt2gd_string(s));
        }
        return arr;
    }

    // generate_dab returns Dictionary with: image (PackedByteArray),
    // width, height, position (Vector2), scale, rotation
    Dictionary generate_dab(const Vector2& position, double pressure,
                            double x_tilt = 0.0, double y_tilt = 0.0,
                            double rotation = 0.0, double time = 0.0,
                            double speed = 0.0) {
        Dictionary result;
        FeatherKrita::BrushInput input;
        input.position = gd2qt_vec2(position);
        input.pressure = pressure;
        input.xTilt    = x_tilt;
        input.yTilt    = y_tilt;
        input.rotation = rotation;
        input.time     = time;
        input.speed    = speed;
        input.isHovering = false;

        FeatherKrita::BrushDab dab = m_brush.generateDab(input);

        // Convert QImage → PackedByteArray (RGBA)
        QImage img = dab.image.convertToFormat(QImage::Format_RGBA8888);
        PackedByteArray bytes;
        bytes.resize(img.sizeInBytes());
        if (img.constBits() && bytes.size() > 0) {
            memcpy(bytes.ptrw(), img.constBits(), img.sizeInBytes());
        }
        result["image"]    = bytes;
        result["width"]    = img.width();
        result["height"]   = img.height();
        result["position"] = qt2gd_vec2(dab.position);
        result["scale"]    = dab.scaleFactor;
        result["rotation"] = dab.rotation;
        return result;
    }

    void set_brush_size(int size)         { m_brush.setBrushSize(size); }
    void set_brush_color(const Color& c)  { m_brush.setBrushColor(gd2qt_color(c)); }
    void set_brush_opacity(double opacity){ m_brush.setBrushOpacity(opacity); }
    void set_brush_spacing(double spacing){ m_brush.setBrushSpacing(spacing); }
    bool is_ready() const                 { return m_brush.isReady(); }
    String current_preset_name() const    { return qt2gd_string(m_brush.currentPresetName()); }
    void reset_stroke()                   { m_brush.resetStroke(); }

protected:
    static void _bind_methods() {
        ClassDB::bind_method(D_METHOD("load_preset", "path"),
                             &FeatherKritaBrush::load_preset);
        ClassDB::bind_method(D_METHOD("list_presets", "folder"),
                             &FeatherKritaBrush::list_presets);
        ClassDB::bind_method(D_METHOD("generate_dab", "position", "pressure",
                                      "x_tilt", "y_tilt", "rotation",
                                      "time", "speed"),
                             &FeatherKritaBrush::generate_dab,
                             DEFVAL(0.0), DEFVAL(0.0), DEFVAL(0.0),
                             DEFVAL(0.0), DEFVAL(0.0));

        ClassDB::bind_method(D_METHOD("set_brush_size", "size"),
                             &FeatherKritaBrush::set_brush_size);
        ClassDB::bind_method(D_METHOD("set_brush_color", "color"),
                             &FeatherKritaBrush::set_brush_color);
        ClassDB::bind_method(D_METHOD("set_brush_opacity", "opacity"),
                             &FeatherKritaBrush::set_brush_opacity);
        ClassDB::bind_method(D_METHOD("set_brush_spacing", "spacing"),
                             &FeatherKritaBrush::set_brush_spacing);
        ClassDB::bind_method(D_METHOD("is_ready"),
                             &FeatherKritaBrush::is_ready);
        ClassDB::bind_method(D_METHOD("current_preset_name"),
                             &FeatherKritaBrush::current_preset_name);
        ClassDB::bind_method(D_METHOD("reset_stroke"),
                             &FeatherKritaBrush::reset_stroke);
    }
};

// ===========================================================================
// FeatherTexturePainter — Godot wrapper untuk FeatherKrita::TexturePainter
// ===========================================================================

class FeatherTexturePainter : public RefCounted {
    GDCLASS(FeatherTexturePainter, RefCounted)

private:
    FeatherKrita::TexturePainter m_painter;

public:
    FeatherTexturePainter() : m_painter(2048, 2048) {}
    ~FeatherTexturePainter() {}

    void initialize(const Color& bg = Color(0, 0, 0, 0)) {
        QColor c = gd2qt_color(bg);
        if (c.alphaF() < 0.001f) c = Qt::transparent;
        m_painter.initialize(c);
    }

    // dab_dict format (from FeatherKritaBrush.generate_dab):
    //   {image: PackedByteArray, width, height, position, scale, rotation}
    void apply_dab(const Dictionary& dab_dict, const Vector2& uv) {
        if (!dab_dict.has("image") || !dab_dict.has("width") || !dab_dict.has("height")) {
            return;
        }
        PackedByteArray bytes = dab_dict["image"];
        int w = dab_dict["width"];
        int h = dab_dict["height"];
        if (bytes.size() != w * h * 4) return;

        QImage img(reinterpret_cast<const uchar*>(bytes.ptr()),
                   w, h, w * 4, QImage::Format_RGBA8888);
        img = img.copy(); // detach from external buffer

        FeatherKrita::BrushDab dab;
        dab.image      = img;
        dab.position   = gd2qt_vec2(dab_dict.has("position") ?
                                    (Vector2)dab_dict["position"] : Vector2());
        dab.scaleFactor= dab_dict.has("scale") ? double(dab_dict["scale"]) : 1.0;
        dab.rotation   = dab_dict.has("rotation") ? double(dab_dict["rotation"]) : 0.0;

        m_painter.applyDab(dab, gd2qt_vec2(uv));
    }

    PackedByteArray get_texture_data() const {
        const uchar* data = m_painter.getTextureData();
        if (!data) return PackedByteArray();
        PackedByteArray bytes;
        bytes.resize(m_painter.width() * m_painter.height() * 4);
        memcpy(bytes.ptrw(), data, bytes.size());
        return bytes;
    }

    int get_width()  const { return m_painter.width(); }
    int get_height() const { return m_painter.height(); }

    void clear() { m_painter.clear(); }

    Vector2 uv_to_pixel(const Vector2& uv) const {
        return qt2gd_vec2(m_painter.uvToPixel(gd2qt_vec2(uv)));
    }

    Vector2 pixel_to_uv(const Vector2& pixel) const {
        return qt2gd_vec2(m_painter.pixelToUV(gd2qt_vec2(pixel)));
    }

    void set_eraser_mode(bool eraser) { m_painter.setEraserMode(eraser); }
    bool is_eraser_mode() const { return m_painter.isEraserMode(); }

protected:
    static void _bind_methods() {
        ClassDB::bind_method(D_METHOD("initialize", "bg"),
                             &FeatherTexturePainter::initialize,
                             DEFVAL(Color(0, 0, 0, 0)));
        ClassDB::bind_method(D_METHOD("apply_dab", "dab_dict", "uv"),
                             &FeatherTexturePainter::apply_dab);
        ClassDB::bind_method(D_METHOD("get_texture_data"),
                             &FeatherTexturePainter::get_texture_data);
        ClassDB::bind_method(D_METHOD("get_width"),
                             &FeatherTexturePainter::get_width);
        ClassDB::bind_method(D_METHOD("get_height"),
                             &FeatherTexturePainter::get_height);
        ClassDB::bind_method(D_METHOD("clear"),
                             &FeatherTexturePainter::clear);
        ClassDB::bind_method(D_METHOD("uv_to_pixel", "uv"),
                             &FeatherTexturePainter::uv_to_pixel);
        ClassDB::bind_method(D_METHOD("pixel_to_uv", "pixel"),
                             &FeatherTexturePainter::pixel_to_uv);
        ClassDB::bind_method(D_METHOD("set_eraser_mode", "eraser"),
                             &FeatherTexturePainter::set_eraser_mode);
        ClassDB::bind_method(D_METHOD("is_eraser_mode"),
                             &FeatherTexturePainter::is_eraser_mode);
    }
};

} // namespace feather

// ===========================================================================
// GDExtension entry points
// ===========================================================================

using namespace godot;

extern "C" {

// Called by Godot on extension initialization
GDExtensionBool GDE_EXPORT gdextension_initialize(
    GDExtensionInterfaceGetProcAddress p_get_proc_address,
    const GDExtensionClassLibraryPtr p_library,
    GDExtensionInitialization* r_initialization)
{
    GDExtensionInitialization initialization = {};
    initialization.minimum_initialization_level = GDEXTENSION_INITIALIZATION_SCENE;

    initialization.initialize = [](GDExtensionInitializationLevel p_level) -> void {
        if (p_level != GDEXTENSION_INITIALIZATION_SCENE) return;

        // Register all classes
        ClassDB::register_class<feather::FeatherGuideSurface>();
        ClassDB::register_class<feather::FeatherStrokeManager>();
        ClassDB::register_class<feather::FeatherExportLock>();
        ClassDB::register_class<feather::FeatherKritaBrush>();
        ClassDB::register_class<feather::FeatherTexturePainter>();

        UtilityFunctions::print("[FeatherKrita] GDExtension initialized — ",
                                 "FeatherGuideSurface, FeatherStrokeManager, ",
                                 "FeatherExportLock, FeatherKritaBrush, ",
                                 "FeatherTexturePainter registered");
    };

    initialization.deinitialize = [](GDExtensionInitializationLevel p_level) -> void {
        if (p_level != GDEXTENSION_INITIALIZATION_SCENE) return;
        UtilityFunctions::print("[FeatherKrita] GDExtension deinitialized");
    };

    *r_initialization = initialization;

    // Initialize godot-cpp bindings
    godot::GDExtensionBinding::InitObject init_obj(p_get_proc_address,
                                                   p_library,
                                                   r_initialization);
    init_obj.register_initializer(initialization.initialize);
    init_obj.register_terminator(initialization.deinitialize);
    init_obj.set_minimum_library_initialization_level(
        GDEXTENSION_INITIALIZATION_SCENE);

    return init_obj.init();
}

} // extern "C"
