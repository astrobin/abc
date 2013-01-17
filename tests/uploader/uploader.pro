TARGET = uploader-test

QT += \
    sql \
    testlib

LIBABC = ../../libabc
SRC = ../../uploader

INCLUDEPATH += \
    $${SRC} \
    $${LIBABC}/include \
    $${LIBABC}/src

QMAKE_LIBDIR += \
    $${LIBABC}/src
QMAKE_RPATHDIR = $${QMAKE_LIBDIR}

LIBS += \
    -labc

SOURCES += \
    $${SRC}/application.cpp \
    $${SRC}/configuration.cpp \
    $${SRC}/file-log.cpp \
    $${SRC}/file-monitor.cpp \
    $${SRC}/upload-queue.cpp \
    uploader-test.cpp

HEADERS += \
    $${SRC}/application.h \
    $${SRC}/configuration.h \
    $${SRC}/file-log.h \
    $${SRC}/file-monitor.h \
    $${SRC}/upload-queue.h \
    uploader-test.h

check.commands = ./uploader-test
check.depends = uploader-test
QMAKE_EXTRA_TARGETS += check
