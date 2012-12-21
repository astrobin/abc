TARGET = uploader-test

QT += \
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
    $${SRC}/file-monitor.cpp \
    uploader-test.cpp

HEADERS += \
    $${SRC}/file-monitor.h \
    uploader-test.h

check.commands = ./uploader-test
check.depends = uploader-test
QMAKE_EXTRA_TARGETS += check
