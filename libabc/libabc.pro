TEMPLATE = subdirs
SUBDIRS = src tests

!CONFIG(disable_python) {
    SUBDIRS += python
}
