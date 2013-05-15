# Set sources
set(SRC main.cpp 
        mainpage.cpp 
        multiplewindowsifadaptor.cpp 
        multiwindowapplication.cpp
        multiwindowservice.cpp)

# Set moc headers
set(MOC_HDRS mainpage.h multiplewindowsifadaptor.h multiwindowapplication.h multiwindowservice.h)
qt4_wrap_cpp(MOC_SRC ${MOC_HDRS})

# By default only QtCore and QtGui modules are enabled
set(QT_USE_QTDBUS TRUE)
link_libraries(${MEEGOTOUCH_LIBRARIES})

# Enable Qt-support
include(${QT_USE_FILE})

set(NAME multiplewindows)
add_executable(${NAME} ${SRC} ${MOC_SRC})

# Install
install(FILES com.nokia.fali_mw1.service DESTINATION /usr/share/dbus-1/services/)
install(PROGRAMS ${NAME} DESTINATION /usr/bin RENAME fali_mw1)

install(FILES com.nokia.fali_mw2.service DESTINATION /usr/share/dbus-1/services/)
install(PROGRAMS ${NAME} DESTINATION /usr/bin RENAME fali_mw2)
