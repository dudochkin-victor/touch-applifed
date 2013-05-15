set(APPLIFED ${CMAKE_HOME_DIRECTORY}/src)

# Set sources
set(SRC ut_cpuload.cpp ${APPLIFED}/cpuload.cpp)

# Set moc headers
set(MOC_HDRS ut_cpuload.h)
set(QT_USE_QTTEST TRUE)

# Run moc
qt4_wrap_cpp(MOC_SRC ${MOC_HDRS})

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${APPLIFED})
link_libraries(${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTDBUS_LIBRARY} ${QT_QTTEST_LIBRARY})

# Enable Qt-support
include(${QT_USE_FILE})
add_executable(ut_cpuload ${SRC} ${MOC_SRC})

# Install
install(PROGRAMS ut_cpuload DESTINATION /usr/share/applifed-tests/)

