#!/bin/bash

function aptcheck() {
  NO=$(apt-cache policy "${1}" | grep Installed | grep -v none)
  if [ -z "$NO" ]; then
    sudo apt install "${1}"
  fi
}

aptcheck make
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

