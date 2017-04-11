TEMPLATE = app

QT += 3dcore 3drender 3dinput 3danimation 3dextras 3dquick 3dlogic qml quick 3dquickextras 3dquickanimation
CONFIG += resources_big

RESOURCES += \
    main.qrc \
    ../resources/egg.qrc \
    ../resources/wobbly-bridge.qrc

SOURCES += \
    main.cpp

OTHER_FILES += \
    main.qml \
    DefaultSceneEntity.qml
