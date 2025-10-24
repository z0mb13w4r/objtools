#!/bin/bash

CUTBIN=/usr/bin/cut
SEDBIN=/usr/bin/sed
COPYBIN=/usr/bin/cp
FINDBIN=/usr/bin/find
GREPBIN=/usr/bin/grep
MAKEBIN=/usr/bin/make
MKDIRBIN=/usr/bin/mkdir
MD5SUMBIN=/usr/bin/md5sum
DPKGDEBBIN=/usr/bin/dpkg-deb

EXTERNBIN=../bin

VER=$($GREPBIN Version $EXTERNBIN/control | $CUTBIN -d ' ' -f 2)
ARCH=$($GREPBIN Architecture $EXTERNBIN/control | $CUTBIN -d ' ' -f 2)

NAME=objtools-$VER-$ARCH
LOCALBIN=$NAME/usr/local/bin
DEBIANBIN=$NAME/DEBIAN

$MAKEBIN -f readpe-ng.mk all
$MAKEBIN -f convert-ng.mk all
$MAKEBIN -f objcopy-ng.mk all
$MAKEBIN -f objdump-ng.mk all
$MAKEBIN -f objhash-ng.mk all
$MAKEBIN -f readelf-ng.mk all
$MAKEBIN -f objdwarf-ng.mk all

$MKDIRBIN -v -p  $NAME
$MKDIRBIN -v -p $LOCALBIN
$MKDIRBIN -v -p $DEBIANBIN

$COPYBIN -v $EXTERNBIN/readpe-ng $LOCALBIN/
$COPYBIN -v $EXTERNBIN/convert-ng $LOCALBIN/
$COPYBIN -v $EXTERNBIN/objcopy-ng $LOCALBIN/
$COPYBIN -v $EXTERNBIN/objdump-ng $LOCALBIN/
$COPYBIN -v $EXTERNBIN/objhash-ng $LOCALBIN/
$COPYBIN -v $EXTERNBIN/readelf-ng $LOCALBIN/
$COPYBIN -v $EXTERNBIN/objdwarf-ng $LOCALBIN/
$COPYBIN -v $EXTERNBIN/control $DEBIANBIN/

$FINDBIN $NAME/usr/ -type f -exec $MD5SUMBIN '{}' \; | $SEDBIN "s/$NAME\///" > $DEBIANBIN/md5sums

$DPKGDEBBIN --build $NAME

