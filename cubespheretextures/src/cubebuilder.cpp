#include "cubebuilder.h"
#include "quadnode.h"
#include "mapzenimagerytileprovider.h"
#include <Qt3DRender/QCamera>
#include <QtGui/QVector2D>
#include <QtConcurrent/QtConcurrent>


CubeBuilder::CubeBuilder(const int baseLevel,
                         const int maxLevel,
                         Qt3DRender::QCamera *camera)
    : m_cullPlanes(6)
    , m_baseLevel(baseLevel)
    , m_maxLevel(maxLevel)
    , m_camera(camera)
{
    // Register tile providers for elevation and satellite imagery
    m_tileProviders.insert(ImageryTileProvider::Satellite, new MapzenSatelliteImageryTileProvider());
    m_tileProviders.insert(ImageryTileProvider::Elevation, new MapzenElevationTileProvider());

    // Extends from -1 - 1 on each axis
    for (int i = 0; i < 6; ++i) {
        CubeFace &face = m_cube.faces[i];
        face.node = new QuadNode();

        // Initialize root node on face
        switch (i) {
        case 0: { // Positive Z
            face.node->vertices[0] = QVector3D(-1.0f, 1.0f, 1.0f);
            face.node->vertices[1] = QVector3D(-1.0f, -1.0, 1.0f);
            face.node->vertices[2] = QVector3D(1.0f, 1.0f, 1.0f);
            face.node->vertices[3] = QVector3D(1.0f, -1.0f, 1.0f);
            break;
        }
        case 1: { // Negative Z
            face.node->vertices[0] = QVector3D(1.0f, 1.0f, -1.0f);
            face.node->vertices[1] = QVector3D(1.0f, -1.0, -1.0f);
            face.node->vertices[2] = QVector3D(-1.0f, 1.0f, -1.0f);
            face.node->vertices[3] = QVector3D(-1.0f, -1.0f, -1.0f);
            break;
        }
        case 2: { // Positive X
            face.node->vertices[0] = QVector3D(1.0f, 1.0f, 1.0f);
            face.node->vertices[1] = QVector3D(1.0f, -1.0, 1.0f);
            face.node->vertices[2] = QVector3D(1.0f, 1.0f, -1.0f);
            face.node->vertices[3] = QVector3D(1.0f, -1.0f, -1.0f);
            break;
        }
        case 3: { // Negative X
            face.node->vertices[0] = QVector3D(-1.0f, 1.0f, -1.0f);
            face.node->vertices[1] = QVector3D(-1.0f, -1.0, -1.0f);
            face.node->vertices[2] = QVector3D(-1.0f, 1.0f, 1.0f);
            face.node->vertices[3] = QVector3D(-1.0f, -1.0f, 1.0f);
            break;
        }
        case 4: { // Positive Y
            face.node->vertices[0] = QVector3D(1.0f, 1.0f, 1.0f);
            face.node->vertices[1] = QVector3D(1.0f, 1.0, -1.0f);
            face.node->vertices[2] = QVector3D(-1.0f, 1.0f, 1.0f);
            face.node->vertices[3] = QVector3D(-1.0f, 1.0f, -1.0f);
            break;
        }
        case 5: { // Negative Y
            face.node->vertices[0] = QVector3D(-1.0f, -1.0f, 1.0f);
            face.node->vertices[1] = QVector3D(-1.0f, -1.0, -1.0f);
            face.node->vertices[2] = QVector3D(1.0f, -1.0f, 1.0f);
            face.node->vertices[3] = QVector3D(1.0f, -1.0f, -1.0f);
            break;
        }
        }
        face.node->generateSphereVerticesAndGeoCoordinates();
    }
}

CubeBuilder::~CubeBuilder()
{
    qDeleteAll(m_tileProviders);
}

void CubeBuilder::update()
{
    // Update frustum culling planes
    m_viewProjection = m_camera->projectionMatrix() * m_camera->viewMatrix();
    m_cullPlanes[0] = Plane(m_viewProjection.row(3) + m_viewProjection.row(0)); // Left
    m_cullPlanes[1] = Plane(m_viewProjection.row(3) - m_viewProjection.row(0)); // Right
    m_cullPlanes[2] = Plane(m_viewProjection.row(3) + m_viewProjection.row(1)); // Top
    m_cullPlanes[3] = Plane(m_viewProjection.row(3) - m_viewProjection.row(1)); // Bottom
    m_cullPlanes[4] = Plane(m_viewProjection.row(3) + m_viewProjection.row(2)); // Front
    m_cullPlanes[5] = Plane(m_viewProjection.row(3) - m_viewProjection.row(2)); // Back

    // Perform QuadNode subdivision in parallel
    QtConcurrent::blockingMap(std::begin(m_cube.faces), std::end(m_cube.faces), [this] (CubeFace &face)
    {

//            for (int i = 0; i < 1; ++i) {
//                CubeFace &face = m_cube.faces[i];
        // Subdivide the node until the desired subdivision level is reached
        subDivideQuadFace(&face);

        //        qDebug() << "Face";
//                qDebug() << RAD_TO_DEG(face.node->coords[0].lon) << RAD_TO_DEG(face.node->coords[0].lat);
//                qDebug() << RAD_TO_DEG(face.node->coords[1].lon) << RAD_TO_DEG(face.node->coords[1].lat);
//                qDebug() << RAD_TO_DEG(face.node->coords[2].lon) << RAD_TO_DEG(face.node->coords[2].lat);
//                qDebug() << RAD_TO_DEG(face.node->coords[3].lon) << RAD_TO_DEG(face.node->coords[3].lat);

        // Build buffer of vertices and ImageEntries for each face
        buildGPUResources(&face);
//            }
        //    qDebug() << "-----";
    });
}

QUrl CubeBuilder::imageryUrls(ImageryTileProvider::ImageryType type, int zoom, quint64 x, quint64 y) const
{
    return m_tileProviders.value(type)->imageryUrl(zoom, x, y);
}

void CubeBuilder::subDivideQuadFace(CubeFace *face)
{
    // Clear leaves from previous frames
    face->m_leaves.clear();
    // Notes: we could imagine a better way of doing things
    // where we start based on the leaves we already have
    // and rebuild the faces from there
    // Easier said than done though
    subDivideQuadNodeHelper(face, face->node);
}


void CubeBuilder::subDivideQuadNodeHelper(CubeFace *face, QuadNode *node)
{
    // Reset node's visibility
    node->m_isToBeRendered = false;

    const bool shouldBeSplit = shouldNodeBeSplit(node);

    // If yes
    if (shouldBeSplit) {
        // We need to subdivide
        node->split();

        // Recurse
        subDivideQuadNodeHelper(face, node->nw);
        subDivideQuadNodeHelper(face, node->ne);
        subDivideQuadNodeHelper(face, node->sw);
        subDivideQuadNodeHelper(face, node->se);
    } else {
        // Destroy children otherwise
        if (!node->isLeaf())
            node->merge();

        // Add node as a leaf for the current face
        if (isFrustumCulled(node)) {
            face->m_leaves.push_back(node);
            node->m_isToBeRendered = true;
        } else {
            if (node->m_parent != nullptr && !node->m_parent->m_isToBeRendered) {
                face->m_leaves.push_back(node->m_parent);
                node->m_parent->m_isToBeRendered = true;
            }
        }
    }
}

bool CubeBuilder::shouldNodeBeSplit(QuadNode *node) const
{
    // Has our node reached the maximum level?
    if (node->m_level >= m_maxLevel)
        return false;

    // Do we have a valid viewport?
    if (m_viewportSize.isNull())
        return false;

    // Is our node one of the based level ones
    if (node->m_level < m_baseLevel) {
        if (node->m_level < 4)
            return true;
        return isFrustumCulled(node);
    }

    // Check distance to camera
    const QDoubleVector3D nw = node->sphereVertices[0];
    const QDoubleVector3D se = node->sphereVertices[3];

    const QDoubleVector3D cameraPosition = m_camera->position();
    const QDoubleVector3D nodeCenter = (nw + se) * 0.5;

    // In meters
    const double distanceToCamera = (nodeCenter - cameraPosition).length();

    // Distance from camera to earth center (0, 0, 0)
    const double maxDistance = (-cameraPosition).length();
    if (distanceToCamera > maxDistance)
        return false;

    // Performs fructum culling
    if (!isFrustumCulled(node))
        return false;

    // TO DO: Perform Horizon culling

    const QDoubleVector3D ne = node->sphereVertices[2];
    const QDoubleVector3D sw = node->sphereVertices[1];

    // We check if the face normal is visible from the camera viewVector
    // If the normal is pointing backward, the node is a back face of the earth
    const QDoubleVector3D faceNormal = QDoubleVector3D::crossProduct((ne - nw).normalized(),
                                                                     (sw - nw).normalized()).normalized();
    const double dot = QDoubleVector3D::dotProduct(faceNormal, m_camera->viewVector().normalized());
    if (dot < 0.0)
        return false;

    // Assuming all the previous step succeeded, we are now
    // going to project the edges of the node and check wether
    // the size onscreen of the quad is within our tolerance
    // (Adadptative LOD)

    // Check if node actually needs to be subdivided
    const QRect viewPort = QRect(0, 0, m_viewportSize.width(), m_viewportSize.height());
    const QVector3D nwPosScreenPos = nw.toQVector3D().project(m_camera->viewMatrix(),
                                                              m_camera->projectionMatrix(),
                                                              viewPort);
    const QVector3D swPosScreenPos =  sw.toQVector3D().project(m_camera->viewMatrix(),
                                                               m_camera->projectionMatrix(),
                                                               viewPort);
    const QVector3D nePosScreenPos =  ne.toQVector3D().project(m_camera->viewMatrix(),
                                                               m_camera->projectionMatrix(),
                                                               viewPort);
    const QVector3D sePosScreenPos =  se.toQVector3D().project(m_camera->viewMatrix(),
                                                               m_camera->projectionMatrix(),
                                                               viewPort);

    // If node is visible, check if it's size requires it to be subdivided further
    const double pixelDistancenWnE = (nwPosScreenPos - nePosScreenPos).lengthSquared();
    const double pixelDistancenWsW = (nwPosScreenPos - swPosScreenPos).lengthSquared();
    const double pixelDistancenEsE = (nePosScreenPos - sePosScreenPos).lengthSquared();
    const double pixelDistancesWsE = (swPosScreenPos - sePosScreenPos).lengthSquared();

    // Check if projected distance between two edge is bigger than 256 px
    const double targetDistance = 256.0 * 256.0; // Squared to simplify computations

    return (pixelDistancenWnE > targetDistance ||
            pixelDistancenWsW > targetDistance ||
            pixelDistancenEsE > targetDistance ||
            pixelDistancesWsE > targetDistance);
}

bool CubeBuilder::isFrustumCulled(QuadNode *node) const
{
    // Coarse bounding sphere
    const Sphere s(node);

    // Unrolled loop
    if (QDoubleVector3D::dotProduct(s.center(), m_cullPlanes[0].normal) + m_cullPlanes[0].d < -s.radius())
        return false;
    if (QDoubleVector3D::dotProduct(s.center(), m_cullPlanes[1].normal) + m_cullPlanes[1].d < -s.radius())
        return false;
    if (QDoubleVector3D::dotProduct(s.center(), m_cullPlanes[2].normal) + m_cullPlanes[2].d < -s.radius())
        return false;
    if (QDoubleVector3D::dotProduct(s.center(), m_cullPlanes[3].normal) + m_cullPlanes[3].d < -s.radius())
        return false;
    if (QDoubleVector3D::dotProduct(s.center(), m_cullPlanes[4].normal) + m_cullPlanes[4].d < -s.radius())
        return false;
    if (QDoubleVector3D::dotProduct(s.center(), m_cullPlanes[5].normal) + m_cullPlanes[5].d < -s.radius())
        return false;

    return true;
}

void CubeBuilder::buildGPUResources(CubeFace *face)
{
    // Note: each face stores its resources, so that we can perform this in parallel

    // Clear data from previous run
    QVector<QDoubleVector3D> &sphereVertices = face->m_sphereVertices;
    QVector<QDoubleVector3D> &cubeVertices = face->m_cubeVertices;
    sphereVertices.clear();
    cubeVertices.clear();
    sphereVertices.reserve(face->m_leaves.size() * 4);
    cubeVertices.reserve(face->m_leaves.size() * 4);

    for (ImageryTileProvider *provider : m_tileProviders.values()) {
        QVector<TileImageryMapping> &nodeRenderDataForProvider = face->m_nodeRenderDataPerProvider[provider->type()];
        nodeRenderDataForProvider.clear();
        nodeRenderDataForProvider.reserve(face->m_leaves.size());
    }

    // Append node vertices to face vertices
    for (QuadNode *node : qAsConst(face->m_leaves)) {
        sphereVertices.append(CubeBuilder::sphereVertexDataForNode(node));
        cubeVertices.append(CubeBuilder::cubeVertexDataForNode(node));
    }

    // Request ImageMappings for each Tile provider and for all nodes
    for (ImageryTileProvider *provider : m_tileProviders.values()) {
        QVector<TileImageryMapping> &nodeRenderDataForProvider = face->m_nodeRenderDataPerProvider[provider->type()];

        for (QuadNode *node : qAsConst(face->m_leaves)) {
            const TileImageryMapping renderData = CubeBuilder::imageDataForNode(node, provider);
            nodeRenderDataForProvider.push_back(renderData);
        }
    }
}

// Concatenates vertices from each face
QVector<QDoubleVector3D> CubeBuilder::sphereVertices() const
{
    quint32 totalSize = 0;

    for (const CubeFace &face : m_cube.faces)
        totalSize += face.m_sphereVertices.size();

    QVector<QDoubleVector3D> verts;
    verts.reserve(totalSize);
    for (const CubeFace &face : m_cube.faces)
        verts.append(face.m_sphereVertices);

    return verts;
}

QVector<QDoubleVector3D> CubeBuilder::cubeVertices() const
{
    quint32 totalSize = 0;

    for (const CubeFace &face : m_cube.faces)
        totalSize += face.m_cubeVertices.size();

    QVector<QDoubleVector3D> verts;
    verts.reserve(totalSize);
    for (const CubeFace &face : m_cube.faces)
        verts.append(face.m_cubeVertices);

    return verts;
}

// Concatenates vertex to tile mappings for a given type of tile provider for each face
QVector<TileImageryMapping> CubeBuilder::tileImageMappings(ImageryTileProvider::ImageryType type) const
{
    quint32 totalSize = 0;

    for (const CubeFace &face : m_cube.faces)
        totalSize += face.m_nodeRenderDataPerProvider[type].size();

    QVector<TileImageryMapping> mappings;
    mappings.reserve(totalSize);
    for (const CubeFace &face : m_cube.faces)
        mappings.append(face.m_nodeRenderDataPerProvider[type]);

    return mappings;
}

QVector<QDoubleVector3D> CubeBuilder::sphereVertexDataForNode(QuadNode *node)
{
    return { node->sphereVertices[0], node->sphereVertices[1], node->sphereVertices[2], node->sphereVertices[3] };
}

QVector<QDoubleVector3D> CubeBuilder::cubeVertexDataForNode(QuadNode *node)
{
    return { node->vertices[0], node->vertices[1], node->vertices[2], node->vertices[3] };
}

TileImageryMapping CubeBuilder::imageDataForNode(QuadNode *node, ImageryTileProvider *tileProvider)
{
    // Compute texture coordinates and load textures For that we assume
    // that each leaf has a visual size of closely 256 x 256 which implies
    // that in the worst case there needs to be 4 textures for a leaf (if
    // tile doesn't align properly to the leaf's edges)

    // Note: we use the node's level to match with the imagery provider's level

    double minLat = node->coords[0].lat;
    double minLon = node->coords[0].lon;
    double maxLat = node->coords[0].lat;
    double maxLon = node->coords[0].lon;

    const int layerMaxZoomLevel = tileProvider->maxSupportedZoomLevel();
    int tileLevel = std::min(node->m_level, layerMaxZoomLevel);

    // 1) We find the min and max lat and long
    // This gives us the area covered by the geometric tile

    // We repeat this process until we are sure there is no more than 4 imagery tiles covering our geometric tile
    quint64 minTileX = 0;
    quint64 maxTileX = 0;
    quint64 minTileY = 0;
    quint64 maxTileY = 0;
    int numTiles = 0;
    QPair<double, double> cornerTileIdsD[4];

    while (tileLevel > 0) {

        // Store corners tileID for zoom level
        for (int i = 0; i < 4; ++i)
            cornerTileIdsD[i] = slippyTileId(node->coords[i].lon, node->coords[i].lat, tileLevel);

        // TO DO: handle the case where we have min 1 and max 359 properly
        // Compute bounding rect
        for (int i = 1; i < 4; ++i) {
            const double lon = node->coords[i].lon;
            const double lat = node->coords[i].lat;

            minLat = std::min(minLat, lat);
            minLon = std::min(minLon, lon);
            maxLat = std::max(maxLat, lat);
            maxLon = std::max(maxLon, lon);
        }

        // 2) We convert the lat / long coordinates of the bounding rect to slippy tile names
        //    We compute number of tiles based on node's depth level
        const QPair<double, double> nwTd = slippyTileId(minLon, maxLat, tileLevel);
        const QPair<double, double> neTd = slippyTileId(maxLon, maxLat, tileLevel);
        const QPair<double, double> swTd = slippyTileId(minLon, minLat, tileLevel);
        const QPair<double, double> seTd = slippyTileId(maxLon, minLat, tileLevel);
        const QPair<quint64, quint64> nwT = flooredSlippyTileId(nwTd);
        const QPair<quint64, quint64> neT = flooredSlippyTileId(neTd);
        const QPair<quint64, quint64> swT = flooredSlippyTileId(swTd);
        const QPair<quint64, quint64> seT = flooredSlippyTileId(seTd);

        // 3) We compute the number of imagery tiles needed for the geometry tile based on the bounding rect tileIds
        minTileX = std::min(std::min(nwT.first, neT.first), std::min(swT.first, seT.first));
        maxTileX = std::max(std::max(nwT.first, neT.first), std::max(swT.first, seT.first));
        minTileY = std::min(std::min(nwT.second, neT.second), std::min(swT.second, seT.second));
        maxTileY = std::max(std::max(nwT.second, neT.second), std::max(swT.second, seT.second));

        // This allows us to know how many tiles are need to cover our geometric tile
        // Then we can use the difference between nwTd and nwT to compute the
        // offset between the start of the imagery tile and the geometric tile

        // If there is more than 4 tiles covering our grid, we try again at zoom level tileLevel - 1
        numTiles = (maxTileX - minTileX + 1) * (maxTileY - minTileY + 1);
        if (numTiles <= 4)
            break;

        --tileLevel;
    }


//    qDebug() << "Zoom" << tileLevel << "X [" << minTileX << " - " << maxTileX << "] Y [" << minTileY << " - " << maxTileY << "]" << "Img tiles for node:" << numTiles;
    Q_ASSERT(numTiles <= 4 && numTiles > 0);
    /*
                -------------------------
                |           |           |
                |           |           |
                |  4;4      |   5;4     |
                |      _____|_____      |               Shader struct (per geometric tile)
                |      |    |    |      |              - float layer[4]
                -------|---------|-------              - float scale[4]
                |      |    |    |      |              - vec2 offsets[4]
                |      |    |    |      |              - vec2 extentMin[4]
                |      |____|____|      |              - vec2 extentMax[4]
                |           |           |
                |  4;5      |   5;5     |
                -------------------------
                */

    TileImageryMapping renderData;
    int imageCount = 0;
    for (quint64 y = minTileY; y <= maxTileY; ++y) {
        for (quint64 x = minTileX; x <= maxTileX; ++x) {
            ImageryMapping &mappingEntry = renderData.mappingEntries[imageCount++];
            mappingEntry.zoom = tileLevel;
            mappingEntry.tileX = x;
            mappingEntry.tileY = y;
            mappingEntry.scale = std::max(tileLevel / node->m_level, 1);
            // Compute offset to Top Left corner (0, 0)
            mappingEntry.offsetX = cornerTileIdsD[0].first - x;
            mappingEntry.offsetY = cornerTileIdsD[0].second - y;
            mappingEntry.extentXMin = mappingEntry.offsetX;
            mappingEntry.extentYMin = mappingEntry.offsetY;
            mappingEntry.extentXMax = mappingEntry.offsetX + 1.0;
            mappingEntry.extentYMax = mappingEntry.offsetY;
//            qDebug() << "Zoom, X, Y (" << mappingEntry.zoom << "," << mappingEntry.tileX << "," << mappingEntry.tileY << ") scale: " << mappingEntry.scale << "offsetX" << mappingEntry.offsetX << "offsetY" << mappingEntry.offsetY << "extentXMin" << mappingEntry.extentXMin << "extentYMin"<< mappingEntry.extentYMin << "extentXMax"<< mappingEntry.extentXMax << "extentYMax"<< mappingEntry.extentYMax;
        }
    }
    return renderData;
}

QPair<double, double> CubeBuilder::slippyTileId(double lon, double lat, int zoom)
{
    // http://wiki.openstreetmap.org/wiki/Slippy_map_tilenames#Derivation_of_tile_names
    // Keep in mind web mercator projection doesn't allow latitude values above 85.0511

    // Reproject in Mercator projection -> EPSG:4326 to EPSG:3857
    static const double maxLat = atan(sinh(M_PI)) - 0.0001; //85.0511
    const double xMer = lon;
    const double boundedLat = std::min(std::max(-maxLat, lat), maxLat);
    const double yMer = asinh(tan(boundedLat));
    //            const double yMer = log(tan(lat) + 1.0 / (cos(lat)))

    // Transform to 0 - 1 range
    const double x = (1.0 + (xMer / M_PI)) / 2.0;
    const double y = (1.0 - (yMer / M_PI)) / 2.0;

    // imageryTileCountForZoom is the max number of images on the x or y axes (2 ^ zoom - 1)
    const int imageryTileCountForZoom = (1 << zoom) - 1; //pow(2, node->m_level) - 1;
    return qMakePair(x * imageryTileCountForZoom, y * imageryTileCountForZoom);
}

CubeBuilder::Sphere::Sphere(QuadNode *node)
{
    const QDoubleVector3D nw = node->sphereVertices[0];
    const QDoubleVector3D sw = node->sphereVertices[1];
    const QDoubleVector3D ne = node->sphereVertices[2];
    const QDoubleVector3D se = node->sphereVertices[3];

    // centroid of quad
    m_center = ((nw + se + ne + sw) * 0.25);
    m_radius = qMax((sw - ne).length(), (se - nw).length()) * 0.9;
}
