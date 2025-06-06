#include "printf.h"
#include "externs.h"
#include "options.h"
#include "ocdwarf-lines.h"

int ocdwarf_debug_line(handle_t p, handle_t s, handle_t d) {
  int n = 0;
  if (isopcode(p)) {
    popcode_t p0 = CAST(popcode_t, p);
    if (MODE_ISSET(p0->ocdump, OPTDEBUGELF_DEBUG_LINE)) {
//      printf_text("Offset", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE) | USE_EOL);
//      printf_text("Length", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE) | USE_EOL);
//      printf_text("DWARF Version", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE) | USE_EOL);
//      printf_text("Prologue Length", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE) | USE_EOL);
//      printf_text("Minimum Instruction Length", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE) | USE_EOL);
//      printf_text("Initial value of 'is_stmt'", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE) | USE_EOL);
//      printf_text("Line Base", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE) | USE_EOL);
//      printf_text("Line Range", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE) | USE_EOL);
//      printf_text("Opcode Base", USE_LT | USE_TAB | USE_COLON | SET_PAD(MAXSIZE) | USE_EOL);
//      printf_eol();
//      printf_text("Opcodes", USE_LT | USE_SPACE | USE_COLON | USE_EOL);
//      printf_eol();
//      printf_text("The Directory Table", USE_LT | USE_SPACE);
//      printf_nice(0x1b, USE_COLON | USE_FHEX8 | USE_OFFSET | USE_EOL);
//      printf_eol();
//      printf_text("The File Name Table", USE_LT | USE_SPACE);
//      printf_nice(0xf8, USE_COLON | USE_FHEX8 | USE_OFFSET | USE_EOL);
//      printf_eol();
//      printf_text("Line Number Statements", USE_LT | USE_SPACE | USE_COLON | USE_EOL);
//      printf_eol();
    }

    if (MODE_ISSET(p0->ocdump, OPTDEBUGELF_DEBUG_LINE_DECODED)) {
    }
  }

  return n;
}

