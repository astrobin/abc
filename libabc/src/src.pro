include(../../common-config.pri)

TEMPLATE = lib
TARGET = abc

LIBS += -L$${TOP_BUILD_DIR}/cfitsio -lcfitsio
INCLUDEPATH += $${TOP_SRC_DIR}/cfitsio

SOURCES += \
    calibration-set.cpp \
    configuration.cpp \
    image-set.cpp \
    image.cpp
