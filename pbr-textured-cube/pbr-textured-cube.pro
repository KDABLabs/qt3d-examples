TEMPLATE = app

QT += quick
CONFIG += resources_big

HEADERS += \

SOURCES += \
    main.cpp

OTHER_FILES += \
    main.qml

RESOURCES += \
    main.qrc \
    ../resources/wobbly-bridge.qrc \
    ../resources/meshes.qrc \
    ../resources/pbr-default-textures.qrc

DISTFILES += \
    PbrCubeScene.qml \
    Environment.qml

