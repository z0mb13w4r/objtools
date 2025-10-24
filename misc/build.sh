#!/bin/bash

NAME=objtools-0.0

make -f objdwarf-ng.mk all
make -f convert-ng.mk all
make -f objcopy-ng.mk all
make -f objdump-ng.mk all
make -f objhash-ng.mk all
make -f readelf-ng.mk all
make -f readpe-ng.mk all

mkdir $NAME
mkdir $NAME/DEBIAN
mkdir $NAME/usr/local/bin
