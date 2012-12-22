TARGET = abc-uploader

QT += \
    gui

LIBABC = ../libabc

INCLUDEPATH += \
    $${LIBABC}/include \
    $${LIBABC}/src

QMAKE_LIBDIR += \
    $${LIBABC}/src
QMAKE_RPATHDIR = $${QMAKE_LIBDIR}

LIBS += \
    -labc

SOURCES += \
    about-screen.cpp \
    application.cpp \
    config-screen.cpp \
    configuration.cpp \
    controller.cpp \
    file-monitor.cpp \
    main.cpp \
    status-screen.cpp \
    system-tray.cpp \
    upload-queue.cpp

HEADERS += \
    about-screen.h \
    application.h \
    config-screen.h \
    configuration.h \
    controller.h \
    debug.h \
    file-monitor.h \
    status-screen.h \
    system-tray.h \
    upload-queue.h

RESOURCES += \
    data/icons.qrc
