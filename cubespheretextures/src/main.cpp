#include <QGuiApplication>
#include <QQuickView>
#include <QQmlEngine>
#include "cubesphere.h"

// QuadNode
// Attributes: Center Matrix
// Texture Arrays + Texture Coordinates

int main(int ac, char **av)
{
    QSurfaceFormat format = QSurfaceFormat::defaultFormat();
    format.setProfile(QSurfaceFormat::CoreProfile);
    format.setMajorVersion(4);
    format.setMinorVersion(3);
    format.setDepthBufferSize(32);
    QSurfaceFormat::setDefaultFormat(format);

    QGuiApplication app(ac, av);
    QQuickView view;

    qmlRegisterType<CubeSphere>("MyModule", 1, 0, "CubeSphere");

//    view.setSource(QUrl(QStringLiteral("qrc:/qml/mainOption1.qml")));
    view.setSource(QUrl(QStringLiteral("qrc:/qml/mainOption2.qml")));
    view.setResizeMode(QQuickView::SizeRootObjectToView);
    view.show();

    QSurfaceFormat f = view.format();
    qDebug() << f.depthBufferSize();

    return app.exec();
}
