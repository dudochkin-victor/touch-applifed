project(HelloWorld)

include(FindPkgConfig) 
pkg_check_modules(MEEGOTOUCH meegotouch>=0.20 REQUIRED)
include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${MEEGOTOUCH_INCLUDE_DIRS})

# Set sources
set(HelloWorldSRC fali_multiapp.cpp)

# Set moc headers
#set(HelloWorld_MOC_HDRS)
# Run moc
#qt4_wrap_cpp(HelloWorld_MOC_SRC ${HelloWorld_MOC_HDRS})

link_libraries(${MEEGOTOUCH_LIBRARIES} ${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTDBUS_LIBRARY})

include(${QT_USE_FILE} "/usr/include/meegotouch" )
add_executable(fali_multiapp ${HelloWorldSRC} ${HelloWorld_MOC_SRC})

# Install
install(PROGRAMS fali_multiapp DESTINATION /usr/bin/)

