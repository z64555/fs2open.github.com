#!/usr/bin/env sh

set -e

mkdir -p travis-build
cd travis-build

if [ "$TRAVIS_OS_NAME" = "linux" ]; then
    export CXXFLAGS="-m64 -mtune=generic -mfpmath=sse -msse -msse2 -pipe -Wno-unknown-pragmas"
    $HOME/cmake/bin/cmake -G "Ninja" -DCMAKE_BUILD_TYPE=$CONFIGURATION -DFSO_FATAL_WARNINGS=ON ..
elif [ "$TRAVIS_OS_NAME" = "osx" ]; then
    $HOME/cmake/CMake.app/Contents/bin/cmake -G "Xcode" -DFSO_FATAL_WARNINGS=ON ..
fi
