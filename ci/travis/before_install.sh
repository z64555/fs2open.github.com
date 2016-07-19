#!/usr/bin/env sh

set -e

if [ "$TRAVIS_OS_NAME" = "linux" ]; then
    sudo add-apt-repository --yes ppa:george-edison55/cmake-3.x
    sudo apt-get update -qq
elif [ "$TRAVIS_OS_NAME" = "osx" ]; then
    # Nothing to do here
    :
fi
