#!/bin/bash

NAME=samples/example-64.coredump
#NAME=samples/exampled-64.coredump

PICK1='-a'
#PICK1='-a -D'
#PICK1='-a -C'
#PICK1='-t'
#PICK1='-S'
#PICK1='-s'
#PICK1='-d'
#PICK1='-V'
#PICK1='-g'
#PICK1='-A'
#PICK1='-h -u'
#PICK1='-n'

#PICK1='-l'
#PICK1='-r -V'
#PICK1='-I'
#PICK1='-m .text'

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

#PICK1='--debug-dump=frames-interp'
#PICK2='--print-eh-frame'
#PICK1='--debug-dump=decodedline'
#PICK2='--print-lines'

PRGNAMENG=./coredump-ng
OUT1=test-1.out
OUT2=test-2.out

function go_rm() {
  if [ -e "$1" ]; then
    echo "-> rm $1"
    rm $1
  fi
}

function go_1() {
  ${PRGNAMENG} ${PICK1} ${NAME} \
    | sed 's/^[ \t]*//;s/[ \t]*$//' \
    | sed 's/\t/ /g' \
    | tr -s '[:space:]' | tr -d '\r' > ${OUT1}
}

go_rm ${OUT1}
go_rm ${OUT2}

if [ "$1" == "-p" ] || [ "$1" == "--print" ]; then
  echo ${PRGNAMENG} ${PICK1} ${NAME}
elif [ "$1" == "-g" ] || [ "$1" == "--go" ]; then
  ${PRGNAMENG} ${PICK1} ${NAME}
elif [ "$1" == "-r" ] || [ "$1" == "--raw" ]; then
  ${PRGNAMENG} ${PICK1} ${NAME} > ${OUT1}
elif [ "$1" == "-v" ] || [ "$1" == "--verbose" ]; then
  ${PRGNAMENG} ${PICK1} --verbose ${NAME} > ${OUT1}
elif [ "$1" == "-d" ] || [ "$1" == "--debug" ]; then
  gdb --args ${PRGNAMENG}d ${PICK1} ${NAME}
else
  go_1
fi

if [ -e "${OUT1}" ] && [ -e "${OUT2}" ]; then
  meld ${OUT1} ${OUT2}
elif [ -e "${OUT1}" ]; then
  gedit ${OUT1} &
fi

