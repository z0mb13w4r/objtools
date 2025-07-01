#include "printf.h"
#include "externs.h"
#include "options.h"
#include "ocdwarf-str.h"

int ocdwarf_debug_str(handle_t p, handle_t s, handle_t d) {
  int x = DW_DLV_ERROR;
  int n = 0;

  if (isopcode(p) && (isopshdr(s) || isopshdrNN(s))) {
    char* string = 0;
    Dwarf_Off offset = 0;
    Dwarf_Signed length = 0;

    x = dwarf_get_str(ocget(p, OPCODE_DWARF_DEBUG), offset, &string, &length, ocget(p, OPCODE_DWARF_ERROR));
    while (IS_DLV_OK(x)) {
      n += printf_text("name at offset", USE_LT);
      n += printf_nice(offset, USE_FHEX32);
      n += printf_eol();

      offset += length + 1;
      x = dwarf_get_str(ocget(p, OPCODE_DWARF_DEBUG), offset, &string, &length, ocget(p, OPCODE_DWARF_ERROR));
    }
  }

  return OCDWARF_ERRCODE(x, n);
}

