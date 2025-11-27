#!/bin/bash

NAME=samples/exampled-32
NAME=samples/exampled-64
#NAME=samples/exampled-32.o
#NAME=samples/exampled-64.o
#NAME=samples/example-043-arm32
#NAME=samples/example-043-arm32.o
#NAME=samples/example-043-arm64
#NAME=samples/example-043-arm64.o

#NAME=samples/binutils-2.45/strip
#NAME=samples/binutils-2.45/objcopy
#NAME=samples/binutils-2.45/readelf
#NAME=samples/binutils-2.45/strings

PRGNAME=/usr/bin/md5deep
PRGNAMENG=./objhash-ng
OUT1=test-1.out
OUT2=test-2.out

PICK1='--md5'
PICK2=''
#PICK1='-P --md5'
#PICK2=''

function go_rm() {
  if [ -e "$1" ]; then
    echo "-> rm $1"
    rm $1
  fi
}

function go_1() {
  $PRGNAMENG ${PICK1} $NAME \
    | sed 's/^[ \t]*//;s/[ \t]*$//' \
    | sed 's/\t/ /g' \
    | tr -s '[:space:]' | tr -d '\r' > $OUT1
}

function go_2() {
  $PRGNAME ${PICK2} $NAME \
    | sed 's/^[ \t]*//;s/[ \t]*$//' \
    | sed 's/\t/ /g' \
    | tr -s '[:space:]' | tr -d '\r' > $OUT2
}

go_rm $OUT1
go_rm $OUT2

go_1
go_2

if [ -e "$OUT1" ] && [ -e "$OUT2" ]; then
  meld $OUT1 $OUT2
fi

