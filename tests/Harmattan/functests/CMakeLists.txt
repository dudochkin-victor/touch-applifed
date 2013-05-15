if ($ENV{MEEGO})
    # Do nothing
endif ($ENV{MEEGO})

if ($ENV{HARMATTAN})
    install(FILES tests.xml DESTINATION /usr/share/applifed-functional-tests)
endif ($ENV{HARMATTAN})
