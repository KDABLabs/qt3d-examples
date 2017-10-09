#include "cubesphere.h"
#include <Qt3DRender/QGeometryRenderer>
#include <Qt3DRender/QBuffer>
#include <Qt3DRender/QAttribute>
#include <Qt3DRender/QCamera>
#include <Qt3DRender/QTexture>
#include <QVector3D>
#include <QVector2D>
#include <QElapsedTimer>
#include <qmath.h>

#include "qdoublevector3d_p.h"
#include "qdoublematrix4x4_p.h"

#include "cubebuilder.h"

// Attributes:
// [ Vec4 ] NW Tiles Corner relative to Eye ( 1 per instance)
// [ Vec4 ] NE Tiles Corner relative to Eye ( 1 per instance)
// [ Vec4 ] SW Tiles Corner relative to Eye ( 1 per instance)
// [ Vec4 ] SE Tiles Corner relative to Eye ( 1 per instance)
// [ Uint ] index into ubo SSBO

namespace {

struct TextureMappingInfo
{
    // float layer, float scale, vec2 offset
    QVector4D layerScaleOffset[4]; // 4 * 16
    QVector4D extentMinMax[4]; // 4 * 16
};
static_assert(sizeof(TextureMappingInfo) == 8 * 16, "TextureInfo doesn't satisfy std140 layout");

// For now we assume you have a GL 4.5 capable gpu
const int maxLayerCount = 2048;
const int maxUniformBlockSize = 65536; // 16384 is what the standard mandates but recent gpu support 4 times more
const int maxTextureMappingInfoCount = maxUniformBlockSize / sizeof(TextureMappingInfo);// 65536 / 128 -> 512

} // anonymous

CubeSphere::CubeSphere(Qt3DCore::QNode *parent)
    : Qt3DCore::QEntity(parent)
    , m_sphereRenderer(new Qt3DRender::QGeometryRenderer(this))
    , m_cubeRenderer(new Qt3DRender::QGeometryRenderer(this))
    , m_sphereVertexPositionAttribute(new Qt3DRender::QAttribute(this))
    , m_cubeVertexPositionAttribute(new Qt3DRender::QAttribute(this))
    , m_cubeIndexAttribute(new Qt3DRender::QAttribute(this))
    , m_sphereVertexBuffer(new Qt3DRender::QBuffer(Qt3DRender::QBuffer::VertexBuffer, this))
    , m_cubeVertexBuffer(new Qt3DRender::QBuffer(Qt3DRender::QBuffer::VertexBuffer, this))
    , m_cubeIndexBuffer(new Qt3DRender::QBuffer(Qt3DRender::QBuffer::IndexBuffer, this))
    , m_satelliteUniformBuffer(new Qt3DRender::QBuffer(Qt3DRender::QBuffer::UniformBuffer, this))
    , m_elevationUniformBuffer(new Qt3DRender::QBuffer(Qt3DRender::QBuffer::UniformBuffer, this))
    , m_satelliteImagery(new Qt3DRender::QTexture2DArray(this))
    , m_elevation(new Qt3DRender::QTexture2DArray(this))
    , m_camera(nullptr)
    , m_baseLevel(1)
    , m_maxLevel(1)
    , m_tilesToRender(0)
    , m_imageryTilesCount(0)
    , m_elevationTilesCount(0)
    , m_updateElapsed(0.0f)
    , m_useImagery(false)
    , m_useElevation(false)
    , m_requiresUpdate(true)
    , m_imageEntriesForSatellite(maxTextureMappingInfoCount)
    , m_imageEntriesForElevation(maxTextureMappingInfoCount)
{
    m_sphereVertexBuffer->setUsage(Qt3DRender::QBuffer::DynamicDraw);
    m_cubeVertexBuffer->setUsage(Qt3DRender::QBuffer::DynamicDraw);
    m_cubeIndexBuffer->setUsage(Qt3DRender::QBuffer::DynamicDraw);
    m_satelliteUniformBuffer->setUsage(Qt3DRender::QBuffer::DynamicDraw);
    m_elevationUniformBuffer->setUsage(Qt3DRender::QBuffer::DynamicDraw);

    m_satelliteImagery->setLayers(2048);
    m_satelliteImagery->setGenerateMipMaps(false);
    m_satelliteImagery->setMagnificationFilter(Qt3DRender::QTexture2DArray::Linear);
    m_satelliteImagery->setMinificationFilter(Qt3DRender::QTexture2DArray::Linear);
    m_satelliteImagery->setWidth(256);
    m_satelliteImagery->setHeight(256);
    m_satelliteImagery->setWrapMode(Qt3DRender::QTextureWrapMode(Qt3DRender::QTextureWrapMode::Repeat));
    m_satelliteImagery->setFormat(Qt3DRender::QTexture2DArray::RGB8_UNorm);

    m_elevation->setLayers(2048);
    m_elevation->setGenerateMipMaps(false);
    m_elevation->setMagnificationFilter(Qt3DRender::QTexture2DArray::Linear);
    m_elevation->setMinificationFilter(Qt3DRender::QTexture2DArray::Linear);
    m_elevation->setWidth(256);
    m_elevation->setHeight(256);
    m_elevation->setFormat(Qt3DRender::QTexture2DArray::Automatic);

    // SPHERE
    {
        m_sphereVertexPositionAttribute->setName(QStringLiteral("vertexPositionRTE"));
        m_sphereVertexPositionAttribute->setVertexBaseType(Qt3DRender::QAttribute::Float);
        m_sphereVertexPositionAttribute->setAttributeType(Qt3DRender::QAttribute::VertexAttribute);
        m_sphereVertexPositionAttribute->setBuffer(m_sphereVertexBuffer);
        m_sphereVertexPositionAttribute->setVertexSize(4);
        m_sphereVertexPositionAttribute->setByteOffset(0);
        m_sphereVertexPositionAttribute->setByteStride(4 * sizeof(float));


        Qt3DRender::QGeometry *geometry = new Qt3DRender::QGeometry();
        geometry->addAttribute(m_sphereVertexPositionAttribute);

        m_sphereRenderer->setGeometry(geometry);
        m_sphereRenderer->setPrimitiveType(Qt3DRender::QGeometryRenderer::Patches);
        m_sphereRenderer->setVerticesPerPatch(4);
    }

    // CUBE
    {
        m_cubeVertexPositionAttribute->setName(QStringLiteral("vertexPosition"));
        m_cubeVertexPositionAttribute->setVertexBaseType(Qt3DRender::QAttribute::Float);
        m_cubeVertexPositionAttribute->setAttributeType(Qt3DRender::QAttribute::VertexAttribute);
        m_cubeVertexPositionAttribute->setBuffer(m_cubeVertexBuffer);
        m_cubeVertexPositionAttribute->setVertexSize(4);
        m_cubeVertexPositionAttribute->setByteOffset(0);
        m_cubeVertexPositionAttribute->setByteStride(4 * sizeof(float));

        m_cubeIndexAttribute->setBuffer(m_cubeIndexBuffer);
        m_cubeIndexAttribute->setAttributeType(Qt3DRender::QAttribute::IndexAttribute);
        m_cubeIndexAttribute->setVertexBaseType(Qt3DRender::QAttribute::UnsignedShort);

        Qt3DRender::QGeometry *geometry = new Qt3DRender::QGeometry();
        geometry->addAttribute(m_cubeVertexPositionAttribute);
        geometry->addAttribute(m_cubeIndexAttribute);

        m_cubeRenderer->setGeometry(geometry);
        m_cubeRenderer->setPrimitiveType(Qt3DRender::QGeometryRenderer::Triangles);
    }

    updateCube();
}

CubeSphere::~CubeSphere()
{
}

void CubeSphere::setBaseLevel(int baseLevel)
{
    if (m_baseLevel == baseLevel)
        return;
    m_baseLevel = baseLevel;
    emit baseLevelChanged();
    m_requiresUpdate |= true;
    updateCube();
}

void CubeSphere::setMaxLevel(int maxLevel)
{
    if (m_maxLevel == maxLevel)
        return;
    m_maxLevel = maxLevel;
    emit maxLevelChanged();
    m_requiresUpdate |= true;
    updateCube();
}

void CubeSphere::setUseImagery(bool use)
{
    if (use == m_useImagery)
        return;
    m_useImagery = use;
    emit useImageryChanged();
    m_requiresUpdate |= true;
    updateCube();
}

void CubeSphere::setUseElevation(bool use)
{
    if (use == m_useElevation)
        return;
    m_useElevation = use;
    emit useElevationChanged();
    m_requiresUpdate |= true;
    updateCube();
}

void CubeSphere::setCamera(Qt3DRender::QCamera *camera)
{
    if (m_camera == camera)
        return;
    if (m_camera)
        QObject::disconnect(m_camera, &Qt3DRender::QCamera::positionChanged, this, &CubeSphere::updateCube);
    m_camera = camera;
    if (m_camera)
        QObject::connect(m_camera, &Qt3DRender::QCamera::positionChanged, this, &CubeSphere::updateCube);
    m_requiresUpdate |= true;
    updateCube();
    emit cameraChanged(camera);
}

void CubeSphere::setViewportSize(QSize viewportSize)
{
    if (m_viewportSize == viewportSize)
        return;

    m_viewportSize = viewportSize;
    emit viewportSizeChanged(viewportSize);
}

void CubeSphere::updateCube()
{
    if (m_camera == nullptr)
        return;

    // Since updating could be costly, we only update if we have to:
    // e.g camera has changed or viewport has changed
    //////// DO WE NEED TO REALLY UPDATE ? .//////////

    if (m_cubeBuilder.isNull()) {
        m_cubeBuilder.reset(new CubeBuilder(m_baseLevel, m_maxLevel, m_camera));
        m_requiresUpdate |= true;
    }

    if (m_cubeBuilder->camera() != m_camera) {
        m_cubeBuilder->setCamera(m_camera);
    } else {
        // If viewMatrix or projectionMatrix have check
        m_requiresUpdate |= m_cubeBuilder->viewProjectionMatrix() != (m_camera->projectionMatrix() * m_camera->viewMatrix());
    }

    if (m_cubeBuilder->viewportSize() != m_viewportSize)
        m_cubeBuilder->setViewportSize(m_viewportSize);

    if (m_baseLevel != m_cubeBuilder->baseLevel())
        m_cubeBuilder->setBaseLevel(m_baseLevel);

    if (m_maxLevel != m_cubeBuilder->maxLevel())
        m_cubeBuilder->setMaxLevel(m_maxLevel);

    // Do we need to update
    if (!m_requiresUpdate)
        return;
    m_requiresUpdate = false;

    QElapsedTimer t;
    t.start();

    ///////////  UPDATE THE CUBE /////////////

    // Update the quadtree
    m_cubeBuilder->update();

    const quint64 updateElapsed = t.nsecsElapsed();
    qDebug() << Q_FUNC_INFO << "QuadTree update in " << updateElapsed * 0.000001 << "ms";

    /////////// BUILD UBO WITH TEXTURE INFO AND UPDATE TEXTURE ARRAY //////////////////

    // Build UBO/SSBO here at the same time
    // We have one mapping for satellite imagery
    // One mapping for elevation imagery
    // Perform UBO update for Satellite Imagery
    m_imageryTilesCount = 0;
    m_elevationTilesCount = 0;

    if (m_useImagery) {
        QByteArray uboData;
        uboData.resize(maxUniformBlockSize);
        TextureMappingInfo *textureMappings = reinterpret_cast<TextureMappingInfo *>(uboData.data());
        const QVector<TileImageryMapping> mappings = m_cubeBuilder->tileImageMappings(ImageryTileProvider::Satellite);

        // 1 Tile Imagery Mapping contains up to 4 ImageryMappings
        // Each ImageryMapping contains
        // offsetXY to textureCoords
        // extentXY
        // scaleXY
        // textureLayer
        for (int i = 0, m = std::min(maxTextureMappingInfoCount, mappings.size()); i < m; ++i) {
            const TileImageryMapping &mapping = mappings.at(i);
            for (int j = 0; j < 4; ++j) {
                const ImageryMapping &vertexMapping = mapping.mappingEntries[j];
                if (vertexMapping.zoom == -1) {
                     textureMappings[i].layerScaleOffset[j][0] = -1.0f;
                } else {
                    const float layer = satelliteTextureLayerFor(vertexMapping.zoom,
                                                                 vertexMapping.tileX,
                                                                 vertexMapping.tileY);
                    textureMappings[i].layerScaleOffset[j] = QVector4D(layer,
                                                                        vertexMapping.scale,
                                                                        vertexMapping.offsetX,
                                                                        vertexMapping.offsetY);
                    textureMappings[i].extentMinMax[j] = QVector4D(vertexMapping.extentXMin,
                                                                   vertexMapping.extentYMin,
                                                                   vertexMapping.extentXMax,
                                                                   vertexMapping.extentYMax);
                    ++m_imageryTilesCount;
                }
            }
        }
        m_satelliteUniformBuffer->setData(uboData);
    }

    // Perform UBO update for Elevation Imagery
    if (m_useElevation) {
        QByteArray uboData;
        uboData.resize(maxUniformBlockSize);
        TextureMappingInfo *textureMappings = reinterpret_cast<TextureMappingInfo *>(uboData.data());
        const QVector<TileImageryMapping> mappings = m_cubeBuilder->tileImageMappings(ImageryTileProvider::Elevation);

        // 1 Tile Imagery Mapping contains up to 4 ImageryMappings
        // Each ImageryMapping contains
        // offsetXY to textureCoords
        // extentXY
        // scaleXY
        // textureLayer
        for (int i = 0, m = std::min(maxTextureMappingInfoCount, mappings.size()); i < m; ++i) {
            const TileImageryMapping &mapping = mappings.at(i);
            for (int j = 0; j < 4; ++j) {
                const ImageryMapping &vertexMapping = mapping.mappingEntries[j];
                if (vertexMapping.zoom == -1) {
                     textureMappings[i].layerScaleOffset[j][0] = -1.0f;
                } else {
                    const float layer = elevationTextureLayerFor(vertexMapping.zoom,
                                                                 vertexMapping.tileX,
                                                                 vertexMapping.tileY);
                    textureMappings[i].layerScaleOffset[j] = QVector4D(layer,
                                                                        vertexMapping.scale,
                                                                        vertexMapping.offsetX,
                                                                        vertexMapping.offsetY);
                    textureMappings[i].extentMinMax[j] = QVector4D(vertexMapping.extentXMin,
                                                                   vertexMapping.extentYMin,
                                                                   vertexMapping.extentXMax,
                                                                   vertexMapping.extentYMax);
                    ++m_elevationTilesCount;
                }
            }
        }
        m_elevationUniformBuffer->setData(uboData);
    }

    const quint64 textureUpdateElapsed = t.nsecsElapsed() - updateElapsed;
    qDebug() << Q_FUNC_INFO << "Texture updates in" << (textureUpdateElapsed) * 0.000001 << "ms";

    /////////// TRANSFER VERTICES ON GPU ///////////////

    // SPHERE VERTICES
    {
        // Copy vertex data to buffers
        const QVector<QDoubleVector3D> vertices = m_cubeBuilder->sphereVertices();
        const QDoubleVector3D eyePos = m_camera->position();
        QVector<QVector4D> verticesRelativeToEye(vertices.size());

        int i = 0;
        // We transform each world vertex so that it is relative to eye
        for (const QDoubleVector3D &vec3d : vertices) {
            const QDoubleVector3D rte = vec3d - eyePos;
            // We slightly abuse from the 4th component of the vertex
            // to store the vertex entry into the UBO
            const int indexIntoUBO = i / 4; // Same index for all the vertices of a same tile
            const QVector4D rteVertexData(rte.toQVector3D(), indexIntoUBO);
            verticesRelativeToEye[i++] = rteVertexData;
        }

        const quint64 vertexRTETransformElapsed = t.nsecsElapsed() - textureUpdateElapsed;
        qDebug() << Q_FUNC_INFO << "Convertion relative to eye in" << (vertexRTETransformElapsed) * 0.000001 << "ms";

        QByteArray vertexData;
        const int vertexBufferByteSize = vertices.size() * sizeof(QVector4D);
        vertexData.resize(vertexBufferByteSize);
        memcpy(vertexData.data(), verticesRelativeToEye.constData(), vertexBufferByteSize);

        m_sphereVertexBuffer->setData(vertexData);
        // Each tile has 4 corners, tessellation shaders will convert that to 8x8 grids
        const int patchesCount = vertices.size() / 4;
        m_tilesToRender = patchesCount;
        m_sphereVertexPositionAttribute->setCount(vertices.size());
    }

    // CUBE_VERTICES
    {
        // Copy vertex data to buffers
        const QVector<QDoubleVector3D> vertices = m_cubeBuilder->cubeVertices();
        QVector<QVector4D> floatVertices(vertices.size());

        int i = 0;
        // We transform each world vertex so that it is relative to eye
        for (const QDoubleVector3D &vec3d : vertices) {
            // We slightly abuse from the 4th component of the vertex
            // to store the vertex entry into the UBO
            const int indexIntoUBO = i / 4; // Same index for all the vertices of a same tile
            const QVector4D vertex(vec3d.toQVector3D(), indexIntoUBO);
            floatVertices[i++] = vertex;
        }

        QByteArray vertexData;
        const int vertexBufferByteSize = vertices.size() * sizeof(QVector4D);
        vertexData.resize(vertexBufferByteSize);
        memcpy(vertexData.data(), floatVertices.constData(), vertexBufferByteSize);

        m_cubeVertexBuffer->setData(vertexData);
        // Each tile has 4 corners, tessellation shaders will convert that to 8x8 grids
        m_cubeVertexPositionAttribute->setCount(vertices.size());

        QByteArray indexData;
        const int indexCount = vertices.size() / 4 * 6;
        const int indexBufferByteSize = indexCount * sizeof(ushort);
        indexData.resize(indexBufferByteSize);
        ushort *indices = reinterpret_cast<ushort *>(indexData.data());

        for (int i = 0, c = 0, v = 0; i < vertices.size() / 4; ++i) {
            indices[c] = v;
            indices[c + 1] = v + 2;
            indices[c + 2] = v + 1;
            indices[c + 3] = v + 1;
            indices[c + 4] = v + 2;
            indices[c + 5] = v + 3;
            v += 4;
            c += 6;
        }

        m_cubeIndexBuffer->setData(indexData);
        m_cubeIndexAttribute->setCount(indexCount);
    }

    // Note: we don't use instancing but instead we rely on the tessellation shader
    // and to lookup values in UBOs

    // Update performance values for the UI
    m_updateElapsed = float(t.nsecsElapsed() * 0.000001);
    emit tilesToRenderChanged();
    emit imageryTilesCountChanged();
    emit elevationTilesCountChanged();
    emit updateElapsedChanged();

    // update imageEntry scores for the next time
    updateImageryScores();
}

void CubeSphere::updateImageryScores()
{
    for (ImageEntryToLayer &e : m_imageEntriesForSatellite)
        --e.score;
    for (ImageEntryToLayer &e : m_imageEntriesForElevation)
        --e.score;
}

int CubeSphere::satelliteTextureLayerFor(int zoom, quint64 x, quint64 y)
{
    int matchingIdx = -1;
    int nearestUnused = -1;
    int idx = 0;

    // Find if we have a matching layer
    for (const ImageEntryToLayer &e : m_imageEntriesForSatellite) {
        if (nearestUnused ==  -1 && e.score < 0)
            nearestUnused = idx;
        if (e.score >= 0 && e.tileX == x && e.tileY == y && e.zoom == zoom) {
            matchingIdx = idx;
            break;
        }
        ++idx;
    }

    // If we have a matching idx, use it
    if (matchingIdx != -1) {
        ImageEntryToLayer &e = m_imageEntriesForSatellite[matchingIdx];
        // Reset score
        e.score = 1;
        return e.layerId;
    }

    // Otherwise if we've found an unused block
    if (nearestUnused != -1 ) {
        // We are dealing with a block that was never used before
        ImageEntryToLayer &e = m_imageEntriesForSatellite[nearestUnused];
        if (e.layerId == -1) {
            // Try to create a new QTextureImage for the texture2DArray
            if (m_satelliteImagery->textureImages().size() + 1< maxLayerCount) {
                Qt3DRender::QTextureImage *img = new Qt3DRender::QTextureImage();
                const int layerId = m_satelliteImagery->textureImages().size();
                img->setLayer(layerId);
                e.layerId = layerId;
                m_satelliteImagery->addTextureImage(img);
            }
            else {
                return 0;
            }
        }

        e.score = 1;
        e.zoom = zoom;
        e.tileX = x;
        e.tileY = y;
        Qt3DRender::QTextureImage *img = static_cast<Qt3DRender::QTextureImage *>(m_satelliteImagery->textureImages().at(e.layerId));
        img->setSource(m_cubeBuilder->imageryUrls(ImageryTileProvider::Satellite, zoom, x, y));
        return e.layerId;
    }
    return 0;
}


int CubeSphere::elevationTextureLayerFor(int zoom, quint64 x, quint64 y)
{
    int matchingIdx = -1;
    int nearestUnused = -1;
    int idx = 0;

    // Find if we have a matching layer
    for (const ImageEntryToLayer &e : m_imageEntriesForElevation) {
        if (nearestUnused ==  -1 && e.score < 0)
            nearestUnused = idx;
        if (e.score >= 0 && e.tileX == x && e.tileY == y && e.zoom == zoom) {
            matchingIdx = idx;
            break;
        }
        ++idx;
    }

    // If we have a matching idx, use it
    if (matchingIdx != -1) {
        ImageEntryToLayer &e = m_imageEntriesForElevation[matchingIdx];
        // Reset score
        e.score = 1;
        return e.layerId;
    }

    // Otherwise if we've found an unused block
    if (nearestUnused != -1 ) {
        // We are dealing with a block that was never used before
        ImageEntryToLayer &e = m_imageEntriesForElevation[nearestUnused];
        if (e.layerId == -1) {
            // Try to create a new QTextureImage for the texture2DArray
            if (m_elevation->textureImages().size() + 1< maxLayerCount) {
                Qt3DRender::QTextureImage *img = new Qt3DRender::QTextureImage();
                const int layerId = m_elevation->textureImages().size();
                img->setLayer(layerId);
                e.layerId = layerId;
                m_elevation->addTextureImage(img);
            }
            else {
                return 0;
            }
        }

        e.score = 1;
        e.zoom = zoom;
        e.tileX = x;
        e.tileY = y;
        Qt3DRender::QTextureImage *img = static_cast<Qt3DRender::QTextureImage *>(m_elevation->textureImages().at(e.layerId));
        img->setSource(m_cubeBuilder->imageryUrls(ImageryTileProvider::Elevation, zoom, x, y));
        return e.layerId;
    }
    return 0;
}
