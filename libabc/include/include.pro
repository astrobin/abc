include(../../common-config.pri)

TEMPLATE = subdirs

headers.files = \
    ABC/CalibrationSet \
    ABC/ImageSet \
    ABC/Image \
    ABC/Site \
    ABC/UploadItem

headers.path = $${INSTALL_PREFIX}/include/ABC/
INSTALLS += headers
