#!/bin/bash

#NAME=samples/exampled-32
NAME=samples/exampled-64
#NAME=samples/exampled-32.o
#NAME=samples/exampled-64.o
#NAME=samples/example-043-arm32
#NAME=samples/example-043-arm32.o
#NAME=samples/example-043-arm64
#NAME=samples/example-043-arm64.o
#NAME=samples/bstrings.dll
#PICK='-f'
#PICK='-s'
PICK='-h -R -t -T -d'
#PICK='-p'
#PICK='-g'
#PICK='-e'
#PICK='-r'
#PICK='-dSl --prefix-addresses'
#PICK='-dSl --start-address=0x001000 --stop-address=0x001300'
#PICK='-d'
#PICK='-dSl'
#PICK='-W'
# x t T f h s p


PRGNAME=objdump
PRGNAMENG=./$PRGNAME-ng
OUT1=test-1.out
OUT2=test-2.out

function go_rm() {
  if [ -e "$1" ]; then
    echo "-> rm $1"
    rm $1
  fi
}

function go_1() {
  $PRGNAMENG ${PICK} $NAME \
    | sed 's/^[ \t]*//;s/[ \t]*$//' \
    | sed 's/\t/ /g' \
    | tr -s '[:space:]' | tr -d '\r' > $OUT1
}

function go_2() {
  $PRGNAME $PICK $NAME \
    | sed 's/none/NONE/g' \
    | sed 's/^[ \t]*//;s/[ \t]*$//' \
    | sed 's/\t/ /g' \
    | tr -s '[:space:]' | tr -d '\r' > $OUT2
}

go_rm $OUT1
go_rm $OUT2

if [ "$1" == "-p" ] || [ "$1" == "--print" ]; then
  echo $PRGNAME ${PICK} $NAME
  echo $PRGNAMENG ${PICK} $NAME
elif [ "$1" == "-r" ] || [ "$1" == "--raw" ]; then
  $PRGNAMENG ${PICK} $NAME > $OUT1
  $PRGNAME ${PICK} $NAME > $OUT2
elif [ "$1" == "-d" ] || [ "$1" == "--debug" ]; then
  $PRGNAMENG ${PICK} -1 $NAME > $OUT1
  $PRGNAME ${PICK} $NAME > $OUT2
elif [ "$1" == "-v" ] || [ "$1" == "--verbose" ]; then
  $PRGNAMENG ${PICK} -v $NAME > $OUT1
  $PRGNAME ${PICK} $NAME > $OUT2
elif [ "$1" == "-d" ] || [ "$1" == "--debug" ]; then
  gdb --args $PRGNAMENGd ${PICK} $NAME
else
  go_1
  go_2
fi

if [ -e "$OUT1" ] && [ -e "$OUT2" ]; then
  meld $OUT1 $OUT2
fi

