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
aptcheck gcc-multilib
aptcheck g++-multilib
aptcheck gcc-mingw-w64
aptcheck g++-mingw-w64
aptcheck gcc-arm-linux-gnueabihf
aptcheck g++-arm-linux-gnueabihf
aptcheck gcc-aarch64-linux-gnu
aptcheck g++-aarch64-linux-gnu
aptcheck gcc-riscv64-linux-gnu
aptcheck g++-riscv64-linux-gnu
aptcheck gcc-mips-linux-gnu
aptcheck g++-mips-linux-gnu
aptcheck libssl-dev
aptcheck binutils-dev

VERDWARF=0.11.1
VERSSDEEP=2.14.1

LIBDWARF=libdwarf-${VERDWARF}
LIBSSDEEP=ssdeep-${VERSSDEEP}

PWD=$(pwd)
INC=${PWD}/inc
LIB=${PWD}/libs

# ---- CAPSTONE FRAMEWORK ----
cdmk ${LIB}

git clone https://github.com/capstone-engine/capstone.git
cdmv capstone
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build

# ---- DWARF LIBRARY ----
cdmk ${LIB}

wget https://github.com/davea42/libdwarf-code/releases/download/v${VERDWARF}/libdwarf-${VERDWARF}.tar.xz
tar xf libdwarf-${VERDWARF}.tar.xz
cdmk ${LIBDWARF}/build
../configure
make

# ---- SDEEP LIBRARY ----
cdmk ${LIB}
wget https://github.com/ssdeep-project/ssdeep/releases/download/release-${VERSSDEEP}/ssdeep-${VERSSDEEP}.tar.gz
tar xf ssdeep-${VERSSDEEP}.tar.gz
cdmk ${LIBSSDEEP}
./configure
make

# ---- LIBRARY SYMLINKS ----
cdmv ${LIB}

ln -s ${LIBDWARF}/build/src/lib/libdwarf/ libdwarf
cdmv libdwarf
ln -s .libs libs

cdmv ${LIB}

ln -s ${LIBSSDEEP} ssdeep
cdmv ssdeep
ln -s .libs libs

# ---- INCLUDE SYMLINKS ----
cdmv ${INC}

ln -s ../libs/ssdeep/ ssdeep
ln -s ../libs/capstone/include/ capstone
ln -s ../libs/libdwarf-${VERDWARF}/src/lib/libdwarf/ libdwarf

