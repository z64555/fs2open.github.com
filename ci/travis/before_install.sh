#!/usr/bin/env sh

set -ex

if [ "$TRAVIS_OS_NAME" = "linux" ]; then
	sudo add-apt-repository --yes ppa:george-edison55/cmake-3.x
    sudo apt-get -y update
elif [ "$TRAVIS_OS_NAME" = "osx" ]; then
    # Nothing to do here
    :
fi
