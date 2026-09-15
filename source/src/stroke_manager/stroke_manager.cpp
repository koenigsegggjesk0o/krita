/*
 * SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * stroke_manager.cpp — Implementasi stroke manager
 *
 * Manages 3D strokes: add/select/move/rotate/scale/liquify/mirror
 */

#include "stroke_manager.h"

#include <QtMath>
#include <QDebug>
#include <QRegularExpression>
#include <cmath>
#include <algorithm>

namespace FeatherKrita {

// ---------------------------------------------------------------------------
// Constants
// ---------------------------------------------------------------------------

static constexpr float kEpsilon = 1e-6f;

// ---------------------------------------------------------------------------
// Construction / Destruction
// ---------------------------------------------------------------------------

StrokeManager::StrokeManager()
{
}

StrokeManager::~StrokeManager()
{
}

// ---------------------------------------------------------------------------
// Stroke Management
// ---------------------------------------------------------------------------

QString StrokeManager::addStroke(const Stroke& stroke)
{
    pushUndo();

    Stroke s = stroke;
    if (s.id.isEmpty()) {
        s.id = QUuid::createUuid().toString(QUuid::WithoutBraces);
    }
    m_strokes.append(s);

    // Live mirror: jika aktif, buat mirror strokes
    if (m_liveMirror != MirrorAxis::None) {
        QVector<QString> mirrors;
        // Generate satu mirror per axis yang aktif
        if (m_liveMirror & MirrorAxis::X) {
            QString mid = mirrorStrokeInternal(s.id, MirrorAxis::X);
            if (!mid.isEmpty()) mirrors.append(mid);
        }
        if (m_liveMirror & MirrorAxis::Y) {
            QString mid = mirrorStrokeInternal(s.id, MirrorAxis::Y);
            if (!mid.isEmpty()) mirrors.append(mid);
        }
        if (m_liveMirror & MirrorAxis::Z) {
            QString mid = mirrorStrokeInternal(s.id, MirrorAxis::Z);
            if (!mid.isEmpty()) mirrors.append(mid);
        }
        m_mirrorMap.insert(s.id, mirrors);
    }

    return s.id;
}

QString StrokeManager::addStroke(StrokeType type,
                                 const QVector<StrokePoint>& points,
                                 const QColor& color,
                                 float thickness,
                                 const QString& brushPreset)
{
    Stroke s;
    s.type         = type;
    s.points       = points;
    s.color        = color;
    s.thickness    = thickness;
    s.brushPreset  = brushPreset;
    return addStroke(s);
}

bool StrokeManager::removeStroke(const QString& id)
{
    // Hapus juga mirror strokes-nya
    auto it = m_mirrorMap.find(id);
    if (it != m_mirrorMap.end()) {
        for (const QString& mid : it.value()) {
            for (int i = 0; i < m_strokes.size(); ++i) {
                if (m_strokes[i].id == mid) {
                    m_strokes.removeAt(i);
                    break;
                }
            }
        }
        m_mirrorMap.erase(it);
    }

    // Hapus entry mirror untuk strokes lain yang mereferensi id ini
    for (auto& mirrors : m_mirrorMap) {
        mirrors.removeAll(id);
    }

    for (int i = 0; i < m_strokes.size(); ++i) {
        if (m_strokes[i].id == id) {
            pushUndo();
            m_strokes.removeAt(i);
            if (m_selectedId == id) m_selectedId.clear();
            return true;
        }
    }
    return false;
}

void StrokeManager::clear()
{
    pushUndo();
    m_strokes.clear();
    m_mirrorMap.clear();
    m_selectedId.clear();
}

Stroke* StrokeManager::getStroke(const QString& id)
{
    for (auto& s : m_strokes) {
        if (s.id == id) return &s;
    }
    return nullptr;
}

const Stroke* StrokeManager::getStroke(const QString& id) const
{
    for (const auto& s : m_strokes) {
        if (s.id == id) return &s;
    }
    return nullptr;
}

// ---------------------------------------------------------------------------
// Selection
// ---------------------------------------------------------------------------

bool StrokeManager::selectStroke(const QString& id, bool exclusive)
{
    Stroke* s = getStroke(id);
    if (!s) return false;

    if (exclusive) {
        for (auto& st : m_strokes) st.selected = false;
    }
    s->selected = true;
    m_selectedId = id;
    return true;
}

QString StrokeManager::pickStroke(const QVector3D& point, float maxDistance)
{
    QString bestId;
    float bestDist = maxDistance * maxDistance;

    for (const auto& s : m_strokes) {
        if (!s.visible) continue;

        // Cek jarak point ke setiap titik di stroke (world space)
        QMatrix4x4 m = strokeMatrix(s.id);
        for (const auto& p : s.points) {
            QVector3D worldP = m.map(p.position);
            float d = (worldP - point).lengthSquared();
            // Tambah thickness ke tolerance
            float tol = s.thickness * 0.5f + p.thickness * s.thickness * 0.5f;
            float tolSq = (maxDistance + tol) * (maxDistance + tol);
            if (d < tolSq && d < bestDist) {
                bestDist = d;
                bestId = s.id;
            }
        }
    }

    if (!bestId.isNull()) {
        selectStroke(bestId);
    }
    return bestId;
}

void StrokeManager::deselectAll()
{
    for (auto& s : m_strokes) s.selected = false;
    m_selectedId.clear();
}

QString StrokeManager::selectedStrokeId() const
{
    return m_selectedId;
}

QVector<QString> StrokeManager::selectedStrokeIds() const
{
    QVector<QString> ids;
    for (const auto& s : m_strokes) {
        if (s.selected) ids.append(s.id);
    }
    return ids;
}

// ---------------------------------------------------------------------------
// Transform
// ---------------------------------------------------------------------------

bool StrokeManager::moveStroke(const QString& id, const QVector3D& delta)
{
    Stroke* s = getStroke(id);
    if (!s || s->locked) return false;
    pushUndo();
    s->position += delta;
    updateMirrorStrokes(id);
    return true;
}

bool StrokeManager::rotateStroke(const QString& id, const QVector3D& eulerDelta)
{
    Stroke* s = getStroke(id);
    if (!s || s->locked) return false;
    pushUndo();
    s->rotation += eulerDelta;
    updateMirrorStrokes(id);
    return true;
}

bool StrokeManager::scaleStroke(const QString& id, float scaleFactor)
{
    Stroke* s = getStroke(id);
    if (!s || s->locked) return false;
    pushUndo();
    s->scale *= scaleFactor;
    updateMirrorStrokes(id);
    return true;
}

bool StrokeManager::scaleStroke(const QString& id, const QVector3D& scaleDelta)
{
    Stroke* s = getStroke(id);
    if (!s || s->locked) return false;
    pushUndo();
    s->scale *= scaleDelta;
    updateMirrorStrokes(id);
    return true;
}

bool StrokeManager::setStrokeTransform(const QString& id,
                                       const QVector3D& position,
                                       const QVector3D& rotation,
                                       const QVector3D& scale)
{
    Stroke* s = getStroke(id);
    if (!s || s->locked) return false;
    pushUndo();
    s->position = position;
    s->rotation = rotation;
    s->scale    = scale;
    updateMirrorStrokes(id);
    return true;
}

QMatrix4x4 StrokeManager::strokeMatrix(const QString& id) const
{
    const Stroke* s = getStroke(id);
    if (!s) return QMatrix4x4();

    QMatrix4x4 m;
    m.translate(s->position);
    m.rotate(s->rotation.x(), QVector3D(1, 0, 0));
    m.rotate(s->rotation.y(), QVector3D(0, 1, 0));
    m.rotate(s->rotation.z(), QVector3D(0, 0, 1));
    m.scale(s->scale);
    return m;
}

// ---------------------------------------------------------------------------
// Liquify
// ---------------------------------------------------------------------------

bool StrokeManager::liquify(const QString& id, const LiquifyParams& params)
{
    Stroke* s = getStroke(id);
    if (!s || s->locked) return false;
    pushUndo();
    applyLiquifyToStroke(*s, params);
    updateMirrorStrokes(id);
    return true;
}

int StrokeManager::liquifySelected(const LiquifyParams& params)
{
    int count = 0;
    for (auto& s : m_strokes) {
        if (s.selected && !s.locked) {
            applyLiquifyToStroke(s, params);
            updateMirrorStrokes(s.id);
            ++count;
        }
    }
    if (count > 0) pushUndo();
    return count;
}

bool StrokeManager::smoothStroke(const QString& id, int iterations)
{
    Stroke* s = getStroke(id);
    if (!s || s->locked || s->points.size() < 3) return false;

    pushUndo();

    for (int iter = 0; iter < iterations; ++iter) {
        QVector<StrokePoint> smoothed = s->points;
        for (int i = 1; i < s->points.size() - 1; ++i) {
            smoothed[i].position = (
                s->points[i - 1].position +
                s->points[i].position * 2.0f +
                s->points[i + 1].position) * 0.25f;
            smoothed[i].thickness = (
                s->points[i - 1].thickness +
                s->points[i].thickness * 2.0f +
                s->points[i + 1].thickness) * 0.25f;
        }
        s->points = smoothed;
    }
    updateMirrorStrokes(id);
    return true;
}

// ---------------------------------------------------------------------------
// Mirror
// ---------------------------------------------------------------------------

// Helper internal untuk membuat mirror stroke (tidak push undo)
QString StrokeManager::mirrorStrokeInternal(const QString& id,
                                            MirrorAxes axis,
                                            const QVector3D& planeOrigin)
{
    const Stroke* s = getStroke(id);
    if (!s) return QString();

    Stroke mirror = *s;
    mirror.id = QUuid::createUuid().toString(QUuid::WithoutBraces);

    // Mirror position
    mirror.position = mirrorPoint(s->position, axis, planeOrigin);

    // Mirror rotation: untuk mirror X, Y rotasi di-flip (180° rotasi Y + flip)
    if (axis & MirrorAxis::X) {
        mirror.rotation.setY(-s->rotation.y());
        mirror.rotation.setZ(-s->rotation.z());
        mirror.scale.setX(-s->scale.x());
    }
    if (axis & MirrorAxis::Y) {
        mirror.rotation.setX(-s->rotation.x());
        mirror.rotation.setZ(-s->rotation.z());
        mirror.scale.setY(-s->scale.y());
    }
    if (axis & MirrorAxis::Z) {
        mirror.rotation.setX(-s->rotation.x());
        mirror.rotation.setY(-s->rotation.y());
        mirror.scale.setZ(-s->scale.z());
    }

    m_strokes.append(mirror);
    return mirror.id;
}

QString StrokeManager::mirrorStroke(const QString& id,
                                    MirrorAxes axis,
                                    const QVector3D& planeOrigin)
{
    const Stroke* s = getStroke(id);
    if (!s) return QString();

    pushUndo();
    QString mid = mirrorStrokeInternal(id, axis, planeOrigin);

    // Register di mirror map
    auto it = m_mirrorMap.find(id);
    if (it == m_mirrorMap.end()) {
        m_mirrorMap.insert(id, { mid });
    } else {
        it.value().append(mid);
    }
    return mid;
}

void StrokeManager::enableLiveMirror(MirrorAxes axis)
{
    m_liveMirror = axis;
}

void StrokeManager::disableLiveMirror()
{
    m_liveMirror = MirrorAxis::None;
}

QVector<QString> StrokeManager::mirrorStrokesOf(const QString& id) const
{
    return m_mirrorMap.value(id);
}

void StrokeManager::updateMirrorStrokes(const QString& id)
{
    QVector<QString> mirrors = m_mirrorMap.value(id);
    if (mirrors.isEmpty()) return;

    const Stroke* original = getStroke(id);
    if (!original) return;

    // Re-apply mirror transform ke setiap mirror stroke.
    // Karena satu stroke bisa punya beberapa mirror (X, Y, Z),
    // kita gunakan indeks untuk menentukan axis.
    for (int i = 0; i < mirrors.size() && i < 3; ++i) {
        Stroke* m = getStroke(mirrors[i]);
        if (!m) continue;

        MirrorAxis axis;
        if (i == 0 && (m_liveMirror & MirrorAxis::X))      axis = MirrorAxis::X;
        else if (i == 0 && (m_liveMirror & MirrorAxis::Y)) axis = MirrorAxis::Y;
        else if (i == 0 && (m_liveMirror & MirrorAxis::Z)) axis = MirrorAxis::Z;
        else if (i == 1 && (m_liveMirror & MirrorAxis::Y)) axis = MirrorAxis::Y;
        else if (i == 1 && (m_liveMirror & MirrorAxis::Z)) axis = MirrorAxis::Z;
        else if (i == 2 && (m_liveMirror & MirrorAxis::Z)) axis = MirrorAxis::Z;
        else continue;

        m->position = mirrorPoint(original->position, axis, QVector3D(0, 0, 0));
        m->rotation = original->rotation;
        m->scale    = original->scale;

        if (axis & MirrorAxis::X) {
            m->rotation.setY(-original->rotation.y());
            m->rotation.setZ(-original->rotation.z());
            m->scale.setX(-original->scale.x());
        }
        if (axis & MirrorAxis::Y) {
            m->rotation.setX(-original->rotation.x());
            m->rotation.setZ(-original->rotation.z());
            m->scale.setY(-original->scale.y());
        }
        if (axis & MirrorAxis::Z) {
            m->rotation.setX(-original->rotation.x());
            m->rotation.setY(-original->rotation.y());
            m->scale.setZ(-original->scale.z());
        }

        // Copy points (jika stroke original di-edit, mirror ikut)
        m->points    = original->points;
        m->color     = original->color;
        m->thickness = original->thickness;
        m->opacity   = original->opacity;
    }
}

// ---------------------------------------------------------------------------
// Utility
// ---------------------------------------------------------------------------

QVector3D StrokeManager::worldToStrokeLocal(const QString& id,
                                            const QVector3D& world) const
{
    return strokeMatrix(id).inverted().map(world);
}

QVector3D StrokeManager::strokeLocalToWorld(const QString& id,
                                            const QVector3D& local) const
{
    return strokeMatrix(id).map(local);
}

void StrokeManager::getSceneBounds(QVector3D& min, QVector3D& max) const
{
    if (m_strokes.isEmpty()) {
        min = QVector3D();
        max = QVector3D();
        return;
    }

    bool first = true;
    for (const auto& s : m_strokes) {
        QMatrix4x4 m = strokeMatrix(s.id);
        for (const auto& p : s.points) {
            QVector3D wp = m.map(p.position);
            if (first) {
                min = max = wp;
                first = false;
            } else {
                for (int ax = 0; ax < 3; ++ax) {
                    if (wp[ax] < min[ax]) min[ax] = wp[ax];
                    if (wp[ax] > max[ax]) max[ax] = wp[ax];
                }
            }
        }
    }

    if (first) {
        min = QVector3D();
        max = QVector3D();
    }
}

// ---------------------------------------------------------------------------
// Undo / Redo
// ---------------------------------------------------------------------------

void StrokeManager::pushUndo()
{
    m_undoStack.append(m_strokes);
    if (m_undoStack.size() > m_maxUndo) {
        m_undoStack.removeFirst();
    }
    m_redoStack.clear();
}

bool StrokeManager::undo()
{
    if (m_undoStack.isEmpty()) return false;
    m_redoStack.append(m_strokes);
    m_strokes = m_undoStack.takeLast();
    m_selectedId.clear();
    return true;
}

bool StrokeManager::redo()
{
    if (m_redoStack.isEmpty()) return false;
    m_undoStack.append(m_strokes);
    m_strokes = m_redoStack.takeLast();
    m_selectedId.clear();
    return true;
}

void StrokeManager::pruneUndoStack()
{
    while (m_undoStack.size() > m_maxUndo) {
        m_undoStack.removeFirst();
    }
}

// ---------------------------------------------------------------------------
// Internal helpers
// ---------------------------------------------------------------------------

void StrokeManager::applyLiquifyToStroke(Stroke& s, const LiquifyParams& params)
{
    // Liquify memodifikasi titik-titik stroke di local space.
    // Karena params.center di world space, kita konversi ke local.
    QMatrix4x4 m = strokeMatrix(s.id);
    QMatrix4x4 invM = m.inverted();
    QVector3D localCenter = invM.map(params.center);

    float radiusSq = params.radius * params.radius;

    // Direction di local space (hilangkan translate)
    QVector3D localDir = invM.mapVector(params.direction);

    for (auto& p : s.points) {
        QVector3D toP = p.position - localCenter;
        float distSq = toP.lengthSquared();
        if (distSq > radiusSq) continue;

        float weight = falloff(distSq, params.radius);

        switch (params.mode) {
        case LiquifyMode::Push:
            p.position += localDir * weight * params.strength;
            break;
        case LiquifyMode::Pull:
            p.position -= localDir * weight * params.strength;
            break;
        case LiquifyMode::Smooth: {
            // Smooth: rata-rata dengan neighbors (sederhana)
            // Tidak di sini karena butuh context; implementasi minimal:
            // dorong ke pusat area
            p.position += (localCenter - p.position) * weight * params.strength * 0.5f;
            break;
        }
        case LiquifyMode::Inflate:
            p.position += toP.normalized() * weight * params.strength * 0.5f;
            p.thickness = std::min(1.0f, p.thickness + weight * params.strength * 0.2f);
            break;
        case LiquifyMode::Deflate:
            p.position -= toP.normalized() * weight * params.strength * 0.5f;
            p.thickness = std::max(0.0f, p.thickness - weight * params.strength * 0.2f);
            break;
        case LiquifyMode::Twirl: {
            // Rotasi titik di sekitar axis (gunakan direction sebagai axis)
            QVector3D axis = localDir.normalized();
            if (axis.lengthSquared() < kEpsilon) axis = QVector3D(0, 1, 0);
            float angle = weight * params.strength * float(M_PI) * 0.5f;
            QMatrix4x4 rot;
            rot.rotate(qRadiansToDegrees(double(angle)), axis);
            QVector3D rel = p.position - localCenter;
            p.position = localCenter + rot.mapVector(rel);
            break;
        }
        }
    }
}

QVector3D StrokeManager::mirrorPoint(const QVector3D& p, MirrorAxes axis,
                                     const QVector3D& origin)
{
    QVector3D r = p - origin;
    if (axis & MirrorAxis::X) r.setX(-r.x());
    if (axis & MirrorAxis::Y) r.setY(-r.y());
    if (axis & MirrorAxis::Z) r.setZ(-r.z());
    return origin + r;
}

float StrokeManager::falloff(float distSq, float radius)
{
    if (distSq >= radius * radius) return 0.0f;
    // Smooth quadratic falloff
    float t = 1.0f - std::sqrt(distSq) / radius;
    return t * t;
}

} // namespace FeatherKrita
