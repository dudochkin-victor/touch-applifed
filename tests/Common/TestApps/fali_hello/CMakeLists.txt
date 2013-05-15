include(FindPkgConfig) 
pkg_check_modules(MEEGOTOUCH meegotouch>=0.20 REQUIRED)
include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${MEEGOTOUCH_INCLUDE_DIRS})

project(HelloWorld)

# Set sources
set(HelloWorldSRC fali_hello.cpp)

# Set moc headers
#set(HelloWorld_MOC_HDRS)
# Run moc
#qt4_wrap_cpp(HelloWorld_MOC_SRC ${HelloWorld_MOC_HDRS})

link_libraries(${MEEGOTOUCH_LIBRARIES} ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTDBUS_LIBRARY})

include(${QT_USE_FILE})

add_executable(fali_hello ${HelloWorldSRC} ${HelloWorld_MOC_SRC})


# Install
install(PROGRAMS fali_hello DESTINATION /usr/bin/)
install(PROGRAMS fali_hello DESTINATION /usr/bin/ RENAME fali_hello1)
install(PROGRAMS fali_hello DESTINATION /usr/bin/ RENAME fali_perf)

install(FILES com.nokia.fali_hello.service DESTINATION /usr/share/dbus-1/services/)
install(FILES com.nokia.fali_hello1.service DESTINATION /usr/share/dbus-1/services/)
install(FILES com.nokia.fali_perf.service DESTINATION /usr/share/dbus-1/services/)
install(FILES com.nokia.fali_hello.service.sb DESTINATION /usr/share/applifed-testscripts/sbservice/)
install(FILES fali_hello.desktop DESTINATION /usr/share/applications/)
install(FILES fali_hello1.desktop DESTINATION /usr/share/applications/)

