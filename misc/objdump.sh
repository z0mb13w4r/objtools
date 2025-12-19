#!/bin/bash

PRGNAME=/usr/bin/objdump
#PRGNAME=samples/binutils-2.45/objdump
PRGNAME=/usr/bin/arm-linux-gnueabihf-objdump
#PRGNAME=/usr/bin/aarch64-linux-gnu-objdump

NAME=samples/exampled-32
NAME=samples/exampled-64
#NAME=samples/exampled-32.o
#NAME=samples/exampled-64.o
NAME=samples/exampled-arm32
#NAME=samples/exampled-arm64
#NAME=samples/exampled-arm32.o
#NAME=samples/exampled-arm64.o

#NAME=samples/example-043-arm32
#NAME=samples/example-043-arm32.o
#NAME=samples/example-043-arm64
#NAME=samples/example-043-arm64.o
#NAME=samples/bstrings.dll
#NAME=samples/example-043-win32.exe
#NAME=samples/example-043-win64.exe
#NAME=samples/exampled-win32.exe
#NAME=samples/exampled-win64.exe

#NAME=samples/binutils-2.45/strip
#NAME=samples/binutils-2.45/objcopy
#NAME=samples/binutils-2.45/readelf
#NAME=samples/binutils-2.45/strings
#NAME=samples/binutils-2.45/bfdtest1
#NAME=samples/binutils-2.45/bfdtest2
#NAME=samples/binutils-2.45/addr2line
#NAME=samples/binutils-2.44-arm64/arm-linux-gnueabi-strip
#NAME=samples/binutils-2.44-arm64/arm-linux-gnueabi-objcopy
#NAME=samples/binutils-2.44-arm64/arm-linux-gnueabi-objdump
#NAME=samples/binutils-2.44-arm64/arm-linux-gnueabi-readelf
#NAME=samples/binutils-2.44-arm64/arm-linux-gnueabi-strings
#NAME=samples/binutils-2.44-arm64/arm-linux-gnueabi-addr2line
#NAME=samples/binutils-2.44-arm64/libopcodes-2.44-armel.so

#PICK='-p'
#PICK='-h'
#PICK='-t'
#PICK='-r'
#PICK='-R'
#PICK='-T'
#PICK='-f'
#PICK='-s'
#PICK='-x'

#PICK='-g'
#PICK='-e'
#PICK='-d'
PICK='-dSl'

#PICK='-dSl --prefix-addresses'
#PICK='-dSl --no-addresses'
#PICK='-dSl --no-show-raw-insn'
#PICK='-dSl --start-address=0x001000 --stop-address=0x001300'

#PICK='-dSl -M att'
#PICK='-dSl -M att-mnemonic'
#PICK='-dSl -M intel'
#PICK='-dSl -M intel-mnemonic'
#PICK="-dSl -M 'intel-mnemonic,x86-64'"
#PICK='-W'

#PICK='--dwarf=info'
#PICK='--dwarf=aranges'
#PICK='--dwarf=abbrev'
#PICK='--dwarf=frames'
#PICK='--dwarf=frames-interp'
#PICK='--dwarf=rawline --dwarf=enhanced'
#PICK='--dwarf=decodedline'
#PICK='--dwarf=macro'
#PICK='--dwarf=str'
#PICK='--dwarf=loc'

# not supported by:
#   exampled-64
#PICK='--dwarf=pubnames'
#PICK='--dwarf=Ranges'
#PICK='--dwarf=pubtypes'
#PICK='--dwarf=trace_info'
#PICK='--dwarf=trace_abbrev'
#PICK='--dwarf=trace_aranges'
#PICK='--dwarf=gdb_index'
#PICK='--dwarf=addr'
#PICK='--dwarf=cu_index'
#PICK='--dwarf=links'
#PICK='--dwarf=follow-links'

PRGNAMENG=./objdump-ng
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

go_rm ${OUT1}
go_rm ${OUT2}

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
  ${PRGNAMENG} ${PICK} ${NAME} > ${OUT1}
  ${PRGNAME} ${PICK} ${NAME} > ${OUT2}
elif [ "$1" == "-v" ] || [ "$1" == "--verbose" ]; then
  ${PRGNAMENG} ${PICK} -v ${NAME} > ${OUT1}
  ${PRGNAME} ${PICK} ${NAME} > ${OUT2}
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

if [ -e "${OUT1}" ] && [ -e "${OUT2}" ]; then
  meld ${OUT1} ${OUT2}
  diff ${OUT1} ${OUT2}
#  grep -nw -A 1 'ERROR' ${OUT1}
#  grep -nw 'ERROR' ${OUT1} | wc -l
#  grep -nw '<unknown:' ${OUT1}
#  grep -nw '<unknown:' ${OUT1} | wc -l
fi

