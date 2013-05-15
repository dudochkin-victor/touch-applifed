# Set sources
set(SRC main.cpp 
        mainpage.cpp 
        mywindow.cpp 
        multiplewindowsifadaptor.cpp 
        multiwindowservice.cpp)

# Set moc headers
set(MOC_HDRS mainpage.h mywindow.h multiplewindowsifadaptor.h multiwindowservice.h)
# And finally this will run moc:
qt4_wrap_cpp(MOC_SRC ${MOC_HDRS})

link_libraries(${MEEGOTOUCH_LIBRARIES})

# Enable Qt-support
include(${QT_USE_FILE})

set(NAME fali_mw3)
add_executable(${NAME} ${SRC} ${MOC_SRC})

# Install
install(FILES com.nokia.fali_mw3.service DESTINATION /usr/share/dbus-1/services/)
# Are these needed?
#install(FILES multi1.desktop multi2.desktop multi3.desktop DESTINATION /usr/share/applications)
install(PROGRAMS ${NAME} DESTINATION /usr/bin)

