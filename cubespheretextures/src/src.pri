SOURCES += \
    $$PWD/cubesphere.cpp \
    $$PWD/cubebuilder.cpp \
    $$PWD/earthellipsoid.cpp \
    $$PWD/mapzenimagerytileprovider.cpp

HEADERS += \
    $$PWD/cubesphere.h \
    $$PWD/cubebuilder.h \
    $$PWD/earthellipsoid.h \
    $$PWD/geodetic.h \
    $$PWD/quadnode.h \
    $$PWD/mapzenimagerytileprovider.h \
    $$PWD/imagerytileprovider.h

INCLUDEPATH += $$PWD

include(maths/maths.pri)
