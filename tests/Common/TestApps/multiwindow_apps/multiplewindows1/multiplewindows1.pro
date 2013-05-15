TEMPLATE     = app
TARGET       = multiplewindows1
target.path  = /usr/bin
OBJECTS_DIR  = ./.obj
MOC_DIR      = ./.moc
DEPENDPATH  += $$INCLUDEPATH
CONFIG      -= app_bundle
CONFIG      += meegotouch
QT          += dbus

SOURCES += main.cpp \
    mainpage.cpp \
    multiplewindowsifadaptor.cpp \
    multiwindowservice.cpp \
    multiwindowapplication.cpp

HEADERS += mainpage.h \
    multiplewindowsifadaptor.h \
    multiwindowservice.h

services.target = .dummy
services.commands = touch \
    $$services.target
services.path = /usr/share/dbus-1/services
services.files = *.service

# Install instructions
INSTALLS += target \
    services 

