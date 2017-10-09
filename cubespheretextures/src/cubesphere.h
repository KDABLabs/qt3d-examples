#ifndef CUBESPHERE_H
#define CUBESPHERE_H

#include <Qt3DCore/QEntity>
#include <QVector3D>
#include <QSize>

namespace Qt3DRender {
class QCamera;
class QTexture2DArray;
class QBuffer;
class QAttribute;
class QGeometryRenderer;
} // Qt3DRender

class CubeBuilder;

class CubeSphere : public Qt3DCore::QEntity
{
    Q_OBJECT
    Q_PROPERTY(Qt3DRender::QCamera *camera READ camera WRITE setCamera NOTIFY cameraChanged)
    Q_PROPERTY(QSize viewportSize READ viewportSize WRITE setViewportSize NOTIFY viewportSizeChanged)
    Q_PROPERTY(Qt3DRender::QGeometryRenderer *sphereRenderer READ sphereRenderer CONSTANT)
    Q_PROPERTY(Qt3DRender::QGeometryRenderer *cubeRenderer READ cubeRenderer CONSTANT)
    Q_PROPERTY(Qt3DRender::QTexture2DArray *satelliteImagery READ satelliteImagery CONSTANT)
    Q_PROPERTY(Qt3DRender::QTexture2DArray *elevation READ elevation CONSTANT)
    Q_PROPERTY(Qt3DRender::QBuffer *satelliteUniformBuffer READ satelliteUniformBuffer CONSTANT)
    Q_PROPERTY(Qt3DRender::QBuffer *elevationUniformBuffer READ elevationUniformBuffer CONSTANT)
    Q_PROPERTY(int baseLevel READ baseLevel WRITE setBaseLevel NOTIFY baseLevelChanged)
    Q_PROPERTY(int maxLevel READ maxLevel WRITE setMaxLevel NOTIFY maxLevelChanged)
    Q_PROPERTY(int tilesToRender READ tilesToRender NOTIFY tilesToRenderChanged)
    Q_PROPERTY(int imageryTilesCount READ imageryTilesCount NOTIFY imageryTilesCountChanged)
    Q_PROPERTY(int elevationTilesCount READ elevationTilesCount NOTIFY elevationTilesCountChanged)
    Q_PROPERTY(float updateElapsed READ updateElapsed NOTIFY updateElapsedChanged)
    Q_PROPERTY(bool useImagery READ useImagery WRITE setUseImagery NOTIFY useImageryChanged)
    Q_PROPERTY(bool useElevation READ useElevation WRITE setUseElevation NOTIFY useElevationChanged)

public:
    explicit CubeSphere(Qt3DCore::QNode *parent = nullptr);
    ~CubeSphere();

    inline Qt3DRender::QCamera *camera() const { return m_camera; }
    inline QSize viewportSize() const { return m_viewportSize; }
    inline Qt3DRender::QGeometryRenderer *sphereRenderer() const { return m_sphereRenderer; }
    inline Qt3DRender::QGeometryRenderer *cubeRenderer() const { return m_cubeRenderer; }
    inline Qt3DRender::QTexture2DArray *satelliteImagery() const { return m_satelliteImagery; }
    inline Qt3DRender::QTexture2DArray *elevation() const { return m_elevation; }
    inline Qt3DRender::QBuffer *satelliteUniformBuffer() const { return m_satelliteUniformBuffer; }
    inline Qt3DRender::QBuffer *elevationUniformBuffer() const { return m_elevationUniformBuffer; }

    inline int baseLevel() const { return m_baseLevel; }
    void setBaseLevel(int baseLevel);

    inline int maxLevel() const { return m_maxLevel; }
    void setMaxLevel(int maxLevel);

    inline int tilesToRender() const { return m_tilesToRender; }
    inline int imageryTilesCount() const { return m_imageryTilesCount; }
    inline int elevationTilesCount() const { return m_elevationTilesCount; }
    inline float updateElapsed() const { return m_updateElapsed; }

    void setUseImagery(bool use);
    inline bool useImagery() const { return m_useImagery; }
    void setUseElevation(bool use);
    inline bool useElevation() const { return m_useElevation; }

public slots:
    void setCamera(Qt3DRender::QCamera * camera);
    void setViewportSize(QSize viewportSize);

signals:
    void sphereProjectionChanged(bool sphereProjection);
    void cameraPositionChanged(QVector3D cameraPosition);
    void cameraChanged(Qt3DRender::QCamera * camera);
    void viewportSizeChanged(QSize viewportSize);
    void baseLevelChanged();
    void maxLevelChanged();
    void tilesToRenderChanged();
    void imageryTilesCountChanged();
    void elevationTilesCountChanged();
    void updateElapsedChanged();
    void useImageryChanged();
    void useElevationChanged();

private slots:
    void updateCube();

private:
    Qt3DRender::QGeometryRenderer *m_sphereRenderer;
    Qt3DRender::QGeometryRenderer *m_cubeRenderer;
    Qt3DRender::QAttribute *m_sphereVertexPositionAttribute;
    Qt3DRender::QAttribute *m_cubeVertexPositionAttribute;
    Qt3DRender::QAttribute *m_cubeIndexAttribute;
    Qt3DRender::QBuffer *m_sphereVertexBuffer;
    Qt3DRender::QBuffer *m_cubeVertexBuffer;
    Qt3DRender::QBuffer *m_cubeIndexBuffer;
    Qt3DRender::QBuffer *m_satelliteUniformBuffer;
    Qt3DRender::QBuffer *m_elevationUniformBuffer;
    Qt3DRender::QTexture2DArray *m_satelliteImagery;
    Qt3DRender::QTexture2DArray *m_elevation;

    struct ImageEntryToLayer
    {
        int zoom = 0;
        int score = -1;
        int layerId = -1;
        quint64 tileX = 0;
        quint64 tileY = 0;
    };


    void updateImageryScores();
    int satelliteTextureLayerFor(int zoom, quint64 x, quint64 y);
    int elevationTextureLayerFor(int zoom, quint64 x, quint64 y);

    Qt3DRender::QCamera *m_camera;
    QScopedPointer<CubeBuilder> m_cubeBuilder;
    QSize m_viewportSize;

    int m_baseLevel;
    int m_maxLevel;
    int m_tilesToRender;
    int m_imageryTilesCount;
    int m_elevationTilesCount;
    float m_updateElapsed;
    bool m_useImagery;
    bool m_useElevation;
    bool m_requiresUpdate;

    QVector<ImageEntryToLayer> m_imageEntriesForSatellite;
    QVector<ImageEntryToLayer> m_imageEntriesForElevation;
};



#endif // CUBESPHERE_H
