# Find MeeGo Touch libraries for the test programs using pkg-config
if ($ENV{MEEGO})
    # Do nothing
endif ($ENV{MEEGO})

if ($ENV{HARMATTAN})
    include(FindPkgConfig) 
    pkg_check_modules(MEEGOTOUCH meegotouch>=0.20 REQUIRED)
    include_directories(${CMAKE_CURRENT_SOURCE_DIR} ${MEEGOTOUCH_INCLUDE_DIRS})

    add_subdirectory(cpu_load)

    install(FILES functional.drive fautils.rb test-perf-prestart.py  
    	DESTINATION /usr/share/applifed-testscripts)

    install(PROGRAMS tc_applifed_one_instance.rb cp-service.rb ts_prestart.rb
        ts_multi_instance.rb tc_daemonize.rb
        test-perf.rb ts_prestart_prevent.rb
        DESTINATION /usr/share/applifed-testscripts)

    install(FILES
        functest_applifed.conf
        DESTINATION /usr/share/applifed-testscripts)
endif ($ENV{HARMATTAN})
