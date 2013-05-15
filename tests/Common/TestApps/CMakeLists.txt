# Find MeeGo Touch libraries for the test programs using pkg-config
include(FindPkgConfig) 
pkg_check_modules(MEEGOTOUCH meegotouch>=0.20 REQUIRED)
include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${MEEGOTOUCH_INCLUDE_DIRS})

# Sub build: fali_multiapp 
add_subdirectory(fali_multiapp)

# Sub build: fali_hello
add_subdirectory(fali_hello)

# Sub build: fali_toc
add_subdirectory(fali_toc)

# Sub build: fali_close
add_subdirectory(fali_close)

# Sub build: multiwindow_apps
add_subdirectory(multiwindow_apps)

# Sub build: memoryhog
add_subdirectory(memoryhog)

