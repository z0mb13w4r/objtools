#!/bin/bash

CUTBIN=/usr/bin/cut
SEDBIN=/usr/bin/sed
COPYBIN=/usr/bin/cp
FINDBIN=/usr/bin/find
GREPBIN=/usr/bin/grep
MAKEBIN=/usr/bin/make
SUDOBIN=/usr/bin/sudo
CHOWNBIN=/usr/bin/chown
MKDIRBIN=/usr/bin/mkdir
MD5SUMBIN=/usr/bin/md5sum
DPKGDEBBIN=/usr/bin/dpkg-deb

EXTERNBIN=../bin

VER=$($GREPBIN Version $EXTERNBIN/control | $CUTBIN -d ' ' -f 2)
ARCH=$($GREPBIN Architecture $EXTERNBIN/control | $CUTBIN -d ' ' -f 2)

NAME=objtools-$VER-$ARCH
USRBIN=$NAME/usr/bin
LOCALBIN=$NAME/usr/local/bin
DEBIANBIN=$NAME/DEBIAN

$MAKEBIN -f readpe-ng.mk all
$MAKEBIN -f convert-ng.mk all
#$MAKEBIN -f objcopy-ng.mk all
$MAKEBIN -f objdump-ng.mk all
$MAKEBIN -f objhash-ng.mk all
$MAKEBIN -f readelf-ng.mk all
$MAKEBIN -f objdwarf-ng.mk all

$MKDIRBIN -v -p $NAME
$MKDIRBIN -v -p $USRBIN
$MKDIRBIN -v -p $DEBIANBIN

$COPYBIN -v $EXTERNBIN/readpe-ng $USRBIN/
$COPYBIN -v $EXTERNBIN/convert-ng $USRBIN/
#$COPYBIN -v $EXTERNBIN/objcopy-ng $USRBIN/
$COPYBIN -v $EXTERNBIN/objdump-ng $USRBIN/
$COPYBIN -v $EXTERNBIN/objhash-ng $USRBIN/
$COPYBIN -v $EXTERNBIN/readelf-ng $USRBIN/
$COPYBIN -v $EXTERNBIN/objdwarf-ng $USRBIN/
$COPYBIN -v $EXTERNBIN/control $DEBIANBIN/

$SUDOBIN $CHOWNBIN root:root $USRBIN/*

$FINDBIN $NAME/usr/ -type f -exec $MD5SUMBIN '{}' \; | $SEDBIN "s/$NAME\///" > $DEBIANBIN/md5sums

$DPKGDEBBIN --build $NAME

