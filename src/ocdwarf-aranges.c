#include "printf.h"
#include "externs.h"
#include "options.h"
#include "ocdwarf-aranges.h"

int ocdwarf_debug_aranges(handle_t p, handle_t s, handle_t d) {
  int x = DW_DLV_ERROR;
  int n = 0;

  if (isopcode(p) && (isopshdr(s) || isopshdrNN(s))) {
    Dwarf_Signed arange_count = 0;
    Dwarf_Arange *arange_array = NULL;
    x = dwarf_get_aranges(ocget(p, OPCODE_DWARF_DEBUG), &arange_array, &arange_count, ocget(p, OPCODE_DWARF_ERROR));
    if (IS_DLV_ANY_ERROR(x)) {
      if (IS_DLV_ERROR(x)) {
        printf_e("dwarf_get_aranges failed! - %d", x);
      }
      return OCDWARF_ERRCODE(x, n);
    }

  }

  return OCDWARF_ERRCODE(x, n);
}

