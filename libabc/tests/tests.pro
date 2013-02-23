TARGET = abc-test

QT += \
    network \
    testlib

SRC = ../src

INCLUDEPATH += \
    $${SRC}

Debug: OBJECTS_DIR=debug
Release: OBJECTS_DIR=release

QMAKE_LIBDIR += \
    $${SRC}/$${OBJECTS_DIR}
QMAKE_RPATHDIR = $${QMAKE_LIBDIR}

LIBS += \
    -labc

SOURCES += \
    abc-test.cpp

HEADERS += \
    abc-test.h

check.commands = ./abc-test
check.depends = abc-test
QMAKE_EXTRA_TARGETS += check
