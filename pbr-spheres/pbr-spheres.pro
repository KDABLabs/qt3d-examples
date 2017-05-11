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
    ../resources/wobbly-bridge.qrc

DISTFILES += \
    PbrSphereScene.qml \
    Environment.qml \
    Spheres.qml

