#!/bin/bash

#NAME=samples/exampled-32
#NAME=samples/exampled-64
NAME=samples/exampled-32.o
#NAME=samples/exampled-64.o
#NAME=samples/example-043-arm32
#NAME=samples/example-043-arm32.o
#NAME=samples/example-043-arm64
#NAME=samples/example-043-arm64.o

#PICK1='--dwarf-dump=info --dwarf-dump=enhanced'
#PICK2='--print-info'

#PICK1='--dwarf-dump=macro --dwarf-dump=enhanced'
#PICK2='--print-macinfo'

#PICK1='--dwarf-dump=aranges --dwarf-dump=enhanced'
#PICK2='--print-aranges'

#PICK1='--dwarf-dump=rawline'
#PICK1='--dwarf-dump=decodedline'
#PICK2='--print-lines'

#PICK1='--dwarf-dump=frames --dwarf-dump=enhanced'
#PICK2='--print-eh-frame'

#PICK1='--dwarf-dump=abbrev --dwarf-dump=enhanced'
#PICK2='--print-abbrev'

PICK1='--dwarf-dump=str'
PICK2='--print-strings'

PRGNAMENG=./objdwarf-ng
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
  $DWARFDUMP ${PICK2} $NAME \
    | sed 's/^[ \t]*//;s/[ \t]*$//' \
    | sed 's/\t/ /g' \
    | tr -s '[:space:]' | tr -d '\r' > $OUT2
}

go_rm $OUT1
go_rm $OUT2

if [ "$1" == "y" ]; then
  $PRGNAMENG ${PICK1} $NAME > $OUT1
  $DWARFDUMP ${PICK2} $NAME > $OUT2
elif [ "$1" == "v" ]; then
  $PRGNAMENG ${PICK1} --verbose $NAME > $OUT1
  $DWARFDUMP ${PICK2} $NAME > $OUT2
else
  go_1
  go_2
fi

if [ -e "$OUT1" ] && [ -e "$OUT2" ]; then
  meld $OUT1 $OUT2
fi

