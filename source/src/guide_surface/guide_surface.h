/*
 * SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * guide_surface.h — 3D guide surfaces untuk Feather-Krita app
 *
 * File ini mengimplementasikan 3D guide surface seperti Feather 3D:
 *   - Sphere  (bola)
 *   - Cylinder (silinder)
 *   - Cone    (kerucut)
 *   - Ring    (cincin/torus)
 *
 * Setiap surface punya:
 *   - Vertices + normals + UV coordinates (untuk texture paint)
 *   - Indices untuk triangle rendering
 *   - Function raycast untuk dapat UV dari titik hit 3D
 *
 * Guide surface adalah "canvas 3D" tempat user mengambar.
 * Brush stroke dari Krita di-map ke texture via UV coordinate.
 */

#ifndef GUIDE_SURFACE_H
#define GUIDE_SURFACE_H

#include <QString>
#include <QVector>
#include <QVector3D>
#include <QVector2D>
#include <QImage>
#include <QMatrix4x4>
#include <cmath>
#include <limits>

namespace FeatherKrita {

/**
 * Vertex — single vertex dari mesh
 *
 * position : posisi 3D di object space
 * normal   : normal vector untuk lighting
 * uv       : texture coordinate (0..1)
 */
struct Vertex {
    QVector3D position;
    QVector3D normal;
    QVector2D uv;
};

/**
 * Ray — sinar dari camera ke scene (untuk raycast)
 *
 * origin    : titik mulai sinar
 * direction : arah sinar (harus normalized)
 */
struct Ray {
    QVector3D origin;
    QVector3D direction;
};

/**
 * RaycastHit — hasil raycast terhadap surface
 *
 * hit         : true jika sinar mengenai surface
 * point       : titik hit di world space
 * normal      : normal di titik hit
 * uv          : UV coordinate di titik hit (untuk texture paint)
 * distance    : jarak dari ray origin ke hit point
 * triangleIdx : index segitiga yang kena (untuk debugging)
 */
struct RaycastHit {
    bool      hit = false;
    QVector3D point;
    QVector3D normal;
    QVector2D uv;
    float     distance = 0.0f;
    int       triangleIdx = -1;
};

/**
 * SurfaceType — jenis guide surface
 */
enum class SurfaceType {
    Sphere,
    Cylinder,
    Cone,
    Ring,
    Custom
};

/**
 * GuideSurface — 3D guide surface (canvas untuk drawing)
 *
 * Class ini generate mesh untuk sphere/cylinder/cone/ring
 * dan menyediakan raycast untuk dapat UV dari titik 3D.
 *
 * Workflow:
 *   1. createSphere() / createCylinder() / ... → generate mesh
 *   2. User click di 3D viewport → raycast dari camera
 *   3. raycast() → return RaycastHit dengan UV coordinate
 *   4. UV dikirim ke KritaBrushWrapper + TexturePainter
 *   5. Texture di-update → Godot render mesh dengan stroke
 */
class GuideSurface {
public:
    GuideSurface();
    ~GuideSurface();

    // === Mesh Generators ===

    /**
     * Buat sphere mesh dengan UV mapping.
     * Sphere dibuat dengan parameter spherical (theta, phi).
     *
     * @param radius     Radius sphere
     * @param segmentsH  Jumlah segment horizontal (longitude)
     * @param segmentsV  Jumlah segment vertical (latitude)
     * @return true jika berhasil
     */
    bool createSphere(float radius = 1.0f,
                      int   segmentsH = 48,
                      int   segmentsV = 32);

    /**
     * Buat cylinder mesh dengan UV mapping.
     * Cylinder punya top cap, bottom cap, dan side surface.
     *
     * @param radius     Radius cylinder
     * @param height     Tinggi cylinder
     * @param segments   Jumlah segment keliling
     * @return true jika berhasil
     */
    bool createCylinder(float radius = 1.0f,
                        float height = 2.0f,
                        int   segments = 48);

    /**
     * Buat cone mesh dengan UV mapping.
     *
     * @param baseRadius Radius alas cone
     * @param height     Tinggi cone
     * @param segments   Jumlah segment keliling
     * @return true jika berhasil
     */
    bool createCone(float baseRadius = 1.0f,
                    float height = 2.0f,
                    int   segments = 48);

    /**
     * Buat ring (torus) mesh dengan UV mapping.
     * Ring adalah canvas melingkar seperti Feather 3D.
     *
     * @param majorRadius Radius utama (jarak titik tengah ke tube)
     * @param minorRadius Radius tube
     * @param majorSeg     Segment melingkar utama
     * @param minorSeg     Segment tube
     * @return true jika berhasil
     */
    bool createRing(float majorRadius = 1.5f,
                    float minorRadius = 0.25f,
                    int   majorSeg = 64,
                    int   minorSeg = 24);

    /**
     * Buat plane custom (untuk custom curve guide)
     * @param width  Lebar plane
     * @param height Tinggi plane
     * @param segX   Segment X
     * @param segY   Segment Y
     */
    bool createPlane(float width = 2.0f,
                     float height = 2.0f,
                     int   segX = 16,
                     int   segY = 16);

    // === Raycast ===

    /**
     * Raycast terhadap surface.
     * Mengembalikan UV coordinate di titik hit (untuk texture paint).
     *
     * Memakai Möller–Trumbore ray-triangle intersection algorithm.
     *
     * @param ray Sinar dari camera
     * @return RaycastHit dengan UV coordinate
     */
    RaycastHit raycast(const Ray& ray) const;

    /**
     * Dapatkan UV dari titik 3D di permukaan surface.
     * Berguna kalau titik sudah diketahui (e.g. dari physics engine).
     *
     * @param point Titik 3D di permukaan
     * @return UV coordinate (QVector2D(-1) kalau tidak di surface)
     */
    QVector2D getUVFromPoint(const QVector3D& point) const;

    // === Accessors ===

    const QVector<Vertex>& vertices() const { return m_vertices; }
    const QVector<unsigned int>& indices() const { return m_indices; }

    SurfaceType type() const { return m_type; }
    QString     typeName() const;

    int vertexCount() const { return m_vertices.size(); }
    int indexCount()  const { return m_indices.size(); }
    int triangleCount() const { return m_indices.size() / 3; }

    bool isEmpty() const { return m_vertices.isEmpty(); }

    /**
     * Transform (model matrix) — posisi/rotasi/scale surface di world
     */
    void setTransform(const QVector3D& position,
                      const QVector3D& rotation = QVector3D(),
                      const QVector3D& scale = QVector3D(1, 1, 1));

    QVector3D position() const { return m_position; }
    QVector3D rotation() const { return m_rotation; }
    QVector3D scale()    const { return m_scale; }

    /**
     * Convert local vertex → world position
     */
    QVector3D localToWorld(const QVector3D& local) const;

    /**
     * Convert world position → local
     */
    QVector3D worldToLocal(const QVector3D& world) const;

    /**
     * Build the model (local→world) matrix from position/rotation/scale.
     */
    QMatrix4x4 localToWorldMatrix() const;

    /**
     * Get bounding box (local space)
     */
    void getBounds(QVector3D& min, QVector3D& max) const;

    /**
     * Clear mesh data
     */
    void clear();

private:
    QVector<Vertex>       m_vertices;
    QVector<unsigned int> m_indices;
    SurfaceType           m_type = SurfaceType::Custom;

    // Transform
    QVector3D m_position = QVector3D(0, 0, 0);
    QVector3D m_rotation = QVector3D(0, 0, 0); // Euler degrees
    QVector3D m_scale    = QVector3D(1, 1, 1);

    // Bounding box (local space)
    QVector3D m_boundsMin;
    QVector3D m_boundsMax;

    // === Internal helpers ===

    void addVertex(const Vertex& v);
    void addTriangle(unsigned int a, unsigned int b, unsigned int c);
    void computeBounds();

    /**
     * Ray-triangle intersection (Möller–Trumbore)
     * @return true if hit, fills t (distance), u, v (barycentric)
     */
    static bool intersectTriangle(const QVector3D& rayOrigin,
                                  const QVector3D& rayDir,
                                  const QVector3D& v0,
                                  const QVector3D& v1,
                                  const QVector3D& v2,
                                  float& t,
                                  float& u,
                                  float& v);

    /**
     * Interpolate UV menggunakan barycentric coordinates
     */
    static QVector2D interpolateUV(const QVector2D& uv0,
                                   const QVector2D& uv1,
                                   const QVector2D& uv2,
                                   float u, float v);

    /**
     * Interpolate normal menggunakan barycentric coordinates
     */
    static QVector3D interpolateNormal(const QVector3D& n0,
                                       const QVector3D& n1,
                                       const QVector3D& n2,
                                       float u, float v);
};

} // namespace FeatherKrita

#endif // GUIDE_SURFACE_H
