#!/bin/bash

RMBIN=/usr/bin/rm
TRBIN=/usr/bin/tr
CUTBIN=/usr/bin/cut
SEDBIN=/usr/bin/sed
TEEBIN=/usr/bin/tee
COPYBIN=/usr/bin/cp
FINDBIN=/usr/bin/find
GREPBIN=/usr/bin/grep
GZIPBIN=/usr/bin/gzip
HEADBIN=/usr/bin/head
MAKEBIN=/usr/bin/make
SUDOBIN=/usr/bin/sudo
WGETBIN=/usr/bin/wget
CHMODBIN=/usr/bin/chmod
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

THIS_NAME=objtools-$VER-ubuntu-$REL-$ARCH
USRBIN=$THIS_NAME/usr/bin
USRMAN=$THIS_NAME/usr/share/man/man1
USROBJ=$THIS_NAME/usr/share/objtools
LOCALBIN=$THIS_NAME/usr/local/bin
LOCALMAN=$THIS_NAME/usr/local/share/man/man1
DEBIANBIN=$THIS_NAME/DEBIAN

function log_info() {
  echo "> $*"
}

function log_error() {
  echo "ERROR: $*" >&2
}

function log_warn() {
  echo "WARNING: $*" >&2
}

function log_detail() {
  echo ">> $*"
}

log_detail "Create package ${THIS_NAME}.deb"
$RMBIN -v $EXTERNBIN/convert-ng
$RMBIN -v $EXTERNBIN/detect-ng
$RMBIN -v $EXTERNBIN/objcopy-ng
$RMBIN -v $EXTERNBIN/objdump-ng
$RMBIN -v $EXTERNBIN/objdwarf-ng
$RMBIN -v $EXTERNBIN/objhash-ng
$RMBIN -v $EXTERNBIN/readelf-ng
$RMBIN -v $EXTERNBIN/readpe-ng
$RMBIN -v $EXTERNBIN/coredump-ng
$RMBIN -v $EXTERNBIN/release/*.a
$RMBIN -vrf $THIS_NAME/
$RMBIN -v $THIS_NAME.deb
$RMBIN -v *-ng.map
$RMBIN -v *-ng

if [[ ! -f /etc/os-release ]]; then
  log_error "Cannot determine operating system. /etc/os-release not found."
  exit 1
fi

source /etc/os-release

if [[ "${ID:-}" != "ubuntu" ]]; then
  log_error "This script only supports Ubuntu. Detected: ${ID:-unknown}"
  exit 1
fi

THIS_VERSION="${VERSION_ID:-}"
log_detail "Detected Ubuntu ${THIS_VERSION}"

$MAKEBIN -f detect-ng.mk all
$MAKEBIN -f readpe-ng.mk all
$MAKEBIN -f convert-ng.mk all
$MAKEBIN -f objcopy-ng.mk all
$MAKEBIN -f objdump-ng.mk all
$MAKEBIN -f objhash-ng.mk all
$MAKEBIN -f readelf-ng.mk all
$MAKEBIN -f objdwarf-ng.mk all
$MAKEBIN -f coredump-ng.mk all

$SUDOBIN $MKDIRBIN -v -p $THIS_NAME
$SUDOBIN $MKDIRBIN -v -p $USRBIN
$SUDOBIN $MKDIRBIN -v -p $USROBJ
$SUDOBIN $MKDIRBIN -v -p $LOCALBIN
$SUDOBIN $MKDIRBIN -v -p $LOCALMAN
$SUDOBIN $MKDIRBIN -v -p $DEBIANBIN

if [[ "${THIS_VERSION}" == "24.04" ]]; then
  $SUDOBIN $COPYBIN -v $EXTERNBIN/binwalk3 $USRBIN/
  $SUDOBIN $COPYBIN -v $EXTERNBIN/sasquatch $USRBIN/
  $SUDOBIN $COPYBIN -v $EXTERNBIN/sasquatch-v4be $USRBIN/
fi

$SUDOBIN $COPYBIN -v $EXTERNBIN/detect-ng $LOCALBIN/
$SUDOBIN $COPYBIN -v $EXTERNBIN/readpe-ng $LOCALBIN/
$SUDOBIN $COPYBIN -v $EXTERNBIN/convert-ng $LOCALBIN/
#$SUDOBIN $COPYBIN -v $EXTERNBIN/objcopy-ng $LOCALBIN/
$SUDOBIN $COPYBIN -v $EXTERNBIN/objdump-ng $LOCALBIN/
$SUDOBIN $COPYBIN -v $EXTERNBIN/objhash-ng $LOCALBIN/
$SUDOBIN $COPYBIN -v $EXTERNBIN/readelf-ng $LOCALBIN/
$SUDOBIN $COPYBIN -v $EXTERNBIN/coredump-ng $LOCALBIN/
$SUDOBIN $COPYBIN -v $EXTERNBIN/objdwarf-ng $LOCALBIN/
$SUDOBIN $COPYBIN -v $EXTERNBIN/strings-ng.py $LOCALBIN/
$SUDOBIN $COPYBIN -v $EXTERNBIN/yara-rules-ng $LOCALBIN/
$SUDOBIN $COPYBIN -v $EXTERNBIN/ubi-magic.py $LOCALBIN/
$SUDOBIN $COPYBIN -v $EXTERNBIN/enumerate-ng.sh $LOCALBIN/
$SUDOBIN $COPYBIN -v $EXTERNBIN/firmwalker-ng.sh $LOCALBIN/

$SUDOBIN $COPYBIN -v $EXTERNMAN/detect-ng.1 $LOCALMAN/
$SUDOBIN $COPYBIN -v $EXTERNMAN/readpe-ng.1 $LOCALMAN/
$SUDOBIN $COPYBIN -v $EXTERNMAN/convert-ng.1 $LOCALMAN/
$SUDOBIN $COPYBIN -v $EXTERNMAN/objdump-ng.1 $LOCALMAN/
$SUDOBIN $COPYBIN -v $EXTERNMAN/objhash-ng.1 $LOCALMAN/
$SUDOBIN $COPYBIN -v $EXTERNMAN/readelf-ng.1 $LOCALMAN/
$SUDOBIN $COPYBIN -v $EXTERNMAN/coredump-ng.1 $LOCALMAN/
$SUDOBIN $COPYBIN -v $EXTERNMAN/objdwarf-ng.1 $LOCALMAN/

$SUDOBIN $COPYBIN -v $EXTERNBIN/control $DEBIANBIN/
$SUDOBIN $COPYBIN -v $EXTERNBIN/userdb.sig $USROBJ/

$SUDOBIN $GZIPBIN -v $LOCALMAN/detect-ng.1
$SUDOBIN $GZIPBIN -v $LOCALMAN/readpe-ng.1
$SUDOBIN $GZIPBIN -v $LOCALMAN/convert-ng.1
$SUDOBIN $GZIPBIN -v $LOCALMAN/objdump-ng.1
$SUDOBIN $GZIPBIN -v $LOCALMAN/objhash-ng.1
$SUDOBIN $GZIPBIN -v $LOCALMAN/readelf-ng.1
$SUDOBIN $GZIPBIN -v $LOCALMAN/coredump-ng.1
$SUDOBIN $GZIPBIN -v $LOCALMAN/objdwarf-ng.1

$SUDOBIN $FINDBIN $THIS_NAME/usr/ -type f -exec $MD5SUMBIN '{}' \; | $SUDOBIN $SEDBIN "s/$THIS_NAME\///" | $SUDOBIN $TEEBIN $DEBIANBIN/md5sums

$DPKGDEBBIN --build $THIS_NAME

$SUDOBIN $CHOWNBIN -R $USER:$USER $THIS_NAME

echo $($GREPBIN Version $EXTERNBIN/control | $CUTBIN -d ' ' -f 2) \
     $($HEADBIN -1 $EXTERNMAN/*.1 | $GREPBIN '.TH' | $CUTBIN -d ' ' -f 7 | $TRBIN -d '"') \
     $($GREPBIN '#define VERSION_VALUE' $EXTERNSRC/options.c | $CUTBIN -d ' ' -f 3 | $TRBIN -d '"')

