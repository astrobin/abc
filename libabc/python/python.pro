TEMPLATE = lib

CONFIG += \
    link_pkgconfig

CONFIG += plugin no_plugin_name_prefix

QT += core gui

INCLUDEPATH += ../src
LIBS += -L../src -labc

PKGCONFIG += \
    pyside \
    shiboken

PYSIDE_INC_ROOT = $$system(pkg-config --variable includedir pyside)
PYTHON_LIBDIR = $$system(python -c \"from distutils.sysconfig import get_python_lib; print (get_python_lib())\")
INCLUDEPATH += $${PYSIDE_INC_ROOT}/QtCore

QTROOT_INC=$$[QT_INSTALL_HEADERS]
QTCORE_INC=$${QTROOT_INC}/QtCore

system(shiboken --enable-pyside-extensions \
    --typesystem-paths=/usr/share/PySide/typesystems \
    --include-paths=../src:$${QTCORE_INC}:$${QTROOT_INC}:/usr/include \
    --output-directory=. \
    global.h typesystem_abc.xml)

TARGET = PyABC

SOURCES += \
    PyABC/pyabc_module_wrapper.cpp \
    PyABC/abc_image_wrapper.cpp \
    PyABC/abc_imageset_wrapper.cpp

target.path = $${PYTHON_LIBDIR}
INSTALLS += target

check.commands = "LD_LIBRARY_PATH=../src:$LD_LIBRARY_PATH ./abc-test.py"
QMAKE_EXTRA_TARGETS *= check
