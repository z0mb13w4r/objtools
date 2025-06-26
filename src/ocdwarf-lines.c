#include "printf.h"
#include "externs.h"
#include "options.h"
#include "ocdwarf-lines.h"

int ocdwarf_debug_line(handle_t p, handle_t s, handle_t d) {
  int x = DW_DLV_ERROR;
  int n = 0;

  if (isopcode(p) && (isopshdr(s) || isopshdrNN(s))) {
    popcode_t oc = ocget(p, OPCODE_THIS);

    printf_text("NS new statement, BB new basic block, ET end of text sequence", USE_LT | USE_EOL);
    printf_text("PE prologue end, EB epilogue begin", USE_LT | USE_EOL);
    printf_text("IS=val ISA number, DI=val discriminator value", USE_LT | USE_EOL);

    if (MODE_ISSET(oc->ocdump, OPTDEBUGELF_DEBUG_LINE)) {
    }

    if (MODE_ISSET(oc->ocdump, OPTDEBUGELF_DEBUG_LINE_DECODED)) {
    }
  }

  return OCDWARF_ERRCODE(x, n);
}

