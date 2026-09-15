/*
 * SPDX-FileCopyrightText: 2026 Feather-Krita App Contributors
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * guide_surface.cpp — Implementasi 3D guide surface
 *
 * Generate mesh untuk Sphere / Cylinder / Cone / Ring dengan UV mapping
 * dan raycast untuk dapat UV dari titik hit 3D.
 */

#include "guide_surface.h"

#include <QMatrix4x4>
#include <QtMath>
#include <QDebug>

namespace FeatherKrita {

// ---------------------------------------------------------------------------
// Constants
// ---------------------------------------------------------------------------

static constexpr float kEpsilon = 1e-6f;

// ---------------------------------------------------------------------------
// Construction / Destruction
// ---------------------------------------------------------------------------

GuideSurface::GuideSurface()
{
}

GuideSurface::~GuideSurface()
{
}

// ---------------------------------------------------------------------------
// Mesh generators
// ---------------------------------------------------------------------------

bool GuideSurface::createSphere(float radius,
                                int   segmentsH,
                                int   segmentsV)
{
    if (radius <= 0.0f || segmentsH < 3 || segmentsV < 2) {
        qWarning() << "GuideSurface::createSphere: invalid parameters";
        return false;
    }

    clear();
    m_type = SurfaceType::Sphere;

    // Sphere parameterization:
    //   theta: 0..2π (longitude, U axis)
    //   phi  : 0..π  (latitude, V axis)
    //
    // position = (r·sinφ·cosθ, r·cosφ, r·sinφ·sinθ)
    // normal   = position / r
    // uv       = (theta / 2π, phi / π)

    m_vertices.reserve((segmentsH + 1) * (segmentsV + 1));

    for (int i = 0; i <= segmentsV; ++i) {
        float phi     = float(M_PI) * float(i) / float(segmentsV);
        float sinPhi  = std::sin(phi);
        float cosPhi  = std::cos(phi);

        for (int j = 0; j <= segmentsH; ++j) {
            float theta    = 2.0f * float(M_PI) * float(j) / float(segmentsH);
            float sinTheta = std::sin(theta);
            float cosTheta = std::cos(theta);

            Vertex v;
            v.position = QVector3D(
                radius * sinPhi * cosTheta,
                radius * cosPhi,
                radius * sinPhi * sinTheta);
            v.normal = v.position.normalized();
            v.uv     = QVector2D(float(j) / float(segmentsH),
                                 float(i) / float(segmentsV));
            addVertex(v);
        }
    }

    // Indices: dua segitiga per quad
    m_indices.reserve(segmentsH * segmentsV * 6);
    for (int i = 0; i < segmentsV; ++i) {
        for (int j = 0; j < segmentsH; ++j) {
            unsigned int a = unsigned(i * (segmentsH + 1) + j);
            unsigned int b = unsigned(a + segmentsH + 1);
            addTriangle(a, b, a + 1);
            addTriangle(b, b + 1, a + 1);
        }
    }

    computeBounds();
    return true;
}

bool GuideSurface::createCylinder(float radius,
                                  float height,
                                  int   segments)
{
    if (radius <= 0.0f || height <= 0.0f || segments < 3) {
        qWarning() << "GuideSurface::createCylinder: invalid parameters";
        return false;
    }

    clear();
    m_type = SurfaceType::Cylinder;

    const float halfH = height * 0.5f;

    // Side surface
    // UV: U = theta/2π, V = y/height (0=bottom, 1=top)
    for (int j = 0; j <= segments; ++j) {
        float theta    = 2.0f * float(M_PI) * float(j) / float(segments);
        float cosTheta = std::cos(theta);
        float sinTheta = std::sin(theta);

        // bottom vertex
        Vertex vb;
        vb.position = QVector3D(radius * cosTheta, -halfH, radius * sinTheta);
        vb.normal   = QVector3D(cosTheta, 0.0f, sinTheta);
        vb.uv       = QVector2D(float(j) / float(segments), 0.0f);
        addVertex(vb);

        // top vertex
        Vertex vt;
        vt.position = QVector3D(radius * cosTheta, halfH, radius * sinTheta);
        vt.normal   = QVector3D(cosTheta, 0.0f, sinTheta);
        vt.uv       = QVector2D(float(j) / float(segments), 1.0f);
        addVertex(vt);
    }

    // Side indices
    for (int j = 0; j < segments; ++j) {
        unsigned int b0 = unsigned(j * 2);
        unsigned int t0 = b0 + 1;
        unsigned int b1 = b0 + 2;
        unsigned int t1 = b0 + 3;
        addTriangle(b0, t0, t1);
        addTriangle(b0, t1, b1);
    }

    // --- Bottom cap ---
    unsigned int bottomCenterIdx = unsigned(m_vertices.size());
    {
        Vertex c;
        c.position = QVector3D(0.0f, -halfH, 0.0f);
        c.normal   = QVector3D(0.0f, -1.0f, 0.0f);
        c.uv       = QVector2D(0.5f, 0.5f);
        addVertex(c);
    }
    unsigned int bottomStart = unsigned(m_vertices.size());
    for (int j = 0; j <= segments; ++j) {
        float theta    = 2.0f * float(M_PI) * float(j) / float(segments);
        float cosTheta = std::cos(theta);
        float sinTheta = std::sin(theta);
        Vertex v;
        v.position = QVector3D(radius * cosTheta, -halfH, radius * sinTheta);
        v.normal   = QVector3D(0.0f, -1.0f, 0.0f);
        v.uv       = QVector2D(cosTheta * 0.5f + 0.5f,
                               sinTheta * 0.5f + 0.5f);
        addVertex(v);
    }
    for (int j = 0; j < segments; ++j) {
        addTriangle(bottomCenterIdx,
                    bottomStart + unsigned(j) + 1,
                    bottomStart + unsigned(j));
    }

    // --- Top cap ---
    unsigned int topCenterIdx = unsigned(m_vertices.size());
    {
        Vertex c;
        c.position = QVector3D(0.0f, halfH, 0.0f);
        c.normal   = QVector3D(0.0f, 1.0f, 0.0f);
        c.uv       = QVector2D(0.5f, 0.5f);
        addVertex(c);
    }
    unsigned int topStart = unsigned(m_vertices.size());
    for (int j = 0; j <= segments; ++j) {
        float theta    = 2.0f * float(M_PI) * float(j) / float(segments);
        float cosTheta = std::cos(theta);
        float sinTheta = std::sin(theta);
        Vertex v;
        v.position = QVector3D(radius * cosTheta, halfH, radius * sinTheta);
        v.normal   = QVector3D(0.0f, 1.0f, 0.0f);
        v.uv       = QVector2D(cosTheta * 0.5f + 0.5f,
                               sinTheta * 0.5f + 0.5f);
        addVertex(v);
    }
    for (int j = 0; j < segments; ++j) {
        addTriangle(topCenterIdx,
                    topStart + unsigned(j),
                    topStart + unsigned(j) + 1);
    }

    computeBounds();
    return true;
}

bool GuideSurface::createCone(float baseRadius,
                              float height,
                              int   segments)
{
    if (baseRadius <= 0.0f || height <= 0.0f || segments < 3) {
        qWarning() << "GuideSurface::createCone: invalid parameters";
        return false;
    }

    clear();
    m_type = SurfaceType::Cone;

    const float halfH = height * 0.5f;

    // Apex (top)
    unsigned int apexIdx = 0;
    {
        Vertex apex;
        apex.position = QVector3D(0.0f, halfH, 0.0f);
        apex.normal   = QVector3D(0.0f, 1.0f, 0.0f);
        apex.uv       = QVector2D(0.5f, 1.0f);
        addVertex(apex);
    }

    // Side surface: bottom ring of vertices
    unsigned int baseStart = unsigned(m_vertices.size());
    for (int j = 0; j <= segments; ++j) {
        float theta    = 2.0f * float(M_PI) * float(j) / float(segments);
        float cosTheta = std::cos(theta);
        float sinTheta = std::sin(theta);

        Vertex v;
        v.position = QVector3D(baseRadius * cosTheta, -halfH, baseRadius * sinTheta);

        // Cone slant normal:
        // Surface: P(theta,y) = (r(y) cosθ, y, r(y) sinθ) where r(y) decreases linearly.
        // Normal ≈ (cosθ, baseR/height, sinθ) normalized
        float slope = baseRadius / height;
        v.normal = QVector3D(cosTheta, slope, sinTheta).normalized();
        v.uv     = QVector2D(float(j) / float(segments), 0.0f);
        addVertex(v);
    }

    // Side triangles (apex to base)
    for (int j = 0; j < segments; ++j) {
        addTriangle(apexIdx,
                    baseStart + unsigned(j),
                    baseStart + unsigned(j) + 1);
    }

    // --- Bottom cap ---
    unsigned int bottomCenterIdx = unsigned(m_vertices.size());
    {
        Vertex c;
        c.position = QVector3D(0.0f, -halfH, 0.0f);
        c.normal   = QVector3D(0.0f, -1.0f, 0.0f);
        c.uv       = QVector2D(0.5f, 0.5f);
        addVertex(c);
    }
    unsigned int bottomStart = unsigned(m_vertices.size());
    for (int j = 0; j <= segments; ++j) {
        float theta    = 2.0f * float(M_PI) * float(j) / float(segments);
        float cosTheta = std::cos(theta);
        float sinTheta = std::sin(theta);
        Vertex v;
        v.position = QVector3D(baseRadius * cosTheta, -halfH, baseRadius * sinTheta);
        v.normal   = QVector3D(0.0f, -1.0f, 0.0f);
        v.uv       = QVector2D(cosTheta * 0.5f + 0.5f,
                               sinTheta * 0.5f + 0.5f);
        addVertex(v);
    }
    for (int j = 0; j < segments; ++j) {
        addTriangle(bottomCenterIdx,
                    bottomStart + unsigned(j) + 1,
                    bottomStart + unsigned(j));
    }

    computeBounds();
    return true;
}

bool GuideSurface::createRing(float majorRadius,
                              float minorRadius,
                              int   majorSeg,
                              int   minorSeg)
{
    if (majorRadius <= 0.0f || minorRadius <= 0.0f ||
        majorSeg < 3 || minorSeg < 3) {
        qWarning() << "GuideSurface::createRing: invalid parameters";
        return false;
    }

    clear();
    m_type = SurfaceType::Ring;

    // Torus parameterization:
    //   u: 0..2π (around main ring)
    //   v: 0..2π (around tube)
    //   P(u,v) = ((R + r·cos v)·cos u,
    //             r·sin v,
    //             (R + r·cos v)·sin u)
    //   normal = (cos v · cos u, sin v, cos v · sin u)
    //   UV     = (u / 2π, v / 2π)

    m_vertices.reserve((majorSeg + 1) * (minorSeg + 1));

    for (int i = 0; i <= majorSeg; ++i) {
        float u    = 2.0f * float(M_PI) * float(i) / float(majorSeg);
        float cu   = std::cos(u);
        float su   = std::sin(u);

        for (int j = 0; j <= minorSeg; ++j) {
            float v    = 2.0f * float(M_PI) * float(j) / float(minorSeg);
            float cv   = std::cos(v);
            float sv   = std::sin(v);

            Vertex vert;
            vert.position = QVector3D(
                (majorRadius + minorRadius * cv) * cu,
                minorRadius * sv,
                (majorRadius + minorRadius * cv) * su);
            vert.normal = QVector3D(cv * cu, sv, cv * su).normalized();
            vert.uv     = QVector2D(float(i) / float(majorSeg),
                                    float(j) / float(minorSeg));
            addVertex(vert);
        }
    }

    // Indices
    m_indices.reserve(majorSeg * minorSeg * 6);
    for (int i = 0; i < majorSeg; ++i) {
        for (int j = 0; j < minorSeg; ++j) {
            unsigned int a = unsigned(i * (minorSeg + 1) + j);
            unsigned int b = unsigned(a + minorSeg + 1);
            addTriangle(a, b, a + 1);
            addTriangle(b, b + 1, a + 1);
        }
    }

    computeBounds();
    return true;
}

bool GuideSurface::createPlane(float width,
                               float height,
                               int   segX,
                               int   segY)
{
    if (width <= 0.0f || height <= 0.0f || segX < 1 || segY < 1) {
        qWarning() << "GuideSurface::createPlane: invalid parameters";
        return false;
    }

    clear();
    m_type = SurfaceType::Custom;

    const float halfW = width * 0.5f;
    const float halfH = height * 0.5f;

    m_vertices.reserve((segX + 1) * (segY + 1));

    for (int y = 0; y <= segY; ++y) {
        for (int x = 0; x <= segX; ++x) {
            Vertex v;
            float fx = float(x) / float(segX);
            float fy = float(y) / float(segY);
            v.position = QVector3D(-halfW + width * fx,
                                   0.0f,
                                   -halfH + height * fy);
            v.normal = QVector3D(0.0f, 1.0f, 0.0f);
            v.uv     = QVector2D(fx, fy);
            addVertex(v);
        }
    }

    m_indices.reserve(segX * segY * 6);
    for (int y = 0; y < segY; ++y) {
        for (int x = 0; x < segX; ++x) {
            unsigned int a = unsigned(y * (segX + 1) + x);
            unsigned int b = a + 1;
            unsigned int c = a + unsigned(segX + 1);
            unsigned int d = c + 1;
            addTriangle(a, c, b);
            addTriangle(b, c, d);
        }
    }

    computeBounds();
    return true;
}

// ---------------------------------------------------------------------------
// Raycast
// ---------------------------------------------------------------------------

RaycastHit GuideSurface::raycast(const Ray& ray) const
{
    RaycastHit result;
    if (m_indices.isEmpty()) {
        return result;
    }

    // Transform ray ke local space (inverse of model matrix)
    QMatrix4x4 model = localToWorldMatrix();
    QMatrix4x4 invModel = model.inverted();

    QVector3D localOrigin    = invModel.map(ray.origin);
    QVector3D localDirection = invModel.mapVector(ray.direction).normalized();

    float closestT = std::numeric_limits<float>::max();
    int   closestTri = -1;
    float hitU = 0.0f, hitV = 0.0f;

    const int triCount = m_indices.size() / 3;
    for (int i = 0; i < triCount; ++i) {
        unsigned int i0 = m_indices[i * 3 + 0];
        unsigned int i1 = m_indices[i * 3 + 1];
        unsigned int i2 = m_indices[i * 3 + 2];

        const QVector3D& v0 = m_vertices[i0].position;
        const QVector3D& v1 = m_vertices[i1].position;
        const QVector3D& v2 = m_vertices[i2].position;

        float t, u, v;
        if (intersectTriangle(localOrigin, localDirection,
                              v0, v1, v2, t, u, v)) {
            if (t > 0.0f && t < closestT) {
                closestT  = t;
                closestTri = i;
                hitU = u;
                hitV = v;
            }
        }
    }

    if (closestTri < 0) {
        return result; // hit=false
    }

    // Hit info
    unsigned int i0 = m_indices[closestTri * 3 + 0];
    unsigned int i1 = m_indices[closestTri * 3 + 1];
    unsigned int i2 = m_indices[closestTri * 3 + 2];

    QVector3D localHitPoint = localOrigin + localDirection * closestT;
    QVector3D localNormal   = interpolateNormal(
        m_vertices[i0].normal,
        m_vertices[i1].normal,
        m_vertices[i2].normal,
        hitU, hitV);

    result.hit          = true;
    result.point        = model.map(localHitPoint);
    result.normal       = model.mapVector(localNormal).normalized();
    result.uv           = interpolateUV(
        m_vertices[i0].uv,
        m_vertices[i1].uv,
        m_vertices[i2].uv,
        hitU, hitV);
    result.distance     = closestT;
    result.triangleIdx  = closestTri;

    return result;
}

QVector2D GuideSurface::getUVFromPoint(const QVector3D& point) const
{
    if (m_vertices.isEmpty()) {
        return QVector2D(-1.0f, -1.0f);
    }

    // Convert world point to local space
    QMatrix4x4 invModel = localToWorldMatrix().inverted();
    QVector3D localPoint = invModel.map(point);

    // Cari triangle yang berisi point (proyeksi ke surface)
    float bestDist = std::numeric_limits<float>::max();
    QVector2D bestUV(-1.0f, -1.0f);

    const int triCount = m_indices.size() / 3;
    for (int i = 0; i < triCount; ++i) {
        unsigned int i0 = m_indices[i * 3 + 0];
        unsigned int i1 = m_indices[i * 3 + 1];
        unsigned int i2 = m_indices[i * 3 + 2];

        const QVector3D& v0 = m_vertices[i0].position;
        const QVector3D& v1 = m_vertices[i1].position;
        const QVector3D& v2 = m_vertices[i2].position;

        // Normal segitiga
        QVector3D n = QVector3D::crossProduct(v1 - v0, v2 - v0).normalized();

        // Project point ke plane segitiga
        float d = QVector3D::dotProduct(n, v0 - localPoint);
        QVector3D projected = localPoint + n * d;

        // Barycentric check
        QVector3D vp = projected - v0;
        QVector3D e0 = v1 - v0;
        QVector3D e1 = v2 - v0;
        float denom = QVector3D::dotProduct(e0, e0) * QVector3D::dotProduct(e1, e1)
                    - std::pow(QVector3D::dotProduct(e0, e1), 2);
        if (std::abs(denom) < kEpsilon) continue;

        float u = (QVector3D::dotProduct(vp, e0) * QVector3D::dotProduct(e1, e1)
                 - QVector3D::dotProduct(vp, e1) * QVector3D::dotProduct(e0, e1)) / denom;
        float v = (QVector3D::dotProduct(vp, e1) * QVector3D::dotProduct(e0, e0)
                 - QVector3D::dotProduct(vp, e0) * QVector3D::dotProduct(e0, e1)) / denom;

        float distSq = (localPoint - projected).lengthSquared();
        if (u >= 0.0f && v >= 0.0f && (u + v) <= 1.0f) {
            // Inside triangle — perfect hit
            bestUV = m_vertices[i0].uv * (1.0f - u - v)
                   + m_vertices[i1].uv * u
                   + m_vertices[i2].uv * v;
            return bestUV;
        }
        if (distSq < bestDist) {
            bestDist = distSq;
            bestUV   = m_vertices[i0].uv * (1.0f - u - v)
                     + m_vertices[i1].uv * u
                     + m_vertices[i2].uv * v;
        }
    }

    return bestUV;
}

// ---------------------------------------------------------------------------
// Transform
// ---------------------------------------------------------------------------

void GuideSurface::setTransform(const QVector3D& position,
                                const QVector3D& rotation,
                                const QVector3D& scale)
{
    m_position = position;
    m_rotation = rotation;
    m_scale    = scale;
}

QVector3D GuideSurface::localToWorld(const QVector3D& local) const
{
    return localToWorldMatrix().map(local);
}

QVector3D GuideSurface::worldToLocal(const QVector3D& world) const
{
    return localToWorldMatrix().inverted().map(world);
}

QMatrix4x4 GuideSurface::localToWorldMatrix() const
{
    QMatrix4x4 m;
    m.translate(m_position);
    m.rotate(m_rotation.x(), QVector3D(1, 0, 0));
    m.rotate(m_rotation.y(), QVector3D(0, 1, 0));
    m.rotate(m_rotation.z(), QVector3D(0, 0, 1));
    m.scale(m_scale);
    return m;
}

void GuideSurface::getBounds(QVector3D& min, QVector3D& max) const
{
    min = m_boundsMin;
    max = m_boundsMax;
}

void GuideSurface::clear()
{
    m_vertices.clear();
    m_indices.clear();
    m_boundsMin = QVector3D();
    m_boundsMax = QVector3D();
}

// ---------------------------------------------------------------------------
// Type info
// ---------------------------------------------------------------------------

QString GuideSurface::typeName() const
{
    switch (m_type) {
    case SurfaceType::Sphere:   return QStringLiteral("Sphere");
    case SurfaceType::Cylinder: return QStringLiteral("Cylinder");
    case SurfaceType::Cone:     return QStringLiteral("Cone");
    case SurfaceType::Ring:     return QStringLiteral("Ring");
    case SurfaceType::Custom:   return QStringLiteral("Custom");
    }
    return QStringLiteral("Unknown");
}

// ---------------------------------------------------------------------------
// Internal helpers
// ---------------------------------------------------------------------------

void GuideSurface::addVertex(const Vertex& v)
{
    m_vertices.append(v);
}

void GuideSurface::addTriangle(unsigned int a, unsigned int b, unsigned int c)
{
    m_indices.append(a);
    m_indices.append(b);
    m_indices.append(c);
}

void GuideSurface::computeBounds()
{
    if (m_vertices.isEmpty()) {
        m_boundsMin = QVector3D();
        m_boundsMax = QVector3D();
        return;
    }

    m_boundsMin = m_vertices[0].position;
    m_boundsMax = m_vertices[0].position;
    for (const Vertex& v : m_vertices) {
        for (int ax = 0; ax < 3; ++ax) {
            float val = v.position[ax];
            if (val < m_boundsMin[ax]) m_boundsMin[ax] = val;
            if (val > m_boundsMax[ax]) m_boundsMax[ax] = val;
        }
    }
}

bool GuideSurface::intersectTriangle(const QVector3D& rayOrigin,
                                     const QVector3D& rayDir,
                                     const QVector3D& v0,
                                     const QVector3D& v1,
                                     const QVector3D& v2,
                                     float& t,
                                     float& u,
                                     float& v)
{
    // Möller–Trumbore ray-triangle intersection
    QVector3D edge1 = v1 - v0;
    QVector3D edge2 = v2 - v0;
    QVector3D h     = QVector3D::crossProduct(rayDir, edge2);
    float det       = QVector3D::dotProduct(edge1, h);

    if (std::abs(det) < kEpsilon) {
        return false; // ray parallel to triangle
    }

    float invDet = 1.0f / det;
    QVector3D s  = rayOrigin - v0;
    u = invDet * QVector3D::dotProduct(s, h);
    if (u < 0.0f || u > 1.0f) return false;

    QVector3D q = QVector3D::crossProduct(s, edge1);
    v = invDet * QVector3D::dotProduct(rayDir, q);
    if (v < 0.0f || (u + v) > 1.0f) return false;

    t = invDet * QVector3D::dotProduct(edge2, q);
    return t > kEpsilon;
}

QVector2D GuideSurface::interpolateUV(const QVector2D& uv0,
                                      const QVector2D& uv1,
                                      const QVector2D& uv2,
                                      float u, float v)
{
    return uv0 * (1.0f - u - v) + uv1 * u + uv2 * v;
}

QVector3D GuideSurface::interpolateNormal(const QVector3D& n0,
                                          const QVector3D& n1,
                                          const QVector3D& n2,
                                          float u, float v)
{
    return (n0 * (1.0f - u - v) + n1 * u + n2 * v).normalized();
}

} // namespace FeatherKrita
