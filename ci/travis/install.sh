#!/usr/bin/env sh

set -e

FILENAME=
if [ "$TRAVIS_OS_NAME" = "linux" ]; then
    sudo apt-get install -y ninja-build libsdl2-dev
    FILENAME=cmake-3.4.3-Linux-x86_64
elif [ "$TRAVIS_OS_NAME" = "osx" ]; then
    gem install xcpretty xcpretty-travis-formatter thefox-pastebin
    FILENAME=cmake-3.4.3-Darwin-x86_64
fi

mkdir -p $HOME/cmake/

wget -O /tmp/cmake.tar.gz --no-check-certificate http://www.cmake.org/files/v3.4/$FILENAME.tar.gz
tar -xzf /tmp/cmake.tar.gz -C $HOME/cmake/ --strip-components=1
