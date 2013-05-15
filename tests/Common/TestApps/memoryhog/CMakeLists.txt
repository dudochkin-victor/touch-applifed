# Set sources
set(MemoryHogSRC memoryhog.c)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
add_executable(memoryhog ${MemoryHogSRC})

# Install
install(PROGRAMS memoryhog DESTINATION /usr/bin/)
