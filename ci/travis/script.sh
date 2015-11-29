#!/usr/bin/env sh

set -e

cd travis-build

if [ "$TRAVIS_OS_NAME" = "linux" ]; then
    ninja
elif [ "$TRAVIS_OS_NAME" = "osx" ]; then
    cat "freespace2/fixup_bundle.cmake"
    cat "freespace2/fixup_bundle-$CONFIGURATION.cmake"
	
    $HOME/cmake/CMake.app/Contents/bin/cmake --build . --config "$CONFIGURATION" | xcpretty -f `xcpretty-travis-formatter` && exit ${PIPESTATUS[0]}
fi
