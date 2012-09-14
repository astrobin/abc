PROJECT_NAME = abc
PROJECT_VERSION = 1.0

TOP_SRC_DIR = $$PWD
TOP_BUILD_DIR = $${TOP_SRC_DIR}/$(BUILD_DIR)

INSTALL_PREFIX = /usr

isEmpty(PREFIX) {
    message("====")
    message("==== NOTE: To override the installation path run: `qmake PREFIX=/custom/path'")
    message("==== (current installation path is `$${INSTALL_PREFIX}')")
} else {
    INSTALL_PREFIX = $${PREFIX}
    message("====")
    message("==== install prefix set to `$${INSTALL_PREFIX}'")
}
