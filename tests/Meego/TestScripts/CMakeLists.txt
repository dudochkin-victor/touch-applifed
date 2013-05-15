# Find MeeGo Touch libraries for the test programs using pkg-config
include(FindPkgConfig) 
pkg_check_modules(MEEGOTOUCH meegotouch>=0.20 REQUIRED)
include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${MEEGOTOUCH_INCLUDE_DIRS})

add_subdirectory(cpu_load)
add_subdirectory(periodic_re_prestart)

install(FILES functional.drive fautils.rb test-perf-prestart.py  
	DESTINATION /usr/share/applifed-M-testscripts)

install(PROGRAMS tc_applifed_one_instance.rb cp-service.rb ts_prestart.rb
  ts_mwtests.rb tc_daemonize.rb
  tc_prestart_preventing.rb
  test-perf.rb
  DESTINATION /usr/share/applifed-M-testscripts)

install(FILES
  functest_applifed.conf
  prevent.conf
  prevent.drive
  DESTINATION /usr/share/applifed-M-testscripts)
