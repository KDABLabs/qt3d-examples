TEMPLATE = app

QT += 3dcore 3drender 3dinput 3dquick qml quick 3dquickextras
CONFIG += resources_big

HEADERS += \

SOURCES += \
    main.cpp

OTHER_FILES += \
    main.qml

RESOURCES += \
    main.qrc \
    ../resources/powerup.qrc \
    ../resources/wobbly-bridge.qrc

