include(../../common-config.pri)

TEMPLATE = lib
TARGET = abc

CONFIG += \
    link_pkgconfig

PKGCONFIG += \
    libraw

LIBS += -L$${TOP_BUILD_DIR}/cfitsio -lcfitsio
INCLUDEPATH += $${TOP_SRC_DIR}/cfitsio

SOURCES += \
    calibration-set.cpp \
    configuration.cpp \
    image-set.cpp \
    image.cpp
