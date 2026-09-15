/*
 * SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * stroke_manager.h — Manages 3D strokes untuk Feather-Krita app
 *
 * Setiap stroke adalah individual 3D entity (seperti Feather 3D):
 *   - Position (XYZ)
 *   - Rotation (Euler XYZ)
 *   - Scale (XYZ)
 *   - Thickness
 *   - Color
 *   - List of points (stroke path di 3D space)
 *
 * Strokes bisa di-edit:
 *   - addStroke()      → tambah stroke baru
 *   - selectStroke()   → pilih stroke untuk edit
 *   - moveStroke()     → geser stroke di 3D
 *   - rotateStroke()   → putar stroke
 *   - scaleStroke()    → scale stroke
 *   - liquify()        → push/pull stroke di 3D space
 *   - mirrorStroke()   → mirror symmetrical across X/Y/Z
 *
 * Strokes di-render di Godot sebagai 3D tube/ribbon mesh.
 */

#ifndef STROKE_MANAGER_H
#define STROKE_MANAGER_H

#include <QVector>
#include <QVector3D>
#include <QColor>
#include <QUuid>
#include <QImage>
#include <QMatrix4x4>
#include <QHash>

namespace FeatherKrita {

/**
 * StrokePoint — satu titik di dalam stroke path
 */
struct StrokePoint {
    QVector3D position;    // Local position (relatif ke stroke origin)
    float     thickness;   // Thickness di titik ini (0..1, scaled by stroke thickness)
    float     pressure;    // Pressure saat titik dibuat (0..1)
    float     tilt;        // Tilt angle (radians)
};

/**
 * StrokeType — jenis stroke
 */
enum class StrokeType {
    Freehand,      // Gambar bebas
    Line,          // Garis lurus
    Circle,        // Lingkaran
    Ellipse,       // Ellipse
    Curve,         // Smooth curve (Catmull-Rom)
    Shape          // Geometric shape (preset)
};

/**
 * MirrorAxis — axis untuk symmetrical drawing
 */
enum class MirrorAxis {
    None  = 0,
    X     = 1,
    Y     = 2,
    Z     = 4,
    All   = 7
};

Q_DECLARE_FLAGS(MirrorAxes, MirrorAxis)
Q_DECLARE_OPERATORS_FOR_FLAGS(MirrorAxes)

/**
 * Stroke — satu 3D entity stroke
 *
 * Stroke adalah kumpulan StrokePoint yang di-render sebagai 3D tube.
 * Setiap stroke punya transform sendiri (position/rotation/scale).
 */
struct Stroke {
    QString           id;             // Unique ID
    StrokeType        type;           // Jenis stroke
    QVector<StrokePoint> points;      // Titik-titik stroke (local space)

    // Transform
    QVector3D         position;       // Position di world space
    QVector3D         rotation;       // Euler angles (degrees)
    QVector3D         scale;          // Scale (1,1,1 = default)

    // Visual properties
    QColor            color;          // Stroke color
    float             thickness;      // Base thickness (world units)
    float             opacity;        // 0..1
    QString           brushPreset;    // Krita brush preset name

    // Metadata
    bool              visible;
    bool              locked;
    QString           layerId;        // Group/layer
    bool              selected;

    Stroke()
        : type(StrokeType::Freehand)
        , position(0, 0, 0)
        , rotation(0, 0, 0)
        , scale(1, 1, 1)
        , color(Qt::black)
        , thickness(0.05f)
        , opacity(1.0f)
        , brushPreset()
        , visible(true)
        , locked(false)
        , selected(false)
    {
        id = QUuid::createUuid().toString(QUuid::WithoutBraces);
    }
};

/**
 * LiquifyMode — mode untuk liquify tool
 */
enum class LiquifyMode {
    Push,        // Dorong stroke menjauh dari cursor
    Pull,        // Tarik stroke mendekati cursor
    Smooth,      // Smooth stroke di area cursor
    Inflate,     // Besarkan stroke di area cursor
    Deflate,     // Kecilkan stroke di area cursor
    Twirl        // Putar stroke di area cursor
};

/**
 * LiquifyParams — parameter untuk liquify operation
 */
struct LiquifyParams {
    LiquifyMode mode = LiquifyMode::Push;
    QVector3D   center;          // Pusat liquify di world space
    float       radius = 1.0f;   // Radius pengaruh
    float       strength = 1.0f; // Kekuatan (0..1)
    QVector3D   direction;       // Arah untuk push/pull (normalized)
};

/**
 * StrokeManager — Manages 3D strokes
 *
 * Class ini menyimpan semua stroke di scene, dan menyediakan
 * function untuk manipulasi stroke (move/rotate/scale/liquify/mirror).
 */
class StrokeManager {
public:
    StrokeManager();
    ~StrokeManager();

    // === Stroke Management ===

    /**
     * Tambah stroke baru
     * @return ID stroke yang baru dibuat
     */
    QString addStroke(const Stroke& stroke);

    /**
     * Tambah stroke baru dengan parameter minimum
     */
    QString addStroke(StrokeType type,
                      const QVector<StrokePoint>& points,
                      const QColor& color = Qt::black,
                      float thickness = 0.05f,
                      const QString& brushPreset = QString());

    /**
     * Hapus stroke berdasarkan ID
     */
    bool removeStroke(const QString& id);

    /**
     * Hapus semua stroke
     */
    void clear();

    /**
     * Get stroke by ID
     */
    Stroke* getStroke(const QString& id);
    const Stroke* getStroke(const QString& id) const;

    /**
     * Get semua strokes
     */
    const QVector<Stroke>& strokes() const { return m_strokes; }
    QVector<Stroke>& strokes() { return m_strokes; }

    /**
     * Jumlah stroke
     */
    int count() const { return m_strokes.size(); }

    // === Selection ===

    /**
     * Pilih stroke berdasarkan ID
     */
    bool selectStroke(const QString& id, bool exclusive = true);

    /**
     * Select stroke terdekat dengan titik 3D (untuk click selection)
     * @param point Titik di world space
     * @param maxDistance Maximum distance untuk hit-test
     * @return ID stroke yang terpilih, atau QString() jika tidak ada
     */
    QString pickStroke(const QVector3D& point, float maxDistance = 0.5f);

    /**
     * Deselect semua stroke
     */
    void deselectAll();

    /**
     * Get ID stroke yang sedang dipilih (single selection)
     */
    QString selectedStrokeId() const;

    /**
     * Get semua stroke yang sedang dipilih
     */
    QVector<QString> selectedStrokeIds() const;

    // === Transform ===

    /**
     * Geser stroke berdasarkan ID
     */
    bool moveStroke(const QString& id, const QVector3D& delta);

    /**
     * Putar stroke berdasarkan ID
     * @param eulerDelta Perubahan rotasi dalam degrees
     */
    bool rotateStroke(const QString& id, const QVector3D& eulerDelta);

    /**
     * Scale stroke berdasarkan ID
     * @param scaleFactor Uniform scale factor (1.0 = no change)
     */
    bool scaleStroke(const QString& id, float scaleFactor);

    /**
     * Scale stroke non-uniform
     */
    bool scaleStroke(const QString& id, const QVector3D& scaleDelta);

    /**
     * Set transform stroke absolute
     */
    bool setStrokeTransform(const QString& id,
                            const QVector3D& position,
                            const QVector3D& rotation,
                            const QVector3D& scale);

    /**
     * Get transform matrix stroke (world space)
     */
    QMatrix4x4 strokeMatrix(const QString& id) const;

    // === Liquify ===

    /**
     * Apply liquify ke stroke
     *
     * @param id      Stroke ID
     * @param params  Liquify parameters
     * @return true jika stroke termodifikasi
     */
    bool liquify(const QString& id, const LiquifyParams& params);

    /**
     * Apply liquify ke semua selected strokes
     */
    int liquifySelected(const LiquifyParams& params);

    /**
     * Smooth stroke (kurangi noise di point path)
     * @param id        Stroke ID
     * @param iterations Jumlah smoothing pass
     */
    bool smoothStroke(const QString& id, int iterations = 1);

    // === Mirror ===

    /**
     * Mirror stroke across axis
     *
     * @param id       Stroke ID
     * @param axis     Axis untuk mirror (X/Y/Z atau kombinasi)
     * @param planeOrigin Titik origin mirror plane (default: world origin)
     * @return ID stroke hasil mirror, atau QString() jika gagal
     */
    QString mirrorStroke(const QString& id,
                         MirrorAxes axis,
                         const QVector3D& planeOrigin = QVector3D(0, 0, 0));

    /**
     * Aktifkan live mirror mode
     * Saat mode ini aktif, setiap addStroke() akan otomatis
     * membuat mirror stroke.
     *
     * @param axis Axis untuk live mirror
     */
    void enableLiveMirror(MirrorAxes axis);
    void disableLiveMirror();
    MirrorAxes liveMirrorAxes() const { return m_liveMirror; }

    /**
     * Dapatkan semua mirror strokes untuk stroke tertentu
     * (untuk live update saat stroke asli di-edit)
     */
    QVector<QString> mirrorStrokesOf(const QString& id) const;

    /**
     * Update posisi mirror strokes untuk stroke tertentu
     * (dipanggil otomatis saat stroke asli di-edit)
     */
    void updateMirrorStrokes(const QString& id);

    // === Utility ===

    /**
     * Convert world position → local position di stroke
     */
    QVector3D worldToStrokeLocal(const QString& id, const QVector3D& world) const;

    /**
     * Convert local position → world position di stroke
     */
    QVector3D strokeLocalToWorld(const QString& id, const QVector3D& local) const;

    /**
     * Get bounding box dari semua stroke
     */
    void getSceneBounds(QVector3D& min, QVector3D& max) const;

    /**
     * Set undo/redo callback (sederhana: snapshot of stroke list)
     */
    void pushUndo();
    bool undo();
    bool redo();

    /**
     * Get stroke data sebagai QVariant (untuk serialize/save)
     */
    // (Serialization dilakukan di sisi Godot via Dictionary)

private:
    QVector<Stroke> m_strokes;
    QString         m_selectedId;

    // Live mirror state
    MirrorAxes      m_liveMirror = MirrorAxis::None;
    QHash<QString, QVector<QString>> m_mirrorMap; // originalId → mirrorIds

    // Undo/redo stacks
    QVector<QVector<Stroke>> m_undoStack;
    QVector<QVector<Stroke>> m_redoStack;
    int                      m_maxUndo = 50;

    // Internal helpers
    void applyLiquifyToStroke(Stroke& s, const LiquifyParams& params);
    static QVector3D mirrorPoint(const QVector3D& p, MirrorAxes axis,
                                 const QVector3D& origin);
    static float falloff(float distSq, float radius);
    void pruneUndoStack();

    // Internal mirror helper (does NOT push undo)
    QString mirrorStrokeInternal(const QString& id,
                                 MirrorAxes axis,
                                 const QVector3D& planeOrigin = QVector3D(0, 0, 0));
};

} // namespace FeatherKrita

#endif // STROKE_MANAGER_H
