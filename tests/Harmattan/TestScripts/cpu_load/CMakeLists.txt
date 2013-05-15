if ($ENV{MEEGO})
    # Do nothing
endif ($ENV{MEEGO})

if ($ENV{HARMATTAN})
    install(FILES cpu_load_high.drive cpu_load_low.drive DESTINATION /usr/share/applifed-testscripts/cpu_load)
    install(PROGRAMS tc_cpu_load.rb DESTINATION /usr/share/applifed-testscripts/cpu_load)
    install(FILES cpu_load.conf DESTINATION /usr/share/applifed-testscripts/cpu_load)
endif ($ENV{HARMATTAN})

