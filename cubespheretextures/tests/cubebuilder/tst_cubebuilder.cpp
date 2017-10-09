#include <QtTest/QTest>
#include <cubebuilder.h>
#include <mapzenimagerytileprovider.h>
#include <quadnode.h>
#include <Qt3DRender/QCamera>

#define EPSOLON

namespace {

class FakeImageProvider : public ImageryTileProvider
{
    // ImageryTileProvider interface
public:
    ImageryType type() const { return ImageryTileProvider::Satellite; }
    int maxSupportedZoomLevel() const Q_DECL_OVERRIDE { return 2; }
    QUrl imageryUrl(quint64 /*zoom*/, quint64 /*x*/, quint64 /*y*/) Q_DECL_OVERRIDE { return QUrl(); }
    QImage imageryTile(quint64 /*zoom*/, quint64 /*x*/, quint64 /*y*/) Q_DECL_OVERRIDE { return QImage(); }
};

} // anonymous

class tst_CubeBuilder : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void checkEllipsoidRadii()
    {
        // GIVEN
        const QDoubleVector3D earthRadii = EarthEllipsoid::wgs84Radii;

        // THEN
        QCOMPARE(earthRadii.x(), 6378137.0);
        QCOMPARE(earthRadii.y(), 6356752.3);
        QCOMPARE(earthRadii.z(), 6378137.0);
    }

    void checkGeodeticSurfaceNormalGeodectic()
    {
        {
            // GIVEN
            Geodetic2D geo(0.0, 0.0);

            // WHEN
            const QDoubleVector3D n = EarthEllipsoid::geodeticSurfaceNormal(geo);

            // THEN
            QVERIFY(fabs(n.x() - 1.0) < 1e-15);
            QVERIFY(fabs(n.y() - 0.0) < 1e-15);
            QVERIFY(fabs(n.z() - 0.0) < 1e-15);
        }
        {
            // GIVEN
            Geodetic2D geo(0.0, M_PI_2); // 0.0, DEG_TO_RAD(90.0)

            // WHEN
            const QDoubleVector3D n = EarthEllipsoid::geodeticSurfaceNormal(geo);

            // THEN
            QVERIFY(fabs(n.x() - 0.0) < 1e-15);
            QVERIFY(fabs(n.y() - 1.0) < 1e-15);
            QVERIFY(fabs(n.z() - 0.0) < 1e-15);
        }
        {
            // GIVEN
            Geodetic2D geo(M_PI_2, 0.0); // DEG_TO_RAD(90.0), 0.0;

            // WHEN
            const QDoubleVector3D n = EarthEllipsoid::geodeticSurfaceNormal(geo);
            qDebug() << n;

            // THEN
            QVERIFY(fabs(n.x() - 0.0) < 1e-15);
            QVERIFY(fabs(n.y() - 0.0) < 1e-15);
            QVERIFY(fabs(n.z() - 1.0) < 1e-15);
        }
    }

    void checkGeodeticSurfaceNormalWGS84()
    {
        {
            // GIVEN
            QDoubleVector3D wgs84(EarthEllipsoid::wgs84Radii.x(), 0.0, 0.0);

            // WHEN
            const QDoubleVector3D n = EarthEllipsoid::geodeticSurfaceNormal(wgs84);

            // THEN
            QCOMPARE(n.x(), 1.0);
            QCOMPARE(n.y(), 0.0);
            QCOMPARE(n.z(), 0.0);
        }
        {
            // GIVEN
            QDoubleVector3D wgs84(0.0, EarthEllipsoid::wgs84Radii.y(), 0.0);

            // WHEN
            const QDoubleVector3D n = EarthEllipsoid::geodeticSurfaceNormal(wgs84);


            // THEN
            QCOMPARE(n.x(), 0.0);
            QCOMPARE(n.y(), 1.0);
            QCOMPARE(n.z(), 0.0);
        }
        {
            // GIVEN
            QDoubleVector3D wgs84(0.0, 0.0, EarthEllipsoid::wgs84Radii.z());

            // WHEN
            const QDoubleVector3D n = EarthEllipsoid::geodeticSurfaceNormal(wgs84);


            // THEN
            QCOMPARE(n.x(), 0.0);
            QCOMPARE(n.y(), 0.0);
            QCOMPARE(n.z(), 1.0);
        }
    }

    void checkBuildVertexBuffer()
    {
        // GIVEN
        Qt3DRender::QCamera camera;
        CubeBuilder builder(2, 2, &camera);
        builder.update();

        // WHEN
        CubeBuilder::CubeFace *face = &builder.m_cube.faces[0];
        builder.buildGPUResources(face);

        // THEN
        const QVector<QDoubleVector3D> vertices = builder.sphereVertices();
        //        QCOMPARE(vertices.size(), 16);
    }

    void checkSlippyTileId()
    {
        // GIVEN
        QuadNode node(nullptr, 1);
        node.coords[0] = {};
        node.coords[1] = {};
        node.coords[2] = {};
        node.coords[3] = {};

        // WHEN
        const QPair<double, double> nwTd = CubeBuilder::slippyTileId(node.coords[0].lon, node.coords[0].lat, node.m_level);
        const QPair<double, double> neTd = CubeBuilder::slippyTileId(node.coords[1].lon, node.coords[1].lat, node.m_level);
        const QPair<double, double> swTd = CubeBuilder::slippyTileId(node.coords[2].lon, node.coords[2].lat, node.m_level);
        const QPair<double, double> seTd = CubeBuilder::slippyTileId(node.coords[3].lon, node.coords[3].lat, node.m_level);

        // THEN


    }

    void checkImageDataForNode()
    {
        {
            // GIVEN
            FakeImageProvider imageProvider;
            QuadNode node(nullptr, 1);
            node.vertices[0] = QVector3D(-1.0f, 1.0f, 1.0f);
            node.vertices[1] = QVector3D(-1.0f, -1.0, 1.0f);
            node.vertices[2] = QVector3D(1.0f, 1.0f, 1.0f);
            node.vertices[3] = QVector3D(1.0f, -1.0f, 1.0f);
            node.generateSphereVerticesAndGeoCoordinates();

            qDebug() << RAD_TO_DEG(node.coords[0].lon) << RAD_TO_DEG(node.coords[0].lat);
            qDebug() << RAD_TO_DEG(node.coords[1].lon) << RAD_TO_DEG(node.coords[1].lat);
            qDebug() << RAD_TO_DEG(node.coords[2].lon) << RAD_TO_DEG(node.coords[2].lat);
            qDebug() << RAD_TO_DEG(node.coords[3].lon) << RAD_TO_DEG(node.coords[3].lat);

            // WHEN
            const TileImageryMapping renderData = CubeBuilder::imageDataForNode(&node, &imageProvider);

            // THEN

        }
    }

};

//QTEST_APPLESS_MAIN(tst_CubeBuilder)
QTEST_MAIN(tst_CubeBuilder)

#include "tst_cubebuilder.moc"
