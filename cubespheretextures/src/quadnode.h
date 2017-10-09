#ifndef QUADNODE_H
#define QUADNODE_H

#include <QVector3D>
#include <qmath.h>
#include "earthellipsoid.h"
#include "geodetic.h"

inline QDoubleVector3D projectToSphere(const QDoubleVector3D cubeVertex)
{
    // Center of sphere O is 0, 0, 0
    // radius is 0.5

    // For each corner C, compute OC
    // const QVector3D O;
    // const QVector3D OC = C - O;
    // which is actually C
    // Normalize OC and multiply by proper sphere radius
    return cubeVertex.normalized() * EarthEllipsoid::wgs84Radii;
}

//inline QDoubleVector3D geodeticSurfaceNormal(const QDoubleVector3D p)
//{
//    static const QDoubleVector3D oneOverWgs84RadiiSquared(1.0 / (wgs84Radii.x() * wgs84Radii.x()),
//                                                          1.0 / (wgs84Radii.y() * wgs84Radii.y()),
//                                                          1.0 / (wgs84Radii.z() * wgs84Radii.z()));

//    return (p * oneOverWgs84RadiiSquared).normalized();
//}

// These are Geometric tiles
// Each leaf tile is then transformed into a grid of 256 x 256 vertices
struct QuadNode
{
    explicit QuadNode(QuadNode *parentPtr = nullptr, const int level = 1)
        : m_level(level)
        , m_parent(parentPtr)
    {}

    ~QuadNode()
    {
    }

    // Creates children
    void split()
    {
        if (!isLeaf())
            return;

        // We need to subdivide
        nw = new QuadNode(this, m_level + 1);
        ne = new QuadNode(this, m_level + 1);
        sw = new QuadNode(this, m_level + 1);
        se = new QuadNode(this, m_level + 1);

        // Set vertices for each quad node
        nw->vertices[0] = vertices[0];
        sw->vertices[1] = vertices[1];
        ne->vertices[2] = vertices[2];
        se->vertices[3] = vertices[3];

        nw->vertices[1] = (vertices[0] + vertices[1]) * 0.5;
        nw->vertices[2] = (vertices[0] + vertices[2]) * 0.5;
        sw->vertices[3] = (vertices[1] + vertices[3]) * 0.5;
        ne->vertices[3] = (vertices[2] + vertices[3]) * 0.5;
        nw->vertices[3] = (nw->vertices[1] + ne->vertices[3]) * 0.5;

        sw->vertices[0] = nw->vertices[1];
        sw->vertices[2] = nw->vertices[3];
        ne->vertices[1] = nw->vertices[3];
        ne->vertices[0] = nw->vertices[2];
        se->vertices[0] = nw->vertices[3];
        se->vertices[2] = ne->vertices[3];
        se->vertices[1] = sw->vertices[3];

        nw->generateSphereVerticesAndGeoCoordinates();
        ne->generateSphereVerticesAndGeoCoordinates();
        sw->generateSphereVerticesAndGeoCoordinates();
        se->generateSphereVerticesAndGeoCoordinates();
    }

    // Destroys children
    void merge()
    {
        if (isLeaf())
            return;

        Q_ASSERT(nw && sw && ne && se);

        nw->merge();
        sw->merge();
        ne->merge();
        se->merge();

        delete nw;
        delete sw;
        delete ne;
        delete se;

        nw = nullptr;
        sw = nullptr;
        ne = nullptr;
        se = nullptr;
    }

    void generateSphereVerticesAndGeoCoordinates()
    {
        sphereVertices[0] = projectToSphere(vertices[0]);
        sphereVertices[1] = projectToSphere(vertices[1]);
        sphereVertices[2] = projectToSphere(vertices[2]);
        sphereVertices[3] = projectToSphere(vertices[3]);

        // Compute geographic coords of QuadNode based on vertices
        for (int i = 0; i < 4; ++i) {
            const QDoubleVector3D v = sphereVertices[i];
            coords[i] = Geodetic2D::fromWGS84(v);
        }
    }

    inline bool isLeaf() const { return nw == nullptr && ne == nullptr && sw == nullptr && se == nullptr; }

    const int m_level;

    /*
     *      0 ----- 2
     *      |       |
     *      |       |
     *      1-------3
     */
    QDoubleVector3D vertices[4];

    // WGS84
    QDoubleVector3D sphereVertices[4];

    // Geographic coordiantes
    Geodetic2D coords[4];

    QuadNode *nw = nullptr;
    QuadNode *ne = nullptr;
    QuadNode *sw = nullptr;
    QuadNode *se = nullptr;

    QuadNode *m_parent = nullptr;
    bool m_isToBeRendered = false;


    // For a given QuadNode, there could be several map tiles
    // matching
    struct ImageTiles
    {
        int x, y, level; // Assume WebMercator for now
        // WebMercator Tile ID
    };
    QVector<ImageTiles> imageTiles;
};

using QuadNodeWPtr = QWeakPointer<QuadNode>;
using QuadNodePtr = QSharedPointer<QuadNode>;

#endif // QUADNODE_H
