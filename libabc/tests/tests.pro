TARGET = abc-test

QT += \
    testlib

SRC = ../src

INCLUDEPATH += \
    $${SRC}

QMAKE_LIBDIR += \
    $${SRC}
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
