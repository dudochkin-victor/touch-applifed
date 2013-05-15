project(close-event)

# Set sources
set(close-eventSRC main.cpp)

link_libraries(${MEEGOTOUCH_LIBRARIES})

# Enable Qt-support
include(${QT_USE_FILE})

add_executable(close-event ${close-eventSRC})

# Install
install(PROGRAMS close-event DESTINATION /usr/bin)

