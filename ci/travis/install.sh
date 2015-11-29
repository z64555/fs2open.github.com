#!/usr/bin/env sh

set -e

FILENAME=
if [ "$TRAVIS_OS_NAME" = "linux" ]; then
    sudo apt-get install -y ninja-build libsdl2-dev
    FILENAME=cmake-3.3.1-Linux-x86_64
elif [ "$TRAVIS_OS_NAME" = "osx" ]; then
    gem install xcpretty xcpretty-travis-formatter
    FILENAME=cmake-3.3.1-Darwin-universal
fi

mkdir -p $HOME/cmake/

wget -O /tmp/cmake.tar.gz --no-check-certificate http://www.cmake.org/files/v3.3/$FILENAME.tar.gz
tar -xzf /tmp/cmake.tar.gz -C $HOME/cmake/ --strip-components=1
