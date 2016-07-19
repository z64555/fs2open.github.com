#!/usr/bin/env sh

set -e

FILENAME=
if [ "$TRAVIS_OS_NAME" = "linux" ]; then
    sudo apt-get -y install cmake
elif [ "$TRAVIS_OS_NAME" = "osx" ]; then
    gem install xcpretty xcpretty-travis-formatter thefox-pastebin
    brew install cmake
fi

cmake --version
