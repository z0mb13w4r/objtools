#!/bin/bash

NAME=samples/exampled-32
#NAME=samples/exampled-64
#NAME=samples/exampled-32.o
#NAME=samples/exampled-64.o
#NAME=samples/example-043-arm32
#NAME=samples/example-043-arm32.o
#NAME=samples/example-043-arm64
#NAME=samples/example-043-arm64.o

PICK1='-a'
#PICK1='-s'
#PICK1='-d'
#PICK1='-V'

#PICK1='--debug-dump=macro'
#PICK2='--print-macinfo'
#PICK1='--debug-dump=info'
#PICK2='--print-info'
#PICK1='--debug-dump=aranges'
#PICK2='--print-aranges'
#PICK1='--debug-dump=abbrev'
#PICK2='--print-abbrev'
#PICK1='--debug-dump=str'
#PICK2='--print-strings'

#PICK1='--debug-dump=decodedline'
#PICK1='--debug-dump=rawline'
#PICK2='--print-lines'
PICK1='--debug-dump=frames'
PICK1='--debug-dump=frames-interp'
PICK2='--print-eh-frame'

PRGNAME=readelf
PRGNAMENG=./$PRGNAME-ng
DWARFDUMP=./dwarfdump
OUT1=test-1.out
OUT2=test-2.out

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
  $PRGNAME ${PICK1} $NAME \
    | sed 's/R_X86_64_JUMP_SLO/R_X86_64_JUMP_SLOT/g' \
    | sed 's/R_386_JUMP_SLOT/R_386_JMP_SLOT/g' \
    | sed 's/none/NONE/g' \
    | sed 's/^[ \t]*//;s/[ \t]*$//' \
    | sed 's/\t/ /g' \
    | tr -s '[:space:]' | tr -d '\r' > $OUT2
}

function go_3() {
  $PRGNAMENG ${PICK1} --debug-dump=enhanced $NAME \
    | sed 's/^[ \t]*//;s/[ \t]*$//' \
    | sed 's/\t/ /g' \
    | tr -s '[:space:]' | tr -d '\r' > $OUT1
}

function go_4() {
  $DWARFDUMP ${PICK2} $NAME \
    | sed 's/^[ \t]*//;s/[ \t]*$//' \
    | sed 's/\t/ /g' \
    | tr -s '[:space:]' | tr -d '\r' > $OUT2
}

go_rm $OUT1
go_rm $OUT2

if [ "$1" == "-p" ] || [ "$1" == "--print" ]; then
  echo $PRGNAME ${PICK1} $NAME
  echo $PRGNAMENG ${PICK1} $NAME
elif [ "$1" == "-g" ] || [ "$1" == "--go" ]; then
  $PRGNAMENG ${PICK1} $NAME
elif [ "$1" == "-r" ] || [ "$1" == "--raw" ]; then
  $PRGNAMENG ${PICK1} $NAME > $OUT1
  $PRGNAME ${PICK1} $NAME > $OUT2
elif [ "$1" == "-v" ] || [ "$1" == "--verbose" ]; then
  $PRGNAMENG ${PICK1} -V $NAME > $OUT1
  $PRGNAME ${PICK1} $NAME > $OUT2
elif [ "$1" == "-d" ] || [ "$1" == "--debug" ]; then
  gdb --args $PRGNAMENGd ${PICK1} $NAME
elif [ "$1" == "-e" ]; then
  go_3
  go_4
else
  go_1
  go_2
fi

if [ -e "$OUT1" ] && [ -e "$OUT2" ]; then
  meld $OUT1 $OUT2
fi

