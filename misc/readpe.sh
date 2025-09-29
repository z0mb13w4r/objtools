#!/bin/bash

#NAME1=samples/example-043.exe
#NAME1=samples/bstrings.dll
NAME1=samples/msvcp120.dll
NAME2=${NAME1}-pecheck
PICK='-a'

PRGNAMENG=./readpe-ng
OUT1=test-1.out
OUT2=test-2.out

function go_rm() {
  if [ -e "$1" ]; then
    echo "-> rm $1"
    rm $1
  fi
}

function go_1() {
  $PRGNAMENG ${PICK} ${NAME1} \
    | sed 's/0x0*\([0-9A-F]\)/0x\1/g' \
    | sed 's/^[ \t]*//;s/[ \t]*$//' \
    | sed 's/\t/ /g' \
    | tr -s '[:space:]' | tr -d '\r' > ${OUT1}
}

function go_2() {
  sed '1,25d' ${NAME2} \
    | sed 's/^0x[0-9A-F]* *0x[0-9A-F]* *//g' \
    | sed 's/^[0-9a-zA-Z]*\.dll\.//g' \
    | sed 's/^[0-9a-zA-Z]*\.DLL\.//g' \
    | sed 's/0x0*\([0-9A-F]\)/0x\1/g' \
    | sed 's/Hint//g' \
    | sed "/^Entropy/d" \
    | sed "/^MD5/d" \
    | sed "/^SHA-1/d" \
    | sed "/^SHA-256/d" \
    | sed "/^SHA-512/d" \
    | sed "/^---/d" \
    | sed "/entropy/d" \
    | sed -e 's/\(0x[0-9A-F]*\)/\L\1/' \
    | sed 's/^[ \t]*//;s/[ \t]*$//' \
    | sed 's/\t/ /g' \
    | tr -s '[:space:]' | tr -d '\r' \
    | sed 's/^\[\([0-9A-Z_]*\)\]/\1:/g' > ${OUT2}
}

go_rm $OUT1
go_rm $OUT2

if [ "$1" == "p" ]; then
  echo $PRGNAMENG ${PICK} ${NAME1}
elif [ "$1" == "y" ]; then
  $PRGNAMENG ${PICK} ${NAME1} > ${OUT1}
  sed '1,25d' ${NAME2} > ${OUT2}
elif [ "$1" == "h" ]; then
  $PRGNAMENG -H
else
  go_1
  go_2
fi

if [ -e "$OUT1" ] && [ -e "$OUT2" ]; then
  meld ${OUT1} ${OUT2}
fi

