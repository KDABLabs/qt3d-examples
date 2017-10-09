#ifndef GEODETIC_H
#define GEODETIC_H

#include <qdoublevector3d_p.h>
#include "earthellipsoid.h"
#include <cmath>

#define RAD_TO_DEG(rad) 180.0 * rad / M_PI
#define DEG_TO_RAD(deg) M_PI * deg / 180.0

struct Geodetic2D
{
    Geodetic2D()
        : lat(0.0)
        , lon(0.0)
    {}

    Geodetic2D(double lat, double lon)
        : lat(lat)
        , lon(lon)
    {}

    double lat, lon; // in radians

    static Geodetic2D fromWGS84(const QDoubleVector3D &p)
    {
        const QDoubleVector3D geoditicSurfaceNormal = EarthEllipsoid::geodeticSurfaceNormal(p);
        Geodetic2D geo;
        Q_ASSERT(qFuzzyCompare(geoditicSurfaceNormal.length(), 1.0));
        geo.lat = asin(geoditicSurfaceNormal.z()); // should be n / |n| but |n| should be 1
        geo.lon = atan2(geoditicSurfaceNormal.y(), geoditicSurfaceNormal.x());
        return geo;
    }
};

struct Geodetic3D
{
    Geodetic3D()
        : lat(0.0)
        , lon(0.0)
        , height(0.0)
    {}

    Geodetic3D(Geodetic2D geo2D, double height = 0.0)
        : lat(geo2D.lat)
        , lon(geo2D.lon)
        , height(height)
    {}

    Geodetic3D(double lat, double lon, double height = 0.0)
        : lat(lat)
        , lon(lon)
        , height(height)
    {}

    double lat, lon, height; // lat, lon in radians, height in meters

    QDoubleVector3D toWGS84() const
    {
        const QDoubleVector3D geodeticSurfaceNormal = EarthEllipsoid::geodeticSurfaceNormal(*this);
        const QDoubleVector3D k = EarthEllipsoid::wgs84RadiiSquared * geodeticSurfaceNormal;
        const double g = sqrt(QDoubleVector3D::dotProduct(k, geodeticSurfaceNormal));
        const QDoubleVector3D rSurface = k / g;
        return rSurface + (height * geodeticSurfaceNormal);
    }
};

#endif // GEODETIC_H
