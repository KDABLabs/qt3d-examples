#ifndef IMAGERYTILEPROVIDER_H
#define IMAGERYTILEPROVIDER_H

#include <QImage>
#include <QUrl>

class ImageryTileProvider
{
public:
    enum ImageryType {
        Satellite,
        Elevation
    };

    virtual ~ImageryTileProvider() {}

    virtual ImageryType type() const = 0;
    virtual int maxSupportedZoomLevel() const = 0;
    virtual QUrl imageryUrl(quint64 zoom, quint64 x, quint64 y) = 0;
    virtual QImage imageryTile(quint64 zoom, quint64 x, quint64 y) = 0;
};

#endif // IMAGERYTILEPROVIDER_H
