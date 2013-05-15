set(APPLIFED ${CMAKE_HOME_DIRECTORY}/src)

# Set sources
set(SRC ut_prestarter.cpp ${APPLIFED}/cpuload.cpp ${APPLIFED}/configdata.cpp ${APPLIFED}/configparser.cpp ${APPLIFED}/prestarter.cpp ${APPLIFED}/prestartapp.cpp    
${APPLIFED}/logger.cpp ${APPLIFED}/systemstate.cpp ${APPLIFED}/x11util.cpp)

# Set moc headers
set(MOC_HDRS ut_prestarter.h ${APPLIFED}/prestarter.h ${APPLIFED}/systemstate.h)

set(QT_USE_QTTEST TRUE)
set(QT_USE_QTDBUS TRUE)
set(QT_USE_QTXML  TRUE)

# Run moc
qt4_wrap_cpp(MOC_SRC ${MOC_HDRS})

include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${APPLIFED})
link_libraries(${QT_QTCORE_LIBRARY} ${QT_QTGUI_LIBRARY} ${QT_QTDBUS_LIBRARY} ${QT_QTTEST_LIBRARY} ${CONTEXTSUBSCRIBER_LIBRARIES})

# Enable Qt-support
include(${QT_USE_FILE})
add_executable(ut_prestarter ${SRC} ${MOC_SRC})

# Install
install(PROGRAMS ut_prestarter DESTINATION /usr/share/applifed-tests/)

