#include "ocdwarf-printf.h"

int ocdwarf_printf_srcfiles(handle_t p, handle_t s, dwarf_srcfiles_t *sf) {
  int n = 0;
  if (isopcode(p)) {
    if (DW_DLV_OK == sf->status && 0 != sf->size) {
      for (Dwarf_Signed i = 0; i < sf->size; ++i) {
        n += printf_nice(i, USE_DEC3 | USE_TB | USE_COLON);
        n += printf_text(sf->data[i], USE_LT | USE_EOL);
      }
    }
  }

  return n;
}

