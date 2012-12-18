TARGET = abc-uploader

QT += \
    gui

LIBABC = ../libabc/src

INCLUDEPATH += \
    $${LIBABC}

QMAKE_LIBDIR += \
    $${LIBABC}
QMAKE_RPATHDIR = $${QMAKE_LIBDIR}

LIBS += \
    -labc

SOURCES += \
    about-screen.cpp \
    main.cpp \
    status-screen.cpp \
    system-tray.cpp

HEADERS += \
    about-screen.h \
    debug.h \
    status-screen.h \
    system-tray.h

RESOURCES += \
    data/icons.qrc
