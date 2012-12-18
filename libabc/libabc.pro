TEMPLATE = subdirs
SUBDIRS = src include tests

!CONFIG(disable_python) {
    SUBDIRS += python
}
