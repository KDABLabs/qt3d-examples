#include "mapzenimagerytileprovider.h"

MapzenSatelliteImageryTileProvider::MapzenSatelliteImageryTileProvider()
{
}

QUrl MapzenSatelliteImageryTileProvider::imageryUrl(quint64 zoom, quint64 x, quint64 y)
{
//    const QString tileName = m_tileBaseName + QString("%1_%2_%3.jpg").arg(int(zoom), int(x), int(y));
//    return QUrl::fromLocalFile(m_tileDirectory + tileName);
//    const QString tileName = QString("/home/lemire_p/work/cubespheretextures/src/textures/satellite/satellite_%1_%2_%3.jpg").arg(int(zoom), int(x), int(y));
    const QString tileName = QString("/home/lemire_p/work/cubespheretextures/src/textures/satellite/satellite_") + QString::number(zoom) + QString("_") + QString::number(x) + "_" + QString::number(y) + QString(".jpg");
    return QUrl::fromLocalFile(tileName);
}

QImage MapzenSatelliteImageryTileProvider::imageryTile(quint64 zoom, quint64 x, quint64 y)
{
    return QImage(imageryUrl(zoom, x, y).toLocalFile());
}


MapzenElevationTileProvider::MapzenElevationTileProvider()
{
}

QUrl MapzenElevationTileProvider::imageryUrl(quint64 zoom, quint64 x, quint64 y)
{
    const QString tileName = QString("/home/lemire_p/work/cubespheretextures/src/textures/elevation/elevation_") + QString::number(zoom) + QString("_") + QString::number(x) + "_" + QString::number(y) + QString(".png");
    return QUrl::fromLocalFile(tileName);
}

QImage MapzenElevationTileProvider::imageryTile(quint64 zoom, quint64 x, quint64 y)
{
    return QImage(imageryUrl(zoom, x, y).toLocalFile());
}
