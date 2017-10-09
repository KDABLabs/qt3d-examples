#ifndef CUBEBUILDER_H
#define CUBEBUILDER_H

#include <QtGui/QVector3D>
#include <QtGui/QVector4D>
#include <QtGui/QMatrix4x4>
#include <qdoublevector3d_p.h>
#include <cmath>
#include "cubesphere.h"
#include "imagerytileprovider.h"

namespace Qt3DRender {
class QCamera;
} // Qt3DRender

class QuadNode;
class ImageryTileProvider;

struct ImageEntry
{
    quint64 tileX = 0;
    quint64 tileY = 0;
    int zoom = 0;

    bool operator ==(const ImageEntry &other) const
    {
        return other.tileX == tileX && other.tileY == tileY && other.zoom == zoom;
    }
};

struct ImageryMapping
{
    quint64 tileX = 0;
    quint64 tileY = 0;
    int zoom = -1;
    int cornerIdx = 0;
    double offsetX = 0;
    double offsetY = 0;
    double extentXMin = 0;
    double extentXMax = 0;
    double extentYMin = 0;
    double extentYMax = 0;
    double scale = 0;
};

struct TileImageryMapping {
    ImageryMapping mappingEntries[4];
};

class CubeBuilder
{
public:
    CubeBuilder(const int baseLevel,
                const int maxLevel,
                Qt3DRender::QCamera *camera);

    ~CubeBuilder();

    void update();

    QUrl imageryUrls(ImageryTileProvider::ImageryType type, int zoom, quint64 x, quint64 y) const;
    QVector<QUrl> elevationUrls() const;

    QVector<QDoubleVector3D> sphereVertices() const;
    QVector<QDoubleVector3D> cubeVertices() const;
    QVector<TileImageryMapping> tileImageMappings(ImageryTileProvider::ImageryType type) const;

    void setBaseLevel(int baseLevel) { m_baseLevel = baseLevel; }
    inline int baseLevel() const { return m_baseLevel; }

    void setMaxLevel(int maxLevel) { m_maxLevel = maxLevel; }
    inline int maxLevel() const { return m_maxLevel; }

    inline Qt3DRender::QCamera *camera() const { return m_camera; }
    void setCamera(Qt3DRender::QCamera *camera) { m_camera = camera; }

    inline QSize viewportSize() const { return m_viewportSize; }
    void setViewportSize(QSize viewportSize) { m_viewportSize = viewportSize; }

    QMatrix4x4 viewProjectionMatrix() const { return m_viewProjection; }

#if defined(UNIT_TESTS)
public:
#else
private:
#endif
    struct CubeFace
    {
        QuadNode *node; // root
        QVector<QuadNode*> m_leaves; // nodes to render
        QVector<QDoubleVector3D> m_sphereVertices; // vertices for the nodes to render
        QVector<QDoubleVector3D> m_cubeVertices; // vertices for the nodes to render
        QHash<ImageryTileProvider::ImageryType, QVector<TileImageryMapping>> m_nodeRenderDataPerProvider;
    };

    struct Cube
    {
        CubeFace faces[6];
    };

    struct Plane
    {
        explicit Plane(const QVector4D &planeEquation = QVector4D())
            : normal(planeEquation.toVector3D().normalized())
            , d(planeEquation.w() / planeEquation.toVector3D().length())
        {}

        QDoubleVector3D normal;
        double d;
    };
    QVector<Plane> m_cullPlanes;

    struct Sphere
    {
        explicit Sphere(QuadNode *node);

        QDoubleVector3D center() const { return m_center; }
        double radius() const { return m_radius; }

    private:
        QDoubleVector3D m_center;
        double m_radius;
    };

    int m_baseLevel;
    int m_maxLevel;
    Qt3DRender::QCamera *m_camera;
    QSize m_viewportSize;
    Cube m_cube;
    QMatrix4x4 m_viewProjection;

    void subDivideQuadFace(CubeFace *face);
    bool shouldNodeBeSplit(QuadNode *node) const;
    bool isFrustumCulled(QuadNode *node) const;
    void subDivideQuadNodeHelper(CubeFace *face, QuadNode *node);
    void buildGPUResources(CubeFace *face);

    QHash<ImageryTileProvider::ImageryType, ImageryTileProvider *> m_tileProviders;

    static QVector<QDoubleVector3D> sphereVertexDataForNode(QuadNode *node);
    static QVector<QDoubleVector3D> cubeVertexDataForNode(QuadNode *node);
    static TileImageryMapping imageDataForNode(QuadNode *node, ImageryTileProvider *tileProvider);
    static QPair<double, double> slippyTileId(double lon, double lat, int zoom);

    static QPair<quint64, quint64> flooredSlippyTileId(const QPair<double, double> &tileId)
    {
        return qMakePair(std::floor(tileId.first), std::floor(tileId.second));
    }
};
#endif // CUBEBUILDER_H
