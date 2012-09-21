include(../../common-config.pri)

TEMPLATE = lib
TARGET = abc

LIBS += -L$${TOP_BUILD_DIR}/cfitsio -lcfitsio
INCLUDEPATH += $${TOP_SRC_DIR}/cfitsio

SOURCES += \
    image.cpp
