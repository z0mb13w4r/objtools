#!/bin/bash

RMBIN=/usr/bin/rm
TRBIN=/usr/bin/tr
CUTBIN=/usr/bin/cut
SEDBIN=/usr/bin/sed
COPYBIN=/usr/bin/cp
FINDBIN=/usr/bin/find
GREPBIN=/usr/bin/grep
GZIPBIN=/usr/bin/gzip
HEADBIN=/usr/bin/head
MAKEBIN=/usr/bin/make
SUDOBIN=/usr/bin/sudo
CHOWNBIN=/usr/bin/chown
MKDIRBIN=/usr/bin/mkdir
MD5SUMBIN=/usr/bin/md5sum
DPKGDEBBIN=/usr/bin/dpkg-deb

EXTERNBIN=../bin
EXTERNMAN=../man
EXTERNSRC=../src

REL=$($GREPBIN DISTRIB_RELEASE /etc/lsb-release | $CUTBIN -d '=' -f 2)
VER=$($GREPBIN Version $EXTERNBIN/control | $CUTBIN -d ' ' -f 2)
ARCH=$($GREPBIN Architecture $EXTERNBIN/control | $CUTBIN -d ' ' -f 2)

NAME=objtools-$VER-ubuntu-$REL-$ARCH
USRBIN=$NAME/usr/bin
USRMAN=$NAME/usr/share/man/man1
USROBJ=$NAME/usr/share/objtools
LOCALBIN=$NAME/usr/local/bin
LOCALMAN=$NAME/usr/local/share/man/
DEBIANBIN=$NAME/DEBIAN

echo $NAME
$RMBIN -v $EXTERNBIN/convert-ng
$RMBIN -v $EXTERNBIN/detect-ng
$RMBIN -v $EXTERNBIN/objcopy-ng
$RMBIN -v $EXTERNBIN/objdump-ng
$RMBIN -v $EXTERNBIN/objdwarf-ng
$RMBIN -v $EXTERNBIN/objhash-ng
$RMBIN -v $EXTERNBIN/readelf-ng
$RMBIN -v $EXTERNBIN/readpe-ng
$RMBIN -v $EXTERNBIN/release/*.a
$RMBIN -vrf $NAME/
$RMBIN -v $NAME.deb
$RMBIN -v *-ng.map
$RMBIN -v *-ng

$MAKEBIN -f detect-ng.mk all
$MAKEBIN -f readpe-ng.mk all
$MAKEBIN -f convert-ng.mk all
$MAKEBIN -f objcopy-ng.mk all
$MAKEBIN -f objdump-ng.mk all
$MAKEBIN -f objhash-ng.mk all
$MAKEBIN -f readelf-ng.mk all
$MAKEBIN -f objdwarf-ng.mk all

$MKDIRBIN -v -p $NAME
$MKDIRBIN -v -p $USRBIN
$MKDIRBIN -v -p $USRMAN
$MKDIRBIN -v -p $USROBJ
$MKDIRBIN -v -p $LOCALBIN
$MKDIRBIN -v -p $DEBIANBIN

$COPYBIN -v $EXTERNBIN/detect-ng $USRBIN/
$COPYBIN -v $EXTERNBIN/readpe-ng $USRBIN/
$COPYBIN -v $EXTERNBIN/convert-ng $USRBIN/
#$COPYBIN -v $EXTERNBIN/objcopy-ng $USRBIN/
$COPYBIN -v $EXTERNBIN/objdump-ng $USRBIN/
$COPYBIN -v $EXTERNBIN/objhash-ng $USRBIN/
$COPYBIN -v $EXTERNBIN/readelf-ng $USRBIN/
$COPYBIN -v $EXTERNBIN/objdwarf-ng $USRBIN/
$COPYBIN -v $EXTERNBIN/strings-ng.py $LOCALBIN/
$COPYBIN -v $EXTERNBIN/enumerate-ng.py $LOCALBIN/
$COPYBIN -v $EXTERNBIN/yara-rules-ng $LOCALBIN/

$COPYBIN -v $EXTERNMAN/readpe-ng.1 $USRMAN/
$COPYBIN -v $EXTERNMAN/convert-ng.1 $USRMAN/
$COPYBIN -v $EXTERNMAN/objdump-ng.1 $USRMAN/
$COPYBIN -v $EXTERNMAN/objhash-ng.1 $USRMAN/
$COPYBIN -v $EXTERNMAN/readelf-ng.1 $USRMAN/
$COPYBIN -v $EXTERNMAN/objdwarf-ng.1 $USRMAN/

$COPYBIN -v $EXTERNBIN/control $DEBIANBIN/
$COPYBIN -v $EXTERNBIN/userdb.sig $USROBJ/

$GZIPBIN -v $USRMAN/readpe-ng.1
$GZIPBIN -v $USRMAN/convert-ng.1
$GZIPBIN -v $USRMAN/objdump-ng.1
$GZIPBIN -v $USRMAN/objhash-ng.1
$GZIPBIN -v $USRMAN/readelf-ng.1
$GZIPBIN -v $USRMAN/objdwarf-ng.1

$SUDOBIN $CHOWNBIN root:root $USRBIN/*
$SUDOBIN $CHOWNBIN root:root $USRMAN/*

$FINDBIN $NAME/usr/ -type f -exec $MD5SUMBIN '{}' \; | $SEDBIN "s/$NAME\///" > $DEBIANBIN/md5sums

$DPKGDEBBIN --build $NAME

echo $($GREPBIN Version $EXTERNBIN/control | $CUTBIN -d ' ' -f 2) \
     $($HEADBIN -1 $EXTERNMAN/* | $GREPBIN '.TH' | $CUTBIN -d ' ' -f 7 | $TRBIN -d '"') \
     $($GREPBIN '#define VERSION_VALUE' $EXTERNSRC/options.c | $CUTBIN -d ' ' -f 3 | $TRBIN -d '"')

