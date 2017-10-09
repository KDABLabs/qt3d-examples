#ifndef EARTHELLIPSOID_H
#define EARTHELLIPSOID_H

#include <qdoublevector3d_p.h>

struct Geodetic3D;

struct EarthEllipsoid
{
    // Vector p in WGS84 coordinates
    static QDoubleVector3D centricSurfaceNormal(const QDoubleVector3D &p);
    static QDoubleVector3D geodeticSurfaceNormal(const QDoubleVector3D &p);

    static QDoubleVector3D geodeticSurfaceNormal(const Geodetic3D &p);

    static const QDoubleVector3D wgs84Radii;
    static const QDoubleVector3D wgs84RadiiSquared;
    static const QDoubleVector3D oneOverWgs84RadiiSquared;
};

#endif // EARTHELLIPSOID_H
