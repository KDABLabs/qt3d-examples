TEMPLATE = app

TARGET = tst_cubebuilder

QT += 3dcore 3drender concurrent testlib

CONFIG += testcase

SOURCES += \
    tst_cubebuilder.cpp

DEFINES += UNIT_TESTS

include(../../src/src.pri)
