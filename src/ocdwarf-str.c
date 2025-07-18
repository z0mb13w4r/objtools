#include "printf.h"
#include "externs.h"
#include "options.h"
#include "ocdwarf-str.h"

int ocdwarf_debug_str(handle_t p, handle_t s, handle_t d) {
  int x = DW_DLV_ERROR;
  int n = 0;

  if (isopcode(p) && (isopshdr(s) || isopshdrNN(s))) {
    popcode_t oc = ocget(p, OPCODE_THIS);

    if (MODE_ISSET(oc->ocdump, OPTDEBUGELF_ENHANCED)) {
      n += ocdwarf_printf_groups(p, ocget(p, OPCODE_DWARF_ERROR));
    }

    char* string = 0;
    Dwarf_Off offset = 0;
    Dwarf_Signed length = 0;

    x = dwarf_get_str(ocget(p, OPCODE_DWARF_DEBUG), offset, &string, &length, ocget(p, OPCODE_DWARF_ERROR));
    while (IS_DLV_OK(x)) {
      n += ocdwarf_printf_ADDR(p, offset, USE_TB);
      n += ocdwarf_printf_DEC(p, length, USE_TB | USE_NOSPACE);
      n += printf_text(string, USE_LT | USE_SPACE | USE_SQ);
      n += printf_eol();

      offset += length + 1;
      x = dwarf_get_str(ocget(p, OPCODE_DWARF_DEBUG), offset, &string, &length, ocget(p, OPCODE_DWARF_ERROR));
    }
  }

  return OCDWARF_ERRCODE(x, n);
}

