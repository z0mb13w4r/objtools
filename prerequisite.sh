#!/bin/bash

function aptcheck() {
  NO=$(apt-cache policy "${1}" | grep Installed | grep -v none)
  if [ -z "$NO" ]; then
    sudo apt install "${1}"
  fi
}

function cdmk() {
  if [ ! -d ${1} ]; then
    echo create directory ${1}
    mkdir ${1}
  fi

  cdmv ${1}
}

function cdmv() {
  echo change directory ${1}
  cd ${1}
}

aptcheck git
aptcheck make
aptcheck cmake
aptcheck autoconf
aptcheck libtool
aptcheck pkg-config
aptcheck gcc
aptcheck g++
#aptcheck gcc-multilib
#aptcheck g++-multilib
#aptcheck gcc-mingw-w64
#aptcheck g++-mingw-w64
#aptcheck gcc-arm-linux-gnueabihf
#aptcheck g++-arm-linux-gnueabihf
#aptcheck gcc-aarch64-linux-gnu
#aptcheck g++-aarch64-linux-gnu
aptcheck libssl-dev
aptcheck binutils-dev

LIBDWARF=libdwarf-0.11.1

PWD=$(pwd)
INC=${PWD}/inc
LIB=${PWD}/libs

cdmk ${LIB}

git clone https://github.com/capstone-engine/capstone.git
cdmv capstone
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build

cdmk ${LIB}

wget https://github.com/davea42/libdwarf-code/releases/download/v0.11.1/libdwarf-0.11.1.tar.xz
tar xf libdwarf-0.11.1.tar.xz
cdmk ${LIBDWARF}/build
../configure
make

cdmv ${LIB}
ln -s ${LIBDWARF}/build/src/lib/libdwarf/ libdwarf
cdmv libdwarf
ln -s .libs libs

cdmv ${INC}

ln -s ../libs/capstone/include/ capstone
ln -s ../libs/libdwarf-0.11.1/src/lib/libdwarf/ libdwarf

