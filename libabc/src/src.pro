include(../../common-config.pri)

TEMPLATE = lib
TARGET = abc

QT += network

CONFIG += \
    link_pkgconfig

PKGCONFIG += \
    QJson \
    libraw

LIBS += -L$${TOP_BUILD_DIR}/cfitsio -lcfitsio
INCLUDEPATH += $${TOP_SRC_DIR}/cfitsio

SOURCES += \
    calibration-set.cpp \
    configuration.cpp \
    image-set.cpp \
    image.cpp \
    site.cpp \
    upload-item.cpp

HEADERS += \
    site.h \
    upload-item.h

headers.files = \
    calibration-set.h \
    image-set.h \
    image.h \
    site.h \
    upload-item.h

headers.path = $${INSTALL_PREFIX}/include/ABC/
INSTALLS += headers

target.path = $${INSTALL_LIBDIR}
INSTALLS += target
