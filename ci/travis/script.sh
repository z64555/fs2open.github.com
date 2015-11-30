#!/usr/bin/env sh

set -e

cd travis-build

if [ "$TRAVIS_OS_NAME" = "linux" ]; then
    ninja
elif [ "$TRAVIS_OS_NAME" = "osx" ]; then
    $HOME/cmake/CMake.app/Contents/bin/cmake --build . --config "$CONFIGURATION" | tee xcodebuild.log | xcpretty -f `xcpretty-travis-formatter` && find . && cat xcodebuild.log && exit ${PIPESTATUS[0]}
fi
