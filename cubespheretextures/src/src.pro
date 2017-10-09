TEMPLATE = app
TARGET = cubespheretextures

QT += 3dcore 3drender 3dextras 3dinput quick concurrent

SOURCES += \
    main.cpp

RESOURCES += \
    resources.qrc

include(src.pri)

