CONFIG  += ordered
TEMPLATE = subdirs
SUBDIRS  = src 

# Compile tests if libmeegotouch is present
exists(/usr/include/m) {
    SUBDIRS += tests/perftests/prestart tests/perftests/memoryhog tests/perftests/helloworld tests/functests/helloworld tests/functests/terminateonclose_app tests/unittests/ut_prestarter tests/unittests/ut_prestartparser
} else {
    message("!! libmeegotouch not available, tests not compiled !!")
}

