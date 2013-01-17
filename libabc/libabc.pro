TEMPLATE = subdirs
SUBDIRS = src include tests
CONFIG += ordered

!CONFIG(disable_python) {
    SUBDIRS += python
}
