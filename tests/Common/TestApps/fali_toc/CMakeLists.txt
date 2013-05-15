# Set sources
set(TerminateOnCloseAppSRC fali_toc.cpp)

# Set moc headers
#set(TerminateOnCloseApp_MOC_HDRS)
# And finally this will run moc:
#qt4_wrap_cpp( TerminateOnCloseApp_MOC_SRC ${TerminateOnCloseApp_MOC_HDRS} )

link_libraries(${MEEGOTOUCH_LIBRARIES})

# Enable Qt-support
include(${QT_USE_FILE})

add_executable(fali_toc ${TerminateOnCloseAppSRC})

# Install
install(FILES com.nokia.fali_toc.service DESTINATION /usr/share/dbus-1/services/)
install(FILES com.nokia.fali_toc.service.sb DESTINATION /usr/share/applifed-testscripts/sbservice/)
install(FILES fali_toc.desktop DESTINATION /usr/share/applications)
install(PROGRAMS fali_toc DESTINATION /usr/bin)

