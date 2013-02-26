include(../common-config.pri)

TARGET = abc-uploader

QT += \
    gui \
    network \
    sql

CONFIG += \
    link_pkgconfig

PKGCONFIG += \
    QJson

LIBABC = ../libabc

INCLUDEPATH += \
    $${LIBABC}/include \
    $${LIBABC}/src

QMAKE_LIBDIR += \
    $${LIBABC}/src/$${OBJECTS_DIR}
QMAKE_RPATHDIR = \
    $${QMAKE_LIBDIR} \
    $${INSTALL_PREFIX}/lib

LIBS += \
    -labc

DEFINES += PROJECT_VERSION=\\\"$${PROJECT_VERSION}\\\"

SOURCES += \
    about-screen.cpp \
    application.cpp \
    config-screen.cpp \
    configuration.cpp \
    controller.cpp \
    file-log.cpp \
    file-monitor.cpp \
    main.cpp \
    status-screen.cpp \
    system-tray.cpp \
    updater.cpp \
    upload-queue.cpp

HEADERS += \
    about-screen.h \
    application.h \
    config-screen.h \
    configuration.h \
    controller.h \
    debug.h \
    file-log.h \
    file-monitor.h \
    status-screen.h \
    system-tray.h \
    updater.h \
    upload-queue.h

RESOURCES += \
    data/icons.qrc

unix {
    QMAKE_SUBSTITUTES += \
        data/abc-uploader.desktop.in

    desktopfile.files = data/abc-uploader.desktop
    desktopfile.path = $${INSTALL_PREFIX}/share/applications
    INSTALLS += desktopfile

    icon.files = data/application-icons/abc-uploader.svg
    icon.path = $${INSTALL_PREFIX}/share/icons/hicolor/scalable/apps
    INSTALLS += icon

    target.path = $${INSTALL_PREFIX}/bin
    INSTALLS += target
}

win32 {
    RC_FILE = data/application-icons/abc-uploader.rc
}
