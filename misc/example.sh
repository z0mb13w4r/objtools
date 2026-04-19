#!/bin/bash

BNAME=example
DNAME=${BNAME}d
SAMPLES=samples

RMBIN=/usr/bin/rm
CPBIN=/usr/bin/cp
MAKEBIN=/usr/bin/make

function go_cp() {
  if [ -e "${1}" ]; then
    $CPBIN -v ${1} ${2}
  fi
}

function go_rm() {
  if [ -e "${1}" ]; then
    $RMBIN -v ${1}
  fi
}

function go_copy() {
  go_cp "debug${3}/${BNAME}.o" "${SAMPLES}/${2}.o"
}

function go_make() {
  go_rm ${1}-stripped
  go_rm ${1}.map
  go_rm ${1}

  $MAKEBIN -f example.mk DEBUG=${2} CROSS=${3} all
  $CPBIN -v ${1} ${SAMPLES}/${4}
  go_copy ${2} ${4} ${3}
}

go_make $DNAME y LINUX32 exampled-32
go_make $DNAME y LINUX64 exampled-64

go_make $DNAME y ARM32 exampled-arm32
go_make $DNAME y ARM64 exampled-arm64

