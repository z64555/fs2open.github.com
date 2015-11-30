#!/usr/bin/env sh

set -e

cd travis-build

if [ "$TRAVIS_OS_NAME" = "linux" ]; then
    ninja
elif [ "$TRAVIS_OS_NAME" = "osx" ]; then
    $HOME/cmake/CMake.app/Contents/bin/cmake --build . --config "$CONFIGURATION" | xcpretty -f `xcpretty-travis-formatter` && exit ${PIPESTATUS[0]}
fi
