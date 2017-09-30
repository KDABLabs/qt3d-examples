TEMPLATE = app
CONFIG += resources_big

QT += 3dcore 3drender 3dinput 3dextras 3dquick qml quick 3dquickextras 3danimation

SOURCES += \
    main.cpp

OTHER_FILES += \
    main.qml \
    DefaultSceneEntity.qml \
    SkinnedEntity.qml \
    SkinnedPbrEffect.qml \
    SimpleEntity.qml \
    AnimatedEntity.qml \
    skinnedPbr.vert

RESOURCES += \
    skinned-mesh.qrc \
    ../resources/cedar-bridge.qrc \
    ../resources/gltf.qrc
