include(../../common-config.pri)

TEMPLATE = subdirs

headers.files = \
    ABC/CalibrationSet \
    ABC/ImageSet \
    ABC/Image

headers.path = $${INSTALL_PREFIX}/include/ABC/
INSTALLS += headers
