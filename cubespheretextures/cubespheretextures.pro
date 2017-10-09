TEMPLATE = subdirs

src_app.subdir = $$PWD/src
src_app.target = sub-app

src_tests.subdir = $$PWD/tests
src_tests.target = sub-smart-pss
src_tests.depends = src_app

SUBDIRS += \
    src_app \
    src_tests
