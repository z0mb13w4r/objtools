#!/bin/bash

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

