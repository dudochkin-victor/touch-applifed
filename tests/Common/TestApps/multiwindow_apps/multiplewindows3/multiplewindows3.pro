TEMPLATE     = app
TARGET       = multiplewindows3
target.path  = /usr/bin
OBJECTS_DIR  = ./.obj
MOC_DIR      = ./.moc
DEPENDPATH  += $$INCLUDEPATH
CONFIG      += meegotouch
CONFIG      -= app_bundle
QT          += dbus

SOURCES += main.cpp \
    mainpage.cpp \
    multiplewindowsifadaptor.cpp \
    multiwindowservice.cpp \
    mywindow.cpp 

HEADERS += mainpage.h \
    multiplewindowsifadaptor.h \
    multiwindowservice.h \
    mywindow.h

services.target = .dummy
services.commands = touch \
    $$services.target
services.path = /usr/share/dbus-1/services
services.files = *.service

# Install instructions
INSTALLS += target \
    services 

