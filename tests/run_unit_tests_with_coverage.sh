#!/bin/bash

ERRORS="0"

if [ -z $DISPLAY ]; then
	echo "ERROR!!: Set the DISPLAY to something.."
	ERRORS="1"
fi

which lcov
if [ "$?" -ne "0" ]; then
	echo "ERROR!!: lcov not found."
	ERRORS="1"
fi

which genhtml
if [ "$?" -ne "0" ]; then
	echo "ERROR!!: genhtml not found."
	ERRORS="1"
fi

if [ $ERRORS -ne "0" ]; then
	exit 1
fi

export SBOX_USE_CCACHE=no
pushd ..
#qmake COV_OPTION=on
BUILD_TESTS=1 COVERAGE=1 ./configure
make clean
make -j$(grep processor /proc/cpuinfo | wc -l)
popd
export APPLIFED_SRC=../src
lcov --directory $APPLIFED_SRC --zerocounters
export OUTPUT_DIR=/tmp/Applifed_Coverage_`date +"%Y%m%d%H%M%S"`
mkdir $OUTPUT_DIR
mkdir $OUTPUT_DIR/gov

DIR_LIST=""
for F in `find ./unittests/ -type f -perm +u=x`;
do 
    # Run unit test
    $F; 
    DIR_LIST=$DIR_LIST"--directory $(dirname $F) ";
done

lcov `echo $DIR_LIST` --capture --output-file $OUTPUT_DIR/gov/app.info && genhtml $OUTPUT_DIR/gov/app.info -o $OUTPUT_DIR && echo "Open file $OUTPUT_DIR/index.html with your browser."




