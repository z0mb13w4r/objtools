#!/bin/bash

NO=$(apt-cache policy binutils-dev | grep Installed | grep -v none)
if [ -z "$NO" ]; then
  sudo apt install binutils-dev
fi

NO=$(apt-cache policy gcc-multilib | grep Installed | grep -v none)
if [ -z "$NO" ]; then
  sudo apt install gcc-multilib
fi

NO=$(apt-cache policy g++-multilib | grep Installed | grep -v none)
if [ -z "$NO" ]; then
  sudo apt install g++-multilib
fi

NO=$(apt-cache policy gcc-mingw-w64 | grep Installed | grep -v none)
if [ -z "$NO" ]; then
  sudo apt install gcc-mingw-w64
fi

NO=$(apt-cache policy g++-mingw-w64 | grep Installed | grep -v none)
if [ -z "$NO" ]; then
  sudo apt install g++-mingw-w64
fi

make -f example.mk all
make -f example.mk DEBUG=y all
make -f example.mk CROSS=LINUX32 all
make -f example.mk CROSS=LINUX32 DEBUG=y all

make -f objtool.mk all
make -f readelf-ng.mk all
make -f objdump-ng.mk all

make -f objtool.mk DEBUG=y all
make -f readelf-ng.mk DEBUG=y all
make -f objdump-ng.mk DEBUG=y all

