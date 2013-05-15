project(fali_close)

# Set sources
set(fali_closeSRC main.cpp )
link_libraries(${MEEGOTOUCH_LIBRARIES})

# Enable Qt-support
include(${QT_USE_FILE})

add_executable(fali_close ${fali_closeSRC})

# Install
install(PROGRAMS fali_close DESTINATION /usr/bin)
install(FILES com.nokia.fali_close.service DESTINATION /usr/share/dbus-1/services/)
install(FILES fali_close.desktop DESTINATION /usr/share/applications/)

