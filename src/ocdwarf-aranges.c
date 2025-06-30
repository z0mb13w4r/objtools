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

    for (Dwarf_Signed i = 0; i < arange_count; ++i) {
      Dwarf_Unsigned segment = 0;
      Dwarf_Unsigned segment_entry_size = 0;
      Dwarf_Addr start = 0;
      Dwarf_Unsigned length = 0;
      Dwarf_Off cu_die_offset = 0;
      x = dwarf_get_arange_info_b(arange_array[i], &segment, &segment_entry_size,
                     &start, &length, &cu_die_offset, ocget(p, OPCODE_DWARF_ERROR));
    }
  }

  return OCDWARF_ERRCODE(x, n);
}

