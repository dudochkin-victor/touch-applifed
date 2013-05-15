include(../common.pri)

MSRCINCLUDE = /usr/include/m

INCLUDEPATH += . \
    $$MSRCINCLUDE \

LIBS += -lm

TEMPLATE = app
TARGET = fali_toc
target.path = /usr/bin

DEPENDPATH += $$INCLUDEPATH

SOURCES += \
    fali_toc.cpp 

desktop_entry.path = /usr/share/applications
desktop_entry.files = fali_toc.desktop

services.target = .dummy
services.commands = touch $$services.target
services.path = /usr/share/dbus-1/services
services.files = \
    com.nokia.fali_toc.service 

# Install instructions
INSTALLS += \
    target \
    desktop_entry \
    services \

