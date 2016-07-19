#!/usr/bin/env sh

set -ex

if [ "$TRAVIS_OS_NAME" = "linux" ]; then
    sudo apt-get -y update
elif [ "$TRAVIS_OS_NAME" = "osx" ]; then
    # Nothing to do here
    :
fi
