TEMPLATE = app
CONFIG += resources_big

QT += 3dcore 3drender 3dinput 3dextras 3dquick qml quick 3dquickextras 3danimation widgets

SOURCES += \
    main.cpp \
    scenecontroller.cpp \
    controllerwidget.cpp

OTHER_FILES += \
    main.qml \
    DefaultSceneEntity.qml \
    SkinnedEntity.qml \
    SkinnedPbrEffect.qml \
    AnimatedEntity.qml \
    skinnedPbr.vert

RESOURCES += \
    blended-skinned-animation.qrc \
    ../resources/cedar-bridge.qrc \
    ../resources/gltf.qrc

HEADERS += \
    scenecontroller.h \
    controllerwidget.h

FORMS += \
    controllerwidget.ui
