#!/bin/bash

NAME=objtools-0.0
LOCALBIN=$NAME/usr/local/bin
DEBIANBIN=$NAME/DEBIAN
EXTERNBIN=../bin

COPYBIN=/usr/bin/cp
MAKEBIN=/usr/bin/make
MKDIRBIN=/usr/bin/mkdir
DPKGDEBBIN=/usr/bin/dpkg-deb

$MAKEBIN -f readpe-ng.mk all
$MAKEBIN -f convert-ng.mk all
$MAKEBIN -f objcopy-ng.mk all
$MAKEBIN -f objdump-ng.mk all
$MAKEBIN -f objhash-ng.mk all
$MAKEBIN -f readelf-ng.mk all
$MAKEBIN -f objdwarf-ng.mk all

$MKDIRBIN -p $NAME
$MKDIRBIN -p $LOCALBIN
$MKDIRBIN -p $DEBIANBIN

$COPYBIN $EXTERNBIN/readpe-ng $LOCALBIN/
$COPYBIN $EXTERNBIN/convert-ng $LOCALBIN/
$COPYBIN $EXTERNBIN/objcopy-ng $LOCALBIN/
$COPYBIN $EXTERNBIN/objdump-ng $LOCALBIN/
$COPYBIN $EXTERNBIN/objhash-ng $LOCALBIN/
$COPYBIN $EXTERNBIN/readelf-ng $LOCALBIN/
$COPYBIN $EXTERNBIN/objdwarf-ng $LOCALBIN/
$COPYBIN $EXTERNBIN/control $DEBIANBIN/

$DPKGDEBBIN --build $NAME

