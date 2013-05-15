# Set sources
set(SRC configdata.cpp configparser.cpp cpuload.cpp 
        daemon.cpp 
        logger.cpp 
        main.cpp 
        prestartapp.cpp prestarter.cpp systemstate.cpp 
        x11util.cpp)

# Set program name
add_definitions(-DPROG_NAME="applifed")

# Set config file path
add_definitions(-DCONFIG_FILE="/etc/prestart/nokia.conf")

# Set default log file path
# This can be overridden with --log
add_definitions(-DDEFAULT_LOG_FILE="/tmp/applifed.log")

# Set lock file (prevents running more than one instances of applifed)
add_definitions(-DLOCK_FILE="/tmp/applifed.lock")

# Set moc headers
set(MOC_HDRS daemon.h prestarter.h systemstate.h)

# By default only QtCore and QtGui modules are enabled
set(QT_USE_QTXML  TRUE)
set(QT_USE_QTDBUS TRUE)

# Run moc
qt4_wrap_cpp(MOC_SRC ${MOC_HDRS})

include_directories(${CMAKE_CURRENT_SOURCE_DIR})
link_libraries(${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTDBUS_LIBRARY} ${QT_XML_LIBRARY} ${GCOV} ${CONTEXTSUBSCRIBER_LIBRARIES})

# Enable Qt-support
include(${QT_USE_FILE})

# Define the executable
add_executable(applifed ${SRC} ${MOC_SRC})

# Install
install(PROGRAMS applifed DESTINATION /usr/bin/)

