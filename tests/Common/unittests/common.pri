APPLIFEDSRCDIR = ../../../src
QT += testlib
TEMPLATE = app
INCLUDEPATH += . $$APPLIFEDSRCDIR
DEPENDPATH += $$INCLUDEPATH
LIBS += -lgcov
QMAKE_CXXFLAGS += -ftest-coverage -fprofile-arcs
QMAKE_CLEAN += \
    *.gcda \
    *.gcno \
