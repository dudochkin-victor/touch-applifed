project(Applifed)

cmake_minimum_required(VERSION 2.6)
cmake_policy(VERSION 2.6)

set(CMAKE_VERBOSE_MAKEFILE ON)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -W -Wall -Wextra -g -O2")

# Find Qt4
find_package(Qt4 4.6.3 REQUIRED)

include(${QT_USE_FILE})
if (${QTVERSION} VERSION_LESS 4.6.3)
    message(FATAL_ERROR "You need Qt >= 4.6.3, found ${QTVERSION}.")
endif (${QTVERSION} VERSION_LESS 4.6.3)

# Find contextsubscriber that is used to notify about screen blanks
include(FindPkgConfig) 
pkg_check_modules(CONTEXTSUBSCRIBER contextsubscriber-1.0>=0.5.12 REQUIRED)
include_directories(${CONTEXTSUBSCRIBER_INCLUDE_DIRS})

# Enable coverage if COVERAGE environment variable is set
if ($ENV{COVERAGE})
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage -ftest-coverage -fprofile-arcs")
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage")
	link_libraries(gcov)
endif ($ENV{COVERAGE})

# Sub build: applifed
add_subdirectory(src)

# Build tests if BUILD_TESTS environment variable is set
if ($ENV{BUILD_TESTS})
	# Sub build: tests
	add_subdirectory(tests/Common)
	# Sub build: tests
	add_subdirectory(tests/Harmattan)
	# Sub build: tests
	add_subdirectory(tests/Meego)
endif ($ENV{BUILD_TESTS})

# Install configuration file here
install(FILES prestart/nokia.conf DESTINATION /etc/prestart)
