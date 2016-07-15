#!/usr/bin/env sh

set -e

if [ "$TRAVIS_OS_NAME" = "linux" ]; then
    :
elif [ "$TRAVIS_OS_NAME" = "osx" ]; then
    # Nothing to do here
    :
fi
