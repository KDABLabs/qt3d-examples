#include "earthellipsoid.h"
#include "geodetic.h"
#include <cmath>

const QDoubleVector3D EarthEllipsoid::wgs84Radii = QDoubleVector3D(6378137.0, 6356752.3, 6378137.0); // (in meters)
const QDoubleVector3D EarthEllipsoid::wgs84RadiiSquared = EarthEllipsoid::wgs84Radii * EarthEllipsoid::wgs84Radii;
const QDoubleVector3D EarthEllipsoid::oneOverWgs84RadiiSquared = QDoubleVector3D(1.0 / (EarthEllipsoid::wgs84Radii.x() * EarthEllipsoid::wgs84Radii.x()),
                                                                   1.0 / (EarthEllipsoid::wgs84Radii.y() * EarthEllipsoid::wgs84Radii.y()),
                                                                   1.0 / (EarthEllipsoid::wgs84Radii.z() * EarthEllipsoid::wgs84Radii.z()));


QDoubleVector3D EarthEllipsoid::centricSurfaceNormal(const QDoubleVector3D &p)
{
    return p.normalized();
}

QDoubleVector3D EarthEllipsoid::geodeticSurfaceNormal(const QDoubleVector3D &p)
{
    QDoubleVector3D normal = p * (EarthEllipsoid::oneOverWgs84RadiiSquared);
    return normal.normalized();
}

QDoubleVector3D EarthEllipsoid::geodeticSurfaceNormal(const Geodetic3D &p)
{
    const double cosLat = cos(p.lat);
    return QDoubleVector3D(cosLat * cos(p.lon),
                           cosLat * sin(p.lon),
                           sin(p.lat));
}
