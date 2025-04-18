#!/bin/bash

function aptcheck() {
  NO=$(apt-cache policy "${1}" | grep Installed | grep -v none)
  if [ -z "$NO" ]; then
    sudo apt install "${1}"
  fi
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
aptcheck libssl-dev
aptcheck binutils-dev

mkdir libs
cd libs

git clone https://github.com/capstone-engine/capstone.git
cd capstone
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build

cd ..

wget https://github.com/davea42/libdwarf-code/releases/download/v0.11.1/libdwarf-0.11.1.tar.xz
tar xf libdwarf-0.11.1.tar.xz
mkdir libdwarf-0.11.1/build
cd libdwarf-0.11.1/build
../configure
make

