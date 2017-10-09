#ifndef MAPZENIMAGERYTILEPROVIDER_H
#define MAPZENIMAGERYTILEPROVIDER_H

#include "imagerytileprovider.h"

class MapzenSatelliteImageryTileProvider : public ImageryTileProvider
{
public:
    MapzenSatelliteImageryTileProvider();

    ImageryType type() const { return ImageryTileProvider::Satellite; }
    // Technically max is 18 but downloading tiles higher than zoom 7 takes up
    // space
    int maxSupportedZoomLevel() const Q_DECL_OVERRIDE { return 7; }
    QUrl imageryUrl(quint64 zoom, quint64 x, quint64 y) Q_DECL_OVERRIDE;
    QImage imageryTile(quint64 zoom, quint64 x, quint64 y) Q_DECL_OVERRIDE;
};

class MapzenElevationTileProvider : public ImageryTileProvider
{
public:
    MapzenElevationTileProvider();

    ImageryType type() const { return ImageryTileProvider::Elevation; }
    // Technically max is 18 but downloading tiles higher than zoom 7 takes up
    // space
    int maxSupportedZoomLevel() const Q_DECL_OVERRIDE { return 6; }
    QUrl imageryUrl(quint64 zoom, quint64 x, quint64 y) Q_DECL_OVERRIDE;
    QImage imageryTile(quint64 zoom, quint64 x, quint64 y) Q_DECL_OVERRIDE;
};

#endif // MAPZENIMAGERYTILEPROVIDER_H
