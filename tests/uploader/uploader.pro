TARGET = uploader-test

QT += \
    network \
    sql \
    testlib

CONFIG += \
    link_pkgconfig

PKGCONFIG += \
    QJson

LIBABC = ../../libabc
SRC = ../../uploader

INCLUDEPATH += \
    mock \
    $${SRC} \
    $${LIBABC}/include \
    $${LIBABC}/src

DEPENDPATH += \
    mock

DEFINES += PROJECT_VERSION=\\\"1.0\\\"

SOURCES += \
    $${SRC}/application.cpp \
    $${SRC}/configuration.cpp \
    $${SRC}/file-log.cpp \
    $${SRC}/file-monitor.cpp \
    $${SRC}/updater.cpp \
    $${SRC}/upload-queue.cpp \
    uploader-test.cpp

HEADERS += \
    $${SRC}/application.h \
    $${SRC}/configuration.h \
    $${SRC}/file-log.h \
    $${SRC}/file-monitor.h \
    $${SRC}/updater.h \
    $${SRC}/upload-queue.h \
    mock/site.h \
    mock/upload-item.h \
    uploader-test.h

check.commands = ./uploader-test
check.depends = uploader-test
QMAKE_EXTRA_TARGETS += check
