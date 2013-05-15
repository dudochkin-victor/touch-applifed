#file(MAKE_DIRECTORY /usr/share/applifed-tests)
if ($ENV{MEEGO})
    # Do nothing
endif ($ENV{MEEGO})

if ($ENV{HARMATTAN})
    install(FILES tests.xml DESTINATION /usr/share/applifed-tests)
endif ($ENV{HARMATTAN})

