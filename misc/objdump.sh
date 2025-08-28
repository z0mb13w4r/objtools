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

#PICK='-p'
#PICK='-h'
#PICK='-t'
#PICK='-r'
#PICK='-R'
#PICK='-T'
#PICK='-f'
#PICK='-s'
#PICK='-x'
#PICK='-dSl --prefix-addresses'
#PICK='-dSl --start-address=0x001000 --stop-address=0x001300'

PICK='-g'
#PICK='-e'
#PICK='-d'
#PICK='-dSl'
#PICK='-W'

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
  ${PRGNAMENG} ${PICK} ${NAME} $1 \
    | sed 's/^[ \t]*//;s/[ \t]*$//' \
    | sed 's/\t/ /g' \
    | tr -s '[:space:]' | tr -d '\r' > ${OUT1}
}

function go_2() {
  ${PRGNAME} ${PICK} ${NAME} \
    | sed 's/none/NONE/g' \
    | sed 's/^[ \t]*//;s/[ \t]*$//' \
    | sed 's/\t/ /g' \
    | tr -s '[:space:]' | tr -d '\r' > ${OUT2}
}

go_rm $OUT1
go_rm $OUT2

if [ "$1" == "-h" ] || [ "$1" == "--help" ]; then
  ${PRGNAMENG} -H
elif [ "$1" == "-p" ] || [ "$1" == "--print" ]; then
  echo ${PRGNAME} ${PICK} ${NAME}
  echo ${PRGNAMENG} ${PICK} ${NAME}
elif [ "$1" == "-g" ] || [ "$1" == "--go" ]; then
  ${PRGNAMENG} ${PICK} ${NAME}
elif [ "$1" == "-s" ] || [ "$1" == "--show" ]; then
  ${PRGNAME} ${PICK} ${NAME}
elif [ "$1" == "-r" ] || [ "$1" == "--raw" ]; then
  ${PRGNAMENG} ${PICK} ${NAME} > $OUT1
  ${PRGNAME} ${PICK} ${NAME} > $OUT2
elif [ "$1" == "-v" ] || [ "$1" == "--verbose" ]; then
  ${PRGNAMENG} ${PICK} -v ${NAME} > $OUT1
  ${PRGNAME} ${PICK} ${NAME} > $OUT2
elif [ "$1" == "-d" ] || [ "$1" == "--debug" ]; then
  gdb --args ${PRGNAMENG}d ${PICK} $NAME
elif [ "$1" == "-1" ] || [ "$1" == "--level1" ]; then
  go_1 "-1"
  go_2
elif [ "$1" == "-c" ] || [ "$1" == "--capstone" ]; then
  go_1 "-c"
  go_2
else
  go_1
  go_2
fi

if [ -e "$OUT1" ] && [ -e "$OUT2" ]; then
  meld $OUT1 $OUT2
fi

